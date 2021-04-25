// ---------------------------------------------------------------------------
//	TransDisk for Linux 2.0
//	Copyright (C) cisc 2000.
// Linux ver by @RetroDevDiscord
// Compile:
// $ gcc -lstdc++ main.cpp -o lxdisk
// ---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <unistd.h> // unix
#include <fcntl.h> // file control
#include <errno.h> //error number definitions
#include <termios.h> // posix terminal stuff
#include <stdlib.h> //atoi
#include <iostream>

#include "lz77e.h"
#include "xdisk2bas.h"

typedef signed char s8;
typedef unsigned char u8;

void Usage();

int mode;
bool verbose = false;
int version_l = 7;
int drive = -1;
//	int retry = 10;
char* filename = 0;
char title[17] = "                ";
bool writeprotect = false;
bool fastrecv = true;
int media = -1;
uint baud = B19200;
uint port = 0;
uint16_t crc = -1;
LZ77Enc enc;
static u8 fpa[] = "/dev/ttyUSB ";

uint16_t crctable[0x100] = { 123 };
//SIO sio;
//SystemInfo si;
uint16_t xc_index;
int d, e;
//uint16_t crc;
bool connect;
int unc;
struct termios options;
//u16 baud = B9600;
// TODO FIX BAUD BURST RATE
int RSPORT = 0;

void send_byte(int a, bool cc = true);
inline void send_word(int a, bool cc = true);
uint send_packet(const uint8_t* buffer, int length, bool cmd = false, bool data = false);
uint recv_packet(uint8* buffer, int length, bool burst = false);
int recv_word(bool cc=true);
int recv_byte(bool cc=true);
uint recv_c();

#define calc_crc(d) crc = (crc << 8) ^ crctable[(d ^ (crc >> 8)) & 0xff]

static bool D88Seek(FILE* f);
bool send_disk(int drive, FILE* f);
bool decompress_packet(uint8* buf, uint bufsize, const uint8* src, uint pksize);

// D88 image header definition
struct ImageHeader
{
	char title[17];
	uint8_t reserved[9];
	uint8_t readonly;
	uint8_t disktype;
	uint32_t disksize;
	uint32_t trackptr[164];
};

// Only works with ttyUSB0 right now!
int open_port(unsigned char port)
{
	// O_RDWR - enable read and write
    // O_NOCTTY - do not make TTY for the port
    // O_NDELAY - does not care if other side is ready (data carrier detect)
	fpa[11] = port | 0x30;
    int fd = open((const char*)fpa, O_RDWR | O_NOCTTY | O_NDELAY);
    // set options 
	tcgetattr(fd, &options);
	if(mode == 'b') baud = B9600;
    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB; // mask parity =N
    options.c_cflag &= ~CSTOPB; // mask stop bit =1
    options.c_cflag &= ~CSIZE; // mask size =8
    options.c_cflag |= CS8;     // set
    tcsetattr(fd, TCSANOW, &options); // (N81X)

    if(fd == -1)
    {   // couldn't open?
        perror("open_port: Unable to open /dev/ttyUSB0 - ");
    }
    else
        fcntl(fd, F_SETFL, 0);
    return (fd);
}


void build_crc_table()
{
	for (int i=0; i<0x100; i++)
	{
		int a = i << 8;
		for (int j=0; j<8; j++)
			a = (a << 1) ^ (a & 0x8000 ? 0x1021 : 0);
		crctable[i] = a;
	}
}


uint recv_c()
{
	uint8_t c;
	//if (SIO::OK != sio.Read(&c, 1))
	//	Throw(e_connect);
	if(read(RSPORT, &c, 1) < 0)
		printf("fail to read");
		// todo fixme
	if ((c - 0x40) & 0x80)
	{
		unc = c;
		//Throw(e_protocol);
		printf("fail to read");
		//return 0;
	}
	return c - 0x40;
}

int recv_byte(bool cc)
{
	if (e == 0)
		d = recv_c() << 1, e = 7;
	int c = recv_c();
	int a = d | (c >> (e-1));
	d = (c << (9-e)) & 0xff;
	e--;
	if (cc)
		calc_crc(a);
	return a;
}

inline int recv_word(bool c)
{
	int r = recv_byte(c);
	return r + recv_byte(c) * 256;
}

void send_c(uint8 c)
{
	c = (c & 0x7f) + 0x40;
//	if (SIO::OK != sio.Write(&c, 1))
//		Throw(e_connect);
	s8 n = write(RSPORT, &c, 1);
	if (n < 0) {
		fputs("write() failed.\n", stderr);
	//	return 1;
	}
	//return 0;
}

inline void init_pack()
{
	d = e = 0;
	crc = ~0;
}

void send_byte(int a, bool cc)
{
	send_c(d | (a >> (e+1)));
	if (e < 6)
		d = a << (6-e++);
	else
		send_c(a), d = e = 0;
	if (cc)
		calc_crc(a);
}

inline void send_word(int a, bool cc)
{
	send_byte(a & 0xff, cc);
	send_byte((a >> 8) & 0xff, cc);
}

void send_flush()
{
	if (e)
		send_c(d);
}

bool decompress_packet(uint8* buf, uint bufsize, const uint8* src, uint pksize)
{
	const uint8* st = src;
	const uint8* dt = buf;

	uint orgsize = (src[0] + src[1] * 256) & 0x3fff;
	uint method = (src[1] >> 6) & 3;
//	printf("Compression: %.4x\n", src[0] + src[1] * 256);
	uint exsize = orgsize > bufsize ? bufsize : orgsize;
	src += 2;

	switch (method)
	{
	case 0:		// �����k
		memcpy(buf, src, exsize);
		return true;

	case 1:		// RLE
		while (exsize > 0)
		{
			uint a = *src++;
			if (a & 0x80)
			{
				uint l = (a & 0x7f) + 3;
				uint b = *src++;
				l = l > exsize ? exsize : l;
				exsize -= l;
				for (; l > 0; l--)
					*buf++ = b;
			}
			else
			{
				uint l = (a & 0x7f) + 1;
				l = l > exsize ? exsize : l;
				exsize -= l;
				for (; l > 0; l--)
					*buf++ = *src++;
			}
		}
		if (0)
		{
			crc = -1;
			exsize = orgsize > bufsize ? bufsize : orgsize;
			uint i;
			for (i=0; i<exsize-2; i++)
				calc_crc(dt[i]);
			uint dcrc = *(const uint16*)(dt + exsize - 2);
			if (verbose) 
				printf("(%.4x-%.4x)", dcrc & 0xffff, src - st);
			if (((dcrc ^ crc) & 0xffff) || pksize - (src - st))
				printf("xcom: failed to decompress data\n");
		}

		return true;

	default:
		printf("unsupported-compression method\n");
		return false;
	}
}

uint recv_packet(uint8* buffer, int length, bool burst)
{
	uint count = 3;
	uint8 recvbuf[0x4000];
	uint8 c;

	//if (burst) 
	//	unc = -1, sio.SetMode(baud * 2);

	do
	{
		if (unc != -1)
		{
			c = unc, unc = -1;
		}
		//else if (SIO::OK != sio.Read(&c, 1))
		else if (read(RSPORT, &c, 1) < 0)
		{
			if (!--count)
				return 2;
			continue;
		}
	} while (c != '%' && c != '#' && c != '$');

	try
	{
		init_pack();
		uint i = recv_word();
		if (i != xc_index)
			printf("protocol fail");

		uint l = recv_word() & 0x3fff;
//		printf("RECV[%.4x][%.4x(%.4x)]", index, l, length);
		
		if (c != '$')
		{
			for (; length > 0 && l > 0; length--, l--)
				*buffer++ = recv_byte();
			for (; l > 0; l--)
				recv_byte();
		}
		else
		{
			for (uint j=0; j<l; j++)
				recvbuf[j] = recv_byte();
		}

		if (crc != recv_word(false))
			printf("crc fail");

		//if (burst)
		//	sio.SetMode(baud);

		if (c == '$')
		{
			if (!decompress_packet(buffer, length, recvbuf, l))
				printf("decompress fail");
#if 0
			char buf[100];
			static int c = 0;
			sprintf(buf, "packet%.3d.bin", c++);
			FileIO fio(buf, FileIO::create);
			uint orgsize = (recvbuf[0] + recvbuf[1] * 256) & 0x3fff;
			fio.Write(buffer, orgsize);
#endif
		}
		return 0;
	}
	catch (int err)
	{
		//if (burst)
		//	sio.SetMode(baud);
		return err;
	}
}


uint send_packet(const uint8_t* buffer, int length, bool cmd, bool data)
{

	uint8_t buf[0x4000];
	if(data)
	{
		if(verbose)
			printf(" (%4d", length);
		int t;
		// Calculate CRC
		crc = -1;
		for(t = 0; t<length; t++)
			calc_crc(buffer[t]);
		int cl = enc.Encode(buffer, length, buf, sizeof(buf));
		buf[cl] = crc & 0xff;
		buf[cl+1] = (crc >> 8) & 0xff;
		cl += 2;
#if 0 
		uint8_t decbuf[0x10000];
		enc.Decode(decbuf, buf+2, buffer);
#endif 
		if(verbose)
			printf(" -> %4d, %3d%%)", cl, cl * 100 / length);
		if (cl < length)
			buffer = buf, length = cl;
		else 
			data = false;
	}

	int i ;
	uint err = -2;//e_connect;
	for(i=3; i>0; i--)
	{
		try{
			// header chars
			uint8_t hdr = cmd ? '!' : data ? '$' : '#';
			if(write(RSPORT, &hdr, 1) < 0){
				fputs("write() failed.\n", stderr);
				return 1;
			}
			init_pack();
			if(cmd) xc_index++;
			send_word(xc_index);
			send_word(length);
			for(int l=0; l<length; l++)
				send_byte(buffer[l]);
			send_word(crc, false);
			send_flush();
			if(!cmd)
				return 0;
			uint8_t buf;
			if(recv_packet(&buf, 1) > 0)
			{
				if(buf == 0)
					return 1;
				return 0;
			}
		}
		catch(int e)
		{
			printf("error\n");
		}

	}
	return err;
}

struct IDR
{
	uint8 c, h, r, n;
	uint8 density;
	uint8 status;
	uint8 deleted;
	uint8* buffer;
	uint length;
};

struct SectorHeader
{
	uint8 c, h, r, n;
	uint16 sectors;
	uint8 density;
	uint8 deleted;
	uint8 status;
	uint8 reserved[5];
	uint16 length;
};

int make_write_seq(uint8* dest, IDR* idr, int nsec)
{
	return 0;
}

bool send_disk(int drive, FILE* f)
{

	uint8_t buffer[0x3800];
	uint r;
	const char* type[] = { "2D", "2DD", "???", "2HD" };
	// Read in d88 header
	ImageHeader ih;
	fread(&ih, sizeof(ImageHeader), 1, f);
	// TODO: Check validity of image header 
	//if (!CheckHeader(&ih))
	//	return false;
	media = ih.disktype == 0x20 ? 3 : ih.disktype == 0x10 ? 1 : 0;
	printf("Writing disk: [%.16s] (%s)\n", ih.title, type[media]);
	// TODO: GetSysInfo()
	//if (!GetSysInfo()->fddtype && media)
	//{
	//	printf("この機種では %s のディスクイメージは書き戻しできません．\n", type[media]);
	//	return false;
	//}
	while (1) 
	{
		printf("Please insert a new disk into your PC88 drive %d and press any key.\n", drive+1, type[media]);
		getchar();
		
		// Set drive
		buffer[0] = 2;
		buffer[1] = drive;
		buffer[2] = media | 0x80; // This is the packet format for all drive commands
		r = send_packet(buffer, 3, true);
		if (r < 0)
			return false;
		//return false;
		r = recv_packet(buffer, 1);
		if (r < 0)
			return false;

		if (verbose)
			printf("media = %d\n", buffer[0]);

		if (buffer[0] < 4)
			break;
	
		if (buffer[0] == 0xf9)
		{
			printf("Disk is write protected.\n");
			return false;
		}

		if (buffer[0] != 0xfe)
		{
			printf("Disk control error.\n");
			return false;
		}
	}
	int ntracks = media & 1 ? 164 : 84;

	IDR idr[64];

	for(int tr=0; tr<ntracks; tr++)
	{
		printf("Track %3d:", tr);
		int i =0;
		if(ih.trackptr[tr])
		{
			fseek(f, ih.trackptr[tr], SEEK_SET);
			uint8* buf = buffer;
			int buffre = 0x3000;
			SectorHeader sh;
			do {
				fread(&sh, sizeof(SectorHeader), 1, f);
				int length = sh.length > buffre ? buffre : sh.length;
				fread(buf, length, 1, f);
				if(sh.length - length)
					fseek(f, sh.length-length, SEEK_CUR);
				memset(&idr[i], 0, sizeof(IDR));
				idr[i].c = sh.c;
				idr[i].h = sh.h;
				idr[i].r = sh.r;
				idr[i].n = sh.n;
				idr[i].density = sh.density ^ 0x40;
				idr[i].buffer = buf;
				idr[i].length = length;
				idr[i].deleted = sh.deleted;
				idr[i].status = sh.status;
				buf += length;
				buffre -= length;
			} while ( ++i < sh.sectors);
			printf(" %2d sectors", i);
		}
		else { 
			printf(" unformatted");
		}

		uint8_t seq[0x3800];
		int l = make_write_seq(seq, idr, i);
		if(l < 0)
			return false;
		uint8 cmd[4];
		cmd[0] = 9;
		cmd[1] = tr;
		cmd[2] = l & 0xff;
		cmd[3] = l >> 8;
		if(send_packet(cmd, 4, true) > 0)
			return false;
		if(send_packet(seq, l, false, true) > 0)
			return false;
		if(recv_packet(cmd, 1) > 0)
			return false;
		printf(".\n");
	}
	static const uint8_t cmd[] = { 0x00 };
	send_packet(cmd, 1, true);
	return true;
}

// ---------------------------------------------------------------------------
//	MAIN
//
int main(int ac, char** av)
{

	int index = 1;
	printf("TransDisk 2.%.2d\n"
		   "Copyright (C) 2000 cisc.\n\n", version_l);

	if (ac < 2)
		Usage();
	
	mode = (av[1][0]);
	
	av += 2;
	for (int i=2; i<ac; i++, av++)
	{
		char* arg = *av;
		
		if (arg[0] == '-')
		{
			for (int j=1; arg[j]; j++)
			{
				switch ((arg[j]))
				{
				case 'v':
					verbose = true;
					break;
				
				case 'w':
					writeprotect = true;
					break;

				case 's':
					fastrecv = false;
					break;
				
				case 'p':
					port = atoi(arg + j);
					goto next;

				case 'm':
					media = atoi(arg + j + 1);
					if (media < 0 || media > 2)
						Usage();
					if (media == 2)
						media = 3;
					goto next;

				case 'd':
					drive = atoi(arg + j + 1);
					if (drive != 1 && drive != 2)
						Usage();
					goto next;

				case 't':
					if (++i < ac)
					{
						strncpy(title, *++av, 16);
						goto next;
					}
					Usage();

				case '1':
					baud = B19200;
					break;
				
				case '9':
					baud = B9600;
					break;
				
				default:
					Usage();
				}
			}
		}
		else
		{
			if (filename)
				Usage();
			filename = arg;
		}
next:	
		;
	}

	if (mode == 'r' || mode == 'w')
	{
		if (!filename || (drive != 1 && drive != 2))
		{
			Usage();
			return 0;
		}
	}
	
	if (mode == 'b')
	{
		printf("Connecting to transfer port %d.\n\n", port);//, baud);
		RSPORT = open_port(port);
		if(RSPORT == -1)
		{
			printf("Failed to initialize communication device.\n");
			return 1;
		}

		printf( "Transferring Xdisk2 BASIC program to the 88.\n"
				"Please enter: load \"COM:N81X\" on your 88.\n\n"
				"Press any key when ready...");

		getchar();
		printf("\n");
		// send_basic() :
		int n;
		for(int i = 0; i < sizeof(xdisk2bas); i++) {
			n = write(RSPORT, &xdisk2bas[i], 1);
			if (n < 0)
				fputs("write() failed.\n", stderr);
		}
		close(RSPORT);
		// :
		printf("Complete.\n");
		return 0;
	}

	RSPORT = open_port(port);
	if(e == -1) { 
		printf("Failed to initialize communication device.\n");
	}
	
	switch (mode)
	{
		
	case 'r':
		{
			/*
			FileIO file;
			if (!file.Open(filename, FileIO::openalways) 
				|| (file.GetFlags() & FileIO::readonly))
			{
				fprintf(stderr, "�ե��������ޤ���.\n");
				return 1;
			}
			D88Seek(file, 99999);

			xdisk.ReceiveDisk(drive-1, media, file);
			file.Seek(0, FileIO::begin);
			file.Write(title, 16);
			file.Seek(26, FileIO::begin);
			uint8 wp = writeprotect ? 0x10 : 0;
			file.Write(&wp, 1);
			*/
			printf("TODO\n");
		}
		break;
		
	case 'w':
		{
			FILE *fptr;
			fptr = fopen(filename, "rb");
			if(fptr == NULL) {
				fprintf(stderr, "Cannot open file.\n");
				return 1;
			}
			unsigned char num = 0;
			if(!D88Seek(fptr))
			{
				fprintf(stderr, "Disk can't be opened.\n");
				return 1;
			}
			//fread(......&num, 1, 1, fptr);
			send_disk(drive-1, fptr);
			// D88Seek sets a file pointer to the disk's data start,
			//  and records the total size of the disk.
			//  If its false it's not a disk...
			
		}
		break;

	case 's':
		//xdisk.ReceiveROM();
		printf("TODO\n");
		break;
	
	default:
		Usage();
	}
	printf("Complete.\n");
	return 0;
}

// ---------------------------------------------------------------------------
uint32_t logorg = 0;
// Returns TRUE when the d88 file has been 'seek'd to the data start
static bool D88Seek(FILE* f)
{
	uint32_t size = 0;
	uint32_t pos = 0;
	fseek(f, pos + 0x1c, SEEK_SET); // skip 1c bytes
	fread(&size, 4, 1, f);
	if (size == 0xdeadd13c)					// fail if its too big
		return false;
	if (size == 0 || size > 4 * 1024 * 1024)
		return false;

	fseek(f, 0, SEEK_SET);
	
	return true;
}
//*/
// ---------------------------------------------------------------------------

void Usage()
{
	printf("usage:\n"
		   "  xdisk2 b [-p#]\n"
		   "          Send BASIC program to the PC-8801\n"
		   "  xdisk2 s [-p#] [-19s]\n"
		   "          Extract ROM data from the PC-8801\n"
		   "  xdisk2 r [-p#] [-d#] [-m#] [-19svw] [-t \"title\"] <disk.d88>\n"
		   "          Write a disk image from a disk\n"
		   "  xdisk2 w [-p#] [-d#] [-19v] <disk.d88>\n"//(;#)>\n"
		   "          Write out a disk image to a disk\n"
		   "\n"
		   "option:\n"
           "    -p#   Set port no. (e.g. /dev/ttyUSB#)\n"
           "    -1/-9 Transfer at 19200 or 9600 bps\n"
		   "    -d#   Set drive number to read/write to\n"
		   "    -m#   Set media type for read disk (0:2D 1:2DD 2:2HD)\n"
		   "    -v    Detailed output\n"
		   "    -w    Set write protect on the disk image\n"
		   //"    -s    88 -> PC �֤ǹ�®ž����Ԥ�ʤ� (19200 bps ���Τ�)\n"
		   "    -t    Set disk title to...\n");
	return;
}

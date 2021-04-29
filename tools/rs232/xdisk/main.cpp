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
#include <sys/ioctl.h>

#include "lz77e.h"


typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;

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
u8 basbuf[0x40000];
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
int status;
void send_byte(int a, bool cc = true);
inline void send_word(int a, bool cc = true);
uint send_packet(const uint8_t* buffer, int length, bool cmd = false, bool data = false);
uint recv_packet(uint8* buffer, int length, bool burst = false);
int recv_word(bool cc=true);
int recv_byte(bool cc=true);
uint recv_c();

inline void calc_crc(int data)
{
	crc = (crc << 8) ^ crctable[(data ^ (crc >> 8)) & 0xff];
}
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
	//ioctl(fd, TIOCMGET, &status);
	/*
    options.c_cflag |= (CLOCAL | CREAD);
    
    */
	/*dcb.BaudRate		= baud; OK
	dcb.fBinary			= TRUE; // RAW?
	dcb.fParity			= FALSE; // no parity bit OK
	dcb.fOutxCtsFlow	= TRUE; // cts
	dcb.fOutxDsrFlow	= TRUE; // dsr
	dcb.fDtrControl		= DTR_CONTROL_ENABLE; // dtr
	dcb.fDsrSensitivity	= FALSE; // ?
	dcb.fTXContinueOnXoff = FALSE; // ???
	dcb.fOutX			= TRUE; //ixon
	dcb.fInX			= FALSE; // no ixoff
	dcb.fNull			= FALSE; // ?
	dcb.fRtsControl		= RTS_CONTROL_HANDSHAKE; // ?
	dcb.fAbortOnError	= FALSE; // ?
	dcb.ByteSize		= 8; // ok
	dcb.Parity			= NOPARITY; // ok
	dcb.StopBits		= ONESTOPBIT; // ok
	dcb.EvtChar			= 10; ok */
	//options.c_cc[VINTR] = 10;
	//status |= TIOCM_CTS;
	//status |= TIOCM_DSR;
	//status |= TIOCM_DTR;
	options.c_cflag &= ~CSIZE; // mask size =8
    options.c_cflag |= CS8;     // set
	options.c_cflag &= ~PARENB; // mask parity =N
	options.c_cflag &= ~CSTOPB; // mask stop bit =1
	
	//options.c_iflag |= IXON;
	//options.c_cflag |= CRTSCTS;
	//ioctl(fd, TIOCMSET, status);
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
		printf("malformed byte");
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

void make_normal_id(uint8*& dest, IDR* idr, int nsec, int le)
{
	le = le > 8 ? 8 : le;
	int slen = (0x80 << le);
	int gap3;
	int tracksize = media & 2 ? 10416 : 6250;
	
	if (!idr->density)
	{
		tracksize >>= 1;
		gap3 = (tracksize - 137 - nsec * ((0x80 << le) + 32)) / nsec;
		if (gap3 < 12)
			gap3 = (tracksize - 73 - nsec * ((0x80 << le) + 32)) / nsec;
		if (gap3 < 8)
			gap3 = (tracksize - 32 - nsec * ((0x80 << le) + 32)) / nsec;
	}
	else
	{
		gap3 = (tracksize - 274 - nsec * ((0x80 << le) + 62)) / nsec;
		if (gap3 < 24)
			gap3 = (tracksize - 146 - nsec * ((0x80 << le) + 62)) / nsec;
		if (gap3 < 16)
			gap3 = (tracksize - 64 - nsec * ((0x80 << le) + 62)) / nsec;
	}
	
	*dest++ = 1 | idr->density;	// WRITE ID
	*dest++ = le;	// LE
	*dest++ = nsec;	// NS
	*dest++ = gap3; // GAP
	*dest++ = 0;	// D
	for (int i=0; i<nsec; i++)
	{
		*dest++ = idr[i].c;
		*dest++ = idr[i].h;
		*dest++ = idr[i].r;
		*dest++ = idr[i].n;
	}
	printf(" (N:%d Gap3:%.2xh)", le, gap3);
}

void make_mix_length_id(uint8*& dest, IDR* idr, int nsec, int le, int gap)
{
	*dest++ = 1 | idr->density;	// WRITE ID
	*dest++ = le;	// LE
	uint8* psec = dest++;
	*dest++ = gap; // GAP
	*dest++ = 0x4e;	// D
	printf(" (N:%d Gap3:%.2xh[Mixed])", le, gap);
	int misc = idr->density ? 64 : 34;
	int sos = (0x80 << le) + 0x3e + gap;
	int ws = 0;
	for (int s=0; s<nsec; s++)
	{
		if (!idr->status)
		{
			int size = sos - ((0x80 << idr->n) + misc);
			*dest++ = idr->c;
			*dest++ = idr->h;
			*dest++ = idr->r;
			*dest++ = idr->n;
			idr++, ws++;
			
			if (s < nsec - 1 && size < 0)
			{
				while (size<0)
				{
					dest += 4, size += sos, ws++;
				}
			}
		}
	}
	*psec = ws;
}

bool calc_mix_length_condition(IDR* idr, int nsec, int* mle, int* mgap, int tracksize)
{
	int le;
	int gap3;
	int max = 0;
	int misc = idr->density ? 62 : 33;
	for (le=0; le<=3; le++)
	{
		for (gap3=2; gap3<256; gap3++)
		{
			int sgap = 9999;
			int sos = (0x80 << le) + misc + gap3;
			int bytes = 0;
			IDR* i = idr;
			int ns = 0;
			for (int s=0; s<nsec; s++, i++)
			{
				if (!i->status)
				{
					int size = -((0x80 << i->n) + misc + 2);
					while (size<0)
						size += sos, bytes += sos, ns++;
					if (sgap >= size)
						sgap = size;
				}
			}
			if (bytes < tracksize && ns < 64)
			{
				if (max < sgap)
				{
//					printf("sgap: %d  bytes: %d¥n", sgap, bytes);
					max = sgap, *mle = le, *mgap = gap3;
				}
			}
		}
	}
	return max > 0;
}

int make_write_seq(uint8* dest, IDR* idr, int nsec)
{
	int i;
	//ID
	if(!nsec){
		dest[0] = 0x41; // id
		dest[1] = media & 2 ? 7 : 6; // le
		dest[2] = 1; // ns 
		dest[3] = 6; //gap
		dest[4] = 0x4e; // D
		dest[5] = 0;
		return 6;

	}
	uint8_t* dorg = dest;
	int le = -1;
	bool mlen = false;
	for(i=0;i<nsec;i++)
	{
		if(!(idr[i].status))
		{
			if(le!=-1 && le!= idr[i].n)
				mlen = true;
			else 
				le = idr[i].n;
		}
	}
	if(mlen)
	{
		int gap3;
		int tracksize = (media & 2 ? 10416 : 6250) - 50;
		if(idr[0].density == 0) tracksize >>= 1;
		if(!calc_mix_length_condition(idr, nsec, &le, &gap3, tracksize))
		{
			printf("[TOO DIFFICULT!]");
		}
		make_mix_length_id(dest, idr, nsec, le, gap3);
	}
	else { 
		make_normal_id(dest, idr, nsec, le);
	}
	//data 
	for(i=0;i<nsec;i++){
		if(idr[i].status)
			continue;

		*dest++ = 2 | idr[i].density | (idr[i].deleted ? 0x80 : 0);
		*dest++ = idr[i].c;
		*dest++ = idr[i].h;
		*dest++ = idr[i].r;
		*dest++ = idr[i].n;
		*dest++ = idr[i].length & 0xff;
		*dest++ = (idr[i].length >> 8) & 0xff;
		memcpy(dest, idr[i].buffer, idr[i].length);
		dest += idr[i].length;
	}
	*dest++ = 0;

	return dest - dorg;
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
		init_pack();
		u8 hdr = '!';
		send_byte(hdr, false);
		send_word((u16)0);
		send_word((u16)3);
		for(u8 w = 0; w < 3; w++) send_byte(buffer[w]);
		send_word(crc, false);
		
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
	return 0;
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

	//int index = 1;
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
		//printf(filename);
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
		        // read file to write into buffer     
        FILE* diskf;
        diskf = fopen(filename, "rb");
        unsigned long sz = 0;
        fseek(diskf, 0L, SEEK_END);
        sz = ftell(diskf);              // get filesize 
        fseek(diskf, 0x0, SEEK_SET);    // and reset file pointer
        fread((void*)&basbuf, sz, 1, diskf);  // read the entire thing into ram
        fclose(diskf);      // .. and close the file

        // Write the file
        int n;
        write(RSPORT, &basbuf, sz);
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

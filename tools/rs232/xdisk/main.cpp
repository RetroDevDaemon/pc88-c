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

void Usage();

bool verbose = false;
int version_l = 7;
int drive = -1;
//	int retry = 10;
char* filename = 0;
char title[17] = "                ";
bool writeprotect = false;
bool fastrecv = true;
int media = -1;
uint baud = 19200;
uint port = 0;
uint16_t crc = -1;
LZ77Enc enc;

static bool D88Seek(FILE* f);
bool send_disk(int drive, FILE* f);

// Only works with ttyUSB0 right now!
int open_port(int p)
{
	int fd;
    // O_RDWR - enable read and write
    // O_NOCTTY - do not make TTY for the port
    // O_NDELAY - does not care if other side is ready (data carrier detect)
    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd == -1)
    {   // couldn't open?
        perror("open_port: Unable to open /dev/ttyUSB0 - ");
    }
    else
        fcntl(fd, F_SETFL, 0);
    return (fd);
}

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

uint16_t crctable[0x100] = { 123 };
//SIO sio;
//SystemInfo si;
uint16_t xc_index;
int d, e;
//uint16_t crc;
bool connect;
int unc;
//int baud;
	

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

//#define calc_crc(crc) crc = (crc << 8) ^ crctable[(data ^ (crc >> 8)) & 0xff]

uint send_packet(const uint8_t* buffer, int length, bool cmd = false, bool data = false);

uint send_packet(const uint8_t* buffer, int length, bool cmd, bool data)
{
	int pf = open_port(port);
	if(pf == -1)
	{
		printf("Failed to initialize communication device.\n");
		return 1;
	}
	uint8_t buf[0x4000];
	if(data)
	{
		if(verbose)
			printf(" (%4d", length);
		int t;
		// Calculate CRC
		crc = -1;
		for(t = 0; t<length; t++)
			crc = (crc << 8) ^ crctable[(data ^ (crc >> 8)) & 0xff];
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
			if(write(pf, &hdr, 1) < 0){
				fputs("write() failed.\n", stderr);
				return 1;
			}
			//InitPack()
			// TODO here
		}
		catch(int e)
		{
			printf("error\n");
		}

	}
	return err;
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
	media = ih.disktype == 0x20 ? 3 : ih.disktype == 0x10 ? 1 : 0;
	printf("Writing disk: [%.16s] (%s)\n", ih.title, type[media]);
	// TODO: GetSysInfo()
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
			printf("ディスクはライトプロテクトされています.\n");
			return false;
		}

		if (buffer[0] != 0xfe)
		{
			printf("ディスク操作に関するエラーが生じました.\n");
			return false;
		}
	}

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
	
	int mode = (av[1][0]);
	
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
					baud = 19200;
					break;
				
				case '9':
					baud = 9600;
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
		int pf = open_port(port);
		if(pf == -1)
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
			n = write(pf, &xdisk2bas[i], 1);
			if((i-1)%256==0)std::cout<<"."<<std::flush;
			if (n < 0)
				fputs("write() failed.\n", stderr);
		}
		close(pf);
		// :
		printf("Complete.\n");
		return 0;
	}

	int e = open_port(port);
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
				fprintf(stderr, "ファイルを作れません.\n");
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
			/*
			FileIO file;
			
			_mbstok((uchar*) filename, (uchar*) ";");
			char* x = (char*) _mbstok(0, (uchar*) ";");
			int index = x ? atoi(x) : 1;

			if (!file.Open(filename, FileIO::readonly))
			{
				fprintf(stderr, "ファイルを開けません.\n");
				return 1;
			}
			if (!D88Seek(file, index))
			{
				fprintf(stderr, "指定されたディスクがありません.\n");
				return 1;
			}
			xdisk.SendDisk(drive-1, file);
			*/

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
		   //"    -s    88 -> PC 間で高速転送を行わない (19200 bps 時のみ)\n"
		   "    -t    Set disk title to...\n");
	return;
}

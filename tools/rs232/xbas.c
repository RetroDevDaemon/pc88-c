#include <stdio.h>
#include <string.h>
#include <unistd.h>     // unix
#include <fcntl.h>      // file control
#include <errno.h>      // error number definitions
#include <termios.h>    // posix terminal stuff
#include <sys/ioctl.h>

//#include "xdisk2bas.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned char bool;
typedef u8 uint8_t;
typedef u8 uint8;
typedef unsigned int uint;
typedef u16 uint16;
typedef u16 uint16_t;
typedef u32 uint32;
typedef u32 uint32_t;

#define true 1
#define false 0
int status;
u16 p_len = 0;
u8 media = -1;
u8 drive = 2;
u16 p_index = 0;
uint16_t crctable[0x100] = { 123 };
static u8 fpa[] = "/dev/ttyUSB ";
static u8 buf[0x40000];
u8 port = 0;
struct termios options;
u16 baud = B9600;
char mode = 'q';
int fp;

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
struct SystemInfo
{
    uint8 verh;
    uint8 verl;
    uint8 fddtype;
    uint8 romtype;
    uint8 siotype;
};
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

/** open_port()
* open usb serial port. returns FS or -1 on error
*/
int open_port(unsigned char port) {
    // O_RDWR - enable read and write
    // O_NOCTTY - do not make TTY for the port
    // O_NDELAY - does not care if other side is ready (data carrier detect)
    if(crctable[0] == 123) build_crc_table();
    fpa[11] = port | 0x30;
    int fd = open(fpa, O_RDWR);
    
    // set options 
    tcgetattr(fd, &options);
    baud = B9600;
    cfsetispeed(&options, (speed_t)baud);
    cfsetospeed(&options, (speed_t)baud);
    
    options.c_cc[VINTR] = 10;
	//status |= TIOCM_CTS;
	//status |= TIOCM_DSR;
	//status |= TIOCM_DTR;
	options.c_cflag &= ~CSIZE; // mask size =8
    options.c_cflag |= CS8;     // set
	options.c_cflag &= ~PARENB; // mask parity =N
	options.c_cflag &= ~CSTOPB; // mask stop bit =1
	/*
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag |= CREAD | CLOCAL;
    options.c_lflag &= ~ICANON;
    options.c_lflag &= ~ISIG;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    //options.c_iflag |= IXON;
    options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    options.c_oflag &= ~OPOST; 
    options.c_oflag &= ~ONLCR;
	options.c_cc[VTIME] = 10;
	options.c_cc[VMIN] = 0;
*/
    //ioctl(fd, TIOCMSET, status);
    
    tcflush(fd, TCIFLUSH );
	tcsetattr(fd, TCSANOW, &options); // (N81X)

    if(fd == -1)
    {   // couldn't open?
        perror("open_port: Unable to open /dev/ttyUSB0 - ");
    }
    else
        fcntl(fd, F_SETFL, 0);
    return (fd);
}

int d, e;
unsigned short crc;

void init_pack()
{
	d = e = 0;
	crc = ~0;
}

void send_word(int a, bool cc);
void send_byte(int a, bool cc);

void send_c(uint8 c)
{
	c = (c & 0x7f) + 0x40;
    write(fp, &c, 1);
    //printf("%c",c);
}

void calc_crc(int data)
{
    crc = (crc << 8) ^ crctable[(data ^ (crc >> 8)) & 0xff];

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

void send_flush()
{
	if (e)
		send_c(d);
}

void send_word(int a, bool cc)
{
    send_byte(a & 0xff, cc);
    send_byte((a>>8)&0xff, cc);
}
uint RecvPacket(uint8* buffer, int buffersize, bool burst) // burst=false
{
    return 1;
}

uint recv_c()
{
	uint8 c;
	read(fp, &c, 1);
	if ((c - 0x40) & 0x80)
	{
        //unc = c;
		printf("protocol fail");
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

int recv_word(bool c)
{
    int r = recv_byte(c);
    return r + recv_byte(c) * 256;
}

int main(int argc, char* argv[])
{
    // arg check
    if(argc < 2) goto _man;
    int act = argc - 1;
    while(act > 0)
    {
        if(argv[act][0] == '-') {
            switch (argv[act][1]) {
                case 'p':
                    port = (argv[act][2] - 0x30);
                    break;
                case 's':
                    switch(argv[act][2] - 0x30){
                      case(9):
                        baud=B9600;
                        break;
                      case(1):
                        baud=B19200;
                        break;
                      default:
                        goto _man;
                        break;
                    }
            } // end switch
        }
        
        mode = argv[act][0];
        if(mode == 'q') goto _man;
        
        act--;
    }
    
    // open usb serial port
    fp = open_port(port);
    if(fp == -1) { // couldn't open?
        printf("open_port: Unable to open %s\n", fpa);
        return 1;
    }
    /*
///////////////////////////////////////////////////
// PACKET ONE TEST 
///////////////////////////////////////////////////
    // send_packet:cmd:1

    u8 ex = '!';
    u8 cx = 1;
    u8 pak=' ';//[256];
    p_index = 0;
    //for(u8 i = 0; i < 256; i++) pak[i] = 0;
    u8 r = 255;
    //  
    init_pack();
    p_index++;
    write(fp, &ex, 1);
    send_word((u16)p_index, true);
    send_word((u16)1, true); //len
    send_byte((u8)1, true); //data
    send_word((u16)crc, false);
    send_flush();
    while(pak != '%' && pak!= '#' && pak!='$'){
        r = read(fp, &pak, 1);
        if (r == 255) perror("error ");
    }
    printf("received ack %c\n",pak);
    init_pack();
    u16 indextest = recv_word(true);
    printf("index test %d = %d\n",indextest, p_index);
    if(indextest != p_index) printf("packet index mismatch.");
    uint l = recv_word(true) & 0x3fff;
    printf("RECV[%.4x][%.4x]", p_index, l);
    u8 recvbuf[0x4000];
    u8* rb = &recvbuf;
    for(;l>0;l--){ 
        *rb++ = recv_byte(true);
    }
    if(crc != recv_word(false)) printf("crc error");
    else printf("wow this packet is OK!\n");
    
    // get another packet?
    while(pak != '%' && pak!= '#' && pak!='$'){
        r = read(fp, &pak, 1);
        if (r == 255) perror("error ");
    }
    printf("received ack %c\n",pak);
    init_pack();
    indextest = recv_word(true);
    printf("index test %d = %d\n",indextest, p_index);
    if(indextest != p_index) printf("packet index mismatch.");
    l = recv_word(true) & 0x3fff;
    printf("RECV[%.4x][%.4x]", p_index, l);
    //u8 recvbuf[0x4000];
    rb = &recvbuf;
    for(;l>0;l--){ 
        *rb++ = recv_byte(true);
    }
    if(crc != recv_word(false)) printf("crc error");
    else printf("wow this packet is OK!\n");
    */
/////////////////////////////////////

    printf("On your PC-88, enter: load\"COM:N81X\"\nPress any key when ready...\n");
    getchar(); // the any key   
    if(mode == 'b')
    {
        printf("Write using 'w' instead.\n");
		return 0;
    }
    else if(mode == 'w'){
    
        // read file to write into buffer     
        FILE* diskf;
        diskf = fopen(argv[argc-1], "rb");
        unsigned long sz = 0;
        fseek(diskf, 0L, SEEK_END);
        sz = ftell(diskf);              // get filesize 
        fseek(diskf, 0x0, SEEK_SET);    // and reset file pointer
        fread((void*)&buf, sz, 1, diskf);  // read the entire thing into ram
        fclose(diskf);      // .. and close the file

        //uint8_t buffer[0x3800];
        uint r;
        
        // Before: Write the file
        //int n;
        write(fp, &buf, sz);
        // After: send command 
///////////////////////////////////////////////////
// PACKET TWO TEST 
///////////////////////////////////////////////////
       // send_packet:cmd:getdisk
        /*
        const char* type[] = { "2D", "2DD", "???", "2HD" };
        // Read in d88 header
        struct ImageHeader ih;
        fread(&ih, sizeof(struct ImageHeader), 1, diskf);
        // TODO: Check validity of image header 
        //if (!CheckHeader(&ih))
        //	return false;
        media = ih.disktype == 0x20 ? 3 : ih.disktype == 0x10 ? 1 : 0;
        printf("Writing disk: [%.16s] (%s)\n", ih.title, type[media]);
        */
///////////////////////////////////////////
    }

    // cleanup
    close(fp);
    
    printf("Complete.\n");
    return 0;

_man:
    printf(" -= XBAS v1.0 =-\nBased on Xdisk2 by Cisc\nAdapted by @RetroDevDiscord\n\n\
  Usage: xbas <mode> [-p#] [-s#] <file.bas>\n\n\
  Where:\n\tmode:\tb : Copy over TransDisk/88 BASIC prog\n\t\tw : Write binary file to 88\n\tp#:\t\
USB-Serial port no., e.g. /dev/ttyUSB#\n\ts#:\tBaudrate, 1=19200, 9=9600\n\n");
    return 1;    
}

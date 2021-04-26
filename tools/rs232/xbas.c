#include <stdio.h>
#include <string.h>
#include <unistd.h>     // unix
#include <fcntl.h>      // file control
#include <errno.h>      // error number definitions
#include <termios.h>    // posix terminal stuff

#include "xdisk2bas.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

static u8 fpa[] = "/dev/ttyUSB ";
static u8 buf[0x40000];
u8 port = 0;
struct termios options;
u16 baud = B9600;
char mode = 'q';

/** open_port()
* open usb serial port. returns FS or -1 on error
*/
int open_port(unsigned char port) {
    // O_RDWR - enable read and write
    // O_NOCTTY - do not make TTY for the port
    // O_NDELAY - does not care if other side is ready (data carrier detect)
    fpa[11] = port | 0x30;
    int fd = open(fpa, O_RDWR | O_NOCTTY);
    
    // set options 
    
    tcgetattr(fd, &options);
    //baud = B4800;
    //cfsetispeed(&options, baud);
    //cfsetospeed(&options, baud);
    options.c_iflag |= (IXON | IXOFF | IXANY | IGNCR);
    options.c_oflag &= ~(OPOST);
    options.c_oflag &= ~(ONLCR | ONLRET | OCRNL);
    options.c_oflag &= ~(NLDLY);
    options.c_oflag |= NLDLY;
    options.c_oflag &= ~(CRDLY);
    options.c_oflag |= CR0;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    
    //options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CBAUD;
    options.c_cflag |= baud;
    options.c_cflag &= ~PARENB; // mask parity =N
    options.c_cflag &= ~CSTOPB; // mask stop bit =1
    options.c_cflag &= ~CSIZE; // mask size =8
    options.c_cflag |= CS8;     // set
    tcsetattr(fd, TCSANOW, &options); // (N81X)
        
    fcntl(fd, F_SETFL, 0); // blocking wait
    
    return (fd);
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
        
        if(argv[act][0] == 'b') {
            mode = 'b';
        }
        else if(argv[act][0] == 'w') {
            mode = 'w';
        }
        else if(argv[act][0] == 'q') {
            goto _man;
        }
        
        act--;
    }
    
    // open usb serial port
    int fp = open_port(port);
    if(fp == -1) { // couldn't open?
        printf("open_port: Unable to open %s\n", fpa);
        return 1;
    }

    printf("On your PC-88, enter: load\"COM:N81X\"\nPress any key when ready...\n");
    getchar(); // the any key   
    if(mode == 'b')
    {
        int n;
		for(int i = 0; i < sizeof(xdisk2bas); i++) {
			n = write(fp, &xdisk2bas[i], 1);
			if (n < 0)
				fputs("write() failed.\n", stderr);
		}
		close(fp);
		printf("Complete: Enter RUN to start TransDisk/88.\n");
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

        // Write the file
        int n;
        /*
        for(int i = 0; i < 256+4; i += 1) {
            n = write(fp, (u8*)&buf[i], 1);
            if (n < 0) {
                fputs("write() failed.\n", stderr);
            }
        }
        */
        diskf = fopen("b.bin", "wb");
        fwrite(&buf, sz, 1, diskf);
        fclose(diskf);
        
         write(fp, &buf, sz);
         //for(int i = 0; i < sz; i++)
         //{
        //    write(fp, &buf[i], 1);
        // }
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

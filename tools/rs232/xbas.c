#include <stdio.h>
#include <string.h>
#include <unistd.h>     // unix
#include <fcntl.h>      // file control
#include <errno.h>      // error number definitions
#include <termios.h>    // posix terminal stuff

typedef unsigned char u8;
static u8 fpa[] = "/dev/ttyUSB ";
static u8 buf[0x40000];
u8 port = 0;
    
/** open_port()
* open serial port 1. returns FS or -1 on error
*/
int open_port(unsigned char port) {
    // O_RDWR - enable read and write
    // O_NOCTTY - do not make TTY for the port
    // O_NDELAY - does not care if other side is ready (data carrier detect)
    fpa[11] = port | 0x30;
    int fd = open(fpa, O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd != -1)
        fcntl(fd, F_SETFL, 0);
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
            }
        }
        act--;
    }
    
    // open usb serial port
    int fp = open_port(port);
    if(fp == -1) { // couldn't open?
        printf("open_port: Unable to open %s\n", fpa);
        return 1;
    }

    // read file to write into buffer     
    FILE* diskf;
    diskf = fopen(argv[argc-1], "rb");
    unsigned long sz = 0;
    fseek(diskf, 0L, SEEK_END);
    sz = ftell(diskf);              // get filesize 
    fseek(diskf, 0x0, SEEK_SET);    // and reset file pointer
    fread((void*)&buf, 1, sz, diskf);  // read the entire thing into ram
    fclose(diskf);      // .. and close the file

    printf("On your PC-88, enter: load\"COM:N81X\"\nPress any key when ready...\n");
    getchar(); // the any key   
    // Write the file
    int n;
    for(int i = 0; i < sz; i++) {
        n = write(fp, &buf[i], 1);
        if (n < 0) {
            fputs("write() failed.\n", stderr);
            return 1;
        }
    }

    // cleanup
    close(fp);
    
    printf("Complete.\n");
    return 0;

_man:
    printf(" -= XBAS v1.0 =-\nBased on Xdisk2 by Cisc\nAdapted by @RetroDevDiscord\n\n\
  Usage: xbas [-p#] <file.bas>\n\n\
  Where:\n\tp#:\tUSB-Serial port no., e.g. /dev/ttyUSB0\n");
    return 1;    
}

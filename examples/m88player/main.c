#include <pc88-c.h>

#define DRIVE1 0

void byToHex(u8 by, u8* res)
{
    u8 a = by & 0xf;
    u8 b = (by & 0xf0) >> 4;
    if (a > 9) a += 7;
    if (b > 9) b += 7;
    a += 0x30;
    b += 0x30;
    res[0] = b; res[1] = a; res[2] = 0; 
}

void main()
{
    //IRQ_OFF 
    // Load MUCOM88!
    // Load 4608by (18rec) from TSR 10/0/1, drive 1 to 0xb000
    print("loading music driver .. ");
    DiskLoad((u8*)0xb000, 20, 1, 18, DRIVE1);
    // Load song to c200 - from TSR 5/0/1, 1280bytes
    print("loading music..");
    DiskLoad((u8*)0xc200, 12, 1, 5, DRIVE1);
    //IRQ_ON 
    print("playing ...");
    IRQ_OFF
    __asm
        ld a, #0
        call #0xb000
    __endasm;
    IRQ_ON     
    print("ok");
    while(1) {}
}
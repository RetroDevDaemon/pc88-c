
#include <pc88-c.h>

void main(void)
{   
    IRQ_OFF
    // V1 draw mode , one color plane at a time

    // Double the number from maked88. e.g.
    // $ python3 tools/maked88.py app.d88 file1 6 0 1
    // then => DiskLoad(DEST_ADDR, 12, 1, LENGTH/256, DRIVENO)
    SETBANK_RED()
    DiskLoad((u8*)0xc000, 4, 1, 63, DRIVE_1);
    SETBANK_GREEN()
    DiskLoad((u8*)0xc000, 16, 1, 63, DRIVE_1);
    SETBANK_BLUE()
    DiskLoad((u8*)0xc000, 22, 1, 63, DRIVE_1);

    SETBANK_MAINRAM()
    IRQ_ON

    while(1)
    {
        Wait_VBLANK();
    }
}
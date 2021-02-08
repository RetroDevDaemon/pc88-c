
#include "pc88-c.h"

void main()
{   
    IRQ_OFF
    // Pal test for layered planar gfx 
    /*
    SetIOReg(PAL_REG0, CLR_RED);
    SetIOReg(PAL_REG1, CLR_BLACK);
    SetIOReg(PAL_REG2, CLR_FUSCHIA);
    SetIOReg(PAL_REG3, CLR_GREEN);
    SetIOReg(PAL_REG4, CLR_CYAN);
    SetIOReg(PAL_REG5, CLR_CYAN);
    SetIOReg(PAL_REG6, CLR_CYAN);
    SetIOReg(PAL_REG7, CLR_CYAN);
    */
    
    PlanarBitmap* pb = &layeredImage;    
    DrawPlanarBitmap(pb, 0, 0);
    SetPixel(360, 180, CLR_BLUE);
    SETBANK_MAINRAM() // must reset after draw!

   /* SEMIGFC MODE
    print("\xe0\xce\xc0\x0e\x00");
    SetCursorPos(0, 1);
    print("\x70\xef\xf9\x07\x00");
    SetCursorPos(0, 2);
    print("\x47\xf8\x8f\xf4\x00");
    SetCursorPos(0, 3);
    print("\x10\x72\x3f\x01\x00");
    SetTextAttribute(0, 0, 0x78);
    SetTextAttribute(4, 0, bit(3));
    SetTextAttribute(0, 1, 0x58);
    SetTextAttribute(4, 1, bit(3));
    SetTextAttribute(0, 2, 0x98);
    SetTextAttribute(4, 2, bit(3));
    SetTextAttribute(0, 3, 0xd8);
    SetTextAttribute(4, 3, bit(3));
    */
    IRQ_ON

    while(1)
    {
        Wait_VBLANK();
        if(GetKeyDown(KB_RETURN)) print("HI!\x00");
    }
}


    
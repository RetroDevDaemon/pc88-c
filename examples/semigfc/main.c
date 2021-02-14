
#include "pc88-c.h"

void main()
{   
    IRQ_OFF
   /* SEMIGFC MODE */

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
    
    IRQ_ON

    while(1)
    {
        Wait_VBLANK();
        if(GetKeyDown(KB_RETURN)) print("HI!\x00");
    }
}


    
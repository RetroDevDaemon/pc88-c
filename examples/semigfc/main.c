
#include "pc88-c.h"

void main()
{   
    IRQ_OFF
    /* SEMIGFC MODE */
    // Simply print bytes to VRAM. Doesn't require screen setup,
    //  only clever use of screen attributes. 
    //
    // -Only one TextAttribute per each X and Y 
    // -First attribute on each row is always counted as X=0
    // -Remember to enable/disable as needed
    
    // Rose:
    print("\xe0\xce\xc0\x0e\x00");
    SetCursorPos(0, 1);
    print("\x70\xef\xf9\x07\x00");
    SetCursorPos(0, 2);
    print("\x47\xf8\x8f\xf4\x00");
    SetCursorPos(0, 3);
    print("\x10\x72\x3f\x01\x00");
    SetTextAttribute(0, 0, COLORMODE_SET(CLR_FUSCHIA, true));  // color + semi 
    SetTextAttribute(1, 0, COLORMODE_ATTR(0, 0, true, 0, 0));// reverse
    SetTextAttribute(4, 0, COLORMODE_SET(CLR_BLACK, true));
    SetTextAttribute(0, 1, 0x58);
    SetTextAttribute(4, 1, COLORMODE_SET(CLR_BLACK, true));
    SetTextAttribute(0, 2, 0x98);
    SetTextAttribute(4, 2, COLORMODE_SET(CLR_BLACK, true));
    SetTextAttribute(0, 3, 0xd8);
    SetTextAttribute(4, 3, COLORMODE_SET(CLR_BLACK, true));
    SetTextAttribute(5, 3, 0);// reverse off 
    
    IRQ_ON

    while(1)
    {
        Wait_VBLANK();
        if(GetKeyDown(KB_RETURN)) print("HI!\x00");
    }
}


    
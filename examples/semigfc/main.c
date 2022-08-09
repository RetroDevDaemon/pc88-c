
#include <pc88-c.h>
#include <textmode.h>

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
    
    // Byte layout:
    // 0044  where:
    // 1155   76543210 are bits in the byte
    // 2266    and R is if reverse is enabled
    // 3377
    // RRRR
     // One byte makes 20 pixels on screen.

    // E.g. python3 tools/png2zemi.py semi-rose.png
    /* outputs:
    [['0x1f', '0x31', '0x3f', '0xf1'], '0x78', 255, 
    ['0x70', '0xef', '0xf9', '0x7'], '0x58', 0, 
    ['0xb8', '0x7', '0xf0', '0xb'], '0x98', 255, 
    ['0xde', '0x8d', '0xd0', '0xfe'], '0xd8', 255]
        Character data, COLORMODE_SET, and if you need reverse on (255 = true)
    */
    // Rose:
    print("\x80\x1f\x31\x3f\xf1\x00");
    SetCursorPos(0, 1);
    print("\xff\x70\xef\xf9\x07\x00");
    SetCursorPos(1, 2);
    print("\xb8\x07\xf0\x0b\x00");
    SetCursorPos(0, 3);
    print("\xff\xde\x8d\xd0\xfe\x00");
    SetTextAttribute(0, 0, COLORMODE_SET(CLR_FUSCHIA, true));  // color + semi 
    SetTextAttribute(1, 0, COLORMODE_ATTR(0, 0, true, 0, 0));// reverse
    SetTextAttribute(5, 0, COLORMODE_SET(CLR_BLACK, true));
    
    SetTextAttribute(0, 1, 0x58);
    SetTextAttribute(1, 1, 0);// reverse off
    SetTextAttribute(5, 1, COLORMODE_SET(CLR_BLACK, true));
    
    SetTextAttribute(0, 2, 0x98);
    SetTextAttribute(1, 2, COLORMODE_ATTR(0, 0, true, 0, 0));// reverse on
    SetTextAttribute(5, 2, COLORMODE_SET(CLR_BLACK, true));
    
    SetTextAttribute(0, 3, 0xd8);
    SetTextAttribute(5, 3, COLORMODE_SET(CLR_BLACK, true));
    SetTextAttribute(6, 3, 0);
    
    IRQ_ON

    while(1)
    {
        Wait_VBLANK();
        if(GetKeyDown(KB_RETURN)) print("HI!\x00");
    }
}


    
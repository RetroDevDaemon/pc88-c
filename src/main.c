//////////////////
/// Pc-88 HELLO WORLD
/// (c)2021 Ben Ferguson
/////////////////

#include "pc88-c.h"

#include "img_b.h"
#include "img_r.h"
#include "img_g.h"


// Consts
String strdat = "H e l l o   W o r l d \x00";

PlanarBitmap santaBitmap = { 
    img_r, img_g, img_b, 119, 48
};


void main()
{   
    //SetIOReg(0x53, 1);  // Hide text 
    // turn on 80 col + color mode 
    vu8* tgt = (vu8*)(SCREEN_TXT_BASE + 80);
    // 25 line, color, gfc on, n88, 64kb, 200line
    //SetIOReg(0x31, 0b00111011);
    //SetIOReg(0x32, 0b10011001); // default state
    //SetIOReg(0x35, (u8)bit(7)); // GVRAM access on 

    IRQ_OFF()

    PlanarBitmap* pb = &santaBitmap;
    DrawPlanarBitmap(pb, 20, 10);

    SETBANK_MAINRAM()
    
    IRQ_ON()

    while(1)
    {
        Wait_VBLANK();
        
        if((SCREEN_POINTER < tgt-3)&&(SCREEN_POINTER < (vu8*)0xff80))
            print("HI!\x00");
        else {
            tgt += 120;
            SCREEN_POINTER += 40;
        }
        
    }
}

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

PlanarBitmap layeredImage = { 
    img_r, img_g, img_b, 248/8, 100
};

/// Keep me here!
void main()
{   
    //SetIOReg(0x53, 1);  // Hide text 
    // turn on 80 col + color mode 
    //vu8* tgt = (vu8*)(SCREEN_TXT_BASE + 80);
    // 25 line, color, gfc on, n88, 64kb, 200line
    //SetIOReg(0x31, 0b00111011);
    //SetIOReg(0x32, 0b10011001); // default state
    //SetIOReg(0x35, (u8)bit(7)); // GVRAM access on 

    IRQ_OFF
    // Test yellow ALU draw (V2)
    ExpandedGVRAM_On();                 // Expanded mode GVRAM on, comp data off on VRAM ld (???)
    EnableALU();                        // ALU on - must be performed AFTER r32 GVRAM enable! (0x89 is default)
    SetIOReg(EXPANDED_ALU_CTRL, CLR_YELLOW);     // OR bits or bit reset to set color
    vu8* vp = (vu8*)0xc100;
    *vp = 0xff;
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | CLR_WHITE | bit(4)) );
    SetIOReg(EXPANDED_ALU_CTRL, CLR_BLUE);
    __asm
      ld a,(0xC100)
      ld (0xC101),a
    __endasm;
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | CLR_WHITE ) );
    *vp = 0xff;
    ExpandedGVRAM_Off();
    // Planar bitmap (V1) draw and individual pixels
    DisableALU(); // ALU off
    PlanarBitmap* pb = &layeredImage;    
    DrawPlanarBitmap(pb, 20, 10);
    SetPixel(360, 180, CLR_BLUE);
    SetPixel(361, 181, CLR_CYAN);
    SetPixel(362, 182, CLR_GREEN);
    SETBANK_MAINRAM() // must reset after draw!
    
    IRQ_ON

    while(1)
    {
        Wait_VBLANK();
        if(GetKeyDown(KB_RETURN)) print("HI!\x00");
        if(GetKeyDown(KB_Q)) print("BYTE!\x00");
    }
}



/* Pal test for layered planar gfx 
    SetIOReg(PAL_REG0, CLR_RED);
    SetIOReg(PAL_REG1, CLR_BLACK);
    SetIOReg(PAL_REG2, CLR_FUSCHIA);
    SetIOReg(PAL_REG3, CLR_GREEN);
    SetIOReg(PAL_REG4, CLR_CYAN);
    SetIOReg(PAL_REG5, CLR_CYAN);
    SetIOReg(PAL_REG6, CLR_CYAN);
    SetIOReg(PAL_REG7, CLR_CYAN);
    */
    
  

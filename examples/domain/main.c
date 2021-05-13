// main.c
#include <pc88-c.h>

// DOMAIN includes
#include "tile_01b.h"

#define CTLB_STATUS_DEFAULT (HCOLOR | GRAPH | MMODE | LINE200)

//
void Vblank() __critical __interrupt;
inline void SetVBLIRQ();

//
inline void SetVBLIRQ()
{
    SetIOReg(IRQ_LEVEL_SET, 2);      // Set IRQ high for VBL
    SetIOReg(IRQ_MASK, 0b10);        // Reset mask for VBL
}

void Vblank() __critical __interrupt
{
    IRQ_OFF         // Disable interrupts during VBL
    SetVBLIRQ();    // Reset interrupt level and mask
    // Do our Vblank code:


    IRQ_ON
}

inline void CheckV2()
{
    u16 h = GetSysMode();
    if(!(h & V2_MODE_FLAG))
    {
        print("V 2   M o d e   p l e a s e !");
        while(1){}
    }
}

//
void main()
{
    IRQ_OFF 

    CheckV2();          // Don't proceed if not
    // VBL set
    __asm 
        ld hl, #_Vblank
        ld (CRTC_IRQ),hl
    __endasm;
    SetVBLIRQ();

#define GAMEVIEW_W 16
#define GAMEVIEW_H 9
    // Game Init
    // Disable graphic display        
    // trial: draw a tile : 01b is 32x16px : 20 x 12.5 tiles max
    // draw ALL tiles - draw all on one plane first 
    u16* scr = (u16*)0xc000;
    u16* til = (u16*)&tile_01b_b[0];
    SetIOReg(SYS_CTL_REGB, CTLB_STATUS_DEFAULT ^ GRAPH);

    SETBANK_BLUE()
    u8 yrow;
    u8 i;
    u16 ofs;
    for(yrow = 1; yrow < GAMEVIEW_H; yrow++)
    {
        u8 xrow;
        for(xrow = 1; xrow < GAMEVIEW_W; xrow++)
        {
            scr = (u16*)(0xc000 + (xrow * 4) + (yrow * 1280)); //80*16
            til = (u16*)&tile_01b_b[0];
            for(i = 0; i < 16; i++)
            {
                *scr++ = *til++;
                *scr++ = *til++;
                scr += 38;
            }
        }
    }
    
    SETBANK_RED()
    for(yrow = 1; yrow < GAMEVIEW_H; yrow++)
    {
        u8 xrow;
        for(xrow = 1; xrow < GAMEVIEW_W; xrow++)
        {
            scr = (u16*)(0xc000 + (xrow * 4) + (yrow * 1280));
            til = (u16*)&tile_01b_r[0];
            for(i = 0; i < 16; i++)
            {
                *scr++ = *til++;
                *scr++ = *til++;
                scr += 38;
            }
        }
    }
    
    SETBANK_GREEN()
    for(yrow = 1; yrow < GAMEVIEW_H; yrow++)
    {
        u8 xrow;
        for(xrow = 1; xrow < GAMEVIEW_W; xrow++)
        {
            scr = (u16*)(0xc000 + (xrow * 4) + (yrow * 1280));
            til = (u16*)&tile_01b_g[0];
            for(i = 0; i < 16; i++)
            {
                *scr++ = *til++;
                *scr++ = *til++;
                scr += 38;
            }
        }
    }
    SetIOReg(SYS_CTL_REGB, CTLB_STATUS_DEFAULT);
    
    SETBANK_MAINRAM()

    IRQ_ON 

    while(1){ }
}
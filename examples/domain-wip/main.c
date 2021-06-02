// main.c
#include <pc88-c.h>

// DOMAIN includes
#include "tile_01b.h"

#define CTLB_STATUS_DEFAULT (HCOLOR | GRAPH | MMODE | LINE200)

//
void Vblank() __critical __interrupt;
inline void SetVBLIRQ();
void LoadFullscreenMap(u8* curMap);
inline void GameInit();
inline void CheckV2();

//
static u8 map_a[144];
static u16* tiles[1];// = { &tile_01b_b[0] };
// Tiles are 64*3 bytes (4x16x3)
static u8* CURRENT_MAP;

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


inline void GameInit()
{
    tiles[0] = &tile_01b_b[0];
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
    // Game Init - sets up pointers, memcpys etc
    GameInit();
    
    // clear map (testing)
    u8 i;
    for(i = 0; i < 144; i++) map_a[i] = 0;
    
    // Disable graphic display        
    SetIOReg(SYS_CTL_REGB, CTLB_STATUS_DEFAULT ^ GRAPH);
    
    // trial: draw a tile : 01b is 32x16px : 20 x 12.5 tiles max
    // draw ALL tiles - draw all on one plane first 
    LoadFullscreenMap(&map_a[0]);    
    
    // Enable mainram and graphics
    SETBANK_MAINRAM()
    SetIOReg(SYS_CTL_REGB, CTLB_STATUS_DEFAULT);


    IRQ_ON 

    while(1){ }
}


void LoadFullscreenMap(u8* curMap)
{
    u16* scr;// = (u16*)0xc000;
    u16* til;// = tiles[0];
    SETBANK_BLUE()
    u8 yrow;
    u8 i;
    //u16 ofs;
    for(yrow = 1; yrow < GAMEVIEW_H; yrow++)
    {
        u8 xrow;
        for(xrow = 1; xrow < GAMEVIEW_W; xrow++)
        {
            scr = (u16*)(0xc000 + (xrow * 4) + (yrow * 1280)); //80*16
            til = tiles[curMap[(yrow*GAMEVIEW_W)+xrow]];
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
            til = tiles[curMap[(yrow*GAMEVIEW_W)+xrow]] + 32;
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
            til = tiles[curMap[(yrow*GAMEVIEW_W)+xrow]] + 64;
            for(i = 0; i < 16; i++)
            {
                *scr++ = *til++;
                *scr++ = *til++;
                scr += 38;
            }
        }
    }
}
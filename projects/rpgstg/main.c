#include <pc88-c.h>
#include <opn.h>
#include <draw.h>
#include <textmode.h>
//#include <beeper.h>

// project includes
#include "mecha.h"

// function defs 
void DrawBattleGrid();
void WriteHLineFast(u8 col, u8 y, u8 len);
void WriteVLine(u16 x, u8 y, u8 len);
inline void SetVBLIRQ();
void Vblank() __critical __interrupt;

// globals 
vu8* pc0_loc;
vu8* pc0_oldloc;
bool vbl_done;
u8 playerDir;
bool playerMoved;

// game defines
#define DIR_RIGHT 4
#define DIR_DOWN 2
#define DIR_LEFT 6

void main()
{
    IRQ_OFF 
	__asm 
		ld hl, #_Vblank
		ld (CRTC_IRQ),hl
	__endasm;
	SetVBLIRQ();

    // battle grid is 48x24 px
    DrawBattleGrid();
    
    // V2 line writing
    ExpandedGVRAM_On();
    EnableALU(FASTMEM_ON);
    
    // dark blue line
    SetIOReg(EXPANDED_ALU_CTRL, CLR_BLUE);
    WriteHLineFast(6, 164, 28);
    WriteVLine(6*8, 164, 32);
    // cyan line
    SetIOReg(EXPANDED_ALU_CTRL, CLR_CYAN);
    WriteHLineFast(6, 165, 28);
    WriteVLine(6*8+1, 165, 31);
    
    // Load sprite into vram
    // format on disk is 4bpp. 
    pc0_loc = DrawTransparentImage_V2(0, 0, &mecha[0], 32/8, 16);
    pc0_oldloc = pc0_loc;
    // copy from vram to buffer the bytes at this address
    ExpandedGVRAM_Copy_On();
    ALUCopyOut(pc0_loc, 0xfe80, 4, 16);
    
    DisableALU(FASTMEM_OFF);
    ExpandedGVRAM_Off();

    // enable vblank
    IRQ_ON;
    while(1)
    {
         // stop main loop from running more than once per vbl
        while(!vbl_done){};
        vbl_done = false;

        playerMoved = false;
        pc0_oldloc = pc0_loc;
            
        if(GetKeyDown(KB_PAD4))
        {
            playerDir = DIR_RIGHT;
            pc0_loc += 1;
            playerMoved = true;
        }
        else if(GetKeyDown(KB_PAD2))
        {
            playerDir = DIR_DOWN;
            pc0_loc += 80;
            playerMoved = true;
        }
        else if(GetKeyDown(KB_PAD6))
        {
            playerDir = DIR_LEFT;
            pc0_loc -= 1;
            playerMoved = true;
        }
    }  
    
}

inline void SetVBLIRQ()
{
    SetIOReg(IRQ_LEVEL_SET, 2);      // Set IRQ high for VBL
    SetIOReg(IRQ_MASK, 0b10);        // Reset mask for VBL
}

void Vblank() __critical __interrupt
{
    IRQ_OFF 
    
    // v2 drawin'!
    EnableALU(FASTMEM_OFF);
    ExpandedGVRAM_On();

    // erase the shadow of the player sprite
    if(playerMoved)
    {
        SetIOReg(EXPANDED_ALU_CTRL, 0); // (draw black)
        switch(playerDir)
        {
            case(DIR_RIGHT): // left column
                EraseVRAMAreaAddr(pc0_oldloc, 1, 16);
                break;
            case(DIR_DOWN): // top row
                EraseVRAMAreaAddr(pc0_oldloc, 4, 8);
                break;
            case(DIR_LEFT): // right col 
                EraseVRAMAreaAddr(pc0_oldloc+3, 1, 16);
                break;
        }
    }
    // player
    ExpandedGVRAM_Copy_On();
    ALUCopyIn(0xfe80, (vu8*)(pc0_loc), 4, 16); 

    DisableALU(FASTMEM_OFF);
    ExpandedGVRAM_Off();

    vbl_done = true;
    SetVBLIRQ(); 
    IRQ_ON
}

// block of 8 for speed
void WriteHLineFast(u8 col, u8 y, u8 len)
{
    vu8* s = 0xc000 + (y * 80) + col;

    while ( len > 0 )
    {
        *s++ = 0xff;
        len--;
    }
}


void WriteVLine(u16 x, u8 y, u8 len)
{
    vu8* s = 0xc000 + (y * 80) + (x >> 3);
    
    u8 b = x % 8;
    b = (0x80 >> b);
    
    while(len > 0)
    {
        *s = b;
        s += 80;
        len--;
    }
}

void DrawBattleGrid()
{
    ExpandedGVRAM_On();
    EnableALU(FASTMEM_ON);
    SetIOReg(EXPANDED_ALU_CTRL, CLR_BLUE);
    __asm 
        ld d,#6
        ld hl,#(0xc000 + (24*80) + 6)
        ld b,#60
        ld a,#0xff
        .topline:
        inc hl
        ld (hl),a
        djnz .topline
        ld bc,#(20 + (24*80))
        add hl,bc 
        dec d 
        xor a 
        or d
        ld a,#0xff 
        ld b,#60
        jr nz,.topline
    __endasm;
    __asm 
        ld a,#0x80
        ld c,#11
        ld b,#((24*5)+5)
        ld hl,#(0xc000 + (24*80) + 7)
        push bc 
        push hl

        .col:
        ld (hl),a 
        ld de,#80
        add hl,de 
        djnz .col
        
        pop hl
        ld de,#6
        add hl,de 

        pop bc 
        dec c 
        push bc
        push hl  
        jr nz,.col 
        pop hl 
        pop bc 
    __endasm;
    DisableALU(FASTMEM_OFF);
    ExpandedGVRAM_Off();   
    
}
#include <pc88-c.h>
#include <opn.h>
#include <draw.h>
#include <textmode.h>
//#include <beeper.h>

// function defs 
void DrawBattleGrid();
void WriteHLineFast(u8 col, u8 y, u8 len);
void WriteVLine(u16 x, u8 y, u8 len);

void main()
{
    // battle grid is 48x24 px
    DrawBattleGrid();
    
    ExpandedGVRAM_On();
    EnableALU(FASTMEM_ON);
    
    SetIOReg(EXPANDED_ALU_CTRL, CLR_BLUE);
    
    WriteHLineFast(6, 164, 28);
    WriteVLine(6*8, 164, 32);
    
    SetIOReg(EXPANDED_ALU_CTRL, CLR_CYAN);
    WriteHLineFast(6, 165, 28);
    WriteVLine(6*8+1, 165, 31);
    
    DisableALU(FASTMEM_OFF);
    ExpandedGVRAM_Off();

    while(1){};
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
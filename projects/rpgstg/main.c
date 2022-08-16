#include <pc88-c.h>
#include <opn.h>
#include <draw.h>
#include <textmode.h>
//#include <beeper.h>

// function defs 
void DrawBattleGrid();


void main()
{
    // battle grid is 48x24 px
    DrawBattleGrid();
    
    while(1){};
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
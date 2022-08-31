#include <pc88-c.h>
#include <draw.h>

#include "tileset.h"

void main()
{
    IRQ_OFF;

    EnableALU(1);
    ExpandedGVRAM_On();

    // load tiles into vram
    for(u8 i = 0; i < 24; i++)
        DrawImage_V2(0 + (i * 2), 0, (&tileset[0] + (64 * i)), 2, 8);
    
    // out
    ExpandedGVRAM_Copy_On();
    
    // copy out tiles 
    __asm 
        ld hl,#0xc002
        ld de,#0xfe80
        ld bc,#8

        ._tilealus:
        ld a,(hl)
        ld (de),a 
        inc hl 
        inc de
        ld a,(hl)
        ld (de),a 
        inc hl 
        inc de

        push bc 
        ld bc,#78
        add hl,bc
        pop bc 
        djnz ._tilealus

    __endasm;

    __asm 
        ld hl,#0xfe80
        ld de,#0xd000
        ld bc,#8

        ._tilealus2:
        ld a,(hl)
        ld (de),a 
        inc hl 
        inc de
        ld a,(hl)
        ld (de),a 
        inc hl 
        inc de

        push bc
        push hl 
        push de 
        pop hl 
        ld bc,#78
        add hl,bc
        push hl 
        pop de
        pop hl  
        pop bc 
        djnz ._tilealus2

    __endasm;

    DisableALU(FASTMEM_OFF);
    ExpandedGVRAM_Off();

    IRQ_ON;
    while(1){};

}
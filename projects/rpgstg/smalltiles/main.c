#include <pc88-c.h>
#include <draw.h>

#include "tileset.h"

void CopyTileToBuffer(u8* src, u8* dst);
void DrawTileFromBuffer(u8* src, u8* dst);


void main()
{
    IRQ_OFF;

    EnableALU(1);
    ExpandedGVRAM_On();

    // load tiles into vram
    for(u8 i = 0; i < 24; i++)
        DrawImage_V2(0 + (i * 2), 0, (&tileset[0] + (64 * i)), 2, 8);
    // tile is stored in 4bpp format on disk, which is 64 bytes per tile
    //   when loaded in vram, it is only 16 bytes with alu
    
    // out
    ExpandedGVRAM_Copy_On();
    
    // copy out tiles 
    CopyTileToBuffer(0xc002, 0xfe80);
    
    DrawTileFromBuffer(0xfe80, 0xc200);
    
    DisableALU(FASTMEM_OFF);
    ExpandedGVRAM_Off();

    IRQ_ON;
    while(1){};

}

void CopyTileToBuffer(u8* src, u8* dst) 
{
    // HL and DE are already loaded
    __asm 
        ld bc,#8
        _tcpoutlp:
        push bc 
        ldi
        ldi 
        ld bc,#78
        add hl,bc
        pop bc 
        djnz _tcpoutlp
    __endasm;
}

void DrawTileFromBuffer(u8* src, u8* dst) 
{
    // HL and DE are already loaded
    __asm 
        ld bc,#8
        _tcpinlp:
        push bc
        push hl         ; faster way?
        push de 
        pop hl 
        ld bc,#78
        add hl,bc 
        push hl 
        pop de 
        pop hl 
        ldi
        ldi 
        pop bc 
        djnz _tcpinlp
    __endasm;
}
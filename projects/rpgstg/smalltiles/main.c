#include <pc88-c.h>
#include <draw.h>

#include "tileset.h"

void CopyTileToBuffer(vu8* src, vu8* dst) __naked;
void DrawTileFromBuffer(vu8* src, vu8* dst);


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
    
    // Fill 40x20 area with grass tile 
    u8* t = 0xc000;
    for(u8 y = 0; y < 20; y++){
        for(u8 i = 0; i < 40; i++)
        {
            DrawTileFromBuffer(0xfe80, t + (i * 2));
        }
        t += 0x280;
    }
    
    DisableALU(FASTMEM_OFF);
    ExpandedGVRAM_Off();

    IRQ_ON;
    while(1){};

}

void CopyTileToBuffer(vu8* src, vu8* dst) __naked
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
        ret
    __endasm;
}

void DrawTileFromBuffer(vu8* src, vu8* dst)  
{
    
    // HL and DE are already loaded
    __asm 
        push bc 
        ld bc,#78
        push bc 

        ldi 
        ldi 
        ex de,hl 
        pop bc 
        push bc 
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        ex de,hl
        pop bc 
        push bc  
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        ex de,hl 
        pop bc 
        push bc 
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        ex de,hl 
        pop bc 
        push bc 
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        ex de,hl 
        pop bc 
        push bc 
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        ex de,hl 
        pop bc 
        push bc 
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        ex de,hl 
        pop bc 
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        
        pop bc 
    __endasm;
    /*
    for(u8 i = 0; i < 8; i ++)
    {
        *dst = *src;
        dst += 1;
        src += 1;
        *dst = *src;
        dst += 79;
        src += 1;
    }
    */
}
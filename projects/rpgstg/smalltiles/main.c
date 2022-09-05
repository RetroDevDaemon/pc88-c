#include <pc88-c.h>
#include <draw.h>

#include "tileset.h"

typedef unsigned char Map;
typedef enum directions { Up = 8, Left = 4, Right = 6, Down = 2 } Direction;

void CopyTileToBuffer(vu8* src, vu8* dst);
void DrawTileFromBuffer(vu8* src, vu8* dst);
void ScrollMapBuffer(Direction d);


void main()
{
    IRQ_OFF;

    Map* map_ = 0x6000;
    u8* mapBuffer_A = 0x8000;
    u8* mapBuffer_B = mapBuffer_A + (40*20);
    char* mapBuffer_end = mapBuffer_B + (40*20);

    DiskLoad((u16)0x8000, 4, 1, (u16)(0x2000/256), 0); 

    EnableALU(0);
    ExpandedGVRAM_On();

    // load tiles into vram
    for(u8 i = 0; i < 24; i++)
        DrawImage_V2(0 + (i * 2), 0, (&tileset[0] + (64 * i)), 2, 8);
    // tile is stored in 4bpp format on disk, which is 64 bytes per tile
    //   when loaded in vram, it is only 16 bytes with alu
    
    // out
    ExpandedGVRAM_Copy_On();
    
    // copy out tiles 
    for(u8 i = 0; i < 24; i ++)
        CopyTileToBuffer(0xc000 + (i*2), 0xfe80 + (i * 0x10));
    
    // Load map
    for(u8 y = 0; y < 20; y++){
        for(u8 i = 0; i < 40; i++)
        {
            mapBuffer_A[(y * 40) + i] = map_[(y * 128) + i];
        }
    }
    
    // Draw map window
    u8* t = 0xc000;
    mapBuffer_A = 0x8000;
    for(u8 y = 0; y < 20; y++){
        for(u8 i = 0; i < 40; i++)
        {
            u8 ti = mapBuffer_A[(y * 40) + i];
            DrawTileFromBuffer(0xfe80 + (0x10 * ti), t + (i * 2));
        }
        t += 0x280;
    }
    
    DisableALU(FASTMEM_OFF);
    ExpandedGVRAM_Off();

    IRQ_ON;
    while(1){};

}

void ScrollMapBuffer(Direction d)
{
    switch(d)
    {
        case Up:
            break;
        case Down:
            break;
        case Left:
            break;
        case Right:
            break;
    }
}

void CopyTileToBuffer(vu8* src, vu8* dst) 
{
    // Generated assembly is... fine
    vu8* op = dst;
    vu8* inp = src;
    *op++ = *inp++;
    *op++ = *inp++;
    inp += 78;
    *op++ = *inp++;
    *op++ = *inp++;
    inp += 78;
    *op++ = *inp++;
    *op++ = *inp++;
    inp += 78;
    *op++ = *inp++;
    *op++ = *inp++;
    inp += 78;
    *op++ = *inp++;
    *op++ = *inp++;
    inp += 78;
    *op++ = *inp++;
    *op++ = *inp++;
    inp += 78;
    *op++ = *inp++;
    *op++ = *inp++;
    inp += 78;
    *op++ = *inp++;
    *op++ = *inp++;
    
}

void DrawTileFromBuffer(vu8* src, vu8* dst)  
{
    // HL and DE are already loaded
    __asm 

        ldi 
        ldi 
        ld bc,#78
        ld a,c
        ex de,hl 
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        ex de,hl
        ld c,a
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        ex de,hl 
        ld c,a
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        ex de,hl 
        ld c,a
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        ex de,hl 
        ld c,a
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        ex de,hl 
        ld c,a
        add hl,bc
        ex de,hl 
        ldi 
        ldi 
        ex de,hl 
        ld c,a
        add hl,bc
        ex de,hl 
        ldi 
        ldi 

    __endasm;
}
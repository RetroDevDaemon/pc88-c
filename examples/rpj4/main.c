#include <pc88-c.h>

#include "song_b.h"
#include "sprites.h"

#include "treetile.h"
#include "grasstile.h"

const u8 map1[] = { \
    2,2,2,1,1,1,2,1,
    2,2,1,2,1,2,2,2,
    1,2,1,1,1,1,1,1,
    2,2,1,2,1,2,1,2,
    1,2,1,1,2,1,1,1,
    1,2,1,1,1,1,1,1,
    1,2,2,1,2,2,1,1,
    2,1,1,2,2,2,2,2
};



u8 revealedMap[] = { \
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

// only 4 for now i guess


inline void SetIRQs()
{
    SetIOReg(IRQ_LEVEL_SET, 2);
    SetIOReg(IRQ_MASK, 2);
}

extern bool playingSong;
extern struct Song currentSong;
extern signed int ticker;
extern u16 RANDOMSEED;

u32 idleCount;
u8 ctr;

struct tileinfo { 
    struct xypos pos;
    u8 tileNo;
};

struct tileinfo loadedTiles[8];
struct xypos zeropos;
u8* tilePtrs[8];
void DrawHexTile(u8 x, u8 y, u8 index);
//struct xypos zeropos;


// F DEFS
void DrawCleanGrid();
s16 roll(u8 numDie, u8 sides, s8 mod);
void GameInit();

void main()
{
    IRQ_OFF; 
    // clear vars
    RANDOMSEED += rand();
    ctr = 0;
    idleCount = 0;
    u8 i = 0;
    u8 c = 0;
    playingSong = false;
    // set up quick draw ram
    
    GameInit();
    
    // Set up currentSong pointers
    LoadSong(&song[0]);
    playingSong = true;
    ticker = 0;

    // Use this entire sequence to change CRT Rows
    //volatile static __sfr __at 0x40 CRTPort;
    //i = CRTPort;
    //if(CRTPort & 0b10)
    //    SetMonitor(15, 25);
    //else 
    //    SetMonitor(24, 25);

    SetCursorPos(15,19);
    print("Loading...");

    // ALU drawing here
    ExpandedGVRAM_On();     
    EnableALU(FASTMEM_ON);
    
    // Grid test:
    // init blank grid 
    DrawCleanGrid();

#define GRASS 1
#define TREE 2
#define TEMPGVR_SPRITE_0 0xfe80 
#define TEMPGVR_SPRITE_1 0xfee0

    DrawHexTile(4,4,GRASS);
    DrawHexTile(2,0,TREE);
    DrawHexTile(1,4,GRASS);
    // Hero:
    // MASKING TEST 
    // Remember, the sprites are 384 bytes when STORED, but they only take up
    //  96 bytes per plane. 
    // 0xfe80, 0xfee0, ff40, ffa0 room for 4
    // backup
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | bit(4) ));
    ALUCopyOut(0xc000+(64*80)+12, TEMPGVR_SPRITE_0, 4, 24);
    ALUCopyOut(0xc000+(64*80)+12, TEMPGVR_SPRITE_1, 4, 24);
    // draw sprite 
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80));
    DrawTransparentImage_V2(12, 64, &librarianSprite[0], 32/8, 24);
    // return backup
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | bit(4) ));
    ALUCopyIn(TEMPGVR_SPRITE_1, 0xc000+(64*80)+12, 4, 24);

    // ALU off
    DisableALU(FASTMEM_OFF);
    ExpandedGVRAM_Off();   

    // disalbe load text
    SetCursorPos(15,19);
    print("             ");

    // First, Write the address of the Vblank routine to the CRTC IRQ vector @ f302
    __asm 
        ld hl, #_Vblank
        ld (CRTC_IRQ),hl
    __endasm;
    SetIRQs();        // And enable the VBL interrupt!
    

    IRQ_ON 
    while(1)
    { 

        // Main Loop
        IRQ_OFF 
        idleCount++;
        RANDOMSEED += rand();
        s16 randnum = roll(1, 6, 0);
        char* t = byToHex(randnum>>8);
        SetCursorPos(60, 8);
        print(t);
        t = byToHex(randnum & 0xff);
        print(t);
        IRQ_ON 
    }
}


void Vblank() __critical __interrupt
{
    IRQ_OFF; 
    
    // MUSIC?
    if(playingSong)
        PlaySong();
    else {
        SetCursorPos(4, 0);
        print("OVER");
    }

    // PRINT IDLE COUNTS
    SetCursorPos(0, 0);
    u8* d = byToHex((u8)(idleCount >> 8)); 
    print(d);
    u8* d2 = byToHex((u8)(idleCount & 0xff));
    print(d2);
    
    idleCount = 0;

    SETBANK_MAINRAM();      
    //

    SetIRQs(); 
    IRQ_ON;
}


s16 roll(u8 numDie, u8 sides, s8 mod)
{
    s16 r = 0;
    for(u8 i = 0; i < numDie; i++)
        r += (u8)(1 + (rand() & (sides-1))); // 0-255
    r += mod;
    return r;
}

void DrawHexTile(u8 x, u8 y, u8 index)
{
    //TODO check already drawn
    u8 z = y % 2;
    switch(z)
    {
        case(0):
            DrawImage_V2(2+(x*8), (y*16)+8, tilePtrs[index], 64/8, 24);
            break;
        case(1):
            DrawImage_V2(6+(x*8), (y*16)+24, tilePtrs[index], 64/8, 24);
            break;
        default:
            break;
    // TODO redraw sprites ?
    }
}

void DrawCleanGrid()
{
    DrawImage_V2(2, 8, &hexa[0], 64/8, 24);
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | bit(4) ));
    for(u8 x = 0; x < 8; x++)
    {
        ALUCopy(VRAMAddrByTile(2,8), VRAMAddrByTile(2+(8*x),8), 8, 24);
        ALUCopy(VRAMAddrByTile(2,8), VRAMAddrByTile(2+(8*x),40), 8, 24);
        ALUCopy(VRAMAddrByTile(2,8), VRAMAddrByTile(2+(8*x),72), 8, 24);
        ALUCopy(VRAMAddrByTile(2,8), VRAMAddrByTile(2+(8*x),104), 8, 24);
    }
    ALUCopy(VRAMAddrByTile(2,8), VRAMAddrByTile(6,24), 8, 24);
    for(u8 x = 0; x < 8; x++)
    {
        ALUCopy(VRAMAddrByTile(6,24), VRAMAddrByTile(6+(8*x),24), 8, 24);
        ALUCopy(VRAMAddrByTile(6,24), VRAMAddrByTile(6+(8*x),56), 8, 24);
        ALUCopy(VRAMAddrByTile(6,24), VRAMAddrByTile(6+(8*x),88), 8, 24);
        ALUCopy(VRAMAddrByTile(6,24), VRAMAddrByTile(6+(8*x),120), 8, 24);
    }
    //*********//
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80));

}

void GameInit()
{
    zeropos.x = 0;
    zeropos.y = 0;
    for(u8 i = 0; i < 8; i++ )
    {
        loadedTiles[0].pos = zeropos; // original tile to copy
        loadedTiles[0].tileNo = 0;    // number to pull from tilePtrs[]
    }
    tilePtrs[0] = &hexa[0];
    tilePtrs[1] = &grasstile[0];
    tilePtrs[2] = &treetile[0];
}
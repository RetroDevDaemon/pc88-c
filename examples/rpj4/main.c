#include <pc88-c.h>

#include "song_b.h"
#include "sprites.h"

#include "graphics.h"


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
    2,2,0,0,0,0,0,0,
    1,2,1,0,0,0,0,0,
    1,2,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};


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
    XYPos pos;
    u8 tileNo;
};

struct tileinfo loadedTiles[8];
XYPos zeropos;
XYPos player_hex_pos;
u8* tilePtrs[8];
char* res;
s16 randnum;
bool frameDone;
//struct xypos zeropos;

// F DEFS
void DrawHexTile(u8 x, u8 y, u8 index);
void DrawCleanGrid();
s16 roll(u8 numDie, u8 sides, s8 mod);
void GameInit();
void DrawFullUI();


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

    // Use this entire sequence to change CRT Rows (bugged)
    //volatile static __sfr __at 0x40 CRTPort;
    //i = CRTPort;
    //if(CRTPort & 0b10)
    //    SetMonitor(15, 25);
    //else 
    //    SetMonitor(24, 25);

    SetCursorPos(30,19);
    print("Loading...");

    // ALU drawing here
    ExpandedGVRAM_On();     
    EnableALU(FASTMEM_ON);
    
    // Grid test:
    // init blank grid 
    DrawCleanGrid();

    DrawFullUI();

#define GRASS 1
#define TREE 2
#define TEMPGVR_SPRITE_0 0xfe80 
#define TEMPGVR_SPRITE_1 0xfee0

// hero hex area ::
    player_hex_pos.x = 1;
    player_hex_pos.y = 4;
    for(i = player_hex_pos.x-1; i <= player_hex_pos.x+1; i++)
        DrawHexTile(i, player_hex_pos.y, map1[(player_hex_pos.y*8)+i]);
    i -= 2;
    if(player_hex_pos.y % 2 == 0) //even - go up and -1
    {
        DrawHexTile(i-1, player_hex_pos.y-1, map1[(player_hex_pos.y*8)+i-9]);
        DrawHexTile(i, player_hex_pos.y-1, map1[(player_hex_pos.y*8)+i-8]);
        // down and -1
        DrawHexTile(i, player_hex_pos.y+1, map1[(player_hex_pos.y*8)+i-8]);
        DrawHexTile(i-1, player_hex_pos.y+1, map1[(player_hex_pos.y*8)+i-7]);
    } else { //odd - go up and +1
        DrawHexTile(i+1, player_hex_pos.y-1, map1[(player_hex_pos.y*8)+i-7]);
        DrawHexTile(i, player_hex_pos.y-1, map1[(player_hex_pos.y*8)+i-8]);
        // d and
        DrawHexTile(i, player_hex_pos.y+1, map1[(player_hex_pos.y*8)+i-8]);
    }
    // Hero:// MASKING TEST 
    // Remember, the sprites are 384 bytes of bitmap data, but they only take up
    //  96 bytes per plane. 
    // 0xfe80, 0xfee0, ff40, ffa0 room for 4
    XYPos player_pos;
    player_pos.x = 12;
    player_pos.y = 64;
    // copy out mask
    ExpandedGVRAM_Copy_On();
    ALUCopyOut(GVRAM_BASE+(player_pos.y*80)+player_pos.x, TEMPGVR_SPRITE_1, 4, 24);
    // draw sprite 
    ExpandedGVRAM_On();
    DrawTransparentImage_V2(player_pos.x, player_pos.y, &librarianSprite[0], 32/8, 24);
    // return backup
    //ExpandedGVRAM_Copy_On();
    //ALUCopyIn(TEMPGVR_SPRITE_1, GVRAM_BASE+(player_pos.y*80)+player_pos.x, 4, 24);


    // ALU off
    DisableALU(FASTMEM_OFF);
    ExpandedGVRAM_Off();   
    // disable load text
    SetCursorPos(75, 4);
    print("2/2");
    SetCursorPos(75, 6);
    print("2/2");
    SetCursorPos(75, 8);
    print("2/2");
    SetCursorPos(75, 12);
    print("10/10");
    SetCursorPos(30,19);
    print("           ");
    SetCursorPos(5,16);
    print("[7   9]\n");
    print("[4   6]  Move      [5]  Confirm\n");
    print("[1   3]         [0]     Cancel");
    
    // First, Write the address of the Vblank routine to the CRTC IRQ vector @ f302
    SetCRTC_IRQ((void*)&Vblank);
    SetIRQs();          // And enable the VBL interrupt!
    IRQ_ON              // run once!
    while(1)
    { 
        // Main Loop
        // wait for 1/60 irq to finish
        while(!frameDone){idleCount++;};
        IRQ_OFF 
        frameDone = false;

        // modify seed 
        RANDOMSEED += rand();
        randnum = roll(1, 6, 0);
        
        // return 
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
        //SetCursorPos(5, 0);
        //print("OVER!");
    }

    // PRINT IDLE COUNTS
    //SetCursorPos(0, 0);
    //u8* d = byToHex((u8)(idleCount >> 8)); 
    //print(d);
    //u8* d2 = byToHex((u8)(idleCount & 0xff));
    //print(d2);
    SetCursorPos(0, 19);
    res = byToHex(randnum & 0xff);
    print(res);
    
    
    idleCount = 0;

    //SETBANK_MAINRAM();
    //
    frameDone = true;
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
    
    u8 z = y & 0b1;
    y = y >> 1;
    switch(z)
    {
        case(0):
            DrawImage_V2(2+(x*8), (y*32)+8, tilePtrs[index], 64/8, 24);
            break;
        case(1):
            DrawImage_V2(6+(x*8), (y*32)+24, tilePtrs[index], 64/8, 24);
            break;
        default:
            break;
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
    player_hex_pos.x = 0;
    player_hex_pos.y = 0;
    for(u8 i = 0; i < 8; i++ )
    {
        loadedTiles[i].pos = zeropos; // original tile to copy
        loadedTiles[i].tileNo = 0;    // number to pull from tilePtrs[]
    }
    tilePtrs[0] = &hexa[0];
    tilePtrs[1] = &grasstile[0];
    tilePtrs[2] = &treetile[0];
}

void DrawFullUI()
{
    u8 i;   
    // PAPER ROLL UI THING
    DrawImage_V2(0, 0, &paperroll[0], 2, 8);
    
    DrawImage_V2(0, 8, &paperside[0], 2, 36);
    DrawImage_V2(0, 44, &paperside[0], 2, 36);
    DrawImage_V2(0, 80, &paperside[0], 2, 36);
    DrawImage_V2(0, 116, &paperside[0], 2, 36);
    
    DrawImage_V2(70, 8, &paperside[0], 2, 36);
    DrawImage_V2(70, 44, &paperside[0], 2, 36);
    DrawImage_V2(70, 80, &paperside[0], 2, 36);
    DrawImage_V2(70, 116, &paperside[0], 2, 36);
    
    for(i = 2; i < 13; i++)
        DrawImage_V2(i, 1, &rolltop[0], 1, 6);
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | bit(4) ));
    for(i = 10; i < 64; i+=8)
        ALUCopy(VRAMAddrByTile(2,1),VRAMAddrByTile(3+i,1), 8, 6);
    ExpandedGVRAM_On();    
    DrawImage_V2(71, 1, &paperend[0], 1, 6);    
    DrawImage_V2(1, 149, &paperbottom[0], 2, 2);
    DrawImage_V2(3, 149, &paperbottom[0], 2, 2);
    DrawImage_V2(5, 149, &paperbottom[0], 2, 2);
    DrawImage_V2(7, 149, &paperbottom[0], 2, 2);
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | bit(4) ));
    for(i = 8; i < 64; i+=8)
        ALUCopy(VRAMAddrByTile(1,149),VRAMAddrByTile(i,149), 8, 2);
    ExpandedGVRAM_On();
    //////
    
    // LIBBY UI
    DrawImage_V2(72, 8,     &libbytext[0],  7, 14);
    DrawImage_V2(73, 32,    &gunicon[0],    2, 7);
    DrawImage_V2(73, 51,    &booticon[0],   2, 8);
    DrawImage_V2(73, 69,    &bookicon[0],   2, 11);
    DrawImage_V2(72, 116,   &hearticon[0],  3, 7);
    //
}
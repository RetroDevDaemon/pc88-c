#include <pc88-c.h>

#include "song_b.h"
#include "sprites.h"

#include "graphics.h"


const unsigned char introSong[] = {
	0, // num songs
    185,0, // offset
    5,1, // binary size 0x105

    190, // tempo??
    
    47,0,0,0, //offset, loop
    87,0,0,0,
    //135,0,0,0,
    172,0,0,0,
    172,0,0,0,
    173,0,0,0,
    174,0,0,0,
    175,0,0,0,
    176,0,0,0,
    177,0,0,0,
    178,0,0,0,
    179,0,0,0,
    
    180,0, // data end loc, +5
    
    240,0,241,19,250,38,100,245,28,0,30,48, //64
	16,52,16,50,30,48,16,52,16,50,30,36,16,37,16,39,
	30,36,16,37,24,39,246,4,4,27,0,   0,
    
    240,1,241,18,250,38,100, // part 2
    245,36,0,245,12,0,4,71,4,69,4,68,4,
	66,246,4,4,11,0,245,12,0,4,69,4,68,4,66,4,
	64,246,3,4,11,0,246,7,8,35,0,  0,

    240,2,241,17,250,38,100, // part 3
    245,25,0,30,32,16,32,16,34,30,36,16,32,
	16,34,30,20,30,16,160,16,23,24,21,246,4,4,24,0,  0,
	0,0,0,0,0,0,0,0, //(133 bytes of song)
    
    3, //185
    
    65,49,69,65,
    29,48,15,0,
    89,92,89,78,
    138,141,139,132,
    0,0,0,0,
    21,39,88,6,
    58,
    
    111,15,15,102,
    49,17,44,0,
    31,31,31,159,
    154,145,154,140,
    5,1,9,1,
    11,167,11,248,
    35,
    
    101,2,40,97,
	39,27,17,0,
    223,31,31,31,
    146,143,132,143,
    4,0,10,1,
	16,0,0,0,
    0
};


const u8 map1[] = { \
    2,2,2,1,1,1,2,1,
    2,2,1,2,3,2,2,2,
    1,2,1,1,1,1,1,1,
    2,2,1,2,1,2,1,2,
    1,2,1,3,2,1,1,1,
    1,2,1,1,1,3,3,1,
    1,2,2,1,2,2,1,1,
    2,1,1,2,2,2,2,2,
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
u8 currentMap;

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
bool playerMoved;
XYPos player_pos;
XYPos targetHex;
//struct xypos zeropos;
s8 lastKey;
u8 inputMode;
u8 inputWait;

#define EXPLORING 1
#define WAITING 0
#define ENC_SELECT 2
#define INTRO 3
#define GUN 0
#define SPEED 1
#define BOOK 2

// F DEFS
void DrawHexTile(s8 x, s8 y, u8 index);
void DrawCleanGrid();
s16 roll(u8 numDie, u8 sides, s8 mod);
void GameInit();
void DrawFullUI();
void DrawAreaAroundPlayer();
void DrawInputUI();
void BufferInput();
inline void MovementKeys();
void ConfigIntro();
void CPUWAIT(u16 n);
void RunIntro();
void PrintExploreUI();

#include "intro.h"



#include "encounters.c"

//enc type offsets are 012 map1  123 map2 345 map3
// 3-18
// 3-5 :heal
// 6-8 :a
// 9-11 :b
// 12-14 :c
// 15-18: miss
// STUPID AND BAD AND UNOPTOMIZED.
void LoadMap1()
{
    // roll 3d6
    currentMap = 1;
    
}


#define GRASS 1
#define TREE 2
#define CITY 3
#define TEMPGVR_SPRITE_0 0xfe80 
#define TEMPGVR_SPRITE_1 0xfee0

void BeginEncounter(u8 encNo)
{
    map1_encounters[encNo] = &bs_drone;
    Encounter* enc = map1_encounters[encNo];
    TextRowCopy(0, 15);
    TextRowCopy(0, 16);
    TextRowCopy(0, 17);
    TextRowCopy(0, 18);
    TextRowCopy(0, 19);
    
    SetCursorPos(33, 15);
    print("--ENCOUNTER--");
    
    SetCursorPos(1, 16);
    print(enc->desc);
    print(map1_encounters[0]->desc);

    ExpandedGVRAM_On();     
    EnableALU(1);
    // draw deck
    DrawImage_V2(53, 162, &deck[0], 8, 38);
    DrawImage_V2(67, 162, &deck[0], 8, 38);
    // draw player/enemy
    if(enc->encounterSpr != NULL)
        DrawTransparentImage_V2(68, 150, enc->encounterSpr, 4, 24);
    DrawTransparentImage_V2(55, 150, &librarianSprite[0], 4, 24);
    // erase sprite
    ExpandedGVRAM_Copy_On();
    //ALUCopyIn(TEMPGVR_SPRITE_0, GVRAM_BASE+(150*80)+54, 4, 24); // tempgvr 0 = background of players tile
    ALUCopyIn(TEMPGVR_SPRITE_0, GVRAM_BASE+(player_pos.y*80)+player_pos.x, 4, 24);
    
    ExpandedGVRAM_Off();     
    DisableALU(0);
    //print("OK");
}

enum Inputs { 
    UpRight=9, Right=6, DownRight=3, UpLeft=7, Left=4, DownLeft=1, Confirm=5, Cancel=0
};
    
void SetPlayerPosByGrid(u8 x, u8 y)
{
    if(y % 2 == 0)  // even?
    {
        player_pos.x = (x*8) + 2;
        player_pos.y = ((y>>1)*32) + 8;
    } else {        // odd
        player_pos.x = (x*8) + 6;
        player_pos.y = ((y>>1)*32) + 24;
    }
    player_pos.x += 2;
    player_pos.y -= 8;
 }

inline void NoEnc()
 {
    SetCursorPos(35, 16);
    print("Nothing!!");
    CPUWAIT(400);
    SetCursorPos(35, 16);
    print("          ");
    inputWait = 30;
 }

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
    
    //
    // Stall if not V2 mode 
    //
    c = ReadIOReg(SYS_CTL_REGB);
    if(c & V1MODE_FLAG)
    {
        SetCursorPos(0, 0);
        print("V 2   M O D E   O N L Y ");
        while(1){};
    }
    
    //
    // Run Intro
    //
    SetCRTC_IRQ((void*)&Vblank);
    SetIRQs();
    IRQ_ON  

    inputMode = INTRO;
    ConfigIntro();
    RunIntro();

    IRQ_OFF;

    CLS();
    ClearAttributeRam();
    
    LoadMap1();

    //
    // Load main game
    //
    inputMode = WAITING;

    SetCursorPos(10,5);
    print("                                                     ");
    SetCursorPos(30,16);
    print("Loading...");

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

    // ALU drawing here
    ExpandedGVRAM_On();     
    EnableALU(FASTMEM_ON);
    
    // Grid test:
    // init blank grid 
    DrawCleanGrid();

    DrawFullUI();

// hero hex area ::
    player_hex_pos.x = 1;
    player_hex_pos.y = 4;
    DrawAreaAroundPlayer();

    // Hero:// MASKING TEST 
    // Remember, the sprites are 384 bytes of bitmap data, but they only take up
    //  96 bytes per plane. 
    // 0xfe80, 0xfee0, ff40, ffa0 room for 4
    player_pos.x = 12; // 1
    player_pos.y = 64; // 4
    // grid starts (0,0) at 2,8 and (0,1) at 6,24
    //   from there, +8x +32y per.
    SetPlayerPosByGrid(player_hex_pos.x, player_hex_pos.y);
    
    // copy out BACKGROUND
    ExpandedGVRAM_Copy_On();
    ALUCopyOut(GVRAM_BASE+(player_pos.y*80)+player_pos.x, TEMPGVR_SPRITE_0, 4, 24);
    // draw sprite 
    ExpandedGVRAM_On();
    DrawTransparentImage_V2(player_pos.x, player_pos.y, &librarianSprite[0], 32/8, 24);

    
    // ALU off
    DisableALU(FASTMEM_OFF);
    ExpandedGVRAM_Off();   

    PrintExploreUI();
    
    inputMode = EXPLORING;
    
    SetCursorPos(30,16);
    print("               ");

    inputWait = 60;    

    while(1)
    { 
        // wait for 1/60 irq to finish
        while(!frameDone)
        {
            idleCount++;
            BufferInput();    
        };
        IRQ_OFF 
        frameDone = false;
            
        if((lastKey != -1) && (inputWait == 0))
        {
            if(inputMode == EXPLORING)
            {
                if(lastKey == Confirm)
                {
                    // search this tile - 10% grass, 20% forest event
                    // city - RELIC or EXIT?
                    s16 s = roll(1, 20, 0);
                    u8 tt;
                    if(currentMap == 1)
                        tt = map1[(player_hex_pos.y*8)+player_hex_pos.x];
                    else if(currentMap == 2)
                        tt = map1[(player_hex_pos.y*8)+player_hex_pos.x];
                    else if(currentMap == 3)
                        tt = map1[(player_hex_pos.y*8)+player_hex_pos.x];
                    
                    if(tt == GRASS)
                    {
                        if(s <= 2) //1 or 2?
                        {
                            //FIXME
                            s16 s = roll(3, 6, -3);
                            if(currentMap == 1){
                                SetCursorPos(1,14);
                                print(byToHex(s & 0xff));
                                BeginEncounter((u8)s);
                            }
                            //else if(currentMap == 2)
                            //    BeginEncounter(map2_encounters[(u8)s]);
                            //else if(currentMap == 3)
                            //    BeginEncounter(map3_encounters[(u8)s]);
                        }    
                        else 
                            NoEnc();
                    }
                    else if (tt == TREE)
                    {
                        if(s <= 4) //20%
                        {
                            s16 s = roll(3, 6, -3);
                            if(currentMap == 1){
                                SetCursorPos(1,14);
                                print(byToHex(s & 0xff));
                                BeginEncounter((u8)s);                         
                            }
                            //else if(currentMap == 2)
                            //    BeginEncounter(map2_encounters[(u8)s]);
                            //else if(currentMap == 3)
                            //    BeginEncounter(map3_encounters[(u8)s]);
                        }
                        else { 
                            NoEnc();
                        }    
                    }
                    // every successful encounter gives you +1 HP
                    // map 2 increases stats to 3/3
                    // map 3 insreases to 4/4
                }
                else if (lastKey == Cancel)
                {
                    // open menu
                }
                else
                { 
                    MovementKeys();
                    // now we have the target hex location 
                    if(targetHex.x >= 0 && targetHex.x < 8)
                        if(targetHex.y >= 0 && targetHex.y < 8) // make sure its on the map
                        {
                            player_hex_pos.x = targetHex.x;
                            player_hex_pos.y = targetHex.y;
                            playerMoved = true;
                            inputWait = 20;
                        }
                    ;
                    // if not valid tile, dont take a turn.
                }
            }
        }
        
        // Main Loop
        // modify seed 
        if(inputWait > 0)
            inputWait--;
        lastKey = -1;
        RANDOMSEED += rand();
        randnum = (targetHex.x << 4) | targetHex.y;
        
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

    if(playerMoved)
    {
        EnableALU(1);
        ExpandedGVRAM_Copy_On();
        //restore bg buffer
        ALUCopyIn(TEMPGVR_SPRITE_0, GVRAM_BASE+(player_pos.y*80)+player_pos.x, 4, 24);
        // adjust pos 
        revealedMap[(player_hex_pos.y*8)+player_hex_pos.x] = 0;
        player_hex_pos.x = targetHex.x;
        player_hex_pos.y = targetHex.y;
        SetPlayerPosByGrid(player_hex_pos.x, player_hex_pos.y);
        // copy out new buffer 
        ALUCopyOut(GVRAM_BASE+(player_pos.y*80)+player_pos.x, TEMPGVR_SPRITE_0, 4, 24);
        // redraw player
        ExpandedGVRAM_On();
        DrawAreaAroundPlayer();
        DrawTransparentImage_V2(player_pos.x, player_pos.y, &librarianSprite[0], 4, 24);

        ExpandedGVRAM_Off();
        DisableALU(0);

        playerMoved = false;
    }
    
    // PRINT IDLE COUNTS
    //SetCursorPos(0, 0);
    //u8* d = byToHex((u8)(idleCount >> 8)); 
    //print(d);
    //u8* d2 = byToHex((u8)(idleCount & 0xff));
    //print(d2);
    //SetCursorPos(0, 19);
    //res = byToHex(randnum & 0xff);
    //print(res);
    
    
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
    r += (u16)(mod & 0x00ff);
    return r;
}

void CPUWAIT(u16 n)
{
    for(s32 i = n * 1000; i > 0; i--) { 
        __asm__("nop");
    }
}

#include "rpj4.c"
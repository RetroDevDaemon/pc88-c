// RETRO PLATFORM JAM #4 ENTRY
// v0.9 (Submission ver.)

// "The Librarian"
//  ~ Starfall part II ~


#include <pc88-c.h>

#include "song_b.h"
#include "sprites.h"

#include "graphics.h"

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
const u8 map2[] = {\
    1,1,1,2,1,1,1,1,
    2,1,2,2,2,2,3,1,
    1,2,3,2,2,2,2,1,
    2,1,2,2,2,2,1,2,
    1,2,1,2,2,2,2,1,
    1,2,2,2,2,2,1,1,
    1,1,2,2,2,2,3,1,
    2,1,1,1,1,1,2,1
};
const u8 map3[] = {\
    3,2,2,2,2,2,2,3,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,3,1,2,2,2,
    2,2,2,1,3,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,3
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

XYPos maprelic[3];
XYPos mapexit[3];
u8 foundRelic[3];

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
u8 savedKey;

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
u8 lastRoll;
                        
#define EXPLORING 1
#define WAITING 0
#define ENC_SELECT 2
#define INTRO 3
#define ENC_CONTINUE 4
#define ENC_RETRY 5

#define GUN 11
#define SPEED 12
#define BOOK 13

typedef struct playerStats { \
    s8 gun;
    s8 gunMax;
    s8 speed;
    s8 speedMax;
    s8 book;
    s8 bookMax;
    s8 hp;
    s8 hpMax;
} PlayerStats;
PlayerStats Libby;

XYPos box;

// F DEFS
void DrawHexTile(s8 x, s8 y, u8 index);
void DrawCleanGrid();
s16 roll(u8 numDie, u8 sides, s8 mod);
void GameInit();
void DrawFullUI();
void DrawAreaAroundPlayer();
void BufferInput();
inline void MovementKeys();
void ConfigIntro();
void CPUWAIT(u16 n);
void RunIntro();
void PrintExploreUI(u8 full);
void LoadMap(u8 map);
void BeginEncounter(u8 encNo);
void SetPlayerPosByGrid(u8 x, u8 y);


#include "intro.h"

#include "encounters.c"



#define GRASS 1
#define TREE 2
#define CITY 3
#define TEMPGVR_SPRITE_0 0xfe80 
#define TEMPGVR_SPRITE_1 0xfee0

Encounter* currentEncounter;

enum Inputs { 
    UpRight=9, Right=6, DownRight=3, UpLeft=7, Left=4, DownLeft=1, Confirm=5, Cancel=0, Gun=11, Speed=12, Book=13, Yes=21, No=22
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

 void EraseCombat()
 {
        box.x = 49;
    box.y = 155;
    SETBANK_RED()
    EraseVRAMArea(&box, 30, 44);
    SETBANK_GREEN()
    EraseVRAMArea(&box, 30, 44);
    SETBANK_BLUE()
    EraseVRAMArea(&box, 30, 44);
    SETBANK_MAINRAM()
 }

void DrawLibby()
{
    ExpandedGVRAM_On();     
    EnableALU(FASTMEM_ON);
    DrawTransparentImage_V2(player_pos.x, player_pos.y, &librarianSprite[0], 32/8, 24);
    DisableALU(FASTMEM_OFF);
    ExpandedGVRAM_Off();   
}

void Ending()
{
    CLS();
    box.x = 0;
    box.y = 0;
    SETBANK_RED()
    EraseVRAMArea(&box, 79, 199);
    SETBANK_GREEN()
    EraseVRAMArea(&box, 79, 199);
    SETBANK_BLUE()
    EraseVRAMArea(&box, 79, 199);
    SETBANK_MAINRAM()
    SetCursorPos(30,10);
    print("               YOU WIN!!\n\n");
    print(" Libby has secured the forbidden artefacts\n\n");
    print("and secured this region against the Combine.\n\n");
    print("She is reknowned as a hero for ages to come.\n\n");
    print("         THANKS FOR PLAYING!!");
    while(1){};
    
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
    
    Libby.hp = 10;
    Libby.hpMax = 10;

    
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

    currentMap = 1;

LOADTHEMAP:
    Libby.speedMax = currentMap+1;
    Libby.gunMax = currentMap+1;
    Libby.bookMax = currentMap+1;
    Libby.speed = Libby.speedMax ;
    Libby.book = Libby.bookMax ;
    Libby.gun = Libby.gunMax ;

    playingSong = false;
    ticker = 0;

    if(currentMap == 4)
        Ending();

    for(c = 0; c < 64; c++)
        revealedMap[c] = 0;

    
    IRQ_OFF;

    CLS();
    ClearAttributeRam();
    
    LoadMap(currentMap);

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
    
    SetPlayerPosByGrid(player_hex_pos.x, player_hex_pos.y);
    
    // copy out BACKGROUND
    ExpandedGVRAM_Copy_On();
    ALUCopyOut(GVRAM_BASE+(player_pos.y*80)+player_pos.x, TEMPGVR_SPRITE_0, 4, 24);

    DrawLibby();
    PrintExploreUI(1);
    
    inputMode = EXPLORING;
    
    SetCursorPos(30,16);
    print("               ");

    savedKey = 0;
    inputWait = 60;    

    IRQ_ON;
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
                    RUNHEXENCOUNTER:
                    if((maprelic[currentMap-1].x == player_hex_pos.x) \
                        && (maprelic[currentMap-1].y == player_hex_pos.y))
                    {
                        // Its the relic!!
                        for(u8 g = 15; g < 20; g++)
                            TextRowCopy(0, g);
                        SetCursorPos(25, 17);
                        print("You found the [RELIC]. Time to escape!");
                        foundRelic[currentMap-1] = 1;
                        CPUWAIT(500);
                        TextRowCopy(0, 17);
                        inputWait = 30;
                        PrintExploreUI(true);
                    }
                    if((mapexit[currentMap-1].x == player_hex_pos.x) \
                        && (mapexit[currentMap-1].y == player_hex_pos.y))
                    {
                        // Its the exit!!
                        if(foundRelic[currentMap-1])
                        {
                            for(u8 g = 15; g < 20; g++)
                                TextRowCopy(0, g);
                            SetCursorPos(25, 17);
                            print("You made it out!! On to the next zone...");
                            CPUWAIT(500);
                            currentMap++;
                            //LoadMap(currentMap);
                            inputWait = 30;
                            // END MAP GO TO NEXT!!
                            goto LOADTHEMAP;
                        }
                        else { 
                            for(u8 g = 15; g < 20; g++)
                                TextRowCopy(0, g);
                            SetCursorPos(15, 17);
                            print("You found the exit, but you can't leave yet!\n You have to find the [RELIC]!!");
                            CPUWAIT(400);
                            inputWait = 30;
                            PrintExploreUI(true);
                        }
                    }

                    if(revealedMap[(player_hex_pos.y*8)+player_hex_pos.x] != 2)
                    {
                        revealedMap[(player_hex_pos.y*8)+player_hex_pos.x] = 2;
                         // search this tile - 10% grass, 20% forest event
                        // city - RELIC or EXIT?
                        s16 s = roll(1, 20, 0);
                        u8 tt;
                        if(currentMap == 1)
                            tt = map1[(player_hex_pos.y*8)+player_hex_pos.x];
                        else if(currentMap == 2)
                            tt = map2[(player_hex_pos.y*8)+player_hex_pos.x];
                        else if(currentMap == 3)
                            tt = map3[(player_hex_pos.y*8)+player_hex_pos.x];
                        
                        if(tt == GRASS)
                        {
                            if(s <= 2) //1 or 2?
                            {
                                s16 s = roll(3, 6, -3);
                                BeginEncounter((u8)s);
                            }    
                            else 
                                NoEnc();
                        }
                        else if (tt == TREE)
                        {
                            if(s <= 4) //20%
                            {
                                s16 s = roll(3, 6, -3);
                                BeginEncounter((u8)s);                         
                            }
                            else { 
                                NoEnc();
                            }    
                        }
                    }
                    
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
                            u8 ti = (player_hex_pos.y * 8) + player_hex_pos.x;
                            //if(hh == 1)
                            //    goto RUNHEXENCOUNTER;
                            u8* mi = ((currentMap-1) * 64) + &map1[0] + ti;
                            // get map index of current tile...
                            if(*mi == GRASS)
                            {
                                u8 re = roll(1,3,0);
                                if(re > 2)
                                    goto RUNHEXENCOUNTER;
                            } else if(*mi == TREE){ 
                                u8 re = roll(1,2,0);
                                if(re==2)
                                    goto RUNHEXENCOUNTER;
                            }
                            
                        }
                    ;
                    // if not valid tile, dont take a turn.
                }
            }
            else if(inputMode == ENC_RETRY)
            {
                // NG! You take damage: 2 
                // OK! You got away safely.
                u8 retryFail = 0;        
                if(lastKey == Yes)
                {
                    inputWait = 30;
                    if(savedKey == Gun){
                        if(Libby.gun > 0)
                            Libby.gun -= 1;
                        else 
                            retryFail = 1;
                    } else if(savedKey == Speed){
                        if(Libby.speed > 0)
                            Libby.speed -= 1;
                        else 
                            retryFail = 1;
                    } else if(savedKey == Book){
                        if(Libby.book > 0)
                            Libby.book -= 1;
                        else 
                            retryFail = 1;
                    }
                    if(retryFail == 0){
                        lastRoll += roll(1, 6, 0);
                        for(u8 g = 15; g < 20; g++)
                            TextRowCopy(0, g);
                        
                        PrintExploreUI(0);
                        SetCursorPos(5, 16);
                        print("New roll: ");
                        print(byToDec(lastRoll));
                        inputMode = ENC_SELECT;
                        goto CHECKENCOUNTER;
                        //lastKey = savedKey;
                    }
                    
                }
                else if(lastKey == No)
                {
                    retryFail = 1;
                }
                if(retryFail)
                {
                    // clear text
                    u8 e;
                    EraseVRAMArea(VRAMAddrByTile(53, 154), 25, 45);
                    for(u8 g = 15; g < 20; g++)
                        TextRowCopy(0, g);
                    SetCursorPos(4, 17);
                    print("NG! You take damage: ");
                    for(e = 0; e < 2; e++)
                    {
                        if(currentEncounter->stats[e] == savedKey)
                            break;
                    }
                    Libby.hp -= currentEncounter->damage[e];
                    print(byToDec(currentEncounter->damage[e]));
                    print("\n(Press a key.)");
                    CPUWAIT(100);
                    inputMode = ENC_CONTINUE;
                    savedKey = 0;
                    EraseCombat();
                    DrawLibby();
                }
            }
            else if(inputMode == ENC_SELECT)
            {
                for(u8 e = 0; e < 2; e++)
                {
                    if((currentEncounter->stats[e] == lastKey) || (savedKey != 0)) // Is the key pressed one of available?
                    {
                        if(savedKey != 0)
                            lastKey = savedKey;
                        inputWait = 30;
                        //inputMode = ENC_RETRY;
                        // clear text
                        u8 g;
                        for(g = 16; g < 20; g++)
                            TextRowCopy(0, g);
                        // e.g. 
                        // All stat checks are 2d6. 
                        // Result(Gun/Speed/Book): 4
                        lastRoll = roll(2, 6, 0);
                        SetCursorPos(5,16);
                        print("Result [");
                        if(lastKey == Gun)
                        {
                            print("Gun");
                            savedKey = Gun;
                        } else if (lastKey == Speed)
                        {
                            print("Speed");
                            savedKey = Speed;
                        } else if (lastKey == Book)
                        {
                            print("Book");
                            savedKey = Book;
                        }
                        print("]: ");
                        print(byToDec(lastRoll));
                        CHECKENCOUNTER:
                        if(lastRoll < currentEncounter->difficulty[e])
                        {
                            SetCursorPos(6, 17);
                            print("Failed!! Use effort? (Remaining: ");
                            if(lastKey == Gun)
                                print(byToDec(Libby.gun));
                            else if(lastKey == Book)
                                print(byToDec(Libby.book));
                            else if(lastKey == Speed)
                                print(byToDec(Libby.speed));
                            print(")");
                            SetCursorPos(8, 18);
                            print("[Y]es to retry or [N]o to fail.");
                            inputMode = ENC_RETRY;
                        }
                        else { 
                            SetCursorPos(6, 17);
                            print("OK!! You gain experience and 1 hit point.");
                            Libby.hpMax++;
                            EraseCombat();
                            DrawLibby();
                            //inputWait = 100;
                            CPUWAIT(500);
                            inputMode = EXPLORING;
                            for(g = 15; g < 20; g++)
                                TextRowCopy(0, g);
                           
                            PrintExploreUI(1);
                            savedKey = 0;
                            
                        }
                    }
                }
            }
            else if(inputMode == ENC_CONTINUE)
            {
                if((currentEncounter->encounterSpr == &chest[0]) \
                    && (currentEncounter->stats[0] == 0))
                {
                    //heal?
                    Libby.hp += currentEncounter->damage[1];
                    if(Libby.hp > Libby.hpMax)
                        Libby.hp = Libby.hpMax;
                }
                if(lastKey == Confirm)
                {
                    inputWait = 30;
                    // clear text
                    for(u8 g = 15; g < 20; g++)
                        TextRowCopy(0, g);
                    PrintExploreUI(1);
                    inputMode = EXPLORING;
                }
            }
        } // end input loop

        // Main Loop
        // modify seed 
        if(inputWait > 0)
            inputWait--;
        lastKey = -1;
        RANDOMSEED += rand();
        
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
        revealedMap[(player_hex_pos.y*8)+player_hex_pos.x] = 2;
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
#include <pc88-c.h>

enum directions { UP, DOWN, LEFT, RIGHT };

#include "graphics.h"
#include "math.h"

extern u16 RANDOMSEED;

#define GFX_OFF 0b00010011
#define GFX_ON 0b00011011

#define PLAYFIELD_RIGHT 45
#define PLAYFIELD_LEFT 2
#define PLAYFIELD_BTM 168
#define PLAYFIELD_TOP 8

enum directions playerDir;

XYpos ball_pos;
XYpos ball_oldpos;
XYpos bar_pos;
XYpos bar_oldpos;
s8 by_speed;
s8 bx_speed;
u8 bar_speed;
bool moved;
bool edgeBounce;

typedef struct levelblock { 
    u8 x;
    u8 y;
    u8 hits;
} LevelBlock;

u8 LEVELBLOCKSIZE;
LevelBlock BLOCK_LEVEL[150];
u8 tick;
u8 PLAYER_SPEED;
u16 pScore;

void DrawSpritePlane(u8* dat, XYpos* xy, u8 w, u8 h);
void DrawSprite(Sprite* spr, signed int x, signed int y);
//void DrawRLEBitmap(PlanarBitmap* pb, u16 x, u16 y);
void InitGUI();
void ClearAllVRAM();
inline void GAME_INIT();
inline void GAME_DRAW();
inline void GAME_UPDATE();
inline void GAME_INPUT();
void AddScore(u8 sc);
void EraseBlock(LevelBlock* b);
u8 GetCollisionDirection(u8 x, u8 y, LevelBlock* b);
LevelBlock* GetQuadrantBlock(u8 d, u8 blocknum);
u8 GetBallCollision(s8 xsp, s8 ysp);

void main()
{
    __asm 
      ld sp,#0x0100
    __endasm;
    GAME_INIT();
    
    while(1)
    {
        GAME_UPDATE();

        GAME_INPUT();

        GAME_DRAW();
    }
}

inline void InitGUI()
{
    // Corners
    DrawSprite(&tile_01, PLAYFIELD_LEFT-2, PLAYFIELD_TOP-8);
    DrawSprite(&tile_01, PLAYFIELD_RIGHT+2, PLAYFIELD_TOP-8);
    DrawSprite(&tile_01, PLAYFIELD_LEFT-2, PLAYFIELD_BTM);
    DrawSprite(&tile_01, PLAYFIELD_RIGHT+2, PLAYFIELD_BTM);
    DrawSprite(&tile_02, PLAYFIELD_LEFT, PLAYFIELD_TOP-8);
    DrawSprite(&tile_02, PLAYFIELD_LEFT, PLAYFIELD_BTM);
    // Top/bottom bars
    for(u8 i = 0; i < 43; i ++){
        DrawSprite(&tile_03, PLAYFIELD_LEFT+i+1, PLAYFIELD_TOP-8);
        DrawSprite(&tile_03, PLAYFIELD_LEFT+i+1, PLAYFIELD_BTM);
    }
    DrawSprite(&tile_04, PLAYFIELD_RIGHT+1, PLAYFIELD_TOP-8);
    DrawSprite(&tile_04, PLAYFIELD_RIGHT+1, PLAYFIELD_BTM);
    DrawSprite(&tile_05, PLAYFIELD_LEFT-2, PLAYFIELD_TOP);
    DrawSprite(&tile_05, PLAYFIELD_RIGHT+2, PLAYFIELD_TOP);
    // Left/right bars
    for(u8 i = 2; i < 20; i++){
        DrawSprite(&tile_06, PLAYFIELD_LEFT-2, i * 8);
        DrawSprite(&tile_06, PLAYFIELD_RIGHT+2, i * 8);
    }
    DrawSprite(&tile_07, PLAYFIELD_LEFT-2, PLAYFIELD_BTM-8);
    DrawSprite(&tile_07, PLAYFIELD_RIGHT+2, PLAYFIELD_BTM-8);
    // Flame
    for(u8 t = 2; t <= 46; t++) DrawSprite(&tile_08, t, 160);
    // Draw title/text bitmaps:
    //DrawRLEBitmap(&title_1, 51, 4);
    DrawSprite((Sprite*)&title_1, 51, 4);
    DrawSprite((Sprite*)&leveltxt, 59, 56);
    DrawSprite((Sprite*)&scoretxt, 58, 104);
    // Edge bits
    DrawSprite((Sprite*)&edgetxt, 32, 178);
    for(u8 f = 0; f < 5; f++) DrawSprite(&edgeGem_0, 41 + f, 180);
    DrawSprite(&fire_no, 46, 176);
}

void ClearAllVRAM()
{
    XYpos p;
    p.x = 0;
    p.y = 0;
    SETBANK_BLUE()
    EraseVRAMArea(&p, 80, 200);
    SETBANK_GREEN()
    EraseVRAMArea(&p, 80, 200);
    SETBANK_RED()
    EraseVRAMArea(&p, 80, 200);
    SETBANK_MAINRAM()
}

void LoadLevel(u8* lvl)
{
    //u8* lvl = &level1data[0];
    for(u8 yb = 0; yb < 10; yb++){
        for(u8 xb = 0; xb < 15; xb++){
            u8 ofs = (yb * 15) + xb;
            if(lvl[ofs] != null) { 
                BLOCK_LEVEL[ofs].x = 2 + (3 * xb);
                BLOCK_LEVEL[ofs].y = 8 + (yb * 8);
                if(lvl[ofs] == 11) {
                    BLOCK_LEVEL[ofs].hits = 99;
                } else {
                    BLOCK_LEVEL[ofs].hits = 2;
                }
                DrawSprite(blockTypes[lvl[ofs]], BLOCK_LEVEL[ofs].x, BLOCK_LEVEL[ofs].y);
            }
        }
    }
}


inline void GAME_INIT()
{
    SetIOReg(0x31, GFX_OFF);
    
    SETBANK_MAINRAM()
    ClearAttributeRam();
    for(u8 ff = 0; ff < 24; ff++){
        SetCursorPos(0, ff);
        print("                                                                    \x00");
    }

    PLAYER_SPEED = 1; // 1 tick per frame
    bar_speed = 2;
    SetCursorPos(20, 9);
    print("Loading 'HACHINOID' ...\nPREPARE TO DIE\x00");
    
    // Initialize Hachinoid Vars
    blockTypes[0] = null;
    blockTypes[1] = &redBlock;
    blockTypes[2] = &blueBlock;
    blockTypes[3] = &greenBlock;
    blockTypes[4] = &cyanBlock;
    blockTypes[5] = &magentaBlock;
    blockTypes[6] = &yellowBlock;
    blockTypes[7] = &whiteBlock;
    blockTypes[8] = &hardRed;
    blockTypes[9] = &hardGreen;
    blockTypes[10] = &hardBlue;
    blockTypes[11] = &stoneBlock;
    levels[1] = &level1data[0];
    levels[2] = &level2data[0];
    
    ball_pos.x = 16;
    ball_pos.y = 120;
    bar_pos.x = 20;
    bar_pos.y = 154;
    bx_speed = 4;
    by_speed = 4;
    //moved = true;
    pScore = 0;

    // Draw GUI while screen is off
    InitGUI();
    SetIOReg(0x31, GFX_ON);
    
    // erase load text 
    SETBANK_MAINRAM()
    SetCursorPos(20, 9);
    print("                       \n                  ");
    SetCursorPos(60, 13);
    
    // Level 01!
    DrawSprite(&s_0, 62, 80);
    DrawSprite(&s_1, 64, 80);
    
    // Initialize level + data
    LEVELBLOCKSIZE = 150;
    
    LoadLevel(levels[1]);
    SETBANK_MAINRAM();

    // DOOT DO DOO!
    beep(BEEP_C5, 50);
    for(u16 i = 10000; i > 0; i--) { 
        __asm nop __endasm;
    }
    beep(BEEP_C5, 50);
    beep(BEEP_G5, 200);
}

inline void RandomizeBall()
{
    // adjust bx_speed and by_speed by +-1 within their bounds
    // based on a random roll 
    u8 r = rand();
    if(r < 128) { 
        // x
        if (r < 64) { 
            bx_speed++;
            by_speed--;
        }
        else {
            bx_speed--;
            by_speed++;
        }
    }
    else { 
        // y
        if (r < 192) {
            by_speed++;
            bx_speed--;
        }
        else {
            by_speed--;
            bx_speed++;
        }
    }
    if(abs(bx_speed) + abs(by_speed) < 8) {
        if(bx_speed < 0) bx_speed --;
        if(bx_speed > 0) bx_speed++;
    }
}

///////////////////////
/// Update
//////////////////////
u8 lx, ly;
inline void GAME_UPDATE()
{
    bool xOk = false;
    edgeBounce = false;
    RANDOMSEED += rand();
    // What is my speed? 
    tick += abs(bx_speed); // 4
    PLAYER_SPEED = 8 - abs(bx_speed); // 5
    if(tick >= PLAYER_SPEED) {
        tick = 0;
        xOk = true;
    }
    // save for erase
    ball_oldpos.x = ball_pos.x;
    ball_oldpos.y = ball_pos.y;
    s8 lr= 0;
    if(bx_speed < 0) lr = -1;
    else if(bx_speed < -4) lr = -2;
    else if(bx_speed > 0) lr = 1;
    else if(bx_speed > 4) lr = 2;
    // get collision
    if(ball_pos.y < 136) {     
        if(!GetBallCollision(lr, by_speed)){
            if(xOk) ball_pos.x += lr;
            ball_pos.y += by_speed;
        } else { 
            if(edgeBounce) { 
                // Edge bounce!
                RandomizeBall();
            }
        }
    } else { // small optimization
        Wait_VBLANK();
        if(xOk) ball_pos.x += lr;
        ball_pos.y += by_speed;
    }
    if(bx_speed == 0) RandomizeBall();
    if(by_speed == 0) RandomizeBall();
    if(bx_speed > 8) bx_speed = 8;
    if(bx_speed < -8) bx_speed = -8;
    if(by_speed > 7) by_speed = 7;
    if(by_speed < -7) by_speed = -7;
    if(by_speed == -1) by_speed--;
    if(by_speed == 1) by_speed++;

    // bounce
    if(ball_pos.x > (44 + abs(lr)) ) {
        if(bx_speed > 0) bx_speed *= -1;
        beep(600, 5);
    } else if (ball_pos.x <= (1 + abs(lr))) {
        if(bx_speed < 0) bx_speed *= -1;
        beep(600, 5);
    }
    // hit paddle
    if(bar_pos.x < ball_pos.x + 1){
        if((bar_pos.x + 6) > ball_pos.x){
            if(bar_pos.y < ball_pos.y + 4){
                if((bar_pos.y + 8) > ball_pos.y){
                    ball_pos.y = bar_pos.y - 4;
                    by_speed *= -1;
                    beep(90, 5);
                }
            }
        }
    }
    if(ball_pos.y + abs(by_speed) >= 168) {
        // Miss! Game over!
        by_speed = 0;
        bx_speed = 0;
        ball_pos.y = 15;
        beep(700, 50);
        Wait_VBLANK();
        SETBANK_MAINRAM()
        SetCursorPos(15, 8);
        print("YOU ARE DEAD!!!\x00");
        SetCursorPos(15, 9);
        print("SpaceBar to restart?\x00");
        while(1){
            if(GetKeyDown(KB_SPACE)){
                
                ClearAttributeRam();
                ClearAllVRAM();
                __init();
            }
        }
    } else if (ball_pos.y < (8 - by_speed)) {
        //RandomizeBall();
        by_speed *= -1;
        beep(600, 5);
    }
    
    
    if(edgeBounce)
    {
        //randomize angle!

    }

    // save for erase 
    bar_oldpos.x = bar_pos.x;
    bar_oldpos.y = bar_pos.y;
}

////////////////
/// Input
////////////////
inline void GAME_INPUT()
{
    if (GetKeyDown(KB_D) || GetKeyDown(KB_PAD6)){
        if(bar_pos.x < 41){
            bar_pos.x += bar_speed;
            //moved = true;
            playerDir = RIGHT;
        } else { bar_pos.x = 41; }
    } else if (GetKeyDown(KB_A) || GetKeyDown(KB_PAD4)){
        if(bar_pos.x > 2){
            bar_pos.x -= bar_speed;
            //moved = true;
            playerDir = LEFT;
        } else { bar_pos.x = 2; }
    }
}

///////////////
/// Draw 
///////////////
inline void GAME_DRAW()
{
    Wait_VBLANK();
    // ball 
    SETBANK_GREEN();
    EraseVRAMArea(&ball_oldpos, 1, 4);
    DrawSprite(&ball, ball_pos.x, ball_pos.y);
    
    // paddle
    SETBANK_BLUE();
    EraseVRAMArea(&bar_oldpos, 6, 8);
      // if you want cyan:
    //SETBANK_RED();
    //EraseVRAMArea(&bar_oldpos, 6, 6);
    DrawSprite(&bar, bar_pos.x, bar_pos.y);
    // back to mainRAM, print score
    SETBANK_MAINRAM()
    SetCursorPos(60, 13);
    for(s8 z = 3; z >= 0; z--) {
        u8 c = (pScore & (0xf << (z*4))) >> (z*4);
        putchr(c + 0x30);
    }
}


void DrawSpritePlane(const u8* dat, XYpos* xy, u8 w, u8 h)
{   
    u8 xt = (u8)(xy->x);
    vu8* vp = (vu8*)(0xc000 + xt + (xy->y * 80));
    for(u8 z = 0; z < h; z++)
    {
        for(u8 f = 0; f < w; f++)
            *vp++ = dat[(z*w) + f];
        vp += (80-w);
    }
}

void DrawSprite(Sprite* spr, signed int x, signed int y)
{
    XYpos xy = { x, y };
    if(spr->r != null) { 
        SETBANK_RED();
        DrawSpritePlane(spr->r, &xy, spr->w, spr->h);
    }
    if(spr->g != null) {
        SETBANK_GREEN();
        DrawSpritePlane(spr->g, &xy, spr->w, spr->h);
    }
    if(spr->b != null) {
        SETBANK_BLUE();
        DrawSpritePlane(spr->b, &xy, spr->w, spr->h);
    }
}

// Adds a base-16 number (sc) to a base-10 number (pScore)
//  This keeps pScore in a easily printable format.
void AddScore(u8 sc)
{
    pScore += sc;
    for(u8 n = 0; n < 3; n++) // number of nibbles in pScore
    {
        u8 p = (u8)((pScore & (0xf << (n*4))) >> (n*4));
        if(p > 9) { 
            pScore += (1 << ((n+1)*4)); 
            pScore &= 0xfff0;
        } 
    }
    // for each digit, is AND Fh > 9? then add the remainder
}

void EraseBlock(LevelBlock* b)
{
    XYpos t = { b->x, b->y };
    SETBANK_BLUE()
    EraseVRAMArea(&t, 3, 8);
    SETBANK_GREEN()
    EraseVRAMArea(&t, 3, 8);
    SETBANK_RED()
    EraseVRAMArea(&t, 3, 8);
    b->x = 0; b->y = 0;
    SETBANK_MAINRAM()
    AddScore((u8)1); // How many points am I worth?
}

// For edge collisions
u8 GetCollisionDirection(u8 x, u8 y, LevelBlock* b)
{
    // check what side of the block the ball is on.
    // if L/R : flip x 
    // if U/D : flip y 
    // get the quadrant versus the block!
    u8 h = 0b1111;
    if((x + 1) < (b->x + 3)) h ^= 0b1;// NOT right 
    if((y + 4) < (b->y + 8)) h ^= 0b10;// NOT bottom 
    if((x) > (b->x)) h ^= 0b100;// NOT left 
    if((y) > (b->y)) h ^= 0b1000;// NOT top 
    return h;
    // returns bit field of POSSIBLE DIRECTIONS. 
}

// Not finished
LevelBlock* GetQuadrantBlock(u8 d, u8 blocknum)
{
   return &BLOCK_LEVEL[blocknum];
}


u8 GetBallCollision(s8 xsp, s8 ysp)
{
    // Is the location at ball_pos.x + xsp, ball_pos.y + ysp a COLLIDEABLE?
    // Collideables are in the BLOCK_LEVEL[] array, and
    //  are the ULR borders of the screen. 
    u8 txp; u8 typ;
    txp = ball_pos.x + xsp;
    typ = ball_pos.y + ysp;
    for(u8 i = 0; i < LEVELBLOCKSIZE; i++){
        if(txp < (BLOCK_LEVEL[i].x + 3)){
            if((txp + 1) > BLOCK_LEVEL[i].x){
                if(typ < (BLOCK_LEVEL[i].y + 8)){
                    if((typ + 4) > BLOCK_LEVEL[i].y){
                        LevelBlock* bl = &BLOCK_LEVEL[i];
                        u8 d = GetCollisionDirection(txp, typ, bl); // from what side?
                        //SetCursorPos(45, 18); // debug text
                        //print("         \x00"); 
                        if(d == 0b1) { //rt
                            bx_speed *= -1;
                            ball_pos.x = (bl->x+3);// - xsp; 
                        } 
                        else if(d == 0b100) {  //lf
                            bx_speed *= -1; 
                            ball_pos.x = (bl->x-1);// + xsp;
                        } 
                        else if(d == 0b10) { //btm
                            by_speed *= -1; 
                            ball_pos.y = (bl->y+8);// - ysp;
                        } 
                        else if(d == 0b1000) { //tp
                            by_speed *= -1; 
                            ball_pos.y = (bl->y-4);// + ysp;
                        } 
                        else { 
                            SetCursorPos(45, 18);
                            //print("Edge  \x00");
                            edgeBounce = true;
                            by_speed *= -1;
                            bx_speed *= -1;
                        }
                        if(bl->hits != 99)
                            EraseBlock(bl); // Fix me later
                        beep(100, 10);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}


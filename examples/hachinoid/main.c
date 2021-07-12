#include <pc88-c.h>
//#include "math.h"

enum directions { UP, DOWN, LEFT, RIGHT };

#include "graphics.h"
#include "leveldata.h"

extern u16 RANDOMSEED;

#define GFX_OFF 0b00010011
#define GFX_ON 0b00011011

#define PLAYFIELD_RIGHT 45
#define PLAYFIELD_LEFT 2
#define PLAYFIELD_BTM 168
#define PLAYFIELD_TOP 8

enum directions playerDir;

u8 levelMaxScore;
XYpos ball_pos;
XYpos ball_oldpos;
XYpos bar_pos;
XYpos bar_oldpos;
s8 by_speed;
s8 bx_speed;
u8 bar_speed;
bool moved;
bool edgeBounce;
u8 edgeCtr;
bool ballFlash;
bool firing;
XYpos bullet_pos;
XYpos bullet_oldpos;
bool starting;
s8 lastMoved;
const String fs = "THE SQUADRONS OF BLOXX APPROACH";
const String fs2 = "THEIR DEATHS ARE REQUIRED";
const String fs3 = "IT IS KILL OR BE KILLED";
const String fs4 = "Instructions:\x00";
const String fs5 = "[A]/[D]  or  [4]/[6] :\x00";
const String fs6 = "Move BLOCK-BUSTER left / right\x00";
const String fs7 = " [SPACE or C] :\x00";
const String fs8 = "Fire BUSTER-WAVE (requires 5 Edge)\x00";
const String fs9 = "[0 - 9] : Select Battlefield\n[SPACE or C] : START BATTLE\x00";

typedef struct levelblock { 
    u8 x;
    u8 y;
    u8 hits;
    u8 score;
} LevelBlock;

LevelBlock* bls;
u8 LEVELBLOCKSIZE;
LevelBlock BLOCK_LEVEL[150];
u8 tick;
u8 PLAYER_SPEED;
u16 pScore;
u8 currentLevel;
bool cln;

void DrawSpritePlane(const u8* dat, XYpos* xy, u8 w, u8 h);
void DrawSprite(Sprite* spr, signed int x, signed int y);
//void DrawRLEBitmap(PlanarBitmap* pb, u16 x, u16 y);
void InitGUI();
void ClearAllVRAM();
inline void GAME_INIT();
inline void GAME_DRAW();
inline void GAME_UPDATE();
inline void GAME_INPUT();
void AddScore();
void EraseBlock(LevelBlock* b);
u8 GetCollisionDirection(u8 x, u8 y, LevelBlock* b);
u8 GetBallCollision(s8 xsp, s8 ysp);
void CPUWAIT(u16 n);
void TickPrint(u8* str, u8 len);

void main()
{
    //cln = CLEANRESET;
    __asm__("ld sp,#0x0100");
    
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
    levelMaxScore = 0;
    starting = true;
    for(u8 yb = 0; yb < 10; yb++){
        for(u8 xb = 0; xb < 15; xb++){
            u8 ofs = (yb * 15) + xb;
            if(lvl[ofs] != null) { 
                BLOCK_LEVEL[ofs].x = PLAYFIELD_LEFT + (3 * xb);
                BLOCK_LEVEL[ofs].y = PLAYFIELD_TOP + (yb * 8);
                if(lvl[ofs] == 11) {
                    BLOCK_LEVEL[ofs].hits = 99;
                } else {
                    levelMaxScore++;
                    BLOCK_LEVEL[ofs].hits = 2;
                    BLOCK_LEVEL[ofs].score = 5;
                }
                DrawSprite(blockTypes[lvl[ofs]], BLOCK_LEVEL[ofs].x, BLOCK_LEVEL[ofs].y);
            }
            else { 
                BLOCK_LEVEL[ofs].x = 0;
                BLOCK_LEVEL[ofs].hits = 0;
                BLOCK_LEVEL[ofs].score = 0;
            }
        }
    }
}

inline void GetTitleInput()
{
    XYpos lvpos; lvpos.x = 62; lvpos.y = 80;
        if(GetKeyDown(KB_0)) { 
            currentLevel = 10;
            EraseVRAMArea(&lvpos, 4, 16);
            DrawSprite(&s_1, lvpos.x, lvpos.y);
            DrawSprite(&s_0, lvpos.x + 2, lvpos.y);
        }
        else if(GetKeyDown(KB_1)) { 
            currentLevel = 1;
            EraseVRAMArea(&lvpos, 4, 16);
            DrawSprite(&s_0, lvpos.x, lvpos.y);
            DrawSprite(&s_1, lvpos.x + 2, lvpos.y);
        }
        else if(GetKeyDown(KB_2)) { 
            currentLevel = 2;
            EraseVRAMArea(&lvpos, 4, 16);
            DrawSprite(&s_0, lvpos.x, lvpos.y);
            DrawSprite(&s_2, lvpos.x + 2, lvpos.y);
        }
        else if(GetKeyDown(KB_3)) { 
            currentLevel = 3;
            EraseVRAMArea(&lvpos, 4, 16);
            DrawSprite(&s_0, lvpos.x, lvpos.y);
            DrawSprite(&s_3, lvpos.x + 2, lvpos.y);
        }
        else if(GetKeyDown(KB_4)) { 
            currentLevel = 4;
            EraseVRAMArea(&lvpos, 4, 16);
            DrawSprite(&s_0, lvpos.x, lvpos.y);
            DrawSprite(&s_4, lvpos.x + 2, lvpos.y);
        }
        else if(GetKeyDown(KB_5)) { 
            currentLevel = 5;
            EraseVRAMArea(&lvpos, 4, 16);
            DrawSprite(&s_0, lvpos.x, lvpos.y);
            DrawSprite(&s_5, lvpos.x + 2, lvpos.y);
        }
        else if(GetKeyDown(KB_6)) { 
            currentLevel = 6;
            EraseVRAMArea(&lvpos, 4, 16);
            DrawSprite(&s_0, lvpos.x, lvpos.y);
            DrawSprite(&s_6, lvpos.x + 2, lvpos.y);
        }
        else if(GetKeyDown(KB_7)) { 
            currentLevel = 7;
            EraseVRAMArea(&lvpos, 4, 16);
            DrawSprite(&s_0, lvpos.x, lvpos.y);
            DrawSprite(&s_7, lvpos.x + 2, lvpos.y);
        }
        else if(GetKeyDown(KB_8)) { 
            currentLevel = 8;
            EraseVRAMArea(&lvpos, 4, 16);
            DrawSprite(&s_0, lvpos.x, lvpos.y);
            DrawSprite(&s_8, lvpos.x + 2, lvpos.y);
        }
        else if(GetKeyDown(KB_9)) { 
            currentLevel = 9;
            EraseVRAMArea(&lvpos, 4, 16);
            DrawSprite(&s_0, lvpos.x, lvpos.y);
            DrawSprite(&s_9, lvpos.x + 2, lvpos.y);
        }
}

inline void GAME_INIT()
{
    SetIOReg(0x31, GFX_OFF);
    
    SETBANK_MAINRAM()
    ClearAttributeRam();
    // Clear screen text
    for(u8 ff = 0; ff < 24; ff++){
        SetCursorPos(0, ff);
        print("                                                                    \x00");
    }
    ballFlash = false;
    PLAYER_SPEED = 1; // 1 tick per frame
    bar_speed = 2;
    SetCursorPos(20, 9);
    print("Loading 'HACHINOID' ...\n   PREPARE TO DIE\x00");
    
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
    levels[3] = &level3data[0];
    levels[4] = &level4data[0];
    levels[5] = &level5data[0];
    levels[6] = &level6data[0];
    levels[7] = &level7data[0];
    levels[8] = &level8data[0];
    levels[9] = &level9data[0];
    levels[10] = &level10data[0];
    
    ball_pos.x = 16;
    ball_pos.y = 120;
    bar_pos.x = 20;
    bar_pos.y = 154;
    bx_speed = 4;
    by_speed = 4;
    bullet_pos.x = 0;
    bullet_pos.y = 0;
    if(cln) pScore = 0;
    edgeCtr = 0;
    firing = false;

    // Draw GUI while screen is off
    InitGUI();
    SetIOReg(0x31, GFX_ON);
    
    // erase load text 
    SETBANK_MAINRAM()
    SetCursorPos(20, 9);
    print("                       \n                  ");
    SetCursorPos(60, 13);
    
    // Level :01:
    LEVELBLOCKSIZE = 150;
    DrawSprite(&s_0, 62, 80);
    DrawSprite(&s_1, 64, 80);
    
    // Initialize title screen / level + data
    currentLevel = 1;
    SETBANK_MAINRAM();
    SetCursorPos(6, 5);
    TickPrint(&fs[0], 31);
    CPUWAIT(20);
    SetCursorPos(7, 6);
    TickPrint(&fs2[0], 25);
    CPUWAIT(20);
    SetCursorPos(8, 7);
    TickPrint(&fs3[0], 23);
    CPUWAIT(40);
    // Instructions
    SetCursorPos(10, 9);
    print(&fs4[0]);
    SetCursorPos(9, 10);
    print(&fs5[0]);
    SetCursorPos(5, 11);
    print(&fs6[0]);
    SetCursorPos(12, 12);
    print(&fs7[0]);
    SetCursorPos(6, 13);
    print(&fs8[0]);
    SetCursorPos(6, 14);
    print(&fs9[0]);
    Wait_VBLANK();
    // Title Input Handler
    while(1) {
        Wait_VBLANK();
        GetTitleInput();
        
        if( GetKeyDown(KB_SPACE) || GetKeyDown(KB_C) ){
            goto lblContinue;
        }
    }
    lblContinue:
    // Clear screen text
    for(u8 fq = 0; fq < 19; fq++){
        SETBANK_MAINRAM();
        SetCursorPos(0, fq);
        print("                                        \x00");
    }
    LoadLevel(levels[currentLevel]);
    // DOOT DO DOO!
    SETBANK_MAINRAM();
    beep(BEEP_C5, 50);
    CPUWAIT(10);
    beep(BEEP_C5, 50);
    beep(BEEP_G5, 200);
}
void CPUWAIT(u16 n)
{
    for(s32 i = n * 1000; i > 0; i--) { 
        __asm__("nop");
    }
}

void TickPrint(u8* str, u8 len)
{
    for(u8 f = 0; f < len; f++) { 
        u8 n = *(str + f);
        putchr(n);
        beep(BEEP_G3, 3);
        Wait_VBLANK();
    }
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

inline void EdgeUp()
{
    if(edgeCtr < 4) {
        edgeCtr++;
        DrawSprite(&edgeGem_1, 40 + edgeCtr, 180);
    } else if (edgeCtr == 4) {
        edgeCtr++;
        DrawSprite(&edgeGem_1, 40 + edgeCtr, 180);
        DrawSprite(&fire_yes, 46, 176);
        beep(50, 10);
    }
}

inline void ReorientBall() {
    if(bx_speed == 0) RandomizeBall();
    if(by_speed == 0) RandomizeBall();
    if(bx_speed > 8) bx_speed = 8;
    if(bx_speed < -8) bx_speed = -8;
    if(by_speed > 7) by_speed = 7;
    if(by_speed < -7) by_speed = -7;
    if(by_speed == -1) by_speed--;
    if(by_speed == 1) by_speed++;
}

#define ROW0 (1 * 8) + 7
#define ROW1 (2 * 8) + 7
#define ROW2 (3 * 8) + 7
#define ROW3 (4 * 8) + 7
#define ROW4 (5 * 8) + 7
#define ROW5 (6 * 8) + 7
#define ROW6 (7 * 8) + 7
#define ROW7 (8 * 8) + 7
#define ROW8 (9 * 8) + 7
#define ROW9 (10 * 8) + 7


LevelBlock* GetBulletCollision()
{
    for(u8 i = 0; i < LEVELBLOCKSIZE; i++){
        if(BLOCK_LEVEL[i].hits > 0) { 
            if(bullet_pos.x < (BLOCK_LEVEL[i].x + 3)){
                if((bullet_pos.x + 2) > BLOCK_LEVEL[i].x){
                    if(bullet_pos.y < (BLOCK_LEVEL[i].y + 8)){
                        if((bullet_pos.y + 8) > BLOCK_LEVEL[i].y){
                            return &BLOCK_LEVEL[i];
                        }
                    }
                }
            }
        }
    }
    return null;
}

void EndStage()
{
    SETBANK_MAINRAM();
    SetCursorPos(6, 8);
    print("      VICTORY!!!\nNow try a new battlefield!!\nYour kills are saved, for now...\x00");
    ClearAttributeRam();
    ClearAllVRAM();
    main(); // reset

}

///////////////////////
/// Update
//////////////////////
u8 lx, ly;

inline void GAME_UPDATE()
{
    if(levelMaxScore == 0) EndStage();
    // every-frame vars
    bool xOk = false;
    edgeBounce = false;
    RANDOMSEED += rand();
    if(ballFlash) ballFlash = false;
    else ballFlash = true;
    // What is my speed? 
    tick += abs(bx_speed); 
    PLAYER_SPEED = 8 - abs(bx_speed);
    if(tick >= PLAYER_SPEED) {
        tick = 0;
        xOk = true;
    }
    // save for erase
    ball_oldpos.x = ball_pos.x;
    ball_oldpos.y = ball_pos.y;
    bar_oldpos.x = bar_pos.x;
    bar_oldpos.y = bar_pos.y;
    bullet_oldpos.x = bullet_pos.x;
    bullet_oldpos.y = bullet_pos.y;

    if(starting) { 
        ///////////// 
        // SHOOT THE BALL
        ball_pos.x = bar_pos.x + 3;
        ball_pos.y = bar_pos.y - 4;
    }
    else { 
        //////////////
        // BOUNCE THE BALL 
            // Determine X speed in tile
        s8 lr = 0;
        if(bx_speed < 0) lr = -1;
        else if(bx_speed < -4) lr = -2;
        else if(bx_speed > 0) lr = 1;
        else if(bx_speed > 4) lr = 2;
        // get collision
        if(!GetBallCollision(lr, by_speed)){
            if(xOk) ball_pos.x += lr;
            ball_pos.y += by_speed;
        } else { 
            if(edgeBounce) { 
                // Edge bounce!
                RandomizeBall();
                EdgeUp();
            }
        }
        if(firing) { 
            bls = GetBulletCollision();
            if(bls != null){
                if(bls->hits != 99){
                    EraseBlock(bls);
                    SETBANK_MAINRAM();
                    AddScore(); // How many points am I worth?
                    AddScore();
                }else { 
                    SETBANK_MAINRAM();
                }
                beep(100, 10);
                firing = false;
                
            }
        }
        // Fix the ball if out of whack
        ReorientBall();

        // bounce on edges
        if(ball_pos.x > (PLAYFIELD_RIGHT-1 + abs(lr)) ) {
            if(bx_speed > 0) bx_speed *= -1;
            beep(600, 5);
        } else if (ball_pos.x <= (PLAYFIELD_LEFT-1 + abs(lr))) {
            if(bx_speed < 0) bx_speed *= -1;
            beep(600, 5);
        }
        // hit paddle
        if(bar_pos.x < ball_pos.x + 1){
            if((bar_pos.x + 6) > ball_pos.x){
                if(bar_pos.y < ball_pos.y + 4){
                    if((bar_pos.y + 8) > ball_pos.y){
                        // Adjust ball reflection based on paddle angle
                        u8 po = ball_pos.x - bar_pos.x; // 0-5
                        if(po <= 1) bx_speed--;         // 0-1 // 2-3, bounce normally
                        else if(po >= 4) bx_speed++;    // 4-5 
                        ball_pos.y = bar_pos.y - 4;     // set ball on top of paddle
                        by_speed *= -1;                 // reflect! 
                        beep(90, 5);
                    }
                }
            }
        }
        if(ball_pos.y > 168) {
            // Miss! Game over!
            by_speed = 0;
            bx_speed = 0;
            ball_pos.y = 15;
            beep(700, 50);      // err!
            
            SETBANK_MAINRAM()
            SetCursorPos(15, 8);
            print("YOU ARE DEAD!!!\x00");
            SetCursorPos(15, 9);
            print("SpaceBar to restart?\x00");
            while(1){
                // Loop until space
                if(GetKeyDown(KB_SPACE) || GetKeyDown(KB_C)){
                    pScore = 0;
                    ClearAttributeRam();
                    ClearAllVRAM();
                    main();
                }
            }
        } 
        else if (ball_pos.y < (8 - by_speed)) {
            // Bounce top
            by_speed *= -1;
            beep(600, 5);
        }

    }
    // bullet
    if(firing) bullet_pos.y -= 8;
    if(bullet_pos.y <= 15) firing = false;
}

////////////////
/// Input
////////////////
inline void GAME_INPUT()
{
    //if(GetKeyDown(KB_Q)) EndStage();
    if (GetKeyDown(KB_D) || GetKeyDown(KB_PAD6)){
        if(bar_pos.x < PLAYFIELD_RIGHT-4){
            bar_pos.x += bar_speed;
            playerDir = RIGHT;
            lastMoved = bar_speed;
        } else { bar_pos.x = PLAYFIELD_RIGHT-4; }
    } else if (GetKeyDown(KB_A) || GetKeyDown(KB_PAD4)){
        if(bar_pos.x > PLAYFIELD_LEFT){
            bar_pos.x -= bar_speed;
            lastMoved = bar_speed * -1;
            playerDir = LEFT;
        } else { bar_pos.x = PLAYFIELD_LEFT; }
    }
    if(GetKeyDown(KB_SPACE) || GetKeyDown(KB_C)) {
        if(starting) { // Shooting the ball? 
            starting = false;
            bx_speed = lastMoved * 2;
        }
        else { // or the bullet?
            if( (edgeCtr == 5) && !firing )     // If charged and not firing
            {
                firing = true;
                edgeCtr = 0;
                XYpos p; p.x = 41; p.y = 180;   // Edge UI position
                EraseVRAMArea(&p, 5, 8);        // erase and redraw
                for(u8 e = 0; e < 5; e++) DrawSprite(&edgeGem_0, 41 + e, 180); // the whole section
                DrawSprite(&fire_no, 46, 176);  // and turn off the fire button
                bullet_pos.x = bar_pos.x + 2;
                bullet_pos.y = bar_pos.y - 8;
                //DrawSprite(&bulletspr, bullet_pos.x, bullet_pos.y);
            }
        }
    }
    if(GetKeyDown(KB_ESC)) { 
        SetCursorPos(55, 19);
        print("PAUSED - ESC TO CONTINUE");
        beep(100, 150);
        while(1)
        {
            if(GetKeyDown(KB_ESC)){
                beep(50, 150);
                SetCursorPos(55, 19);
                print("                          ");
                return;
            }
        }
    }
}

///////////////
/// Draw 
///////////////
inline void GAME_DRAW()
{
    Wait_VBLANK();
    // erase
    SETBANK_GREEN();
    EraseVRAMArea(&ball_oldpos, 1, 4);
    EraseVRAMArea(&bullet_oldpos, 2, 8);
    SETBANK_BLUE();
    EraseVRAMArea(&ball_oldpos, 1, 4);
    SETBANK_RED();
    EraseVRAMArea(&bullet_oldpos, 2, 8);
    
    
    if(ballFlash) { // every other frame
        DrawSprite(&ball2, ball_pos.x, ball_pos.y);
    } else { 
        DrawSprite(&ball, ball_pos.x, ball_pos.y);
    }
    
    if(firing) DrawSprite(&bulletspr, bullet_pos.x, bullet_pos.y);

    // paddle
    SETBANK_BLUE();
    EraseVRAMArea(&bar_oldpos, 6, 8);
    DrawSprite(&bar, bar_pos.x, bar_pos.y);
    // back to mainRAM, print score
    SETBANK_MAINRAM()
    SetCursorPos(62, 13);
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
// Highly unoptomized
void AddScore()
{
    pScore ++;
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
    b->x = 0; b->y = 0; b->hits = 0;
    levelMaxScore--;
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


u8 GetBallCollision(s8 xsp, s8 ysp)
{
    // Is the location at ball_pos.x + xsp, ball_pos.y + ysp a COLLIDEABLE?
    // Collideables are in the BLOCK_LEVEL[] array, and
    //  are the ULR borders of the screen. 
    u8 txp; u8 typ;
    txp = ball_pos.x + xsp;
    typ = ball_pos.y + ysp;
    for(u8 i = 0; i < LEVELBLOCKSIZE; i++){
        if(BLOCK_LEVEL[i].hits > 0) {
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
                            if(bl->hits != 99){
                                EraseBlock(bl); // Fix me later
                                SETBANK_MAINRAM();
                                AddScore();
                            }else { 
                                SETBANK_MAINRAM();
                            }
                            beep(100, 10);
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}


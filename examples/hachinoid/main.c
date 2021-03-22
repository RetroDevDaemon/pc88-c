#include <pc88-c.h>

enum directions { UP, DOWN, LEFT, RIGHT };

#include "graphics.h"


enum directions playerDir;

XYpos ball_pos;
XYpos ball_oldpos;
XYpos bar_pos;
XYpos bar_oldpos;
s8 by_speed;
s8 bx_speed;
u8 bar_speed;
bool moved;

typedef struct levelblock { 
    u8 x;
    u8 y;
    u8 hits;
} LevelBlock;

u8 LEVELBLOCKSIZE;
LevelBlock BLOCK_LEVEL[150];
u8 tick;
u8 PLAYER_SPEED;

void DrawSpritePlane(u8* dat, XYpos* xy, u8 w, u8 h);
void DrawSprite(Sprite* spr, signed int x, signed int y);

void DrawRLEBitmap(PlanarBitmap* pb, u16 x, u16 y);
bool GetBallCollision(s8 xsp, s8 ysp);

inline void GAME_INIT();
inline void GAME_DRAW();
inline void GAME_UPDATE();
inline void GAME_INPUT();

void main()
{
    GAME_INIT();
    
    while(1)
    {
        GAME_UPDATE();

        GAME_INPUT();

        GAME_DRAW();
    }
}
/*
void DrawRLEBitmap(PlanarBitmap* pb, u16 x, u16 y)
{
    SETBANK_RED();
    vu8* p = (vu8*)(0xc000) + (y * 80) + x;
    const u8* v = pb->r;
    u8 row = 0;
    u8 col = 0;
    u8 loop = 0;
    u16 siz = pb->w * pb->h;
    for(u16 s = 0; s < siz; s++)
    {
        // check if *v is 0x80
        if(*(v + s) == 0x80){
            // copy *v+1 by *v+2 times
            u8 cp = *(v + s + 1);
            loop = *(v + s + 2);
            for(u8 l = 0; l < loop; l++){
                *(p + row + (col * 80)) = cp;
                row++;
                if(row > pb->w) { row = 0; col++; }
            }
            s += (2 + loop);
        }
        else { 
            *(p + row + (col * 80)) = *(v + s);
            row++;
            if(row > pb->w) { row = 0; col++; }
        }
        /*
        for(u8 yy = 0; yy < pb->h; yy++){
            for(u8 xx = 0; xx < pb->w; xx++){
                *p = *v;
                p++;
                v++;
            }
            p += (80 - pb->w);
        }
        */
/*
    }
}
*/
#define GFX_OFF 0b00010011
#define GFX_ON 0b00011011
    
inline void GAME_INIT()
{
    ClearAttributeRam();
    PLAYER_SPEED = 1; // 1 tick per frame
    bar_speed = 2;
    SetCursorPos(20, 9);
    print("Loading 'HACHINOID' ... PREPARE TO DIE");
    ball_pos.x = 16;
    ball_pos.y = 10;
    bar_pos.x = 8;
    bar_pos.y = 154;
    bx_speed = 1;
    by_speed = 3;
    moved = true;
    // GUI:
    SetIOReg(0x31, GFX_OFF);
    DrawSprite(&tile_01, 0, 0);
    DrawSprite(&tile_01, 47, 0);
    DrawSprite(&tile_01, 0, 168);
    DrawSprite(&tile_01, 47, 168);
    DrawSprite(&tile_02, 2, 0);
    DrawSprite(&tile_02, 2, 168);
    for(u8 i = 0; i < 43; i ++){
        DrawSprite(&tile_03, 3+i, 0);
        DrawSprite(&tile_03, 3+i, 168);
    }
    DrawSprite(&tile_04, 46, 0);
    DrawSprite(&tile_04, 46, 168);
    DrawSprite(&tile_05, 0, 8);
    DrawSprite(&tile_05, 47, 8);
    for(u8 i = 2; i < 20; i++){
        DrawSprite(&tile_06, 0, i * 8);
        DrawSprite(&tile_06, 47, i * 8);
    }
    DrawSprite(&tile_07, 0, 160);
    DrawSprite(&tile_07, 47, 160);
    
    //DrawRLEBitmap(&title_1, 51, 4);
    DrawSprite((Sprite*)&title_1, 51, 4);
    DrawSprite((Sprite*)&leveltxt, 59, 56);
    DrawSprite((Sprite*)&scoretxt, 58, 104);
    
    
    // Level data
    LEVELBLOCKSIZE = 150;
    for(u8 i = 2; i < 9; i++)
    {
        for(u8 j = 3; j < 12; j++) { 
            BLOCK_LEVEL[(i*9)+j].x = 2 + (3 * j);
            BLOCK_LEVEL[(i*9)+j].y = 8 + (i * 8);
            BLOCK_LEVEL[(i*9)+j].hits = 2;
            if(i % 2 == 0) { 
                DrawSprite(&redBlock, 2 + (j * 3), 8 + (i * 8)); 
            }
            if(i % 2 == 1) {
                DrawSprite(&blueBlock, 2 + (j * 3), 8 + (i * 8)); 
            }
        }
    }
    /*
    for(u8 i = 4; i < 13; i++) { 
        DrawSprite(&redBlock, 2 + (i * 3), 24);
        DrawSprite(&greenBlock, 2 + (i * 3), 24+(8*1));
        DrawSprite(&blueBlock, 2 + (i * 3), 24+(8*2));
        DrawSprite(&whiteBlock, 2 + (i * 3), 24+(8*3));
        DrawSprite(&cyanBlock, 2 + (i * 3), 24+(8*4));
        DrawSprite(&yellowBlock, 2 + (i * 3), 24+(8*5));
        DrawSprite(&magentaBlock, 2 + (i * 3), 24+(8*6));
    }
    */
    DrawSprite(&s_0, 62, 80);
    DrawSprite(&s_1, 64, 80);

    // Reset and clear load text
    SetIOReg(0x31, GFX_ON);
    SETBANK_MAINRAM();
    SetCursorPos(20, 9);
    print("                                       ");
    beep(BEEP_C5, 50);
    for(u16 i = 10000; i > 0; i--) { 
        __asm nop __endasm;
    }
    beep(BEEP_C5, 50);
    beep(BEEP_G5, 200);
}

///////////////
/// Update
///////////////

EraseBlock(LevelBlock* b)
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
}

u8 GetCollisionDirection(u8 tx, u8 ty, LevelBlock* b)
{
    // check what side of the block the ball is on.
    // if L/R : flip x 
    // if U/D : flip y 
    // get the quadrant versus the block!
    u8 h = 0b1111;
    if((ball_pos.x + 1) < (b->x + 3)) h ^= 0b1;// NOT right 
    if((ball_pos.y + 4) < (b->y + 8)) h ^= 0b10;// NOT bottom 
    if((ball_pos.x) > (b->x)) h ^= 0b100;// NOT left 
    if((ball_pos.y) > (b->y)) h ^= 0b1000;// NOT top 
    return h;
    // returns bit field of POSSIBLE DIRECTIONS. 

}

inline u8 abs(s8 n)
{
    if(n < 0) return n * -1;
    return n;
}
/*
inline u16 abs(s16 n)
{
    if(n < 0) return n * -1;
    return n;
}
*/

bool GetBallCollision(s8 xsp, s8 ysp)
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
                        u8 d = GetCollisionDirection(txp, typ, &BLOCK_LEVEL[i]);
                        //SetCursorPos(45, 18);
                        if(d == 0b1) { 
                            //print("right   "); 
                            bx_speed *= -1; 
                        } else if(d == 0b10) { 
                            //print("btm     "); 
                            by_speed *= -1; 
                        } else if(d == 0b100) { 
                            //print("left    "); 
                            bx_speed *= -1; 
                        } else if(d == 0b1000) { 
                            //print("top     "); 
                            by_speed *= -1; 
                        } 
                        /*else if(d == 0b0011) {
                            if(abs(xsp) > abs(ysp)){ // hit from right 
                                
                            } else { 
                                
                                // D
                            }
                            //print("btm/rt  ");
                        } else if(d == 0b0110) {
                            if(abs(xsp) > abs(ysp)){ 
                                // hit L/R
                            } else { 
                                // hit U/D
                            }
                            //print("btm/lft ");
                        } else if(d == 0b1001) { 
                            if(abs(xsp) > abs(ysp)){ 
                                // hit L/R
                            } else { 
                                // hit U/D
                            }
                            //print("top/rt  ");
                        } else if(d == 0b1100) {
                            if(abs(xsp) > abs(ysp)){ 
                                // hit L/R
                            } else { 
                                // hit U/D
                            }
                            //print("top/lft ");
                        }*/
                        else { 
                            bx_speed *= -1;
                            by_speed *= -1;
                        }
                        EraseBlock(&BLOCK_LEVEL[i]);
                        beep(100, 10);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

inline void GAME_UPDATE()
{
    tick++;
    if(tick >= PLAYER_SPEED) {
        tick = 0;
        // save for erase
        ball_oldpos.x = ball_pos.x;
        ball_oldpos.y = ball_pos.y;
        // get collision
        if(!GetBallCollision(bx_speed, by_speed)){
            ball_pos.x += bx_speed;
            ball_pos.y += by_speed;
        }
        
        // bounce
        if(ball_pos.x >= 45) {
            bx_speed *= -1;
            beep(600, 5);
        } else if (ball_pos.x <= 2) {
            bx_speed *= -1;
            beep(600, 5);
        }
        if(ball_pos.y >= 160) {
            by_speed *= -1;
            beep(600, 5);
        } else if (ball_pos.y <= 12) {
            by_speed *= -1;
            beep(600, 5);
        }
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
    // Set pen color
    SETBANK_GREEN();
    EraseVRAMArea(&ball_oldpos, 1, 4);
    DrawSprite(&ball, ball_pos.x, ball_pos.y);
    // Player!
    
    SETBANK_BLUE();
    EraseVRAMArea(&bar_oldpos, 6, 6);
    //SETBANK_RED();
    //EraseVRAMArea(&bar_oldpos, 6, 6);
    DrawSprite(&bar, bar_pos.x, bar_pos.y);
    // Write once to 3rd GVRAM plane to balance speed 
    //SetPixel(0, 1, CLR_RED);
    // end VBL
    SETBANK_MAINRAM()
}

//


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
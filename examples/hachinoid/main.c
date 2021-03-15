#include <pc88-c.h>

enum directions { UP, DOWN, LEFT, RIGHT };

#include "graphics.h"


enum directions playerDir;

XYpos ball_pos;
XYpos ball_oldpos;
XYpos bar_pos;
XYpos bar_oldpos;

bool moved;

void DrawSpritePlane(u8* dat, XYpos* xy, u8 w, u8 h);
void DrawSprite(Sprite* spr, signed int x, signed int y);
void EraseVRAMArea(XYpos* xy, u8 w, u8 h);
void DrawRLEBitmap(PlanarBitmap* pb, u16 x, u16 y);

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
    SetCursorPos(20, 9);
    print("Loading 'HACHINOID' ... PREPARE TO DIE");
    ball_pos.x = 16;
    ball_pos.y = 10;
    bar_pos.x = 8;
    bar_pos.y = 154;
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
    // Reset and clear load text
    SetIOReg(0x31, GFX_ON);
    SETBANK_MAINRAM();
    SetCursorPos(20, 9);
    print("                                       ");
}

///////////////
/// Update
///////////////
inline void GAME_UPDATE()
{
    ball_oldpos.x = ball_pos.x;
    ball_oldpos.y = ball_pos.y;
    
    ball_pos.x++;
    ball_pos.y += 4;
    if(ball_pos.x > 45) ball_pos.x = 2;
    if(ball_pos.y > 160) ball_pos.y = 8;

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
            bar_pos.x++;
            moved = true;
            playerDir = RIGHT;
        }
    } else if (GetKeyDown(KB_A) || GetKeyDown(KB_PAD4)){
        if(bar_pos.x > 2){
            bar_pos.x--;
            moved = true;
            playerDir = LEFT;
        }
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
    if(moved){
        SETBANK_BLUE();
        EraseVRAMArea(&bar_oldpos, 6, 6);
        DrawSprite(&bar, bar_pos.x, bar_pos.y);
        moved = false; 
    }
    // end VBL
    SETBANK_MAINRAM()
}

//
void EraseVRAMArea(XYpos* xy, u8 w, u8 h)
{
    u8 xo = xy->x % 8;
    u8 xt = (u8)(xy->x);// / 8);
    vu8* vp = (vu8*)(0xc000 + xt + (xy->y * 80));
    for(u8 z = 0; z < h; z++)
    {
        for(u8 f = 0; f < w; f++)
            *vp++ = 0;
        if(xo != 0) *vp = 0;
        vp += (80 - w);
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
    if(spr->r_data != null) { 
        SETBANK_RED();
        DrawSpritePlane(spr->r_data, &xy, spr->w, spr->h);
    }
    if(spr->g_data != null) {
        SETBANK_GREEN();
        DrawSpritePlane(spr->g_data, &xy, spr->w, spr->h);
    }
    if(spr->b_data != null) {
        SETBANK_BLUE();
        DrawSpritePlane(spr->b_data, &xy, spr->w, spr->h);
    }
}
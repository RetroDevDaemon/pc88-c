#include <pc88-c.h>

#ifndef HACHIGFX
#define HACHIGFX

//
#include "img/ball_sprite.h"
#include "img/bar_sprite.h"

#include "img/block.h"
#include "img/hardblock.h"
#include "img/stoneblock.h"

#include "img/dmg1.h"
#include "img/dmg2.h"
#include "img/dmg3.h"

#include "img/edgepin_empty.h"
#include "img/edgepin_full.h"

#include "img/bullet.h"
#include "img/fire_ok.h"
#include "img/no_fire.h"

#include "img/leveltxt.h"
#include "img/scoretxt.h"
#include "img/edgetxt.h"

#include "img/tile_01.h"
#include "img/tile_02.h"
#include "img/tile_03.h"
#include "img/tile_04.h"
#include "img/tile_05.h"
#include "img/tile_06.h"
#include "img/tile_07.h"
#include "img/tile_08.h"

#include "img/title_1.h"
#include "img/title_2.h"

#include "img/s0.h"
#include "img/s1.h"
#include "img/s2.h"
#include "img/s3.h"
#include "img/s4.h"
#include "img/s5.h"
#include "img/s6.h"
#include "img/s7.h"
#include "img/s8.h"
#include "img/s9.h"
//

// Numbers 
Sprite s_0 = { 
    null, (u8*)s0_g, (u8*)s0_b,
    2, 16
};
Sprite s_1 = { 
    null, (u8*)s1_g, (u8*)s1_b,
    2, 16
};
Sprite s_2 = { 
    null, (u8*)s2_g, (u8*)s2_b,
    2, 16
};
Sprite s_3 = { 
    null, (u8*)s3_g, (u8*)s3_b,
    2, 16
};
Sprite s_4 = { 
    null, (u8*)s4_g, (u8*)s4_b,
    2, 16
};
Sprite s_5 = { 
    null, (u8*)s5_g, (u8*)s5_b,
    2, 16
};
Sprite s_6 = { 
    null, (u8*)s6_g, (u8*)s6_b,
    2, 16
};
Sprite s_7 = { 
    null, (u8*)s7_g, (u8*)s7_b,
    2, 16
};
Sprite s_8 = { 
    null, (u8*)s8_g, (u8*)s8_b,
    2, 16
};
Sprite s_9 = { 
    null, (u8*)s9_g, (u8*)s9_b,
    2, 16
};
//
//Stoneblock
Sprite stoneBlock = { //11
    (u8*)stoneblock_b, (u8*)stoneblock_b, (u8*)stoneblock_b, 
    3, 8
};
// Block B
Sprite hardGreen = { //9
    null, (u8*)hardblock_b, (u8*)hardblock_g,
    3, 8
};
Sprite hardBlue = { //10
    null, (u8*)hardblock_g, (u8*)hardblock_b,
    3, 8
};
Sprite hardRed = { //8
    (u8*)hardblock_b, null, (u8*)hardblock_g, 
    3, 8
};
// Block A
Sprite redBlock = { //1
    (u8*)block_r, (u8*)block_b, (u8*)block_b, 
    3, 8
};
Sprite blueBlock = { //2
    (u8*)block_b, (u8*)block_b, (u8*)block_r, 
    3, 8
};
Sprite greenBlock = { //3
    (u8*)block_b, (u8*)block_r, (u8*)block_b, 
    3, 8
};
Sprite cyanBlock = { //4
    (u8*)block_b, (u8*)block_r, (u8*)block_r, 
    3, 8
};
Sprite magentaBlock = { //5
    (u8*)block_r, (u8*)block_b, (u8*)block_r, 
    3, 8
};
Sprite yellowBlock = { //6
    (u8*)block_r, (u8*)block_r, (u8*)block_b, 
    3, 8
};
Sprite whiteBlock = { //7
    (u8*)block_r, (u8*)block_r, (u8*)block_r, 
    3, 8
};
//

// Sprites
Sprite ball2 = { 
    null, (u8*)&ball_sprite_g, (u8*)&ball_sprite_g, 
    1, 4
};
Sprite ball = { 
    null, (u8*)&ball_sprite_g, null,
    1, 4
};
Sprite bar = { 
    null, null, (u8*)&bar_sprite_b,
    6, 8
};
Sprite block = { 
    (u8*)&block_r, (u8*)&block_b, (u8*)&block_b, 
    3, 8
};
//

// UI
Sprite tile_01 = { 
    (u8*)&tile_01_r, (u8*)&tile_01_g, (u8*)&tile_01_b, 
    2, 8
};
Sprite tile_02 = { 
    (u8*)&tile_02_r, (u8*)&tile_02_g, (u8*)&tile_02_b, 
    1, 8
};
Sprite tile_03 = { 
    (u8*)&tile_03_r, (u8*)&tile_03_g, (u8*)&tile_03_b, 
    1, 8
};
Sprite tile_04 = { 
    (u8*)&tile_04_r, (u8*)&tile_04_g, (u8*)&tile_04_b, 
    1, 8
};
Sprite tile_05 = { 
    (u8*)&tile_05_r, (u8*)&tile_05_g, (u8*)&tile_05_b, 
    2, 8
};
Sprite tile_06 = { 
    (u8*)&tile_06_r, (u8*)&tile_06_g, (u8*)&tile_06_b, 
    2, 8
};
Sprite tile_07 = { 
    (u8*)&tile_07_r, (u8*)&tile_07_g, (u8*)&tile_07_b, 
    2, 8
};
Sprite tile_08 = {
    (u8*)&tile_08_r, null, null, 
    1, 8
};
//

// BMPs
PlanarBitmap title_1 = { 
    (u8*)&title_1_r, (u8*)&title_1_g, (u8*)&title_1_b, 
    28, 38
};
// RLE
PlanarBitmap title_2 = { 
    (u8*)&title_2_r, (u8*)&title_2_g, (u8*)&title_2_b, 
    18, 109
};
//

// Text 
PlanarBitmap leveltxt = { 
    (u8*)&leveltxt_b, (u8*)&leveltxt_b, (u8*)&leveltxt_b, 
    10, 16
};
PlanarBitmap scoretxt = { 
    (u8*)&scoretxt_b, (u8*)&scoretxt_b, (u8*)&scoretxt_b, 
    12, 16
};
PlanarBitmap edgetxt = { 
    (u8*)&edgetxt_b, (u8*)&edgetxt_b, (u8*)&edgetxt_b, 
    8, 16
};

// bullet 
Sprite bulletspr = { 
    (u8*)&bullet_r, (u8*)&bullet_r, null, //yellow
    2, 8
};
// edge GUI
Sprite edgeGem_0 = { 
    null, (u8*)&edgepin_empty_g, null,
    1, 8
};
Sprite edgeGem_1 = { 
    null, (u8*)&edgepin_full_g, null,
    1, 8
};
Sprite fire_yes = { 
    (u8*)fire_ok_r, null, null, 
    2, 16
};
Sprite fire_no = { 
    (u8*)no_fire_r, null, null, 
    2, 16
};
//

Sprite* blockTypes[16];


#endif 
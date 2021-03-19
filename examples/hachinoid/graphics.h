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

#include "img/leveltxt.h"
#include "img/scoretxt.h"

#include "img/tile_01.h"
#include "img/tile_02.h"
#include "img/tile_03.h"
#include "img/tile_04.h"
#include "img/tile_05.h"
#include "img/tile_06.h"
#include "img/tile_07.h"

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

// Block A Colors
Sprite redBlock = { 
    (u8*)block_r, (u8*)block_b, (u8*)block_b, 
    3, 8
};
Sprite blueBlock = { 
    (u8*)block_b, (u8*)block_b, (u8*)block_r, 
    3, 8
};
Sprite greenBlock = { 
    (u8*)block_b, (u8*)block_r, (u8*)block_b, 
    3, 8
};
Sprite cyanBlock = { 
    (u8*)block_b, (u8*)block_r, (u8*)block_r, 
    3, 8
};
Sprite magentaBlock = { 
    (u8*)block_r, (u8*)block_b, (u8*)block_r, 
    3, 8
};
Sprite yellowBlock = { 
    (u8*)block_r, (u8*)block_r, (u8*)block_b, 
    3, 8
};
Sprite whiteBlock = { 
    (u8*)block_r, (u8*)block_r, (u8*)block_r, 
    3, 8
};
//

// Sprites
Sprite ball = { 
    null, (u8*)&ball_sprite_g, null, 
    1, 4
};
Sprite bar = { 
    null, null, (u8*)&bar_sprite_b,
    6, 6
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

// 
PlanarBitmap leveltxt = { 
    (u8*)&leveltxt_b, (u8*)&leveltxt_b, (u8*)&leveltxt_b, 
    10, 16
};
PlanarBitmap scoretxt = { 
    (u8*)&scoretxt_b, (u8*)&scoretxt_b, (u8*)&scoretxt_b, 
    12, 16
};
//

#endif 
#include <pc88-c.h>

#ifndef HACHIGFX
#define HACHIGFX

#include "img/ball_sprite.h"
#include "img/bar_sprite.h"
#include "img/block.h"
#include "img/tile_01.h"
#include "img/tile_02.h"
#include "img/tile_03.h"
#include "img/tile_04.h"
#include "img/tile_05.h"
#include "img/tile_06.h"
#include "img/tile_07.h"
#include "img/title_1.h"
#include "img/title_2.h"

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

// RLE
PlanarBitmap title_1 = { 
    (u8*)&title_1_r, (u8*)&title_1_g, (u8*)&title_1_b, 
    28, 38
};

// RLE
PlanarBitmap title_2 = { 
    (u8*)&title_2_r, (u8*)&title_2_g, (u8*)&title_2_b, 
    18, 109
};

#endif 
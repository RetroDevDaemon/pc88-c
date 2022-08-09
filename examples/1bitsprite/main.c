#include <pc88-c.h>
#include <draw.h>

const unsigned char sprite[] = { 
    0b00011111,0b11100000,0b00000000,
    0b00011110,0b11111000,0b00000000,
    0b01111110,0b00011111,0b11111111,
    0b01111111,0b11111111,0b11110000,
    0b00011000,0b01111111,0b10000000,
    0b00011000,0b11100000,0b00000000,
    0b00111111,0b10000000,0b00000000,
    null
};

enum directions { UP, DOWN, LEFT, RIGHT };
enum directions playerDir;
XYPos spritepos;
XYPos oldpos;
bool moved;

void DrawSprite(XYPos* xy, u8 w, u8 h);

void main()
{
    // First sprite draw 
    spritepos.x = 16;
    spritepos.y = 10;
    SETBANK_BLUE()
    DrawSprite(&spritepos, 3, 7);
    SETBANK_MAINRAM()

    while(1)
    {
        // Update
        oldpos.x = spritepos.x;
        oldpos.y = spritepos.y;
        moved = false;
        
        // Draw
        Wait_VBLANK();

        // Get input and set new sprite position
        if(GetKeyDown(KB_S)){ 
            spritepos.y += 4;
            moved = true;
            playerDir = DOWN;
        } else if (GetKeyDown(KB_W)){
            spritepos.y -= 4;
            moved = true;
            playerDir = UP;
        } else if (GetKeyDown(KB_D)){
            spritepos.x++;
            moved = true;
            playerDir = RIGHT;
        } else if (GetKeyDown(KB_A)){
            spritepos.x--;
            moved = true;
            playerDir = LEFT;
        }

        IRQ_OFF 
        if(moved) { 
            // Set pen color
            SETBANK_BLUE()
            EraseVRAMArea(&oldpos, 3, 7);
            DrawSprite(&spritepos, 3, 7);
            
            SETBANK_MAINRAM()
        }
        IRQ_ON;
    }
}

void DrawSprite(XYPos* xy, u8 w, u8 h)
{   
    u8 xt = (u8)(xy->x);// / 8);
    vu8* vp = (vu8*)(0xc000 + xt + (xy->y * 80));
    for(u8 z = 0; z < h; z++)
    {
        for(u8 f = 0; f < w; f++)
            *vp++ = sprite[(z*w) + f];
        vp += (80-w);
    }
}


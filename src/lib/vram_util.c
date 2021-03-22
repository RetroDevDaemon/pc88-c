#include <pc88-c.h>

void EraseVRAMArea(XYpos* xy, u8 w, u8 h)
{
    //u8 xo = xy->x % 8;
    u8 xt = (u8)(xy->x);
    vu8* vp = (vu8*)(0xc000 + xt + (xy->y * 80));
    for(u8 z = 0; z < h; z++)
    {
        for(u8 f = 0; f < w; f++)
            *vp++ = 0;
        //if(xo != 0) *vp = 0;
        vp += (80 - w);
    }
}
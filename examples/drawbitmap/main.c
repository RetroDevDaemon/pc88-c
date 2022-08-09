
#include <pc88-c.h>
#include <draw.h>

#include "img_b.h"
#include "img_r.h"
#include "img_g.h"

PlanarBitmap layeredImage = { 
    (u8*)&img_r, (u8*)&img_g, (u8*)&img_b, 384/8, 119
};

void main(void)
{   
    IRQ_OFF
    // V1 draw mode , one color plane at a time
    //SetIOReg(0x53, 1);
    PlanarBitmap* pb = &layeredImage;    
    DrawPlanarBitmap(pb, 0, 0);
    //DrawPlaneBMP(pb->r, PLANE_RED, 0, 0, pb->w, pb->h); 
    //DrawPlaneBMP(pb->g, PLANE_GREEN, 0, 0, pb->w, pb->h); 
    //DrawPlaneBMP(pb->b, PLANE_BLUE, 0, 0, pb->w, pb->h);
    SetPixel(360, 180, CLR_BLUE);
    SETBANK_MAINRAM()
    IRQ_ON

    while(1)
    {
        Wait_VBLANK();
    }
}
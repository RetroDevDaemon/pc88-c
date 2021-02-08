
#include "pc88-c.h"

#include "img_b.h"
#include "img_r.h"
#include "img_g.h"

PlanarBitmap layeredImage = { 
    img_r, img_g, img_b, 384/8, 119
};

void main()
{   
    IRQ_OFF
    // V1 draw mode , one color plane at a time
    PlanarBitmap* pb = &layeredImage;    
    DrawPlanarBitmap(pb, 0, 0);
    SetPixel(360, 180, CLR_BLUE);
    SETBANK_MAINRAM()
    IRQ_ON

    while(1)
    {
        Wait_VBLANK();
    }
}
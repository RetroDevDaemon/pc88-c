/*! \addtogroup draw 
 * @{
 */
#include <pc88-c.h>

void DrawPlaneBMP(const u8* img, u8 plane, u16 x, u16 y, u8 w, u8 h)
{
    if(plane == PLANE_RED)
        SETBANK_RED()
    else if(plane == PLANE_BLUE)
        SETBANK_BLUE()
    else 
        SETBANK_GREEN()
    vu8* p = (vu8*)(0xc000) + (y * 80) + (u8)(x/8);
    const u8* v = img;
    for(u8 yy = 0; yy < h; yy++){
        for(u8 xx = 0; xx < w; xx++){
            *p = *v;
            p++;
            v++;
        }
        p += (80 - w);
    } 
}


void SetPixel(u16 x, u8 y, u8 c)
{
    u8 po = 7 - ((x & 0xff) % 8);
    vu8* p = (vu8*)(0xc000) + (y * 80) + (u8)(x / 8);
        
    if(c & 0b001){
        SETBANK_BLUE()
        *p |= bit(po);
    }
    if(c & 0b010){
        SETBANK_RED()
        *p |= bit(po);
    }
    if(c & 0b100){
        SETBANK_GREEN()
        *p |= bit(po);
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
inline void EnableALU(){ SetIOReg(ALU_MODE_CTRL, 0xC9); }
inline void DisableALU(){ SetIOReg(ALU_MODE_CTRL, 0x89); }
inline void ExpandedGVRAM_On() { SetIOReg(EXPANDED_GVRAM_CTRL, 0x80); }
inline void ExpandedGVRAM_Off() { SetIOReg(EXPANDED_GVRAM_CTRL, 0); } 

/*! @} */
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

inline void EnableALU(){ SetIOReg(ALU_MODE_CTRL, 0xC9); }
inline void DisableALU(){ SetIOReg(ALU_MODE_CTRL, 0x89); }
inline void ExpandedGVRAM_On() { SetIOReg(EXPANDED_GVRAM_CTRL, 0x80); }
inline void ExpandedGVRAM_Off() { SetIOReg(EXPANDED_GVRAM_CTRL, 0); } 

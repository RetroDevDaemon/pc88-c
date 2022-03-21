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

inline void EnableALU(u8 fastmem)
{ 
    SetIOReg(ALU_MODE_CTRL, 0xC9|(fastmem<<4)); 
}
//11001001
inline void DisableALU(u8 fastmem)
{ 
    SetIOReg(ALU_MODE_CTRL, 0x89|(fastmem<<4)); 
}
//11001001

inline void ExpandedGVRAM_On() 
{ 
    SetIOReg(EXPANDED_GVRAM_CTRL, 0x80); 
}
inline void ExpandedGVRAM_Off() 
{ 
    SetIOReg(EXPANDED_GVRAM_CTRL, 0); 
} 


inline void CRT_OFF() {
    __asm
        ld			a,#0b00011001
	    out			(0x40),a
    __endasm;
    Wait_VBLANK();
    __asm 
        xor a,a
        out (0x51),a 
    __endasm;
}

inline void CRT_ON() {
    Wait_VBLANK();
    __asm
        ld			a,#0b00010001
	    out			(0x40),a
    __endasm;
    __asm 
        ld a,#0x20 
        out (0x51),a 
    __endasm;
    
}


void SetMonitor(u8 khz, u8 rows)
{

    CRT_OFF();
    //パラメータ1	W	C/B	H6	H5	H4	H3	H2	H1	H0
    __asm 
        ld a,#((1 << 7) | 78)
        out (0x50),a
    __endasm;
    
    //パラメータ2	W	B1	B0	L5	L4	L3	L2	L1	L0
   // パラメータ3	W	S	C1	C0	R4	R3	R2	R1	R0
    //パラメータ4	W	V2	V1	V0	Z4	Z3	Z2	Z1	Z0
    if(khz == 24)
    {

        if(rows == 20)
        {
            __asm 
                ld a,#((2 << 6) | 19) // line #
                out (0x50),a
                ld a,#((2 << 5) | 19) //lines per char 
                out (0x50),a
                ld a,#((1 << 5) | 25) //vbl interval, hbl interval
                out (0x50),a
            __endasm;
        }
        else { // 25 rows 
                // b is 2
                // c is 24
            __asm
                ld a,#((2 << 6) | 24)
                out (0x50),a
                ld a,#((2 << 5) | 15)
                out (0x50),a
                ld a,#((2 << 5) | 24)
                out (0x50),a
            __endasm;
        }
    }
    else // khz 15
    {
    //25 R: 7 for 15khz, 15 for 24khz
    //20 R: 9 for 15khz, 19 for 24khz
    
    //25 V: 6 for 15khz, 2 for 24khz
    //20 V: 5 for 15khz, 1 for 24khz
    //Z 15khz: 31, 24khz: 25

        if(rows==20)
        {
            __asm
                ld a,#((2 << 6) | 19)
                out (0x50),a
                ld a,#((2 << 5) | 9)
                out (0x50),a
                ld a,#((5 << 5) | 31)
                out (0x50),a
            __endasm;
        }else{ // 15khz 25 rows
            __asm
                ld a,#((2 << 6) | 24)
                out (0x50),a
                ld a,#((2 << 5) | 7)
                out (0x50),a
                ld a,#((6 << 5) | 31)
                out (0x50),a
            __endasm;
        }
    }
    
    __asm 
        ld a,#((0b010 << 5) | 19)
        out (0x50),a 
        ld a,#0x41
        out (0x51),a
    __endasm;
    
    CRT_ON();

}


void DrawTransparentImage_V2(u8 x, u8 y, u8* img, u8 w, u8 h)
{
    u8 tran = img[0] >> 4; //first pixel is always transparent
    u8 lastColor = 99;
    vu8* dst = (vu8*)(0xc000 + (y*80) + x); // start at x,y
    while(h > 0)    // at the top
    {
        s8 bytePxLoc = 7;   // left most pixel
        u8 uw = w;          // current width loop
        while(uw > 0)
        {
            u8 p = *img++;  // get the image byte, 4bpp
            
            if (p != (tran << 4)|tran)
            {            

                u8 p2 = p & 0xf;
                p = p >> 4;
                
                if(lastColor != p)
                {
                    SetIOReg(EXPANDED_ALU_CTRL, p);
                    //lastColor = p;
                } 
                    
                if(p != p2) // different colors
                {
                    if(p != tran)
                        *dst = (1 << bytePxLoc);
                    bytePxLoc--;
                    if(p2 != tran){
                        SetIOReg(EXPANDED_ALU_CTRL, p2);
                        *dst = (1 << bytePxLoc);
                    }
                    bytePxLoc--;
                    lastColor = p2;
                }
                else 
                {   // the next two pixels are the same
                    bytePxLoc--;    
                    if(p != tran)
                        *dst = (0b11 << bytePxLoc);
                    bytePxLoc--;
                    lastColor = p;
                }
            }
            else 
                bytePxLoc -= 2;
            
            if(bytePxLoc < 0){   // reset pixel bit loc
                bytePxLoc = 7;
                dst++;
                uw--;               // decrement width
            }
        }

        h--;        //decrement height
        dst += (80-w);  // go to next pixel row
        
    }
}

void DrawImage_V2(u8 x, u8 y, u8* img, u8 w, u8 h)
{
    u8 lastColor = 99;
    vu8* dst = (vu8*)(0xc000 + (y*80) + x); // start at x,y
    while(h > 0)    // at the top
    {
        s8 bytePxLoc = 7;   // left most pixel
        u8 uw = w;          // current width loop
        while(uw > 0)
        {
            u8 p = *img++;  // get the image byte, 4bpp
            
            if (p > 0)
            {            

                u8 p2 = p & 0xf;
                p = p >> 4;
                
                if(lastColor != p)
                {
                    SetIOReg(EXPANDED_ALU_CTRL, p);
                    //lastColor = p;
                } 
                    
                if(p != p2) // different colors
                {
                    *dst = (1 << bytePxLoc);
                    bytePxLoc--;
                    SetIOReg(EXPANDED_ALU_CTRL, p2);
                    *dst = (1 << bytePxLoc);
                    bytePxLoc--;
                    lastColor = p2;
                }
                else 
                {   // the next two pixels are the same
                    bytePxLoc--;    
                    *dst = (0b11 << bytePxLoc);
                    bytePxLoc--;
                    lastColor = p;
                }
            }
            else 
                bytePxLoc -= 2;
            
            if(bytePxLoc < 0){   // reset pixel bit loc
                bytePxLoc = 7;
                dst++;
                uw--;               // decrement width
            }
        }

        h--;        //decrement height
        dst += (80-w);  // go to next pixel row
        
    }
}


/*! @} */
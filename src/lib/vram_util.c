#include <pc88-c.h>

/*! \addtogroup Graphics
* @{ 
*/

void EraseVRAMArea(XYPos* xy, u8 w, u8 h)
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


#define ALUCPY ld	a,(hl) \
        ld	(de),a \
        inc hl \
        inc de 

u16 local_a;
u16 local_b;
u8 local_x;
u16 local_h;

// Specifically to copy 8 bytes (64 pixels) from vram to vram.
// Unrolled for speed. FAST!
void ALUCopy(vu8* src, vu8* dst, u8 w, u16 h)
{
    local_a = (u16)src;
    local_b = (u16)dst;
    local_h = h;
    local_x = 81-w;
    __asm 
        ld	bc,(_local_h)
        ld	hl,(_local_a)
        ld	de,(_local_b)
    00104$:
        push	bc
    
         ALUCPY 
         ALUCPY 
         ALUCPY 
         ALUCPY 
         ALUCPY 
         ALUCPY 
         ALUCPY         
         ld	a,(hl)
         ld	(de),a
         ld b,#0
         ld	a,(_local_x)
         ld c,a 

         add	hl,bc
          push hl  ; new src>stack
           push de ; de  
           pop hl  ; to hl 
           add hl,bc 
           push hl ; new de>stack
           pop de  ; back to de 
          pop hl  ; back to hl 
        pop	bc   ; counter back to bc
        dec	bc
        ld	a,c
        or	b    ; check if 0
        ; cp a,#0
        jr nz,00104$
    __endasm;
}


void ALUCopyOut(vu8* src, vu8* dst, u8 w, u16 h)
{
    local_a = (u16)src;
    local_h = h;
    local_b = (u16)dst;
    local_x = 81-w;
    __asm 
        ld	bc,(_local_h)
        ld	hl,(_local_a)
        ld	de,(_local_b)
    00109$:
        push	bc
    
         ALUCPY 
         ALUCPY 
         ALUCPY 
         ld	a,(hl)
         ld	(de),a
         ld	b,#0 
         ld a,(_local_x)
         ld c,a

         add	hl,bc ; move to next pixel row 
        pop	bc   ; counter back to bc
        dec	bc
        ld	a,c
        or	b    ; check if 0
        ; cp a,#0
        jr nz,00109$
    __endasm;
}

void ALUCopyIn(vu8* src, vu8* dst, u8 w, u16 h)
{
    local_b = (u16)dst;
    local_h = h;
    local_x = 81-w;
    local_a = (u16)src;
    __asm 
        ld	bc,(_local_h)
        ld	hl,(_local_a)
        ld	de,(_local_b)
    00108$:
        push	bc
    
         ALUCPY 
         ALUCPY 
         ALUCPY 
         ld	a,(hl)
         ld	(de),a
         ld	b,#0
         ld a,(_local_x)
         ld c,a

         push hl
          push de 
          pop hl 
          add hl,bc 
          push hl 
          pop de 
        pop hl 
        pop	bc   ; counter back to bc
        dec	bc
        ld	a,c
        or	b    ; check if 0
        ; cp a,#0
        jr nz,00108$
    __endasm;
}

inline void SetCRTC_IRQ(void* func)
{
    local_a = (u16)func;
    __asm 
        ld hl,(_local_a) ; _Vblank
        ld (CRTC_IRQ),hl
    __endasm;
}

/*! @} */
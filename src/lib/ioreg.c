/*! \addtogroup IORegs
* @{
*/

#include <pc88-c.h>

u8 ReadIOReg(u8 r) __naked
{   // Template copied from generated SDCC code and "optimized"
    r;
    __asm 
        ld	iy, #2
        add	iy, sp
        ld a, 0 (iy)
        ld c, a 
        in a, (c)
        ld l, a
        ret 
    __endasm;
}



void SetIOReg(u8 r, u8 v) __naked 
{   
    r; v; 
    __asm
        push ix 
        ld ix,#0
        add	ix,sp
    //; 4 (ix) and 5 (ix) contain r and v 
        ld c, 4 (ix)
        ld a, 5 (ix) 
        out (c), a 
        pop	ix
        ret
    __endasm;
}

void SetOPNReg(u8 r, u8 v) __naked 
{
        r; v; 
    __asm
        push ix 
        ld ix,#0
        add	ix,sp
    //; 4 (ix) and 5 (ix) contain r and v 
        ld c, 4 (ix)
        ld b, 5 (ix) 
    _OPNWait:    
        in a, (0x44)
        rlca 
        jr c, _OPNWait
        ld a, c
        out (0x44), a
        ld a, 0 (ix)
        ld a, b
        out (0x45), a 

        pop	ix
        ret
    __endasm;
}

/*! @} */
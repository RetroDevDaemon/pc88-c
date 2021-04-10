#include <pc88-c.h>
//#include <math.h>


u8 abs(s8 n)
{
    if(n < 0) return n * -1;
    return n;
}

u16 abs16(s16 n)
{
    if(n < 0) return n * -1;
    return n;
}

// To use this as a signed byte
//  just cast with (s8)
u8 rand() __naked
{
    __asm 
    //;-----> Generate a random number
    //; output a=answer 0<=a<=255
    random:
        ld      hl,(_RANDOMSEED)
        ld      a,r
        ld      d,a
        ld      e,(hl)
        add     hl,de
        add     a,l
        xor     h
        ld      (_RANDOMSEED),hl
        ld      l,a
        ret 
    __endasm;
}

u16 rand16()
{
    return rand() << 8 | rand();
}

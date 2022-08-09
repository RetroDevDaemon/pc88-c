/*!
 *  \addtogroup Beep
 *  @{
 */

#include <pc88-c.h>

void beep(u8 length, u16 tone) __naked 
{
    length; tone;
    // A      DE 
    __asm 
        ld b, a 
    BeepLoop:
        push de 
        pop hl  ;ld hl, de
        ld a, #0x80 
        out (0x40),a 
    .wt1:
        dec hl          ; decrement length
        ld a, l 
        or h            ; if n zero
        jr nz, .wt1     ; wait 
        ; inc hl 
        ld a, #0 
        push de     ; ld hl, de 
        pop hl 
        out (0x40), a 
    .wt2:
        dec hl 
        ld a, l 
        or h 
        jr nz, .wt2 
        djnz BeepLoop   ; dec b 
        ret             ; if 0, ret 
    __endasm;
}
/*
void beep(u16 tone, u8 length) __naked // TESTME SDCC
{
    //; tone in HL 
    //; length on stack 

    tone; length;
    __asm
        ld      hl,#0
        add      hl,sp
        push    hl 
        ld	    iy, #0
        add	    iy, sp          // push stack pointer back up 2 bytes
        ld      b, 2 (iy)       // length
    Beeperloop: 
        ;ld      l, 0 (iy)
        ;ld      h, 1 (iy)
        ; hl already has tone 
        ld      a,#0x80         // mk2 
        out     (0x40),a 
    .weet1:
        dec     hl 
        ld      a,l 
        or      h 
        jr      nz,.weet1 
        inc     hl 
        ld      a,#0 
        ld      l, 0 (iy)
        ld      h, 1 (iy)
        ld      a,#0 
        out     (0x40),a 
    .weet2:
        dec     hl 
        ld      a,l 
        or      h 
        jp      nz,.weet2 
        djnz    Beeperloop      // dec b and j if nz
        ;inc     iy 
        ;inc     iy              // push SP back up to original position (+4b)
        pop     hl 
        ld      sp,hl 
        ret 
    __endasm;
}
*/
/*! @} End of Doxygen Groups*/

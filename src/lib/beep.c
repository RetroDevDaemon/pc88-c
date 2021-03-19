#include <pc88-c.h>

void beep(u16 tone, u8 length) __naked
{
    tone; length;
    __asm
    	ld	    iy, #2
	    add	    iy, sp          // push stack pointer back up 2 bytes
        ld      b, 2 (iy)       // length
    Beeperloop: 
        ld      l, 0 (iy)
        ld      h, 1 (iy)
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
        djnz    Beeperloop        // dec bc and j if nz
        inc     iy 
        inc     iy 
        ret 
    __endasm;
}
#include <pc88-c.h>

/*! \addtogroup Graphics
* @{
*/

void Wait_VBLANK() __naked 
{
    /* Taken from PC-88 info page:
     http://mydocuments.g2.xrea.com/html/p8/vraminfo.html 
     */

    __asm
    WaitVBlank:
    in	    a,(0x40)        ; SYS_MODE_SENSE
    and     #0x20           ; VBLANK_SIGNAL
    jr      nz,WaitVBlank   ; bit 5 set is vblank interval 
    .wait:
    in      a,(0x40)
    and     #0x20
    jr      z,.wait         ; wait until the signal is off again to be safe!
    ret
    __endasm;
}

/*! @} */
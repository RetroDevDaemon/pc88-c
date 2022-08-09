/*! \addtogroup System
 * @{ 
*/

#ifndef CRT0
#define CRT0
    
#include <pc88-c.h>

// globals 
vu8* SCREEN_POINTER;
u8 LINE_POINTER;

u16 RANDOMSEED;

void __init()
{
    SCREEN_POINTER = (vu8*)SCREEN_TXT_BASE;
    LINE_POINTER = (u8)0;
    main();
}

void Wait_VBLANK() __naked 
{
    __asm
    WaitVBlank0:
        in a,(0x40)        ; SYS_MODE_SENSE
        and #0x20           ; VBLANK_SIGNAL
        jr nz, WaitVBlank0   ; bit 5 set is vblank interval 
    .wait0:
        in a,(0x40)
        and #0x20
        jr z, .wait0         ; wait until the signal is off again to be safe
        ret
    __endasm;
}

#endif 

/*! @} */

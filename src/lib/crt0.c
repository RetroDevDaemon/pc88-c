#ifndef CRT0
#define CRT0
    
#include <pc88-c.h>

// globals 
extern vu8* SCREEN_POINTER;
extern u8 LINE_POINTER;

u16 RANDOMSEED;

void __init()
{
	__asm
	    ld sp,#0xe000
	__endasm;
    SCREEN_POINTER = (vu8*)SCREEN_TXT_BASE;
    LINE_POINTER = (u8)0;
    main();
}


#endif 
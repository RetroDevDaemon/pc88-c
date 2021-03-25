    
#include <pc88-c.h>
#ifndef CRT0
#define CRT0
// globals 
vu8* SCREEN_POINTER;
u8 LINE_POINTER;

void __init()
{
    SCREEN_POINTER = (vu8*)SCREEN_TXT_BASE;
    LINE_POINTER = (u8)0;
    main();
}


void SetCursorPos(u8 x, u8 y)
{
    SCREEN_POINTER = (vu8*)(SCREEN_TXT_BASE + (u16)(120 * y) + x);
    LINE_POINTER = x;
}

void SetCursorPos40(u8 x, u8 y)
{
    SCREEN_POINTER = (vu8*)(SCREEN_TXT_BASE + (u16)(120 * y) + (x * 2));
    LINE_POINTER = x * 2;
}

inline void putchr(u8 c)
{
    *SCREEN_POINTER = c;
    SCREEN_POINTER++;
    LINE_POINTER++;
    if(LINE_POINTER == 80) { 
        LINE_POINTER = 0;
        SCREEN_POINTER = SCREEN_POINTER + 40;
    }
}

inline void putchr40(u8 c)
{
    *SCREEN_POINTER = c;
    SCREEN_POINTER +=2 ;
    LINE_POINTER++;
    if(LINE_POINTER == 80) { 
        LINE_POINTER = 0;
        SCREEN_POINTER = SCREEN_POINTER + 40;
    }
}

void print(String str)
{
    u8 i = 0;
    while(str[i] != '\x00')
        putchr(str[i++]);
}

void print40(String str)
{
    u8 i = 0;
    while(str[i] != '\x00')
        putchr40(str[i++]);
}

#endif 
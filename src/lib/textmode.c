#include <pc88-c.h>
#include "lib/textmode.h"

u8 TABULATE;
vu8* SCREEN_POINTER;
u8 LINE_POINTER;

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
    TABULATE = LINE_POINTER;
    while(str[i] != '\x00') {
        if(str[i] == '\n') { 
            i++; 
            SCREEN_POINTER += (120 - (((u16)SCREEN_POINTER - SCREEN_TXT_BASE) % 120)) + TABULATE;
            LINE_POINTER = TABULATE;
        }
        else putchr(str[i++]);
    }
}

void print40(String str)
{
    u8 i = 0;
    TABULATE = LINE_POINTER;
    while(str[i] != '\x00') {
        if(str[i] == '\n') { 
            i++; 
            SCREEN_POINTER += (120 - (((u16)SCREEN_POINTER - SCREEN_TXT_BASE) % 120)) + TABULATE;
            LINE_POINTER = TABULATE;
        }
        else putchr(str[i++]);
    }
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
void ClearAttributeRam()
{
    vu8* addr = (vu8*)(SCREEN_ATTR_BASE);
    u16 p = 0xe880;
    for(u8 y = 0; y < 24; y++){
        for(u8 x = 0; x < 20; x++){
            *addr = p;
            addr += 2;   
        }
        addr += 80;
    }
}

void SetTextAttribute(u8 x, u8 y, u8 attr)
{
    vu8* addr = (vu8*)(SCREEN_ATTR_BASE + (y * 120));
    while(*addr != 0x80)
        addr += 2;

    *addr = x; 
    addr++;
    *addr = attr;
}

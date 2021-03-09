#include <pc88-c.h>


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


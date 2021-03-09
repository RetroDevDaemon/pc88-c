#include <pc88-c.h>

bool GetKeyDown(u8 SCANCODE)
{
    u8 r = (SCANCODE & 0xf0) >> 4;
    u8 b = SCANCODE & 0x0f;
    u8 i = ReadIOReg(r);
    if(! (i & bit(b)) ) return true;
    return false;
}

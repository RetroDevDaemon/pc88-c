#include <pc88-c.h>

u8 abs(s8 n)
{
    if(n < 0) return n * -1;
    return n;
}
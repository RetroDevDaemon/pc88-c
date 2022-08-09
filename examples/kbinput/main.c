#include <pc88-c.h>
#include <textmode.h>

void main()
{
    print("Press RETURN or SPACE.");
    while(1)
    {
        Wait_VBLANK();
        if(GetKeyDown(KB_RETURN)) print("HI!\x00");
        if(GetKeyDown(KB_SPACE)) print("BYTE!\x00");
    }
}


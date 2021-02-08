#include "../../src/pc88-c.h"

String strdat = "H e l l o   W o r l d \x00";

void main()
{
    print(strdat);
    while(1)
    {
        Wait_VBLANK();
    }
}
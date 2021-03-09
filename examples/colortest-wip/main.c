#include <pc88-c.h>

void main()
{   
    IRQ_OFF
    
    IRQ_ON

    while(1)
    {
        Wait_VBLANK();
    }
}
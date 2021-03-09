#include <pc88-c.h>

void main()
{
    IRQ_OFF 
    
    // Load 1 sector from T/R 0/1, drive 0 to 0xC100
    DiskLoad((u8*)0xc100, 0, 1, 1, 0);
    print("IPL copied into RAM again @ c100!");

    IRQ_ON 
    
    while(1)
    { }
}


#include <pc88-c.h>

#include "lum.h"

void main()
{
	IRQ_OFF;

	ExpandedGVRAM_On();
    EnableALU();

	// Start by loading the bitmap and checking opcode #1
	u8* bptr = (u8*)&lum[0];
	u8 to_c = 0;
	u8 num = 0;
	if(*bptr == 0xfd) // copy next byte byte+2 times
	{
		bptr++;
		to_c = *bptr++;
		num = *bptr++;
	}
	u8 i;
	u8* vr = (u8*)0xc000;
	for(i = 0; i < num; i++)
	{
		*vr++ = (u8)((to_c & 0xf0) >> 4);
		
	}

	DisableALU();           // don't forget!
    ExpandedGVRAM_Off();    // required after V2 draw
    
    SETBANK_MAINRAM()
    IRQ_ON;
	while(1){}
}
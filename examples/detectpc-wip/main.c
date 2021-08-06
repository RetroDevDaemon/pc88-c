#include <pc88-c.h>

inline void SetIRQs()
{
	// Level 2 is for VBL only (no 1/600)
    SetIOReg(IRQ_LEVEL_SET, 2);     
    SetIOReg(IRQ_MASK, 2);        	
}

u8 status;

void byToHex(u8 by, u8* res)
{
    u8 a = by & 0xf;
    u8 b = (by & 0xf0) >> 4;
    if (a > 9) a += 7;
    if (b > 9) b += 7;
    a += 0x30;
    b += 0x30;
    res[0] = b; res[1] = a; res[2] = 0; 
}

void Vblank() __critical __interrupt
{
	IRQ_OFF 

	u8 dat[3];
	byToHex(status, dat);
	SetCursorPos(0,0);
	print("Status: ");
	print(dat);

	SetIRQs();
	IRQ_ON 
}

void main()
{
	IRQ_OFF 

	// Taken from Bookworm's Library:
	/*
	__asm 
		ld a,#0b00111001
		out (0x31),a 
	__endasm;
	status = *(u8*)(0x79d7);
	
	SetIOReg(0x31, 0b00011011); // gimme back the RAM
	*/
	__asm 
        ld hl, #_Vblank
        ld (CRTC_IRQ),hl
	__endasm;
	SetIRQs(); 		// Call once before we start
	
	IRQ_ON 
	
	while(1) 
	{

	}
}
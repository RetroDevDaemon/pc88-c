//
#include <pc88-c.h>
#include <draw.h>
#include <textmode.h>
//

//
void Vblank() __critical __interrupt;
inline void SetVBLIRQ();

//
inline void SetVBLIRQ()
{
    SetIOReg(IRQ_LEVEL_SET, 2);      // Set IRQ high for VBL
    SetIOReg(IRQ_MASK, 0b10);        // Reset mask for VBL
}

void Vblank() __critical __interrupt
{
    IRQ_OFF         // Disable interrupts during VBL
    //print("K"); // <= debug
    SetVBLIRQ();    // Reset interrupt level and mask
    IRQ_ON
}

//
void main()
{
    IRQ_OFF 
    __asm 
        ld hl, #_Vblank
        ld (CRTC_IRQ),hl
    __endasm;
    SetVBLIRQ();        // And enable the VBL interrupt!
    // Rest of init code:
    // ALU on
    ExpandedGVRAM_On();
    EnableALU(true);
    // Draw rectanglez
    
    // Fill w colors
    // left row
    u8 i;
    for(i = 0; i < 8; i++)
    {
	    SetIOReg(EXPANDED_ALU_CTRL, i);
	    u8 y;
	    for(y = (i * 22); y < (i * 22) + 16; y++)
	    {
	    	u8 x;
	    	for(x = 0; x < 4; x++)
	    	{
		    	vu8* vp = (vu8*)((7+y)*80) + 2 + 0xc000 + x;
		    	*vp = 0xff;	
	    	}	
	    } 
    }

    // ROWS:
    
    for(i = 0; i < 8; i++)
    {
    	SetIOReg(EXPANDED_ALU_CTRL, i);
    	u8 y = 8;
        for(y = 8; y < 24; y+=2){
            for(u8 x = 16; x < 70; x++)
            {
                vu8* vp = GetVRAMPos(x, y + i*(22));//(vu8*)((7+y)*80) + 10 + 0xc000 + x;
                *vp = 0b01010101;
                vp += 80;
                *vp = 0b10101010;
                if((x) % 7 == 0) x += 2;
            }       
        }
    }
    for(i = 0; i < 8; i++){
        SetIOReg(EXPANDED_ALU_CTRL, i+1);
        for(u8 y = 22; y < (7*22)+16; y+=2)
        {
            for(u8 x = 0; x < 5; x++)
            {
                vu8* vp = GetVRAMPos(x+24+(7*i), y+8);
                *vp = 0b10101010;
                vp += 80;
                *vp = 0b01010101; 
            }
            if((y+8)%22 == 0) y += 6;
        }
    }
    // Done:
    DisableALU(FASTMEM_OFF);           // don't forget!
    ExpandedGVRAM_Off();    // required after V2 draw
    SETBANK_MAINRAM()
    SetCursorPos(15, 1);
    print("PC-8801 CRT Dither Test [NEC PC-KD852]");
    //
    IRQ_ON 
    while(1){ }
}
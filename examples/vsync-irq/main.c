#include <pc88-c.h>

void Vblank() __critical __interrupt;
inline void SetVBLIRQ();

inline void SetVBLIRQ()
{
    SetIOReg(IRQ_LEVEL_SET, 2);      // Set IRQ high for VBL
    SetIOReg(IRQ_MASK, 0b10);        // Reset mask for VBL
}

void Vblank() __critical __interrupt
{
    IRQ_OFF         // Disable interrupts during VBL
    SetVBLIRQ();    // Reset interrupt level and mask
    // Do our Vblank code:
    print("Hi!");
    
    IRQ_ON
}

void main()
{
    // First, Write the address of the Vblank routine to the CRTC IRQ vector @ f302
    IRQ_OFF 
    __asm 
        ld hl, #_Vblank
        ld (CRTC_IRQ),hl
    __endasm;
    SetVBLIRQ();        // And enable the VBL interrupt!
    IRQ_ON 

    while(1){ }
}
#include <pc88-c.h>


void Vblank() __critical __interrupt;
void ClockInterrupt() __critical __interrupt;

u8 ctr;

inline void SetIRQs();

inline void SetIRQs()
{
    SetIOReg(IRQ_LEVEL_SET, 3);      // Set IRQ to >2 for VBL+clock R E4
    SetIOReg(IRQ_MASK, 0b11);        // Reset mask for vbl+clock   R E6
}

void Vblank() __critical __interrupt
{
    IRQ_OFF 
    print("Hi!");
    
    SetIRQs(); 
    IRQ_ON
}

void ClockInterrupt() __critical __interrupt
{
    IRQ_OFF 
    
    print(".");
    
    SetIRQs();
    IRQ_ON
}

void main()
{

    ctr = 0;
    // First, Write the address of the Vblank routine to the CRTC IRQ vector @ f302
    IRQ_OFF 
    __asm 
        ld hl, #_Vblank
        ld (CRTC_IRQ),hl
        ld hl, #_ClockInterrupt
        ld (CLOCK_IRQ),hl
    __endasm;
    SetIRQs();        // And enable the VBL interrupt!
    IRQ_ON 

    while(1){ }
}
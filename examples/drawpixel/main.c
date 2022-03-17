#include <pc88-c.h>

void main()
{
    IRQ_OFF
    
    // ALU on
    ExpandedGVRAM_On();                 // Expanded mode GVRAM on, comp data off 
    EnableALU(true);                        // ALU on - must be performed AFTER GVRAM on!
    
    // ALU draw (V2) - CYAN
    SetIOReg(EXPANDED_ALU_CTRL, CLR_CYAN); 
    vu8* vp = (vu8*)0xc100;
    *vp = 0xff;
    
    // V2 ALU vram copy mode
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | bit(4) ) ); // copy mode bit enable
    __asm
      ld a,(0xC100)     ; 0xff
      ld (0xC102),a     ; same color copy
    __endasm;
    
    // Comp read/write mode - comp false
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | CLR_RED ) );  // comp data- is red?
    *(vp + 3) = *(vp + 2);  // c103 = will write 0
    
    // Comp read/write mode - comp true
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | CLR_CYAN ) ); // comp data - is cyan?
    *(vp + 4) = *(vp + 2);  // c104 = c102 - will write 0xff in cyan

    DisableALU(FASTMEM_OFF);           // don't forget!
    ExpandedGVRAM_Off();    // required after V2 draw

    SetPixel(200, 100, CLR_WHITE); // single pixel - V1 mode
    SETBANK_MAINRAM()   // required after V1 draw
    
    IRQ_ON
    
    while(1){ }
}
#include <pc88-c.h>

#include "song_b.h"
#include "sprites.h"

inline void SetIRQs()
{
    SetIOReg(IRQ_LEVEL_SET, 2);
    SetIOReg(IRQ_MASK, 2);
}

extern bool playingSong;
extern struct Song currentSong;
extern signed int ticker;

u32 idleCount;
u8 ctr;


void main()
{
    
    IRQ_OFF; 
    ctr = 0;
    idleCount = 0;
    u8 i = 0;
    playingSong = false;
    
    // Set up currentSong pointers
    LoadSong(&song[0]);
    playingSong = true;
    ticker = 0;

    // First, Write the address of the Vblank routine to the CRTC IRQ vector @ f302
    __asm 
        ld hl, #_Vblank
        ld (CRTC_IRQ),hl
    __endasm;

    // Use this entire sequence to change CRT Rows
    Wait_VBLANK();
    vu8* crtreg = (vu8*)0x40;
    if(*crtreg & 0b10) SetMonitor(15, 25);
    else SetMonitor(24, 25);

    SetCursorPos(15,20);
    print("Loading...");
    
    // ALU drawing here
    ExpandedGVRAM_On();     
    EnableALU(FASTMEM_ON);            
    // Grid test:
    for(u8 gridy = 0; gridy < 4; gridy++)
        for(u8 gridx = 0; gridx < 8; gridx++)
        {
            DrawImage_V2(1 + gridx*8, 10+(32*gridy), &hexa[0], 64/8, 24);
            DrawImage_V2(1 + 4 + gridx*8, 10+16+(32*gridy), &hexa[0], 64/8, 24);
        }
    ;
    // Hero:
    DrawTransparentImage_V2(11, 42 + 20, &librarianSprite[0], 32/8, 24);
    // ALU off
    DisableALU(FASTMEM_OFF);           
    ExpandedGVRAM_Off();   

    // disalbe load text
    SetCursorPos(15,20);
    print("             ");

    SetIRQs();        // And enable the VBL interrupt!
    
    IRQ_ON 
    while(1)
    { 
        // Main Loop
        idleCount++;
    }
}


void Vblank() __critical __interrupt
{
    IRQ_OFF; 
    
    // MUSIC?
    if(playingSong)
        PlaySong();
    else 
        print("OVER");
    

    // PRINT IDLE COUNTS
    SetCursorPos(0, 0);
    u8* d = byToHex((u8)(idleCount >> 8)); 
    print(d);
    u8* d2 = byToHex((u8)(idleCount & 0xff));
    print(d2);
    
    idleCount = 0;

 

    SETBANK_MAINRAM();      
    //

    SetIRQs(); 
    IRQ_ON;
}

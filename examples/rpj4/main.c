#include <pc88-c.h>

#include "song_b.h"

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
    
    SetIRQs(); 
    IRQ_ON;
}

#include <pc88-c.h>

#include "song.h"

inline void SetIRQs()
{
    SetIOReg(IRQ_LEVEL_SET, 2);
    SetIOReg(IRQ_MASK, 2);
}

extern bool playingSong;
extern struct Song curSong;
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
    
    // Set up CurSong pointers
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
        idleCount++;
    }
}

void Vblank() __critical __interrupt
{
    IRQ_OFF; 
    
    if(playingSong)
        PlaySong();
    
    SetCursorPos(0, 0);
    if(playingSong) print("Playing ... ");
    else print(" Song over !");
    SetCursorPos(0, 1);
    print("Idle cycles : ");

    u8* d = byToHex((u8)(idleCount >> 8)); // TODO make byToHex return a str?
    print(d);
    d = byToHex((u8)(idleCount & 0xff));
    print(d);
    
    idleCount = 0;
    
    SetIRQs(); 
    IRQ_ON;
}

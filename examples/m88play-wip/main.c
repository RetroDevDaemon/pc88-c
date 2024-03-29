#include <pc88-c.h>
#include <opn.h>
#include <textmode.h>
#include "song_b.h"

/* 
*
* M88 FORMAT PLAYER (OPN ONLY)
*    Heavily WIP! 大変作成中！
*
*   I need testers and composers willing to help test their compositions
*   and give feedback. Right now it's functional, but not perfect.
*/

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
    
    SetCursorPos(0, 1);
    print("Idle cycles : ");

    IRQ_ON 
    while(1)
    { 
        idleCount++;
    }
}

// cycle counts:
// 367h when song is over
// 284h when 1 SSG (270 223
// 256h when 3 SSG (240 286


void Vblank() __critical __interrupt
{
    IRQ_OFF; 
    
    if(playingSong)
        PlaySong();
    else 
        print("OVER");
    
    SetCursorPos(15, 1);
    u8* d = byToHex((u8)(idleCount >> 8)); 
    print(d);
    u8* d2 = byToHex((u8)(idleCount & 0xff));
    print(d2);
    
    idleCount = 0;
    
    SetIRQs(); 
    IRQ_ON;
}

#include <pc88-c.h>
#include "song.h"

u8 ctr;


inline void SetIRQs()
{
    SetIOReg(IRQ_LEVEL_SET, 2);      // Set IRQ to >2 for VBL+clock R E4
    SetIOReg(IRQ_MASK, 0b10);        // Reset mask for vbl+clock   R E6
    // 2, 0b10 => VBL only, no clock interrupt
}

u32 idleCount;

void Vblank() __critical __interrupt
{
    IRQ_OFF     // Disable IRQs during all interrupts
    
    // Every 1/60s, call the play routine. 毎１／６０秒再生ルーチン起動
    if(SONG_PLAYING)
        PlaySong();
    
    // Print if the song is still playing or not
    SetCursorPos(0, 0);
    if(SONG_PLAYING) print("Playing ... ");
    else print(" Song over !");

    // Print how much free CPU time we have left
    SetCursorPos(0, 1);
    print("Idle cycles : ");
    u8 dat[3];
    byToHex((u8)((idleCount & 0xff0000)>>16), dat);
    print(dat);
    byToHex((u8)((idleCount & 0xff00)>>8), dat);
    print(dat);
    byToHex((u8)(idleCount & 0xff), dat);
    print(dat);
    
    idleCount = 0;
    
    // Reset IRQs - needed at end of every interrupt!
    SetIRQs(); 
    IRQ_ON
}


void main()
{
    IRQ_OFF

    // Set variables
    ctr = 0;
    idleCount = 0;
    u8 i = 0;
    u16 h = GetSysMode();
    SONG_PLAYING = false;
    // and print what mode we found.    
    if(h & V1S_MODE_FLAG) print("V1S ");
    else if(h & V2_MODE_FLAG) print("V2 ");
    else if(h & V1H_MODE_FLAG) print("V1H ");
    //if(h & FOURMHZ_FLAG) print("8mhz detected.");
    //else print("4mhz detected.");
 
    // Load song!
    LoadSong(&song[0]);
    SONG_PLAYING = true; // <- this is all we have to do
    
    // but first, Write the address of the Vblank routine to the CRTC IRQ vector @ f302
    __asm 
        ld hl, #_Vblank
        ld (CRTC_IRQ),hl
    __endasm;    
    SetIRQs();        // And enable the VBL interrupt!
    
    IRQ_ON 

    while(1)
    { 
        // Main loop does nothing but count up how much cpu idle time we have.
        idleCount++;
    }
}

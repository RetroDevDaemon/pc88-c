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

unsigned char hexa[240] = { 
	253,0,15,253,119,2,253,0,28,253,119,2,253,0,2,253,
	119,2,253,0,24,253,119,2,253,0,6,253,119,2,253,0,
	20,253,119,2,253,0,10,253,119,2,253,0,16,253,119,2,
	253,0,14,253,119,2,253,0,12,253,119,2,253,0,18,253,
	119,2,253,0,8,253,119,2,253,0,22,253,119,2,253,0,
	4,253,119,2,253,0,26,253,119,2,254,2,0,119,253,0,
	30,254,2,119,112,253,0,30,254,2,7,112,253,0,30,254,
	2,7,112,253,0,30,254,2,7,112,253,0,30,254,2,7,
	112,253,0,30,254,2,7,112,253,0,30,254,2,7,119,253,
	0,30,254,2,119,0,253,119,2,253,0,26,253,119,2,253,
	0,4,253,119,2,253,0,22,253,119,2,253,0,8,253,119,
	2,253,0,18,253,119,2,253,0,12,253,119,2,253,0,14,
	253,119,2,253,0,16,253,119,2,253,0,10,253,119,2,253,
	0,20,253,119,2,253,0,6,253,119,2,253,0,24,253,119,
	2,253,0,2,253,119,2,253,0,28,253,119,2,253,0,14,
};

u8 PIXELPUSHBUFFER;
s8 PIXELBUFFERCYCLE = 7;

void PushPixel(u8 val)
{
 if color is NOT last color used,
 push the buffer, clear it, do not reset cycle
 swap using io reg 
 
 then write to pixel push buffer by using: 
  PIXELPUSHBUFFER | (1 << PIXELBUFFERCYCLE)
  PIXELBUFFERCYCLE--; (if < 0 then == 7) and inc mem ptr 

if is same color, 
skip and just write the new pixel to the buffer 
}

void DrawImage_V2_RLE(u8 x, u8 y, u8* src)
{// requires ALU to be enabled
    vu8* dst = (vu8*)0xc000; 
    u8 wb, loop;
    //SetIOReg(EXPANDED_ALU_CTRL, CLR_CYAN); 
    u8 c;

    if (*src == 253)
    {
        src++;
        wb = *src++; // byte to write
        loop = *src; // num of times
        for(c = 0; c < loop; c++)
        {
            // New function that CREATES a function to write to the ALU with the correct set
            // of pixels ???
            // e.g. while writeBuf < 4 add to write buff
            // if writebuf = 4 then push to vram.
            // set color here !?
            PushPixel(wb >> 4);
            PushPixel(wb & 0xf);
        }
        // src++ = times to write
    }
    else if(*src == 254)
    {
        src++;
        // src = num of bytes to write through
    }
    else if(*src == 252)
    {
        src++;
        // write next byte raw, should be 0xfc - 0xfe
    }
    src++; 
}

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

    //
    ExpandedGVRAM_On();     
    EnableALU();            
    // ALU drawing here
    


    DisableALU();           
    ExpandedGVRAM_Off();    

    SETBANK_MAINRAM();      
    //

    SetIRQs(); 
    IRQ_ON;
}

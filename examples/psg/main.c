#include <pc88-c.h>

#include "song.h"

inline void SetIRQs();
void Vblank() __critical __interrupt;
void ClockInterrupt() __critical __interrupt;

u8 ctr;

inline void SetIRQs()
{
    SetIOReg(IRQ_LEVEL_SET, 3);      // Set IRQ to >2 for VBL+clock R E4
    SetIOReg(IRQ_MASK, 0b11);        // Reset mask for vbl+clock   R E6
}

void Vblank() __critical __interrupt
{
    IRQ_OFF 
//    print("Hi!");
    SetIRQs(); 
    IRQ_ON
}

void ClockInterrupt() __critical __interrupt
{
    IRQ_OFF 
  //  print(".");
    SetIRQs();
    IRQ_ON
}

void byToHex(u8 by, u8* res)
{
    u8 a = by & 0xf;
    u8 b = (by & 0xf0) >> 4;
    if (a > 9) a += 7;
    if (b > 9) b += 7;
    a += 0x30;
    b += 0x30;
    res[0] = b; res[1] = a; res[2] = 0; 
}

void main()
{
    IRQ_OFF 
    ctr = 0;
    
    u16 h = GetSysMode();

    if(h & V1S_MODE_FLAG) print("V1S ");
    else if(h & V2_MODE_FLAG) print("V2 ");
    else if(h & V1H_MODE_FLAG) print("V1H ");
    //if(h & FOURMHZ_FLAG) print("8mhz detected.");
    //else print("4mhz detected.");

    // Tone is 12 bits, coarse goes in 01, fine in 00

    // 6 bytes per map change (last 3 are 0 if no envelope)
    // +1 for mixer
    // +1 for noise tone

    // tone L (0, 2, 4)
    // tone H (1, 3, 5)
    /// tone noise (channel independant) (6)
    /// mixer (7)
    // amplitude (if bit 4:) (8,9,a)
    /// (env pd H) 
    /// (env pd L)
    /// (env type)

    // Play C4 on Ch A, vol 15, no envelope, mute all other
    SetIOReg(OPN_REG, CHA_TONEL);
    SetIOReg(OPN_DAT, SSG_C4 & 0xff);
    SetIOReg(OPN_REG, CHA_TONEH);
    SetIOReg(OPN_DAT, (SSG_C4 & 0xf00) >> 8);
    //0b00111110:
    SetIOReg(OPN_REG, SSG_MIXER);
    SetIOReg(OPN_DAT, ~(CHA_TONE | 0xc0));
    SetIOReg(OPN_REG, CHA_AMP);
    SetIOReg(OPN_DAT, 15);

    u8* sctr = &song[0];
    struct m88header { 
        u8 numSongs;
        u8* FMOfs;
        u16 binSize;
    };
    struct m88header abc;
    abc.numSongs = *sctr++;
    abc.FMOfs = *sctr++ + *sctr++*256 + &song[0] + 5;
    abc.binSize = *sctr++ + *sctr++*256;
    struct m88data { 
        u8 tempo; 
        u16* partOffsets[11];
        u16 partLoops[11];
        u16* dataEndLoc;
    };
    struct m88data ab; 
    ab.tempo = *sctr++;
    for(u8 i = 0; i < 11; i+=4) { 
        ab.partOffsets[i] = (*sctr++ + *sctr++*256) + &song[0] + 5;
        ab.partLoops[i] = (*sctr++ + *sctr++*256);
    }
    ab.dataEndLoc = *sctr++ + *sctr*256 + &song[0] + 5;
    u8 answ[3];
    byToHex((u8)((u16)ab.partOffsets[3] & 0xff), answ);
    
    print(answ);
    // First, Write the address of the Vblank routine to the CRTC IRQ vector @ f302
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
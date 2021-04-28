#include <pc88-c.h>

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

/*** READ AND WRITE TEST **
    // OPN REGISTER SELECT - 0x44
    // OPN REGISTER DATA - 0x45
    u8 ymarr[16];
    for(u8 i = 0; i <= 0xd; i++)
    {
        SetIOReg(OPN_REG, i);
        SetIOReg(OPN_DAT, 0x33);
        ymarr[i] = ReadIOReg(OPN_DAT);
        
        u8 ob[3];
        byToHex(ymarr[i], &ob[0]);
        SetCursorPos(0, i);
        print("R ");
        u8 ch[2] = { 0, 0 }; 
        ch[0] = i < 10 ? i + 0x30 : i + 0x37;
        print(ch);
        print(": ");
        print(ob);
    }
    for (u8 i = 0; i < 2; i ++)
    {
        SetIOReg(OPN_REG, i + 14);
        ymarr[i + 14] = ReadIOReg(OPN_DAT);
        SetCursorPos(0, i + 15);
        print("R ");
        u8 ob[3];
        byToHex(ymarr[i + 14], &ob[0]);
        u8 ch[2] = { 0, 0 };
        ch[0] = i + 0x37 + 14;
        print(ch); print(": "); print(ob);
    }
/////////////////////////////
*/
    // Tone is 12 bits, coarse goes in 01, fine in 00
#define CHA_TONEH 1
#define CHA_TONEL 0
#define CHB_TONEH 3
#define CHB_TONEL 2
#define CHC_TONEH 5
#define CHC_TONEL 4
#define SSG_MIXER 7
#define SSG_NOISETONE 6
#define CHA_AMP 8
#define CHB_AMP 9
#define CHC_AMP 10
#define SSG_ENVPDH 0xb
#define SSG_ENVPDL 0xc
#define SSG_ENVTYPE 0xd
#define SSG_IOA 0xe
#define SSG_IOB 0xf
#define CHA_NOISE_OFF 0b1000
#define CHB_NOISE_OFF 0b10000
#define CHC_NOISE_OFF 0b100000
#define CHA_TONE 0b1
#define CHB_TONE_OFF 0b10
#define CHC_TONE_OFF 0b100

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

    u16 e = SSG_C4;
    SetIOReg(OPN_REG, CHA_TONEL);
    SetIOReg(OPN_DAT, e & 0xff);
    SetIOReg(OPN_REG, CHA_TONEH);
    SetIOReg(OPN_DAT, (e & 0xf00) >> 8);

    SetIOReg(OPN_REG, SSG_MIXER);
    //0b00111110:
    SetIOReg(OPN_DAT, ~(CHA_TONE | 0xc0));
    SetIOReg(OPN_REG, CHA_AMP);
    SetIOReg(OPN_DAT, 15);
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
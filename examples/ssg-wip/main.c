#include <pc88-c.h>

#include "song.h"

inline void SetIRQs();
void Vblank() __critical __interrupt;
void ClockInterrupt() __critical __interrupt;


bool playingSong;
u8 ctr;

inline void SetIRQs()
{
    SetIOReg(IRQ_LEVEL_SET, 3);      // Set IRQ to >2 for VBL+clock R E4
    SetIOReg(IRQ_MASK, 0b11);        // Reset mask for vbl+clock   R E6
}

// The clock 1/600 interrupt is enabled, but does nothing for this example
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

const u16 octavesix[12] = {
    SSG_C6, SSG_C6S,
    SSG_D6, SSG_D6S, 
    SSG_E6,
    SSG_F6, SSG_F6S,
    SSG_G6, SSG_G6S,
    SSG_A6, SSG_A6S,
    SSG_B6
};
const u16 octavefive[12] = {
    SSG_C5, SSG_C5S,
    SSG_D5, SSG_D5S, 
    SSG_E5,
    SSG_F5, SSG_F5S,
    SSG_G5, SSG_G5S,
    SSG_A5, SSG_A5S,
    SSG_B5
};
const u16 octavefour[12] = {
    SSG_C4, SSG_C4S,
    SSG_D4, SSG_D4S,
    SSG_E4,
    SSG_F4, SSG_F4S,
    SSG_G4, SSG_G4S,
    SSG_A4, SSG_A4S,
    SSG_B4
};
const u16 octavethree[12] = {
    SSG_C3, SSG_C3S,
    SSG_D3, SSG_D3S, 
    SSG_E3,
    SSG_F3, SSG_F3S,
    SSG_G3, SSG_G3S,
    SSG_A3, SSG_A3S,
    SSG_B3
};
const u16 octavetwo[12] = {
    SSG_C2, SSG_C2S,
    SSG_D2, SSG_D2S, 
    SSG_E2,
    SSG_F2, SSG_F2S,
    SSG_G2, SSG_G2S,
    SSG_A2, SSG_A2S,
    SSG_B2
};

typedef signed int fix_16s;
#define FIXED16(n) ((fix_16s)((n) << 8))


struct m88header { 
    u8 numSongs;
    u8* FMOfs;
    u16 binSize;
};
struct m88data { 
    fix_16s tempo; 
    u8* partOffsets[11];
    u16 partLoops[11];
    u8* dataEndLoc;
};

struct Song { 
    struct m88header songheader;
    struct m88data songdata;
    s16 partLengths[11]; // TODO fill these in - at the moment not used.
    u8 ssg_instr[3];
    u8 ssg_mix;
    s8 ssg_vol[3];
    s8 ssg_tone_len[3]; // Counts down!
    u8 ssg_oct[3];
    u8 ssg_tone[3];
    u16 ssg_loc[3];
    bool part_over[3];
    bool ssg_fading[3];
    s8 ssg_base_vol[3];
};
struct Song curSong;
signed int ticker;

void LoadSong(const u8* song)
{   // Load song header
    u8 i;
    u8* sctr = song;
    struct m88header* sh = (struct m88header*)&curSong.songheader;
    sh->numSongs = *sctr++;
    sh->FMOfs = *sctr++;
    sh->FMOfs += (*sctr++*256);
    sh->FMOfs += (u16)(song);
    sh->binSize = *sctr++;
    sh->binSize += (*sctr++*256);
    // Load song data 
    struct m88data* sd = (struct m88data*)&curSong.songdata;
    sd->tempo = FIXED16(*sctr++); // how many bpm (60 << 8) = 15360. each frame, add FIXED16(60) to a counter. 
    // when that counter exceeds FIXED16(sd->tempo), decrement the TONE LENGTH of every SSG by 1. 
    for(i = 0; i < 11; i++) 
    {
        sd->partOffsets[i] = *sctr++;
        sd->partOffsets[i] += *sctr++ * 256;
        sd->partOffsets[i] += (u16)(song) + 5;
        sd->partLoops[i] = *sctr++;
        sd->partLoops[i] += *sctr++ * 256;
    }
    sd->dataEndLoc = *sctr++;
    sd->dataEndLoc += *sctr * 256;
    // end loading in song "headers"
    sctr = &curSong.ssg_loc[0];
    for(i = 0; i < 15; i++) *sctr++ = 0;
    /*
    fill the rest of the song struct with 0 */
}


void SetSSGInstrument(u8 chn, u8 instr)
{
    // Only 1 instrument is supported atm, generic piano 
    switch(chn)
    {
        case(0):
            break;
        case(1):
            break;
        case(2):
            break;
    }
    switch(instr)
    {
        case(0):
            break;
        case(1):
            // 255 255 255 200 0 10
            //    A     D    S   R
            break;
    }
}


#define M_SSGINSTRUMENT 0xf0 // @n
#define M_SSGVOLUME 0xf1    // vn
//#define M_SSGDETUNE 0xf2    // Dn
//#define M_GATETIME 0xf3     // qn
//#define M_SSGSOFTLFO 0xf4   // Mn1,n2,n3,n4
#define M_STARTLOOP 0xf5    // [
    // 0xf5 x2 x1 where x2x1 = distance in hex to end of loop
#define M_ENDLOOP 0xf6      // ]n
    // 0xf6 x1 x2 x3 x4 where x1=work x2=loop no x4x3 = dist in hex to loop start
#define M_SSGMIXER 0xf7 
//#define M_SSGNOISEFQ 0xf8   // wn
//#define M_SETFLAG 0xf9      // #n
//#define M_SSGSOFTENV 0xfa   // not used... this is instrument code
//#define M_RELVOLUME 0xfb    // )n1 or (n2
//#define M_MAKEINSTR 0xfc    // not used yet
//#define M_SLUR 0xfd         // &
//#define M_EXITLOOP 0xfe     // /
    // x1 x2 where x2x1 is distance to loop ending (so its a jump)
//#define M_SSGENVTYP 0xf1ff
//#define M_SSGENVFRQ 0xf2ff
//#define M_REVERB 0xf3ff
//#define M_REVERBMODE 0xf4ff
//#define M_REVERBSWITCH 0xf5ff
#define null 0
#define Null null 

void PlaySong()
{
    u8 j;
    struct m88data* songdata = (struct m88data*)&curSong.songdata;

    for(j = 0; j < 3; j++)
    {
        if(curSong.ssg_tone_len[j] <= 0)
        {   // This means this channel is ready for another input
            //curSong.ssg_fading[j] = false;
            u8 songby = *(songdata->partOffsets[3+j] + curSong.ssg_loc[j]);
            if(songby == M_SSGINSTRUMENT) // set instrument
            { 
                curSong.ssg_loc[j]++;
                songby = *(songdata->partOffsets[3+j] + curSong.ssg_loc[j]); // which?
                // call set instrument for this channel TODO instrument defs in the mucom88 docs
                curSong.ssg_instr[j] = songby;
                //SetSSGInstrument(j, songby);
            }
            else if (songby == M_STARTLOOP)
            {
                
            }
            else if (songby == M_SSGVOLUME)
            {
                curSong.ssg_loc[j]++; // set ssg vol
                songby = *(songdata->partOffsets[3+j] + curSong.ssg_loc[j]);
                curSong.ssg_base_vol[j] = (songby - 2) & 0xf;
            }
            else if (songby == M_SSGMIXER)
            {
                curSong.ssg_loc[j]++; // set SSG mixer
                //[SSG] ミキサモード Pn (0:発音しない 1:トーン 2:ノイズ 3:トーン+ノイズ) - 9, 8, 1, 0
                songby = *(songdata->partOffsets[3+j] + curSong.ssg_loc[j]);
                curSong.ssg_mix = curSong.ssg_mix & ~(0b00001001 << j); // nullify inverse of songby
                curSong.ssg_mix ^= (songby << j); // flip songby bits
                SetIOReg(OPN_REG, SSG_MIXER);
                SetIOReg(OPN_DAT, curSong.ssg_mix);
            }
            else if ((songby > 0) && (songby < 128)) // play sound !!!
            {
                curSong.ssg_loc[j]++; 
                curSong.ssg_tone_len[j] = songby; // first byte is length. 18h = 1/8 beat at 120T. LEN = 24, so 
                songby = *(songdata->partOffsets[3+j] + curSong.ssg_loc[j]); // second byte is tone...
                curSong.ssg_oct[j] = (songby & 0xf0) >> 4; 
                curSong.ssg_tone[j] = (songby & 0x0f);
                curSong.ssg_vol[j] = curSong.ssg_base_vol[j];
                SetIOReg(OPN_REG, CHA_AMP + j);
                SetIOReg(OPN_DAT, curSong.ssg_base_vol[j]);
                //SetIOReg(OPN_REG, 0xc); // coarse envelope frequency adjustment (invesr, so lower = faster repeat)
                //SetIOReg(OPN_DAT, 0b00000111);
                switch(curSong.ssg_oct[j]+1){ 
                    case(2):
                        SetIOReg(OPN_REG, CHA_TONEL + (j*2));
                        SetIOReg(OPN_DAT, octavetwo[curSong.ssg_tone[j]] & 0xff);
                        SetIOReg(OPN_REG, CHA_TONEH + (j*2));
                        SetIOReg(OPN_DAT, (octavetwo[curSong.ssg_tone[j]] & 0xff00) >> 8); 
                        break;
                    case(3):
                        SetIOReg(OPN_REG, CHA_TONEL + (j*2));
                        SetIOReg(OPN_DAT, octavethree[curSong.ssg_tone[j]] & 0xff);
                        SetIOReg(OPN_REG, CHA_TONEH + (j*2));
                        SetIOReg(OPN_DAT, (octavethree[curSong.ssg_tone[j]] & 0xff00) >> 8);                         
                        break;
                    case(4):
                        SetIOReg(OPN_REG, CHA_TONEL + (j*2));
                        SetIOReg(OPN_DAT, octavefour[curSong.ssg_tone[j]] & 0xff);
                        SetIOReg(OPN_REG, CHA_TONEH + (j*2));
                        SetIOReg(OPN_DAT, (octavefour[curSong.ssg_tone[j]] & 0xff00) >> 8); 
                        break;
                    case(5):
                        SetIOReg(OPN_REG, CHA_TONEL + (j*2));
                        SetIOReg(OPN_DAT, octavefive[curSong.ssg_tone[j]] & 0xff);
                        SetIOReg(OPN_REG, CHA_TONEH + (j*2));
                        SetIOReg(OPN_DAT, (octavefive[curSong.ssg_tone[j]] & 0xff00) >> 8); 
                        break;    
                    case(6):
                        SetIOReg(OPN_REG, CHA_TONEL + (j*2));
                        SetIOReg(OPN_DAT, octavesix[curSong.ssg_tone[j]] & 0xff);
                        SetIOReg(OPN_REG, CHA_TONEH + (j*2));
                        SetIOReg(OPN_DAT, (octavesix[curSong.ssg_tone[j]] & 0xff00) >> 8); 
                        break;
                }
            }
            else if ((songby > 0x80) && (songby < 0xf0))
            {   // rest by setting volume to 0? (not needed when 1 instrument)
                //SetIOReg(OPN_REG, CHA_AMP + j);
                //SetIOReg(OPN_DAT, 0);
                curSong.ssg_tone_len[j] = (songby & 0x7f); 
            }
            else if (songby == null)
            {
                SetIOReg(OPN_REG, CHA_AMP + j);
                SetIOReg(OPN_DAT, 0);
                curSong.ssg_loc[j]--;
                curSong.part_over[j] = true;
            }
            curSong.ssg_loc[j]++;
        } 
        //else 
        //{ 
            //u8 tem = curSong.songdata.tempo;
            //curSong.ssg_tone_len[j] -= 1;
        //}
    }
    // Use this for tempo-exact timing!
    ticker += FIXED16(60);
    if(ticker >= curSong.songdata.tempo)
    {
        ticker -= curSong.songdata.tempo;
        for(j = 0; j < 3; j++) 
        {
            if(curSong.ssg_tone_len[j] > 0) // is this tone still playing? 
            {
                curSong.ssg_tone_len[j]--;  // then tick it down
                if(curSong.ssg_tone_len[j] < curSong.ssg_base_vol[j]) // am i OK to fade?
                {
                    curSong.ssg_fading[j] = true;
                }
                if(curSong.ssg_fading[j] == true)
                {
                    if(curSong.ssg_vol[j] > 1) // if I'm not at volume 0 yet... 
                    {
                        SetIOReg(OPN_REG, (CHA_AMP + j));
                        SetIOReg(OPN_DAT, --curSong.ssg_vol[j]); // fade this channel more!
                    }
                    else 
                        curSong.ssg_fading[j] = false;
                }
            }
        }
    }
    // if all 3 parts are done, then stop the song
    bool keepPlaying = false;
    for(j = 0; j < 3; j++)
    {
        if(curSong.part_over[j] == false) 
        {
            keepPlaying = true;
        }
    }
    if(!keepPlaying) playingSong = false;
}

u32 idleCount;

void Vblank() __critical __interrupt
{
    IRQ_OFF 
    
    if(playingSong)
        PlaySong();
    
    SetCursorPos(0, 0);
    if(playingSong) print("Playing ... ");
    else print(" Song over !");
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
    
    SetIRQs(); 
    IRQ_ON
}


void main()
{
    IRQ_OFF 
    ctr = 0;
    idleCount = 0;
    u8 i = 0;
    u16 h = GetSysMode();
    playingSong = false;
    
    if(h & V1S_MODE_FLAG) print("V1S ");
    else if(h & V2_MODE_FLAG) print("V2 ");
    else if(h & V1H_MODE_FLAG) print("V1H ");
    //if(h & FOURMHZ_FLAG) print("8mhz detected.");
    //else print("4mhz detected.");
 
    // Set up CurSong pointers
    LoadSong(&song[0]);
    playingSong = true;
    ticker = 0;
    // First, Write the address of the Vblank routine to the CRTC IRQ vector @ f302
    __asm 
        ld hl, #_Vblank
        ld (CRTC_IRQ),hl
        ld hl, #_ClockInterrupt
        ld (CLOCK_IRQ),hl
    __endasm;
    
    SetIRQs();        // And enable the VBL interrupt!
    
    IRQ_ON 
    while(1)
    { 
        idleCount++;
    }
}

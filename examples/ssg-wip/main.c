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

struct m88header { 
    u8 numSongs;
    u8* FMOfs;
    u16 binSize;
};
struct m88data { 
    u8 tempo; 
    u8* partOffsets[11];
    u16 partLoops[11];
    u8* dataEndLoc;
};

struct Song { 
    struct m88header songheader;
    struct m88data songdata;
    s16 partLengths[11]; // TODO fill these in
    u8 ssg_instr[3];
    u8 ssg_mix;
    u8 ssg_vol[3];
    u16 ssg_tone_len[3]; // Counts down!
    u8 ssg_oct[3];
    u8 ssg_tone[3];
    u16 ssg_loc[3];
};
struct Song curSong;

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
    sd->tempo = (u8)(((*sctr++)*6)/600); // how many measures per minute in timer-B format
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
    curSong.ssg_loc[0] = 0;
    curSong.ssg_tone_len[0] = 0;
    curSong.ssg_loc[1] = 0;
    curSong.ssg_tone_len[1] = 0;
    curSong.ssg_loc[2] = 0;
    curSong.ssg_tone_len[2] = 0;
}


void SetSSGInstrument(u8 chn, u8 instr)
{
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
            break;
    }
}

void PlaySong()
{
    struct m88data* songdata = (struct m88data*)&curSong.songdata;
    

    for(u8 j = 0; j < 3; j++)
    {
        if(curSong.ssg_tone_len[j] == 0)
        {   // This means this channel is ready for another input
            u8 songby = *(songdata->partOffsets[3+j] + curSong.ssg_loc[j]);
            if(songby == 0xf0) // set instrument
            { 
                curSong.ssg_loc[j]++;
                songby = *(songdata->partOffsets[3+j] + curSong.ssg_loc[j]); // which?
                // call set instrument for this channel TODO
                curSong.ssg_instr[j] = songby;
                //SetSSGInstrument(j, songby);
            }
            else if (songby == 0xf1)
            {
                curSong.ssg_loc[j]++; // set ssg vol
                songby = *(songdata->partOffsets[3+j] + curSong.ssg_loc[j]);
                curSong.ssg_vol[j] = songby;
                //SetIOReg(OPN_REG, CHA_AMP + j);
                //SetIOReg(OPN_DAT, songby);
            }
            else if (songby == 0xf7)
            {
                curSong.ssg_loc[j]++; // set SSG mixer
                //[SSG] ミキサモード Pn (0:発音しない 1:トーン 2:ノイズ 3:トーン+ノイズ) - 9, 8, 1, 0
                songby = *(songdata->partOffsets[3+j] + curSong.ssg_loc[j]);
                curSong.ssg_mix = curSong.ssg_mix & ~(0b00001001 << j); // nullify inverse of songby
                curSong.ssg_mix ^= (songby << j); // flip songby bits
                SetIOReg(OPN_REG, SSG_MIXER);
                SetIOReg(OPN_DAT, curSong.ssg_mix);
            }
            else if ((songby > 0) && (songby <= 0x7f))
            {
                curSong.ssg_loc[j]++; // play sound
                curSong.ssg_tone_len[j] = songby; // first byte is length. 16 = 1/8 beat at 120T.
                songby = *(songdata->partOffsets[3+j] + curSong.ssg_loc[j]);
                curSong.ssg_oct[j] = (songby & 0xf0) >> 4; // second is tone
                curSong.ssg_tone[j] = (songby & 0x0f);
                SetIOReg(OPN_REG, CHA_AMP + j);
                SetIOReg(OPN_DAT, curSong.ssg_vol[j]);
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
            {   // rest by setting volume to 0
                SetIOReg(OPN_REG, CHA_AMP + j);
                SetIOReg(OPN_DAT, 0);
                curSong.ssg_tone_len[j] = (songby & 0b00111111); // 16 = 1/8
                // a len of 16 we want to erase in 7.5 ticks (or 8).
                // that is 8 frames, at 120 tempo is, 2 per frame.
            }
            else if (songby == 0)
            {
                SetIOReg(OPN_REG, CHA_AMP + j);
                SetIOReg(OPN_DAT, 0);
                curSong.ssg_loc[j]--;
                curSong.ssg_vol[j] = 0;
            }
            curSong.ssg_loc[j]++;
        } 
        else 
        { 
            curSong.ssg_tone_len[j] -= 1;
        }
    }
    /*bool stopq = true;
    for(u8 s = 0; s < 3; s++){
        if(curSong.ssg_vol[s] != 0){
            stopq = false;
        }
    }
    if(stopq) playingSong = false;*/
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

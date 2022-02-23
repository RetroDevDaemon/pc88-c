
#include <pc88-c.h>

#include "banklite.h"

struct Song currentSong;
int ticker;
bool playingSong;

// TODO: Prolly wont implement this
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

void SetFMInstrument(u8 chn, u8 instr)
{

}

// `D, `E, `F
// (1 instrument for ssg...)
// supported mml: t, v, [, ], P, w, #
// 00        : hard end 
// 1 - 7f +1 : note
// 81 - ef   : rest 

#define M_INSTRUMENT 0xf0 // @n
#define M_VOLUME 0xf1    // vn
//#define M_DETUNE 0xf2    // Dn
//#define M_GATETIME 0xf3     // qn
//#define M_SOFTLFO 0xf4   // Mn1,n2,n3,n4
#define M_STARTLOOP 0xf5    // [
    // 0xf5 x2 x1 where x2x1 = distance in hex to end of loop
#define M_ENDLOOP 0xf6      // ]n
    // 0xf6 x1 x2 x3 x4 where x1=work x2=loop no x4x3 = dist in hex to loop start
#define M_SSGMIXER 0xf7     // P
#define M_FMPAN 0xf8
#define M_SSGNOISEFQ 0xf8   // wn set register 6 to n, 0-31
#define M_SETFLAG 0xf9      // #n
//#define M_SSGSOFTENV 0xfa   // not used... this is instrument code
//#define M_RELVOLUME 0xfb    // )n1 or (n2
//#define M_MAKESSGINSTR 0xfc    // not used yet
//#define M_SLUR 0xfd         // &
//#define M_EXITLOOP 0xfe     // /
    // x1 x2 where x2x1 is distance to loop ending (so its a jump)
//#define M_SSGENVTYP 0xf1ff
//#define M_SSGENVFRQ 0xf2ff
//#define M_REVERB 0xf3ff
//#define M_REVERBMODE 0xf4ff
//#define M_REVERBSWITCH 0xf5ff
#define null (void*)0
#define Null null 

// Organization of 11-channels in m88 files:
// FM1, FM2, FM3
// SSG1 SSG2 SSG3
// RHY
// FM4 FM5 FM6
// (ADPCM)

void LoadSong(const u8* song)
{   // Load song header
    u8 i;
    u8* sctr = song;
    struct m88header* sh = (struct m88header*)&currentSong.songheader;
    sh->numSongs = *sctr++;
    sh->FMOfs = *sctr++;
    sh->FMOfs += (*sctr++*256);
    sh->FMOfs += (u16)(song);
    sh->binSize = *sctr++;
    sh->binSize += (*sctr++*256);
    // Load song data 
    struct m88data* sd = (struct m88data*)&currentSong.songdata;
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
    sctr = &currentSong.part_loc[0];
    for(i = 0; i < 57; i++) *sctr++ = 0;
    /*
    fill the rest of the song struct with 0 */
}


void PlaySong()
{
    u8 j;
    u8 channel;
    u8 du;
    struct m88data* songdata = (struct m88data*)&currentSong.songdata;
    
    // FM loop is 3 + RHY + 3 channels:
    for(channel = 0; channel < 11; channel++)
    {
        // optimize!
        if(currentSong.chan_disabled[channel] == true)
            continue;
        u8* songby = (u16)songdata->partOffsets[channel] + (u16)currentSong.part_loc[channel];
        //
        if(channel <= 2)
        {
            //* FM CHANNELS 1 2 3 *//
            //#define M_INSTRUMENT 0xf0 // @n
            //#define M_VOLUME 0xf1    // vn
            //#define M_DETUNE 0xf2    // Dn
            //#define M_GATETIME 0xf3     // qn
            //#define M_SOFTLFO 0xf4   // Mn1,n2,n3,n4
            //#define M_STARTLOOP 0xf5    // [
                // 0xf5 x2 x1 where x2x1 = distance in hex to end of loop
            //#define M_ENDLOOP 0xf6      // ]n
                // 0xf6 x1 x2 x3 x4 where x1=work x2=loop no x4x3 = dist in hex to loop start
            //#define M_FMPAN 0xf8
            //#define M_SETFLAG 0xf9      // #n
            if(currentSong.fm_tone_len[channel] <= 0 && !currentSong.part_over[channel])
            {   // This FM channel is ready for next note
                switch(*songby)
                {
                    case(M_INSTRUMENT):
                        currentSong.part_loc[channel]++;
                        songby++;
                        currentSong.fm_instr[channel] = *songby;
                        SetFMInstrument(channel, (u8)(*songby));
                        break;
                    default:
                        break;
                }
            }
        }
        else if((channel > 2) && (channel < 6)) // 3 4 5
        {
            j = channel - 3;
            // SSG channels 1-3 are in positions 3-5
            if(currentSong.ssg_tone_len[j] <= 0 && !currentSong.part_over[channel])
            {   // This means this channel is ready for another input
                if(*songby == M_INSTRUMENT) // set instrument
                { // doesn:t do anything right now
                    currentSong.part_loc[channel]++;
                    //songby = *(songdata->partOffsets[channel] + currentSong.part_loc[j]); // which?
                    songby++;
                    // call set instrument for this channel TODO instrument defs in the mucom88 docs
                    currentSong.ssg_instr[j] = *songby;
                    //SetSSGInstrument(j, songby);
                }
                else if (*songby == M_SETFLAG)
                {
                    // f9 n
                    songby++;
                    currentSong.flag = *songby;
                    currentSong.part_loc[channel] ++; // skip 1 variable
                }
                else if (*songby == M_SSGNOISEFQ)
                {
                    // f8 n 
                    songby++;
                    SetIOReg(OPN_REG, 6);
                    SetIOReg(OPN_DAT, *songby);
                    currentSong.part_loc[channel]++; // 1 var
                }
                else if (*songby == M_STARTLOOP)
                {   //    3d 3e 3f
                    songby++; // 22 ...
                    u16 ofs = *songby; // OK.
                    songby++;
                    ofs += (*songby) * 256;
    //#define HALT while(1){};
                    //HALT;
                    currentSong.loopLocs[channel] = (u16)songdata->partOffsets[channel] + (u16)(currentSong.part_loc[channel] + ofs);
                    currentSong.looping[channel] = true;
                    currentSong.part_loc[channel] += 2; // skip 2 variables
                    songby = currentSong.loopLocs[channel] + 1;
                    *currentSong.loopLocs[channel] = *songby; // set the workram byte 
                    // ex f5 22 00 = STARTLOOP to +34(22h)
                    // 34 bytes ahead of 003dh is 005fh. this is the locateion of the first byte AFTER the 0xF6 opcode (work loop byte).
                }
                else if (*songby == M_ENDLOOP)
                {
                    //f6 98 99 21 00
                    songby++; // decrement the workram byte
                    (*songby)--; //
                    if(*songby == 0xff) 
                    {
                        currentSong.looping[channel] = false;
                        currentSong.part_loc[channel] += 4;
                    }
                    else{
                        // full loop counter is next byte so skip it
                        songby++;
                        songby++; // < low byte
                        currentSong.part_loc[channel] -= (u16)(*songby);
                        songby++;
                        currentSong.part_loc[channel] -= (u16)(*songby * 256);
                    }
                }
                else if (*songby == M_VOLUME)
                {
                    currentSong.part_loc[channel]++; // set ssg vol
                    songby++;
                    currentSong.ssg_base_vol[j] = (*songby - 2) & 0xf;
                }
                else if (*songby == M_SSGMIXER)
                {
                    currentSong.part_loc[channel]++; // set SSG mixer
                    //[SSG] ミキサモード Pn (0:発音しない 1:トーン 2:ノイズ 3:トーン+ノイズ) - 9, 8, 1, 0
                    songby++;
                    currentSong.ssg_mix = currentSong.ssg_mix & ~(0b00001001 << j); // nullify inverse of songby
                    currentSong.ssg_mix ^= (*songby << j); // flip songby bits
                    SetIOReg(OPN_REG, SSG_MIXER);
                    SetIOReg(OPN_DAT, currentSong.ssg_mix);
                }
                else if ((*songby > 0) && (*songby < 128)) // play sound !!!
                {
                    currentSong.part_loc[channel]++; 
                    currentSong.ssg_tone_len[j] = *songby; // first byte is length. 18h = 1/8 beat at 120T. LEN = 24, so 
                    songby++;// second byte is tone...
                    currentSong.ssg_oct[j] = (*songby & 0xf0) >> 4; 
                    currentSong.ssg_tone[j] = (*songby & 0x0f);
                    currentSong.ssg_vol[j] = currentSong.ssg_base_vol[j];
                    SetIOReg(OPN_REG, CHA_AMP + j);
                    SetIOReg(OPN_DAT, currentSong.ssg_base_vol[j]);
                    //SetIOReg(OPN_REG, 0xc); // coarse envelope frequency adjustment (invesr, so lower = faster repeat)
                    //SetIOReg(OPN_DAT, 0b00000111);
                    du = j * 2;
                    switch(currentSong.ssg_oct[j]){ // octave+1
                        case(1):
                            SetIOReg(OPN_REG, CHA_TONEL + (du));
                            SetIOReg(OPN_DAT, octavetwo[currentSong.ssg_tone[j]] & 0xff);
                            SetIOReg(OPN_REG, CHA_TONEH + (du));
                            SetIOReg(OPN_DAT, (octavetwo[currentSong.ssg_tone[j]] ) >> 8); 
                            break;
                        case(2):
                            SetIOReg(OPN_REG, CHA_TONEL + (du));
                            SetIOReg(OPN_DAT, octavethree[currentSong.ssg_tone[j]] & 0xff);
                            SetIOReg(OPN_REG, CHA_TONEH + (du));
                            SetIOReg(OPN_DAT, (octavethree[currentSong.ssg_tone[j]] ) >> 8);                         
                            break;
                        case(3):
                            SetIOReg(OPN_REG, CHA_TONEL + (du));
                            SetIOReg(OPN_DAT, octavefour[currentSong.ssg_tone[j]] & 0xff);
                            SetIOReg(OPN_REG, CHA_TONEH + (du));
                            SetIOReg(OPN_DAT, (octavefour[currentSong.ssg_tone[j]] ) >> 8); 
                            break;
                        case(4):
                            SetIOReg(OPN_REG, CHA_TONEL + (du));
                            SetIOReg(OPN_DAT, octavefive[currentSong.ssg_tone[j]] & 0xff);
                            SetIOReg(OPN_REG, CHA_TONEH + (du));
                            SetIOReg(OPN_DAT, (octavefive[currentSong.ssg_tone[j]] ) >> 8); 
                            break;    
                        case(5):
                            SetIOReg(OPN_REG, CHA_TONEL + (du));
                            SetIOReg(OPN_DAT, octavesix[currentSong.ssg_tone[j]] & 0xff);
                            SetIOReg(OPN_REG, CHA_TONEH + (du));
                            SetIOReg(OPN_DAT, (octavesix[currentSong.ssg_tone[j]] ) >> 8); 
                            break;
                    }
                }
                else if ((*songby > 0x80) && (*songby < 0xf0))
                {   // rest by setting volume to 0? (not needed when 1 instrument)
                    //SetIOReg(OPN_REG, CHA_AMP + j);
                    //SetIOReg(OPN_DAT, 0);
                    currentSong.ssg_tone_len[j] = (*songby & 0x7f); 
                }
                else if (*songby == null)
                {
                    SetIOReg(OPN_REG, CHA_AMP + j);
                    SetIOReg(OPN_DAT, 0);
                    currentSong.part_loc[channel]--;
                    currentSong.part_over[channel] = true;
                    currentSong.chan_disabled[j] = true;
                }
                currentSong.part_loc[channel]++;
            }
        }
        // END SSG SECTION 
        else if(channel == 6) // RHY
        {}
        else // FM4 FM5 FM6, channel= 7-9 
        { 

        } 
        //else 
        //{ 
            //u8 tem = currentSong.songdata.tempo;
            //currentSong.ssg_tone_len[j] -= 1;
        //}
    }
    // Use this for tempo-exact timing!
    ticker += FIXED16(60);
    if(ticker >= currentSong.songdata.tempo)
    {
        ticker -= currentSong.songdata.tempo;
        for(j = 0; j < 3; j++) 
        {
            if(currentSong.ssg_tone_len[j] > 0) // is this tone still playing? 
            {
                currentSong.ssg_tone_len[j]--;  // then tick it down
                if(currentSong.ssg_tone_len[j] < currentSong.ssg_base_vol[j]) // am i OK to fade?
                {
                    //currentSong.ssg_fading[j] = true;
                    SetIOReg(OPN_REG, (CHA_AMP + j));
                    SetIOReg(OPN_DAT, 0);
                    currentSong.ssg_vol[j]=0;
                }
                /*
                if(currentSong.ssg_fading[j] == true) 
                {
                    if(currentSong.ssg_vol[j] > 1) // if I'm not at volume 0 yet... 
                    {
                        SetIOReg(OPN_REG, (CHA_AMP + j));
                        SetIOReg(OPN_DAT, 0);//--currentSong.ssg_vol[j]); // fade this channel more!
                        currentSong.ssg_vol[j]=0;
                    }
                    else 
                        currentSong.ssg_fading[j] = false;
                }
                */
            }
        }
    }
    // if all 3 parts are done, then stop the song
    bool keepPlaying = false;
    for(j = 0; j < 3; j++)
    {
        if(currentSong.part_over[channel] == false) //FIXME checks SSG only
        {
            keepPlaying = true;
        }
    }
    if(!keepPlaying) playingSong = false;
}



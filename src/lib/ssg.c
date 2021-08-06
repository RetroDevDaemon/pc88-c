
#include <pc88-c.h>

struct Song curSong;
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

// `D, `E, `F
// (1 instrument for ssg...)
// supported mml: t, v, [, ], P, w, #

#define M_SSGINSTRUMENT 0xf0 // @n
#define M_SSGVOLUME 0xf1    // vn
//#define M_SSGDETUNE 0xf2    // Dn
//#define M_GATETIME 0xf3     // qn
//#define M_SSGSOFTLFO 0xf4   // Mn1,n2,n3,n4
#define M_STARTLOOP 0xf5    // [
    // 0xf5 x2 x1 where x2x1 = distance in hex to end of loop
#define M_ENDLOOP 0xf6      // ]n
    // 0xf6 x1 x2 x3 x4 where x1=work x2=loop no x4x3 = dist in hex to loop start
#define M_SSGMIXER 0xf7     // P
#define M_SSGNOISEFQ 0xf8   // wn set register 6 to n, 0-31
#define M_SETFLAG 0xf9      // #n
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
    for(i = 0; i < 57; i++) *sctr++ = 0;
    /*
    fill the rest of the song struct with 0 */
}


void PlaySong()
{
    u8 j;
    struct m88data* songdata = (struct m88data*)&curSong.songdata;

    for(j = 0; j < 3; j++)
    {
        if(curSong.ssg_tone_len[j] <= 0 && !curSong.part_over[3+j])
        {   // This means this channel is ready for another input
            u8* songby = (u16)songdata->partOffsets[3+j] + (u16)curSong.ssg_loc[j];
            if(*songby == M_SSGINSTRUMENT) // set instrument
            { 
                curSong.ssg_loc[j]++;
                //songby = *(songdata->partOffsets[3+j] + curSong.ssg_loc[j]); // which?
                songby++;
                // call set instrument for this channel TODO instrument defs in the mucom88 docs
                curSong.ssg_instr[j] = *songby;
                //SetSSGInstrument(j, songby);
            }
            else if (*songby == M_SETFLAG)
            {
                // f9 n
                songby++;
                curSong.flag = *songby;
                curSong.ssg_loc[j] ++; // skip 1 variable
            }
            else if (*songby == M_SSGNOISEFQ)
            {
                // f8 n 
                songby++;
                SetIOReg(OPN_REG, 6);
                SetIOReg(OPN_DAT, *songby);
                curSong.ssg_loc[j]++; // 1 var
            }
            else if (*songby == M_STARTLOOP)
            {   //    3d 3e 3f
                songby++; // 22 ...
                u16 ofs = *songby; // OK.
                songby++;
                ofs += (*songby) * 256;
//#define HALT while(1){};
                //HALT;
                curSong.loopLocs[3+j] = (u16)songdata->partOffsets[3+j] + (u16)(curSong.ssg_loc[j] + ofs);
                curSong.looping[3+j] = true;
                curSong.ssg_loc[j] += 2; // skip 2 variables
                songby = curSong.loopLocs[3+j] + 1;
                *curSong.loopLocs[3+j] = *songby; // set the workram byte 
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
                    curSong.looping[3+j] = false;
                    curSong.ssg_loc[j] += 4;
                }
                else{
                    // full loop counter is next byte so skip it
                    songby++;
                    songby++; // < low byte
                    curSong.ssg_loc[j] -= (u16)(*songby);
                    songby++;
                    curSong.ssg_loc[j] -= (u16)(*songby * 256);
                }
            }
            else if (*songby == M_SSGVOLUME)
            {
                curSong.ssg_loc[j]++; // set ssg vol
                songby++;
                curSong.ssg_base_vol[j] = (*songby - 2) & 0xf;
            }
            else if (*songby == M_SSGMIXER)
            {
                curSong.ssg_loc[j]++; // set SSG mixer
                //[SSG] ミキサモード Pn (0:発音しない 1:トーン 2:ノイズ 3:トーン+ノイズ) - 9, 8, 1, 0
                songby++;
                curSong.ssg_mix = curSong.ssg_mix & ~(0b00001001 << j); // nullify inverse of songby
                curSong.ssg_mix ^= (*songby << j); // flip songby bits
                SetIOReg(OPN_REG, SSG_MIXER);
                SetIOReg(OPN_DAT, curSong.ssg_mix);
            }
            else if ((*songby > 0) && (*songby < 128)) // play sound !!!
            {
                curSong.ssg_loc[j]++; 
                curSong.ssg_tone_len[j] = *songby; // first byte is length. 18h = 1/8 beat at 120T. LEN = 24, so 
                songby++;// second byte is tone...
                curSong.ssg_oct[j] = (*songby & 0xf0) >> 4; 
                curSong.ssg_tone[j] = (*songby & 0x0f);
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
            else if ((*songby > 0x80) && (*songby < 0xf0))
            {   // rest by setting volume to 0? (not needed when 1 instrument)
                //SetIOReg(OPN_REG, CHA_AMP + j);
                //SetIOReg(OPN_DAT, 0);
                curSong.ssg_tone_len[j] = (*songby & 0x7f); 
            }
            else if (*songby == null)
            {
                SetIOReg(OPN_REG, CHA_AMP + j);
                SetIOReg(OPN_DAT, 0);
                curSong.ssg_loc[j]--;
                curSong.part_over[3+j] = true;
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
        if(curSong.part_over[3+j] == false) //FIXME checks SSG only
        {
            keepPlaying = true;
        }
    }
    if(!keepPlaying) playingSong = false;
}



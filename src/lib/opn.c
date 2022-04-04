
#include <pc88-c.h>

//#include "banklite.h"


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
//#define null (void*)0
#define OPN_MAX_CHANNELS 6
#define OPNA_MAX_CHANNELS 11
#define M_SETFMREG 0xfa

#define Null null 
#define M_PARTDONE Null 

// Organization of 11-channels in m88 files:
// FM1, FM2, FM3
// SSG1 SSG2 SSG3
// RHY
// FM4 FM5 FM6
// (ADPCM)

struct Song currentSong;
fix_16s ticker;
bool playingSong;

// TODO: Prolly wont implement this
void SetSSGInstrument(u8 chn, u8 instr)
{

}


void SetOPNBatch(u8 nextreg, u8* regby)
{
    u8 i;
    for(i = 0; i < 6; i++)
    {
        SetOPNReg(nextreg, *regby);
        nextreg += 0x10;
        regby += 4;
    }
    SetOPNReg((nextreg & 0xf) | 0x90, 0);
}

void SetFMInstrument(u8 chn, Instrument* ins)
{

    u8 nextreg;
    u8* regby;

    regby = (u16)ins + 24;
    SetOPNReg(0xb0+chn, *regby);
    
    nextreg = 0x30+chn;
    regby = (u16)ins + 0;
    SetOPNBatch(nextreg, regby);
    
    nextreg = 0x38+chn;
    regby = (u16)ins + 2;
    SetOPNBatch(nextreg, regby);
    
    nextreg = 0x34+chn;
    regby = (u16)ins + 1;
    SetOPNBatch(nextreg, regby);

    nextreg = 0x3c+chn;
    regby = (u16)ins + 3;
    SetOPNBatch(nextreg, regby);
    
}

fix_16s realTempo;

void LoadSong(const u8* song)
{   // Load song header
    u8 i;
    u8* sctr = song;
    // From the data given on disk, load into the currentSong area in RAM
    struct m88header* sh = (struct m88header*)&currentSong.songheader;
    sh->numSongs = *sctr++;
    sh->FMOfs = *sctr++;
    sh->FMOfs += (*sctr++*256);
    sh->FMOfs += (u16)(song); // this gets position in ram
    sh->binSize = *sctr++;
    sh->binSize += (*sctr++*256);
    // Load song data 
    struct m88data* sd = (struct m88data*)&currentSong.songdata;
    realTempo = FIXED16(*sctr++); // how many bpm (60 << 8) = 15360. each frame, add FIXED16(60) to a counter. 
    // when that counter exceeds FIXED16(sd->tempo), decrement the TONE LENGTH of every note by 1. 
    for(i = 0; i < OPNA_MAX_CHANNELS; i++) // required for m88 compatibility
    {
        sd->partOffsets[i] = *sctr++;
        sd->partOffsets[i] += *sctr++ * 256;
        sd->partOffsets[i] += (u16)(song) + 5; // position in ram, plus header
        sd->partLoops[i] = *sctr++;
        sd->partLoops[i] += *sctr++ * 256;
    }
    sd->dataEndLoc = *sctr++;
    sd->dataEndLoc += *sctr * 256;
    sd->dataEndLoc += (u16)song + 5;
    sd->num_fm_instr = *(sh->FMOfs);
    sh->FMOfs++;
    // end loading in song "headers"
    sctr = &currentSong.partLengths[0];
    
    /*
    fill the rest of the song struct with 0 */
    u8* e = sctr;
    for(u16 c = (u16)sctr; c < ((u16)sctr + sizeof(struct Song)); c++) *e++ = 0;

    SetIOReg(OPN_REG, (CHA_AMP + 0));
    SetIOReg(OPN_REG, 15);
    SetIOReg(OPN_REG, (CHA_AMP + 1));
    SetIOReg(OPN_REG, 15);
    SetIOReg(OPN_REG, (CHA_AMP + 2));
    SetIOReg(OPN_REG, 15);
    FMNoteOff(0xff);
}

/*
Supported MML:

Tone and rest control (a-g, ., r, >, <)
o, t, v, [, ], P, w, #
Octave, tempo, volume, repeat, SSG mixer control, noise frequency, flag byte.
*/

//extern const char song;

u8* ProcessFM(u8 chn, u8* songby_ptr)//, M88Data* songdat)
{
    switch(*songby_ptr){
        case(M_INSTRUMENT):
            currentSong.part_loc[chn]++;
            songby_ptr++;
            //currentSong.fm_instr[chn] = *songby_ptr;
            Instrument* ins = (Instrument*)(u16)( currentSong.songheader.FMOfs + (*songby_ptr * 25) );
            SetFMInstrument(chn, ins);
            break;
        case(M_SETFLAG):
            songby_ptr = MSetFlag(chn, songby_ptr);
            break;
        case(M_STARTLOOP):
            songby_ptr = MStartLoop(chn, songby_ptr);
            break;
        case(M_ENDLOOP):
            songby_ptr = MEndLoop(chn, songby_ptr);
            break;
        case(M_SETFMREG):
            songby_ptr++;
            u8 r = *songby_ptr;
            currentSong.part_loc[chn]++;
            songby_ptr++;
            u8 v = *songby_ptr;
            currentSong.part_loc[chn]++;

            SetOPNReg(r, v);
            break;
        case(M_VOLUME):
            songby_ptr = FMSetVolume(chn, songby_ptr);
            // This doesn't do anything for now. TODO: 
            break;
        case(M_PARTDONE):
            currentSong.chan_disabled[chn] = true;
            currentSong.part_over[chn] = true;
            break;
        default:
            if(*songby_ptr > 0x7f)
            {   //rest 
                FMNoteOff(chn);
                currentSong.fm_tone_len[chn] += (*songby_ptr & 0x7f);
                
            }
            else 
            {   // play note
                songby_ptr = FMPlayNote(chn, songby_ptr);            
            }
            break;
    }
    return songby_ptr;
}

u8* MStartLoop(u8 chn, u8* sb)
{
    sb++; // 22 ...
    u16 ofs = *sb; // OK.
    sb++;
    ofs += (*sb) * 256;
    //#define HALT while(1){};
    //HALT;
    M88Data* songdata = (M88Data*)&currentSong.songdata;
    currentSong.loopLocs[chn] = (u16)(songdata->partOffsets[chn]) + \
        (u16)(currentSong.part_loc[chn] + ofs);
    currentSong.looping[chn] = true;
    currentSong.part_loc[chn] += 2; // skip 2 variables
    sb = currentSong.loopLocs[chn] + 1;
    *currentSong.loopLocs[chn] = *sb; // set the workram byte 
    // ex f5 22 00 = STARTLOOP to +34(22h)
    // 34 bytes ahead of 003dh is 005fh. this is the locateion of the first byte AFTER the 0xF6 opcode (work loop byte).
    return sb;
}

// WIP WIP WIP WIP
//TODO 
u8* MEndLoop(u8 chn, u8* sb)
{
//f6 98 99 21 00
    sb++; // decrement the workram byte
    (*sb)--; //
    if(*sb == 0) 
    {
        sb+=3;
        currentSong.looping[chn] = false;
        currentSong.part_loc[chn] += 4;
    }
    else{
        // full loop counter is next byte so skip it
        sb++;
        sb++; // < low byte
        currentSong.part_loc[chn] -= (u16)(*sb);
        sb++;
        currentSong.part_loc[chn] -= (u16)(*sb * 256);
        currentSong.part_loc[chn]+=2;
        sb = currentSong.part_loc[chn];
        //sb--; // FIXME!
    }
    return sb;
}

u8* SSGSetVolume(u8 chn, u8* sb)
{
    currentSong.part_loc[chn]++; // set ssg vol
    sb++;
    currentSong.ssg_base_vol[chn-3] = (*sb - 2) & 0xf;
    return sb;
}

u8* FMSetVolume(u8 chn, u8* sb)
{
    currentSong.part_loc[chn]++; 
    sb++;
    //currentSong.ssg_base_vol[chn-3] = (*sb - 2) & 0xf;
    return sb;
}

u8* MSetFlag(u8 chn, u8* songby_ptr)
{
    songby_ptr++;
    currentSong.flag = *songby_ptr;
    currentSong.part_loc[chn] ++; // skip 1 variable
    return songby_ptr;
}

//* FM CHANNELS 1 2 3 *//
        //#define M_DETUNE 0xf2    // Dn
        //#define M_GATETIME 0xf3     // qn
        //#define M_SOFTLFO 0xf4   // Mn1,n2,n3,n4
        //#define M_FMPAN 0xf8


void FMNoteOff(u8 chn)
{
    SetOPNReg(0x28, chn & 0xf);
    
}

void FMNoteOn(u8 chn, u16 tone, u8 oct)
{
    SetOPNReg(0xa4 + chn, tone >> 8 | oct << 3);
    SetOPNReg(0xa0 + chn, tone & 0xff);
    SetOPNReg(0x28, 0xf0+ chn);
}

u8* FMPlayNote(u8 chn, u8* sb)
{
    
    FMNoteOff(chn);
    currentSong.part_loc[chn]++;
    currentSong.fm_tone_len[chn] += *sb;
    sb++;
    currentSong.fm_oct[chn] = ((*sb ) >> 4)-1;
    currentSong.fm_tone[chn] = (*sb & 0x0f);
    //currentSong.fm_vol[chn] = currentSong.fm_base_vol[chn];
    // octave << 11 + 26a
    FMNoteOn(chn, fm_octavefour[currentSong.fm_tone[chn]], currentSong.fm_oct[chn]);
    
    return sb;
}

u8* SSGPlayNote(u8 chn, u8* sb)
{
    u8 j = chn - 3;
    currentSong.part_loc[chn]++; 
    currentSong.ssg_tone_len[j] = *sb; // first byte is length. 18h = 1/8 beat at 120T. LEN = 24, so 
    sb++;// second byte is tone...
    currentSong.ssg_oct[j] = (*sb ) >> 4; 
    currentSong.ssg_tone[j] = (*sb & 0x0f);

    currentSong.ssg_vol[j] = currentSong.ssg_base_vol[j];
    SetIOReg(OPN_REG, CHA_AMP + j);
    SetIOReg(OPN_DAT, currentSong.ssg_base_vol[j]);
    //SetIOReg(OPN_REG, 0xc); // coarse envelope frequency adjustment (invesr, so lower = faster repeat)
    //SetIOReg(OPN_DAT, 0b00000111);
    u8 du = j * 2;
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
        default:
            // octave out of range. Do nothing
            break;
    }
    return sb;
}

void PlaySong()
{
    u8 j;
    u8 channel;
    u8 du;
    struct m88data* songdata = (struct m88data*)&currentSong.songdata;
    
    // Play loop A-K:
    // FM1 FM2 FM3 SSG1 SSG2 SSG3 || RHY FM4 FM5 FM6 PCM
    for(channel = 0; channel < OPN_MAX_CHANNELS; channel++) // OPN only for now
    {
        // optimize!
        if(currentSong.chan_disabled[channel] == true)
            continue;

        u8* songby = (u16)songdata->partOffsets[channel] + (u16)currentSong.part_loc[channel];
        
        //
        //FM Channels
        //
        if(channel <= 2)
        {
            if(currentSong.fm_tone_len[channel] <= 0 && !currentSong.part_over[channel])
            {   // This means this channel is ready for another input
                songby = ProcessFM(channel, songby);//, (M88Data*)songdata);
                currentSong.part_loc[channel]++; // move to next byte. 
                // ^ this must be incremented by number of variables in the call.
            }
        }
        //
        //
        //
        else if((channel > 2) && (channel < OPN_MAX_CHANNELS)) // 3 4 5
        {
        // SSG Channels
        //
            j = channel - 3;
            // SSG channels 1-3 are in positions 3-5
            if(currentSong.ssg_tone_len[j] <= 0 && !currentSong.part_over[channel])
            {   // This means this channel is ready for another input
                switch(*songby)
                {
                    case(M_INSTRUMENT):
                        // doesn:t do anything right now
                        currentSong.part_loc[channel]++;
                        songby++;
                        // call set instrument for this channel TODO: instrument defs in the mucom88 docs
                        currentSong.ssg_instr[j] = *songby;
                        //SetSSGInstrument(j, songby);
                        break;
                    case(M_SETFLAG):
                        songby = MSetFlag(channel, songby);
                        // f9 n
                        break;
                    case(M_SSGNOISEFQ):
                        // f8 n 
                        songby++;
                        SetIOReg(OPN_REG, 6);
                        SetIOReg(OPN_DAT, *songby);
                        currentSong.part_loc[channel]++; // 1 var
                        break;
                    case(M_STARTLOOP):
                        //    3d 3e 3f
                        songby = MStartLoop(channel, songby);
                        break;
                    case(M_ENDLOOP):
                        songby = MEndLoop(channel, songby);
                        break;
                    case(M_VOLUME):
                        songby = SSGSetVolume(channel, songby);
                        break;
                    case(M_SSGMIXER):
                        currentSong.part_loc[channel]++; // set SSG mixer
                        //[SSG] ミキサモード Pn (0:発音しない 1:トーン 2:ノイズ 3:トーン+ノイズ) - 9, 8, 1, 0
                        songby++;
                        currentSong.ssg_mix = currentSong.ssg_mix & ~(0b00001001 << j); // nullify inverse of songby
                        currentSong.ssg_mix ^= (*songby << j); // flip songby bits
                        SetIOReg(OPN_REG, SSG_MIXER);
                        SetIOReg(OPN_DAT, currentSong.ssg_mix);
                        break;
                    case(null):
                        SetIOReg(OPN_REG, CHA_AMP + j);
                        SetIOReg(OPN_DAT, 0);
                        currentSong.part_loc[channel]--;
                        currentSong.part_over[channel] = true;
                        currentSong.chan_disabled[channel] = true;
                        break;
                    default:
                        if((*songby) > 0x7f)
                        {
                            currentSong.ssg_tone_len[j] = (*songby & 0x7f); 
                        }
                        else 
                        {
                            songby = SSGPlayNote(channel, songby);
                        }
                        break;
                }
                // Increment channel location pointer
                currentSong.part_loc[channel]++;
            }
        }
        // END SSG SECTION 
        // END OPN SECTION
        else if(channel == 6) // RHY
        { }
        
        else if(channel < 10) // FM4 FM5 FM6
        { } 

        else {}; // PCM
        
    }
    // Use this for tempo-exact timing!
    ticker += FIXED16(60);
    if(ticker >= realTempo)
    {
        
        ticker -= realTempo;
        
        // TIMING BASED STUFF - REQUIRED FOR TONE LENGTHS 
        for(j = 0; j < 3; j++) 
        {
            if(currentSong.fm_tone_len[j] > 0) // TODO: ONLY 3 CHANNELS FOR NOW
            {
                currentSong.fm_tone_len[j]--;
                if(currentSong.fm_tone_len[j] <= 0)
                {
                    FMNoteOff(j);
                }
                // do I do anything else here?
            }
            if(currentSong.ssg_tone_len[j] > 0) // is this tone still playing? 
            {
                currentSong.ssg_tone_len[j]--;  // then tick it down
                if(currentSong.ssg_tone_len[j] < currentSong.ssg_base_vol[j]) // am i OK to fade?
                {
                    currentSong.ssg_fading[j] = true;
                    //SetIOReg(OPN_REG, (CHA_AMP + j));
                    //SetIOReg(OPN_DAT, 0);
                    //currentSong.ssg_vol[j]=0;
                }
                
                if(currentSong.ssg_fading[j] == true) 
                {
                    if(currentSong.ssg_vol[j] > 1) // if I'm not at volume 0 yet... 
                    {
                        SetIOReg(OPN_REG, (CHA_AMP + j));
                        SetIOReg(OPN_DAT, --currentSong.ssg_vol[j]); // fade this channel more!
                        //currentSong.ssg_vol[j]=0;
                    }
                    else 
                        currentSong.ssg_fading[j] = false;
                }
                
            }
            
        }
        // only one instrument.
    }
    // if all 3 parts are done, then stop the song
    bool keepPlaying = false;
    for(j = 0; j < OPN_MAX_CHANNELS; j++)
    {
        if(currentSong.part_over[j] == false) 
        {
            keepPlaying = true;
        }
    }
    if(!keepPlaying) playingSong = false;
}



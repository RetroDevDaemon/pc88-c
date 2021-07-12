#include <pc88-c.h>



static const u16 octavetwo[12] = {
    SSG_C2, SSG_C2S,
    SSG_D2, SSG_D2S, 
    SSG_E2,
    SSG_F2, SSG_F2S,
    SSG_G2, SSG_G2S,
    SSG_A2, SSG_A2S,
    SSG_B2
};
static const u16 octavethree[12] = {
    SSG_C3, SSG_C3S,
    SSG_D3, SSG_D3S, 
    SSG_E3,
    SSG_F3, SSG_F3S,
    SSG_G3, SSG_G3S,
    SSG_A3, SSG_A3S,
    SSG_B3
};
static const u16 octavefour[12] = {
    SSG_C4, SSG_C4S,
    SSG_D4, SSG_D4S,
    SSG_E4,
    SSG_F4, SSG_F4S,
    SSG_G4, SSG_G4S,
    SSG_A4, SSG_A4S,
    SSG_B4
};
static const u16 octavefive[12] = {
    SSG_C5, SSG_C5S,
    SSG_D5, SSG_D5S, 
    SSG_E5,
    SSG_F5, SSG_F5S,
    SSG_G5, SSG_G5S,
    SSG_A5, SSG_A5S,
    SSG_B5
};
static const u16 octavesix[12] = {
    SSG_C6, SSG_C6S,
    SSG_D6, SSG_D6S, 
    SSG_E6,
    SSG_F6, SSG_F6S,
    SSG_G6, SSG_G6S,
    SSG_A6, SSG_A6S,
    SSG_B6
};



void LoadSong(const u8* song)
{   // Load song header
    u8 i;
    u8* sctr = song;
    struct m88header* sh = (struct m88header*)&CURRENT_SONG.songheader;
    sh->numSongs = *sctr++;
    sh->FMOfs = *sctr++;
    sh->FMOfs += (*sctr++*256);
    sh->FMOfs += (u16)(song);
    sh->binSize = *sctr++;
    sh->binSize += (*sctr++*256);
    // Load song data 
    struct m88data* sd = (struct m88data*)&CURRENT_SONG.songdata;
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
    sctr = &CURRENT_SONG.ssg_loc[0];
    for(i = 0; i < 57; i++) *sctr++ = 0;
    /*
    fill the rest of the song struct with 0 */
}

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

void PlaySong()
{
    u8 j;
    struct m88data* songdata = (struct m88data*)&CURRENT_SONG.songdata;

    for(j = 0; j < 3; j++)
    {
        if(CURRENT_SONG.ssg_tone_len[j] <= 0 && !CURRENT_SONG.part_over[3+j])
        {   // This means this channel is ready for another input
            u8* songby = (u16)songdata->partOffsets[3+j] + (u16)CURRENT_SONG.ssg_loc[j];
            if(*songby == M_SSGINSTRUMENT) // set instrument
            { 
                CURRENT_SONG.ssg_loc[j]++;
                //songby = *(songdata->partOffsets[3+j] + CURRENT_SONG.ssg_loc[j]); // which?
                songby++;
                // call set instrument for this channel TODO instrument defs in the mucom88 docs
                CURRENT_SONG.ssg_instr[j] = *songby;
                //SetSSGInstrument(j, songby);
            }
            else if (*songby == M_SETFLAG)
            {
                // f9 n
                songby++;
                CURRENT_SONG.flag = *songby;
                CURRENT_SONG.ssg_loc[j] ++; // skip 1 variable
            }
            else if (*songby == M_SSGNOISEFQ)
            {
                // f8 n 
                songby++;
                SetIOReg(OPN_REG, 6);
                SetIOReg(OPN_DAT, *songby);
                CURRENT_SONG.ssg_loc[j]++; // 1 var
            }
            else if (*songby == M_STARTLOOP)
            {   //    3d 3e 3f
                songby++; // 22 ...
                u16 ofs = *songby; // OK.
                songby++;
                ofs += (*songby) * 256;
#define HALT while(1){};
                //HALT;
                CURRENT_SONG.loopLocs[3+j] = (u16)songdata->partOffsets[3+j] + (u16)(CURRENT_SONG.ssg_loc[j] + ofs);
                CURRENT_SONG.looping[3+j] = true;
                CURRENT_SONG.ssg_loc[j] += 2; // skip 2 variables
                songby = CURRENT_SONG.loopLocs[3+j] + 1;
                *CURRENT_SONG.loopLocs[3+j] = *songby; // set the workram byte 
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
                    CURRENT_SONG.looping[3+j] = false;
                    CURRENT_SONG.ssg_loc[j] += 4;
                }
                else{
                    // full loop counter is next byte so skip it
                    songby++;
                    songby++; // < low byte
                    CURRENT_SONG.ssg_loc[j] -= (u16)(*songby);
                    songby++;
                    CURRENT_SONG.ssg_loc[j] -= (u16)(*songby * 256);
                }
            }
            else if (*songby == M_SSGVOLUME)
            {
                CURRENT_SONG.ssg_loc[j]++; // set ssg vol
                songby++;
                CURRENT_SONG.ssg_base_vol[j] = (*songby - 2) & 0xf;
            }
            else if (*songby == M_SSGMIXER)
            {
                CURRENT_SONG.ssg_loc[j]++; // set SSG mixer
                //[SSG] ミキサモード Pn (0:発音しない 1:トーン 2:ノイズ 3:トーン+ノイズ) - 9, 8, 1, 0
                songby++;
                CURRENT_SONG.ssg_mix = CURRENT_SONG.ssg_mix & ~(0b00001001 << j); // nullify inverse of songby
                CURRENT_SONG.ssg_mix ^= (*songby << j); // flip songby bits
                SetIOReg(OPN_REG, SSG_MIXER);
                SetIOReg(OPN_DAT, CURRENT_SONG.ssg_mix);
            }
            else if ((*songby > 0) && (*songby < 128)) // play sound !!!
            {
                CURRENT_SONG.ssg_loc[j]++; 
                CURRENT_SONG.ssg_tone_len[j] = *songby; // first byte is length. 18h = 1/8 beat at 120T. LEN = 24, so 
                songby++;// second byte is tone...
                CURRENT_SONG.ssg_oct[j] = (*songby & 0xf0) >> 4; 
                CURRENT_SONG.ssg_tone[j] = (*songby & 0x0f);
                CURRENT_SONG.ssg_vol[j] = CURRENT_SONG.ssg_base_vol[j];
                SetIOReg(OPN_REG, CHA_AMP + j);
                SetIOReg(OPN_DAT, CURRENT_SONG.ssg_base_vol[j]);
                //SetIOReg(OPN_REG, 0xc); // coarse envelope frequency adjustment (invesr, so lower = faster repeat)
                //SetIOReg(OPN_DAT, 0b00000111);
                switch(CURRENT_SONG.ssg_oct[j]+1){ 
                    case(2):
                        SetIOReg(OPN_REG, CHA_TONEL + (j*2));
                        SetIOReg(OPN_DAT, octavetwo[CURRENT_SONG.ssg_tone[j]] & 0xff);
                        SetIOReg(OPN_REG, CHA_TONEH + (j*2));
                        SetIOReg(OPN_DAT, (octavetwo[CURRENT_SONG.ssg_tone[j]] & 0xff00) >> 8); 
                        break;
                    case(3):
                        SetIOReg(OPN_REG, CHA_TONEL + (j*2));
                        SetIOReg(OPN_DAT, octavethree[CURRENT_SONG.ssg_tone[j]] & 0xff);
                        SetIOReg(OPN_REG, CHA_TONEH + (j*2));
                        SetIOReg(OPN_DAT, (octavethree[CURRENT_SONG.ssg_tone[j]] & 0xff00) >> 8);                         
                        break;
                    case(4):
                        SetIOReg(OPN_REG, CHA_TONEL + (j*2));
                        SetIOReg(OPN_DAT, octavefour[CURRENT_SONG.ssg_tone[j]] & 0xff);
                        SetIOReg(OPN_REG, CHA_TONEH + (j*2));
                        SetIOReg(OPN_DAT, (octavefour[CURRENT_SONG.ssg_tone[j]] & 0xff00) >> 8); 
                        break;
                    case(5):
                        SetIOReg(OPN_REG, CHA_TONEL + (j*2));
                        SetIOReg(OPN_DAT, octavefive[CURRENT_SONG.ssg_tone[j]] & 0xff);
                        SetIOReg(OPN_REG, CHA_TONEH + (j*2));
                        SetIOReg(OPN_DAT, (octavefive[CURRENT_SONG.ssg_tone[j]] & 0xff00) >> 8); 
                        break;    
                    case(6):
                        SetIOReg(OPN_REG, CHA_TONEL + (j*2));
                        SetIOReg(OPN_DAT, octavesix[CURRENT_SONG.ssg_tone[j]] & 0xff);
                        SetIOReg(OPN_REG, CHA_TONEH + (j*2));
                        SetIOReg(OPN_DAT, (octavesix[CURRENT_SONG.ssg_tone[j]] & 0xff00) >> 8); 
                        break;
                }
            }
            else if ((*songby > 0x80) && (*songby < 0xf0))
            {   // rest by setting volume to 0? (not needed when 1 instrument)
                //SetIOReg(OPN_REG, CHA_AMP + j);
                //SetIOReg(OPN_DAT, 0);
                CURRENT_SONG.ssg_tone_len[j] = (*songby & 0x7f); 
            }
            else if (*songby == null)
            {
                SetIOReg(OPN_REG, CHA_AMP + j);
                SetIOReg(OPN_DAT, 0);
                CURRENT_SONG.ssg_loc[j]--;
                CURRENT_SONG.part_over[3+j] = true;
            }
            CURRENT_SONG.ssg_loc[j]++;
        } 
        //else 
        //{ 
            //u8 tem = CURRENT_SONG.songdata.tempo;
            //CURRENT_SONG.ssg_tone_len[j] -= 1;
        //}
    }
    // Use this for tempo-exact timing!
    SONG_TICK += FIXED16(60);
    if(SONG_TICK >= CURRENT_SONG.songdata.tempo)
    {
        SONG_TICK -= CURRENT_SONG.songdata.tempo;
        for(j = 0; j < 3; j++) 
        {
            if(CURRENT_SONG.ssg_tone_len[j] > 0) // is this tone still playing? 
            {
                CURRENT_SONG.ssg_tone_len[j]--;  // then tick it down
                if(CURRENT_SONG.ssg_tone_len[j] < CURRENT_SONG.ssg_base_vol[j]) // am i OK to fade?
                {
                    CURRENT_SONG.ssg_fading[j] = true;
                }
                if(CURRENT_SONG.ssg_fading[j] == true)
                {
                    if(CURRENT_SONG.ssg_vol[j] > 1) // if I'm not at volume 0 yet... 
                    {
                        SetIOReg(OPN_REG, (CHA_AMP + j));
                        SetIOReg(OPN_DAT, --CURRENT_SONG.ssg_vol[j]); // fade this channel more!
                    }
                    else 
                        CURRENT_SONG.ssg_fading[j] = false;
                }
            }
        }
    }
    // if all 3 parts are done, then stop the song
    bool keepPlaying = false;
    for(j = 0; j < 3; j++)
    {
        if(CURRENT_SONG.part_over[3+j] == false) //FIXME checks SSG only
        {
            keepPlaying = true;
        }
    }
    if(!keepPlaying) SONG_PLAYING = false;
}

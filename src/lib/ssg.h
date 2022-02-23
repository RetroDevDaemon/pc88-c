#ifndef SSG 
#define SSG 

/*! \addtogroup SSG
* @{ 
*/

/*! R0, R1 (b0-3) : 12 bit period for Ch. A\n
*  R2, R3 (b0-3) : 12 bit period for Ch. B\n
*  R4, R5 (b0-3) : 12 bit period for Ch. C\n
*  R6            : 5 bit noise period\n
*  R7            : Output ctl: ~BANNNTTT\n
*        BA: Input enable on IO port A/B\n
*       NNN: Noise enable on Ch. CBA\n
*       TTT:  Tone enable on Ch. CBA\n
*  R8,R9,RA      : b0-3 amplitude (if b4=0)\n
*                  b4 enable/disable envelope\n
*  RB,RC         : 16-bit envelope period\n
*  RD            : 4 bit envelope shape\n
*  RE/RF         : I/O Ports A/B\n
*/
#define OPN_REG 0x44
#define OPN_DAT 0x45
#define OPNA_REG 0x46
#define OPNA_DAT 0x47
#define OPNA_SYS 0x29
// bit 7 - enable FM456/ADPCM

/*![7] = 0b00111110\n
// derivative:\n
// TONE = CLOCK / (16 * PERIOD)\n
// PERIOD = 256*COARSE + FINE\n
// relational:\n
// PERIOD = CLOCK / (TONE * 16)\n
// COARSE + (FINE / 256) = TONE / 256\n
// https://pages.mtu.edu/~suits/notefreqs.html\n
*/
/// Z80 actual speed:
#define CPU4MHZ 3993600
/// 440 Tunings:
#define C2_440  65.41
#define C2S_440 69.30
#define D2_440 73.42
#define D2S_440 77.78
#define E2_440 82.41
#define F2_440 87.31
#define F2S_440 92.50
#define G2_440 98.00
#define G2S_440 103.83
#define A2_440 110.00
#define A2S_440 116.54
#define B2_440 123.47

#define C3_440 130.81
#define C3S_440 138.59
#define D3_440 146.83
#define D3S_440 155.56
#define E3_440 164.81
#define F3_440 174.61
#define F3S_440 185.00
#define G3_440 196.00
#define G3S_440 207.65
#define A3_440 220.00
#define A3S_440 233.08
#define B3_440 246.94

#define C4_440  261.63
#define C4S_440 277.18	
#define D4_440  293.66
#define D4S_440 311.13
#define E4_440  329.63
#define F4_440  349.23
#define F4S_440 369.99
#define G4_440  392
#define G4S_440 415.30
#define A4_440  440
#define A4S_440 466.16
#define B4_440  493.88

#define C5_440  523.25
#define C5S_440 554.37	
#define D5_440  587.33
#define D5S_440 622.25
#define E5_440  659.25
#define F5_440  698.46
#define F5S_440 739.99
#define G5_440  783.99
#define G5S_440 830.61
#define A5_440  880.00
#define A5S_440 932.33
#define B5_440  987.77

#define C6_440  1046.50
#define C6S_440 1108.73
#define D6_440 1174.66
#define D6S_440 1244.51
#define E6_440 1318.51
#define F6_440 1396.91
#define F6S_440 1479.98
#define G6_440 1567.98
#define G6S_440 1661.22
#define A6_440 1760.00	
#define A6S_440 1864.66
#define B6_440 1975.53

/// Bit flags
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
#define FMTONE_ENVPDH 0xb
#define FMTONE_ENVPDL 0xc
#define FMTONE_ENVTYPE 0xd
#define SSG_IOA 0xe
#define SSG_IOB 0xf
#define CHA_NOISE_OFF 0b1000
#define CHB_NOISE_OFF 0b10000
#define CHC_NOISE_OFF 0b100000
#define CHA_TONE 0b1
#define CHB_TONE_OFF 0b10
#define CHC_TONE_OFF 0b100

/// SSG Period inputs:
//3993600 / (65.41 * 32)
#define FMTONE_C2 (const u16)(1908)//(CPU4MHZ / (C2_440 * 32))
#define FMTONE_C2S (const u16)(CPU4MHZ / (C2S_440 * 32))
#define FMTONE_D2 (const u16)(CPU4MHZ / (D2_440 * 32))
#define FMTONE_D2S (const u16)(CPU4MHZ / (D2S_440 * 32))
#define FMTONE_E2 (const u16)(CPU4MHZ / (E2_440 * 32))
#define FMTONE_F2 (const u16)(CPU4MHZ / (F2_440 * 32))
#define FMTONE_F2S (const u16)(CPU4MHZ / (F2S_440 * 32))
#define FMTONE_G2 (const u16)(CPU4MHZ / (G2_440 * 32))
#define FMTONE_G2S (const u16)(CPU4MHZ / (G2S_440 * 32))
#define FMTONE_A2 (const u16)(CPU4MHZ / (A2_440 * 32))
#define FMTONE_A2S (const u16)(CPU4MHZ / (A2S_440 * 32))
#define FMTONE_B2 (const u16)(CPU4MHZ / (B2_440 * 32))

#define FMTONE_C3 (const u16)(CPU4MHZ / (C3_440 * 32))
#define FMTONE_C3S (const u16)(CPU4MHZ / (C3S_440 * 32))
#define FMTONE_D3 (const u16)(CPU4MHZ / (D3_440 * 32))
#define FMTONE_D3S (const u16)(CPU4MHZ / (D3S_440 * 32))
#define FMTONE_E3 (const u16)(CPU4MHZ / (E3_440 * 32))
#define FMTONE_F3 (const u16)(CPU4MHZ / (F3_440 * 32))
#define FMTONE_F3S (const u16)(CPU4MHZ / (F3S_440 * 32))
#define FMTONE_G3 (const u16)(CPU4MHZ / (G3_440 * 32))
#define FMTONE_G3S (const u16)(CPU4MHZ / (G3S_440 * 32))
#define FMTONE_A3 (const u16)(CPU4MHZ / (A3_440 * 32))
#define FMTONE_A3S (const u16)(CPU4MHZ / (A3S_440 * 32))
#define FMTONE_B3 (const u16)(CPU4MHZ / (B3_440 * 32))

#define FMTONE_C4 (const u16)(CPU4MHZ / (C4_440 * 32))
#define FMTONE_C4S (const u16)(CPU4MHZ / (C4S_440 * 32))
#define FMTONE_D4 (const u16)(CPU4MHZ / (D4_440 * 32))
#define FMTONE_D4S (const u16)(CPU4MHZ / (D4S_440 * 32))
#define FMTONE_E4 (const u16)(CPU4MHZ / (E4_440 * 32))
#define FMTONE_F4 (const u16)(CPU4MHZ / (F4_440 * 32))
#define FMTONE_F4S (const u16)(CPU4MHZ / (F4S_440 * 32))
#define FMTONE_G4 (const u16)(CPU4MHZ / (G4_440 * 32))
#define FMTONE_G4S (const u16)(CPU4MHZ / (G4S_440 * 32))
#define FMTONE_A4 (const u16)(CPU4MHZ / (A4_440 * 32))
#define FMTONE_A4S (const u16)(CPU4MHZ / (A4S_440 * 32))
#define FMTONE_B4 (const u16)(CPU4MHZ / (B4_440 * 32))

#define FMTONE_C5 (const u16)(CPU4MHZ / (C5_440 * 32))
#define FMTONE_C5S (const u16)(CPU4MHZ / (C5S_440 * 32))
#define FMTONE_D5 (const u16)(CPU4MHZ / (D5_440 * 32))
#define FMTONE_D5S (const u16)(CPU4MHZ / (D5S_440 * 32))
#define FMTONE_E5 (const u16)(CPU4MHZ / (E5_440 * 32))
#define FMTONE_F5 (const u16)(CPU4MHZ / (F5_440 * 32))
#define FMTONE_F5S (const u16)(CPU4MHZ / (F5S_440 * 32))
#define FMTONE_G5 (const u16)(CPU4MHZ / (G5_440 * 32))
#define FMTONE_G5S (const u16)(CPU4MHZ / (G5S_440 * 32))
#define FMTONE_A5 (const u16)(CPU4MHZ / (A5_440 * 32))
#define FMTONE_A5S (const u16)(CPU4MHZ / (A5S_440 * 32))
#define FMTONE_B5 (const u16)(CPU4MHZ / (B5_440 * 32))

#define FMTONE_C6 (const u16)(CPU4MHZ / (C6_440 * 32))
#define FMTONE_C6S (const u16)(CPU4MHZ / (C6S_440 * 32))
#define FMTONE_D6 (const u16)(CPU4MHZ / (D6_440 * 32))
#define FMTONE_D6S (const u16)(CPU4MHZ / (D6S_440 * 32))
#define FMTONE_E6 (const u16)(CPU4MHZ / (E6_440 * 32))
#define FMTONE_F6 (const u16)(CPU4MHZ / (F6_440 * 32))
#define FMTONE_F6S (const u16)(CPU4MHZ / (F6S_440 * 32))
#define FMTONE_G6 (const u16)(CPU4MHZ / (G6_440 * 32))
#define FMTONE_G6S (const u16)(CPU4MHZ / (G6S_440 * 32))
#define FMTONE_A6 (const u16)(CPU4MHZ / (A6_440 * 32))
#define FMTONE_A6S (const u16)(CPU4MHZ / (A6S_440 * 32))
#define FMTONE_B6 (const u16)(CPU4MHZ / (B6_440 * 32))

void PlaySong();



// FM
#define OPN_TEST_REG 0x21
#define OPN_TIMERSET_1 0x24
#define OPN_TIMERSET_2 0x25
#define OPN_TIMERSET_3 0x26
#define OPN_TIMER_OP 0x27
#define OPN_INPUTDIV_1 0x2d
#define OPN_INPUTDIV_2 0x2e // 2-6 for fm, 1-4 for square
#define OPN_INPUTDIV_3 0x2f
#define OPN_DETUNEMUL 0x30
// 0 1 2 . 4 5 6 . 8 9 A . C D E
#define OPN_FMLEVEL 0x40
#define OPN_FMKEY 0x50
#define OPN_FMDECAY 0x60
#define OPN_FMSUSTAIN 0x70
#define OPN_FMRELEASE 0x80
#define OPN_FMENVELOPE 0x90

static const u16 octavetwo[12] = {
    FMTONE_C2, FMTONE_C2S,
    FMTONE_D2, FMTONE_D2S, 
    FMTONE_E2,
    FMTONE_F2, FMTONE_F2S,
    FMTONE_G2, FMTONE_G2S,
    FMTONE_A2, FMTONE_A2S,
    FMTONE_B2
};
static const u16 octavethree[12] = {
    FMTONE_C3, FMTONE_C3S,
    FMTONE_D3, FMTONE_D3S, 
    FMTONE_E3,
    FMTONE_F3, FMTONE_F3S,
    FMTONE_G3, FMTONE_G3S,
    FMTONE_A3, FMTONE_A3S,
    FMTONE_B3
};
static const u16 octavefour[12] = {
    FMTONE_C4, FMTONE_C4S,
    FMTONE_D4, FMTONE_D4S,
    FMTONE_E4,
    FMTONE_F4, FMTONE_F4S,
    FMTONE_G4, FMTONE_G4S,
    FMTONE_A4, FMTONE_A4S,
    FMTONE_B4
};
static const u16 octavefive[12] = {
    FMTONE_C5, FMTONE_C5S,
    FMTONE_D5, FMTONE_D5S, 
    FMTONE_E5,
    FMTONE_F5, FMTONE_F5S,
    FMTONE_G5, FMTONE_G5S,
    FMTONE_A5, FMTONE_A5S,
    FMTONE_B5
};
static const u16 octavesix[12] = {
    FMTONE_C6, FMTONE_C6S,
    FMTONE_D6, FMTONE_D6S, 
    FMTONE_E6,
    FMTONE_F6, FMTONE_F6S,
    FMTONE_G6, FMTONE_G6S,
    FMTONE_A6, FMTONE_A6S,
    FMTONE_B6
};

struct m88header { 
    u8 numSongs;
    u8* FMOfs;
    u16 binSize;
};
struct m88data { 
    fix_16s tempo; 
    u8* partOffsets[11];
    // FM1 FM2 FM3
    // SSG1 SSG2 SSG3
    // RHY
    // FM4 FM5 FM6
    u16 partLoops[11];
    
    u8* dataEndLoc;
};

struct Song { 
    struct m88header songheader;
    struct m88data songdata;
    s16 partLengths[11]; // TODO fill these in - at the moment not used.
    
    u8 fm_instr[6];
    s8 fm_vol[6];
    s8 fm_tone_len[6];
    u8 fm_oct[6];
    u8 fm_tone[6];
    //u16 fm_loc[6];
    u16 part_loc[11];

    u8 rhy_instr;
    s8 rhy_vol;

    u8 ssg_mix;
    u8 ssg_instr[3];
    s8 ssg_vol[3];
    s8 ssg_tone_len[3]; // Counts down!
    u8 ssg_oct[3];
    u8 ssg_tone[3];
    //u16 part_loc[3]; // 6

    bool part_over[11]; // 11
    bool chan_disabled[11];
    
    bool ssg_fading[3]; // 3
    s8 ssg_base_vol[3]; // 3
    
    bool looping[11]; // 11
    u8* loopLocs[11]; // 22
    u8 flag;
};

typedef struct instrument_lite { //25 bytes
    u8 fb_alg;      //b0
    u8 dt_mult[4];  //30
    u8 totallv[4];  //40
    u8 ks_atkr[4];  //50
    u8 decay[4];    //60
    u8 susr[4];     //70
    u8 susl_relr[4];//80
} InstrumentL;
typedef struct instrument_full { //32 bytes
    u8 fb_alg;
    u8 dt_mult[4];
    u8 totallv[4];
    u8 ks_atkr[4];
    u8 decay[4];
    u8 susr[4];
    u8 susl_relr[4];
    char name[7];
} Instrument;

#endif 
/*! @} */

/*! *** Usage from examples/psg : ********
// Play C4 on Ch A, vol 15, no envelope, mute all other
<pre>
SetIOReg(OPN_REG, CHA_TONEL);
SetIOReg(OPN_DAT, FMTONE_C4 & 0xff);
SetIOReg(OPN_REG, CHA_TONEH);
SetIOReg(OPN_DAT, (FMTONE_C4 & 0xf00) >> 8);
//0b00111110:
SetIOReg(OPN_REG, SSG_MIXER);
SetIOReg(OPN_DAT, ~(CHA_TONE | 0xc0));
SetIOReg(OPN_REG, CHA_AMP);
SetIOReg(OPN_DAT, 15);
</pre>
**************************************/
// max is 4095 or 4095 = 3993600 / 16X
//65520X = 3993600 or B1, but C2 is better starting


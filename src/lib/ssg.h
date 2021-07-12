/* R0, R1 (b0-3) : 12 bit period for Ch. A
*  R2, R3 (b0-3) : 12 bit period for Ch. B
*  R4, R5 (b0-3) : 12 bit period for Ch. C
*  R6            : 5 bit noise period
*  R7            : Output ctl: ~BANNNTTT
*        BA: Input enable on IO port A/B
*       NNN: Noise enable on Ch. CBA
*       TTT:  Tone enable on Ch. CBA
*  R8,R9,RA      : b0-3 amplitude (if b4=0)
*                  b4 enable/disable envelope
*  RB,RC         : 16-bit envelope period
*  RD            : 4 bit envelope shape
*  RE/RF         : I/O Ports A/B
*/
#define OPN_REG 0x44
#define OPN_DAT 0x45
//[7] = 0b00111110
// derivative:
// TONE = CLOCK / (16 * PERIOD)
// PERIOD = 256*COARSE + FINE
// relational:
// PERIOD = CLOCK / (TONE * 16)
// COARSE + (FINE / 256) = TONE / 256
// https://pages.mtu.edu/~suits/notefreqs.html

// Z80 actual speed:
#define CPU4MHZ 3993600
// 440 Tunings:
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

// Bit flags
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

// SSG Period inputs:
#define SSG_C2 (u16)(CPU4MHZ / (C2_440 * 32))
#define SSG_C2S (u16)(CPU4MHZ / (C2S_440 * 32))
#define SSG_D2 (u16)(CPU4MHZ / (D2_440 * 32))
#define SSG_D2S (u16)(CPU4MHZ / (D2S_440 * 32))
#define SSG_E2 (u16)(CPU4MHZ / (E2_440 * 32))
#define SSG_F2 (u16)(CPU4MHZ / (F2_440 * 32))
#define SSG_F2S (u16)(CPU4MHZ / (F2S_440 * 32))
#define SSG_G2 (u16)(CPU4MHZ / (G2_440 * 32))
#define SSG_G2S (u16)(CPU4MHZ / (G2S_440 * 32))
#define SSG_A2 (u16)(CPU4MHZ / (A2_440 * 32))
#define SSG_A2S (u16)(CPU4MHZ / (A2S_440 * 32))
#define SSG_B2 (u16)(CPU4MHZ / (B2_440 * 32))

#define SSG_C3 (u16)(CPU4MHZ / (C3_440 * 32))
#define SSG_C3S (u16)(CPU4MHZ / (C3S_440 * 32))
#define SSG_D3 (u16)(CPU4MHZ / (D3_440 * 32))
#define SSG_D3S (u16)(CPU4MHZ / (D3S_440 * 32))
#define SSG_E3 (u16)(CPU4MHZ / (E3_440 * 32))
#define SSG_F3 (u16)(CPU4MHZ / (F3_440 * 32))
#define SSG_F3S (u16)(CPU4MHZ / (F3S_440 * 32))
#define SSG_G3 (u16)(CPU4MHZ / (G3_440 * 32))
#define SSG_G3S (u16)(CPU4MHZ / (G3S_440 * 32))
#define SSG_A3 (u16)(CPU4MHZ / (A3_440 * 32))
#define SSG_A3S (u16)(CPU4MHZ / (A3S_440 * 32))
#define SSG_B3 (u16)(CPU4MHZ / (B3_440 * 32))

#define SSG_C4 (u16)(CPU4MHZ / (C4_440 * 32))
#define SSG_C4S (u16)(CPU4MHZ / (C4S_440 * 32))
#define SSG_D4 (u16)(CPU4MHZ / (D4_440 * 32))
#define SSG_D4S (u16)(CPU4MHZ / (D4S_440 * 32))
#define SSG_E4 (u16)(CPU4MHZ / (E4_440 * 32))
#define SSG_F4 (u16)(CPU4MHZ / (F4_440 * 32))
#define SSG_F4S (u16)(CPU4MHZ / (F4S_440 * 32))
#define SSG_G4 (u16)(CPU4MHZ / (G4_440 * 32))
#define SSG_G4S (u16)(CPU4MHZ / (G4S_440 * 32))
#define SSG_A4 (u16)(CPU4MHZ / (A4_440 * 32))
#define SSG_A4S (u16)(CPU4MHZ / (A4S_440 * 32))
#define SSG_B4 (u16)(CPU4MHZ / (B4_440 * 32))

#define SSG_C5 (u16)(CPU4MHZ / (C5_440 * 32))
#define SSG_C5S (u16)(CPU4MHZ / (C5S_440 * 32))
#define SSG_D5 (u16)(CPU4MHZ / (D5_440 * 32))
#define SSG_D5S (u16)(CPU4MHZ / (D5S_440 * 32))
#define SSG_E5 (u16)(CPU4MHZ / (E5_440 * 32))
#define SSG_F5 (u16)(CPU4MHZ / (F5_440 * 32))
#define SSG_F5S (u16)(CPU4MHZ / (F5S_440 * 32))
#define SSG_G5 (u16)(CPU4MHZ / (G5_440 * 32))
#define SSG_G5S (u16)(CPU4MHZ / (G5S_440 * 32))
#define SSG_A5 (u16)(CPU4MHZ / (A5_440 * 32))
#define SSG_A5S (u16)(CPU4MHZ / (A5S_440 * 32))
#define SSG_B5 (u16)(CPU4MHZ / (B5_440 * 32))

#define SSG_C6 (u16)(CPU4MHZ / (C6_440 * 32))
#define SSG_C6S (u16)(CPU4MHZ / (C6S_440 * 32))
#define SSG_D6 (u16)(CPU4MHZ / (D6_440 * 32))
#define SSG_D6S (u16)(CPU4MHZ / (D6S_440 * 32))
#define SSG_E6 (u16)(CPU4MHZ / (E6_440 * 32))
#define SSG_F6 (u16)(CPU4MHZ / (F6_440 * 32))
#define SSG_F6S (u16)(CPU4MHZ / (F6S_440 * 32))
#define SSG_G6 (u16)(CPU4MHZ / (G6_440 * 32))
#define SSG_G6S (u16)(CPU4MHZ / (G6S_440 * 32))
#define SSG_A6 (u16)(CPU4MHZ / (A6_440 * 32))
#define SSG_A6S (u16)(CPU4MHZ / (A6S_440 * 32))
#define SSG_B6 (u16)(CPU4MHZ / (B6_440 * 32))

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

/***** Usage from examples/psg : ********
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
**************************************/
// max is 4095 or 4095 = 3993600 / 16X
//65520X = 3993600 or B1, but C2 is better starting

static struct Song CURRENT_SONG;
static signed int SONG_TICK = 0;
static bool SONG_PLAYING = false;

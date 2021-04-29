// pc88-c.h 

/// A brief C framework for PC-88 development

//  CC0
// Written by @RetroDevDiscord
//  with help from
// http://www.maroon.dti.ne.jp/youkan/pc88/
//  and http://mydocuments.g2.xrea.com/
#ifndef PC88C
#define PC88C

// typedefs
typedef unsigned char u8;           
typedef unsigned int u16;
typedef volatile unsigned char vu8;
typedef volatile unsigned int vu16;
typedef signed char s8;
typedef signed int s16;
typedef volatile signed char vs8;
typedef volatile unsigned int vs16;
typedef const u8 String[];
typedef unsigned char bool;
typedef unsigned long u32;
typedef signed long s32;
typedef unsigned long long u64;
typedef signed long long s64;
#define true 1
#define false 0
#define bit(n) (1 << n)
#define null 0 
#define NULL null 

// PlanarBitmap
//  Consists of three pointers to the three RGB plane data, uncompressed
//  and a width (divided by 8) and height.
typedef const struct planarBitmap {
    const u8* r;
    const u8* g;
    const u8* b;
    u8 w;
    u8 h;
} PlanarBitmap, Sprite;

// 4 bytes
typedef struct xypos { 
    signed int x;
    signed int y;
} XYpos;


#define SCREEN_TXT_BASE 0xf3c8
#define SCREEN_ATTR_BASE SCREEN_TXT_BASE + 80

//////////////////////////
// KEYBOARD MATRIX  
#include "lib/kb-matrix.h" 
//////////////////////////

///////////////////////
// PC BEEPER INFO
#include "lib/beeper.h" 
///////////////////////

///////////////////////
// IO REGISTER INFO
#include "lib/ioregs.h"
// Please read me! Lots of info!
///////////////////////

#define RS232_IRQ 0xf300
#define CRTC_IRQ 0xf302
#define CLOCK_IRQ 0xf304
#define USR_IRQ4 0xf306
#define USR_IRQ3 0xf308
#define USR_IRQ2 0xf30a
#define FM_IRQ1 USR_IRQ4 
#define FM_IRQ2 USR_IRQ3 

#define VBLANK_SIGNAL 0b100000
#define TMODE_BIT 0b10000
#define TEXT_BWMODE_BIT 0b10
#define TEXT_80COL_BIT 0b1
#define TXT_COLOR_SET 0b1000
#define TXT_ATTR_SET 0

#define CLR_WHITE 7
#define CLR_YELLOW 6
#define CLR_CYAN 5
#define CLR_GREEN 4
#define CLR_FUSCHIA 3
#define CLR_RED 2
#define CLR_BLUE 1
#define CLR_BLACK 0

#define PLANE_RED 1
#define PLANE_BLUE 0
#define PLANE_GREEN 2

// Function headers
// CRT0
void __init();
void main();
inline void putchr(u8 c);
/**/inline void putchr40(u8 c);
void print(String str);
/**/void print40(String str);

// TEXTMODE
void SetTextAttribute(u8 x, u8 y, u8 attr);
void ClearAttributeRam();       // Resets to the bios defaults.
void SetCursorPos(u8 x, u8 y);
/**/void SetCursorPos40(u8 x, u8 y);

// IOREGS
//#define SetIOReg(r, v) r = v;//__asm__("ld a, %d", r) 
u8 ReadIOReg(u8 r);
void SetIOReg(u8 r, u8 v);
/* Attributes must be set in ascending X order on each row. 
    Attribute #0 on each row is ALWAYS read as X=0, regardless of its actual value. */

// WAITVBLANK
void Wait_VBLANK();
// GETKEYDOWN
bool GetKeyDown(u8 SCANCODE);
// DRAW
void DrawPlaneBMP(const u8* img, u8 plane, u16 x, u16 y, u8 w, u8 h);
void SetPixel(u16 x, u8 y, u8 c);
inline void EnableALU();
inline void DisableALU();
inline void ExpandedGVRAM_On();
inline void ExpandedGVRAM_Off();
// DISKLOAD
void DiskLoad(u8* dst, u8 track, u8 sector, u8 numSecs, u8 drive); 
// BEEP
void beep(u16 tone, u8 length);
// VRAM_UTIL
void EraseVRAMArea(XYpos* xy, u8 w, u8 h);

// SSG
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
// SSG Period inputs:
#define SSG_C4 (u16)(CPU4MHZ / (C4_440 * 32))
#define SSG_D4 (u16)(CPU4MHZ / (D4_440 * 32))
#define SSG_E4 (u16)(CPU4MHZ / (E4_440 * 32))
#define SSG_F4 (u16)(CPU4MHZ / (F4_440 * 32))
#define SSG_G4 (u16)(CPU4MHZ / (G4_440 * 32))
#define SSG_A4 (u16)(CPU4MHZ / (A4_440 * 32))
#define SSG_B4 (u16)(CPU4MHZ / (B4_440 * 32))
// max is 4095 or 4095 = 3993600 / 16X
//65520X = 3993600 or B1, but C2 is better starting
#define SSG_C2 (u16)(CPU4MHZ / (C2_440 * 32))



// SYS
//;bit7:0=8MHz 1=4MHz (FH以降)
//;bit76.w:10=V1S 11=V1H 01=V2
#define V1S_MODE_FLAG   (2<<14)
#define V2_MODE_FLAG    (1<<14)
#define V1H_MODE_FLAG   (3<<14)
#define FOURMHZ_FLAG    (0x80)
u8 GetN88Rom();
u16 GetSysMode();

// MATH
u8 abs(s8 n);
u16 abs16(s16 n);
u8 rand();
u16 rand16();

#define SetBGColor(c) SetIOReg(0x52, c << 4);
#define SetBorderColor(c) SetIOReg(0x52, c); // PC8001 only?
#define SETBANK_BLUE() SetIOReg(0x5c, 0xff);
#define SETBANK_RED() SetIOReg(0x5d, 0xff);
#define SETBANK_GREEN() SetIOReg(0x5e, 0xff);
#define SETBANK_MAINRAM() SetIOReg(0x5f, 0xff);

#define DrawPlanarBitmap(pb, x, y) \
        DrawPlaneBMP(pb->r, PLANE_RED, x, y, pb->w, pb->h); \
        DrawPlaneBMP(pb->g, PLANE_GREEN, x, y, pb->w, pb->h); \
        DrawPlaneBMP(pb->b, PLANE_BLUE, x, y, pb->w, pb->h);
// Used in COLOR TEXT mode. Defines color and semigraphic toggle.
#define COLORMODE_SET(color, semigraphic) \
    (u8)((color<<5)|(semigraphic<<4)|0b1000)
// Used in COLOR TEXT mode. Defines text attributes.
#define COLORMODE_ATTR(underline, upperline, reverse, blink, hidden) \
    (u8)((underline<<5)|(upperline<<4)|(reverse<<2)|(blink<<1)|(hidden))
// Used in B&W TEXT mode. Bits 7, 5 and 4 must all be set for BW semigraphic mode.
#define BWMODE_ATTR(underline, upperline, reverse, blink, hidden) \
    (u8)((underline<<5)|(upperline<<4)|(reverse<<2)|(blink<<1)|(hidden))
#define ATTR_BW_SEMIGRAPHIC 0b10011000

#define IRQ_OFF __asm di __endasm;
#define IRQ_ON __asm ei __endasm;
#define HALT __asm halt __endasm;
#define BREAKPOINT HALT 

#endif 
// pc88-c.h 

/// A brief C framework for PC-88 development

//  CC0
// Written by @RetroDevDiscord
//  with help from
// http://www.maroon.dti.ne.jp/youkan/pc88/
//  and http://mydocuments.g2.xrea.com/


// typedefs
typedef unsigned char u8;           
typedef unsigned int u16;
typedef volatile unsigned char vu8;
typedef volatile unsigned int vu16;
typedef const u8 String[];
typedef unsigned char bool;
#define true 1
#define false 0
#define bit(n) (1 << n)

// PlanarBitmap
//  Consists of three pointers to the three RGB plane data, uncompressed
//  and a width (divided by 8) and height.
typedef const struct planarBitmap {
    const u8* r;
    const u8* g;
    const u8* b;
    u8 w;
    u8 h;
} PlanarBitmap;

// PC88 Registers and other defs
//
#define SCREEN_TXT_BASE 0xf3c8
#define SCREEN_ATTR_BASE SCREEN_TXT_BASE + 80

#define SYS_CTL_REGA (u8)0x30
#define SYS_CTL_REGB (u8)0x31
#define MODE_SET_REGISTER (u8)0x32
#define EXPANDED_ALU_CTRL (u8)0x34
#define EXPANDED_GVRAM_CTRL (u8)0x35
#define SYS_MODE_SENSE (u8)0x40
#define STROBE_PORT (u8)0x40

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

#define PAL_REG0 0x54
#define PAL_REG1 0x55
#define PAL_REG2 0x56
#define PAL_REG3 0x57
#define PAL_REG4 0x58
#define PAL_REG5 0x59
#define PAL_REG6 0x5a
#define PAL_REG7 0x5b

#define COLORMODE_SET(color, semigraphic) \
    (u8)((color<<5)|(semigraphic<<4)|0b1000)
// Used in COLOR TEXT mode. Defines color and semigraphic toggle.
#define COLORMODE_ATTR(underline, upperline, reverse, blink, hidden) \
    (u8)((underline<<5)|(upperline<<4)|(reverse<<2)|(blink<<1)|(hidden))
// Used in COLOR TEXT mode. Defines text attributes.
#define BWMODE_ATTR(underline, upperline, reverse, blink, hidden) \
    (u8)((underline<<5)|(upperline<<4)|(reverse<<2)|(blink<<1)|(hidden))
// Used in B&W TEXT mode. Bits 7, 5 and 4 must all be set for BW semigraphic mode.
#define ATTR_BW_SEMIGRAPHIC 0b10011000


// Function headers
void main();
void putchr(u8 c);
/**/void putchr40(u8 c);
void print(String str);
/**/void print40(String str);
u8 ReadIOReg(u8 r);
void SetIOReg(u8 r, u8 v);
void SetTextAttribute(u8 x, u8 y, u8 attr);
/* Attributes must be set in ascending X order on each row. 
    Attribute #0 on each row is ALWAYS read as X=0, regardless of its actual value. */
void ClearAttributeRam();       // Resets to the bios defaults.
void SetCursorPos(u8 x, u8 y);
/**/void SetCursorPos40(u8 x, u8 y);
void Wait_VBLANK();
void DrawPlaneBMP(const u8* img, u8 plane, u16 x, u16 y, u8 w, u8 h);
void SetPixel(u16 x, u8 y, u8 c);

#define SetBGColor(c) SetIOReg(0x52, c << 4);
#define SetBorderColor(c) SetIOReg(0x52, c);
#define SETBANK_BLUE() SetIOReg(0x5c, 0xff);
#define SETBANK_RED() SetIOReg(0x5d, 0xff);
#define SETBANK_GREEN() SetIOReg(0x5e, 0xff);
#define SETBANK_MAINRAM() SetIOReg(0x5f, 0xff);
#define PLANE_RED 1
#define PLANE_BLUE 0
#define PLANE_GREEN 2
#define DrawPlanarBitmap(pb, x, y) \
        DrawPlaneBMP(pb->r, PLANE_RED, x, y, pb->w, pb->h); \
        DrawPlaneBMP(pb->g, PLANE_GREEN, x, y, pb->w, pb->h); \
        DrawPlaneBMP(pb->b, PLANE_BLUE, x, y, pb->w, pb->h);

#define IRQ_OFF __asm di __endasm;
#define IRQ_ON __asm ei __endasm;
    
// globals 
vu8* SCREEN_POINTER;


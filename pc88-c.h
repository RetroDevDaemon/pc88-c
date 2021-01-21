// pc88-c.h 

// typedefs
typedef unsigned char u8;
typedef unsigned int u16;
typedef volatile unsigned char vu8;
typedef const u8 String[];
typedef unsigned char bool;
#define true 1
#define false 0
#define bit(n) (1 << n)

// PC88 Defines
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

#define COLORMODE_SET(color, semigraphic) (u8)((color<<5)|(semigraphic<<4)|0b1000)
#define COLORMODE_ATTR(underline, upperline, reverse, blink, hidden) (u8)((underline<<5)|(upperline<<4)|(reverse<<2)|(blink<<1)|(hidden))
#define BWMODE_ATTR(underline, upperline, reverse, blink, hidden) (u8)((underline<<5)|(upperline<<4)|(reverse<<2)|(blink<<1)|(hidden))
#define ATTR_BW_SEMIGRAPHIC 0b10011000


// Function headers
void putchr(u8 c);
/**/void putchr40(u8 c);
void print(String str);
/**/void print40(String str);
u8 ReadIOReg(u8 r);
void SetIOReg(u8 r, u8 v);
void SetTextAttribute(u8 x, u8 y, u8 attr);
void ClearAttributeRam();
void SetCursorPos(u8 x, u8 y);
/**/void SetCursorPos40(u8 x, u8 y);
void Wait_VBLANK();
#define SetBGColor(c) SetIOReg(0x52, c << 4)
#define SetBorderColor(c) SetIOReg(0x52, c)
#define SETBANK_BLUE() SetIOReg(0x5c, 1)
#define SETBANK_RED() SetIOReg(0x5d, 1)
#define SETBANK_GREEN() SetIOReg(0x5e, 1)
#define SETBANK_MAINRAM() SetIOReg(0x5f, 1)
// globals 
vu8* SCREEN_POINTER;
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

// Read only
#define KB_MATRIX_REG_0 0x00
#define KB_MATRIX_REG_1 0x01
#define KB_MATRIX_REG_2 0x02
#define KB_MATRIX_REG_3 0x03
#define KB_MATRIX_REG_4 0x04
#define KB_MATRIX_REG_5 0x05
#define KB_MATRIX_REG_6 0x06
#define KB_MATRIX_REG_7 0x07
#define KB_MATRIX_REG_8 0x08
#define KB_MATRIX_REG_9 0x09
#define KB_MATRIX_REG_A 0x0A
#define KB_MATRIX_REG_B 0x0B
#define KB_MATRIX_REG_C 0x0C
#define KB_MATRIX_REG_D 0x0D
#define KB_MATRIX_REG_E 0x0E
// KB scancodes!
//              0xRB
// R register B bit clear
#define KB_PAD0 0x00
#define KB_PAD1 0x01
#define KB_PAD2 0x02
#define KB_PAD3 0x03
#define KB_PAD4 0x04
#define KB_PAD5 0x05
#define KB_PAD6 0x06
#define KB_PAD7 0x07
#define KB_PAD8 0x10 

#define KB_PAD9 0x11
#define KB_PADASTERISK 0x12
#define KB_PADPLUS 0x13
#define KB_PADEQU 0x14
#define KB_PADPERIOD 0x15
#define KB_PADCOMMA 0x16
#define KB_RETURN 0x17

#define KB_AT 0x20
#define KB_A  0x21
#define KB_B  0x22
#define KB_C  0x23
#define KB_D  0x24
#define KB_E  0x25
#define KB_F  0x26
#define KB_G  0x27

#define KB_H  0x30
#define KB_I  0x31
#define KB_J  0x32
#define KB_K  0x33
#define KB_L  0x34
#define KB_M  0x35
#define KB_N  0x36
#define KB_O  0x37

#define KB_P  0x40
#define KB_Q  0x41
#define KB_R  0x42
#define KB_S  0x43
#define KB_T  0x44
#define KB_U  0x45
#define KB_V  0x46
#define KB_W  0x47

#define KB_X  0x50
#define KB_Y  0x51
#define KB_Z  0x52
#define KB_OPENBRACE  0x53
#define KB_YEN  0x54
#define KB_BACKSLASH  0x54
#define KB_CLOSEBRACE  0x55
#define KB_CARET  0x56
#define KB_MINUS  0x57
#define KB_HYPHEN  0x57

#define KB_0  0x60
#define KB_1  0x61
#define KB_2  0x62
#define KB_3  0x63
#define KB_4  0x64
#define KB_5  0x65
#define KB_6  0x66
#define KB_7  0x67

#define KB_8  0x70
#define KB_9  0x71
#define KB_COLON  0x72
#define KB_SEMICOLON  0x73
#define KB_COMMA  0x74
#define KB_PERIOD  0x75
#define KB_FORWARDSLASH  0x76
#define KB_UNDERSCORE  0x77

#define KB_HOMECLR  0x80
#define KB_ARROWUP  0x81
#define KB_ARROWRIGHT  0x82
#define KB_INSDEL  0x83
#define KB_GRPH  0x84
#define KB_KANA  0x85
#define KB_SHIFT  0x86
#define KB_CTRL  0x87

#define KB_STOP  0x90
#define KB_F1  0x91
#define KB_F2  0x92
#define KB_F3  0x93
#define KB_F4  0x94
#define KB_F5  0x95
#define KB_SPACE  0x96
#define KB_ESC  0x97

#define KB_HTAB  0xA0
#define KB_ARROWDOWN  0xA1
#define KB_ARROWLEFT  0xA2
#define KB_HELP  0xA3
#define KB_COPY  0xA4
#define KB_PADMINUS  0xA5
#define KB_PADDIVIDE  0xA6
#define KB_CAPSLOCK  0xA7

#define KB_ROLLUP  0xB0
#define KB_ROLLDOWN  0xB1
// WARNING: 0xC0 ~ 0xF0 only work on PC-8801 FH+
#define KB_F6  0xC0
#define KB_F7  0xC1
#define KB_F8  0xC2
#define KB_F9  0xC3
#define KB_F10  0xC4
#define KB_BS  0xC5
#define KB_INS  0xC6
#define KB_DEL  0xC7
#define KB_HENKOU  0xD0
#define KB_KETTEI  0xD1
#define KB_PC  0xD2
#define KB_ZENKAKU  0xD3
#define KB_MAINRETURN  0xE0
#define KB_PADRETURN  0xE1
#define KB_LEFTSHIFT  0xE2
#define KB_RIGHTSHIFT  0xE3

#define PD_CTL_REG      0x10         // W - Printer Out OR Calendar Lock (uPD1990AC)
// If PD1990AC - bit0 - bit2 : Command output
//               bit3 : Data output
#define USART_DATA_REG  0x20              // R/W (pd8251C)
#define USART_CTL_REG   0x21              // R/W
/* SYSTEM CONTROL REGISTER A //
**READ: Dipswitch status
Bit	7	6	5	    4	    3	    2	    1	    0
           SW1-S5	SW1-S4	SW1-S3	SW1-S2	SW1-S1	SW4-S1

SW1-S5	DEL code response: 0-OK 1-Ignore
SW1-S4	S parameter: 0-ON 1-OFF
SW1-S3	Text lines: 0-25L 1-20L
SW1-S2	Text columns: 0-80C 1-40C
SW1-S1	Boot mode: 0-Terminal 1-BASIC
SW4-S1	BASIC mode: 0-NBASIC 1-N88-BASIC
**WRITE: System Control Port
Bit	7	6	54	3	    2	1	    0
            BS	MTON	CDS	COLOR	40

BS	USART Channel control:
    00: CMT 600baud     10: RS-232C
    01: CMT 1200baud    11: RS-232C
MTON Motor Control: 0-OFF 1-Motor ON
CDS	Carrier control: 0-Space(1200hz) 1-Mark(2400hz)
COLOR Text screen color mode: 0-Color 1-B&W
40 40 Col mode: 0-40c 1-80c
//*/
#define SYS_CTL_REGA    0x30
/* SYSTEM CONTROL REGISTER B //
** WRITE: System Control Port
Bit	    7	6	5	    4	    3	    2	    1	    0
                25LINE	HCOLOR	GRAPH	RMODE	MMODE	200LINE

25LINE Hi-res 25 row mode: 0-All other modes 1-ON
HCOLOR Graphic color mode: 0-B&W 1-Color
GRAPH Graphic control: 0-Graphic screens off 1-ON
RMODE ROM mode: 0-N88-BASIC 1-N-BASIC
MMODE RAM mode: 0-ROM/RAM 1-64K RAM
200LINE	Hi-res CRT mode: 0-640×400 1-640x200
  Recommended values: 0b000110x1 
** READ: Dipswitch status
Bit	7	    6	    5	    4	    3	    2	    1	    0
    SW4-S2	SW3-S0	SW2-S6	SW2-S5	SW2-S4	SW2-S3	SW2-S2	SW2-S1

SW4-S2 V1/V2 Mode: 0-V2 1-V1
SW3-S0 Hispeed mode: 0-Off (V1S) 1-On (V1H, V2)
SW2-S6 Transfer direction: 0-Half-duplex 1-Full-duplex
SW2-S5 X parameter: 0-ON 1-OFF
SW2-S4 Stop bit length: 0-2 bits 1-1 bit
SW2-S3 Data bit length: 0-8 bits 1-7 bits
SW2-S2 Parity bit: 0-Even 1-Odd
SW2-S1 Parity check: 0-Party 1-No parity
//*/
#define SYS_CTL_REGB    0x31
/* MODE SET REGISTER
[R/W] These registers are not decoded on systems
    before the PC-8801mkII. On models after SR,
    double-check this register on boot.
Bit	7	    6	    5	    4	    32	10
    SINTM	GVAM	PMODE	TMODE	AVC	EROMSL

SINTM Sound IRQ: 0-Enable 1-Disable
GVAM Graphics VRAM access mode: 0-Independant 1-Expanded
PMODE Palette mode: 0-Digital 8 color 1-Analog 512 color
TMODE FastRAM access mode (V1H/V2 only):
  0: F000h～FFFFh FastRAM
  1: F000h～FFFFh Main RAM
AVC	Screen output mode: 
  00-TV/Video out   10-Computer out
  01-(off)          11-Option mode
EROMSL Select expaned ROM bank (0-3) 
INDEPENDANT mode is the same as V1 mode. GVRAM is written separately.
EXPANDED mode (SR+ only) lets you access all planes simultaneously.
*NOTE: When VRAM is selected, overall processing speed is decreased.
  Remember to page in Main RAM!
Default values: xxxx1001
*/
#define ALU_MODE_CTRL 0x32
/* Write only
Bit	7	6	    5	    4	    3	2	    1	    0
        ALU21	ALU11	ALU01	    ALU20	ALU10	ALU00
        GV2H    GV1H    GV0H        GV2L    GV1L    GV0L
GV0 - ALU mode for GVRAM bank 0
GV1 - ALU mode for GVRAM bank 1
GV2 - ALU mode for GVRAM bank 2
 Bits arranged as H:L
00: Bit reset
01: Bit set
10: Bit invert
11: No op */
#define EXPANDED_ALU_CTRL 0x34 
/* Expanded Access Mode GVRAM Control
bit  7      6   5 4     3   2   1   0
     GAM    -   GDM     -   GC  RC  BC

GAM - GVRAM Access Mode
 0 = Main RAM, 1 = GVRAM
GDM - Graphic data multiplexer control
 00 = Write all planes (check port $34)
 01 = Write last VRAM read data to all 3 planes
 10 = Transfer R plane to B
 11 = Transfer B plane to R

* If GDM = 00, you can perform logical operations as explained
using register $34. It appears this mode does not use a buffer.
(There are ways to utilize this to advantage combined with GDM=01 
but I am unclear)

* If GDM = 01, when a single byte is read from VRAM, all 3 planar
data parts are read into a buffer. When writing, the stored buffer
data is written to all 3 planes simultaneously. You can use this to
copy within VRAM without extraneous calculations (ie port $34),
which can be used to create scrolling.

* GC/RC/BC = Bit comp data. Bits written to these fields act as a
color plane filter to VRAM reads. E.g. if bits2-0 are 011 (purple),
the pixel data read from VRAM will be 1 if purple, or 0 if not.

* When ALU EXPANDED access mode is on, reading data from VRAM is
not stored in a register. What is loaded in the register is
'comparison data'; VRAM itself is stored in a hidden buffer.

ld a,%10010000  ; gvram access on, 3 plane buffer r/w mode
out ($35),a 
ld a,($c000)	; this is bit comp data, but the
ld ($c0002),a   ; operation commands a vram 'copy'.
*/
#define EXPANDED_GVRAM_CTRL 0x35  // W only
// Bit 6 - Joystick port output
#define SYS_MODE_SENSE 0x40   // Write
#define STROBE_PORT 0x40      // Read

#define FM_REG_0 0x44     // <FR/MR - OPN, FH/MH> - OPNA SSG/FM1-3
#define FM_REG_1 0x45     // <FR/MR - OPN, FH/MH> - OPNA SSG/FM1-3
#define FM_REG_2 0x46     // FH/MH - OPNA ADPCM/FM4-6
#define FM_REG_3 0x47     // FH/MH - OPNA ADPCM/FM4-6

#define CRTC_CTLREG_A 0x50     // pd3301AC control ports 
#define CRTC_CTLREG_B 0x51
// W - bits 4-6 only.
// bit  7   6 5 4   3   2   1
//          G R B                   
#define BG_COLOR_REG 0x52     
/* Screen Display Control - Write only
Bit	7	6	5	4	3	    2	    1	    0
                    G2DS	G1DS	G0DS	TEXTDS

G2DS GVRAM2 display: 0-ON 1-OFF
G1DS GVRAM1 display: 0-ON 1-OFF
G0DS GVRAM0 display: 0-ON 1-OFF
TEXTDS Text screen display: 0-ON 1-OFF
* Set bit 3 to 1 when using 640x400 mode.
In color graphics mode, G*DS bits are ignored.
*/
#define SCREEN_DISP_CTL 0x53
#define PAL_REG0 0x54  // bits 0-2 contain palette value 0-7.
#define PAL_REG1 0x55  // bits 0-2 contain palette value 0-7.
#define PAL_REG2 0x56  // bits 0-2 contain palette value 0-7.
#define PAL_REG3 0x57  // bits 0-2 contain palette value 0-7.
#define PAL_REG4 0x58  // bits 0-2 contain palette value 0-7.
#define PAL_REG5 0x59  // bits 0-2 contain palette value 0-7.
#define PAL_REG6 0x5a  // bits 0-2 contain palette value 0-7.
#define PAL_REG7 0x5b  // bits 0-2 contain palette value 0-7.
/* Read: GVRAM bank display status
   bit 0 - GVRAM0 
   bit 1 - GVRAM1
   bit 2 - GVRAM3
1 = Selected
0 = Not selected */ 
#define GVRAM_BANK_REG 0x5c

#define GVRAM_SEL_0 0x5c  // Write any value to select vram0 (Blue plane)
#define GVRAM_SEL_1 0x5d  // Write any value to select vram1 (Red plane)
#define GVRAM_SEL_2 0x5e  // Write any value to select vram2 (Green plane)
#define MAINRAM_SEL 0x5f  // Write any value to select Main RAM

#define DMAC_REG0 0x60 // PD8257C DMA controller register (R/W)
#define DMAC_REG1 0x61 // PD8257C DMA controller register (R/W)
#define DMAC_REG2 0x62 // PD8257C DMA controller register (R/W)
#define DMAC_REG3 0x63 // PD8257C DMA controller register (R/W)
#define DMAC_REG4 0x64 // PD8257C DMA controller register (R/W)
#define DMAC_REG5 0x65 // PD8257C DMA controller register (R/W)
#define DMAC_REG6 0x66 // PD8257C DMA controller register (R/W)
#define DMAC_REG7 0x67 // PD8257C DMA controller register (R/W)
#define DMAC_REG8 0x68 // PD8257C DMA controller register  (R/W)

#define CPU_CLOCK_CTL 0x6e // Bit 7 only. 1=4MHz, 0=8MHz
/* (R/W) Baud rate set register, FH+ only
Bits 0-3 only:
0000	75
0001	150
0010	300
0011	600
0100	1200
0101	2400
0110	4800
0111	9600
1000	19200
1001～1111	OFF */
#define BAUDRATE_REG 0x6f
#define TEXTWIN_OFS_REG 0x70 // R/W: Upper 8 bits of text window address
/* Expansion ROM select
Only one bit can be set at once:
Bit7 Expansion ROM 7 select
Bit6 Expansion ROM 6 select
Bit5 Expansion ROM 5 select
Bit4 Expansion ROM 4 select
Bit3 Expansion ROM 3 select
Bit2 Expansion ROM 2 select
Bit1 Expansion ROM 1 select
Bit0 Expansion ROM 0/4th rom select
Writing 0xff selects Main ROM.
*/
#define ROM_EXPANSION_SEL 0x71
#define TEXTWIN_INC_REG 0x78 // W: Increment TEXTWIN_OFS once

// To do:
// Registers Ax - Sound board II registers
// Registers Ex - Expanded RAM read/write control registers, including Kanji ROMs
// Registers Fx - Disc subsystem 8255 control regs

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


// Function headers
void main();
static inline void putchr(u8 c);
/**/static inline void putchr40(u8 c);
void print(String str);
/**/void print40(String str);
u8 ReadIOReg(u8 r);
void SetIOReg(u8 r, u8 v);
/* Attributes must be set in ascending X order on each row. 
    Attribute #0 on each row is ALWAYS read as X=0, regardless of its actual value. */
void SetTextAttribute(u8 x, u8 y, u8 attr);
void ClearAttributeRam();       // Resets to the bios defaults.
void SetCursorPos(u8 x, u8 y);
/**/void SetCursorPos40(u8 x, u8 y);
void Wait_VBLANK();
void DrawPlaneBMP(const u8* img, u8 plane, u16 x, u16 y, u8 w, u8 h);
void SetPixel(u16 x, u8 y, u8 c);
bool GetKeyDown(u8 SCANCODE);
static inline void EnableALU();
static inline void DisableALU();
static inline void ExpandedGVRAM_On();
static inline void ExpandedGVRAM_Off();

#define SetBGColor(c) SetIOReg(0x52, c << 4);
#define SetBorderColor(c) SetIOReg(0x52, c); // PC8001 only?
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
    

// globals 
vu8* SCREEN_POINTER;


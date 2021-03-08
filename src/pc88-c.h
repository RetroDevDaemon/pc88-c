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
/* Expanded Access Mode GVRAM Control   W only
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
ld ($c0002),a   ; operation commands a vram 'copy'.*/
#define EXPANDED_GVRAM_CTRL 0x35
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
/* Interrupt Level Set Register
 bits 0 - 2: Interrupt level 
 0 (high) - RS232 / Serial
 1 - CRTC / Vertical blank
 2 - 1/600s timer
 3 - INT4
 4 - Sound IRQ
 5 - INT2
 6 - FDINT1
 7 - FDINT2
Set to 1 higher than the IRQ you wish to enable.
e.g. writing 0x03 will enable 0, 1, and 2. */
#define IRQ_LEVEL_SET 0xe4
/* Interrupt Mask Register
Bit 2 - Serial / RS232 interrupt
Bit 1 - Vertical blank from CRTC
Bit 0 - 1/600s timer interrupt
 Setting a bit ALLOWS the interrupt. */
#define IRQ_MASK 0xe6
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
void __init();
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
void DiskLoad(u8* dst, u8 track, u8 sector, u8 numSecs, u8 drive); 

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
u8 LINE_POINTER;

void __init()
{
    SCREEN_POINTER = (vu8*)SCREEN_TXT_BASE;
    LINE_POINTER = (u8)0;
    main();
}

bool GetKeyDown(u8 SCANCODE)
{
    u8 r = (SCANCODE & 0xf0) >> 4;
    u8 b = SCANCODE & 0x0f;
    u8 i = ReadIOReg(r);
    if(! (i & bit(b)) ) return true;
    return false;
}

void Wait_VBLANK() __naked 
{
    /* Taken from PC-88 info page:
     http://mydocuments.g2.xrea.com/html/p8/vraminfo.html */

    __asm
    WaitVBlank:
    in	    a,(0x40)        ; SYS_MODE_SENSE
    and     #0x20           ; VBLANK_SIGNAL
    jr      nz,WaitVBlank   ; bit 5 set is vblank interval 
    .wait:
    in      a,(0x40)
    and     #0x20
    jr      z,.wait         ; wait until the signal is off again to be safe!
    ret
    __endasm;
}

void SetCursorPos(u8 x, u8 y)
{
    SCREEN_POINTER = (vu8*)(SCREEN_TXT_BASE + (120 * y) + x);
}

void SetCursorPos40(u8 x, u8 y)
{
    SCREEN_POINTER = (vu8*)(SCREEN_TXT_BASE + (120 * y) + (x * 2));
}

void ClearAttributeRam()
{
    vu8* addr = (vu8*)(SCREEN_ATTR_BASE);
    u16 p = 0xe880;
    for(u8 y = 0; y < 24; y++){
        for(u8 x = 0; x < 20; x++){
            *addr = p;
            addr += 2;   
        }
        addr += 80;
    }
}

void SetTextAttribute(u8 x, u8 y, u8 attr)
{
    vu8* addr = (vu8*)(SCREEN_ATTR_BASE + (y * 120));
    while(*addr != 0x80)
        addr += 2;

    *addr = x; 
    addr++;
    *addr = attr;
}

static inline void putchr(u8 c)
{
    *SCREEN_POINTER = c;
    SCREEN_POINTER++;
    LINE_POINTER++;
    if(LINE_POINTER == 80) { 
        LINE_POINTER = 0;
        SCREEN_POINTER = SCREEN_POINTER + 40;
    }
}

static inline void putchr40(u8 c)
{
    *SCREEN_POINTER = c;
    SCREEN_POINTER +=2 ;
    LINE_POINTER++;
    if(LINE_POINTER == 80) { 
        LINE_POINTER = 0;
        SCREEN_POINTER = SCREEN_POINTER + 40;
    }
}

void print(String str)
{
    u8 i = 0;
    while(str[i] != '\x00')
        putchr(str[i++]);
}

void print40(String str)
{
    u8 i = 0;
    while(str[i] != '\x00')
        putchr40(str[i++]);
}


u8 ReadIOReg(u8 r) __naked
{   // Template copied from generated SDCC code and "optimized"
    r;
    __asm 
        ld	iy, #2
        add	iy, sp
        ld a, 0 (iy)
        ld c, a 
        in a, (c)
        ld l, a
        ret 
    __endasm;
}

void SetIOReg(u8 r, u8 v) __naked 
{   
    r; v; 
    __asm
        push ix 
        ld ix,#0
        add	ix,sp
    //; 4 (ix) and 5 (ix) contain r and v 
        ld c, 4 (ix)
        ld a, 5 (ix) 
        out (c), a 
        pop	ix
        ret
    __endasm;
}


void DrawPlaneBMP(const u8* img, u8 plane, u16 x, u16 y, u8 w, u8 h)
{
    if(plane == PLANE_RED)
        SETBANK_RED()
    else if(plane == PLANE_BLUE)
        SETBANK_BLUE()
    else 
        SETBANK_GREEN()
    vu8* p = (vu8*)(0xc000) + (y * 80) + (u8)(x/8);
    const u8* v = img;
    for(u8 yy = 0; yy < h; yy++){
        for(u8 xx = 0; xx < w; xx++){
            *p = *v;
            p++;
            v++;
        }
        p += (80 - w);
    } 
}


void SetPixel(u16 x, u8 y, u8 c)
{
    u8 po = 7 - ((x & 0xff) % 8);
    vu8* p = (vu8*)(0xc000) + (y * 80) + (u8)(x / 8);
        
    if(c & 0b001){
        SETBANK_BLUE()
        *p |= bit(po);
    }
    if(c & 0b010){
        SETBANK_RED()
        *p |= bit(po);
    }
    if(c & 0b100){
        SETBANK_GREEN()
        *p |= bit(po);
    }

}

static inline void EnableALU(){ SetIOReg(ALU_MODE_CTRL, 0xC9); }
static inline void DisableALU(){ SetIOReg(ALU_MODE_CTRL, 0x89); }
static inline void ExpandedGVRAM_On() { SetIOReg(EXPANDED_GVRAM_CTRL, 0x80); }
static inline void ExpandedGVRAM_Off() { SetIOReg(EXPANDED_GVRAM_CTRL, 0); } 

/* dest - Destination to load data in RAM
srcTrack - Source track on disk 
srcSector - Source sector on disk 
numSecs - Number of sectors to copy (bytes*256)
drive - Drive number (default 0) */
void DiskLoad(u8* dest, u8 srcTrack, u8 srcSector, u8 numSecs, u8 drive) __naked 
{
    /* As-is from IPL.BIN, taken from Maroon's page.
       This is copied in during the auto-load sequence to C0xx. */
    dest; srcTrack; srcSector; numSecs; drive;
    __asm 
        ld iy, #0
        add iy, sp 
        ld l, 2 (iy)     
        ld h, 3 (iy)            ; dest
        ld d, 4 (iy)            ; track 
        ld e, 5 (iy)            ; sector 
        ld b, 6 (iy)            ; nm secs
        ld c, 7 (iy)            ; drive no. 
    DISK_Load:
        ld			a,#0x02					; cmd 2, read data
        call		DISK_SdCmd
        ld			a,b						; sector num
        cp			#17
        jr			c,.load02
        ld			a,#17					; if 17, go to next track 
        sub			e
    .load02:
        call		DISK_SdData1
        ld			a,c						; drive #
        call		DISK_SdData1
        ld			a,d						; trk
        call		DISK_SdData1
        ld			a,e						; sectr
        call		DISK_SdData1

        ld			a,#0x12					; cmd 18 fast send data - required to continue load loop from disk 
        call		DISK_SdCmd

    .loop01:
        push		bc
        ld			b,#128					; 128 * 2bytes
    .loop02:
        call		DISK_RdData2
        djnz		.loop02					; loop in sectr

        inc			e
        ld			a,e
        cp			#17						; next track if 17..
        jr			z,.next
        pop			bc
        djnz		.loop01					; sector loop
        ret                                 ; all done!

    ; Quick sub to increase the load counter... 
    .next:
        inc			d						; from next track
        ld			e,#1					; sector from 1 
        pop			bc
        dec			b
        jr			nz,DISK_Load
        ret
    ; send cmd to subsys 
    ; a is command symbol
    ;  7   6   5   4   3   2   1   0
    ;  W   W   W   W   R   R   R   R
    ; ATN DAC RFD DAV  -  DAC RFD DAV
    DISK_SdCmd:
        push		af                          ; Push the queued disk command to the stack
        ld			a,#0b00001111				; Attention=1 
        out			(0xFF),a                    ; out to control port 
    .wait1:
        in			a,(0xFE)                    ; Read in from port C ...
        bit			1,a						    ; wait until Ready for Data bit is set 
        jr			z,.wait1
        ld			a,#0b00001110				; Attention=0
        out			(0xFF),a                    ;  (to ctl port)
        jr			DISK_SdData1sub             ; continue to send data routine 

    ;get 1 byte 
    DISK_SdData1:
        push		af
    .wait01:
        in			a,(0xFE)                    ; port C... 
        bit			#1,a						; Ready for Data ?
        jr			z,.wait01
    DISK_SdData1sub:
        pop			af                          
        out			(0xFD),a					; output the pushed cmd to port B
        ld			a,#0b00001001				; Data Validate 
        out			(0xFF),a                    ; (to ctl port)
    .wait2:
        in			a,(0xFE)                    ; port C 
        bit			2,a						    ; Data Accepted?
        jr			z,.wait2
        ld			a,#0b00001000				; Data Validate off  
        out			(0xFF),a                    ; 
    .wait3:
        in			a,(0xfe)                    ; in from C 
        bit			2,a						    ; Data Accepted? (bit 2)
        jr			nz,.wait3
        ret                                     ; OK!


    ;get 2 bytes from 0xfc, stores them in (hl) 
    DISK_RdData2:
        ld			a,#0b00001011				; Ready for Data
        out			(0xFF),a
    .wait10:
        in			a,(0xFE)
        rrca								    ; Data Valid?
        jr			nc,.wait10

        ld			a,#0b00001010				; Ready for Data=0
        out			(0xFF),a

        in			a,(0xFC)                    ; get data 
        ld			(hl),a
        inc			hl
        ld			a,#0b00001101				; Data Valid=1
        out			(0xFF),a
    .wait20:
        in			a,(0xFE)
        rrca
        jr			c,.wait20

        in			a,(0xFC)
        ld			(hl),a
        inc			hl
        ld			a,#0b00001100				; Data Accepted
        out			(0xFF),a
        ret

    __endasm;
}
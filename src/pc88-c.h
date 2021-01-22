// pc88-c.h 

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

struct planarBitmap {
    const u8* r;
    const u8* g;
    const u8* b;
    u8 w;
    u8 h;
};
typedef const struct planarBitmap PlanarBitmap;

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

#define COLORMODE_SET(color, semigraphic) \
    (u8)((color<<5)|(semigraphic<<4)|0b1000)
#define COLORMODE_ATTR(underline, upperline, reverse, blink, hidden) \
    (u8)((underline<<5)|(upperline<<4)|(reverse<<2)|(blink<<1)|(hidden))
#define BWMODE_ATTR(underline, upperline, reverse, blink, hidden) \
    (u8)((underline<<5)|(upperline<<4)|(reverse<<2)|(blink<<1)|(hidden))
#define ATTR_BW_SEMIGRAPHIC 0b10011000


// Function headers
void __init();
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
void ClearAttributeRam();
void SetCursorPos(u8 x, u8 y);
/**/void SetCursorPos40(u8 x, u8 y);
void Wait_VBLANK();
void DrawPlaneBMP(const u8* img, u8 plane, u16 x, u16 y, u8 w, u8 h);

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
    
#define IRQ_OFF() __asm di __endasm;
#define IRQ_ON() __asm ei __endasm;
    
// globals 
vu8* SCREEN_POINTER;

// KICKOFF
// Must be the first function def in this file.
void __init()
{
    SCREEN_POINTER = (vu8*)SCREEN_TXT_BASE;
    main();
    return;
}
// DON'T MOVE __init() !!

void Wait_VBLANK() __naked 
{
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

void putchr(u8 c)
{
    *SCREEN_POINTER = c;
    SCREEN_POINTER++;
}

void putchr40(u8 c)
{
    *SCREEN_POINTER = c;
    SCREEN_POINTER +=2 ;
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
{   // Template copied from generated SDCC code
    r;
    __asm 
        ld	iy, #2
        add	iy, sp
        ld a, 0 (iy)
        ld c, a 
        in a, (c)
        ld 0 (iy), a 
        ld	l, 0 (iy)
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
        ld l, a
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
    for(u8 y = 0; y < h; y++){
        for(u8 x = 0; x < w; x++){
            *p = *v;
            p++;
            v++;
        }
        p += (80 - w);
    } 
}

//////////////////
/// Pc-88 HELLO WORLD
/////////////////

typedef unsigned char u8;
typedef unsigned int u16;
typedef volatile unsigned char vu8;
typedef const u8 String[];

#define SCREEN_TXT_BASE 0xf3c8
#define MODE_SET_REGISTER (u8)0x32
#define TMODE_BIT 0b10000

vu8* screen_ptr;

// Function headers
void putchr(u8 c);
void print(String str);
void setcursorpos(u16 x, u16 y);
u8 ReadIOReg(u8 r);
void SetIOReg(u8 r, u8 v);

// Consts
String strdat = "Hello World\x00";

void main()
{
    screen_ptr = (u8*)SCREEN_TXT_BASE;
    
    print(strdat);
    
    u8 t = ReadIOReg(MODE_SET_REGISTER);
    SetIOReg(MODE_SET_REGISTER, t^TMODE_BIT);
    t = ReadIOReg(MODE_SET_REGISTER);
    
    print("Heya\x00");
    
    while(1)
    {
        // Main loop!
        // Do nothing...
    }
}

void putchr(u8 c)
{
    *screen_ptr = c;
    screen_ptr++;
}

void print(String str)
{
    u8 i = 0;
    while(str[i] != '\x00')
        putchr(str[i++]);
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
    ; 4 (ix) and 5 (ix) contain r and v 
        ld c, 4 (ix)
        ld a, 5 (ix) 
        out (c), a 
        ld l, a
        pop	ix
        ret
    __endasm;
}
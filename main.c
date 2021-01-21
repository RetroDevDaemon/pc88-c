//////////////////
/// Pc-88 HELLO WORLD
/// (c)2021 Ben Ferguson
/////////////////

#include "pc88-c.h"


// Consts
String strdat = "H e l l o   W o r l d \x00";

void main()
{
    SCREEN_POINTER = (vu8*)SCREEN_TXT_BASE;
    
    // turn on 80 col + color mode 
    SetIOReg(SYS_CTL_REGISTER, (u8)TEXT_80COL_BIT); 
    
    // Print tests
    ClearAttributeRam();
    SetTextAttribute(0, 0, COLORMODE_SET(CLR_FUSCHIA, true));
    SetTextAttribute(20, 0, COLORMODE_ATTR(false, false, false, true, false));
    SetTextAttribute(5, 1, COLORMODE_SET(CLR_WHITE, false));
    SetTextAttribute(6, 1, COLORMODE_ATTR(false, false, false, false, false));
    print(strdat);
    SetCursorPos(5, 1);
    print("Hello World Too\x00");
    
    // Set ALU mode on!
    u8 t = ReadIOReg(MODE_SET_REGISTER);
    SetIOReg(MODE_SET_REGISTER, t ^ bit(6));
    // reset alu ctl reg 
    SetIOReg(EXPANDED_ALU_CTRL, 0xff);
    SetIOReg(EXPANDED_GVRAM_CTRL, 0x80);
    
    while(1)
    {
        Wait_VBLANK();
    }
}

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
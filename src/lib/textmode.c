/*! \addtogroup Text 
* @{ 
*/

#include <pc88-c.h>

u8 TABULATE;
vu8* SCREEN_POINTER;
u8 LINE_POINTER;

inline void putchr(u8 c)
{
    *SCREEN_POINTER = c;
    SCREEN_POINTER++;
    LINE_POINTER++;
    if(LINE_POINTER == 80) { 
        LINE_POINTER = 0;
        SCREEN_POINTER = SCREEN_POINTER + 40;
    }
}

inline void putchr40(u8 c)
{
    *SCREEN_POINTER = c;
    SCREEN_POINTER +=2 ;
    LINE_POINTER++;
    if(LINE_POINTER == 80) { 
        LINE_POINTER = 0;
        SCREEN_POINTER = SCREEN_POINTER + 40;
    }
}

void print(const u8* str)
{
    u8 i = 0;
    TABULATE = LINE_POINTER;
    while(*(str+i) != '\x00') {
        if(*(str+i) == '\n') { 
            i++; 
            SCREEN_POINTER += (120 - (((u16)SCREEN_POINTER - SCREEN_TXT_BASE) % 120)) + TABULATE;
            LINE_POINTER = TABULATE;
        }
        else { 
            putchr(*(str+i));
            i++;
        }
    }
}

void print40(String str)
{
    u8 i = 0;
    TABULATE = LINE_POINTER;
    while(str[i] != '\x00') {
        if(str[i] == '\n') { 
            i++; 
            SCREEN_POINTER += (120 - (((u16)SCREEN_POINTER - SCREEN_TXT_BASE) % 120)) + TABULATE;
            LINE_POINTER = TABULATE;
        }
        else putchr(str[i++]);
    }
}

void SetCursorPos(u8 x, u8 y)
{
    SCREEN_POINTER = (vu8*)(SCREEN_TXT_BASE + (u16)(120 * y) + x);
    LINE_POINTER = x;
}

void SetCursorPos40(u8 x, u8 y)
{
    SCREEN_POINTER = (vu8*)(SCREEN_TXT_BASE + (u16)(120 * y) + (x * 2));
    LINE_POINTER = x * 2;
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

static u8 BYTOHEXWORK[3] = "  ";
u8* byToHex(u8 by)
{
    u8 a = by & 0xf;
    u8 b = (by & 0xf0) >> 4;
    if (a > 9) a += 7;
    if (b > 9) b += 7;
    a += 0x30;
    b += 0x30;
    BYTOHEXWORK[0] = b; BYTOHEXWORK[1] = a; BYTOHEXWORK[2] = 0;
    return &BYTOHEXWORK[0];
}

// 3127 bytes, 0x78 per
// 0xf3c8 + (120*6)
extern u16 local_a;
extern u16 local_b;
inline void TextRowCopy(u8 src, u8 dst)
{ // 0 to 20
    local_a = 0xf3c8 + (0x78 * src);
    local_b = 0xf3c8 + (0x78 * dst);
    __asm 
    
    ; wipe the dst row, kinda slow
        ld de,(_local_b)
        ld bc,#80
    $00177:
        ld a,#0x20
        ld (de),a 
        inc de 
        dec bc 
        ld a,b 
        or c 
        jr nz,$00177
    
    ; fast copy 
        ld hl,(_local_a)
        ld de,(_local_b)
        ld bc,#80
        ldir 
    
    ; wipe the src row 
        ld de,(_local_a)
        ld bc,#80
    $00178:
        ld a,#0x20
        ld (de),a 
        inc de 
        dec bc 
        ld a,b 
        or c 
        jr nz,$00178
    
    __endasm;

}
/*! @} */
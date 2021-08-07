#define SCREEN_TXT_BASE 0xf3c8

typedef volatile unsigned char vu8;
typedef unsigned char u8;
typedef unsigned int u16;

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

void SetCursorPos(u8 x, u8 y)
{
    SCREEN_POINTER = (vu8*)(SCREEN_TXT_BASE + (u16)(120 * y) + x);
    LINE_POINTER = x;
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

int main()
{
    SCREEN_POINTER = (vu8*)SCREEN_TXT_BASE;
    LINE_POINTER = (u8)0;
    SetCursorPos(20, 10);
    print("Hello World!");

	// Taken from Bookworm's Library:
	/*
	__asm 
		ld a,#0b00111001
		out (0x31),a 
	__endasm;
	status = *(u8*)(0x79d7);
	
	SetIOReg(0x31, 0b00011011); // gimme back the RAM
	*/
    return 0;
}
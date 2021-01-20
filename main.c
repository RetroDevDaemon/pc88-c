//////////////////
/// Pc-88 HELLO WORLD
/////////////////

typedef unsigned char u8;
typedef volatile unsigned char vu8;

#define SCREEN_TXT_BASE 0xf3c8

vu8* screen_ptr;

void putchr(u8 c);
void print(u8* str);

const u8 strdat[] = "Hello World\x00";

void main()
{
    screen_ptr = (u8*)SCREEN_TXT_BASE;
    print(strdat);
    
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

void print(const u8* str)
{
    u8 i = 0;
    while(str[i] != '\x00')
        putchr(str[i++]);
}

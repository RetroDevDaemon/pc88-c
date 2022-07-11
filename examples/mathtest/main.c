#include <pc88-c.h>

u8 stringbuf[32];

static u16 y; 
static u16 y2;
static u64 r;
static u32 ra;
static u32 rb;

// Use this function to convert a 4-bit value to a base 10 decimal equivalent,
// based on the rounding listed below.

// Todo: make this a proper assembly function
u8 nibbleToDec(u8 nib)
{
    /*
0 0.0
1 0.0625
2 0.125
3 0.1875
4 0.25
5 0.3125
6 0.375
7 0.4375
8 0.5
9 0.5625
10 0.625
11 0.6875
12 0.75
13 0.8125
14 0.875
15 0.9375 */
    switch(nib)
    {
        //case(0): rounded:
        case(1):
        case(2):
            return 1;
        case(3):
        case(4):
            return 2;
        case(5):
            return 3;
        case(6):
        case(7):
            return 4;
        case(8):
            return 5;
        case(9):
        case(10):
            return 6;
        case(11):
        case(12):
            return 7;
        case(13):
            return 8;
        case(14):
        case(15):
            return 9;
        default:
            return 0;
    }
}

void main()
{
    u8 a; u8 b; u8 c;
    
    a = 10 / 4;
    b = 10 / 5;
    c = 10 / 6;
    
    u8* z = byToHex(a);
    print("10 / 4: ");
    print(z);
    
    SetCursorPos(0,1);
    z = byToHex(b);
    print("10 / 5: ");
    print(z);

    SetCursorPos(0,2);
    z = byToHex(c);
    print("10 / 6: ");
    print(z);

    SetCursorPos(0,3);
    print("sizeof int:");
    print(byToDec(sizeof(int)));
    
    SetCursorPos(0,4);
    print("sizeof long:");
    print(byToDec(sizeof(long)));

    SetCursorPos(0,5);
    print("sizeof long long:");
    print(byToDec(sizeof(long long)));

    y = (u16)FIXED16(16.4); //1638
    y2 = (u16)FIXED16(42.8); //716
    u32 rr = y + y2;

    //u16* r2 = &r - 2;

    SetCursorPos(0, 7);
    
    print("16.4 + 42.8 = ");
    print(byToDec(rr >> 8));
    print(".");
    print(byToDec(nibbleToDec((rr>>4)&0xf)));
    //print(byToHex(rr));
    //print(byToHex(y2 >> 8));
    ///print(byToHex(y2));
    
    //print(byToHex(r >> 8));
    //print(byToHex(r));      //58696
    
    
    
    while(1)
    { }
}
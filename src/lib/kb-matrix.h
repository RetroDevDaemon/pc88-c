/*! \addtogroup keyboard
* @{
*/
//kb-matrix.h
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

/*! @} */
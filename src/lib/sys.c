/*! \addtogroup System
* @{
*/

// sys.c
#include <pc88-c.h>

u8 GetN88Rom() // UNSAFE!!
{
    u8 romtype = 0;
    __asm 
    Check_N88ROM:
        ld          a,#0b00111001                 ;N88-BASIC, ROM&RAM
        out         (0x31),a

        ld          a,(0x79D7)
        cp          #0x32
        jr          c,.PC8801                   ;1.0-1.1 初代
        jr          z,.PC8801A                  ;1.2 A
        cp          #0x33
        jr          z,.PC8801mk2                ;1.3 mk2
        cp          #0x34
        jr          z,.PC8801mk2SRTR            ;1.4 SR/TR
        cp          #0x38
        jr          c,.PC8801FRMR               ;1.5-1.7 FR/MR
        jr          z,.PC8801FHMH               ;1.8 FH/MH

        ld          a,(0x79D8)
        cp          #0x0D
        jr          z,.PC8801FAMAVAx            ;1.9 VA/FA/MA/VA2/VA3
        cp          #0x31
        jr          z,.PC8801FEMA2              ;1.91 FE/MA2
        cp          #0x32
        jr          z,.PC98DO                   ;1.92 DO
        cp          #0x33
        jr          z,.PC8801FE2MC              ;1.93 FE2/MC
        cp          #0x34
        jr          z,.PC98DOP                  ;1.94 DO+

        jr          .PC8801unknown              ;不明
    .PC8801:        ld a,#0
                    jr _endit
    .PC8801A:       ld a,#1
                    jr _endit
    .PC8801mk2:     ld a,#2
                    jr _endit
    .PC8801mk2SRTR: ld a,#3
                    jr _endit
    .PC8801FRMR:    ld a,#4
                    jr _endit
    .PC8801FHMH:    ld a,#5
                    jr _endit
    .PC8801FAMAVAx: ld a,#6
                    jr _endit
    .PC8801FEMA2:   ld a,#7
                    jr _endit
    .PC98DO:        ld a,#8
                    jr _endit
    .PC8801FE2MC:   ld a,#9
                    jr _endit
    .PC98DOP:       ld a,#10
                    jr _endit
    .PC8801unknown: ld a,#11
                    ;jr _endit
    _endit: 
        ;ld      (_romtype), a
        ld      a, #0b00111011 ; return to ipls default
        out     (0x31), a
    __endasm;
    return romtype;
}


u16 GetSysMode() 
{
    //u32 modestr = 0x12345678;
              //     D E H L
    return (ReadIOReg(0x31) << 8) | ReadIOReg(0x6e);
    
}

/*! @} */
;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 4.0.0 #11528 (MINGW64)
;--------------------------------------------------------
	.module main
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _SCREEN_POINTER
	.globl _strdat
	.globl _Wait_VBLANK
	.globl _SetCursorPos
	.globl _SetCursorPos40
	.globl _ClearAttributeRam
	.globl _SetTextAttribute
	.globl _putchr
	.globl _putchr40
	.globl _print
	.globl _print40
	.globl _ReadIOReg
	.globl _SetIOReg
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
_SCREEN_POINTER::
	.ds 2
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _INITIALIZED
;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area _DABS (ABS)
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area _HOME
	.area _GSINIT
	.area _GSFINAL
	.area _GSINIT
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area _HOME
	.area _HOME
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE
;main.c:12: void main()
;	---------------------------------
; Function main
; ---------------------------------
_main::
;main.c:14: SCREEN_POINTER = (vu8*)SCREEN_TXT_BASE;
	ld	hl, #0xf3c8
	ld	(_SCREEN_POINTER), hl
;main.c:17: SetIOReg(SYS_CTL_REGISTER, (u8)TEXT_80COL_BIT); 
	ld	de, #0x0130
	push	de
	call	_SetIOReg
	pop	af
;main.c:20: ClearAttributeRam();
	call	_ClearAttributeRam
;main.c:21: SetTextAttribute(0, 0, COLORMODE_SET(CLR_FUSCHIA, true));
	ld	a, #0x78
	push	af
	inc	sp
	xor	a, a
	push	af
	inc	sp
	xor	a, a
	push	af
	inc	sp
	call	_SetTextAttribute
	pop	af
	inc	sp
;main.c:22: SetTextAttribute(20, 0, COLORMODE_ATTR(false, false, false, true, false));
	ld	a, #0x02
	push	af
	inc	sp
	xor	a, a
	ld	d,a
	ld	e,#0x14
	push	de
	call	_SetTextAttribute
	pop	af
	inc	sp
;main.c:23: SetTextAttribute(5, 1, COLORMODE_SET(CLR_WHITE, false));
	ld	de, #0xe801
	push	de
	ld	a, #0x05
	push	af
	inc	sp
	call	_SetTextAttribute
	pop	af
	inc	sp
;main.c:24: SetTextAttribute(6, 1, COLORMODE_ATTR(false, false, false, false, false));
	xor	a, a
	ld	d,a
	ld	e,#0x01
	push	de
	ld	a, #0x06
	push	af
	inc	sp
	call	_SetTextAttribute
;main.c:25: print(strdat);
	inc	sp
	ld	hl,#_strdat
	ex	(sp),hl
	call	_print
	pop	af
;main.c:26: SetCursorPos(5, 1);
	ld	de, #0x0105
	push	de
	call	_SetCursorPos
;main.c:27: print("Hello World Too\x00");
	ld	hl, #___str_0
	ex	(sp),hl
	call	_print
	pop	af
;main.c:37: while(1)
00102$:
;main.c:40: print("0\x00");
	ld	hl, #___str_1
	push	hl
	call	_print
	pop	af
;main.c:42: }
	jr	00102$
_strdat:
	.ascii "H e l l o   W o r l d "
	.db 0x00
	.db 0x00
___str_0:
	.ascii "Hello World Too"
	.db 0x00
	.db 0x00
___str_1:
	.ascii "0"
	.db 0x00
	.db 0x00
;main.c:44: void Wait_VBLANK() __naked 
;	---------------------------------
; Function Wait_VBLANK
; ---------------------------------
_Wait_VBLANK::
;main.c:56: __endasm;
	    WaitVBlank:
	in	a,(0x40) ; (u8)0x40
	and	#0x20 ; 0b100000
	jr	nz,WaitVBlank ; bit 5 set is vblank interval
	    .wait:
	in	a,(0x40)
	and	#0x20
	jr	z,.wait ; wait until the signal is off again to be safe!
	ret
;main.c:57: }
;main.c:59: void SetCursorPos(u8 x, u8 y)
;	---------------------------------
; Function SetCursorPos
; ---------------------------------
_SetCursorPos::
	push	ix
	ld	ix,#0
	add	ix,sp
;main.c:61: SCREEN_POINTER = (vu8*)(SCREEN_TXT_BASE + (120 * y) + x);
	ld	c, 5 (ix)
	ld	b, #0x00
	ld	l, c
	ld	h, b
	add	hl, hl
	add	hl, bc
	add	hl, hl
	add	hl, bc
	add	hl, hl
	add	hl, bc
	add	hl, hl
	add	hl, hl
	add	hl, hl
	ld	bc, #0xf3c8
	add	hl, bc
	ld	c, 4 (ix)
	ld	b, #0x00
	add	hl, bc
	ld	(_SCREEN_POINTER), hl
;main.c:62: }
	pop	ix
	ret
;main.c:64: void SetCursorPos40(u8 x, u8 y)
;	---------------------------------
; Function SetCursorPos40
; ---------------------------------
_SetCursorPos40::
	push	ix
	ld	ix,#0
	add	ix,sp
;main.c:66: SCREEN_POINTER = (vu8*)(SCREEN_TXT_BASE + (120 * y) + (x * 2));
	ld	c, 5 (ix)
	ld	b, #0x00
	ld	l, c
	ld	h, b
	add	hl, hl
	add	hl, bc
	add	hl, hl
	add	hl, bc
	add	hl, hl
	add	hl, bc
	add	hl, hl
	add	hl, hl
	add	hl, hl
	ex	de, hl
	ld	hl, #0xf3c8
	add	hl, de
	ex	de, hl
	ld	l, 4 (ix)
	ld	h, #0x00
	add	hl, hl
	add	hl, de
	ld	(_SCREEN_POINTER), hl
;main.c:67: }
	pop	ix
	ret
;main.c:69: void ClearAttributeRam()
;	---------------------------------
; Function ClearAttributeRam
; ---------------------------------
_ClearAttributeRam::
;main.c:71: vu8* addr = (vu8*)(SCREEN_ATTR_BASE);
	ld	hl, #-2
	add	hl, sp
	ld	(hl), #0x18
	inc	hl
	ld	(hl), #0xf4
;main.c:73: for(u8 y = 0; y < 24; y++){
	ld	c, #0x00
00107$:
	ld	a, c
	sub	a, #0x18
	ret	NC
;main.c:74: for(u8 x = 0; x < 20; x++){
	ld	hl, #-2
	add	hl, sp
	ld	b, (hl)
	inc	hl
	ld	e, (hl)
	ld	d, #0x00
00104$:
	ld	a, d
	sub	a, #0x14
	jr	NC,00101$
;main.c:75: *addr = p;
	ld	a, #0x80
	ld	l, b
	ld	h, e
	ld	(hl), a
;main.c:76: addr += 2;   
	ld	a, b
	add	a, #0x02
	ld	b, a
	jr	NC,00133$
	inc	e
00133$:
;main.c:74: for(u8 x = 0; x < 20; x++){
	inc	d
	jr	00104$
00101$:
;main.c:78: addr += 80;
	ld	a, b
	ld	hl, #-2
	add	hl, sp
	add	a, #0x50
	ld	(hl), a
	ld	a, e
	adc	a, #0x00
	inc	hl
	ld	(hl), a
;main.c:73: for(u8 y = 0; y < 24; y++){
	inc	c
;main.c:80: }
	jr	00107$
;main.c:82: void SetTextAttribute(u8 x, u8 y, u8 attr)
;	---------------------------------
; Function SetTextAttribute
; ---------------------------------
_SetTextAttribute::
;main.c:84: vu8* addr = (vu8*)(SCREEN_ATTR_BASE + (y * 120));
	ld	hl, #3+0
	add	hl, sp
	ld	c, (hl)
	ld	b, #0x00
	ld	l, c
	ld	h, b
	add	hl, hl
	add	hl, bc
	add	hl, hl
	add	hl, bc
	add	hl, hl
	add	hl, bc
	add	hl, hl
	add	hl, hl
	add	hl, hl
	ld	bc, #0xf418
	add	hl, bc
;main.c:85: while(*addr != 0x80)
00101$:
	ld	a, (hl)
	sub	a, #0x80
	jr	Z,00103$
;main.c:86: addr += 2;
	inc	hl
	inc	hl
	jr	00101$
00103$:
;main.c:88: *addr = x; 
	ld	iy, #2
	add	iy, sp
	ld	a, 0 (iy)
	ld	(hl), a
;main.c:89: addr++;
	inc	hl
;main.c:90: *addr = attr;
	inc	iy
	inc	iy
	ld	a, 0 (iy)
	ld	(hl), a
;main.c:91: }
	ret
;main.c:93: void putchr(u8 c)
;	---------------------------------
; Function putchr
; ---------------------------------
_putchr::
;main.c:95: *SCREEN_POINTER = c;
	ld	hl, (_SCREEN_POINTER)
	ld	iy, #2
	add	iy, sp
	ld	a, 0 (iy)
	ld	(hl), a
;main.c:96: SCREEN_POINTER++;
	ld	hl, (_SCREEN_POINTER)
	inc	hl
	ld	(_SCREEN_POINTER), hl
;main.c:97: }
	ret
;main.c:99: void putchr40(u8 c)
;	---------------------------------
; Function putchr40
; ---------------------------------
_putchr40::
;main.c:101: *SCREEN_POINTER = c;
	ld	hl, (_SCREEN_POINTER)
	ld	iy, #2
	add	iy, sp
	ld	a, 0 (iy)
	ld	(hl), a
;main.c:102: SCREEN_POINTER +=2 ;
	ld	hl, (_SCREEN_POINTER)
	inc	hl
	inc	hl
	ld	(_SCREEN_POINTER), hl
;main.c:103: }
	ret
;main.c:105: void print(String str)
;	---------------------------------
; Function print
; ---------------------------------
_print::
;main.c:108: while(str[i] != '\x00')
	ld	c, #0x00
00101$:
	ld	hl, #2
	add	hl, sp
	ld	a, (hl)
	inc	hl
	ld	h, (hl)
	ld	l, a
	ld	b, #0x00
	add	hl, bc
	ld	a, (hl)
	or	a, a
	ret	Z
;main.c:109: putchr(str[i++]);
	inc	c
	push	bc
	push	af
	inc	sp
	call	_putchr
	inc	sp
	pop	bc
;main.c:110: }
	jr	00101$
;main.c:112: void print40(String str)
;	---------------------------------
; Function print40
; ---------------------------------
_print40::
;main.c:115: while(str[i] != '\x00')
	ld	c, #0x00
00101$:
	ld	hl, #2
	add	hl, sp
	ld	a, (hl)
	inc	hl
	ld	h, (hl)
	ld	l, a
	ld	b, #0x00
	add	hl, bc
	ld	a, (hl)
	or	a, a
	ret	Z
;main.c:116: putchr40(str[i++]);
	inc	c
	push	bc
	push	af
	inc	sp
	call	_putchr40
	inc	sp
	pop	bc
;main.c:117: }
	jr	00101$
;main.c:120: u8 ReadIOReg(u8 r) __naked
;	---------------------------------
; Function ReadIOReg
; ---------------------------------
_ReadIOReg::
;main.c:132: __endasm;
	ld	iy, #2
	add	iy, sp
	ld	a, 0 (iy)
	ld	c, a
	in	a, (c)
	ld	0 (iy), a
	ld	l, 0 (iy)
	ret
;main.c:133: }
;main.c:135: void SetIOReg(u8 r, u8 v) __naked 
;	---------------------------------
; Function SetIOReg
; ---------------------------------
_SetIOReg::
;main.c:149: __endasm;
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	c, 4 (ix)
	ld	a, 5 (ix)
	out	(c), a
	ld	l, a
	pop	ix
	ret
;main.c:150: }
	.area _CODE
	.area _INITIALIZER
	.area _CABS (ABS)

/*! \addtogroup draw 
 * @{
 */
#include <pc88-c.h>

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
    for(u8 yy = 0; yy < h; yy++){
        for(u8 xx = 0; xx < w; xx++){
            *p = *v;
            p++;
            v++;
        }
        p += (80 - w);
    } 
}

void SetPixel(u16 x, u8 y, u8 c)
{
    u8 po = 7 - ((x & 0xff) % 8);
    vu8* p = (vu8*)(0xc000) + (y * 80) + (u8)(x / 8);
        
    if(c & 0b001){
        SETBANK_BLUE()
        *p |= bit(po);
    }
    if(c & 0b010){
        SETBANK_RED()
        *p |= bit(po);
    }
    if(c & 0b100){
        SETBANK_GREEN()
        *p |= bit(po);
    }

}

/*
void DrawRLEBitmap(PlanarBitmap* pb, u16 x, u16 y)
{
    SETBANK_RED();
    vu8* p = (vu8*)(0xc000) + (y * 80) + x;
    const u8* v = pb->r;
    u8 row = 0;
    u8 col = 0;
    u8 loop = 0;
    u16 siz = pb->w * pb->h;
    for(u16 s = 0; s < siz; s++)
    {
        // check if *v is 0x80
        if(*(v + s) == 0x80){
            // copy *v+1 by *v+2 times
            u8 cp = *(v + s + 1);
            loop = *(v + s + 2);
            for(u8 l = 0; l < loop; l++){
                *(p + row + (col * 80)) = cp;
                row++;
                if(row > pb->w) { row = 0; col++; }
            }
            s += (2 + loop);
        }
        else { 
            *(p + row + (col * 80)) = *(v + s);
            row++;
            if(row > pb->w) { row = 0; col++; }
        }
        /*
        for(u8 yy = 0; yy < pb->h; yy++){
            for(u8 xx = 0; xx < pb->w; xx++){
                *p = *v;
                p++;
                v++;
            }
            p += (80 - pb->w);
        }
        */
/*
    }
}
*/

inline void EnableALU(u8 fastmem)
{ 
    SetIOReg(ALU_MODE_CTRL, 0xC9|(fastmem<<4)); 
}
//11001001
inline void DisableALU(u8 fastmem)
{ 
    SetIOReg(ALU_MODE_CTRL, 0x89|(fastmem<<4)); 
}
//11001001

inline void ExpandedGVRAM_On() 
{ 
    SetIOReg(EXPANDED_GVRAM_CTRL, 0x80); 
}
inline void ExpandedGVRAM_Off() 
{ 
    SetIOReg(EXPANDED_GVRAM_CTRL, 0); 
} 


inline void CRT_OFF() {
    __asm
        ld			a,#0b00011001
	    out			(0x40),a
    __endasm;
    Wait_VBLANK();
    __asm 
        xor a,a
        out (0x51),a 
    __endasm;
}

inline void CRT_ON() {
    Wait_VBLANK();
    __asm
        ld			a,#0b00010001
	    out			(0x40),a
    __endasm;
    __asm 
        ld a,#0x20 
        out (0x51),a 
    __endasm;
    
}
/**
;---------------------------------------------------------------------------------------------------
; CRTC/DMAC 設定
; カラー設定用. 白黒の場合は適宜変更のこと.
;
TVRAM			equ		$F3C8

CRTC_CB			equ		1				;0=DMAバーストモード 1=DMAキャラクタモード (1固定)
CRTC_WIDTH		equ		80				;2-80 画面の桁数(設定時-2）
CRTC_BLINK		equ		%10				;%00=16,32 %01=32,64 %10=48,96 %11=64,128 カーソル点滅間隔,アトリビュート点滅間隔
CRTC_HEIGHT		equ		25				;1-64 画面の行数(設定時-1)
CRTC_SPLIT		equ		0				;0=通常表示 1=一行おき表示
CRTC_CURTYPE	equ		%11				;%00=点滅しないアンダーラインカーソル
										;%01=点滅するアンダーラインカーソル
										;%10=点滅しない■カーソル
										;%11=点滅する■カーソル

;CRTC_CHRH		equ		8				;3-32 文字の縦ドット数(設定時-1)
;CRTC_CHRH25N	equ		8				;	25行+15KHz=8ドット
;CRTC_CHRH25H	equ		16				;	25行+24KHz=16ドット
;CRTC_CHRH20N	equ		10				;	20行+15KHz=10ドット
;CRTC_CHRH20H	equ		20				;	20行+24KHz=20ドット

;CRTC_VBH		equ		7				;1-8 (設定時-1)垂直帰線の行数
;CRTC_VBH25N		equ		7				;	25行+15KHz=7行
;CRTC_VBH25H		equ		3				;	25行+24KHz=3行
;CRTC_VBH20N		equ		6				;	20行+15KHz=6行
;CRTC_VBH20H		equ		2				;	20行+24KHz=2行

;CRTC_HBW		equ		32				;6-33 (設定時-2)水平帰線の桁数 15KHz=32
;CRTC_HBW25N		equ		32				;	25行+15KHz=32桁
;CRTC_HBW25H		equ		26				;	25行+24KHz=26桁
;CRTC_HBW20N		equ		32				;	20行+15KHz=32桁
;CRTC_HBW20H		equ		26				;	20行+24KHz=26桁

CRTC_ATSC		equ		%010			;%000=トランスペアレント[白黒]・特殊制御文字有効
										;%001=アトリビュート無し・特殊制御文字無効
										;%010=トランスペアレント・[カラー]
										;%100=ノントランスペアレント白黒・特殊制御文字有効
										;%101=ノントランスペアレント白黒・特殊制御文字無効
CRTC_ATRMAX		equ		20				;1-20 (設定時-1)一行あたりの最大アトリビュート数
CRTC_DM			equ		0				;0=通常表示 1=反転表示
CRTC_MN			equ		1				;0=画面終了時割り込み要求有効 1=画面終了時割り込み要求無効 (1固定)
CRTC_ME			equ		1				;0=特殊制御文字による割り込み有効 1=特殊制御文字による割り込み無効 (1固定)
CRTC_CM			equ		0				;0=カーソル表示無し 1=カーソル表示有り
CRTC_CURX		equ		0				;0-127 カーソルの座標X 40桁の場合は2倍する
CRTC_CURY		equ		0				;0-63 カーソルの座標Y


;---------------------------------------------------------------------------------------------------
InitCRTC:
;	ld			a,%00100001				;RS232C非同期 | Motor_OFF | CDS1200Hz | Colorモード | 80桁
;	out			($30),a
;	ld			a,%00111011				;25Line,カラーグラフィック,グラフィック画面表示,N88BAS,RAM64K,640x200x3
;	out			($31),a

;	call		ClearTVRAM
	ld			a,%00011001				;CRT I/F 同期ビットオン
	out			($40),a
	call		WaitVBlank

	xor			a
	out			($51),a					;Reset CRTC -> Screen Format 設定*5 に続く

	ld			a,%10100000
	out			($68),a					;DMAC: Mode Set(Reset) [AL][TCS][EW][RP][EN3][EN2][EN1][EN0]
	ld			a,TVRAM & $FF			;VRAM TOP $F3C8
	out			($64),a					;Ch.2 DMA Address
	ld			a,TVRAM >> 8
	out			($64),a
	ld			a,(120 * CRTC_HEIGHT - 1) & $FF
	out			($65),a						;Ch.2 Terminal Count (DMA mode=read)
	ld			a,$80 +  ((120 * CRTC_HEIGHT - 1) >> 8)
	out			($65),a

	ld			a,(CRTC_CB << 7) + CRTC_WIDTH - 2
	out			($50),a					;CRTC: Screen Format 1 [C/B] [H6,H5,H4,H3,H2,H1,H0]
	ld			a,(CRTC_BLINK << 6) + CRTC_HEIGHT - 1
	out			($50),a					;CRTC: Screen Format 2 [B1,B0] [L5,L4,L3,L2,L1,L0]

	in			a,($40)					;bit1:0=24KHz 1=15KHz
	and			%00000010
	jr			nz,.k15hz
.k24hz:
	ld			a,INT((400.0 / CRTC_HEIGHT) + 0.5) - 1	;以下、0.5 は四捨五入
	ld			b,INT(24830.0 / (55.416 * (400.0 / CRTC_HEIGHT)) - CRTC_HEIGHT + 0.5) - 1		;20行の時 55.424
	ld			c,INT(21052600.0 / (24830 * 8) - CRTC_WIDTH + 0.5) - 2
	jr			.format3
.k15hz:
	ld			a,INT((200.0 / CRTC_HEIGHT) + 0.5) - 1	;以下、0.5 は四捨五入
	ld			b,INT(15980.0 / (62.422 * (200.0 / CRTC_HEIGHT)) - CRTC_HEIGHT + 0.5) - 1		;20行の時 61.462
	ld			c,INT(14318180.0 / (15980 * 8) - CRTC_WIDTH + 0.5) - 2
.format3:
	add			a,(CRTC_SPLIT << 7) + (CRTC_CURTYPE << 5)
	out			($50),a					;CRTC: Screen Format 3 [S] [C1,C0] [R4,R3,R2,R1,R0]

	ld			a,b
	cp			8
	jr			c,.format4
	ld			a,7						;b>7 は b=7 とする. 15kHz 時の 32 行設定で引っかかる.
.format4:
	add			a,a
	add			a,a
	add			a,a
	add			a,a
	add			a,a						;a = (b << 5) | c
	or			c
	out			($50),a					;CRTC: Scrren Format 4 [V2,V1,V0] [Z4,Z3,Z2,Z1,Z0]

	ld			a,(CRTC_ATSC << 5) + CRTC_ATRMAX - 1
	out			($50),a					;CRTC: Screen Format 5 [AT1,AT0,SC] [A4,A3,A2,A1,A0]

	ld			a,$40 + CRTC_ME * 2 + CRTC_MN
	out			($51),a					;CRTC: Set Interrupt Mask
	ld			a,%11100100
	out			($68),a					;DMAC: Mode Set [AL][TCS][EW][RP][EN3][EN2][EN1][EN0]
	ld			a,$20 + CRTC_DM
	out			($51),a					;CRTC: Start Display

	call		WaitVBlank
.wait:
	in			a,($40)					;表示期間になってから同期ビットの操作をする
	and			$20
	jr			nz,.wait

	ld			a,%00010001				;CRT I/F 同期ビットオフ
	out			($40),a
	ret

;--------------------------------------------------------------------------------------------------
;vblank開始を待つ
WaitVBlank:
	in			a,($40)
	and			$20
	jr			nz,WaitVBlank			;bit5=1 既に vblank 期間中なら待つ
.wait:
	in			a,($40)
	and			$20
	jr			z,.wait					;bit5=0 表示期間中なら待つ
	ret

;--------------------------------------------------------------------------------------------------
; テキスト VRAM をクリア
ClearTVRAM:
	ld			hl,TVRAM
	ld			de,$80E8				;カラー設定用
	ld			c," "
	ld			a,CRTC_HEIGHT			;25行
.loop1:
	ld			b,40
.loop2:
	ld			(hl),c
	inc			l
	ld			(hl),c
	inc			hl
	djnz		.loop2

	ld			b,20
.loop3:
	ld			(hl),d					;桁 下位7bitのみ有効のはず
	inc			l
	ld			(hl),e					;アトリビュート　カラー（色指定）白
	inc			hl
	djnz		.loop3
	dec			a
	jr			nz,.loop1
	ret

;--------------------------------------------------------------------------------------------------
SetCursor:
	ld			a,$80 + CRTC_CM			;CRTC: Load Cursor Position
	out			($51),a
	ld			a,CRTC_CURX
	out			($50),a					;CRTC: パラメータ1 CH POS(0-) 40桁以下の場合は2倍すること
	ld			a,CRTC_CURY
	out			($50),a					;CRTC: パラメータ2 ROW POS(0-)
	ret
**/

void SetMonitor(u8 khz, u8 rows)
{

    CRT_OFF();
    //パラメータ1	W	C/B	H6	H5	H4	H3	H2	H1	H0
    __asm 
        ld a,#((1 << 7) | 78)
        out (0x50),a
    __endasm;
    
    //パラメータ2	W	B1	B0	L5	L4	L3	L2	L1	L0
   // パラメータ3	W	S	C1	C0	R4	R3	R2	R1	R0
    //パラメータ4	W	V2	V1	V0	Z4	Z3	Z2	Z1	Z0
    if(khz == 24)
    {

        if(rows == 20)
        {
            __asm 
                ld a,#((2 << 6) | 19) // line #
                out (0x50),a
                ld a,#((2 << 5) | 19) //lines per char 
                out (0x50),a
                ld a,#((1 << 5) | 25) //vbl interval, hbl interval
                out (0x50),a
            __endasm;
        }
        else { // 25 rows 
                // b is 2
                // c is 24
            __asm
                ld a,#((2 << 6) | 24)
                out (0x50),a
                ld a,#((2 << 5) | 15)
                out (0x50),a
                ld a,#((2 << 5) | 24)
                out (0x50),a
            __endasm;
        }
    }
    else // khz 15
    {
    //25 R: 7 for 15khz, 15 for 24khz
    //20 R: 9 for 15khz, 19 for 24khz
    
    //25 V: 6 for 15khz, 2 for 24khz
    //20 V: 5 for 15khz, 1 for 24khz
    //Z 15khz: 31, 24khz: 25

        if(rows==20)
        {
            __asm
                ld a,#((2 << 6) | 19)
                out (0x50),a
                ld a,#((2 << 5) | 9)
                out (0x50),a
                ld a,#((5 << 5) | 31)
                out (0x50),a
            __endasm;
        }else{ // 15khz 25 rows
            __asm
                ld a,#((2 << 6) | 24)
                out (0x50),a
                ld a,#((2 << 5) | 7)
                out (0x50),a
                ld a,#((6 << 5) | 31)
                out (0x50),a
            __endasm;
        }
    }
    
    __asm 
        ld a,#((0b010 << 5) | 19)
        out (0x50),a 
        ld a,#0x41
        out (0x51),a
    __endasm;
    
    CRT_ON();

}


void DrawTransparentImage_V2(u8 x, u8 y, u8* img, u8 w, u8 h)
{
    u8 tran = img[0] >> 4; //first pixel is always transparent
    u8 lastColor = 99;
    vu8* dst = (vu8*)(0xc000 + (y*80) + x); // start at x,y
    while(h > 0)    // at the top
    {
        s8 bytePxLoc = 7;   // left most pixel
        u8 uw = w;          // current width loop
        while(uw > 0)
        {
            u8 p = *img++;  // get the image byte, 4bpp
            
            if (p != (tran << 4)|tran)
            {            

                u8 p2 = p & 0xf;
                p = p >> 4;
                
                if(lastColor != p)
                {
                    SetIOReg(EXPANDED_ALU_CTRL, p);
                    //lastColor = p;
                } 
                    
                if(p != p2) // different colors
                {
                    if(p != tran)
                        *dst = (1 << bytePxLoc);
                    bytePxLoc--;
                    if(p2 != tran){
                        SetIOReg(EXPANDED_ALU_CTRL, p2);
                        *dst = (1 << bytePxLoc);
                    }
                    bytePxLoc--;
                    lastColor = p2;
                }
                else 
                {   // the next two pixels are the same
                    bytePxLoc--;    
                    if(p != tran)
                        *dst = (0b11 << bytePxLoc);
                    bytePxLoc--;
                    lastColor = p;
                }
            }
            else 
                bytePxLoc -= 2;
            
            if(bytePxLoc < 0){   // reset pixel bit loc
                bytePxLoc = 7;
                dst++;
                uw--;               // decrement width
            }
        }

        h--;        //decrement height
        dst += (80-w);  // go to next pixel row
        
    }
}

void DrawImage_V2(u8 x, u8 y, u8* img, u8 w, u8 h)
{
    u8 lastColor = 99;
    vu8* dst = (vu8*)(0xc000 + (y*80) + x); // start at x,y
    while(h > 0)    // at the top
    {
        s8 bytePxLoc = 7;   // left most pixel
        u8 uw = w;          // current width loop
        while(uw > 0)
        {
            u8 p = *img++;  // get the image byte, 4bpp
            
            if (p > 0)
            {            

                u8 p2 = p & 0xf;
                p = p >> 4;
                
                if(lastColor != p)
                {
                    SetIOReg(EXPANDED_ALU_CTRL, p);
                    //lastColor = p;
                } 
                    
                if(p != p2) // different colors
                {
                    *dst = (1 << bytePxLoc);
                    bytePxLoc--;
                    SetIOReg(EXPANDED_ALU_CTRL, p2);
                    *dst = (1 << bytePxLoc);
                    bytePxLoc--;
                    lastColor = p2;
                }
                else 
                {   // the next two pixels are the same
                    bytePxLoc--;    
                    *dst = (0b11 << bytePxLoc);
                    bytePxLoc--;
                    lastColor = p;
                }
            }
            else 
                bytePxLoc -= 2;
            
            if(bytePxLoc < 0){   // reset pixel bit loc
                bytePxLoc = 7;
                dst++;
                uw--;               // decrement width
            }
        }

        h--;        //decrement height
        dst += (80-w);  // go to next pixel row
        
    }
}


/*! @} */
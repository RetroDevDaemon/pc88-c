//ioregs.h
/*! \addtogroup IORegs
* @{
*/

#define PD_CTL_REG      0x10         /*! W - Printer Out OR Calendar Lock (uPD1990AC) */
/// If PD1990AC - bit0 - bit2 : Command output
///               bit3 : Data output
#define USART_DATA_REG  0x20              /// R/W (pd8251C)
#define USART_CTL_REG   0x21              /// R/W
/** SYSTEM CONTROL REGISTER A //
<pre>
**READ: Dipswitch status
Bit	7	6	5	    4	    3	    2	    1	    0
		   SW1-S5	SW1-S4	SW1-S3	SW1-S2	SW1-S1	SW4-S1

SW1-S5	DEL code response: 0-OK 1-Ignore
SW1-S4	S parameter: 0-ON 1-OFF
SW1-S3	Text lines: 0-25L 1-20L
SW1-S2	Text columns: 0-80C 1-40C
SW1-S1	Boot mode: 0-Terminal 1-BASIC
SW4-S1	BASIC mode: 0-NBASIC 1-N88-BASIC
**WRITE: System Control Port
Bit	7	6	54	3	    2	1	    0
			BS	MTON	CDS	COLOR	40

BS	USART Channel control:
	00: CMT 600baud     10: RS-232C
	01: CMT 1200baud    11: RS-232C
MTON Motor Control: 0-OFF 1-Motor ON
CDS	Carrier control: 0-Space(1200hz) 1-Mark(2400hz)
COLOR Text screen color mode: 0-Color 1-B&W
40 40 Col mode: 0-40c 1-80c
</pre>
*/
#define SYS_CTL_REGA    0x30
/* SYSTEM CONTROL REGISTER B //
<pre>
** WRITE: System Control Port
Bit	76	5		4		3		2	    1	    0
	--	25LINE	HCOLOR	GRAPH	RMODE	MMODE	200LINE
25LINE Hi-res 25 row mode: 0-All other modes 1-ON
HCOLOR Graphic color mode: 0-B&W 1-Color
GRAPH Graphic control: 0-Graphic screens off 1-ON
RMODE ROM mode: 0-N88-BASIC 1-N-BASIC
MMODE RAM mode: 0-ROM/RAM 1-64K RAM
200LINE	Hi-res CRT mode: 0-640×400 1-640x200
  To check n88 rom temporarily: 0b00111001 (enable ROM and N-Basic if available)
  Recommended value: 0b00111011
					  0b00010011 
** READ: Dipswitch status
Bit	7	    6	    5	    4	    3	    2	    1	    0
	SW4-S2	SW3-S0	SW2-S6	SW2-S5	SW2-S4	SW2-S3	SW2-S2	SW2-S1

SW4-S2 V1/V2 Mode: 0-V2 1-V1
SW3-S0 Hispeed mode: 0-Off (V1S) 1-On (V1H, V2)
SW2-S6 Transfer direction: 0-Half-duplex 1-Full-duplex
SW2-S5 X parameter: 0-ON 1-OFF
SW2-S4 Stop bit length: 0-2 bits 1-1 bit
SW2-S3 Data bit length: 0-8 bits 1-7 bits
SW2-S2 Parity bit: 0-Even 1-Odd
SW2-S1 Parity check: 0-Party 1-No parity
</pre>
//*/
#define SYS_CTL_REGB    0x31
#define V1MODE_FLAG 0x80
#define LINES25 (1<<5)
#define HCOLOR (1<<4)
#define GRAPH (1<<3)
#define RMODE (1<<2)
#define MMODE (1<<1)
#define LINE200 (1)
/*! MODE SET REGISTER
[R/W] These registers are not decoded on systems
	before the PC-8801mkII. On models after SR,
	double-check this register on boot.
<pre>
Bit	7	    6	    5	    4	    32	10
	SINTM	GVAM	PMODE	TMODE	AVC	EROMSL

SINTM Sound IRQ: 0-Enable 1-Disable
GVAM Graphics VRAM access mode: 0-Independant 1-Expanded
PMODE Palette mode: 0-Digital 8 color 1-Analog 512 color
TMODE FastRAM access mode (V1H/V2 only):
  0: F000h～FFFFh FastRAM
  1: F000h～FFFFh Main RAM
AVC	Screen output mode: 
  00-TV/Video out   10-Computer out
  01-(off)          11-Option mode
EROMSL Select expaned ROM bank (0-3) 
INDEPENDANT mode is the same as V1 mode. GVRAM is written separately.
EXPANDED mode (SR+ only) lets you access all planes simultaneously.
</pre>
*NOTE: When VRAM is selected, overall processing speed is decreased.
  Remember to page in Main RAM!\n
Default values: xxxx1001
        01001001
*/
#define ALU_MODE_CTRL 0x32
/*! Write only
<pre>
Bit	7	6	    5	    4	    3	2	    1	    0
		ALU21	ALU11	ALU01	    ALU20	ALU10	ALU00
		GV2H    GV1H    GV0H        GV2L    GV1L    GV0L
GV0 - ALU mode for GVRAM bank 0
GV1 - ALU mode for GVRAM bank 1
GV2 - ALU mode for GVRAM bank 2
 Bits arranged as H:L
00: Bit reset
01: Bit set
10: Bit invert
11: No op 
<pre>

*/
#define EXPANDED_ALU_CTRL 0x34 
/*! 
Expanded Access Mode GVRAM Control   W only
<pre>
bit  7      6   5 4     3   2   1   0
	 GAM    -   GDM     -   GC  RC  BC

GAM - GVRAM Access Mode
 0 = Main RAM, 1 = GVRAM
GDM - Graphic data multiplexer control
 00 = Write all planes (check port $34)
 01 = Write last VRAM read data to all 3 planes
 10 = Transfer R plane to B
 11 = Transfer B plane to R
</pre>
* If GDM = 00, you can perform logical operations as explained
using register $34. It appears this mode does not use a buffer.
(There are ways to utilize this to advantage combined with GDM=01 
but I am unclear)

* If GDM = 01, when a single byte is read from VRAM, all 3 planar
data parts are read into a buffer. When writing, the stored buffer
data is written to all 3 planes simultaneously. You can use this to
copy within VRAM without extraneous calculations (ie port $34),
which can be used to create scrolling.

* GC/RC/BC = Bit comp data. Bits written to these fields act as a
color plane filter to VRAM reads. E.g. if bits2-0 are 011 (purple),
the pixel data read from VRAM will be 1 if purple, or 0 if not.

* When ALU EXPANDED access mode is on, reading data from VRAM is
not stored in a register. What is loaded in the register is
'comparison data'; VRAM itself is stored in a hidden buffer.
<pre>
ld a,%10010000  ; gvram access on, 3 plane buffer r/w mode
out ($35),a 
ld a,($c000)	; this is bit comp data, but the
ld ($c0002),a   ; operation commands a vram 'copy'.
</pre>*/
#define EXPANDED_GVRAM_CTRL 0x35
/** Bit 6 - Joystick port output
// Octave4 (A-440)
// Value in HL (number of loop cycles to wait)
// C 315
// D 281
// E 250 
// F 235
// G 209
// A 186
// B 165
// MkII only
*/
#define SYS_MODE_SENSE 0x40   // Write also for beepin'
#define STROBE_PORT 0x40      // Read

//#define FM_IRQ1 0x32	  /*! PRIMARY FM DEVICE */
//#define FM_IRQ2 0xAA	  /*! SECONDARY FM DEVICE */
#define FM_REG_0 0x44     /*! <FR/MR - OPN, FH/MH> - OPNA SSG/FM1-3 */
#define FM_REG_1 0x45     /*! <FR/MR - OPN, FH/MH> - OPNA SSG/FM1-3 */
#define FM_REG_2 0x46     /*! FH/MH - OPNA ADPCM/FM4-6 */
#define FM_REG_3 0x47     /*! FH/MH - OPNA ADPCM/FM4-6 */

/*!CRTC Control Register A

To write to the uPD3301, you must first send the disable command byte to 0x51 (0x00)
 and follow it with 5 parameter bytes to 0x50. ,
 then re-enable display.
               7   6   5   4  3   2    1   0
パラメータ1	W	C/B	H6	H5	H4	H3	H2	H1	H0	パラメータ*5 -> I/Oポート$50
パラメータ2	W	B1	B0	L5	L4	L3	L2	L1	L0
パラメータ3	W	S	C1	C0	R4	R3	R2	R1	R0
パラメータ4	W	V2	V1	V0	Z4	Z3	Z2	Z1	Z0
パラメータ5	W	AT1	AT0	SC	A4	A3	A2	A1	A0

C/B: 0 is DMA Burst, 1 is DMA Character.
If this is set to 0, the text screen will not display.

H0-H6: Set this to total number of columns - 2. (e.g. 38 or 78)

B0-B1: Cursor/attribute blink speed (default 2, 0 fastest)

L5-L0: Total num of rows - 1 (e.g. 19 or 24)

S: 0 is standard display, 1 is line-by-line. Recommended 0...?

C1-C0: Cursor type
00=Non-blinking underline 
01=Blinking underline
10=Non-blinking block
11=Blinking block

R4-R0: Pixels-per-line - 1
Can be 3-32.
This is different when your monitor is 15 vs 24 khz, and 20 vs 25 lines.
25r x 15khz = 7, 24khz = 15
20r x 15khz = 9, 24khz = 19

V2-V0: VBlank interval - 1
Can be 1-8 chars
Depends on 15khz vs 24khz, and 20 vs 25 rows:
25r x 15khz = 6, 24khz = 2
20r x 15khz = 5, 24khz = 1

Z4-Z0: H-Blank interval - 2
Ranges from 6-33.
Depends on 15khz vs 24khz monitor.
15khz=31, 24KHz=25.

AT1-AT0,SC:
Use only these values:
00,0=Transparent BW, enable special control chars < default for bw mode
00,1=Attributes off, disable special control chars
01,0=Transparent color < default for color mode
10,0=Non-transparent BW, enable special control chars
10,1=Non-transparent BW, disable special control chars

A4-A0: Max attributes per line - 1
Can be 1-20. Default 19. 

SHORTCUT COMMAND:
Write 0 singularly to 0x51 to STOP DISPLAY

Write $20 singularly to 0x51 to START DISPLAY

MORE:

コマンド名	R/W	ビット	備考
7	6	5	4	3	2	1	0
SET INTERRUPT MASKコマンド	W	0	1	0	0	0	0	ME	MN	I/Oポート$51

MN	0=画面終了時割り込み要求有効
1=画面終了時割り込み要求無効
ME	0=特殊制御文字による割り込み有効
1=特殊制御文字による割り込み無効
]

RESET INTERRUPTコマンド	W	1	0	1	0	0	0	0	0	コマンド $A0 -> I/Oポート$51

DMA 要求を有効にするらしい。
リセットコマンドでパラメータを設定した後にこのコマンドを発行します。

RESET COUNTERSコマンド	W	1	1	0	0	0	0	0	0	コマンド $C0 -> I/Oポート$51

内部カウンタをすべてクリアするらしい。

READ STATUSコマンド	R	0	0	0	VE	U	N	E	LP	I/Oポート$51を読む

VE	画面表示が有効。
U	DMAアンダーラン(1行分の転送を完了できなかった)発生。発生した場合は画面表示は停止。
N	特殊制御文字による割り込み発生。割り込み要求がアクティブになった。
E	表示終了時の割り込み発生。割り込み要求がアクティブになった。
LP	ライトペン入力あり。座標がラッチされたことを示す。

何もコマンドを書き込まずに I/O ポート $51 を読むとステータスが読み込めます。
*/
#define CRTC_CTLREG_A 0x50     /*! pd3301AC control ports  */
/*!CRTC Control Register B

*/
#define CRTC_CTLREG_B 0x51
/*! W - bits 4-6 only.
<pre>
bit 7   6 5 4   3   2   1
 		G R B          
</pre>         */
#define BG_COLOR_REG 0x52     
/*! Screen Display Control - Write only
<pre>
Bit	7	6	5	4	3	    2	    1	    0
					G2DS	G1DS	G0DS	TEXTDS
</pre>
G2DS GVRAM2 display: 0-ON 1-OFF\n
G1DS GVRAM1 display: 0-ON 1-OFF\n
G0DS GVRAM0 display: 0-ON 1-OFF\n
TEXTDS Text screen display: 0-ON 1-OFF\n
* Set bit 3 to 1 when using 640x400 mode.\n
In color graphics mode, G*DS bits are ignored.
*/
#define SCREEN_DISP_CTL 0x53
/*! bits 0-2 contain palette value 0-7. */
#define PAL_REG0 0x54  
#define PAL_REG1 0x55  // bits 0-2 contain palette value 0-7.
#define PAL_REG2 0x56  // bits 0-2 contain palette value 0-7.
#define PAL_REG3 0x57  // bits 0-2 contain palette value 0-7.
#define PAL_REG4 0x58  // bits 0-2 contain palette value 0-7.
#define PAL_REG5 0x59  // bits 0-2 contain palette value 0-7.
#define PAL_REG6 0x5a  // bits 0-2 contain palette value 0-7.
#define PAL_REG7 0x5b  // bits 0-2 contain palette value 0-7.
/*! Read: GVRAM bank display status
   bit 0 - GVRAM0 
   bit 1 - GVRAM1
   bit 2 - GVRAM3
1 = Selected
0 = Not selected 
*/ 
#define GVRAM_BANK_REG 0x5c

#define GVRAM_SEL_0 0x5c  /*! Write any value to select vram0 (Blue plane) */
#define GVRAM_SEL_1 0x5d  /*! Write any value to select vram1 (Red plane) */
#define GVRAM_SEL_2 0x5e  /*! Write any value to select vram2 (Green plane) */
#define MAINRAM_SEL 0x5f  /*! Write any value to select Main RAM */

#define DMAC_REG0 0x60 // PD8257C DMA controller register (R/W)
#define DMAC_REG1 0x61 // PD8257C DMA controller register (R/W)
#define DMAC_REG2 0x62 // PD8257C DMA controller register (R/W)
#define DMAC_REG3 0x63 // PD8257C DMA controller register (R/W)
#define DMAC_REG4 0x64 // PD8257C DMA controller register (R/W)
#define DMAC_REG5 0x65 // PD8257C DMA controller register (R/W)
#define DMAC_REG6 0x66 // PD8257C DMA controller register (R/W)
#define DMAC_REG7 0x67 // PD8257C DMA controller register (R/W)
#define DMAC_REG8 0x68 // PD8257C DMA controller register  (R/W)

#define CPU_CLOCK_CTL 0x6e // Bit 7 only. 1=4MHz, 0=8MHz
/*! (R/W) Baud rate set register, FH+ only
<pre>
Bits 0-3 only:
0000	75
0001	150
0010	300
0011	600
0100	1200
0101	2400
0110	4800
0111	9600
1000	19200
1001～1111	OFF 
</pre>*/
#define BAUDRATE_REG 0x6f
#define TEXTWIN_OFS_REG 0x70 // R/W: Upper 8 bits of text window address
/*! Expansion ROM select
Only one bit can be set at once:
Bit7 Expansion ROM 7 select\n
Bit6 Expansion ROM 6 select\n
Bit5 Expansion ROM 5 select\n
Bit4 Expansion ROM 4 select\n
Bit3 Expansion ROM 3 select\n
Bit2 Expansion ROM 2 select\n
Bit1 Expansion ROM 1 select\n
Bit0 Expansion ROM 0/4th rom select\n
Writing 0xff selects Main ROM.
*/
#define ROM_EXPANSION_SEL 0x71
#define TEXTWIN_INC_REG 0x78 /*! W: Increment TEXTWIN_OFS once */

/*! To do:
* Registers Ax - Sound board II registers */

/*! Interrupt Level Set Register
<pre>
 bits 0 - 2: Interrupt level 
 0 (high) - RS232 / Serial
 1 - CRTC / Vertical blank
 2 - 1/600s timer
 3 - INT4
 4 - Sound IRQ
 5 - INT2
 6 - FDINT1
 7 - FDINT2
 </pre>
Set to 1 higher than the IRQ you wish to enable.
e.g. writing 0x03 will enable 0, 1, and 2. */
#define IRQ_LEVEL_SET 0xe4
/*! Interrupt Mask Register
Bit 2 - Serial / RS232 interrupt\n
Bit 1 - Vertical blank from CRTC\n
Bit 0 - 1/600s timer interrupt\n
 Setting a bit ALLOWS the interrupt. */
#define IRQ_MASK 0xe6

// Registers Ex - Expanded RAM read/write control registers, including Kanji ROMs
// Registers Fx - Disc subsystem 8255 control regs

/* IRQ Vector Table @ 0xf300~
f300 - Serial
f302 - Vblank
f304 - Sound1
f306 - Sound2 ...
*/

/*! @} */
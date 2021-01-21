# pc88-c 
`makepc88.bat` - Creates app.d88 for use in an emulator<br>
`makeipl.bat` - If you have ASW, you can recompile the crt0/autoloader/floppy driver.<br>
<br>
`ipl.bin` is the autoloader.<br>
It takes the place of `crt0` by setting up the stack.<br>
PC-8x usually has the stack on page 0 (0x0000 to 0x00ff).<br>
`disk.z80` is a disk I/O driver under 256 bytes.(!!)<br>
<br>
D88SAVER.exe is a special tool to make/add the files to a 2D PCx8 disk.<br>
`0 0 1` must be the IPL, `0 0 2` must be the the file to load into RAM.<br>
Stack/data addresses and load code will need tweaking, and will take<br>
a bit to adapt to sdcc's assembler.<br>
<br>
The disk will auto boot. Note that main() must be the FIRST function in the<br>
`main.c` file or some stack wonkyness happens and everything breaks (???)<br>
(help wanted)
```
 bit#      B&W Mode            Color Mode (set)    Color Mode (attr)
  7	      Semigraphic               Color G               -
  6            -                    Color R               -
  5         Underline               Color B           Underline
  4         Upperline             Semigraphic         Upperline
  3            -                   Set color          0=set attr
  2         Reverse                   -	  		        Reverse
  1         Blink		                  -		  	        Blink
  0         Hide		                  -		  	        Hide

Semigraphics:

Semigraphic mode enables all following bytes to be converted to a
unique pixel layout (mode taken from PC-8001). The pixel layout per-
character is displayed as follows, sized according to column mode:
             04
             15
             26
             37
(Semigraphics can have all normal attributes applied to them.)
```
```
35h - Expanded Access Mode GVRAM Control

bit  7   6 5 4 3 2  1  0
     GAM - GDM - GC RC BC

GAM - GVRAM Access Mode
 0 = Main RAM
 1 = GVRAM
GDM - Graphic data multiplexer control
 00 = Write all planes (via port $34)
 01 = Write last read data to all 3 planes
 10 = Transfer R plane to B
 11 = Transfer B plane to R

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

ld a,%10010000  ; gvram access on, 3 plane buffer r/w mode
out ($35),a 
ld a,($c000)	; this is bit comp data, but the
ld ($c0002),a   ; operation commands a vram 'copy'.
```
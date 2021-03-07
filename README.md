# PC88-C (0.0.12)

## Revision history
0.0.12<br>
-Made a real Makefile and removed the .bat/.sh files.<br>
0.0.11 <br>
-Fixed maked88.py. Was not copying in the last byte of the file causing build issues.<br>
-Various tweaks and fixes<br>
<br>
(kind of out of date):<br>
Manual part 1, overview: https://barelyconsciousgames.blogspot.com/2021/02/pc-88-c-framework-for-nec-pc8801.html <br> 
Manual part 2, basic drawing: https://barelyconsciousgames.blogspot.com/2021/02/pc88-c-frame-for-nec-pc-8801-part-2.html <br> 

## Important: requires SDCC to be on the path<br>
If you have SDCC 4.1.0 and Python3, you should be able to build without issue.<br>
(I use the latest build of SDCC to ensure I have the fewest issues.)<br>
Recommended emulators:<br>
M88x5 - Cycle accurate, excellent for debugging, slow (get from Illusion City, below)<br>
XM8 - SDL2, fast, no debugging (http://retropc.net/pi/xm8/index.html) <br> 
Illusion City, huge tools list, recent M88 builds: https://illusioncity.net/nec-pc-88-series-emulators-tools-lists/ <br>
<br>
Brief overview:<br>
`Makefile` - Creates app.d88 for use in an emulator.<br>
Usage: `make PROJECT=examples/1bitsprite`<br>

`makeipl.bat` - If you have ASW, you can recompile the crt0/autoloader/floppy driver.<br>
(A recent ASW build is included in the old/ folder).<br>
<br>
`ipl.bin` is the autoloader.<br>
It takes the place of `crt0` by setting up the stack.<br>
Important byte locations in IPL.BIN:<br>
`0x2F : Number of sectors loaded by autoloader (bytes / 256, default: 0x5F)`<br>
`0x34-0x35 : Stack pointer location (Default: 80 00 (=$0080))`<br>
`0x38-0x39 : Code start location (Default: 00 40 (=$4000))`<br>
(SDCC _data_ is set to 0x0100~).<br>
PC-8x usually has the stack on page 0 (0x0000 to 0x00ff).<br>
The stack pointer is set in IPL.BIN to 0x0080.<br>
`disk.z80` is a disk I/O driver under 256 bytes.(!!)<br>
<br>
maked88.py is a special tool (replaces D88SAVER.EXE) to make/add the files to<br>
a 2D PC-88 disk image.<br>
`0 0 1` must be the IPL, `0 0 2` must be the the file to load into RAM.<br>
Stack/data addresses and load code will need tweaking, and will take<br>
a bit to adapt to sdcc's assembler.<br>
<br>
The disk will auto boot.<br>

A hello world minimal main.c looks like:<br>
```
#include "pc88-c.h"

void main() { 
    print("Hello World!\x00");
    while(1) { }
}
```
<br><br>
A large amount of information is in `pc88-c.h`
<br>
<br>
Python dependencies for tools:<br>
`$ python3 -m pip install --upgrade Pillow`<br>
`$ pip install numpy`<br>
`$ pip install intelhex`<br>

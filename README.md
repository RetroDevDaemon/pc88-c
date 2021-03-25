# PC88-C (0.0.14b)

# Overview 
A very minimal framework for PC88 development using C. <br> 
`src/pc88-c.h` has all the good stuff. <br> 
Check out the `src/lib/` folder for the detailed code. Everything should be laid bare.<br>
<br>
Consider joining the RetroDev Discord to discuss, ask questions, and contribute:<br>
https://discord.gg/Js3uUrc
<br>
## To-Dos
-V2 drawing<br>
-PSG, OPN/+A<br>
-Joystick<br>
-Disk saving<br>
-Kanji ROM, nado<br>
<br>
(kind of out of date):<br>
Manual part 1, overview: https://barelyconsciousgames.blogspot.com/2021/02/pc-88-c-framework-for-nec-pc8801.html <br> 
Manual part 2, basic drawing: https://barelyconsciousgames.blogspot.com/2021/02/pc88-c-frame-for-nec-pc-8801-part-2.html <br> 

## What's new
0.0.14b<br>
-Fixed SetCursorPos (wasn't updating ROWPOINTER)<br>
-Added VERY BASIC math routines<br>
-Added CMDFLAGS to the `make` for your own build flags. <br>
e.g. `make PROJECT=examples/hachinoid USEDSEC=0x49 CMDFLAGS=--cyclomatic`<br>
-Moved stack pointer back to 0x100 (doubled stack size)<br>
<br>

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
Usage: `make PROJECT=examples/1bitsprite USEDSEC=0x2f`<br>
Creates app.d88 in root of '1bitsprite' project and overrides the value of the initial number of sectors to copy in from the autoloader to 2Fh.<br>
You can also do `make IPL` to recompile src/ipl.z80 and src/disk.z80.<br>
<br>
`ipl.bin` is the autoloader.<br>
In some ways it takes place of crt0. For me, src/crt0.c instead sets up the PC88-C environment. Notably, the putchr/print functions are probably quite clumsy since I haven't written them for a system before. <br>
Important byte locations in IPL.BIN:<br>
`0x2F : Number of sectors loaded by autoloader (bytes / 256, default: 0x5F)`<br>
`0x34-0x35 : Stack pointer location (Default: 00 01 (=$0100))`<br>
`0x38-0x39 : Code start location (Default: 00 10 (=$1000))`<br>
(SDCC _data_ is set to 0x0100~).<br>
PC-8x usually has the stack on page 0 (0x0000 to 0x00ff).<br>
The stack pointer is initially set in IPL.BIN to 0x0100.<br>
`disk.z80` is a disk I/O driver under 256 bytes.(!!)<br>
<br>
maked88.py is a special tool (replaces D88SAVER.EXE) to make/add the files to<br>
a 2D PC-88 disk image.<br>
`0 0 1` must be the IPL, `0 0 2` must be the the file to load into RAM.<br>
I tried to make it as easy as possible to adjust memory addresses and load options.<br>
<br>
The disk will auto boot!<br>

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
```
$ python3 -m pip install --upgrade Pillow
$ pip install numpy
$ pip install intelhex
```
<br>

## Revision history
0.0.14<br>
-Fixed EraseVRAMArea - deleted old code fragments<br>
-Still WIP Hachinoid example<br>
-Added BEEP support<br>
0.0.13b<br>
-Added IPL target to makefile (`make IPL`). No longer requires ASW, compiles with SDCC.<br> 
-Updated png288.py. If you append `-rle`, `/rle` etc. it will RLE-encode the file with the following schema:<br>
```
0x80 [X] [Y] : Duplicate [X] by [Y] times.
All other bytes : Copy through
```
-Added getsjis.py. Scans and prints a file for N88-BASIC kana. <br>
-Added Sprite, XYPos structs to lib header<br>
-WIP: Arkanoid clone example<br>
0.0.12<br>
-Made a real Makefile and removed the .bat/.sh files.<br>
0.0.11 <br>
-Fixed maked88.py. Was not copying in the last byte of the file causing build issues.<br>
-Various tweaks and fixes<br>
<br>


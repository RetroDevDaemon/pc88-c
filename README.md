# PC88-C
`makepc88.bat` - Creates app.d88 for use in an emulator<br>
e.g. `makepc88 main.bin` will give you filesize information.<br>
`makeipl.bat` - If you have ASW, you can recompile the crt0/autoloader/floppy driver.<br>
<br>
`ipl.bin` is the autoloader.<br>
It takes the place of `crt0` by setting up the stack.<br>
`0x2F : Number of sectors loaded by autoloader (bytes / 256, default: 0x4F)`<br>
`0x34-0x35 : Stack pointer location (Default: 0x0080)<br>
`0x38-0x39 : Code start location (Default: 0x1000)<br>
(SDCC _data is set to 0x0100 - 0x0fff).<br>
PC-8x usually has the stack on page 0 (0x0000 to 0x00ff).<br>
ATM, it is located at 0x80 because its unclear to me in SDCC if<br>
the stack/ptr are meant to grow up or down.<br>
`disk.z80` is a disk I/O driver under 256 bytes.(!!)<br>
<br>
maked88.py is a special tool (replaces D88SAVER.EXE) to make/add the files to<br>
a 2D PC-88 disk image.<br>
`0 0 1` must be the IPL, `0 0 2` must be the the file to load into RAM.<br>
Stack/data addresses and load code will need tweaking, and will take<br>
a bit to adapt to sdcc's assembler.<br>
<br>
The disk will auto boot. Note that main() must be the FIRST function in the<br>
`main.c` file or some stack wonkyness happens and everything breaks (???)<br>
(help wanted)
<br><br>
A large amount of information is in `pc88-c.h`
<br>
<br>
Python dependencies for tools:<br>
`$ python3 -m pip install --upgrade Pillow`<br>
`$ pip install numpy`<br>
`$ pip install intelhex`<br>

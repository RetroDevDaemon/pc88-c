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

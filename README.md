# pc88-c 
`ipl.bin` is the autoloader.<br>
It takes the place of `crt0` by setting up the stack.<br>
PC-8x usually has the stack on page 0 (0x0000 to 0x00ff).<br>
The autoloader has a disk I/O driver under 256 bytes.(!!)<br>
`makepc88.bat` uses D88SAVER.exe to add the files to a 2D disk.<br>
`0 0 1` must be the IPL, `0 0 2` must be the the file to load into RAM.<br>
This is going to need tweaking, unfortunately not adapted to sdasz80 yet.<br>
<br>
The disk is autoloading. Note that main() must be the FIRST function in the<br>
`main.c` file or some stack wonkyness happens and everything breaks (???)<br>

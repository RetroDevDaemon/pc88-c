@echo OFF
del app.d88
D88SAVER app.d88 -2d

set usedsec=79
sdcc -mz80 --code-loc 0x1000 --stack-loc 0x0080 --data-loc 0x0100 --fomit-frame-pointer --no-std-crt0 src/main.c
hex2bin main.ihx

D88SAVER app.d88 ipl.bin 0 0 1
D88SAVER app.d88 main.bin 0 0 2

rem del main.bin
del *.ihx 
del *.lk 
del *.lst 
del *.map 
del *.noi 
del *.sym 
rem del *.asm 
del *.rel 

echo Current Memory Map:
echo Stack: 0x000 - 0x0ff
echo Data: 0x0100 - 0x0fff
echo Prog: 0x1000 ~
echo On disk:
set /a by=%usedsec% * 272
echo 0 0 1 - IPL (256b) 
echo 0 0 2-%usedsec% - Main (%by%b)

if "%1" == "" goto:eof

set sz=%~z1
echo %1 is %sz% bytes.
if %sz% GTR %by% (
    echo Warning: Filesize exceeded %by% bytes!
    echo Recompile ipl.z80!
)


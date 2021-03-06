@echo OFF

SET proj=%1
SET filename=app.d88
SET usedsec=0x5f
SET emuexe=C:\Users\Bent\Downloads\m88\m88x5.exe 

REM CREATE FRESH app.d88 FILE
del %filename%
REM D88SAVER app.d88 -2d
python3 tools/maked88.py %filename%

rem SET usedsec TO APPROPRIATE SIZE TO COPY FROM DISK.
python3 tools/hexer.py src/ipl.bin 0x2f %usedsec%

REM COMPILE WITH SDCC AND CONVERT TO BINARY FORMAT
echo Compiling sources...
sdcc -mz80 --stack-loc 0x80 --data-loc 0x0100 --code-loc 0x4000 --fomit-frame-pointer --no-std-crt0 -Isrc %1/main.c
rem REQUIRES: pip install intelhex
python3 tools/fixboot.py src/ipl.bin 
python3 tools/hex2bin.py main.ihx main.bin
rem hex2bin main.ihx

REM ADD BOOTLOADER AND COMPILED BINARY TO D88
python3 tools/maked88.py %filename% src/ipl.bin 0 0 1
python3 tools/maked88.py %filename% main.bin 0 0 2
rem D88SAVER app.d88 ipl.bin 0 0 1
rem D88SAVER app.d88 main.bin 0 0 2

REM Delete build artifacts
rem del main.bin
del *.ihx 
del *.lk 
del *.lst 
del *.map 
del *.noi 
del *.sym 
rem del *.asm 
del *.rel 

REM PRINT MEMORY ESTIMATIONS
echo On disk:
set /a by=%usedsec% * 256
echo 0 0 1 - IPL (256b) 
echo 0 0 2-%usedsec% - Main (%by%b)


%emuexe%

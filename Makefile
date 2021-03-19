# Very simple makefile for PC88-C.
#  Please use official GNU make installer 3.81!
#  i.e. do not use the one distributed with SGDK etc.

CC=sdcc
CFLAGS=-Isrc --cyclomatic 
PY=python3
DEL=rm -rf

## USED SECTORS ON DISC ##
USEDSEC=0x5f
# If this number isn't correct, 
# the app won't load right!
# Make sure it's big enough!

## PROJECT FOLDER ##
PROJECT=examples/helloworld
# This can also be explicit on the commandline i.e.
# $ make PROJECT=myproj

## MEMORY LOCATIONS ##
STACK=0x80
DATA=0x100
CODE=0x1000
# Stack should stay in ZP.
# This is due to VRAM being in C000~.

88FLAGS=-mz80 --stack-loc $(STACK) --code-loc $(CODE) --data-loc $(DATA) --fomit-frame-pointer --no-std-crt0

## DISC FILE NAME ##
APPNAME=app.d88

## EMULATOR EXECUTABLE ##
EMUEXE=C:\Users\Bent\Downloads\m88\m88x5.exe 

# This is updated when new .c files are added
PC88CFILES=out/crt0.rel out/getkeydown.rel out/waitvblank.rel out/diskload.rel out/ioreg.rel out/draw.rel out/textmode.rel out/beep.rel out/vram_util.rel  

out/%.rel: src/lib/%.c
	sdcc -c -mz80 $(CFLAGS) -o $@ $< 

default: $(PROJECT) $(PC88CFILES)
	$(PY) tools/maked88.py $(APPNAME)
	$(PY) tools/hexer.py src/ipl.bin 0x2f $(USEDSEC)
	$(CC) $(88FLAGS) $(CFLAGS) $(PROJECT)/main.c out/*.rel 
	$(PY) tools/fixboot.py src/ipl.bin 
	$(PY) tools/hex2bin.py main.ihx main.bin
	$(PY) tools/maked88.py $(APPNAME) src/ipl.bin 0 0 1
	$(PY) tools/maked88.py $(APPNAME) main.bin 0 0 2	
	@$(DEL) *.ihx
	@$(DEL) *.lk
	@$(DEL) *.lst
	@$(DEL) *.map
	@$(DEL) *.noi
	@$(DEL) *.rel
	@$(DEL) *.sym
	@$(DEL) out/*.asm 
	@$(DEL) out/*.sym 
	@$(DEL) out/*.lst   
	$(EMUEXE)

IPL: src/ipl.z80 src/disk.z80 
	sdasz80 -o ipl.rel src/ipl.z80 
	sdldz80 -b _CODE=0xC000 -i ipl.ihx ipl.rel 
	$(PY) tools/hex2bin.py ipl.ihx ipl.bin 
	@$(DEL) *.ihx 
	@$(DEL) *.rel 
	@cp ipl.bin src/
	@$(DEL) ipl.bin 

clean:
	@$(DEL) out/*.*
	@$(DEL) $(APPNAME)
	@$(DEL) *.ihx
	@$(DEL) *.lk
	@$(DEL) *.lst
	@$(DEL) *.map
	@$(DEL) *.noi
	@$(DEL) *.rel
	@$(DEL) *.sym
	@$(DEL) *.bin
	@$(DEL) *.asm

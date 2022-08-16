# Very simple makefile for PC88-C.
#  Please use official GNU make installer 3.81!
#  i.e. do not use the one distributed with SGDK etc.

CC=sdcc
LD=sdldz80 
AS=sdasz80
CFLAGS=-Isrc -Isrc/lib
OPTIMIZE=0
ifeq ($(OPTIMIZE), 1) 
CMDFLAGS=--cyclomatic --max-allocs-per-node100000 --opt-code-speed
else
CMDFLAGS=--max-allocs-per-node2000 #--fomit-frame-pointer
endif
PY=python3 -B
DEL=rm -rf

## USED SECTORS ON DISC ##
USEDSEC=0x9f
# If this number isn't correct, 
# the app won't load right!
# Make sure it's big enough!

## PROJECT FOLDER ##
PROJECT=examples/helloworld
# This can also be explicit on the commandline i.e.
# $ make PROJECT=myproj

## MEMORY LOCATIONS ##
# stack grows downward, code/data grows upward(?)
default: STACK=0xff
default: DATA=0xb000
default: CODE=0x100
dirty: STACK=0xff
dirty: DATA=0xb000
dirty: CODE=0x100
binary: STACK=0xc000
binary: DATA=0xd800
binary: CODE=0xc000

# Stack should stay in ZP.
# This is due to VRAM being in C000~.

88FLAGS=-mz80 \
	--stack-loc $(STACK) --code-loc $(CODE) --data-loc $(DATA)\
	--no-std-crt0\
	#--fomit-frame-pointer 

## DISC FILE NAME ##
APPNAME=app.d88

## EMULATOR EXECUTABLE ##
#EMUEXE=C:\Users\Bent\Downloads\m88\m88x5.exe 
EMUEXE=~/Downloads/quasi88-0.6.4/quasi88.sdl -mem_wait -fdc_wait -fmgen ./app.d88
#EMUEXE=quasi88 app.d88

XDISK=~/Projects/xdisk3/pc/xdisk3 w -p/dev/ttyUSB0 -1 -d2 $(APPNAME)

# This is updated when new .c files are added
PC88CFILES=out/crt0.rel \
	out/math.rel \
	out/getkeydown.rel \
	out/waitvblank.rel \
	out/diskload.rel \
	out/ioreg.rel \
	out/vram_util.rel \
	out/sys.rel 
#	out/textmode.rel \
#	out/beeper.rel \
#	out/draw.rel 
#	out/opn.rel ; OPTIONAL, include opn.h
#PC88CFILES=out/crt0.rel out/ioreg.rel out/textmode.rel out/sys.rel


out/%.rel: src/lib/%.c 
	@if [ ! -d "out" ]; then mkdir out; fi
	sdcc -c -mz80 $(CFLAGS) $(CMDFLAGS) -o $@ $<


default: clean $(PROJECT) $(PC88CFILES)
	$(PY) tools/maked88.py $(APPNAME)
	$(PY) tools/hexer.py src/ipl.bin 0x2f $(USEDSEC)
	$(CC) $(88FLAGS) $(CFLAGS) $(CMDFLAGS) $(PROJECT)/main.c out/*.rel -o out/main.ihx
	$(PY) tools/fixboot.py out/main.map src/ipl.bin
	$(PY) tools/hex2bin.py out/main.ihx main.bin
	$(PY) tools/maked88.py $(APPNAME) src/ipl.bin 0 0 1
	$(PY) tools/maked88.py $(APPNAME) main.bin 0 0 2	
	


run:
	$(EMUEXE)

xdisk:
	$(XDISK)

#m88bin:
#	$(AS) -los out/music2.rel src/music2.z80
#	$(LD) -i out/music2.ihx out/music2.rel
#	$(PY) tools/hex2bin.py out/music2.ihx music2.bin


binary: $(PROJECT) $(PC88FILES)
	$(DEL) out/main.*
	$(CC) $(88FLAGS) $(CFLAGS) $(CMDFLAGS) $(PROJECT)/main.c out/*.rel -o out/main.ihx
	$(PY) tools/hex2bin.py out/main.ihx main.bin
	$(PY) tools/bin2bas.py main.bin


IPL: src/ipl.z80 src/disk.z80
	sdasz80 -o ipl.rel src/ipl.z80
	sdldz80 -b _CODE=0xC000 -i ipl.ihx ipl.rel
	$(PY) tools/hex2bin.py ipl.ihx ipl.bin
	@$(DEL) *.ihx
	@$(DEL) *.rel
	@cp ipl.bin src/
	@$(DEL) ipl.bin

clean:
	@$(DEL) out
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
	@echo Directories cleaned.

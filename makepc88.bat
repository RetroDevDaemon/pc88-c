@echo OFF
del app.d88
D88SAVER app.d88 -2d

sdcc -mz80 --code-loc 0x1000 --stack-loc 0x0000 --data-loc 0x0100 --no-std-crt0 main.c
hex2bin main.ihx

D88SAVER app.d88 ipl.bin 0 0 1
D88SAVER app.d88 main.bin 0 0 2

del main.bin
del *.ihx 
del *.lk 
del *.lst 
del *.map 
del *.noi 
del *.sym 
del *.asm 
del *.rel 

echo Current Memory Map:
echo Stack: 0x000 - 0x0ff
echo Data: 0x0100 - 0x0fff
echo Prog: 0x1000 ~
echo On disk:
echo 0 0 1 - IPL (256b) 
echo 0 0 2 - Main (256b)

@echo ON 

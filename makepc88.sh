#!/bin/sh
# MAKEPC88.SH
# Set these to suit your needs!
filename=app.d88
usedsec=05
emuexe=C:/Users/Bent/Downloads/m88/m88x5.exe
# Check if we're on windows
if [ `uname | grep "MINGW"` ] ; then 
  PYR="winpty python3"
else 
 PYR="python3" 
fi
# Clean disk
rm $filename 
$PYR tools/maked88.py $filename 
$PYR tools/hexer.py ipl.bin 0x2f $usedsec 
sdcc -mz80 --code-loc 0x1000 --data-loc -0x0100 --fomit-frame-pointer --no-std-crt0 -Isrc examples/helloworld/main.c 
$PYR tools/hex2bin.py main.ihx main.bin 
$PYR tools/maked88.py $filename ipl.bin 0 0 1
$PYR tools/maked88.py $filename main.bin 0 0 2 
rm *.ihx 
rm *.lk 
rm *.lst 
rm *.map 
rm *.noi 
rm *.sym 
rm *.rel 
fsize=$(wc -c main.bin | awk '{print $1}')
let fsize=fsize/256 
if [ $fsize -ge $usedsec ] ; then 
  echo Update build script! main.bin too large for IPL!
fi

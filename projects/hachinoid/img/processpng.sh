#!/bin/sh
for fn in raw/*.png; do 
    winpty python3 ../../../tools/png288.py $fn
done
#winpty python3 ../../../tools/png288.py raw/title_1.png /rle
winpty python3 ../../../tools/png288.py raw/title_2.png /rle

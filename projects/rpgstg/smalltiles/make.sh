cd ../../../
make -f ./Makefile PROJECT=./projects/rpgstg/smalltiles/ USEDSEC=0x9f
echo ---*---*---*---*---*---*---
echo Writing "map1.bin" to disk...
python3 ./tools/maked88.py ./app.d88 ./projects/rpgstg/smalltiles/map1.bin 0 4 1 

del app.d88
D88SAVER app.d88 -2d

set TRG=ipl
asw -cpu z80undoc -g map %TRG%.z80
p2bin %TRG%.p -l 0 -r $-$
del %TRG%.p
del *map.txt
del *.map
D88SAVER app.d88 ipl.bin 0 0 1

set ascmd=-D PC80=1
set TRG=main
asw -cpu z80undoc -g map %TRG%.z80
p2bin %TRG%.p -l 0 -r $-$
del %TRG%.p
del *map.txt
del *.map
D88SAVER app.d88 main.bin 0 0 2

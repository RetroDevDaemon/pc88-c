set TRG=ipl
asw -cpu z80undoc -g map %TRG%.z80
p2bin %TRG%.p -k -l 0 -r $-$

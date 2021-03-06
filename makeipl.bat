set TRG=ipl
asw -cpu z80undoc -g map src/%TRG%.z80
p2bin src/%TRG%.p -k -l 0 -r $-$

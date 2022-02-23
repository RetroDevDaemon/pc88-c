#!/usr/bin/python3
# parsem88dat.py
import sys
#if(len(sys.argv)!=3):
#  print("usage:\n$ parsem88dat.py /path/to/voice.dat <#>\n\twhere # is a required test parameter. \n\tthe instrument # given will be printed from the bank\n\tduring conversion.\n")
#  sys.exit()
#f=open(sys.argv[1],"rb")
#inbytes=f.read()
#f.close()
#instno=int(sys.argv[2])

from instruments import instrs 

j=0

outby=[]

for i in instrs:
  hx = '${:02X}'.format( ((i[0]<<3)|i[1]) )
  if(j == i):
    print('$Bx:',hx)
  outby.append(int(hx[1:],16))

  hx = '${:02X}'.format( ((i[10]<<4)|i[9]) )
  hx2 = '${:02X}'.format( ((i[10+9]<<4)|i[9+9]) )
  hx3 = '${:02X}'.format( ((i[10+9+9]<<4)|i[9+9+9]) )
  hx4 = '${:02X}'.format( ((i[10+9+9+9]<<4)|i[9+9+9+9]) )  
  if(j == i):
    print('$3x:',hx,hx2,hx3,hx4)
  outby.append(int(hx[1:],16))
  outby.append(int(hx2[1:],16))
  outby.append(int(hx3[1:],16))
  outby.append(int(hx4[1:],16))

  hx = '${:02X}'.format( i[7] )
  hx2 = '${:02X}'.format( i[7+9] )
  hx3 = '${:02X}'.format( i[7+9+9] )
  hx4 = '${:02X}'.format( i[7+9+9+9] )
  if(j == i):
    print('$4x:',hx,hx2,hx3,hx4)
  outby.append(int(hx[1:],16))
  outby.append(int(hx2[1:],16))
  outby.append(int(hx3[1:],16))
  outby.append(int(hx4[1:],16))

  hx = '${:02X}'.format( i[2] | (i[8]<<6) )
  hx2 = '${:02X}'.format( i[2+9] | (i[8+9]<<6) )
  hx3 = '${:02X}'.format( i[2+9+9] | (i[8+9+9]<<6) )
  hx4 = '${:02X}'.format( i[2+9+9+9] | (i[8+9+9+9]<<6) )
  if(j == i):
    print('$5x:',hx,hx2,hx3,hx4)
  outby.append(int(hx[1:],16))
  outby.append(int(hx2[1:],16))
  outby.append(int(hx3[1:],16))
  outby.append(int(hx4[1:],16))

  hx = '${:02X}'.format( i[3] )
  hx2 = '${:02X}'.format( i[3+9]  )
  hx3 = '${:02X}'.format( i[3+9+9]  )
  hx4 = '${:02X}'.format( i[3+9+9+9]  )
  if(j == i):
    print('$6x:',hx,hx2,hx3,hx4)
  outby.append(int(hx[1:],16))
  outby.append(int(hx2[1:],16))
  outby.append(int(hx3[1:],16))
  outby.append(int(hx4[1:],16))
    
  hx = '${:02X}'.format( i[4] )
  hx2 = '${:02X}'.format( i[4+9]  )
  hx3 = '${:02X}'.format( i[4+9+9]  )
  hx4 = '${:02X}'.format( i[4+9+9+9]  )
  if(j == i):
    print('$7x:',hx,hx2,hx3,hx4)
  outby.append(int(hx[1:],16))
  outby.append(int(hx2[1:],16))
  outby.append(int(hx3[1:],16))
  outby.append(int(hx4[1:],16))

  hx = '${:02X}'.format( (i[6]<<4)|i[5] )
  hx2 = '${:02X}'.format( (i[6+9]<<4)|i[5]  )
  hx3 = '${:02X}'.format( (i[6+9+9]<<4)|i[5]  )
  hx4 = '${:02X}'.format( (i[6+9+9+9]<<4)|i[5]  )
  if(j == i):
    print('$8x:',hx,hx2,hx3,hx4)
  outby.append(int(hx[1:],16))
  outby.append(int(hx2[1:],16))
  outby.append(int(hx3[1:],16))
  outby.append(int(hx4[1:],16))

  if(j == i):
    print('name:',i[len(i)-1])
  ## Uncomment 90-98 to write 32-byte string-supported dat
  #ct=0
  #for g in i[len(i)-1]:
  #  outby.append(ord(g))
  #  print(g)
  #  ct+=1
  #while(ct<7):
  #  outby.append(0)
  #  ct+=1
  #print(ct)
""" 
//FB,AL
//AR,DR,SR,RR,SL,TL,KS,ML,DT ; op1
//AR,DR,SR,RR,SL,TL,KS,ML,DT ; op2
//AR,DR,SR,RR,SL,TL,KS,ML,DT ; op3
//AR,DR,SR,RR,SL,TL,KS,ML,DT ; op4
// FEEDBACK, [Algorithm=Connection]
//AttackRate, DecayRate, SustainRate, RelRate, SustainLevel, TotalLevel, KeyScale, Multi, Detune
//.. 
//  $B0, B1, B2
// 7 6 5 4 3 2 1 0 
// - - [FB ] [ALG]

//  $30.4.8.c 31.5.9.d 32.6.a.e
// 7 6 5 4 3 2 1 0
// - [DT ] [MULTI] 

//  $40..
// 7 6 5 4 3 2 1 0
// - [TotalLevel ]

//  $50
// 7 6 5 4 3 2 1 0
//[KS] - [AttackR]

//  $60 
// 7 6 5 4 3 2 1 0
// - - [DecayRate]

//  $70
// 7 6 5 4 3 2 1 0
// - - [SustnRate]

//  $80
// 7 6 5 4 3 2 1 0
// [SusLv] [RelsRt]
 """
#print(outby)
print(len(outby),"bytes written (lite mode).")
print(len(instrs),"instruments found in given .dat.")

f=open('banklite.dat',"wb")
for b in outby:
  f.write(bytes([b]))
f.close()
print('banklite.dat written OK.\n')
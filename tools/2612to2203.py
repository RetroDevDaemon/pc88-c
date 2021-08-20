#!/usr/bin/python3

# Conversion: 
#  FM 1-6:
#   Same register numbers and values
#  FM6/DAC:
#   Disable
#  Data blocks:
#   ignore/skip
#  SSG>PSG: (r0-rf)
#   split data byte into register and data
#   -change data to 16-bit were needed
#   -noise on 1 channel(?)

import sys 

""" Function defs """

def Usage():
        print("Usage:\n\
$ 2612to2203.py /path/to/file.vgm\n")

""" Conversion script """

# args test 
if len(sys.argv) < 2:
        Usage()

# read in vgm file
f = open(sys.argv[1], 'rb')
inby = f.read() 
f.close() 

# file type test
vgmstr = chr(inby[0]) + chr(inby[1]) + chr(inby[2]) + chr(inby[3])
vgmver = inby[8] | (inby[9] << 8) 
if((vgmstr != "Vgm ") and ((vgmver != 0x160) or (vgmver != 0x150))):
        print("Error: Not a valid VGM v1.50/1.60 file!")
        sys.exit()

# main loop 
outsong = []
vgmptr = 0x34 + (inby[0x34] | (inby[0x35]<<8)) # 0x80
datstart = vgmptr 
i = 0 
while i < datstart:
        outsong.append(inby[i])
        i += 1
outsong.append(0x55)
outsong.append(0x7)
outsong.append(0b00111000)
# 477
# 954
# 1907
while(vgmptr < len(inby)):
        if inby[vgmptr] == 0x67: # data block, skip
                vgmptr += 1 # 0x66
                vgmptr += 1 # data type
                vgmptr += 1 # size low byte
                sz = inby[vgmptr] | (inby[vgmptr+1]<<8) | (inby[vgmptr+2]<<16) | (inby[vgmptr+3]<<24)
                vgmptr += sz + 3
                print('data block skipped by ' + str(sz) + ' bytes')
        elif inby[vgmptr] == 0x50:
                vgmptr += 1 # data
                psgdat = inby[vgmptr]
                # format:
                # 10 bits - 0-2048
                r = (inby[vgmptr] & 0b01110000) >> 4
                #                   3210
                d = inby[vgmptr] & 0b00001111
                d2 = 0
                # 3579540
                # 3993600
                # 1ac <> d6
                # get full data size for frequency 
                if((r == 0) or (r == 2) or (r == 4)):
                        vgmptr += 2 # data2      987654   
                        #print(hex(inby[vgmptr]))
                        d2 = (inby[vgmptr] & 0b00111111) << 4
                        #d2 = d2 >> 8
                        #print(hex(d+d2))
                if(r==0):
                        #print("tone 1 freq", end=' ')
                        d = d + d2  
                        d = int(d * 1.1157)
                       # print('real', d)
                        outsong.append(0x55)
                        outsong.append(0)
                        outsong.append(d & 0xff)
                        outsong.append(0x55)
                        outsong.append(1)
                        outsong.append((d & 0xff00) >> 8)
                        #print("PSG: r0,r1", hex(d & 0xff), hex((d & 0xff00) >> 8), end = ' ')
                        
                elif(r==1):
                        #print("tone 1 att", end=' ')
                        if d == 0xf:
                                d = 0
                        elif d == 0:
                                d = 0
                        else:
                                v = 14
                                if (d & 0b1000):
                                        v -= 8
                                if (d & 0b100):
                                        v -= 4
                                if (d & 0b10):
                                        v -= 2
                                if (d & 0b1):
                                        v -= 1
                                d = v
                        outsong.append(0x55)
                        outsong.append(8)
                        outsong.append(d)
                        #print("PSG: r8", d, end=' ')
                elif(r==2):
                        #print("tone 2 freq", end=' ')
                        #print("PSG: r2,r3", hex(d & 0xff), hex(d2), end = ' ')
                        d = d + d2  
                        d = int(d * 1.1157)
                        outsong.append(0x55)
                        outsong.append(2)
                        outsong.append(d & 0xff)
                        outsong.append(0x55)
                        outsong.append(3)
                        outsong.append((d & 0xff00) >> 8)
                        #print("PSG: r2,r3", hex(d & 0xff), hex((d & 0xff00) >> 8), end = ' ')
                        
                elif(r==3):
                        #print("tone 2 att", end=' ')
                        if d == 0xf:
                                d = 0
                        elif d == 0:
                                d = 0
                        else:
                                v = 14
                                if (d & 0b1000):
                                        v -= 8
                                if (d & 0b100):
                                        v -= 4
                                if (d & 0b10):
                                        v -= 2
                                if (d & 0b1):
                                        v -= 1
                                d = v
                        outsong.append(0x55)
                        outsong.append(9)
                        outsong.append(d)
                        #print("PSG: r9", d, end=' ')
                elif(r==4):
                        #print("tone 3 freq", end=' ')
                        #print("PSG: r4,r5", hex(d & 0xff), hex(d2), end = ' ')
                        d = d + d2  
                        d = int(d * 1.1157)
                        outsong.append(0x55)
                        outsong.append(4)
                        outsong.append(d & 0xff)
                        outsong.append(0x55)
                        outsong.append(5)
                        outsong.append((d & 0xff00) >> 8)
                        #print("PSG: r4,r5", hex(d & 0xff), hex((d & 0xff00) >> 8), end = ' ')
                elif(r==5):
                        #print("tone 3 att", end=' ')
                        if d == 0xf:
                                d = 0
                        elif d == 0:
                                d = 0
                        else:
                                v = 14
                                if (d & 0b1000):
                                        v -= 8
                                if (d & 0b100):
                                        v -= 4
                                if (d & 0b10):
                                        v -= 2
                                if (d & 0b1):
                                        v -= 1
                                d = v
                        outsong.append(0x55)
                        outsong.append(10)
                        outsong.append(d)
                        #print("PSG: r10", d, end=' ')
                else: 
                        print('error', r, d)
                """
                elif(r==6):
                        print("noise ctl", end=' ')
                        # frequency is 0-2, 3 is "tone #3 output"?
                        d = d & 0b11
                        d = d << 2  # x4 for now?
                        print("PSG: r6", d, end=' ')
                        outsong.append(6)
                        outsong.append(d & 0xf)
                elif(r==7):
                        print("noise att", end=' ')
                        d = 15-d 
                        d = int(d/2)
                        d = 1 # io input off, noise off, tone on FIXME
                        outsong.append(0x7)
                        outsong.append(d)
                """
                
                        
                #print('')
                #print(d)
        elif inby[vgmptr] == 0x62:
                outsong.append(0x62)
        elif inby[vgmptr] == 0x66:
                outsong.append(0x66)
                vgmptr = len(inby)
                print("song end")
        vgmptr += 1

#print(outsong)
f = open("outsong.vgm", 'wb')
i = 0
while i < len(outsong):
        #print(type(outsong[i]))
        f.write(bytes([outsong[i]]))
        i += 1
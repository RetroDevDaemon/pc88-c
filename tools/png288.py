#!/usr/bin/python3
# png288.py
# use: $ python3 png288.py <imagefile>

# $ python3 -m pip install --upgrade Pillow
# $ pip install numpy

import numpy
import sys 
import os 
from PIL import Image, ImageDraw

img, b2, b3 = None, None, None 

###
raw = True
###

def FilterPlane(p):
    a = []
    wid = img.size[0]
    hei = img.size[1]
    for ii in range(0, hei):
        for jj in range(0, wid):
            data = img.getpixel((jj,ii))
            if(data & p):
                a.append(1)
            else:
                a.append(0)
    return a
totalbytes = 0
rlebytes = 0
def RLEEncode(arr):
    s = 0
    rlearr = []
    while s < len(arr):
        if(arr[s] == 0x80):
            rlearr.append(0x80)
            rlearr.append(0x80)
            rlearr.append(0x1)
        else:
            if(s < len(arr)-2):
                # Check the next 2 bytes. Is it the same?
                if (arr[s] == arr[s + 1]):
                    if(arr[s] == arr[s + 2]):
                        rlearr.append(0x80)
                        rlearr.append(arr[s])
                        # how many?
                        ct = 3
                        while ( ((s+ct) < len(arr)-1) & (arr[s+ct] == arr[s]) ):
                            ct += 1
                        rlearr.append(ct)
                        s += (ct-1)
                # If not, append normally
                else:
                    rlearr.append(arr[s])
                    rlearr.append(arr[s+1])
                    rlearr.append(arr[s+2])
                    s += 2
            else: # finish word
                rlearr.append(arr[s])
        s += 1
    global totalbytes 
    global rlebytes 
    totalbytes += len(arr)
    rlebytes += len(rlearr)
    return rlearr 

def OutBytes(a,fn, rle):
    # a is array from FilterPlane
    global obstr
    iarr = []
    wid = b3.size[0]
    hei = b3.size[1]
    for ii in range(0, hei):
        for jj in range(0, wid, 8):
            bistr = ''
            n = 0
            while n < 8:
                if(a[(ii*wid)+jj+n] == 1):
                    bistr = bistr + '1'
                else: 
                    bistr = bistr + '0'
                n += 1
            iarr.append(int(bistr,2))
    if rle == 1:
        iarr = RLEEncode(iarr)
        obstr += "// RLE encoded. 0xFE 0xNN - copy the next NN bytes through\n//     0xFD 0xNN 0xJJ - copy NN JJ times\n"
    if(raw==False):
        obstr = obstr + "// rgb image header\nconst unsigned char " + fn.split('.')[0] + '[' + str(len(iarr)) + '] = { '
        for s in range(0,len(iarr)):
            if s % 16 == 0:
                obstr += '\n'
            obstr += str(iarr[s])
            obstr += ','
        obstr += ' \n};\n'
    else:
        for s in range(0, len(iarr)):
            obstr.append(iarr[s])

    #print(len(iarr), "rle = " + str(rle))
    
obstr = ''                
img = Image.open(sys.argv[1])
b2 = numpy.asarray(img)
b3 = Image.new('L', (b2.shape[1], b2.shape[0]), 0)
o = []

fn = os.path.splitext(sys.argv[1])[0]
fn2 = fn.split('/')
fn2 = fn2[len(fn2)-1]

b = FilterPlane(0b001)
g = FilterPlane(0b100)
r = FilterPlane(0b010)
rle = 0
i = 0
while i < len(sys.argv):
    a = sys.argv[i]
    if a.find("rle") != -1:
        rle = 1
    i += 1
obstr = []
OutBytes(b, fn2 + '_b.h', rle)
OutBytes(r, fn2 + '_r.h', rle)
OutBytes(g, fn2 + '_g.h', rle)

if(raw == False):
    f = open(fn + '.h', 'w')
    f.write(obstr)
    f.close()
else:
    f = open(fn + '_b.b', 'wb')
    i = 0
    while i < len(obstr)/3:
        f.write(bytes([obstr[i]]))
        i += 1
    f = open(fn + '_r.b', 'wb')
    while i < (len(obstr)/3) * 2:
        f.write(bytes([obstr[i]]))
        i += 1
    f = open(fn + '_g.b', 'wb')
    while i < len(obstr):
        f.write(bytes([obstr[i]]))
        i += 1
print(totalbytes, rlebytes)

def OutColorPlane(c,fn):
    wid = b3.size[0]
    hei = b3.size[1]
    for ii in range(0, hei):
        for jj in range(0, wid):
            if(c[(ii*wid)+jj] == 1):
                b3.putpixel((jj,ii), (255))
            else:
                b3.putpixel((jj,ii), (0))
    b3.save(fn)
#OutColorPlane(b, sys.argv[1].split('.')[0] + '_b.png')
#OutColorPlane(g, sys.argv[1].split('.')[0] + '_g.png')
#OutColorPlane(r, sys.argv[1].split('.')[0] + '_r.png')

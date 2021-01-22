# png288.py
# use: $ python3 png288.py <imagefile>

# $ python3 -m pip install --upgrade Pillow
# $ pip install numpy

import numpy
import sys 
from PIL import Image, ImageDraw

img, b2, b3 = None, None, None 

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

def OutBytes(a,fn):
    # a is array from FilterPlane
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
            iarr.append(str(int(bistr,2)))
    f = open(fn, 'w')
    f.write("// rgb image header\nconst unsigned char " + fn.split('.')[0] + '[' + str(len(iarr)) + '] = { ')
    for s in range(0,len(iarr)):
        f.write(iarr[s])
        f.write(',')
    f.write(' \n};\n')
    f.close()
    print(len(iarr))
                

img = Image.open(sys.argv[1])
b2 = numpy.asarray(img)
b3 = Image.new('L', (b2.shape[1], b2.shape[0]), 0)
o = []

b = FilterPlane(0b001)
g = FilterPlane(0b100)
r = FilterPlane(0b010)
OutBytes(b, 'img_b.h')
OutBytes(r, 'img_r.h')
OutBytes(g, 'img_g.h')



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

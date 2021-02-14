# png2zemi.py
# use: $ python3 png2zemi.py <imagefile>

# $ python3 -m pip install --upgrade Pillow
# $ pip install numpy

import numpy
import sys 
from PIL import Image, ImageDraw

img, b2, b3 = None, None, None 


img = Image.open(sys.argv[1])
b2 = numpy.asarray(img)
b3 = Image.new('L', (b2.shape[1], b2.shape[0]), 0)
o = []

def invert(r):
    i = 0
    while i < len(r):
        r[i] = hex(0xff ^ int(r[i],16))
        i += 1
    return r

a = []
wid = img.size[0]
hei = img.size[1]
for yt in range(0, hei, 5):
    row = []
    for xt in range(0, wid, 2):
        ob = ''
        c = 0
        for x in range(xt, xt+2):
            for y in range(yt, yt+4):
                data = img.getpixel((x,y))
                if(data != 0):
                    c = data
                    ob = '1' + ob
                else:
                    ob = '0' + ob 
            
        row.append(hex(int(ob,2)))
    if(img.getpixel((0, yt+4)) != 0):
        row = invert(row)
        a.append(row)
        a.append(hex((c << 5) | (1 << 3) | (1 << 4)))    
        a.append(255)
    else:
        a.append(row)
        a.append(hex((c << 5) | (1 << 3) | (1 << 4)))
        a.append(0)
print(a)
                    

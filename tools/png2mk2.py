#!/usr/bin/python3
# png2mk2.py

import sys, os
from PIL import Image

# load in image pixels
def LoadImg(inf):
	f = Image.open(inf)
	px = f.load()
	if (type(px[0,0]) == tuple):
	    print('not indexed image. quitting...')
	    f.close()
	    sys.exit()
	# save img size
	imsize = f.size
	f.close()
	return imsize,px

# init output
def InitImgOutput(inf, imsize, px):
	bn = os.path.basename(inf)
	bn = bn.split('.')[0]
	ob = [] # out byte array
	iy = 0
	while iy < imsize[1]:
		ix = 0
		while ix < imsize[0]:
			pind = px[ix,iy]
			if(pind > 7):
				print('more than 8 colors. quitting...')
				sys.exit()
			ob.append(px[ix,iy]<<4|px[ix+1,iy])
			ix += 2
		iy += 1
	return bn, ob

# rle encode
# convert to touple
def ToRLETuple(ob):
	oba=[]
	i = 0
	while i < len(ob)-1:
		j = 1
		if(ob[i] == ob[i+j]):
			while (ob[i+j] == ob[i+j-1]) and (i+j < len(ob)-1):
				j += 1
		oba.append((ob[i], j))
		i += j
	return oba

# 0xfe 0xnn - copy next n bytes thru
# 0xfd 0xaa 0xbb - copy 0xaa 0xbb times
# 0xfc 0xnn - copy next byte raw (non-opcode byte 0xfc-0xfe)
#print(oba)
def RLEEncode(oba):
	obb=[]
	i = 0
	mod = False
	while i < len(oba):
		j = 1
		s = 0
		if((oba[i][0] >= 0xfc) and (oba[i][0] <= 0xfe)): #0xfc
			print("AGH!")
			sys.exit()
		if(oba[i][1] > 1): # 0xfd
			mod = False
			obb.append(0xfd)
			obb.append(oba[i][0])
			obb.append(oba[i][1])
		else: #0xfe 
			if(mod==False):
				mod = True
				obb.append(0xfe)
				j = 0
				while (oba[i+j][1] == 1):
					j += 1
				obb.append(j)
			obb.append(oba[i][0])
		i += 1
	#print(len(obb))
	return obb

def WriteCHeader(bn, obb):
	outcstr = 'const unsigned char ' + bn + '[' + str(len(obb)) + '] = { \n\t'
	i = 0
	while i < len(obb):
		outcstr += str(obb[i]) + ','
		i += 1
		if(i%16==0):
			outcstr += '\n\t'
	outcstr += '};\n'
	#f = open(bn + '.h', 'w')
	#f.write(outcstr)
	#f.close()
	print(outcstr)

def WriteBytes(bn, obb):
	out=[]
	i = 0
	while i < len(obb):
		out.append(obb[i]) # outcstr += str(obb[i]) + ','
		i += 1
	print(out)


MAKERLE = 0

''' Script '''
if(MAKERLE==0):
	imgsize, pix = LoadImg(sys.argv[1])
	basen, uncompr = InitImgOutput(sys.argv[1], imgsize, pix)	
	WriteBytes(basen, uncompr)
	print("\n" + str(len(uncompr)) + "bytes written.")
	#WriteCHeader(basen, uncompr)
	#print(basen + '.h written.')
else:
	imgsize, pix = LoadImg(sys.argv[1])
	basen, uncompr = InitImgOutput(sys.argv[1], imgsize, pix)	
	rles = ToRLETuple(uncompr)
	out = RLEEncode(rles)
	WriteCHeader(basen, uncompr)
	print(imgsize[0]*imgsize[1], 'pixels to',len(uncompr), 'bytes compressed to', len(out))
#8px to 3by
# 11122233 | 34445556 | 66777888
#2px to 1by (8>4)
# 11112222

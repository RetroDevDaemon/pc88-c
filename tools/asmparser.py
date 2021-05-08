#!/usr/bin/python3
# asmparser.py

# Changs the following conventions:
# DW and DB
# to .dw and .db
# DS %d and DS %d,%d
# to .ds %d

# %s: EQU
# to %s .equ 

# 0FFFFH and FFFFH
# to #0xFFFF
# 0FFH and FFH
# to #0xFF

# 11111111B
# to #0b11111111

# ,%d 
# to ,#%d 

# ,%s (s > 2)
# to ,#%s

# (IX+%d)
# to %d(IX)

import re, sys 

# read in file
f = open(sys.argv[1], 'r', encoding='shift_jis')
intxt=['!']
while(intxt[len(intxt)-1] != ''):
	intxt.append(f.readline())
f.close()

# write blank
f = open(sys.argv[1] + '.out', 'w')
f.write('')
f.close()

# parse line by line
i = 0 
while i < len(intxt):
	y = intxt[i]

	x = re.search("DW", y)
	if (x != None):
		y = re.sub("DW", ".dw", y)
	x = re.search("DB", y)
	if (x != None):
		y = re.sub("DB", ".db", y)
	x = re.search("DS", y)
	if (x != None):
		y = re.sub("DS", ".ds", y)
	x = re.search("[A-Z]+:.*EQU", y)
	if (x!= None):
		ya = y.split(':')
		y = ya[0] + ya[1]
		ya = y.split('EQU')
		y = ya[0] + '.equ' + ya[1]
	x = re.search("[(]IX[+][0-9]+[)]", y)
	if (x!=None):
		ya = y.split(',(')
		if(len(ya) > 1):
			yb = ya[1].split('+')
			yc = yb[1].split(')')
			y = ya[0] + ',' + yc[0] + '(' + yb[0] + ')'
	x = re.search("0[A-F0-9]{4}H", y)
	if(x != None):
		ya = y[:x.start(0)]
		yb = y[x.start(0):x.end(0)-1]
		yc = y[x.end(0):]
		yi = '#' + hex(int(yb,16))
		y = ya + yi + yc
	x = re.search("[ABCDE],[0-9]+H", y)
	if(x!=None):
		ya = y[:x.start(0)+2]
		yb = y[x.start(0)+2:x.end(0)-1]
		yc = y[x.end(0):]
		yi = '#' + hex(int(yb,16))
		y = ya + yi + yc
	x = re.search("[ABCDE],[1-9](?![0-9])(?![(])(?![A-G])", y)
	if(x!=None):
		#print(y,x)
		ya = y[:x.start(0)+2]
		yb = y[x.start(0)+2:x.end(0)]
		yc = y[x.end(0):]
		yb = '#' + str(int(yb))
		y = ya + yb + yc
#	x = re.search(",[AF0-9][A-F0-9]+H", y)
#	if (x!=None):
#		yi = y[x.start(0)+1:x.end(0)-1]
#		yi = '#' + hex(int(yi,16))
#		y = y[x.start(0):] + ',' + yi + y[x.end(0):]
#		print(y)
	intxt[i] = y
	i += 1

# write to new file
f = open(sys.argv[1] + '.out', 'a', encoding='shift_jis')
i = 0
while i < len(intxt):
	f.write(intxt[i])
	i += 1
f.close()

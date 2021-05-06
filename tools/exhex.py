#!/usr/bin/python3
#exhex.py
# requires: $ python3 exhex.py [.d88 file] 
#ref: D88STRUC.TXT

import sys, math

f = open(sys.argv[1],'rb')
inby = f.read()
f.close()

BASICFILE = 0x80
BINFILE = 0x01
RAWFILE = 0x00
f_ind = 0

class dheader():
	def __init__(self, dn='                ',wp=0,med=0,ds=0,tl=[]):
		self.diskname = dn
		self.wrpr = wp # or 16 for write-protected
		self.media = med # or 0x10 (2DD), 0x20 (2HD)
		self.disksize = ds
		self.tracklist = tl # 164 entries

class dbfile():
	def __init__(self, p=0, n='\xff\xff\xff\xff\xff\xff\xff\xff\xff', 
				 t=0, a=0x000000,i=0):
		self.ptr = p
		self.name = n
		self.type = t
		self.start_addr = a
		self.fsz = 0
		self.index = i

def ReadD88Header(inby):
	k = 0
	dn = ''
	while k < 16:
		dn = dn + chr(inby[k])
		k += 1
	wp = inby[0x1a]
	med = inby[0x1b]
	ds = inby[0x1c]|(inby[0x1d]<<8)|(inby[0x1e]<<16)|(inby[0x1f]<<24)
	i = 0x20
	tl = []
	while i < ((164*4) + 0x20):
		tl.append(inby[i]|(inby[i+1]<<8)|(inby[i+2]<<16)|(inby[i+3]<<24))
		i += 4
	return dheader(dn, wp, med, ds, tl)

def GetSizes(db, hdr):
	i = 0
	while i < len(db)-1:
		db[i].fsz = db[i+1].ptr - db[i].ptr
		r = db[i].ptr % 2
		trg = int(db[i].ptr/2)
		db[i].start_addr = (hdr.tracklist[trg] + 0x880*r)
		i += 1
	i = 0
	while i < len(db):
		if(db[i].type == 0x80):
			p = 0
			s = 0
			while p < 0xffff:
				if (inby[db[i].start_addr + 16 + p] == 0) and (inby[db[i].start_addr + 16 + p+1] == 0) and (inby[db[i].start_addr + 16 + p+2] == 0):
						break
				p += 1
				s += 1
				if (s == 256):
					s = 1
					p += 16
			db[i].fsz = p + 3
		elif(db[i].type == 1):
			st = inby[db[i].start_addr+16]
			st |= inby[db[i].start_addr+1+16] << 8
			en = inby[db[i].start_addr+2+16]
			en |= inby[db[i].start_addr+3+16] << 8
			db[i].fsz = (en - st) + 3
		elif(db[i].type == 0xff):
			db[i].fsz = -1
		i += 1

	return

def GetFType(g):
	if(g == 0x80):
		return 'BAS'
	elif(g == 0x00):
		return 'RAW'
	elif(g == 0x01):
		return 'BIN'

def SortFiles(db):
	arranged = []
	while(len(db) > 0):
		i = 0
		smallest = 0xffff
		next = 0
		while i < len(db):
			if(db[i].ptr < smallest):
				smallest = db[i].ptr
				next = i
			i += 1
		arranged.append(db[next])
		db.pop(next)
	return arranged

# Get header
diskhead = ReadD88Header(inby)

# Populate files
fileindex=0x277c0
file_database=[]
s = 0
i = fileindex
while i < 0x27bef:
	fn = ''
	k = 0
	while k < 9:
		fn += chr(inby[i+k])
		k += 1
	if(inby[i] != 0x12):
		file_database.append(dbfile(inby[i+10], fn, inby[i+9], 0,f_ind)) 
		f_ind += 1
	i += 16

# Sort
file_database = SortFiles(file_database)

# Populate sizes
GetSizes(file_database, diskhead)

# print files
print("No.\tName\t\tType\tLoc\tDisk Addr\tSize")
i = 0
while i < len(file_database):
	if file_database[i].name != "\xff\xff\xff\xff\xff\xff\xff\xff\xff": #"ÿÿÿÿÿÿÿÿÿ":
		print('#' + str(file_database[i].index) + '\t',file_database[i].name,end='\t')
		ft = file_database[i].type
		print(GetFType(ft), end='\t')
		r = file_database[i].ptr % 2
		trg = int(file_database[i].ptr/2)
		if(ft == 0):
			print(str(trg) + '-' + str(r) + '\t@' + hex(file_database[i].start_addr), 
				'\t(' + str(file_database[i].fsz) + ' blk)')
		else:
			print(str(trg) + '-' + str(r) + '\t@' + hex(file_database[i].start_addr), 
				'\t' + str(file_database[i].fsz) + ' bytes')
	i += 1
# Get input
exnum = input("Extract which file? (0-63, or 'a' for 'all'): ")
try:
	if(exnum != 'a'):
		exnum = int(exnum)
except:
	print("Invalid. Type a number or the letter 'a'.")
	sys.exit()

# Get selected index
i = 0
sel = -1
if(exnum != 'a'):
	while i < len(file_database):
		if(file_database[i].index == exnum):
			sel = i
			i = 99
		i += 1

# Initiate file writing loop
t = 0
fc = 0
if (sel != -1):
	t = sel
else: 
	t = 0
	sel = len(file_database)-1
while t <= sel:
	# extract and write file, skipping disk headers
	if(file_database[t].fsz > 0):
		#try:
			f = open(file_database[t].name + '.' + GetFType(file_database[t].type),'wb')
			if(file_database[t].type == 0):
				print("Estimated file size for file " + file_database[t].name + ":", file_database[t].fsz * (2*1024))
				nfz = input("Enter new file size, or press return for (" + 
					str(file_database[t].fsz * 2 * 1024) + "): ")
				if nfz == '':
					nfz = file_database[t].fsz * 2048
				file_database[t].fsz = nfz
			s = 0
			i = file_database[t].start_addr
			ofs = 16
			while i < (file_database[t].start_addr + (file_database[t].fsz) ):
				f.write(bytes([inby[i+ ofs]]))
				i += 1
				s += 1
				if(s == 256):
					ofs += 16
					s = 0
			fc += 1
		#except:
		#	print("Writing " + file_database[t].name + " failed...")
	t += 1
	f.close()
print(str(fc) + " file(s) written successfully!")

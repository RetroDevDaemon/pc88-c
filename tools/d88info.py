import sys
f = open(sys.argv[1], 'rb')
inby = f.read()
f.close()

class d88sector():
	def __init__(self):
		self.c = 0
		self.h = 0
		self.r = 0
		self.n = 0
		self.sectors = 0
		self.density = 0
		self.deleted = 0
		self.fdc = 0
		self.bytesize = 0
		self.bytes = []
		pass 
	def __str__(self):
		o = 'C H R: ' + str(self.c) + ' '\
			      + str(self.h) + ' '\
			      + str(self.r) + ' '\
			      + '\n'
		o += 'Size byte (128 << n): ' + str(self.n) + '\t'
		o += 'Sectors this track: ' + str(self.sectors) + '\n'
		o += 'Density: ' + str(self.density) + '\t' 
		o += 'Delete flag: ' + str(self.deleted) + '\n' 
		o += 'FDC status: ' + str(self.deleted) + '\t'
		o += 'Total size (b): ' + str(self.bytesize) + '\n'
		return o


class d88track():
	def __init__(self):
		self.sectors = [] 
		self.sectorcount = 0
	def __str__(self):
		o = ''
		i = 0
		while i < self.sectorcount:
			o += str(i+1) + ':' + str(self.sectors[i].r) + ' '
			i += 1
		return o
	
class d88disk():
	def __init__(self, fn = -1, sz = 0x55eb0):
		if(fn != -1):
			f = open(fn, 'rb')
			self.bytes = f.read()
			f.close()
		else:
			self.bytes = []
			i = 0
			while i < sz:
				self.bytes.append(0)
				i += 1
		
		self.diskname = ''
		self.pointer = 0
		self.writeprotect = 0
		self.mediatype = 0
		self.disksize = 0
		
		self.tracktable = []
		self.tracks = []
		
		self.GetDiskInfo()
		self.GetTrackInfo()
		self.PopulateSectors()
	###
	def PopulateSectors(self):
		f = 0
		while f < len(self.tracktable):
			i = self.tracktable[f] 
			tr = d88track() # make new track
			# how  many sectors? read from first sector:
			tr.sectorcount = self.bytes[i+4] | (self.bytes[i+5] << 8) 
			sn = 0
			while sn < tr.sectorcount:
				s = d88sector() # make a sector
				s.c = self.bytes[i] 
				i += 1
				s.h = self.bytes[i] 
				i += 1
				s.r = self.bytes[i] 
				i += 1
				s.n = self.bytes[i] # 128 < s.n = byte approximation
				i += 1
				s.sectors = self.bytes[i] | (self.bytes[i+1] << 8) 
				i += 2
				s.density = self.bytes[i] 
				i += 1
				s.deleted = self.bytes[i] 
				i += 1 
				s.fdc = self.bytes[i] 
				i += 6
				s.bytesize = self.bytes[i] + (self.bytes[i+1] << 8) 
				i += 2
				j = 0
				while j < s.bytesize:
					s.bytes.append(self.bytes[i])
					i += 1
					j += 1
				tr.sectors.append(s)
				sn += 1
			self.tracks.append(tr)
			f += 1
		
	def GetDiskInfo(self):
		i = 0
		while i < 0x10:
			if(self.bytes[i] == 0):
				i = 0x10 
			else:
				self.diskname = self.diskname + chr(self.bytes[i])
			i += 1
		self.writeprotect = self.bytes[0x1a] 
		self.mediatype = self.bytes[0x1b] 
		self.disksize = self.bytes[0x1c] + (self.bytes[0x1d]<<8) + (self.bytes[0x1e]<<16)
	###
	
	def GetTrackInfo(self):
		i = 0x20
		trofs = -1
		
		while (trofs != 0):
			trofs = self.bytes[i] | (self.bytes[i+1] << 8) | (self.bytes[i+2] << 16)
			self.tracktable.append(trofs)
			#print(trofs)
			i += 4
		self.tracktable.pop() ## remove last element		
	###
	def copy(self, cop = -1):
		if(cop == -1):
			cop = d88disk(sz=self.disksize)
		i = 0
		while i < 16 and i < (len(self.diskname)):
			cop.bytes[i] = chr(self.diskname[i])
		cop.bytes[0x1a] = self.writeprotect 
		cop.bytes[0x1b] = self.mediatype
		cop.bytes[0x1c] = self.disksize & (0xff) 
		cop.bytes[0x1d] = (self.disksize & 0xff00) >> 8
		cop.bytes[0x1e] = (self.disksize & 0xff0000) >> 16 
		print('disk size to copy', self.disksize)
		# now copy track table
		i = 0x20
		tn = 0
		while tn < len(self.tracks):
			cop.bytes[i] = self.tracktable[tn] & 0xff 
			cop.bytes[i+1] = (self.tracktable[tn] & 0xff00) >> 8
			cop.bytes[i+2] = (self.tracktable[tn] & 0xff0000) >> 16
			i += 4
			tn += 1
		# skip to 2b0, write each track header, then each track bytes
		tt = 0
		#print(len(self.tracks))
		i = 0x2b0
		while tt < len(self.tracks):
			print('copying track', tt)
			ss = self.tracks[tt].sectorcount
			print('sector count: ', ss)
			si = 0
			while si < ss:
				cop.bytes[i] = self.tracks[tt].sectors[si].c
				i += 1
				cop.bytes[i] = self.tracks[tt].sectors[si].h 
				i += 1
				cop.bytes[i] = self.tracks[tt].sectors[si].r
				i += 1
				cop.bytes[i] = self.tracks[tt].sectors[si].n 
				i += 1
				cop.bytes[i] = self.tracks[tt].sectorcount & 0xff
				i += 1
				cop.bytes[i] = (self.tracks[tt].sectorcount & 0xff00) >> 8 
				i += 1
				cop.bytes[i] = self.tracks[tt].sectors[si].density
				i += 1
				cop.bytes[i] = self.tracks[tt].sectors[si].deleted
				i += 1
				cop.bytes[i] = self.tracks[tt].sectors[si].fdc
				i += 1
				i += 5
				cop.bytes[i] = self.tracks[tt].sectors[si].bytesize & 0xff
				i += 1
				cop.bytes[i] = (self.tracks[tt].sectors[si].bytesize & 0xff00) >> 8
				i += 1
				bc = 0
				while bc < self.tracks[tt].sectors[si].bytesize:
					cop.bytes[i] = self.tracks[tt].sectors[si].bytes[bc]
					i += 1
					bc += 1
				si += 1
			#print(self.tracks[tt].sectorcount)
			print(hex(i))
			tt += 1
			
		cop.GetDiskInfo()
		cop.GetTrackInfo()
		cop.PopulateSectors()
		return cop

#### LOAD DISK
MYDISK = d88disk(sys.argv[1])
print('Disk name: "' + MYDISK.diskname + '"')

##### PRINT DISK INFO
wp = MYDISK.writeprotect 
if(wp == 0):
	print("not write-protected")
else:
	print("write-protected") 
med = MYDISK.mediatype
if(med == 0):
	print("media: 2D")
elif(med == 0x10):
	print("media: 2DD")
elif(med == 0x20):
	print("media: 2HD")
### CHECK FILESIZE
disksize = MYDISK.disksize
print("disk size:",disksize)
if(disksize != len(inby)):
	print("does not match byte length!")
else:
	print("size OK.")
#### PRINT TRACK TABLE AND OFFSETS
#print(MYDISK.tracktable)
i = 0
while i < len(MYDISK.tracktable):
	print(hex(MYDISK.tracktable[i]), end='\t')
	if(i>0):
		ts = MYDISK.tracktable[i] - MYDISK.tracktable[i-1]
		print('\t track size: ', ts)
	else: 
		print('')
	i += 1
#################################################
	
## odd track out:
#print(MYDISK.tracks[2].sectors[0])
#print(MYDISK.tracks[2].sectors[0].bytes, len(MYDISK.tracks[2].sectors[0].bytes))

## replace the bad sectors:
#MYDISK.tracks[2].sectors[0] = None 
#MYDISK.tracks[2].sectors[9] = None#.pop(8) 
#MYDISK.tracks[2].sectors[18] = None#.pop(16) 

## rearrange disk:
i = 0
temptrack = d88track()
while i < 19:#
	temptrack.sectorcount = 16
	if(i != 0 and i != 9 and i != 18):
		temptrack.sectors.append(MYDISK.tracks[2].sectors[i])
	i += 1
MYDISK.tracks[2] = None 
MYDISK.tracks[2] = temptrack
#i = 0
#while i < len(MYDISK.tracks):
#	print(MYDISK.tracks[i])
#	i += 1

## change the header values: everything after tracks[2] needs to be -7472
i = 3
while i < len(MYDISK.tracktable):
	MYDISK.tracktable[i] -= 3120 
	print(hex(MYDISK.tracktable[i]))
	i += 1
# total disk size is reduced 
MYDISK.disksize -= 3120


#newdisk = d88disk(sz=0x55eb0) alternately
newdisk = MYDISK.copy()
print(newdisk.diskname)
if(len(newdisk.bytes) != newdisk.disksize):
	print('write failed')
else:
	f = open('fix.d88', 'wb')
	i = 0
	while i < len(newdisk.bytes):
		f.write(bytes([newdisk.bytes[i]]))
		i += 1
	f.close()
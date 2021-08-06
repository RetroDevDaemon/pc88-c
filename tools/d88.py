# d88
# header for python scripting with PC88-C
'''
  N88 BASIC DISK FORMAT
Files listing
d88 offset @ 0x277b0
CHR 12-01-01

x0 - x8 : 9 letter name, padded with 0x20
x9      : file status byte: typically 0x80, 0x01
xA      : data pointer; multiply by 8 to find sector
           or: [0xA] * (2048 + 128)+688 = offset into disk image 
xB - xF : generally FF
'''

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
    
class disk():
    def __init__(self, fn = -1, sz = 0x55eb0):
        if(fn != -1):
            f = open(fn, 'rb')
            self.bytes = f.read()
            f.close()
        else:
            self.bytes = []
            i = 0
            while i < 0x2b0:
                self.bytes.append(0)
                i += 1
            while i < sz:
                self.bytes.append(0xff) 
                i += 1
        
        self.diskname = ''
        self.pointer = 0
        self.writeprotect = 0
        self.mediatype = 0
        self.disksize = len(self.bytes)
        
        self.tracktable = []
        self.tracks = []
        
        if(fn != -1):
            f = open(fn, 'rb')
            self.bytes = f.read() 
            f.close()
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
                #print('DEBUG: ',s.bytesize)
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
            i += 4
        self.tracktable.pop() ## remove last element		
    ###
    def UpdateMasterBytes(self):
    	newbytes = []
    	# write file name, flags, disk size then track headers
    	i = 0
    	while i < len(self.diskname):
    		newbytes.append(chr(self.diskname[i])) 
    		i += 1
    	while i < 0x1b:
    		newbytes.append(0) 
    		i += 1
    	newbytes.append(self.mediatype) 
    	newbytes.append(self.disksize & 0xff) 
    	newbytes.append((self.disksize & 0xff00) >> 8)
    	newbytes.append((self.disksize & 0xff0000) >> 16) 
    	newbytes.append(0) #32 bit place
    	i = 0
    	ofs = 0x2b0
    	while i < len(self.tracks):
    		ofs = self.tracktable[i]
    		newbytes.append(ofs & 0xff)
    		newbytes.append((ofs & 0xff00) >> 8)
    		newbytes.append((ofs & 0xff0000) >> 16) 
    		newbytes.append(0) 
    		i += 1
    	i = len(newbytes) 
    	while i < 0x2b0:
    		newbytes.append(0)
    		i += 1
    	#then bytes!
    	# tracks[n].sectors[j].bytes[x]
    	n = 0
    	while n < len(self.tracks):
    		j = 0
    		while j < len(self.tracks[n].sectors):
    			newbytes.append(self.tracks[n].sectors[j].c)
    			newbytes.append(self.tracks[n].sectors[j].h)
    			newbytes.append(self.tracks[n].sectors[j].r)
    			newbytes.append(self.tracks[n].sectors[j].n)
    			newbytes.append(self.tracks[n].sectors[j].sectors & 0xff)
    			newbytes.append((self.tracks[n].sectors[j].sectors & 0xff00)>>8)
    			newbytes.append(self.tracks[n].sectors[j].density) # 0 = 2D, 40h = SD, 01h = HD
    			newbytes.append(self.tracks[n].sectors[j].deleted) #del/status flag
    			newbytes.append(self.tracks[n].sectors[j].fdc) #del/status flag
    			i = 0 
    			while i < 5:
    				newbytes.append(0) # reserved
    				i += 1
    			newbytes.append(self.tracks[n].sectors[j].bytesize & 0xff)
    			newbytes.append((self.tracks[n].sectors[j].bytesize & 0xff00)>>8)
    			x = 0
    			while x < len(self.tracks[n].sectors[j].bytes):
    				newbytes.append(self.tracks[n].sectors[j].bytes) 
    				x += 1
    			j += 1
    		n+=1
    	print(len(newbytes))
    	self.bytes = newbytes
    ##
    def copy(self, cop = -1):
        if(cop == -1):
            cop = disk(sz=self.disksize)
        i = 0
        while i < 16 and i < (len(self.diskname)):
            cop.bytes[i] = chr(self.diskname[i])
        cop.bytes[0x1a] = self.writeprotect 
        cop.bytes[0x1b] = self.mediatype
        cop.bytes[0x1c] = self.disksize & (0xff) 
        cop.bytes[0x1d] = (self.disksize & 0xff00) >> 8
        cop.bytes[0x1e] = (self.disksize & 0xff0000) >> 16 
        #print('disk size to copy', self.disksize)
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
            #print('copying track', tt)
            ss = self.tracks[tt].sectorcount
            #print('sector count: ', ss)
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
            tt += 1
            
        cop.GetDiskInfo()
        cop.GetTrackInfo()
        cop.PopulateSectors()
        return cop
    ##
    def Format(self, name=''):
        # write disk header
        i = 0
        #self.diskname = name 
        while i < len(self.diskname):
            self.bytes[i] = ord(self.diskname[i])#self.diskname[i] 
            i += 1
        print('Disk size:',self.disksize)
        self.bytes[0x1a] = 0#self.writeprotect
        self.bytes[0x1b] = 0#self.mediatype
        self.bytes[0x1c] = self.disksize & 0xff
        self.bytes[0x1d] = (self.disksize & 0xff00) >> 8
        self.bytes[0x1e] = (self.disksize & 0xff0000) >> 16
        # track pointers
        i = 0x20
        ofs = 0x2b0
        ct = 0
        ## By default, Format will format 256(+16)x16x80.
        # Afterwards you can pop and change the disk format  
        gap = 4352 # TODO
        while ct < 80:#i < 0x2b0:
            self.bytes[i] = (ofs & 0xff)
            self.bytes[i+1] = (ofs & 0xff00) >> 8
            self.bytes[i+2] = (ofs & 0xff0000) >> 16
            ofs += (256+16)*16
            ct += 1
            i += 4
        # sector headers: i = 0x2b0 ...
        i = 0x2b0
        #C, H, R, n, (0010), dd, del, fdc, 0, 0, 0, 0, 0, (0100)
        c = 0
        while c < 0x28:
            #print('c',c)
            h = 0
            while h < 2:
                #print('h',h)
                r = 1
                while r <= 16:
                    #print('r',r)
                    self.bytes[i] = c 
                    self.bytes[i+1] = h 
                    self.bytes[i+2] = r 
                    self.bytes[i+3] = 1 # TODO n
                    self.bytes[i+4] = 0x10 
                    self.bytes[i+5] = 0
                    self.bytes[i+6] = 0
                    self.bytes[i+7] = 0
                    self.bytes[i+8] = 0
                    self.bytes[i+9] = 0
                    self.bytes[i+0xa] = 0
                    self.bytes[i+0xb] = 0
                    self.bytes[i+0xc] = 0
                    self.bytes[i+0xd] = 0
                    self.bytes[i+0xe] = 0 
                    self.bytes[i+0xf] = 1 #TODO bytes
                    r += 1
                    i += (256+16)
                h += 1
            c += 1

    def WriteBytes(self, fn):
        f = open(fn, 'wb')
        i  =0
        while i < len(self.bytes):
            f.write(bytes([self.bytes[i]]))
            i += 1
        f.close()        
        print(fn, 'written successfully.')

    def AddFile(self, fn, c, h, r):
        tow = open(fn, 'rb') 
        outdat = bytearray(tow.read()) 
        tow.close() 
        self.bytes = bytearray(self.bytes)
        i = 0
        bc = 0 
        starttrack = (c * 2) + h 
        startsec = r 
        start = self.tracktable[starttrack] + ((startsec - 1) * (256+16)) + 16
        print("Track:",starttrack,"Sector:",startsec)
        print("Disk offset",hex(start))
        while i < len(outdat):
            self.bytes[start + bc] = outdat[i]
            i += 1
            bc += 1 
            if (i % 256 == 0):
                bc += 16 
        # this is error checking, it could be useless.
        if(self.bytes[start+bc] == 0xff):
            self.bytes[start+bc] = 0xc9
        # done!
        
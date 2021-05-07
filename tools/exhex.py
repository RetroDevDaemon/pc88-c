#!/usr/bin/python3
# exhex.py
#  $ python3 exhex.py [.d88 file] 
# Run with no argument to open GUI mode.
# ref: D88STRUC.TXT

import sys, math

BASICFILE = 0x80
BINFILE = 0x01
RAWFILE = 0x00
fileindex=0x277c0
dirlen=1072
inby=[]

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

def ReadFile(fn):
	f = open(fn,'rb')
	a = f.read()
	f.close()
	return a

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

def GetSizes(db, hdr, inby):
	i = 0
	while i < len(db):
		if(i < len(db)-1):
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
				if (inby[db[i].start_addr + 16 + p] == 0) and \
				 (inby[db[i].start_addr + 16 + p+1] == 0) and \
				 (inby[db[i].start_addr + 16 + p+2] == 0):
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


def GetFileDB(adr, inby):
	db = []
	s = 0
	i = adr
	f_ind = 0
	while i < adr+1072:
		fn = ''
		k = 0
		while k < 9:
			fn += chr(inby[i+k])
			k += 1
		if( (inby[i] != 0x12) and \
		(fn != '\xff\xff\xff\xff\xff\xff\xff\xff\xff')):
			db.append(dbfile(inby[i+10], fn, inby[i+9], 0,f_ind)) 
			f_ind += 1
		i += 16
	return db

def PrintFileList(db):
	print("No.\tName\t\tType\tLoc\tDisk Addr\tSize")
	i = 0
	while i < len(file_database):
		#"ÿÿÿÿÿÿÿÿÿ":
		if file_database[i].name != "\xff\xff\xff\xff\xff\xff\xff\xff\xff":
			print('#' + str(file_database[i].index) + '\t', \
				file_database[i].name,end='\t')
			ft = file_database[i].type
			print(GetFType(ft), end='\t')
			r = file_database[i].ptr % 2
			trg = int(file_database[i].ptr/2)
			if(ft == 0):
				print(str(trg) + '-' + str(r) + '\t@' + \
					hex(file_database[i].start_addr), '\t(' + \
					str(file_database[i].fsz) + ' blk)')
			else:
				print(str(trg) + '-' + str(r) + '\t@' + \
					hex(file_database[i].start_addr), '\t' + \
					str(file_database[i].fsz) + ' bytes')
		i += 1

def Get_FileToExtract(db):
	e = input("Extract which file? (0-63, or 'a' for 'all'): ")
	try:
		if(e != 'a'):
			e = int(e)
	except:
		print("Invalid. Type a number or the letter 'a'.")
		sys.exit()
	# Get selected index
	i = 0
	sel = -1
	if(e != 'a'):
		while i < len(db):
			if(db[i].index == e):
				sel = i
				i = 99
			i += 1
	return sel

def WriteFiles(e, db):
	t = 0
	fc = 0
	if (e != -1):
		t = e
	else: 
		t = 0
		e = len(db)-1
	while t <= e:
		# extract and write file, skipping disk headers
		if(db[t].fsz > 0):
			#try:
			f = open(db[t].name + '.' + GetFType(db[t].type),'wb')
			if(db[t].type == 0):
				print("Estimated file size for file " + db[t].name + ":", \
					db[t].fsz * (2*1024))
				nfz = input("Enter new file size, or press return for (" + \
					str(db[t].fsz * 2 * 1024) + "): ")
				if nfz == '':
					nfz = db[t].fsz * 2048
				db[t].fsz = nfz
			s = 0
			i = db[t].start_addr
			ofs = 16
			while i < (db[t].start_addr + (db[t].fsz) ):
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

## Script:
if(len(sys.argv) > 1):
	inby = ReadFile(sys.argv[1])
	# Get header
	diskhead = ReadD88Header(inby)
	# Populate files
	file_database = GetFileDB(fileindex, inby)
	# Sort
	file_database = SortFiles(file_database)
	# Populate sizes
	GetSizes(file_database, diskhead, inby)
	# print files
	PrintFileList(file_database)
	# Get input
	sel = Get_FileToExtract(file_database)
	# Initiate file writing loop
	WriteFiles(sel, file_database)
## App: 
else:
	import tkinter as tk 
	import tkinter.filedialog
	import tkinter.font
	class d88app(tk.Tk):
		def __init__(self):
			super().__init__()
			# Vars init
			self.diskbytes = []
			self.diskheader = None
			self.file_db = []
			# Frame:
			self.myFrame = tk.Frame(width=640, height=480)
			self.myFrame.pack()
			# Labels:
			self.lblF = tk.Label(self.myFrame, text="Files list:")
			self.lblF.place(x=10, y=10)
			# File list box:
			self.filelist = tk.Listbox(self.myFrame, width=40,height=20, \
				font=tk.font.Font(family='Courier'))
			self.filelist.place(x=10, y=30)
			# Scrollbar
			w = tk.Scrollbar(self.filelist)
			w.place(x=380,y=0, height=400,width=20)
			self.filelist.config(yscrollcommand=w.set)
			w.config(command=self.filelist.yview)
			# Window title / menu
			self.title("D88App")
			self.SetUpMenu()
			#	
		def SetUpMenu(self):
			self.menubar = tk.Menu(self)
			self.filemenu = tk.Menu(self.menubar, tearoff=0)
			self.filemenu.add_command(label="Open .d88 file...", \
				command=self.LoadD88Dialog)
			self.filemenu.add_command(label="Save", \
				command=None)
			self.filemenu.add_command(label="Save as...", \
				command=None)
			self.filemenu.add_command(label="-----------", \
				command=None)
			self.filemenu.add_command(label="Quit", \
				command=sys.exit)
			
			self.menubar.add_cascade(label="File", menu=self.filemenu)
			self.config(menu=self.menubar)
			#
		def LoadD88Dialog(self):
			fn = tk.filedialog.askopenfilename(parent=self, \
				title="Select .d88 file", initialdir='.',\
				filetypes=(("D88 files","*.d88"),("all files","*.*")))
			self.diskbytes = ReadFile(fn)
			print(fn + ' loaded.')
			self.diskheader = ReadD88Header(self.diskbytes)
			self.file_db = GetFileDB(0x277c0, self.diskbytes)
			self.file_db = SortFiles(self.file_db)
			GetSizes(self.file_db, self.diskheader, self.diskbytes)
			self.PopulateList(self.file_db)
			#
		def PopulateList(self, db):
			self.filelist.insert(1,'No.| Filename  | Address |Type | Size ')
			self.filelist.insert(2,'--------------------------------------')
			i = 0
			while i < len(db):
				fstr = ''
				fstr += '{:2d}'.format(db[i].index) + ' : '
				fstr += db[i].name + ' : '
				fstr += '${:06x}'.format(db[i].start_addr) + ' : '
				fstr += GetFType(db[i].type) + ' : '
				fstr += '{:5d}'.format(db[i].fsz)
				self.filelist.insert(i+3, fstr)
				i += 1

			#

	app = d88app()
	app.mainloop()

""" 
class instrument():
  def __init__(self, arr, ptr):
    if(arr[ptr]!=0xff):
      print("instrument may be off...")
    ptr+=1
    self.attack=[0,0,0,0]	#0-31
    self.decay=[0,0,0,0]	#0-31
    self.sustain=[0,0,0,0]	#0-31
    self.release=[0,0,0,0]	#0-15
    self.suslvl=[0,0,0,0]	#0-15
    self.lvl=[0,0,0,0]		#0-127
    self.ks=[0,0,0,0]		#0-3
    self.mult=[0,0,0,0]		#0-15
    self.detune=[0,0,0,0]	#0-7
    self.fbalg=[0,0]		#0-7
    i=0
    while i<4:
      self.attack[i]=int(arr[ptr])
      ptr+=1
      i+=1
    i=0
    while i<4:
      self.decay[i]=arr[ptr]
      ptr+=1
      i+=1
    i=0
    while i<4:
      self.sustain[i]=arr[ptr]
      ptr+=1
      i+=1
    i=0
    while i<4:
      self.release[i]=arr[ptr]
      ptr+=1
      i+=1
    i=0
    while i<4:
      self.suslvl[i]=arr[ptr]
      ptr+=1
      i+=1
    i=0
    while i<4:
      self.lvl[i]=arr[ptr]
      ptr+=1
      i+=1
    ptr+=1 #mystery byte
    n=[]
    i=0
    self.name=''
    while i<6:
      n.append(arr[ptr])
      ptr+=1
      i+=1
      self.name+=chr(n[i-1])
sel=0x40
 """
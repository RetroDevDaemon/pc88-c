import sys
f = open(sys.argv[1], 'rb')
aa = int(sys.argv[2], 16)
rb = f.read()
f.close()
inby = []
i = 0
while i < len(rb):
    inby.append(rb[i])
    i += 1
st_ad = aa
en_ad = aa + len(rb)
a, b, c, d = 0,0,0,0
a = st_ad & 0xff
b = (st_ad & 0xff00) >> 8
c = en_ad & 0xff
d = (en_ad & 0xff00) >> 8
inby.insert(0, d)
inby.insert(0, c)
inby.insert(0, b)
inby.insert(0, a)
f = open(sys.argv[1], 'wb')
i = 0
while i < len(rb):
    f.write(bytes([inby[i]]))
    i += 1
f.close()

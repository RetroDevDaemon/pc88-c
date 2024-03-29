#// 0x38 and 0x39 in IPL contain the jump address 
import re 
import sys

# usage:
# $ python3 ./fixboot.py out/main.map src/ipl.bin

f = open(sys.argv[1],'r') #'out/main.map', 'r')
s = []
tr = ' '
while tr != '':
    s.append(tr)
    tr = f.readline()
f.close()

i = 0
q = []
while i < len(s):
    if s[i].find('__init') != -1:
        q = re.findall(r'\w\w\w\w\w\w\w\w', s[i])
    i += 1
ll = q[0][6:]
hh = q[0][4:6]

f = open(sys.argv[2], 'rb')
ib = bytearray(f.read())
f.close()

ib[0x38] = int(ll,16)
ib[0x39] = int(hh,16) 

f = open(sys.argv[2], 'wb')
f.write(ib)
f.close()

print("IPL redirected to __init @", hh+ll)
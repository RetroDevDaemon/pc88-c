#!/usr/bin/python3 
# bytecomp.py by b.ferguson

import sys

if(len(sys.argv) != 3):
    print("Usage:\n$ python3 bytecomp.py <file1> <file2>\n")
    quit()

filea = open(sys.argv[1], 'rb')
filea_bytes = filea.read()
filea.close()

fileb = open(sys.argv[2], 'rb')
fileb_bytes = fileb.read()
fileb.close()

alen = len(filea_bytes)
blen = len(fileb_bytes)
if(alen != blen):
    print("Files are not equal size. Comparison will stop at location ", hex(alen))

i = 0 
parity = 0
inbadblock = False
while i < alen:
    if( (filea_bytes[i] == fileb_bytes[i]) ):
        if(filea_bytes[i] != 0):
            if(inbadblock):
                inbadblock = False 
                print(offbytes, '\tmismatched bytes found @', '{:04X}'.format(i-offbytes), 'until', '{:04X}'.format(i))
        parity += 1
    else:
        if(inbadblock):
            offbytes += 1
        else:
            inbadblock = True 
            offbytes = 1
    i += 1

print("\nFile parity: ", '{0:0.2f}%'.format(parity / alen * 100))
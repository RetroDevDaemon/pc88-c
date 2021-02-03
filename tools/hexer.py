# hexer
#  by @retrodevdiscord CC0
# changes a single byte in a given file.
import sys 
if(len(sys.argv) != 4):
    print("hexer.py\nUsage:\n $ python3 hexer.py <file> <location> <value>\n  Use HEX format only! (e.g. 0xff)")
    sys.exit()
f = open(sys.argv[1], 'rb')
ib = bytearray(f.read())
l = int(sys.argv[2], 16)
o = int(sys.argv[3], 16)
f.close()
ib[l] = o 
f = open(sys.argv[1], 'wb')
f.write(ib)
f.close()
print("Location " + sys.argv[2] + " in file " + sys.argv[1] + " changed to value: " + sys.argv[3])
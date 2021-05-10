# removes first four bytes
import sys 
f = open(sys.argv[1], 'rb')
by = f.read()
f.close()
i = 4
f = open(sys.argv[1] + '.strip', 'wb')
while i < len(by):
    f.write(bytes([by[i]]))
    i += 1
f.close()
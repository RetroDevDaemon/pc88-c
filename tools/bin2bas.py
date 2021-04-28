import sys 
# read in file to write
f = open(sys.argv[1], 'rb')
ib = f.read()
f.close()
# BASIC stuff
row = 1000
out = str(row) + ' DATA '
# write DATA string
pad = 0
i = 0
while i < len(ib):
    ob = '{:02X}'.format(ib[i])
    out += ob
    if((i+1)%34==0):
        row += 100
        out += '\n' + str(row) + ' DATA ' 
    i += 1

f = open('a.txt', 'w')
f.write(out)
f.close()

import sys 
# base64 string
b64='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/='
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
    # read in 3 bytes to convert to 4
    ba = ib[i+0]
    if(len(ib) - i) == 2:
        bb = ib[i+1]
        bc = 0
        pad = 1
    elif(len(ib) - i) == 1:
        bb = 0
        bc = 0
        pad = 2
    else: 
        bb = ib[i+1]
        bc = ib[i+2]
    # make the large byte
    bt = (ba << 16) | (bb << 8) | bc
    # break into 4 with padding
    ob = [ 0, 0, 0, 0]
    j = 3
    while j >= pad:
        ob[j] = (bt&(0x3f) << (6*j))>>(6*j)
        out += b64[ob[j]]
        j -= 1
    while(pad > 0):
        out += '='
        pad -= 1
    i += 3
    if(i % 51 == 0): # newline in BASIC
        row += 10
        out += "\n" + str(row) + " DATA "

f = open('a.txt', 'w')
f.write(out)
f.close()

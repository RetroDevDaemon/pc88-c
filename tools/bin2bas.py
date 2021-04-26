import sys 
# base64 string
b64='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/='
# read in file to write
f = open(sys.argv[1], 'rb')
ib = f.read()
f.close()
# basic stuff
row = 100
out = str(row) + ' DATA '
# loop!
i = 0
while i < len(ib)-2:
    # read in 3 bytes to convert to 4
    ba = ib[i+0]
    bb = ib[i+1]
    bc = ib[i+2]
    # make the large byte
    bt = (ba<<16)|(bb<<8)|bc
    # break into 4 with padding
    oba, obb, obc, obd = 0,0,0,0
    oba = (bt & (0b111111 << 18))>>18 
    obb = (bt & (0b111111 << 12))>>12 
    obc = (bt & (0b111111 << 6))>>6 
    obd = bt & (0b111111) 
    out += b64[oba]
    out += b64[obb]
    out += b64[obc]
    out += b64[obd]
    i += 3
    if(i%51==0):
        row += 10
        out += "\n" + str(row) + " DATA "
# catch and pad
left = len(ib) - i
if(left == 2):
    ba = ib[i]    
    bb = ib[i+1]
    bt = (ba << 16)|(bb<<8)
    oba,obb,obc,obd = 0,0,0,64
    oba = (bt&(0b111111<<18))>>18
    obb = (bt&(0b111111<<12))>>12
    obc = (bt&(0b111111<<6))>>6
    obd = (bt&(0b111111))
    out += b64[oba]
    out += b64[obb]
    out += b64[obc]
    out += '='
elif (left == 1):
    ba = ib[i]    
    bt = (ba << 16)
    oba,obb,obc,obd = 0,0,64,64
    oba = (bt&(0b111111<<18))>>18
    obb = (bt&(0b111111<<12))>>12
    obc = (bt&(0b111111<<6))>>6
    obd = (bt&(0b111111))
    out += b64[oba]
    out += b64[obb]
    out += '='
    out += '='

f = open('a.txt', 'w')
f.write(out)
f.close()

# maked88.py 

# by @retrodevdiscord, CC0
# usage:
# $ python3 MakeD88.py <diskname> [<filename> <C><H><R>]
#  Creates a blank 2D PC88 disk with name <diskname>, or
#  optionally adds <filename> to the disk at location CHR

import sys 
import os 

def WriteBlank2D():
    # WRITE DISK HEADER
    ###
    f = open(sys.argv[1], 'wb')
    # WRITE DISK NAME
    disk_name = "                "
    f.write( bytearray(disk_name, 'ansi') )
    # null
    f.write(bytes([0]))
    i = 0x11
    while i <= 0x19:
        f.write(bytes([0]))
        i += 1
    # write prot flag
    f.write(bytes([0]))
    # 2D
    f.write(bytes([0]))
    # ^ disk size:
    dsize = 348848
    f.write(bytes([dsize & 0xff]))
    f.write(bytes([(dsize & 0xff00)>>8]))
    f.write(bytes([(dsize & 0xff0000)>>16]))
    f.write(bytes([(dsize & 0xff000000)>>24]))
    # print sector locations
    headerloc = 688 
    i = 0
    while i < 80:
        v = headerloc + (0x1100 * i) # b0 35 00 00 
        bn = 0
        while bn < 4:
            o = v & (0xff << (bn * 8))
            o = o >> (bn * 8)
            f.write(bytes([o]))
            bn += 1
        # each head (group of sectors)
        i += 1
    # 0x150 null bytes
    i = 0x160
    while i < 0x2b0:
        f.write(bytes([0]))
        i += 1

    # WRITE SECTOR 
    c = 0
    h = 0
    r = 0
    while c < 0x28:
        h = 0
        while h < 2:
            r = 1
            while r <= 16:
                f.write(bytes([c])) # cylinder - 28 per disk
                f.write(bytes([h])) # head - 2 per disk, (0 and 1)
                f.write(bytes([r])) # record/sector id - 16 per head (0x01 to 0x10!)
                # bytes / 256 in sector (always 1)
                f.write(bytes([1]))
                # num. sectors in disk - always 10 00
                f.write(bytes([0x10]))
                f.write(bytes([0]))
                # double density 00 = true
                f.write(bytes([0]))
                # deleted flag
                f.write(bytes([0]))
                # FDC code (0 = ok)
                f.write(bytes([0]))
                # reserved null bytes
                i = 0
                while i < 5:
                    f.write(bytes([0]))
                    i += 1
                # sector data size (256)
                f.write(bytes([0]))
                f.write(bytes([1]))
                # fill with FF
                i = 0
                while i < 256:
                    f.write(bytes([0xff]))
                    i += 1
                r += 1
            h += 1
        c += 1

    f.close()

import math 

def AddFile(filename, cyl, hed, rec):
    inb = open(sys.argv[1], 'rb')
    indat = bytearray(inb.read())
    inb.close() 
    rsize = (256 + 16)
    hsize = rsize * 16
    csize = hsize * 2
    start = (0x2b0 + 16) - rsize 
    start += (rec * rsize) + (hed * hsize) + (cyl * csize)
    # iterate every 256 bytes, then skipping 16... 
    print("Embedding at location " + hex(start))
    tow = open(sys.argv[2], 'rb')
    outdat = bytearray(tow.read())
    tow.close()
    i = 0
    bc = 0
    while i < len(outdat):
        indat[start+bc] = outdat[i]
        i += 1
        bc += 1
        if (i % 256 == 0):
            bc += 16
    if(indat[start+bc] == 0xff):
        indat[start+bc] = 0xc9
    f = open(sys.argv[1], 'wb')
    f.write(indat)
    f.close()

if(len(sys.argv) == 1):
    print('MakeD88.py\n\tUsage: $ python3 maked88.py <diskname> [<filename> <C> <H> <R>]\n\
Where:\n<diskname> is the name of the disk file.\n  (Without further arguments this generates a blank 2D PC-88 disk.)\n\
<filename> File to add to target CHR location\n<C> Target cylinder (0-39)\n<H> Target head (0 or 1)\n<R> Record no (1-16)\n\
e.g. $ python3 maked88.py new.d88 ipl.bin 0 0 1\n Adds ipl.bin to the boot sector, and makes the disk if it does not exist.')
elif(len(sys.argv) > 2):
    if(len(sys.argv) != 6):
        print('Error parsing arguments. Run `python3 maked88.py` to see instructions.')
    else:
        if(os.path.exists(sys.argv[1]) == False):
            print(sys.argv[1] + ' not found. Creating.')
            WriteBlank2D()
        print('Adding file ' + sys.argv[2] + ' to ' + sys.argv[1] + '...', end=' ')
        AddFile(sys.argv[2], int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]))
else:
    print('Creating blank 2D disk ' + sys.argv[1] + '...')
    WriteBlank2D()
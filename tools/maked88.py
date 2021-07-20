# maked88.py 

# by @retrodevdiscord, CC0
# usage:
# $ python3 MakeD88.py <diskname> [<filename> <C><H><R>]
#  Creates a blank 2D PC88 disk with name <diskname>, or
#  optionally adds <filename> to the disk at location CHR

import sys 
import os 
import d88 

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
            new = d88.disk(sz=348848) 
            new.Format() 
            new.WriteBytes(sys.argv[1])
        else:
            new = d88.disk(sys.argv[1])
            new.GetTrackInfo()
        print('Adding file ' + sys.argv[2] + ' to ' + sys.argv[1] + '...', end=' ')
        new.AddFile(sys.argv[2], int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]))
        new.WriteBytes(sys.argv[1])
else:
    print('Creating blank 2D disk ' + sys.argv[1] + '...')
    new = d88.disk(sz=348848) 
    new.Format() 
    new.WriteBytes(sys.argv[1])

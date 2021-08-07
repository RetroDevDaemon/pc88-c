# maked88.py 
# by @retrodevdiscord, CC0

import sys 
import os 

import d88

if(len(sys.argv) == 1):
    print('MakeD88.py\n\
    Usage: $ python3 maked88.py <diskname> [<filename> <C> <H> <R>]\n\
                                    or     [<filename> -b <O> [<type> <L>]]\n\
                                    or     [<title>]\n\
Where:\n\
    <diskname> : is the name of the disk file.\n\
      (Without further arguments this generates a blank 2D PC-88 disk.)\n\
    <filename> : File to add to target CHR location\n\
    <C> : Target cylinder (0-39)\n\
    <H> : Target head (0 or 1)\n\
    <R> : Record no (1-16)\n\
    <O> : Block number (N88 format)\n\
    <type> : one of ASCII, BASIC, or BINARY. Default is ASCII.\n\
        * BASIC compiler supported in the future, for now use binary/ascii.\n\
    <L> : Start location in hex (only required for BINARY)\n\
    <title> : Changes name of disk when this is the only argument.\n\
\n\
e.g.\n\
     $ python3 maked88.py new.d88 ipl.bin 0 0 1\n\
    Adds ipl.bin to the boot sector, and makes the disk if it does not exist.\n\
  -OR-\n\
     $ python3 maked88.py new.d88 app.bas -b 0x48\n\
    Adds app.bas in ASCII mode to new.d88 at block 72 (=CHR 12,0,1),\n\
    and adds a directory entry (so it can be loaded via \'files\').\n\
  -OR-\n\
     $ python3 maked88.py new.d88 TestName\n\
    A string and no other arguments makes a new disk named TestName.\n\n')

elif(len(sys.argv) > 2):
    ############
    # UPDATE DISK NAME ONLY 
    ############
    if(len(sys.argv) == 3):
        print("Changing " + sys.argv[1] + "'s disk name to:\n" + sys.argv[2])
        if(os.path.exists(sys.argv[1]) == False):
            print(sys.argv[1] + ' not found. Creating.')
            new = d88.disk(sz=348848) 
            new.Format() 
            new.WriteBytes(sys.argv[1])
        f = open(sys.argv[1], 'rb')
        ib = f.read() 
        f.close()
        bytes = bytearray(ib)
        f = open(sys.argv[1], 'wb')
        i = 0 
        while ( (i < len(sys.argv[2])) and (i < 16) ):
            bytes[i] = ord(sys.argv[2][i])
            i += 1
        while i < 16:
            bytes[i] = 0x20
            i += 1
        f.write(bytes) 
        f.close() 
        sys.exit()
    ## ERROR
    if(len(sys.argv) < 5):# and len(sys.argv) != 5):
        print('Error parsing arguments. Run `python3 maked88.py` to see instructions.')
    else:
    ########################################
    #  N88 DISK BASIC FILE FORMATS 
    ########################################
        if(os.path.exists(sys.argv[1]) == False):
            print(sys.argv[1] + ' not found. Creating.')
            new = d88.disk(sz=348848) 
            new.Format() 
            new.WriteBytes(sys.argv[1])
        else:
            new = d88.disk(sys.argv[1])
        new.GetTrackInfo()
        fn = sys.argv[2]
        f = open(sys.argv[2], 'rb') 
        f.seek(0,2)
        fsize = f.tell()
        f.close()
        stloc = -1
        if(fsize > 348848):
            print("Won't fit!")
            sys.exit()
        if ( sys.argv[3] == '-b' ):
            ftype = 'ASCII'
            if(len(sys.argv) >= 6):
                if (sys.argv[5].upper() == 'BASIC'):
                    #TODO Add compiler
                    ftype = 'BASIC'
                    print("No BASIC compiler support yet... use binary.")
                    sys.exit()
                elif (sys.argv[5].upper() == 'BINARY'):
                    if(len(sys.argv) != 7):
                        print("Please designate a start address in hex format.")
                        sys.exit()
                    stloc = int(sys.argv[6],16) 
                    enloc = stloc + fsize 
                    if(enloc > 0xffff):
                        print("Won't fit!")
                        sys.exit()
                    print("Appending: ", hex(stloc), hex(enloc))
                    ftype = 'BINARY'
                elif (sys.argv[5].upper() == 'ASCII'):
                    ftype = 'ASCII'
            print("Adding file " + fn + ' in N88 mode to ' + sys.argv[1] + '...',end='')
            dirofs = 0
            if(sys.argv[4][1] == 'x'):
                dirofs = int(sys.argv[4], 16)
            else:
                dirofs = int(sys.argv[4])
            # n88 disk basic mode - 8 sector (2kb) fidelity.
            tgc = int(dirofs / 8)*2
            tgh = dirofs % 2
            tgr = 1
            print(dirofs, tgc, tgh, tgr)
            bnew = d88.disk(sys.argv[1])
            atf = False
            if(ftype=='ASCII'):
                atf = True
            a = bnew.AddFile(fn, tgc, tgh, tgr, respectBAM=True, ascii=atf, loadaddr=stloc, endaddr=stloc+fsize)
            if(a == -1):
                print("Creation failed.")
                sys.exit()
            bn = os.path.basename(fn)
            if(ftype == 'ASCII'):
                #ensure line endings are 0D 0A
                f = open(sys.argv[2], 'rb')
                b = f.read(256)
                f.close()
                i = 0
                ok = False 
                while i < 256:
                    if(b[i] == 0x0d):
                        ok = True
                    i += 1
                if(not ok):
                    print("Input failed! Ensure input is CRLF format.")
                    sys.exit()
            a = bnew.AddDirEntry(bn, dirofs, fsize, filetype=ftype)
            if(a == -1):
                print("Creation failed.")
                sys.exit()
            a = bnew.WriteBytes(sys.argv[1])
            if(a == -1):
                print("Creation failed.")
                sys.exit()
        else: 
        ##################################
        #  NORMAL C-H-R BASED FILE ADD
        ##################################
            print('Adding file ' + fn + ' to ' + sys.argv[1] + '...', end=' ')
            new.AddFile(fn, int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]))
            new.WriteBytes(sys.argv[1])
else:
    ###############################
    # JUST MAKE A BLANK DISK
    ###############################
    print('Creating blank 2D disk ' + sys.argv[1] + '...')
    new = d88.disk(sz=348848) 
    new.Format() 
    new.WriteBytes(sys.argv[1])

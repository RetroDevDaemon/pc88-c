# c2bin.py
# input: file with one or more const char arrays in C format
# output: binary 

import sys

f = open(sys.argv[1], 'r')
instr = f.read()
f.close()

c = instr.count('{')
d = instr.count('}')
if c != d:
    print('Invalid input file. Double check brackets?')
    sys.exit()

groups = []

while c > 0:
    i = instr.find('{')
    f = instr.find('}')
    groups.append(instr[i+1:f])
    instr = instr[f+1:]    
    c -= 1

og = []

for g in groups:
    inst = g.split(',')
    for ch in inst:
        ch.replace('\n','')
        ch.replace(' ','')
        ch.replace('\t','')
    og.append(inst)

f = open(sys.argv[2], 'wb')
for g in og:
    for ch in g:
        f.write(bytes([int(ch)]))
f.close()

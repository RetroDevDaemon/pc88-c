#!bin/python3 

pc88kana = [ #0xa0
    " ","｡","｢","｣","､","･","ｦ","ｧ","ｨ","ｩ","ｪ","ｫ","ｬ","ｭ","ｮ","ｯ",
    "ｰ","ｱ","ｲ","ｳ","ｴ","ｵ","ｶ","ｷ","ｸ","ｹ","ｺ","ｻ","ｼ","ｽ","ｾ","ｿ",
    "ﾀ","ﾁ","ﾂ","ﾃ","ﾄ","ﾅ","ﾆ","ﾇ","ﾈ","ﾉ","ﾊ","ﾋ","ﾌ","ﾍ","ﾎ","ﾏ",
    "ﾐ","ﾑ","ﾒ","ﾓ","ﾔ","ﾕ","ﾖ","ﾗ","ﾘ","ﾙ","ﾚ","ﾛ","ﾜ","ﾝ","\"","゜",
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, "円","年","月","日","時","分","秒"
]

import sys

f = open(sys.argv[1], 'rb')
inby = f.read()
f.close()


print(pc88kana[0xbe - 0xa0])
print(hex(inby[0x9373]))
print(pc88kana[inby[0x9373] - 0xa0])
print(pc88kana[inby[0x9374] - 0xa0])
print(pc88kana[inby[0x9375] - 0xa0])
print(pc88kana[inby[0x9376] - 0xa0])

i = 0
block = ''
while i < len(inby):
    if (inby[i] > 0xa0):
        if(inby[i] < 0xe0):
            block += str(pc88kana[inby[i] - 0xa0])
    if(i % 16 == 0): 
        if(len(block) > 0):
            print(hex(i) + ":  ", end='')
            print(block)
            block = ''
    i += 1

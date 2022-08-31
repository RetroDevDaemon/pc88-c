/*
3fec0 - ff
3fec1 - 1
3fec2 - 6

3fec3-3feca
Player name

3fef1 - 3fefa:
Magic spell count

3fefb,c,d,e,f:
Items 1-5

3fee8-3feec
Helms

3feed - 3f3f0
Shields 

3fed9,a
Gold in BCD, high first x 100
*/

[0] = 0xff
[1] = 1
[2] = 6
[3-0xa] = player name 
[0xb] = 0
[0xc] = 0x19
[0x19-0x1a] = gold in BCD, high first
[0x1b-0x23] = weapons 
[0x28-0x2c] = helms 
[0x2d-0x30] = shields 
[0x31-0x3a] = magic spells 
[0x3b-0x40] = items 1-6
[0x41-0x51] = special items 1-17

[0x54] = HP 0-255
[0x55] = ??
[0x56] = exp 0-255
[0x57] = power 0 - 255

[0xeb] = map item count
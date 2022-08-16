// main.c 

typedef volatile unsigned char vu8;
typedef unsigned char u8;

const char hw_str[] = "Hello World!\x00";

void main()
{
	vu8* p = (vu8*)0xf3c8;
	
	for(u8 i = 0; i < sizeof(hw_str); i++)
		*p++ = hw_str[i];
	
	while(1){};
}

void __init()
{
	main();
}

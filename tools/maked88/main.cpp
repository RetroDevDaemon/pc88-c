// maked88/main.c 
#include "headers.h"

int main(int argc, char* argv[])
{
    // Handle args
    if(argc < 2)
    {
        printf("Please add a filename.\n"
        "e.g. \'maked88 <diskname.d88>\'\n");
        return 1;
    }
    char* fileToOpen = argv[1];

    // Make a new empty disk
    //d88disk d = d88disk(DISK_DEFAULT_SIZE);
    
    // Open the given disk name
    d88disk d = d88disk(fileToOpen);
    
    //testing: print it
    for(int i = 0; i < d.disksize; i++) printf("%c", d.bytes[i]);
    
    return 0;
}
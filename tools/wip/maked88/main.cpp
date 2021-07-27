// maked88/main.c 
#include "headers.h"


int main(int argc, char* argv[])
{
    d88disk disk_a;
    d88disk disk_b;

    // Handle args
    if(argc < 2)
    {
        printf("Please add a filename.\n"
        "e.g. \'maked88 <diskname.d88>\'\n");
        return 1;
    }
    char* fileToOpen = argv[1];

    // Make a new empty disk
    //disk_a = d88disk(DISK_DEFAULT_SIZE);
    
    // Open the given disk name
    //disk_b = d88disk();
    //disk_a = 0;//(void*)NULL;
    disk_a = d88disk(fileToOpen);
    disk_b = 0;
    
    //testing: print it
    //printf("%d", open_disks[0].disksize);
    for(int i = 0; i < disk_a.disksize; i++) printf("%d", disk_a.bytes[i]);
    
    return 0;
}
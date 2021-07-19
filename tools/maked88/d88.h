#include "headers.h"
//#include "stdio.h" // 

#define DISK_DEFAULT_SIZE 348848

class d88disk 
{
    public:
        int disksize;
        std::vector<char> bytes;
    private:
        FILE* diskfile;
        int fno;
    public:
        d88disk();
        d88disk(int _disksize);
        d88disk(char* filename);
     
};

d88disk::d88disk()
    : disksize(DISK_DEFAULT_SIZE), 
      bytes(std::vector<char>(disksize))
{
    for(int i = 0; i < disksize; i++) bytes[i] = 0;
}

d88disk::d88disk(int _disksize)
    : disksize(_disksize)
    , bytes(std::vector<char>(disksize))
{
    for(int i = 0; i < disksize; i++) bytes[i] = 0;
}

d88disk::d88disk(char* filename)
{
    // Open given file 
    diskfile = fopen(filename, "rb");
    // Seek to end, get fsize
    fno = fileno(diskfile);
    fseek(diskfile, 0L, SEEK_END);
    disksize = ftell(diskfile);
    // Allocate byte array
    bytes = std::vector<char>(disksize);
    // read and close
    fseek(diskfile, 0L, SEEK_SET);
    //FILE* f = fdopen(diskfile, "rb");
    //read(fno, &bytes[0], disksize);
    for(int i = 0; i < disksize; i++)
    {
        char c = '?';
        printf("%d", read(fno, &c, 1));
        fseeko(diskfile, i, SEEK_SET);
    }
    fclose(diskfile);
}
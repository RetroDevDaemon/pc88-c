#include "headers.h"
//#include "stdio.h" // 

#define DISK_DEFAULT_SIZE 348848
#define DEFAULT_NUM_TRACKS 80
#define DEFAULT_NUM_SECTORS 16

typedef unsigned char u8;

class d88sector 
{
    public:
         u8 c; // cylinder (2 tracks)
         u8 h; // head - front or back?
         u8 r; // record or sector, starts at 1 (so minus 1 for index 0)
         u8 n; // = 0
         int sectors; //= 0
         u8 density; //= 0
         u8 deleted;// = 0
         u8 fdc;// = 0
         int bytesize;// = 0
         std::vector<char> bytes;
    // remember: 
    // starttrack = (c * 2) + h 
    // startsec = r 
};

class d88track
{
    public: 
        int numSectors;
        std::vector<d88sector> sectors; 

    public: 
        d88track();
        d88track(int numSectors);
};

// default number of sectors 
d88track::d88track() 
    : numSectors(DEFAULT_NUM_SECTORS), 
      sectors(std::vector<d88sector>(DEFAULT_NUM_SECTORS))
{};

// specific num of sectors 
d88track::d88track(int _numsec)
    : numSectors(_numsec),
      sectors(std::vector<d88sector>(_numsec))
{};

class d88disk 
{
    public:
        // define in constructor:
        int disksize;
        std::vector<char> bytes;
        // Allocate  for these:
         std::vector<d88track> tracks;
         char diskname[16];
         u8 writeprotect;
         u8 mediatype;

    private:
        FILE* diskfile;

    public:
        d88disk();
        d88disk(int _disksize);
        d88disk(char* filename);
     
};

// Default constructor, makes all null bytes
d88disk::d88disk()
    : disksize(DISK_DEFAULT_SIZE), 
      bytes(std::vector<char>(disksize))
{
    for(int i = 0; i < disksize; i++) bytes[i] = 0;
}

// makes disk of specific size 
d88disk::d88disk(int _disksize)
    : disksize(_disksize)
    , bytes(std::vector<char>(disksize))
{
    for(int i = 0; i < disksize; i++) bytes[i] = 0;
}

// loads in disk from file 
d88disk::d88disk(char* filename)
{
    std::vector<d88track> tracks = std::vector<d88track>(80);
    //writeprotect = 0;
    //tracks[0] = NULL;
    diskfile = fopen(filename, "rb");
    // get file size
    fseek(diskfile, 0L, SEEK_END);
    disksize = ftell(diskfile);
    // allocate bytes
    bytes = std::vector<char>(disksize);
    fclose(diskfile);

    int fd;
    if ((fd = open(filename, O_RDWR)) >= 0)
    {
        char c;
        int i;
        while (read(fd, &c, 1) == 1){
            bytes[i++] = c;     // read in all bytes 
        }
    }
    close(fd);

}
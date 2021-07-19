#include "headers.h"
//#include "stdio.h" // 

#define DISK_DEFAULT_SIZE 348848
#define DEFAULT_NUM_TRACKS 80
#define DEFAULT_NUM_SECTORS 16

typedef unsigned char u8;

class d88sector 
{
    public:
        static u8 c;// = 0
        static u8 h;// = 0
        static u8 r;// = 0
        static u8 n;// = 0
        static int sectors; //= 0
        static u8 density; //= 0
        static u8 deleted;// = 0
        static u8 fdc;// = 0
        int bytesize;// = 0
        std::vector<char> bytes;
    public: 
        d88sector();
        ~d88sector();
};

d88sector::d88sector()
    : bytesize(256)
    , bytes(std::vector<char>(bytesize))
{
};
d88sector::~d88sector(){};

class d88track
{
    public: 
        int numSectors;
        std::vector<d88sector> sectors; 
    public: 
        d88track();
        d88track(int numSectors);
        ~d88track();
};

d88track::d88track() 
    : numSectors(DEFAULT_NUM_SECTORS), 
      sectors(std::vector<d88sector>(DEFAULT_NUM_SECTORS))
{};
d88track::d88track(int _numsec)
    : numSectors(_numsec),
      sectors(std::vector<d88sector>(_numsec))
{};
d88track::~d88track(){};

class d88disk 
{
    public:
        // define in constructor:
        int disksize;
        std::vector<char> bytes;
        // Allocate static for these:
        static std::vector<d88track> tracks;
        static char diskname[16];
        static u8 writeprotect;
        static u8 mediatype;
    private:
        FILE* diskfile;
    public:
        d88disk();
        d88disk(int _disksize);
        d88disk(char* filename);
        ~d88disk();
     
};

d88disk::d88disk()
    : disksize(DISK_DEFAULT_SIZE), 
      bytes(std::vector<char>(disksize))
{
    std::vector<d88track> tracks = std::vector<d88track>(80);
    for(int i = 0; i < disksize; i++) bytes[i] = 0;
}

d88disk::d88disk(int _disksize)
    : disksize(_disksize)
    , bytes(std::vector<char>(disksize))
{
    std::vector<d88track> tracks = std::vector<d88track>(80);
    for(int i = 0; i < disksize; i++) bytes[i] = 0;
}

d88disk::d88disk(char* filename)
{
    std::vector<d88track> tracks = std::vector<d88track>(80);
    //writeprotect = 0;
    //tracks[0] = NULL;
    diskfile = fopen(filename, "rb");
    fseek(diskfile, 0L, SEEK_END);
    disksize = ftell(diskfile);
    bytes = std::vector<char>(disksize);
    fclose(diskfile);

    int fd;
    if ((fd = open(filename, O_RDWR)) >= 0)
    {
        char c;
        int i;
        while (read(fd, &c, 1) == 1){
            bytes[i++] = c;
        }
    }
    close(fd);

}

d88disk::~d88disk(){};
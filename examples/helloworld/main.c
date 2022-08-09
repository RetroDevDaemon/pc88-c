#include <pc88-c.h>
#include <textmode.h>

String strdat = "H e l l o   W o r l d \x00";

void main()
{
    print(strdat);
    while(1)
    { }
}
#include <pc88-c.h>

void main()
{
    print("Musical Beeper: Use Z,S,X,D,C,F,V");
    while(true)
    {
        if(GetKeyDown(KB_Z)) beep(BEEP_C4, 5);
        else if(GetKeyDown(KB_S)) beep(BEEP_D4, 6);
        else if(GetKeyDown(KB_X)) beep(BEEP_E4, 7);
        else if(GetKeyDown(KB_D)) beep(BEEP_F4, 7);
        else if(GetKeyDown(KB_C)) beep(BEEP_G4, 8);
        else if(GetKeyDown(KB_F)) beep(BEEP_A4, 9);
        else if(GetKeyDown(KB_V)) beep(BEEP_B4, 10);
        Wait_VBLANK();
    }
}

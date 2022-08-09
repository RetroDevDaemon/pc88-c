#include <pc88-c.h>
#include <beeper.h>
#include <textmode.h>

u8 octave;

#define beep_(a,b) beep(b,a)

void main()
{
    octave = 4;
    print("Musical Beeper: Use Z,S,X,D,C,F,V   3-5 change Octave");
    while(true)
    {
        SetCursorPos(0, 1);
        print("Octave: ");
        if(octave==4)print("4");
        if(octave==3)print("3");
        if(octave==5)print("5");

        if(octave == 4){ 
            if(GetKeyDown(KB_Z)) beep_(BEEP_C4, 5);
            else if(GetKeyDown(KB_S)) beep_(BEEP_D4, 6);
            else if(GetKeyDown(KB_X)) beep_(BEEP_E4, 7);
            else if(GetKeyDown(KB_D)) beep_(BEEP_F4, 7);
            else if(GetKeyDown(KB_C)) beep_(BEEP_G4, 8);
            else if(GetKeyDown(KB_F)) beep_(BEEP_A4, 9);
            else if(GetKeyDown(KB_V)) beep_(BEEP_B4, 10);
        } else if (octave == 5) { 
            if(GetKeyDown(KB_Z)) beep_(BEEP_C5, 10);
            else if(GetKeyDown(KB_S)) beep_(BEEP_D5, 11);
            else if(GetKeyDown(KB_X)) beep_(BEEP_E5, 11);
            else if(GetKeyDown(KB_D)) beep_(BEEP_F5, 12);
            else if(GetKeyDown(KB_C)) beep_(BEEP_G5, 13);
            else if(GetKeyDown(KB_F)) beep_(BEEP_A5, 14);
            else if(GetKeyDown(KB_V)) beep_(BEEP_B5, 15);
        } else if (octave == 3) { 
            if(GetKeyDown(KB_Z)) beep_(BEEP_C3, 5);
            else if(GetKeyDown(KB_S)) beep_(BEEP_D3, 5);
            else if(GetKeyDown(KB_X)) beep_(BEEP_E3, 5);
            else if(GetKeyDown(KB_D)) beep_(BEEP_F3, 5);
            else if(GetKeyDown(KB_C)) beep_(BEEP_G3, 5);
            else if(GetKeyDown(KB_F)) beep_(BEEP_A3, 5);
            else if(GetKeyDown(KB_V)) beep_(BEEP_B3, 5);
        }
        
        if(GetKeyDown(KB_3)) octave = 3;
        if(GetKeyDown(KB_4)) octave = 4;
        if(GetKeyDown(KB_5)) octave = 5;
        
        Wait_VBLANK();
    }
}

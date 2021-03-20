#include <pc88-c.h>

u8 octave;

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
            if(GetKeyDown(KB_Z)) beep(BEEP_C4, 5);
            else if(GetKeyDown(KB_S)) beep(BEEP_D4, 6);
            else if(GetKeyDown(KB_X)) beep(BEEP_E4, 7);
            else if(GetKeyDown(KB_D)) beep(BEEP_F4, 7);
            else if(GetKeyDown(KB_C)) beep(BEEP_G4, 8);
            else if(GetKeyDown(KB_F)) beep(BEEP_A4, 9);
            else if(GetKeyDown(KB_V)) beep(BEEP_B4, 10);
        } else if (octave == 5) { 
            if(GetKeyDown(KB_Z)) beep(BEEP_C5, 10);
            else if(GetKeyDown(KB_S)) beep(BEEP_D5, 11);
            else if(GetKeyDown(KB_X)) beep(BEEP_E5, 11);
            else if(GetKeyDown(KB_D)) beep(BEEP_F5, 12);
            else if(GetKeyDown(KB_C)) beep(BEEP_G5, 13);
            else if(GetKeyDown(KB_F)) beep(BEEP_A5, 14);
            else if(GetKeyDown(KB_V)) beep(BEEP_B5, 15);
        } else if (octave == 3) { 
            if(GetKeyDown(KB_Z)) beep(BEEP_C3, 5);
            else if(GetKeyDown(KB_S)) beep(BEEP_D3, 5);
            else if(GetKeyDown(KB_X)) beep(BEEP_E3, 5);
            else if(GetKeyDown(KB_D)) beep(BEEP_F3, 5);
            else if(GetKeyDown(KB_C)) beep(BEEP_G3, 5);
            else if(GetKeyDown(KB_F)) beep(BEEP_A3, 5);
            else if(GetKeyDown(KB_V)) beep(BEEP_B3, 5);
        }
        
        if(GetKeyDown(KB_3)) octave = 3;
        if(GetKeyDown(KB_4)) octave = 4;
        if(GetKeyDown(KB_5)) octave = 5;
        
        Wait_VBLANK();
    }
}

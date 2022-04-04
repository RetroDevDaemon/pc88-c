
const char intro_0[] = "It is the year 2129.";
const char intro_1[] = "\n";
const char intro_2[] = "In an event now known as \"Starfall\", mankind's extravagant\n";
const char intro_3[] = "ring of satellites and mini-colonies all simultaneously failed\n";
const char intro_4[] = "and begin a crashing descent to Earth. \n";
const char intro_5[] = "\n";
const char intro_6[] = "Fifty years after the cataclysimic event, an entity emerges. \n";
const char intro_7[] = "\n";
const char intro_8[] = "Calling themselves \"The Combine\", they recruited aimless bandits,\n";
const char intro_9[] = "mercenaries and other lost warriors for their cause - to collect\n";
const char intro_10[] = "and control as much of the knowledge and technology from before\n";
const char intro_11[] = "the stars fell as they can get their hands on. \n";
const char intro_12[] = "\n";
const char intro_13[] = "Their end goal is not known, but their efforts are deadly.\n";
const char intro_14[] = "\n";
const char intro_15[] = "You are one of a small cabal of anti-Combine archivists that\n";
const char intro_16[] = "work to recover and restore What Was Before. \n";
const char intro_17[] = "\n";
const char intro_18[] = "With zealous-like devotion, you are one of the few that can \n";
const char intro_19[] = "change the face of the future. You are one of the few whose\n";
const char intro_20[] = "name rings loud in the ears of The Combine.\n";
const char intro_21[] = "\n";
const char intro_22[] = "Your name is Libby. You are The Librarian.";
char* introText[23];

const u8 intro_offsets[] = {
    10, 0, 29, 31, 19, 0, 30, 0, 32, 32, 31, 23, 0, 29, 0, 30, 22, 0, 29, 29, 21, 0, 21
};

void RunIntro()
{
    // vars reset
    u8 i;
    u8 c;
    lastKey = -1;

    // music
    LoadSong(&introSong[0]);
    playingSong = true;
    ticker = 0;
    
    // load text
    for(i = 0; i < 23; i++)
    {    
        BufferInput();
        if(lastKey != -1)
        {
            i = 10;
            break;
        }
        CPUWAIT(250);
        for(c = 6; c < 16; c++) 
            TextRowCopy(c, c-1);
        SetCursorPos(40 - intro_offsets[i], 15);
        print(introText[i]);    
    }
    if(lastKey != 0) goto SKIPINTRO;
    // scroll remaining text until done
    for(i = 0; i < 10; i++)
    {
        BufferInput();
        if(lastKey != -1)
        {
            i = 10;
            break;
        }
        CPUWAIT(250);
        for(c = 6; c < 16-i; c++) 
            TextRowCopy(c, c-1);
    }
    
    SKIPINTRO:
}

void ConfigIntro()
{
    introText[0] = &intro_0[0];
    introText[1] = &intro_1[0];
    introText[2] = &intro_2[0];
    introText[3] = &intro_3[0];
    introText[4] = &intro_4[0];
    
    introText[5] = &intro_5[0];
    introText[6] = &intro_6[0];
    introText[7] = &intro_7[0];
    introText[8] = &intro_8[0];
    introText[9] = &intro_9[0];

    introText[10] = &intro_10[0];
    introText[11] = &intro_11[0];
    introText[12] = &intro_12[0];
    introText[13] = &intro_13[0];
    introText[14] = &intro_14[0];

    introText[15] = &intro_15[0];
    introText[16] = &intro_16[0];
    introText[17] = &intro_17[0];
    introText[18] = &intro_18[0];
    introText[19] = &intro_19[0];

    introText[20] = &intro_20[0];
    introText[21] = &intro_21[0];
    introText[22] = &intro_22[0];

    SetTextAttribute(0, 5, COLORMODE_SET(CLR_BLUE, false));
    SetTextAttribute(0, 6, COLORMODE_SET(CLR_CYAN, false));
    SetTextAttribute(0, 14, COLORMODE_SET(CLR_CYAN, false));
    SetTextAttribute(0, 15, COLORMODE_SET(CLR_BLUE, false));
}
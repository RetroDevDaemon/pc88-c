//template.c 
 /*
        // Scroll test
        SetIOReg(0x32, bit(6)); // EXPANDED on
        SetIOReg(0x35, (u8)(bit(7)|bit(4))); // ALU on, copy mode
        SetIOReg(EXPANDED_ALU_CTRL, 0b01010110);
        p = (vu8*)0xc000;
        __asm 
        ld hl, #0xc001
        ld a,(hl)
        ld de,#0xc000
        ld (de),a 
        inc hl 
        inc de 
        ld a,(hl) 
        ld (de),a 
        __endasm;
        */
        /*
    // turn on 80 col + color mode 
    SetIOReg(SYS_CTL_REGA, TEXT_BWMODE_BIT | TEXT_80COL_BIT); 
    // Print tests
    ClearAttributeRam();
    //SetIOReg(0x31, )
    SetTextAttribute(0, 0, COLORMODE_SET(CLR_FUSCHIA, true));
    SetTextAttribute(20, 0, COLORMODE_ATTR(false, false, false, true, false));
    SetTextAttribute(5, 1, COLORMODE_SET(CLR_WHITE, false));
    SetTextAttribute(6, 1, COLORMODE_ATTR(false, false, false, false, false));
    print(strdat);
    SetCursorPos(5, 1);
    print("Hello World Too\x00");
    */
    // Set ALU mode on!
    //u8 t = ReadIOReg(MODE_SET_REGISTER);
    //SetIOReg(MODE_SET_REGISTER, t | bit(6));
    //SetBGColor(CLR_RED);
    // pixels test by filling screen blue
        
    SETBANK_RED();
    p = (vu16*)0xc000;
    for(u8 y = 0; y < 200; y++){
        for(u8 x = 0; x < 40; x++){
            *p = (u16)0b0000111111110000;    // 0101010101010101
            p++;
        }
    }
    SETBANK_GREEN();
    p = (vu16*)0xc000;
    for(u8 y = 0; y < 200; y++){
        for(u8 x = 0; x < 40; x++){
            *p = (u16)0b1100000011110011;  
            p++;
        }
    }
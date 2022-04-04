//#include <pc88-c.h>

void DrawHexTile(u8 x, u8 y, u8 index)
{
    
    u8 z = y & 0b1;
    y = y >> 1;
    switch(z)
    {
        case(0):
            DrawImage_V2(2+(x*8), (y*32)+8, tilePtrs[index], 64/8, 24);
            break;
        case(1):
            DrawImage_V2(6+(x*8), (y*32)+24, tilePtrs[index], 64/8, 24);
            break;
        default:
            break;
    }
}

inline void MovementKeys()
{
    // x goes up if y is odd
    // x stays the same if y is even 
    XYPos vector;
    vector.y = 0;
    vector.x = 0;
    
    if (lastKey == UpRight) 
    {
        vector.x = 1;
        vector.y = -1;
    }
    else if (lastKey == Right)
    {
        vector.x = 1;
    }
    else if (lastKey == DownRight)
    {
        vector.y = 1;
        vector.x = 1;
    }
    else if (lastKey == UpLeft)
    {
        vector.y = -1;
        vector.x = -1;
    }
    else if (lastKey == Left)
    {
        vector.x = -1;
    }
    else if (lastKey == DownLeft)
    {
        vector.y = 1;
        vector.x = -1;
    }

    u8 yo = player_hex_pos.y % 2;
    if(yo == 0) // on even rows,
    {
        if(vector.y != 0) // when moving diagonally,
            if(vector.x > 0) // and to the right,
                vector.x--;   // adjust X by -1
    }
    else { 
        if(vector.y != 0)
            if(vector.x < 0)
                vector.x++;
    }
    targetHex.y = player_hex_pos.y + vector.y;
    targetHex.x = player_hex_pos.x + vector.x;
}


void BufferInput()
{
    switch(inputMode)
    {
        case(EXPLORING):
            if(GetKeyDown(KB_PAD9))
                lastKey = UpRight;
            if(GetKeyDown(KB_PAD6))
                lastKey = Right;
            if(GetKeyDown(KB_PAD3))
                lastKey = DownRight;
            if(GetKeyDown(KB_PAD7))
                lastKey = UpLeft;
            if(GetKeyDown(KB_PAD4))
                lastKey = Left;
            if(GetKeyDown(KB_PAD1))
                lastKey = DownLeft;
            if(GetKeyDown(KB_PAD5))
                lastKey = Confirm;
            if(GetKeyDown(KB_PAD0))
                lastKey = Cancel;
            if(GetKeyDown(KB_PADRETURN))
                lastKey = Confirm;
            break;
        case(INTRO):
            if(GetKeyDown(KB_RETURN))
                lastKey = Confirm;
            if(GetKeyDown(KB_SPACE))
                lastKey = Confirm;
            if(GetKeyDown(KB_ESC))
                lastKey = Cancel;
            break;
        default:
            lastKey = -1;
            break;
    }
}

void DrawCleanGrid()
{
    DrawImage_V2(2, 8, &hexa[0], 64/8, 24);
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | bit(4) ));
    for(u8 x = 0; x < 8; x++)
    {
        ALUCopy(VRAMAddrByTile(2,8), VRAMAddrByTile(2+(8*x),8), 8, 24);
        ALUCopy(VRAMAddrByTile(2,8), VRAMAddrByTile(2+(8*x),40), 8, 24);
        ALUCopy(VRAMAddrByTile(2,8), VRAMAddrByTile(2+(8*x),72), 8, 24);
        ALUCopy(VRAMAddrByTile(2,8), VRAMAddrByTile(2+(8*x),104), 8, 24);
    }
    ALUCopy(VRAMAddrByTile(2,8), VRAMAddrByTile(6,24), 8, 24);
    for(u8 x = 0; x < 8; x++)
    {
        ALUCopy(VRAMAddrByTile(6,24), VRAMAddrByTile(6+(8*x),24), 8, 24);
        ALUCopy(VRAMAddrByTile(6,24), VRAMAddrByTile(6+(8*x),56), 8, 24);
        ALUCopy(VRAMAddrByTile(6,24), VRAMAddrByTile(6+(8*x),88), 8, 24);
        ALUCopy(VRAMAddrByTile(6,24), VRAMAddrByTile(6+(8*x),120), 8, 24);
    }
    //*********//
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80));

}

void GameInit()
{
    playerMoved = false;
    inputMode = WAITING;
    zeropos.x = 0;
    zeropos.y = 0;
    player_hex_pos.x = 0;
    player_hex_pos.y = 0;
    for(u8 i = 0; i < 8; i++ )
    {
        loadedTiles[i].pos = zeropos; // original tile to copy
        loadedTiles[i].tileNo = 0;    // number to pull from tilePtrs[]
    }
    tilePtrs[0] = &hexa[0];
    tilePtrs[1] = &grasstile[0];
    tilePtrs[2] = &treetile[0];

    test.desc = (const char*)&desctest[0];
    test.stats[0] = GUN;
    test.stats[1] = SPEED;
    test.difficulty = 8;
    test.damage = 3;
}

void DrawFullUI()
{
    u8 i;   
    // PAPER ROLL UI THING
    DrawImage_V2(0, 0, &paperroll[0], 2, 8);
    
    DrawImage_V2(0, 8, &paperside[0], 2, 36);
    DrawImage_V2(0, 44, &paperside[0], 2, 36);
    DrawImage_V2(0, 80, &paperside[0], 2, 36);
    DrawImage_V2(0, 116, &paperside[0], 2, 36);
    
    DrawImage_V2(70, 8, &paperside[0], 2, 36);
    DrawImage_V2(70, 44, &paperside[0], 2, 36);
    DrawImage_V2(70, 80, &paperside[0], 2, 36);
    DrawImage_V2(70, 116, &paperside[0], 2, 36);
    
    for(i = 2; i < 13; i++)
        DrawImage_V2(i, 1, &rolltop[0], 1, 6);
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | bit(4) ));
    for(i = 10; i < 64; i+=8)
        ALUCopy(VRAMAddrByTile(2,1),VRAMAddrByTile(3+i,1), 8, 6);
    ExpandedGVRAM_On();    
    DrawImage_V2(69, 1, &rolltop[0], 1, 6);
    DrawImage_V2(70, 1, &rolltop[0], 1, 6);
    DrawImage_V2(71, 1, &paperend[0], 1, 6);    
    DrawImage_V2(1, 149, &paperbottom[0], 2, 2);
    DrawImage_V2(3, 149, &paperbottom[0], 2, 2);
    DrawImage_V2(5, 149, &paperbottom[0], 2, 2);
    DrawImage_V2(7, 149, &paperbottom[0], 2, 2);
    DrawImage_V2(7, 149, &paperbottom[0], 2, 2);
    DrawImage_V2(64, 149, &paperbottom[0], 2, 2);
    DrawImage_V2(66, 149, &paperbottom[0], 2, 2);
    DrawImage_V2(68, 149, &paperbottom[0], 2, 2);
    
    SetIOReg(EXPANDED_GVRAM_CTRL, (u8)(0x80 | bit(4) ));
    for(i = 8; i < 64; i+=8)
        ALUCopy(VRAMAddrByTile(1,149),VRAMAddrByTile(i,149), 8, 2);
    ExpandedGVRAM_On();
    //////
    
    // LIBBY UI
    DrawImage_V2(72, 8,     &libbytext[0],  7, 14);
    DrawImage_V2(73, 32,    &gunicon[0],    2, 7);
    DrawImage_V2(73, 51,    &booticon[0],   2, 8);
    DrawImage_V2(73, 69,    &bookicon[0],   2, 11);
    DrawImage_V2(72, 116,   &hearticon[0],  3, 7);
    //
}

void DrawAreaAroundPlayer()
{
    u8 i;
    //TODO: double check that the tile is not already drawn
    for(i = player_hex_pos.x-1; i <= player_hex_pos.x+1; i++)
        DrawHexTile(i, player_hex_pos.y, map1[(player_hex_pos.y*8)+i]);
    i -= 2;
    if(player_hex_pos.y % 2 == 0) //even - go up and -1
    {
        DrawHexTile(i-1, player_hex_pos.y-1, map1[(player_hex_pos.y*8)+i-9]);
        DrawHexTile(i, player_hex_pos.y-1, map1[(player_hex_pos.y*8)+i-8]);
        // down and -1
        DrawHexTile(i, player_hex_pos.y+1, map1[(player_hex_pos.y*8)+i-8]);
        DrawHexTile(i-1, player_hex_pos.y+1, map1[(player_hex_pos.y*8)+i-7]);
    } else { //odd - go up and +1
        DrawHexTile(i+1, player_hex_pos.y-1, map1[(player_hex_pos.y*8)+i-7]);
        DrawHexTile(i, player_hex_pos.y-1, map1[(player_hex_pos.y*8)+i-8]);
        // d and
        DrawHexTile(i, player_hex_pos.y+1, map1[(player_hex_pos.y*8)+i-8]);
    }
    
}

void PrintExploreUI()
{
    SetCursorPos(75, 4);
    print("2/2");
    SetCursorPos(75, 6);
    print("2/2");
    SetCursorPos(75, 8);
    print("2/2");
    SetCursorPos(75, 12);
    print("10/10");
    SetCursorPos(30,19);
    print("           ");
    SetCursorPos(5,16);

    //print("ENCOUNTER!\n\n   A wild RoboBoar crashes through the brush!");
    print("[7   9]\n");
    // explore mode
    print("[4   6]  Move      [5]  Search\n"); 
    print("[1   3]         [0]     Camp");
    
}

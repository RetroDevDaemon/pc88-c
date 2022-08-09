//#include <pc88-c.h>

//extern u8 revealedMap[64];

void DrawHexTile(s8 x, s8 y, u8 index)
{
    if(x < 0 || x > 7) return;
    if(y < 0 || y > 7) return;
    if (revealedMap[((y*8)+x)] == 1) return;
    //if(revealedMap[(y*8)+x] == 0)
    {
        revealedMap[(y*8)+x] = 1;
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
    } //else { 
        //return;
    //}
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
        case(INTRO):
        case(ENC_CONTINUE):
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
            if(GetKeyDown(KB_RETURN))
                lastKey = Confirm;
            if(GetKeyDown(KB_SPACE))
                lastKey = Confirm;
            if(GetKeyDown(KB_ESC))
                lastKey = Cancel;
            break;
        case(ENC_SELECT):
            if(GetKeyDown(KB_G))
                lastKey = Gun;
            if(GetKeyDown(KB_S))
                lastKey = Speed;
            if(GetKeyDown(KB_B))
                lastKey = Book;
            break;
        case(ENC_RETRY):
            if(GetKeyDown(KB_Y))
                lastKey = Yes;
            if(GetKeyDown(KB_N))
                lastKey = No;
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
    tilePtrs[3] = &citytile[0];

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
    s8 i;
    i = player_hex_pos.x;
    u8* mapp = &map1[0];
    if(currentMap == 2)
        mapp += 64;
    else if(currentMap == 3)
        mapp += 128;
    DrawHexTile(i-1, player_hex_pos.y, *(u8*)((u16)mapp + (player_hex_pos.y*8)+i-1));
    DrawHexTile(i, player_hex_pos.y, *(u8*)((u16)mapp + (player_hex_pos.y*8)+i));
    DrawHexTile(i+1, player_hex_pos.y,*(u8*)((u16)mapp + (player_hex_pos.y*8)+i+1));
    i = player_hex_pos.x-1;
    if(player_hex_pos.y % 2 == 0) //even - go up and -1
    {
        DrawHexTile(i, player_hex_pos.y-1, *(u8*)((u16)mapp + (player_hex_pos.y*8)+i-8));
        i++;
        DrawHexTile(i, player_hex_pos.y-1, *(u8*)((u16)mapp + (player_hex_pos.y*8)+i-8));
        i = player_hex_pos.x-1;    
        DrawHexTile(i, player_hex_pos.y+1, *(u8*)((u16)mapp + (player_hex_pos.y*8)+i+8));
        i++;
        DrawHexTile(i, player_hex_pos.y+1, *(u8*)((u16)mapp + (player_hex_pos.y*8)+i+8));
        
    } else { //odd - go up and +1
        i++;
        DrawHexTile(i, player_hex_pos.y-1, *(u8*)((u16)mapp + (player_hex_pos.y*8)+i-8));
        i++;
        DrawHexTile(i, player_hex_pos.y-1, *(u8*)((u16)mapp + (player_hex_pos.y*8)+i-8));
        i = player_hex_pos.x;    
        DrawHexTile(i, player_hex_pos.y+1, *(u8*)((u16)mapp + (player_hex_pos.y*8)+i+8));
        i++;
        DrawHexTile(i, player_hex_pos.y+1, *(u8*)((u16)mapp + (player_hex_pos.y*8)+i+8));
        
    }
    
}

void PrintExploreUI(u8 full)
{
    SetCursorPos(73, 3);
    print(byToDec(Libby.gun));
    print("/");
    SetCursorPos(74, 4);
    print(byToDec(Libby.gunMax));

    SetCursorPos(73, 5);
    print(byToDec(Libby.speed));
    print("/");
    SetCursorPos(74, 6);
    print(byToDec(Libby.speedMax));

    SetCursorPos(73, 7);
    print(byToDec(Libby.book));
    print("/");
    SetCursorPos(74, 8);
    print(byToDec(Libby.bookMax));

    SetCursorPos(73, 12);
    print(byToDec(Libby.hp));
    print("/");
    SetCursorPos(74, 13);
    print(byToDec(Libby.hpMax));

    SetCursorPos(30,19);
    print("           ");
    SetCursorPos(5,16);
    if(full){
        //print("ENCOUNTER!\n\n   A wild RoboBoar crashes through the brush!");
        print("[7   9]\n");
        // explore mode
        print("[4   6]  Move      [5]  Search\n"); 
        print("[1   3]         ");//[0]     Camp");
    }
    
}


void LoadMap(u8 map)
{
    // roll 3d6
    //currentMap = 1;
    if(map == 1){
        map_encounters[0] = &heal1;
        map_encounters[1] = &heal1;
        map_encounters[2] = &heal5;

        map_encounters[3] = &gs_boar;
        map_encounters[4] = &gs_drone;
        map_encounters[5] = &gs_soldier;
        
        map_encounters[6] = &gb_boar;
        map_encounters[7] = &gb_drone;
        map_encounters[8] = &gb_soldier;
        
        map_encounters[9] = &bs_blank;
        map_encounters[10] = &bs_chest;
        map_encounters[11] = &bs_drone;

        map_encounters[12] = &blank1;
        map_encounters[13] = &blank2;
        map_encounters[14] = &blank1;
        map_encounters[15] = &blank2;
        
        maprelic[0].x = 6;
        maprelic[0].y = 5;
        
        mapexit[0].x = 4;
        mapexit[0].y = 1;
    }
    else if(map == 2)
    {
        map_encounters[0] = &heal1;
        map_encounters[1] = &heal3;
        map_encounters[2] = &heal5;

        map_encounters[3] = &gs_mine;
        map_encounters[4] = &gs_drone;
        map_encounters[5] = &gs_soldier;
        
        map_encounters[6] = &gb_boar;
        map_encounters[7] = &gb_drone;
        map_encounters[8] = &gb_soldier;
        map_encounters[9] = &gb_roper;

        map_encounters[10] = &bs_roper;
        map_encounters[11] = &bs_chest;
        map_encounters[12] = &bs_drone;

        map_encounters[13] = &blank1;
        map_encounters[14] = &blank2;
        map_encounters[15] = &blank1;
        
        maprelic[1].x = 6;
        maprelic[1].y = 6;
        
        mapexit[1].x = 2;
        mapexit[1].y = 2;
    }  
    else if(map == 3)
    {
        map_encounters[0] = &heal3;
        map_encounters[1] = &heal3;
        map_encounters[2] = &heal5;

        map_encounters[3] = &gs_mine;
        map_encounters[4] = &gs_roper;
        map_encounters[5] = &gs_soldier;
        
        map_encounters[6] = &gb_roper;
        map_encounters[7] = &gb_blank;
        map_encounters[8] = &gb_drone;
        map_encounters[9] = &gb_soldier;
        
        map_encounters[10] = &bs_roper;
        map_encounters[11] = &bs_chest;
        map_encounters[12] = &bs_drone;
        map_encounters[13] = &bs_soldier;


        map_encounters[14] = &blank1;
        map_encounters[15] = &blank2;
        
        maprelic[2].x = 0;
        maprelic[2].y = 0;
        
        mapexit[2].x = 7;
        mapexit[2].y = 7;
    }

}

void BeginEncounter(u8 encNo)
{
    Encounter* enc = map_encounters[encNo];
    currentEncounter = enc;

    TextRowCopy(0, 15);
    TextRowCopy(0, 16);
    TextRowCopy(0, 17);
    TextRowCopy(0, 18);
    TextRowCopy(0, 19);
    
    SetCursorPos(33, 15);
    print("--ENCOUNTER--");
    
    SetCursorPos(1, 16);
    print(enc->desc);
    //print(map1_encounters[0]->desc);

    ExpandedGVRAM_On();     
    EnableALU(1);
    // draw deck
    DrawImage_V2(53, 162, &deck[0], 8, 38);
    DrawImage_V2(67, 162, &deck[0], 8, 38);
    // draw player/enemy
    if(enc->encounterSpr != NULL)
        DrawTransparentImage_V2(68, 154, enc->encounterSpr, 4, 24);
    DrawTransparentImage_V2(55, 154, &librarianSprite[0], 4, 24);
    // erase sprite
    ExpandedGVRAM_Copy_On();
    //ALUCopyIn(TEMPGVR_SPRITE_0, GVRAM_BASE+(150*80)+54, 4, 24); // tempgvr 0 = background of players tile
    ALUCopyIn(TEMPGVR_SPRITE_0, GVRAM_BASE+(player_pos.y*80)+player_pos.x, 4, 24);
    
    ExpandedGVRAM_Off();     
    DisableALU(0);

    inputMode = ENC_SELECT;
    if(enc->stats[0] == 0){
        if(enc->stats[1] == 0){
            inputMode = ENC_CONTINUE;
        }
    }
}

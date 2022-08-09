// encounters.c


typedef struct encounter { 
    const char* desc;
    u8 stats[2];    // 0 1 or 2 to identify stat
    u8 difficulty[2];  // 3-18
    u8 damage[2];      // always on fail 
    const char* encounterSpr;
} Encounter;

//heal1 TREASURE?
const char desc01[] = \
"You find a can of barely-expired cat food.\n\
 (You regain 1 HP. Press a key.)";
//heal3 TREASURE?
const char desc02[] = \
"You find a discarded bottle of MetaboStim, still\n\
half-full. (You regain 3 HP! Press a key.)";
//heal5 TREASURE?
const char desc03[] = \
"You find a nano-steroidal injector. This should\n\
help you keep going for a while!\n\
(You heal 5 HP! Press a key.)";

//gs1
//BOAR
const char desc04[] = \
"You search about, but find nothing useful.\n\
Suddenly, a wild RoboBoar crashes through the brush!\n\
Will you try to pull your gun and take aim, or\n\
abscond while you are able? ([G]un or [S]peed?)";
//TODO DRAW COMBINE DRONE
const char desc05[] = \
"A siren suddenly echoes through the forest--\n\
the telltale screech of a Combine drone. Will\n\
you shoot it down, or quickly try to hide in a\n\
nearby bush? ([G]un or [S]peed?)";
//SOLDIER
const char desc06[] = \
"A lone Combine soldier appears to be patrolling\n\
this area. Do you take aim while his back is\n\
turned, or attempt to deftly evade his vision?\n\
 ([G]un or [S]peed?)";
// NO SPR
const char desc07[] = \
"Before you are aware of what is happening, you\n\
stumble over a well-hidden tripwire! Do you try to\n\
detonate the explosive with your gun, or attempt\n\
to evade the blast? ([G]un or [S]peed?)";
// TODO DRAW ROPE MONSTER
const char desc08[] = \
"You nearly shriek in disgust as an Irradiated\n\
Roper snaps a tentacle out at you from its hiding\n\
place. Do you shoot the hideous thing, or escape\n\
from its limited reach? ([G]un or [S]peed?)";


//gb1 boar
const char desc09[] = \
"Following a strange cry, you wander off the path.\n\
Just then, several RoboBoar appear and surround you!\n\
Will you fire your gun to scare them off, or read\n\
a heresy from the book? ([G]un or [B]ook?)";
//drone
const char desc10[] = \
"You encounter a damaged Combine drone. It is too\n\
dangerous to leave alone. Will you destroy it with\n\
your gun, or recite a disassembly heresy from the\n\
Chapter of Eng? ([G]un or [B]ook?)";
//soldier
const char desc11[] = \
"You are confronted with a retinue of Combine\n\
soldiers! You manage to evade pursuit by all but\n\
one. Do you take aim with your gun, or recite a\n\
heresy from the book? ([G]un or [B]ook?)";
//roper
const char desc12[] = \
"Blast it! An Irradiated Roper snakes out of no-\n\
where and wraps you in its clutches! Will you \n\
shoot the tentacles away with your gun, or recite\n\
a heresy from the book? ([G]un or [B]ook?)";
//blank
const char desc13[] = \
"You inexplicably sense a presense nearby. It\n\
could be a Hidden Ghast, or something worse. Will\n\
you fire your gun blindly and maybe scare it off,\n\
or read an exorcism heresy? ([G]un or [B]ook?)";

//B/S
//blank
const char desc14[] = \
"The telltale ticking of a Combine land mine rings\n\
in your ears. It's close - too close to escape.\n\
Will you recite a nullification heresy, or dive\n\
behind cover? ([B]ook or [S]peed?)";
//chest
const char desc15[] = \
"You come upon a Combine storage chest. Unable to\n\
hold back, you punch the control panel - only to\n\
be assaulted by poison gas! Will you recite a\n\
purification heresy, or run? ([B]ook or [S]peed?)";
//drone
const char desc16[] = \
"A Combine drone buzzes by and takes aim! A red\n\
dot appears on your chest. Will you dash away as\n\
fast as you can, or consult the book for a\n\
jamming prayer? ([B]ook or [S]peed?)";
//roper
const char desc17[] = \
"You come upon an Irradiated Roper, feasting on a\n\
recent kill. You might be able to sneak around it,\n\
or you can consult the book and read a\n\
purification heresy. ([B]ook or [S]peed?)";
//combine
const char desc18[] = \
"A pair of Combine soldiers, clearly inebriated,\n\
draw near. Not recognizing your guild colors, they\n\
ask you for your papers. Do you recite a con-\n\
fusion heresy, or run away? ([B]ook or [S]peed?)";

//nothing
const char desc19[] = \
"You find an abandonded building with the lock\n\
still intact. You go inside and search about, but\n\
find nothing of use... (Press a key.)";
//nothing
const char desc20[] = \
"Your keen eye spots an old shelter, cleverly\n\
hidden beneath some natural brush. After\n\
confirming it is empty, you search it thoroughly,\n\
but find nothing...  (Press a key.)";


//HEALS
const Encounter heal1 = { \
    {(const char*)&desc01[0]},
    {0, 0},
    {0, 0},
    {0, 1},
    {(const char*)&chest[0]}
};
const Encounter heal3 = { \
    {(const char*)&desc02[0]},
    {0, 0},
    {0, 0},
    {0, 3},
    {(const char*)&chest[0]}
};
const Encounter heal5 = { \
    {(const char*)&desc03[0]},
    {0, 0},
    {0, 0},
    {0, 5},
    {(const char*)&chest[0]}
};
const Encounter blank1 = {\
    {&desc19[0]},
    {0,0},
    {0,0},
    {0,0},
    {0}
};
const Encounter blank2 = {\
    {&desc20[0]},
    {0,0},
    {0,0},
    {0,0},
    {0}
};
/////////////////////
// GUN AND SPEED 
const Encounter gs_boar = {\
    {&desc04[0]},
    {GUN,  SPEED},
    {9,    7},
    {3,    3},
    {(const char*)&roboboar[0]}
};
const Encounter gs_drone = {\
    {&desc05[0]},
    {GUN,  SPEED},
    {8,    6},
    {2,    3},
    {(const char*)&drone[0]}
};
const Encounter gs_soldier = {\
    {&desc06[0]},
    {GUN,  SPEED},
    {10,    7},
    {3,    4},
    {(const char*)&enemy1[0]}
};
const Encounter gs_mine = {\
    {&desc07[0]},
    {GUN,  SPEED},
    {8,    8},
    {4,    4},
    {(const char*)0}
};
const Encounter gs_roper = {\
    {&desc08[0]},
    {GUN,  SPEED},
    {9,    10},
    {5,   4},
    {(const char*)&roper[0]}
};

/////////
// GUN AND BOOK 
const Encounter gb_boar = {\
    {&desc09[0]},
    {GUN,  BOOK},
    {9,    9},
    {3,    3},
    {&roboboar[0]}
};
const Encounter gb_drone = {\
    {&desc10[0]},
    {GUN,  BOOK},
    {9,    8},
    {2,    3},
    {&drone[0]}
};
const Encounter gb_soldier = {\
    {&desc11[0]},
    {GUN,  BOOK},
    {10,    7},
    {3,    5},
    {&enemy1[0]}
};
const Encounter gb_roper = {\
    {&desc12[0]},
    {GUN,  BOOK},
    {10,    9},
    {3,    4},
    {&roper[0]}
};
const Encounter gb_blank = {\
    {&desc13[0]},
    {GUN,  BOOK},
    {8,    11},
    {4,    5},
    {0}
};

///////////////////////
// FINISHME
// BOOK AND SPEED LEVEL 1
const Encounter bs_blank = {\
    {&desc14[0]},
    {SPEED,  BOOK},
    {9,    10},
    {3,    3},
    {0}
};
const Encounter bs_chest = {\
    {&desc15[0]},
    {SPEED,  BOOK},
    {9,    7},
    {2,    3},
    {&chest[0]}
};
const Encounter bs_drone = {\
    {&desc16[0]},
    {SPEED,  BOOK},
    {10,    6},
    {2,    4},
    {&drone[0]}
};
const Encounter bs_roper = {\
    {&desc17[0]},
    {SPEED,  BOOK},
    {10,    7},
    {3,    4},
    {&roper[0]}
};
const Encounter bs_soldier = {\
    {&desc18[0]},
    {SPEED,  BOOK},
    {8,    11},
    {5,    3},
    {&enemy1[0]}
};




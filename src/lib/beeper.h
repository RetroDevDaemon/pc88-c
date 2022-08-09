// beeper.h
/*!
 *  \addtogroup Beep
 *  @{
 */
#ifndef _88_beeper_
#define _88_beeper_ 

// BEEP
void beep(u8 length, u16 tone) __naked; 

// Snd
//            ((CPU SPD /  TUNEHZ)  - 138) / 48
/// Formula: ((3993600 / <Tune>) - 138) / 48
#define BEEP_C3 (u16)(((3993600/131)-138)/48)
#define BEEP_D3 (u16)(((3993600/147)-138)/48)
#define BEEP_E3 (u16)(((3993600/165)-138)/48)
#define BEEP_F3 (u16)(((3993600/174)-138)/48)
#define BEEP_G3 (u16)(((3993600/196)-138)/48)
#define BEEP_A3 (u16)(((3993600/220)-138)/48)
#define BEEP_B3 (u16)(((3993600/247)-138)/48)

#define BEEP_C4 315
#define BEEP_D4 281 
#define BEEP_E4 250 
#define BEEP_F4 235 
#define BEEP_G4 209 
#define BEEP_A4 186 
#define BEEP_B4 165

#define BEEP_C5 (u16)(((3993600/523)-138)/48)
#define BEEP_D5 (u16)(((3993600/587)-138)/48)
#define BEEP_E5 (u16)(((3993600/659)-138)/48)
#define BEEP_F5 (u16)(((3993600/698)-138)/48)
#define BEEP_G5 (u16)(((3993600/784)-138)/48)
#define BEEP_A5 (u16)(((3993600/880)-138)/48)
#define BEEP_B5 (u16)(((3993600/988)-138)/48)


#include <beeper.c>

#endif 
/*! @} */
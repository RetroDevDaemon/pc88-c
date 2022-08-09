
//textmode.h

#ifndef _88_textmode_
#define _88_textmode_

// TEXTMODE
/** Attributes must be set in ascending X order on each row. 
 *   Attribute #0 on each row is ALWAYS read as X=0, regardless of its actual value. 
 */
void SetTextAttribute(u8 x, u8 y, u8 attr);

/// Resets to the bios defaults.
void ClearAttributeRam();       

void SetCursorPos(u8 x, u8 y);
/**/void SetCursorPos40(u8 x, u8 y);

/*! Converts a single byte to ASCII numeric form */
u8* byToHex(u8 by); 
u8* byToDec(u8 by);

/*! Moves a row of text from row src to row dst (0-24)*/
inline void TextRowCopy(u8 src, u8 dst);

/*! Sets entire 80x25 text screen to 0x20 (' ')
 *  Note - you will still need to ClearAttributeRam()
 *  to get rid of color codes.*/
void CLS();

#include "textmode.c"

#endif 
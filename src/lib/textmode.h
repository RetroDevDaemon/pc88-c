#ifndef PC88TEXTMODE
#define PC88TEXTMODE
#include <pc88-c.h>
// textmode.h


inline void putchr(u8 c);
inline void putchr40(u8 c);

void SetTextAttribute(u8 x, u8 y, u8 attr);
void ClearAttributeRam();       // Resets to the bios defaults.

void SetCursorPos(u8 x, u8 y);
/**/void SetCursorPos40(u8 x, u8 y);

#endif 
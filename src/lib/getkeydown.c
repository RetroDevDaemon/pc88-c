/*! \addtogroup keyboard
* @{
*/

#include <pc88-c.h>

/** Takes specific scancode value defined above. 
 * \returns true if pressed, otherwise false. 
 * \note Only reads one key at a time.
 */
bool GetKeyDown(u8 SCANCODE)
{
    u8 r = (SCANCODE & 0xf0) >> 4;
    u8 b = SCANCODE & 0x0f;
    u8 i = ReadIOReg(r);
    if(! (i & bit(b)) ) { return true; }
    return false;
}

/*! @} */
//draw.h
#ifndef _draw_88_
#define _draw_88_

#define FASTMEM_ON 1 
#define FASTMEM_OFF false 
void SetMonitor(u8 khz, u8 rows);
/*! The first pixel in the given 4bpp bitmap array is read as
 *  the transparent color. This color will not be written, thus
 *  will be XORed with the background. 
 */
vu8* DrawTransparentImage_V2(u8 x, u8 y, u8* img, u8 w, u8 h);
/*! Writes a 4bpp bitmap without transparent index.
 *  slightly faster than transparent version.
 */
void DrawImage_V2(u8 x, u8 y, u8* img, u8 w, u8 h);
/*! Use this for one-plane images, otherwise use DrawPlanarBitmap()
 *  For use in V1 mode. 1bpp planar bitmap.
 */


void DrawPlaneBMP(const u8* img, u8 plane, u16 x, u16 y, u8 w, u8 h);
void SetPixel(u16 x, u8 y, u8 c);
inline void EnableALU(u8 fastmem);
inline void DisableALU(u8 fastmem);
inline void ExpandedGVRAM_On(); 
inline void ExpandedGVRAM_Off(); 
inline void CRT_OFF();
inline void CRT_ON();


#include "draw.c"

#endif 
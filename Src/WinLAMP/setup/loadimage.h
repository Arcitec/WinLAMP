#ifndef WINLAMP_LOADIMAGE_HEADER
#define WINLAMP_LOADIMAGE_HEADER

#include <windows.h>

HBITMAP WALoadImage(HMODULE hMod, LPCWSTR pszSection, LPCWSTR pszImage, BOOL bPremult);
HBITMAP WAResizeImage(HBITMAP hbmp, INT cx, INT cy, HBRUSH hbBk);
HBITMAP WABlendOnColor(HBITMAP hbmp, COLORREF rgbBk);
HBITMAP WABlendOnARGB32(HBITMAP hbmp, LPBYTE pRGB);
HBITMAP WACreatePatternBitmap(INT cx, INT cy, INT bpp, HBRUSH hbPattern, LPBYTE *ppData);

#endif //WINLAMP_LOADIMAGE_HEADER
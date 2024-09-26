#ifndef WINLAMP_POSTSETUP_HEADER
#define WINLAMP_POSTSETUP_HEADER

#include <windows.h>

HWND CreateStatusWnd(HWND hwndParent, INT x, INT y, INT cx, INT cy);
void SetStatusText();
BOOL StartWinLAMP(BOOL bWaitShow, HWND *phwndWA, LPCSTR pszParam);

#endif //WINLAMP_POSTSETUP_HEADER
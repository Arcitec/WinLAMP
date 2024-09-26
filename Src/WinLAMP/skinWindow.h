#ifndef NULLOSFT_WINLAMP_SKINNEDWINDOW_HEADER
#define NULLOSFT_WINLAMP_SKINNEDWINDOW_HEADER

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <wtypes.h>

BOOL SkinWindow(HWND hwndToSkin, REFGUID windowGuid, UINT flagsEx, FFCALLBACK callbackFF);

#endif //NULLOSFT_WINLAMP_SKINNEDWINDOW_HEADER
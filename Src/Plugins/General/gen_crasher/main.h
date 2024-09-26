#ifndef NULLSOFT_CRASHER_MAIN_H
#define NULLSOFT_CRASHER_MAIN_H

#include <windows.h>
#include <dbghelp.h>
#include "resource.h"
#define NO_IVIDEO_DECLARE
#include "..\winlamp\wa_ipc.h"
#include "settings.h"
#include "../winlamp/gen.h"

extern Settings settings;
extern prefsDlgRecW prefItem;
extern char *winlampVersion;
extern "C" winlampGeneralPurposePlugin plugin;

extern "C" __declspec(dllexport) int StartHandler(wchar_t* iniPath);
extern "C" LONG WINAPI FeedBackFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );

//typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

static BOOL alreadyProccessing;

#endif // NULLSOFT_CRASHER_MAIN_H
#ifndef NULLSOFT_WINLAMP_HOOK_HEADER
#define NULLSOFT_WINLAMP_HOOK_HEADER

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <wtypes.h>
#include <map>

class ifc_winlamphook;

class WinLAMPHook
{
protected:
	WinLAMPHook(HWND hwndWinLAMP);
	~WinLAMPHook();

public:
	static HRESULT CreateInstance(HWND hwndWinLAMP, WinLAMPHook **instance);

public:
	ULONG AddRef();
	ULONG Release();

	HRESULT RegisterCallback(ifc_winlamphook *callback, UINT *cookie);
	HRESULT UnregisterCallback(UINT cookie);

	HWND GetWinLAMP();
	LRESULT CallPrevWinLAMPProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CallDefWinLAMPProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	HRESULT AttachToWinLAMP();
	HRESULT DetachFromWinLAMP();
	LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnWinLAMPDestroy();
	LRESULT OnWinLAMPIPC(UINT commandId, WPARAM param);
	void OnWinLAMPCommand(UINT commandId, UINT controlId, HWND hControl);
	void OnSysColorChange();

protected:
	typedef enum
	{
		flagUnicode = 0x00000001,
	} Flags;

	typedef std::map<UINT, ifc_winlamphook*> CallbackMap;

	friend static LRESULT CALLBACK WinLAMPWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	ULONG ref;
	HWND hwnd;
	WNDPROC originalProc;
	UINT flags;
	UINT lastCookie;
	CallbackMap callbackMap;
};

#endif //NULLSOFT_WINLAMP_HOOK_HEADER
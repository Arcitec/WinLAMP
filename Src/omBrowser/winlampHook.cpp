#include "main.h"
#include "./winlampHook.h"
#include "./ifc_winlamphook.h"
#include "../winlamp/wa_ipc.h"

#include <windows.h>

#define WINLAMP_REFRESHSKIN              40291
#define WHPROCRECOVERY					L"WaHookProcRecovery"

static ATOM WAWNDATOM = 0;
#define GetWinLAMPHook(__hwnd) ((WinLAMPHook*)GetProp((__hwnd), MAKEINTATOM(WAWNDATOM)))

WinLAMPHook::WinLAMPHook(HWND hwndWinLAMP)
	: ref(1), hwnd(hwndWinLAMP), originalProc(NULL), flags(0), lastCookie(0)
{
}

WinLAMPHook::~WinLAMPHook()
{
	DetachFromWinLAMP();

	if (0 != WAWNDATOM)
	{
		GlobalDeleteAtom(WAWNDATOM);
		WAWNDATOM = 0;
	}
}

HRESULT WinLAMPHook::CreateInstance(HWND hwndWinLAMP, WinLAMPHook **instance)
{
	if (NULL == instance) return E_POINTER;

	*instance = NULL;

	if (NULL == hwndWinLAMP || FALSE == IsWindow(hwndWinLAMP))
		return E_INVALIDARG;

	*instance = new WinLAMPHook(hwndWinLAMP);
	if (NULL == *instance) return E_OUTOFMEMORY;

	return S_OK;
}

ULONG WinLAMPHook::AddRef()
{
	return InterlockedIncrement((LONG*)&ref);
}

ULONG WinLAMPHook::Release()
{
	if (0 == ref)
		return ref;

	LONG r = InterlockedDecrement((LONG*)&ref);
	if (0 == r)
		delete(this);

	return r;
}

HRESULT WinLAMPHook::RegisterCallback(ifc_winlamphook *callback, UINT *cookie)
{
	if (NULL == cookie) return E_POINTER;
	*cookie = 0;

	if (NULL == callback) return E_INVALIDARG;

	if (FAILED(AttachToWinLAMP()))
		return E_UNEXPECTED;

	*cookie = ++lastCookie;

	callbackMap.insert({ *cookie, callback });
	callback->AddRef();

	return S_OK;
}

HRESULT WinLAMPHook::UnregisterCallback(UINT cookie)
{
	if (0 == cookie) return E_INVALIDARG;

	for(CallbackMap::iterator iter = callbackMap.begin(); iter != callbackMap.end(); iter++)
	{
		if (cookie == iter->first)
		{
			ifc_winlamphook *hook = iter->second;
			callbackMap.erase(iter);
			
			if (NULL != hook) 
				hook->Release();

			return S_OK;
		}
	}
	return S_FALSE;
}

HWND WinLAMPHook::GetWinLAMP()
{
	return hwnd;
}

LRESULT WinLAMPHook::CallPrevWinLAMPProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (NULL == originalProc || NULL == hwnd) return 0;
	return (0 != (flagUnicode & flags)) ? 
		CallWindowProcW(originalProc, hwnd, uMsg, wParam, lParam) : 
		CallWindowProcA(originalProc, hwnd, uMsg, wParam, lParam);
}

LRESULT WinLAMPHook::CallDefWinLAMPProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (NULL == hwnd) return 0;
	return (0 != (flagUnicode & flags)) ? 
		DefWindowProcW(hwnd, uMsg, wParam, lParam) : 
		DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

HRESULT WinLAMPHook::AttachToWinLAMP()
{
	if (NULL == hwnd || !IsWindow(hwnd)) return E_UNEXPECTED;

	if (0 == WAWNDATOM)
	{
		 WAWNDATOM = GlobalAddAtom(L"WinLAMPHook");
		 if (0 == WAWNDATOM) return E_FAIL;
	}

	WinLAMPHook *hookInstance = GetWinLAMPHook(hwnd);
	if (this == hookInstance) return S_FALSE;

	if (NULL != hookInstance || NULL != originalProc) 
		return E_FAIL;

	flags = 0;
	if (IsWindowUnicode(hwnd)) flags |= flagUnicode;

	originalProc = (WNDPROC)(LONG_PTR)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONGX86)(LONG_PTR)WinLAMPWindowProc);
	if (NULL == originalProc || FALSE == SetProp(hwnd, MAKEINTATOM(WAWNDATOM), this))
	{
		if (NULL != originalProc)
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONGX86)(LONG_PTR)originalProc);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT WinLAMPHook::DetachFromWinLAMP()
{
	if (NULL == hwnd || !IsWindow(hwnd)) return E_UNEXPECTED;
	if (0 == WAWNDATOM) return E_FAIL;

	for(CallbackMap::iterator iter = callbackMap.begin(); iter != callbackMap.end(); iter++)
	{
		if (NULL != iter->second) 
			iter->second->Release();
	}
	callbackMap.clear();

	WinLAMPHook *hookInstance = GetWinLAMPHook(hwnd);
	if (this != hookInstance) return E_FAIL;

	RemoveProp(hwnd, MAKEINTATOM(WAWNDATOM));
	WNDPROC currentProc = (WNDPROC)(LONG_PTR)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
	if (currentProc == WinLAMPWindowProc)
	{
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONGX86)(LONG_PTR)originalProc);
	}
	else
	{
		SetProp(hwnd, WHPROCRECOVERY, (HANDLE)originalProc);
	}
	originalProc = NULL;
	flags = 0;
	return S_OK;
}

LRESULT WinLAMPHook::OnWinLAMPDestroy()
{
	WNDPROC proc = originalProc;
	DetachFromWinLAMP();
	return (IsWindowUnicode(hwnd)) ? 
			CallWindowProcW(proc, hwnd, WM_DESTROY, 0, 0L) : 
			CallWindowProcA(proc, hwnd, WM_DESTROY, 0, 0L);
}

LRESULT WinLAMPHook::OnWinLAMPIPC(UINT commandId, WPARAM param)
{
	switch(commandId)
	{
		case IPC_CB_RESETFONT:
			for(CallbackMap::iterator iter = callbackMap.begin(); iter != callbackMap.end(); iter++)
				if (NULL != iter->second) iter->second->ResetFont();
			break;

		case IPC_HOOK_OKTOQUIT:
			for(CallbackMap::iterator iter = callbackMap.begin(); iter != callbackMap.end(); iter++)
				if (NULL != iter->second && S_FALSE == iter->second->IsQuitAllowed()) return 0;
			break;

		case IPC_SKIN_CHANGED:
			{
				WCHAR szBuffer[MAX_PATH*2] = {0};
				SENDWAIPC(hwnd, IPC_GETSKINW, szBuffer);
				for(CallbackMap::iterator iter = callbackMap.begin(); iter != callbackMap.end(); iter++)
					if (NULL != iter->second) iter->second->SkinChanged(szBuffer);
			}
			break;
		
		case IPC_FF_ONCOLORTHEMECHANGED:
			if (FALSE != IS_INTRESOURCE(param))
				param = 0L;
			for(CallbackMap::iterator iter = callbackMap.begin(); iter != callbackMap.end(); iter++)
				if (NULL != iter->second) iter->second->SkinColorChange((LPCWSTR)param);
			break;

		case IPC_FILE_TAG_MAY_HAVE_UPDATED:
			{
				WCHAR szBuffer[MAX_PATH*2] = {0};
				param = (0 != MultiByteToWideChar(CP_ACP, 0, (char*)param, -1, szBuffer, ARRAYSIZE(szBuffer))) ?
						(WPARAM)szBuffer : 0L;

				for(CallbackMap::iterator iter = callbackMap.begin(); iter != callbackMap.end(); iter++)
					if (NULL != iter->second) iter->second->FileMetaChange((LPCWSTR)param);
			}
			break;

		case IPC_FILE_TAG_MAY_HAVE_UPDATEDW:
			for(CallbackMap::iterator iter = callbackMap.begin(); iter != callbackMap.end(); iter++)
				if (NULL != iter->second) iter->second->FileMetaChange((LPCWSTR)param);
			break;
	}

	return CallPrevWinLAMPProc(WM_WA_IPC, param, (LPARAM)commandId);
}

void WinLAMPHook::OnWinLAMPCommand(UINT commandId, UINT controlId, HWND hControl)
{
	switch(commandId)
	{
		case WINLAMP_REFRESHSKIN:
			for(CallbackMap::iterator iter = callbackMap.begin(); iter != callbackMap.end(); iter++)
				if (NULL != iter->second) iter->second->SkinChanging();
			break;
	}
}

void WinLAMPHook::OnSysColorChange()
{
	for(CallbackMap::iterator iter = callbackMap.begin(); iter != callbackMap.end(); iter++)
		if (NULL != iter->second) iter->second->SysColorChange();
}

LRESULT WinLAMPHook::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_DESTROY:		return OnWinLAMPDestroy();
		case WM_WA_IPC:			return OnWinLAMPIPC((UINT)lParam, wParam);
		case WM_COMMAND:		OnWinLAMPCommand(LOWORD(wParam), HIWORD(wParam), (HWND)lParam); break;
		case WM_SYSCOLORCHANGE:	OnSysColorChange(); break;
	}
	return CallPrevWinLAMPProc(uMsg, wParam, lParam);
}

static LRESULT CALLBACK WinLAMPWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WinLAMPHook *hookInstance = GetWinLAMPHook(hwnd);
	if (NULL == hookInstance)
	{
		WNDPROC recovery = (WNDPROC)GetProp(hwnd, WHPROCRECOVERY);
		if (NULL != recovery)
		{			
			return (IsWindowUnicode(hwnd)) ? 
				CallWindowProcW(recovery, hwnd, uMsg, wParam, lParam) : 
				CallWindowProcA(recovery, hwnd, uMsg, wParam, lParam);
		}

		return (IsWindowUnicode(hwnd)) ? 
			DefWindowProcW(hwnd, uMsg, wParam, lParam) : 
			DefWindowProcA(hwnd, uMsg, wParam, lParam);
	}

	return hookInstance->WindowProc(uMsg, wParam, lParam);
}
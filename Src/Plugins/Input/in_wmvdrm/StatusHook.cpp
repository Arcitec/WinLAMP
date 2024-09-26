#if 0
#include <windows.h>
#include "../WinLAMP/wa_ipc.h"
#include "Main.h"
#include <shlwapi.h>

static WNDPROC waProc=0;
static bool winlampisUnicode=false;

static LRESULT WINAPI StatusHookProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	if (msg == WM_WA_IPC && lParam == IPC_HOOK_TITLESW)
	{
		LRESULT downTheLine = winlampisUnicode?CallWindowProcW(waProc, hwnd, msg, wParam, lParam):CallWindowProcA(waProc, hwnd, msg, wParam, lParam);
		waHookTitleStructW *hook = (waHookTitleStructW *)wParam;
		if (!PathIsURLW(hook->filename) && winlamp.GetStatusHook(hook->title, 2048, hook->filename))
		{
			return TRUE;
		}
		else
			return downTheLine;
	}

	if (waProc)
	{
		if (winlampisUnicode)
			return CallWindowProcW(waProc, hwnd, msg, wParam, lParam);
		else 
			return CallWindowProcA(waProc, hwnd, msg, wParam, lParam);
	}
	else
		return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Hook(HWND winlamp)
{
	if (winlamp)
	{
		winlampisUnicode = !!IsWindowUnicode(winlamp);
		if (winlampisUnicode) 
			waProc = (WNDPROC)SetWindowLongPtrW(winlamp, GWLP_WNDPROC, (LONG_PTR)StatusHookProc);
		else 
			waProc = (WNDPROC)SetWindowLongPtrA(winlamp, GWLP_WNDPROC, (LONG_PTR)StatusHookProc);
	}
}

void Unhook(HWND winlamp)
{
//	if (winlamp && GetWindowLongA(winlamp,GWL_WNDPROC) == (LONG)StatusHookProc)
		//SetWindowLong(winlamp, GWL_WNDPROC, (LONG)waProc);
	//waProc=0;
}
#endif
#include "Main.h"
#include "WinLAMPInterface.h"
#include "../WinLAMP/wa_ipc.h"
#include <cassert>
#include "WMDRMModule.h"
#include <strsafe.h>
#include "WMPlaylist.h"
#include "../nu/AutoChar.h"
WinLAMPInterface winlamp;

extern WMDRM mod;
using namespace Nullsoft::Utility;

#ifndef NO_DRM
#include "vid_overlay.h"
#include "vid_ddraw.h"

OverlayVideoOutput overlay;
DDrawVideoOutput ddraw;
#endif


WinLAMPInterface::WinLAMPInterface()
		: videoWindow(0), bufferCount(0),
		statusGuard(GUARDNAME("WinLAMPInterface::statusGuard"))
{
	statusFilename[0] = 0;
	status[0] = 0;
}

/*
@returns winlamp's video window handle 
*/
HWND WinLAMPInterface::GetVideoWindow()
{
	return (HWND)GetVideoOutput()->extended(VIDUSER_GET_VIDEOHWND, 0, 0); // ask for the video hwnd
}

IVideoOutput *WinLAMPInterface::GetVideoOutput()
{
	if (!videoWindow)
		videoWindow = (IVideoOutput *)SendMessage(GetWinLAMPWindow(), WM_WA_IPC, 0, IPC_GET_IVIDEOOUTPUT);		// ask winlamp for an interface to the video output
	return videoWindow;

}

void WinLAMPInterface::EndOfFile()
{
	PostMessage(GetWinLAMPWindow(), WM_WA_MPEG_EOF, 0, 0);
}

HWND WinLAMPInterface::GetWinLAMPWindow()
{
	return plugin.hMainWindow;
}

void WinLAMPInterface::SetStatus(wchar_t *_status)
{
	{
		AutoLock lock (statusGuard);
		StringCchCopy(status, 1024, _status);
		StringCchCopy(statusFilename, FILENAME_SIZE, activePlaylist.GetFileName());
	}
	PostMessage(GetWinLAMPWindow(), WM_WA_IPC, 0, IPC_UPDTITLE);
}

bool WinLAMPInterface::GetStatus(wchar_t *title, size_t titleLen, const wchar_t *filename)
{
	AutoLock lock (statusGuard);

	if (status[0] && title && filename && !lstrcmpi(statusFilename, filename))
	{
		StringCchPrintf(title, titleLen, L"[%s]%s", status, filename);
		return true;
	}
	else
		return false;
}

bool WinLAMPInterface::GetStatusHook(wchar_t *title, size_t titleLen, const wchar_t *filename)
{
	AutoLock lock (statusGuard);

	if (status[0] && title && filename && !lstrcmpi(statusFilename, filename))
	{
		wchar_t *oldTitle = _wcsdup(title);
		StringCchPrintf(title, titleLen, L"[%s]%s", status, oldTitle);
		free(oldTitle);
		return true;
	}
	else
		return false;
}

bool WinLAMPInterface::HasStatus(const wchar_t *filename)
{
	AutoLock lock (statusGuard);
	if (status[0] && filename && !lstrcmpi(statusFilename, filename))
		return true;
	return false;
}

void WinLAMPInterface::ClearStatus()
{
	{
		//AutoLock lock (statusGuard); // should be safe not to lock here
		status[0] = 0;
		statusFilename[0]=0;
	}
	PostMessage(GetWinLAMPWindow(), WM_WA_IPC, 0, IPC_UPDTITLE);
}

void WinLAMPInterface::EncryptedDrawFrame(void *frame)
{
#ifndef NO_DRM
	overlay.SetFrame(frame);
	ddraw.SetFrame(frame);
	SecureZeroMemory(&frame, sizeof(void *));
	GetVideoOutput()->draw((void *)1);
#endif
}

bool WinLAMPInterface::OpenEncryptedVideo(int width, int height, bool flip, double aspect, int fourcc)
{
#ifndef NO_DRM
	VideoOpenStruct openVideo = {width, height, flip, aspect, fourcc};

	bool openedOK = false;
	if (config_video.overlays())
	{
		openedOK = !!GetVideoOutput()->extended(VIDUSER_OPENVIDEORENDERER, (intptr_t)(VideoRenderer *)&overlay, (intptr_t)&openVideo);
		if (openedOK)
			return true;
	}

	openedOK = !!GetVideoOutput()->extended(VIDUSER_OPENVIDEORENDERER, (intptr_t)(VideoRenderer *)&ddraw, (intptr_t)&openVideo);
  if (openedOK)
		return true;
#endif

	return false;
}

void WinLAMPInterface::CloseEncryptedVideo()
{
	GetVideoOutput()->extended(VIDUSER_CLOSEVIDEORENDERER, 0, 0);
}

void WinLAMPInterface::Buffering(int bufStatus, const wchar_t *displayString)
{
	char tempdata[75*2] = {0, };

	int csa = plugin.SAGetMode();
	if (csa & 1)
	{
		for (int x = 0; x < bufStatus*75 / 100; x ++)
			tempdata[x] = x * 16 / 75;
	}
	else if (csa&2)
	{
		int offs = (csa & 1) ? 75 : 0;
		int x = 0;
		while (x < bufStatus*75 / 100)
		{
			tempdata[offs + x++] = -6 + x * 14 / 75;
		}
		while (x < 75)
		{
			tempdata[offs + x++] = 0;
		}
	}
	else if (csa == 4)
	{
		tempdata[0] = tempdata[1] = (bufStatus * 127 / 100);
	}
	if (csa)	plugin.SAAdd(tempdata, ++bufferCount, (csa == 3) ? 0x80000003 : csa);

	wchar_t temp[64] = {0};
	StringCchPrintf(temp, 64, L"%s: %d%%",displayString, bufStatus);
	SetStatus(temp);
	//SetVideoStatusText(temp); // TODO: find a way to set the old status back
	GetVideoOutput()->notifyBufferState(static_cast<int>(bufStatus*2.55f));
}

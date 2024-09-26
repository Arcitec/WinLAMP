#include <precomp.h>
/*------------------------------------------------------------------------------------------------
 WinLAMP 2.9/5 frontend class
------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "main.h"
#include "wa2frontend.h"
#include "../winlamp/wa_ipc.h"
#include "../winlamp/ipc_pe.h"
#include "../gen_ml/ml.h"
#include "../gen_ml/main.h"
#include "../gen_hotkeys/wa_hotkeys.h"
#include "../nu/AutoChar.h"
#include "resource.h"
#include "../Agave/Language/api_language.h"

#define WINLAMP_EDIT_ID3                 40188


WinLAMP2FrontEnd wa2;

void WinLAMP2FrontEnd::init(HWND hwndParent)
{
	// find WinLAMP's HWND so we can talk to it
	hwnd_winlamp = hwndParent; //FindWindow("WinLAMP v1.x",NULL);

	// check that hwnd_winlamp isnt null and that we know about this specific ipc version
	getVersion();

	// more init
	enabled = 1;
	visible = 1;

	*(void **)&export_sa_get=(void*)SendMessageW(hwnd_winlamp,WM_WA_IPC,2,IPC_GETSADATAFUNC);
	*(void **)&export_sa_setreq=(void *)SendMessageW(hwnd_winlamp,WM_WA_IPC,1,IPC_GETSADATAFUNC);
	*(void **)&export_sa_get_deprecated=(void*)SendMessageW(hwnd_winlamp,WM_WA_IPC,0,IPC_GETSADATAFUNC);
	*(void **)&export_vu_get=(void*)SendMessageW(hwnd_winlamp,WM_WA_IPC,0,IPC_GETVUDATAFUNC);
	if (reinterpret_cast<intptr_t>(export_vu_get) == -1)
		export_vu_get=0;

	hwnd_playlist = getWnd(IPC_GETWND_PE);
}

//-----------------------------------------------------------------------------------------------
WinLAMP2FrontEnd::WinLAMP2FrontEnd()
{
	m_version = (char *)malloc(64);
	*m_version = 0;
	enabled = visible = foundvis =
    saved_video =
#ifdef MINIBROWSER_SUPPORT
	saved_mb =
#endif
	saved_pe = saved_eq = saved_main = 0;

	got_length_cache = 0;
	cached_length = -1;
	cached_length_time = 0;
	got_pos_cache = 0;
	cached_pos = -1;
	cached_pos_time = 0;

	video_ideal_height = -1;
	video_ideal_width = -1;

	hwnd_winlamp = hwnd_playlist = NULL;

	export_sa_get = 0;
	export_sa_setreq = 0;
	export_sa_get_deprecated = 0;
	export_vu_get = 0;
}

//-----------------------------------------------------------------------------------------------
WinLAMP2FrontEnd::~WinLAMP2FrontEnd()
{
	setWindowsVisible(1);
	free(m_version);
}

//-----------------------------------------------------------------------------------------------
const char *WinLAMP2FrontEnd::getVersion()
{
	if (hwnd_winlamp == NULL)
	{
		char err[16] = {0};
		MessageBoxA(NULL, WASABI_API_LNGSTRING(IDS_COULD_NOT_FIND_WINLAMP),
		           WASABI_API_LNGSTRING_BUF(IDS_ERROR,err,16), 0);
		m_version = "WinLAMP not found";
		return m_version;
	}

	if (hwnd_winlamp == NULL)
		return NULL;

	if (*m_version == 0)
	{
		// get version number
		int version = SendMessageW(hwnd_winlamp,WM_WA_IPC,0,IPC_GETVERSION);
		// check that we support this version of the ipc
		//assert(((version & 0xFF) >> 8) > 0x20);
		// format the version number into a string
		wsprintfA(m_version, "%d.%d%d", (version & 0xF000) >> 12, version & (0xF0 >> 4), version & 0xF);
	}
	return m_version;
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::userButton(int button, int modifier)
{
	if (!IsWindow(hwnd_winlamp)) return;
	int mod = 0;
	switch (modifier)
	{
	case WA2_USERBUTTONMOD_NONE : break;
	case WA2_USERBUTTONMOD_SHIFT: mod = 100; break;
	case WA2_USERBUTTONMOD_CTRL : mod = 110; break;
	}
	switch (button)
	{
	case WA2_USERBUTTON_PREV: SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_BUTTON1 + mod,0); break;
	case WA2_USERBUTTON_PLAY: SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_BUTTON2 + mod,0); break;
	case WA2_USERBUTTON_PAUSE: SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_BUTTON3 + mod,0); break;
	case WA2_USERBUTTON_STOP: SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_BUTTON4 + mod,0); break;
	case WA2_USERBUTTON_NEXT: SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_BUTTON5 + mod,0); break;
	}
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setOnTop(int ontop)
{
	if (!!ontop == !!isOnTop()) return;
	toggleOnTop();
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::toggleOnTop()
{
	SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_OPTIONS_AOT, 0);
}


//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::enqueueFile(const wchar_t *file)
{
	if (!IsWindow(hwnd_winlamp)) return;
	COPYDATASTRUCT cds;
	cds.dwData = IPC_PLAYFILEW;
	cds.lpData = (void *)file;
	cds.cbData = sizeof(wchar_t) * (wcslen(file)+1);
	SendMessageW(hwnd_winlamp,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cds);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isPlaying()
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,0,IPC_ISPLAYING) == 1;
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isPaused()
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,0,IPC_ISPLAYING) == 3;
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isStopped()
{
	if (!IsWindow(hwnd_winlamp)) return 1;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,0,IPC_ISPLAYING) == 0;
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getPosition()
{
	if ( !IsWindow( hwnd_winlamp ) )
		return 0;

	if ( got_pos_cache && GetTickCount() < cached_pos_time + 20 )
		return cached_pos;

	got_pos_cache = 1;
	cached_pos_time = GetTickCount();

	return cached_pos = SendMessageW( hwnd_winlamp, WM_WA_IPC, 0, IPC_GETOUTPUTTIME );
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getLength()
{
	if ( got_length_cache && GetTickCount() < cached_length_time + 3000 )
		return cached_length;

	if ( !IsWindow( hwnd_winlamp ) )
		return 0;

	int l = SendMessageW( hwnd_winlamp, WM_WA_IPC, 1, IPC_GETOUTPUTTIME );

	if ( l == -1 )
		cached_length = -1;
	else
		cached_length = l * 1000;

	got_length_cache = 1;
	cached_length_time = GetTickCount();

	return cached_length;
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::seekTo(int ms)
{
	got_pos_cache = 0;
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,ms,IPC_JUMPTOTIME);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setVolume(int v)
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp,WM_WA_IPC,v,IPC_SETVOLUME);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getVolume()
{
	if (!IsWindow(hwnd_winlamp)) return 255;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)-666,IPC_SETVOLUME);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setPanning(int p)
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)p,IPC_SETPANNING);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getPanning()
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)-666,IPC_SETPANNING);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getInfo( int what )
{
	if ( !IsWindow( hwnd_winlamp ) )
		return 0;

	return SendMessageW( hwnd_winlamp, WM_WA_IPC, (WPARAM)what, IPC_GETINFO );
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getEqData(int what)
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)what,IPC_GETEQDATA);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setEqData(int what, int val)
{
	if (!IsWindow(hwnd_winlamp)) return;
	//SendMessageW(hwnd_winlamp,WM_WA_IPC,what,IPC_GETEQDATA); // f this :)
	SendMessageW(hwnd_winlamp,WM_WA_IPC,0xDB000000 | ((what&0xFF) << 16) | (val&0xFFFF),IPC_SETEQDATA);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getShuffle()
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)0,IPC_GET_SHUFFLE);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getRepeat()
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)0,IPC_GET_REPEAT);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setShuffle(int shuffle)
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)shuffle,IPC_SET_SHUFFLE);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setRepeat(int repeat)
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)repeat,IPC_SET_REPEAT);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setManualPlaylistAdvance(int manual)
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)manual,IPC_SET_MANUALPLADVANCE);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getManualPlaylistAdvance()
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)0,IPC_GET_MANUALPLADVANCE);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::enableWindows( int enabled )
{
	if ( !IsWindow( hwnd_winlamp ) )
		return;

	SendMessageW( hwnd_winlamp, WM_WA_IPC, enabled ? 0 : 0xdeadbeef, IPC_ENABLEDISABLE_ALL_WINDOWS );
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::areWindowsEnabled()
{
	if ( !IsWindow( hwnd_winlamp ) )
		return 1;

	return 1; // todo !!
}

//-----------------------------------------------------------------------------------------------
int _IsWindowVisible( HWND w )
{
	if ( !IsWindowVisible( w ) )
		return 0;

	RECT r;
	GetWindowRect( w, &r );

	return !( r.left >= 3000 && r.top >= 3000 );
}

int WinLAMP2FrontEnd::isMainWindowVisible()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)0, IPC_ISMAINWNDVISIBLE);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setWindowsVisible(int v)
{
	if (visible == v)
		return;

	if (!IsWindow(hwnd_winlamp))
		return;

	if (v)
	{
		if (saved_main && ! isMainWindowVisible())
			SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_MAIN_WINDOW, 0);
		if (saved_eq && !isWindowVisible(IPC_GETWND_EQ))
			SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_OPTIONS_EQ, 0);
#ifdef MINIBROWSER_SUPPORT
		if (saved_mb && !isWindowVisible(IPC_GETWND_MB))
			SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_OPTIONS_MINIBROWSER, 0);
#endif
		if (saved_pe && !isWindowVisible(IPC_GETWND_PE))
			SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_OPTIONS_PLEDIT, 0);
		if (saved_video && !isWindowVisible(IPC_GETWND_VIDEO))
			SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_OPTIONS_VIDEO, 0);
	}
	else
	{
		if (isMainWindowVisible())
		{
			saved_main = 1;
			SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_MAIN_WINDOW, 0);
		}
		else
			saved_main = 0;

		if (isWindowVisible(IPC_GETWND_EQ))
		{
			saved_eq = 1;
			SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_OPTIONS_EQ, 0);
		}
		else
			saved_eq = 0;

#ifdef MINIBROWSER_SUPPORT
		if (isWindowVisible(IPC_GETWND_MB))
		{
			saved_mb = 1;
			SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_OPTIONS_MINIBROWSER, 0);
		}
		else saved_mb = 0;
#endif
		if (isWindowVisible(IPC_GETWND_PE))
		{
			saved_pe = 1;
			SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_OPTIONS_PLEDIT, 0);
		}
		else
			saved_pe = 0;

		if (isWindowVisible(IPC_GETWND_VIDEO))
		{
			saved_video = 1;
			SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_OPTIONS_VIDEO, 0);
		}
		else saved_video = 0;
	}
	visible = v;
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::areWindowsVisible()
{
	if (!IsWindow(hwnd_winlamp))
		return 1;

	return visible;
}

//-----------------------------------------------------------------------------------------------
HWND WinLAMP2FrontEnd::getWnd(int wnd)
{
	if (!IsWindow(hwnd_winlamp))
		return NULL;

	return (HWND)SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)wnd,IPC_GETWND);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getPlaylistLength()
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)0,IPC_GETLISTLENGTH);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getCurPlaylistEntry()
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)0,IPC_GETLISTPOS);
}

//-----------------------------------------------------------------------------------------------
const wchar_t *WinLAMP2FrontEnd::getTitle(int plentry)
{
	if (!IsWindow(hwnd_winlamp)) return NULL;
	return (const wchar_t *)SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)plentry,IPC_GETPLAYLISTTITLEW);
}

//-----------------------------------------------------------------------------------------------
const char *WinLAMP2FrontEnd::getFile(int plentry)
{
	if (!IsWindow(hwnd_winlamp)) return NULL;
	return (const char *)SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)plentry,IPC_GETPLAYLISTFILE);
}

const wchar_t *WinLAMP2FrontEnd::getFileW(int plentry)
{
	if (!IsWindow(hwnd_winlamp)) return NULL;
	return (const wchar_t *)SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)plentry,IPC_GETPLAYLISTFILEW);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::playAudioCD(int cd)
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp,WM_COMMAND,ID_MAIN_PLAY_AUDIOCD1+cd,0);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::openFileDialog(HWND w)
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)w, IPC_OPENFILEBOX);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::openUrlDialog(HWND w)
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_BUTTON2_CTRL, 0);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::openDirectoryDialog(HWND w)
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)w, IPC_OPENDIRBOX);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::ejectPopupMenu()
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)0, IPC_SPAWNBUTTONPOPUP);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::previousPopupMenu()
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)1, IPC_SPAWNBUTTONPOPUP);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::nextPopupMenu()
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)2, IPC_SPAWNBUTTONPOPUP);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::pausePopupMenu()
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)3, IPC_SPAWNBUTTONPOPUP);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::playPopupMenu()
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)4, IPC_SPAWNBUTTONPOPUP);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::stopPopupMenu()
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)5, IPC_SPAWNBUTTONPOPUP);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setDialogBoxParent(HWND w)
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)w, IPC_SETDIALOGBOXPARENT);
}

void WinLAMP2FrontEnd::updateDialogBoxParent(HWND w)
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)w, IPC_UPDATEDIALOGBOXPARENT);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isOnTop()
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_IS_AOT);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::triggerPopupMenu(int x, int y)
{
	if (!IsWindow(hwnd_winlamp)) return;
	HMENU hMenu = (HMENU)SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)0,IPC_GET_HMENU);
	DoTrackPopup(hMenu, TPM_RIGHTBUTTON, x, y, hwnd_winlamp);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::triggerEQPresetMenu(int x, int y)
{
	waSpawnMenuParms p = {hwnd_winlamp, x, y};
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNEQPRESETMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerFileMenu(int x, int y, int width, int height)
{
	waSpawnMenuParms2 p = {hwnd_winlamp, x, y, width, height};
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNFILEMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerPlayMenu(int x, int y, int width, int height)
{
	waSpawnMenuParms2 p = {hwnd_winlamp, x, y, width, height};
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNPLAYMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerOptionsMenu(int x, int y, int width, int height)
{
	waSpawnMenuParms2 p = {hwnd_winlamp, x, y, width, height};
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNOPTIONSMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerWindowsMenu(int x, int y, int width, int height)
{
	waSpawnMenuParms2 p = {hwnd_winlamp, x, y, width, height};
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNWINDOWSMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerHelpMenu(int x, int y, int width, int height)
{
	waSpawnMenuParms2 p = {hwnd_winlamp, x, y, width, height};
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNHELPMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerPEFileMenu(int x, int y, int width, int height)
{
	int IPC_GETMLWINDOW=SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)&"LibraryGetWnd",IPC_REGISTER_WINLAMP_IPCMESSAGE);
	if (IPC_GETMLWINDOW > 65536) SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)-1, IPC_GETMLWINDOW);
	waSpawnMenuParms2 p = {hwnd_winlamp, x, y, width, height};
	p.wnd = getWnd(IPC_GETWND_PE);
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNPEFILEMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerPEPlaylistMenu(int x, int y, int width, int height)
{
	waSpawnMenuParms2 p = {hwnd_winlamp, x, y, width, height};
	p.wnd = getWnd(IPC_GETWND_PE);
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNPEPLAYLISTMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerPESortMenu(int x, int y, int width, int height)
{
	waSpawnMenuParms2 p = {hwnd_winlamp, x, y, width, height};
	p.wnd = getWnd(IPC_GETWND_PE);
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNPESORTMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerPEHelpMenu(int x, int y, int width, int height)
{
	waSpawnMenuParms2 p = {hwnd_winlamp, x, y, width, height};
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNPEHELPMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerPEListOfListsMenu(int x, int y)
{
	int IPC_GETMLWINDOW=SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)&"LibraryGetWnd",IPC_REGISTER_WINLAMP_IPCMESSAGE);
	if (IPC_GETMLWINDOW > 65536) SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)-1, IPC_GETMLWINDOW);
	waSpawnMenuParms p = {hwnd_winlamp, x, y};
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNPELISTOFPLAYLISTS);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerMLFileMenu(int x, int y, int width, int height)
{
	waSpawnMenuParms2 p = {hwnd_winlamp, x, y, width, height};
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNMLFILEMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerMLViewMenu(int x, int y, int width, int height)
{
	waSpawnMenuParms2 p = {hwnd_winlamp, x, y, width, height};
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNMLVIEWMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::triggerMLHelpMenu(int x, int y, int width, int height)
{
	waSpawnMenuParms2 p = {hwnd_winlamp, x, y, width, height};
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&p, IPC_SPAWNMLHELPMENU);
}

//-----------------------------------------------------------------------------------------------
HMENU WinLAMP2FrontEnd::getPopupMenu()
{
	if (!IsWindow(hwnd_winlamp)) return NULL;
	return (HMENU)SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)0,IPC_GET_HMENU);
}

//-----------------------------------------------------------------------------------------------
HMENU WinLAMP2FrontEnd::getTopMenu()
{
	if (!IsWindow(hwnd_winlamp)) return NULL;
	return (HMENU)SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)-1,IPC_GET_HMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::adjustOptionsPopupMenu(int direction)
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)direction,IPC_ADJUST_OPTIONSMENUPOS);
}

//-----------------------------------------------------------------------------------------------
HMENU WinLAMP2FrontEnd::getMenuBarMenu(int which)
{
	if (!IsWindow(hwnd_winlamp)) return NULL;
	return (HMENU)SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)which+1,IPC_GET_HMENU);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::adjustFFWindowsMenu(int direction)
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)direction,IPC_ADJUST_FFWINDOWSMENUPOS);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::adjustFFOptionsMenu(int direction)
{
	if (!IsWindow(hwnd_winlamp)) return 0;
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)direction,IPC_ADJUST_FFOPTIONSMENUPOS);
}

//-----------------------------------------------------------------------------------------------
HWND WinLAMP2FrontEnd::getMainWindow()
{
	return hwnd_winlamp;
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::quit()
{
	if (!IsWindow(hwnd_winlamp)) return;
	SendMessageW(hwnd_winlamp,WM_CLOSE,0,0);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isWindowVisible(intptr_t which)
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, which, IPC_ISWNDVISIBLE);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setWindowVisible(intptr_t which, int visible)
{
	int state = isWindowVisible(which);
	if (!state == !visible) return;
	int id = 0;
	switch (which)
	{
	case IPC_GETWND_EQ:
		DebugStringW(L"Showing EQ!!\n");
		id = WINLAMP_OPTIONS_EQ;
		break;
	case IPC_GETWND_PE:
		id = WINLAMP_OPTIONS_PLEDIT;
		break;
#ifdef MINIBROWSER_SUPPORT
	case IPC_GETWND_MB:
		id = WINLAMP_OPTIONS_MINIBROWSER;
		break;
#endif
	case IPC_GETWND_VIDEO:
		id = WINLAMP_OPTIONS_VIDEO;
		break;
	}
	SendMessageW(hwnd_winlamp, WM_COMMAND, id, 0);
}


//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::sendPlCmd(int which, int x, int y, int menu_align_flag)
{
	windowCommand wc = {which, x, y, menu_align_flag};
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (intptr_t)&wc, IPC_PLCMD);
}

#ifdef MINIBROWSER_SUPPORT
//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::sendMbCmd(int which, int x, int y, int menu_align_flag)
{
	windowCommand wc = {which, x, y, menu_align_flag};
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (intptr_t)&wc, IPC_MBCMD);
}
#endif

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::sendVidCmd(int which, int x, int y, int menu_align_flag)
{
	windowCommand wc = {which, x, y, menu_align_flag};
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (intptr_t)&wc, IPC_VIDCMD);
}


//-----------------------------------------------------------------------------------------------
#define WINLAMP_VISPLUGIN                40192
void WinLAMP2FrontEnd::toggleVis()
{
	SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_VISPLUGIN, 0);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isVisRunning()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_ISVISRUNNING);
}

//-----------------------------------------------------------------------------------------------
HWND WinLAMP2FrontEnd::getVisWnd()
{
	return (HWND)SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_GETVISWND);
}

//-----------------------------------------------------------------------------------------------
IDropTarget *WinLAMP2FrontEnd::getDropTarget()
{
	return (IDropTarget *)SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_GETDROPTARGET);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getSamplerate()
{
	int realRate = SendMessageW(hwnd_winlamp,WM_WA_IPC,5,IPC_GETINFO);
	if (realRate == 1)
		return 1000*SendMessageW(hwnd_winlamp,WM_WA_IPC,0,IPC_GETINFO);
	else
		return realRate;
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getBitrate()
{
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,1,IPC_GETINFO);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getChannels()
{
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,2,IPC_GETINFO);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isValidEmbedWndState(embedWindowState *ws)
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, (intptr_t)ws, IPC_EMBED_ISVALID);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::PE_getNumItems()
{
	return SendMessageW(hwnd_playlist, WM_USER, IPC_PE_GETINDEXTOTAL, 0);
}

//-----------------------------------------------------------------------------------------------
fileinfo2 *WinLAMP2FrontEnd::PE_getFileTitle(int index)
{
	static fileinfo2 fi;
	fi.fileindex = index;
	*(fi.filetitle) = 0;
	*(fi.filelength) = 0;
	SendMessageW(hwnd_playlist, WM_USER, IPC_PE_GETINDEXTITLE, (LPARAM)&fi);
	return &fi;
}

//-----------------------------------------------------------------------------------------------
fileinfo2W *WinLAMP2FrontEnd::PE_getFileTitleW(int index)
{
	static fileinfo2W fi;
	fi.fileindex = index;
	*(fi.filetitle) = 0;
	*(fi.filelength) = 0;
	SendMessageW(hwnd_playlist, WM_USER, IPC_PE_GETINDEXTITLEW, (LPARAM)&fi);
	return &fi;
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::PE_getCurrentIndex()
{
	//return SendMessageW(hwnd_playlist, WM_USER, IPC_PE_GETCURINDEX, 0);
	return SendMessageW(hwnd_winlamp, WM_USER, 0, IPC_GETLISTPOS);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::PE_setCurrentIndex(int i)
{
	SendMessageW(hwnd_winlamp, WM_WA_IPC, i, IPC_SETPLAYLISTPOS);
}

//-----------------------------------------------------------------------------------------------
HWND WinLAMP2FrontEnd::getMediaLibrary()
{
	int IPC_GETMLWINDOW=SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)&"LibraryGetWnd",IPC_REGISTER_WINLAMP_IPCMESSAGE);
	return IPC_GETMLWINDOW > 65536 ? (HWND)SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_GETMLWINDOW) : NULL;
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::ensureMediaLibraryLoaded()
{
	int IPC_GETMLWINDOW=SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)&"LibraryGetWnd",IPC_REGISTER_WINLAMP_IPCMESSAGE);
	if (IPC_GETMLWINDOW > 65536) SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)-1, IPC_GETMLWINDOW);
}


//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::hasVideoSupport()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_HAS_VIDEO_SUPPORT);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isPlayingVideo()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_IS_PLAYING_VIDEO) > 0;
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isPlayingVideoFullscreen()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_IS_FULLSCREEN);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isDoubleSize()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_ISDOUBLESIZE);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getTimeDisplayMode()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_GETTIMEDISPLAYMODE);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::switchSkin(const wchar_t *skinname)
{
	static StringW wideSkinName;
	wideSkinName=skinname;
	PostMessage(hwnd_winlamp, WM_WA_IPC, (intptr_t)wideSkinName.getValue(), IPC_SETSKINW);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::visNext()
{
	PostMessage(hwnd_winlamp, WM_COMMAND, ID_VIS_NEXT, 0);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::visFullscreen()
{
	PostMessage(hwnd_winlamp, WM_COMMAND, ID_VIS_FS, 0);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::visPrev()
{
	PostMessage(hwnd_winlamp, WM_COMMAND, ID_VIS_PREV, 0);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::visRandom(int set)
{
	PostMessage(hwnd_winlamp, WM_COMMAND, ID_VIS_RANDOM | (set << 16), 0);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::pollVisRandom()
{
	PostMessage(hwnd_winlamp, WM_COMMAND, ID_VIS_RANDOM | 0xFFFF0000, 0);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::visConfig()
{
	PostMessage(hwnd_winlamp, WM_COMMAND, ID_VIS_CFG, 0);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::visMenu()
{
	PostMessage(hwnd_winlamp, WM_COMMAND, ID_VIS_MENU, 0);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::getIdealVideoSize(int *w, int *h)
{
	if (w) *w = video_ideal_width;
	if (h) *h = video_ideal_height;
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getStopOnVideoClose()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_GETSTOPONVIDEOCLOSE);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setStopOnVideoClose(int stop)
{
	SendMessageW(hwnd_winlamp, WM_WA_IPC, stop, IPC_SETSTOPONVIDEOCLOSE);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::GetVideoResize()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_GETVIDEORESIZE);
}


void WinLAMP2FrontEnd::SetVideoResize(int stop)
{
	SendMessageW(hwnd_winlamp, WM_WA_IPC, stop, IPC_SETVIDEORESIZE);
}


//-----------------------------------------------------------------------------------------------
BOOL CALLBACK findVisWndProc(HWND hwnd, LPARAM lParam)
{
	WinLAMP2FrontEnd *fe = (WinLAMP2FrontEnd*)lParam;
	if (hwnd == fe->getVisWnd())
	{
		fe->setFoundVis(); return FALSE;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isVis(HWND hwnd)
{
	if (hwnd == wa2.getVisWnd()) return 1;
	foundvis = 0;
	EnumChildWindows(hwnd, findVisWndProc, (LPARAM)this);
	return foundvis;
}

//-----------------------------------------------------------------------------------------------
HWND WinLAMP2FrontEnd::getPreferencesWindow()
{
	return (HWND)SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_GETPREFSWND);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setPlEditWidthHeight(int width, int height)
{
	POINT pt={width, height};
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&pt, IPC_SET_PE_WIDTHHEIGHT);
}

//-----------------------------------------------------------------------------------------------
HINSTANCE WinLAMP2FrontEnd::getLanguagePackInstance()
{
	return (HINSTANCE)SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_GETLANGUAGEPACKINSTANCE);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::openTrackInfo()
{
	SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_EDIT_ID3, 0);
}

//-----------------------------------------------------------------------------------------------
const char *WinLAMP2FrontEnd::getOutputPlugin()
{
	return (const char *)SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_GETOUTPUTPLUGIN);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::setDrawBorders(int d)
{
	SendMessageW(hwnd_winlamp, WM_WA_IPC, d, IPC_SETDRAWBORDERS);
}

//-----------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::disableSkinnedCursors(int disable)
{
	SendMessageW(hwnd_winlamp, WM_WA_IPC, disable, IPC_DISABLESKINCURSORS);
}

//-----------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getMetaData(const wchar_t *filename, const wchar_t *name, wchar_t *data, int data_len)
{

	if (!_wcsnicmp(filename, L"file://", 7))
		filename+=7;
	extendedFileInfoStructW efis=
	{
		filename,
		name,
		data,
		data_len,
	};
	return SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)&efis,IPC_GET_EXTENDED_FILE_INFOW_HOOKABLE);
}
//------------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::invalidateCache()
{
	got_length_cache = 0;
	got_pos_cache = 0;
}

//------------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::registerGlobalHotkey(const char *name, int msg, int wparam, int lparam, int flags, const char *id)
{
	int m_genhotkeys_add_ipc=SendMessageW(hwnd_winlamp,WM_WA_IPC,(WPARAM)&"GenHotkeysAdd",IPC_REGISTER_WINLAMP_IPCMESSAGE);
	genHotkeysAddStruct hs={0,};
	hs.name = (char *)name;
	hs.uMsg = msg;
	hs.wParam = wparam;
	hs.lParam = lparam;
	hs.flags = flags;
	hs.id = (char *)id;
	if (m_genhotkeys_add_ipc > 65536) SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&hs, m_genhotkeys_add_ipc);
}

//------------------------------------------------------------------------------------------------
const char *WinLAMP2FrontEnd::getVideoInfoString()
{
	return (const char *)SendMessageW(hwnd_winlamp, WM_WA_IPC, 4, IPC_GETINFO);
}

//------------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::playFile(const wchar_t *file)
{
	SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_DELETE);
	COPYDATASTRUCT cds;
	cds.dwData = IPC_PLAYFILEW;
	cds.lpData = (void *)file;
	cds.cbData = sizeof(wchar_t) * (wcslen(file)+1);  // +1 to get the NULL, missing forever
	SendMessageW(hwnd_winlamp, WM_COPYDATA, (WPARAM)hwnd_winlamp, (LPARAM)&cds);
	SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_STARTPLAY);
}

void WinLAMP2FrontEnd::clearPlaylist()
{
	SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_DELETE);
}

//------------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::rewind5s()
{
	SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_REW5S, 0);
}

//------------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::forward5s()
{
	SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_FFWD5S, 0);
}

//------------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::endOfPlaylist()
{
	SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_BUTTON5_CTRL, 0);
}

//------------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::startOfPlaylist()
{
	SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_BUTTON1_CTRL, 0);
}

//------------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::stopWithFade()
{
	SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_BUTTON4_SHIFT, 0);
}

//------------------------------------------------------------------------------------------------
void WinLAMP2FrontEnd::stopAfterCurrent()
{
	SendMessageW(hwnd_winlamp, WM_COMMAND, WINLAMP_BUTTON4_CTRL, 0);
}

//------------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isWindowShade(int whichwnd)
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, whichwnd, IPC_IS_WNDSHADE);
}

//------------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::getCurTrackRating()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_GETRATING);
}

void WinLAMP2FrontEnd::setCurTrackRating(int rating)
{
	SendMessageW(hwnd_winlamp, WM_WA_IPC, rating, IPC_SETRATING);
}

//------------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::isExitEnabled()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_IS_EXIT_ENABLED);
}

//------------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::pushExitDisabled()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_PUSH_DISABLE_EXIT);
}

//------------------------------------------------------------------------------------------------
int WinLAMP2FrontEnd::popExitDisabled()
{
	return SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_POP_DISABLE_EXIT);
}

void WinLAMP2FrontEnd::GetFileInfo(const wchar_t *filename, wchar_t *title, int titleCch, int *length)
{
	basicFileInfoStructW infoStruct = {0};
	infoStruct.filename = filename;
	infoStruct.title = title;
	infoStruct.titlelen = titleCch;
	SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&infoStruct, IPC_GET_BASIC_FILE_INFOW);
	*length = infoStruct.length;
}

const wchar_t *WinLAMP2FrontEnd::GetCurrentTitle()
{
	return (const wchar_t *)SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_GET_PLAYING_TITLE);
}

const wchar_t *WinLAMP2FrontEnd::GetCurrentFile()
{
	return (const wchar_t *)SendMessageW(hwnd_winlamp, WM_WA_IPC, 0, IPC_GET_PLAYING_FILENAME);
}

void *WinLAMP2FrontEnd::CanPlay(const wchar_t *fn)
{
	return (void *)SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)fn, IPC_CANPLAY);
}

bool WinLAMP2FrontEnd::IsPlaylist(const wchar_t *fn)
{
	return AGAVE_API_PLAYLISTMANAGER->CanLoad(fn);
}



HWND GetMainContainerHWND();
bool WinLAMP2FrontEnd::GetAlbumArt(const wchar_t *filename)
{
	// disabled 30 May 2012 as per email from Tejas w.r.t. to Rovi deal ending
	#if 0
	if (!_wcsnicmp(filename, L"file://", 7))
		filename+=7;

	artFetchData fetch = {sizeof(artFetchData), GetMainContainerHWND(), };

	

	wchar_t artist[512]=L"", album[512]=L"", gracenoteFileId[512]=L"", year[5]=L"";
		getMetaData(filename, L"albumartist", artist, 512);
		if (!WCSICMP(artist, L""))		//Add artist if albumartist not available
			getMetaData(filename, L"artist", artist, 512);
		getMetaData(filename, L"album", album, 512);
		getMetaData(filename, L"GracenoteFileID", gracenoteFileId, 512);
		getMetaData(filename, L"year", year, 5);

		fetch.artist=artist;
		fetch.album=album;
		fetch.gracenoteFileId=gracenoteFileId;
		fetch.year=_wtoi(year);

	int r = SendMessageW(hwnd_winlamp, WM_WA_IPC, (WPARAM)&fetch, IPC_FETCH_ALBUMART);
	if(r == 0 && fetch.imgData && fetch.imgDataLen) // success, save art in correct location
	{
		AGAVE_API_ALBUMART->SetAlbumArt(filename,L"cover",0,0,fetch.imgData,fetch.imgDataLen,fetch.type);
		WASABI_API_MEMMGR->sysFree(fetch.imgData);
		return true;
	}
	#endif
	return false;
}

static void code(long* v, long* k)
{
	unsigned long y = v[0], z = v[1], sum = 0,         /* set up */
		delta = 0x9e3779b9, n = 32 ;  /* key schedule constant*/

	while (n-- > 0)
	{                                       /* basic cycle start */
		sum += delta;
		y += ((z << 4) + k[0]) ^(z + sum) ^((z >> 5) + k[1]);
		z += ((y << 4) + k[2]) ^(y + sum) ^((y >> 5) + k[3]); /* end cycle */
	}
	v[0] = y; v[1] = z;

}

static int getRegVer()
{
	int *x = (int*)malloc(32);
	long s[3];
	long ss[2] = {GetTickCount(), (int)x + (int)s}; // fucko: better challenge, too
	long tealike_key[4] = { 31337, 0xf00d, 0xdead, 0xbeef}; //fucko: hide this a bit
	free(x);
	s[0] = ss[0];
	s[1] = ss[1];
	s[2] = 0;

	SendMessageW(plugin.hwndParent, WM_WA_IPC, (WPARAM)s, IPC_GETREGISTEREDVERSION);

	code(ss, tealike_key);

	if (memcmp(s, ss, 8)) return 0;

	return s[2];
}

bool WinLAMP2FrontEnd::IsWinLAMPPro()
{
	return !!getRegVer();
}

void WinLAMP2FrontEnd::openUrl(const wchar_t *url)
{
	SendMessageW(plugin.hwndParent, WM_WA_IPC, (WPARAM)url, IPC_OPEN_URL);
}

#include "../gen_ml/menu.h"

LRESULT sendMlIpc(int msg, WPARAM param)
{
	static LRESULT IPC_GETMLWINDOW;
	if (!IPC_GETMLWINDOW) IPC_GETMLWINDOW = SendMessageW(plugin.hwndParent, WM_WA_IPC, (WPARAM)&"LibraryGetWnd", IPC_REGISTER_WINLAMP_IPCMESSAGE);
	HWND mlwnd = (HWND)SendMessageW(plugin.hwndParent, WM_WA_IPC, 0, IPC_GETMLWINDOW);

	if (param == 0 && msg == 0) return (LRESULT)mlwnd;

	if (IsWindow(mlwnd))
		return SendMessageW(mlwnd, WM_ML_IPC, param, msg);

	return 0;
}

BOOL DoTrackPopup(HMENU hMenu, UINT fuFlags, int x, int y, HWND hwnd)
{
	if(hMenu == NULL)
	{
		return NULL;
	}

	HWND ml_wnd = (HWND)sendMlIpc(0, 0);
	if (IsWindow(ml_wnd))
	{
		return Menu_TrackPopup(ml_wnd, hMenu, fuFlags, x, y, hwnd, NULL);
	}
	else
	{ 
		return TrackPopupMenu(hMenu, fuFlags, x, y, 0, hwnd, NULL);
	}
}
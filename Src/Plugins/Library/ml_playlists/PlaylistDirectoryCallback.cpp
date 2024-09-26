#include "main.h"
#include "PlaylistDirectoryCallback.h"

PlaylistDirectoryCallback::PlaylistDirectoryCallback(const char *_extlist, const char *winlampIni)
		: extlist(_extlist), recurse(true)
{
	if (winlampIni)
	{
		int rofiob = GetPrivateProfileIntA("winlamp", "rofiob", 1, winlampIni);
		recurse = (rofiob & 2) ? false : true;
	}
}

bool PlaylistDirectoryCallback::ShouldRecurse(const wchar_t *path)
{
	return recurse;
}

bool PlaylistDirectoryCallback::ShouldLoad(const wchar_t *filename)
{
	const wchar_t *ext = PathFindExtensionW(filename);
	if (!*ext)
		return false;

	ext++;

	const char *a = extlist;
	while (a && *a)
	{
		if (!lstrcmpiW(AutoWide(a), ext))
			return true;
		a += lstrlenA(a) + 1;
	}
	return false;
}

#define CBCLASS PlaylistDirectoryCallback
START_DISPATCH;
CB( IFC_PLAYLISTDIRECTORYCALLBACK_SHOULDRECURSE, ShouldRecurse )
CB( IFC_PLAYLISTDIRECTORYCALLBACK_SHOULDLOAD, ShouldLoad )
END_DISPATCH;

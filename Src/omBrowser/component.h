#ifndef NULLSOFT_WINLAMP_OMBROWSER_COMPONENT_HEADER
#define NULLSOFT_WINLAMP_OMBROWSER_COMPONENT_HEADER

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <wtypes.h>
#include "../Agave/Component/ifc_wa5component.h"
#include "./ifc_winlamphook.h"
#include <bfc/multipatch.h>
#include <vector>

class WasabiHelper;
class WinLAMPHook;
class SkinHelper;
class ifc_wasabihelper;
class ifc_skinhelper;
class ifc_ombrowserclass;
class InternetFeatures;

#define MPIID_WA5COMPONENT		10
#define MPIID_WINLAMPHOOK		20

class OmBrowserComponent : public MultiPatch<MPIID_WA5COMPONENT, ifc_wa5component>,
							public MultiPatch<MPIID_WINLAMPHOOK, ifc_winlamphook>
{
public:
	OmBrowserComponent();
	~OmBrowserComponent();

public:
	/* Dispatchable */
	size_t AddRef();
	size_t Release();
	int QueryInterface(GUID interface_guid, void **object);

	/* ifc_wa5component */
	void RegisterServices(api_service *service);
	int RegisterServicesSafeModeOk();
	void DeregisterServices(api_service *service);
	
	/* ifc_winlamphook (partial) */
	HRESULT ResetFont(void);
	HRESULT SkinChanged(const wchar_t *skinName);
	HRESULT SkinColorChange(const wchar_t *colorTheme);

public:
	HRESULT InitializeComponent(HWND hwndWinLAMP);
	HRESULT GetWasabiHelper(ifc_wasabihelper **wasabiOut);
	HRESULT GetSkinHelper(ifc_skinhelper **skinOut);
	HRESULT RegisterWinLAMPHook(ifc_winlamphook *hook, UINT *cookieOut);
	HRESULT UnregisterWinLAMPHook(UINT cookie);
	HRESULT GetWinLAMPWnd(HWND *hwndWinLAMP);
	HRESULT RegisterUnloadCallback(PLUGINUNLOADCALLBACK callback);
	HRESULT GetBrowserClass(LPCWSTR pszName, ifc_ombrowserclass **instance);
	HRESULT UnregisterBrowserClass(LPCWSTR pszName);

protected:
	void ReleaseServices(void);
	void UpdateColors(void);
	void SetInternetFeautures(void);
	BOOL SetUserAgent(void);

protected:
	RECVS_MULTIPATCH;

private:
	typedef std::vector<PLUGINUNLOADCALLBACK> UnloadCallbackList;
	typedef std::vector<ifc_ombrowserclass*> BrowserClassList;


private:
	WasabiHelper *wasabiHelper;
	WinLAMPHook *winlampHook;
	SkinHelper *skinHelper;
	UINT hookCookie;
	CRITICAL_SECTION lock;
	UnloadCallbackList unloadCallbacks;
	BrowserClassList browserClasses;
	InternetFeatures *internetFeatures;
};

#endif //NULLSOFT_WINLAMP_OMBROWSER_COMPONENT_HEADER
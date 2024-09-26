#include "main.h"
#include "./winlampApi.h"


WinLAMPApi::WinLAMPApi()
{}
WinLAMPApi::~WinLAMPApi()
{}

HRESULT WinLAMPApi::CreateInstance(WinLAMPApi **instance)
{
	if (NULL == instance) return E_POINTER;
	*instance = new WinLAMPApi();
	if (NULL == *instance) return E_OUTOFMEMORY;
	return S_OK;
}

const char *WinLAMPApi::getServiceName() 
{ 
	return "WinLAMP API"; 
}

const GUID WinLAMPApi::getServiceGuid()
{ 
	return winlampApiGuid; 
}

size_t WinLAMPApi::AddRef()
{
	return _ref.fetch_add( 1 );
}

size_t WinLAMPApi::Release()
{
	if ( _ref.load() == 0 )
		return _ref.load();
	
	LONG r = _ref.fetch_sub( 1 );
	if (0 == r)
		delete(this);
	
	return r;
}

int WinLAMPApi::QueryInterface(GUID interface_guid, void **object)
{
	if (NULL == object) return E_POINTER;
	*object = NULL;
	return E_NOINTERFACE;
}

HWND WinLAMPApi::GetMainWindow(void)
{
	return hMainWindow;
}

HWND WinLAMPApi::GetDlgParent(void)
{
	return (g_dialog_box_parent) ? g_dialog_box_parent : hMainWindow;
}

HRESULT WinLAMPApi::OpenUrl(HWND hwnd, const wchar_t *url)
{
	myOpenURL(hwnd, (wchar_t*)url);
	return S_OK;
}

int WinLAMPApi::GetRegVer()
{
	return 2;
}

#define CBCLASS WinLAMPApi
START_DISPATCH;
CB(ADDREF, AddRef)
CB(RELEASE, Release)
CB(QUERYINTERFACE, QueryInterface)
CB(API_GETMAINWINDOW, GetMainWindow)
CB(API_GETDLGPARENT, GetDlgParent)
CB(API_OPENURL, OpenUrl)
CB(API_GETREGVER, GetRegVer)
END_DISPATCH;
#undef CBCLASS

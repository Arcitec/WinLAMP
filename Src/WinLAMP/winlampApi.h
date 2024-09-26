#ifndef NULLSOFT_WINLAMP_API_WINLAMP_IMPLEMENTATION_HEADER
#define NULLSOFT_WINLAMP_API_WINLAMP_IMPLEMENTATION_HEADER

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <atomic>

#include "./api_winlamp.h"

class WinLAMPApi : public api_winlamp
{
public:
	WinLAMPApi();
	~WinLAMPApi();

public:
	static HRESULT CreateInstance(WinLAMPApi **instance);
	static const char *getServiceName();
	static const GUID getServiceGuid();

public:
	/* Dispatchable */
	size_t AddRef();
	size_t Release();
	int QueryInterface(GUID interface_guid, void **object);

	/* api_winlamp */
	HWND GetMainWindow(void);
	HWND GetDlgParent(void);
	HRESULT OpenUrl(HWND hwnd, const wchar_t *url);
	int GetRegVer();

protected:
	std::atomic<std::size_t> _ref = 1;

protected:
	RECVS_DISPATCH;
};

#endif //NULLSOFT_WINLAMP_API_WINLAMP_IMPLEMENTATION_HEADER
#include "main.h"
#include "handler.h"
#include "../Agave/URIHandler/svc_urihandler.h"
#include <api/service/waservicefactory.h>
#include "api.h"

int HandleFilename(const wchar_t *filename)
{
	size_t i=0;
	waServiceFactory *sf = 0;
	int handled_count=0;

	while (NULL != (sf=WASABI_API_SVC->service_enumService(svc_urihandler::getServiceType(), i++)))
	{
		svc_urihandler *handler = (svc_urihandler *)sf->getInterface();
		if (handler)
		{
			int ret = handler->ProcessFilename(filename);
			sf->releaseInterface(handler);
			if (ret == svc_urihandler::HANDLED)
				handled_count++;
			else if (ret == svc_urihandler::HANDLED_EXCLUSIVE)
				return svc_urihandler::HANDLED;
		}

	}
	return handled_count?svc_urihandler::HANDLED:svc_urihandler::NOT_HANDLED;
}


int WinLAMPURIHandler::ProcessFilename(const wchar_t *filename)
{
	//MessageBoxW(NULL, filename, L"oi", MB_OK);
	return NOT_HANDLED;
}

int WinLAMPURIHandler::IsMine(const wchar_t *filename)
{
	if (CompareStringW(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), NORM_IGNORECASE, filename, (int)wcsnlen(filename, 9), L"winlamp://", 9) == CSTR_EQUAL)
		return HANDLED;
	else
		return NOT_HANDLED;
}

int WinLAMPURIHandler::EnumProtocols(size_t n, wchar_t *protocol, size_t protocolCch, wchar_t *description, size_t descriptionCch)
{
	switch(n)
	{
	case 0:
		StringCchCopyW(protocol, protocolCch, L"winlamp");
		StringCchCopyW(description, descriptionCch, L"WinLAMP Command Handler");
		return 0;
	default:
		return 1;
	}
}

int WinLAMPURIHandler::RegisterProtocol(const wchar_t *protocol, const wchar_t *winlampexe)
{
	return 1;

	IFileTypeRegistrar *registrar=0;
	if (GetRegistrar(&registrar, true) == 0 && registrar)
	{
		wchar_t str[MAX_PATH+32] = {0};
		StringCchPrintfW(str,MAX_PATH+32,L"\"%s\" /HANDLE \"%%1\"",winlampexe);
		wchar_t icon[MAX_PATH+32] = {0};
		StringCchPrintfW(icon,MAX_PATH+32,L"\"%s\",%d",winlampexe, config_whichicon);
		registrar->RegisterProtocol(L"winlamp", str, icon);
		registrar->Release();
		return 0;
	}
	return 1;
}

int WinLAMPURIHandler::UnregisterProtocol(const wchar_t *protocol)
{
  // TODO:
	return 1;
}

#define CBCLASS WinLAMPURIHandler
START_DISPATCH;
CB(PROCESSFILENAME, ProcessFilename);
CB(ISMINE, IsMine);
CB(ENUMPROTOCOLS, EnumProtocols);
CB(REGISTERPROTOCOL, RegisterProtocol);
CB(UNREGISTERPROTOCOL, UnregisterProtocol);
END_DISPATCH;
#undef CBCLASS
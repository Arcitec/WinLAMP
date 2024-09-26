#include "WinLAMPFactory.h"
#include "WinLAMP.h"

//FileTypeRegistrar registrar;

WinLAMPFactory::WinLAMPFactory()
{
}

WinLAMPFactory::~WinLAMPFactory()
{
}

ULONG WinLAMPFactory::AddRef()
{
	return 10;
}

ULONG WinLAMPFactory::Release()
{
	return 10;
}

HRESULT WinLAMPFactory::QueryInterface(REFIID riid, void ** ppAny)
{
	// IID_IUnknown is the REFIID of standard interface IUnknown
	if(riid == IID_IUnknown)
	{
		*ppAny = (IUnknown *)this;
	}
	else if(riid == IID_IClassFactory)
	{
		*ppAny = (IClassFactory *)this;
	}
	else
	{
		*ppAny = NULL;
		return E_NOINTERFACE;
	}

	((IUnknown *)(*ppAny))->AddRef();

	return S_OK;
}

HRESULT WinLAMPFactory::LockServer(BOOL fLock)
{
	return S_OK;
}

HRESULT WinLAMPFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void **ppAny)
{
	if(pUnkOuter != NULL)
	{
		return CLASS_E_NOAGGREGATION;
	}

	WinLAMP *winlamp = new WinLAMP;
	HRESULT hr = winlamp->QueryInterface(riid, ppAny);
	if (FAILED(hr))
		delete winlamp;
	return hr;


}
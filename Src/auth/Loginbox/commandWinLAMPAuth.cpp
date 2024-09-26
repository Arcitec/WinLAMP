#include "./commandWinLAMPAuth.h"
#include "./resultWinLAMPAuth.h"


LoginCommandWinLAMPAuth::LoginCommandWinLAMPAuth() 
	: ref(1)
{	
}

LoginCommandWinLAMPAuth::~LoginCommandWinLAMPAuth()
{

}

HRESULT LoginCommandWinLAMPAuth::CreateInstance(LoginCommandWinLAMPAuth **instance)
{
	if (NULL == instance) return E_POINTER;
	*instance = new LoginCommandWinLAMPAuth();
	if (NULL == *instance) return E_OUTOFMEMORY;
	return S_OK;
}

ULONG LoginCommandWinLAMPAuth::AddRef()
{
	return InterlockedIncrement((LONG*)&ref);
}

ULONG LoginCommandWinLAMPAuth::Release()
{
	if (0 == ref)
		return ref;
	
	LONG r = InterlockedDecrement((LONG*)&ref);
	if (0 == r)
		delete(this);
	
	return r;
}

HRESULT LoginCommandWinLAMPAuth::GetType(GUID *commandUid)
{
	if (NULL == commandUid) return E_INVALIDARG;
	*commandUid = LCUID_WINLAMPAUTH;
	return S_OK;
}

HRESULT LoginCommandWinLAMPAuth::SetParameter(LPCWSTR pszKey, LPCWSTR pszValue)
{
	return E_NOTIMPL;
}

HRESULT LoginCommandWinLAMPAuth::IsValid()
{
	return S_OK;
}

HRESULT LoginCommandWinLAMPAuth::IsIdentical(LoginCommand *test)
{
	if (NULL == test)
		return E_INVALIDARG;

	GUID typeId;
	if (FAILED(test->GetType(&typeId)) || FALSE == IsEqualGUID(LCUID_WINLAMPAUTH, typeId))
		return S_FALSE;

	return S_OK;
}


HRESULT LoginCommandWinLAMPAuth::BeginLogin(LoginData *data, LoginResult::Callback callback, void *user, LoginResult **result)
{
	LoginResultWinLAMPAuth *winlampAuth;
	HRESULT hr = LoginResultWinLAMPAuth::CreateInstance(data, callback, user, &winlampAuth);
	if (SUCCEEDED(hr))
	{
		if (NULL != result)
			*result = winlampAuth;
		else
			winlampAuth->Release();
	}
	else
	{
		if (NULL != result) 
			*result = NULL;
	}

	return hr;
}

HRESULT LoginCommandWinLAMPAuth::EndLogin(LoginResult *result, INT *authCode, LoginCredentials **credentials)
{
	if (NULL == result)
		return E_INVALIDARG;

	HRESULT hr = result->IsCompleted();
	if (S_OK != hr)
	{
		HANDLE completed;
		hr = result->GetWaitHandle(&completed);
		if (SUCCEEDED(hr))
		{
			WaitForSingleObjectEx(completed, INFINITE, TRUE);
			CloseHandle(completed);
		}
	}
	
	if (SUCCEEDED(hr))
	{
		LoginResultWinLAMPAuth *winlampAuth;
		hr = result->QueryInterface(LCUID_WINLAMPAUTH, (void**)&winlampAuth);
		if(SUCCEEDED(hr))
		{
			hr = winlampAuth->GetResult(authCode, credentials);
			winlampAuth->Release();
		}
	}

	return hr;
}

HRESULT LoginCommandWinLAMPAuth::RequestAbort(LoginResult *result, BOOL drop)
{
	if (NULL == result) return E_INVALIDARG;
	return result->RequestAbort(drop);
}

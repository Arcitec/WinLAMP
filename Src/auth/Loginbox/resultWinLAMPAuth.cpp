#include "./resultWinLAMPAuth.h"	
#include "./common.h"
#include "./commandWinLAMPAuth.h"
#include "./loginData.h"
#include "./dataCredentials.h"
#include "./loginCredentials.h"
#include "./loginbox.h"

#include "../resource.h"

LoginResultWinLAMPAuth::LoginResultWinLAMPAuth(api_auth *auth, LoginDataCredentials *pInput, Callback fnCallback, void *pUser)
	: ref(1), authApi(auth), input(pInput), callback(fnCallback), user(pUser), 
	thread(NULL), abort(NULL), completed(NULL), credentials(NULL), authCode(AUTH_NOT_AUTHORIZED), statusCookie(0)
{
	InitializeCriticalSection(&lock);

	authApi->AddRef();
	input->AddRef();
}

LoginResultWinLAMPAuth::~LoginResultWinLAMPAuth()
{
	if (NULL != abort) 
		CloseHandle(abort);

	if (NULL != completed) 
		CloseHandle(completed);

	if (NULL != thread)
		CloseHandle(thread);
	
	authApi->Release();
	input->Release();
	
	if (NULL != credentials)
		credentials->Release();

	DeleteCriticalSection(&lock);
}

HRESULT LoginResultWinLAMPAuth::CreateInstance(LoginData *input, Callback callback, void *user, LoginResultWinLAMPAuth **instance)
{
	if (NULL == instance) return E_POINTER;
	*instance = NULL;
	
	LoginDataCredentials *credentialsInput;
	if (NULL == input || FAILED(input->QueryInterface(IID_LoginDataCredentials, (void**)&credentialsInput)))
		return E_INVALIDARG;

	HRESULT hr;
	api_auth *authApi;
	
	HWND hLoginbox = credentialsInput->GetLoginbox();
	if (NULL == hLoginbox || FALSE == LoginBox_GetAuthApi(hLoginbox, &authApi))
		hr = E_FAIL;
	else
	{
		LoginResultWinLAMPAuth *result = new LoginResultWinLAMPAuth(authApi, credentialsInput, callback, user);
		if (NULL == result) 
			hr = E_OUTOFMEMORY;
		else
		{
			hr = result->Start();
			if (FAILED(hr))
			{
				result->Release();
				result = NULL;
			}
			else
			{
				*instance = result;
			}
		}

		authApi->Release();
	}
	credentialsInput->Release();
	return hr;
}

STDMETHODIMP_(ULONG) LoginResultWinLAMPAuth::AddRef(void)
{
	return InterlockedIncrement((LONG*)&ref);
}

STDMETHODIMP_(ULONG) LoginResultWinLAMPAuth::Release(void)
{
	if (0 == ref)
		return ref;
	
	LONG r = InterlockedDecrement((LONG*)&ref);
	if (0 == r)
		delete(this);
	
	return r;
}

STDMETHODIMP LoginResultWinLAMPAuth::QueryInterface(REFIID riid, PVOID *ppvObject)
{
	if (NULL == ppvObject) 
		return E_POINTER;
	
	if (IsEqualIID(riid, LCUID_WINLAMPAUTH))
		*ppvObject = static_cast<LoginResultWinLAMPAuth*>(this);
	else if (IsEqualIID(riid, IID_IUnknown))
		*ppvObject = static_cast<IUnknown*>(this);
	else
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}

	if (NULL == *ppvObject)
		return E_UNEXPECTED;

	AddRef();
	return S_OK;
}

size_t LoginResultWinLAMPAuth::Wasabi_AddRef()
{
	return AddRef();	
}

size_t LoginResultWinLAMPAuth::Wasabi_Release()
{
	return Release();
}

int LoginResultWinLAMPAuth::Wasabi_QueryInterface(GUID iid, void **object)
{
	return QueryInterface(iid, object);
}

HRESULT LoginResultWinLAMPAuth::Start()
{
	//HRESULT hr;
	HANDLE threadCopy = NULL;

	input->SetStatus(MAKEINTRESOURCE(IDS_STATUS_INITIALIZING));
	EnterCriticalSection(&lock);
	if (NULL != thread)
	{
		//hr = E_PENDING;
	}
	else
	{
		AddRef();
		thread = CreateThread(NULL, 0, WinLAMPAuth_ThreadProcCallback, this, CREATE_SUSPENDED, NULL);
		if (NULL == thread)
		{
			Release();
		}
		else
		{
			if (0 == DuplicateHandle(GetCurrentProcess(), thread, GetCurrentProcess(), &threadCopy, 0, FALSE, DUPLICATE_SAME_ACCESS))
			{
				ResumeThread(thread); // grr...
				threadCopy = NULL;
			}
		}
	}
	LeaveCriticalSection(&lock);

	if (NULL != threadCopy)
	{
		ResumeThread(threadCopy);
		CloseHandle(threadCopy);
	}

	return S_OK;
}
DWORD LoginResultWinLAMPAuth::ThreadProc()
{
	Callback callbackCopy(NULL);
	HANDLE completedCopy(NULL);
	INT resultCode = AUTH_SUCCESS;
	LPWSTR username(NULL), password(NULL);
	
	api_auth::AuthResults	authResults;
	SecureZeroMemory(&authResults, sizeof(authResults));

	EnterCriticalSection(&lock);
	authCode = AUTH_NOT_AUTHORIZED;
	if (NULL != credentials)
	{
		credentials->Release();
		credentials = NULL;
	}
	LeaveCriticalSection(&lock);

	username = GetUsername(input->GetUsername(), &resultCode);
	
	if (AUTH_SUCCESS == resultCode)
		password = GetPassword(input->GetPassword(), &resultCode);
		
	if (AUTH_SUCCESS == resultCode)
	{
		if (NULL == input->GetContext())
		{
			resultCode = authApi->Login(username, password, &authResults, this);
		}
		else
		{
			LPWSTR passcode = GetPasscode(input->GetPasscode(), &resultCode);
			if (AUTH_SUCCESS == resultCode)
			{
				resultCode = authApi->LoginSecurID(username, password, input->GetContext(), passcode, &authResults, this); 
				LoginBox_FreeStringSecure(passcode);
			}
		}
	}
	
	EnterCriticalSection(&lock);
	
	authCode = resultCode;
	switch(authCode)
	{
		case AUTH_SUCCESS:
			{
				GUID realm;
				if (FAILED(input->GetRealm(&realm)) ||
					FAILED(LoginCredentials::CreateInstance(&realm, username, authResults.session_key, 
								authResults.token, authResults.expire, &credentials)))
				{
					authCode = AUTH_UNEXPECTED;
				}
			}
			break;
		case AUTH_SECURID:
			if(FAILED(input->SetContext(authResults.context)))
				authCode = AUTH_NOT_AUTHORIZED;
			break;
	}
	
	INT statusId;
	switch(authCode)
	{
			case AUTH_SUCCESS:	statusId = IDS_STATUS_SUCCEEDED; break;
			case AUTH_SECURID:	statusId = IDS_STATUS_PASSCODE_REQUIRED; break;
			case AUTH_ABORT:	statusId = IDS_STATUS_ABORTED; break;
			default:			statusId = IDS_STATUS_FAILED; break;
	}
	input->SetStatus(MAKEINTRESOURCE(statusId));

	CloseHandle(thread);
	thread = NULL;
	callbackCopy = callback;

	if (NULL == completed || FALSE == DuplicateHandle(GetCurrentProcess(), completed, 
			GetCurrentProcess(), &completedCopy, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		completedCopy = NULL;
	}
	
	LeaveCriticalSection(&lock);

	SecureZeroMemory(&authResults, sizeof(authResults));
	LoginBox_FreeStringSecure(username);
	LoginBox_FreeStringSecure(password);

	if (NULL != completedCopy)
	{
		SetEvent(completedCopy);
		CloseHandle(completedCopy);
	}

	if (NULL != callbackCopy)
		callbackCopy(this);
	
	return 0;
}

HRESULT LoginResultWinLAMPAuth::GetWaitHandle(HANDLE *handle)
{
	if (NULL == handle)
		return E_POINTER;

	HRESULT hr = S_OK;

	EnterCriticalSection(&lock);

	if (NULL == completed)
	{
		completed = CreateEvent(NULL, TRUE, (S_OK == IsCompleted()), NULL);
		if (NULL == completed)
		{
			*handle = NULL;
			DWORD error = GetLastError();
			hr = HRESULT_FROM_WIN32(error);
		}
	}

	if (SUCCEEDED(hr) && 
		FALSE == DuplicateHandle(GetCurrentProcess(), completed, 
					GetCurrentProcess(), handle, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		*handle = NULL;
		DWORD error = GetLastError();
		hr = HRESULT_FROM_WIN32(error);
	}
	
	LeaveCriticalSection(&lock);
	return hr;
}

HRESULT LoginResultWinLAMPAuth::GetUser(void **pUser)
{
	if (NULL == pUser) return E_POINTER;
	EnterCriticalSection(&lock);
	*pUser = user;
	LeaveCriticalSection(&lock);
	return S_OK;
}

HRESULT	LoginResultWinLAMPAuth::RequestAbort(BOOL fDrop)
{
	HRESULT hr;
	EnterCriticalSection(&lock);
	if (NULL == abort)
	{
		abort = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (NULL == abort)
		{			
			DWORD error = GetLastError();
			hr = HRESULT_FROM_WIN32(error);
		}
	}
		
	if (NULL != abort && FALSE == SetEvent(abort))
	{
		DWORD error = GetLastError();
		hr = HRESULT_FROM_WIN32(error);
	}
	else
	{
		input->SetStatus(MAKEINTRESOURCE(IDS_STATUS_ABORTING));
	}
	
	if (FALSE != fDrop)
	{
		callback = NULL;
		user = NULL;
	}

	LeaveCriticalSection(&lock);
	return hr;
}

HRESULT LoginResultWinLAMPAuth::IsCompleted()
{
	return (NULL == thread) ? S_OK : S_FALSE;
}

HRESULT LoginResultWinLAMPAuth::IsAborting()
{
	return IsAbortingEx(0);
}

HRESULT LoginResultWinLAMPAuth::IsAbortingEx(UINT waitMs)
{
	return (NULL != abort && WAIT_OBJECT_0 == WaitForSingleObjectEx(abort, waitMs, TRUE))? 
			S_OK : S_FALSE;
}

HRESULT LoginResultWinLAMPAuth::GetLoginData(LoginData **loginData)
{
	if (NULL == loginData) return E_POINTER;
	EnterCriticalSection(&lock);
	
	*loginData = input;
	if (NULL != input) 
		input->AddRef();

	LeaveCriticalSection(&lock);
	return S_OK;
}

HRESULT LoginResultWinLAMPAuth::GetResult(INT *pAuthCode, LoginCredentials **ppCredentials)
{
	if (S_OK != IsCompleted())
		return E_PENDING;

	EnterCriticalSection(&lock);

	if (NULL != pAuthCode)
		*pAuthCode = authCode;

	if (NULL != ppCredentials)
	{
		*ppCredentials = credentials;
		if (NULL != credentials)
			credentials->AddRef();
	}

	LeaveCriticalSection(&lock);
	
	return S_OK;
}

int LoginResultWinLAMPAuth::Event_AuthConnecting()
{
	if (S_OK == IsAbortingEx(0))
		return 1;

	input->SetStatus(MAKEINTRESOURCE(IDS_STATUS_CONNECTING));
	return 0;
}

int LoginResultWinLAMPAuth::Event_AuthSending()
{
	if (S_OK == IsAbortingEx(0))
		return 1;

	input->SetStatus(MAKEINTRESOURCE(IDS_STATUS_SENDING));
	return 0;
}

int LoginResultWinLAMPAuth::Event_AuthReceiving()
{
	if (S_OK == IsAbortingEx(0))
		return 1;

	input->SetStatus(MAKEINTRESOURCE(IDS_STATUS_RECEIVING));
	return 0;
}

int LoginResultWinLAMPAuth::Event_AuthIdle()
{
	if (S_OK == IsAbortingEx(50))
		return 1;

	return 0;
}

LPWSTR LoginResultWinLAMPAuth::GetUsername(LPCWSTR pszInput, INT *authError)
{
	return MakeAuthParam(pszInput, -1, 3, 48, TRUE, TRUE, (0xFFFF & ~C1_CNTRL), AUTH_USERNAME, authError);
}

LPWSTR LoginResultWinLAMPAuth::GetPassword(LPCWSTR pszInput, INT *authError)
{	
	return MakeAuthParam(pszInput, -1, 6, 48, TRUE, FALSE, (0xFFFF & ~C1_CNTRL), AUTH_PASSWORD, authError);
}

LPWSTR LoginResultWinLAMPAuth::GetPasscode(LPCWSTR pszInput, INT *authError)
{
	return MakeAuthParam(pszInput, -1, 6, 6, FALSE, FALSE, C1_DIGIT, AUTH_PASSCODE, authError);
}

LPWSTR LoginResultWinLAMPAuth::MakeAuthParam(LPCWSTR pszInput, INT cchInput, INT min, INT max, BOOL removeSpaces, BOOL firstLetter, WORD typeMask, INT errorBase, INT *authError)
{
	if (cchInput < 0 || NULL == pszInput)
		cchInput = (NULL != pszInput) ? lstrlen(pszInput) : 0;

	if (cchInput < min || (FALSE == removeSpaces && cchInput > max)) 
	{
		if (NULL != authError)
			*authError = errorBase + ((cchInput < min) ? AUTHPARAM_TOOSHORT : AUTHPARAM_TOOLONG);
		return NULL;
	}

	WORD *info = (WORD*)calloc(cchInput, sizeof(WORD));
	if (NULL == info)
	{
		if (NULL != authError) *authError = AUTH_UNEXPECTED;
		return NULL;
	}	

	if (FALSE == GetStringTypeW(CT_CTYPE1, pszInput, cchInput, info))
	{
		free(info);
		if (NULL != authError) *authError = AUTH_UNEXPECTED;
		return NULL;
	}
	
	INT error = AUTH_SUCCESS;
	LPWSTR dest = NULL;
		
	BOOL firstChecked = FALSE;
	INT cchSpaces = 0;
	for (INT i = 0; i < cchInput; i++)
	{
		if (FALSE != removeSpaces && 0 != (C1_SPACE & info[i]))
			cchSpaces++;
		else if (0 == (typeMask & info[i]))
		{
			error = errorBase + AUTHPARAM_BADFORMAT;
			break;
		}
		else
		{
			if (FALSE != firstLetter && FALSE == firstChecked)
			{
				if (0 == (C1_ALPHA & info[i]))
				{
					error = errorBase + AUTHPARAM_BADFORMAT;
					break;
				}
				firstChecked = TRUE;
			}
		}
	}

	if (AUTH_SUCCESS == error)
	{			
		INT cchTotal = cchInput - cchSpaces;
		if (cchTotal < min) 
			error = errorBase + AUTHPARAM_TOOSHORT;
		else if (cchTotal > max)
			error = errorBase + AUTHPARAM_TOOLONG;
		else
		{
			dest = LoginBox_MallocString(cchTotal + 1);
			if (NULL == dest)
				error = AUTH_UNEXPECTED;
			else
			{
				if (FALSE != removeSpaces)
				{
					LPCWSTR s = pszInput;
					LPWSTR d = dest;
					for (INT i = 0; i < cchInput; i++, s++)
					{
						if (0 == (C1_SPACE & info[i]))
						{
							*d = *s;
							d++;
						}
					}
					*d = L'\0';
				}
				else
				{
					CopyMemory(dest, pszInput, (cchInput * sizeof(WCHAR)));
					dest[cchInput] = L'\0';
				}
			}
		}
	}
	
	
	free(info);

	if (NULL != authError) 
		*authError = error;

	return dest;
}


static DWORD WINAPI WinLAMPAuth_ThreadProcCallback(void *param)
{
	LoginResultWinLAMPAuth *result =(LoginResultWinLAMPAuth*)param;
	if (NULL == result) return 1;
	
	INT exitCode = result->ThreadProc();
	result->Release();
	return exitCode;
}


#define CBCLASS LoginResultWinLAMPAuth
START_DISPATCH;
CB(ADDREF, Wasabi_AddRef)
CB(RELEASE, Wasabi_Release)
CB(QUERYINTERFACE, Wasabi_QueryInterface)
CB(ONCONNECTING, Event_AuthConnecting)
CB(ONSENDING, Event_AuthSending)
CB(ONRECEIVING, Event_AuthReceiving)
CB(ONIDLE, Event_AuthIdle)
END_DISPATCH;
#undef CBCLASS


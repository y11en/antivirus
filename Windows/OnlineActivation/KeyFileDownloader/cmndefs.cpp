#include <tchar.h>
#include "cmndefs.h"

//-----------------------------------------------------------------------------

StopFlag::ValueType StopFlag::instance			= 0;
ThreadHandle::ValueType ThreadHandle::instance	= 0;

Callback::ValueType Callback::instance;
KeyFileName::ValueType KeyFileName::instance;

//-----------------------------------------------------------------------------


CAppData::CAppData() :
					m_hDialog(0),
					m_nDlgItem(0),
					m_nProdId(0),
					m_lpszProdVer(NULL),
					m_lpszAgentName(NULL),
					m_lpszServerName(NULL),
					m_nPort(0),
					m_lpszUserName(NULL),
					m_lpszPassword(NULL),
					m_lpszResource(NULL),
					m_lpszDstFolder(NULL),
					m_lpszKeyNum(NULL),
					m_lpszCompany(NULL),
					m_lpszName(NULL),
					m_lpszCountry(NULL),
					m_lpszCity(NULL),
					m_lpszAddress(NULL),
					m_lpszTelephone(NULL),
					m_lpszFax(NULL),
					m_lpszEmail(NULL),
					m_lpszWww(NULL)
{
}

//-----------------------------------------------------------------------------

void CAppData::SetParameters(
					HWND	hDialog,
					int		nDlgItem,
					int		nProdId,
					LPCWSTR	lpszProdVer,
					LPCTSTR	lpszAgentName,
					LPCTSTR	lpszServerName,
					int		nPort,
					LPCTSTR	lpszUserName,
					LPCTSTR	lpszPassword,
					LPCTSTR	lpszResource,
					LPCTSTR	lpszDstFolder,
					LPCWSTR	lpszKeyNum,
					LPCWSTR	lpszCompany,
					LPCWSTR	lpszName,
					LPCWSTR	lpszCountry,
					LPCWSTR	lpszCity,
					LPCWSTR	lpszAddress,
					LPCWSTR	lpszTelephone,
					LPCWSTR	lpszFax,
					LPCWSTR	lpszEmail,
					LPCWSTR	lpszWww
					)
{
	m_hDialog = hDialog;
	m_nDlgItem = nDlgItem;
	m_nProdId = nProdId;
	
	SetStringParm(m_lpszProdVer, lpszProdVer);
	SetStringParm(m_lpszAgentName, lpszAgentName);
	SetStringParm(m_lpszServerName, lpszServerName);

	m_nPort = nPort;

	SetStringParm(m_lpszUserName, lpszUserName);
	SetStringParm(m_lpszPassword, lpszPassword);
	SetStringParm(m_lpszResource, lpszResource);
	SetStringParm(m_lpszDstFolder, lpszDstFolder);
	SetStringParm(m_lpszKeyNum, lpszKeyNum);
	SetStringParm(m_lpszCompany, lpszCompany);
	SetStringParm(m_lpszName, lpszName);
	SetStringParm(m_lpszCountry, lpszCountry);
	SetStringParm(m_lpszCity, lpszCity);
	SetStringParm(m_lpszAddress, lpszAddress);
	SetStringParm(m_lpszTelephone, lpszTelephone);
	SetStringParm(m_lpszFax, lpszFax);
	SetStringParm(m_lpszEmail, lpszEmail);
	SetStringParm(m_lpszWww, lpszWww);
}

//-----------------------------------------------------------------------------
void CAppData::SetStringParm(LPSTR &lpszParm, LPCSTR lpszValue)
{
	if (lpszParm != NULL)
	{
		delete[] lpszParm;
		lpszParm = NULL;
	}

	if (lpszValue != NULL)
	{
		lpszParm = new char[strlen(lpszValue) + 1];
		strcpy(lpszParm, lpszValue);
	}
		
}

//-----------------------------------------------------------------------------

void CAppData::SetStringParm(LPWSTR &lpszParm, LPCWSTR lpszValue)
{
	if (lpszParm != NULL)
	{
		delete[] lpszParm;
		lpszParm = NULL;
	}

	if (lpszValue != NULL)
	{
		lpszParm = new wchar_t[wcslen(lpszValue) + 1];
		wcscpy(lpszParm, lpszValue);
	}
}

//-----------------------------------------------------------------------------

CAppData::~CAppData()
{
	delete[] m_lpszAgentName;
	m_lpszAgentName = NULL;
	delete[] m_lpszServerName;
	m_lpszServerName = NULL;
	delete[] m_lpszUserName;
	m_lpszUserName = NULL;
	delete[] m_lpszPassword;
	m_lpszPassword = NULL;
	delete[] m_lpszResource;
	m_lpszResource = NULL;
	delete[] m_lpszDstFolder;
	m_lpszDstFolder = NULL;
	delete[] m_lpszKeyNum;
	m_lpszKeyNum = NULL;
	delete[] m_lpszCompany;
	m_lpszCompany = NULL;
	delete[] m_lpszName;
	m_lpszName = NULL;
	delete[] m_lpszCountry;
	m_lpszCountry = NULL;
	delete[] m_lpszCity;
	m_lpszCity = NULL;
	delete[] m_lpszAddress;
	m_lpszAddress = NULL;
	delete[] m_lpszTelephone;
	m_lpszTelephone = NULL;
	delete[] m_lpszFax;
	m_lpszFax = NULL;
	delete[] m_lpszEmail;
	m_lpszEmail = NULL;
	delete[] m_lpszWww;
	m_lpszWww = NULL;
}

//=============================================================================

void SendDlgMsg(HWND hDialog, int nDlgItem, int nMsg)
{
	if (Callback::Instance().IsSet())
	{
		Callback::Instance().Call(nMsg);
	}
	else
	{

		TCHAR buf[33] = { 0 };
		_itot(nMsg, buf, 10);
		SetDlgItemText(hDialog, nDlgItem, buf);
		SendMessage(hDialog, WM_COMMAND, MAKEWPARAM(nDlgItem, 0), 
											(LPARAM) GetDlgItem(hDialog, nDlgItem));
		InvalidateRect(hDialog, NULL, TRUE);
	}
}

//-----------------------------------------------------------------------------

CSafeString::CSafeString() : m_szwValue(NULL)
{
	InitializeCriticalSection(&m_csGuard);
	m_szwValue = (TCHAR*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TCHAR) * MAX_PATH);
}

//-----------------------------------------------------------------------------

const TCHAR* CSafeString::GetValue()
{
	const TCHAR* szwResult = NULL;

	__try
	{
		EnterCriticalSection(&m_csGuard);
		szwResult = m_szwValue;
	}
	__finally
	{
		LeaveCriticalSection(&m_csGuard);
	}

	return szwResult;
}

//-----------------------------------------------------------------------------

void CSafeString::SetValue(const TCHAR* szwValue)
{
	__try
	{
		EnterCriticalSection(&m_csGuard);
		lstrcpyn(m_szwValue, szwValue, MAX_PATH - 1);
	}
	__finally
	{
		LeaveCriticalSection(&m_csGuard);
	}
}

//-----------------------------------------------------------------------------

CSafeString::~CSafeString()
{
	DeleteCriticalSection(&m_csGuard);
	HeapFree(GetProcessHeap(), 0, m_szwValue);
	m_szwValue = NULL;
}

//-----------------------------------------------------------------------------

CCallback::CCallback()
{
	Clear();
}

//-----------------------------------------------------------------------------

void CCallback::Set(KFD_NOTIFICATION_CALLBACK fCallback, void* pContext)
{
	m_pContext = pContext;
	m_fCallback = fCallback;
}

//-----------------------------------------------------------------------------

bool CCallback::IsSet() const
{
	return m_fCallback != 0;
}

//-----------------------------------------------------------------------------

void CCallback::Call(int nMsg)
{
	m_fCallback(m_pContext, nMsg);
}

//-----------------------------------------------------------------------------

void CCallback::Clear()
{
	m_fCallback = 0;
	m_pContext = 0;
}

//-----------------------------------------------------------------------------

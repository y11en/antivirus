#ifndef KFD_CMNDEFS_H
#define KFD_CMNDEFS_H


#include <windows.h>
#include <wininet.h>
#include "keyfiledl.h"

//-----------------------------------------------------------------------------

template <typename T, int> class Singleton
{
public:
	typedef T ValueType;
	static T& Instance() { return instance; }
private:
	static T instance;
};

//-----------------------------------------------------------------------------

typedef Singleton<bool, 1> StopFlag;
typedef Singleton<HANDLE, 1> ThreadHandle;
typedef Singleton<HINTERNET, 1> HttpHandle;

//-----------------------------------------------------------------------------

class CAppData
{
public:
	HWND	m_hDialog;
	int		m_nDlgItem;
	int		m_nProdId;
	LPWSTR	m_lpszProdVer;
	LPTSTR	m_lpszAgentName;
	LPTSTR	m_lpszServerName;
	int		m_nPort;
	LPTSTR	m_lpszUserName;
	LPTSTR	m_lpszPassword;
	LPTSTR	m_lpszResource;
	LPTSTR	m_lpszDstFolder;
	LPWSTR	m_lpszKeyNum;
	LPWSTR	m_lpszCompany;
	LPWSTR	m_lpszName;
	LPWSTR	m_lpszCountry;
	LPWSTR	m_lpszCity;
	LPWSTR	m_lpszAddress;
	LPWSTR	m_lpszTelephone;
	LPWSTR	m_lpszFax;
	LPWSTR	m_lpszEmail;
	LPWSTR	m_lpszWww;

public:
	CAppData();
	~CAppData();

	void SetParameters(
			HWND	hDialog,
			int		nDlgItem,
			int		nProdId,
			LPCWSTR lpszProdVer,
			LPCTSTR	lpszAgentName,
			LPCTSTR	lpszServerName,
			int		nPort,
			LPCTSTR	lpszUserName,
			LPCTSTR	lpszPassword,
			LPCTSTR	lpszResource,
			LPCTSTR	lpszDstFolder,
			LPCWSTR	lpszKeyNum,
			LPCWSTR	lpszCompany,
			LPCWSTR lpszName,
			LPCWSTR	lpszCountry,
			LPCWSTR	lpszCity,
			LPCWSTR	lpszAddress,
			LPCWSTR	lpszTelephone,
			LPCWSTR	lpszFax,
			LPCWSTR	lpszEmail,
			LPCWSTR	lpszWww
	);

protected:
	void SetStringParm(LPSTR &lpszParm, LPCSTR lpszValue);
	void SetStringParm(LPWSTR &lpszParm, LPCWSTR lpszValue);
};

//-----------------------------------------------------------------------------

class CCallback
{
public:
	CCallback();
	void Set(KFD_NOTIFICATION_CALLBACK fCallback, void* pContext);
	void Call(int nMsg);
	bool IsSet() const;
	void Clear();
private:
	KFD_NOTIFICATION_CALLBACK m_fCallback;
	void* m_pContext;
};

//-----------------------------------------------------------------------------

typedef Singleton<CCallback, 1>	Callback;

//-----------------------------------------------------------------------------

class CSafeString
{
public:
	CSafeString();
	~CSafeString();

	const TCHAR* GetValue();
	void SetValue(const TCHAR* szwValue);

	TCHAR* m_szwValue;

private:
	CRITICAL_SECTION m_csGuard;

};

//-----------------------------------------------------------------------------

typedef Singleton<CSafeString, 1>	KeyFileName;

//-----------------------------------------------------------------------------

void SendDlgMsg(HWND hDialog, int nDlgItem, int nMsg);

//-----------------------------------------------------------------------------

#endif // KFD_CMNDEFS_H
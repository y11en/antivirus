#ifndef KFD_HTTPMSG_H
#define KFD_HTTPMSG_H

#include <windows.h>
#include <wininet.h>
#include "cmndefs.h"

//-----------------------------------------------------------------------------

class CInetBase
{
public:
	CInetBase();
	virtual ~CInetBase();

	DWORD	GetInitErrCode() const;
	DWORD	EnableCallBack(BOOL bEnabled);

	virtual operator HINTERNET() const = 0;

protected:
	void SetInitErrCode(DWORD dwCode);
	BOOL IsCallBackEnabled();
	virtual void FreeSpaceAllocated() = 0;
	virtual void CallBackFunc(
					DWORD dwInternetStatus, 
					LPVOID lpvStatusInformation,
					DWORD dwStatusInformationLength
	);

private:
	BOOL	m_dwInitErrorCode;
	BOOL	m_bIsCallBackEnabled;
	BOOL	m_bIsCallBackSet;

	friend void CALLBACK InetCallBack(
						HINTERNET hInternet,
						DWORD_PTR dwContext,
						DWORD dwInternetStatus,
						LPVOID lpvStatusInformation,
						DWORD dwStatusInformationLength
	);
};

//-----------------------------------------------------------------------------

class CInetSession : public CInetBase
{
public:
	CInetSession::CInetSession(
			LPCTSTR lpszAgentName = NULL,
			DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG,
			LPCTSTR lpszProxyName = NULL,
			LPCTSTR lpszProxyBypass = NULL,
			DWORD dwFlags = NULL
	);

	virtual ~CInetSession();

	virtual operator HINTERNET() const;	

protected:
	virtual void FreeSpaceAllocated();

private:
	HINTERNET	m_hSession;
	
	DWORD		m_dwAccessType;
	DWORD		m_dwFlags;

	LPTSTR		m_lpszAgentName;
	LPTSTR		m_lpszProxyName;
	LPTSTR		m_lpszProxyBypass;
};

//-----------------------------------------------------------------------------

class CReqestEntity
{
public:
	CReqestEntity();
	virtual ~CReqestEntity();

	virtual	LPCTSTR GetMethod() const;
	virtual LPCTSTR	GetResource() const;
	virtual LPCTSTR GetHeaders() const;

	virtual LPVOID	GetData();
	virtual size_t	GetDataSize() const;

	virtual DWORD	SetHeaders(LPCTSTR lpdszHeaders);


protected:
	DWORD SetMethod(LPCTSTR lpszMethod);
	DWORD SetResource(LPCTSTR lpszResource);
	DWORD SetData(LPVOID lpData, size_t nSize);

private:
	LPTSTR	m_lpszMethod;
	LPTSTR	m_lpszResource;
	LPTSTR	m_lpszHeaders;

	LPVOID	m_lpDataBuf;
	size_t	m_nDataSize;
};

//-----------------------------------------------------------------------------

class CPostForm : public CReqestEntity
{
public:
	CPostForm(LPCTSTR lpszResource);
	virtual ~CPostForm();

	virtual DWORD AddInput(LPCWSTR lpszName, LPCWSTR lpszValue);
	virtual void ClearInput();

protected:
	DWORD UcsToUtf8Enc(LPCWSTR lpszSource, DWORD nSrcSz, LPSTR lpszDest, DWORD &nDstSz);

};

//-----------------------------------------------------------------------------

class CHttpMessage : public CInetBase
{
public:
	CHttpMessage::CHttpMessage(
			CInetSession *pInetSession,
			CAppData *pAppData
	);

	~CHttpMessage();

	DWORD SendRequest(CReqestEntity &requestEntity);
	DWORD GetResponseCode(LPDWORD lpdwCode);
	DWORD QueryInfo(LPVOID lpBuf, LPDWORD lpdwSize, DWORD dwFlags);

	DWORD ReadData(
			LPVOID lpBuffer,
			DWORD dwBytesToRead,
			LPDWORD lpdwNumBytesRead
	);

	DWORD CloseRequest();

	DWORD SaveToFile(LPCTSTR lpszFilePath);
	DWORD GetContext() const;

	virtual void FreeSpaceAllocated();
	virtual operator HINTERNET() const;	

protected:
	virtual void CallBackFunc(
					DWORD dwInternetStatus, 
					LPVOID lpvStatusInformation,
					DWORD dwStatusInformationLength
	);

private:
	HINTERNET	m_hConnection;
	HINTERNET	m_hRequest;
	
	CAppData	*m_pAppData;

	DWORD		m_dwContext;
};

//=============================================================================

inline DWORD CInetBase::GetInitErrCode() const
{
	return m_dwInitErrorCode;
}

//-----------------------------------------------------------------------------

inline void CInetBase::SetInitErrCode(DWORD dwCode)
{
	m_dwInitErrorCode = dwCode;
}

//-----------------------------------------------------------------------------

inline BOOL CInetBase::IsCallBackEnabled()
{
	return m_bIsCallBackEnabled;
}

//=============================================================================

inline CInetSession::operator HINTERNET() const
{
	return m_hSession;
}

//=============================================================================

inline DWORD CHttpMessage::GetContext() const
{
	return m_dwContext;
}

//-----------------------------------------------------------------------------

inline CHttpMessage::operator HINTERNET() const
{
	return m_hConnection;
}

//=============================================================================

inline LPCTSTR CReqestEntity::GetMethod() const
{
	return m_lpszMethod;
}

//-----------------------------------------------------------------------------

inline LPCTSTR CReqestEntity::GetResource() const
{
	return m_lpszResource;
}

//-----------------------------------------------------------------------------

inline LPCTSTR CReqestEntity::GetHeaders() const
{
	return m_lpszHeaders;
}

//-----------------------------------------------------------------------------

inline LPVOID CReqestEntity::GetData()
{
	return m_lpDataBuf;
}

//-----------------------------------------------------------------------------

inline size_t CReqestEntity::GetDataSize() const
{
	return m_nDataSize;
}

//=============================================================================
// AtlUnicodeToUTF8
//
// Support for converting UNICODE strings to UTF8 
// (WideCharToMultiByte does not support UTF8 in Win98)
//
// This function is from the SDK implementation of 
// WideCharToMultiByte with the CP_UTF8 codepage
//
//=============================================================================

int AtlUnicodeToUTF8(LPCWSTR wszSrc, int nSrc, LPSTR szDest, int nDest);

//=============================================================================

#endif // KFD_HTTPMSG_H
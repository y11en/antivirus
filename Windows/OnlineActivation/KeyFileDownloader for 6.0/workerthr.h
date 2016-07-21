#ifndef KFD_WORKERTHR_H
#define KFD_WORKERTHR_H

#include "httpmsg.h"

//-----------------------------------------------------------------------------

DWORD WINAPI WorkerThreadFunc(LPVOID lpParam);

//-----------------------------------------------------------------------------

class CWorkerThread
{
public:
	CWorkerThread(CAppData* pAppData);
	~CWorkerThread();

	DWORD Run();

protected:
	bool AnalyzeStatusCode(DWORD code);
	DWORD HandleServerReply(CHttpMessage& httpMsg);
	DWORD DownloadLicense(CInetSession& inetSession, LPCTSTR lpszLicFileName);
	DWORD PostActivationRequest(LPCTSTR url, LPCTSTR licFileName);

	DWORD FillFormKeyData(CPostForm& postForm);
	DWORD FillFormStatus(CPostForm& postForm, LPCWSTR lpszStatus);
	DWORD CheckLicense(
				LPCTSTR lpszLicFileName, 
				DWORD dwProductId, 
				LPCTSTR lpszDestFolder,
				LPTSTR lpszLicDestPathName,
				DWORD& dwNameOfs
	);

private:
	CAppData*					m_pAppData;
	std::basic_string<TCHAR>	m_serverUrl;
	std::wstring				m_requestId;
};

//-----------------------------------------------------------------------------

#endif // KFD_WORKERTHR_H
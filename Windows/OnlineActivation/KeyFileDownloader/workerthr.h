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
	DWORD CheckReqStatus(CHttpMessage& httpMessage, BOOL& bIsGood);
	DWORD FillFormKeyData(CPostForm& postForm);
	DWORD FillFormStatus(CPostForm& postForm, LPCWSTR lpszStatus);
	DWORD CheckLicense(
				LPCTSTR lpszLicFileName, 
				DWORD dwProductId, 
				LPCTSTR lpszDestFolder,
				LPTSTR lpszLicDestPathName,
				DWORD& dwNameOfs
	);
	DWORD SaveKeyToTemp(CHttpMessage& httpMessage, LPTSTR lpszSavedFileName);


private:
	CAppData*	m_pAppData;
};

//-----------------------------------------------------------------------------

#endif // KFD_WORKERTHR_H
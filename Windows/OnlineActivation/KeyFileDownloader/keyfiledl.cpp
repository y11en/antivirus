#include <fstream.h>
#include "cmndefs.h"
#include "workerthr.h"

//-----------------------------------------------------------------------------

//CSafeFlag thrStopCond;
//CSafeHandle thrHandle;
//CSafeString keyFileName;

//-----------------------------------------------------------------------------

int StartDownloadKeyFile_I(
	KFD_NOTIFICATION_CALLBACK fCallback,
	void*			pContext,
	HWND			hDialog,
	int				nDlgItem,
	const CUserData	*pUserData,
	bool            bSync
)
{
	// check thread for running
	if (ThreadHandle::Instance())
	{
		DWORD dwExitCode;
		GetExitCodeThread(ThreadHandle::Instance(), &dwExitCode);
		if (dwExitCode != STILL_ACTIVE)
		{
			CloseHandle(ThreadHandle::Instance());
			ThreadHandle::Instance() = NULL;
		}
		else
			return ERR_THR_STARTED;
	}

	// set callback pointer
	Callback::Instance().Set(fCallback, pContext);

	if (!Callback::Instance().IsSet())
	{
		// check HWND
		if (!IsWindow(hDialog))
			return ERR_BAD_HWND;

		// check resource identifier
		if (GetDlgItem(hDialog, nDlgItem) == NULL)
			return ERR_BAD_RID;
	}

	// check destination folder
	if (pUserData->lpszDstFolder == NULL || !pUserData->lpszDstFolder[0])
		return ERR_BAD_FOLDER;

	// check key
	if (pUserData->lpszKeyNum == NULL || !pUserData->lpszKeyNum[0])
		return ERR_BAD_KEY;

	// check URL
	if (pUserData->lpszUrl == NULL || !pUserData->lpszUrl[0])
		return ERR_BAD_URL;

	// buffers length
	int nFldrBufSz, nUrlBufSz, nAgentBufSz;

#ifndef UNICODE
	// get required buffer length
	nFldrBufSz = WideCharToMultiByte(CP_ACP, 0, pUserData->lpszDstFolder, -1, NULL, 0, NULL, NULL);
	nUrlBufSz = WideCharToMultiByte(CP_ACP, 0, pUserData->lpszUrl, -1, NULL, 0, NULL, NULL);
	nAgentBufSz = WideCharToMultiByte(CP_ACP, 0, pUserData->lpszAgent, -1, NULL, 0, NULL, NULL);
#else
	nFldrBufSz = lstrlen(pUserData->lpszDstFolder);
	nUrlBufSz = lstrlen(pUserData->lpszUrl);
	nAgentBufSz = lstrlen(pUserData->lpszAgent);
#endif

	if (!(nFldrBufSz && nUrlBufSz && nAgentBufSz))
		return ERR_CONV_ANSI;

	// allocate memory
	LPTSTR lpszFldrAnsi = new TCHAR[nFldrBufSz];
	LPTSTR lpszUrlAnsi = new TCHAR[nUrlBufSz];
	LPTSTR lpszAgentAnsi = new TCHAR[nAgentBufSz];
	CAppData *pAppData = new CAppData();


	if (!(lpszFldrAnsi && lpszUrlAnsi && lpszAgentAnsi && pAppData))
	{
		delete[] lpszFldrAnsi;
		delete[] lpszUrlAnsi;
		delete[] lpszAgentAnsi;
		delete pAppData;
		return ERR_MEM_ALLOC;
	}

#ifndef UNICODE
	// convert to ANSI
	if(!(WideCharToMultiByte(CP_ACP, 0, pUserData->lpszDstFolder, -1, lpszFldrAnsi, nFldrBufSz, NULL, NULL) &&
			WideCharToMultiByte(CP_ACP, 0, pUserData->lpszUrl, -1, lpszUrlAnsi, nUrlBufSz, NULL, NULL) &&
			WideCharToMultiByte(CP_ACP, 0, pUserData->lpszAgent, -1, lpszAgentAnsi, nAgentBufSz, NULL, NULL)))
	{
		delete[] lpszFldrAnsi;
		delete[] lpszUrlAnsi;
		delete[] lpszAgentAnsi;
		delete pAppData;
		return ERR_CONV_ANSI;
	}
#else
	lstrcpy(lpszFldrAnsi, pUserData->lpszDstFolder);
	lstrcpy(lpszUrlAnsi, pUserData->lpszUrl);
	lstrcpy(lpszAgentAnsi, pUserData->lpszAgent);
#endif

	// check for directory existance
	if (GetFileAttributes(lpszFldrAnsi) == INVALID_FILE_ATTRIBUTES)
	{
		delete[] lpszFldrAnsi;
		delete[] lpszUrlAnsi;
		delete[] lpszAgentAnsi;
		delete pAppData;
		return ERR_BAD_FOLDER;
	}

	URL_COMPONENTS urlComp;
	TCHAR szServer[MAX_PATH], szResource[MAX_PATH];
	TCHAR szUserName[MAX_PATH], szPassword[MAX_PATH];

	ZeroMemory(&urlComp, sizeof(urlComp));
	urlComp.dwStructSize = sizeof(urlComp);
	urlComp.lpszHostName = szServer;
	urlComp.dwHostNameLength = MAX_PATH;
	urlComp.lpszUserName = szUserName;
	urlComp.dwUserNameLength = MAX_PATH;
	urlComp.lpszPassword = szPassword;
	urlComp.dwPasswordLength = MAX_PATH;
	urlComp.lpszUrlPath = szResource;
	urlComp.dwUrlPathLength = MAX_PATH;

	if (!InternetCrackUrl(lpszUrlAnsi, nUrlBufSz, 0, &urlComp))
	{
		delete[] lpszFldrAnsi;
		delete[] lpszUrlAnsi;
		delete[] lpszAgentAnsi;
		delete pAppData;
		return ERR_BAD_URL;
	}

	// set thread cancel condition to false
	StopFlag::Instance() = FALSE;

	// set parameters
	pAppData->SetParameters(
					hDialog,
					nDlgItem,
					pUserData->nProdId,
					pUserData->lpszProdVer,
					lpszAgentAnsi,
					szServer,
					urlComp.nPort,
					szUserName,
					szPassword,
					szResource,
					lpszFldrAnsi,
					pUserData->lpszKeyNum,
					pUserData->lpszCompany,
					pUserData->lpszName,
					pUserData->lpszCountry,
					pUserData->lpszCity,
					pUserData->lpszAddress,
					pUserData->lpszTelephone,
					pUserData->lpszFax,
					pUserData->lpszEmail,
					pUserData->lpszWww
	);


	delete[] lpszFldrAnsi;
	delete[] lpszUrlAnsi;
	delete[] lpszAgentAnsi;

	// don't delete pAppData here

	if( bSync )
		return WorkerThreadFunc(pAppData) == ERROR_SUCCESS ? ERR_SUCCESS : ERR_BAD_KEY;
	
	DWORD dwThrId;
	HANDLE hThread;
	hThread = CreateThread(NULL, 0, WorkerThreadFunc, pAppData, 0, &dwThrId);

	if (hThread == NULL)
	{
		delete pAppData;
		return ERR_THR_CREATE;
	}
	
	ThreadHandle::Instance() = hThread;
	return ERR_SUCCESS;
}

//-----------------------------------------------------------------------------

int WINAPI StartDownloadKeyFileCallback(
	KFD_NOTIFICATION_CALLBACK fCallback,
	void* pContext,
	const CUserData	*pUserData
)
{
	return StartDownloadKeyFile_I(fCallback, pContext, 0, 0, pUserData, false);
}

//-----------------------------------------------------------------------------

int WINAPI StartDownloadKeyFileSyncCallback(
	KFD_NOTIFICATION_CALLBACK fCallback,
	void* pContext,
	const CUserData	*pUserData
)
{
	return StartDownloadKeyFile_I(fCallback, pContext, 0, 0, pUserData, true);
}

//-----------------------------------------------------------------------------

int WINAPI StartDownloadKeyFile(
	HWND			hDialog,
	int				nDlgItem,
	const CUserData	*pUserData
)
{
	return StartDownloadKeyFile_I(0, 0, hDialog, nDlgItem, pUserData, false);
}

//-----------------------------------------------------------------------------

int WINAPI StopDownloadKeyFile()
{
	// set cancellation flag
	StopFlag::Instance() = TRUE;

	// if thread is not created
	if (!ThreadHandle::Instance())
		return ERR_THR_STOPPED;

	// if thread have already exited
	DWORD dwExitCode;
	GetExitCodeThread(ThreadHandle::Instance(), &dwExitCode);
	if (dwExitCode != STILL_ACTIVE)
	{
		CloseHandle(ThreadHandle::Instance());
		ThreadHandle::Instance()= NULL;
		return ERR_THR_STOPPED;
	}

	return ERR_SUCCESS;
}

//-----------------------------------------------------------------------------

int WINAPI InitLibrary()
{
	return ERR_SUCCESS;
}

//-----------------------------------------------------------------------------

int WINAPI DeinitLibrary()
{
	if (ThreadHandle::Instance())
	{
		// if thread is running set cancellation flag and wait
		DWORD dwExitCode;
		GetExitCodeThread(ThreadHandle::Instance(), &dwExitCode);
		if (dwExitCode == STILL_ACTIVE)
		{
			StopFlag::Instance() = TRUE;
			WaitForSingleObject(ThreadHandle::Instance(), INFINITE);
		}

		CloseHandle(ThreadHandle::Instance());
		ThreadHandle::Instance() = NULL;
	}

	return ERR_SUCCESS;
	
}

//-----------------------------------------------------------------------------

int WINAPI GetKeyFileName(TCHAR* szName, int* pLength)
{
	const TCHAR* szFileName = KeyFileName::Instance().GetValue();
	int nFileNameLen = lstrlen(szFileName);

	if (nFileNameLen + 1 > *pLength)
	{
		*pLength = nFileNameLen + 1;
		return 0;
	}
	
	if(lstrcpy(szName, szFileName))
		return nFileNameLen;
	else
	{
		szName[0] = TEXT('\0');
		return 0;
	}
}


//-----------------------------------------------------------------------------

int WINAPI SetCredentialProxy(TCHAR* szUserName, TCHAR* szPassword)
{
	HINTERNET hInet = HttpHandle::Instance();

	if (!InternetSetOption(
					hInet, 
					INTERNET_OPTION_PROXY_USERNAME, 
					szUserName, 
					lstrlen(szUserName) + 1))
		return GetLastError();

	if (!InternetSetOption(
					hInet, 
					INTERNET_OPTION_PROXY_PASSWORD, 
					szPassword, 
					lstrlen(szPassword) + 1))
		return GetLastError();

	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

int WINAPI SetCredentialServer(TCHAR* szUserName, TCHAR* szPassword)
{
	HINTERNET hInet = HttpHandle::Instance();

	if (!InternetSetOption(
					hInet, 
					INTERNET_OPTION_USERNAME, 
					szUserName, 
					lstrlen(szUserName) + 1))
		return GetLastError();

	if (!InternetSetOption(
					hInet, 
					INTERNET_OPTION_PASSWORD, 
					szPassword, 
					lstrlen(szPassword) + 1))
		return GetLastError();

	return ERROR_SUCCESS;
	
}

//-----------------------------------------------------------------------------

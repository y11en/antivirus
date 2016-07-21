#include <vector>
#include "cmndefs.h"
#include "workerthr.h"

//-----------------------------------------------------------------------------

bool WINAPI IsActivationServerError(const int msg_code)
{
	return (msg_code & SERVER_ERROR_MASK) != 0;
}

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
	if (InstanceData::Instance().thread_handle)
	{
		DWORD dwExitCode;
		GetExitCodeThread(InstanceData::Instance().thread_handle, &dwExitCode);
		if (dwExitCode != STILL_ACTIVE)
		{
			CloseHandle(InstanceData::Instance().thread_handle);
			InstanceData::Instance().thread_handle = NULL;
		}
		else
			return ERR_THR_STARTED;
	}

	// set callback pointer
	InstanceData::Instance().callback.Set(fCallback, pContext);

	if (!InstanceData::Instance().callback.IsSet())
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
	int nFldrBufSz, nUrlBufSz, nAgentBufSz, nProxySz, nBypassSz;

#ifndef UNICODE
	// get required buffer length
	nFldrBufSz = WideCharToMultiByte(CP_ACP, 0, pUserData->lpszDstFolder, -1, NULL, 0, NULL, NULL);
	nUrlBufSz = WideCharToMultiByte(CP_ACP, 0, pUserData->lpszUrl, -1, NULL, 0, NULL, NULL);
	nAgentBufSz = pUserData->lpszAgent ? WideCharToMultiByte(CP_ACP, 0, pUserData->lpszAgent, -1, NULL, 0, NULL, NULL) : 0;
	nProxySz =  pUserData->lpszProxyList ? WideCharToMultiByte(CP_ACP, 0, pUserData->lpszProxyList, -1, NULL, 0, NULL, NULL) : 0;
	nBypassSz = pUserData->lpszProxyBypass ? WideCharToMultiByte(CP_ACP, 0, pUserData->lpszProxyBypass, -1, NULL, 0, NULL, NULL) :0;
#else
	nFldrBufSz = lstrlen(pUserData->lpszDstFolder);
	nUrlBufSz = lstrlen(pUserData->lpszUrl);
	nAgentBufSz = pUserData->lpszAgent ? lstrlen(pUserData->lpszAgent) : 0;
	nProxySz = pUserData->lpszProxyList ? lstrlen(pUserData->lpszProxyList) : 0;
	nBypassSz = pUserData->lpszProxyBypass ? lstrlen(pUserData->lpszProxyBypass) : 0;
#endif

	if (!(nFldrBufSz && nUrlBufSz))
		return ERR_CONV_ANSI;

	std::auto_ptr<CAppData> pAppData(new CAppData());

	std::vector<TCHAR> 
		lpszFldrAnsi(nFldrBufSz + 1, 0), 
		lpszUrlAnsi(nUrlBufSz + 1, 0), 
		lpszAgentAnsi(nAgentBufSz + 1, 0),
		lpszProxyAnsi(nProxySz + 1, 0),
		lpszBypassAnsi(nBypassSz + 1, 0);

#ifndef UNICODE
	// convert to ANSI
	WideCharToMultiByte(CP_ACP, 0, pUserData->lpszDstFolder, -1, &lpszFldrAnsi[0], nFldrBufSz, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, pUserData->lpszUrl, -1, &lpszUrlAnsi[0], nUrlBufSz, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, pUserData->lpszAgent, -1, &lpszAgentAnsi[0], nAgentBufSz, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, pUserData->lpszProxyList, -1, &lpszProxyAnsi[0], nProxySz, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, pUserData->lpszProxyBypass, -1, &lpszBypassAnsi[0], nBypassSz, NULL, NULL);
#else
	lstrcpy(&lpszFldrAnsi[0], pUserData->lpszDstFolder);
	lstrcpy(&lpszUrlAnsi[0], pUserData->lpszUrl);
	lstrcpy(&lpszAgentAnsi[0], pUserData->lpszAgent);
	lstrcpy(&lpszProxyAnsi[0], pUserData->lpszProxyList);
	lstrcpy(&lpszBypassAnsi[0], pUserData->lpszProxyBypass);
#endif

	// check for directory existance
	if (GetFileAttributes(&lpszFldrAnsi[0]) == INVALID_FILE_ATTRIBUTES)
		return ERR_BAD_FOLDER;

	// set thread cancel condition to false
	InstanceData::Instance().stop_flag = FALSE;

	// set parameters
	pAppData->SetParameters(
					hDialog,
					nDlgItem,
					pUserData->nProdId,
					pUserData->lpszProdVer,
					&lpszAgentAnsi[0],
					&lpszFldrAnsi[0],
					&lpszUrlAnsi[0],
					pUserData->lpszKeyNum,
					pUserData->lpszCompany,
					pUserData->lpszName,
					pUserData->lpszCountry,
					pUserData->lpszCity,
					pUserData->lpszAddress,
					pUserData->lpszTelephone,
					pUserData->lpszFax,
					pUserData->lpszEmail,
					pUserData->lpszWww,
					pUserData->bUseIeSettings,
					pUserData->bUseProxy,
					pUserData->bTryAutoAuth,
					&lpszProxyAnsi[0],
					&lpszBypassAnsi[0]
	);

	// don't delete pAppData here

	if( bSync )
		return WorkerThreadFunc(pAppData.release()) == ERROR_SUCCESS ? ERR_SUCCESS : ERR_UNSPECIFIED;
	
	DWORD dwThrId;
	HANDLE hThread;
	hThread = CreateThread(NULL, 0, WorkerThreadFunc, pAppData.get(), CREATE_SUSPENDED, &dwThrId);

	if (hThread == NULL)
		return ERR_THR_CREATE;
	
	InstanceData::Instance().thread_handle = hThread;
	pAppData.release();
	ResumeThread(hThread);
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
	InstanceData::Instance().stop_flag = TRUE;

	// if thread is not created
	if (!InstanceData::Instance().thread_handle)
		return ERR_THR_STOPPED;

	// if thread have already exited
	DWORD dwExitCode;
	GetExitCodeThread(InstanceData::Instance().thread_handle, &dwExitCode);
	if (dwExitCode != STILL_ACTIVE)
	{
		CloseHandle(InstanceData::Instance().thread_handle);
		InstanceData::Instance().thread_handle = NULL;
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
	if (InstanceData::Instance().thread_handle)
	{
		// if thread is running set cancellation flag and wait
		DWORD dwExitCode;
		GetExitCodeThread(InstanceData::Instance().thread_handle, &dwExitCode);
		if (dwExitCode == STILL_ACTIVE)
		{
			InstanceData::Instance().stop_flag = TRUE;
			WaitForSingleObject(InstanceData::Instance().thread_handle, INFINITE);
		}

		CloseHandle(InstanceData::Instance().thread_handle);
		InstanceData::Instance().thread_handle = NULL;
	}

	return ERR_SUCCESS;
	
}

//-----------------------------------------------------------------------------

int WINAPI GetKeyFileName(TCHAR* szName, int* pLength)
{
	const TCHAR* szFileName = 
		InstanceData::Instance().key_file_name.GetValue();

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
	if (!(szUserName && szPassword))
		return ERROR_INVALID_PARAMETER;

	InstanceData::Instance().proxy_login.SetValue(szUserName);
	InstanceData::Instance().proxy_pass.SetValue(szPassword);

	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

int WINAPI SetCredentialServer(TCHAR* szUserName, TCHAR* szPassword)
{
	HINTERNET hInet = InstanceData::Instance().inet_handle;

	if (!hInet)
		return ERROR_INVALID_HANDLE;

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

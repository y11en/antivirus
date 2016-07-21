#include <stdio.h>
#include "workerthr.h"
#include <klkeyrec.h>

#include "keyfiledl.h"

//-----------------------------------------------------------------------------

const TCHAR DEFAULT_LIC_EXT[]		= TEXT(".key");
const int RETRY_COUNT	= 3;
const int FILE_NAME_OFS	= 8;


//-----------------------------------------------------------------------------

const DWORD	RC_LIC_GOOD			= 0;
const DWORD RC_LIC_BAD			= 1;
const DWORD RC_LIC_CORRUPTED	= 2;

//-----------------------------------------------------------------------------

//extern CSafeFlag thrStopCond;
//extern CSafeString keyFileName;

//-----------------------------------------------------------------------------

DWORD WINAPI WorkerThreadFunc(LPVOID lpParam)
{
	CAppData *lpAppData = (CAppData*)lpParam;
	CWorkerThread workerThread(lpAppData);

	DWORD dwResult = workerThread.Run();
	HttpHandle::Instance() = NULL;

	return dwResult;
}

//-----------------------------------------------------------------------------

void* MemAlloc(AVP_uint nBytes)
{
	return HeapAlloc(GetProcessHeap(), 0, nBytes);
}

//-----------------------------------------------------------------------------

void MemFree(void* lpMem)
{
	HeapFree(GetProcessHeap(), 0, lpMem);
}

//-----------------------------------------------------------------------------

CWorkerThread::CWorkerThread(CAppData* pAppData): m_pAppData(pAppData)
{
}

//-----------------------------------------------------------------------------

DWORD CWorkerThread::Run()
{
	DWORD	dwOpResult;
	wchar_t	szStatusBuf[MAX_PATH] = L"";

	// initializing message
	SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_INITING);

	// create internet session
	CInetSession inetSession(m_pAppData->m_lpszAgentName);
	if ((dwOpResult = inetSession.GetInitErrCode()) != ERROR_SUCCESS)
	{
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_INITING);
		return dwOpResult;
	}

	// create HTTP message object
	CHttpMessage httpMessage(&inetSession, m_pAppData);
	if ((dwOpResult = inetSession.GetInitErrCode()) != ERROR_SUCCESS)
	{
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_INITING);
		return dwOpResult;
	}

	HttpHandle::Instance() = httpMessage;

	// create form to submit data
	CPostForm postForm(m_pAppData->m_lpszResource);

	// about to post message
	SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_POSTING_DATA);

	// filling key data
	if ((dwOpResult = FillFormKeyData(postForm)) != ERROR_SUCCESS)
	{
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_FILLDATA);
		return dwOpResult;
	}

	// main retrying loop
	for (int i = 0; i < RETRY_COUNT; i++)
	{
		// authorization loop
		for (bool bContinue = true; bContinue; )
		{
			// enable callback to notify status
			httpMessage.EnableCallBack(TRUE);

			// post form data and check for cancellation error
			if (((dwOpResult = httpMessage.SendRequest(postForm)) != ERROR_SUCCESS) && !StopFlag::Instance())
			{
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SENDING);
				return dwOpResult;
			}

			// request accomplished so disable callback
			httpMessage.EnableCallBack(FALSE);

			// check for cancellation
			if (StopFlag::Instance())
			{
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CANCELLED);
				return ERROR_CANCELLED;
			}

			// check response status
			BOOL bIsGoodStatus;
			if ((dwOpResult = CheckReqStatus(httpMessage, bIsGoodStatus)) != ERROR_SUCCESS)
			{
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_GETSTATUS);
				return dwOpResult;
			}

			// if wrong reply received
			if (!bIsGoodStatus)
			{
				DWORD dwResponse;
				httpMessage.GetResponseCode(&dwResponse);
				// if authorization required - retry (client must set credentials in the callback)
				if ((dwResponse == HTTP_STATUS_DENIED) || (dwResponse == HTTP_STATUS_PROXY_AUTH_REQ))
					continue;
				return ERROR_INVALID_DATA;
			}
			bContinue = !bIsGoodStatus;
		}

		// about to save license
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_SAVING_TMP);

		TCHAR szTempFileName[MAX_PATH];
		TCHAR szDestFileName[MAX_PATH];
		DWORD dwFileNameOfs = 0;

		// saving license to temporary file
		if ((dwOpResult = SaveKeyToTemp(httpMessage, szTempFileName)) != ERROR_SUCCESS)
		{
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SAVETEMP);
			return dwOpResult;
		}

		// about ot check license
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_VERIFYING);

		// check license
		DWORD dwLicCheck = CheckLicense(
									szTempFileName, 
									m_pAppData->m_nProdId, 
									m_pAppData->m_lpszDstFolder,
									szDestFileName,
									dwFileNameOfs
							);

		if (dwLicCheck == RC_LIC_GOOD)
		{
			// if good license - move it to permanent location and break
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_SAVING_DST);
			// if file already exists
			DeleteFile(szDestFileName);
			// move to destination
			if (!MoveFile(szTempFileName, szDestFileName))
			{
				dwOpResult = GetLastError();
				DeleteFile(szTempFileName);
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SAVEDST);
				return dwOpResult;
			}
			KeyFileName::Instance().SetValue(szDestFileName + dwFileNameOfs);
			break;
		}
		else if (dwLicCheck == RC_LIC_BAD)
		{
			// if bad license - delete it send status and exit
			DeleteFile(szTempFileName);

			// about to post status
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_POSTING_STAT);

			// filling status
			swprintf(szStatusBuf, MSG_LIC_BAD, m_pAppData->m_nProdId, m_pAppData->m_lpszProdVer);
			if ((dwOpResult = FillFormStatus(postForm, szStatusBuf)) != ERROR_SUCCESS)
			{
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_FILLDATA);
				return dwOpResult;
			}

			// enable callback to notify status
			httpMessage.EnableCallBack(TRUE);

			// post form data and check for cancellation error
			if (((dwOpResult = httpMessage.SendRequest(postForm)) != ERROR_SUCCESS) && !StopFlag::Instance())
			{
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SENDING);
				return dwOpResult;
			}

			// request accomplished so disable callback
			httpMessage.EnableCallBack(FALSE);

			// check for cancellation
			if (StopFlag::Instance())
			{
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CANCELLED);
				return ERROR_CANCELLED;
			}

			// send wrong license file message
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_BADLICENSE);
			return ERROR_INVALID_DATA;
		}
		else // dwLicCheck == RC_LIC_CORRUPTED
		{
			// if corrupted license - retrying
			DeleteFile(szTempFileName);
			if (i == RETRY_COUNT - 1) // last try finished
			{
				// about to post status
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_POSTING_STAT);

				// filling status
				swprintf(szStatusBuf, MSG_LIC_CORRUPT, m_pAppData->m_nProdId, m_pAppData->m_lpszProdVer);
				if ((dwOpResult = FillFormStatus(postForm, szStatusBuf)) != ERROR_SUCCESS)
				{
					SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_FILLDATA);
					return dwOpResult;
				}

				// enable callback to notify status
				httpMessage.EnableCallBack(TRUE);

				// post form data and check for cancellation error
				if (((dwOpResult = httpMessage.SendRequest(postForm)) != ERROR_SUCCESS) && !StopFlag::Instance())
				{
					SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SENDING);
					return dwOpResult;
				}

				// request accomplished so disable callback
				httpMessage.EnableCallBack(FALSE);

				// check for cancellation
				if (StopFlag::Instance())
				{
					SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CANCELLED);
					return ERROR_CANCELLED;
				}
				
				// send corrupted license file message
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CORRUPTLIC);
				return ERROR_INVALID_DATA;
			}
			else
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_RETRYING);
		}
	} // end retrying loop

	// about to post status
	SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_POSTING_STAT);
	
	// filling acknowledgement
	swprintf(szStatusBuf, MSG_LIC_OK, m_pAppData->m_nProdId, m_pAppData->m_lpszProdVer);
	if ((dwOpResult = FillFormStatus(postForm, szStatusBuf)) != ERROR_SUCCESS)
	{
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_FILLDATA);
		return dwOpResult;
	}

	httpMessage.EnableCallBack(TRUE);

	// posting acknowledgement and check for cancellation error
	if (((dwOpResult = httpMessage.SendRequest(postForm)) != ERROR_SUCCESS) && !StopFlag::Instance())
	{
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SENDING);
		return dwOpResult;
	}

	httpMessage.EnableCallBack(FALSE);

	// check for cancellation after operation
	if (StopFlag::Instance())
	{
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CANCELLED);
		return ERROR_CANCELLED;
	}

	SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_COMPLETED);
	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

DWORD CWorkerThread::CheckReqStatus(CHttpMessage& httpMsg, BOOL& bIsGood)
{
	char szGoodContent[] = "application/";
	const DWORD CNT_LENGTH = strlen(szGoodContent);

	int nCompResult = 0;
	DWORD dwStatus, dwOpResult, dwAnswSize = 0;
	char *lpszRecvContent = NULL;

	if ((dwOpResult = httpMsg.GetResponseCode(&dwStatus)) != ERROR_SUCCESS)
		return dwOpResult;

	// analyze only 2xx, 4xx, 5xx status codes
	switch (dwStatus / 100)
	{
	// succesful
	case 2:
		// check for proper Content-Type ("application/*")

		dwOpResult = httpMsg.QueryInfo(NULL, &dwAnswSize, HTTP_QUERY_CONTENT_TYPE);
		if (!(dwOpResult == ERROR_SUCCESS || dwOpResult == ERROR_INSUFFICIENT_BUFFER))
			return dwOpResult;

		if ((lpszRecvContent = (char*) HeapAlloc(GetProcessHeap(), 0, dwAnswSize)) == NULL)
			return ERROR_NOT_ENOUGH_MEMORY;
		
		// query Content-Type
		dwOpResult = httpMsg.QueryInfo(lpszRecvContent, &dwAnswSize, HTTP_QUERY_CONTENT_TYPE);
		if (dwOpResult != ERROR_SUCCESS)
			return dwOpResult;
		
		// compare answer with "application/"
		nCompResult = strncmp(
							szGoodContent, 
							lpszRecvContent, 
							CNT_LENGTH < dwAnswSize ? CNT_LENGTH : dwAnswSize);

		HeapFree(GetProcessHeap(), 0, lpszRecvContent);

		if (!nCompResult)
			bIsGood = TRUE;
		else 
		{
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_BADCONTENT);
			bIsGood = FALSE;
		}
		break;

	// client error
	case 4:
		switch (dwStatus)
		{
		case HTTP_STATUS_BAD_REQUEST:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_SYNTHAX);
			break;
		case HTTP_STATUS_DENIED:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_UNAUTH);
			break;
		case HTTP_STATUS_FORBIDDEN:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_FORBID);
			break;
		case HTTP_STATUS_NOT_FOUND:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_NOTFND);
			break;
		case HTTP_STATUS_BAD_METHOD:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_BADMETH);
			break;
		case HTTP_STATUS_NONE_ACCEPTABLE:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_NONACPT);
			break;
		case HTTP_STATUS_PROXY_AUTH_REQ:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_PRXAUTHRQ);
			break;
		case HTTP_STATUS_REQUEST_TIMEOUT:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_TIMOUT);
			break;
		case HTTP_STATUS_CONFLICT:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_STCONFL);
			break;
		case HTTP_STATUS_GONE:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_GONE);
			break;
		case HTTP_STATUS_LENGTH_REQUIRED:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_LENREQ);
			break;
		case HTTP_STATUS_PRECOND_FAILED:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_PREFAIL);
			break;
		case HTTP_STATUS_REQUEST_TOO_LARGE:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_TOOLARGE);
			break;
		case HTTP_STATUS_URI_TOO_LONG:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_URILONG);
			break;
		case HTTP_STATUS_UNSUPPORTED_MEDIA:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_UNSMEDIA);
			break;
		default:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CLNT_OTHER);
			break;
		}
		bIsGood = FALSE;
		break;
	// server error
	case 5:
		switch (dwStatus)
		{
		case HTTP_STATUS_SERVER_ERROR:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SERV_INT);
			break;
		case HTTP_STATUS_NOT_SUPPORTED:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SERV_NSUP);
			break;
		case HTTP_STATUS_BAD_GATEWAY:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SERV_BADGATE);
			break;
		case HTTP_STATUS_SERVICE_UNAVAIL:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SERV_UNVL);
			break;
		case HTTP_STATUS_GATEWAY_TIMEOUT:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SERV_GATIMO);
			break;
		case HTTP_STATUS_VERSION_NOT_SUP:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SERV_PROT);
			break;
		default:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SERV_OTHER);
			break;
		}
		bIsGood = FALSE;
		break;
	default:
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_HTTP_OTHER);
		bIsGood = FALSE;
		break;
	}

	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

DWORD CWorkerThread::CheckLicense(
						LPCTSTR lpszLicFileName, 
						DWORD dwProductId, 
						LPCTSTR lpszDestFolder,
						LPTSTR lpszLicDestPathName,
						DWORD& dwNameOfs
)
{
	DWORD dwResult = RC_LIC_BAD;

	KLKR_Init_Library(MemAlloc, MemFree);

	if (KLKR_SetKeyFileName(lpszLicFileName))
	{
		if ((KLKR_GetKeyPLPositionI() == dwProductId) && 
			!KLKR_GetKeyIsTrialI() && !KLKR_GetKeyIsOEMI() && !KLKR_GetKeyIsUpgradeI())
		{
			TCHAR lpszDate[32];
			AVP_bool bExpired = TRUE;
			KLKR_GetKeyExpDateExExS(lpszDate, &bExpired, FALSE);
			
			if (!bExpired)
				dwResult = RC_LIC_GOOD;
			else
				dwResult = RC_LIC_BAD;
		}
		else
			dwResult = RC_LIC_BAD;
	}
	else
		dwResult = RC_LIC_CORRUPTED;


	// form destination license file name
	if (dwResult == RC_LIC_GOOD)
	{
		lstrcpy(lpszLicDestPathName, lpszDestFolder);
		int nPathLen = lstrlen(lpszDestFolder);
		if (lpszLicDestPathName[nPathLen - 1] != TEXT('\\'))
		{
			lpszLicDestPathName[nPathLen++] = TEXT('\\');
			lpszLicDestPathName[nPathLen] = TEXT('\0');
		}

		TCHAR szKeySerial[MAX_PATH];
		KLKR_GetKeySerialNumberSL(szKeySerial, MAX_PATH);
		int nNameOfs = lstrlen(szKeySerial) - FILE_NAME_OFS;
		dwNameOfs = lstrlen(lpszLicDestPathName);
		lstrcat(lpszLicDestPathName, szKeySerial + nNameOfs);
		lstrcat(lpszLicDestPathName, DEFAULT_LIC_EXT);
	}


	KLKR_Shutdown_Library();

	return dwResult;
}

//-----------------------------------------------------------------------------

DWORD CWorkerThread::FillFormKeyData(CPostForm& postForm)
{
	DWORD dwResult;

	postForm.ClearInput();

	dwResult = postForm.AddInput(L"keynum", m_pAppData->m_lpszKeyNum);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	dwResult = postForm.AddInput(L"company", m_pAppData->m_lpszCompany);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	dwResult = postForm.AddInput(L"name", m_pAppData->m_lpszName);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	dwResult = postForm.AddInput(L"country", m_pAppData->m_lpszCountry);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	dwResult = postForm.AddInput(L"city", m_pAppData->m_lpszCity);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	dwResult = postForm.AddInput(L"address", m_pAppData->m_lpszAddress);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	dwResult = postForm.AddInput(L"telephone", m_pAppData->m_lpszTelephone);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	dwResult = postForm.AddInput(L"fax", m_pAppData->m_lpszFax);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	dwResult = postForm.AddInput(L"email", m_pAppData->m_lpszEmail);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	dwResult = postForm.AddInput(L"www", m_pAppData->m_lpszWww);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

DWORD CWorkerThread::FillFormStatus(CPostForm& postForm, LPCWSTR lpszStatus)
{
	DWORD dwResult;

	postForm.ClearInput();

	dwResult = postForm.AddInput(L"keynum", m_pAppData->m_lpszKeyNum);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	dwResult = postForm.AddInput(L"status", lpszStatus);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

DWORD CWorkerThread::SaveKeyToTemp(CHttpMessage& httpMessage, LPTSTR lpszSavedFileName)
{
	TCHAR szTempPath[MAX_PATH];

	if (!GetTempPath(MAX_PATH, szTempPath))
		return GetLastError();

	if (!GetTempFileName(szTempPath, TEXT(""), 0, lpszSavedFileName))
		return GetLastError();

	return httpMessage.SaveToFile(lpszSavedFileName);
}

//-----------------------------------------------------------------------------

CWorkerThread::~CWorkerThread()
{
	// free resources allocated by StartDownloadKeyFile()
	delete m_pAppData;
	m_pAppData = NULL;
}
//=============================================================================

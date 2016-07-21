#include <stdio.h>
#include <tchar.h>
#include <vector>
#include "workerthr.h"
#include <klkeyrec.h>
#include <time.h>

#include "keyfiledl.h"

//-----------------------------------------------------------------------------

const TCHAR DEFAULT_LIC_EXT[]		= TEXT(".key");
const int FATAL_RETRY_COUNT			= 1;
const int FILE_NAME_OFS				= 8;

//-----------------------------------------------------------------------------

const DWORD	RC_LIC_GOOD				= 0;
const DWORD RC_LIC_BAD				= 1;
const DWORD RC_LIC_CORRUPTED		= 2;

const DWORD CONTENT_TYPE_UNKNOWN	= 0;
const DWORD CONTENT_TYPE_BINARY		= 1;
const DWORD CONTENT_TYPE_TEXT		= 2;

//-----------------------------------------------------------------------------

bool IsLocalTimeCorrect(CHttpMessage& httpMessage)
{
	SYSTEMTIME srvTime = { 0 }; DWORD dwTimeSize = sizeof(srvTime);
	// query server time
	if (httpMessage.QueryInfo(&srvTime, &dwTimeSize, HTTP_QUERY_DATE | HTTP_QUERY_FLAG_SYSTEMTIME) == ERROR_SUCCESS)
	{
		FILETIME loc = { 0 }, srv = { 0 };
		// get local computer time
		GetSystemTimeAsFileTime(&loc);
		// convert server time to file time
		SystemTimeToFileTime(&srvTime, &srv);
		// prepare for comparison
		ULARGE_INTEGER st = { srv.dwLowDateTime, srv.dwHighDateTime }, lt = { loc.dwLowDateTime, loc.dwHighDateTime };
		// dates difference in days
		ULONGLONG diff = (st.QuadPart > lt.QuadPart ? st.QuadPart - lt.QuadPart : lt.QuadPart - st.QuadPart) / 10000000 / 86400;
		// dates difference limit (1 day) exceeded
		return diff < 1;
	}
	// unable to get server time - ignore this case
	return true;
}

//-----------------------------------------------------------------------------

DWORD QueryContentType(CHttpMessage& httpMessage)
{
	const TCHAR szBinaryContent[] = TEXT("application/");
	const TCHAR szTextContent[] = TEXT("text/");

	DWORD dwAnswSize = 0;
	DWORD dwOpResult = httpMessage.QueryInfo(NULL, &dwAnswSize, HTTP_QUERY_CONTENT_TYPE);
	if (!(dwOpResult == ERROR_SUCCESS || dwOpResult == ERROR_INSUFFICIENT_BUFFER))
		return dwOpResult;

	std::vector<TCHAR> bufContent(dwAnswSize / sizeof(TCHAR) + 1, 0);
	// query Content-Type
	dwOpResult = httpMessage.QueryInfo(&bufContent[0], &dwAnswSize, HTTP_QUERY_CONTENT_TYPE);
	if (dwOpResult != ERROR_SUCCESS)
		return CONTENT_TYPE_UNKNOWN;
	
	// compare answer with "application/" (license binary data returned)
	if (!_tcsncmp(szBinaryContent, &bufContent[0], _tcslen(szBinaryContent) < bufContent.size() ? _tcslen(szBinaryContent) : bufContent.size()))
		return CONTENT_TYPE_BINARY;
	// compare answer with "text/" (server error returned)
	else if (!_tcsncmp(szTextContent, &bufContent[0], _tcslen(szTextContent) < bufContent.size() ? _tcslen(szTextContent) : bufContent.size()))
		return CONTENT_TYPE_TEXT;

	return CONTENT_TYPE_UNKNOWN;
}

//-----------------------------------------------------------------------------

int TranslateServerError(unsigned long error)
{
	int msgCode;
	
	switch(error)
	{
	case 600: // key not found
		msgCode = MSGERR_KEYNOTFOUND;
		break;
	case 601:
		msgCode = MSGERR_BLOCKED;
		break;
	case 602:
	case 603:
		msgCode = MSGERR_SRVERROR;
		break;
	case 620:
	case 623:
		msgCode = MSGERR_MALFORMEDREQ;
		break;
	case 621:
		msgCode = MSGERR_WRONGCUSTORPWD;
		break;
	case 622:
	case 624:
		msgCode = MSGERR_WRONACTGCODE;
		break;
	case 633:
		msgCode = MSGERR_PERIODEXPIRED;
		break;
	case 634:
		msgCode = MSGERR_NOMOREATTEMPTS;
		break;
	default:
		// unknown server error
		msgCode = SERVER_ERROR_MASK | error;
		break;
	}
	return msgCode;
}

//-----------------------------------------------------------------------------

DWORD WINAPI WorkerThreadFunc(LPVOID lpParam)
{
	CAppData *lpAppData = (CAppData*)lpParam;
	CWorkerThread workerThread(lpAppData);

	DWORD dwResult = workerThread.Run();
	InstanceData::Instance().inet_handle = NULL;
	InstanceData::Instance().proxy_login.SetValue(_T(""));
	InstanceData::Instance().proxy_pass.SetValue(_T(""));

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

CWorkerThread::CWorkerThread(CAppData* pAppData) : m_pAppData(pAppData)
{
	UUID uuid = {0};
    _TUCHAR* pszResult = NULL;
    UuidCreate(&uuid);
    UuidToString(&uuid, &pszResult);

#ifndef UNICODE
	DWORD bufsz = MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<char*>(pszResult), -1, NULL, 0);
	std::vector<wchar_t> buf(bufsz);
	bufsz = MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<char*>(pszResult), -1, &buf[0], bufsz);
	m_requestId.assign(&buf[0], bufsz);
#else
	m_requestId = pszResult;
#endif
    RpcStringFree(&pszResult);
}

//-----------------------------------------------------------------------------

DWORD CWorkerThread::Run()
{
	SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_INITING);

	DWORD dwAccessType = m_pAppData->m_bUseIeSettings  ? INTERNET_OPEN_TYPE_PRECONFIG : 
		m_pAppData->m_bUseProxy ? INTERNET_OPEN_TYPE_PROXY : INTERNET_OPEN_TYPE_DIRECT;

	// create internet session
	CInetSession inetSession
	(
		m_pAppData->m_lpszAgentName,
		dwAccessType,
		m_pAppData->m_lpszProxyList,
		m_pAppData->m_lpszProxyBypass
	);
	DWORD dwOpResult; 
	if ((dwOpResult = inetSession.GetInitErrCode()) != ERROR_SUCCESS)
	{
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_INITING);
		return dwOpResult;
	}

	TCHAR szTempPath[MAX_PATH] = { 0 }, szTempFileName[MAX_PATH] = { 0 };

	if (!GetTempPath(MAX_PATH, szTempPath))
	{
		dwOpResult = GetLastError();
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_INITING);
		return dwOpResult;
	}

	if (!GetTempFileName(szTempPath, TEXT(""), 0, szTempFileName))
	{
		dwOpResult = GetLastError();
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_INITING);
		return dwOpResult;
	}

	// fatal errors retrying loop
	for (int i = 0; i < FATAL_RETRY_COUNT; ++i)
	{
		// send retrying notification
		if (dwOpResult == ERROR_RETRY)
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_RETRYING);
		// download license
		dwOpResult = DownloadLicense(inetSession, szTempFileName);
		// retry is not requested - break the loop
		if (dwOpResult != ERROR_RETRY)
			break;
	}

	// failed to download license
	if (dwOpResult != ERROR_SUCCESS)
		return dwOpResult;

	// about ot check license
	SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_VERIFYING);

	TCHAR szDestFileName[MAX_PATH] = { 0 };
	DWORD dwFileNameOfs = 0;

	// check license
	DWORD dwLicCheck = CheckLicense
	(
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
		InstanceData::Instance().key_file_name.SetValue(szDestFileName + dwFileNameOfs);
	}
	else // dwLicCheck == RC_LIC_CORRUPTED
	{
		DeleteFile(szTempFileName);
		// send corrupted license file message
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, dwLicCheck == RC_LIC_BAD ? MSGERR_BADLICENSE : MSGERR_CORRUPTLIC);
		return ERROR_INVALID_DATA;
	}
	SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_COMPLETED);
	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

DWORD CWorkerThread::DownloadLicense(CInetSession& inetSession, LPCTSTR lpszLicFileName)
{
	// set initial url
	m_serverUrl = m_pAppData->m_lpszServerUrl;
	// retry after timeout or authentication loop
	while (true)
	{
		// check for cancellation
		if (InstanceData::Instance().stop_flag)
		{
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CANCELLED);
			return ERROR_CANCELLED;
		}

		// about to post message
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_POSTING_DATA);

		// create HTTP message object
		CHttpMessage httpMessage(m_serverUrl.c_str(), inetSession, m_pAppData);
		DWORD dwOpResult;
		if ((dwOpResult = httpMessage.GetInitErrCode()) != ERROR_SUCCESS)
		{
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SENDING);
			return dwOpResult;
		}
		
		// set authentication for proxy
		httpMessage.SetProxyAuth(InstanceData::Instance().proxy_login.GetValue(), 
			InstanceData::Instance().proxy_pass.GetValue());
		// set internet handle for stopping
		InstanceData::Instance().inet_handle = httpMessage;

		// create form to submit data
		CPostForm postForm(httpMessage.GetResource());
		// fill key data
		if ((dwOpResult = FillFormKeyData(postForm)) != ERROR_SUCCESS)
		{
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_FILLDATA);
			return dwOpResult;
		}
		
		// enable callback to notify status
		httpMessage.EnableCallBack(TRUE);

		// post form data and check for cancellation error
		if (((dwOpResult = httpMessage.SendRequest(postForm)) != ERROR_SUCCESS) && !InstanceData::Instance().stop_flag)
		{
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SENDING);
			return dwOpResult;
		}

		// request accomplished so disable callback
		httpMessage.EnableCallBack(FALSE);

		// check for cancellation
		if (InstanceData::Instance().stop_flag)
		{
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_CANCELLED);
			return ERROR_CANCELLED;
		}

		// check response status
		DWORD dwResponse;
		if ((dwOpResult = httpMessage.GetResponseCode(&dwResponse)) != ERROR_SUCCESS)
		{
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_GETSTATUS);
			return dwOpResult;
		}
		// analyze status code
		if (!AnalyzeStatusCode(dwResponse))
		{
			// authorization required - this is not a fatal error
			if ((dwResponse == HTTP_STATUS_DENIED) || (dwResponse == HTTP_STATUS_PROXY_AUTH_REQ))
				// client must set credentials in callback function
				continue;
			// retry from the beginning
			return ERROR_RETRY;
		}
		// server reply received
		switch(QueryContentType(httpMessage))
		{
		// text reply received - this is error or retry request
		case CONTENT_TYPE_TEXT:
			// analyze received content, continue if succeeded
			if ((dwOpResult = HandleServerReply(httpMessage)) != ERROR_SUCCESS)
				return dwOpResult;
			break;
		// license key successfully received
		case CONTENT_TYPE_BINARY:
			if (!IsLocalTimeCorrect(httpMessage))
			{
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_BADLOCALDATE);
				return ERROR_CANCELLED;
			}
			// about to save license
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_SAVING_TMP);
			if ((dwOpResult = httpMessage.SaveToFile(lpszLicFileName)) != ERROR_SUCCESS)
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_SAVETEMP);
			return dwOpResult;
		default:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_BADCONTENT);
			return MSGERR_BADCONTENT;
		}
	}
}

//-----------------------------------------------------------------------------

bool CWorkerThread::AnalyzeStatusCode(DWORD code)
{
	// analyze only 2xx, 4xx, 5xx response codes
	switch (code / 100)
	{
	// succesful
	case 2:
		return true;
		break;
	// client error
	case 4:
		switch (code)
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
		break;
	// server error
	case 5:
		switch (code)
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
		break;
	default:
		SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_HTTP_OTHER);
		break;
	}
	return false;
}

//-----------------------------------------------------------------------------

DWORD CWorkerThread::HandleServerReply(CHttpMessage& httpMsg)
{
	DWORD dwBytesRead = 0;
	char bufData[MAX_PATH] = { 0 };
	// read received content
	DWORD dwOpResult = httpMsg.ReadData(bufData, MAX_PATH - 1, &dwBytesRead);
	// unable to read content
	if (dwOpResult == ERROR_SUCCESS && bufData)
	{
		if (const char* token = strtok(bufData, " "))
		{
			// retry reply received
			if (!_stricmp(token, "RETRY"))
			{
				// extract timeout value
				if (!(token = strtok(0, " ")))
				{
					SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_BADCONTENT);
					return ERROR_INVALID_DATA;
				}
				long timeout = atol(token);
				// extract new url value
				if (!(token = strtok(0, " ")))
				{
					SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_BADCONTENT);
					return ERROR_INVALID_DATA;
				}
				m_serverUrl = token;
				// wait <timeout> seconds and check for cancellation
				for (time_t ct = time(0); (!InstanceData::Instance().stop_flag) && (time(0) - ct < timeout); Sleep(100));
				// retry again
				return ERROR_SUCCESS;
			}
			// server error code received
			else
			{
				// retrieve server error code
				long code = atol(token);
				// send message with specified server code
				SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, TranslateServerError(code));
				// 603 - fatal server error
				return code == 603 ? ERROR_RETRY : ERROR_INTERNAL_ERROR;
			}
		}
	}
	SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSGERR_BADCONTENT);
	return ERROR_INVALID_DATA;
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
		if (!KLKR_GetKeyIsOEMI() && !KLKR_GetKeyIsUpgradeI())
			dwResult = RC_LIC_GOOD;
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

	dwResult = postForm.AddInput(L"request_id", m_requestId.c_str());
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

	wchar_t buf[128];
	_itow(m_pAppData->m_nProdId, buf, 10);
	dwResult = postForm.AddInput(L"APP_ID", buf);
	if (dwResult != ERROR_SUCCESS)
		return dwResult;

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

CWorkerThread::~CWorkerThread()
{
	// free resources allocated by StartDownloadKeyFile()
	delete m_pAppData;
	m_pAppData = NULL;
}
//=============================================================================

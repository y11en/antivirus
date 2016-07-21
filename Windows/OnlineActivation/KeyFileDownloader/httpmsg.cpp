#include "keyfiledl.h"
#include "httpmsg.h"

const TCHAR *DEFAULT_ACCEPT_TYPES[] = {TEXT("text/*"), TEXT("application/*"), TEXT("*/*"), NULL};
const TCHAR  DEFAULT_FORM_HEADERS[] =  TEXT("Content-Type: application/x-www-form-urlencoded");

//-----------------------------------------------------------------------------

void CALLBACK InetCallBack(
					HINTERNET hInternet,
					DWORD_PTR dwContext,
					DWORD dwInternetStatus,
					LPVOID lpvStatusInformation,
					DWORD dwStatusInformationLength
)
{
	CInetBase *pInetBase = (CInetBase*) dwContext;
	if (pInetBase->IsCallBackEnabled())
		pInetBase->CallBackFunc(dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
}

//-----------------------------------------------------------------------------

CInetBase::CInetBase():
				m_dwInitErrorCode(ERROR_SUCCESS), 
				m_bIsCallBackEnabled(FALSE),
				m_bIsCallBackSet(FALSE)
{
}

//-----------------------------------------------------------------------------

DWORD CInetBase::EnableCallBack(BOOL bEnabled)
{
	INTERNET_STATUS_CALLBACK icbResult;

	if (bEnabled)
	{
		if (!m_bIsCallBackSet)
		{
			icbResult = 
				InternetSetStatusCallback(this->operator HINTERNET(), InetCallBack);

			if (icbResult == INTERNET_INVALID_STATUS_CALLBACK)
				return ERROR_INVALID_FUNCTION;
			m_bIsCallBackSet = TRUE;
		}
		m_bIsCallBackEnabled = TRUE;
	}
	else 
		m_bIsCallBackEnabled = FALSE;

	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

void CInetBase::CallBackFunc(
					DWORD dwInternetStatus, 
					LPVOID lpvStatusInformation,
					DWORD dwStatusInformationLength
)
{
	return;
}

//-----------------------------------------------------------------------------

CInetBase::~CInetBase()
{
}

//=============================================================================

CInetSession::CInetSession(
	LPCTSTR lpszAgentName,
	DWORD dwAccessType,
	LPCTSTR lpszProxyName,
	LPCTSTR lpszProxyBypass,
	DWORD dwFlags
):
			m_hSession(NULL), 
			m_lpszAgentName(NULL),
			m_dwAccessType(dwAccessType),
			m_lpszProxyName(NULL),
			m_lpszProxyBypass(NULL),
			m_dwFlags(dwFlags)
{
	// copy string arguments
	if (lpszAgentName)
	{
		m_lpszAgentName = new TCHAR[lstrlen(lpszAgentName) + 1];
		lstrcpy(m_lpszAgentName, lpszAgentName);
	}
	if (lpszProxyName)
	{
		m_lpszProxyName = new TCHAR[lstrlen(lpszProxyName) + 1];
		lstrcpy(m_lpszProxyName, lpszProxyName);
	}
	if (lpszProxyBypass)
	{
		m_lpszProxyBypass = new TCHAR[lstrlen(lpszProxyBypass) + 1];
		lstrcpy(m_lpszProxyBypass, lpszProxyBypass);
	}
	// initialize library
	m_hSession = InternetOpen(
						m_lpszAgentName,
						m_dwAccessType,
						m_lpszProxyName,
						m_lpszProxyBypass,
						m_dwFlags

					);
	// in case of errors
	if (!m_hSession)
	{
		FreeSpaceAllocated();
		SetInitErrCode(GetLastError());
	}
	else
		SetInitErrCode(ERROR_SUCCESS);

}

//-----------------------------------------------------------------------------

void CInetSession::FreeSpaceAllocated()
{
	delete[] m_lpszAgentName;
	m_lpszAgentName =NULL;
	delete[] m_lpszProxyName;
	m_lpszProxyName = NULL;
	delete[] m_lpszProxyBypass;
	m_lpszProxyBypass = NULL;
}

//-----------------------------------------------------------------------------

CInetSession::~CInetSession()
{
	FreeSpaceAllocated();

	if (m_hSession)
	{
		InternetCloseHandle(m_hSession);
		m_hSession = NULL;
	}
}

//=============================================================================

CHttpMessage::CHttpMessage(CInetSession *pInetSession, CAppData *pAppData):
	m_pAppData(pAppData),
	m_hConnection(NULL),
	m_hRequest(NULL),
	m_dwContext(NULL)
{
	// set context
	m_dwContext = DWORD(this);

	// open internet connection
	m_hConnection = InternetConnect(
						pInetSession->operator HINTERNET(),
						m_pAppData->m_lpszServerName,
						m_pAppData->m_nPort ? m_pAppData->m_nPort : INTERNET_INVALID_PORT_NUMBER,
						m_pAppData->m_lpszUserName,
						m_pAppData->m_lpszPassword,
						INTERNET_SERVICE_HTTP,
						0,
						m_dwContext
					);
	// in case of errors
	if (!m_hConnection)
	{
		FreeSpaceAllocated();
		SetInitErrCode(GetLastError());
	}
	else
		SetInitErrCode(ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------

DWORD CHttpMessage::SendRequest(CReqestEntity &requestEntity)
{
	if (m_hRequest)
		CloseRequest();

	m_hRequest = HttpOpenRequest(
						m_hConnection,
						requestEntity.GetMethod(),
						requestEntity.GetResource(),
						NULL,
						NULL,
						DEFAULT_ACCEPT_TYPES,
						INTERNET_FLAG_PRAGMA_NOCACHE | 
							INTERNET_FLAG_RELOAD | 
							INTERNET_FLAG_NO_COOKIES |
							INTERNET_FLAG_KEEP_CONNECTION,
						m_dwContext
					);
	// check for errors
	if (!m_hRequest)
		return GetLastError();

	BOOL bResult;
	LPCTSTR lpszHeaders = requestEntity.GetHeaders();

	bResult = HttpSendRequest(
						m_hRequest,
						lpszHeaders, 
						lpszHeaders ? lstrlen(lpszHeaders) : NULL,
						requestEntity.GetData(),
						requestEntity.GetDataSize()
				);

	// check for errors
	if (!bResult)
		return GetLastError();

	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

DWORD CHttpMessage::GetResponseCode(LPDWORD lpdwCode)
{
	if (!m_hRequest)
		return ERROR_INVALID_HANDLE;

	DWORD dwIndex = 0, dwLen = sizeof(DWORD);

	if(!HttpQueryInfo(m_hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
														lpdwCode, &dwLen, &dwIndex))
		return GetLastError();
	else
		return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

DWORD CHttpMessage::QueryInfo(LPVOID lpBuf, LPDWORD lpdwSize, DWORD dwFlags)
{
	DWORD dwIndex = 0;
	if (!HttpQueryInfo(m_hRequest, dwFlags, lpBuf, lpdwSize, &dwIndex))
		return GetLastError();
	else
		return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

DWORD CHttpMessage::ReadData(
						LPVOID lpBuffer,
						DWORD dwBytesToRead,
						LPDWORD lpdwNumBytesRead
)
{
	if (!InternetReadFile(m_hRequest, lpBuffer, dwBytesToRead, lpdwNumBytesRead))
		return GetLastError();
	else	
		return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

DWORD CHttpMessage::CloseRequest()
{
	if (m_hRequest)
	{
		InternetCloseHandle(m_hRequest);
		m_hRequest = NULL;
	}

	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

void CHttpMessage::FreeSpaceAllocated()
{
}

//-----------------------------------------------------------------------------

DWORD CHttpMessage::SaveToFile(LPCTSTR lpszFilePath)
{
	HANDLE hFile;

	hFile = CreateFile(
				lpszFilePath, 
				GENERIC_WRITE,
				FILE_SHARE_READ,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL
			);

	if (hFile != INVALID_HANDLE_VALUE)
	{

		const DWORD BUF_SIZE = 1024;
		DWORD dwActBytesRd, dwActBytesWr;
		LPVOID lpBuffer = HeapAlloc(GetProcessHeap(), 0, BUF_SIZE);
		DWORD bResRd;
		BOOL bResWr = TRUE;
		
		bResRd = ReadData(lpBuffer, BUF_SIZE, &dwActBytesRd);

		while ((bResRd == ERROR_SUCCESS) && bResWr && (dwActBytesRd > 0))
		{
			bResWr = WriteFile(hFile, lpBuffer, dwActBytesRd, &dwActBytesWr, NULL);
			bResRd = ReadData(lpBuffer, BUF_SIZE, &dwActBytesRd);
		}

		HeapFree(GetProcessHeap(), 0, lpBuffer);
		CloseHandle(hFile);

		if ((bResRd == ERROR_SUCCESS) && bResWr)
			return ERROR_SUCCESS;
		else if (!bResWr)
			return GetLastError();
		else // bResRd != ERROR_SUCCESS
			return bResRd;
	}
	else
		return GetLastError();
}
//-----------------------------------------------------------------------------

void CHttpMessage::CallBackFunc(
					DWORD dwInternetStatus, 
					LPVOID lpvStatusInformation,
					DWORD dwStatusInformationLength
)
{
	if (StopFlag::Instance() && m_hConnection != NULL)
	{
		InternetCloseHandle(m_hConnection);
		m_hConnection = NULL;
	}
	else
	{
		switch(dwInternetStatus)
		{
		case INTERNET_STATUS_CONNECTING_TO_SERVER:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_CONNECTING);
			break;
		case INTERNET_STATUS_RESOLVING_NAME:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_RESOLVING);
			break;
		case INTERNET_STATUS_SENDING_REQUEST:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_SENDING);
			break;
		case INTERNET_STATUS_REQUEST_SENT:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_SENT);
			break;
		case INTERNET_STATUS_RECEIVING_RESPONSE:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_RECEIVING);
			break;
		case INTERNET_STATUS_RESPONSE_RECEIVED:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_RECEIVED);
			break;
		case INTERNET_STATUS_REDIRECT:
			SendDlgMsg(m_pAppData->m_hDialog, m_pAppData->m_nDlgItem, MSG_REDIRECTING);
			break;
		default:
			break;
		}
	}
}

//-----------------------------------------------------------------------------

CHttpMessage::~CHttpMessage()
{
	FreeSpaceAllocated();

	if (m_hRequest)
	{
		InternetCloseHandle(m_hRequest);
		m_hRequest = NULL;
	}
	if (m_hConnection)
	{
		InternetCloseHandle(m_hConnection);
		m_hRequest = NULL;
	}
}

//=============================================================================

CReqestEntity::CReqestEntity():
				m_lpszMethod(NULL),
				m_lpszResource(NULL),
				m_lpszHeaders(NULL),
				m_lpDataBuf(NULL),
				m_nDataSize(0)
{
}

//-----------------------------------------------------------------------------

DWORD CReqestEntity::SetHeaders(LPCTSTR lpszHeaders)
{
	if (lpszHeaders == NULL)
		return ERROR_INVALID_PARAMETER;
	
	if (m_lpszHeaders != NULL)
	{
		delete[] m_lpszHeaders;
		m_lpszHeaders = NULL;
	}

	m_lpszHeaders = new TCHAR[lstrlen(lpszHeaders) + 1];
	if (m_lpszHeaders == NULL)
		return ERROR_NOT_ENOUGH_MEMORY;

	lstrcpy(m_lpszHeaders, lpszHeaders);

	return ERROR_SUCCESS;
}


//-----------------------------------------------------------------------------

DWORD CReqestEntity::SetMethod(LPCTSTR lpszMethod)
{
	if (lpszMethod == NULL)
		return ERROR_INVALID_PARAMETER;

	if (m_lpszMethod != NULL) 
	{
		delete[] m_lpszMethod;
		m_lpszMethod = NULL;
	}

	m_lpszMethod = new TCHAR[lstrlen(lpszMethod) + 1];

	if (m_lpszMethod == NULL)
		return ERROR_NOT_ENOUGH_MEMORY;

	lstrcpy(m_lpszMethod, lpszMethod);

	return ERROR_SUCCESS;

}

//-----------------------------------------------------------------------------

DWORD CReqestEntity::SetResource(LPCTSTR lpszResource)
{
	if (lpszResource == NULL)
		return ERROR_INVALID_PARAMETER;

	if (m_lpszResource != NULL)
	{
		delete[] m_lpszResource;
		m_lpszResource = NULL;
	}

	m_lpszResource = new TCHAR[lstrlen(lpszResource) + 1];

	if (m_lpszResource == NULL)
		return ERROR_NOT_ENOUGH_MEMORY;

	lstrcpy(m_lpszResource, lpszResource);

	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

DWORD CReqestEntity::SetData(LPVOID lpData, size_t nSize)
{
	if (m_lpDataBuf != NULL)
	{
		HeapFree(GetProcessHeap(), 0, m_lpDataBuf);
		m_lpDataBuf = NULL;
		m_nDataSize = 0;
	}

	if ((lpData == NULL) || (nSize == 0))
	{
		m_nDataSize = 0;
		return ERROR_SUCCESS;
	}

	m_lpDataBuf = HeapAlloc(GetProcessHeap(), 0, nSize);

	if (m_lpDataBuf == NULL)
		return ERROR_NOT_ENOUGH_MEMORY;

	CopyMemory(m_lpDataBuf, lpData, nSize);
	m_nDataSize = nSize;

	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

CReqestEntity::~CReqestEntity()
{
	delete[] m_lpszMethod;
	m_lpszMethod = NULL;
	delete[] m_lpszResource;
	m_lpszResource = NULL;
	delete[] m_lpszHeaders;
	m_lpszHeaders = NULL;
	
	if (m_lpDataBuf != NULL)
	{
		HeapFree(GetProcessHeap(), 0, m_lpDataBuf);
		m_lpDataBuf = NULL;
	}
}

//=============================================================================

CPostForm::CPostForm(LPCTSTR lpszResource)
{
	SetMethod(TEXT("POST"));
	SetResource(lpszResource);
	SetHeaders(DEFAULT_FORM_HEADERS);
}

//-----------------------------------------------------------------------------

DWORD CPostForm::AddInput(LPCWSTR lpszName, LPCWSTR lpszValue)
{
	if (lpszName == NULL || *lpszName == L'\0')
		return ERROR_INVALID_PARAMETER;
	
	DWORD nParamU8Len = 0, nValueU8Len = 0;

	UcsToUtf8Enc(lpszName, wcslen(lpszName), NULL, nParamU8Len);

	if (lpszValue != NULL)
		UcsToUtf8Enc(lpszValue, wcslen(lpszValue), NULL, nValueU8Len);

	// calculate data size of input
	DWORD nDataSz = GetDataSize();
	if (nDataSz > 0)
		nDataSz += sizeof('&');

	nDataSz += nParamU8Len + sizeof('=') + nValueU8Len;

	// temporary storage
	char *lpCurPtr, *lpNewData;
	lpCurPtr = lpNewData = (char*) HeapAlloc(GetProcessHeap(), 0, nDataSz);
	
	if (lpNewData == NULL)
		return ERROR_NOT_ENOUGH_MEMORY;
	
	// copy old contents of the old buffer
	if (GetDataSize() > 0)
	{
		CopyMemory(lpCurPtr, GetData(), GetDataSize());
		lpCurPtr += GetDataSize();
		*lpCurPtr = '&';
		lpCurPtr++;
	}

	// add parameter
	UcsToUtf8Enc(lpszName, wcslen(lpszName), lpCurPtr, nParamU8Len);
	lpCurPtr += nParamU8Len;
	*lpCurPtr = '=';
	lpCurPtr++;

	if (lpszValue != NULL)
		UcsToUtf8Enc(lpszValue, wcslen(lpszValue), lpCurPtr, nValueU8Len);

	
	DWORD dwResSet = SetData((LPVOID)lpNewData, nDataSz);
	HeapFree(GetProcessHeap(), 0, lpNewData);

	return dwResSet;

}

//-----------------------------------------------------------------------------

DWORD CPostForm::UcsToUtf8Enc(LPCWSTR lpszSource, DWORD nSrcSz, LPSTR lpszDest, DWORD &nDstSz)
{
	// 1 UTF8 symbol is converted to 3 escaped symbols - %XX
	const DWORD CHARS_IN_UTF8_SYMB = 3;
	char HexBuf[] = "0123456789ABCDEF";

	DWORD nUtf8Len = AtlUnicodeToUTF8(lpszSource, nSrcSz, NULL, 0);

	if (nDstSz < nUtf8Len * CHARS_IN_UTF8_SYMB)
	{
		nDstSz = nUtf8Len * CHARS_IN_UTF8_SYMB;
		return ERROR_INSUFFICIENT_BUFFER;
	}

	if (lpszDest == NULL)
		return ERROR_INVALID_PARAMETER;

	LPSTR lpszTempU8Buf = (LPSTR)HeapAlloc(GetProcessHeap(), 0, nUtf8Len);
	if (lpszTempU8Buf == NULL)
		return ERROR_NOT_ENOUGH_MEMORY;

	AtlUnicodeToUTF8(lpszSource, nSrcSz, lpszTempU8Buf, nUtf8Len);

	nDstSz = 0;
	for (DWORD i = 0; i < nUtf8Len; i++)
	{
		lpszDest[nDstSz++] = '%';
		lpszDest[nDstSz++] = HexBuf[(lpszTempU8Buf[i] >> 4) & 0x0F];
		lpszDest[nDstSz++] = HexBuf[lpszTempU8Buf[i] & 0x0F];
	}

	HeapFree(GetProcessHeap(), 0, lpszTempU8Buf);

	return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

void CPostForm::ClearInput()
{
	// clears data buffer
	SetData(NULL, 0);
}

//-----------------------------------------------------------------------------

CPostForm::~CPostForm()
{
}

//-----------------------------------------------------------------------------

#define ATL_ASCII                 0x007f

#define ATL_UTF8_2_MAX            0x07ff  // max UTF8 2-byte sequence (32 * 64 = 2048)
#define ATL_UTF8_1ST_OF_2         0xc0    // 110x xxxx
#define ATL_UTF8_1ST_OF_3         0xe0    // 1110 xxxx
#define ATL_UTF8_1ST_OF_4         0xf0    // 1111 xxxx
#define ATL_UTF8_TRAIL            0x80    // 10xx xxxx

#define ATL_HIGHER_6_BIT(u)       ((u) >> 12)
#define ATL_MIDDLE_6_BIT(u)       (((u) & 0x0fc0) >> 6)
#define ATL_LOWER_6_BIT(u)        ((u) & 0x003f)


#define ATL_HIGH_SURROGATE_START  0xd800
#define ATL_HIGH_SURROGATE_END    0xdbff
#define ATL_LOW_SURROGATE_START   0xdc00
#define ATL_LOW_SURROGATE_END     0xdfff

int AtlUnicodeToUTF8(LPCWSTR wszSrc, int nSrc, LPSTR szDest, int nDest)
{
	LPCWSTR pwszSrc = wszSrc;
	int     nU8 = 0;                // # of UTF8 chars generated
	DWORD   dwSurrogateChar;
	WCHAR   wchHighSurrogate = 0;
	BOOL    bHandled;

	while ((nSrc--) && ((nDest == 0) || (nU8 < nDest)))
	{
		bHandled = FALSE;

		// Check if high surrogate is available
		if ((*pwszSrc >= ATL_HIGH_SURROGATE_START) && (*pwszSrc <= ATL_HIGH_SURROGATE_END))
		{
			if (nDest)
			{
				// Another high surrogate, then treat the 1st as normal Unicode character.
				if (wchHighSurrogate)
				{
					if ((nU8 + 2) < nDest)
					{
						szDest[nU8++] = (char)(ATL_UTF8_1ST_OF_3 | ATL_HIGHER_6_BIT(wchHighSurrogate));
						szDest[nU8++] = (char)(ATL_UTF8_TRAIL    | ATL_MIDDLE_6_BIT(wchHighSurrogate));
						szDest[nU8++] = (char)(ATL_UTF8_TRAIL    | ATL_LOWER_6_BIT(wchHighSurrogate));
					}
					else
					{
						// not enough buffer
						nSrc++;
						break;
					}
				}
			}
			else
			{
				nU8 += 3;
			}
			wchHighSurrogate = *pwszSrc;
			bHandled = TRUE;
		}

		if (!bHandled && wchHighSurrogate)
		{
			if ((*pwszSrc >= ATL_LOW_SURROGATE_START) && (*pwszSrc <= ATL_LOW_SURROGATE_END))
			{
				 // valid surrogate pairs
				 if (nDest)
				 {
					 if ((nU8 + 3) < nDest)
					 {
						 dwSurrogateChar = (((wchHighSurrogate-0xD800) << 10) + (*pwszSrc - 0xDC00) + 0x10000);
						 szDest[nU8++] = (ATL_UTF8_1ST_OF_4 |
											   (unsigned char)(dwSurrogateChar >> 18));           // 3 bits from 1st byte
						 szDest[nU8++] =  (ATL_UTF8_TRAIL |
												(unsigned char)((dwSurrogateChar >> 12) & 0x3f)); // 6 bits from 2nd byte
						 szDest[nU8++] = (ATL_UTF8_TRAIL |
											   (unsigned char)((dwSurrogateChar >> 6) & 0x3f));   // 6 bits from 3rd byte
						 szDest[nU8++] = (ATL_UTF8_TRAIL |
											   (unsigned char)(0x3f & dwSurrogateChar));          // 6 bits from 4th byte
					 }
					 else
					 {
						// not enough buffer
						nSrc++;
						break;
					 }
				 }
				 else
				 {
					 // we already counted 3 previously (in high surrogate)
					 nU8 += 1;
				 }
				 bHandled = TRUE;
			}
			else
			{
				 // Bad Surrogate pair : ERROR
				 // Just process wchHighSurrogate , and the code below will
				 // process the current code point
				 if (nDest)
				 {
					 if ((nU8 + 2) < nDest)
					 {
						szDest[nU8++] = (char)(ATL_UTF8_1ST_OF_3 | ATL_HIGHER_6_BIT(wchHighSurrogate));
						szDest[nU8++] = (char)(ATL_UTF8_TRAIL    | ATL_MIDDLE_6_BIT(wchHighSurrogate));
						szDest[nU8++] = (char)(ATL_UTF8_TRAIL    | ATL_LOWER_6_BIT(wchHighSurrogate));
					 }
					 else
					 {
						// not enough buffer
						nSrc++;
						break;
					 }
				 }
			}
			wchHighSurrogate = 0;
		}

		if (!bHandled)
		{
			if (*pwszSrc <= ATL_ASCII)
			{
				//  Found ASCII.
				if (nDest)
				{
					szDest[nU8] = (char)*pwszSrc;
				}
				nU8++;
			}
			else if (*pwszSrc <= ATL_UTF8_2_MAX)
			{
				//  Found 2 byte sequence if < 0x07ff (11 bits).
				if (nDest)
				{
					if ((nU8 + 1) < nDest)
					{
						//  Use upper 5 bits in first byte.
						//  Use lower 6 bits in second byte.
						szDest[nU8++] = (char)(ATL_UTF8_1ST_OF_2 | (*pwszSrc >> 6));
						szDest[nU8++] = (char)(ATL_UTF8_TRAIL    | ATL_LOWER_6_BIT(*pwszSrc));
					}
					else
					{
						//  Error - buffer too small.
						nSrc++;
						break;
					}
				}
				else
				{
					nU8 += 2;
				}
			}
			else
			{
				//  Found 3 byte sequence.
				if (nDest)
				{
					if ((nU8 + 2) < nDest)
					{
						//  Use upper  4 bits in first byte.
						//  Use middle 6 bits in second byte.
						//  Use lower  6 bits in third byte.
						szDest[nU8++] = (char)(ATL_UTF8_1ST_OF_3 | ATL_HIGHER_6_BIT(*pwszSrc));
						szDest[nU8++] = (char)(ATL_UTF8_TRAIL    | ATL_MIDDLE_6_BIT(*pwszSrc));
						szDest[nU8++] = (char)(ATL_UTF8_TRAIL    | ATL_LOWER_6_BIT(*pwszSrc));
					}
					else
					{
						//  Error - buffer too small.
						nSrc++;
						break;
					}
				}
				else
				{
					nU8 += 3;
				}
			}
		}
		pwszSrc++;
	}

	// If the last character was a high surrogate, then handle it as a normal unicode character.
	if ((nSrc < 0) && (wchHighSurrogate != 0))
	{
		if (nDest)
		{
			if ((nU8 + 2) < nDest)
			{
				szDest[nU8++] = (char)(ATL_UTF8_1ST_OF_3 | ATL_HIGHER_6_BIT(wchHighSurrogate));
				szDest[nU8++] = (char)(ATL_UTF8_TRAIL    | ATL_MIDDLE_6_BIT(wchHighSurrogate));
				szDest[nU8++] = (char)(ATL_UTF8_TRAIL    | ATL_LOWER_6_BIT(wchHighSurrogate));
			}
			else
			{
				nSrc++;
			}
		}
	}

	//  Make sure the destination buffer was large enough.
	if (nDest && (nSrc >= 0))
	{
		return 0;
	}

	//  Return the number of UTF-8 characters written.
	return nU8;
}
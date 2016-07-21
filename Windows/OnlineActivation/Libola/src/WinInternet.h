/** 
 * @file
 * @brief	Implementation of Windows Internet library facade.
 * @author	Andrey Guzhov
 * @date	18.04.2007
 */

#ifndef _AG_WININTERNET_H_
#define _AG_WININTERNET_H_

#include "InternetHandle.h"

namespace OnlineActivation
{
/**
 * Windows Internet library wrapper facade.
 * @stereotype wrapper-facade 
 */
class WinInternet
{
public:
	static InternetHandle Open
	(
		const char* szAgent,
		DWORD dwAccessType,
		const char* szProxy,
		const char* szProxyBypass,
		DWORD dwFlags
	);
	static InternetHandle Connect
	(
		InternetHandle& hInternet,
		const char* szServerName,
		INTERNET_PORT nServerPort,
		const char* szUserName,
		const char* szPassword,
		DWORD dwService,
		DWORD dwFlags,
		DWORD_PTR dwContext
	);
	static InternetHandle OpenRequest
	(
		InternetHandle& hConnect,
		const char* szVerb,
		const char* szObjectName,
		const char* szVersion,
		const char* szReferrer,
		const char** pszAcceptTypes,
		DWORD dwFlags,
		DWORD_PTR dwContext
    );
	static void AddRequestHeaders
	(
		InternetHandle& hRequest,
		const char* szHeaders,
		DWORD dwHeadersLength,
		DWORD dwModifiers
    );
	static void SendRequest
	(
		InternetHandle& hRequest,
		const char* szHeaders,
		DWORD dwHeadersLength,
		LPVOID lpOptional,
		DWORD dwOptionalLength
    );
	static void AbortRequest
	(
		InternetHandle& hRequest
	);
	static void SetOption
	(
		InternetHandle& hInternet,
		DWORD dwOption,
		LPCVOID lpBuffer,
		DWORD dwBufferLength
    );
	static DWORD QueryStatusCode
	(
		InternetHandle& hRequest
    );
	static BOOL QueryContentType
	(
		InternetHandle& hRequest, 
		char* szContentType,
		LPDWORD pdwContentTypeLength
	);
	static DWORD ReadData
	(
		InternetHandle& hFile,
		LPVOID lpBuffer,
		DWORD dwNumberOfBytesToRead
    );
	static INTERNET_STATUS_CALLBACK SetStatusCallback
	(
		InternetHandle& hInternet,
		INTERNET_STATUS_CALLBACK lpfnInternetCallback
    );
};

}
#endif //_AG_WININTERNET_H_

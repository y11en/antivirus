/*!
*		(C) 2002 Kaspersky Lab 
*		
*		\file	netenum.cpp
*		\brief	
*		
*		\author Андрей Брыксин
*		\date	18.10.2002 17:17:07
*		
*		Example:	
*/		


#include <windows.h>
#include <std/err/klerrors.h>
#include <std/base/klstd.h>

#include "netenum.h"
#pragma comment(lib, "netapi32.lib")

#define KLCS_MODULENAME L"KLWNF"

/////////////////////////////////////////////////////////////////////////////

CNetEnumBase::CNetEnumBase(bool bEnumServers)
{
	m_bEnumServers = bEnumServers;
}

void CNetEnumBase::AddConnection(NETRESOURCE& rNetRC, DWORD dwFlags, wchar_t* szUserName,	wchar_t* szPassword) 
{
	DWORD dwResult = WNetAddConnection2(&rNetRC, szUserName, szPassword, dwFlags);
	if (dwResult != NO_ERROR) ThrowNetError(GetLastError());
}

void CNetEnumBase::CancelConnection(wchar_t* szName, DWORD dwFlags, bool bForce) 
{
	DWORD dwResult = WNetCancelConnection2(szName, dwFlags, bForce);
	if (dwResult != NO_ERROR) ThrowNetError(GetLastError());
}

void CNetEnumBase::Enumerate(LPNETRESOURCE lpNetRC_p, DWORD dwFlags_p, DWORD dwUsage)
{
	DWORD dwScope = RESOURCE_GLOBALNET;

	if (dwFlags_p & CONNECTED) dwScope = RESOURCE_CONNECTED;
	else 
	if (dwFlags_p & REMEMBERED) dwScope = RESOURCE_REMEMBERED;
	else
	if (dwFlags_p & CONTEXT) dwScope = RESOURCE_CONTEXT;
	// else GLOBALNET ...

	DWORD dwType = RESOURCETYPE_ANY;

	if (dwFlags_p & TYPE_DISK)
		dwType = RESOURCETYPE_DISK;
	else 
		if (dwFlags_p & TYPE_PRINT) dwType = RESOURCETYPE_PRINT;
	// else TYPE_ANY ...

	LPNETRESOURCE lpnrLocal = 0;
	HANDLE hEnum = NULL;

	try 
	{
		DWORD dwBuffer = 16384;			// 16K is reasonable size
		DWORD dwEntries = 0xFFFFFFFF ;	// enumerate all possible entries

		DWORD dwResult = WNetOpenEnum(dwScope, dwType, 0, lpNetRC_p, &hEnum);
		if (dwResult != NO_ERROR) 
		{
			KLSTD_TRACE1(3, L"CNetEnumBase::Enumerate::WNetOpenEnum(%ls) failed\n", lpNetRC_p?lpNetRC_p->lpRemoteName:L"");
//			ThrowNetError(dwResult);
			return;						// Don't enumerate this node if minor error occured
		}

		// first allocate buffer for NETRESOURCE structures ...
		lpnrLocal = (LPNETRESOURCE)GlobalAlloc(GPTR, dwBuffer);
		if (!lpnrLocal) ThrowNetError(GetLastError());

		do 
		{
			KLSTD_TRACE0(3, L"CNetEnumBase::Enumerate:do { WNetEnumResource()\n");
			dwResult =	WNetEnumResource(hEnum, &dwEntries, lpnrLocal, &dwBuffer);
			if (dwResult == ERROR_NO_MORE_ITEMS) break;
			if (dwResult == NO_ERROR) 
			{
				KLSTD_TRACE0(3, L"CNetEnumBase::Enumerate:do { WNetEnumResource() == OK; for {\n");
				for (register DWORD i = 0;i < dwEntries;i++)
				{
					KLSTD_TRACE1(3, L"CNetEnumBase::Enumerate::OnHitResource(%ls)\n", lpnrLocal[i].lpRemoteName);
					if (!OnHitResource(lpnrLocal[i])) KLSTD_THROW(KLSTD::STDE_CANCELED);
					KLSTD_TRACE1(3, L"CNetEnumBase::Enumerate::OnHitResource-end(%ls)\n", lpnrLocal[i].lpRemoteName);

					if (RESOURCEUSAGE_CONTAINER == (lpnrLocal[i].dwUsage & RESOURCEUSAGE_CONTAINER) &&
						(m_bEnumServers || lpnrLocal[i].dwDisplayType != RESOURCEDISPLAYTYPE_DOMAIN) &&
						lpnrLocal[i].dwDisplayType != RESOURCEDISPLAYTYPE_SERVER)
					{
						Enumerate(&lpnrLocal[i], dwFlags_p, RESOURCEUSAGE_CONTAINER);
					}
				}
				KLSTD_TRACE0(3, L"CNetEnumBase::Enumerate:do { WNetEnumResource();for == OK\n");
			}
			else
			{
				KLSTD_TRACE1(1, L"CNetEnumBase::Enumerate:do { WNetEnumResource() == FAILED [%u]\n", dwResult);
				ThrowNetError(dwResult);
				break;					// Stop enumeration if minor error occured
			}
		}
		while(dwResult != ERROR_NO_MORE_ITEMS);

		KLSTD_TRACE0(1, L"CNetEnumBase::Enumerate - DONE\n");

		if (lpnrLocal) GlobalFree((HGLOBAL) lpnrLocal);
		WNetCloseEnum(hEnum);
	}
	catch(...)
	{
		if (lpnrLocal) GlobalFree((HGLOBAL)lpnrLocal);
		if (hEnum) WNetCloseEnum(hEnum);
		KLSTD_TRACE0(1, L"CNetEnumBase::Enumerate:catch(...)\n");

		throw;
	}
}

void CNetEnumBase::ThrowNetError(DWORD dwErrorCode)
{ 
	switch(dwErrorCode) {
		case ERROR_EXTENDED_ERROR:
		{
			DWORD dwLastError;
			wchar_t szDescription[256];

			// The following code performs error-handling when the
			// ERROR_EXTENDED_ERROR return value indicates that WNetGetLastError
			// can retrieve additional information.
			if (WNetGetLastError(&dwLastError, szDescription, sizeof(szDescription), NULL, 0) == NO_ERROR) 
				szDescription[wcslen(szDescription)-2] = L'\0';  //remove cr/nl characters

			KLSTD_THROW_LASTERROR_CODEMSG2(dwLastError, szDescription);
		}
		break;
		default:
			KLSTD_THROW_LASTERROR_CODE2(dwErrorCode);
	}
} 

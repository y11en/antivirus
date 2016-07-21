////////////////////////////////////////////////////////////////////
//
//	WinFileInfo.cpp: implementation of the CWinFileInfo class.
//
//	»нтерфейс дл€ доступа к информации о файле
//
//	Remark: You must initialize COM by CoInitialize before using this class
////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "WinFileInfo.h"
#include "Winerror.h"

#define __SWAPWORDS__(X)((X<<16) |(X>>16))

static const TCHAR _szRussianNtoskrnlAlt[] = _T("С®бв•ђ≠л© ђЃ§гЂм п§а† NT");
static const TCHAR _szRussianNtoskrnlWin[] = _T("—истемный модуль €дра NT");

CWinFileInfo::CWinFileInfo()
{
	m_pVersionInfo = NULL;	
	m_hLargeIcon = NULL;
	m_hSmallIcon = NULL;
}

CWinFileInfo::~CWinFileInfo()
{
	ResetState();
}

void CWinFileInfo::ResetState()
{
	if	(m_pVersionInfo)
	{
		HeapFree(GetProcessHeap(), 0, m_pVersionInfo);
		m_pVersionInfo = NULL;
	}

	if	(m_hLargeIcon)
	{
		DestroyIcon(m_hLargeIcon);
		m_hLargeIcon = NULL;
	}

	if	(m_hSmallIcon)
	{
		DestroyIcon(m_hSmallIcon);
		m_hSmallIcon = NULL;
	}
}

// -----------------------------------------------------------------------------------------

DWORD CWinFileInfo::QueryInfo(LPTSTR pszFileName)
{
	if ((pszFileName == NULL) ||(lstrlen(pszFileName) == 0))
		return ERROR_INVALID_PARAMETER;
	
	// -----------------------------------------------------------------------------------------
	ResetState();

	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(sfi));

	UINT uFlags = SHGFI_ICON | SHGFI_USEFILEATTRIBUTES;
	
	if (SHGetFileInfo(pszFileName, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), uFlags | SHGFI_LARGEICON))
	{
		//save large icon sfi.hIcon;
		m_hLargeIcon = CopyIcon(sfi.hIcon);
		DestroyIcon(sfi.hIcon);
	}

	ZeroMemory(&sfi, sizeof(sfi));
	
	if (SHGetFileInfo(pszFileName, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), uFlags | SHGFI_SMALLICON))
	{
		// save small icon sfi.hIcon;
		m_hSmallIcon = CopyIcon(sfi.hIcon);
		DestroyIcon(sfi.hIcon);
	}
	
	DWORD dwHandle;
	DWORD dwVersionSize = GetFileVersionInfoSize(pszFileName, &dwHandle);
	if (dwVersionSize == 0)
	{
		DWORD dwerr = GetLastError();
		return ERROR_FILE_NOT_FOUND;
	}
	
	m_pVersionInfo = HeapAlloc(GetProcessHeap(), 0, dwVersionSize);

	if (m_pVersionInfo == NULL)
		return GetLastError();
	
	if (GetFileVersionInfo(pszFileName, NULL, dwVersionSize, m_pVersionInfo) == 0)
		return ERROR_FILE_NOT_FOUND;
	
	// -----------------------------------------------------------------------------------------
	UINT dwUint;
	LPDWORD lpdwLangCp;
	if (!VerQueryValue(m_pVersionInfo, _T("\\VarFileInfo\\Translation"),(LPVOID*) &lpdwLangCp, &dwUint)) 
		return GetLastError();
	
	wsprintf(m_szLangCp, _T("%08X"), __SWAPWORDS__(*lpdwLangCp));

	// -----------------------------------------------------------------------------------------
	return 0;
}

PTCHAR CWinFileInfo::GetInfo(PTCHAR pInfoName)
{
	PTCHAR Buffer = NULL;
	UINT dwUint;
	TCHAR SubBlock[2048];

	if	(m_pVersionInfo)
	{
		wsprintf(SubBlock, _T("\\StringFileInfo\\%s\\%s"), m_szLangCp, pInfoName);
		
		if (!VerQueryValue(m_pVersionInfo, SubBlock, (LPVOID*)&Buffer, &dwUint)) 
			return 0;
	}
	return Buffer;
}

// -----------------------------------------------------------------------------------------
PTCHAR CWinFileInfo::GetCompanyInfo()
{
	return GetInfo(_T("CompanyName"));
}

PTCHAR CWinFileInfo::GetFileDescription()
{
     PTCHAR pszRet = GetInfo(_T("FileDescription"));

	 // под Windows 2000 ntoskrnl.exe имеет FileDescription в неправильной кодировке
	 if (pszRet && lstrcmp(pszRet, _szRussianNtoskrnlAlt) == 0)
	 {
		 return(PTCHAR)_szRussianNtoskrnlWin;
	 }

     return pszRet ? pszRet : _T("");
}

PTCHAR CWinFileInfo::GetProductVersion()
{
	return GetInfo(_T("ProductVersion"));
}

PTCHAR CWinFileInfo::GetFileVersion()
{
	return GetInfo(_T("FileVersion"));
}

HICON CWinFileInfo::GetIcon(bool bLarge)
{
	return bLarge == true ? m_hLargeIcon : m_hSmallIcon;
}
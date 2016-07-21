// FileVersion.cpp: implementation of the CFileVersion class.
// by Manuel Laflamme 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileVersion.h"

#pragma comment(lib, "version")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CFileVersion::CFileVersion() 
{ 
    m_lpVersionData = NULL;
    m_dwLangCharset = 0;
	m_dwLangCharset2= 0;
	memset(&fd,0,sizeof(fd));
}

CFileVersion::~CFileVersion() 
{ 
    Close();
} 

void CFileVersion::Close()
{
	if (m_lpVersionData!=NULL)
	{
		free(m_lpVersionData);
		m_lpVersionData = NULL;
		m_dwLangCharset = 0;
		m_dwLangCharset2= 0;
	}
	memset(&fd,0,sizeof(fd));
}

BOOL CFileVersion::Open(LPCTSTR lpszModuleName)
{
	DWORD dwHandle;     
	HANDLE	hFind=NULL;
	if ((hFind=FindFirstFile(lpszModuleName,&fd))!=INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
	}
	else memset(&fd,0,sizeof(fd));
	DWORD dwDataSize = ::GetFileVersionInfoSize((LPTSTR)lpszModuleName, &dwHandle); 
	if (dwDataSize==0) return FALSE;
	m_lpVersionData=(LPBYTE)malloc(dwDataSize);
	if (!::GetFileVersionInfo((LPTSTR)lpszModuleName, dwHandle, dwDataSize, (void**)m_lpVersionData) )
	{
		if (m_lpVersionData!=NULL)
		{
			free(m_lpVersionData);
			m_lpVersionData = NULL;
			m_dwLangCharset = 0;
			m_dwLangCharset2= 0;
		}
		return FALSE;
	}
	UINT nQuerySize=8;
	DWORD* pTransTable;
	if (!::VerQueryValue(m_lpVersionData, _T("\\VarFileInfo\\Translation"), (void **)&pTransTable, &nQuerySize) )
	{
		WCHAR	fnd[]=L"StringFileInfo";
		WCHAR	LANG[18];
		DWORD	FndLng=(DWORD)m_lpVersionData;
		while ((FndLng=(DWORD)memchr((void*)FndLng,'S',dwDataSize-((DWORD)FndLng-(DWORD)m_lpVersionData)))!=0)
		{
			if (wcscmp((const WCHAR*)FndLng,fnd)==0)
			{
				FndLng=FndLng+28;
				while (*(WORD*)FndLng==0) FndLng=FndLng+2;
				FndLng=FndLng+6;
				memcpy((void*)LANG,(void*)FndLng,16);
				LANG[8]=0;
				WCHAR	frmt[]=L"%x";
				m_dwLangCharset=wcstoul(LANG,NULL,16);
				m_dwLangCharset2= MAKELONG(0x04b0, LOWORD(m_dwLangCharset));
				return TRUE;
			}
			else FndLng++;
			if (dwDataSize<=((DWORD)FndLng-(DWORD)m_lpVersionData)) break;
		}
		if (m_lpVersionData!=NULL)
		{
			free(m_lpVersionData);
			m_lpVersionData = NULL;
			m_dwLangCharset = 0;
			m_dwLangCharset2= 0;
		}
		return FALSE;
	}
	m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
	m_dwLangCharset2= MAKELONG(0x04b0, LOWORD(pTransTable[0]));
	WCHAR	fnd[]=L"StringFileInfo";
	WCHAR	LANG[18];
	DWORD	FndLng=(DWORD)m_lpVersionData;
	while ((FndLng=(DWORD)memchr((void*)FndLng,'S',dwDataSize-((DWORD)FndLng-(DWORD)m_lpVersionData)))!=0)
	{
		if (wcscmp((const WCHAR*)FndLng,fnd)==0)
		{
			FndLng=FndLng+28;
			while (*(WORD*)FndLng==0) FndLng=FndLng+2;
			FndLng=FndLng+6;
			memcpy((void*)LANG,(void*)FndLng,16);
			LANG[8]=0;
			WCHAR	frmt[]=L"%x";
			m_dwLangCharset2=wcstoul(LANG,NULL,16);
			break;
		}
		else FndLng++;
		if (dwDataSize<=((DWORD)FndLng-(DWORD)m_lpVersionData)) break;
	}
	return TRUE;
}

char* CFileVersion::QueryValue(LPCTSTR lpszValueName, DWORD dwLangCharset /* = 0*/)
{
	if ( m_lpVersionData == NULL ) return NULL;
	if ( dwLangCharset == 0 ) dwLangCharset = m_dwLangCharset;
	UINT nQuerySize;
	LPVOID lpData;
	char* strValue=NULL;
	char	strBlockName[26+40];
	sprintf(strBlockName,"\\StringFileInfo\\%08lx\\%s",dwLangCharset,lpszValueName);
	if (::VerQueryValue((void **)m_lpVersionData,strBlockName,&lpData,&nQuerySize)) 
	{
		strValue=(char*)malloc(strlen((LPCTSTR)lpData)+1);
		strcpy(strValue,(LPCTSTR)lpData);
	}
	else
	{
		sprintf(strBlockName,"\\StringFileInfo\\%08lx\\%s",m_dwLangCharset2,lpszValueName);
		if (::VerQueryValue((void **)m_lpVersionData,strBlockName,&lpData,&nQuerySize)) 
		{
			strValue=(char*)malloc(strlen((LPCTSTR)lpData)+1);
			strcpy(strValue,(LPCTSTR)lpData);
		}
	}
	return strValue;
}

char* CFileVersion::GetFileModificationDateMy()
{
	char*		FileSizeStr=NULL;
	SYSTEMTIME	stModify;
	
	FileTimeToSystemTime(&fd.ftLastWriteTime,&stModify);
	FileSizeStr=(char*)malloc(17);
	if (FileSizeStr!=NULL) 
	{
		wsprintf(FileSizeStr,"%02d/%02d/%04d  %02d:%02d",stModify.wDay,stModify.wMonth,stModify.wYear,stModify.wHour,stModify.wMinute);
		return FileSizeStr;
	}
	return NULL;
}

char* CFileVersion::GetFileSizeMy()
{
	TCHAR	FileSize[20];
	char*	FileSizeStr=NULL;
	if (fd.nFileSizeLow!=0)
	{
		_ultoa(fd.nFileSizeLow,FileSize,10);
		FileSizeStr=(char*)malloc(strlen(FileSize)+1);
		if (FileSizeStr!=NULL) 
		{
			strcpy(FileSizeStr,FileSize);
			return FileSizeStr;
		}
	}
	return NULL;
}


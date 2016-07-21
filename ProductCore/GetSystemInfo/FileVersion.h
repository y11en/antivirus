#include <tchar.h>

// FileVersion.h: interface for the CFileVersion class.
// by Manuel Laflamme
//////////////////////////////////////////////////////////////////////

#ifndef __FILEVERSION_H_
#define __FILEVERSION_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CFileVersion
{ 
// Construction
public: 
    CFileVersion();

// Operations   
public: 
    BOOL    Open(LPCTSTR lpszModuleName);
    void    Close();
				
    char*	QueryValue(LPCTSTR lpszValueName, DWORD dwLangCharset = 0);
    char*	GetFileDescription()		{return QueryValue(_T("FileDescription"));	};
    char*	GetFileVersion()			{return QueryValue(_T("FileVersion"));		};
    char*	GetInternalName()			{return QueryValue(_T("InternalName"));		};
    char*	GetCompanyName()			{return QueryValue(_T("CompanyName"));		}; 
    char*	GetLegalCopyright()			{return QueryValue(_T("LegalCopyright"));	};
    char*	GetOriginalFilename()		{return QueryValue(_T("OriginalFilename"));	};
    char*	GetProductName()			{return QueryValue(_T("ProductName"));		};
    char*	GetProductVersion()			{return QueryValue(_T("ProductVersion"));	};
    char*	GetFileSizeMy();
    char*	GetFileModificationDateMy();
	
// Attributes
protected:
    LPBYTE  m_lpVersionData; 
    DWORD   m_dwLangCharset; 

	DWORD	m_dwLangCharset2;
	WIN32_FIND_DATA fd;
				
// Implementation
public:
    ~CFileVersion(); 
}; 

#endif  // __FILEVERSION_H_

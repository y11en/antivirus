////////////////////////////////////////////////////////////////////
//
//	WinFileInfo.h: interface for the CWinFileInfo class.
//
//	Интерфейс для доступа к информации о файле
//
//	Remark: You must initialize COM by CoInitialize before using this class
////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINFILEINFO_H__98433CAE_65EC_41D9_AAB6_A7C9A0DAEF95__INCLUDED_)
#define AFX_WINFILEINFO_H__98433CAE_65EC_41D9_AAB6_A7C9A0DAEF95__INCLUDED_

#ifndef PTCHAR
typedef LPSTR PTCHAR;
#endif

class CWinFileInfo  
{
public:
	CWinFileInfo();
	virtual ~CWinFileInfo();

// -----------------------------------------------------------------------------------------
// Function name	: QueryInfo
// Description	    : Acquire file info
// Return type		: DWORD If 0 - successful, other - error code (See GetLastError)
// Argument         : LPTSTR pFileName - source file name
	DWORD QueryInfo(LPTSTR pFileName);

// -----------------------------------------------------------------------------------------
// Function name	: GetCompanyInfo
// Description	    : Getting company name
// Return type		: PTCHAR - pointer on string; NULL if fail
	PTCHAR CWinFileInfo::GetCompanyInfo();
	
// -----------------------------------------------------------------------------------------
// Function name	: GetFileDescription
// Description	    : Get internal file info
// Return type		: PTCHAR - pointer on string with information; NULL if fail
	PTCHAR GetFileDescription();

// -----------------------------------------------------------------------------------------
// Function name	: GetProductVersion
// Description	    : Get product version
// Return type		: PTCHAR - pointer on string with version; NULL if fail
	PTCHAR GetProductVersion();

// -----------------------------------------------------------------------------------------
// Function name	: GetFileVersion
// Description	    : Get file version
// Return type		: PTCHAR - pointer on string with version; NULL if fail
	PTCHAR GetFileVersion();
	
// -----------------------------------------------------------------------------------------
// Function name	: GetIcon
// Description	    : Get icon associated with file 
// Return type		: HICON - icon handle; NULL if fail
// Argument         : bool bLarge == 0 - large icon, otherwise small icon
	HICON GetIcon(bool bLarge);
// -----------------------------------------------------------------------------------------
	
private:
	void ResetState();
	PTCHAR GetInfo(PTCHAR pInfoName);
	
	PVOID m_pVersionInfo;
	TCHAR m_szLangCp[20];

	HICON m_hLargeIcon;
	HICON m_hSmallIcon;

};

#endif // !defined(AFX_WINFILEINFO_H__98433CAE_65EC_41D9_AAB6_A7C9A0DAEF95__INCLUDED_)

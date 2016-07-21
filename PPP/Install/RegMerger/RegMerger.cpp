// RegMerger.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <tchar.h>
#include <string>
#include <map>

#if defined(UNICODE) || defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

struct less_ci : public std::binary_function<tstring, tstring, bool>
{
	bool operator()(const tstring& _Left, const tstring& _Right) const
	{
		tstring __Left(_Left), __Right(_Right);
		std::_USE(std::locale(), std::ctype<tstring::traits_type::char_type>).toupper(&__Left[0], &__Left[__Left.size()]);
		std::_USE(std::locale(), std::ctype<tstring::traits_type::char_type>).toupper(&__Right[0], &__Right[__Right.size()]);
		return __Left.compare(__Right) < 0;
	}
};

typedef std::map<tstring, tstring, less_ci> secmap;

#define STR_REGEDIT4_SIGN_A  "REGEDIT4\r\n\r\n"
#define STR_REGEDIT4_SIGN_W  L"\xfeffREGEDIT4\r\n\r\n"
#define STR_REGMERGERCAPTION _T("Registry files merger")
#define STR_REGMERGER        _T("Usage: regmerger.exe dest.reg src1.reg src2.reg [src3.reg [...]]\nMerges two or more registy files\n\tsrc*.reg - source files,\n\tdest.reg - destination (if already exists, will be overwritten).")

static bool GetSectionNames(LPTSTR *pszBuffer, LPCTSTR szFile)
{
	const DWORD dwAllocGranularity = 4096;
	DWORD dwSize = dwAllocGranularity, dwReqSize;
	*pszBuffer = new TCHAR[dwSize];
	while((dwReqSize = GetPrivateProfileSectionNames(*pszBuffer, dwSize, szFile)) == dwSize - 2)
	{
		dwSize += dwAllocGranularity;
		delete []*pszBuffer;
		*pszBuffer = new TCHAR[dwSize];
		if(!*pszBuffer)
			return false;
	}
//	if(!dwReqSize)
//	{
//		delete []*pszBuffer;
//		*pszBuffer = NULL;
//		return false;
//	}
	return true;
}

static bool GetSection(LPTSTR *pszBuffer, LPCTSTR szSection, LPCTSTR szFile)
{
	const DWORD dwAllocGranularity = 4096;
	DWORD dwSize = dwAllocGranularity, dwReqSize;
	*pszBuffer = new TCHAR[dwSize];
	while((dwReqSize = GetPrivateProfileSection(szSection, *pszBuffer, dwSize, szFile)) == dwSize - 2)
	{
		dwSize += dwAllocGranularity;
		delete []*pszBuffer;
		*pszBuffer = new TCHAR[dwSize];
		if(!*pszBuffer)
			return false;
	}
//	if(!dwReqSize)
//	{
//		delete []*pszBuffer;
//		*pszBuffer = NULL;
//		return false;
//	}
	return true;
}

static bool DeleteSections(LPCTSTR szSectionPrefix, LPCTSTR szFile)
{
	LPTSTR szBuffer = NULL;
	if(!GetSectionNames(&szBuffer, szFile))
		return false;
	LPCTSTR pSection = szBuffer;
	size_t dwSectionPrefixLen = _tcslen(szSectionPrefix);
	while(*pSection)
	{
		if(_tcsnicmp(pSection, szSectionPrefix, dwSectionPrefixLen) == 0)
			WritePrivateProfileSection(pSection, NULL, szFile);
		pSection += _tcslen(pSection) + 1;
	}
	delete []szBuffer;
	return true;
}

static void AddSectionsToMap(secmap &Map, LPCTSTR pSection)
{
	while(*pSection)
	{
		LPCTSTR pEq = _tcschr(pSection, _T('='));
		if(pEq)
		{
			tstring key(pSection, pEq - pSection);
			tstring val(pEq + 1);
			Map[key] = val;
		}
		pSection += _tcslen(pSection) + 1;
	}
}

static bool AddSections(LPCTSTR szSection, LPCTSTR szSrcFile, LPCTSTR szDstFile)
{
	LPTSTR szBufferSrc = NULL;
	if(!GetSection(&szBufferSrc, szSection, szSrcFile))
		return false;
	LPTSTR szBufferDst = NULL;
	GetSection(&szBufferDst, szSection, szDstFile);
	if(szBufferDst)
	{
		secmap Map;
		AddSectionsToMap(Map, szBufferDst);
		AddSectionsToMap(Map, szBufferSrc);
		delete []szBufferSrc;
		secmap::iterator it;
		unsigned dwReqSizeInTChars = 0;
		for(it = Map.begin(); it != Map.end(); ++it)
			dwReqSizeInTChars += it->first.size() + 1 + it->second.size() + 1;
		dwReqSizeInTChars += 1;
		szBufferSrc = new TCHAR[dwReqSizeInTChars];
		if(!szBufferSrc)
		{
			delete []szBufferDst;
			return false;
		}
		LPTSTR pSection = szBufferSrc;
		for(it = Map.begin(); it != Map.end(); ++it)
		{
			_tcscpy(pSection, it->first.c_str());
			pSection += it->first.size();
			_tcscpy(pSection, _T("="));
			pSection += 1;
			_tcscpy(pSection, it->second.c_str());
			pSection += it->second.size();
			*pSection++ = 0;
		}
		*pSection = 0;
	}
	bool bRetValue = WritePrivateProfileSection(szSection, szBufferSrc, szDstFile) != 0;
	delete []szBufferDst;
	delete []szBufferSrc;
	return bRetValue;
}

static bool ProcessFile(LPCTSTR szFile, LPCTSTR szResFile)
{
	TCHAR szFileBuffer[MAX_PATH];
	if(!_tcschr(szFile, _T('\\')) && !_tcschr(szFile, _T('/')))
	{
		if(GetCurrentDirectory(_countof(szFileBuffer), szFileBuffer))
		{
			_tcscat_s(szFileBuffer, _countof(szFileBuffer), _T("\\"));
			_tcscat_s(szFileBuffer, _countof(szFileBuffer), szFile);
			szFile = szFileBuffer;
		}
	}
	TCHAR szResFileBuffer[MAX_PATH];
	if(!_tcschr(szResFile, _T('\\')) && !_tcschr(szResFile, _T('/')))
	{
		if(GetCurrentDirectory(_countof(szResFileBuffer), szResFileBuffer))
		{
			_tcscat_s(szResFileBuffer, _countof(szResFileBuffer), _T("\\"));
			_tcscat_s(szResFileBuffer, _countof(szResFileBuffer), szResFile);
			szResFile = szResFileBuffer;
		}
	}
	LPTSTR szBuffer = NULL;
	if(!GetSectionNames(&szBuffer, szFile))
		return false;
	LPCTSTR pSection = szBuffer;
	while(*pSection)
	{
		if(*pSection == _T('-'))
			DeleteSections(pSection + 1, szResFile);
		else
			AddSections(pSection, szFile, szResFile);
		pSection += _tcslen(pSection) + 1;
	}
	delete []szBuffer;
	return true;
}

static bool IsFileUnicode(LPCTSTR szFile)
{
	HANDLE hFile = CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return false;
	BYTE Signature[2];
	DWORD dwBytesRead = 0;
	BOOL bReadRes = ReadFile(hFile, Signature, sizeof(Signature), &dwBytesRead, NULL);
	CloseHandle(hFile);
	if(!bReadRes || dwBytesRead != sizeof(Signature))
		return false;
	return Signature[0] == 0xff && Signature[1] == 0xfe;
}

static bool CreateOutputFile(LPCTSTR szFile, bool bOutputIsUnicode)
{
	HANDLE hFile = CreateFile(szFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return false;
	LPCVOID pBufferToWrite = bOutputIsUnicode ? (LPCVOID)STR_REGEDIT4_SIGN_W : (LPCVOID)STR_REGEDIT4_SIGN_A;
	DWORD dwSize = bOutputIsUnicode ? sizeof(STR_REGEDIT4_SIGN_W) : sizeof(STR_REGEDIT4_SIGN_A), dwSizeWritten = 0;
	BOOL bWriteRes = WriteFile(hFile, pBufferToWrite, dwSize, &dwSizeWritten, NULL);
	CloseHandle(hFile);
	if(!bWriteRes || dwSizeWritten != dwSize)
	{
		DeleteFile(szFile);
		return false;
	}
	return true;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR     lpCmdLine,
					   int       nCmdShow)
{
	if(__argc < 3)
	{
		MessageBox(NULL, STR_REGMERGER, STR_REGMERGERCAPTION, MB_OK|MB_ICONINFORMATION);
		return 1;
	}
	bool bOutputIsUnicode = false;
	for(int n = 2; n < __argc; ++n)
		bOutputIsUnicode |= IsFileUnicode(__targv[n]);
	if(!CreateOutputFile(__targv[1], bOutputIsUnicode))
		return 2;
	for(int n = 2; n < __argc; ++n)
	{
		if(!ProcessFile(__targv[n], __targv[1]))
		{
			DeleteFile(__targv[1]);
			return 3;
		}
	}
	return 0;
}

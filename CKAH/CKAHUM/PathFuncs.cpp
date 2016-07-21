#include "stdafx.h"
#include <CKAH/ckahum.h>
#include "PathFuncs.h"
#include "UnicodeConv.h"
#include "malloc.h"
#include <stuff/StdString.h>

DWORD GetLongPathName (CStdStringW strShortPath, CStdStringW &strLongPath)
{
	int iFound = strShortPath.ReverseFind (L'\\');

	if (iFound > 1)
	{
		// recurse to peel off components
		//
		if (GetLongPathName(strShortPath.Left (iFound), strLongPath) > 0)
		{
			strLongPath += L'\\';
				
				if (strShortPath.Right(1) != L"\\")
				{
					HANDLE hFind;

					if (g_bIsNT)
					{
						WIN32_FIND_DATAW findData;
						
						// append the long component name to the path
						//
						if (INVALID_HANDLE_VALUE != (hFind = ::FindFirstFileW (strShortPath, &findData)))
						{
							strLongPath += findData.cFileName;
							::FindClose (hFind);
						}
						else
						{
							// if FindFirstFile fails, return the error code
							//
							strLongPath.Empty();
							return 0;
						}
					}
					else
					{
						WIN32_FIND_DATAA findData;
						
						// append the long component name to the path
						//
						if (INVALID_HANDLE_VALUE != (hFind = ::FindFirstFileA (CStdStringA (strShortPath), &findData)))
						{
							strLongPath += findData.cFileName;
							::FindClose (hFind);
						}
						else
						{
							// if FindFirstFile fails, return the error code
							//
							strLongPath.Empty();
							return 0;
						}
					}
				}
		}
	}
	else
	{
		strLongPath = strShortPath;
	}
	
	return strLongPath.GetLength();
}

static wchar_t * _AtoU(const char *cstr, int cstrlen, wchar_t *wstr)
{
    for (int i = cstrlen; i >= 0; --i) wstr[i] = (wchar_t)(unsigned char)cstr[i];
    return wstr;
}

#define AtoU(cstr, cstrlen)  (_AtoU(cstr, cstrlen, (wchar_t*)_alloca((cstrlen + 1)*sizeof(wchar_t))))

static void ConvertMultiStrToPseudoUnicode (LPBYTE pBuff, DWORD dwSize)
{
	int len;
	std::string str;
	while (dwSize > sizeof(WCHAR))
	{
		str = __LPSTR ((LPCWSTR)pBuff); // Unicode -> ANSI
		_AtoU(str.c_str(), str.size(), (LPWSTR)pBuff);

		len = (wcslen ((LPCWSTR)pBuff) + 1) * sizeof (WCHAR);
		dwSize -= len;
		pBuff += len;
	}
}

bool GetPathParam (LPCWSTR szInitialPath, IN OUT DWORD &dwParamSize, OUT BYTE *pBuff)
{
	DWORD dwUserParamSize = dwParamSize;
	
	if (_wcsicmp (szInitialPath, CKAHFW::szwSystemAppName) == 0)
	{
		DWORD dwSize = (wcslen (CKAHFW::szwSystemAppName) + 1 + 1) * sizeof (WCHAR);
		
		dwParamSize = dwSize;

		if (dwUserParamSize < dwSize)
			return false;

		if (pBuff)
		{
			ZeroMemory (pBuff, dwSize);
			wcscpy ((WCHAR *)pBuff, CKAHFW::szwSystemAppName);
		}

		return true;
	}

	bool bUseShortPath = true, bUseLongPath = true;

	// short path

	DWORD dwShortSize = MAX_PATH * 2; /*IOSGetShortPathNameW (szInitialPath, NULL, 0);*/

//	if (dwShortSize == 0)
//	{
//		assert (0);
//		return false;
//	}

	WCHAR *szShortPath = (WCHAR *)_alloca (dwShortSize * sizeof (WCHAR));
	
	dwShortSize = IOSGetShortPathNameW (szInitialPath, szShortPath, dwShortSize);
	if (!dwShortSize)
	{
		//assert (0);
		bUseShortPath = false;
	}

	// long path

	CStdStringW szLongPath;
	DWORD dwLongSize = GetLongPathName (szInitialPath, szLongPath);

	if (dwLongSize == 0)
	{
		//assert (0);
		bUseLongPath = false;
	}

	if (bUseShortPath && bUseLongPath && _wcsicmp (szShortPath, szLongPath) == 0)
		bUseLongPath = false;

	bool bUseInitialPath = true;

	if(bUseLongPath && _wcsicmp(szInitialPath, szLongPath) == 0)
		bUseInitialPath = false;
	else if(bUseShortPath && _wcsicmp(szInitialPath, szShortPath) == 0)
		bUseInitialPath = false;

	DWORD dwSize = (
					(bUseShortPath ? (dwShortSize + 1) : 0) +
					(bUseLongPath ? (dwLongSize + 1) : 0) +
					(bUseInitialPath ? (wcslen (szInitialPath) + 1) : 0) +
					1
				) * sizeof (WCHAR);

	dwParamSize = dwSize;

	if (dwUserParamSize < dwSize)
		return false;

	if (pBuff)
	{
		ZeroMemory (pBuff, dwSize);

		WCHAR *pTempPtr = (WCHAR *)pBuff;

		if (bUseLongPath)
		{            
			wcscpy (pTempPtr, szLongPath);            
            _wcsupr(pTempPtr);

			pTempPtr += dwLongSize + 1;
		}

		if (bUseShortPath)
		{
			wcscpy (pTempPtr, szShortPath);
            _wcsupr(pTempPtr);
			pTempPtr += dwShortSize + 1;
		}

		if (bUseInitialPath)
		{
			wcscpy (pTempPtr, szInitialPath);
            _wcsupr(pTempPtr);
		}

		if (!g_bIsNT)
			ConvertMultiStrToPseudoUnicode (pBuff, dwSize);
	}

	return true;
}

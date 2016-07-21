#include "stdafx.h"
#include "CKAHStorage.h"
#include "CKAHManifest.h"
#include "UnicodeConv.h"
#include "malloc.h"

CCKAHRegStorageImpl::CCKAHRegStorageImpl (HKEY hKey, LPCWSTR szSubKey) :
	m_hKey (hKey)
{
	m_szSubKey = _wcsdup (szSubKey);
}

CCKAHRegStorageImpl::~CCKAHRegStorageImpl ()
{
	free ((void *)m_szSubKey);
}

void CCKAHRegStorageImpl::SetPath (HKEY hKey, LPCWSTR szSubKey)
{
	m_hKey = hKey;

	free ((void *)m_szSubKey);
	m_szSubKey = _wcsdup (szSubKey);
}

DWORD CCKAHRegStorageImpl::GetParam (LPCWSTR IN szParamName, DWORD IN nParamValueBufferLength, LPWSTR OUT szParamValue)
{
	if (wcslen (szParamName) == 0 || m_hKey == NULL || m_szSubKey == NULL)
		return 0;

	std::wstring sParamName (szParamName);
	std::wstring sParamPath (szParamName);

	std::wstring::size_type nLastSlashPos;

	if ((nLastSlashPos = sParamName.rfind (L"\\")) != std::wstring::npos)
	{
		sParamName = sParamPath.substr (nLastSlashPos + 1);
		sParamPath = std::wstring (m_szSubKey) + L"\\" + sParamPath.erase (nLastSlashPos);
	}
	else
		sParamPath = m_szSubKey;

	HKEY hValKey = NULL;
	LONG lRegOpenResult;

	if ((lRegOpenResult = RegOpenKeyEx (m_hKey, __LPTSTR (sParamPath.c_str ()), 0, KEY_READ, &hValKey)) != ERROR_SUCCESS)
		return 0;

	DWORD dwReqSize = 0, dwReqSizeBytes = 0, dwType = 0;
	if (RegQueryValueEx (hValKey, __LPTSTR (sParamName.c_str ()), NULL, &dwType, NULL, &dwReqSize) == ERROR_SUCCESS &&
			dwType == REG_SZ)
	{
		dwReqSizeBytes = dwReqSize / sizeof (TCHAR) * sizeof (WCHAR);

		if (szParamValue != NULL)
		{
			LPVOID szParamVal = _alloca (dwReqSizeBytes);

			if (g_bIsNT)
			{
				if (RegQueryValueExW (hValKey, sParamName.c_str (), NULL, &dwType, (LPBYTE)szParamVal, &dwReqSizeBytes) == ERROR_SUCCESS &&
						dwType == REG_SZ && dwReqSizeBytes <= nParamValueBufferLength)
				{
					memcpy (szParamValue, szParamVal, dwReqSizeBytes);
				}
				else
					dwReqSizeBytes = 0;
			}
			else
			{
				if (RegQueryValueExA (hValKey, __LPSTR (sParamName.c_str ()), NULL, &dwType, (LPBYTE)szParamVal, &dwReqSize) == ERROR_SUCCESS &&
						dwType == REG_SZ && dwReqSizeBytes <= nParamValueBufferLength)
				{
					wcscpy (szParamValue, __LPWSTR ((LPSTR)szParamVal));
				}
				else
					dwReqSizeBytes = 0;
			}
		}
	}

	RegCloseKey (hValKey);

	return dwReqSizeBytes;
}

bool CCKAHRegStorageImpl::SetParam (LPCWSTR IN szParamName, LPCWSTR IN szParamValue)
{
	if (wcslen (szParamName) == 0 || m_hKey == NULL || m_szSubKey == NULL)
		return 0;

	std::wstring sParamName (szParamName);
	std::wstring sParamPath (szParamName);

	std::wstring::size_type nLastSlashPos;

	if ((nLastSlashPos = sParamName.rfind (L"\\")) != std::wstring::npos)
	{
		sParamName = sParamPath.substr (nLastSlashPos + 1);
		sParamPath = std::wstring (m_szSubKey) + L"\\" + sParamPath.erase (nLastSlashPos);
	}
	else
		sParamPath = m_szSubKey;

	HKEY hValKey = NULL;

	if (RegCreateKeyEx (m_hKey, __LPTSTR (sParamPath.c_str ()), 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hValKey, NULL) != ERROR_SUCCESS)
		return false;

	bool bRetValue;
	
	if (g_bIsNT)
	{
		bRetValue = RegSetValueExW (hValKey,
									sParamName.c_str (),
									NULL,
									REG_SZ,
									(LPBYTE)szParamValue,
									(wcslen (szParamValue) + 1) * sizeof (WCHAR)
								) == ERROR_SUCCESS;
	}
	else
	{
		bRetValue = RegSetValueExA (hValKey,
									__LPTSTR (sParamName.c_str ()),
									NULL,
									REG_SZ,
									(LPBYTE)__LPTSTR (szParamValue),
									(_tcslen (__LPTSTR (szParamValue)) + 1) * sizeof (TCHAR)
								) == ERROR_SUCCESS;
	}
		
	RegCloseKey (hValKey);

	return bRetValue;
}

bool CCKAHRegStorageImpl::ClearParams (LPCWSTR IN szParamName)
{
	if (wcslen (szParamName) == 0 || m_hKey == NULL || m_szSubKey == NULL)
		return false;

	RecurClearRegistryKey (m_hKey, __LPTSTR ((std::wstring (m_szSubKey) + L"\\" + szParamName).c_str ()));

	return true;
}

bool CCKAHRegStorageImpl::Cleanup ()
{
    HMODULE hLib = ::LoadLibrary("shlwapi.dll");
    if (hLib)
    {
        typedef DWORD (__stdcall *tSHDeleteKey)(HKEY hkey, LPCSTR pszSubKey);

        tSHDeleteKey pSHDeleteKey = (tSHDeleteKey)::GetProcAddress(hLib, "SHDeleteKeyA");

        pSHDeleteKey(m_hKey, __LPTSTR(m_szSubKey));
    }

    return true;
}
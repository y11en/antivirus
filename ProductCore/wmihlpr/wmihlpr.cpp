// -----------------------------------
#define _WIN32_DCOM
#include <windows.h>
#include <comdef.h>
#include <atlbase.h>
#include <string>

#include <wbemidl.h>

#include <ProductCore/wmihlpr.h>
#include "ver_mod.h"
#include <ProductCore/plugin/p_wmihlpr.h>
#include <Prague/iface/i_root.h>
#include <map>

#define MAX_GUID_SIZE 128

#ifdef _WIN64
#define SOFTWARE_COMPANY_KEY "Software\\Wow6432Node\\KasperskyLab"
#else
#define SOFTWARE_COMPANY_KEY "Software\\KasperskyLab"
#endif

#ifndef countof
	#define countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

CRITICAL_SECTION g_csWMI;
HANDLE g_hWMISemaphore = NULL;
BOOL g_bExitFlag = FALSE;

class cCoIn
{
	bool m_bInitSucceeded;
public:
	cCoIn() { m_bInitSucceeded = SUCCEEDED(CoInitialize(NULL)); }
	~cCoIn() { if(m_bInitSucceeded) CoUninitialize(); }
};

struct CSAutoLock
{
	CSAutoLock(CRITICAL_SECTION& cs) : m_locker(cs) { ::EnterCriticalSection(&cs); }
	~CSAutoLock() { ::LeaveCriticalSection(&m_locker); }
private:
	CRITICAL_SECTION& m_locker;
};

class cGUIDStr : public std::string
{
public:
	typedef std::string _Base;

	cGUIDStr(LPCSTR _str) : _Base(_str){}
	cGUIDStr(LPCGUID guidID)
	{
		USES_CONVERSION;
		WCHAR szID[MAX_GUID_SIZE] = L""; 
		StringFromGUID2(*guidID, szID, MAX_GUID_SIZE);
		append(W2A(szID));
	}
};

struct cWMIStat
{
	GUID m_GUID;
	DWORD m_dwComponent;
};

template<> struct std::less<cWMIStat>
{
	bool operator()(const cWMIStat& __x, const cWMIStat& __y) const
	{
		if (__x.m_GUID.Data1 < __y.m_GUID.Data1)
			return true;
		else if(__x.m_GUID.Data1 > __y.m_GUID.Data1)
			return false;

		if (__x.m_GUID.Data2 < __y.m_GUID.Data2)
			return true;
		else if(__x.m_GUID.Data2 > __y.m_GUID.Data2)
			return false;

		if (__x.m_GUID.Data3 < __y.m_GUID.Data3)
			return true;
		else if(__x.m_GUID.Data3 > __y.m_GUID.Data3)
			return false;

		if ( *((unsigned __int64*)(__x.m_GUID.Data4)) < *((unsigned __int64*)(__y.m_GUID.Data4)) )				
			return true;
		else if ( *((unsigned __int64*)(__x.m_GUID.Data4)) > *((unsigned __int64*)(__y.m_GUID.Data4)) )				
			return false;

		return __x.m_dwComponent < __y.m_dwComponent;
	}
};

std::map<cWMIStat, DWORD> g_WMIQueue;

bool IsPreXPSP2()
{
	bool bPreXPSP2 = false;

	OSVERSIONINFOEX Info;
	Info.dwOSVersionInfoSize = sizeof(Info);

	if (GetVersionEx ((LPOSVERSIONINFO)&Info) && (Info.dwMajorVersion < 5 || 
		(5 == Info.dwMajorVersion && Info.dwMinorVersion == 0) ||
		(5 == Info.dwMajorVersion && 1 == Info.dwMinorVersion && Info.wServicePackMajor < 2)))
	{
		bPreXPSP2 = true;
	}

	return bPreXPSP2;
}

BOOL IsWow64()
{
	BOOL bIsWow64 = FALSE;

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess,PBOOL Wow64Process);
	LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;

	if (!fnIsWow64Process)
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");
 
	if (fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			// handle error
		}
	}
	return bIsWow64;
}

HRESULT ConnectServer(IWbemServices **ppWbemServices)
{
	CComPtr<IWbemLocator> pWbemLocator;
	HRESULT hr = pWbemLocator.CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER);
	
	if (FAILED(hr))
	{
		PR_TRACE((g_root, prtIMPORTANT, "wmi\tConnectServer pWbemLocator.CoCreateInstance FAILED (HRESULT=%08X)!", hr));
		return hr;
	}

	_bstr_t bstrPath(L"\\\\.\\root\\SecurityCenter");
		
	if (!bstrPath.length())
		return E_OUTOFMEMORY;
		
	return pWbemLocator->ConnectServer(bstrPath, NULL, NULL, NULL, 0, NULL, NULL, ppWbemServices);
}

HRESULT DisableLegacyDetection(const WCHAR *pszLegacyRegKey)
{
	if (NULL == pszLegacyRegKey)
		return E_INVALIDARG;
	
	CRegKey hKey;
	long result = hKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Security Center\\Monitoring"), KEY_WRITE);
	
	if (ERROR_SUCCESS == result)
	{
		CRegKey hKey2;
		
		USES_CONVERSION;
		result = hKey2.Create(hKey, W2CT(pszLegacyRegKey), 0, REG_OPTION_NON_VOLATILE, KEY_WRITE);
		
		if (ERROR_SUCCESS == result)
			result = hKey2.SetDWORDValue(_T("DisableMonitoring"), 1);
	}
	
	return HRESULT_FROM_WIN32(result);
}


HRESULT ReenableLegacyDetection(const WCHAR *pszLegacyRegKey)
{
	if (NULL == pszLegacyRegKey)
		return E_INVALIDARG;

	CRegKey hKey;
	LONG result = hKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Security Center\\Monitoring"), KEY_WRITE);

	if (ERROR_SUCCESS == result)
	{
		USES_CONVERSION;
		result = hKey.DeleteSubKey(W2CT(pszLegacyRegKey));
	}

	return HRESULT_FROM_WIN32(result);
}

//////////////////////////////////////////////////////////////////////////////////////////

void WmiHlp_Read(cGUIDStr& guidID, LPCSTR strName, std::wstring& strValue)
{
	USES_CONVERSION;

	std::string strRegPath(SOFTWARE_COMPANY_KEY "\\WmiHlp\\");
	strRegPath += guidID;

	CRegKey hKey;
	if( hKey.Open(HKEY_LOCAL_MACHINE, A2T((LPSTR)strRegPath.c_str()), KEY_READ) == ERROR_SUCCESS )
	{
		WCHAR pData[0x200]; DWORD dwSize = sizeof(pData);
		if( ::RegQueryValueExW(hKey, A2W(strName), NULL, NULL, (LPBYTE)pData, &dwSize) == ERROR_SUCCESS )
			strValue = pData;
	}
}

void WmiHlp_Write(cGUIDStr& guidID, LPCSTR strName, LPCWSTR strValue)
{
	USES_CONVERSION;

	std::string strRegPath(SOFTWARE_COMPANY_KEY "\\WmiHlp\\");
	strRegPath += guidID;

	CRegKey hKey;
	if( hKey.Create(HKEY_LOCAL_MACHINE, A2T((LPSTR)strRegPath.c_str()), REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE) == ERROR_SUCCESS )
		::RegSetValueExW(hKey, A2W(strName), NULL, REG_SZ, (CONST BYTE *)strValue, (DWORD)wcslen(strValue)*sizeof(WCHAR));
}

bool _WmiHlp_IsKeyEmpty(HKEY hKey)
{
	DWORD nKeys = 0, nVals = 0;
	::RegQueryInfoKey(hKey, NULL, NULL, NULL, &nKeys, NULL, NULL, &nVals, NULL, NULL, NULL, NULL);
	return !nKeys && !nVals;
}

void WmiHlp_Delete(cGUIDStr& guidID, LPCSTR strName)
{
	USES_CONVERSION;

	CRegKey _reg;
	if( _reg.Open(HKEY_LOCAL_MACHINE, A2T((LPSTR)SOFTWARE_COMPANY_KEY), KEY_READ|KEY_WRITE) != ERROR_SUCCESS )
		return;

	bool bDelWmiHlp = false;

	CRegKey _hWmiHlp;
	if( _hWmiHlp.Open(_reg, _T("WmiHlp"), KEY_READ|KEY_WRITE) == ERROR_SUCCESS )
	{
		bool bDelId = false;
		CRegKey _hId;
		if( _hId.Open(_hWmiHlp, A2T((LPSTR)guidID.c_str()), KEY_READ|KEY_WRITE) == ERROR_SUCCESS )
		{
			_hId.DeleteValue(A2T((LPSTR)strName));
			
			if( _WmiHlp_IsKeyEmpty(_hId) )
				bDelId = true;
		}
		
		if( bDelId )
			_hWmiHlp.DeleteSubKey(A2T((LPSTR)guidID.c_str()));
		
		if( _WmiHlp_IsKeyEmpty(_hWmiHlp) )
			bDelWmiHlp = true;
	}

	if( bDelWmiHlp )
		_reg.DeleteSubKey(_T("WmiHlp"));
}

//////////////////////////////////////////////////////////////////////////////////////////

HRESULT wmih_Install(LPCGUID pguidID, const CHAR *pszLegacyRegKey, const WCHAR *pszCompanyName, const WCHAR *pszProductVersion, const WCHAR *pszDisplayName)
{
	PR_TRACE((g_root, prtIMPORTANT, "wmi\tInstall"));

	if( !pguidID )
		return E_INVALIDARG;

	if (IsPreXPSP2())
		return S_FALSE;

	WCHAR wszLegacyRegKey[0x200];
	if( !MultiByteToWideChar(CP_ACP, 0, pszLegacyRegKey, -1, wszLegacyRegKey, countof(wszLegacyRegKey)) )
		return E_INVALIDARG;

	cGUIDStr guidID(pguidID);
	WmiHlp_Write(guidID, "LegacyRegKey", wszLegacyRegKey);
	WmiHlp_Write(guidID, "DisplayName", pszDisplayName);
	WmiHlp_Write(guidID, "CompanyName", pszCompanyName);
	WmiHlp_Write(guidID, "VersionNumber", pszProductVersion);

	if( *wszLegacyRegKey )
		return DisableLegacyDetection(wszLegacyRegKey);

	return S_OK;
}

static bool MakeProductRootPath(LPCWSTR szwTail, LPWSTR szwPath, DWORD dwSize )
{
	if (!szwPath)
		return false;

	HKEY hKey = NULL;
	bool bRet = false;

	if (::RegOpenKeyExW (HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH_W L"\\environment", 0, KEY_READ | KEY_WOW64_32KEY, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_SZ, dwBuferSize = dwSize;
		if (RegQueryValueExW (hKey, L"ProductRoot", NULL, &dwType, (LPBYTE)szwPath, &dwBuferSize) == ERROR_SUCCESS)
		{
			size_t len = lstrlenW(szwPath);
			if(len && len < dwSize-1 && szwPath[len-1]!=L'\\')
			{
				szwPath[len]=L'\\';
				szwPath[len+1]=L'\0';
			}

			wcscat_s(szwPath, dwSize/sizeof(WCHAR), szwTail);
			bRet = true;
		}

		RegCloseKey( hKey );
	}

	return bRet;
}

HRESULT wmih_UpdateStatusEx(cGUIDStr& pguidID, DWORD eComponent, DWORD eStatus)
{
	PR_TRACE((g_root, prtIMPORTANT, "wmi\twmih_UpdateStatusEx (%d,%d)", eComponent, eStatus));

	USES_CONVERSION;
	if (IsPreXPSP2())
		return S_FALSE;

	std::wstring wstrDisplayName, wstrCompanyName, wstrVersionNumber;
	WmiHlp_Read(pguidID, "DisplayName", wstrDisplayName);
	WmiHlp_Read(pguidID, "CompanyName", wstrCompanyName);
	WmiHlp_Read(pguidID, "VersionNumber", wstrVersionNumber);

	if (wstrDisplayName.empty())
		wstrDisplayName = A2W(VER_PRODUCTNAME_STR);

	if (wstrCompanyName.empty())
		wstrCompanyName = A2W(VER_COMPANYNAME_STR);

	if (wstrVersionNumber.empty())
		wstrVersionNumber = A2W(VER_PRODUCTVERSION_STR);

	LPCWSTR lpzClass = NULL, lpzExe = NULL;
	switch( eComponent )
	{
	case wmicAntiVirus:   lpzClass = L"AntiVirusProduct";   lpzExe = L"wmiav.exe"; break;
	case wmicAntiHaker:   lpzClass = L"FirewallProduct";    lpzExe = L"wmifw.exe"; break;
	case wmicAntiSpyWare: lpzClass = L"AntiSpywareProduct"; lpzExe = L"wmias.exe"; break;
	}

	if( !lpzClass )
		return E_INVALIDARG;

	cCoIn _tmpcoin;
	CComPtr<IWbemServices> pWbemServices;
	PR_TRACE((g_root, prtIMPORTANT, "wmi\tConnectServer begin..."));
	HRESULT hr = ConnectServer(&pWbemServices);
	if (FAILED(hr))
	{
		PR_TRACE((g_root, prtIMPORTANT, "wmi\tConnectServer FAILED (HRESULT=%08X)!", hr));
		return hr;
	}

	PR_TRACE((g_root, prtIMPORTANT, "wmi\tConnectServer done successfuly"));


	if( eStatus != wmisUninstalled )
	{
		_bstr_t bstrClass(lpzClass);
		if (!bstrClass.length())
			return E_OUTOFMEMORY;

		CComPtr<IWbemClassObject> pClassObject;
		hr = pWbemServices->GetObject(bstrClass, 0, NULL, &pClassObject, NULL);
		if (FAILED(hr))
		{
			PR_TRACE((g_root, prtIMPORTANT, "wmi\tpWbemServices->GetObject(%S ...) FAILED! (HRESULT=%08X)!", lpzClass, hr));
			return hr;
		}

		CComPtr<IWbemClassObject> pInstanceObject;
		hr = pClassObject->SpawnInstance(0, &pInstanceObject);
		if (FAILED(hr))
		{
			PR_TRACE((g_root, prtIMPORTANT, "wmi\tpClassObject->SpawnInstance FAILED! (HRESULT=%08X)!", hr));
			return hr;
		}

		//NOTE: some of the code to populate instances does not check return values when allocating strings.
		//worst case here is null strings get inadvertedly populated into wmi in low memory situations
		CComVariant v = pguidID.c_str();
		pInstanceObject->Put(L"instanceGuid", 0, &v, 0);

		v = wstrDisplayName.c_str();
		pInstanceObject->Put(L"displayName", 0, &v, 0);

		// Set the companyName property.
		v = wstrCompanyName.c_str();
		pInstanceObject->Put(L"companyName", 0, &v, 0);

		// Set the versionNumber property.
		v = wstrVersionNumber.c_str();
		pInstanceObject->Put(L"versionNumber", 0, &v, 0);


		WCHAR pwchExe[MAX_PATH] = {0,};
		MakeProductRootPath(lpzExe, pwchExe, sizeof(pwchExe));

		if( lstrlenW(pwchExe) )
		{
			v = pwchExe;
			pInstanceObject->Put(L"pathToSignedProductExe", 0, &v, 0);
		}

		if( eComponent == wmicAntiVirus )
		{
			// Set the productUptoDate property
			v = (eStatus & wmisSignaturesUpToDate)?VARIANT_TRUE:VARIANT_FALSE;
			hr = pInstanceObject->Put(L"productUptoDate", 0, &v, 0);

			if (FAILED(hr))
			{
				PR_TRACE((g_root, prtIMPORTANT, "wmi\twmicAntiVirus pInstanceObject->Put(productUptoDate ...) FAILED! (HRESULT=%08X)", hr));
			}

			// Set the onAccessScanningEnabled property.
			v = (eStatus & wmisEnabled)?VARIANT_TRUE:VARIANT_FALSE;
			hr = pInstanceObject->Put(L"onAccessScanningEnabled", 0, &v, 0);
			if (FAILED(hr))
			{
				PR_TRACE((g_root, prtIMPORTANT, "wmi\twmicAntiVirus pInstanceObject->Put(onAccessScanningEnabled ...) FAILED! (HRESULT=%08X)", hr));
			}

		}
		else if( eComponent == wmicAntiSpyWare )
		{
			// Set the productUptoDate property
			v = VARIANT_TRUE;//!!(eStatus & wmisSignaturesUpToDate);
			hr = pInstanceObject->Put(L"productUptoDate", 0, &v, 0);
			if (FAILED(hr))
			{
				PR_TRACE((g_root, prtIMPORTANT, "wmi\twmicAntiSpyWare pInstanceObject->Put(productUptoDate ...) FAILED! (HRESULT=%08X)", hr));
			}

			// Set the onAccessScanningEnabled property.
			v = (eStatus & wmisEnabled)?VARIANT_TRUE:VARIANT_FALSE;
			hr = pInstanceObject->Put(L"productEnabled", 0, &v, 0);
			if (FAILED(hr))
			{
				PR_TRACE((g_root, prtIMPORTANT, "wmi\twmicAntiSpyWare pInstanceObject->Put(productEnabled ...) FAILED! (HRESULT=%08X)", hr));
			}

		}
		else if( eComponent == wmicAntiHaker )
		{
			// Set the enabled property.
			v = (eStatus & wmisEnabled)?VARIANT_TRUE:VARIANT_FALSE;
			hr = pInstanceObject->Put(L"enabled", 0, &v, 0);
			if (FAILED(hr))
			{
				PR_TRACE((g_root, prtIMPORTANT, "wmi\twmicAntiHaker pInstanceObject->Put(enabled ...) FAILED! (HRESULT=%08X)", hr));
			}
		}

		PR_TRACE((g_root, prtIMPORTANT, "wmi\tPutInstance before"));
		return pWbemServices->PutInstance(pInstanceObject, WBEM_FLAG_CREATE_OR_UPDATE, NULL, NULL);
	}

	std::wstring wstrPath(lpzClass);
	wstrPath += L".instanceGuid=\"";
	wstrPath += A2W(pguidID.c_str());
	wstrPath += L"\"";
	
	_bstr_t bstrPath(wstrPath.c_str());
	if (!bstrPath.length())
		return E_OUTOFMEMORY;
	
	return pWbemServices->DeleteInstance(bstrPath, 0, NULL, NULL);
}

HRESULT Update64bitStatus(LPCGUID pguidID, DWORD eComponent, DWORD eStatus)
{
	WCHAR pwchCurrDir[MAX_PATH];
	MakeProductRootPath(L"x64\\wmi64.exe", pwchCurrDir, sizeof(pwchCurrDir));

	WCHAR CommandLine[MAX_PATH];
	wsprintfW(CommandLine, L"\"%s\" \"%S\" \"%d\" \"%d\"", pwchCurrDir, cGUIDStr(pguidID).c_str(), eComponent, eStatus);

	PROCESS_INFORMATION procinfo = {0,};

	STARTUPINFOW startinfo;
	memset(&startinfo, 0, sizeof(startinfo));
	startinfo.cb = sizeof(startinfo);
	startinfo.wShowWindow = SW_HIDE;

	HRESULT hres = S_FALSE;
	if (CreateProcessW(NULL, CommandLine, NULL, NULL, FALSE, CREATE_NO_WINDOW, 
		NULL, NULL, &startinfo, &procinfo))
	{
		DWORD dwexit;
		WaitForSingleObject(procinfo.hProcess, 20000);
		if (GetExitCodeProcess(procinfo.hProcess, &dwexit))
			hres = dwexit;

		PR_TRACE((g_root, prtIMPORTANT, "wmi\twmi64 retcode(%x)", dwexit));

		CloseHandle(procinfo.hProcess);
		CloseHandle(procinfo.hThread);
	}

	return hres;
}

HRESULT wmih_UpdateStatus(LPCGUID pguidID, DWORD eComponent, DWORD eStatus)
{
	if( !pguidID )
		return E_INVALIDARG;

	if (IsWow64())
		return Update64bitStatus(pguidID, eComponent, eStatus);


	return wmih_UpdateStatusEx(cGUIDStr(pguidID), eComponent, eStatus);
}

HRESULT wmih_UpdateStatusAsync(LPCGUID pguidID, DWORD eComponent, DWORD eStatus)
{
	if( !pguidID )
		return E_INVALIDARG;

	if (!g_hWMISemaphore)
		return E_FAIL;

	{
		CSAutoLock _lock(g_csWMI);
		cWMIStat stat = {*pguidID, eComponent};
		g_WMIQueue[stat] = eStatus;
		ReleaseSemaphore(g_hWMISemaphore, 1, NULL);
	}

	return S_OK;
}

DWORD WINAPI g_fnUpdateWMIStatus(LPVOID lpParam)
{
	while(WaitForSingleObject(g_hWMISemaphore, INFINITE)==WAIT_OBJECT_0)
	{
		while(1)
		{
			cWMIStat stat;
			DWORD dwStatus;
			{
				CSAutoLock _lock(g_csWMI);
				if(g_WMIQueue.empty())
					break;

				stat = g_WMIQueue.begin()->first;
				dwStatus = g_WMIQueue.begin()->second;
				g_WMIQueue.erase(g_WMIQueue.begin());
			}

			wmih_UpdateStatus(&stat.m_GUID, stat.m_dwComponent, dwStatus);
		}

		if(g_bExitFlag)
			break;
	}

	return 0;
}

HRESULT wmih_Uninstall(LPCGUID pguidID)
{
	if( !pguidID )
		return E_INVALIDARG;

	if (IsPreXPSP2())
		return S_FALSE;

	cGUIDStr guidID(pguidID);

	std::wstring szLegacyRegKey;
	WmiHlp_Read(guidID, "LegacyRegKey", szLegacyRegKey);

	if( !szLegacyRegKey.empty() )
		ReenableLegacyDetection(szLegacyRegKey.c_str());

	WmiHlp_Delete(guidID, "DisplayName");
	WmiHlp_Delete(guidID, "LegacyRegKey");

	WmiHlp_Delete(guidID, "CompanyName");
	WmiHlp_Delete(guidID, "VersionNumber");

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////

/**************************************************************************

   File:          BandObjs.cpp
   
   Description:   Contains DLLMain and standard OLE COM object creation stuff.

**************************************************************************/

/**************************************************************************
   #include statements
**************************************************************************/

#include <ole2.h>
#include <comcat.h>
#include <olectl.h>
#include "ClsFact.h"

/**************************************************************************
   GUID stuff
**************************************************************************/

//this part is only done once
//if you need to use the GUID in another file, just include Guid.h
#define INITGUID
#include <initguid.h>
#include <shlguid.h>
#include "Guid.h"


#include "ExplrBar.h"
#include "resource.h"
#include <atlbase.h>
#include <atlapp.h>

#include "Localizer.h"

/**************************************************************************
   global variables
**************************************************************************/

HINSTANCE   g_hInst;
UINT        g_DllRefCount;
CAppModule _Module;

/**************************************************************************
   private function prototypes
**************************************************************************/

extern "C" BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID);
BOOL RegisterServer(CLSID, LPCTSTR);
BOOL UnregisterServer(CLSID clsID);
BOOL RegisterComCat(CLSID, CATID);
BOOL UnregisterComCat(CLSID, CATID);
BOOL RegisterIeToolbarButton(CLSID IEButtonClsId, CLSID IEBarClsId, LPCTSTR szButtonText, UINT nIconId, UINT nHotIconId);
BOOL UnregisterIeToolbarButton(CLSID IEButtonClsId);

void IID2String(CLSID clsID, LPTSTR szCLSID);

/**************************************************************************

   DllMain

**************************************************************************/

extern "C" BOOL WINAPI DllMain(  HINSTANCE hInstance, 
                                 DWORD dwReason, 
                                 LPVOID lpReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hInst = hInstance;
		{
			HRESULT hRes = _Module.Init(NULL, hInstance);
			ATLASSERT(SUCCEEDED(hRes));
		}
		break;

	case DLL_PROCESS_DETACH:
		_Module.Term();
		break;
	}
	
	return TRUE;
}                                 

/**************************************************************************

   DllCanUnloadNow

**************************************************************************/

STDAPI DllCanUnloadNow(void)
{
	return (g_DllRefCount ? S_FALSE : S_OK);
}

/**************************************************************************

   DllGetClassObject

**************************************************************************/

STDAPI DllGetClassObject(  REFCLSID rclsid, 
                           REFIID riid, 
                           LPVOID *ppReturn)
{
	*ppReturn = NULL;

	//if we don't support this classid, return the proper error code
	if(!IsEqualCLSID(rclsid, CLSID_SampleExplorerBar))
		return CLASS_E_CLASSNOTAVAILABLE;
   
	//create a CClassFactory object and check it for validity
	CClassFactory *pClassFactory = new CClassFactory(rclsid);
	if(!pClassFactory)
		return E_OUTOFMEMORY;
   
	//get the QueryInterface return for our return value
	HRESULT hResult = pClassFactory->QueryInterface(riid, ppReturn);

	//call Release to decement the ref count - creating the object set it to one 
	//and QueryInterface incremented it - since its being used externally (not by 
	//us), we only want the ref count to be 1
	pClassFactory->Release();

	return hResult;
}

/**************************************************************************

   DllRegisterServer / DllUnregisterServer

**************************************************************************/

STDAPI DllRegisterServer(void)
{
	BOOL bSuccess = TRUE;
	
	CPRRemotePrague Prague(g_hInst);
	cLocalizedResources Res;
	Res.Load(g_hInst, g_root);

	//Register the explorer bar object.
	if( bSuccess )
	{
		bSuccess = RegisterServer(CLSID_SampleExplorerBar, Res.m_sCaption.c_str());
		if( !bSuccess )
			OutputDebugStringA("RegisterServer failed\n");
	}

	//Register the component categories for the explorer bar object.
	if( bSuccess )
	{
		bSuccess = RegisterComCat(CLSID_SampleExplorerBar, CATID_InfoBand);
		if( !bSuccess )
			OutputDebugStringA("RegisterComCat failed\n");
	}

	//Register the explorer toolbar button
	if( bSuccess )
	{
		bSuccess = RegisterIeToolbarButton(CLSID_SampleExplorerBarCtrlBtn, CLSID_SampleExplorerBar, Res.m_sCaption.c_str(), IDI_ICON_IE_TOOLBAR, IDI_ICON_IE_TOOLBAR_HOT);
		if( !bSuccess )
			OutputDebugStringA("RegisterIeToolbarButton failed\n");
	}

	if(!bSuccess)
	{
		DllUnregisterServer();
		return SELFREG_E_CLASS;
	}

	// Remove the cache of the deskbands on Windows 2000. This will cause the new 
	// deskband to be displayed in the toolbar menu the next time the user brings it 
	// up. See KB article Q214842 for more information on this.
	TCHAR    szSubKey[MAX_PATH];
	TCHAR    szCATID[MAX_PATH];
	
	IID2String(CATID_DeskBand, szCATID);
	wsprintf(szSubKey, TEXT("Component Categories\\%s\\Enum"), szCATID);
	RegDeleteKey(HKEY_CLASSES_ROOT, szSubKey);

	IID2String(CATID_InfoBand, szCATID);
	wsprintf(szSubKey, TEXT("Component Categories\\%s\\Enum"), szCATID);
	RegDeleteKey(HKEY_CLASSES_ROOT, szSubKey);

	IID2String(CATID_CommBand, szCATID);
	wsprintf(szSubKey, TEXT("Component Categories\\%s\\Enum"), szCATID);
	RegDeleteKey(HKEY_CLASSES_ROOT, szSubKey);
	
	return S_OK;
}


STDAPI DllUnregisterServer(void)
{
	UnregisterIeToolbarButton(CLSID_SampleExplorerBarCtrlBtn);
	UnregisterComCat(CLSID_SampleExplorerBar, CATID_InfoBand);
	UnregisterServer(CLSID_SampleExplorerBar);
	
	return S_OK;
}


/**************************************************************************

   RegisterServer / UnregisterServer

**************************************************************************/

#define IE_REG_TB_CLSID				TEXT("{E0DD6CAB-2D10-11D2-8F1A-0000F87ABD16}")
#define IE_EXTENTIONS_PATH			TEXT("Software\\Microsoft\\Internet Explorer\\Extensions")
#define IE_SHELL_EXTENTIONS_PATH	TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved")

BOOL RegisterServer(CLSID clsid, LPCTSTR lpszTitle)
{
	typedef struct
	{
		HKEY	hRootKey;
		LPCTSTR	szSubKey;//TCHAR szSubKey[MAX_PATH];
		LPCTSTR	lpszValueName;
		LPCTSTR	szData;//TCHAR szData[MAX_PATH];
	} DOREGSTRUCT, *LPDOREGSTRUCT;

	int      i;
	HKEY     hKey;
	LRESULT  lResult;
	DWORD    dwDisp;
	TCHAR    szSubKey[MAX_PATH];
	TCHAR    szCLSID[MAX_PATH];
	TCHAR    szModule[MAX_PATH];

	//get the CLSID in string form
	IID2String(clsid, szCLSID);

	//get this app's path and file name
	GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule));
	
	//register the CLSID entries
	DOREGSTRUCT ClsidEntries[] = {HKEY_CLASSES_ROOT,   TEXT("CLSID\\%s"),                  NULL,                   lpszTitle,
								  HKEY_CLASSES_ROOT,   TEXT("CLSID\\%s\\InprocServer32"),  NULL,                   szModule,
								  HKEY_CLASSES_ROOT,   TEXT("CLSID\\%s\\InprocServer32"),  TEXT("ThreadingModel"), TEXT("Apartment"),
								  NULL,                NULL,                               NULL,                   NULL};
	for(i = 0; ClsidEntries[i].hRootKey; i++)
	{
		//create the sub key string - for this case, insert the file extension
		wsprintf(szSubKey, ClsidEntries[i].szSubKey, szCLSID);

		if(RegCreateKeyEx(ClsidEntries[i].hRootKey, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp) == ERROR_SUCCESS)
		{
			TCHAR szData[MAX_PATH];

			//if necessary, create the value string
			wsprintf(szData, ClsidEntries[i].szData, szModule);
   
			lResult = RegSetValueEx(hKey, ClsidEntries[i].lpszValueName, 0, REG_SZ, (LPBYTE)szData, lstrlen(szData) + 1);
      		
			RegCloseKey(hKey);
		}
		else
			return FALSE;
	}

	//If running on NT, register the extension as approved.
	OSVERSIONINFO  osvi;
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	GetVersionEx(&osvi);
	if(VER_PLATFORM_WIN32_NT == osvi.dwPlatformId)
	{
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, IE_SHELL_EXTENTIONS_PATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp) == ERROR_SUCCESS)
		{
			TCHAR szData[MAX_PATH];

			//Create the value string.
			lstrcpyn(szData, lpszTitle, ARRAYSIZE(szData));
			lResult = RegSetValueEx(hKey, szCLSID, 0, REG_SZ, (LPBYTE)szData, (lstrlen(szData) + 1) * sizeof(TCHAR));
      
			RegCloseKey(hKey);
		}
		else
			return FALSE;
	}
	
	return TRUE;
}

BOOL UnregisterServer(CLSID clsID)
{
	TCHAR    szCLSID[MAX_PATH];

	//get the CLSID in string form
	IID2String(clsID, szCLSID);

	CRegKey Key;
	if(Key.Open(HKEY_CLASSES_ROOT, TEXT("CLSID")) == ERROR_SUCCESS)
	{
		Key.RecurseDeleteKey(szCLSID);
		Key.Close();
	}

	//If running on NT, register the extension as approved.
	OSVERSIONINFO  osvi;
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	GetVersionEx(&osvi);
	if(VER_PLATFORM_WIN32_NT == osvi.dwPlatformId)
	{
		LONG	err = 0;
		HKEY	hKey;
		DWORD	dwDisp;
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, IE_SHELL_EXTENTIONS_PATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp) == ERROR_SUCCESS)
		{
			err = RegDeleteValue(hKey, szCLSID);
			RegCloseKey(hKey);
		}
	}
	return TRUE;
}

/**************************************************************************

   RegisterComCat / UnregisterComCat

**************************************************************************/

BOOL RegisterComCat(CLSID clsid, CATID CatID)
{
	ICatRegister   *pcr;
	HRESULT        hr = S_OK ;
    
	CoInitialize(NULL);

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
							NULL, 
							CLSCTX_INPROC_SERVER, 
							IID_ICatRegister, 
							(LPVOID*)&pcr);

	if(FAILED(hr))
		OutputDebugStringA("cant instantiate IID_ICatRegister object\n");

	if(SUCCEEDED(hr))
	{
		hr = pcr->RegisterClassImplCategories(clsid, 1, &CatID);
		if(FAILED(hr))
			OutputDebugStringA("cant register RegisterClassImplCategories\n");

		pcr->Release();
	}
        
	CoUninitialize();

	return SUCCEEDED(hr);
}

BOOL UnregisterComCat(CLSID clsid, CATID CatID)
{
	ICatRegister   *pcr;
	HRESULT        hr = S_OK ;
    
	CoInitialize(NULL);

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
							NULL, 
							CLSCTX_INPROC_SERVER, 
							IID_ICatRegister, 
							(LPVOID*)&pcr);

	if(SUCCEEDED(hr))
	{
		hr = pcr->UnRegisterClassImplCategories(clsid, 1, &CatID);

		pcr->Release();
	}
        
	CoUninitialize();

	return SUCCEEDED(hr);
}


/**************************************************************************

   RegisterIeToolbarButton / UnregisterIeToolbarButton

**************************************************************************/

BOOL RegisterIeToolbarButton(CLSID IEButtonClsId, CLSID IEBarClsId, LPCTSTR szButtonText, UINT nIconId, UINT nHotIconId)
{
	TCHAR    szSubKey[MAX_PATH];
	TCHAR    szCLSID[MAX_PATH];
	TCHAR    szModule[MAX_PATH];
	
	//get this app's path and file name
	GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule));

	//get the CLSID in string form
	IID2String(IEButtonClsId, szCLSID);

	// construct registry path
	wsprintf(szSubKey, IE_EXTENTIONS_PATH TEXT("\\%s"), szCLSID);

	// register toolbar button
	HKEY	hKey;
	DWORD	dwDisp;
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp) == ERROR_SUCCESS)
	{
		TCHAR szData[MAX_PATH];
		
		//get the CLSID in string form
		IID2String(IEBarClsId, szCLSID);

		wsprintf(szData, TEXT("%s,%d"), szModule, nHotIconId);
		RegSetValueEx(hKey, TEXT("HotIcon"),		0, REG_SZ, (LPBYTE)szData,			(lstrlen(szData) + 1) * sizeof(TCHAR));
		
		wsprintf(szData, TEXT("%s,%d"), szModule, nIconId);
		RegSetValueEx(hKey, TEXT("Icon"),			0, REG_SZ, (LPBYTE)szData,			(lstrlen(szData) + 1) * sizeof(TCHAR));
		
		RegSetValueEx(hKey, TEXT("ButtonText"),		0, REG_SZ, (LPBYTE)szButtonText,	(lstrlen(szButtonText) + 1) * sizeof(TCHAR));
		RegSetValueEx(hKey, TEXT("CLSID"),			0, REG_SZ, (LPBYTE)IE_REG_TB_CLSID,	(lstrlen(IE_REG_TB_CLSID) + 1) * sizeof(TCHAR));
		RegSetValueEx(hKey, TEXT("BandCLSID"),		0, REG_SZ, (LPBYTE)szCLSID,			(lstrlen(szCLSID) + 1) * sizeof(TCHAR));
		
		RegSetValueEx(hKey, TEXT("Default Visible"),0, REG_SZ, (LPBYTE)TEXT("Yes"),		(lstrlen(TEXT("Yes")) + 1) * sizeof(TCHAR));

		RegCloseKey(hKey);

		return TRUE;
	}
	return FALSE;
}

BOOL UnregisterIeToolbarButton(CLSID IEButtonClsId)
{
	TCHAR    szCLSID[MAX_PATH];

	//get the CLSID in string form
	IID2String(IEButtonClsId, szCLSID);

	HKEY	hKey;
	DWORD	dwDisp;
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, IE_EXTENTIONS_PATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp) == ERROR_SUCCESS)
	{
		RegDeleteKey(hKey, szCLSID);
		RegCloseKey(hKey);
		return TRUE;
	}
	return FALSE;
}

/**************************************************************************

   IID2String

   the szCLSID must have at least 40 symbols
**************************************************************************/
void IID2String(CLSID clsID, LPTSTR szCLSID)
{
	if(szCLSID)
	{
		*szCLSID = 0;

		//get the CLSID in string form
		LPWSTR pwsz;
		StringFromIID(clsID, &pwsz);
		if(pwsz)
		{
#ifdef UNICODE
		   lstrcpyn(szCLSID, pwsz, 40);
#else
		   WideCharToMultiByte( CP_ACP, 0, pwsz, -1, szCLSID, 40, NULL, NULL);
#endif
		   CoTaskMemFree(pwsz);
		}
	}
}





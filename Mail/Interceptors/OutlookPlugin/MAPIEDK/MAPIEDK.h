
#ifndef _MAPIEDK_H_
#define _MAPIEDK_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MAPIEDK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MAPIEDK_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef MAPIEDK_EXPORTS
#define MAPIEDK_API __declspec(dllexport)
#else
#define MAPIEDK_API __declspec(dllimport)
#endif

#define EDK_USES_IID
#define _PRIVATE_EDKDEBUG_H
#include <edk.h>
#include <mapiguid.h>


#include "mapiedk_func_types.h"

#define KEY_MAIL_CLIENT		"SOFTWARE\\Clients\\Mail"
#define KEY_OUTLOOK			"Microsoft Outlook"

struct CMAPIEDK 
{
	CMAPIEDK::CMAPIEDK()
	{ 
		MAPIEDK_FUNC_NULL( MAPIAdminProfiles  );
		//MAPIEDK_FUNC_NULL( MAPIInitialize     );
		MAPIEDK_FUNC_NULL( MAPIUninitialize   );
		MAPIEDK_FUNC_NULL( MAPILogonEx        );
		MAPIEDK_FUNC_NULL( MAPIAllocateBuffer );
		MAPIEDK_FUNC_NULL( MAPIAllocateMore   );
		MAPIEDK_FUNC_NULL( MAPIFreeBuffer     );
		MAPIEDK_FUNC_NULL( CreateIProp        );
		MAPIEDK_FUNC_NULL( HrQueryAllRows     );
		MAPIEDK_FUNC_NULL( UlRelease          );
		MAPIEDK_FUNC_NULL( FPropExists        );
		MAPIEDK_FUNC_NULL( FreePadrlist       );
		MAPIEDK_FUNC_NULL( FreeProws          );
		MAPIEDK_FUNC_NULL( ScDupPropset       ); 
		MAPIEDK_FUNC_NULL( HrMAPIGetPropBinary    ); 
		MAPIEDK_FUNC_NULL( HrMAPIGetPropBoolean   ); 
		MAPIEDK_FUNC_NULL( HrMAPIGetPropString    ); 
		MAPIEDK_FUNC_NULL( HrMAPIGetPropLong      ); 
		MAPIEDK_FUNC_NULL( HrGetOneProp           ); 
		MAPIEDK_FUNC_NULL( HrMAPISetPropBinary    ); 
		MAPIEDK_FUNC_NULL( HrMAPISetPropBoolean   ); 
		MAPIEDK_FUNC_NULL( HrMAPISetPropString    ); 
		MAPIEDK_FUNC_NULL( HrMAPISetPropLong      ); 
		MAPIEDK_FUNC_NULL( HrMAPIDeleteMessage    ); 
		MAPIEDK_FUNC_NULL( HrMAPIMoveMessage    ); 
		MAPIEDK_FUNC_NULL( HrMAPICopyMessage    ); 
		MAPIEDK_FUNC_NULL( HrMAPIGetPropSystime   );
		MAPIEDK_FUNC_NULL( HrMAPIOpenFolderEx     );
		MAPIEDK_FUNC_NULL( HrMAPICreateSizedAddressList   );
		MAPIEDK_FUNC_NULL( HrMAPISetAddressList           );
		MAPIEDK_FUNC_NULL( HrOpenExchangePrivateStore     );
		MAPIEDK_FUNC_NULL( HrMAPIFindOutbox               );
		MAPIEDK_FUNC_NULL( HrMAPICreateAddressList        );
		MAPIEDK_FUNC_NULL( LpValFindProp                  );
		MAPIEDK_FUNC_NULL( PpropFindProp				  );
		MAPIEDK_FUNC_NULL( HrMAPIFindDefaultMsgStore	  );
		MAPIEDK_FUNC_NULL( HrMAPIFindInbox				  );

		m_pMAPIInitialize = NULL;
	};
		MAPIEDK_FUNC_DEF( MAPIAdminProfiles  );
		//MAPIEDK_FUNC_DEF( MAPIInitialize     );
		MAPIEDK_FUNC_DEF( MAPIUninitialize   );
		MAPIEDK_FUNC_DEF( MAPILogonEx        );
		MAPIEDK_FUNC_DEF( MAPIAllocateBuffer );
		MAPIEDK_FUNC_DEF( MAPIAllocateMore   );
		MAPIEDK_FUNC_DEF( MAPIFreeBuffer     );
		MAPIEDK_FUNC_DEF( CreateIProp        );
		MAPIEDK_FUNC_DEF( HrQueryAllRows     );
		MAPIEDK_FUNC_DEF( UlRelease          );
		MAPIEDK_FUNC_DEF( FPropExists        );
		MAPIEDK_FUNC_DEF( FreePadrlist       );
		MAPIEDK_FUNC_DEF( FreeProws          );
		MAPIEDK_FUNC_DEF( ScDupPropset       ); 
		MAPIEDK_FUNC_DEF( HrMAPIGetPropBinary    ); 
		MAPIEDK_FUNC_DEF( HrMAPIGetPropBoolean   ); 
		MAPIEDK_FUNC_DEF( HrMAPIGetPropString    ); 
		MAPIEDK_FUNC_DEF( HrMAPIGetPropLong      ); 
		MAPIEDK_FUNC_DEF( HrGetOneProp           ); 
		MAPIEDK_FUNC_DEF( HrMAPISetPropBinary    ); 
		MAPIEDK_FUNC_DEF( HrMAPISetPropBoolean   ); 
		MAPIEDK_FUNC_DEF( HrMAPISetPropString    ); 
		MAPIEDK_FUNC_DEF( HrMAPISetPropLong      ); 
		MAPIEDK_FUNC_DEF( HrMAPIDeleteMessage    ); 
		MAPIEDK_FUNC_DEF( HrMAPIMoveMessage    ); 
		MAPIEDK_FUNC_DEF( HrMAPICopyMessage    ); 
		MAPIEDK_FUNC_DEF( HrMAPIGetPropSystime   ); 
		MAPIEDK_FUNC_DEF( HrMAPIOpenFolderEx     );
		MAPIEDK_FUNC_DEF( HrMAPICreateSizedAddressList   );
		MAPIEDK_FUNC_DEF( HrMAPISetAddressList           );
		MAPIEDK_FUNC_DEF( HrOpenExchangePrivateStore     );
		MAPIEDK_FUNC_DEF( HrMAPIFindOutbox               );
		MAPIEDK_FUNC_DEF( HrMAPICreateAddressList        );
		MAPIEDK_FUNC_DEF( LpValFindProp                  );
		MAPIEDK_FUNC_DEF( PpropFindProp					 );
		MAPIEDK_FUNC_DEF( HrMAPIFindDefaultMsgStore		 );
		MAPIEDK_FUNC_DEF( HrMAPIFindInbox				 );

		HMODULE FindMAPIModule();
		bool Init();
		HRESULT pMAPIInitialize(LPVOID lpMapiInit);
		
		HRESULT HrLog(IN LPSTR lpszFile, IN ULONG ulLine, IN HRESULT hr, IN DWORD dwLastError);
		BOOL HrFailed(IN LPSTR lpszFile, IN ULONG ulLine, IN HRESULT hr, IN DWORD dwLastError);
		void Assert(IN LPSTR lpszTag, IN LPSTR lpszFile, IN ULONG ulLine, IN DEBUGLEVEL Level, IN BOOL fValue, IN LPSTR lpszFormat, ...);

private:

	MAPIInitialize_t *	m_pMAPIInitialize;
			
};

bool __stdcall MAPIEDK_Init(OUT CMAPIEDK* pMAPIEDK);

typedef bool (__stdcall MAPIEDK_Init_t)(CMAPIEDK*& pMAPIEDK);
inline bool GetMAPIEDK(CMAPIEDK*& pMAPIEDK, HMODULE hModule)
{
	static HINSTANCE _hMAPIEDK = 0;
	static CMAPIEDK* _pMAPIEDK;
	if ( _hMAPIEDK )
	{
		if ( _pMAPIEDK )
		{
			pMAPIEDK = _pMAPIEDK;
			return true;
		}
		else
		{
			return false;
		}
	}

	_hMAPIEDK = LoadLibrary( _T("mapiedk.dll") );
	if ( !_hMAPIEDK )
	{
		// Пробуем грузить из-под вызывающего модуля
		TCHAR szLoadPath[MAX_PATH];
		TCHAR* pModuleName;

		if (!GetModuleFileName(hModule, szLoadPath, sizeof(szLoadPath)))
			*szLoadPath = 0;
		#ifdef _UNICODE
		pModuleName = _tcsrchr(szLoadPath, L'\\');
		#else
		pModuleName = _tcsrchr(szLoadPath, '\\');
		#endif
		if (!pModuleName)
			pModuleName = szLoadPath;
		else
			pModuleName++;
		
		_tcscpy(pModuleName, _T("mapiedk.dll"));
		_hMAPIEDK = LoadLibraryEx(szLoadPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	}

	if ( _hMAPIEDK ) 
	{
		MAPIEDK_Init_t* pMAPIEDK_Init = 
			(MAPIEDK_Init_t*)GetProcAddress(
				_hMAPIEDK, 
				"MAPIEDK_Init"
				);
		if ( pMAPIEDK_Init )
		{
			if ( pMAPIEDK_Init(_pMAPIEDK) )
			{
				pMAPIEDK = _pMAPIEDK;
				return true;
			}
		}
	}
	return false;
}

inline HRESULT CMAPIEDK::pMAPIInitialize(LPVOID lpMapiInit)
{

	if (m_pMAPIInitialize)
	{
		HRESULT hr = E_FAIL;
		HKEY hMailKey = NULL;
		LONG lResult = RegOpenKey(HKEY_LOCAL_MACHINE, KEY_MAIL_CLIENT, &hMailKey);
		if (lResult != ERROR_SUCCESS)
			return E_FAIL;

		char defaultMail[MAX_PATH];
		ULONG ulSize = sizeof(defaultMail);

		lResult = RegQueryValueEx(hMailKey, NULL, 0, NULL, (LPBYTE)defaultMail, &ulSize);

		bool fDefault = false;
		if (lResult == ERROR_SUCCESS)
		{
			if( !strcmp(defaultMail, KEY_OUTLOOK) )
				fDefault = true;
			else
				lResult = RegSetValueEx(hMailKey, NULL, 0, REG_SZ, (LPBYTE)KEY_OUTLOOK, strlen(KEY_OUTLOOK)+1);
		}

		hr = m_pMAPIInitialize(lpMapiInit);

		if (!fDefault)
			RegSetValueEx(hMailKey, NULL, 0, REG_SZ, (LPBYTE)defaultMail, strlen(defaultMail)+1);

		RegCloseKey(hMailKey);

		return hr;
	}

	return E_FAIL;
}

#endif//_MAPIEDK_H_
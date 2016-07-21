#define PR_IMPEX_DEF

#include "stdafx.h"
#include "resource.h"

#include <windows.h>
#include <shlobj.h>

#include "ver_mod.h"

#include <Prague/prague.h>
#include <Prague/pr_loadr.h>
#include <Prague/pr_remote.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h> 
#include <Prague/iface/i_os.h>
#include <Prague/plugin/p_win32loader.h>

#include <Extract/plugin/p_mailmsg.h>

#include <Mail/iface/i_antispamfilter.h>
#include <Mail/plugin/p_mctask.h>
#include <Mail/structs/s_antispam.h>
#include <Mail/structs/s_mc.h>

#include <PPP/common/Tracer.h>
#include <PPP/structs/s_gui.h>

#include <ProductCore/iface/i_taskmanager.h>
#include <ProductCore/structs/s_profiles.h>

#include "antispamousupportclass.h"

#include "trace.h"

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <Prague/iface/e_ktime.h> 
	#include <Prague/iface/e_loader.h> 
IMPORT_TABLE_END

#include <Prague/plugin/p_win32_reg.h>
#include <Extract/plugin/p_memmod.h>
#include <Mail/plugin/p_as_trainsupport.h>

#include "OutlookAddin.h"
#include "PageAntispam.h"

class CGuiRoot : public CRootSink
{
public:
	void GetHelpStorageId(cStrObj& strHelpStorageId)
	{
		strHelpStorageId = "%HelpFile%";
		if( ::g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strHelpStorageId), 0, 0) != errOK_DECIDED )
			strHelpStorageId.erase();
	}
} g_RootSink;



using namespace AntispamOUSupport;

CAntispamOUSupportClass* AntispamOUSupport::g_pAntispamOUSupportClass = NULL;
PRRemoteAPI g_RP_API = {0};
HMODULE g_hModuleRemotePrague = NULL;
hROOT g_root = NULL;
hPLUGIN g_pxstub_ppl = NULL;
static hOBJECT g_hBL = NULL;
static hOBJECT g_hMailChecker = NULL;
static bool g_bAntispamEnabled = false;
static tPROPID g_propMessageIsIncoming;
static tPROPID g_propMailerPID;
static hMSG_RECEIVER g_hMsgReceiver = NULL;
static hREGISTRY     g_hEnvironment = NULL;

CMAPIEDK* g_pMAPIEDK = NULL;

cPrTracer g_Tracer;

typedef HRESULT (WINAPI *PFNGETFOLDERPATH)(HWND hwndOwner, int nFolder, HANDLE hToken,DWORD dwFlags,LPSTR pszPath);
PFNGETFOLDERPATH pfnSHGetFolderPath = NULL; 
inline HRESULT MySHGetFolderPath (HWND hwndOwner, int nFolder, HANDLE hToken,DWORD dwFlags,LPSTR pszPath)
{
	HRESULT hr = S_OK;
	if ( pfnSHGetFolderPath )
		hr = pfnSHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, NULL, 0, pszPath);
	else
	{
	   DWORD dwResult = ExpandEnvironmentStrings( 
				   _T("USERPROFILE=%USERPROFILE%"),
				   pszPath, 
				   MAX_PATH);
		if ( dwResult > (MAX_PATH-sizeof("\\Application Data")) )
			hr = E_FAIL;
		else
			_tcscat(pszPath, _T("\\Application Data"));
	}
	return hr;
};

tERROR pr_call fMsgReceive( hOBJECT _this, tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	cERROR err = errOK;
//	if ( p_msg_cls_id == pmcMCOU_ANTISPAM )
//	{
//		tDWORD dwPID = p_par_buf ? *(tDWORD*)p_par_buf : 0;
//		if ( p_msg_id == pmMCOU_ANTISPAM_ENABLED )
//			err = errOK_DECIDED;
//	}
//	else 
	if ( 
		p_msg_cls_id == pmc_ANTISPAM_ASK_PLUGIN && 
		((cSerializable*)p_par_buf)->isBasedOn(cAntispamAskPlugin::eIID)
		)
	{
		cAntispamAskPlugin* pAntispamAskPlugin = (cAntispamAskPlugin*)p_par_buf;
		if ( pAntispamAskPlugin->m_dwProcessID == GetCurrentProcessId() ) 
			err = errOK_DECIDED;
		else
		{
			// Узнаем: если процесс называется OUTLOOK.EXE или MAPISP32.EXE - тоже возвращаем errOK_DECIDED
			hOS hOs = 0;
			tDWORD dwProcessID;
			err = CALL_SYS_ObjectCreateQuick(g_root, (hOBJECT*) &(hOs), IID_OS, PID_MEMSCAN, 0);
			if (PR_SUCC(err))
			{
				// creating enumerator
				hObjPtr hPtr = 0;
				err = hOs->PtrCreate(&hPtr, 0);
				if (PR_SUCC(err))
				{
					err = hPtr->sysCreateObjectDone();
					if (PR_SUCC(err))
					{
						while ( PR_SUCC(err = hPtr->Next()) )
						{
							dwProcessID = hPtr->propGetDWord(plPID);
							if ( dwProcessID == pAntispamAskPlugin->m_dwProcessID )
							{
								cStringObj szProcessName; 
								if ( PR_SUCC(szProcessName.assign(hPtr, pgOBJECT_NAME)) )
								{
									if ( 
										(szProcessName.compare( "OUTLOOK.EXE",  fSTRING_COMPARE_CASE_INSENSITIVE ) == cSTRING_COMP_EQ ) ||
										(szProcessName.compare( "MAPISP32.EXE", fSTRING_COMPARE_CASE_INSENSITIVE ) == cSTRING_COMP_EQ ) 
										)
									{
										err = errOK_DECIDED;
										break;
									}
								}
							}
						}
					}
					CALL_SYS_ObjectClose(hPtr);
				}
				else PR_TRACE((0, prtIMPORTANT, "fMsgReceive => ObjectCreateQuick(hPtr) returned 0x%X", err));
				CALL_SYS_ObjectClose(hOs);
			}
			else PR_TRACE((0, prtIMPORTANT, "fMsgReceive => ObjectCreateQuick(hOs) returned 0x%X", err));
		}
	}
	return (err != errOK_DECIDED) ? errOK : errOK_DECIDED;
}

HRESULT GetCurrentMAPISession(IN OUT LPMAPISESSION& lpSession)
{
	if ( lpSession )
		return S_OK;
	
	HRESULT hr = g_pMAPIEDK->pMAPILogonEx(
		(ULONG) 0,
		TEXT(""),
		TEXT(""),
		MAPI_EXTENDED,
		&lpSession);
	if ( FAILED(hr) )
		PR_TRACE((0, prtIMPORTANT, "GetCurrentMAPISession => g_pMAPIEDK->pMAPILogonEx failed, 0x%X", hr));
	return hr;
}

HRESULT GetCurrentProfileName(IN LPMAPISESSION& lpSession, OUT std::string* szProfileName)
{
	if ( !szProfileName->empty() )
		return S_OK;
	
	HRESULT hr = GetCurrentMAPISession(lpSession);
	if ( FAILED(hr) )
		return hr;

	LPMAPITABLE     lpStatusTable = NULL;
	SRestriction    sres;
	SPropValue      spvResType;
	LPSRowSet       pRows = NULL;
	LPTSTR          lpszProfileName = NULL;
	SizedSPropTagArray(2, Columns) =
	{
		2, 
		PR_DISPLAY_NAME, 
		PR_RESOURCE_TYPE
	};

	hr = lpSession->GetStatusTable(NULL, &lpStatusTable);
	if ( SUCCEEDED(hr) ) 
		hr = lpStatusTable->SetColumns((LPSPropTagArray)&Columns, NULL);

	if ( SUCCEEDED(hr) ) 
	{
		spvResType.ulPropTag = PR_RESOURCE_TYPE;
		spvResType.Value.ul = MAPI_SUBSYSTEM;
		
		sres.rt = RES_PROPERTY;
		sres.res.resProperty.relop = RELOP_EQ;
		sres.res.resProperty.ulPropTag = PR_RESOURCE_TYPE;
		sres.res.resProperty.lpProp = &spvResType;
		
		if ( SUCCEEDED(hr) ) 
			hr = lpStatusTable->Restrict(&sres, TBL_ASYNC);
		if ( SUCCEEDED(hr) ) 
			hr = lpStatusTable->FindRow(&sres, BOOKMARK_BEGINNING, 0);
		if ( SUCCEEDED(hr) ) 
			hr = lpStatusTable->QueryRows(1,0,&pRows);
		if (SUCCEEDED(hr))
		{
			lpszProfileName = pRows->aRow[0].lpProps[0].Value.lpszA;
			*szProfileName = lpszProfileName;
		} 
		
	}
   
	ULRELEASE(lpStatusTable);
	FREEPROWS(pRows);

	if ( FAILED(hr) )
		PR_TRACE((0, prtIMPORTANT, "GetCurrentProfileName failed"));

	return hr;
}

tERROR PragueInit(HINSTANCE hModule)
{
	tCHAR szLoadPath[MAX_PATH];
	tCHAR* pModuleName;
	tERROR err = errOK;

	if (!GetModuleFileName(hModule, szLoadPath, sizeof(szLoadPath)))
		*szLoadPath = 0;
	pModuleName = strrchr(szLoadPath, '\\');
	if (!pModuleName)
		pModuleName = szLoadPath;
	else
		pModuleName++;
	
	strcpy(pModuleName, PR_REMOTE_MODULE_NAME);
	g_hModuleRemotePrague = LoadLibraryEx(szLoadPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	
	if (!g_hModuleRemotePrague)
	{
		return errMODULE_NOT_FOUND;
	}
	else
	{
		tERROR ( *hGetAPI )(PRRemoteAPI *api) = NULL;

		*(void**)&hGetAPI = GetProcAddress(g_hModuleRemotePrague, "PRGetAPI");
		if( hGetAPI )
			err = hGetAPI(&g_RP_API);

		if( PR_SUCC(err) )
			err = g_RP_API.Initialize(PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH, NULL);

		if (PR_SUCC(err))
			err = g_RP_API.GetRoot(&g_root);
		
		if( PR_SUCC(err) )
		{
			if (PR_FAIL(err = g_root->LoadModule(&g_pxstub_ppl,"pxstub.ppl",strlen("pxstub.ppl"),cCP_ANSI)))
			{
				PR_TRACE((0, prtIMPORTANT, "Сannot load module pxstub.ppl (%terr)", err));
			}
		}

		if( PR_SUCC(err) )
			err = g_root->RegisterCustomPropId(&g_propMessageIsIncoming, npMESSAGE_IS_INCOMING, pTYPE_BOOL | pCUSTOM_HERITABLE);
		if( PR_SUCC(err) )
			err = g_root->RegisterCustomPropId(&g_propMailerPID, npMAILER_PID, pTYPE_DWORD | pCUSTOM_HERITABLE);
		if( PR_SUCC(err) )
			err = g_root->ResolveImportTable(NULL, import_table, PID_APPLICATION);
		
		if( PR_SUCC(err) )
		{
			err = g_root->sysCreateObject((hOBJECT*)&g_hMsgReceiver, IID_MSG_RECEIVER);
			if ( PR_SUCC(err) ) 
				err = g_hMsgReceiver->set_pgRECEIVE_PROCEDURE((tFUNC_PTR)fMsgReceive);
			if ( PR_SUCC(err) ) 
				err = g_hMsgReceiver->sysCreateObjectDone();
			if ( PR_FAIL(err) )
				g_hMsgReceiver->sysCloseObject(), g_hMsgReceiver = NULL;
		}
		
		if( PR_SUCC(err) )
		{
			LPCSTR sSourceName = {"HKLM\\" VER_PRODUCT_REGISTRY_PATH};
			
			err = g_root->sysCreateObject((hOBJECT*)&g_hEnvironment, IID_REGISTRY, PID_WIN32_REG);
			if( PR_SUCC(err) ) err = g_hEnvironment->propSetStr(0, pgROOT_POINT, (void *)sSourceName);
			if( PR_SUCC(err) ) err = g_hEnvironment->propSetBool(pgOBJECT_RO, cTRUE);
			if( PR_SUCC(err) ) err = g_hEnvironment->propSetStr(0, pgROOT_POINT, "environment", 12);
			if( PR_SUCC(err) ) err = g_hEnvironment->sysCreateObjectDone();
			if( PR_FAIL(err) ) 
				g_hEnvironment->sysCloseObject(), g_hEnvironment = NULL;
			else
				PrSetEnviroment((hOBJECT)g_hEnvironment, NULL);
		}
	}
	return err;
}

tERROR PragueTerm()
{
	PR_TRACE((0, prtIMPORTANT, "PragueTerm => start"));

	if ( g_hEnvironment )
	{
		g_hEnvironment->sysCloseObject(), g_hEnvironment = NULL;
		PR_TRACE((0, prtIMPORTANT, "PragueTerm => g_hEnvironment"));
	}

	if ( g_hMsgReceiver )
	{
		if ( g_hBL )
			g_hMsgReceiver->sysUnregisterMsgHandler( pmc_ANTISPAM_ASK_PLUGIN, g_hBL );
		g_hMsgReceiver->sysCloseObject(), g_hMsgReceiver = NULL;
		PR_TRACE((0, prtIMPORTANT, "PragueTerm => g_hMsgReceiver"));
	}

	if ( g_hBL ) 
	{
		g_RP_API.ReleaseObjectProxy(g_hBL);
		PR_TRACE((0, prtIMPORTANT, "PragueTerm => g_hBL"));
	}

	if ( g_hMailChecker ) 
	{
		g_RP_API.ReleaseObjectProxy(g_hMailChecker);
		PR_TRACE((0, prtIMPORTANT, "PragueTerm => g_hMailChecker"));
	}

	if ( g_pxstub_ppl )
	{
		g_pxstub_ppl->Unload();
		PR_TRACE((0, prtIMPORTANT, "PragueTerm => g_pxstub_ppl"));
	}

	if ( g_RP_API.Deinitialize )
	{
		PR_TRACE((0, prtIMPORTANT, "PragueTerm => Unloading"));
		g_RP_API.Deinitialize();
	}
	// здесь нельзя ставить трассировку - упадем
	if ( g_root )
		g_root = NULL;

	if (g_hModuleRemotePrague )
	{
		//FreeLibrary(g_hModuleRemotePrague);
		//PR_TRACE((0, prtIMPORTANT, "PragueTerm => g_hModuleRemotePrague"));
	}

	PR_TRACE((0, prtIMPORTANT, "PragueTerm => finish"));
	return errOK;
}

static CCriticalSection g_cs;

tERROR _GetProxy(hOBJECT& hProxy, char* szId)
{
	CCriticalSection::CLock lock(g_cs);

	tERROR err = errOK;

	if ( hProxy ) 
	{
		if ( g_RP_API.IsValidProxy )
		{
			if ( PR_FAIL(err= g_RP_API.IsValidProxy(hProxy)) )
			{
				if ( g_hMsgReceiver )
					g_hMsgReceiver->sysUnregisterMsgHandler( pmc_ANTISPAM_ASK_PLUGIN, hProxy );
				g_RP_API.ReleaseObjectProxy(hProxy);
				hProxy = NULL;
			}
		}
	}
	
	if ( g_RP_API.GetObjectProxy && !hProxy )
	{
		if ( PR_FAIL(err= g_RP_API.GetObjectProxy(PR_PROCESS_ANY, szId, &hProxy)) ) 
			hProxy = NULL;
		else
		{
			if ( g_hMsgReceiver )
			{
				err = g_hMsgReceiver->sysRegisterMsgHandler(
					pmc_ANTISPAM_ASK_PLUGIN, 
					rmhDECIDER, 
					hProxy, 
					IID_ANY, PID_ANY, IID_ANY, PID_ANY 
					);
			}
		}
	}

	return err;
}

hOBJECT GetBL()
{
	if ( PR_SUCC(_GetProxy(g_hBL, "TaskManager")) )
		return g_hBL;
	return NULL;
}

hOBJECT GetMailChecker()
{
	if ( PR_SUCC(_GetProxy(g_hMailChecker, "OutlookMailChecker")) )
		return g_hMailChecker;
	return NULL;
}

CAntispamOUSupportClass::CAntispamOUSupportClass():
	m_pMAPISession(0),
//	m_pTrainThread(NULL),
	m_hBackgroundScan(0),
	m_pAntispamPluginSettings(NULL),
	m_bPluginHasSettings(cFALSE),
	m_pMDB(NULL),
	m_pTrainSupport(0),
	m_hNotifyWnd(NULL)
{
}

CAntispamOUSupportClass::~CAntispamOUSupportClass()
{
	PR_TRACE((0, prtIMPORTANT, "CAntispamOUSupportClass::~CAntispamOUSupportClass => start"));
	if ( m_hBackgroundScan )
	{
		m_hBackgroundScan->TerminateAndWait();
		delete m_hBackgroundScan;
		m_hBackgroundScan = NULL;
		PR_TRACE((0, prtIMPORTANT, "CAntispamOUSupportClass::~CAntispamOUSupportClass => m_hBackgroundScan"));
	}
	if ( m_hNotifyWnd )
	{
		DestroyWindow(m_hNotifyWnd);
		m_hNotifyWnd = NULL;
	}
	if ( m_pMDBs.size() ) 
	{
		for ( ULONG 
			ulCounter = 0;
			ulCounter < m_pMDBs.size();
			ulCounter++
			)
			delete m_pMDBs.at(ulCounter);
		m_pMDBs.clear();
		PR_TRACE((0, prtIMPORTANT, "CAntispamOUSupportClass::~CAntispamOUSupportClass => m_pMDBs"));
	}
//	if ( m_pTrainThread ) 
//	{
//		m_pTrainThread->TerminateAndWait();
//		delete m_pTrainThread;
//		m_pTrainThread = NULL;
//		PR_TRACE((0, prtIMPORTANT, "CAntispamOUSupportClass::~CAntispamOUSupportClass => m_pTrainThread"));
//	}	
	if ( m_pMAPISession ) 
	{
		m_pMAPISession->Logoff(0, 0, 0);
		ULRELEASE(m_pMAPISession);
		PR_TRACE((0, prtIMPORTANT, "CAntispamOUSupportClass::~CAntispamOUSupportClass => m_pMAPISession"));
	}
	if ( m_pAntispamPluginSettings )
	{
		delete m_pAntispamPluginSettings;
		m_pAntispamPluginSettings = NULL;
		PR_TRACE((0, prtIMPORTANT, "CAntispamOUSupportClass::~CAntispamOUSupportClass => m_pAntispamPluginSettings"));
	}
	if ( m_pTrainSupport ) 
	{
		m_pTrainSupport->sysCloseObject();
		m_pTrainSupport = NULL;
		PR_TRACE((0, prtIMPORTANT, "CAntispamOUSupportClass::~CAntispamOUSupportClass => m_pTrainSupport"));
	}
	if ( m_pMDB )
	{
		delete m_pMDB;
		m_pMDB = NULL;
		PR_TRACE((0, prtIMPORTANT, "CAntispamOUSupportClass::~CAntispamOUSupportClass => m_pMDB"));
	}

	m_szProfileName.erase();
	if ( g_pMAPIEDK )
		g_pMAPIEDK->pMAPIUninitialize();

	PR_TRACE((0, prtIMPORTANT, "CAntispamOUSupportClass::~CAntispamOUSupportClass => finish"));

	g_Tracer.Deinit();
	PragueTerm();
}

const UINT WM_COPYMESSAGE = RegisterWindowMessage("KLASNotify_CopyMessage");

struct CCopyMessageData
{
	LPSPropValue  pProp;
	IMAPIFolder*  lpSrcFolder;
	IMAPIFolder*  lpDstFolder;
	DWORD         dwFlag;
	
	CCopyMessageData(
		LPSPropValue p = 0,
		IMAPIFolder* s = 0,
		IMAPIFolder* d = 0,
		DWORD        f = 0
		):
		pProp(p),
		lpSrcFolder(s),
		lpDstFolder(d),
		dwFlag(f)
		{};
};

LRESULT CALLBACK _NotifyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message != WM_COPYMESSAGE )
		return DefWindowProc(hWnd, message, wParam, lParam);

	CCopyMessageData* pData = (CCopyMessageData*)wParam;
	if ( 
		pData &&
		pData->lpSrcFolder &&
		pData->pProp &&
		pData->lpDstFolder
		)
	{
		//	return pData->lpSrcFolder->CopyMessages(
		//		pData->entryList, 
		//		&IID_IMAPIFolder, 
		//		pData->lpDstFolder, 
		//		NULL, 
		//		NULL, 
		//		pData->dwFlag
		//		);
		switch ( pData->dwFlag ) 
		{
		case MAPI_MOVE:
			return g_pMAPIEDK->pHrMAPIMoveMessage(
				pData->lpSrcFolder, 
				pData->lpDstFolder, 
				pData->pProp->Value.bin.cb,
				(LPENTRYID)pData->pProp->Value.bin.lpb
				);
		case 0:
			return g_pMAPIEDK->pHrMAPICopyMessage(
				pData->lpSrcFolder, 
				pData->lpDstFolder, 
				pData->pProp->Value.bin.cb, 
				(LPENTRYID)pData->pProp->Value.bin.lpb
				);
		}
	}
	return MAPI_E_INVALID_PARAMETER;
}

void CAntispamOUSupportClass::InitNotifyWnd(HINSTANCE hModule)
{
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(WNDCLASSEX));
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.hInstance		= hModule;
	wcex.lpfnWndProc    = _NotifyWndProc;
	wcex.lpszClassName	= "KLASNotify";

	RegisterClassEx(&wcex);

	m_hNotifyWnd = CreateWindowEx(0, "KLASNotify", NULL, WS_POPUP,
		0, 0, 0, 0, GetDesktopWindow(), NULL, hModule, NULL);
}


HRESULT CAntispamOUSupportClass::SetSettings( IN AntispamPluginSettings_t* pAntispamPluginSettings )
{
	cAutoCS cs(m_lock, true);
	if ( !pAntispamPluginSettings )
		return E_FAIL;
	if ( !m_pAntispamPluginSettings )
		return E_FAIL;

	*m_pAntispamPluginSettings = *pAntispamPluginSettings;
	SaveSettings();
	
	return S_OK;	
}

HRESULT CAntispamOUSupportClass::GetSettings( OUT AntispamPluginSettings_t* pAntispamPluginSettings )
{
	cAutoCS cs(m_lock, false);
	if ( !pAntispamPluginSettings )
		return E_FAIL;
	if ( !m_pAntispamPluginSettings )
		return E_FAIL;

	*pAntispamPluginSettings = *m_pAntispamPluginSettings;
	return S_OK;	
}

HRESULT CAntispamOUSupportClass::Init(HINSTANCE hModule) 
{
	PR_TRACE((0, prtIMPORTANT, "CAddin::Init => Start"));
	HRESULT			hr = S_OK;
	MAPIINIT_0      MapiInit = {0, 0};

	if ( PR_FAIL(PragueInit(hModule)) ) 
	{
		PR_TRACE((0, prtIMPORTANT, "CAddin::Init => PragueInit failed"));
		return E_FAIL;
	}

	if( PR_FAIL(g_root->sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION, PID_WIN32LOADER_2LEVEL_CSECT)) )
	{
		PR_TRACE((0, prtIMPORTANT, "CAddin::Init => Create IID_CRITICAL_SECTION failed"));
		return E_FAIL;
	}

	g_Tracer.Init(NULL, true, "mcouas");

	m_pAntispamPluginSettings = new AntispamPluginSettings_t();
	if ( !m_pAntispamPluginSettings )
	{
		PR_TRACE((0, prtIMPORTANT, "CAddin::Init => new AntispamPluginSettings_t failed"));
		return E_FAIL;
	}
	
	if ( !GetMAPIEDK( g_pMAPIEDK, hModule) )
	{
		PR_TRACE((0, prtIMPORTANT, "CAddin::Init => GetMAPIEDK failed"));
		return E_FAIL;
	}

	{
		HINSTANCE hFolderDll = LoadLibrary("shfolder.dll");
		pfnSHGetFolderPath = (PFNGETFOLDERPATH)GetProcAddress(hFolderDll,"SHGetFolderPathA");
	}
	
	ZeroMemory(&MapiInit, sizeof(MAPIINIT_0));
	MapiInit.ulVersion = MAPI_INIT_VERSION;
    MapiInit.ulFlags = 0 ;
	
    hr = g_pMAPIEDK->pMAPIInitialize(&MapiInit);
	if ( FAILED(hr) )
		PR_TRACE((0, prtIMPORTANT, "CAddin::Init => g_pMAPIEDK->pMAPIInitialize failed"));
	if ( SUCCEEDED(hr) )
		GetCurrentProfileName(m_pMAPISession, &m_szProfileName);


	if ( SUCCEEDED(hr) )
	{
		//	m_pTrainSupport = new CTrainSupport( PID_MDB, PID_MAILMSG, MAIL_OS_TYPE_OUTLOOK, GetBL());
		//	m_pTrainSupport = new CTrainSupport( GetBL());
		cERROR err = g_root->sysCreateObject((hOBJECT*)&m_pTrainSupport, IID_AS_TRAINSUPPORT, PID_AS_TRAINSUPPORT);
		
		if (PR_SUCC(err))
			err = m_pTrainSupport->set_pgTrainSupp_TM( GetBL() );

		if (PR_SUCC(err))
			err = m_pTrainSupport->sysCreateObjectDone();

		//cERROR err = g_root->sysCreateObjectQuick( (hOBJECT*)&m_pTrainSupport, IID_AS_TRAINSUPPORT, PID_AS_TRAINSUPPORT );
		if ( PR_FAIL(err) )
		{
			PR_TRACE((0, prtIMPORTANT, "CAddin::Init => sysCreateObjectQuick(AS_TRAINSUPPORT) failed"));
			hr = E_FAIL;
		}
	}
	
	if ( SUCCEEDED(hr) )
	{
		// Сейчас мы сделаем фокус:
		// Мы однократно откроем все доступные хранилища, чтобы их папки стали доступны сессии МАПИ 
		// (см. ремарки статьи МСДН "IMAPISession::OpenEntry" по поводу ошибки MAPI_E_UNKNOWN_ENTRYID)
		ReopenMDBs();
	}

	if ( SUCCEEDED(hr) )
	{
		CRootItem* pRoot = g_root ? CGuiLoader::Instance().CreateAndInitRoot(g_root, 0, &g_RootSink) : NULL;
		LoadSettings();
//		if(pRoot)
//		{
//			if ( FAILED(LoadSettings()) )
//			{
//				CPageAntispamSink(GetActiveWindow()).DoModal(pRoot, "OutlookPlugun.AntispamDlg");
//			}
//		}
	}
	
	if ( SUCCEEDED(hr) )
	{
		InitNotifyWnd(hModule);
		m_hBackgroundScan = new CBackgroundScan(this);
		if ( m_hBackgroundScan )
			m_hBackgroundScan->Start();
	}


	PR_TRACE((0, prtIMPORTANT, "CAddin::Init => Finish"));
	return hr;
}

HRESULT CAntispamOUSupportClass::GetFolderInfoList( IN FolderInfo_t* pFolderInfo, OUT FolderInfoList_t* pFolderInfoList )
{
	GetCurrentProfileName(m_pMAPISession, &m_szProfileName);

	FolderInfo_t fiRoot;
	fiRoot.MDB.szName = "Microsoft Office Outlook";
	fiRoot.szName = m_szProfileName.c_str();

	if(!pFolderInfo)
		pFolderInfo = &fiRoot;

	tERROR err = PR_SUCC(m_pTrainSupport->GetFolderInfoList(pFolderInfo, pFolderInfoList)) ? S_OK : E_FAIL;
	if(pFolderInfo->m_objPtr)
		pFolderInfo->m_objPtr->StepDown();
	return err;
}

HRESULT CAntispamOUSupportClass::CreateSubFolder( FolderInfo_t* pFolderInfo, FolderInfo_t* pNewFolderInfo )
{
	return PR_SUCC(m_pTrainSupport->CreateSubFolder(pFolderInfo, pNewFolderInfo)) ? S_OK : E_FAIL;
}

HRESULT CAntispamOUSupportClass::ReopenMDBs()
{	
	if ( FAILED(GetCurrentMAPISession(m_pMAPISession))) 
		return MAPI_E_NO_ACCESS;
	
	HRESULT hr = S_OK;
	LPMAPITABLE lpMDBTable = NULL;
	LPSRowSet lpRowSet = NULL;
	ULONG ulObjType = 0;
	ULONG ulLoop = 0;
	static SizedSPropTagArray(2, sptaMDBs) = 
	{
		2,
		{
			PR_DISPLAY_NAME,
			PR_ENTRYID,
		}
	};

	if ( SUCCEEDED(hr) )
		hr = m_pMAPISession->GetMsgStoresTable(0, &lpMDBTable);
	if ( SUCCEEDED(hr) )
		hr = g_pMAPIEDK->pHrQueryAllRows (
			lpMDBTable,
			reinterpret_cast<LPSPropTagArray>(&sptaMDBs),
			NULL, 
			NULL,
			0,
			&lpRowSet
			);
	if ( SUCCEEDED(hr) )
	{
		for (ulLoop = 0;ulLoop < lpRowSet->cRows; ulLoop++)
		{
			// Собираем все MDB-шки в m_pMDBs. 
			EntryID_t entry; entry = lpRowSet->aRow[ulLoop].lpProps[1].Value.bin;
			m_pMDBs.push_back( new MDB_t(m_pMAPISession, entry) );
		}
	}
	ULRELEASE(lpMDBTable);
	FREEPROWS(lpRowSet);
	
	return hr;
}

bool CAntispamOUSupportClass::HasSubfolders(FolderInfo_t* pFolderInfo)
{
	return true; //((CTrainSupport*)m_pTrainSupport)->HasSubfolders(pFolderInfo);
}

//HANDLE CAntispamOUSupportClass::TrainEx( 
//									   IN AntispamPluginTrainSettings_t* pAntispamPluginTrainSettings,
//									   IN TrainStatusCallback_t TrainStatusCallback,
//									   IN void* _context
//									   )
//{
//	if ( !pAntispamPluginTrainSettings )
//		return NULL;
//	if ( !m_pMAPISession )
//		return NULL;
//	if ( m_pTrainThread ) 
//	{
//		m_pTrainThread->TerminateAndWait();
//		delete m_pTrainThread;
//		m_pTrainThread = NULL;
//	}
//
//	//////////////////////////////////////////////////////////////////////////
//	//
//	// Стартуем поток обучения и отпускаем его - он нам больше не интересен.
//	// Он сам завершит свою работу по окончание обучения либо по сигналу из 
//	// коллбека. Он сам освободит за собой память.
//	//
//	TrainAsinch_t data;
//	data._this = this;
//	data._context = _context;
//	data.pAntispamPluginTrainSettings = pAntispamPluginTrainSettings;
//	data.TrainStatusCallback = TrainStatusCallback;
//	m_pTrainThread = new CTrainThread(data);
//	if ( m_pTrainThread )
//		return m_pTrainThread->Start();
//
//	return NULL;
//}
//
//HRESULT CAntispamOUSupportClass::Train( 
//									   IN AntispamPluginTrainSettings_t* pFolderList,
//									   IN TrainStatusCallback_t TrainStatusCallback,
//									   IN void* _context
//									   )
//{
//	if ( PR_SUCC( m_pTrainSupport->Train(pFolderList)) )
//		return S_OK;
//	else
//		return E_FAIL;
//}
//
//HRESULT CAntispamOUSupportClass::Train(
//									   LPMDB lpMDB, 
//									   LPMAPIFOLDER lpFolder, 
//									   bool bIsSpam, 
//									   TrainStatusCallback_t TrainStatusCallback,
//									   void* _context,
//									   tUINT ulFolderNumber,
//									   tUINT ulFolderCount
//									   )
//{
////    LPSPropValue pProp = NULL;
////    if ( SUCCEEDED(g_pMAPIEDK->pHrGetOneProp(lpFolder, PR_ENTRYID, &pProp)) )
////	{
////		FolderInfo_t folder; 
////		folder.szName = "[unknown folder]";
////		folder.EntryID = pProp->Value.bin;
////		if ( PR_SUCC(((CTrainSupport*)m_pTrainSupport)->Train(&folder, bIsSpam, TrainStatusCallback, _context, ulFolderNumber, ulFolderCount)) )
////			return S_OK;
////	}
//	return E_FAIL;
//
////	if ( !m_pMAPISession )
////		return E_FAIL;
////	if ( !lpMDB || !lpFolder ) 
////		return MAPI_E_NO_ACCESS;
////
////
////	HRESULT      hr				= S_OK;
////	LPMAPITABLE  lpMessageTable = NULL;
////	LPSRowSet    lpRowSet       = NULL;
////	ULONG        ulLoop			= 0;
////	ULONG		ulCount			= 1024;
////	char*		szFolderName	= "unknown folder";
////	static SizedSPropTagArray(2, sptaMessages) = 
////	{
////		2,
////		{
////			PR_SUBJECT,
////			PR_ENTRYID,
////		}
////	};
////	g_pMAPIEDK->pHrMAPIGetPropString(lpFolder, PR_DISPLAY_NAME, &ulCount, (LPVOID*)&szFolderName);
////	hr = lpFolder->GetContentsTable(
////			MAPI_DEFERRED_ERRORS, 
////			&lpMessageTable);
////	if ( SUCCEEDED(hr) )
////		hr = g_pMAPIEDK->pHrQueryAllRows (
////			lpMessageTable,
////			reinterpret_cast<LPSPropTagArray>(&sptaMessages),
////			NULL,
////			NULL,
////			0,
////			&lpRowSet
////			);
////	if ( SUCCEEDED(hr) )
////	{
////		for (ulLoop = 0; ulLoop < lpRowSet->cRows; ulLoop++)
////		{
////			// получить указатель на письмо и выполнить на нем TrainOnMessage()
////			LPMESSAGE pMessage = NULL;
////			ULONG ulObjType = 0;
////			char* szMessageSubject = "unknown message";
////			ulCount = 1024;
////
////			hr = lpFolder->OpenEntry(
////				lpRowSet->aRow[ulLoop].lpProps[1].Value.bin.cb,
////				reinterpret_cast<LPENTRYID>(lpRowSet->aRow[ulLoop].lpProps[1].Value.bin.lpb),
////				&IID_IMessage,
////				MAPI_BEST_ACCESS,
////				&ulObjType,
////				reinterpret_cast<LPUNKNOWN *>(&pMessage)
////				);
////			if ( SUCCEEDED(hr) )
////			{
////				hr = ProcessOnMessage( pMessage, bIsSpam ? TrainAsSpam : TrainAsHam, NULL );
////				if ( SUCCEEDED(hr) )
////					g_pMAPIEDK->pHrMAPIGetPropString(pMessage, PR_SUBJECT, &ulCount, (LPVOID*)&szMessageSubject);
////			}
////			ULRELEASE(pMessage);
////
////			tUINT ulMessageCount_Total = (lpRowSet->cRows) * ulFolderCount;
////			tUINT ulMessageNumber_Total = (lpRowSet->cRows) * ulFolderNumber + ulLoop+1;
////			hr = TrainStatusCallback(
////				_context, 
////				ulMessageCount_Total, 
////				ulMessageNumber_Total, 
////				szFolderName, 
////				szMessageSubject
////				);
////			MAPIFREEBUFFER(szMessageSubject);
////			if ( FAILED(hr) )
////				break;
////		}
////	}
////
////	MAPIFREEBUFFER(szFolderName);
////	ULRELEASE(lpMessageTable);
////	FREEPROWS(lpRowSet);
////	return hr;
//}
//
bool CAntispamOUSupportClass::CompareEntryID(const EntryID_t* pEntry1, const EntryID_t* pEntry2)
{
	if(FAILED(GetCurrentMAPISession(m_pMAPISession)) || !pEntry1 || !pEntry2)
		return false;
	HRESULT hr = S_OK;
	ULONG ulCompareResult = FALSE;
	hr = m_pMAPISession->CompareEntryIDs(
		pEntry1->cb,
		reinterpret_cast<LPENTRYID>(((EntryID_t*)pEntry1)->get_lpb()),
		pEntry2->cb,
		reinterpret_cast<LPENTRYID>(((EntryID_t*)pEntry2)->get_lpb()),
		0,
		&ulCompareResult
		);
	return SUCCEEDED(hr) && ulCompareResult;
}

HRESULT CAntispamOUSupportClass::ProcessOnMessage( 
	IN LPMESSAGE pMessage,
	IN Action_t Action,
	OUT tDWORD* result
	)
{
	tERROR err = errNOT_OK;
	hOBJECT bl = GetMailChecker();
	if ( !bl )
		bl = GetBL();
	if ( bl )
	{
		hOS hMyOS = 0;
		err = errOK;
		err = g_RP_API.CreateObject(PR_PROCESS_LOCAL, (hOBJECT)bl, (hOBJECT*)&hMyOS, IID_OS, PID_MAILMSG, 0);
		if ( PR_SUCC(err) )
			err = CALL_SYS_PropertySetPtr(hMyOS, plMAILMSG_MESSAGE, pMessage );
		if ( PR_SUCC(err) )
			err = CALL_SYS_PropertySetBool(hMyOS, g_propMessageIsIncoming, TRUE );
		if ( PR_SUCC(err) )
			err = CALL_SYS_PropertySetDWord(hMyOS, g_propMailerPID, ::GetCurrentProcessId());
		if ( PR_SUCC(err) )
			err = CALL_SYS_ObjectCreateDone( hMyOS );
		if ( PR_SUCC(err) )
		{
			if ( Action != Process )
			{
				cAntispamTrainParams params;
				params.p_Action = Action;
				err = bl->sysSendMsg( pmc_ANTISPAM_TRAIN, NULL, hMyOS, &params, SER_SENDMSG_PSIZE);
				if(PR_FAIL(err))
					PR_TRACE((0, prtERROR, "Train failed with %terr", err));
			}
			else
			{
				cMCProcessParams params;
				err = bl->sysSendMsg( pmc_ANTISPAM_PROCESS, NULL, hMyOS, &params, SER_SENDMSG_PSIZE);
				if(PR_SUCC(err))
				{
					if(result)
						*result = params.p_MailCheckerVerdicts;
				}
				else
					PR_TRACE((0, prtERROR, "Processing failed with %terr", err));
			}
		}
		else
			PR_TRACE((0, prtERROR, "Failed to create message object (%terr)", err));
		if ( hMyOS )
			hMyOS->sysCloseObject();
	}
	else
		PR_TRACE((0, prtERROR, "Failed to get BL"));
	if ( PR_SUCC(err) )
	{
		if ( result && (*result & mcv_MESSAGE_CHECKED) )
			g_pMAPIEDK->pHrMAPISetPropLong( pMessage, PR_MESSAGE_AS_PROCESSED, TRUE);
		return S_OK;
	}
	else
		return E_FAIL;
}

HRESULT CAntispamOUSupportClass::ActionOnMessage( 
	IN LPMESSAGE pMessage,
	IN Action_t Action
	)
{
	cAutoCS cs(m_lock, false);
	if ( !pMessage ) 
		return MAPI_E_NO_ACCESS;
	
	HRESULT hr = S_OK;
	if ( SUCCEEDED(hr) ) 
	{
		if ( 
			Action == Process && 
			m_pAntispamPluginSettings->bUseRequestLogic &&
			IsMessageReRequest(pMessage, true) )
		{
			// 1. Посылаем "Спасибо"
			SendMessageThanks( pMessage );
			// 2. Вынимаем все сообщения этого пользователя из папки "Спам"
			hr = RemoveMessages( pMessage );
			return hr;
		}

		tDWORD processResult = 0;
		hr = ProcessOnMessage(pMessage, Action, &processResult);
		_TRACEHR(hr, "CAntispamOUSupportClass::ActionOnMessage => ProcessOnMessage");
		if ( SUCCEEDED(hr) && (Action == Process) )
		{
			// Процессирование
			if ( processResult & mcv_MESSAGE_AS_SPAM )
			{
				if ( m_pAntispamPluginSettings->bUseRequestLogic )
					// Отправим отчет отправителю, рассчитывая на ответ
					SendMessageRequest( pMessage );
				hr = MoveMessage(pMessage, &m_pAntispamPluginSettings->CertainSpamRule);
				_TRACEHR(hr, "CAntispamOUSupportClass::ActionOnMessage => MoveMessage(CertainSpamRule)");
				if ( FAILED(hr) )
					// Игнорируем ошибку перемещения письма
					hr = S_OK;
			}
			else if ( processResult & mcv_MESSAGE_AS_PROBABLE_SPAM )
			{
				hr = MoveMessage(pMessage, &m_pAntispamPluginSettings->PossibleSpamRule);
				_TRACEHR(hr, "CAntispamOUSupportClass::ActionOnMessage => MoveMessage(PossibleSpamRule)");
				if ( FAILED(hr) )
					// Игнорируем ошибку перемещения письма
					hr = S_OK;
			}
			else if ( processResult & mcv_MESSAGE_CHECKED )
				pMessage->SaveChanges( KEEP_OPEN_READWRITE );
		}


		if ( Action != Process )
		{
			// Тренировка
			switch( Action ) 
			{
			case TrainAsSpam:		//! Это - спам
			case RemoveFromHam:		//! Удалить это письмо из базы не_спама
				hr = MoveMessage(pMessage, &m_pAntispamPluginSettings->CertainSpamRule);
				_TRACEHR(hr, "CAntispamOUSupportClass::ActionOnMessage => MoveMessage(TrainAsSpam)");
				break;
			case TrainAsHam:		//! Это - не спам
			case RemoveFromSpam:	//! Удалить это письмо из базы спама
				hr = RemoveMessage( pMessage );
				_TRACEHR(hr, "CAntispamOUSupportClass::ActionOnMessage => RemoveMessage(RemoveFromSpam)");
				break;
			default:
				break;
			}
		}
	}

	return hr;
}

HRESULT HrMoveMessage(HWND hNotifyWnd, LPMAPIPROP lpMessage, IMAPIFolder *lpSrcFolder, IMAPIFolder *lpDstFolder, DWORD dwFlag = MESSAGE_MOVE)
{
    LPSPropValue pProp = NULL;
    HRESULT hr = g_pMAPIEDK->pHrGetOneProp(lpMessage, PR_ENTRYID, &pProp);
	_TRACEHR(hr, "HrMoveMessage => HrGetOneProp(PR_ENTRYID)");
//    if( SUCCEEDED(hr) && pProp ) 
//	{ 
//        ENTRYLIST entryList = {1,&pProp->Value.bin};
//
//		if ( hNotifyWnd )
//		{
//			CCopyMessageData hData(&entryList, lpSrcFolder, lpDstFolder, dwFlag);
//			hr = SendMessage(hNotifyWnd, WM_COPYMESSAGE, (WPARAM)&hData, NULL);
//		}
//		else
//		{
//			hr = lpSrcFolder->CopyMessages(
//				&entryList, 
//				&IID_IMAPIFolder, 
//				lpDstFolder, 
//				NULL, 
//				NULL, 
//				dwFlag
//				);
//		}
//
//		_TRACEHR(hr, "HrMoveMessage => lpSrcFolder->CopyMessages");
//        MAPIFREEBUFFER(pProp);
//    }

    if( SUCCEEDED(hr) && pProp ) 
	{
		if ( hNotifyWnd )
		{
			CCopyMessageData hData(pProp, lpSrcFolder, lpDstFolder, dwFlag);
			hr = SendMessage(hNotifyWnd, WM_COPYMESSAGE, (WPARAM)&hData, NULL);
		}
		else
		{
			switch ( dwFlag ) 
			{
			case MAPI_MOVE:
				hr = g_pMAPIEDK->pHrMAPIMoveMessage(
					lpSrcFolder, 
					lpDstFolder, 
					pProp->Value.bin.cb, 
					(LPENTRYID)pProp->Value.bin.lpb
					);
				break;
			case 0:
				hr = g_pMAPIEDK->pHrMAPICopyMessage(
					lpSrcFolder, 
					lpDstFolder, 
					pProp->Value.bin.cb, 
					(LPENTRYID)pProp->Value.bin.lpb
					);
				break;
			}
		}
		_TRACEHR(hr, "HrMoveMessage => HrMAPI****Message");
        MAPIFREEBUFFER(pProp);
	}
	return hr;
}

HRESULT HrDeleteMessage(LPMAPIPROP lpMessage, IMAPIFolder *lpSrcFolder)
{
    LPSPropValue pProp = NULL;
    HRESULT hr = g_pMAPIEDK->pHrGetOneProp(lpMessage, PR_ENTRYID, &pProp);
	_TRACEHR(hr, "HrDeleteMessage => HrGetOneProp(PR_ENTRYID)");
    if( SUCCEEDED(hr) && pProp ) 
	{ 
		hr = g_pMAPIEDK->pHrMAPIDeleteMessage(
			lpSrcFolder, 
			pProp->Value.bin.cb, 
			(LPENTRYID)pProp->Value.bin.lpb
			);
		_TRACEHR(hr, "HrDeleteMessage => pHrMAPIDeleteMessage");
        MAPIFREEBUFFER(pProp);
    }
    return hr;
}

HRESULT CAntispamOUSupportClass::RemoveMessage(LPMESSAGE pMessage)
{
	if ( FAILED(GetCurrentMAPISession(m_pMAPISession))) 
		return MAPI_E_NO_ACCESS;
	if ( !pMessage )
		return MAPI_E_NO_ACCESS;
	
	HRESULT hr = S_OK;
	LPMAPIFOLDER	pCurrentFolder	=	NULL;
	LPMAPIFOLDER	pOriginalFolder	=	NULL;
	LPENTRYID		lpeidFolder		=	0;
	ULONG			cbeidFolder		=	0;
	ULONG			ulObjType		=	0;

	if ( SUCCEEDED(hr) ) 
		// Узнаем идентификатор текущей папки
		hr = g_pMAPIEDK->pHrMAPIGetPropBinary(
			pMessage, 
			PR_PARENT_ENTRYID, 
			&cbeidFolder, 
			reinterpret_cast<LPVOID *>(&lpeidFolder)
			);
	if ( SUCCEEDED(hr) ) 
		// Получаем указатель на текущую папку
		hr = m_pMAPISession->OpenEntry(
			cbeidFolder,
			reinterpret_cast<LPENTRYID>(lpeidFolder),
			&IID_IMAPIFolder,
			MAPI_BEST_ACCESS,
			&ulObjType,
			reinterpret_cast<LPUNKNOWN *>(&pCurrentFolder)
			);
	MAPIFREEBUFFER(lpeidFolder);
	
	if ( SUCCEEDED(hr) ) 
		// Узнаем идентификатор исходной папки
		hr = g_pMAPIEDK->pHrMAPIGetPropBinary(
			pMessage, 
			PR_MESSAGE_PARENT_FOLDER, 
			&cbeidFolder, 
			reinterpret_cast<LPVOID *>(&lpeidFolder)
			);
	if ( SUCCEEDED(hr) ) 
		// Получаем указатель на исходную папку
		hr = m_pMAPISession->OpenEntry(
			cbeidFolder,
			reinterpret_cast<LPENTRYID>(lpeidFolder),
			&IID_IMAPIFolder,
			MAPI_BEST_ACCESS,
			&ulObjType,
			reinterpret_cast<LPUNKNOWN *>(&pOriginalFolder)
			);
	MAPIFREEBUFFER(lpeidFolder);
	
	if ( (hr != MAPI_E_NOT_FOUND) && FAILED(hr) )
	{
		// Исходная папка не существует.
		// Принимаем в качестве исходной приемную папку по умолчанию
		// <GetReceiveFolder>
		LPMDB lpMDB = 0;
		char* szMessageClass = 0;
		ULONG ulMessageClass = 0;
		// Узнаем идентификатор текущего хранилища
		hr = g_pMAPIEDK->pHrMAPIGetPropBinary(
			pMessage, 
			PR_STORE_ENTRYID, 
			&cbeidFolder, 
			reinterpret_cast<LPVOID *>(&lpeidFolder)
			);
		if ( SUCCEEDED(hr) ) 
			// Получаем указатель на текущеее хранилище
			hr = m_pMAPISession->OpenEntry(
				cbeidFolder,
				reinterpret_cast<LPENTRYID>(lpeidFolder),
				&IID_IMsgStore,
				MAPI_BEST_ACCESS, 
				&ulObjType,
				reinterpret_cast<LPUNKNOWN *>(&lpMDB)
				);
		MAPIFREEBUFFER(lpeidFolder);
		
		if ( SUCCEEDED(hr) ) 
			// Узнаем класс сообщения
			hr = g_pMAPIEDK->pHrMAPIGetPropString(
				pMessage, 
				PR_MESSAGE_CLASS, 
				&ulMessageClass, 
				reinterpret_cast<LPVOID *>(&szMessageClass) 
				);
		if ( SUCCEEDED(hr) ) 
			// Получаем идентификатор приемной папки
			hr = lpMDB->GetReceiveFolder(
				szMessageClass, 
				NULL, 
				&cbeidFolder, 
				&lpeidFolder, 
				NULL 
				);
		if ( SUCCEEDED(hr) ) 
			// Получаем указатель на приемную папку
			hr = m_pMAPISession->OpenEntry(
				cbeidFolder,
				reinterpret_cast<LPENTRYID>(lpeidFolder),
				&IID_IMAPIFolder,
				MAPI_BEST_ACCESS,
				&ulObjType,
				reinterpret_cast<LPUNKNOWN *>(&pOriginalFolder)
				);
		MAPIFREEBUFFER(lpeidFolder);
		MAPIFREEBUFFER(szMessageClass);
		ULRELEASE(lpMDB);
	}

	if ( SUCCEEDED(hr) )
	{
		// Удаляем свойство PR_MESSAGE_PARENT_FOLDER
		SizedSPropTagArray(1, TagsToDelete);
		TagsToDelete.cValues = 1;
		TagsToDelete.aulPropTag[0] = PR_MESSAGE_PARENT_FOLDER;
		hr = pMessage->DeleteProps((LPSPropTagArray)&TagsToDelete, NULL);
		if ( SUCCEEDED(hr) )
			pMessage->SaveChanges(MAPI_DEFERRED_ERRORS);

		// Перемещаем письмо из текущей папки в исходную 
		// (либо в приемную папку для данного класса в текущем хранилище)
		hr = HrMoveMessage(
			m_hNotifyWnd,
			pMessage,
			pCurrentFolder, 
			pOriginalFolder 
			);
	}

	
	ULRELEASE(pOriginalFolder);
	ULRELEASE(pCurrentFolder);

	return hr;
}

HRESULT CAntispamOUSupportClass::OpenEntryForFolder(IN FolderInfo_t* pFolderInfo, OUT LPMAPIFOLDER* ppFolder)
{
	if ( FAILED(GetCurrentMAPISession(m_pMAPISession))) 
		return MAPI_E_NO_ACCESS;

	HRESULT hr = S_OK;
	cERROR err = errOK;
	EntryID_t &EntryID = pFolderInfo->EntryID;
	ULONG ulObjType = 0;
	if ( PR_SUCC(err) )
	{
		hr = m_pMAPISession->OpenEntry(
			EntryID.cb,
			reinterpret_cast<LPENTRYID>((tPTR)EntryID.get_lpb()),
			&IID_IUnknown,
			MAPI_BEST_ACCESS,
			&ulObjType,
			reinterpret_cast<LPUNKNOWN *>(ppFolder)
			);

		PR_TRACE((0, prtIMPORTANT, "OpenEntryForFolder type %d", ulObjType));

		if (MAPI_FOLDER != ulObjType)
			hr = E_FAIL;
		else if ( SUCCEEDED(hr) )
		{
			// Переоткроем эту папку уже по-обычному
			ULRELEASE(*ppFolder);
			hr = m_pMAPISession->OpenEntry(
				EntryID.cb,
				reinterpret_cast<LPENTRYID>((tPTR)EntryID.get_lpb()),
				&IID_IMAPIFolder,
				MAPI_BEST_ACCESS,
				&ulObjType,
				reinterpret_cast<LPUNKNOWN *>(ppFolder)
				);
		}

		_TRACEHR(hr, "CAntispamOUSupportClass::OpenEntryForFolder => m_pMAPISession->OpenEntry");
	}
	return hr;
}

HRESULT CAntispamOUSupportClass::MoveMessage(LPMESSAGE pMessage, AntispamPluginRule_t* pRule)
{
	HRESULT hr = S_OK;
	if ( FAILED(GetCurrentMAPISession(m_pMAPISession))) 
		return MAPI_E_NO_ACCESS;
	if ( !pMessage )
		return MAPI_E_NO_ACCESS;
	if ( pRule->bMarkAsRead ) 
	{
		hr = pMessage->SetReadFlag(0);
		_TRACEHR(hr, "CAntispamOUSupportClass::MoveMessage => pMessage->SetReadFlag(0)");
	}
	if ( pRule->AntispamPluginAction == AntispamPluginAction_KeepAsIs )
	{
		pMessage->SaveChanges(KEEP_OPEN_READWRITE);
		return S_OK;
	}
	
	LPMAPIFOLDER	pCurrentFolder	=	NULL;
	LPMAPIFOLDER	pSpamFolder		=	NULL;
	LPENTRYID		lpeidFolder		=	0;
	ULONG			cbeidFolder		=	0;
	ULONG			ulObjType		=	0;
	LPMDB			lpMDB			=	0;

	if ( SUCCEEDED(hr) ) 
	{
		// Узнаем идентификатор текущей папки
		hr = g_pMAPIEDK->pHrMAPIGetPropBinary(
			pMessage, 
			PR_PARENT_ENTRYID, 
			&cbeidFolder, 
			reinterpret_cast<LPVOID *>(&lpeidFolder)
			);
		_TRACEHR(hr, "CAntispamOUSupportClass::MoveMessage => HrMAPIGetPropBinary(PR_PARENT_ENTRYID)");
	}
	if ( SUCCEEDED(hr) ) 
	{
		// Получаем указатель на текущую папку
		hr = m_pMAPISession->OpenEntry(
			cbeidFolder,
			lpeidFolder,
			NULL, 
			MAPI_BEST_ACCESS, 
			&ulObjType, 
			reinterpret_cast<LPUNKNOWN*>(&pCurrentFolder)
			);
		_TRACEHR(hr, "CAntispamOUSupportClass::MoveMessage => m_pMAPISession->OpenEntry(&pCurrentFolder)");
	}

	tBOOL bDestFolderEqualSrcFolder = cFALSE;

	if(pRule->AntispamPluginAction == AntispamPluginAction_Move ||
			pRule->AntispamPluginAction == AntispamPluginAction_Copy)
	{
		if(SUCCEEDED(hr))
		{
			EntryID_t eidParent;
			eidParent.cb = cbeidFolder;
			eidParent.buff.set(cbeidFolder, (const tBYTE *)lpeidFolder);
			bDestFolderEqualSrcFolder = CompareEntryID(&pRule->FolderForAction.EntryID, &eidParent);
			if(bDestFolderEqualSrcFolder)
				PR_TRACE((0, prtIMPORTANT, "CAntispamOUSupportClass::MoveMessage => Destination and source folders are the same"));
		}
		if(!bDestFolderEqualSrcFolder)
	{
		if ( SUCCEEDED(hr) ) 
		{
			hr = OpenEntryForFolder(&pRule->FolderForAction, &pSpamFolder);
			_TRACEHR(hr, "CAntispamOUSupportClass::MoveMessage => OpenEntryForFolder");
			if ( FAILED(hr) ) 
			{
				CRootItem* pGuiRoot = CGuiLoader::Instance().GetRoot();
#if _BASEGUI_VER >= 0x0200
				tString szCaption; pGuiRoot->GetString(szCaption, PROFILE_LOCALIZE, "AS_Error", "dlg_caption");
				tString szText; pGuiRoot->GetString(szText, PROFILE_LOCALIZE, "AS_Error", "CannotMoveToFolder");
#else
				std::string szCaption = pGuiRoot->GetStringBuf(PROFILE_LOCALIZE, "AS_Error", "dlg_caption");
				std::string szText = pGuiRoot->GetStringBuf(PROFILE_LOCALIZE, "AS_Error", "CannotMoveToFolder");
#endif
				::MessageBox(0, szText.c_str(), szCaption.c_str(), MB_OK);
			}
		}
		if ( SUCCEEDED(hr) )
		{
			// Запоминаем идентификатор текущей папки в свойстве письма PR_MESSAGE_PARENT_FOLDER
			hr = g_pMAPIEDK->pHrMAPISetPropBinary(
				pMessage, 
				PR_MESSAGE_PARENT_FOLDER, 
				cbeidFolder, 
				lpeidFolder
				);
			_TRACEHR(hr, "CAntispamOUSupportClass::MoveMessage => HrMAPISetPropBinary(PR_MESSAGE_PARENT_FOLDER)");
		}
		// Сохраним изменения (результ интересен только для дебага)
		HRESULT hres = pMessage->SaveChanges(KEEP_OPEN_READWRITE);
	}
	}

	if ( SUCCEEDED(hr) )
	{
		switch( pRule->AntispamPluginAction ) 
		{
		case AntispamPluginAction_Move:
			if(bDestFolderEqualSrcFolder)
				break;
			hr = HrMoveMessage(
				m_hNotifyWnd,
				pMessage,
				pCurrentFolder, 
				pSpamFolder 
				);
			_TRACEHR(hr, "CAntispamOUSupportClass::MoveMessage => HrMoveMessage(AntispamPluginAction_Move)");
			if ( SUCCEEDED(hr) )
				break;
		case AntispamPluginAction_Copy:
			if(bDestFolderEqualSrcFolder)
				break;
			hr = HrMoveMessage(
				m_hNotifyWnd,
				pMessage,
				pCurrentFolder, 
				pSpamFolder,
				NULL
				);
			_TRACEHR(hr, "CAntispamOUSupportClass::MoveMessage => HrMoveMessage(AntispamPluginAction_Copy)");
			break;
		case AntispamPluginAction_Delete:
			hr = HrDeleteMessage(
				pMessage, 
				pCurrentFolder
				);
			_TRACEHR(hr, "CAntispamOUSupportClass::MoveMessage => HrDeleteMessage");
			break;
		default:
			break;
		}
	}

	ULRELEASE(pSpamFolder);
	ULRELEASE(pCurrentFolder);
	MAPIFREEBUFFER(lpeidFolder);
	ULRELEASE(lpMDB);

	return hr;
}

bool CAntispamOUSupportClass::ScanNewMessages()
{
	if ( m_hBackgroundScan )
		return m_hBackgroundScan->ScanNow();
	return false;
}

HRESULT CAntispamOUSupportClass::SaveSettings()
{
	if ( FAILED(GetCurrentProfileName(m_pMAPISession, &m_szProfileName))) 
		return MAPI_E_NO_ACCESS;
	if ( !m_pAntispamPluginSettings )
		return E_FAIL;

	cERROR err = errOK;

	if ( g_root )
	{
		hREGISTRY hRegistry;
		err = g_root->sysCreateObject( (hOBJECT*)&hRegistry, IID_REGISTRY, PID_WIN32_REG );
		if ( PR_SUCC(err) )
			err = hRegistry->propSetStr( 0, pgROOT_POINT, "HKCU\\" VER_PRODUCT_REGISTRY_PATH "\\mcou_antispam" );
		if ( PR_SUCC(err) )
			err = hRegistry->propSetBool( pgOBJECT_RO, cFALSE  );
		if ( PR_SUCC(err) )
			err = hRegistry->sysCreateObjectDone();
		if ( PR_SUCC(err) )
			err = g_root->RegSerialize(
				m_pAntispamPluginSettings, 
				SERID_UNKNOWN, 
				hRegistry, 
				m_szProfileName.c_str() 
				);
		if ( hRegistry )
			hRegistry->sysCloseObject();
	}

	if ( PR_SUCC(err) )
	{
		m_bPluginHasSettings = cTRUE;
		return S_OK;
	}
	else
		return E_FAIL;
}

HRESULT CAntispamOUSupportClass::LoadSettings()
{
	if ( FAILED(GetCurrentProfileName(m_pMAPISession, &m_szProfileName))) 
		return MAPI_E_NO_ACCESS;

	cERROR err = errNOT_INITIALIZED;
	if ( g_root )
	{
		hREGISTRY hRegistry;
		err = g_root->sysCreateObject( (hOBJECT*)&hRegistry, IID_REGISTRY, PID_WIN32_REG );
		if ( PR_SUCC(err) )
			err = hRegistry->propSetStr( 0, pgROOT_POINT, "HKCU\\" VER_PRODUCT_REGISTRY_PATH "\\mcou_antispam" );
		if ( PR_SUCC(err) )
			err = hRegistry->sysCreateObjectDone();
		if ( PR_SUCC(err) )
			err = g_root->RegDeserialize(
				(cSerializable**)&m_pAntispamPluginSettings, 
				hRegistry, 
				m_szProfileName.c_str(), 
				AntispamPluginSettings_t::eIID
				);
		if ( hRegistry )
			hRegistry->sysCloseObject();
	}

	if ( PR_FAIL(err) )
		return E_FAIL;
	m_bPluginHasSettings = cTRUE;
	return S_OK;
}

AntispamOUSupport::CAntispamOUSupportClass::MDB_t::~MDB_t()
{
	g_pMAPIEDK->pUlRelease(lpMDB); 
	lpMDB = 0;
};

bool CAntispamOUSupportClass::IsAVPTaskManagerEnabled()
{
	hOBJECT tm = GetBL();
	if ( tm )
		return true;
	else
		return false;
}

check_algorithm_t CAntispamOUSupportClass::GetCheckAlgorithm()
{
	return (check_algorithm_t)m_pAntispamPluginSettings->dwCheckAlgorithm;
}
bool CAntispamOUSupportClass::IsAntispamEngineEnabled()
{
	cERROR err = errOK;
	hOBJECT bl = GetBL();
	if ( bl )
		err = bl->sysSendMsg( pmc_ANTISPAM_PROCESS, NULL, NULL, NULL, NULL);
	g_bAntispamEnabled = ( err == errOK_DECIDED );

	return g_bAntispamEnabled;
}

tERROR CAntispamOUSupportClass::ShowProductASSettings()
{
	hTASKMANAGER tm = (hTASKMANAGER)GetBL();
	if ( tm )
		return tm->AskAction(
			TASKID_TM_ITSELF, 
			cAskGuiAction::SHOW_SETTINGS_WND, 
			&cProfileAction(AVP_PROFILE_ANTISPAM)
			);
	else 
		return errNOT_OK;
}

tBOOL CAntispamOUSupportClass::IsTaskExist(LPCSTR sProfileName)
{
	hTASKMANAGER tm = (hTASKMANAGER)GetBL();
	if(tm)
	{
		cProfile Profile;
		return PR_SUCC(tm->GetProfileInfo(sProfileName, &Profile));
	}
	return cFALSE;
}

bool CAntispamOUSupportClass::CheckPluginSettings()
{
	if ( !m_bPluginHasSettings && (E_FAIL == LoadSettings()) )
		return ShowPluginSettings(true) == -1 ? false : true;
	return true;
}

static HWND GetParentOwner(HWND hWnd)
{
	return (::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) ? ::GetParent(hWnd) : ::GetWindow(hWnd, GW_OWNER);
}

static HWND GetTopLevelParent(HWND hWnd)
{
	if(hWnd == NULL)
		return NULL;
	HWND hWndParent = hWnd;
	HWND hWndT;
	while ((hWndT = GetParentOwner(hWndParent)) != NULL)
		hWndParent = hWndT;
	return hWndParent;
}

int CAntispamOUSupportClass::ShowPluginSettings(bool bAskOnCancel, bool bAdvansedSettings)
{
	CGuiLoader gr;
	CRootItem* pRoot = g_root ? gr.CreateAndInitRoot(g_root, 0, &g_RootSink) : NULL;
	if(pRoot)
	{
		HWND hwnd = GetTopLevelParent(GetActiveWindow());
		if(hwnd == NULL)
			return -1;
		return CPageAntispamSink(hwnd, bAskOnCancel, bAdvansedSettings).DoModal(pRoot, "OutlookPlugun.AntispamDlg");
	}
	return 0;
}

tERROR CAntispamOUSupportClass::MassTrainingStart()
{
	hOBJECT bl = GetMailChecker();
	if ( !bl )
		bl = GetBL();
	if ( bl )
	{
		cAntispamTrainParams params; params.p_Action = cAntispamTrainParams::Train_Begin; params.p_dwPID = ::GetCurrentProcessId();	
			return bl->sysSendMsg( pmc_ANTISPAM_TRAIN, NULL, NULL, &params, SER_SENDMSG_PSIZE );	
	}
	return errNOT_OK;
}

tERROR CAntispamOUSupportClass::MassTrainingFinish()
{
	hOBJECT bl = GetMailChecker();
	if ( !bl )
		bl = GetBL();
	if ( bl )
	{
		cAntispamTrainParams params; params.p_Action = cAntispamTrainParams::MergeDatabase;	params.p_dwPID = ::GetCurrentProcessId();	
			return bl->sysSendMsg( pmc_ANTISPAM_TRAIN, NULL, NULL, &params, SER_SENDMSG_PSIZE );	
	}
	return errNOT_OK;
}
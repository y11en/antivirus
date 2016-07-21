/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	InitTerm.cpp
*		\brief	Initalize / cleanup plugin routines
*		
*		\author Nick Lebedev
*		\date	19.06.2003 17:34:02
*		
*		Example:	
*		
*		
*		
*/		

#include "stdafx.h"
#include "mcou.h"
#include "util.h"
#include "initterm.h"
#include "resource.h"
#include "MCOUAsync.h"

#include <Prague/pr_cpp.h>
#include <Prague/pr_loadr.h>
#include <Prague/pr_wrappers.h>

#include <Mail/iface/i_antispamfilter.h>
#include <Mail/plugin/p_mctask.h>

#include <ProductCore/GuiLoader.h>

#include <PPP/common/Tracer.h>

#include <set>
#include <shlobj.h>

static bool g_bNeedTermEnvironment = false;
static hOBJECT g_hBL = NULL;
static hOBJECT g_hMBL = NULL;
static hMSG_RECEIVER g_hMsgReceiver = NULL;

//CMCThread* g_hMCThread = NULL;
CMAPIEDK* g_pMAPIEDK = NULL;
tPROPID g_propMessageIsIncoming = 0;
tPROPID g_propMailerPID = 0;

cPrTracer g_Tracer;

typedef HRESULT (WINAPI *PFNGETFOLDERPATH)(HWND hwndOwner, int nFolder, HANDLE hToken,DWORD dwFlags,LPSTR pszPath);
static PFNGETFOLDERPATH g_pfnSHGetFolderPath = NULL; 
inline HRESULT MySHGetFolderPath (HWND hwndOwner, int nFolder, HANDLE hToken,DWORD dwFlags,LPSTR pszPath)
{
	HRESULT hr = S_OK;
	if(g_pfnSHGetFolderPath)
		hr = g_pfnSHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, NULL, 0, pszPath);
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
	if(p_msg_cls_id == pmc_MAILCHECKER_PROCESS || p_msg_cls_id == pmc_MAILTASK_PROCESS ||
		p_msg_cls_id == pmc_ANTISPAM_PROCESS || p_msg_cls_id == pmc_ANTISPAM_TRAIN)
	{
		PR_TRACE((0, prtIMPORTANT, "mcou\tProcessing request received"));

		static PRLocker g_cs;
		PRAutoLocker _cs_(g_cs);

		PR_TRACE((0, prtIMPORTANT, "mcou\tProcessing..."));

		// ѕроцессирование
		if(hOBJECT bl = GetBL())
		{
			if(PR_SUCC(bl->sysSendMsg(p_msg_cls_id, p_msg_id, p_ctx, p_par_buf, p_par_buf_len)))
				err = errOK_DECIDED;
		}
		PR_TRACE((0, prtIMPORTANT, "mcou\tProcessing done"));
	}
	return err;
}

//////////////////////////////////////////////////////////////////////////

PRRemoteAPI g_RP_API = {0};
HMODULE g_hModuleRemotePrague = NULL;
hROOT g_root = NULL;
hPLUGIN g_pxstub_ppl = NULL;

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
		OutputDebugString("KAV OutlookPlugin: ");
		OutputDebugString("cannot load module ");
		OutputDebugString(szLoadPath);
		OutputDebugString("\n");
		err = errMODULE_CANNOT_BE_LOADED;
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
				OutputDebugString("KAV OutlookPlugin: ");
				OutputDebugString("cannot load module ");
				OutputDebugString("pxstub.ppl");
				OutputDebugString("\n");
			}
		}
		
		if( PR_SUCC(err) )
			err = g_root->RegisterCustomPropId(&g_propMessageIsIncoming, npMESSAGE_IS_INCOMING, pTYPE_BOOL | pCUSTOM_HERITABLE);
		if( PR_SUCC(err) )
			err = g_root->RegisterCustomPropId(&g_propMailerPID, npMAILER_PID, pTYPE_DWORD | pCUSTOM_HERITABLE);
		if( PR_SUCC(err) )
		{
			// —оздаем g_hMsgReceiver, который будет выстраивать все сообщени€ в одну очередь (от mcou & mcou_antispam)
			err = g_root->sysCreateObject( (hOBJECT*)&g_hMsgReceiver, IID_MSG_RECEIVER );
			if ( PR_SUCC(err) ) 
				err = g_hMsgReceiver->set_pgRECEIVE_PROCEDURE((tFUNC_PTR)fMsgReceive);
			if ( PR_SUCC(err) ) 
				err = g_hMsgReceiver->sysCreateObjectDone();
			if ( PR_SUCC(err) ) 
				g_RP_API.RegisterObject("OutlookMailChecker", (hOBJECT)g_hMsgReceiver);
			if ( PR_FAIL(err) )
			{
				g_hMsgReceiver->sysCloseObject();
				g_hMsgReceiver = NULL;
			}
			else
			{
				g_hMsgReceiver->sysRegisterMsgHandler(
					pmc_ANTISPAM_PROCESS, 
					rmhDECIDER, 
					g_hMsgReceiver, 
					IID_ANY, PID_ANY, IID_ANY, PID_ANY 
					);
				g_hMsgReceiver->sysRegisterMsgHandler(
					pmc_MAILCHECKER_PROCESS, 
					rmhDECIDER, 
					g_hMsgReceiver, 
					IID_ANY, PID_ANY, IID_ANY, PID_ANY 
					);
				g_hMsgReceiver->sysRegisterMsgHandler(
					pmc_ANTISPAM_TRAIN, 
					rmhDECIDER, 
					g_hMsgReceiver, 
					IID_ANY, PID_ANY, IID_ANY, PID_ANY 
					);
				g_hMsgReceiver->sysRegisterMsgHandler(
					pmc_MAILTASK_PROCESS, 
					rmhDECIDER, 
					g_hMsgReceiver, 
					IID_ANY, PID_ANY, IID_ANY, PID_ANY 
					);
			}
		}
	}
	return err;
}

tERROR PragueTerm()
{
	if ( g_hMsgReceiver )
	{
		g_RP_API.UnregisterObject((hOBJECT)g_hMsgReceiver);
		g_hMsgReceiver->sysUnregisterMsgHandler( pmc_MAILTASK_PROCESS,    g_hMsgReceiver );
		g_hMsgReceiver->sysUnregisterMsgHandler( pmc_MAILCHECKER_PROCESS, g_hMsgReceiver );
		g_hMsgReceiver->sysUnregisterMsgHandler( pmc_ANTISPAM_PROCESS,    g_hMsgReceiver );
		g_hMsgReceiver->sysUnregisterMsgHandler( pmc_ANTISPAM_TRAIN,      g_hMsgReceiver );
		g_hMsgReceiver->sysCloseObject();
		g_hMsgReceiver = NULL;
	}
	
	if ( g_RP_API.ReleaseObjectProxy ) 
	{
		if ( g_hMBL ) 
			g_RP_API.ReleaseObjectProxy(g_hMBL);
		if ( g_hBL )
			g_RP_API.ReleaseObjectProxy(g_hBL);
	}
	g_hMBL = NULL;
	g_hBL = NULL;

	if ( g_pxstub_ppl )
	{
		g_pxstub_ppl->Unload();
		g_pxstub_ppl = NULL;
	}

	if ( g_RP_API.Deinitialize )
		g_RP_API.Deinitialize();
	// здесь нельз€ ставить трассировку - упадем
	if ( g_root )
		g_root = NULL;
	return errOK;
}


//! \fn				: InitPlugin
//! \brief			: Initializes plugin (called from DllMain)
//! \return			: BOOL 
//! \param          : HINSTANCE hModule
BOOL InitPlugin(HINSTANCE hModule)
{
	MCOU::g_hInstance = hModule;

	if(!GetMAPIEDK(g_pMAPIEDK, hModule))
		return FALSE;

	if(PR_FAIL(PragueInit(hModule)))
		return FALSE;

	g_Tracer.Init(NULL, true, "mcou");

	return TRUE;
}


//! \fn				: InitEnvironment
//! \brief			: Iniatializes environment
//! \return			: BOOL
BOOL InitEnvironment()
{
	MCOU::CWaitCur wc;

	if(!InitPlugin(MCOU::g_hInstance))
		return FALSE;

	g_bNeedTermEnvironment = true;

	PR_TRACE((0, prtIMPORTANT, "mcou\tInitializing environment"));

	// delete temp files
	MCOU::DeleteTempFilesPrefix ( szMCOUTmpFilePrefix );
	MCOU::DeleteTempFilesPrefix ( szMCOUTmpFilePrefix, _T("com") );

	// “ут мы запускаем поток общени€ с kav50
    StartAsyncProcessorThread();
/*
	if(!g_hMCThread) 
	{
		g_hMCThread = new CMCThread();
		if(g_hMCThread)
			g_hMCThread->Start();
	}
*/
	{
		HINSTANCE hFolderDll = LoadLibrary("shfolder.dll");
		g_pfnSHGetFolderPath = (PFNGETFOLDERPATH)GetProcAddress(hFolderDll,"SHGetFolderPathA");
	}

	return TRUE;
}


inline tERROR _GetProxy(hOBJECT& hProxy, char* szId)
{
	if ( !MCOU::g_bInitSucceeded )
		return errHANDLE_INVALID;

	tERROR err = errOK;

	if ( hProxy ) 
	{
		if ( g_RP_API.IsValidProxy )
		{
			if ( PR_FAIL(err= g_RP_API.IsValidProxy(hProxy)) )
			{
				g_RP_API.ReleaseObjectProxy(hProxy);
				hProxy = NULL;
			}
		}
	}
	
	if ( g_RP_API.GetObjectProxy && !hProxy )
	{
		if ( PR_FAIL(err= g_RP_API.GetObjectProxy(PR_PROCESS_ANY, szId, &hProxy)) ) 
			hProxy = NULL;
	}

	return err;
}

hOBJECT GetBL()
{
	hOBJECT hOldBL = g_hBL;
	if ( PR_SUCC(_GetProxy(g_hBL, "TaskManager")) )
		return g_hBL;

	return NULL;
}

hOBJECT GetMBL(BOOL bCheckIncoming)
{
	hOBJECT hOldMBL = g_hMBL;
	if ( PR_SUCC(_GetProxy(g_hMBL, "OutlookMailChecker")) )
		return g_hMBL;
	return NULL;
}



//! \fn				: TermPlugin
//! \brief			: Cleans up plugin (called from DllMain)
//! \return			: BOOL 
BOOL TermPlugin()
{
	if(g_bNeedTermEnvironment)
		PR_TRACE((0, prtIMPORTANT, "mcou\tUbnormal termination"));
	PragueTerm();
	g_Tracer.Deinit();
	return TRUE;
}


//! \fn				: TermEnvironment
//! \brief			: Cleans up environment
//! \return			: BOOL
BOOL TermEnvironment ()
{
	if ( !g_bNeedTermEnvironment )
		return TRUE;

	MCOU::CWaitCur wc;

	g_bNeedTermEnvironment = false;
	MCOU::g_bInitSucceeded = false;

	TermAsyncProcessorThread();

	// “ут мы останавливаем поток общени€ с kav50
/*
	if ( g_hMCThread )
	{
		g_hMCThread->TerminateAndWait();
		delete g_hMCThread;
		g_hMCThread = NULL;
	}
*/
	TermPlugin();

	return TRUE;
}


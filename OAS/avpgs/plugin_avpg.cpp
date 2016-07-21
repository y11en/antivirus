// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  26 October 2004,  16:52 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Плагин для обработки событий из avpg.sys(vxd)
// Author      -- Sobko
// File Name   -- plugin_avpg.cpp
// Additional info
//    Реализация для Windows 9x/NT
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call AVPG_Register( hROOT g_root );

// AVP Prague stamp end

#include "avpgimpl.h"
#include "exphooks.h"

#include "..\windows\hook\hook\FSDrvLib.h"

DWORD g_dwTlsIndexEventDefinition   = TLS_OUT_OF_INDEXES;
static HMODULE s_hThisModule = NULL;

EXTERN_C hROOT g_root = NULL;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {

	switch( dwReason ) {
	case DLL_PROCESS_ATTACH:
		g_dwTlsIndexEventDefinition   = TlsAlloc();
		s_hThisModule = (HMODULE)hInstance;
		break;
	case DLL_PROCESS_DETACH:
		g_root = NULL;
		DoneExportHooks();
		if (g_dwTlsIndexEventDefinition   != TLS_OUT_OF_INDEXES)
			TlsFree(g_dwTlsIndexEventDefinition);
		FSDrvInterceptorDone();
		break;
	case DLL_THREAD_ATTACH :
		FSDrvInterceptorRegisterInvisibleThread();
		break;
	case DLL_THREAD_DETACH :
		FSDrvInterceptorUnregisterInvisibleThread();
		break;

	case PRAGUE_PLUGIN_LOAD :
		g_root = (hROOT)hInstance;
		*pERROR = errOK;

		CALL_Root_RegisterCustomPropId(g_root, &propid_EventDefinition, cAVPG_EVENT_DEFINITION_PROP, pTYPE_DWORD | pCUSTOM_HERITABLE);
		CALL_Root_RegisterCustomPropId(g_root, &propid_UserInfo, cAVPG_USER_DEFINITION_PROP, pTYPE_PTR);
		CALL_Root_RegisterCustomPropId(g_root, &propid_CrUserInfo, cAVPG_CRUSER_DEFINITION_PROP, pTYPE_PTR);
		
		CALL_Root_RegisterCustomPropId(g_root, &propid_ProcessDefinition, cAVPG_PROCESS_DEFINITION_PROP, pTYPE_STRING);
		CALL_Root_RegisterCustomPropId(g_root, &propid_ThreadDefinition, cAVPG_THREAD_DEFINITION_PROP, pTYPE_DWORD | pCUSTOM_HERITABLE);
		CALL_Root_RegisterCustomPropId(g_root, &propid_ObjectModified, cAVPG_PROP_OBJECT_MODIFIED, pTYPE_DWORD);
		CALL_Root_RegisterCustomPropId(g_root, &propid_EventProcessID, cAVPG_PROP_EVENT_PROCESSID, pTYPE_DWORD);

		CALL_Root_RegisterCustomPropId(g_root, &propid_EventStartProcessing, cAVPG_STAT_TIME_EVENT_START_PROCESSING, pTYPE_QWORD);
		CALL_Root_RegisterCustomPropId(g_root, &propid_BeforeCreateIO, cAVPG_STAT_TIME_BEFORE_CREATE_IO, pTYPE_QWORD);
		CALL_Root_RegisterCustomPropId(g_root, &propid_BeforeCheck1, cAVPG_STAT_TIME_BEFORE_CHECK1, pTYPE_QWORD);
		CALL_Root_RegisterCustomPropId(g_root, &propid_BeforeCheck2, cAVPG_STAT_TIME_BEFORE_CHECK2, pTYPE_QWORD);
		CALL_Root_RegisterCustomPropId(g_root, &propid_BeforeCheck3, cAVPG_STAT_TIME_BEFORE_CHECK3, pTYPE_QWORD);
		CALL_Root_RegisterCustomPropId(g_root, &propid_ICheckerReadTime, cAVPG_STAT_TIME_ICHECKER_READ, pTYPE_QWORD);
		
		CALL_Root_RegisterCustomPropId(g_root, &propid_EventVerdict, cAVPG_STAT_TIME_EVENT_VERDICT_TIME, pTYPE_QWORD);
		CALL_Root_RegisterCustomPropId(g_root, &propid_EventEndProcessing, cAVPG_STAT_TIME_EVENT_END_PROCESSING, pTYPE_QWORD);

		CALL_Root_RegisterCustomPropId(g_root, &propid_ResultCreateIO, cAVPG_STAT_RESULT_CREATE_IO, pTYPE_DWORD);
		CALL_Root_RegisterCustomPropId(g_root, &propid_ResultProcessing1, cAVPG_STAT_RESULT_PROCESSING1, pTYPE_DWORD);
		CALL_Root_RegisterCustomPropId(g_root, &propid_ResultProcessingVerdict, cAVPG_STAT_RESULT_PROCESSING_VERDICT, pTYPE_DWORD);

		CALL_Root_RegisterCustomPropId(g_root, &propid_CustomOrigin, cAVPG_PROP_CUSTOM_ORIGIN, pTYPE_ORIG_ID);

		CALL_Root_RegisterCustomPropId(g_root, &propid_DosDeviceName, cAVPG_PROP_DOS_DEVICE_NAME, pTYPE_STRING);

		CALL_Root_RegisterCustomPropId(g_root, &propid_Streamed, cAVPG_PROP_STEAMED_FIXED_MEDIA, pTYPE_BOOL);

		CALL_Root_RegisterCustomPropId(g_root, &propid_Delayed, cAVPG_PROP_DELAYED_PROCESSING, pTYPE_BOOL);
	
		// CALL_Root_ResolveImportTable( g_root, NULL, import_table, AVPG_ID);

		// register my interfaces
		if ( PR_FAIL(*pERROR=AVPG_Register(g_root)) ) {
			PR_TRACE(( g_root, prtERROR, "cannot register \"AVPG\" interface"));
			return cFALSE;
		}
		break;

	case PRAGUE_PLUGIN_UNLOAD :
		g_root = NULL;
		// free system resources
		// unregister my custom property ids -- you can drop it, kernel do it by itself
		// release    my imports		         -- you can drop it, kernel do it by itself
		// unregister my exports		         -- you can drop it, kernel do it by itself
		// unregister my interfaces          -- you can drop it, kernel do it by itself
		break;
	}
	return cTRUE;
}
// AVP Prague stamp end

//+ ------------------------------------------------------------------------------------------
static BOOL bUshHook_Inited = FALSE;
static BOOL bUshFsdr_Inited = FALSE;

BOOLEAN Ushata(BOOLEAN bMakeHooks)
{
	if (bMakeHooks)
	{
		if (bUshHook_Inited)
		{
			PR_TRACE((0, prtIMPORTANT, "AVPG: ExportHooks already set"));
		}
		else
		{
			bUshHook_Inited = TRUE;
			if (!InitExportHooks(s_hThisModule, bMakeHooks == 2))
			{
				PR_TRACE((0, prtDANGER, "AVPG: can't set hook"));
				bUshHook_Inited = FALSE;
				return FALSE;
			}
		}

	}

/*
#ifdef _DEBUG
	// sing check disabled
#else
	{
		#define _MOD_FILE_NAME_SIZE	MAX_PATH * 4
		
		DWORD SignCheck = 1;

		DWORD ModuleNameSize;
		PTCHAR CurrentModule = (PTCHAR) HeapAlloc(GetProcessHeap(), 0, _MOD_FILE_NAME_SIZE);
		if (CurrentModule == NULL)
		{
			PR_TRACE((0, prtDANGER, "AVPG: low resources"));
		}
		else
		{
			ModuleNameSize = GetModuleFileName(NULL, CurrentModule, _MOD_FILE_NAME_SIZE);
			if (ModuleNameSize)
				SignCheck = sign_check_file(CurrentModule, 1, NULL, 0, 0);
			else
			{
				PR_TRACE((0, prtDANGER, "AVPG: bad main module name"));
			}

			HeapFree(GetProcessHeap(), 0, CurrentModule);

			if (SIGN_OK != SignCheck)
			{
				PR_TRACE((0, prtDANGER, "AVPG: bad sign"));
				
				return FALSE;
			}
		}
#endif
*/
	if (bUshFsdr_Inited)
	{
		PR_TRACE((0, prtIMPORTANT, "AVPG: FSDrvLib already inited"));
	}
	else
	{
		bUshFsdr_Inited = TRUE;
		if (!FSDrvInterceptorInit())
		{
			PR_TRACE((0, prtDANGER, "AVPG: can't init fsdrvlib"));
			bUshFsdr_Inited = FALSE;
			return FALSE;
		}
		
		if (!FSDrvInterceptorRegisterInvisibleThread())
		{
			PR_TRACE((0, prtDANGER, "AVPG: can't register main thread as invisible!!!"));
		}

		PR_TRACE((0, prtDANGER, "AVPG: ushata ok"));
	}
	
	return TRUE;
}

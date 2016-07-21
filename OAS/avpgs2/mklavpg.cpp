#include "mklavpg.h"
#include "wavpg.h"
#include "../../windows/hook/mklif/inc/commdata.h"

#include <prague/iface/i_io.h>
#include <prague/plugin/p_win32_nfio.h>
#include <prague/plugin/p_mkavio.h>
#include <extract/plugin/p_windiskio.h>

bool
IsNeedPostCreate (
	PVOID pMessage
	)
{
	PSINGLE_PARAM pSingleContextFlags = DRV_GetEventParam( pMessage, 0, _PARAM_OBJECT_CONTEXT_FLAGS, sizeof(ULONG) );
	if (!pSingleContextFlags)
		return false;

	ULONG ContextFlags = *(ULONG*) pSingleContextFlags->ParamValue;

	if (_CONTEXT_OBJECT_FLAG_NETWORKTREE & ContextFlags)
		return false;
	
	if (_CONTEXT_OBJECT_FLAG_DIRECTORY & ContextFlags)
		return false;

	if (!(_CONTEXT_OBJECT_FLAG_FORREAD & ContextFlags))
		return false;

	return true;
}

bool
IsReadAllowed (
	PVOID pMessage
	)
{
	PSINGLE_PARAM pSingleContextFlags = DRV_GetEventParam( pMessage, 0, _PARAM_OBJECT_CONTEXT_FLAGS, sizeof(ULONG) );
	if (!pSingleContextFlags)
		return false;

	ULONG ContextFlags = *(ULONG*) pSingleContextFlags->ParamValue;

	if (_CONTEXT_OBJECT_FLAG_FORREAD & ContextFlags)
		return true;

	return false;
}

void
Proceed (
	cMklAvpg* This,
	PVOID pDrvContext,
	PVOID pMessage
	)
{
	HRESULT hResult = S_OK;

	bool bAllow = true;
	bool bExecute = false;
	bool bSetEventDescr = true;

	PMKLIF_EVENT_HDR pHdr = (PMKLIF_EVENT_HDR) Drv_GetEventHdr( pMessage );

	cString* hCtx = NULL;

	if (_EVENT_FLAG_POSTPROCESSING & pHdr->m_EventFlags)
	{
		// PostCreate
		if (FLTTYPE_NFIOR == pHdr->m_HookID && IRP_MJ_CREATE == pHdr->m_FunctionMj)
		{
			if (IsNeedPostCreate( pMessage ))
				hCtx = GenerateContext( This->m_pContext, This->m_pPragueContext, pDrvContext, pMessage, pHdr->m_ProcessId );
		}
		else if (FLTTYPE_NFIOR == pHdr->m_HookID
			&& IRP_MJ_FILE_SYSTEM_CONTROL == pHdr->m_FunctionMj
			&& (IRP_MN_MOUNT_VOLUME == pHdr->m_FunctionMi || IRP_MN_VERIFY_VOLUME == pHdr->m_FunctionMi))
		{
			SendVolumeObject( This->m_pContext, &pMessage );
		}
		else
		{
			PR_TRACE(( g_root, prtERROR, TR "UNKNOWN post event!!! 0x%x:%x", pHdr->m_HookID, pHdr->m_FunctionMj ));
		}
	}
	else // pre processing
	{
		if (FLTTYPE_SYSTEM == pHdr->m_HookID && MJ_SYSTEM_CREATE_SECTION == pHdr->m_FunctionMj)
		{
			bExecute = true;
			PR_TRACE(( g_root, prtSPAM, TR "Execute" ));
			hCtx = GenerateContext( This->m_pContext, This->m_pPragueContext, pDrvContext, pMessage, pHdr->m_ProcessId );
		}
		else if (FLTTYPE_NFIOR == pHdr->m_HookID && IRP_MJ_SET_INFORMATION == pHdr->m_FunctionMj)
		{
			bSetEventDescr = false;
			PR_TRACE(( g_root, prtERROR, TR "PreRename" ));
			hCtx = GenerateContext( This->m_pContext, This->m_pPragueContext, pDrvContext, pMessage, pHdr->m_ProcessId );
			if (!hCtx)
			{
				PR_TRACE(( g_root, prtERROR, TR "PreRename create ctx failed" ));
			}
		}
		else if (FLTTYPE_NFIOR == pHdr->m_HookID && IRP_MJ_CLEANUP == pHdr->m_FunctionMj)
		{
			PR_TRACE(( g_root, prtSPAM, TR "PreCleanup" ));
			if (IsModifiedObject( pMessage ))
			{
				hCtx = GenerateContext( This->m_pContext, This->m_pPragueContext, pDrvContext, pMessage, pHdr->m_ProcessId );
				if (hCtx)
				{
					if (!IsReadAllowed( pMessage ))
					{
						AddToDelayed( This->m_pContext, hCtx, pMessage );
						AnswerMessage( This->m_pPragueContext, pDrvContext, &pMessage, TRUE, FALSE, 0 );

						hCtx->sysCloseObject();
						hCtx = NULL;
					}
				}
			}
			else
			{
				PR_TRACE(( g_root, prtSPAM, TR "skip readonly close" ));
			}
		}
		else if (FLTTYPE_NFIOR == pHdr->m_HookID && IRP_MJ_CREATE == pHdr->m_FunctionMj)
		{
			PSINGLE_PARAM pParamLuid = DRV_GetEventParam (
				pMessage,
				0,
				_PARAM_OBJECT_LUID,
				sizeof(LUID)
				);
			
			if (pParamLuid)
			{
				PLUID pLuid = (PLUID) pParamLuid->ParamValue;
				if (This->m_pContext->m_Users.IsBanned( pLuid ))
					bAllow = false;
			}
		}
		else
		{
			PR_TRACE(( g_root, prtERROR, TR "UNKNOWN pre event!!! 0x%x:%x", pHdr->m_HookID, pHdr->m_FunctionMj ));
		}
	}

	//!
	if (hCtx)
	{
		if (bSetEventDescr)
		{
			MKLIF_EVENT_DESCRIBE EventDescribe;
			memset( &EventDescribe, 0, sizeof(EventDescribe) );

			EventDescribe.m_pClientContext = This->m_pContext->m_pDrvContext;
			EventDescribe.m_pMessage = pMessage;
			EventDescribe.m_MessageSize = 0;

			tERROR error = hCtx->propSetPtr( propid_EventDefinition, &EventDescribe );
		}

		SendObject( This->m_pContext, This->m_WorkingMode, hCtx, &pMessage, pHdr->m_ProcessId, bExecute );

		hCtx->sysCloseObject();
		hCtx = NULL;
	}

	if (pMessage)
		AnswerMessage( This->m_pPragueContext, pDrvContext, &pMessage, bAllow, FALSE, 0 );
}

void __cdecl
MklProceedEvent (
	cContext* pContext,
	PVOID pMessage
	)
{
	Proceed( (cMklAvpg*) pContext->m_AvpgImp, pContext->m_pDrvContext, pMessage );
}

cMklAvpg::cMklAvpg (
	cContext* pContext,
	tAVPG2_WORKING_MODE WorkingMode
	) : cAvpgImpl( pContext->m_pPragueContext, WorkingMode, pContext->m_ThreadMaxCount )
{
	PR_TRACE(( g_root, prtWARNING, TR "init mklif agent..." ));

	m_pContext = pContext;
	pContext->m_AvpgImp = this;
	pContext->m_pfProceed = MklProceedEvent;

	AddFilters();

	PR_TRACE(( g_root, prtWARNING, TR "init mklif ok" ));
}

cMklAvpg::~cMklAvpg (
	)
{
	PR_TRACE(( g_root, prtWARNING, TR "~cMklAvpg" ));
}

bool
cMklAvpg::AddFilters()
{
	ULONG FilterId;

	PR_TRACE((g_root, prtWARNING, TR "adding filters. working mode %d", m_WorkingMode ));

	FILTER_PARAM ParamFN;
	FillParamNOP( &ParamFN, _PARAM_OBJECT_URL_W, _FILTER_PARAM_FLAG_CACHABLE );

	//+ ban access filter
	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_NFIOR,
		IRP_MJ_CREATE, 0, 0, PreProcessing,
		NULL, NULL, NULL, NULL, NULL );
	//- ban access filter

	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT,
		FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_SYSTEM,
		MJ_SYSTEM_CREATE_SECTION, 0, 0, PreProcessing,
		&ParamFN, NULL, NULL, NULL, NULL );

	if (cAVPG2_WORKING_MODE_ON_EXECUTE != m_WorkingMode)
	{
		if (cAVPG2_WORKING_MODE_ON_MODIFY != m_WorkingMode)
		{
			DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT,
				FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_NFIOR, 
				IRP_MJ_CREATE, 0, 0, PostProcessing, 
				&ParamFN, NULL, NULL, NULL, NULL );

			DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT,
				FLT_A_POPUP | FLT_A_USECACHE | FLT_A_SAVE2CACHE, FLTTYPE_NFIOR,
				IRP_MJ_SET_INFORMATION, FileRenameInformation, 0, PreProcessing, 
				&ParamFN, NULL, NULL, NULL, NULL );
		}

		if (cAVPG2_WORKING_MODE_ON_OPEN == m_WorkingMode)
		{
			PR_TRACE((g_root, prtWARNING, TR "close op skipped by working mode" ));
		}
		else
		{
			DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT,
				FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, FLTTYPE_NFIOR,
				IRP_MJ_CLEANUP, 0, 0, PreProcessing, 
				&ParamFN, NULL, NULL, NULL, NULL );
		}
	}

	//+ reset cache item
	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_NFIOR,
		IRP_MJ_WRITE, IRP_MN_NORMAL, 0, PostProcessing, 
		&ParamFN, NULL, NULL, NULL, NULL );

	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_NFIOR,
		IRP_MJ_WRITE, IRP_MN_MDL, 0, PostProcessing, 
		&ParamFN, NULL, NULL, NULL, NULL );

	//- reset cache item

	ParamFN.m_ParamID = _PARAM_OBJECT_URL_DEST_W;

	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_NFIOR,
		IRP_MJ_SET_INFORMATION, FileRenameInformation, 0, PostProcessing, 
		&ParamFN, NULL, NULL, NULL, NULL );

	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_INFO | FLT_A_RESETCACHE, FLTTYPE_NFIOR,
		IRP_MJ_FILE_SYSTEM_CONTROL, IRP_MN_MOUNT_VOLUME, 0, PostProcessing, 
		NULL, NULL, NULL, NULL, NULL );

	PR_TRACE((g_root, prtWARNING, TR "add filters complete" ));

	return true;
}

tERROR
cMklAvpg::CreateIO (
	hOBJECT* p_phObject,
	hSTRING p_hObjectName
	)
{
	tERROR error = errNOT_OK;
	
	PR_TRACE((g_root, prtSPAM, TR "create io request" ));

	if (p_hObjectName == NULL)
		return errPARAMETER_INVALID;

	hIO hIo = NULL;
		
	tORIG_ID Origin = OID_GENERIC_IO;
	PVOID pMessage = NULL;
		
	tDWORD dwPropSize = 0;
		
	error = p_hObjectName->propGet( &dwPropSize, propid_CustomOrigin, &Origin, sizeof(Origin) );
	if (PR_FAIL( error ))
		Origin = OID_GENERIC_IO;

	error = p_hObjectName->propGet( &dwPropSize, propid_EventDefinition, &pMessage, sizeof(pMessage) );
	if (PR_FAIL( error ) || sizeof(pMessage) != dwPropSize)
		pMessage = NULL;

	switch (Origin)
	{
	case OID_AVP3_BOOT:
		PR_TRACE(( g_root, prtIMPORTANT, TR "create disk io request" ));
		error = CALL_SYS_ObjectCreate(p_hObjectName, &hIo, IID_IO, PID_WINDISKIO, 0);
		if (PR_SUCC(error))
		{
			error = p_hObjectName->ExportToProp( &dwPropSize, cSTRING_WHOLE, (hOBJECT)hIo, pgOBJECT_NAME );
			if (PR_SUCC(error))
				error = CALL_SYS_ObjectCreateDone( hIo );
		}

		PR_TRACE(( g_root, prtIMPORTANT, TR "create disk io result %terr", error ));
		break;

	case OID_GENERIC_IO:
		if (pMessage)
		{
			error = p_hObjectName->sysCreateObjectQuick( (hOBJECT*) &hIo, IID_IO, PID_MKAVIO, SUBTYPE_ANY );
			if (PR_SUCC( error ))
				DRV_Yeild( m_pContext->m_pDrvContext, pMessage, 5 );

			PR_TRACE((g_root, prtIMPORTANT, TR "create mkavio result %terr", error));

			break;
		}
		else
		{
			error = p_hObjectName->sysCreateObject( (hOBJECT*) &hIo, IID_IO, PID_WIN32_NFIO, 0 );
			if (PR_SUCC(error))
			{
				hIo->propSetDWord( pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST | fOMODE_OPEN_IF_EXECUTABLE );
				hIo->propSetDWord( pgOBJECT_ACCESS_MODE, fACCESS_READ | fACCESS_FORCE_READ );
				hIo->propSetDWord( plSAVE_FILE_TIME, cTRUE );

				error = CALL_String_ExportToProp( p_hObjectName, NULL, cSTRING_WHOLE, (hOBJECT) hIo, pgOBJECT_NAME );

				error = CALL_SYS_ObjectCreateDone(hIo);

				PR_TRACE(( g_root, prtIMPORTANT, TR "create nfio result %terr", error ));
			}
		}
		break;
	}
	
	if (PR_FAIL( error ) && hIo)
	{
		CALL_SYS_ObjectClose(hIo);
		hIo = NULL;
	}
	
	*p_phObject = (hOBJECT) hIo;

	return error;
}
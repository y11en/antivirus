#include "klavpg.h"
#include "wavpg.h"

#include <prague/pr_oid.h>
#include <prague/iface/i_io.h>
#include <prague/plugin/p_win32_nfio.h>
#include <prague/plugin/p_mkavio.h>

bool
IsNeedPreCreate (
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

	if (_CONTEXT_OBJECT_FLAG_FORREAD & ContextFlags)
		return true;

	return false;
}

void __cdecl
KlProceedEvent (
	cContext* pContext,
	PVOID pMessage
	)
{
	cKlAvpg* This = (cKlAvpg*) pContext->m_AvpgImp;
	PVOID pDrvContext = pContext->m_pDrvContext;

	cString* hCtx = NULL;

	bool bExecute = false;
	bool bAllow = true;

	PEVENT_TRANSMIT pEvt = (PEVENT_TRANSMIT) Drv_GetEventHdr( pMessage );

	PR_TRACE(( g_root, prtNOT_IMPORTANT, TR "income mark 0x%x, procees 0x%x", DRV_GetEventMark( pMessage ), pEvt->m_ProcessID ));

	switch(pEvt->m_HookID)
	{
	case FLTTYPE_SYSTEM:
		{
			PSINGLE_PARAM pParamAttr = DRV_GetEventParam( pMessage, 0, _PARAM_OBJECT_ACCESSATTR, sizeof(ULONG) );
			PSINGLE_PARAM pParamProt = DRV_GetEventParam( pMessage, 0, _PARAM_OBJECT_PAGEPROTECTION, sizeof(ULONG) );
			if (pParamAttr && pParamProt)
			{
				ULONG SectionFlags = *(ULONG*) pParamAttr->ParamValue;
				ULONG ProtFlags = *(ULONG*) pParamProt->ParamValue;
				
				PR_TRACE(( g_root, prtNOTIFY, TR "pid <%d> execute flags 0x%x, protection 0x%x",
					pEvt->m_ProcessID,
					SectionFlags,
					ProtFlags
					));
			}
		}
		hCtx = GenerateContext( This->m_pContext, This->m_pPragueContext, pDrvContext, pMessage, pEvt->m_ProcessID );
		bExecute = true;
		break;
	
	case FLTTYPE_NFIOR:
		if (IRP_MJ_CREATE == pEvt->m_FunctionMj)
		{
			PLUID pLuid = NULL;
			PSINGLE_PARAM pParamLuid = DRV_GetEventParam (
				pMessage,
				0,
				_PARAM_OBJECT_LUID,
				sizeof(LUID)
				);

			if (pParamLuid)
				pLuid = (PLUID) pParamLuid->ParamValue;

			if (This->m_pContext->m_Users.IsBanned ( pLuid ))
				bAllow = false;
			else
			{
				if (IsNeedPreCreate( pMessage ))
					hCtx = GenerateContext( This->m_pContext, This->m_pPragueContext, pDrvContext, pMessage, pEvt->m_ProcessID );
			}
		}
		else if (IRP_MJ_CLEANUP == pEvt->m_FunctionMj)
		{
			hCtx = GenerateContext( This->m_pContext, This->m_pPragueContext, pDrvContext, pMessage, pEvt->m_ProcessID );
			if (hCtx)
			{
				AddToDelayed( This->m_pContext, hCtx, pMessage );
				AnswerMessage( This->m_pPragueContext, pDrvContext, &pMessage, TRUE, FALSE, 0 );

				hCtx->sysCloseObject();
				hCtx = NULL;
			}
		}
		else if (IRP_MJ_SET_INFORMATION == pEvt->m_FunctionMj && FileRenameInformation == pEvt->m_FunctionMi)
		{
			hCtx = GenerateContext( This->m_pContext, This->m_pPragueContext, pDrvContext, pMessage, pEvt->m_ProcessID );
			if (hCtx)
				This->m_pContext->m_EvQueue.SkipItem( hCtx );
		}
		else if (IRP_MJ_FILE_SYSTEM_CONTROL == pEvt->m_FunctionMj
			&& (IRP_MN_MOUNT_VOLUME == pEvt->m_FunctionMi || IRP_MN_VERIFY_VOLUME == pEvt->m_FunctionMi))
		{
			if (IRP_MN_MOUNT_VOLUME == pEvt->m_FunctionMi)
				PR_TRACE(( g_root, prtWARNING, TR "mount volume" ));
			else
				PR_TRACE(( g_root, prtWARNING, TR "verify volume" ));

			SendVolumeObject( This->m_pContext, &pMessage );
		}
		else
		{
			PR_TRACE(( g_root, prtERROR, TR "not processed FLTTYPE_NFIOR 0x%x:%x", pEvt->m_FunctionMj, pEvt->m_FunctionMi ));
		}
		break;
	
	default:
		PR_TRACE(( g_root, prtERROR, TR "not processed 0x%x:%x:%x", pEvt->m_HookID, pEvt->m_FunctionMj, pEvt->m_FunctionMi ));
		break;
	}

	if (hCtx)
	{
		tERROR error = hCtx->propSetPtr( propid_EventDefinition, pMessage );

		SendObject( This->m_pContext, This->m_WorkingMode, hCtx, &pMessage, pEvt->m_ProcessID, bExecute);

		hCtx->sysCloseObject();
		hCtx = NULL;
	}
	
	if (pMessage)
		AnswerMessage( This->m_pPragueContext, pDrvContext, &pMessage, bAllow, FALSE, 0 );
}

cKlAvpg::cKlAvpg (
	cContext* pContext,
	tAVPG2_WORKING_MODE WorkingMode
	) : cAvpgImpl( pContext->m_pPragueContext, WorkingMode, pContext->m_ThreadMaxCount )
{
	PR_TRACE(( g_root, prtERROR, TR "init klif agent..." ));

	m_pContext = pContext;
	pContext->m_AvpgImp = this;
	pContext->m_pfProceed = KlProceedEvent;

	AddFilters();

	/*if (!bValid)
	{
		PR_TRACE(( g_root, prtERROR, TR "throw: kl init failure!" ));
		throw "kl init failure!";
	}*/

	PR_TRACE(( g_root, prtERROR, TR "init klif ok" ));
}

cKlAvpg::~cKlAvpg (
	)
{
	PR_TRACE(( g_root, prtERROR, TR "~cKlAvpg" ));
}

void
cKlAvpg::AddSkipFName (
	PWCHAR pwch
	)
{
	ULONG FilterId;
	ULONG Length = (lstrlenW( pwch ) + 1) * sizeof(WCHAR);

	PFILTER_PARAM pParam = (PFILTER_PARAM) MemAlloc( m_pPragueContext, sizeof(FILTER_PARAM) + Length, 0 );
	if (!pParam)
		return;

	pParam->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
	pParam->m_ParamFlt = FLT_PARAM_WILDCARD;
	pParam->m_ParamID = _PARAM_OBJECT_URL_W;
	pParam->m_ParamSize = Length;
	memcpy( pParam->m_ParamValue, pwch, Length );

	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, 0, FLT_A_DEFAULT,
		FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing,
		pParam, NULL, NULL, NULL, NULL );

	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, 0, FLT_A_DEFAULT,
		FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileRenameInformation, 0, PreProcessing,
		pParam, NULL, NULL, NULL, NULL );

	MemFree( m_pPragueContext, (void**) &pParam );
}

bool
cKlAvpg::AddFilters()
{
	PR_TRACE((g_root, prtWARNING, TR "adding filters. working mode %d", m_WorkingMode ));

	ULONG FilterId;

	AddSkipFName( L"*$INDEX_ALLOCATION" );
	AddSkipFName( L"*:\\System Volume Information\\tracking.log" );
	AddSkipFName( L"*:\\$Extend\\$ObjId" );

	FILTER_PARAM ParamFN, ParamFNDest;
	FillParamNOP( &ParamFN, _PARAM_OBJECT_URL_W, _FILTER_PARAM_FLAG_CACHABLE | _FILTER_PARAM_FLAG_CACHEUPREG );
	FillParamNOP( &ParamFNDest, _PARAM_OBJECT_URL_DEST_W, _FILTER_PARAM_FLAG_CACHABLE | _FILTER_PARAM_FLAG_CACHEUPREG );

	{
		BYTE Buf[sizeof(FILTER_PARAM) + sizeof(ULONG)];
		PFILTER_PARAM Param = (PFILTER_PARAM) Buf;
		
		FillParamNOP( Param, _PARAM_OBJECT_ACCESSATTR, _FILTER_PARAM_FLAG_NONE );
		Param->m_ParamFlt = FLT_PARAM_AND;
		Param->m_ParamSize = sizeof(ULONG);
		*(ULONG*)Param->m_ParamValue = SECTION_MAP_EXECUTE;

		BYTE Buf2[sizeof(FILTER_PARAM) + sizeof(ULONG)];
		PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;
		
		FillParamNOP( Param2, _PARAM_OBJECT_PAGEPROTECTION, _FILTER_PARAM_FLAG_NONE );
		Param2->m_ParamFlt = FLT_PARAM_AND;
		Param2->m_ParamSize = sizeof(ULONG);
		*(ULONG*)Param2->m_ParamValue = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

		DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, 0, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE,
			FLTTYPE_SYSTEM, MJ_SYSTEM_CREATE_SECTION, 0, 0, PreProcessing,
			Param, Param2, &ParamFN, NULL, NULL );
	}

	// -----------------------------------------------------------------------------------------
	if (cAVPG2_WORKING_MODE_ON_EXECUTE != m_WorkingMode)
	{
		{
			// opening file with attributes
			ULONG AccessFlg;
			BYTE Buf[sizeof(FILTER_PARAM) + sizeof(ULONG)];
			PFILTER_PARAM Param = (PFILTER_PARAM) Buf;
			
			Param->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
			Param->m_ParamFlt = FLT_PARAM_AND;
			Param->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
			AccessFlg = FILE_READ_EA | FILE_READ_ATTRIBUTES | DELETE;
			AccessFlg = ~AccessFlg;
			*(ULONG*)Param->m_ParamValue = AccessFlg;
			Param->m_ParamSize = sizeof(ULONG);
			
			DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, 
				FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing,
				&ParamFN, Param, NULL, NULL, NULL );
		}
		
		DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_DROPCACHE, 
			FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileRenameInformation, 0, PreProcessing,
			&ParamFNDest, NULL, NULL, NULL, NULL );

		//close file
		if (cAVPG2_WORKING_MODE_ON_OPEN != m_WorkingMode)
		{
			DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, 
				FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0, 0, PostProcessing,
				&ParamFN, NULL, NULL, NULL, NULL );
		}
	}

	//+ reset cache item
	//* mount
	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_RESETCACHE, FLTTYPE_NFIOR,
		IRP_MJ_FILE_SYSTEM_CONTROL, IRP_MN_MOUNT_VOLUME, 0, PostProcessing, 
		NULL, NULL, NULL, NULL, NULL );

	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP | FLT_A_RESETCACHE, FLTTYPE_NFIOR,
		IRP_MJ_FILE_SYSTEM_CONTROL, IRP_MN_VERIFY_VOLUME, 0, PostProcessing, 
		NULL, NULL, NULL, NULL, NULL );

	//+ write
	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_NFIOR,
		IRP_MJ_WRITE, IRP_MN_NORMAL, 0, PreProcessing,
		&ParamFN, NULL, NULL, NULL, NULL );

	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_NFIOR,
		IRP_MJ_WRITE, IRP_MN_MDL, 0, PreProcessing,
		&ParamFN, NULL, NULL, NULL, NULL );

	DRV_AddFilter( &FilterId, m_pContext->m_pDrvContext, DEADLOCKWDOG_TIMEOUT, FLT_A_DROPCACHE, FLTTYPE_FIOR,
		FastIoWrite, 0, 0, PreProcessing, 
		&ParamFN, NULL, NULL, NULL, NULL );
	//- reset cache item

	return true;
}

tERROR
cKlAvpg::CreateIO (
	hOBJECT* p_phObject,
	hSTRING p_hObjectName
	)
{
	tERROR error = errNOT_OK;
	
	PR_TRACE((g_root, prtSPAM, TR "create io request" ));

	if (!p_hObjectName)
		return errPARAMETER_INVALID;

	hIO hIo = NULL;
		
	tORIG_ID Origin = OID_GENERIC_IO;

	tDWORD dwPropSize;
	error = p_hObjectName->propGet( &dwPropSize, propid_CustomOrigin, &Origin, sizeof(Origin) );
	if (PR_FAIL( error ))
		Origin = OID_GENERIC_IO;

	switch (Origin)
	{
	case OID_AVP3_BOOT:
		//! create disk io
		/*error = CALL_SYS_ObjectCreate(p_hObjectName, &hIo, IID_IO, PID_WINDISKIO, 0);
		if (PR_SUCC(error))
		{
			len = 0;
			error = CALL_String_ExportToProp(p_hObjectName, &len, cSTRING_WHOLE, (hOBJECT)hIo, pgOBJECT_NAME);
			if (PR_SUCC(error))
				error = CALL_SYS_ObjectCreateDone(hIo);
		}*/
		break;
	case OID_GENERIC_IO:

		error = p_hObjectName->sysCreateObject( (hOBJECT*) &hIo, IID_IO, PID_WIN32_NFIO, 0 );
		if (PR_SUCC(error))
		{
				hIo->propSetDWord( pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST | fOMODE_OPEN_IF_EXECUTABLE );
				hIo->propSetDWord( pgOBJECT_ACCESS_MODE, fACCESS_READ | fACCESS_FORCE_READ );
				hIo->propSetDWord( plSAVE_FILE_TIME, cTRUE );

				error = CALL_String_ExportToProp( p_hObjectName, NULL, cSTRING_WHOLE, (hOBJECT) hIo, pgOBJECT_NAME );

				error = CALL_SYS_ObjectCreateDone(hIo);
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
#include "pch.h"
#include "evqueue.h"
#include "client.h"
#include "..\hook\HookSpec.h"
#include "klsecur.h"

#include "evqueue.tmh"

ULONG
CalcFEvParamSize (
	PFILTER_EVENT_PARAM pEventParam
	)
{
	DWORD FEParamSize;
	PSINGLE_PARAM pParam;
	BYTE *pPointer;
	DWORD cou;
	DWORD tmp;
	
	FEParamSize = 0;
	if (pEventParam->ParamsCount != 0)
	{
		pParam = (PSINGLE_PARAM) pEventParam->Params;
		for (cou = 0; cou < pEventParam->ParamsCount; cou++)
		{
			if (pParam->ParamFlags & _SINGLE_PARAM_FLAG_POINTER)
				tmp = sizeof(DWORD);
			else
				tmp = sizeof(SINGLE_PARAM) + pParam->ParamSize;
			FEParamSize += tmp;
			pPointer = (BYTE*)pParam + tmp;
			pParam = (PSINGLE_PARAM) pPointer;
		}
	}
	
	return FEParamSize;
}

PMKLIF_EVENT_TRANSMIT
QueueAllocEventTr (
	PFILTER_EVENT_PARAM pEventParam,
	PFILTER_SUB pFilterSub,
	ULONG EventFlags
	)
{
	PMKLIF_EVENT_TRANSMIT pEventTr = NULL;

	ULONG EvSize = 0;
	DWORD ParamSize;
	DWORD MappedParamsSize = 0;

	ParamSize = CalcFEvParamSize( pEventParam );

	if (pFilterSub->m_pMappedFilterParams)
		MappedParamsSize = pFilterSub->m_pMappedFilterParams->m_Param_Size;

	EvSize = sizeof(MKLIF_EVENT_TRANSMIT) + ParamSize + MappedParamsSize;

	pEventTr = ExAllocatePoolWithTag(PagedPool, EvSize, 'tboS');
	if (!pEventTr)
		return NULL;

	pEventTr->m_EventHdr.m_EventSize = EvSize;

	pEventTr->m_EventHdr.m_HookID = pEventParam->HookID;
	pEventTr->m_EventHdr.m_FunctionMj = pEventParam->FunctionMj;
	pEventTr->m_EventHdr.m_FunctionMi = pEventParam->FunctionMi;
	
	pEventTr->m_EventHdr.m_ProcessId = pEventParam->ProcessID;
	pEventTr->m_EventHdr.m_ThreadId = pEventParam->ThreadID;

	pEventTr->m_EventHdr.m_FilterID = pFilterSub->m_FilterID;

	pEventTr->m_EventHdr.m_Timeout = pFilterSub->m_Timeout;
	pEventTr->m_EventHdr.m_EventFlags = EventFlags;

	if (pFilterSub->m_Flags & FLT_A_ALERT1)
		pEventTr->m_EventHdr.m_EventFlags |= _EVENT_FLAG_ALERT1;
	if (pFilterSub->m_Flags & FLT_A_USERLOG)
		pEventTr->m_EventHdr.m_EventFlags |= _EVENT_FLAG_LOGTHIS;
	if (pFilterSub->m_Flags & FLT_A_TOUCH)
		pEventTr->m_EventHdr.m_EventFlags |= _EVENT_FLAG_TOUCH;
	if (pFilterSub->m_Flags & FLT_A_SAVE2CACHE)
		pEventTr->m_EventHdr.m_EventFlags |= _EVENT_FLAG_SAVECACHE_AVAILABLE;

#ifdef _WIN64
	if (!FltIs32bitProcess( NULL ))
		pEventTr->m_EventHdr.m_EventFlags |= _EVENT_FLAG_64BIT;
#endif
	// -----------------------------------------------------------------------------------------
	if (pFilterSub->m_Flags == FLT_A_DEFAULT)
		pEventTr->m_EventHdr.m_EventFlags |= _EVENT_FLAG_AUTO_VERDICT_DEFAULT;
	else if (pFilterSub->m_Flags & FLT_A_PASS)
		pEventTr->m_EventHdr.m_EventFlags |= _EVENT_FLAG_AUTO_VERDICT_PASS;
	else if (pFilterSub->m_Flags & FLT_A_DENY)
		pEventTr->m_EventHdr.m_EventFlags |= _EVENT_FLAG_AUTO_VERDICT_DISCARD;

	if (FlagOn( pFilterSub->m_Flags, FLT_A_POPUP))
		SetFlag( pEventTr->m_EventHdr.m_EventFlags, _EVENT_FLAG_POPUP );
	else
	{
		if (pFilterSub->m_Flags & FLT_A_DELETE_ON_MARK)
			pEventTr->m_EventHdr.m_EventFlags |= _EVENT_FLAG_DELETED_ON_MARK;
	}

	pEventTr->m_EventHdr.m_ParamsCount = pEventParam->ParamsCount;
	if (pEventParam->ParamsCount) 
		memcpy(pEventTr->m_SingleParams, pEventParam->Params, ParamSize);

	if (pFilterSub->m_pMappedFilterParams)
	{
		memcpy((BYTE*)(pEventTr->m_SingleParams) + ParamSize, pFilterSub->m_pMappedFilterParams->m_SingleParams, 
			pFilterSub->m_pMappedFilterParams->m_Param_Size);
		pEventTr->m_EventHdr.m_ParamsCount += pFilterSub->m_pMappedFilterParams->m_ParamsCount;
	}

	return pEventTr;
}

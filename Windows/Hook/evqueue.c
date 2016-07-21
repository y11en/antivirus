#include "evqueue.h"
#include "client.h"
#include "hook\HookSpec.h"

#define _MAX_EVENTS_SHOW	1000

ULONG gDeadLocks = 0;
ULONG gReplyWaiterEnters = 0;

#ifdef _HOOK_IMPERSONATE_
#pragma message ("----------------------- impersonate code enabled")
#else
#pragma message ("----------------------- impersonate code DISABLED!!!")
#endif

BOOLEAN
InitEventList(PEVENT_LIST pEvList)
{
	pEvList->m_pEvQueue = NULL;
	pEvList->m_pEvQueue_FirstUnmarked = NULL;
	pEvList->m_QLen = 0;
	pEvList->m_QUnmarkedLen = 0;
	pEvList->m_MaxQueueLen = 0;
	pEvList->m_pEventMutex = InitKlgResourse();
	if (pEvList->m_pEventMutex == NULL)
		return FALSE;

	return TRUE;
}

void
DoneEventList(PEVENT_LIST pEvList)
{
	// ждать pEvList->m_QLen пок ане будет = 0
	if (pEvList->m_QLen != 0)
	{
		DbgBreakPoint();
	}
	else
	{
		pEvList->m_pEvQueue = NULL;
		pEvList->m_pEvQueue_FirstUnmarked = NULL;
		pEvList->m_QUnmarkedLen = 0;

		if (pEvList->m_pEventMutex != NULL)
			FreeKlgResourse(&pEvList->m_pEventMutex);
	}
}


#ifdef __DBG__
void
ShowEventsInfo(PEVENT_LIST pEvList)
{
	DWORD cou = 0;
	PQELEM Curr;

	if (pEvList->m_QLen == 0)
		return;

	AcquireResource(pEvList->m_pEventMutex, FALSE);

	if (pEvList->m_QLen > _MAX_EVENTS_SHOW)
	{
		DbPrint(DC_EVENT,DL_IMPORTANT, ("Events too many. Will be showed first %d :)\n", _MAX_EVENTS_SHOW));
	}

	Curr = pEvList->m_pEvQueue;
	while((Curr) && (cou < _MAX_EVENTS_SHOW))
	{
		if (Curr->m_Flags & FLT_A_POPUP)
		{
			DbPrint(DC_EVENT, DL_INFO, ("Event mark %#x (marker %#x) Proc: %s PkEv %#x\n", 
					Curr->m_Mark, Curr->m_MarkerProcID, Curr->m_ProcName,	&Curr->m_KEvent));
		}
		cou++;
		Curr = Curr->m_pNext;
	}

	ReleaseResource(pEvList->m_pEventMutex);
}
#endif

DWORD
CalcFEvParamSize(PFILTER_EVENT_PARAM pEventParam)
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

DWORD
CalcEventParamSize(PQELEM pEvent)
{
	DWORD EventSize;
	DWORD tmp;
	PSINGLE_PARAM pParam;
	BYTE *pPointer;
	DWORD cou;
	EventSize = 0;
	if (pEvent->m_ParamsCount != 0)
	{
		pParam = (PSINGLE_PARAM) pEvent->m_Params;
		for (cou = 0; cou < pEvent->m_ParamsCount; cou++)
		{
			if (pParam->ParamFlags & _SINGLE_PARAM_FLAG_POINTER)
				tmp = sizeof(DWORD);
			else
				tmp = sizeof(SINGLE_PARAM) + pParam->ParamSize;
			EventSize += tmp;
			pPointer = (BYTE*)pParam + tmp;
			pParam = (PSINGLE_PARAM) pPointer;
		}
	}
	return EventSize;
}

PSINGLE_PARAM
GetEventSingleParamPtr(PQELEM pEvent, ULONG ParamID)
{
	DWORD cou;
	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) pEvent->m_Params;
	for (cou =  0; cou < pEvent->m_ParamsCount; cou++)
	{
		if (pSingleParam->ParamID == ParamID)
			return pSingleParam;
		pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
	}
	return NULL;
}


_inline void
DecreaseUnmarked(PEVENT_LIST pEvList)
{
	if (pEvList->m_QUnmarkedLen != 0)
		pEvList->m_QUnmarkedLen--;
	else
	{
		DbPrint(DC_EVENT, DL_ERROR, ("pEvList->m_QUnmarkedLen already == 0\n"));
		DbgBreakPoint();		
	}
}

void
DelEventInQueue(PEVENT_LIST pEvList, PQELEM pEvent, BOOLEAN bManulDestroyEvent)
{
	PQELEM pPrevElem;

	if (pEvent == pEvList->m_pEvQueue_FirstUnmarked)
		pEvList->m_pEvQueue_FirstUnmarked = pEvent->m_pPrev;

	if (pEvent->m_Mark == 0)
		DecreaseUnmarked(pEvList);

	if (!bManulDestroyEvent)
		KeDestroyEvent(&pEvent->m_KEvent);

#ifdef _HOOK_IMPERSONATE_
	if (pEvent->m_SecurityFlag & _SecurityFlagsCaptured)
	{
		Security_ReleaseContext(&pEvent->m_ClientContext);
		pEvent->m_SecurityFlag &= (~_SecurityFlagsCaptured);
		pEvent->m_SecurityFlag &= (~_SecurityFlags_Impersonated);
	}
#endif
				
	if (pEvList->m_QLen != 0)
		pEvList->m_QLen--;
	else
	{
		DbPrint(DC_EVENT, DL_ERROR, ("pEvList->m_QLen already == 0\n"));
		DbgBreakPoint();		
	}

	pPrevElem = pEvent->m_pPrev;
	if (pPrevElem != NULL)
	{
		pPrevElem->m_pNext = pEvent->m_pNext;
		if (pEvent->m_pNext != NULL)
			pEvent->m_pNext->m_pPrev = pPrevElem;
	}
	else
	{
		pEvList->m_pEvQueue = pEvent->m_pNext;
		if (pEvent->m_pNext != NULL)
			pEvent->m_pNext->m_pPrev = NULL;
		
	}
}

PQELEM
QueueAdd(PFILTER_EVENT_PARAM pEventParam, PEVENT_LIST pEvList, PFILTER_SUB pFilterSub, ULONG YeldTimeout, 
		 DWORD EventFlags, DWORD dwClientFalgs, PEVENT_OBJECT_INFO pEventObject)
{
	PQELEM newEvQueue = NULL;
	DWORD ParamSize;
	DWORD MappedParamsSize = 0;

	if (pEvList->m_MaxQueueLen != 0)
	{
		if (pEvList->m_QLen > pEvList->m_MaxQueueLen)
		{
			DbPrint(DC_EVENT, DL_IMPORTANT, ("Max queue len reached! Skipping event with default client verdict\n"));
			DbgBreakPoint();
			return NULL;
		}
	}

#ifdef __DBG__
	if(pEventParam == NULL)
	{
		DbPrint(DC_EVENT,DL_ERROR, ("pEventParam is NULL\n"));
		DbgBreakPoint();
		return NULL;
	}
#endif
	AcquireResource(pEvList->m_pEventMutex, TRUE);

	//+ ----------------------------------------------------------------------------------------
	/*if (!(dwClientFalgs & _CLIENT_FLAG_WITHOUTCASCADE))
	{
		//try search such event
	}*/
	
	//+ ----------------------------------------------------------------------------------------
	

	ParamSize = CalcFEvParamSize(pEventParam);

	if (pFilterSub->m_pMappedFilterParams != NULL)
	{
		MappedParamsSize = pFilterSub->m_pMappedFilterParams->m_Param_Size;
	}
#ifdef __DBG__
	if (pFilterSub->m_Flags & FLT_A_POPUP)
		newEvQueue = ExAllocatePoolWithTag(NonPagedPool,sizeof(QELEM) + ParamSize + MappedParamsSize,'QboS');
	else
		newEvQueue = ExAllocatePoolWithTag(NonPagedPool,sizeof(QELEM) + ParamSize + MappedParamsSize,'IboS');
#else
	newEvQueue = ExAllocatePoolWithTag(NonPagedPool,sizeof(QELEM) + ParamSize + MappedParamsSize,'QboS');
#endif
	if(newEvQueue != NULL)
	{
		DbPrint(DC_EVENT,DL_INFO, ("IRP Queued (addr = %x)\n", newEvQueue));
		
		newEvQueue->m_Flags = pFilterSub->m_Flags;
		newEvQueue->m_FilterID = pFilterSub->m_FilterID;
		newEvQueue->m_HookID = pEventParam->HookID;
		newEvQueue->m_FunctionMj = pEventParam->FunctionMj;
		newEvQueue->m_FunctionMi = pEventParam->FunctionMi;
		newEvQueue->m_EventFlags = EventFlags;
		newEvQueue->m_ProcessID = pEventParam->ProcessID;
		newEvQueue->m_ThreadID = (ULONG)PsGetCurrentThreadId();

		if (pFilterSub->m_Flags & FLT_A_ALERT1)
			newEvQueue->m_EventFlags |= _EVENT_FLAG_ALERT1;
		if (pFilterSub->m_Flags & FLT_A_USERLOG)
			newEvQueue->m_EventFlags |= _EVENT_FLAG_LOGTHIS;
		if (pFilterSub->m_Flags & FLT_A_TOUCH)
			newEvQueue->m_EventFlags |= _EVENT_FLAG_TOUCH;
		if (pFilterSub->m_Flags & FLT_A_SAVE2CACHE)
			newEvQueue->m_EventFlags |= _EVENT_FLAG_SAVECACHE_AVAILABLE;

		// -----------------------------------------------------------------------------------------
		if (pFilterSub->m_Flags & FLT_A_DEFAULT)
			newEvQueue->m_EventFlags |= _EVENT_FLAG_AUTO_VERDICT_DEFAULT;
		else if (pFilterSub->m_Flags & FLT_A_PASS)
			newEvQueue->m_EventFlags |= _EVENT_FLAG_AUTO_VERDICT_PASS;
		else if (pFilterSub->m_Flags & FLT_A_DENY)
			newEvQueue->m_EventFlags |= _EVENT_FLAG_AUTO_VERDICT_DISCARD;
		// -----------------------------------------------------------------------------------------

		if(pEventObject != NULL)
		{
			newEvQueue->m_EventFlags |= _EVENT_FLAG_OBJECT_ATTENDED;
			newEvQueue->m_pEventObject = pEventObject;
		}
		else
		{
			newEvQueue->m_EventFlags &= (~_EVENT_FLAG_OBJECT_ATTENDED);
			newEvQueue->m_pEventObject = NULL;
		}
		
		if (!(pFilterSub->m_Flags & FLT_A_POPUP))
		{
			if (pFilterSub->m_Flags & FLT_A_DELETE_ON_MARK)
				newEvQueue->m_EventFlags |= _EVENT_FLAG_DELETED_ON_MARK;
		}

#ifdef _HOOK_IMPERSONATE_
		newEvQueue->m_StoppedThread = PsGetCurrentThread();
		newEvQueue->m_SecurityFlag = _SecurityFlagsNone;
		
		if (dwClientFalgs & _CLIENT_FLAG_WANTIMPERSONATE)
		{
			if (Security_CaptureContext(newEvQueue->m_StoppedThread, &newEvQueue->m_ClientContext) == STATUS_SUCCESS)
				newEvQueue->m_SecurityFlag |= _SecurityFlagsCaptured;
			else
			{
				DbPrint(DC_EVENT, DL_WARNING, ("Capture security context failed!\n"));
			}
		}
#endif

#ifdef _HOOK_VXD_ 		
		newEvQueue->m_pInBuffer = NULL;
		newEvQueue->m_InBufferSize = 0;
		newEvQueue->m_OutBufferSize = 0;
		newEvQueue->m_pOutBuffer = NULL;
#endif

		newEvQueue->m_Mark = 0;
		newEvQueue->m_MarkerProcID = 0;
		newEvQueue->m_dwCacheValue = pFilterSub->m_dwCacheValue;
		newEvQueue->m_ParamsCount = pEventParam->ParamsCount;

		newEvQueue->m_bVerdict = FALSE;
		newEvQueue->m_YeldTimeout = YeldTimeout;

		if (pEventParam->ParamsCount != 0) 
		{
			DbPrint(DC_EVENT,DL_SPAM, ("EventParam parameters size %d\n", ParamSize));
			memcpy(newEvQueue->m_Params, pEventParam->Params, ParamSize);
		}

		if (pFilterSub->m_pMappedFilterParams != NULL)
		{
			memcpy((BYTE*)(newEvQueue->m_Params) + ParamSize, pFilterSub->m_pMappedFilterParams->m_SingleParams, 
				pFilterSub->m_pMappedFilterParams->m_Param_Size);
			newEvQueue->m_ParamsCount += pFilterSub->m_pMappedFilterParams->m_ParamsCount;
		}

		strncpy(newEvQueue->m_ProcName, pEventParam->ProcName, PROCNAMELEN);
		newEvQueue->m_Timeout = pFilterSub->m_Timeout;
		newEvQueue->m_Verdict = pFilterSub->m_Verdict;

		KeInitializeEvent(&newEvQueue->m_KEvent, SynchronizationEvent, FALSE);

		newEvQueue->m_pPrev = NULL;
		newEvQueue->m_pNext = pEvList->m_pEvQueue;
		if (pEvList->m_pEvQueue != NULL)
			pEvList->m_pEvQueue->m_pPrev = newEvQueue;
		pEvList->m_pEvQueue = newEvQueue;

		if (pEvList->m_pEvQueue_FirstUnmarked == NULL)
			pEvList->m_pEvQueue_FirstUnmarked = newEvQueue;

		DbPrint(DC_EVENT, DL_SPAM, ("Queuing -> new item %#x\n", newEvQueue));
	
		pEvList->m_QLen++;
		pEvList->m_QUnmarkedLen++;
	}
	else
	{
		DbPrint(DC_EVENT,DL_ERROR, ("Queuing failed, no memory???\n"));
	}
	
	ReleaseResource(pEvList->m_pEventMutex);
	
	return newEvQueue;
}

BOOLEAN
QueueDelByRef(PEVENT_LIST pEvList, PQELEM pElem)
{
	PQELEM Prev;
	PQELEM Curr;

	AcquireResource(pEvList->m_pEventMutex, TRUE);

	Prev = NULL;
	Curr = pEvList->m_pEvQueue;
	while (Curr != NULL)
	{
		if(Curr == pElem) 
		{
			KeSetEvent(&Curr->m_KEvent, 0, FALSE);

			DelEventInQueue(pEvList, Curr, FALSE);

			DbPrint(DC_EVENT,DL_INFO, ("Event %x deleted (addr = %x)\n", Curr->m_Mark, Curr));
			ExFreePool(Curr);

			ReleaseResource(pEvList->m_pEventMutex);
			return TRUE;
		}
		Prev = Curr;
		Curr = Curr->m_pNext;
	}
	
	DbPrint(DC_EVENT,DL_WARNING, ("Deleting by ref failed, why??? (addr = %x)\n", pElem));
	
	ReleaseResource(pEvList->m_pEventMutex);
	
	return FALSE;
}

BOOLEAN
QueueSkipEvent(PEVENT_LIST pEvList)
{
	BOOLEAN bRet = FALSE;
	PQELEM Curr;

	AcquireResource(pEvList->m_pEventMutex, TRUE);

	Curr = pEvList->m_pEvQueue_FirstUnmarked;

	if (Curr != NULL)
	{
		DbPrint(DC_EVENT,DL_NOTIFY, ("Skip event %#x\n", Curr));
#ifdef __DBG__
		if (pEvList->m_QLen == 0)
		{
			DbPrint(DC_EVENT, DL_ERROR, ("Strange! event is empty but m_pEvQueue_FirstUnmarked != 0\n"));
			DbgBreakPoint();
		}
#endif
		pEvList->m_pEvQueue_FirstUnmarked = Curr->m_pPrev;
		Curr->m_bVerdict = TRUE;
		KeSetEvent(&Curr->m_KEvent, 0, FALSE);
		if (!(Curr->m_Flags & FLT_A_POPUP))
		{
			DelEventInQueue(pEvList, Curr, FALSE);

			ExFreePool(Curr);
			bRet = TRUE;
		}
		DbPrint(DC_EVENT,DL_INFO, ("Event skiped\n"));
	}
	
	ReleaseResource(pEvList->m_pEventMutex);
	
	return bRet;
}

NTSTATUS
SkipEventForClient(ULONG AppID)
{
	NTSTATUS Status = STATUS_NOT_FOUND;
	PQCLIENT pClient = AcquireClient(AppID);
	if (pClient != NULL) {
		if (QueueSkipEvent(&pClient->m_EventList))
			Status = STATUS_SUCCESS;
		ReleaseClient(pClient);
	} else {
		DbPrint(DC_EVENT,DL_WARNING, ("IOCTLHOOK_GetEvent - client not found(%d)!\n", AppID));
	}
	return Status;
}

BOOLEAN
QueueGetSize(PEVENT_LIST pEvList, DWORD* pdwSize)
{
	BOOLEAN ret = FALSE;
	PQELEM pCurrElem;

	*pdwSize = 0;

	AcquireResource(pEvList->m_pEventMutex, TRUE);
	
	pCurrElem = pEvList->m_pEvQueue_FirstUnmarked;
	if (pCurrElem != NULL)
	{
		DWORD ParamSize;

		ParamSize = CalcEventParamSize(pCurrElem);
		*pdwSize = ParamSize + sizeof(EVENT_TRANSMIT);
	}

	ReleaseResource(pEvList->m_pEventMutex);
	return ret;
}

void
GetEventSizeForClient(ULONG AppID, DWORD *pSize)
{
	PQCLIENT pClient = AcquireClient(AppID);
	if (pClient != NULL)
	{
		QueueGetSize(&pClient->m_EventList, pSize);
		ReleaseClient(pClient);
	}
	else
		DbPrint(DC_EVENT,DL_WARNING, ("IOCTLHOOK_GetEvent - client not found(%d)!\n", AppID));
}

BOOLEAN
QueueGet(PEVENT_LIST pEvList, ULONG Mark, PEVENT_TRANSMIT pEvent, DWORD* pInBufferLength)
{
	BOOLEAN ret = FALSE;
	PQELEM pFoundElem;

	if (Mark == 0)
		return FALSE;

	AcquireResource(pEvList->m_pEventMutex, TRUE);

	pFoundElem = pEvList->m_pEvQueue_FirstUnmarked;

	if (pFoundElem != NULL)
	{
		DWORD ParamSize;

		ParamSize = CalcEventParamSize(pFoundElem);
		if (ParamSize + sizeof(EVENT_TRANSMIT) <= *pInBufferLength)
		{
			*pInBufferLength = ParamSize + sizeof(EVENT_TRANSMIT);
			pEvList->m_pEvQueue_FirstUnmarked = pEvList->m_pEvQueue_FirstUnmarked->m_pPrev;

			DecreaseUnmarked(pEvList);

			pFoundElem->m_Mark = Mark;
			pFoundElem->m_MarkerProcID = (ULONG) PsGetCurrentProcessId();

			pEvent->m_FilterID = pFoundElem->m_FilterID;
			
			strncpy(pEvent->m_ProcName, pFoundElem->m_ProcName, PROCNAMELEN);
			pEvent->m_ProcessID = pFoundElem->m_ProcessID;
			pEvent->m_HookID = pFoundElem->m_HookID;
			pEvent->m_FunctionMj = pFoundElem->m_FunctionMj;
			pEvent->m_FunctionMi = pFoundElem->m_FunctionMi;
			pEvent->m_EventFlags = pFoundElem->m_EventFlags;
			pEvent->m_Timeout = pFoundElem->m_Timeout;

			pEvent->m_ParamsCount = pFoundElem->m_ParamsCount;

			memcpy(pEvent->m_Params, pFoundElem->m_Params, ParamSize);
			
			ret=TRUE;
			if (pFoundElem->m_EventFlags & _EVENT_FLAG_DELETED_ON_MARK)
			{
				DelEventInQueue(pEvList, pFoundElem, FALSE);

				DbPrint(DC_EVENT,DL_INFO, ("Event %#x is marked with %x and deleted\n", pFoundElem, Mark));
				ExFreePool(pFoundElem);
			}
			else
			{
				DbPrint(DC_EVENT,DL_INFO, ("Event %#x is marked with %x\n", pFoundElem, Mark));
			}
		}
		else
		{
			DbPrint(DC_EVENT,DL_WARNING, ("too small buffer (%d) for 0x%x:0x%x need %d - EVENT_TRANSMIT\n", *pInBufferLength, 
				pFoundElem->m_HookID, pFoundElem->m_FunctionMj, ParamSize + sizeof(EVENT_TRANSMIT)));
			
			ReleaseResource(pEvList->m_pEventMutex);

			//DbgBreakPoint();
			
			return ret;
		}
	}
	else
	{
		if (pEvList->m_QUnmarkedLen != 0)
		{
			DbPrint(DC_EVENT, DL_ERROR, ("Strange! m_QUnmarkedLen != 0 but m_pEvQueue_FirstUnmarked == 0\n"));
			DbgBreakPoint();
			pEvList->m_QUnmarkedLen = 0;
		}
	}

	ReleaseResource(pEvList->m_pEventMutex);
#ifdef __DBG__
	if (!ret)
		DbPrint(DC_EVENT,DL_IMPORTANT, ("Marking %x failed, no free events\n",Mark));
#endif //__DBG__
	
	return ret;
}

BOOLEAN
GetEventForClient(PGET_EVENT pGetEvent, PEVENT_TRANSMIT pEventTransmit, DWORD* pOutputBufferLength)
{
	BOOLEAN bRet = FALSE;
	PQCLIENT pClient = AcquireClient(pGetEvent->m_AppID);
	if (pClient == NULL)
	{
		DbPrint(DC_EVENT,DL_WARNING, ("IOCTLHOOK_GetEvent - client not found(%d)!\n", pGetEvent->m_AppID));
	}
	else
	{
		if(QueueGet(&pClient->m_EventList, pGetEvent->Mark, pEventTransmit, pOutputBufferLength))
		{
			PQSUBCLIENT pSubClient = NULL;
			AcquireClientResource(TRUE);
			
			pSubClient = GetCurrentSubClient(pClient->m_pSubClient);
			if (pSubClient != NULL)
				pSubClient->m_ActivityWatchDog = ACTIVITY_WATCHDOG_TIMEOUT;

			ReleaseClientResource();
			bRet = TRUE;
		}
		ReleaseClient(pClient);
	}

	return bRet;
}

BOOLEAN
CheckYeldExFiled(PYELD_EVENT pInYeld, ULONG InYeldSize)
{
	if ((InYeldSize <= (sizeof(YELD_EVENT) + pInYeld->YeldParameterSize)) && (pInYeld->YeldParameterSize >= sizeof(ULONG)))
		return TRUE;

	DbgBreakPoint();
	return FALSE;
}

NTSTATUS
ImpersonateThreadForClient(PIMPERSONATE_REQUEST pImperonate)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
#ifdef _HOOK_IMPERSONATE_	

	PQCLIENT pClient;
	PEVENT_LIST pEvList;
	PQELEM Curr;

	pClient = AcquireClient(pImperonate->m_AppID);
	if (pClient == NULL)
	{
		DbPrint(DC_EVENT,DL_WARNING, ("ImpersonateThreadForClient - client not found (%d)!\n", pImperonate->m_AppID));
	}
	else
	{
		Status = STATUS_NOT_FOUND;
		AcquireResource(pClient->m_EventList.m_pEventMutex, TRUE);

		pEvList = &pClient->m_EventList;

		if (pEvList->m_pEvQueue_FirstUnmarked == NULL)
			Curr = pEvList->m_pEvQueue;
		else
			Curr = pEvList->m_pEvQueue_FirstUnmarked;
		
		while(Curr && Status == STATUS_NOT_FOUND)
		{
			if((Curr->m_Mark == pImperonate->m_Mark) && (Curr->m_MarkerProcID == (ULONG) PsGetCurrentProcessId()))
			{
				//!! sdes imperosnate
				if ((Curr->m_SecurityFlag & _SecurityFlagsCaptured) && 
					(!(Curr->m_SecurityFlag & _SecurityFlags_Impersonated)))
				{
					NTSTATUS ntImpersonate;
					DbPrint(DC_EVENT, DL_NOTIFY, ("Impersonating by mark %#x\n", Curr->m_Mark));
					ntImpersonate = MyImpersonateClient(PsGetCurrentThread(), &Curr->m_ClientContext);
					Curr->m_SecurityFlag |= _SecurityFlags_Impersonated;
				}
				
				Status = STATUS_SUCCESS;
			}
			Curr = Curr->m_pNext;
		}
			
		ReleaseResource(pEvList->m_pEventMutex);
		ReleaseClient(pClient);
#ifdef __DBG__
		if(Status == STATUS_NOT_FOUND) {
			DbPrint(DC_EVENT,DL_WARNING, ("ImpersonateThreadForClient - STATUS_NOT_FOUND (mark %#x)!\n", 
				pImperonate->m_Mark));
		}
#endif
	}
#endif
	
	return Status;
}

NTSTATUS
QueueYeld(PEVENT_LIST pEvList, PYELD_EVENT pInYeld, ULONG InYeldSize, PCHAR pOutBuffer, PULONG pOutBufferSize)
{
	NTSTATUS StatusRet = STATUS_NOT_FOUND;
	PQELEM Curr;
	
	AcquireResource(pEvList->m_pEventMutex, TRUE);

	if (pEvList->m_pEvQueue_FirstUnmarked == NULL)
		Curr = pEvList->m_pEvQueue;
	else
		Curr = pEvList->m_pEvQueue_FirstUnmarked;

	while(Curr) {
		if((Curr->m_Mark == pInYeld->Mark) && (Curr->m_MarkerProcID == (ULONG) PsGetCurrentProcessId()))
		{
			Curr->m_YeldTimeout = pInYeld->Timeout;
			if (pInYeld->YeldParameterSize == 0)
				StatusRet = STATUS_SUCCESS;
			else
			{
				if (CheckYeldExFiled(pInYeld, InYeldSize) == FALSE || !(Curr->m_Flags & FLT_A_POPUP))
				{
					DbgBreakPoint();
					StatusRet = STATUS_INVALID_PARAMETER;
				}
				else
				{
					PEXEC_ON_HOLD pExec = (PEXEC_ON_HOLD) pInYeld->YeldParams;
					if (__he_ForceYeld == pExec->m_FunctionID)
						KeSetEvent(&Curr->m_KEvent, 0, FALSE);
					else
					{
#ifdef _HOOK_VXD_
						PCHAR		pInBufferTmp = NULL;
						PCHAR		pOutBufferTmp = NULL;
						KEVENT		HoldEvent;

						DbPrint(DC_EVENT,DL_NOTIFY, ("Yielding: processing on holded thread (mark %x timeout %d)\n", 
							pInYeld->Mark, pInYeld->Timeout));

						KeInitializeEvent(&HoldEvent, NotificationEvent, FALSE);
						
						Curr->m_HoldKEvent = HoldEvent;

						Curr->m_InBufferSize = pInYeld->YeldParameterSize;
						Curr->m_OutBufferSize = *pOutBufferSize;
						
						pInBufferTmp  = ExAllocatePoolWithTag(NonPagedPool, Curr->m_InBufferSize,'DboS');
						pOutBufferTmp = ExAllocatePoolWithTag(NonPagedPool, Curr->m_OutBufferSize,'DboS');
						
						if (pInBufferTmp  != NULL && pOutBufferTmp != NULL)
						{
							Curr->m_pOutBuffer = pOutBufferTmp;
							Curr->m_pInBuffer = pInBufferTmp;

							memcpy(Curr->m_pInBuffer, pInYeld->YeldParams, Curr->m_InBufferSize);
							
							KeSetEvent(&Curr->m_KEvent,0, FALSE);
							// обработка на остановленной треде
							
//#ifdef _HOOK_NT_
//							KeWaitForSingleObject(&HoldEvent, UserRequest, KernelMode, FALSE, NULL);
//#else
							Wait_Semaphore(HoldEvent, BLOCK_FORCE_SVC_INTS);
//#endif
							{
								*(pOutBufferSize) = Curr->m_OutBufferSize;
								if (Curr->m_OutBufferSize != 0)
									memcpy(pOutBuffer, Curr->m_pOutBuffer, Curr->m_OutBufferSize);
								Curr->m_pOutBuffer = NULL;
								
								StatusRet = STATUS_SUCCESS;
							}
						}

						if (pInBufferTmp != NULL)
							ExFreePool(pInBufferTmp);
						if (pOutBufferTmp != NULL)
							ExFreePool(pOutBufferTmp);
						
						if (Curr != 0)
						{
							Curr->m_pInBuffer = NULL;
							Curr->m_InBufferSize = 0;
							Curr->m_pOutBuffer = NULL;
							Curr->m_OutBufferSize = 0;
							
							KeDestroyEvent(&HoldEvent);
						}
						DbPrint(DC_EVENT,DL_NOTIFY, ("Yielding: end processing on holded thread (mark %x)\n", 
							pInYeld->Mark));
#else
						StatusRet = STATUS_UNSUCCESSFUL;
						DbgBreakPoint();
#endif
					}
				}
			}
			
			DbPrint(DC_EVENT,DL_SPAM, ("Yielding: mark %x timeout %d\n", pInYeld->Mark, pInYeld->Timeout));
			ReleaseResource(pEvList->m_pEventMutex);
			
			return StatusRet;
		}
		Curr = Curr->m_pNext;
	}
	
	ReleaseResource(pEvList->m_pEventMutex);
	DbPrint(DC_EVENT,DL_SPAM, ("Yielding failed, mark %x not found\n", pInYeld->Mark));

	return STATUS_UNSUCCESSFUL;
}

NTSTATUS
YeldEventForClient(PYELD_EVENT pYeldEvent, ULONG InYeldSize, PCHAR OutputBuffer, DWORD* pOutDataLen)
{
	NTSTATUS Status = STATUS_NOT_FOUND;
	PQCLIENT pClient = AcquireClient(pYeldEvent->m_AppID);
	if (pClient != NULL)
	{
		Status = QueueYeld(&pClient->m_EventList, pYeldEvent, InYeldSize, OutputBuffer, pOutDataLen);
		ReleaseClient(pClient);
	}
	else
		DbPrint(DC_EVENT,DL_WARNING, ("IOCTLHOOK_YieldEvent - client not found (%d)!\n", pYeldEvent->m_AppID));
	
	return Status;
}

BOOLEAN
QueueSet(PQCLIENT pClient, ULONG Mark, VERDICT Verdict, ULONG ExpirationTimeShift)
{
	PQELEM Curr;
	PEVENT_LIST pEvList;

	if (Mark == 0)
		return FALSE;

	pEvList = &pClient->m_EventList;

	AcquireResource(pEvList->m_pEventMutex, TRUE);
	
	if (pEvList->m_pEvQueue_FirstUnmarked == NULL)
		Curr = pEvList->m_pEvQueue;
	else
		Curr = pEvList->m_pEvQueue_FirstUnmarked;
	
	while(Curr)
	{
		if((Curr->m_Mark == Mark) && (Curr->m_MarkerProcID == (ULONG) PsGetCurrentProcessId()))
		{
			Curr->m_bVerdict = TRUE;
			if ((pClient->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE) && (Curr->m_Flags &FLT_A_SAVE2CACHE))
			{
				if (Verdict != Verdict_Default)
				{
					DbPrint(DC_EVENT,DL_INFO, ("Cache update Verdict %d dwVal: %x\n", Verdict, Curr->m_dwCacheValue));
					Obj_CacheUpdate(pClient, Curr->m_dwCacheValue, Verdict, ExpirationTimeShift);
				}
			}
			Curr->m_Verdict = Verdict;
			
			DbPrint(DC_EVENT,DL_INFO, ("Verdict from clients: mark %x\n", Mark));
			if (!(Curr->m_Flags & FLT_A_POPUP))
			{
				DelEventInQueue(pEvList, Curr, TRUE);

				DbPrint(DC_EVENT,DL_INFO, ("Event %x deleted (addr = %x)\n", Curr->m_Mark, Curr));
				ReleaseResource(pEvList->m_pEventMutex);
				
				KeSetEvent(&Curr->m_KEvent,0, FALSE);
				KeDestroyEvent(&Curr->m_KEvent);
				ExFreePool(Curr);
			}
			else
			{
				KeSetEvent(&Curr->m_KEvent,0,FALSE);
				ReleaseResource(pEvList->m_pEventMutex);
			}

			DbPrint(DC_EVENT,DL_INFO, ("Event %x judged %x\n", Mark, Verdict));

			return TRUE;
		}
		Curr = Curr->m_pNext;
	}
	
	ReleaseResource(pEvList->m_pEventMutex);
	DbPrint(DC_EVENT,DL_WARNING, ("Judging failed, mark %x not found\n",Mark));

	return FALSE;
}

NTSTATUS
EventSetVerdict(PSET_VERDICT pSetVerdict)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	PQCLIENT pClient = AcquireClient(pSetVerdict->m_AppID);
	if (pClient != NULL)
	{
		if (QueueSet(pClient, pSetVerdict->m_Mark, pSetVerdict->m_Verdict, pSetVerdict->m_ExpTime))
			Status = STATUS_SUCCESS;
		ReleaseClient(pClient);
	}
	else
		DbPrint(DC_EVENT,DL_WARNING, ("IOCTLHOOK_GetEvent - client not found (%d)!\n", pSetVerdict->m_AppID));

	return Status;
}

void
ReleaseMarkedEvents(ULONG AppID, PEVENT_LIST pEvList, ULONG ProcessID)
{
	PQELEM EvQueue;
	PQELEM Curr;
	PQELEM Tmp;
	PQELEM Prev = NULL;
	DWORD EvReleased = 0;
	
	DbPrint(DC_EVENT,DL_INFO, ("Release marked events for %d. EveList = 0x%x \n", AppID, pEvList));
	
	if (pEvList->m_QLen == 0)
		return;
	
	AcquireResource(pEvList->m_pEventMutex, TRUE);
	
	EvQueue = pEvList->m_pEvQueue;
	
	Curr = EvQueue;

	while(Curr)
	{
		Tmp	= Curr->m_pNext;
		if ((Curr->m_Mark != 0) && (Curr->m_MarkerProcID == ProcessID))
		{
			EvReleased++;
			
			KeSetEvent(&Curr->m_KEvent,0, FALSE);
			
			if (!(Curr->m_Flags & FLT_A_POPUP))
			{
				DelEventInQueue(pEvList, Curr, FALSE);

				DbPrint(DC_EVENT, DL_INFO, ("Release event %#x\n", Curr));
				ExFreePool(Curr);
			}
		}
		Curr = Tmp;
	}
	
	DbPrint(DC_EVENT,DL_NOTIFY, ("EvQueue - %d events signaled\n", EvReleased));
	ReleaseResource(pEvList->m_pEventMutex);
}

void
ReleasePopupEventsByProcess(PEVENT_LIST pEvList, ULONG ProcessId, ULONG HookId)
{
	PQELEM Curr;
	DWORD EvReleased = 0;

	AcquireResource(pEvList->m_pEventMutex, TRUE);

	Curr = pEvList->m_pEvQueue;

	while(Curr)
	{
		if ((Curr->m_ProcessID == ProcessId)
			&& (Curr->m_FunctionMj == HookId))
		{
			KeSetEvent(&Curr->m_KEvent,0, FALSE);
			EvReleased++;
		}

		Curr = Curr->m_pNext;
	}

	ReleaseResource(pEvList->m_pEventMutex);

	if (EvReleased)
	{
		DbgBreakPoint();
	}
}

void
ReleaseEvents(ULONG AppID, PEVENT_LIST pEvList)
{
	PQELEM EvQueue;
	PQELEM Curr;
	PQELEM Tmp;
	PQELEM Prev = NULL;
	DWORD EvReleased = 0;

	DbPrint(DC_EVENT,DL_INFO, ("Release events for %d. EveList = 0x%x \n", AppID, pEvList));

	if (pEvList->m_QLen == 0)
		return;
	
	AcquireResource(pEvList->m_pEventMutex, TRUE);

	EvQueue = pEvList->m_pEvQueue;

	Curr = EvQueue;
	while(Curr)
	{
		EvReleased++;
		
		KeSetEvent(&Curr->m_KEvent,0, FALSE);

		Tmp	= Curr->m_pNext;

		if (!(Curr->m_Flags & FLT_A_POPUP))
		{
			DelEventInQueue(pEvList, Curr, FALSE);
			
			DbPrint(DC_EVENT,DL_INFO, ("Release event %#x\n", Curr));
			ExFreePool(Curr);
		}
		Curr = Tmp;
	}
	
	if (pEvList->m_pEvQueue)
	{
		DbgBreakPoint();
	}

	DbPrint(DC_EVENT,DL_NOTIFY, ("EvQueue - %d events signaled\n", EvReleased));
	ReleaseResource(pEvList->m_pEventMutex);
}

#ifdef _HOOK_VXD_ 
unsigned long MsgBox(char *Title, char *Message);

extern PCHAR VideoMem;

CHAR *AvpgStr=" KL Guard ";
CHAR *AllowStr="Allow this action? [Y/N]";

PriorityVMEvent_THUNK VMEvent_Thunk;
TIMEOUT_THUNK   Timeout_thunk;

VOID __stdcall
SysVMEvent4WhistleUp(VMHANDLE hVM, PKLG_EVENT pEvent, PCLIENT_STRUCT pcrs, DWORD Flags)
{
	DbPrint(DC_EVENT,DL_NOTIFY, ("SysVMEvent4WhistleUp\n"));
	VWIN32_SetWin32Event(pEvent->hWhistle); 
}

VOID __stdcall
TimeOutCallBack(VMHANDLE hVM,PCLIENT_STRUCT pcrs,PQELEM pEventElem,DWORD Extra)
{
	DbPrint(DC_EVENT,DL_NOTIFY, ("TimeoutCallBack\n"));
	KeSetEvent(&pEventElem->m_KEvent, 0, FALSE);
}

#endif //_HOOK_VXD_ 

VERDICT
BlueScreenReq(PFILTER_EVENT_PARAM pEventParam, PCHAR pUrl)
{
	VERDICT Verdict = Verdict_Default;
#ifdef _HOOK_VXD_ 
CHAR				Message[MAX_MSG_LEN];
CHAR				FuncName[MAX_MSG_LEN];
VMHANDLE			CurVM;
ULONG				Width, Height;
	DbPrint(DC_EVENT,DL_NOTIFY, ("BlueScreenReq\n"));
	VxDGetFunctionStr(FuncName,pEventParam->HookID, pEventParam->FunctionMj, pEventParam->FunctionMi);
	sprintf(Message,"%s trying to %s\"%s\"Allow this action?", pEventParam->ProcName, FuncName, pUrl);
	CurVM=Get_Sys_VM_Handle();

	Begin_Critical_Section(BLOCK_SVC_INTS);
	System_Control(BEGIN_MESSAGE_MODE,CurVM,0,0,0);
	System_Control(SET_DEVICE_FOCUS,CurVM,0,0,0);

	VDD_Msg_ClrScrn(CurVM,1,&Width,&Height);

	VideoMem=MapPhysToLinear((VOID*)0xb8000,50*80*2,0);
	DbPrint(DC_SYS,DL_INFO, ("VideoMem=%x\n",VideoMem));
	Verdict = MsgBox(AvpgStr, Message);

	System_Control(END_MESSAGE_MODE,CurVM,0,0,0);
	End_Critical_Section();
#endif //_HOOK_VXD_ 
	return Verdict;
}

//+ ------------------------------------------------------------------------------------------

//2 sobko!!! if returned value FALSE you should not stop thread and rescedule event for delayd processing
// don't forget it

BOOLEAN
PulseEventToSubClient(PKLG_EVENT pEvent, DWORD DeadLockWatchDog, ULONG AppID)
{
	if (pEvent == NULL)
		return FALSE;
#ifdef _HOOK_NT_
	if(KeGetCurrentIrql()>DISPATCH_LEVEL)
		return FALSE;

	if(pEvent->pWhistle)
	{
		DbPrint(DC_EVENT,DL_NOTIFY, ("PulseEventToClient for AppID %d\n", AppID));
		KeSetEvent(pEvent->pWhistle, 0, FALSE);
		return TRUE;
	}
	else
	{
		DbPrint(DC_EVENT,DL_ERROR, ("PulseEventToClient for AppID %d, pWhistle=NULL\n", AppID));
		DbgBreakPoint();
		return FALSE;
	}
#else
	if(Test_Sys_VM_Handle(Get_Cur_VM_Handle()))
	{
		DbPrint(DC_EVENT,DL_NOTIFY, ("SetWin32Event in SysVM\n"));
		VWIN32_SetWin32Event(pEvent->hWhistle);
	}
	else 
		Call_Priority_VM_Event(CUR_RUN_VM_BOOST, Get_Sys_VM_Handle(), 0, pEvent, SysVMEvent4WhistleUp, 0, 
			&VMEvent_Thunk);
	
	return TRUE;
#endif //_HOOK_NT_
	return TRUE;
}

BOOLEAN
PulseEventToSubClientSafe(PQSUBCLIENT pSubClient, BOOLEAN bWhistlerUp, DWORD DeadLockWatchDog, ULONG AppID)
{
	BOOLEAN bPulse = FALSE;
#ifdef _HOOK_NT_
	AcquireClientResource(FALSE);
#endif
	if (IsSubClientConnected(pSubClient))
		bPulse = PulseEventToSubClient(bWhistlerUp ? pSubClient->m_pWhistleUp : pSubClient->m_pWhistlerChanged, 
		DeadLockWatchDog, AppID);
	else
	{
		DbPrint(DC_EVENT, DL_IMPORTANT, ("PulseEventToSubClientSafe - SubClient is disconnected now\n"));
		DbgBreakPoint();
	}

#ifdef _HOOK_NT_
	ReleaseClientResource();
#endif //_HOOK_NT_

	return bPulse;
}


VERDICT
ReplyWaiter(PQSUBCLIENT pSubClient, PEVENT_LIST pEvList, PQELEM pEventElem, PCHAR pUrl, ULONG AppID, 
			peReplyWaiter peReply)
{
	VERDICT Verdict = Verdict_WDog;
	BOOLEAN bExit;
	BOOLEAN bPulse;
#ifdef _HOOK_NT_
	LARGE_INTEGER Timeout;
#else
	TIMEOUTHANDLE hTimeout=0;
#endif //_HOOK_NT_
	NTSTATUS ntstatus=STATUS_SUCCESS;
#ifdef __DBG__
	DWORD ThreadID;
#ifdef _HOOK_NT_
	ThreadID = (ULONG)PsGetCurrentThreadId();
#else
	ThreadID = (ULONG)Get_Cur_Thread_Handle();
#endif//_HOOK_NT_
#endif//Debug
	InterlockedIncrement(&gReplyWaiterEnters);

	*peReply = ReplyWaiter_SendFailed;

// -----------------------------------------------------------------------------------------
	DbPrint(DC_EVENT,DL_NOTIFY, ("Thread %x from process %s with "_STRING_DEF" stopped on event %#x\n", 
		ThreadID, pEventElem->m_ProcName, pUrl, pEventElem));

	bPulse = PulseEventToSubClientSafe(pSubClient, TRUE, 
		pEventElem->m_Timeout > 0 ? pEventElem->m_Timeout : DEADLOCKWDOG_TIMEOUT, AppID);

	if (bPulse == TRUE) 
	{
#ifdef _HOOK_NT_
		DbPrint(DC_EVENT,DL_INFO, ("Set timeout %d sec\n", pEventElem->m_Timeout));
		*(__int64*)&Timeout= -((__int64)pEventElem->m_Timeout*10000000L);
		bExit = FALSE;

		while (bExit == FALSE)
		{
			bExit = TRUE;
			if (pEventElem->m_Timeout)
				ntstatus = KeWaitForSingleObject(&pEventElem->m_KEvent, UserRequest, KernelMode, FALSE, &Timeout);
			else
				ntstatus = KeWaitForSingleObject(&pEventElem->m_KEvent, UserRequest, KernelMode, FALSE, NULL);
			
			pEventElem->m_Timeout = pEventElem->m_YeldTimeout;
			*(__int64*)&Timeout= -((__int64)pEventElem->m_Timeout*10000000L);

			if (pEventElem->m_bVerdict == FALSE)
			{
				if(pEventElem->m_Mark != 0)
				{
					*peReply = ReplyWaiter_WaitFailed;	//client gave packet from driver
					if (pEventElem->m_YeldTimeout != 0)
					{
						pEventElem->m_YeldTimeout = 0;
						DbPrint(DC_EVENT,DL_INFO, ("Event %x yielded with new timeout = %d\n", pEventElem->m_Mark, 
							pEventElem->m_Timeout));
						
						bExit = FALSE;
						/*if (ntstatus == STATUS_SUCCESS)
						{
							// check processing on holded thread
							if ((pEventElem->m_pInBuffer != NULL) && 
								(pEventElem->m_InBufferSize  >= sizeof(EXEC_ON_HOLD)))
							{
								DbPrint(DC_EVENT,DL_NOTIFY, ("Event %x yielded - execution on holded thread\n", 
									pEventElem->m_Mark));
								
								ExecutionOnHoldedThread(pEventElem->m_pInBuffer, pEventElem->m_InBufferSize, 
									pEventElem->m_pOutBuffer, &pEventElem->m_OutBufferSize);
								
								KeSetEvent(&pEventElem->m_HoldKEvent, 0, FALSE);
							}
						}*/
					}
				}
			}
		}
		if (ntstatus == STATUS_TIMEOUT)
		{
			DbPrint(DC_EVENT,DL_IMPORTANT, ("Deadlock (%u) for mark %x detected, status - %x\n", gDeadLocks++, 
				pEventElem->m_Mark, ntstatus));
			DbgBreakPoint();
		}
	#else
		bExit = FALSE;

		while (bExit == FALSE)
		{
			bExit = TRUE;
			DbPrint(DC_EVENT,DL_NOTIFY, ("Set timeout %d sec (Mark %x)for %s - "_STRING_DEF"\n",
				pEventElem->m_Timeout, pEventElem->m_Mark, pEventElem->m_ProcName, pUrl));//!!

			hTimeout = Set_Global_Time_Out(pEventElem->m_Timeout*1000, pEventElem, TimeOutCallBack, &Timeout_thunk);

			Wait_Semaphore(pEventElem->m_KEvent, BLOCK_FORCE_SVC_INTS);

			if (hTimeout != 0)
			{
				Cancel_Time_Out(hTimeout);
				hTimeout = 0;
			}

			if (pEventElem->m_bVerdict == FALSE)
			{
				if(pEventElem->m_Mark != 0)
				{
					*peReply = ReplyWaiter_WaitFailed;	//client gave packet from driver
					if (pEventElem->m_YeldTimeout != 0)
					{
						pEventElem->m_Timeout = pEventElem->m_YeldTimeout;
						pEventElem->m_YeldTimeout = 0;
						if ((pEventElem->m_pInBuffer != NULL) && (pEventElem->m_InBufferSize >= sizeof(EXEC_ON_HOLD)))
						{
							DbPrint(DC_EVENT,DL_NOTIFY, ("Event %x yielded - execution on holded thread\n", 
								pEventElem->m_Mark));
							ExecutionOnHoldedThread(pEventElem->m_pInBuffer, pEventElem->m_InBufferSize, 
								pEventElem->m_pOutBuffer, &pEventElem->m_OutBufferSize);
							KeSetEvent(&pEventElem->m_HoldKEvent, 0, FALSE);
						}
						else
 							DbPrint(DC_EVENT,DL_INFO, ("Event %x yielded with new timeout = %d\n", pEventElem->m_Mark,
							pEventElem->m_Timeout));

						bExit = FALSE;
					}
				}
			}
		}
	#endif //_HOOK_NT_
		DbPrint(DC_EVENT,DL_INFO, ("Waited %x (timeout=%d), status - %x\n",pEventElem->m_Mark, 
			pEventElem->m_Timeout, ntstatus));
		
		DbPrint(DC_EVENT,DL_NOTIFY, ("Thread %x from process %s with "_STRING_DEF" resumed\n", ThreadID, 
			pEventElem->m_ProcName, pUrl));
	}
	
	Verdict = pEventElem->m_Verdict;
#ifdef __DBG__
	if (Verdict > Verdict_Debug)
	{
		DbPrint(DC_EVENT,DL_WARNING, ("Verdict value is broken - new %d\n", Verdict));
		DbgBreakPoint();
	}
#endif
	
	if (pEventElem->m_bVerdict == TRUE)
		*peReply = ReplyWaiter_Succeed;	
	else
		pEventElem->m_bVerdict = TRUE;

	if (QueueDelByRef(pEvList, pEventElem) == FALSE)
	{
		if (pEvList->m_pEvQueue == NULL)
		{
			// we can ExFreePool for this event
			KeDestroyEvent(&pEventElem->m_KEvent);
			ExFreePool(pEventElem);	//начхать на память?
		}
	}

	InterlockedDecrement(&gReplyWaiterEnters);
	return Verdict;
}

NTSTATUS
QueueRequestEvent(PQCLIENT pClient, PEVENT_OBJECT_REQUEST pInRequest, PVOID pOutRequest, ULONG OutputBufferLength, 
				  ULONG* pOutDataLen)
{
	NTSTATUS StatusRet = STATUS_NOT_FOUND;
	PQELEM Curr;
	BOOLEAN bExit = FALSE;
	PEVENT_LIST pEvList = &pClient->m_EventList;
	
	DbPrint(DC_EVENT, DL_NOTIFY, ("Request %d on mark %#x with buffer len = %d\n", pInRequest->m_RequestType, 
		pInRequest->m_Mark, pInRequest->m_RequestBufferSize));
	
	if (pInRequest->m_RequestType == _event_request_queue_set_max_len)
	{
		pClient->m_DefaultVerdict = pInRequest->m_Verdict;
		pEvList->m_MaxQueueLen = pInRequest->m_Offset;

		return STATUS_SUCCESS;
	}
		
	AcquireResource(pEvList->m_pEventMutex, TRUE);
	
	if (pEvList->m_pEvQueue_FirstUnmarked == NULL)
		Curr = pEvList->m_pEvQueue;
	else
		Curr = pEvList->m_pEvQueue_FirstUnmarked;
	
	while((Curr != NULL) && (bExit == FALSE))
	{
		if((Curr->m_Mark == pInRequest->m_Mark) && (Curr->m_MarkerProcID == (ULONG) PsGetCurrentProcessId()))
		{
			StatusRet = STATUS_UNSUCCESSFUL;
			switch (pInRequest->m_RequestType)
			{
			case _event_request_set_preverdict:
				if (pInRequest->m_Verdict > Verdict_Kill)
				{
					StatusRet = STATUS_INVALID_PARAMETER;
					DbPrint(DC_EVENT, DL_WARNING, ("Request on mark %#x - not allowed Verdict %d\n", 
						pInRequest->m_Mark, pInRequest->m_Verdict));
					DbgBreakPoint();
				}
				else
				{
					Curr->m_Verdict = pInRequest->m_Verdict;
					DbPrint(DC_EVENT, DL_NOTIFY, ("Request on mark %#x - PreVerdict %d\n", pInRequest->m_Mark, 
						pInRequest->m_Verdict));
					StatusRet = STATUS_SUCCESS;
				}
				bExit = TRUE;
				break;
			case _event_delsame_in_queue_by_cacheid:
				{
					if (Curr->m_dwCacheValue == _INVALID_VALUE)
					{
						DbPrint(DC_EVENT,DL_WARNING, ("Request on mark %#x - del same unmarked by invalid CahceID\n", 
							Curr->m_Mark));
						DbgBreakPoint();
					}
					else
					{
						ULONG deleted = 0;
						PQELEM pCurrElem;
						PQELEM pCurrElemTmp;
						
						DbPrint(DC_EVENT,DL_INFO, ("Request on mark %#x - del same unmarked by CacheID %#x\n", 
							Curr->m_Mark, Curr->m_dwCacheValue));
						
						pCurrElem = pEvList->m_pEvQueue_FirstUnmarked;
						while (pCurrElem != NULL)
						{
							pCurrElemTmp = pCurrElem->m_pPrev;
							if (pCurrElem->m_dwCacheValue == Curr->m_dwCacheValue)
							{
								DelEventInQueue(pEvList, pCurrElem, FALSE);
								ExFreePool(pCurrElem);
								deleted++;

							}
							pCurrElem = pCurrElemTmp;
						}
						DbPrint(DC_EVENT,DL_INFO, ("Request on mark %#x - events deleted %d\n", Curr->m_Mark, deleted));
					}
				}
				break;
			case _event_update_cache:
				{
					DWORD dwNewCacheID = _INVALID_VALUE;
					if (pInRequest->m_RequestBufferSize == 0)
					{
						DbPrint(DC_EVENT,DL_IMPORTANT, ("Request on mark %#x - incorrect buffer size (in %d)\n", 
							pInRequest->m_Mark, pInRequest->m_RequestBufferSize));
					}
					else
					{
						dwNewCacheID = CalcCacheID((ULONG) pInRequest->m_RequestBufferSize, 
							(BYTE*) pInRequest->m_Buffer, dwNewCacheID);
						Obj_CacheUpdate(pClient, *(DWORD*) pInRequest->m_Buffer, pInRequest->m_Verdict, 
							(pInRequest->m_Flags & EVENT_OBJECT_REQUEST_FLAG_TIMEOUT) != 0 ? pInRequest->m_Timeout : 0);
						StatusRet = STATUS_SUCCESS;
					}
					bExit = TRUE;
				}
				break;
			case _event_request_write:
			case _event_request_get_size:
			case _event_request_read:
			case _event_request_get_param:
			case _event_request_get_granularity:
				{
					if (Curr->m_EventFlags & _EVENT_FLAG_OBJECT_ATTENDED)
					{
						if (Curr->m_pEventObject->m_pCallback != NULL)
							StatusRet = Curr->m_pEventObject->m_pCallback(pInRequest, Curr->m_pEventObject, 
							pOutRequest, OutputBufferLength, pOutDataLen);
					}
					bExit = TRUE;
				}
				break;
			case _event_request_get_param_size:
				{
					if (Curr->m_EventFlags & _EVENT_FLAG_OBJECT_ATTENDED)
					{
						if (Curr->m_pEventObject->m_pCallback != NULL)
							StatusRet = Curr->m_pEventObject->m_pCallback(pInRequest, Curr->m_pEventObject, 
							pOutRequest, OutputBufferLength, pOutDataLen);
					}
					else
					{
						PSINGLE_PARAM pParam = GetEventSingleParamPtr(Curr, pInRequest->m_Offset);
						if ((pParam == NULL) || (pOutRequest == NULL) || (OutputBufferLength < sizeof(ULONG)))
							StatusRet = STATUS_UNSUCCESSFUL;
						else
						{
							*(ULONG*)pOutRequest = pParam->ParamSize;
							*pOutDataLen = sizeof(ULONG);
							StatusRet = STATUS_SUCCESS;
						}
					}
					bExit = TRUE;
				}
				break;
			case _event_request_check:
				{
					if (Curr->m_EventFlags & _EVENT_FLAG_OBJECT_ATTENDED)
					{
						StatusRet = STATUS_SUCCESS;
						DbPrint(DC_EVENT, DL_INFO, ("Request %d on mark %#x object exist\n",
							pInRequest->m_RequestType, pInRequest->m_Mark));
					}
					else
					{
						StatusRet = STATUS_NOT_FOUND;
						DbPrint(DC_EVENT, DL_INFO, ("Request %d on mark %#x object not found!\n",
							pInRequest->m_RequestType, pInRequest->m_Mark));
					}
					bExit = TRUE;
				}
				break;
			case _event_request_get_thread_id:
				if ((OutputBufferLength >= sizeof(DWORD)) && pOutRequest != 0) 
				{
					*(DWORD*) pOutRequest = Curr->m_ThreadID;
					StatusRet = STATUS_SUCCESS;
				}
				bExit = TRUE;
				break;
			default:
				break;
			}
		}
		Curr = Curr->m_pNext;
	}
	ReleaseResource(pEvList->m_pEventMutex);			

	return StatusRet;
}

NTSTATUS
DoEventRequest(PEVENT_OBJECT_REQUEST pInRequest, PVOID pOutRequest, ULONG OutRequestSize, DWORD* pOutDataLen)
{
	NTSTATUS Status = STATUS_NOT_FOUND;
	PQCLIENT pClient = AcquireClient(pInRequest->m_AppID);
	if (pClient != NULL)
	{
		Status = QueueRequestEvent(pClient, pInRequest, pOutRequest, OutRequestSize, pOutDataLen);
		ReleaseClient(pClient);
	}
	else
		DbPrint(DC_EVENT,DL_WARNING, ("IOCTLHOOK_RequestEvent - client not found (%d)!\n", pInRequest->m_AppID));

	return Status;
}


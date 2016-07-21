#include "filter.h"

#include "debug.h"
#include "client.h"
#include "InvThread.h"

#ifdef _HOOK_NT_
//#include "stdio.h"
_CRTIMP int __cdecl swprintf(wchar_t *, const wchar_t *, ...);
_CRTIMP int __cdecl sprintf(char *, const char *, ...);
#endif

#include "PPage.c"

#include "nt/runproc.h"

typedef enum _teQUEUE_VAL {
	_QUEUE_TOP,
		_QUEUE_BOTTOM,
		_QUEUE_POSITION,
}eQUEUE_VAL;

typedef struct _QFLT
{
	struct _QFLT*	m_pNext;
	ULONG			m_AppID;
	ULONG			m_FilterID;
	PROCESSING_TYPE	m_ProcessingType;
	CHAR			m_pProcName[PROCNAMELEN];
	ULONG			m_Flags;
	ULONG			m_Timeout;
	__int64			m_Expiration;

	ULONG			m_GroupID;
	ULONG			m_GroupCounter;

	ULONG			m_ParamsCount;
	BYTE			m_Params[0];		// FILTER_PARAM 
}QFLT, *PQFLT;

PPOINTERPAGE pPageFilters = NULL;

ULONG	g_FilterID = _INVALID_FILTER_ID;
ULONG	gFiltersCount[HOOK_MAX_ID];
ULONG	gFilterEventEntersCount = 0;

PERESOURCE	g_pFilterMutex;

#define _GROUP_COUNTER_MAX		128

QSUBCLIENT gNonPopupClient;

#ifdef _HOOK_NT_
	#include "lists.h"

	typedef struct _DI_FILTER
	{
		IMPLIST_ENTRY	m_List;

		ULONG			m_HookID;
		ULONG			m_FunctionMj;
		ULONG			m_FunctionMi;
		ULONG			m_AppID;
		BOOLEAN			m_bClientPaused;
		ULONG			m_FilterID;
		ULONG			m_Flags;
		ULONG			m_ParamsCount;
		BYTE			m_Params[0];		// FILTER_PARAM 
	}DI_FILTER, *PDI_FILTER;

	KSPIN_LOCK		gDi_SpinLock;
	IMPLIST_ENTRY	gDi_FltList;
	LONG			gDi_Allocs = 0;

	VERDICT Di_FilterEvent(PFILTER_EVENT_PARAM pEventParam, PEVENT_OBJECT_INFO pEventObject);
	BOOLEAN Di_FltCheckParams(PFILTER_TRANSMIT pFltIn);
	PDI_FILTER Di_AllocateFilter(PFILTER_TRANSMIT pFltIn);
	void Di_AddFilter(PDI_FILTER pDiFilter, eQUEUE_VAL eSite, ULONG FilterID);
	void Di_DelFilter(FilterID);
	void Di_FreeFlt(PDI_FILTER pDiFilter);

	void Di_FilterSwitch(ULONG AppID, ULONG FilterID, BOOLEAN bEnable);

	void Di_ResetFiltersForApp(ULONG AppID);
#else
	#define PDI_FILTER PVOID
	#define Di_FilterEvent() Verdict_NotFiltered
	#define Di_FltCheckParams 
	#define Di_AllocateFilter
	#define Di_AddFilter
	#define Di_DelFilter
	#define Di_FreeFlt
	#define Di_ResetFiltersForApp
	#define Di_FilterSwitch
#endif

#ifdef __DBG__
ULONG FltOpArr[256];		// size must be greater then FLT_PARAM_OPERATION items count
ULONG FltOpArrRes[512];
ULONG FltDrvArr[64];
ULONG gFltRequests = 0;
ULONG gFltRequestsDispatch = 0;

void
DumpFilter (
	ULONG FilterId, 
	ULONG AppID, 
	ULONG FunctionMj, 
	ULONG FunctionMi,
	ULONG ParamsCount,
	PFILTER_PARAM pFilterParam
	)
{
	ULONG cou;
	ULONG cou2;
	BYTE *pPointer;

	DbgBreakPoint();
	DbPrint(DC_FILTER, DL_IMPORTANT, ("FltId 0x%x (0x%x- 0x%x, 0x%x)\n", FilterId,
		AppID, FunctionMj, FunctionMi, ParamsCount));

	for (cou = 0; cou < ParamsCount; cou++)
	{
		if ((pFilterParam->m_ParamFlt != FLT_PARAM_NOP))
		{
			DbPrint(DC_FILTER, DL_IMPORTANT, ("\tID 0x%x, Flg 0x%x, Op 0x%x, Size %d\n", 
				pFilterParam->m_ParamID,
				pFilterParam->m_ParamFlags,
				pFilterParam->m_ParamFlt,
				pFilterParam->m_ParamSize));


			if (pFilterParam->m_ParamSize)
			{
				DbPrint(DC_FILTER, DL_IMPORTANT, ("\n\t\t0x"));
				for (cou2 = 0; cou2 < pFilterParam->m_ParamSize; cou2++)
				{
					DbPrint(DC_FILTER, DL_IMPORTANT, ("%02x", pFilterParam->m_ParamValue[cou2]));
				}

				DbPrint(DC_FILTER, DL_IMPORTANT, ("\n"));
			}
		}
		
		pPointer = (BYTE*)pFilterParam + sizeof(FILTER_PARAM) + pFilterParam->m_ParamSize;
		pFilterParam = (PFILTER_PARAM) pPointer;
	}

	DbgBreakPoint();
}

#endif

extern void KlickRefreshData();

BOOLEAN
RequestHook(DWORD HookID)
{
	BOOLEAN bRet = TRUE;
	
	if (HookID == FLTTYPE_EMPTY || HookID > HOOK_MAX_ID)
		return FALSE;
	
	if (gFiltersCount[HookID - 1] == 0)
	{
		if (DrvRequestHook(HookID) == FALSE)
			bRet = FALSE;
	}
	
	if (FLTTYPE_KLICK == HookID)
		KlickRefreshData();
	
	if (bRet == TRUE)
		gFiltersCount[HookID - 1]++;
	
	return bRet;
}

void
RequestUnhook(DWORD HookID)
{
	if (FLTTYPE_KLICK == HookID)
		KlickRefreshData();

	if (gFiltersCount[HookID - 1] > 0)
		gFiltersCount[HookID - 1]--;
	
	if (gFiltersCount[HookID - 1] == 0)
	{
		DbPrint(DC_FILTER,DL_NOTIFY, ("Request unhook %d...\n", HookID));
		DrvRequestUnhook(HookID);
		DbPrint(DC_FILTER,DL_NOTIFY, ("Request unhook %d complete\n", HookID));
	}
}

__inline BOOLEAN
IsParameterUnicode(DWORD ParamID)
{
	if ((ParamID == _PARAM_OBJECT_URL_W) || (ParamID == _PARAM_OBJECT_URL_DEST_W))
		return TRUE;
	
	return FALSE;
}

void
FreeFiltersPages(PPOINTERPAGE pPageRoot)
{
	PPOINTERPAGE pPage;
	PPOINTERPAGE pPageTmp;
	DWORD cou;
	PSINGLEPAGE pSinglePage;
	
	PQFLT pQFlt;
	PQFLT pQFltTmp;
	
	if (pPageRoot == NULL)
		pPageRoot = pPageFilters;
	
	pPage = pPageRoot;
	while (pPage != NULL)
	{
		for (cou = 0; cou < pPage->CurrentPos; cou++)
		{
			pSinglePage = &pPage->Page[cou];
			if (pSinglePage->pNextLevel != NULL)
				FreeFiltersPages(pSinglePage->pNextLevel);
		}
		pPageTmp = pPage;
		
		pQFlt = (PQFLT) pSinglePage->pData;
		while (pQFlt != NULL)
		{
			pQFltTmp = pQFlt->m_pNext;
			ExFreePool(pQFlt);
			pQFlt = pQFltTmp;
		}
		
		pPage = (PPOINTERPAGE) pPage->pNext;
		ExFreePool(pPageTmp);
	}
}

void
GlobalFiltersInit(void)
{
	DWORD cou;
	for (cou = 0; cou < HOOK_MAX_ID; cou++)
		gFiltersCount[cou] = 0;
	
#ifdef _HOOK_NT_
	KeInitializeSpinLock(&gDi_SpinLock);
	_impInitializeListHead(&gDi_FltList);
#endif

	g_pFilterMutex = InitKlgResourse();
	if (g_pFilterMutex == NULL)
	{
		DbPrint(DC_FILTER, DL_ERROR, ("Can't init g_pFilterMutex\n"));
		return;
	}


	DbPrint(DC_FILTER, DL_IMPORTANT, ("GlobalFiltersInit\n"));
	
	gNonPopupClient.m_ActivityState = _INTERNAL_APP_STATE_ACTIVE | _INTERNAL_APP_STATE_DEAD;
	gNonPopupClient.m_ActivityWatchDog = DEADLOCKWDOG_TIMEOUT;
	gNonPopupClient.m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;
	gNonPopupClient.m_pNext = 0;
	gNonPopupClient.m_ProcessId = 0;
	gNonPopupClient.m_pWhistlerChanged = NULL;
	gNonPopupClient.m_pWhistleUp = NULL;
	gNonPopupClient.m_Tasks = 0;
	gNonPopupClient.m_ThreadId = 0;
	
#ifdef __DBG__
	for (cou = 0; cou < sizeof(FltOpArr) / sizeof(FltOpArr[0]); cou++)
		FltOpArr[cou] = 0;
	for (cou = 0; cou < sizeof(FltOpArrRes) / sizeof(FltOpArrRes[0]); cou++)
		FltOpArrRes[cou] = 0;

	for (cou = 0; cou < sizeof(FltDrvArr) / sizeof(FltDrvArr[0]); cou++)
		FltDrvArr[cou] = 0;
#endif
}

void
GlobalFiltersDone(void)
{
	DbPrint(DC_FILTER, DL_IMPORTANT, ("GlobalFiltersDone\n"));
	FreeKlgResourse(&g_pFilterMutex);
	
	FreeFiltersPages(pPageFilters);
	pPageFilters = NULL;
}

__inline BOOLEAN
MatchOkayW(PWCHAR Pattern)
{
	if(*Pattern && *Pattern!=L'*')
		return FALSE;
	return TRUE;
}

__inline BOOLEAN MatchOkayA(PCHAR Pattern)
{
	if(*Pattern && *Pattern!='*')
		return FALSE;
	return TRUE;
}

#define ucase(_wc) (_wc >= 'a' ? (_wc <= 'z' ? _wc & ~0x20 : _wc) : _wc)

__inline BOOLEAN
MatchWithPatternW(PWCHAR Pattern, PWCHAR Name, BOOLEAN CaseSensivity)
{
	WCHAR ch;

	if(*Pattern==L'*')
	{
		Pattern++;
		while(*Name && *Pattern)
		{
			if (CaseSensivity == FALSE)
			{
				ch = *Name;
				if (ch > 0x80)
					ch = _UpperCharW(ch);
				else
					ch = ucase(ch);
			}
			else
				ch = *Name;
			if ((ch == L'*') || (*Pattern == ch))
			{
				if(MatchWithPatternW(Pattern+1,Name+1, CaseSensivity)) 
					return TRUE;
			}
			
			Name++;
		}
		return MatchOkayW(Pattern);
	} 
	
	while(*Name && *Pattern != L'*')
	{
		if (CaseSensivity == TRUE)
			ch = *Name;
		else
		{
			ch = *Name;
			if (ch > 0x80)
				ch = _UpperCharW(ch);
			else
				ch = ucase(ch);
		}
		
		if(*Pattern == ch)
		{
			Pattern++;
			Name++;
		} else
			return FALSE;
	}
	if(*Name)
		return MatchWithPatternW(Pattern,Name, CaseSensivity);
	
	return MatchOkayW(Pattern);
}

__inline BOOLEAN
MatchWithPatternA(PCHAR Pattern, PCHAR Name, BOOLEAN CaseSensivity)
{
	CHAR ch;
	
	if(*Pattern=='*')
	{
		Pattern++;
		while(*Name && *Pattern)
		{
			if (CaseSensivity == FALSE)
				ch = _UpperCharA(*Name);
			else
				ch = *Name;
			if ((ch == '*') || (*Pattern == ch))
			{
				if(MatchWithPatternA(Pattern+1,Name+1, CaseSensivity)) 
					return TRUE;
			}
			
			Name++;
		}
		return MatchOkayA(Pattern);
	} 
	
	while(*Name && *Pattern != '*')
	{
		if (CaseSensivity == TRUE)
			ch = *Name;
		else
			ch = _UpperCharA(*Name);
		
		if(*Pattern == ch)
		{
			Pattern++;
			Name++;
		} else
			return FALSE;
	}
	if(*Name)
		return MatchWithPatternA(Pattern,Name, CaseSensivity);
	
	return MatchOkayA(Pattern);
}

__inline BOOLEAN
MatchWithPattern(PCHAR Pattern, PCHAR Name, BOOLEAN CaseSensivity, DWORD ParamID)
{
	if (IsParameterUnicode(ParamID) == TRUE)
	{
		PWCHAR pwchPattern = (PWCHAR) Pattern;
		DbPrint(DC_PARAMS, DL_SPAM, ("check ustring (sens %d) pattern '%S' with '%S'\n", CaseSensivity, 
			pwchPattern, (PWCHAR) Name));
		if (*pwchPattern == 0)
		{
			DbgBreakPoint();
			return TRUE;
		}
		return MatchWithPatternW(pwchPattern, (PWCHAR) Name, CaseSensivity);
	}
	
	return MatchWithPatternA(Pattern, Name, CaseSensivity);
}

DWORD
Filter_GetHint (
	PFILTER_EVENT_PARAM pEventParam )
{
	ULONG Hint = pEventParam->ProcessID;
	DWORD cou;

	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) pEventParam->Params;

	for (cou =  0; cou < pEventParam->ParamsCount; cou++)
	{
		if (pSingleParam->ParamFlags & _SINGLE_PARAM_FLAG_INFORMATION)
			continue;
		
		Hint = CalcCacheID(pSingleParam->ParamSize, pSingleParam->ParamValue, Hint);

		pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
	}

	return Hint;
}

// Usage -----------------------------------------------------------------------------------
void
_FreeFiltersForApp(PPOINTERPAGE pPageRoot, PQCLIENT pClient, DWORD HookID, BOOLEAN bRoot)
{
	PPOINTERPAGE pPage;
	PSINGLEPAGE pSinglePage;
	DWORD cou;
	PQFLT pQFlt;
	PQFLT pQFltTmp;
	PQFLT pQFltPrev;
	
	pPage = pPageRoot;
	while (pPage != NULL)
	{
		for (cou = 0; cou < pPage->CurrentPos; cou++)
		{
			pSinglePage = &pPage->Page[cou];
			if (pSinglePage->pNextLevel != NULL)
			{
				if (bRoot == TRUE)
					HookID = pSinglePage->dwData;
				_FreeFiltersForApp(pSinglePage->pNextLevel, pClient, HookID, FALSE);
			}
			
			pQFlt = (PQFLT) pSinglePage->pData;
			pQFltPrev = pQFlt;
			
			while (pQFlt != NULL)
			{
				if (pQFlt->m_AppID == pClient->m_AppID)
				{
					// clear
					pClient->m_FiltersCount--;
					if (pSinglePage->pData == pQFlt)
					{
						pSinglePage->pData = pQFlt->m_pNext;
						pQFltTmp = pSinglePage->pData;
					}
					else
					{
						pQFltPrev->m_pNext = pQFlt->m_pNext;
						pQFltTmp = pQFlt->m_pNext;
					}
					DbPrint(DC_FILTER,DL_INFO, ("Free filter %d\n", pQFlt->m_FilterID));
					ExFreePool(pQFlt);
					pQFlt = pQFltTmp;
					
					RequestUnhook(HookID);
				}
				else
				{
					pQFltPrev = pQFlt;
					pQFlt = pQFlt->m_pNext;
				}
			}
		}
		pPage = (PPOINTERPAGE) pPage->pNext;
	}
}

void
_FreeAppFilters(PQCLIENT pClient)
{
	Di_ResetFiltersForApp(pClient->m_AppID);

	DbPrint(DC_FILTER, DL_IMPORTANT, ("Client %d free filter - before lock\n", pClient->m_AppID));
	AcquireResource(g_pFilterMutex, TRUE);
	
	DbPrint(DC_FILTER,DL_INFO, ("Client %d free filter start...\n", pClient->m_AppID));
	
	_FreeFiltersForApp(pPageFilters, pClient, FLTTYPE_EMPTY, TRUE);
	
	DbPrint(DC_FILTER,DL_INFO, ("Client %d free filter end\n", pClient->m_AppID));
	
	ReleaseResource(g_pFilterMutex);
	DbPrint(DC_FILTER, DL_IMPORTANT, ("Client %d free filter - after lock\n", pClient->m_AppID));
}

#ifdef _HOOK_NT_
void
PrepareDiFilterParams (
	PDI_FILTER pDiFilter )
{
	PFILTER_PARAM pFilterParam = (PFILTER_PARAM) pDiFilter->m_Params;
	ULONG cou;
	BYTE *pPointer;

	for (cou = 0; (cou < pDiFilter->m_ParamsCount); cou++)
	{
		pFilterParam->m_ParamFlags &= ~_FILTER_PARAM_FLAG_CACHABLE;
		
		pPointer = (BYTE*)pFilterParam + sizeof(FILTER_PARAM) + pFilterParam->m_ParamSize;
		pFilterParam = (PFILTER_PARAM) pPointer;
	}
}
#endif

void
PrepareFilterParams(PQFLT pQFlt)
{
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD tmp;
	DWORD cou;
	int paramcou;
	CHAR ch;
	
	BOOLEAN bHaveMappedParams = FALSE;
	
	_UpperStringZeroTerninatedA(pQFlt->m_pProcName, pQFlt->m_pProcName);

	if (pQFlt->m_pProcName[0] == '*' && (pQFlt->m_pProcName[1]) == 0)
	{
		pQFlt->m_pProcName[0] = 0;
		pQFlt->m_pProcName[1] = '*'; // optimizer
	}
	
	pParam = (PFILTER_PARAM) pQFlt->m_Params;
	
	for (cou = 0; cou < pQFlt->m_ParamsCount; cou++)
	{
		// prepare param
		pParam->m_ParamFlags &= ~_FILTER_PARAM_INTERNAL_OPTIMIZER1;
		
		if (pParam->m_ParamFlags & _FILTER_PARAM_FLAG_MAP_TO_EVENT)
			bHaveMappedParams = TRUE;
		switch (pParam->m_ParamFlt)
		{
		case FLT_PARAM_WILDCARD:
		case FLT_PARAM_MASKUNSENS_LIST:
		case FLT_PARAM_MASKUNSENS_LISTW:
			{
				// to upper str
				int tmplen;
				int dwLastLen = pParam->m_ParamSize;
				pPointer = pParam->m_ParamValue;
				
				while (dwLastLen > 0)
				{
					if ((FLT_PARAM_MASKUNSENS_LISTW == pParam->m_ParamFlt) || IsParameterUnicode(pParam->m_ParamID))
					{
						_UpperStringZeroTerninatedW((PWCHAR) pPointer, (PWCHAR) pPointer);
						tmplen = (wcslen((PWCHAR) pPointer) + 1) * sizeof(WCHAR);
						if (tmplen == 2)
							dwLastLen = 0;
						else
						{
							pPointer += tmplen;
							dwLastLen -= tmplen;
						}
					}
					else
					{
						PCHAR pch = pPointer;
						tmplen = strlen(pPointer);
						for (paramcou = 0; paramcou < tmplen; paramcou++)
						{
							ch = _UpperCharA(*pch);
							*pch = ch;
							pch++;
						}
						
						tmplen = tmplen + 1;
						if (tmplen == 1)
							dwLastLen = 0;	
						else
						{
							pPointer += tmplen;
							dwLastLen -= tmplen;
						}
					}
				}
			}
			break;
		default:
			break;
		}
		// end prepare
		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		pPointer = (BYTE*)pParam + tmp;
		pParam = (PFILTER_PARAM) pPointer;
	}
	
	if (bHaveMappedParams == TRUE)
		pQFlt->m_Flags |= FLT_A_HAVE_MAPPED_PARAMS;
	else
		pQFlt->m_Flags &= ~FLT_A_HAVE_MAPPED_PARAMS;
}

PFILTER_PARAM
GetParamPtrFromFilter(PQFLT pQFlt, ULONG ParamID)
{
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD tmp;
	DWORD cou;
	pParam = (PFILTER_PARAM) pQFlt->m_Params;
	for (cou = 0; cou < pQFlt->m_ParamsCount; cou++)
	{
		if (pParam->m_ParamID == ParamID)
			return pParam;

		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		pPointer = (BYTE*)pParam + tmp;
		pParam = (PFILTER_PARAM) pPointer;
	}

	return NULL;
}

DWORD
CalcFltParamSize(PQFLT pQFlt, DWORD* pParamSize)
{
	DWORD FltSize;
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD tmp;
	DWORD cou;
#ifdef __DBG__
	PCHAR pch;
#endif
	FltSize = sizeof(QFLT);
	*pParamSize = 0;
	pParam = (PFILTER_PARAM) pQFlt->m_Params;
	for (cou = 0; cou < pQFlt->m_ParamsCount; cou++)
	{
		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		FltSize += tmp;
		*pParamSize += tmp;
		pPointer = (BYTE*)pParam + tmp;
#ifdef __DBG__
		pch = pParam->m_ParamValue;
#endif
		pParam = (PFILTER_PARAM) pPointer;
	}
	return FltSize;
}

DWORD CalcMappedParamSize(PQFLT pQFlt, DWORD* pParamCount)
{
	DWORD MappedParamSize;
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD cou;
	
	MappedParamSize = 0;
	*pParamCount = 0;
	
	
	pParam = (PFILTER_PARAM) pQFlt->m_Params;
	for (cou = 0; cou < pQFlt->m_ParamsCount; cou++)
	{
		if (pParam->m_ParamFlags & _FILTER_PARAM_FLAG_MAP_TO_EVENT)
		{
			(*pParamCount)++;
			MappedParamSize += sizeof(SINGLE_PARAM) + pParam->m_ParamSize;
		}
		
		pPointer = (BYTE*)pParam + sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		pParam = (PFILTER_PARAM) pPointer;
	}
	
	return MappedParamSize;
}

void CopyMappedParams(PMAPPED_PARAMS pMappedParams, PQFLT pQFlt)
{
	PFILTER_PARAM pParam;
	PSINGLE_PARAM pParamDest;
	BYTE *pPointer;
	DWORD tmp;
	DWORD cou;
	
	pParam = (PFILTER_PARAM) pQFlt->m_Params;
	pParamDest = (PSINGLE_PARAM) pMappedParams->m_SingleParams;
	
	for (cou = 0; cou < pQFlt->m_ParamsCount; cou++)
	{
		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		
		if (pParam->m_ParamFlags & _FILTER_PARAM_FLAG_MAP_TO_EVENT)
		{
			pParamDest->ParamID = pParam->m_ParamID;
			pParamDest->ParamSize = pParam->m_ParamSize;
			pParamDest->ParamFlags = _SINGLE_PARAM_FLAG_MAPPED_FILTER_PARAM;
			memcpy(pParamDest->ParamValue, pParam->m_ParamValue, pParam->m_ParamSize);
			
			pPointer = (BYTE*)pParamDest + sizeof(SINGLE_PARAM) + pParam->m_ParamSize;
			pParamDest = (PSINGLE_PARAM) pPointer;
		}
		
		pPointer = (BYTE*)pParam + tmp;
		pParam = (PFILTER_PARAM) pPointer;
	}
}

DWORD CalcFilterSize(PFILTER_TRANSMIT pFltIn)
{
	DWORD FilterSize;
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD tmp;
	DWORD cou;
#ifdef __DBG__
	PCHAR pch;
#endif
	FilterSize = sizeof(FILTER_TRANSMIT);
	pParam = (PFILTER_PARAM) pFltIn->m_Params;
	for (cou = 0; cou < pFltIn->m_ParamsCount; cou++)
	{
		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		FilterSize += tmp;
		pPointer = (BYTE*)pParam + tmp;
#ifdef __DBG__
		pch = pParam->m_ParamValue;
#endif
		pParam = (PFILTER_PARAM) pPointer;
	}
	return FilterSize;
}

PQFLT _FindFilter(ULONG AppID, ULONG FilterID, PPOINTERPAGE pPageRoot)
{
	PPOINTERPAGE pPage;
	PSINGLEPAGE pSinglePage;
	DWORD cou;
	PQFLT pQFlt = NULL;
	
	if (pPageRoot == NULL)
		pPageRoot = pPageFilters;
	
	pPage = pPageRoot;
	while (pPage != NULL)
	{
		for (cou = 0; cou < pPage->CurrentPos; cou++)
		{
			pSinglePage = &pPage->Page[cou];
			if (pSinglePage->pNextLevel != NULL)
				pQFlt = _FindFilter(AppID, FilterID, pSinglePage->pNextLevel);
			
			if (pQFlt)
				return pQFlt;

			pQFlt = (PQFLT) pSinglePage->pData;
			
			while (pQFlt != NULL)
			{
				if ((pQFlt->m_AppID == AppID) && (pQFlt->m_FilterID == FilterID))
					return pQFlt;

				pQFlt = pQFlt->m_pNext;
			}
		}
		pPage = (PPOINTERPAGE) pPage->pNext;
	}
	
	return NULL;
}

BOOLEAN
CheckEventParams (
	PFILTER_EVENT_PARAM pEvent,
	ULONG EventSize
	)
{
	PSINGLE_PARAM pSingleParam = NULL;
	DWORD cou;

	if (EventSize < sizeof(FILTER_EVENT_PARAM))
		return FALSE;

	EventSize -= sizeof(FILTER_EVENT_PARAM);

	pSingleParam = (PSINGLE_PARAM) pEvent->Params;
	for (cou =  0; cou < pEvent->ParamsCount; cou++)
	{
		if (EventSize < pSingleParam->ParamSize + sizeof(SINGLE_PARAM))
			return FALSE;

		EventSize -= (pSingleParam->ParamSize + sizeof(SINGLE_PARAM));
		pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
	}

	return TRUE;
}

BOOLEAN _FilterSwitch(ULONG AppID, ULONG FilterID, BOOLEAN bEnable)
{
	PQFLT pQFlt;
	
	AcquireResource(g_pFilterMutex, TRUE);

	pQFlt = _FindFilter(AppID, FilterID, NULL);
	
	if (pQFlt)
	{
		if (bEnable)
		{
			DbPrint(DC_FILTER,DL_IMPORTANT, ("Filter enabled\n"));
			pQFlt->m_Flags &= ~FLT_A_DISABLED;
		}
		else
		{
			DbPrint(DC_FILTER,DL_IMPORTANT, ("Filter disabled\n"));
			pQFlt->m_Flags |= FLT_A_DISABLED;
		}
	}
	
	ReleaseResource(g_pFilterMutex);

	return pQFlt ? TRUE : FALSE;
}

NTSTATUS _FilterChangeParams(ULONG AppID, ULONG FilterID, ULONG ParamsCount, PFILTER_PARAM pParams)
{
	NTSTATUS Status = STATUS_NOT_FOUND;
	PQFLT pQFlt;

	if (ParamsCount != 1)
		return STATUS_NOT_SUPPORTED;
	
	AcquireResource(g_pFilterMutex, TRUE);

	pQFlt = _FindFilter(AppID, FilterID, NULL);

	if (pQFlt)
	{
		PFILTER_PARAM pFltParam = GetParamPtrFromFilter(pQFlt, pParams->m_ParamID);
		if (!pFltParam)
			Status = STATUS_NOT_FOUND;
		else
		{
			if (pFltParam->m_ParamSize != pParams->m_ParamSize)
				Status = STATUS_NOT_SUPPORTED;
			else
			{
				memcpy(pFltParam, pParams, sizeof(FILTER_PARAM) + pParams->m_ParamSize);
				Status = STATUS_SUCCESS;	
			}
		}
	}
	
	ReleaseResource(g_pFilterMutex);

	return Status;
}

BOOLEAN _FilterSetGroupID(ULONG AppID, ULONG FilterID, ULONG GroupID)
{
	PQFLT pQFlt;
	
	AcquireResource(g_pFilterMutex, TRUE);

	pQFlt = _FindFilter(AppID, FilterID, NULL);
	
	if (pQFlt)
	{
		pQFlt->m_GroupID = GroupID;
	}
	
	ReleaseResource(g_pFilterMutex);

	return pQFlt ? TRUE : FALSE;
}
// -----------------------------------------------------------------------------------------
// Filters traversal
BOOLEAN
_GetNextFilter(ULONG AppID, ULONG* pStartFilterID, PFILTER_TRANSMIT pOutputFilter, ULONG OutputBufferLength, 
			   PPOINTERPAGE pPageRoot, NTSTATUS *pStatus)
{
	PPOINTERPAGE pPage;
	PSINGLEPAGE pSinglePage;
	DWORD cou;
	PQFLT pQFlt;
	BOOLEAN bFound = FALSE;
	BOOLEAN bRoot = FALSE;
	
	AcquireResource(g_pFilterMutex, FALSE);
	
	if (pPageRoot == NULL)
	{
		pPageRoot = pPageFilters;
		pOutputFilter->m_HookID = -1;
		pOutputFilter->m_FunctionMj = -1;
		pOutputFilter->m_FunctionMi = -1;
		
		bRoot = TRUE;
	}
	
	pPage = pPageRoot;
	while (pPage != NULL  && bFound == FALSE)
	{
		for (cou = 0; cou < pPage->CurrentPos && bFound == FALSE; cou++)
		{
			pSinglePage = &pPage->Page[cou];
			if (pSinglePage->pNextLevel != NULL)
				bFound = _GetNextFilter(AppID, pStartFilterID, pOutputFilter, OutputBufferLength, 
				pSinglePage->pNextLevel, pStatus);
			
			if (bFound == FALSE)
			{
				pQFlt = (PQFLT) pSinglePage->pData;
				
				while (pQFlt != NULL)
				{
					if (pQFlt->m_AppID == AppID)
					{
						if (*pStartFilterID == (ULONG) -1)
						{
							DWORD ParamSize;
							bFound = TRUE;
							
							CalcFltParamSize(pQFlt, &ParamSize);
							if ((ParamSize + sizeof(FILTER_TRANSMIT)) > OutputBufferLength)
							{
								DbPrint(DC_FILTER,DL_WARNING, ("Can't get filter. Too small output buffer (Out = %d, need = %d\n", 
									OutputBufferLength, ParamSize + sizeof(FILTER_TRANSMIT)));
								*pStatus = STATUS_BUFFER_TOO_SMALL;
							}
							else
							{
								// copy filter data and return;
								pOutputFilter->m_AppID = pQFlt->m_AppID;
								pOutputFilter->m_ProcessingType = pQFlt->m_ProcessingType;
								pOutputFilter->m_Expiration = pQFlt->m_Expiration;
								pOutputFilter->m_FilterID = pQFlt->m_FilterID;
								pOutputFilter->m_Flags = pQFlt->m_Flags;
								pOutputFilter->m_FunctionMi = pSinglePage->dwData;
								pOutputFilter->m_ParamsCount = pQFlt->m_ParamsCount;
								strncpy(pOutputFilter->m_ProcName, pQFlt->m_pProcName, PROCNAMELEN);
								pOutputFilter->m_Timeout = pQFlt->m_Timeout;
								
								memcpy(pOutputFilter->m_Params , pQFlt->m_Params, ParamSize);

								if ((pOutputFilter->m_ProcName[0] == 0) && (pOutputFilter->m_ProcName[1] == '*'))
								{
									pOutputFilter->m_ProcName[0] = '*';
									pOutputFilter->m_ProcName[1] = 0;
								}
							}
							
							DbPrint(DC_FILTER,DL_INFO, ("Filter found id = %d\n", pQFlt->m_FilterID));
							ReleaseResource(g_pFilterMutex);
							
							return bFound;
						}
						if (pQFlt->m_FilterID == *pStartFilterID) 
							*pStartFilterID = -1;
					}
					pQFlt = pQFlt->m_pNext;
				}
			}
			else
			{
				if (bRoot == FALSE)
					pOutputFilter->m_FunctionMj = pSinglePage->dwData;
				else
					pOutputFilter->m_HookID = pSinglePage->dwData;
			}
		}
		pPage = (PPOINTERPAGE) pPage->pNext;
	}
	
	ReleaseResource(g_pFilterMutex);
	
	return bFound;
}

// -----------------------------------------------------------------------------------------
#pragma intrinsic(memcmp)

BOOLEAN 
CheckSingleParam(PFILTER_EVENT_PARAM pEventParam, PFILTER_PARAM pFilterParam, DWORD* pdwCacheValue, 
				 BOOLEAN* bpParamFound)
{
	BOOLEAN bMatch;
	PSINGLE_PARAM pEvParam;
	PSINGLE_PARAM pEvParamNext;
	BYTE *pPointer;
	DWORD cou;
	
	bMatch = TRUE;
	*bpParamFound = FALSE;
	
	pEvParamNext = (PSINGLE_PARAM) pEventParam->Params;
	for (cou = 0; cou < pEventParam->ParamsCount && bMatch == TRUE; cou++)
	{
		pEvParam = pEvParamNext;
		
		pPointer = (BYTE*)pEvParam + sizeof(SINGLE_PARAM) + pEvParam->ParamSize;
		pEvParamNext = (PSINGLE_PARAM) pPointer;

		if (pEvParam->ParamID == pFilterParam->m_ParamID) 
		{
			*bpParamFound = TRUE;
#ifdef __DBG__
			if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_DEBUG_BREAK)
			{
				DbgBreakPoint();
			}
			FltOpArr[pFilterParam->m_ParamFlt]++;
#endif
			switch (pFilterParam->m_ParamFlt)
			{
			case FLT_PARAM_WILDCARD:
				{
					//compare wildcard
					if (FALSE == MatchWithPattern(pFilterParam->m_ParamValue, pEvParam->ParamValue, FALSE, 
						pFilterParam->m_ParamID))
						bMatch = FALSE;
				}
				break;
			case FLT_PARAM_NOP:
				break;
			case FLT_PARAM_AND:
				if (pEvParam->ParamSize != pFilterParam->m_ParamSize)
					bMatch = FALSE;
				else
				{
					BYTE b1, b2;
					DWORD tmpi;
					bMatch = FALSE;
					for (tmpi = 0; tmpi < (pFilterParam->m_ParamSize) && (bMatch != TRUE); tmpi++)
					{
						b1 = *(((BYTE*) pEvParam->ParamValue) + tmpi);
						b2 = *(((BYTE*) pFilterParam->m_ParamValue) + tmpi);
						if ((b1 & b2) != 0)
							bMatch = TRUE;
					}
				}
				break;
			case FLT_PARAM_EUA:
				if (pEvParam->ParamSize != pFilterParam->m_ParamSize)
					bMatch = FALSE;
				else
				{
					if (memcmp(pEvParam->ParamValue, pFilterParam->m_ParamValue, pFilterParam->m_ParamSize))
						bMatch = FALSE;
				}
				break;
			case FLT_PARAM_WILDCARDSENS:
				{
					//compare wildcard
					if (FALSE == MatchWithPattern(pFilterParam->m_ParamValue, pEvParam->ParamValue, TRUE, 
						pFilterParam->m_ParamID))
						bMatch = FALSE;
				}
				break;
			case FLT_PARAM_MASKUNSENS_LIST:
			case FLT_PARAM_MASKUNSENS_LISTW:
			case FLT_PARAM_MASK_LIST:
			case FLT_PARAM_MASKSENS_LISTW:
				{
					int tmplen;
					BYTE *p1 = pFilterParam->m_ParamValue;
					int dwLastLen = pFilterParam->m_ParamSize;

					int zerolen = sizeof(CHAR);
					BOOLEAN bUni = FALSE;
					BOOLEAN bSens = FALSE;

					bMatch = FALSE;

					if ((FLT_PARAM_MASK_LIST == pFilterParam->m_ParamFlt) || (FLT_PARAM_MASKSENS_LISTW == pFilterParam->m_ParamFlt))
						bSens = TRUE;
					
					if ((FLT_PARAM_MASKUNSENS_LISTW == pFilterParam->m_ParamFlt) || 
						(FLT_PARAM_MASKSENS_LISTW == pFilterParam->m_ParamFlt) || 
						IsParameterUnicode(pFilterParam->m_ParamID))
					{
						bUni = TRUE;
						zerolen = sizeof(WCHAR);
					}

					while ((dwLastLen > zerolen) && (bMatch == FALSE))
					{
						if (bUni)
						{
							bMatch = MatchWithPatternW((PWCHAR) p1, (PWCHAR)pEvParam->ParamValue, bSens);
							DbPrint(DC_PARAMS, DL_SPAM, ("check ustring parameter %x (%s) pattern '%S' with '%S'\n", 
								pFilterParam->m_ParamID, bMatch ? "ok" : "not matched", 
								p1, pEvParam->ParamValue));

						}
						else
							bMatch = MatchWithPatternA(p1, pEvParam->ParamValue, bSens);
						
						if (!bMatch)
						{
							if (bUni)
							{
								tmplen = (wcslen((PWCHAR) p1) + 1) * sizeof(WCHAR);
								dwLastLen -= tmplen;
								p1 += tmplen;
							}
							else
							{
								tmplen = strlen(p1) + 1;
								dwLastLen -= tmplen;
								p1 += tmplen;
							}
						}
					}
				}
				break;
			case FLT_PARAM_EQU_LIST:
				{
					BYTE *p1 = pFilterParam->m_ParamValue;
					DWORD dwLastLen = pFilterParam->m_ParamSize;
					bMatch = FALSE;
					while ((dwLastLen > 0) && (bMatch == FALSE))
					{
						if (dwLastLen < pEvParam->ParamSize)
							break;
						if (memcmp(pEvParam->ParamValue, p1, pEvParam->ParamSize) == 0)
							bMatch = TRUE;
						else
						{
							p1 += pEvParam->ParamSize;
							dwLastLen -= pEvParam->ParamSize;
						}
					}
#ifdef __DBG__
					if ((pFilterParam->m_ParamSize % pEvParam->ParamSize) != 0)
					{
						DbPrint(DC_FILTER,DL_WARNING, ("EQU_LIST operator for ParamID %d - size mismatch: event %d, param %d\n", 
							pFilterParam->m_ParamID, pEvParam->ParamSize, pFilterParam->m_ParamSize));
						DbgBreakPoint();
					}
#endif
				}
				break;
			case FLT_PARAM_ABV:
				if (pEvParam->ParamSize < pFilterParam->m_ParamSize)
					bMatch = FALSE;
				else
				{
					BYTE b1, b2;
					DWORD tmpi;
					for (tmpi = 0; tmpi < (pFilterParam->m_ParamSize) && (bMatch == TRUE); tmpi++)
					{
						b1 = *(((BYTE*) pEvParam->ParamValue) + tmpi);
						b2 = *(((BYTE*) pFilterParam->m_ParamValue) + tmpi);
						if (b1 < b2)
							bMatch = FALSE;
					}
				}
				break;
			case FLT_PARAM_BEL:
				if (pEvParam->ParamSize > pFilterParam->m_ParamSize)
					bMatch = FALSE;
				else {
					BYTE b1, b2;
					DWORD tmpi;
					for (tmpi = 0; tmpi < (pFilterParam->m_ParamSize) && (bMatch == TRUE); tmpi++)
					{
						b1 = *(((BYTE*) pEvParam->ParamValue) + tmpi);
						b2 = *(((BYTE*) pFilterParam->m_ParamValue) + tmpi);
						if (b1 > b2)
							bMatch = FALSE;
					}
				}
				break;
			case FLT_PARAM_MORE:
				if (pEvParam->ParamSize != pFilterParam->m_ParamSize)
				{
					DbPrint(DC_FILTER,DL_WARNING, ("Param size mismatch in FLT_PARAM_MORE for ParamID %d)!\n", 
						pFilterParam->m_ParamID));
					bMatch = FALSE;
				}
				else
				{
					BYTE b1, b2;
					int tmpi;
					for (tmpi = pFilterParam->m_ParamSize - 1; (tmpi >= 0) && (bMatch == TRUE); tmpi--)
					{
						b1 = *(((BYTE*) pEvParam->ParamValue) + tmpi);
						b2 = *(((BYTE*) pFilterParam->m_ParamValue) + tmpi);
						if (b1 > b2)
							bMatch = FALSE;
						else if (b1 < b2)
							break;
					}
				}
				break;
			case FLT_PARAM_LESS:
				if (pEvParam->ParamSize != pFilterParam->m_ParamSize)
				{
					DbPrint(DC_FILTER,DL_WARNING, ("Param size mismatch in FLT_PARAM_LESS for ParamID %d)!\n", 
						pFilterParam->m_ParamID));
					bMatch = FALSE;
				}
				else {
					BYTE b1, b2;
					int tmpi;
					for (tmpi = pFilterParam->m_ParamSize - 1; (tmpi >= 0) && (bMatch == TRUE); tmpi--)
					{
						b1 = *(((BYTE*) pEvParam->ParamValue) + tmpi);
						b2 = *(((BYTE*) pFilterParam->m_ParamValue) + tmpi);
						if (b1 < b2) 
							bMatch = FALSE;
						else if (b1 > b2) 
							break;
					}
				}
				break;
			case FLT_PARAM_MASK:
				//Filter param = value + mask; sizeof(value) = sizeof(mask); total param size = sizeof(value) + sizeof(mask)
				if (2*pEvParam->ParamSize != pFilterParam->m_ParamSize)
				{
					DbPrint(DC_FILTER,DL_WARNING, ("Param size mismatch in FLT_PARAM_MASK for ParamID %d)!\n", 
						pFilterParam->m_ParamID));
					bMatch = FALSE;
				}
				else
				{
					BYTE b1, b2, msk;
					DWORD tmpi;
					for (tmpi = 0; (tmpi < pEvParam->ParamSize) && (bMatch == TRUE); tmpi++)
					{
						b1 = *(((BYTE*) pEvParam->ParamValue) + tmpi);
						b2 = *(((BYTE*) pFilterParam->m_ParamValue) + tmpi);
						msk = *(((BYTE*) pFilterParam->m_ParamValue) + pEvParam->ParamSize + tmpi);
						if ((b1 & msk) != (b2 & msk))
							bMatch = FALSE;
					}
				}
				break;
#ifdef _HOOK_NT_				
			case FLT_PARAM_CHECK_SID:
				if (pEvParam->ParamSize != pFilterParam->m_ParamSize)
				{
					bMatch = FALSE;
					DbPrint(DC_FILTER, DL_WARNING, ("Unsupported sid length!\n"));
				}
				else
				{
					bMatch = RtlEqualSid(pEvParam->ParamValue, pFilterParam->m_ParamValue);
					if (!bMatch)
					{
						DbPrint(DC_FILTER, DL_WARNING, ("sid compare: event sid ptr 0x%x, filter sid ptr 0x%x\n", 
							pEvParam->ParamValue, pFilterParam->m_ParamValue));

						DbgBreakPoint();
					}
				}
				break;
#endif

			default:
				{
					bMatch = FALSE;
					DbPrint(DC_FILTER,DL_WARNING, ("Unsupported operation (%d)!\n", pFilterParam->m_ParamFlt));
				}
			}
			
			if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_INVERS_OP)
			{
				bMatch = !bMatch;
			}

#ifdef __DBG__
			if (bMatch)
				FltOpArrRes[pFilterParam->m_ParamFlt * 2]++;
			else
				FltOpArrRes[pFilterParam->m_ParamFlt * 2 + 1]++;
#endif
			
			if ((bMatch == TRUE) && (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_CACHABLE))
			{
				if (pdwCacheValue)
				{
					if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_CACHEUPREG)
					{
						*pdwCacheValue = CalcCacheIDUnSens(pEvParam->ParamSize, pEvParam->ParamValue, *pdwCacheValue, 
							IsParameterUnicode(pFilterParam->m_ParamID));
					}
					else
						*pdwCacheValue = CalcCacheID(pEvParam->ParamSize, pEvParam->ParamValue, *pdwCacheValue);
				}
			}
		}
	}
	
	if ((pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_CACHABLE) && 
		(pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_MAP_TO_EVENT))
	{
		if (pdwCacheValue)
		{
			DbPrint(DC_FILTER,DL_INFO, ("calc CacheID by mapped param\n"));
			*pdwCacheValue = CalcCacheID(pFilterParam->m_ParamSize, pFilterParam->m_ParamValue, *pdwCacheValue);
		}
	}
	
	return bMatch;
}

BOOLEAN
_AddFilter(PFILTER_TRANSMIT pFltIn, eQUEUE_VAL eSite)
{
	BOOLEAN bRet = FALSE;
	PSINGLEPAGE pSinglePage;
	PPOINTERPAGE* pPagesTmp;
	
	DWORD Funcs[3];
	DWORD cou;
	
	DWORD ParamSize;
	ULONG ClientPriority;
	PQFLT pQFlt;
	PQFLT pQFltMemory;
	PQCLIENT pClient;
	PDI_FILTER DiFilter = NULL;
	
	DbPrint(DC_FILTER, DL_NOTIFY, ("add new filter request: (Hook %#x Mj %#x Mi %#x) %s\n", 
		pFltIn->m_HookID, pFltIn->m_FunctionMj, pFltIn->m_FunctionMi,
		pFltIn->m_Flags & FLT_A_DISABLED ? "disabled" : ""));
	// -----------------------------------------------------------------------------------------
	// forbidden flags combination
	if ((pFltIn->m_Flags & FLT_A_POPUP) && (pFltIn->m_Flags & FLT_A_DELETE_ON_MARK))
	{
		DbPrint(DC_FILTER,DL_WARNING, ("forbidden flags combination\n", pFltIn->m_Flags));
		DbgBreakPoint();
		return bRet;
	}

	if (pFltIn->m_Flags & FLT_A_DISPATCH_MIRROR)
	{
		if (!Di_FltCheckParams(pFltIn))
		{
			DbPrint(DC_FILTER, DL_WARNING, ("wrong parameters for dispatch filter\n"));
			DbgBreakPoint();
			return bRet;
		}
	}
	
	if (RequestHook(pFltIn->m_HookID) == FALSE)
	{
		DbPrint(DC_FILTER,DL_WARNING, ("Driver can't set hook for %x\n", pFltIn->m_HookID));
		
		return bRet;
	}
	else
	{
		if (pFltIn->m_Flags & FLT_A_TOUCH)
		{
			if (RequestHook(pFltIn->m_HookID) == FALSE)
				return bRet;
		}
		
		Funcs[0] = pFltIn->m_HookID;
		Funcs[1] = pFltIn->m_FunctionMj;
		Funcs[2] = pFltIn->m_FunctionMi;
		
		pClient = AcquireClient(pFltIn->m_AppID);
		if (pClient == 0)
		{
			DbPrint(DC_FILTER,DL_WARNING, ("Driver can't find client %d during add filter\n", pFltIn->m_AppID));
			RequestUnhook(pFltIn->m_HookID);
			
			return bRet;
		}
		
		ParamSize = CalcFilterSize(pFltIn) - sizeof(FILTER_TRANSMIT);
		pQFltMemory = ExAllocatePoolWithTag(NonPagedPool, sizeof(QFLT) + ParamSize,'fboS');

		#ifdef _HOOK_NT_
		if (pQFltMemory && pFltIn->m_Flags & FLT_A_DISPATCH_MIRROR)
		{
			DiFilter = Di_AllocateFilter(pFltIn);
			if (!DiFilter)
			{
				DbPrint(DC_FILTER,DL_ERROR, ("Allocate memory for new dispatch filter failed (Param size = %d)!\n"));
		
				ExFreePool(pQFltMemory);
				pQFltMemory = NULL;
			}
			else
			{
				if (pClient->m_ClientFlags & _CLIENT_FLAG_PAUSED)
					DiFilter->m_bClientPaused = TRUE;
			}
		}
		#endif

		if (!pQFltMemory)
		{
			DbPrint(DC_FILTER,DL_ERROR, ("Allocate memory for new filter failed (Param size = %d)!\n", 
				ParamSize + sizeof(QFLT)));
		}
		else
		{
			pQFltMemory->m_AppID = pFltIn->m_AppID;
			pQFltMemory->m_Expiration = pFltIn->m_Expiration;
			pQFltMemory->m_ProcessingType =  pFltIn->m_ProcessingType;
			pQFltMemory->m_Flags = pFltIn->m_Flags;
			pQFltMemory->m_ParamsCount = pFltIn->m_ParamsCount;
			memcpy(pQFltMemory->m_Params, pFltIn->m_Params, ParamSize);
			strcpy(pQFltMemory->m_pProcName, pFltIn->m_ProcName);
			pQFltMemory->m_Timeout = pFltIn->m_Timeout;

			pQFltMemory->m_GroupID = INVALID_GROUP_ID;
			pQFltMemory->m_GroupCounter = 0;
			
			PrepareFilterParams(pQFltMemory);
			
			AcquireResource(g_pFilterMutex, TRUE);
			
			for (cou = 0; cou < 3; cou++)
			{
				if (cou == 0)
					pPagesTmp = &pPageFilters;
				else
					pPagesTmp = &(PPOINTERPAGE) pSinglePage->pNextLevel;
				
				pSinglePage = GetPData(pPagesTmp, Funcs[cou]);
				if (pSinglePage == NULL)
				{
					pSinglePage = AddNewData(pPagesTmp, Funcs[cou], NULL, NULL);
					if (pSinglePage == NULL)
					{
						DbPrint(DC_FILTER, DL_IMPORTANT, ("AddNewData failed!\n"));
						ReleaseResource(g_pFilterMutex);
						ReleaseClient(pClient);
						RequestUnhook(pFltIn->m_HookID);
						ExFreePool(pQFltMemory);

						Di_FreeFlt(DiFilter);
					
						return bRet;
					}
				}
				if (Funcs[cou] == -1)
					break;
			}
			
			// имеем pSinglePage->pData - очередь фильтров на событие
			ClientPriority = pClient->m_Priority;

			pQFlt = (PQFLT) pSinglePage->pData;
			
			pQFltMemory->m_pNext = NULL;
			pClient->m_FiltersCount++;
			
			if (pQFlt == NULL)
				pSinglePage->pData = pQFltMemory;				// add in root
			else
			{
				BOOLEAN bFound = FALSE;
				BOOLEAN bDefSite = FALSE;
				PQFLT pQFltPrev = NULL;
				
				while (TRUE)
				{
					// search place for filter - near existing filters
					if (pQFlt == NULL)
						break;
					
					if (pFltIn->m_AppID == pQFlt->m_AppID)
					{
						bFound = TRUE;

						switch (eSite)
						{
						case _QUEUE_TOP:
							// в начало очереди
							// автоматом
							break;
						case _QUEUE_POSITION:
							// найти позицию по pFltIn->m_FilterID и добавить после него
							while ((pQFlt != NULL) && (pFltIn->m_FilterID != pQFlt->m_FilterID) && 
								(pFltIn->m_AppID == pQFlt->m_AppID))
								pQFlt = pQFlt->m_pNext;
							pQFltPrev = pQFlt;
							break;
						case _QUEUE_BOTTOM:
						default:
							// промотать до конца фильтров данного приложения
							while ((pQFlt != NULL) && (pFltIn->m_AppID == pQFlt->m_AppID))
							{
								pQFltPrev = pQFlt;
								pQFlt = pQFlt->m_pNext;
							}
							break;
						}
						
						break;
					}
					else
					{
						pQFltPrev = pQFlt;
						pQFlt = pQFlt->m_pNext;
					}
				}

				if (!bFound)
				{
					// (by priority)
					pQFlt = (PQFLT) pSinglePage->pData;
					pQFltPrev = NULL;

					while (TRUE)
					{
						if (!pQFlt || ClientPriority > GetClientPriority(pQFlt->m_AppID))
						{
							break;
						}

						pQFltPrev = pQFlt;
						pQFlt = pQFlt->m_pNext;
					}
				}

				
				if (pQFltPrev == NULL)
				{
					pQFltMemory->m_pNext = pSinglePage->pData;
					pSinglePage->pData = pQFltMemory;
				}
				else
				{
					pQFltMemory->m_pNext = pQFltPrev->m_pNext;
					pQFltPrev->m_pNext = pQFltMemory;
				}
			}
			// copy data
			g_FilterID++;
			if (g_FilterID == _INVALID_FILTER_ID)
				g_FilterID++;
			pFltIn->m_FilterID = pQFltMemory->m_FilterID = g_FilterID;
			
			bRet = TRUE;
/*#ifdef __DBG__
			{
				PQFLT pQFltTmp = pSinglePage->pData;
				DbPrint(DC_FILTER, DL_IMPORTANT, ("fltlist: "));
				while(pQFltTmp)
				{
					DbgPrint(" %x", pQFltTmp->m_FilterID);
					pQFltTmp = pQFltTmp->m_pNext;
				}
				DbgPrint("\n");
			}
#endif*/
			
			ReleaseResource(g_pFilterMutex);

			//
			if (DiFilter)
				Di_AddFilter(DiFilter, eSite, pFltIn->m_FilterID);
		}
		
		ReleaseClient(pClient);
	}
	
	if (bRet == FALSE)
		RequestUnhook(pFltIn->m_HookID);
	
	return bRet;
}

// Delete filter
BOOLEAN
_DelFilter(ULONG FilterID, PPOINTERPAGE pPageRoot, BOOLEAN bRoot)
{
	BOOLEAN bFound = FALSE;
	
	PPOINTERPAGE pPage;
	PSINGLEPAGE pSinglePage;
	
	PQFLT pQFlt;
	PQFLT pQFltPrev;
	DWORD cou;
	
	ULONG AppID;

	Di_DelFilter(FilterID);
	
	AcquireResource(g_pFilterMutex, TRUE);
	
	if (pPageRoot == NULL)
		pPageRoot = pPageFilters;
	
	pPage = pPageRoot;
	while (pPage != NULL  && bFound == FALSE)
	{
		for (cou = 0; cou < pPage->CurrentPos && bFound == FALSE; cou++)
		{
			pSinglePage = &pPage->Page[cou];
			if (pSinglePage->pNextLevel != NULL)
				bFound = _DelFilter(FilterID, pSinglePage->pNextLevel, FALSE);
			
			if (bFound == FALSE)
			{
				pQFlt = (PQFLT) pSinglePage->pData;
				pQFltPrev = pQFlt;
				
				while (pQFlt != NULL)
				{
					if (pQFlt->m_FilterID == FilterID)
					{
						AppID = pQFlt->m_AppID;
						bFound = TRUE;
						
						if (pQFlt == pQFltPrev)		// root
							pSinglePage->pData = pQFlt->m_pNext;
						else 
							pQFltPrev->m_pNext = pQFlt->m_pNext;
						
						DbPrint(DC_FILTER,DL_NOTIFY, ("Filter %d deleted\n", pQFlt->m_FilterID));
						ExFreePool(pQFlt);
						pQFlt = NULL;
					}
					else
					{
						pQFltPrev = pQFlt;
						pQFlt = pQFlt->m_pNext;
					}
				}
			}
			if ((bFound == TRUE) && (bRoot == TRUE))
				RequestUnhook(pSinglePage->dwData);
		}
		pPage = (PPOINTERPAGE) pPage->pNext;
	}
	
	ReleaseResource(g_pFilterMutex);
	
	if (bFound == TRUE)
	{
		PQCLIENT pClient = AcquireClient(AppID);
		if (pClient != NULL)
		{
			AcquireClientResource(FALSE);
			{
				PQSUBCLIENT pSubClient = pClient->m_pSubClient;
				pClient->m_FiltersCount--;
				DbPrint(DC_FILTER,DL_NOTIFY, ("Filter %d will be deleted. Notify to client %d\n", FilterID, AppID));
				
				while (pSubClient != NULL)
				{
					if (pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED)
						PulseEventToSubClientSafe(pSubClient, FALSE, 0, AppID);
					
					pSubClient = pSubClient->m_pNext;
				}
			}
			
			ReleaseClientResource();

			ReleaseClient(pClient);
		}
	}

	return bFound;
}

// if AppID == _SYSTEM_APPLICATION (-1) look in all queue
// StartAppID - start search from application with this id  (включительно)
// return AppID and pFoundFlt == &working filter

__inline PQFLT
 CaseFlt(PQFLT pQFlt, ULONG StartAppID, DWORD StartFilterID)
{
	if (StartAppID != _SYSTEM_APPLICATION)
	{
		while (pQFlt != NULL && pQFlt->m_AppID != StartAppID)
			pQFlt = pQFlt->m_pNext;
		
		if (StartFilterID != _INVALID_FILTER_ID)
		{
			while (pQFlt != NULL && pQFlt->m_AppID == StartAppID && pQFlt->m_FilterID != StartFilterID)
				pQFlt = pQFlt->m_pNext;
			if (pQFlt != NULL && pQFlt->m_AppID == StartAppID && pQFlt->m_FilterID == StartFilterID)
				pQFlt = pQFlt->m_pNext;
		}
		else
		{
			while (pQFlt != NULL && pQFlt->m_AppID == StartAppID)
				pQFlt = pQFlt->m_pNext;
		}
	}
	
	return pQFlt;
}

PQFLT
GetPQFlt(ULONG HookID, ULONG FuncMj, ULONG FuncMi, ULONG StartAppID, DWORD StartFilterID, PQFLT* ppQFltList)
{
	// this function must be called on acquired g_pFilterMutex!
	PSINGLEPAGE pSinglePage;
	
	PPOINTERPAGE* pPagesTmpR;
	PPOINTERPAGE* pPagesTmpMj;
	PPOINTERPAGE* pPagesTmpMi;
	
	PQFLT pQFlt = NULL;
	pPagesTmpR = &pPageFilters;
	pSinglePage = GetPData(pPagesTmpR, HookID);
	
	if (pSinglePage != NULL)
	{
		pPagesTmpMj = &(PPOINTERPAGE) pSinglePage->pNextLevel;
		pSinglePage = GetPData(pPagesTmpMj, FuncMj);
		if (pSinglePage != NULL)
		{
			pPagesTmpMi = &(PPOINTERPAGE) pSinglePage->pNextLevel;
			pSinglePage = GetPData(pPagesTmpMi, FuncMi);
			if (pSinglePage != NULL)
			{
				*ppQFltList = (PQFLT) &pSinglePage->pData;
				pQFlt = CaseFlt((PQFLT) pSinglePage->pData, StartAppID, StartFilterID);
			}
			if (pQFlt == NULL)
			{
				pSinglePage = GetPData(pPagesTmpMi, -1);
				if (pSinglePage != NULL)
				{
					*ppQFltList = (PQFLT) &pSinglePage->pData;
					pQFlt = CaseFlt((PQFLT) pSinglePage->pData, StartAppID, StartFilterID);
				}
			}
		}
		if (pQFlt == NULL)
		{
			pSinglePage = GetPData(pPagesTmpMj, -1);
			if (pSinglePage != NULL)
			{
				*ppQFltList = (PQFLT) &pSinglePage->pData;
				pQFlt = CaseFlt((PQFLT) pSinglePage->pData, StartAppID, StartFilterID);
			}
		}
	}
	
	return pQFlt;
}

__inline BOOLEAN __stdcall
IsNeedFilterEventSingle(ULONG HookID, ULONG FuncMj, ULONG FuncMi)
{
	PQFLT pQFltList;
	BOOLEAN bRet = FALSE;

	if (g_pFilterMutex == NULL)
		return FALSE;
	
	AcquireResource(g_pFilterMutex, FALSE);
	if (GetPQFlt(HookID, FuncMj, FuncMi, _SYSTEM_APPLICATION, 0, &pQFltList) != NULL)
		bRet = TRUE;
	ReleaseResource(g_pFilterMutex);
	
	return bRet;
}


BOOLEAN __stdcall
IsNeedFilterEventEx(ULONG HookID, ULONG FuncMj, ULONG FuncMi, HANDLE ThreadID)
{
	BOOLEAN bRet = FALSE;
	
	if(IsInvisibleThread((ULONG) ThreadID))
		return FALSE;
	
	bRet = IsNeedFilterEventSingle(HookID, FuncMj, FuncMi);

	return bRet;
}

BOOLEAN __stdcall
IsNeedFilterEvent(ULONG HookID, ULONG FuncMj, ULONG FuncMi)
{
	return IsNeedFilterEventEx(HookID, FuncMj, FuncMi, (HANDLE) PsGetCurrentThreadId());
}

#ifdef _HOOK_NT_
BOOLEAN
IsNeedFilterEventAsyncChk(ULONG HookID, ULONG FuncMj, ULONG FuncMi,PIRP Irp, BOOLEAN* pInvisible)
{
	ULONG ThreadID = GetRequestorThreadId(Irp);
	if(IsInvisibleThread(ThreadID))
	{
		*pInvisible = TRUE;
		return FALSE;
	}

	*pInvisible = FALSE;

	return IsNeedFilterEventSingle(HookID, FuncMj, FuncMi);
}
#endif

#ifdef CHECK_WHAT_AVPM_DO
#pragma message("----------------------- CHECK_WHAT_AVPM_DO (imp)")
BOOLEAN IsNeedFilterEventNoInvCheck(ULONG HookID, ULONG FuncMj, ULONG FuncMi)
{
	BOOLEAN bRet = FALSE;
	
	if(!UnloadInProgress){
		AcquireResource(g_pFilterMutex, FALSE);
		if (GetPQFlt(HookID, FuncMj, FuncMi, _SYSTEM_APPLICATION, 0) != NULL)
			bRet = TRUE;
		ReleaseResource(g_pFilterMutex);
	}
	return bRet;
}
#endif

PSINGLE_PARAM
GetSingleParamPtr(PFILTER_EVENT_PARAM pEventParam, ULONG ParamID)
{
	DWORD cou;
	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) pEventParam->Params;
	for (cou =  0; cou < pEventParam->ParamsCount; cou++)
	{
		if (pSingleParam->ParamID == ParamID)
			return pSingleParam;
		pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
	}
	return NULL;
}

void
FilterFillSID(PFILTER_EVENT_PARAM pEventParam)
{
#ifdef _HOOK_NT_
	DWORD sid_length;
	PSINGLE_PARAM pSidParam;
	pSidParam = GetSingleParamPtr(pEventParam, _PARAM_OBJECT_SID);
	if (pSidParam != NULL)
	{
		if (pSidParam->ParamValue[0] == 0)
		{
			sid_length = pSidParam->ParamSize = __SID_LENGTH;
			if (SeGetSID(pSidParam->ParamValue, &sid_length) == FALSE)
				DbPrint(DC_FILTER, DL_SPAM, ("can't get sid on %s\n", pEventParam->ProcName));
		}
	}
#endif
}

/*DWORD gPassHint = 0;
ULONG gPassAppID;
FILTER_SUB gPassFltSub;
ULONG gHinOk = 0;
ULONG gHinFail = 0;*/

ULONG
_IsFiltered(PFILTER_EVENT_PARAM pEventParam, ULONG AppID, ULONG StartAppID,  PFILTER_SUB pFilterSub)
{
	ULONG RetAppID = _SYSTEM_APPLICATION;
	ULONG FltIDExpired = -1;
	
	BOOLEAN bParamFound;
	
	if (g_pFilterMutex == NULL)
		return RetAppID;
	
	if (pFilterSub->m_pMappedFilterParams != NULL)
	{
		ExFreePool(pFilterSub->m_pMappedFilterParams);
		pFilterSub->m_pMappedFilterParams = NULL;
	}
	
	if (pEventParam->HookID == FLTTYPE_EMPTY)
	{
		DbPrint(DC_FILTER,DL_IMPORTANT, ("Income filter has FLTTYPE_EMPTY\n"));
	}
	else
	{
		PQFLT pQFlt;
		PQFLT pQFltPrev = NULL;

		DWORD cou;
		PQCLIENT pClient = NULL;
		PQSUBCLIENT pSubClient = NULL;

		PQFLT pQFltList;

		PQFLT pQFltGroupsStart = NULL;
		PQFLT pQFltGroupCurStart = NULL;

		BOOLEAN bMatch = FALSE;

		DWORD Hint = 0;
		
		if (pEventParam->ProcName[0] == 0)
			GetProcName(pEventParam->ProcName, NULL);
		
		FilterFillSID(pEventParam);
								
		AcquireResource(g_pFilterMutex, FALSE);

		/*if (FLTTYPE_KLICK == pEventParam->HookID)
		{
			Hint = Filter_GetHint(pEventParam);
			if (Hint && (Hint == gPassHint))
			{
				RetAppID = gPassAppID;
				*pFilterSub = gPassFltSub;
				gHinOk++;

				ReleaseResource(g_pFilterMutex);
			
				return RetAppID;
			}
		}*/

#ifdef __DBG__
		gFltRequests++;
		FltDrvArr[pEventParam->HookID]++;
#endif
		
		pQFlt = GetPQFlt(pEventParam->HookID, pEventParam->FunctionMj, pEventParam->FunctionMi, StartAppID, 
			pFilterSub->m_FilterID, &pQFltList);
		
		while (pQFlt != NULL)
		{
			PFILTER_PARAM pFilterParam;
			
			BYTE *pPointer;

			bMatch = FALSE;
			
			// check parameters;
			pFilterSub->m_dwCacheValue = _INVALID_VALUE;
			
			if ((pQFlt->m_Expiration != 0) && (IsTimeExpired(pQFlt->m_Expiration) == TRUE))
				FltIDExpired = pQFlt->m_FilterID; // filter expired! will be deleted on exit
			else
			{
				if (pQFlt->m_Flags & FLT_A_DISABLED)
				{
					// filter disabled
					pQFltPrev = pQFlt;
					pQFlt = pQFlt->m_pNext;
					continue;
				}
				
				if (!pClient || (pClient->m_AppID != pQFlt->m_AppID))
				{
					pQFltGroupsStart = NULL;
					if (pClient)
					{	
						ReleaseClient(pClient);
						pClient = NULL;
					}

					if (pSubClient)
					{
						if (pSubClient != &gNonPopupClient)
							ReleaseSubClient(pSubClient);

						pSubClient = NULL;
					}
					
					pClient = AcquireClient(pQFlt->m_AppID);
					if (pClient)
					{
						pSubClient = GetFreeSubClient(pClient);
						if (!pSubClient	&& (!(pClient->m_ClientFlags & (_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_PAUSED))))
							pSubClient = &gNonPopupClient;

					}
				}

				if (pSubClient != NULL)
				{
					if (INVALID_GROUP_ID == pQFlt->m_GroupID)
					{
						pQFltGroupsStart = NULL;
						pQFltGroupCurStart = NULL;
					}
					else
					{
						if (!pQFltGroupsStart)
						{
							if (pQFltPrev)
								pQFltGroupsStart = pQFltPrev;
							else
								pQFltGroupsStart = pQFltList;

							pQFltGroupCurStart = pQFlt;
						}
						else
						{
							if (pQFltGroupCurStart->m_GroupID != pQFlt->m_GroupID)
								pQFltGroupCurStart = pQFlt;
						}
					}

					if ((pQFlt->m_ProcessingType == pEventParam->ProcessingType) || 
						(pQFlt->m_ProcessingType == AnyProcessing) || 
						(pEventParam->ProcessingType == CheckProcessing))
					{
						if((!pQFlt->m_pProcName[0]) || (MatchWithPatternA(pQFlt->m_pProcName, pEventParam->ProcName, FALSE)))
						{
							bMatch = TRUE;
							
							//DbPrint(DC_PARAMS, DL_SPAM, ("filter %d check - start\n", pQFlt->m_FilterID));
							for (cou = 0; (cou < pQFlt->m_ParamsCount) && bMatch; cou++)
							{
								if (cou == 0)
									pFilterParam = (PFILTER_PARAM) pQFlt->m_Params;
								else {
									pPointer = (BYTE*)pFilterParam + sizeof(FILTER_PARAM) + pFilterParam->m_ParamSize;
									pFilterParam = (PFILTER_PARAM) pPointer;
								}
								
								if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_SPECIAL_THISISPROCID)
								{
									if (pFilterParam->m_ParamSize != sizeof(ULONG))
									{
										DbPrint(DC_PARAMS, DL_WARNING, ("filter %d check param (procid) - size invalid"));
									}
									else
									{
										if (*(ULONG*)pFilterParam->m_ParamValue != pEventParam->ProcessID)
											bMatch = FALSE;

										if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_INVERS_OP)
											bMatch = !bMatch;
										
										if (bMatch)
										{
											if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_CACHABLE)
											{
												pFilterSub->m_dwCacheValue = CalcCacheID(sizeof(ULONG), 
													pFilterParam->m_ParamValue, pFilterSub->m_dwCacheValue);
											}
										}
									}
								} 
								else if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_SPECIAL_THISISTHREADID)										
								{
									if (pFilterParam->m_ParamSize != sizeof(ULONG))
									{
										DbPrint(DC_PARAMS, DL_WARNING, ("filter %d check param (threadid) - size invalid"));
									}
									else
									{
										if (*(ULONG*)pFilterParam->m_ParamValue != pEventParam->ThreadID)
											bMatch = FALSE;
										else
										{
											if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_CACHABLE)
											{
												pFilterSub->m_dwCacheValue = CalcCacheID(sizeof(ULONG), 
													pFilterParam->m_ParamValue, pFilterSub->m_dwCacheValue);
											}
										}
									}
								}								
								else
								{
									if ((pFilterParam->m_ParamFlags & (_FILTER_PARAM_FLAG_CACHABLE | _FILTER_PARAM_FLAG_MUSTEXIST)) || 
										(pFilterParam->m_ParamFlt != FLT_PARAM_NOP))
									{
										bMatch = CheckSingleParam(pEventParam, pFilterParam, &(pFilterSub->m_dwCacheValue), &bParamFound);
			
										if (bMatch && !bParamFound)
										{
											if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_MUSTEXIST)
											{
												bMatch = FALSE;
											}
										}
									}
									else
										bMatch = TRUE;
								}
							}
							
							if (!bMatch)
							{
								DbPrint(DC_PARAMS, DL_SPAM, ("filter %d check param - not matched\n", 
									pQFlt->m_FilterID));
							}
							else
							{
								DbPrint(DC_PARAMS, DL_SPAM, ("filter %d check param success\n", 
									pQFlt->m_FilterID));

/*#ifdef __DBG__
									// trace why
									if (CheckProcessing == pEventParam->ProcessingType)
									{
										PWCHAR pwchName = NULL;
										PFILTER_PARAM pParamName = GetParamPtrFromFilter(pQFlt, 0x0800000A);
										if (pParamName)
											pwchName = (PWCHAR) pParamName->m_ParamValue;
										else
											pwchName = L"<fltnoname>";

										DbPrint(DC_PARAMS, DL_IMPORTANT, ("check ok %S (fltid %x). fltflags %x\n", 
											pwchName, pQFlt->m_FilterID, pQFlt->m_Flags));
									}
#endif*/									

								InterlockedIncrement(&pSubClient->m_Tasks);
								pFilterSub->m_pSubClient = pSubClient;
							
								pFilterSub->m_FilterID = pQFlt->m_FilterID;
								pFilterSub->m_Flags = pQFlt->m_Flags;
								pFilterSub->m_Timeout = pQFlt->m_Timeout;
								RetAppID = pQFlt->m_AppID;
								
								if (pFilterSub->m_Flags & FLT_A_PROCESSCACHEABLE)
									pFilterSub->m_dwCacheValue = CalcCacheID(strlen(pEventParam->ProcName), 
									pEventParam->ProcName, pFilterSub->m_dwCacheValue);
								
								if (pFilterSub->m_Flags & FLT_A_TOUCH) {
									DbPrint(DC_FILTER,DL_IMPORTANT, ("Touch filter detected\n"));
									FltIDExpired = pQFlt->m_FilterID; //may overwrite expired by timeout filter
								}
								
								if (pQFlt->m_Flags & (FLT_A_POPUP | FLT_A_INFO | FLT_A_NOTIFY))
								{

									if (pQFlt->m_Flags & FLT_A_HAVE_MAPPED_PARAMS)
									{
										DWORD MappedParamsCount;
										DWORD MappedParamSize;
										
										MappedParamSize = CalcMappedParamSize(pQFlt, &MappedParamsCount);
										if (MappedParamSize != 0)
										{
											pFilterSub->m_pMappedFilterParams = ExAllocatePoolWithTag(NonPagedPool,
												MappedParamSize + sizeof(MAPPED_PARAMS),'zboS');
											if (pFilterSub->m_pMappedFilterParams == NULL)
											{
												DbPrint(DC_FILTER, DL_IMPORTANT, ("Can't allocate memory for mapped params\n"));
											}
											else
											{
												pFilterSub->m_pMappedFilterParams->m_ParamsCount = MappedParamsCount;
												pFilterSub->m_pMappedFilterParams->m_Param_Size = MappedParamSize;
												CopyMappedParams(pFilterSub->m_pMappedFilterParams, pQFlt);
											}
										}
									}
								}
								
								if (pQFltGroupCurStart)
								{
									pQFltGroupCurStart->m_GroupCounter++;

									if (pQFltGroupCurStart->m_GroupCounter > _GROUP_COUNTER_MAX)
									{
										pQFltGroupCurStart->m_GroupCounter = 0;

										// relink filter's queue

										if (pQFltGroupsStart->m_pNext == pQFltGroupCurStart)
										{
											// nothing todo - this group is first in queue
										}
										else
										{
											PQFLT pQFltTmpPrev = pQFltGroupsStart->m_pNext;
											while (pQFltTmpPrev->m_pNext->m_FilterID != pQFltGroupCurStart->m_FilterID)
												pQFltTmpPrev = pQFltTmpPrev->m_pNext;

											pQFltTmpPrev->m_pNext = pQFlt->m_pNext;

											pQFlt->m_pNext = pQFltGroupsStart->m_pNext;
											pQFltGroupsStart->m_pNext = pQFltGroupCurStart;
										}
									}
								}
								pQFlt = NULL;
							}
							
							//DbPrint(DC_PARAMS, DL_SPAM, ("filter check param - finish\n"));
						}
					}
				}
			}
			if (pQFlt != NULL)
			{
				pQFltPrev = pQFlt;
				pQFlt = pQFlt->m_pNext;
			}
		}

/*		if (bMatch
			&& Hint
			&& (_INVALID_VALUE == pFilterSub->m_dwCacheValue)
			&& (0 == (pFilterSub->m_Flags & (FLT_A_POPUP | FLT_A_INFO | FLT_A_NOTIFY)))
			)
		{
			gPassHint = Hint;
			gPassFltSub = *pFilterSub;
			gPassFltSub.m_pSubClient = NULL;
			gPassAppID = RetAppID;

			gHinFail++;
		}*/

		ReleaseResource(g_pFilterMutex);
		
		if (!bMatch)
		{
			if (pSubClient && pSubClient != &gNonPopupClient)
				ReleaseSubClient(pSubClient);
		}
		if (pClient)
			ReleaseClient(pClient);
	}
	
	if (FltIDExpired != -1)
	{
		DbPrint(DC_FILTER,DL_NOTIFY, ("Expired filter detected\n"));
		_DelFilter(FltIDExpired, NULL, TRUE);
	}
	
	return RetAppID;//_SYSTEM_APPLICATION;
}


// -----------------------------------------------------------------------------------------
// Function name	: FilterEvent
// Description	    : обработка события от хука
// Return type		: VERDICT			- see avpgcom.h
// Argument         : ULONG Function	- function number
// Argument         : PCHAR ProcName	- process name
// Argument         : PCHAR ObjName		- object name
// Argument         : DWORD UnsafeValue	- 0 if ReplyWaiting absolutly safe (deadlock free)

#define __EMPTY_URL "<empty>"
#define __EMPTY_URL_W L"<empty>"
VERDICT __stdcall
FilterEvent(PFILTER_EVENT_PARAM pEventParam, PEVENT_OBJECT_INFO pEventObject)
{
	//! Помещать ли в лог информацию об ошибках при обработке event-а?
	PSINGLE_PARAM pSingleParam;
	
	PQCLIENT pClient;
	ULONG AppID;
	
	FILTER_SUB FilterSub;
	
	DWORD dwCache;	// 0 - not cahced, 1 - cached but temporary dirty, 2 - cached
	
	PCHAR VerdStr;
	PCHAR UserStr;
	
	PCHAR pUrl = NULL;
	DWORD EventFlags;
	
	BOOLEAN bNeedProcessing;
	
#ifdef _HOOK_NT_
	BOOLEAN bUniName = FALSE;
	
	DbPrint(DC_FILTER, DL_SPAM, ("FilterEvent enter Hook %#x Mj %#x Mi %#x!\n", pEventParam->HookID,
		pEventParam->FunctionMj, pEventParam->FunctionMi));

	if (pEventParam->ThreadID == 0)
		pEventParam->ThreadID = (ULONG)PsGetCurrentThreadId();
	
	if (IsInvisibleThread(pEventParam->ThreadID))
		return Verdict_NotFiltered;

	if (KeGetCurrentIrql() >= DISPATCH_LEVEL)
	{
		if (pEventParam->ProcessingType == PreDispatch)
		{
#ifdef __DBG__
			gFltRequestsDispatch++;
#endif
			return Di_FilterEvent(pEventParam, pEventObject);
		}

		DbPrint(DC_FILTER,DL_WARNING, ("FilterEvent failed - IRQ level (%x) too high!\n",KeGetCurrentIrql()));
		return Verdict_NotFiltered;
	}
#endif

	RtlZeroMemory(&FilterSub, sizeof(FilterSub));
	FilterSub.m_Verdict = Verdict_NotFiltered;
	FilterSub.m_FilterID = _INVALID_FILTER_ID;
	
	if (pEventParam->ProcessID == 0)
		pEventParam->ProcessID = (ULONG) PsGetCurrentProcessId();
	
	pSingleParam = (PSINGLE_PARAM) pEventParam->Params;
	
#ifdef _HOOK_VXD_	
	if (pEventParam->ParamsCount != 0)
	{
		if (pSingleParam->ParamID == _PARAM_OBJECT_URL)
		{
			pUrl = pSingleParam->ParamValue;
			if(*(DWORD*)UnknownStr==*(DWORD*)pUrl) //if (strncmp(UnknownStr, pUrl, 3) == 0)
				return FilterSub.m_Verdict;
		}
	}
#else
	if (pEventParam->ParamsCount != 0)
	{
		if (pSingleParam->ParamID == _PARAM_OBJECT_URL_W)
		{
			pUrl = pSingleParam->ParamValue;
			bUniName = TRUE;
		}
	}
#endif
	if(!pUrl)
	{
#ifdef _HOOK_VXD_
		pUrl = __EMPTY_URL;
#else
		pUrl = (PCHAR) __EMPTY_URL_W;
#endif
		DbPrint(DC_FILTER,DL_SPAM, ("pUrl === __EMPTY_URL HookID=%x Mj=%x Mn=%x\n", pEventParam->HookID, 
			pEventParam->FunctionMj,pEventParam->FunctionMi));
	}
	// -----------------------------------------------------------------------------------------
	//!
/*#ifdef __DBG__
	if (MatchWithPatternA("MSDEV*", pEventParam->ProcName, FALSE) == TRUE)
		return FilterSub.m_Verdict;
	if (MatchWithPatternA("WINDBG*", pEventParam->ProcName, FALSE) == TRUE)
		return FilterSub.m_Verdict;	
#endif*/
	// -----------------------------------------------------------------------------------------
	
	pEventParam->UnsafeValue += IsWaitingSafe();

	if (!pEventParam->UnsafeValue)
	{
		if (!CheckProc_IsAllowProcessExecuting())
			return Verdict_Discard;
	}
	
	AppID = _IsFiltered(pEventParam, _SYSTEM_APPLICATION, _SYSTEM_APPLICATION, &FilterSub);
	
	if (pEventParam->ProcessingType == CheckProcessing)
	{
		if (FilterSub.m_pSubClient != 0)
		{
			if (FilterSub.m_pSubClient->m_Tasks > 0)
				InterlockedDecrement(&FilterSub.m_pSubClient->m_Tasks);

			if (FilterSub.m_pSubClient != &gNonPopupClient)
			{
				ReleaseSubClient(FilterSub.m_pSubClient);
				FilterSub.m_pSubClient = 0;
			}
		}
		if (AppID == _SYSTEM_APPLICATION)
			FilterSub.m_Verdict = Verdict_NotFiltered;
		else
		{
			FilterSub.m_Verdict = Verdict_Pass;
			
			if (FilterSub.m_Flags & FLT_A_POPUP)
				FilterSub.m_Verdict = Verdict_UserRequest;
			else
			{
				if (FilterSub.m_Flags & FLT_A_DEFAULT)
					FilterSub.m_Verdict = Verdict_Default;
				if (FilterSub.m_Flags & FLT_A_DENY)
					FilterSub.m_Verdict = Verdict_Discard;
			}
		}
		
		if (FilterSub.m_pMappedFilterParams != NULL)
			ExFreePool(FilterSub.m_pMappedFilterParams);


		DbPrint(DC_FILTER, DL_NOTIFY, ("CheckProcessing verdict: %x (Hook %#x Mj %#x Mi %#x)\n", 
			FilterSub.m_Verdict, pEventParam->HookID, pEventParam->FunctionMj, pEventParam->FunctionMi));

		return FilterSub.m_Verdict;
	}
	
	if (AppID == _SYSTEM_APPLICATION)
	{
		DbPrint(DC_FILTER, DL_SPAM, ("FilterEvent end (no app) Hook %#x Mj %#x Mi %#x!"_STRING_DEF"\n", 
			pEventParam->HookID, pEventParam->FunctionMj, pEventParam->FunctionMi, pUrl));
		
		return Verdict_NotFiltered;
	}
	
	gFilterEventEntersCount++;
	
	DbPrint(DC_FILTER, DL_SPAM, ("Filter event started with client %d for "_STRING_DEF"\n", AppID, pUrl));
	
	while (AppID != _SYSTEM_APPLICATION)
	{
		pClient = AcquireClient(AppID);
		AppID = _SYSTEM_APPLICATION;
		
		if (pClient == NULL)
		{
			DbPrint(DC_FILTER,DL_FATAL_ERROR, ("Filtered but client not found!\n"));
			if (FilterSub.m_pSubClient != &gNonPopupClient)
			{
				ReleaseSubClient(FilterSub.m_pSubClient);
				FilterSub.m_pSubClient = 0;
			}
		}
		else
		{
			FilterSub.m_Verdict = Verdict_Default;
			EventFlags = _EVENT_FLAG_NONE;
			UserStr="Auto";
			
			bNeedProcessing = FALSE;
			
			if(FilterSub.m_Flags & FLT_A_DENY)
				FilterSub.m_Verdict = Verdict_Discard;
			else if(FilterSub.m_Flags & FLT_A_PASS)
				FilterSub.m_Verdict = Verdict_Pass;
			
			if (pEventParam->ProcessingType == PostProcessing)
				EventFlags |= _EVENT_FLAG_POSTPROCESSING;
/*#ifdef __DBG__
#ifndef __DRV_FUNC_NOPRINT
			{
				PCHAR Action;
				Action = ExAllocatePoolWithTag(PagedPool, MAX_MSG_LEN, 'LSeB');
				if(Action != NULL)
				{
					_GetFunctionStr(Action, pEventParam->HookID, pEventParam->FunctionMj, pEventParam->FunctionMi);
					DbPrint(DC_FILTER, DL_INFO, ("Event filtered(%s-%x). Client %d Process: %s Url "_STRING_DEF"\n", 
						Action, FilterSub.m_Flags, pClient->m_AppID, pEventParam->ProcName, pUrl));
					
					ExFreePool(Action);
					Action = NULL;
				}
				else
				{
					DbPrint(DC_FILTER, DL_INFO, ("Event filtered(%x:%x:%x-%x). Client %d Process: %s Url "_STRING_DEF"\n", 
						pEventParam->HookID, pEventParam->FunctionMj, pEventParam->FunctionMi, FilterSub.m_Flags, 
						pClient->m_AppID, pEventParam->ProcName, pUrl));
				}
			}
#endif //__DRV_FUNC_NOPRINT
#endif*/
			// cache -----------------------------------------------------------------------------------
			dwCache = _CL_CACHE_NOTCHECKED;
			
			if (pClient->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE)
			{
				if (FilterSub.m_Flags & FLT_A_RESETCACHE)
					Obj_CacheReset(pClient);
				else
				{
					if (FilterSub.m_Flags & FLT_A_USECACHE)
					{
						dwCache = Obj_IsCached(pClient, FilterSub.m_dwCacheValue, TRUE, &FilterSub.m_Verdict);
						if (dwCache == _CL_CACHE_CACHED)
						{
							UserStr = "Cached";
							DbPrint(DC_FILTER,DL_INFO, ("CacheV hit - %d CacheValue: %x ("_STRING_DEF")\n", 
								FilterSub.m_Verdict, FilterSub.m_dwCacheValue, pUrl));
							FilterSub.m_Flags &= ~FLT_A_CHECKNEXTFILTER;
						}
					}
				}
				
				if (FilterSub.m_Flags & FLT_A_SAVE2CACHE)
				{
					DWORD tmpCached;
					DWORD OldVerdict;
					if (dwCache == _CL_CACHE_NOTCHECKED)
						tmpCached = Obj_IsCached(pClient, FilterSub.m_dwCacheValue, TRUE, &OldVerdict);
					else
						tmpCached = dwCache;
					if (FilterSub.m_Flags & FLT_A_PASS)
					{
						if (tmpCached == _CL_CACHE_NOTCACHED)
							Obj_CacheAdd(pClient, FilterSub.m_dwCacheValue, FALSE, FilterSub.m_Verdict, 0);
						else	if (tmpCached == _CL_CACHE_CACHED_DIRTY)
							Obj_CacheUpdate(pClient, FilterSub.m_dwCacheValue, FilterSub.m_Verdict, 0);
						
					}
					else
					{
						if (tmpCached == _CL_CACHE_NOTCACHED)
							Obj_CacheAdd(pClient, FilterSub.m_dwCacheValue, TRUE, FilterSub.m_Verdict, 0);
					}
				}
			}
			
			// end cache -------------------------------------------------------------------------------
			if (dwCache != _CL_CACHE_CACHED)
				bNeedProcessing = TRUE;
			
			if (bNeedProcessing == TRUE)
			{
				if (FilterSub.m_Flags & (~FLT_A_POPUP))
				{
					if ((FilterSub.m_Flags & FLT_A_NOTIFY) && 
						(FilterSub.m_pSubClient->m_ActivityState & (~_INTERNAL_APP_STATE_DEAD)))
					{
						UserStr = "Notify";
						PulseEventToSubClientSafe(FilterSub.m_pSubClient, TRUE, 
							FilterSub.m_pSubClient->m_BlueScreenTimeout, pClient->m_AppID);
					}
					
					if ((FilterSub.m_Flags & FLT_A_INFO) && 
						(FilterSub.m_pSubClient->m_ActivityState & ~_INTERNAL_APP_STATE_DEAD))
					{
						if (FilterSub.m_pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED)
						{
							if (QueueAdd(pEventParam, &pClient->m_EventList, &FilterSub, 0, EventFlags, 
								pClient->m_ClientFlags, NULL) != NULL)
							{
								PulseEventToSubClientSafe(FilterSub.m_pSubClient, TRUE, 
									FilterSub.m_pSubClient->m_BlueScreenTimeout, pClient->m_AppID);
								UserStr = "Info";
							}
							else
							{
								// max len reached
								FilterSub.m_Verdict = pClient->m_DefaultVerdict;
								DbPrint(DC_EVENT, DL_IMPORTANT, ("Max queue len reached! Skipping event with default client verdict %d\n",
									FilterSub.m_Verdict));
								DbgBreakPoint();
							}
						}
					}
				}
				
				if (FilterSub.m_pSubClient->m_ActivityState & _INTERNAL_APP_STATE_DEAD)			//default value and log
				{
					UserStr="Auto";
					FilterSub.m_Flags |= FLT_A_LOG;
				}
				if ((FilterSub.m_pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED) && 
					(FilterSub.m_Flags & FLT_A_POPUP))
				{
					BOOLEAN bPr = FALSE;
					UserStr="User";
					
					if ((pEventParam->UnsafeValue + pClient->m_ClientsSafeValue) != 0)
					{
						DbPrint(DC_FILTER,DL_NOTIFY, ("Waiting unsafe. Value is %d (Client unsafe %d). Delay processing...",
							pEventParam->UnsafeValue, pClient->m_ClientsSafeValue));
						EventFlags |= _EVENT_FLAG_WAITINGUNSAFE;
					} 
					else 
					{					
						PQELEM newEvQueue;
						FilterSub.m_Verdict = Verdict_Default;
						EventFlags |= _EVENT_FLAG_POPUP;
						newEvQueue = QueueAdd(pEventParam, &pClient->m_EventList, &FilterSub, 0, EventFlags, 
							pClient->m_ClientFlags, pEventObject);
						if (newEvQueue == NULL)
						{
							// max len reached
							FilterSub.m_Verdict = pClient->m_DefaultVerdict;
							DbPrint(DC_EVENT, DL_IMPORTANT, ("Max queue len reached! Skipping event with default client verdict %d\n",
								FilterSub.m_Verdict));
							DbgBreakPoint();
						}
						else
						{
							eReplyWaiter eReply;
							FilterSub.m_Verdict = ReplyWaiter(FilterSub.m_pSubClient, &pClient->m_EventList,
								newEvQueue, pUrl, pClient->m_AppID, &eReply); // processing + free memory;
							
							if (eReply == ReplyWaiter_Succeed)
								bPr = TRUE;
							else
							{
								switch (eReply)
								{
								case ReplyWaiter_WaitFailed:
/*#ifdef __DBG__
#ifndef __DRV_FUNC_NOPRINT
									{
										PCHAR Action;
										Action=ExAllocatePoolWithTag(PagedPool,MY_PAGE_SIZE,' SeB');
										if(Action)
										{
#ifdef _HOOK_NT_
											NTGetFunctionStr(Action,pEventParam->HookID, pEventParam->FunctionMj,
												pEventParam->FunctionMi);
#else
											VxDGetFunctionStr(Action,pEventParam->HookID, pEventParam->FunctionMj,
												pEventParam->FunctionMi);
#endif
											DbPrint(DC_FILTER,DL_WARNING, ("Can't proceed (verdict %d) with this marked event\n(%s) %s - "_STRING_DEF"\n", 
												FilterSub.m_Verdict, Action,pEventParam->ProcName, pUrl));
											ExFreePool(Action);
										}
										else
										{
											DbPrint(DC_FILTER,DL_WARNING, ("Can't proceed with this marked event\n%x:%x:%x %s - "_STRING_DEF"\n", 
												pEventParam->HookID, pEventParam->FunctionMj, pEventParam->FunctionMi, pEventParam->ProcName, pUrl));
										}
										DbgBreakPoint();
									}
#endif /__DRV_FUNC_NOPRINT
#endif //__DBG__*/
									EventFlags |= _EVENT_FLAG_RESCHEDULED;
									// to info mode
									FilterSub.m_Flags &= ~FLT_A_POPUP;	// снятие флага POPUP
									FilterSub.m_Flags |= FLT_A_INFO;	// 
									
									if ((pClient->m_ClientFlags & _CLIENT_FLAG_CACHDEADLOCK) && (pClient->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE))
										Obj_CacheUpdate(pClient, FilterSub.m_dwCacheValue, Verdict_Pass, 10 * 1000);	// 10 sec
									
									QueueAdd(pEventParam, &pClient->m_EventList, &FilterSub, 0/*pClient->m_BlueScreenTimeout*/, EventFlags, pClient->m_ClientFlags, NULL);
									break;
								case ReplyWaiter_SendFailed:
									DbPrint(DC_FILTER,DL_WARNING, ("Can't proceed (Safe values is %d) with this event (timeout in ReplyWaiter)\n%x:%x:%x %s - "_STRING_DEF"\n", 
										pEventParam->UnsafeValue, pEventParam->HookID, pEventParam->FunctionMj, pEventParam->FunctionMi, pEventParam->ProcName, pUrl));
									DbgBreakPoint();
									if ((pClient->m_ClientFlags & _CLIENT_FLAG_CACHDEADLOCK) && (pClient->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE))
										Obj_CacheUpdate(pClient, FilterSub.m_dwCacheValue, Verdict_Pass, 10 * 1000);	// 10 sec
									break;
								}
							}
						}
					}
					
					EventFlags &= ~_EVENT_FLAG_POPUP;	// снятие флага POPUP
					
					if (bPr == FALSE) {
						if (pClient->m_ClientFlags & _CLIENT_FLAG_USE_BLUE_SCREEN_REQUEST) {
							DbPrint(DC_FILTER,DL_NOTIFY, ("BlueScreenRequest\n"));
							//can't switch to ring3
							FilterSub.m_Verdict = BlueScreenReq(pEventParam, pUrl); // switch to blue_screen_request mode
						}
						else
						{
							//!!
							if (pClient->m_ClientFlags & _CLIENT_FLAG_DONTPROCEEDDEADLOCK)
							{
								DbPrint(DC_FILTER,DL_SPAM, ("Deadlock but client don't want work with such event on rescheduled\n"));
							}
							else
							{
								// to info mode
								FilterSub.m_Flags &= ~FLT_A_POPUP;	// снятие флага POPUP
								FilterSub.m_Flags |= FLT_A_INFO;	// 
								EventFlags |= _EVENT_FLAG_DEADLOCK_DETECTED;
								QueueAdd(pEventParam, &pClient->m_EventList, &FilterSub, 0/*pClient->m_BlueScreenTimeout*/, EventFlags, pClient->m_ClientFlags, NULL);
#ifdef _HOOK_VXD_	
								if (pClient->m_AppID != 3)
#endif
									PulseEventToSubClientSafe(FilterSub.m_pSubClient, TRUE, FilterSub.m_pSubClient->m_BlueScreenTimeout, pClient->m_AppID); //Оставленно событие в очереди для постобработки.
							}
						}
					}
				}
			}
			
			if (FilterSub.m_pSubClient->m_Tasks > 0)
				InterlockedDecrement(&FilterSub.m_pSubClient->m_Tasks);

			if (FilterSub.m_pSubClient != &gNonPopupClient)
			{
				ReleaseSubClient(FilterSub.m_pSubClient);
				FilterSub.m_pSubClient = 0;
			}

			switch (FilterSub.m_Verdict) {
			case Verdict_Pass:
				{
					VerdStr = "Pass";
					DbPrint(DC_FILTER,DL_NOTIFY, ("%s - pass: "_STRING_DEF"\n", pEventParam->ProcName, pUrl));
				}
				break;
			case Verdict_Default:
				VerdStr = "Default";
				FilterSub.m_Verdict = Verdict_Pass;
				DbPrint(DC_FILTER,DL_NOTIFY, ("%s - Default: "_STRING_DEF"\n", pEventParam->ProcName, pUrl));
				break;
			case Verdict_Discard:
				VerdStr = "Discard";
				DbPrint(DC_FILTER,DL_NOTIFY, ("%s - Discard: "_STRING_DEF"\n", pEventParam->ProcName, pUrl));
				DbgBreakPoint();
				break;
			case Verdict_Debug:
				VerdStr = "Verdict_Debug->Verdict_Default";
				UserStr="Timeout";
				FilterSub.m_Verdict = Verdict_Default;
				DbPrint(DC_FILTER, DL_WARNING, ("Verdict debug.. making breakpoint in dbg version'\n"));
				DbgBreakPoint();
				break;
			case Verdict_Kill:
				VerdStr = "Kill";
				DbPrint(DC_FILTER,DL_NOTIFY, ("%s - Kill \n", pEventParam->ProcName));
				break;
			case Verdict_WDog:
				{
					VerdStr = "Verdict_WDog->Verdict_Pass";
					UserStr="Timeout";
					FilterSub.m_Verdict = Verdict_Pass;
					DbPrint(DC_FILTER,DL_WARNING, ("Dead lock! Changed to 'Pass'\n"));
				}
				break;
			case Verdict_NotFiltered:
			case Verdict_UserRequest:
				VerdStr = "slugebnie -> default";
				UserStr="default";
				FilterSub.m_Verdict = Verdict_Default;
				break;
			case Verdict_Continue:
				{
					VerdStr = "Verdict_Continue->Verdict_Pass";
					UserStr="Continue";
					FilterSub.m_Verdict = Verdict_Pass;
					FilterSub.m_Flags |= FLT_A_CHECKNEXTFILTER;
					DbPrint(DC_FILTER,DL_NOTIFY, ("%s - Continue: "_STRING_DEF". Try to locate next filter for this client...\n",
						pEventParam->ProcName, pUrl));
				}
				break;
			default:
				{
					DbPrint(DC_FILTER,DL_ERROR, ("!!! Unknow verdict! Changed to 'Pass'. Value was %x\n", 
						FilterSub.m_Verdict));
					DbgBreakPoint();
					FilterSub.m_Verdict = Verdict_Pass;
					VerdStr = "Pass";
				}
			}
			
			if ((pClient->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE) && (FilterSub.m_Flags & FLT_A_DROPCACHE))
				Obj_CacheDel(pClient, FilterSub.m_dwCacheValue);
#ifdef __DBG__
			if (FilterSub.m_Flags & FLT_A_DBG_BREAK)
			{
				DbPrint(DC_FILTER, DL_WARNING, ("Filter with make dbgbreakpoint.. making breakpoint in dbg version'\n"));
				DbgBreakPoint();
			}
#endif
			if (_PASS_VERDICT(FilterSub.m_Verdict))
			{
				if (!(FilterSub.m_Flags & FLT_A_DENYPASS2LOWPR))
				{
					if (!(FilterSub.m_Flags & FLT_A_CHECKNEXTFILTER))
						FilterSub.m_FilterID = _INVALID_FILTER_ID;
					
					AppID = _IsFiltered(pEventParam, _SYSTEM_APPLICATION, pClient->m_AppID, &FilterSub);
					if (AppID != _SYSTEM_APPLICATION)
					{
						DbPrint(DC_FILTER,DL_INFO, ("Next Client %d\n", AppID));
					}
				}
			}
			ReleaseClient(pClient);
		}
	}
	
	DbPrint(DC_FILTER, DL_SPAM, ("Filter event ended\n"));
	gFilterEventEntersCount--;
	
	if (FilterSub.m_pMappedFilterParams != NULL)
		ExFreePool(FilterSub.m_pMappedFilterParams);
	
	return FilterSub.m_Verdict;
}

//------------------------------------------------------------------------------------------

#ifdef _HOOK_NT_
#define SizeOfStringFiltersKeyName 8
WCHAR FiltersKeyNameBuffer[SizeOfStringFiltersKeyName+1]=L"\\Filters";
UNICODE_STRING	FiltersKeyName=
{SizeOfStringFiltersKeyName*sizeof(WCHAR),
(SizeOfStringFiltersKeyName+1)*sizeof(WCHAR),
FiltersKeyNameBuffer};
WCHAR *LogFNameValBuffer=L"Client";
WCHAR *BldValBuffer=L"Build";
#else
CHAR	*FiltersKeyName="\\Filters";
CHAR	*BldValBuffer="Build";
CHAR	*LogFNameValBuffer="Client";
#endif //_HOOK_NT_

#define _SAVE_FILTER_SIZE	4096*2 
BOOLEAN SaveFilters(PQCLIENT pClient)
{
	NTSTATUS				ntstatus;
	HKEY					ParKeyHandle;
	HKEY					KeyHandle;
	ULONG					i;
	DWORD					dwFilterSize;
	DWORD					StartFilterID;
	PFILTER_TRANSMIT		pTmpFlt=NULL;
	PCLIENT_SAV				pClientRec=NULL;
#ifdef _HOOK_NT_
	OBJECT_ATTRIBUTES	objectAttributes;
	UNICODE_STRING		FltKeyName;
	UNICODE_STRING		ValueName;
	WCHAR					ValueBuff[10];
	FltKeyName.Buffer=NULL;
	FltKeyName.Length=0;
	FltKeyName.MaximumLength=MAXPATHLEN;
#else 
	CHAR					ValueBuff[10];
	CHAR					*FltKeyName;
#endif //_HOOK_NT_
	if (!(pClient->m_ClientFlags & _CLIENT_FLAG_SAVE_FILTERS))
		return STATUS_SUCCESS;
	
	DbPrint(DC_CLIENT,DL_IMPORTANT, ("Client %d - save filters...\n", pClient->m_AppID));
	
	pTmpFlt = ExAllocatePoolWithTag(PagedPool, _SAVE_FILTER_SIZE, 'fSeB');
	pClientRec = ExAllocatePoolWithTag(PagedPool,sizeof(CLIENT_SAV),'fSeB');
	
	if((pTmpFlt == NULL) || (pClientRec == NULL))
	{
		if(pClientRec != NULL)
			ExFreePool(pClientRec);
		if(pTmpFlt != NULL)
			ExFreePool(pTmpFlt);
		return FALSE;
	}
	
	memset(pClientRec, 0, sizeof(CLIENT_SAV));
#ifdef _HOOK_NT_
	if(!(FltKeyName.Buffer=ExAllocatePoolWithTag(PagedPool,FltKeyName.MaximumLength,'fSeB')))
	{
		ExFreePool(pTmpFlt);
		ExFreePool(pClientRec);
		return FALSE;
	}
	//HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\avpg\Parameters
	InitializeObjectAttributes(&objectAttributes,&RegParams,OBJ_CASE_INSENSITIVE,NULL,NULL);
	if((ntstatus=ZwCreateKey(&ParKeyHandle,KEY_WRITE,&objectAttributes,0,NULL,0,NULL))==STATUS_SUCCESS)
	{
		if((ntstatus=RtlIntegerToUnicodeString(pClient->m_AppID,0,&FltKeyName))==STATUS_SUCCESS)
		{
			InitializeObjectAttributes(&objectAttributes,&FltKeyName,OBJ_CASE_INSENSITIVE,ParKeyHandle,NULL);
			if((ntstatus=ZwCreateKey(&KeyHandle,KEY_SET_VALUE,&objectAttributes,0,NULL,0,NULL))==STATUS_SUCCESS)
			{
#else
	if(!(FltKeyName=ExAllocatePoolWithTag(PagedPool,MAXPATHLEN,'fSeB')))
	{
		ExFreePool(pTmpFlt);
		ExFreePool(pClientRec);
		return FALSE;
	}
	//HKEY_LOCAL_MACHINE\SOFTWARE\KasperskyLab\"AVPGNAME"\Parameters
	if((ntstatus=RegCreateKey(HKEY_LOCAL_MACHINE,RegParams,&ParKeyHandle))==ERROR_SUCCESS)
	{
		if(ltoa(pClient->m_AppID,FltKeyName,10))
		{
			if((ntstatus=RegCreateKey(ParKeyHandle,FltKeyName,&KeyHandle))==ERROR_SUCCESS)
			{
#endif //_HOOK_NT_

				i=HOOK_REGESTRY_INTERFACE_NUM;
#ifdef _HOOK_NT_
				RtlInitUnicodeString(&ValueName,BldValBuffer);
				ZwSetValueKey(KeyHandle,&ValueName,0,REG_DWORD,&i,sizeof(i));
#else 
				RegSetValueEx(KeyHandle,BldValBuffer,0,REG_DWORD,(PBYTE)&i,sizeof(i));
#endif //_HOOK_NT_

				pClientRec->Priority=pClient->m_Priority;
				pClientRec->ClientFlags = pClient->m_ClientFlags;
				if (pClient->m_pSubClient != NULL)
					pClientRec->BlueScreenTimeout = pClient->m_pSubClient->m_BlueScreenTimeout;
				else
					pClientRec->BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;
				pClientRec->CacheSize = pClient->m_CacheSize;
#ifdef _HOOK_NT_
				RtlInitUnicodeString(&ValueName,LogFNameValBuffer);
				ntstatus=ZwSetValueKey(KeyHandle,&ValueName,0,REG_BINARY,pClientRec,sizeof(CLIENT_SAV));
				DbPrint(DC_FILTER,DL_NOTIFY, ("Write Client %S. Status=%x\n",FltKeyName.Buffer,ntstatus));
				ZwClose(KeyHandle);
				if((ntstatus=RtlAppendUnicodeStringToString(&FltKeyName,&FiltersKeyName))==STATUS_SUCCESS)
				{
					InitializeObjectAttributes(&objectAttributes,&FltKeyName,OBJ_CASE_INSENSITIVE,ParKeyHandle,NULL);
					if((ntstatus=ZwCreateKey(&KeyHandle,KEY_WRITE,&objectAttributes,0,NULL,0,NULL))==STATUS_SUCCESS)
					{
						ZwDeleteKey(KeyHandle);
						ZwClose(KeyHandle);
						if((ntstatus=ZwCreateKey(&KeyHandle,KEY_SET_VALUE,&objectAttributes,0,NULL,0,NULL))==STATUS_SUCCESS)
						{
#else
				ntstatus=RegSetValueEx(KeyHandle,LogFNameValBuffer,0,REG_BINARY,(PBYTE)pClientRec,sizeof(CLIENT_SAV));
				DbPrint(DC_FILTER,DL_NOTIFY, ("Write Client %s. Status=%x\n",FltKeyName,ntstatus));
				RegCloseKey(KeyHandle);
				if(strcat(FltKeyName,FiltersKeyName)) {
					ntstatus=RegDeleteKey(ParKeyHandle,FltKeyName);
					if(ntstatus==ERROR_SUCCESS || ntstatus==ERROR_FILE_NOT_FOUND) {
						if((ntstatus=RegCreateKey(ParKeyHandle,FltKeyName,&KeyHandle))==ERROR_SUCCESS)
						{
#endif //_HOOK_NT_

							i = 0;
							StartFilterID = -1;
							while(_GetNextFilter(pClient->m_AppID, &StartFilterID, pTmpFlt, _SAVE_FILTER_SIZE, NULL, &ntstatus))
							{
								dwFilterSize = CalcFilterSize(pTmpFlt);
								if (dwFilterSize != 0)
								{
	#ifdef _HOOK_NT_
									swprintf(ValueBuff,L"%09u",i);
									RtlInitUnicodeString(&ValueName,ValueBuff);
									ntstatus=ZwSetValueKey(KeyHandle,&ValueName, 0, REG_BINARY, pTmpFlt, dwFilterSize);
									DbPrint(DC_FILTER,DL_INFO, ("Write Filter %S. Status=%x\n",ValueName.Buffer, ntstatus));
	#else
									sprintf(ValueBuff,"%09u",i);
									ntstatus=RegSetValueEx(KeyHandle,ValueBuff,0,REG_BINARY,(PBYTE)pTmpFlt, dwFilterSize);
									DbPrint(DC_FILTER,DL_INFO, ("Write Filter %s. Status=%x\n",ValueBuff,ntstatus));

	#endif //_HOOK_NT_
								}
								StartFilterID = pTmpFlt->m_FilterID;
								i++;
							}
#ifdef _HOOK_NT_
							ZwClose(KeyHandle);
#else
							RegCloseKey(KeyHandle);
#endif //_HOOK_NT_
						} else {
							DbPrint(DC_FILTER,DL_ERROR, ("ZwCreateKey failed. Status=%x\n",ntstatus));
						}
					} else {
						DbPrint(DC_FILTER,DL_ERROR, ("ZwCreateKey failed. Status=%x\n",ntstatus));
					}
				} else {
					DbPrint(DC_FILTER,DL_ERROR, ("RtlAppendUnicodeStringToString failed. Status=%x\n",ntstatus));
				}
			} else {
				DbPrint(DC_FILTER,DL_ERROR, ("ZwCreateKey failed. Status=%x\n",ntstatus));
			}
		} else {
			DbPrint(DC_FILTER,DL_ERROR, ("RtlIntegerToUnicodeString failed. Status=%x\n",ntstatus));
		}
#ifdef _HOOK_NT_
		ZwClose(ParKeyHandle);
#else
		RegCloseKey(ParKeyHandle);
#endif //_HOOK_NT_
	} else {
		DbPrint(DC_FILTER,DL_ERROR, ("ZwCreateKey failed. Status=%x\n",ntstatus));
	}
#ifdef _HOOK_NT_
	ExFreePool(FltKeyName.Buffer);
#else
	ExFreePool(FltKeyName);
#endif //_HOOK_NT_
	ExFreePool(pTmpFlt);
	ExFreePool(pClientRec);
	DbPrint(DC_FILTER,DL_NOTIFY, ("Filters Saved. Last status=%x\n",ntstatus));
	return TRUE;
}

// Don't forget check driver's version!!!!!!!!!!!
// located in "INIT" section, look for #pragma alloc_text("INIT",...)
NTSTATUS LoadFilters()
{
#ifdef _HOOK_NT_
OBJECT_ATTRIBUTES	objectAttributes;
UNICODE_STRING		FltKeyName;
UNICODE_STRING		ValueName;
WCHAR					ValueBuff[10];
ULONG					Res;
#else 
CHAR					ValueBuff[10];
ULONG					DataSize;
#endif //_HOOK_NT_
NTSTATUS				ntstatus;
HKEY					ParKeyHandle;
HKEY					KeyHandle;
HKEY					FltKeyHandle;
ULONG					i,KeyIndex=0,AppID;
PVOID					LoadBuff=NULL;
PFILTER_TRANSMIT		pTmpFlt=NULL;
PCHAR					FltKeyNameBuff = NULL;

	if(!((LoadBuff=ExAllocatePoolWithTag(PagedPool,PAGE_SIZE,'fSeB')) &&
		(FltKeyNameBuff=ExAllocatePoolWithTag(PagedPool,MAXPATHLEN,'fSeB')))) {
		if(FltKeyNameBuff)
			ExFreePool(FltKeyNameBuff);
		if(LoadBuff)
			ExFreePool(LoadBuff);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
#ifdef _HOOK_NT_
	InitializeObjectAttributes(&objectAttributes,&RegParams,OBJ_CASE_INSENSITIVE,NULL,NULL);
	if((ntstatus=ZwOpenKey(&ParKeyHandle,KEY_READ,&objectAttributes))==STATUS_SUCCESS) {
		while(ZwEnumerateKey(ParKeyHandle,KeyIndex++,KeyBasicInformation,FltKeyNameBuff,MAXPATHLEN,&Res)==STATUS_SUCCESS) {
			((PKEY_BASIC_INFORMATION)FltKeyNameBuff)->Name[((PKEY_BASIC_INFORMATION)FltKeyNameBuff)->NameLength/sizeof(WCHAR)]=0;
			RtlInitUnicodeString(&FltKeyName,((PKEY_BASIC_INFORMATION)FltKeyNameBuff)->Name);
			FltKeyName.MaximumLength=MAXPATHLEN;
			if(RtlUnicodeStringToInteger(&FltKeyName,0,&AppID)==STATUS_SUCCESS) { // MS bug -- Win2k/chk always return STATUS_SUCCESS
				InitializeObjectAttributes(&objectAttributes,&FltKeyName,OBJ_CASE_INSENSITIVE,ParKeyHandle,NULL);
				if((ntstatus=ZwOpenKey(&KeyHandle,KEY_READ,&objectAttributes))==STATUS_SUCCESS) {
					RtlInitUnicodeString(&ValueName,BldValBuffer);
					if((ntstatus=ZwQueryValueKey(KeyHandle,&ValueName,KeyValuePartialInformation,LoadBuff,PAGE_SIZE,&Res))==STATUS_SUCCESS) {
						if(*(ULONG*)(((PKEY_VALUE_PARTIAL_INFORMATION)LoadBuff)->Data)==HOOK_REGESTRY_INTERFACE_NUM) {
							RtlInitUnicodeString(&ValueName,LogFNameValBuffer);
							if((ntstatus=ZwQueryValueKey(KeyHandle,&ValueName,KeyValuePartialInformation,LoadBuff,PAGE_SIZE,&Res))==STATUS_SUCCESS) {
								if(RegisterAppFromSav(AppID,(PCLIENT_SAV)(((PKEY_VALUE_PARTIAL_INFORMATION)LoadBuff)->Data))) {
									DbPrint(DC_FILTER,DL_NOTIFY, ("Try to load filters for %d\n", AppID));
									if((ntstatus=RtlAppendUnicodeStringToString(&FltKeyName,&FiltersKeyName))==STATUS_SUCCESS) {
										InitializeObjectAttributes(&objectAttributes,&FltKeyName,OBJ_CASE_INSENSITIVE,ParKeyHandle,NULL);
										if((ntstatus=ZwCreateKey(&FltKeyHandle,KEY_WRITE,&objectAttributes,0,NULL,0,NULL))==STATUS_SUCCESS) {

#else
	if((ntstatus=RegCreateKey(HKEY_LOCAL_MACHINE,RegParams,&ParKeyHandle))==ERROR_SUCCESS) {
		while(RegEnumKey(ParKeyHandle,KeyIndex++,FltKeyNameBuff,MAXPATHLEN)==ERROR_SUCCESS) {
			if((AppID=atol(FltKeyNameBuff))!=0) {
				if((ntstatus=RegCreateKey(ParKeyHandle,FltKeyNameBuff,&KeyHandle))==ERROR_SUCCESS) {
					DataSize=sizeof(ULONG);
					if((ntstatus=RegQueryValueEx(KeyHandle,BldValBuffer,NULL,NULL,LoadBuff,&DataSize))==ERROR_SUCCESS) {
						if(*(ULONG*)LoadBuff==HOOK_REGESTRY_INTERFACE_NUM) {
							DataSize=sizeof(CLIENT_SAV);
							if((ntstatus=RegQueryValueEx(KeyHandle,LogFNameValBuffer,NULL,NULL,LoadBuff,&DataSize))==ERROR_SUCCESS) {
								if(RegisterAppFromSav(AppID,(PCLIENT_SAV)LoadBuff)) {
									DbPrint(DC_FILTER,DL_NOTIFY, ("Try to load filters for %d", AppID));
									if(strcat(FltKeyNameBuff,FiltersKeyName)) {
										if((ntstatus=RegCreateKey(ParKeyHandle,FltKeyNameBuff,&FltKeyHandle))==ERROR_SUCCESS) {
#endif //_HOOK_NT_
											i = 0;
											do {
#ifdef _HOOK_NT_
												swprintf(ValueBuff,L"%09u",i++);
												RtlInitUnicodeString(&ValueName,ValueBuff);
												if((ntstatus=ZwQueryValueKey(FltKeyHandle,&ValueName,KeyValuePartialInformation,LoadBuff,PAGE_SIZE,&Res))==STATUS_SUCCESS) {
													DbPrint(DC_FILTER,DL_INFO, ("Read Filter %S. Status=%x\n",ValueName.Buffer,ntstatus));
													pTmpFlt=(PFILTER_TRANSMIT) &((PKEY_VALUE_PARTIAL_INFORMATION)LoadBuff)->Data;
#else

												sprintf(ValueBuff,"%09u",i++);
												DataSize=PAGE_SIZE;
												if((ntstatus=RegQueryValueEx(FltKeyHandle,ValueBuff,NULL,NULL,LoadBuff,&DataSize))==ERROR_SUCCESS) {
													DbPrint(DC_FILTER,DL_INFO, ("Read Filter %s. Status=%x\n",ValueBuff,ntstatus));
													pTmpFlt=(PFILTER_TRANSMIT)LoadBuff;

#endif //_HOOK_NT_
													_AddFilter(pTmpFlt, _QUEUE_BOTTOM);
												}
#ifdef _HOOK_NT_
											} while (ntstatus==STATUS_SUCCESS);
											ZwClose(FltKeyHandle);
#else
											} while (ntstatus==ERROR_SUCCESS);
											RegCloseKey(FltKeyHandle);
#endif //_HOOK_NT_
										} else {
											DbPrint(DC_FILTER,DL_ERROR, ("ZwCreateKey failed. Status=%x\n",ntstatus));
										}
									} else {
										DbPrint(DC_FILTER,DL_ERROR, ("RtlAppendUnicodeStringToString failed. Status=%x\n",ntstatus));
									}
								}
							} else {
								DbPrint(DC_FILTER,DL_ERROR, ("ZwQueryValueKey failed. Status=%x\n",ntstatus));
							}
#ifdef _HOOK_NT_
						} else {
							DbPrint(DC_FILTER,DL_ERROR, ("Settings for unsupported build num %d\n",(ULONG)(((PKEY_VALUE_PARTIAL_INFORMATION)LoadBuff)->Data)));
						}
					} else {
						DbPrint(DC_FILTER,DL_ERROR, ("RegQueryValueEx failed. Status=%x\n",ntstatus));
					}
					ZwClose(KeyHandle);
#else
						} else {
							DbPrint(DC_FILTER,DL_ERROR, ("Settings for unsupported build num %d\n",(ULONG)LoadBuff));
						}
					} else {
						DbPrint(DC_FILTER,DL_ERROR, ("RegQueryValueEx failed. Status=%x\n",ntstatus));
					}
					RegCloseKey(KeyHandle);
#endif //_HOOK_NT_
				} else {
					DbPrint(DC_FILTER,DL_ERROR, ("ZwCreateKey failed. Status=%x\n",ntstatus));
				}
			} else {
				DbPrint(DC_FILTER,DL_ERROR, ("RtlUnicodeStringToInteger failed. Status=%x\n",ntstatus));
			}
		}
#ifdef _HOOK_NT_
		ZwClose(ParKeyHandle);
#else
		RegCloseKey(ParKeyHandle);
#endif //_HOOK_NT_
	} else {
		DbPrint(DC_FILTER,DL_ERROR, ("ZwCreateKey failed. Status=%x\n",ntstatus));
	}
	ExFreePool(FltKeyNameBuff);
	ExFreePool(LoadBuff);
	DbPrint(DC_FILTER,DL_NOTIFY, ("Filters Loaded. Last status=%x\n",ntstatus));
	DbPrint(DC_FILTER,DL_IMPORTANT, ("Applications: gActiveClientCounter=%d\n", gActiveClientCounter));
	return ntstatus;
}
								
// -----------------------------------------------------------------------------------------
NTSTATUS
FilterTransmit(PFILTER_TRANSMIT pFilterTransmitIn, PFILTER_TRANSMIT pFilterTransmitOut, ULONG OutputBufferLength)
{
	NTSTATUS Status = STATUS_SUCCESS;
	PQCLIENT pClient = AcquireClient(pFilterTransmitIn->m_AppID);
	if (pClient == NULL)
	{
		DbPrint(DC_FILTER,DL_WARNING, ("IOCTLHOOK_FiltersCtl - Client not found(%d)\n", pFilterTransmitIn->m_AppID));
		Status = STATUS_NOT_FOUND;
	}
	else
	{
		ULONG StartFilterID;

		ReleaseClient(pClient);
		
		switch(pFilterTransmitIn->m_FltCtl)
		{
		case FLTCTL_FIRST:
			StartFilterID = -1;
			if (!_GetNextFilter(pFilterTransmitIn->m_AppID, &StartFilterID, pFilterTransmitOut, OutputBufferLength, 
				NULL, &Status))
			{	// -1 - first filter
				DbPrint(DC_FILTER,DL_WARNING, ("IOCTLHOOK_FiltersCtl - GetFirst error\n"));
				Status = STATUS_NOT_FOUND;
			}
			break;
		case FLTCTL_NEXT:
			StartFilterID = pFilterTransmitIn->m_FilterID;
			if (!_GetNextFilter(pFilterTransmitIn->m_AppID, &StartFilterID, pFilterTransmitOut, 
				OutputBufferLength, NULL, &Status))
			{	// -1 - first filter
				DbPrint(DC_FILTER,DL_WARNING, ("IOCTLHOOK_FiltersCtl - GetNext error\n"));
				Status = STATUS_NOT_FOUND;
			}
			break;
		case FLTCTL_ADD:
			if (!_AddFilter(pFilterTransmitIn, _QUEUE_BOTTOM))
			{
				DbPrint(DC_FILTER,DL_ERROR, ("IOCTLHOOK_FiltersCtl - error during add new filter\n"));
				Status = STATUS_MEMORY_NOT_ALLOCATED;
			}
			if (pFilterTransmitOut)
				pFilterTransmitOut->m_FilterID = pFilterTransmitIn->m_FilterID;
			break;
		case FLTCTL_ADD2TOP:
			if (!_AddFilter(pFilterTransmitIn, _QUEUE_TOP))
			{
				DbPrint(DC_FILTER,DL_ERROR, ("IOCTLHOOK_FiltersCtl - error during add new filter at the top\n"));
				Status = STATUS_MEMORY_NOT_ALLOCATED;
			}
			if (pFilterTransmitOut)
				pFilterTransmitOut->m_FilterID = pFilterTransmitIn->m_FilterID;
			break;
		case FLTCTL_ADD2POSITION:
			if (!_AddFilter(pFilterTransmitIn, _QUEUE_POSITION))
			{
				DbPrint(DC_FILTER,DL_ERROR, ("IOCTLHOOK_FiltersCtl - error during add new filter at the specified position\n"));
				Status = STATUS_MEMORY_NOT_ALLOCATED;
			}
			if (pFilterTransmitOut)
				pFilterTransmitOut->m_FilterID = pFilterTransmitIn->m_FilterID;
			break;
		case FLTCTL_DEL:
			if (!_DelFilter(pFilterTransmitIn->m_FilterID, NULL, TRUE))
			{
				DbPrint(DC_FILTER,DL_ERROR, ("IOCTLHOOK_FiltersCtl - can't delete filter\n"));
				Status = STATUS_UNSUCCESSFUL;
			}
			break;
			/*case FLTCTL_FIND:
			if (_IsFiltered((PFILTER_TRANSMIT)InputBuffer, ((PFILTER_TRANSMIT)InputBuffer)->m_AppID, 
			_SYSTEM_APPLICATION, &pQFltTmp, TRUE) == _SYSTEM_APPLICATION) {
			Status = STATUS_NOT_FOUND;
			return TRUE;
			}
			break;*/
		case FLTCTL_DISABLE_FILTER:
			if (!_FilterSwitch(pFilterTransmitIn->m_AppID, pFilterTransmitIn->m_FilterID, FALSE))
			{
				DbPrint(DC_FILTER,DL_ERROR, ("IOCTLHOOK_FiltersCtl - can't disable filter\n"));
				Status = STATUS_UNSUCCESSFUL;
			}
			break;
		case FLTCTL_ENABLE_FILTER:
			if (!_FilterSwitch(pFilterTransmitIn->m_AppID, pFilterTransmitIn->m_FilterID, TRUE))
			{
				DbPrint(DC_FILTER,DL_ERROR, ("IOCTLHOOK_FiltersCtl - can't enable filter\n"));
				Status = STATUS_UNSUCCESSFUL;
			}
			break;
		case FLTCTL_RESET_FILTERS:
			pClient = AcquireClient(pFilterTransmitIn->m_AppID);
			if (pClient != 0)
			{
				_FreeAppFilters(pClient);
				ReleaseClient(pClient);
			}
			break;
		case FLTCTL_SET_FILTER_GROUP_ID:
			if (INVALID_GROUP_ID == pFilterTransmitIn->m_HookID)
			{
				Status = STATUS_INVALID_PARAMETER;
				DbgBreakPoint();
			}
			else
			{
				if (!_FilterSetGroupID(pFilterTransmitIn->m_AppID, pFilterTransmitIn->m_FilterID, pFilterTransmitIn->m_HookID))
					Status = STATUS_NOT_FOUND;
			}
			break;
		case FLTCTL_CHANGE_FILTER_PARAM:
			Status = _FilterChangeParams(pFilterTransmitIn->m_AppID, pFilterTransmitIn->m_FilterID, 
				pFilterTransmitIn->m_ParamsCount, (FILTER_PARAM*) pFilterTransmitIn->m_Params);
			break;
		default:
			Status = STATUS_NOT_FOUND;
			break;
		}
	}
	return Status;
}

void InitStartFiltering(PEXT_START_FILTERING pStartFiltering)
{
	pStartFiltering->m_FltVersion = HOOK_INTERFACE_NUM;
	pStartFiltering->m_pIsNeedFilterEvent = IsNeedFilterEvent;
	pStartFiltering->m_pFilterEvent = (FILTER_EVENT_FUNC) FilterEvent;
	pStartFiltering->m_pIsNeedFilterEventEx = IsNeedFilterEventEx;
}

// --- Dispatch filtering
#ifdef _HOOK_NT_

BOOLEAN Di_FilterParams(PDI_FILTER pDiFilter, PFILTER_EVENT_PARAM pEventParam)
{
	BOOLEAN bRet = TRUE;
	ULONG cou;
	BYTE *pPointer;
	BOOLEAN bParamFound;
	
	PFILTER_PARAM pFilterParam = (PFILTER_PARAM) pDiFilter->m_Params;

	for (cou = 0; (cou < pDiFilter->m_ParamsCount) && bRet; cou++)
	{
		if ((pFilterParam->m_ParamFlt != FLT_PARAM_NOP) ||
			pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_MUSTEXIST)
		{
			bRet = CheckSingleParam(pEventParam, pFilterParam, NULL, &bParamFound);
			if (bRet && !bParamFound)
			{
				if (pFilterParam->m_ParamFlags & _FILTER_PARAM_FLAG_MUSTEXIST)
					bRet = FALSE;
			}
		}
		
		pPointer = (BYTE*)pFilterParam + sizeof(FILTER_PARAM) + pFilterParam->m_ParamSize;
		pFilterParam = (PFILTER_PARAM) pPointer;
	}

	return bRet;
}

typedef struct _tHintArr
{
	DWORD m_DispHint;
	VERDICT m_DispHintVerdict;
}sHintArr;

sHintArr gHintArr[32];
DWORD gHintPos;

#define _countof(_arr) (sizeof(_arr) / sizeof(_arr[0]))

void
DI_ResetHintArr (
	)
{
	memset(gHintArr, 0, sizeof(gHintArr));
	gHintPos = 0;
}

ULONG gDHinOk = 0;
ULONG gDHinFail = 0;

#ifdef __DBG__
	BOOLEAN gDumpFilters = FALSE;
#endif


VERDICT Di_FilterEvent(PFILTER_EVENT_PARAM pEventParam, PEVENT_OBJECT_INFO pEventObject)
{
	VERDICT Verdict = Verdict_NotFiltered;
	KIRQL NewIrql;
//	ULONG hint;
	ULONG cou;

#ifdef __DBG__
	BOOLEAN bStop = FALSE;
#endif

//	hint = Filter_GetHint(pEventParam);

	KeAcquireSpinLock(&gDi_SpinLock, &NewIrql);

/*	if (hint)
	{
		for (cou = 0; cou < _countof(gHintArr); cou++)
		{
			if (hint == gHintArr[cou].m_DispHint)
			{
				VERDICT vertmp = gHintArr[cou].m_DispHintVerdict;
				gDHinOk++;
				KeReleaseSpinLock(&gDi_SpinLock, NewIrql);
				return vertmp;
			}
		}
	}*/

	if (!IsListEmpty(&gDi_FltList))
	{
		BOOLEAN bMatch;
		PDI_FILTER pDiFilter;
		PIMPLIST_ENTRY Flink;
		Flink = gDi_FltList.Flink;

		while((Flink != &gDi_FltList) && (Verdict_NotFiltered == Verdict))
		{
			pDiFilter = (PDI_FILTER) Flink;
			Flink = Flink->Flink;

			if (pDiFilter->m_bClientPaused || (pDiFilter->m_Flags & FLT_A_DISABLED))
				continue;
			if ((pDiFilter->m_HookID != pEventParam->HookID) || 
				(pDiFilter->m_FunctionMj != pEventParam->FunctionMj) ||
				pDiFilter->m_FunctionMi != pEventParam->FunctionMi)
			{
				continue;
			}

			bMatch = Di_FilterParams(pDiFilter, pEventParam);
#ifdef __DBG__
			if (gDumpFilters)
			{
				DumpFilter(pDiFilter->m_FilterID, pDiFilter->m_AppID, pDiFilter->m_FunctionMj, pDiFilter->m_FunctionMi,
					pDiFilter->m_ParamsCount, (PFILTER_PARAM) pDiFilter->m_Params);
			}
#endif
			if (bMatch)
			{
				if (pDiFilter->m_Flags & (FLT_A_POPUP | FLT_A_LOG | FLT_A_NOTIFY | FLT_A_INFO))
					Verdict = Verdict_UserRequest;
				else
				{
					if (pDiFilter->m_Flags & FLT_A_DENY)
						Verdict = Verdict_Discard;
					else if (pDiFilter->m_Flags & FLT_A_PASS)
						Verdict = Verdict_Pass;
					else
						Verdict = Verdict_Default;
				}
			}
		}
	}

/*	if (hint)
	{
		gHintArr[gHintPos].m_DispHintVerdict = Verdict;
		gHintArr[gHintPos].m_DispHint = hint;

		gDHinFail++;
		gHintPos++;

		if (gHintPos == _countof(gHintArr))
			gHintPos = 0;
	}*/

#ifdef __DBG__
	gDumpFilters = FALSE;

	if (bStop)
	{
		DbgBreakPoint();
	}
#endif

	KeReleaseSpinLock(&gDi_SpinLock, NewIrql);

	return Verdict;
}

BOOLEAN Di_FltCheckParams(PFILTER_TRANSMIT pQFlt)
{
	PFILTER_PARAM pParam;
	BYTE *pPointer;
	DWORD tmp;

	ULONG cou;

	pParam = (PFILTER_PARAM) pQFlt->m_Params;
	
	for (cou = 0; cou < pQFlt->m_ParamsCount; cou++)
	{
		if (pParam->m_ParamFlt == FLT_PARAM_WILDCARD)
		{
			DbgBreakPoint();
			return FALSE;
		}

		tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		pPointer = (BYTE*)pParam + tmp;
		pParam = (PFILTER_PARAM) pPointer;
	}

	return TRUE;
}

PDI_FILTER Di_AllocateFilter(PFILTER_TRANSMIT pFltIn)
{
	PDI_FILTER pDiFilter = NULL;

	ULONG ParamSize = 0;
	
	ParamSize = CalcFilterSize(pFltIn) - sizeof(FILTER_TRANSMIT);

	pDiFilter = ExAllocatePoolWithTag(NonPagedPool, ParamSize + sizeof(DI_FILTER), 'DSOB');
	if (!pDiFilter)
		return pDiFilter;

	InterlockedIncrement(&gDi_Allocs);

	pDiFilter->m_HookID = pFltIn->m_HookID;
	pDiFilter->m_FunctionMj = pFltIn->m_FunctionMj;
	pDiFilter->m_FunctionMi = pFltIn->m_FunctionMi;
	pDiFilter->m_AppID = pFltIn->m_AppID;
	pDiFilter->m_bClientPaused = FALSE;
	pDiFilter->m_FilterID = 0;
	pDiFilter->m_Flags = pFltIn->m_Flags;
	pDiFilter->m_ParamsCount = pFltIn->m_ParamsCount;

	memcpy(pDiFilter->m_Params, pFltIn->m_Params, ParamSize);
	PrepareDiFilterParams(pDiFilter);

	return pDiFilter;
}

void Di_FreeFlt(PDI_FILTER pDiFilter)
{
	ExFreePool(pDiFilter);
	InterlockedDecrement(&gDi_Allocs);
}

void Di_AddFilter(PDI_FILTER pDiFilter, eQUEUE_VAL eSite, ULONG FilterID)
{
	KIRQL NewIrql;

	KeAcquireSpinLock(&gDi_SpinLock, &NewIrql);

	DI_ResetHintArr();

	if (eSite == _QUEUE_POSITION)
	{
		DbgBreakPoint();
	}

	pDiFilter->m_FilterID = FilterID;

	switch (eSite)
	{
	case _QUEUE_TOP:
		_impInsertHeadList(&gDi_FltList, &pDiFilter->m_List);
		break;
	case _QUEUE_BOTTOM:
		_impInsertTailList(&gDi_FltList, &pDiFilter->m_List);
		break;
	case _QUEUE_POSITION:
		DbgBreakPoint();
		break;
	}

	KeReleaseSpinLock(&gDi_SpinLock, NewIrql);
}

void Di_DelFilter(ULONG FilterID)
{
	KIRQL NewIrql;
	KeAcquireSpinLock(&gDi_SpinLock, &NewIrql);

	DI_ResetHintArr();

	if (!IsListEmpty(&gDi_FltList))
	{
		PDI_FILTER pDiFilter;
		PIMPLIST_ENTRY Flink;
		Flink = gDi_FltList.Flink;

		while(Flink != &gDi_FltList)
		{
			pDiFilter = (PDI_FILTER) Flink;
			Flink = Flink->Flink;

			if (pDiFilter->m_FilterID == FilterID)
			{
				_impRemoveEntryList(&pDiFilter->m_List);
				Di_FreeFlt(pDiFilter);
			}
		}
	}

	KeReleaseSpinLock(&gDi_SpinLock, NewIrql);
}

void Di_ResetFiltersForApp(ULONG AppID)
{
	KIRQL NewIrql;
	KeAcquireSpinLock(&gDi_SpinLock, &NewIrql);

	DI_ResetHintArr();

	if (!IsListEmpty(&gDi_FltList))
	{
		PDI_FILTER pDiFilter;
		PIMPLIST_ENTRY Flink;
		Flink = gDi_FltList.Flink;

		while(Flink != &gDi_FltList)
		{
			pDiFilter = (PDI_FILTER) Flink;
			Flink = Flink->Flink;

			if (pDiFilter->m_AppID == AppID)
			{
				_impRemoveEntryList(&pDiFilter->m_List);
				Di_FreeFlt(pDiFilter);
			}
		}
	}

	KeReleaseSpinLock(&gDi_SpinLock, NewIrql);
}

void Di_FilterSwitch(ULONG AppID, ULONG FilterID, BOOLEAN bEnable)
{
	KIRQL NewIrql;
	KeAcquireSpinLock(&gDi_SpinLock, &NewIrql);

	DI_ResetHintArr();

	if (!IsListEmpty(&gDi_FltList))
	{
		PDI_FILTER pDiFilter;
		PIMPLIST_ENTRY Flink;
		Flink = gDi_FltList.Flink;

		while(Flink != &gDi_FltList)
		{
			pDiFilter = (PDI_FILTER) Flink;
			Flink = Flink->Flink;

			if ((pDiFilter->m_AppID == AppID) && (pDiFilter->m_FilterID == FilterID))
			{
				if (bEnable)
					pDiFilter->m_Flags &= ~FLT_A_DISABLED;
				else
					pDiFilter->m_Flags |= FLT_A_DISABLED;
			}
		}
	}

	KeReleaseSpinLock(&gDi_SpinLock, NewIrql);
}

void Di_PauseClient(ULONG AppID, BOOLEAN bGoSleep)
{
	KIRQL NewIrql;
	KeAcquireSpinLock(&gDi_SpinLock, &NewIrql);

	if (!IsListEmpty(&gDi_FltList))
	{
		PDI_FILTER pDiFilter;
		PIMPLIST_ENTRY Flink;
		Flink = gDi_FltList.Flink;

		while(Flink != &gDi_FltList)
		{
			pDiFilter = (PDI_FILTER) Flink;
			Flink = Flink->Flink;

			if (pDiFilter->m_AppID == AppID)
			{
				if (bGoSleep)
					pDiFilter->m_bClientPaused = TRUE;
				else
					pDiFilter->m_bClientPaused = FALSE;
			}
		}
	}

	KeReleaseSpinLock(&gDi_SpinLock, NewIrql);
}

#endif

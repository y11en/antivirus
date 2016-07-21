// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  07 December 2005,  12:15 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- PPP
// Purpose     -- Not defined
// Author      -- Mike Pavlyushchik
// File Name   -- scheduler.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Scheduler_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <ProductCore/plugin/p_scheduler.h>
// AVP Prague stamp end



#include <Prague/plugin/p_timer.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_ktrace.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_event.h>
#include <Prague/iface/i_mutex.h>
#include <Prague/plugin/p_thpoolimp.h>

#include <ProductCore/structs/s_scheduler.h>
#include <Prague/pr_serializable.h>


typedef struct tag_SCHEDULE_EX {
	cScheduleSettings* m_pSchedule;
	cSerializable* m_pScheduleParams;
	tDWORD    m_dwTimeShiftSeconds;
	tDWORD    m_dwTimeShiftMonths;
	tDWORD    m_dwTimeShiftDayOfMonth;
	tDWORD    m_dwTimeStartDeltaRandom; // randomized number of seconds for start delta, recalculated on each shift
	tDWORD    m_dwAllowWindowTimeShiftSeconds;
	tDWORD    m_dwAllowWindowTimeShiftMonths;
	tDATETIME m_dtLastShiftedTime;
	tBOOL     m_bMarkedForDelete;
	tLONG     m_nRunningJobs;
//	hSYNC_EVENT m_hDeleteSync;
	tDWORD    m_dwTID;
} tSCHEDULE_EX;

tDATETIME dtNull;
tDATETIME dtTimeTBase;

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable Scheduler : public cScheduler
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations
	tERROR pr_call SetFrequency( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call SetSettings( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call SetSchedule( tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID, const cSerializable* p_pSchedule, const cSerializable* p_pScheduleParams );
	tERROR pr_call GetSchedule( tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID, cSerializable* p_pSchedule, cSerializable* p_pScheduleParams );
	tERROR pr_call DeleteSchedule( tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID );
	tERROR pr_call GetNextSchedule( tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID, cSerializable* p_pSchedule, cSerializable* p_pScheduleParams );

// Data declaration
	hREGISTRY m_hRegistry; // --
	tDWORD    m_dwFrequency; // --
// AVP Prague stamp end



	hTIMER m_hTimer;
	tDWORD m_dwTimerID;
	hCRITICAL_SECTION m_hSync;
	tDATETIME m_dtLastTime;
	hTHREADPOOL m_hThPool;

	tSCHEDULE_EX**  m_arrSchedule;
	tDWORD m_dwSchedulesCount;
	
	tERROR LoadSettings();
	tERROR SaveSettings();
	tBOOL  VerifyRaiseIfSkipped(cScheduleSettings* pSchedule);
	tERROR StoreLastRunTime(cScheduleSettings* pSchedule);

	tERROR TimeShiftNext(tSCHEDULE_EX* pScheduleEx, tDATETIME* pdtNow);
	tERROR TimeShiftForward(tSCHEDULE_EX* pScheduleEx, tDATETIME* pdtNow);
	tERROR FindSchedule( tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID, tDWORD* pdwIndex );
	tERROR FindNextSchedule( tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID, tDWORD* pdwIndex );
	tERROR CreateScheduleEx( tSCHEDULE_EX** ppScheduleEx );
	tERROR AdjustTimes( tSCHEDULE_EX* pScheduleEx, tDATETIME* pdtNow );
	tERROR DestroyScheduleEx(tSCHEDULE_EX* pScheduleEx, tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID, tDWORD dwTimeout);
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(Scheduler)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Scheduler". Static(shared) property table variables
// AVP Prague stamp end



#if defined (_WIN32 ) && ( defined(_DEBUG) || defined(_PRAGUE_TRACE_) )
#include <windows.h>

tVOID DateTime2SystemTime(tDATETIME* pDT, SYSTEMTIME* pST)
{
	tDWORD dwYear, dwMonth, dwDay, dwHour, dwMin, dwSec, dwNanosec;
	//GetSystemTime(pST);
	DTGet(pDT, &dwYear, &dwMonth, &dwDay, &dwHour, &dwMin, &dwSec, &dwNanosec );
	pST->wYear = (tWORD)dwYear;
	pST->wMonth = (tWORD)dwMonth;
	pST->wDay = dwDay;
	pST->wHour = dwHour;
	pST->wMinute = dwMin;
	pST->wSecond = dwSec;
	pST->wMilliseconds = dwNanosec/1000000;
	DTWeekDay(pDT, cTRUE);
}
#endif

tERROR pr_call ThreadCallback(tThreadCallType CallType, tPTR pCommonThreadContext, tPTR* ppThreadContext)
{
	Scheduler* pScheduler = (Scheduler*)pCommonThreadContext;
	tERROR error = errOK;
	switch(CallType)
	{
	case TP_CBTYPE_THREAD_INIT_CONTEXT:
		PR_TRACE((pScheduler, prtNOTIFY, "schd\tScheduler: new working thread %x added into pool", PrGetThreadId()));
		*ppThreadContext = pScheduler;
		break;
	case TP_CBTYPE_THREAD_DONE_CONTEXT:
		PR_TRACE((pScheduler, prtNOTIFY, "schd\tScheduler: working thread %x exiting", PrGetThreadId()));
		break;
	case TP_CBTYPE_THREAD_YIELD:
		break;
	}
	return error;
}

tERROR pr_call ThreadTaskCallback(tThreadCallType CallType, tPTR pThreadContext, tPTR* ppTaskContext, tPTR pTaskData, tDWORD TaskDataLen)
{
	tERROR error = errOK;
	Scheduler* pScheduler = (Scheduler*)pThreadContext;
	tSCHEDULE_EX* pScheduleEx = *(tSCHEDULE_EX**) pTaskData;
	
	switch(CallType)
	{
	case TP_CBTYPE_TASK_INIT_CONTEXT:
		pScheduleEx->m_dwTID = PrGetThreadId();
		PR_TRACE((pScheduler, prtNOTIFY, "schd\tSchedule %08X:%08X TASK_INIT_CONTEXT on thread %x jobs=%d", pScheduleEx->m_pSchedule->m_dwScheduleClass, pScheduleEx->m_pSchedule->m_dwScheduleID, pScheduleEx->m_dwTID, pScheduleEx->m_nRunningJobs));
		break;
	case TP_CBTYPE_TASK_PROCESS:
		PR_TRACE((pScheduler, prtNOTIFY, "schd\tSchedule %08X:%08X alerting asynchronously on thread %x", pScheduleEx->m_pSchedule->m_dwScheduleClass, pScheduleEx->m_pSchedule->m_dwScheduleID, PrGetThreadId()));
		error = pScheduler->sysSendMsg(pScheduleEx->m_pSchedule->m_dwScheduleClass, pScheduleEx->m_pSchedule->m_dwScheduleID, NULL, pScheduleEx->m_pScheduleParams, SER_SENDMSG_PSIZE);
		PR_TRACE((pScheduler, prtNOTIFY, "schd\tSchedule %08X:%08X done on thread %x with %terr", pScheduleEx->m_pSchedule->m_dwScheduleClass, pScheduleEx->m_pSchedule->m_dwScheduleID, PrGetThreadId(), error));
		break;
	case TP_CBTYPE_TASK_DONE_CONTEXT:
		{
			if (!pScheduler)
			{
				PR_TRACE((pScheduler, prtERROR, "schd\tTASK_DONE_CONTEXT on thread %x, pThreadContext(pScheduler)=NULL", PrGetThreadId()));
				return errOK;
			}
			if (!pScheduleEx)
			{
				PR_TRACE((pScheduler, prtERROR, "schd\tTASK_DONE_CONTEXT on thread %x, pTaskData(pScheduleEx)=NULL", PrGetThreadId()));
				return errOK;
			}
			
			pScheduler->m_hSync->Enter(SHARE_LEVEL_WRITE);
			tDWORD dwClass, dwID;
			tBOOL bMarkedForDelete = pScheduleEx->m_bMarkedForDelete;
			pScheduleEx->m_nRunningJobs--;
			if (bMarkedForDelete)
			{
				dwClass = pScheduleEx->m_pSchedule->m_dwScheduleClass;
				dwID    = pScheduleEx->m_pSchedule->m_dwScheduleID;
			}
			PR_TRACE((pScheduler, prtNOTIFY, "schd\tSchedule %08X:%08X TASK_DONE_CONTEXT on thread %x, jobs=%d", pScheduleEx->m_pSchedule->m_dwScheduleClass, pScheduleEx->m_pSchedule->m_dwScheduleID, pScheduleEx->m_dwTID, pScheduleEx->m_nRunningJobs));
			pScheduler->m_hSync->Leave(NULL);

			if (bMarkedForDelete)
				pScheduler->DestroyScheduleEx(0, dwClass, dwID, 0);
		}
		break;
	}
	return error;
}

tERROR Scheduler::LoadSettings()
{
	tERROR error = errOK;
	tDWORD i;
	tRegKey SubKey;
	tDATETIME dtNow;
	tTYPE_ID tid = tid_DWORD;

	if (!m_hRegistry)
		return errNOT_OK;

	cAutoCS _lock(m_hSync, cTRUE);

	Now(&dtNow);
	error = m_hRegistry->GetValue(NULL, cRegRoot, "Count", &tid, &m_dwSchedulesCount, sizeof(tDWORD));
	if (PR_SUCC(error) && m_dwSchedulesCount) 
	{
		if (PR_SUCC(error = heapRealloc((tPTR*)&m_arrSchedule, m_arrSchedule, m_dwSchedulesCount*sizeof(tSCHEDULE_EX*))))
		{
			for (i=0; i<m_dwSchedulesCount; i++)	
			{
				if (PR_SUCC(error = heapAlloc((tPTR*)&m_arrSchedule[i], sizeof(tSCHEDULE_EX))))
				{
					tCHAR szKeyName[0x10];
					pr_sprintf(szKeyName, sizeof(szKeyName), "S%u", i);
					if (PR_SUCC(error = m_hRegistry->OpenKey(&SubKey, cRegRoot, szKeyName, cFALSE)))
					{
						error = ::g_root->RegDeserialize( (cSerializable**)&m_arrSchedule[i]->m_pSchedule, m_hRegistry, "Schedule", cScheduleSettings::eIID );
						
						if (PR_SUCC(error))
						{
							tUINT uniq;
							tid = tid_UINT;
							error = m_hRegistry->GetValue(NULL, SubKey, "ParamsUniq", &tid, &uniq, sizeof(uniq));
							if (PR_SUCC(error))
								error = ::g_root->RegDeserialize( (cSerializable**)&m_arrSchedule[i]->m_pScheduleParams, m_hRegistry, "Params", uniq );
						}
	
						if (PR_SUCC(error))
						{
							tid = tid_DATETIME;
							error = m_hRegistry->GetValue(NULL, SubKey, "LastModified", &tid, &m_arrSchedule[i]->m_dtLastShiftedTime, sizeof(tDATETIME));
						}
						
						if (PR_SUCC(error))
							error = AdjustTimes(m_arrSchedule[i], &dtNow);
						m_hRegistry->CloseKey(SubKey);
	
					}
					if (PR_FAIL(error)) // error during schedule load
						DestroyScheduleEx(m_arrSchedule[i], 0, 0, 0);
				}
			}
		}
	}
	return error;
}


tERROR Scheduler::SaveSettings()
{
	tERROR error = errOK;
	tDWORD i;
	tDWORD dwCurSchedule = 0;
	tRegKey SubKey;

	if (!m_hRegistry)
		return errNOT_OK;
	
	cAutoCS _lock(m_hSync, cTRUE);

	error = m_hRegistry->Clean();

	if (PR_SUCC(error))
		error = m_hRegistry->SetValue(cRegRoot, "Count", tid_DWORD, &dwCurSchedule, sizeof(tDWORD), cTRUE);

	if (PR_SUCC(error)) // write 0 here!
	{
		for (i=0; i<m_dwSchedulesCount; i++)	
		{
			tCHAR szKeyName[0x10];
			pr_sprintf(szKeyName, sizeof(szKeyName), "S%u", dwCurSchedule);
			
			if (!m_arrSchedule[i]) // schedule deleted
				continue;
			if (m_arrSchedule[i]->m_bMarkedForDelete) // schedule deleted
				continue;
			if (!m_arrSchedule[i]->m_pSchedule) // schedule deleted
				continue;

			if (PR_SUCC(error))
				error = m_hRegistry->OpenKey(&SubKey, cRegRoot, szKeyName, cTRUE);

			if (PR_SUCC(error))
			{
				error = m_hRegistry->SetRootKey(SubKey, cFALSE);
				if (PR_SUCC(error))
				{
					error = ::g_root->RegSerialize( m_arrSchedule[i]->m_pSchedule, SERID_UNKNOWN, m_hRegistry, "Schedule" );

					if (m_arrSchedule[i]->m_pScheduleParams)
					{
						if (PR_SUCC(error))
						{
							tDWORD unique = m_arrSchedule[i]->m_pScheduleParams->getIID();
							error = m_hRegistry->SetValue(SubKey, "ParamsUniq", tid_DWORD, &unique, sizeof(unique), cTRUE);
							if (PR_SUCC(error))
								error = ::g_root->RegSerialize( m_arrSchedule[i]->m_pScheduleParams, SERID_UNKNOWN, m_hRegistry, "Params" );
						}
					}

					if (PR_SUCC(error))
						error = m_hRegistry->SetValue(SubKey, "LastModified", tid_DATETIME, &m_arrSchedule[i]->m_dtLastShiftedTime, sizeof(tDATETIME), cTRUE);
				}
				m_hRegistry->CloseKey(SubKey);
			}
			if (PR_SUCC(error))
				dwCurSchedule++;
		}
		if ( dwCurSchedule )
			error = m_hRegistry->SetValue(cRegRoot, "Count", tid_DWORD, &dwCurSchedule, sizeof(tDWORD), cTRUE); // write actual number of saved schedules
	}

	return error;
}

#define cSCHEDUILE_WAIT_TIME 50

tERROR Scheduler::DestroyScheduleEx(tSCHEDULE_EX* pScheduleEx, tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID, tDWORD dwTimeout)
{
	tDWORD i;
	tSCHEDULE_EX* pFindScheduleEx = pScheduleEx;
	bool bFound = false;
	tDWORD dwWaitTime = 0;
	bool bWait;

	do {
		bWait = false;
		{
			cAutoCS _lock(m_hSync, cTRUE);
			for (i=0; i<m_dwSchedulesCount; i++)
			{
				if (!m_arrSchedule[i])
					continue;
				if ((pFindScheduleEx && (m_arrSchedule[i] == pFindScheduleEx)) || 
					(m_arrSchedule[i]->m_pSchedule->m_dwScheduleClass == p_dwScheduleClass && m_arrSchedule[i]->m_pSchedule->m_dwScheduleID == p_dwScheduleID))
				{
					pScheduleEx = m_arrSchedule[i];
					bFound = true;
					
					if (pScheduleEx->m_nRunningJobs)
					{
						if (!pScheduleEx->m_bMarkedForDelete)
						{
							PR_TRACE((this, prtNOTIFY, "schd\tSchedule %08X:%08X marked for delete", pScheduleEx->m_pSchedule->m_dwScheduleClass, pScheduleEx->m_pSchedule->m_dwScheduleID));
							pScheduleEx->m_bMarkedForDelete = cTRUE;
						}
						if (dwTimeout)
						{
							if (PrGetThreadId() != pScheduleEx->m_dwTID)
							{
								PR_TRACE((this, prtNOTIFY, "schd\tSchedule %08X:%08X still executing on thread %X, will wait", pScheduleEx->m_pSchedule->m_dwScheduleClass, pScheduleEx->m_pSchedule->m_dwScheduleID, pScheduleEx->m_dwTID));
								bWait = true;
							}
							else
							{
								PR_TRACE((this, prtNOTIFY, "schd\tSchedule %08X:%08X deleting on self thread %X, returning WITHOUT WAIT", pScheduleEx->m_pSchedule->m_dwScheduleClass, pScheduleEx->m_pSchedule->m_dwScheduleID, pScheduleEx->m_dwTID));
								bWait = false;
								return errOK;
							}
						}
					}
					else
					{
						if (pScheduleEx->m_pSchedule)
						{
							PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X deleted", pScheduleEx->m_pSchedule->m_dwScheduleClass, pScheduleEx->m_pSchedule->m_dwScheduleID));
							::g_root->DestroySerializable(pScheduleEx->m_pSchedule);
						}
						else
							PR_TRACE((this, prtNOTIFY, "schd\tSchedule deleted"));
						if (pScheduleEx->m_pScheduleParams)
							::g_root->DestroySerializable(pScheduleEx->m_pScheduleParams);
						memset(pScheduleEx, 0, sizeof(tSCHEDULE_EX));
						heapFree(pScheduleEx);
						m_arrSchedule[i] = NULL;
					}
				}
			}
			if (!bFound)
				return errNOT_FOUND;
		} // end of lock

		if (!bWait || !dwTimeout)
			break;

		if (dwTimeout != cSYNC_INFINITE && dwWaitTime >= dwTimeout)
			return errTIMEOUT;
		if ((dwWaitTime % (cSCHEDUILE_WAIT_TIME * 20)) == 0)
		{
			PR_TRACE((this, prtNOTIFY, "schd\tSchedule %08X:%08X (%08X) delete pending %d sec", p_dwScheduleClass, p_dwScheduleID, pScheduleEx, dwWaitTime / 1000));
		}
		PrSleep(cSCHEDUILE_WAIT_TIME);
		dwWaitTime += cSCHEDUILE_WAIT_TIME;

	} while (true); // wait running jobs exit

	return errOK;
}

tERROR Scheduler::FindSchedule( tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID, tDWORD* pdwIndex )
{
	tDWORD i;
	if (m_dwSchedulesCount == 0)
		return errNOT_FOUND;		
	for (i=0; i<m_dwSchedulesCount; i++)
	{
		if (NULL == m_arrSchedule[i])
			continue;
		if (m_arrSchedule[i]->m_bMarkedForDelete)
			continue;
		if (m_arrSchedule[i]->m_pSchedule == NULL) 
			continue;
		if (p_dwScheduleClass == m_arrSchedule[i]->m_pSchedule->m_dwScheduleClass)
		{
			if (p_dwScheduleID == m_arrSchedule[i]->m_pSchedule->m_dwScheduleID)
			{
				*pdwIndex = i;
				return errOK;
			}
		}
	}
	return errNOT_FOUND;
}

tERROR Scheduler::FindNextSchedule( tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID, tDWORD* pdwIndex )
{
	tDWORD i;
	if (m_dwSchedulesCount == 0)
		return errEND_OF_THE_LIST;		
	for (i=0; i<m_dwSchedulesCount; i++)
	{
		if (NULL == m_arrSchedule[i])
			continue;
		if (m_arrSchedule[i]->m_bMarkedForDelete)
			continue;
		if (m_arrSchedule[i]->m_pSchedule == NULL) 
			continue;
		if ( (p_dwScheduleClass == 0)
			&& (p_dwScheduleID == 0) )
		{
				*pdwIndex = i;
				return errOK;
		}
		if ( (p_dwScheduleClass == m_arrSchedule[i]->m_pSchedule->m_dwScheduleClass)
			&& (p_dwScheduleID == m_arrSchedule[i]->m_pSchedule->m_dwScheduleID) )
		{
			p_dwScheduleClass = 0;
			p_dwScheduleID = 0;
		}
	}
	return errEND_OF_THE_LIST;
}

tERROR Scheduler::CreateScheduleEx( tSCHEDULE_EX** ppScheduleEx )
{
	tDWORD i;
	tERROR error;
	for (i=0; i<m_dwSchedulesCount; i++)
	{
		if (NULL == m_arrSchedule[i])
		{
			if (PR_FAIL(error = heapAlloc((tPTR*)&m_arrSchedule[i], sizeof(tSCHEDULE_EX))))
				return error;
		}
		else
		{
			if (m_arrSchedule[i]->m_bMarkedForDelete)
				continue;
			if (m_arrSchedule[i]->m_pSchedule) 
				continue;
			// clean old data
			memset(m_arrSchedule[i], 0, sizeof(tSCHEDULE_EX));
		}
		*ppScheduleEx = m_arrSchedule[i];
		return errOK;
	}
	// no free slots found - add new one
	if (PR_FAIL(error = heapRealloc((tPTR*)&m_arrSchedule, m_arrSchedule, (m_dwSchedulesCount+1)*sizeof(tSCHEDULE_EX*))))
		return error;
	m_arrSchedule[m_dwSchedulesCount] = NULL;
	if (PR_FAIL(error = heapAlloc((tPTR*)&m_arrSchedule[m_dwSchedulesCount], sizeof(tSCHEDULE_EX))))
		return error;
	memset(m_arrSchedule[m_dwSchedulesCount], 0, sizeof(tSCHEDULE_EX));
	*ppScheduleEx = m_arrSchedule[m_dwSchedulesCount];
	m_dwSchedulesCount++;
	return errOK;
}

tDWORD __rand(tDWORD p_dwRange, tDATETIME* pCurrTime)
{
    tDATETIME dt ;
	if (pCurrTime)
		DTCpy(&dt, pCurrTime);
	else
		Now(&dt);
    tDWORD dwValue = pr_rand(0xFFFFFFFF) ;
    dwValue *= *(tDWORD*)dt ;

    if (p_dwRange > 0xFFFFF)
    {
        dwValue = _rotl(dwValue, dwValue & 0xF) ;
    }

    return dwValue % p_dwRange ;
}

tERROR Scheduler::TimeShiftForward(tSCHEDULE_EX* pScheduleEx, tDATETIME* pdtNow)
{
	tERROR error = errOK;
	cScheduleSettings* pSchedule = pScheduleEx->m_pSchedule;
	tBOOL bShiftForward = cTRUE;
	tDWORD dwProtectCounter = 0;
	if (!pSchedule)
		return errNOT_OK;
	
	if (pScheduleEx->m_dwTimeShiftSeconds == 0 && pScheduleEx->m_dwTimeShiftMonths == 0)
		return errOK;
	
	// Relative => Next<-Now
	if (pSchedule->m_bRelativeTimeShift)
		DTCpy(&pSchedule->m_dtNextRun, pdtNow);

#if defined (_WIN32) && defined ( _PRAGUE_TRACE_)
	{
		SYSTEMTIME st;
		DateTime2SystemTime(pdtNow, &st);
		PR_TRACE((this, prtNOTIFY, "schd\tTimeShiftForward %s, now time %02d.%02d.%04d %02d:%02d:%02d.%d", (PR_SUCC(error) ? "successed" : "failed"), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
		DateTime2SystemTime(&pSchedule->m_dtFirstRun, &st);
		PR_TRACE((this, prtNOTIFY, "schd\tTimeShiftForward %s, first time %02d.%02d.%04d %02d:%02d:%02d.%d", (PR_SUCC(error) ? "successed" : "failed"), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
	}
#endif

	while (bShiftForward)
	{
		bShiftForward = cFALSE;

		if (++dwProtectCounter > 1000)
		{
			error = errMORE_THAN;
			PR_TRACE((this, prtFATAL, "schd\t!!! TimeShiftForward, cannot shift forward > 1000 times???"));
			break;
		}

		do
		{
			if (pScheduleEx->m_dwTimeShiftSeconds)
			{
				if (PR_FAIL(error = DTTimeShift(&pSchedule->m_dtNextRun, 0, 0, pScheduleEx->m_dwTimeShiftSeconds)))
					break;
			}

			if (pScheduleEx->m_dwTimeShiftMonths)
			{
				tDWORD dwYear, dwMonth, dwDay, dwHour, dwMin, dwSec, dwNanosec;
				DTGet(&pSchedule->m_dtNextRun, &dwYear, &dwMonth, &dwDay, &dwHour, &dwMin, &dwSec, &dwNanosec);
				if (pScheduleEx->m_dwTimeShiftDayOfMonth)
					dwDay = pScheduleEx->m_dwTimeShiftDayOfMonth;
				dwMonth += pScheduleEx->m_dwTimeShiftMonths;
				if (dwMonth > 12)
				{
					dwYear += dwMonth/12;
					dwMonth %= 12;
					if (dwMonth == 0)
					{
						dwMonth = 12;
						--dwYear;
					}
				}
				static const tDWORD daysInMon[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
				if (daysInMon[dwMonth-1] < dwDay)
				{
					if (dwMonth == 2 /*Feb*/)
						dwDay = 28 + (dwYear % 4 == 0) ? 1 : 0;
					else
						dwDay = daysInMon[dwMonth-1];
				}
				DTSet(&pSchedule->m_dtNextRun, dwYear, dwMonth, dwDay, dwHour, dwMin, dwSec, dwNanosec);
			}

#if defined (_WIN32) && defined ( _PRAGUE_TRACE_)
			{
				SYSTEMTIME st;
				DateTime2SystemTime(&pSchedule->m_dtNextRun, &st);
				PR_TRACE((this, prtNOTIFY, "schd\tTimeShiftForward go %s, next time %02d.%02d.%04d %02d:%02d:%02d.%d", (PR_SUCC(error) ? "successed" : "failed"), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
			}
#endif

		} while (DTCmp(&pSchedule->m_dtNextRun, pdtNow) <= 0 || DTCmp(&pSchedule->m_dtNextRun, &pSchedule->m_dtFirstRun) < 0);
		
		if (PR_FAIL(error))
			break;

		// shift AllowWindow if it's < Next time
		if (DTCmp(&pSchedule->m_dtAllowWindowEnd, &dtNull) != 0)
		{
			while (DTCmp(&pSchedule->m_dtAllowWindowEnd, &pSchedule->m_dtNextRun) <= 0)
			{
				if (PR_FAIL(error = DTTimeShift(&pSchedule->m_dtAllowWindowEnd, 0, 0, pScheduleEx->m_dwAllowWindowTimeShiftSeconds)))
					break;
				if (PR_FAIL(error = DTDateShift(&pSchedule->m_dtAllowWindowEnd, 0, pScheduleEx->m_dwAllowWindowTimeShiftMonths, 0)))
					break;
				if (PR_FAIL(error = DTTimeShift(&pSchedule->m_dtAllowWindowStart, 0, 0, pScheduleEx->m_dwAllowWindowTimeShiftSeconds)))
					break;
				if (PR_FAIL(error = DTDateShift(&pSchedule->m_dtAllowWindowStart, 0, pScheduleEx->m_dwAllowWindowTimeShiftMonths, 0)))
					break;
			}
		}

		if (PR_FAIL(error))
			break;

		if (pSchedule->m_bWeekDays2Skip[DTWeekDay(&pSchedule->m_dtNextRun, pSchedule->m_bSunday)]) // this day marked to skip
		{
#ifdef _PRAGUE_TRACE_
			tCHAR wd[8];
			int i;
			for (i=0; i<7; i++)
				wd[i] = pSchedule->m_bWeekDays2Skip[i] ? 's' : '+';
			wd[7] = 0;
			wd[DTWeekDay(&pSchedule->m_dtNextRun, pSchedule->m_bSunday)] = 'S';
			PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X weekday says skip [%s], run forward...", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, wd));
#endif
			bShiftForward = cTRUE;
		}
		if (DTCmp(&pSchedule->m_dtNextRun, &pSchedule->m_dtAllowWindowStart) < 0) // time not in window
		{
			PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X not in window, run forward...", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID));
			bShiftForward = cTRUE;
		}
	}

#if defined (_WIN32) && defined (_PRAGUE_TRACE_)
	{
		SYSTEMTIME st;
		DateTime2SystemTime(&pSchedule->m_dtNextRun, &st);
		PR_TRACE((this, prtNOTIFY, "schd\tTimeShiftForward %s, new whistle time %02d.%02d.%04d %02d:%02d:%02d.%d", (PR_SUCC(error) ? "successed" : "failed"), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
	}
#endif


	if (PR_FAIL(error))
		pSchedule->m_bEnabled = cFALSE;

	if (pSchedule->m_dwTimeStartDelta)
		pScheduleEx->m_dwTimeStartDeltaRandom = __rand(pSchedule->m_dwTimeStartDelta, pdtNow);

	DTCpy(&pScheduleEx->m_dtLastShiftedTime, pdtNow);

	return error;
}

tERROR Scheduler::TimeShiftNext(tSCHEDULE_EX* pScheduleEx, tDATETIME* pdtNow)
{
	tERROR error = errOK;
	cScheduleSettings* pSchedule = pScheduleEx->m_pSchedule;
	
	if (!pSchedule)
	{
		PR_TRACE((this, prtERROR, "schd\tTimeShiftNext: pScheduleEx->m_pSchedule = NULL"));
		return errNOT_OK;
	}
	
	if (0 == DTCmp(&pSchedule->m_dtNextRun, &dtNull))
		DTCpy(&pSchedule->m_dtNextRun, pdtNow);

	if (pScheduleEx->m_dwTimeShiftSeconds == 0 && pScheduleEx->m_dwTimeShiftMonths == 0)
	{
		// ok
	}
	else
	{

		// shift Next time back
		while (DTCmp(&pSchedule->m_dtNextRun, pdtNow) > 0)
		{
			if (PR_FAIL(error = DTTimeShift(&pSchedule->m_dtNextRun, 0, 0, -(tINT)pScheduleEx->m_dwTimeShiftSeconds)))
				break;
			if (PR_FAIL(error = DTDateShift(&pSchedule->m_dtNextRun, 0, -(tINT)pScheduleEx->m_dwTimeShiftMonths, 0)))
				break;
		};

		DTTimeShift(&pSchedule->m_dtNextRun, 0, 0, -(tINT)pScheduleEx->m_dwTimeShiftSeconds);
		DTDateShift(&pSchedule->m_dtNextRun, 0, -(tINT)pScheduleEx->m_dwTimeShiftMonths, 0);
		
		// shift AllowWindow back 
		if (PR_SUCC(error))
		{
			if (DTCmp(&pSchedule->m_dtAllowWindowEnd, &dtNull) != 0)
			{
				if (DTCmp(&pSchedule->m_dtAllowWindowEnd, pdtNow) > 0)
				{
					do
					{
						if (PR_FAIL(error = DTTimeShift(&pSchedule->m_dtAllowWindowEnd, 0, 0, -(tINT)pScheduleEx->m_dwAllowWindowTimeShiftSeconds)))
							break;
						if (PR_FAIL(error = DTDateShift(&pSchedule->m_dtAllowWindowEnd, 0, -(tINT)pScheduleEx->m_dwAllowWindowTimeShiftMonths, 0)))
							break;
						if (PR_FAIL(error = DTTimeShift(&pSchedule->m_dtAllowWindowStart, 0, 0, -(tINT)pScheduleEx->m_dwAllowWindowTimeShiftSeconds)))
							break;
						if (PR_FAIL(error = DTDateShift(&pSchedule->m_dtAllowWindowStart, 0, -(tINT)pScheduleEx->m_dwAllowWindowTimeShiftMonths, 0)))
							break;
					} while (DTCmp(&pSchedule->m_dtAllowWindowEnd, pdtNow) > 0);
				}
			}
		}
			

#if defined (_WIN32) && defined ( _PRAGUE_TRACE_)
		{
			SYSTEMTIME st;
			DateTime2SystemTime(&pSchedule->m_dtNextRun, &st);
			PR_TRACE((this, prtNOTIFY, "schd\tTimeShiftNext go back %s, new time %02d.%02d.%04d %02d:%02d:%02d.%d", (PR_SUCC(error) ? "successed" : "failed"), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
		}
#endif

		// now go Forward
		if (PR_SUCC(error))
			error = TimeShiftForward(pScheduleEx, pdtNow);

	}

#if defined (_WIN32) && defined ( _PRAGUE_TRACE_)
	{
		SYSTEMTIME st;
		DateTime2SystemTime(&pSchedule->m_dtNextRun, &st);
		PR_TRACE((this, prtNOTIFY, "schd\tTimeShiftNext %s, new whistle time %02d.%02d.%04d %02d:%02d:%02d.%d + rnd:%d", (PR_SUCC(error) ? "successed" : "failed"), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, pScheduleEx->m_dwTimeStartDeltaRandom));
	}
#endif

	if (PR_FAIL(error))
		pSchedule->m_bEnabled = cFALSE;

	DTCpy(&pScheduleEx->m_dtLastShiftedTime, pdtNow);

	return error;
}


tERROR Scheduler::AdjustTimes( tSCHEDULE_EX* pScheduleEx, tDATETIME* pdtNow )
{
	cScheduleSettings* pSchedule;

	if (!pScheduleEx)
		return errPARAMETER_INVALID;
	pSchedule = pScheduleEx->m_pSchedule;
	if (!pSchedule)
		return errUNEXPECTED;

	// time_t -> dt
	if (0 == DTCmp(&pSchedule->m_dtFirstRun, &dtTimeTBase))
		DTCpy(&pSchedule->m_dtFirstRun, &dtNull);
	if (0 == DTCmp(&pSchedule->m_dtLastRun, &dtTimeTBase))
		DTCpy(&pSchedule->m_dtLastRun, &dtNull);
	if (0 == DTCmp(&pSchedule->m_dtNextRun, &dtTimeTBase))
		DTCpy(&pSchedule->m_dtNextRun, &dtNull);
	
//	if (DTCmp(&pSchedule->m_dtLastRun, pdtNow) > 0)
//		DTCpy(&pSchedule->m_dtLastRun, &dtNull);
//	if (DTCmp(&pSchedule->m_dtFirstRun, &pSchedule->m_dtLastRun) > 0)
//		DTCpy(&pSchedule->m_dtLastRun, &dtNull);

//	if (0 == DTCmp(&pSchedule->m_dtFirstRun, &dtNull) && 0 != DTCmp(&pSchedule->m_dtLastRun, &dtNull))
//		DTCpy(&pSchedule->m_dtFirstRun, &pSchedule->m_dtLastRun);
//	if (0 == DTCmp(&pSchedule->m_dtFirstRun, &dtNull) && 0 != DTCmp(&pSchedule->m_dtNextRun, &dtNull))
//		DTCpy(&pSchedule->m_dtFirstRun, &pSchedule->m_dtNextRun);
	if (pSchedule->m_bRelativeTimeShift &&  0 != DTCmp(&pSchedule->m_dtLastRun, &dtNull))
		DTCpy(&pSchedule->m_dtNextRun, &pSchedule->m_dtLastRun);
	else if (0 != DTCmp(&pSchedule->m_dtFirstRun, &dtNull))
		DTCpy(&pSchedule->m_dtNextRun, &pSchedule->m_dtFirstRun);
	if (0 == DTCmp(&pSchedule->m_dtNextRun, &dtNull))
		DTCpy(&pSchedule->m_dtNextRun, pdtNow);

	if (DTCmp(&pScheduleEx->m_dtLastShiftedTime, &dtNull) == 0) // is it new time?
	{
		pScheduleEx->m_dwTimeShiftSeconds = 
			pSchedule->m_TimeShift.dwSeconds + 
			pSchedule->m_TimeShift.dwMinutes*60 + 
			pSchedule->m_TimeShift.dwHours*60*60 +
			pSchedule->m_TimeShift.dwDays*60*60*24;
		pScheduleEx->m_dwTimeShiftMonths = 
			pSchedule->m_TimeShift.dwMonths + 
			pSchedule->m_TimeShift.dwYears*12;
		pScheduleEx->m_dwTimeShiftDayOfMonth = pSchedule->m_TimeShift.dwTimeShiftDayOfMonth;
		if (!pScheduleEx->m_dwTimeShiftDayOfMonth && !pScheduleEx->m_dwTimeShiftSeconds && pScheduleEx->m_dwTimeShiftMonths)
		{
			tDWORD dwYear, dwMonth, dwHour, dwMin, dwSec, dwNanosec;
			DTGet(&pSchedule->m_dtNextRun, &dwYear, &dwMonth, &pScheduleEx->m_dwTimeShiftDayOfMonth, &dwHour, &dwMin, &dwSec, &dwNanosec );
		}
	}

	if (/*0 == DTCmp(&pSchedule->m_dtNextRun, &pSchedule->m_dtLastRun) && */0 != DTCmp(&pSchedule->m_dtLastRun, &dtNull))
		TimeShiftNext(pScheduleEx, &pSchedule->m_dtLastRun);
	else if ((pScheduleEx->m_dwTimeShiftSeconds || pScheduleEx->m_dwTimeShiftMonths) && !pSchedule->m_bRaiseIfSkipped)
		TimeShiftNext(pScheduleEx, pdtNow);

	if (pSchedule->m_bWeekDays2Skip[DTWeekDay(&pSchedule->m_dtNextRun, pSchedule->m_bSunday)]) // this day marked to skip
	{
#ifdef _PRAGUE_TRACE_
		tCHAR wd[8];
		int i;
		for (i=0; i<7; i++)
			wd[i] = pSchedule->m_bWeekDays2Skip[i] ? 's' : '+';
		wd[7] = 0;
		wd[DTWeekDay(&pSchedule->m_dtNextRun, pSchedule->m_bSunday)] = 'S';
		PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X weekday says skip [%s], run forward...", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, wd));
#endif
		tDATETIME dtNextRun;
		DTCpy(&dtNextRun, &pSchedule->m_dtNextRun);
		TimeShiftNext(pScheduleEx, &dtNextRun);
	}

	DTCpy(&pScheduleEx->m_dtLastShiftedTime, pdtNow);

	return errOK;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR Scheduler::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Scheduler::ObjectInit method" );

	m_dwFrequency = 60 * 1000;
	memset(&dtNull, 0, sizeof(dtNull));
	DTSet(&dtTimeTBase, 1970, 1, 1, 0, 0, 0, 0); // time_t(0)

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR Scheduler::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Scheduler::ObjectInitDone method" );

	error = Now(&m_dtLastTime);

	if (PR_SUCC(error))
		error = sysCreateObjectQuick((hOBJECT*)&m_hSync, IID_CRITICAL_SECTION);
	if (PR_SUCC(error))
		LoadSettings();
	if (PR_SUCC(error))
		error = sysCreateObject((hOBJECT*)&m_hTimer, IID_TIMER, PID_TIMER);
	if (PR_SUCC(error))
		error = m_hTimer->set_pgFREQUENCY(m_dwFrequency);
	if (PR_SUCC(error))
		error = m_hTimer->set_pgENABLED(cTRUE);
	if (PR_SUCC(error))
		error = m_hTimer->sysCreateObjectDone();
	if (PR_SUCC(error))
		m_dwTimerID = m_hTimer->get_pgTIMER_ID();
	if (PR_SUCC(error))
		error = sysCreateObject((hOBJECT*)&m_hThPool, IID_THREADPOOL, PID_THPOOLIMP);
	if (PR_SUCC(error))
		error = m_hThPool->set_pgTP_NAME((void *)"Scheduler", sizeof("Scheduler"), cCP_ANSI);
	if (PR_SUCC(error))
		error = m_hThPool->set_pgTP_THREAD_CALLBACK(ThreadCallback);
	if (PR_SUCC(error))
		error = m_hThPool->set_pgTP_THREAD_CALLBACK_CONTEXT(this);
	if (PR_SUCC(error))
		error = m_hThPool->set_pgTP_DELAYED_DONE(cFALSE);
	if (PR_SUCC(error))
		error = m_hThPool->set_pgTP_MIN_THREADS(0);
	if (PR_SUCC(error))
		error = m_hThPool->set_pgTP_MAX_THREADS(100);
	if (PR_SUCC(error))
		error = m_hThPool->set_pgTP_MAX_IDLE_COUNT(1);
	if (PR_SUCC(error))
		error = m_hThPool->set_pgTP_YIELD_TIMEOUT(10000);
	if (PR_SUCC(error))
		error = m_hThPool->set_pgTP_QUICK_EXECUTE(cTRUE);
	if (PR_SUCC(error))
		error = m_hThPool->set_pgTP_QUICK_DONE(cTRUE);
	if (PR_SUCC(error))
		error = m_hThPool->sysCreateObjectDone();
	if (PR_SUCC(error))
		error = sysRegisterMsgHandler( pmc_TIMER, rmhLISTENER, m_hTimer, IID_TIMER, PID_TIMER, IID_ANY, PID_ANY );
	if (PR_SUCC(error))
		PR_TRACE((this, prtNOTIFY, "schd\tScheduler inited successfully, freq=%d", m_dwFrequency));
	else
		PR_TRACE((this, prtERROR,  "schd\tScheduler init failed with %terr", error));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR Scheduler::ObjectPreClose()
{
	tERROR error = errOK;
	tDWORD i;
	PR_TRACE_FUNC_FRAME( "Scheduler::ObjectPreClose method" );

	// stop timer
	if (m_hTimer)
		m_hTimer->sysCloseObject();

	// wait while all tasks finished
	if (m_hThPool)
		m_hThPool->sysCloseObject();
	
	m_hSync->Enter(SHARE_LEVEL_WRITE);

	m_hTimer = NULL;

	SaveSettings();

	for (i=0; i<m_dwSchedulesCount; i++)
	{
		if (NULL == m_arrSchedule[i])
			continue;
		if (m_arrSchedule[i]->m_nRunningJobs)
		{
			PR_TRACE((this, prtERROR, "schd\tScheduler::ObjectPreClose: Schedule %08X:%08X has running job(s)[%d], while scheduler closing, exception expected!", m_arrSchedule[i]->m_pSchedule->m_dwScheduleClass, m_arrSchedule[i]->m_pSchedule->m_dwScheduleID, m_arrSchedule[i]->m_nRunningJobs));
			INT3;
		}
		DestroyScheduleEx(m_arrSchedule[i], 0, 0, 0);
	}

	if (m_arrSchedule)
		heapFree(m_arrSchedule);

	return error;
}
// AVP Prague stamp end

static void GetScheduleIdString(tDWORD p_msg_cls_id, tDWORD p_msg_id, char sIdString[17])
{
	tQWORD id = (((tQWORD)p_msg_cls_id) << 32) | p_msg_id;
	for (int i=0; i<16;i++)
	{
		tDWORD c = (tDWORD)(id >> 60);
		id <<= 4;
		if (c <= 9)
			sIdString[i] = c + '0';
		else 
			sIdString[i] = c - 10 + 'a';
	}
	sIdString[16] = 0;
}

tBOOL Scheduler::VerifyRaiseIfSkipped(cScheduleSettings* pSchedule)
{
	if (!pSchedule)
		return cFALSE;
	tBOOL bRaiseIfSkipped = pSchedule->m_bRaiseIfSkipped;
	if (!bRaiseIfSkipped)
		return bRaiseIfSkipped;
	if (!m_hRegistry)
		return bRaiseIfSkipped;
	tRegKey key;
	if (PR_SUCC(m_hRegistry->OpenKey(&key, cRegRoot, "LastRun", cFALSE)))
	{
		tDATETIME dtLastRun = {0};
		char sIdString[17];
		GetScheduleIdString(pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, sIdString);
		if (PR_SUCC(m_hRegistry->GetValue(NULL, key, sIdString, NULL, &dtLastRun, sizeof(dtLastRun))))
		{
			if (0 == DTCmp(&pSchedule->m_dtNextRun, &dtLastRun)) // we already ran at this time
				bRaiseIfSkipped = cFALSE;
		}
#if defined (_WIN32) && defined ( _PRAGUE_TRACE_)
		{
			SYSTEMTIME st;
			DateTime2SystemTime(&dtLastRun, &st);
			PR_TRACE((this, prtIMPORTANT, "schd\tVerifyRaiseIfSkipped LastRun %02d.%02d.%04d %02d:%02d:%02d.%d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
			DateTime2SystemTime(&pSchedule->m_dtNextRun, &st);
			PR_TRACE((this, prtIMPORTANT, "schd\tVerifyRaiseIfSkipped NextRun %02d.%02d.%04d %02d:%02d:%02d.%d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
			PR_TRACE((this, prtIMPORTANT, "schd\tVerifyRaiseIfSkipped result: %s", bRaiseIfSkipped ? "true" : "false"));
		}
#endif
		m_hRegistry->CloseKey(key);
	}
	return bRaiseIfSkipped;
}

tERROR Scheduler::StoreLastRunTime(cScheduleSettings* pSchedule)
{
	tERROR error = errOK;
	if (!pSchedule)
		return errPARAMETER_INVALID;
	if (!m_hRegistry)
		return errOBJECT_NOT_INITIALIZED;
	tRegKey key;
	if (PR_SUCC(error = m_hRegistry->OpenKey(&key, cRegRoot, "LastRun", cTRUE)))
	{
		char sIdString[17];
		GetScheduleIdString(pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, sIdString);
		error = m_hRegistry->SetValue(key, sIdString, tid_DATETIME, &pSchedule->m_dtNextRun, sizeof(tDATETIME), cTRUE);
		m_hRegistry->CloseKey(key);
	}
	return error;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR Scheduler::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Scheduler::MsgReceive method" );

	if (p_msg_cls_id == pmc_TIMER && p_msg_id == m_dwTimerID)
	{
		tDWORD i;
		tDATETIME dtNow;
		cScheduleSettings* pSchedule;
		tBOOL bWasOffline = cFALSE;
		tLONGLONG llPassedSeconds;

		// check schedules

		if (PR_FAIL(error = Now(&dtNow)))
			return error;

#if defined (_WIN32) && defined ( _PRAGUE_TRACE_)
		{
			SYSTEMTIME st;
			DateTime2SystemTime(&dtNow, &st);
			PR_TRACE((this, prtNOTIFY, "schd\tScheduler timer event, now %02d.%02d.%04d %02d:%02d:%02d.%d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
		}
#endif

		if (PR_SUCC(DTIntervalGet(&dtNow, &m_dtLastTime, &llPassedSeconds, 1000000000)))
		{
			if ( (llPassedSeconds < 0) || (llPassedSeconds > 60 && (llPassedSeconds) > (tLONGLONG)(m_dwFrequency / 1000 * 5)) )
			{
				// if passed more then 5 timer intrervals but at least 1 minute, it is possible
				// system has been hibernated or sleep or something like that... or time was changed by user...
				tDWORD size = sizeof(m_dtLastTime);
				PR_TRACE((this, prtIMPORTANT, "schd\tScheduler was offline (hibernate, sleep, time changed, etc)? PassedSeconds=%I64d, sending pmc_SYSTEM_TIME_CHANGED", llPassedSeconds));
				g_root->sysSendMsg(pmc_SYSTEM_TIME_CHANGED, 0, 0, &m_dtLastTime, &size);
				bWasOffline = cTRUE;
			}
		}

		cAutoCS _lock(m_hSync, cTRUE);

		for (i=0; i<m_dwSchedulesCount; i++)
		{
			if (NULL == m_arrSchedule[i])
				continue;
			pSchedule = m_arrSchedule[i]->m_pSchedule;
			if (m_arrSchedule[i]->m_bMarkedForDelete) // skip marked for delete
				continue;
			if (pSchedule == NULL) // skip deleted
				continue;
			if (pSchedule->m_bEnabled == cFALSE) // skip disabled
			{
				PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X, disabled"));
				continue;
			}
			if (DTCmp(&dtNow, &m_arrSchedule[i]->m_dtLastShiftedTime) < 0) // !!! time rollback !!!
			{
#if defined (_WIN32) && defined ( _PRAGUE_TRACE_)
				{
					SYSTEMTIME st;
					DateTime2SystemTime(&m_arrSchedule[i]->m_dtLastShiftedTime, &st);
					PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X, TIME ROLLBACK! last shift time %02d.%02d.%04d %02d:%02d:%02d.%03d", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
				}
#endif
				error = TimeShiftNext(m_arrSchedule[i], &dtNow);
			}
			else
			{
				tDATETIME dtNextRun;
				tSCHEDULE_EX* pScheduleEx = m_arrSchedule[i];
				if (0 == DTCmp(&pSchedule->m_dtNextRun, &dtNull))
					DTCpy(&pSchedule->m_dtNextRun, &dtNow);
				DTCpy(&dtNextRun, &pSchedule->m_dtNextRun);
				DTTimeShift(&dtNextRun, 0, 0, pScheduleEx->m_dwTimeStartDeltaRandom);
#if defined (_WIN32) && defined ( _PRAGUE_TRACE_)
				SYSTEMTIME st;
				DateTime2SystemTime(&pSchedule->m_dtNextRun, &st);
#endif
				if (DTCmp(&dtNextRun, &dtNow) <= 0) // if time passed
				{
#if defined (_WIN32)
					PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X, whistle time %02d.%02d.%04d %02d:%02d:%02d.%03d+rnd:%d, * time to whistle", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, pScheduleEx->m_dwTimeStartDeltaRandom));
#endif
					tBOOL bRaiseEvent = cTRUE;
					
					// check if raise event needed?
					if (bWasOffline)
					{
						PR_TRACE((this, prtIMPORTANT, "schd\tScheduler was offline? Schedule %08X:%08X RaiseIfSkipped=%s", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, pSchedule->m_bRaiseIfSkipped ? "TRUE" : "FALSE"));
						bRaiseEvent = pSchedule->m_bRaiseIfSkipped;
					}
					else if (DTCmp(&dtNextRun, &m_dtLastTime) <= 0) // time skipped
					{
						PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X time skipped... RaiseIfSkipped=%s", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, pSchedule->m_bRaiseIfSkipped ? "TRUE" : "FALSE"));
						bRaiseEvent = pSchedule->m_bRaiseIfSkipped;
						//bRaiseEvent = VerifyRaiseIfSkipped(pSchedule);
					}
/*
					if (DTCmp(&dtNow, &pSchedule->m_dtAllowWindowStart) < 0) // time not in window
					{
						PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X time not in window", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID));
						bRaiseEvent = cFALSE;
					}
*/
					if (!pSchedule->m_bRaiseIfSkipped
						&& pSchedule->m_bWeekDays2Skip[DTWeekDay(&dtNow, pSchedule->m_bSunday)]) // weekday 2 skip
					{
#ifdef _PRAGUE_TRACE_
						tCHAR wd[8];
						int i;
						for (i=0; i<7; i++)
							wd[i] = pSchedule->m_bWeekDays2Skip[i] ? 's' : '+';
						wd[7] = 0;
						wd[DTWeekDay(&dtNow, pSchedule->m_bSunday)] = 'S';
						PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X weekday says skip [%s]", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, wd));
#endif
						bRaiseEvent = cFALSE;
					}


					//if (bRaiseEvent)
					//	StoreLastRunTime(pSchedule);
					
					// shift time forward (recalculate next run time) BEFORE whistling up
					// in this way client may know (with GetSchedule) and change (with SetSchedule)
					// next whistle-up time during schedule notify processing
					error = TimeShiftForward(m_arrSchedule[i], &dtNow);

					// now whistle up notification if needed
					if (bRaiseEvent)
					{
						PR_TRACE((this, prtNOTIFY, "schd\tSchedule %08X:%08X whistling up", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID));
						if (m_hThPool)
						{
							if (0 == m_arrSchedule[i]->m_nRunningJobs)
							{
								if (PR_SUCC(error = m_hThPool->AddTask(NULL, ThreadTaskCallback, &m_arrSchedule[i], sizeof(tSCHEDULE_EX*), 0)))
								{
									PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X job added for thread pool", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID));
									m_arrSchedule[i]->m_nRunningJobs++;
								}
								else
								{
									PR_TRACE((this, prtERROR, "schd\tSchedule %08X:%08X job add failed with %terr", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, error));
								}
							}
							else
							{
								PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X has running task, skipped", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, error));
							}
						}
						else // call syncronously
						{
							PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X alerting synchronously on timer thread", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID));
							if (0 == m_arrSchedule[i]->m_nRunningJobs)
							{
								m_arrSchedule[i]->m_nRunningJobs++;
								error = sysSendMsg(pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, NULL, m_arrSchedule[i]->m_pScheduleParams, SER_SENDMSG_PSIZE);
								m_arrSchedule[i]->m_nRunningJobs--;
							}
							PR_TRACE((this, prtNOTIFY, "schd\tSchedule %08X:%08X done with %terr", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, error));
						}
					}
					else
					{
						PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X skipped by RaiseIfSkipped, AllowWindow or WeekDays2Skip", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID));
					}

					if (m_arrSchedule[i]->m_dwTimeShiftSeconds==0 && m_arrSchedule[i]->m_dwTimeShiftMonths==0)
					{
						// Single event - delete
						//PR_TRACE((this, prtNOTIFY, "schd\tSchedule %08X:%08X disabled", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID));
						//pSchedule->m_bEnabled = cFALSE;
						PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X was single-time schedule", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID));
						DestroyScheduleEx(m_arrSchedule[i], 0, 0, 0);
					}
					else
					{
#if defined (_WIN32) && defined ( _PRAGUE_TRACE_)
						SYSTEMTIME st;
						DateTime2SystemTime(&pSchedule->m_dtNextRun, &st);
						PR_TRACE((this, prtIMPORTANT, "schd\tSchedule %08X:%08X, next time %02d.%02d.%04d %02d:%02d:%02d.%03d+rnd:%d", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, pScheduleEx->m_dwTimeStartDeltaRandom));
#endif
					}
				}
				else
				{
#if defined (_WIN32)
					PR_TRACE((this, prtNOTIFY, "schd\tSchedule %08X:%08X, whistle time %02d.%02d.%04d %02d:%02d:%02d.%03d+rnd:%d, continue waiting", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, pScheduleEx->m_dwTimeStartDeltaRandom));
#endif

				}
			}
		}

		DTCpy(&m_dtLastTime, &dtNow);

		error = errOK_DECIDED;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetFrequency )
// Interface "Scheduler". Method "Set" for property(s):
//  -- FREQUENCY
tERROR Scheduler::SetFrequency( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	tDWORD dwFreq;
	PR_TRACE_A0( this, "schd\tEnter *SET* method SetFrequency for property pgFREQUENCY" );

	if (!buffer || size < sizeof(tDWORD))
		return errPARAMETER_INVALID;

	dwFreq = *(tDWORD*)buffer;

	if (m_hTimer && m_hTimer->propGetBool(psOBJECT_OPERATIONAL))
		error = m_hTimer->set_pgFREQUENCY(dwFreq);

	if (PR_SUCC(error))
		m_dwFrequency = dwFreq;
	PR_TRACE_A2( this, "schd\tLeave *SET* method SetFrequency for property pgFREQUENCY, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetSettings )
// Interface "Scheduler". Method "Set" for property(s):
//  -- SETTINGS
tERROR Scheduler::SetSettings( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	hOBJECT hObj;
	PR_TRACE_A0( this, "schd\tEnter *SET* method SetSettings for property plSETTINGS" );

	*out_size = 0;

	if (!buffer || size<sizeof(hOBJECT))
		return errPARAMETER_INVALID;

	hObj = *(hOBJECT*)buffer;

	if (PR_SUCC(error = sysCheckObject(hObj, IID_REGISTRY)))
		m_hRegistry = (hREGISTRY)hObj;


	PR_TRACE_A2( this, "schd\tLeave *SET* method SetSettings for property plSETTINGS, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end


bool isValidTime(time_t time)
{
	return time != -1 && time && (unsigned)time > 2*24*60*60;
}

// AVP Prague stamp begin( External (user called) interface method implementation, SetSchedule )
// Parameters are:
tERROR Scheduler::SetSchedule( tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID, const cSerializable* p_pSchedule, const cSerializable* p_pScheduleParams )
{
	tERROR error = errOK;
	cScheduleSettings ss;
	cScheduleSettings* pSchedule;
	tSCHEDULE_EX* pScheduleEx = NULL;
	tDWORD i;
	tDATETIME dtNow;
	bool bRunNow = false;
	PR_TRACE_FUNC_FRAME( "Scheduler::SetSchedule method" );

	if (p_pSchedule==NULL)
		return errPARAMETER_INVALID;

	if (PR_FAIL(error = Now(&dtNow)))
		return error;

	if (p_pSchedule->isBasedOn(cScheduleSettings::eIID))
	{
		pSchedule = (cScheduleSettings*)p_pSchedule;
	}
	else if (p_pSchedule->isBasedOn(cTaskSchedule::eIID))
	{
		cTaskSchedule* pTaskSchedule = (cTaskSchedule*)p_pSchedule;
		tBOOL bUseTime = pTaskSchedule->m_bUseTime;

		PR_TRACE((this, prtIMPORTANT, "schd\tSetSchedule %08X:%08X, converting from TaskSchedule "
			"type=%d mode=%d every=%d usetime=%d raiseIfSkipped=%d period(M:d:h:m)=%d:%d:%d:%d "
			"firstrun=%08x lastrun=%08x",
			p_dwScheduleClass, p_dwScheduleID,
			pTaskSchedule->m_eType,
			pTaskSchedule->m_eMode,
			pTaskSchedule->m_eEvery,
			pTaskSchedule->m_bUseTime,
			pTaskSchedule->m_bRaiseIfSkipped,
			pTaskSchedule->m_nMonths,
			pTaskSchedule->m_nDays,
			pTaskSchedule->m_nHours,
			pTaskSchedule->m_nMinutes,
			pTaskSchedule->m_nFirstRunTime,
			pTaskSchedule->m_nLastRunTime));

		ss.m_bSunday = cFALSE;
		ss.m_bRaiseIfSkipped = pTaskSchedule->m_bRaiseIfSkipped;
		ss.m_dwTimeStartDelta = pTaskSchedule->m_nStartDelta;
		
		if (pTaskSchedule->m_eType != schtExactTime && !isValidTime(pTaskSchedule->m_nLastRunTime) && isValidTime(pTaskSchedule->m_nFirstRunTime))
		{
			tDATETIME t;
			DTCpy(&t,  &dtTimeTBase);
			DTTimeShift(&t, 0, 0, (tINT)pTaskSchedule->m_nFirstRunTime);
			if (DTCmp(&t, &dtNow) < 0)
				pTaskSchedule->m_nLastRunTime = pTaskSchedule->m_nFirstRunTime;
		}
		
		switch( pTaskSchedule->m_eType ) {
			case schtExactTime:
				if (pTaskSchedule->m_nLastRunTime && pTaskSchedule->m_nLastRunTime != -1) // this schedule already done
					return errOK;
				bUseTime = cTRUE;
				break;
			case schtMinutely:
				ss.m_TimeShift.dwMinutes = pTaskSchedule->m_nMinutes;
				break;
			case schtHourly:
				ss.m_TimeShift.dwHours = pTaskSchedule->m_nHours;
				break;
			case schtDaily:
				ss.m_TimeShift.dwDays = 1;
				switch ( pTaskSchedule->m_eEvery ) {
					case schEveryDays:
						ss.m_TimeShift.dwDays = pTaskSchedule->m_nDays;
						break;
					case schEveryWeekday:
						ss.m_bWeekDays2Skip[5] = cTRUE; // sat
						ss.m_bWeekDays2Skip[6] = cTRUE; // sun
						break;
					case schEveryHoliday:
						ss.m_bWeekDays2Skip[0] = cTRUE; // mon
						ss.m_bWeekDays2Skip[1] = cTRUE; // tue
						ss.m_bWeekDays2Skip[2] = cTRUE; // wed
						ss.m_bWeekDays2Skip[3] = cTRUE; // thu
						ss.m_bWeekDays2Skip[4] = cTRUE; // fri
						break;
					default:
						return errOBJECT_INCOMPATIBLE;
				}
				break;
			case schtWeekly:
				ss.m_TimeShift.dwDays = 1;
				for (i=0; i<7; i++)
					ss.m_bWeekDays2Skip[i] = !(pTaskSchedule->m_bWeekDays[i]);
				break;
			case schtMonthly:
				ss.m_TimeShift.dwMonths = 1;
				ss.m_TimeShift.dwTimeShiftDayOfMonth = pTaskSchedule->m_nDays;
				break;
			default:
				return errOBJECT_INCOMPATIBLE;
		}

		DTCpy(&ss.m_dtFirstRun, &dtTimeTBase);
		DTCpy(&ss.m_dtLastRun,  &dtTimeTBase);

		if (pTaskSchedule->m_nLastRunTime != -1)
			DTTimeShift(&ss.m_dtLastRun, 0, 0, (tINT)pTaskSchedule->m_nLastRunTime);
		if (bUseTime)
		{
			DTTimeShift(&ss.m_dtFirstRun, 0, 0, (tINT)pTaskSchedule->m_nFirstRunTime);
			if (pTaskSchedule->m_nFirstRunTime < 24*60*60) // 01.01.1970 xx:xx
			{
				tDWORD dwYear, dwMonth, dwDay, dwHour, dwMin, dwSec, dwNanosec;
				DTGet(&ss.m_dtFirstRun, &dwYear, &dwMonth, &dwDay, &dwHour, &dwMin, &dwSec, &dwNanosec );
				DTGet(&dtNow, &dwYear, &dwMonth, &dwDay, NULL, NULL, NULL, NULL);
				DTSet(&ss.m_dtFirstRun, dwYear, dwMonth, dwDay, dwHour, dwMin, dwSec, dwNanosec);
			}
			ss.m_bRelativeTimeShift = cFALSE;
		}
		else
		{
			ss.m_bRelativeTimeShift = cTRUE;
		}

		pSchedule = &ss;
	}
	else
		return errPARAMETER_INVALID;


	cAutoCS _lock(m_hSync, cTRUE);

	// destroy old schedule
	DestroyScheduleEx(0, p_dwScheduleClass, p_dwScheduleID, 0);

	error = CreateScheduleEx(&pScheduleEx);
	if (PR_SUCC(error))
		error = ::g_root->CopySerializable((cSerializable**)&pScheduleEx->m_pSchedule, pSchedule);
	pSchedule = pScheduleEx->m_pSchedule;
	if (PR_SUCC(error) && p_pScheduleParams)
		error = ::g_root->CopySerializable((cSerializable**)&pScheduleEx->m_pScheduleParams, p_pScheduleParams);

	if (PR_SUCC(error))
	{
		pSchedule->m_dwScheduleClass = p_dwScheduleClass;
		pSchedule->m_dwScheduleID = p_dwScheduleID;

#if defined (_WIN32) && defined (_PRAGUE_TRACE_)
		SYSTEMTIME st;
		tDATETIME  dtWasNextRun;
		DTCpy(&dtWasNextRun, &pSchedule->m_dtNextRun);
		if (0!=DTCmp(&pSchedule->m_dtFirstRun, &dtNull) && 0!=DTCmp(&pSchedule->m_dtFirstRun, &dtTimeTBase))
		{
			DateTime2SystemTime(&pSchedule->m_dtFirstRun, &st);
			PR_TRACE((this, prtIMPORTANT, "schd\tSetSchedule %08X:%08X, requested first time %02d.%02d.%04d %02d:%02d:%02d.%d", p_dwScheduleClass, p_dwScheduleID, st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
		}
		if (0!=DTCmp(&pSchedule->m_dtLastRun, &dtNull) && 0!=DTCmp(&pSchedule->m_dtLastRun, &dtTimeTBase))
		{
			DateTime2SystemTime(&pSchedule->m_dtLastRun, &st);
			PR_TRACE((this, prtIMPORTANT, "schd\tSetSchedule %08X:%08X, requested last  time %02d.%02d.%04d %02d:%02d:%02d.%d", p_dwScheduleClass, p_dwScheduleID, st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
		}
		if (0!=DTCmp(&pSchedule->m_dtNextRun, &dtNull) && 0!=DTCmp(&pSchedule->m_dtNextRun, &dtTimeTBase))
		{
			DateTime2SystemTime(&pSchedule->m_dtNextRun, &st);
			PR_TRACE((this, prtIMPORTANT, "schd\tSetSchedule %08X:%08X, requested next  time %02d.%02d.%04d %02d:%02d:%02d.%d", p_dwScheduleClass, p_dwScheduleID, st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
		}
#endif

		AdjustTimes(pScheduleEx, &dtNow);

		if (0 == pScheduleEx->m_dwTimeShiftSeconds && 0 == pScheduleEx->m_dwTimeShiftMonths && 0!=DTCmp(&pSchedule->m_dtLastRun, &dtNull))
		{
			PR_TRACE((this, prtIMPORTANT, "schd\tSetSchedule %08X:%08X, ExactTime schedule already done", p_dwScheduleClass, p_dwScheduleID ));
			DestroyScheduleEx(0, p_dwScheduleClass, p_dwScheduleID, 0);
			return errOK;
		}

#if defined (_WIN32) && defined (_PRAGUE_TRACE_)
		if (0 != DTCmp(&dtWasNextRun, &pSchedule->m_dtNextRun))
		{
			DateTime2SystemTime(&pSchedule->m_dtNextRun, &st);
			PR_TRACE((this, prtIMPORTANT, "schd\tSetSchedule %08X:%08X, adjusted next whistle time %02d.%02d.%04d %02d:%02d:%02d.%d", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
		}

		DateTime2SystemTime(&pSchedule->m_dtNextRun, &st);
		PR_TRACE((this, prtNOTIFY, "schd\tSetSchedule %08X:%08X, next whistle time %02d.%02d.%04d %02d:%02d:%02d.%d", pSchedule->m_dwScheduleClass, pSchedule->m_dwScheduleID, st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds));
#endif

		MsgReceive(pmc_TIMER, m_dwTimerID, *this, 0, *this, 0, 0);
	}

#if defined (_WIN32) && defined (_PRAGUE_TRACE_)
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtNOTIFY, "schd\tSetSchedule %08X:%08X failed with %terr", p_dwScheduleClass, p_dwScheduleID, error));
	}
#endif
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSchedule )
// Parameters are:
tERROR Scheduler::GetSchedule( tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID, cSerializable* p_pSchedule, cSerializable* p_pScheduleParams )
{
	tERROR error = errOK;
	tDWORD dwIndex;
	PR_TRACE_FUNC_FRAME( "Scheduler::GetSchedule method" );
	
	cAutoCS _lock(m_hSync, cTRUE);
	error = FindSchedule(p_dwScheduleClass, p_dwScheduleID, &dwIndex);
	if (PR_SUCC(error) && p_pSchedule)
	{
		error = ::g_root->CopySerializable(&p_pSchedule, m_arrSchedule[dwIndex]->m_pSchedule);
		if (PR_SUCC(error))
			cScheduleSettings* pSchedule = (cScheduleSettings*)p_pSchedule;
	}
	if (PR_SUCC(error) && p_pScheduleParams)
		error = ::g_root->CopySerializable(&p_pScheduleParams, m_arrSchedule[dwIndex]->m_pScheduleParams);
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, DeleteSchedule )
// Parameters are:
tERROR Scheduler::DeleteSchedule( tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Scheduler::DeleteSchedule method" );

	error = DestroyScheduleEx(0, p_dwScheduleClass, p_dwScheduleID, cSYNC_INFINITE);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetNextSchedule )
// Parameters are:
tERROR Scheduler::GetNextSchedule( tDWORD p_dwScheduleClass, tDWORD p_dwScheduleID, cSerializable* p_pSchedule, cSerializable* p_pScheduleParams )
{
	tERROR error = errOK;
	tDWORD dwIndex;
	PR_TRACE_FUNC_FRAME( "Scheduler::GetNextSchedule method" );

	cAutoCS _lock(m_hSync, cTRUE);
	error = FindNextSchedule(p_dwScheduleClass, p_dwScheduleID, &dwIndex);
	if (PR_SUCC(error) && p_pSchedule)
		error = ::g_root->CopySerializable(&p_pSchedule, m_arrSchedule[dwIndex]->m_pSchedule);
	if (PR_SUCC(error) && p_pScheduleParams)
		error = ::g_root->CopySerializable(&p_pScheduleParams, m_arrSchedule[dwIndex]->m_pScheduleParams);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Scheduler". Register function
tERROR Scheduler::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Scheduler_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Scheduler_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Scheduler", 10 )
	mpLOCAL_PROPERTY( pgFREQUENCY, Scheduler, m_dwFrequency, cPROP_BUFFER_READ_WRITE, NULL, FN_CUST(SetFrequency) )
	mpLOCAL_PROPERTY( plSETTINGS, Scheduler, m_hRegistry, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT, NULL, FN_CUST(SetSettings) )
mpPROPERTY_TABLE_END(Scheduler_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Scheduler)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(Scheduler)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Scheduler)
	mEXTERNAL_METHOD(Scheduler, SetSchedule)
	mEXTERNAL_METHOD(Scheduler, GetSchedule)
	mEXTERNAL_METHOD(Scheduler, DeleteSchedule)
	mEXTERNAL_METHOD(Scheduler, GetNextSchedule)
mEXTERNAL_TABLE_END(Scheduler)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "schd\tScheduler::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_SCHEDULER,                          // interface identifier
		PID_SCHEDULER,                          // plugin identifier
		0x00000000,                             // subtype identifier
		Scheduler_VERSION,                      // interface version
		VID_MIKE,                               // interface developer
		&i_Scheduler_vtbl,                      // internal(kernel called) function table
		(sizeof(i_Scheduler_vtbl)/sizeof(tPTR)),// internal function table size
		&e_Scheduler_vtbl,                      // external function table
		(sizeof(e_Scheduler_vtbl)/sizeof(tPTR)),// external function table size
		Scheduler_PropTable,                    // property table
		mPROPERTY_TABLE_SIZE(Scheduler_PropTable),// property table size
		sizeof(Scheduler)-sizeof(cObjImpl),     // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Scheduler(IID_SCHEDULER) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Scheduler_Register( hROOT root ) { return Scheduler::Register(root); }
// AVP Prague stamp end




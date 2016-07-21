// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  29 October 2004,  14:41 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- avstreats.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Interface version,  )
#define AVSTreats_VERSION ((tVERSION)1)
// AVP Prague stamp end

#include "avs.h"

// AVP Prague stamp begin( Includes for interface,  )
#include "avstreats.h"
// AVP Prague stamp end

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "AVSTreats". Static(shared) property table variables
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR AVSTreats::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSTreats::ObjectInitDone method" );

	sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);

	// Place your code here
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR AVSTreats::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSTreats::ObjectPreClose method" );

	// Place your code here
	if( m_pAVS )
		m_pAVS->SaveTreatsData(this);
	return error;
}
// AVP Prague stamp end

void AVSTreats::ClearTreatInfo()
{
	cAutoCS locker(m_lock, true);
	m_vecTreats.clear();
	m_vecUtreated.clear();
	m_tmStamp = PrGetTickCount();
}

bool AVSTreats::UpdateTreatInfo(AVSSession* pSession, tTreatInfo& pInfo, bool bNew)
{
	cAutoCS locker(m_lock, true);

	bool   bTreatOwner = (!m_nTaskId || pSession && m_nTaskId == pSession->m_nTaskId);
	bool   bTreated = !IS_INFECTED(pInfo.m_nStatus);
	bool   bDiscarded = IS_DISCARDED(pInfo.m_nStatus);
	tDWORD nStoredPos = pInfo.m_nStoredPos;

	tDWORD nPos = bNew ? -1 : m_vecTreats.find(nStoredPos);
	if( nPos != -1 )
	{
		m_tmStamp = PrGetTickCount();

		if( bDiscarded )
			m_vecTreats.remove(nPos);

		nPos = m_vecUtreated.find(nStoredPos);
		if( nPos != -1 && (bTreated || bDiscarded) )
			m_vecUtreated.remove(nPos);
		else if( nPos == -1 && !bTreated )
			m_vecUtreated.push_back(nStoredPos);
	}
	else if( bTreatOwner && !bDiscarded )
	{
		m_tmStamp = PrGetTickCount();
		m_vecTreats.push_back(nStoredPos);

		if( !bTreated )
			m_vecUtreated.push_back(nStoredPos);
	}

	if( bTreatOwner && pSession )
	{
		tINT nDif1 = m_vecTreats.size() - pSession->m_nNumThreats;
		tINT nDif2 = m_vecUtreated.size() - pSession->m_nNumUntreated;

		if( nDif1 ) pSession->IncrementStatistics(pInfo.m_nScopeId, STATISTIC_OFFSET(m_nNumThreats), nDif1);
		if( nDif2 ) pSession->IncrementStatistics(pInfo.m_nScopeId, STATISTIC_OFFSET(m_nNumUntreated), nDif2);

		if( m_pTask && m_pTask->m_pSession == pSession )
			m_pTask->UpdateThreatInfo(pInfo);
	}
	return true;
}

// AVP Prague stamp begin( External (user called) interface method implementation, GetTreatInfo )
// Parameters are:
tERROR AVSTreats::GetTreatInfo( tDWORD p_filter, tDWORD p_pos, cSerializable* p_info )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSTreats::GetTreatInfo method" );

	// Place your code here
	tDWORD nIndex = -1;
	{
		cAutoCS locker(m_lock, true);

		if( !p_info->isBasedOn(cInfectedObjectInfo::eIID) )
			error = errPARAMETER_INVALID;
		else if( p_filter == FILTER_NONE )
		{
			if( p_pos < m_vecTreats.size() )
				nIndex = m_vecTreats[p_pos];
		}
		else
		{
			if( p_pos < m_vecUtreated.size() )
				nIndex = m_vecUtreated[p_pos];
		}
	}

	if( nIndex == -1 || !m_pAVS->GetTreatInfo(nIndex, (cInfectedObjectInfo*)p_info) )
		error = errUNEXPECTED;

	return error;
}
// AVP Prague stamp end


// AVP Prague stamp begin( External (user called) interface method implementation, GetInfo )
// Parameters are:
tERROR AVSTreats::GetInfo( cSerializable* p_info )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSTreats::GetInfo method" );

	// Place your code here
	cAutoCS locker(m_lock, true);

	if( !p_info )
		return errPARAMETER_INVALID;

	if( p_info->isBasedOn(cThreatsInfo::eIID) )
	{
		cThreatsInfo* pInfo = (cThreatsInfo*)p_info;
		pInfo->m_nThreats = m_vecTreats.size();
		pInfo->m_nUntreated = m_vecUtreated.size();
		pInfo->m_tmStamp = m_tmStamp;
	}

	if( p_info->isBasedOn(cScanTreatsInfo::eIID) )
	{
		((cScanTreatsInfo*)p_info)->m_bCompleted = !m_pTask;
		if( m_pTask )
			m_pTask->GetInfo((cScanTreatsInfo*)p_info);
	}

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, Process )
// Parameters are:
tERROR AVSTreats::Process( tDWORD p_filter, const cSerializable* p_process_info )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AVSTreats::Process method" );

	cAskProcessTreats* pScanInfo = (cAskProcessTreats*)p_process_info;
	if( !pScanInfo || !pScanInfo->isBasedOn(cAskProcessTreats::eIID) )
		return errPARAMETER_INVALID;

	// Place your code here
	CProcessThreatsTask* pTask = NULL;
	{
		cAutoCS locker(m_lock, true);
		if( m_pTask )
			return errOBJECT_ALREADY_EXISTS;

		if( pScanInfo->m_nAction == cAskProcessTreats::DISCARD )
			pTask = new CDiscardThreatsTask();
		else
			pTask = m_pTask = new CScanThreatsTask();
	}

	return pTask->Process(this, pScanInfo, p_filter);
}
// AVP Prague stamp end


// AVP Prague stamp begin( External (user called) interface method implementation, ProcessCancel )
// Parameters are:
tERROR AVSTreats::ProcessCancel() 
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSTreats::ProcessCancel method" );

	// Place your code here
	cAutoCS locker(m_lock, true);
	if( m_pTask )
		m_pTask->m_pSession->SetState(TASK_REQUEST_STOP);
	return error;
}
// AVP Prague stamp end

void AVSTreats::ProcessEnd()
{
	bool bClose = false;
	{
		cAutoCS locker(m_lock, true);
		if( m_pTask )
		{
			delete m_pTask;
			m_pTask = NULL;
			bClose = !m_nRef;
		}
	}
	if( bClose )
		sysCloseObject();
}

void AVSTreats::SetCurrentObject(CObjectContext& pCtx)
{
	if( m_pTask )
		m_pTask->SetCurPos(pCtx);
}

void AVSTreats::GetCurrentObject(tDWORD nPos, cStrObj& strName)
{
	if( m_pTask )
	{
		cInfectedObjectInfo pInfo;
		if( PR_SUCC(m_pAVS->GetTreatInfo(m_pTask->m_aIdx[nPos-1].m_nStoredPos, &pInfo)) )
			strName = pInfo.m_strObjectName;
	}
}

// -------------------------------------------

tERROR CProcessThreatsTask::Process(AVSTreats* pTreats, cAskProcessTreats* pScanInfo, tDWORD nFilter)
{
	m_pScanInfo = pScanInfo;
	m_pTreats = pTreats;

	tERROR error = OnInit();
	if( PR_SUCC(error) )
	{
		bool bAll = !pScanInfo->m_aScanRecords.size();
		bool bByIndex = !!pScanInfo->m_bByIndex;

		cDwordVector& aIdx = nFilter == FILTER_NONE ? m_pTreats->m_vecTreats : m_pTreats->m_vecUtreated;
		tDWORD nCount = bAll ? aIdx.size() : pScanInfo->m_aScanRecords.size();

		for(tDWORD i = 0; i < nCount; i++)
		{
			tDWORD nIdx = bAll ? i : pScanInfo->m_aScanRecords[i];
			tDWORD nPos = bByIndex ? (nIdx < aIdx.size() ? aIdx[nIdx] : -1) : nIdx;
			if( nPos != -1 )
				OnEnum(nPos, nIdx);
		}
	}

	if( PR_SUCC(error) )
		error = OnProcess();

	if( PR_FAIL(error) )
		on_exit();

	return error;
}

// -------------------------------------------

tERROR CDiscardThreatsTask::OnEnum(tDWORD nPos, tDWORD nIdx)
{
	m_vecPos.push_back(nPos);
	return errOK;
}

tERROR CDiscardThreatsTask::OnProcess()
{
	m_pTreats->m_pAVS->DiscardTreatsData(m_vecPos);
	return errOK;
}

// -------------------------------------------

CScanThreatsTask::~CScanThreatsTask()
{
	if( m_hIO )
		m_hIO->sysCloseObject();

	if( m_pEngine )
	{
		m_pSession->m_pAVS->UnlockEngine(m_pEngine);
		m_pEngine = NULL;
	}

	if( m_pSession )
	{
		if( m_bOwnSession )
			m_pSession->sysCloseObject();
		else
			m_pSession->sysRelease();
	}

	if( m_hToken )
		m_hToken->sysCloseObject();
}

tERROR CScanThreatsTask::OnInit()
{
	AVSImpl* pAVS = m_pTreats->m_pAVS;

	tERROR error = errOK;
	if( PR_SUCC(pAVS->sysValidateLock(*m_pTreats->m_pInitSession, IID_AVSSESSION, PID_AVS)) )
	{
		m_pSession = m_pTreats->m_pInitSession;
		m_bOwnSession = cFALSE;
	}
	else
	{
		error = pAVS->CreateSession((cAVSSession**)&m_pSession, (cTask*)pAVS,
			ENGINE_DEFAULT, OBJECT_ORIGIN_GENERIC);

		if( PR_SUCC(error) )
		{
			error = m_pSession->SetState(TASK_REQUEST_RUN);
			if( PR_FAIL(error) )
				PR_TRACE((g_root,prtALWAYS_REPORTED_MSG,"avs\tScan threats failed (cannot load engine)..."));
		}

		if( PR_SUCC(error) )
		{
			m_pSession->m_nTaskId = m_pTreats->m_nTaskId;
			m_pSession->m_strTaskType = L"avs";
			m_pSession->m_bVerifyThreats = m_pScanInfo->m_nAction == cAskProcessTreats::VERIFY;
			error = pAVS->GetTreats(m_pTreats->m_nTaskId, (hAVSTREATS*)&m_pSession->m_pTreats);
		}
	}

    if( PR_SUCC(error) )
        error = pAVS->LockEngine(m_pSession->m_eBaseEngine, &m_pEngine);

	if( PR_SUCC(error) )
	{
		cIOObj pTmpIo(*m_pTreats, (cString*)NULL, fACCESS_RW, fOMODE_OPEN_ALWAYS, PID_TMPFILE);
		if( PR_SUCC(error = pTmpIo.last_error()) )
			m_hIO = pTmpIo.relinquish();
	}

	if( PR_SUCC(error) )
	{
		if( m_pScanInfo->m_bSyncScan )
			m_bUseExcludes = cFALSE;

		if( m_pScanInfo->m_nAction == cAskProcessTreats::DELETE_ )
		{
			m_nScanAction = SCAN_ACTION_DISINFECT;
			m_bTryDisinfect = cFALSE;
		}
		else
			m_nScanAction = SCAN_ACTION_ASKUSER;

		m_nScanFilter = SCAN_FILTER_ALL_OBJECTS;
		m_bUseIChecker = cFALSE;
		m_bUseIStreams = cFALSE;
		m_bAskPassword = m_pScanInfo->m_bAskPassword;
		m_bEmulEnable = cTRUE;
		m_nEmulValue = 100;

		m_ProcessInfo.m_nActionSessionId = m_pScanInfo->m_nSessionId;
		m_ProcessInfo.m_nActionPID = -1; 
	}

	return error;
}

tERROR CScanThreatsTask::OnEnum(tDWORD nPos, tDWORD nIdx)
{
	cInfectedObjectInfo pThreatInfo;
	if( PR_FAIL(m_pTreats->m_pAVS->GetTreatInfo(nPos, &pThreatInfo)) )
		return errUNEXPECTED;

	if( !pThreatInfo.m_pReopenData.size() )
		return errOK;

	if( m_pScanInfo->m_nAction != cAskProcessTreats::VERIFY &&
			!IS_INFECTED(pThreatInfo.m_nObjectStatus) )
		return errOK;

	m_pEngine->AddReopenData(m_hIO, pThreatInfo.m_pReopenData.data(), nPos, ++m_nCount);

	tTreatInfo pInfo(pThreatInfo.m_qwUniqueId, nPos, OBJSTATUS_UNKNOWN);
	pInfo.m_nProcessIdx = nIdx;

	if( pThreatInfo.m_tmScanningBases == -1 ) // external detect
	{
		pInfo.m_pData = new cDetectObjectInfo(pThreatInfo);
	}

	if( !m_bOwnSession )
		pInfo.m_nScopeId = pThreatInfo.m_nObjectScope;

	m_aIdx.push_back(pInfo);
	return errOK;
}

tERROR CScanThreatsTask::OnProcess()
{
	if( !m_nCount )
		return errOBJECT_NOT_FOUND;

	m_pTreats->sysCreateObjectQuick((hOBJECT*)&m_hToken, IID_TOKEN);
	if( !m_pScanInfo->m_bSyncScan )
		return start(*m_pTreats->m_pAVS);

	do_work();
	on_exit();
	return errOK;
}

void CScanThreatsTask::UpdateThreatInfo(tTreatInfo& pInfo)
{
	for(tDWORD i = 0; i < m_aIdx.size(); i++)
		if( m_aIdx[i].m_qwUnique == pInfo.m_qwUnique )
		{
			m_aIdx[i].m_nStatus = pInfo.m_nStatus;
			return;
		}
}

void CScanThreatsTask::GetInfo(cScanTreatsInfo* pInfo)
{
	pInfo->m_nCurrentThreat = m_pSession ? m_aIdx[m_nPos].m_nProcessIdx : 0;
	if( pInfo->m_bCompleted )
	{
		pInfo->m_nProgress = 100;
	}
	else
	{
		pInfo->m_nProgress = m_nCount ? m_nPos * 100 / m_nCount : 0;
		if( !pInfo->m_nProgress )
			pInfo->m_nProgress = 1;
	}
}

void CScanThreatsTask::SetCurPos(CObjectContext& pCtx)
{
	tDWORD userdata = pCtx.m_pObject->propGetDWord(CObjectContext::m_propTreatUserData);
	if (userdata == 0) // пришёл не наш объект
		return;
	m_nPos = userdata-1;

	tTreatInfo& pInfo = m_aIdx[m_nPos];
	pCtx.m_DetectInfo = (cDetectObjectInfo*)pInfo.m_pData;
	pCtx.m_nObjectScope = pInfo.m_nScopeId;
}

void CScanThreatsTask::do_work()
{
	PR_TRACE((g_root,prtALWAYS_REPORTED_MSG,"avs\tScan threats begin..."));

	if( m_hToken )
		m_hToken->Impersonate();

	m_hIO->propSetDWord(pgOBJECT_ORIGIN, OID_REOPEN_DATA);
	{
		CScanContext ctx((hOBJECT)m_hIO, m_pSession, this, &m_ProcessInfo);
		ctx.m_fTreatsProcessing = 1;
		ctx.Process();
	}

	for(tDWORD i = 0; i < m_nCount; i++)
	{
		if( !(m_ProcessInfo.m_nProcessStatusMask & cScanProcessInfo::CANCELED) )
			if( m_aIdx[i].m_nStatus == OBJSTATUS_UNKNOWN )
				m_pTreats->m_pAVS->UpdateTreatStatus(m_pSession, m_aIdx[i]);

		if( m_aIdx[i].m_pData )
			delete (cDetectObjectInfo*)m_aIdx[i].m_pData;
	}

	if( m_hToken )
		m_hToken->Revert();

	PR_TRACE((g_root,prtALWAYS_REPORTED_MSG,"avs\tScan threats done..."));
}

void CScanThreatsTask::on_exit()
{
	m_pTreats->ProcessEnd();
}

// -------------------------------------------

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "AVSTreats". Register function
tERROR AVSTreats::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(AVSTreats_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, AVSTreats_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "AVSTreats", 10 )
mpPROPERTY_TABLE_END(AVSTreats_PropTable)
// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(AVSTreats)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(AVSTreats)
// AVP Prague stamp end

// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(AVSTreats)
	mEXTERNAL_METHOD(AVSTreats, GetTreatInfo)
	mEXTERNAL_METHOD(AVSTreats, Process)
	mEXTERNAL_METHOD(AVSTreats, GetInfo)
	mEXTERNAL_METHOD(AVSTreats, ProcessCancel)
mEXTERNAL_TABLE_END(AVSTreats)
// AVP Prague stamp end

// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "AVSTreats::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_AVSTREATS,                          // interface identifier
		PID_AVS,                                // plugin identifier
		0x00000000,                             // subtype identifier
		AVSTreats_VERSION,                      // interface version
		VID_PETROVITCH,                         // interface developer
		&i_AVSTreats_vtbl,                      // internal(kernel called) function table
		(sizeof(i_AVSTreats_vtbl)/sizeof(tPTR)),// internal function table size
		&e_AVSTreats_vtbl,                      // external function table
		(sizeof(e_AVSTreats_vtbl)/sizeof(tPTR)),// external function table size
		AVSTreats_PropTable,                    // property table
		mPROPERTY_TABLE_SIZE(AVSTreats_PropTable),// property table size
		sizeof(AVSTreats)-sizeof(cObjImpl),     // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"AVSTreats(IID_AVSTREATS) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end

// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call AVSTreats_Register( hROOT root ) { return AVSTreats::Register(root); }
// AVP Prague stamp end


// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  30 January 2007,  14:21 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- avssession.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Interface version,  )
#define AVSSession_VERSION ((tVERSION)1)
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>

#include <AV/plugin/p_avs.h>
#include <AV/plugin/p_startupenum2.h>

#include <ProductCore/plugin/p_cryptohelper.h>
// AVP Prague stamp end

#include "avs.h"

// AVP Prague stamp begin( Includes for interface,  )
#include "avssession.h"
// AVP Prague stamp end

#define  PERFORMANCE_CHECK_PERIOD    1000
#define  PERFORMANCE_MAX_SLEEP       (PERFORMANCE_CHECK_PERIOD / 10)

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "AVSSession". Static(shared) property table variables
// AVP Prague stamp end

tPROPID	AVSSession::m_propTaskType;
tPROPID	AVSSession::m_propTaskId;

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
tERROR AVSSession::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter AVSSession::ObjectInitDone method" );

	// Place your code here
	tMsgHandlerDescr hdl[] = {
		{ (hOBJECT)this, rmhDECIDER , pmc_ENGINE_IO,     IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhDECIDER , pmc_IO,            IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhDECIDER , pmc_PROCESSING,	 IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhDECIDER , pmc_ENGINE_ACTION, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ (hOBJECT)this, rmhLISTENER, pmc_STARTUPENUM2,  IID_ANY, PID_ANY, IID_ANY, PID_ANY },
	};
	memcpy(&m_hndList, hdl	, sizeof(m_hndList));

	sysRegisterMsgHandler(pmc_REMOTE_GLOBAL, rmhDECIDER, g_root, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	m_pAVS = (AVSImpl*)sysGetParent(IID_AVS);
	m_nState = TASK_STATE_RUNNING;
	m_nTaskId = m_task->propGetDWord(m_propTaskId);
	m_strTaskType.assign(m_task, m_propTaskType);

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&m_ask_lock, IID_CRITICAL_SECTION);

	PR_TRACE_A1( this, "Leave AVSSession::ObjectInitDone method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
tERROR AVSSession::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter AVSSession::ObjectPreClose method" );

	// Place your code here
	if( m_nTotalSleepTime )
		PR_TRACE((this, prtIMPORTANT, "%S\tTotal sleep time=%d", m_strTaskType.data(), m_nTotalSleepTime));

	while( m_nProcessCount )
	{
		PR_TRACE((this, prtALWAYS_REPORTED_MSG, "AVSSession\t waiting for processing objects(%d)", m_nProcessCount));
		PrSleep(50);
	}

	if( m_pAVS && m_bEngineLocked )
	{
		m_pAVS->LockEngines(m_eEngineMask, cFALSE);
		PR_TRACE((this, prtALWAYS_REPORTED_MSG, "AVSSession\t unlocked engine for task %S", m_strTaskType.data()));
	}

	if( m_pTreats )
		m_pAVS->ReleaseTreats((hAVSTREATS)m_pTreats);

	if( m_pStartupSvc )
		m_pAVS->m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_pStartupSvc);

	if( m_hCrypter )
		m_pAVS->m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hCrypter);

	PR_TRACE_A1( this, "Leave AVSSession::ObjectPreClose method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
tERROR AVSSession::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter AVSSession::MsgReceive method" );

	// Place your code here
	if( p_msg_cls_id == pmc_REMOTE_GLOBAL )
	{
		if( p_msg_id == pm_AVS_ASK_ACTION )
		{
			cAskObjectAction* pRequest = (cAskObjectAction*)p_par_buf;
			if( pRequest && pRequest->isBasedOn(cAskObjectAction::eIID)  )
			{
				if( pRequest->m_nActionId == cAskObjectAction::ACTIVE_DETECT )
					return m_bActiveDisinfection ? errOPERATION_CANCELED : errOK;
				else if( pRequest->m_nTaskId == m_nTaskId )
					return FindAskResult(*pRequest);
			}

		}
		return errOK;
	}

	if( p_msg_cls_id == pmc_IO && p_msg_id == pm_IO_CREATE_FAILED )
		return errOK;

	CObjectContext* ctx = (CObjectContext*)p_send_point->propGetPtr(CObjectContext::m_propContext);
	CObjectContext* ctx_parent;

	if( !ctx || (p_send_point != ctx->m_pObject && p_msg_cls_id != pmc_PROCESSING) )
	{
		hOBJECT pParent = p_send_point->sysGetParent(IID_ANY);
		ctx_parent = pParent ? (CObjectContext*)pParent->propGetPtr(CObjectContext::m_propContext) : ctx;

		if( !ctx )
			ctx = ctx_parent;
		else
			ctx = ctx_parent->GetNextContext(p_send_point);
	}

	if( ctx )
	{
		CSaveContext pSaveCtx(ctx);

		ctx->m_pContext = p_ctx;
		ctx->m_pBuff = p_par_buf;
		ctx->m_pBuffLen = p_par_buf_len;

		error = ctx->OnMsgRecieve(p_msg_cls_id, p_msg_id);
	}

	PR_TRACE_A1( this, "Leave AVSSession::MsgReceive method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, PreProcessObject )
tERROR AVSSession::PreProcessObject( cSerializable* p_process_info, const cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSSession::PreProcessObject method" );

	// Place your code here
	CScanContext ctx(NULL, this, (cProtectionSettings*)p_settings, (cScanProcessInfo*)p_process_info);

	if( PR_FAIL(ctx.CheckState()) )
		return errOPERATION_CANCELED;

	if( ctx.CheckExcludeByMasks(true) )
		return errOK_DECIDED;

//	if( ctx.CheckExcludeByStreams() )
//		return errOK_DECIDED;

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, ProcessObject )
tERROR AVSSession::ProcessObject( hOBJECT p_object, cSerializable* p_process_info, const cSerializable* p_settings, cSerializable* p_statistics )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSSession::ProcessObject method" );

	// Place your code here
	cScanProcessInfo* pInfo = (cScanProcessInfo*)p_process_info;
	if( !pInfo )
		return errPARAMETER_INVALID;

	if( pInfo->m_fnAsyncResult )
	{
		CAsyncScanContext* pctx = new CAsyncScanContext(p_object, this, (cProtectionSettings*)p_settings, pInfo);

		if( PR_FAIL(pctx->CheckState()) || !pctx->IsShouldBeScan(true) )
		{
			delete pctx;
			return errOPERATION_CANCELED;
		}
		
		return pctx->start(*m_pAVS);
	}

	CScanContext ctx(p_object, this, (cProtectionSettings*)p_settings, pInfo, p_statistics);

	if( PR_FAIL(ctx.CheckState()) )
		return errOPERATION_CANCELED;

	if( ctx.IsShouldBeScan(true) )
		error = ctx.Process();

	if( p_statistics )
		ctx.GetStatistics(p_statistics);

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, ProcessStream )
tERROR AVSSession::ProcessStream( tDWORD p_chunk_type, tPTR p_chunk, tDWORD p_size, cSerializable* p_process_info, const cSerializable* p_settings, cSerializable* p_statistic )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSSession::ProcessStream method" );

	// Place your code here
	if( !p_process_info )
		return errPARAMETER_INVALID;

	cScanProcessInfo* info = (cScanProcessInfo*)p_process_info;
	CScanContext*& ctx = (CScanContext*&)info->m_pCtx;

	if( p_chunk_type == CHUNK_TYPE_FIRST )
		ctx = new CScanContext(NULL, this, (cProtectionSettings*)p_settings, info, NULL, true);

	if( !ctx )
		return errUNEXPECTED;

	error = ctx->CheckState();

	if( PR_SUCC(error) && (!p_size && (p_chunk_type == CHUNK_TYPE_FIRST)) )
		error = warnFALSE;

	if( error == errOK )
	{
		if( !ctx->m_pObject )
			ctx->m_pObject = info->m_hICheckObject;

		error = ctx->GetEngine()->ProcessStream(*ctx, (enChunkType)p_chunk_type, p_chunk, p_size);
		ctx->UpdateProcessInfo();
	}

	if( p_statistic )
		ctx->GetStatistics(p_statistic);

	if( p_chunk_type == CHUNK_TYPE_LAST )
		delete ctx, ctx = NULL;

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistic )
tERROR AVSSession::GetStatistic( cSerializable* p_statistics )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter AVSSession::GetStatistic method" );

	// Place your code here
	if( !p_statistics )
		return errPARAMETER_INVALID;

	cAutoCS locker(m_lock, true);
	error = p_statistics->assign(*this, false);

	PR_TRACE_A1( this, "Leave AVSSession::GetStatistic method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
tERROR AVSSession::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter AVSSession::SetState method" );

	// Place your code here
	if( p_state == TASK_REQUEST_RUN && !m_bEngineLocked )
	{
		m_bEngineLocked = cTRUE;
		error = m_pAVS->LockEngines(m_eEngineMask, cTRUE, &m_eBaseEngine);
		if( PR_FAIL(error) )
		{
			PR_TRACE((this, prtERROR, "AVSSession\tfailed to lock engine (mask 0x%08x) for task %S, err = %terr", m_eEngineMask, m_strTaskType.data(), error));
//			m_task->sysSendMsg(pmc_TASK_STATE_CHANGED, TASK_STATE_FAILED, 0, &error, 0);
			return error;
		}

		if( m_eBaseEngine == ENGINE_KLAV )
			m_hndList[0].msg_cls = pmc_OS_VOLUME;

		PR_TRACE((this, prtALWAYS_REPORTED_MSG, "AVSSession\tlocked engine for task %S", m_strTaskType.data()));
	}

	switch(p_state)
	{
	case TASK_REQUEST_RUN:   m_nState = TASK_STATE_RUNNING; break;
	case TASK_REQUEST_PAUSE: m_nState = TASK_STATE_PAUSED; break;
	case TASK_REQUEST_STOP:  m_nState = TASK_STATE_STOPPED; break;
	default: return errTASK_STATE_UNKNOWN;
	}

	PR_TRACE_A1( this, "Leave AVSSession::SetState method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, ResetStatistic )
// Parameters are:
//! tERROR AVSSession::ResetStatistic( cSerializable* p_statistics )
tERROR AVSSession::ResetStatistic( cSerializable* p_statistics, tBOOL p_in )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSSession::ResetStatistic method" );

	// Place your code here
	cAutoCS locker(m_lock, true);
	if( p_in )
	{
		error = cProtectionStatisticsEx::assign(*p_statistics, false);
	}
	else
	{
		error = p_statistics->assign(*this, false);
		cProtectionStatistics::init();
	}
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, RegisterScope )
// Parameters are:
tERROR AVSSession::RegisterScope( tDWORD* p_scope, const cSerializable* p_info )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSSession::RegisterScope method" );

	// Place your code here
	if( !p_info || !p_info->isBasedOn(cScanObject::eIID) )
		return errPARAMETER_INVALID;

	cAutoCS locker(m_lock, true);

	cScanObjectStatistics& pStat = m_aObjStat.push_back();
	pStat.m_pScanObj = *(cScanObject*)p_info;
	pStat.m_nScopeId = m_aObjStat.size()-1;

	if( p_scope )
		*p_scope = pStat.m_nScopeId;

	return error;
}
// AVP Prague stamp end

void AVSSession::PerformABS(tDWORD nCurrentTime, tDWORD nABSNumber)
{
	if( nABSNumber == 1 )
		nABSNumber = 40;

	m_nYeildCounter++;

	tDWORD nRequestPeriod = nCurrentTime - m_nLastABSRequest;
	if( nRequestPeriod >= PERFORMANCE_CHECK_PERIOD )
	{
		tBOOL bNeedSleep = cFALSE;
		tDWORD nDiskUsage = 0, nProcessorUsage = 0;
		tQWORD qwTimeCurrent, qwTotalWaitTime;

		if( m_pAVS->m_avspm )
		{
			if( m_pAVS->m_avspm->GetProcessorUsage(&nProcessorUsage) == /*no user input*/warnFALSE )
				nABSNumber = 95;

			bNeedSleep = nProcessorUsage >= nABSNumber;
		}

		if( !bNeedSleep && FSGetDiskStat && PR_SUCC(FSGetDiskStat(&qwTimeCurrent, &qwTotalWaitTime)) &&
			qwTimeCurrent && m_qwTimeCurrent != qwTimeCurrent )
		{
			nDiskUsage = (tDWORD)(100 * (qwTotalWaitTime - m_qwTotalWaitTime) / (qwTimeCurrent - m_qwTimeCurrent));
			bNeedSleep = nDiskUsage >= nABSNumber;

			m_qwTimeCurrent = qwTimeCurrent;
			m_qwTotalWaitTime = qwTotalWaitTime;
		}

		if( bNeedSleep )
		{
			if( !m_nSleepTimeout && m_nYeildCounter )
				m_nSleepTimeout = 1 + nRequestPeriod / m_nYeildCounter;
			else
				m_nSleepTimeout *= 2;

			if( m_nSleepTimeout > PERFORMANCE_MAX_SLEEP )
				m_nSleepTimeout = PERFORMANCE_MAX_SLEEP;

			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "%S\t: disk=%d, processor=%d => sleep=%d",
				m_strTaskType.data(), nDiskUsage, nProcessorUsage, m_nSleepTimeout));
		}
		else
			m_nSleepTimeout = 0;

		m_nLastABSRequest = nCurrentTime;
		m_nYeildCounter = 0;
	}

	if( m_nSleepTimeout )
	{
		PrSleep(m_nSleepTimeout);
		IncrementStatistics(-1, STATISTIC_OFFSET(m_nTotalSleepTime), m_nSleepTimeout);
	}
}

void AVSSession::CleanupStartupObject(cStringObj& strObjectName, hOBJECT pObject, cStringObj& strDetectName)
{
	if( !m_pAVS->m_tm )
		return;

	if( !m_pStartupSvc )
		m_pAVS->m_tm->GetService(TASKID_TM_ITSELF, (hOBJECT)this, sid_TM_STARTUP_SVC, (hOBJECT*)&m_pStartupSvc);

	if( m_pStartupSvc )
	{
		if( m_bActiveDisinfection )
			m_pStartupSvc->propSetBool(plADVANCED_DISINFECTION, cTRUE);

		m_pStartupSvc->CleanUp(cAutoString(strObjectName), pObject, strDetectName.c_str(cCP_ANSI));

		tDWORD nTreatPos = pObject->propGetDWord(CObjectContext::m_propTreatUserData);
		if( m_pTreats && nTreatPos )
		{
			cStrObj strOriginName;
			m_pTreats->GetCurrentObject(nTreatPos, strOriginName);
			if( !strOriginName.empty() && strOriginName != strObjectName )
				m_pStartupSvc->CleanUp(cAutoString(strOriginName), pObject, strDetectName.c_str(cCP_ANSI));
		}
	}
}

void AVSSession::SetCurrentObject(CObjectContext& pCtx)
{
	cAutoCS locker(m_lock, true);

	m_strCurObject = pCtx.m_strObjectName;
	m_tmCurObject = cDateTime::now_utc();

	if( m_pTreats && pCtx.m_pScanCtx->m_fTreatsProcessing )
		m_pTreats->SetCurrentObject(pCtx);
}

void AVSSession::IncrementStatistics(tDWORD nScope, tDWORD nOffset, tINT nNum)
{
	cAutoCS locker(m_lock, true);

	(*(tLONG*)((tBYTE*)(cProtectionStatistics*)this + nOffset))+=nNum;
	if( nScope != -1 && nScope < m_aObjStat.size() )
	{
		cScanObjectStatistics& pStat = m_aObjStat.at(nScope);
		(*(tLONG*)((tBYTE*)(cProtectionStatistics*)&pStat + nOffset))+=nNum;
	}
}

bool AVSSession::UpdateDetectedInfo(cInfectedObjectInfo* pInfInfo)
{
	return m_pAVS->UpdateTreatInfo(this, pInfInfo);
}

bool AVSSession::MSVerify(cStrObj& strFName)
{
	if( !m_hCrypter )
		m_pAVS->m_tm->GetService(TASKID_TM_ITSELF, (hOBJECT)this, CRYPTOHELPER_SERVICE_NAME, (hOBJECT*)&m_hCrypter);

	if( !m_hCrypter || PR_FAIL(m_hCrypter->IsMSTrusted(strFName.data())) )
		return false;

	PR_TRACE((this, prtIMPORTANT, "AVSSession::MSVerify\t%S verified", strFName.data()));
	return true;
}

tERROR AVSSession::SendReport(tDWORD nMsgClass, cDetectObjectInfo* pData, cObj* pObject)
{
	if( m_task != (cTask*)m_pAVS )
		return m_task->sysSendMsg(nMsgClass, pData->m_nObjectStatus, pObject, pData, SER_SENDMSG_PSIZE);
	else
		return m_pAVS->m_tm->AddRecordToTaskReport(m_nTaskId, nMsgClass, pData);
}

tERROR AVSSession::FindAskResult(cAskObjectAction &pRequest)
{
	tAskResult *nCacheResult = NULL;

	cAutoCS locker(m_ask_lock, true);

	int nCount = m_vecAskResults.size();
	for(int i = 0; i < nCount; i++)
	{
		tAskResult& res = m_vecAskResults[i];

		if( pRequest.m_nActionId != res.m_nAction )
			continue;

		if( pRequest.m_nDetectType != res.m_nDetectType )
			continue;

		if( pRequest.m_nActionsMask != res.m_nActionsMask )
		{
			// if previous Result can be used for this request
			if( !( pRequest.m_nActionsMask & res.m_nResult ) )
				continue;

			if( res.m_nResult == ACTION_CANCEL )
				if( !(res.m_nActionsMask & (ACTION_QUARANTINE|ACTION_DISINFECT|ACTION_DELETE)) &&
						res.m_nActionsMask != pRequest.m_nActionsMask )
					continue;

			// if user said Delete having Disinect available
			if( res.m_nResult == ACTION_DELETE )
				if( !(res.m_nActionsMask & pRequest.m_nActionsMask &
						(ACTION_QUARANTINE|ACTION_DISINFECT)) )
					continue;
		}

		nCacheResult = &res;
		break;
	}

	if( !nCacheResult )
		return errOK;

	pRequest.m_nResultAction = nCacheResult->m_nResult;
	return errOK_DECIDED;
}

enActions AVSSession::AskObjectAction(tActionId nAction, cAskObjectAction &pRequest, bool bUseCache)
{
	enActions& nResult = pRequest.m_nResultAction;
	pRequest.m_nActionId = nAction;

	tERROR error = FindAskResult(pRequest);

	if( error != errOK_DECIDED && (m_pAVS->m_bIsRemote || nAction == cAskObjectAction::ACTIVE_DETECT) )
	{
		if( pRequest.isBasedOn(cAskQBAction::eIID) )
			error = m_task->sysSendMsg(pmc_AVS_QB_ACTION, 0, (cObj*)((cAskQBAction&)pRequest).m_hSource,
				&pRequest, SER_SENDMSG_PSIZE); 
		else
			error = m_task->sysSendMsg(pmc_REMOTE_GLOBAL, pm_AVS_ASK_ACTION, 0, &pRequest, SER_SENDMSG_PSIZE);
	}

	if( error == errOPERATION_CANCELED )
		return ACTION_CANCEL;

	if( (error == errOK_DECIDED) && ((nResult == ACTION_CANCEL) || bUseCache && (nResult != ACTION_UNKNOWN)) )
	{
		pRequest.m_nApplyToAll = APPLYTOALL_TRUE;
		return nResult;
	}

	cAutoCS locker(m_ask_lock, true);

	tERROR err = m_task->sysSendMsg(pmcASK_ACTION, nAction, NULL, &pRequest, SER_SENDMSG_PSIZE);
	if( err != errOK_DECIDED )
	{
		if( err == errOUT_OF_SPACE )
			pRequest.m_nNonCuredReason = NCREASON_OUTOFSPACE;
		return ACTION_CANCEL;
	}

	if( !(nResult & pRequest.m_nActionsMask) )
		nResult = pRequest.m_nDefaultAction;

	if( pRequest.m_nApplyToAll )
	{
		tAskResult& res = m_vecAskResults.push_back(tAskResult(nAction, pRequest.m_nActionsMask));
		res.m_nResult = nResult;
		res.m_nDetectType = pRequest.m_nDetectType;
	}

	return nResult;
}

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "AVSSession". Register function
tERROR AVSSession::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(AVSSession_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, AVSSession_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "AVS session object", 19 )
mpPROPERTY_TABLE_END(AVSSession_PropTable)
// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(AVSSession)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(AVSSession)
// AVP Prague stamp end

// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(AVSSession)
	mEXTERNAL_METHOD(AVSSession, PreProcessObject)
	mEXTERNAL_METHOD(AVSSession, ProcessObject)
	mEXTERNAL_METHOD(AVSSession, GetStatistic)
	mEXTERNAL_METHOD(AVSSession, SetState)
	mEXTERNAL_METHOD(AVSSession, ResetStatistic)
	mEXTERNAL_METHOD(AVSSession, RegisterScope)
	mEXTERNAL_METHOD(AVSSession, ProcessStream)
mEXTERNAL_TABLE_END(AVSSession)
// AVP Prague stamp end

// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_A0( root, "Enter AVSSession::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_AVSSESSION,                         // interface identifier
		PID_AVS,                                // plugin identifier
		0x00000000,                             // subtype identifier
		AVSSession_VERSION,                     // interface version
		VID_PETROVITCH,                         // interface developer
		&i_AVSSession_vtbl,                     // internal(kernel called) function table
		(sizeof(i_AVSSession_vtbl)/sizeof(tPTR)),// internal function table size
		&e_AVSSession_vtbl,                     // external function table
		(sizeof(e_AVSSession_vtbl)/sizeof(tPTR)),// external function table size
		AVSSession_PropTable,                   // property table
		mPROPERTY_TABLE_SIZE(AVSSession_PropTable),// property table size
		sizeof(AVSSession)-sizeof(cObjImpl),    // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"AVSSession(IID_AVSSESSION) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave AVSSession::Register method, ret %terr", error );
// AVP Prague stamp end

	if( PR_SUCC(error) )
		error = CObjectContext::InitCustomProps(root);

	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(&m_propTaskType, cpTASK_MANAGER_TASK_TYPE_NAME, pTYPE_STRING);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(&m_propTaskId, cpTASK_MANAGER_TASK_ID, pTYPE_DWORD);

// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call AVSSession_Register( hROOT root ) { return AVSSession::Register(root); }
// AVP Prague stamp end


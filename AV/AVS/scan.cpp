// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- avs.cpp
// -------------------------------------------

#include "avs.h"
#include <Prague/iface/i_buffer.h> 

// -------------------------------------------

CScanContext::CScanContext(hOBJECT pObject, AVSSession *pSession, cProtectionSettings *pSettings,
		cScanProcessInfo *pProcessInfo, cSerializable *pDetectInfo, bool bForStream) :
	CObjectContext(pObject),
	m_Session(pSession),
	m_Settings(pSettings),
	m_ProcessInfo(pProcessInfo),
	m_Engine(NULL),
	m_pNative(NULL),
	m_pScanTop(NULL),
	m_nProcessId(0),
	m_nScopePos(-1),
	m_nReason(NCREASON_NONE),
	m_nClassMaskEx(0),
	m_nClassMaskForUnchanged(0),
	m_nIStreamsCtx(0),
	m_dtTimeStamp(0)
{
	if( !m_Settings )
	{
		m_Session->m_pAVS->ProcessBegin(*this);
		return;
	}

	if( m_Settings->m_bActiveDisinfection )
		m_Session->m_bActiveDisinfection = cTRUE;

	if( !m_pObject )
	{
		m_pObject = m_ProcessInfo->m_hICheckObject;
		if( bForStream )
			m_fStreamProcessing = 1;
		else
			m_fPreProcessing = 1;
	}
	else
	{
		m_ProcessInfo->m_hICheckObject = m_pObject;

		if( m_ProcessInfo->m_nProcessStatusMask & cScanProcessInfo::EXTRENAL_DETECT )
			m_DetectInfo = (cDetectObjectInfo*)pDetectInfo;

		if( m_ProcessInfo->m_nProcessStatusMask & cScanProcessInfo::PRE_PROCESSED )
			m_fPreProcessed = 1;

		if( m_ProcessInfo->m_nProcessStatusMask & cScanProcessInfo::UNCHANGED )
			m_fUnchangedObject = 1;
		
		tDWORD dwOrigin = m_pObject->propGetDWord(pgOBJECT_ORIGIN);
		if( dwOrigin == OID_QUARANTINED_OBJECT )
		{
			m_pObject->propSetDWord(pgOBJECT_ORIGIN, OID_GENERIC_IO);
			m_fQuarantineProcessing = 1;
		}

		if( m_pObject->propGetDWord(pgINTERFACE_ID) == IID_OS )
			m_fOSObject = 1;

		m_bUseABS = m_Session->m_pAVS->m_settings.m_bUseABS &&
			m_Settings->m_nABSNumber && m_Settings->m_nScanAction != SCAN_ACTION_DETECT;
	}

	m_Session->m_pAVS->ProcessBegin(*this);
	if( !m_Engine )
		return;

	m_nClassMask = m_Engine->InitClassMask(*m_Settings);
	m_nUseICheckDays = m_Settings->m_nMandatoryScanPeriod;
	m_nObjectScope = m_ProcessInfo->m_nObjectScopeId;

	if( m_Settings->m_bEmulEnable )
	{
		m_nClassMaskEx |= IC_BIT_RANGE_EMUL_ON;

		for(int i = 0; i < 8; i++)
		{
			if( (1 << i) > (int)m_Settings->m_nEmulValue )
				break;
			m_nClassMaskEx |= (tQWORD)1 << (32 + i);
		}
	}

	if( m_Settings->m_pForUnchangedObjects )
	{
		m_nClassMaskForUnchanged = m_Engine->InitClassMask(*m_Settings->m_pForUnchangedObjects);
		m_nClassMaskForUnchanged &= m_nClassMask;
		if( m_nClassMaskForUnchanged == m_nClassMask )
			m_nClassMaskForUnchanged = 0;
	}

	m_Engine->InitContext(*this); 

	if( m_pObject )
		SetActionClassMask(m_nClassMask);
}

CScanContext::~CScanContext()
{
	CleanUp();
	m_Session->m_pAVS->ProcessEnd(*this);
}

void CScanContext::CleanUp()
{
	if( m_nIStreamsCtx )
		m_Engine->ICFree(m_nIStreamsCtx), m_nIStreamsCtx = NULL;

	if( m_Engine )
		m_Engine->FreeContext(*this);

	CObjectContext *ctx = m_pNext, *next;
	while( ctx )
	{
		next = ctx->m_pNext;
		delete ctx;
		ctx = next;
	}
	m_pNext = NULL;
}

inline void CScanContext::SetActionClassMask(tDWORD nClassMask)
{
	m_pObject->propSetDWord(m_propActionClassMask, nClassMask);
}

void CScanContext::GetStatistics(cSerializable* pStat)
{
	if( pStat->isBasedOn(cProtectionStatistics::eIID) )
		pStat->assign((const cProtectionStatistics&)*this,true);

	if( pStat->isBasedOn(cDetectObjectInfo::eIID) && m_InfCtx )
	{
		CopyInfectedInfo((cDetectObjectInfo*)pStat);
		if( pStat->isBasedOn(cInfectedObjectInfo::eIID) )
			((cInfectedObjectInfo*)pStat)->m_nNonCuredReason = m_InfCtx->m_nNonCuredReason;
	}
}

void CScanContext::UpdateProcessInfo()
{
	m_ProcessInfo->m_nProcessStatusMask |= m_nStatus;

	if( m_nStatus & cScanProcessInfo::DETECTED )
	{
		if( !(m_nStatus & cScanProcessInfo::DISINFECTED) )
			m_Session->m_bSomeUntreated = cTRUE;

		if( m_Settings->m_nScanAction == SCAN_ACTION_DETECT )
		{
			m_Session->IncrementStatistics(m_nObjectScope, STATISTIC_OFFSET(m_nNumArchived), -m_nNumArchived);
			m_Session->IncrementStatistics(m_nObjectScope, STATISTIC_OFFSET(m_nNumPacked), -m_nNumPacked);
		}
	}
}

void CScanContext::ICUpdate(tPID nICheckerPID, tBOOL bDeleteStatus, tPTR pICheckerCtx, cObj* pObject)
{
	tQWORD qwBitCompareData = (m_nClassMaskEx | m_nClassMask);
	if( nICheckerPID == PID_ICHECKERSA )
	{
		pICheckerCtx = m_nIStreamsCtx;
		pObject = m_ProcessInfo->m_hICheckObject;

		if( !pObject )
			return;

		tBYTE pCtxData[AVPMGR_ICDATA_SIZE];
		if( !pICheckerCtx && (m_ProcessInfo->m_nProcessStatusMask & cScanProcessInfo::NTFS_KNOWN) )
		{
			if( PR_SUCC(pObject->propGet(NULL, m_propIStreamsCtx, pCtxData, AVPMGR_ICDATA_SIZE)) )
			{
				PR_TRACE((m_Engine, prtNOTIFY, "UpdateIS get OK"));
				pICheckerCtx = pCtxData;
			}
			else
			{
				PR_TRACE((m_Engine, prtNOTIFY, "UpdateIS get failed"));
			}
		}

		if( !pICheckerCtx )
			return;

		if( m_Settings->m_bUseMSVerify && m_Session->MSVerify(m_strObjectName) )
			qwBitCompareData |= IC_BIT_RANGE_MSTRUSTED;
	}

	GetEngine()->ICUpdate(pObject, nICheckerPID, pICheckerCtx, qwBitCompareData, bDeleteStatus);
}

bool CScanContext::CheckByIChecker(tPTR& pICheckerCtx, cObj* pObject, tPID nICheckerPID)
{
	tBOOL bScanUnchanged = !!m_Settings->m_bScanUnchangedObjects;
	tBOOL bUseICheck = (nICheckerPID == PID_ICHECKERSA) ? m_Settings->m_bUseIStreams : m_Settings->m_bUseIChecker;
	tBOOL bNeedToCheckUnchanged = (pObject == m_pObject) && (!bScanUnchanged || m_nClassMaskForUnchanged);

	if( !m_Settings->m_bScanThreats )
		return false;

	if( !bUseICheck && !bNeedToCheckUnchanged )
		return false;

	GetEngine()->ICGetData(pObject, nICheckerPID, pICheckerCtx);

	if( !pICheckerCtx )
		return false;

	if( nICheckerPID == PID_ICHECKERSA )
	{
		m_ProcessInfo->m_nProcessStatusMask |= cScanProcessInfo::NTFS_KNOWN;
		m_ProcessInfo->m_hICheckObject->propSet(NULL, m_propIStreamsCtx, pICheckerCtx, AVPMGR_ICDATA_SIZE);
	}

	bool bSkip = false;

	if( bNeedToCheckUnchanged )
	{
		if( !m_fUnchangedObject )
		{
			tERROR err = GetEngine()->ICCheckData(pICheckerCtx, m_nClassMaskEx|m_nClassMask, 0);
			if( err == warnFALSE )
			{
				cDateTime dtCreation;
				if( PR_SUCC(pObject->propGet(NULL, pgOBJECT_CREATION_TIME, dtCreation, sizeof(tDATETIME))) )
				{
					if( (time_t)cDateTime::now_utc() - (time_t)dtCreation < 3*24*60*60 )
						err = errNOT_OK;
				}
			}

			m_fUnchangedObject = PR_SUCC(err) ? 1 : 0;
		}

		if( m_fUnchangedObject )
		{
			m_ProcessInfo->m_nProcessStatusMask |= cScanProcessInfo::UNCHANGED;

			if( !bScanUnchanged )
				bSkip = true;
			else if( m_nClassMaskForUnchanged )
				SetActionClassMask(m_nClassMaskForUnchanged);
		}
	}

	if( !bSkip && bUseICheck )
	{
		bSkip = PR_SUCC(GetEngine()->ICCheckData(pICheckerCtx,
			m_nClassMaskEx|m_nClassMask, m_nUseICheckDays));

		if( nICheckerPID == PID_ICHECKERSA )
			PR_TRACE((m_Engine, prtNOTIFY, "Engine_ICCheckData say %s", bSkip ? "Skip" : "Not Skip"));
	}

	if( bSkip )
		GetEngine()->ICFree(pICheckerCtx);

	return bSkip;
}

bool CScanContext::CheckExcludeByStreams()
{
	if( !CheckByIChecker(m_nIStreamsCtx, m_ProcessInfo->m_hICheckObject, PID_ICHECKERSA) )
		return false;

	IncrementStatistic(STATISTIC_OFFSET(m_nNumScaned));
	SendReport(pmc_PROTECTION_EVENTS_NOTIFY, OBJSTATUS_OK, OKREASON_ISTREAMS);
	TraceOut(".ntfs");
	return true;
}

bool CScanContext::CheckTimeLimit()
{
	tDWORD nCurrentTime = PrGetTickCount();

	if( m_bUseABS )
		m_Session->PerformABS(nCurrentTime, m_Settings->m_nABSNumber);

	if( !m_Session->m_pAVS->ProcessYield(*this, nCurrentTime) )
		return false;

	if( m_nNumDetected )
		return true;

	tBOOL bCheckLimit = m_Settings->m_nTimeLimit.m_on && m_Settings->m_nTimeLimit.m_val;
	tBOOL bCheckCancel = !!m_Settings->m_nTimeoutForCancelRequest;

	tDWORD nWorkTime = 0;
	if( bCheckLimit || bCheckCancel )
		nWorkTime = (nCurrentTime - m_tmStart) / 1000;

	if( !nWorkTime )
		return true;

	if( bCheckCancel && !m_nProcessId && nWorkTime >= m_Settings->m_nTimeoutForCancelRequest )
		m_Session->m_pAVS->AddCancelProcessRequest(*this);

	if( bCheckLimit && nWorkTime > (tDWORD)m_Settings->m_nTimeLimit.m_val )
		return false;

	return true;
}

tERROR CScanContext::CheckState(bool bPause)
{
	if( m_fCancelRequest )
		ProcessStop(true);

	if( m_fStopProcessing )
		return errOPERATION_CANCELED;

	if( m_pScanTop && m_pScanTop->m_fStopProcessing )
		return errENGINE_ACTION_SKIP;

	if( bPause)
		while( m_Session->m_nState == TASK_STATE_PAUSED ) PrSleep(50);

	if( m_Session->m_nState == TASK_STATE_STOPPED )
	{
		ProcessStop(true);
		return errOPERATION_CANCELED;
	}

	return m_Engine ? errOK : errUNEXPECTED;
}

void CScanContext::ProcessStop(bool bStopAll)
{
	CObjectContext* pCtx = (bStopAll || !m_pScanTop) ? this : m_pScanTop;

	if( !pCtx->m_fStopProcessing && bStopAll )
	{
		CObjectContext* pReportCtx = m_pScanTop ? m_pScanTop : this;
		pReportCtx->SendReport(pmc_PROTECTION_EVENTS_NOTIFY, OBJSTATUS_NOTPROCESSED, NPREASON_SKIPPED);
		pReportCtx->IncrementStatistic(STATISTIC_OFFSET(m_nNumNotProcessed));
	}

	pCtx->m_fStopProcessing = 1;
	pCtx->m_nStatus |= cScanProcessInfo::CANCELED;
	pCtx->TraceOut(".stop processing");
}

tERROR CScanContext::Process()
{
	if( !m_Engine )
		return errUNEXPECTED;

	if( PR_FAIL(OnEngineProcessing()) )
		return errOPERATION_CANCELED;

	m_pObject->sysRegisterMsgHandlerList(m_Session->m_hndList, countof(m_Session->m_hndList));
	m_pObject->propSetPtr(m_propContext, this);

	if( m_fOSObject && !m_ProcessInfo->m_hSessionObject )
		m_ProcessInfo->m_hSessionObject = m_pObject;

	if( m_ProcessInfo->m_hSessionObject )
		m_pObject->propSetObj(m_propSessionObject, m_ProcessInfo->m_hSessionObject);

	cAVSPM * avspm = m_Session->m_pAVS->m_avspm;
	if( avspm )
		avspm->ProcessBegin(m_Engine);

	tERROR err = errOK;
	if( !m_fOSObject )
	{
		err = GetEngine()->ProcessObject(*this);
		if( m_fTreatsProcessing )
			UpdateDetectedInfo();
	}
	else
	{
		if( !m_ProcessInfo->m_hSessionObject )
			m_ProcessInfo->m_hSessionObject = m_pObject;

		err = ProcessOS();
	}

	if( avspm )
		avspm->ProcessEnd();

	if( !(m_nStatus & cScanProcessInfo::DELETED) )
		m_pObject->sysUnregisterMsgHandlerList(m_Session->m_hndList, countof(m_Session->m_hndList));
	else if( m_fProcessing )
	{
		CleanUp();
		m_pObject->sysCloseObject();
	}

	if( err == errOPERATION_CANCELED )
		err = errOK;

	UpdateProcessInfo();
	return err;
}

enActions CScanContext::AskAction(tActionId nAction, cAskObjectAction &pRequest, tDWORD nAccessMode, bool bUseCache)
{
	tDWORD nOldMode;
	tBOOL  bRestoreMode = cFALSE;
	if( nAccessMode && m_pNative )
	{
		nOldMode = m_pNative->m_pObject->propGetDWord(pgOBJECT_ACCESS_MODE);
		if( !(nOldMode & nAccessMode) )
		{
			m_pNative->m_pObject->propSetDWord(pgOBJECT_ACCESS_MODE, nAccessMode);
			bRestoreMode = cTRUE;
		}
	}

	enActions nResult = m_Session->AskObjectAction(nAction, pRequest, bUseCache);

//	if( m_fTreatsProcessing && nResult == ACTION_CANCEL && pRequest.m_nApplyToAll )
//		ProcessStop(true);

	if( !bRestoreMode )
		return nResult;

	tERROR error = m_pNative->m_pObject->propSetDWord(pgOBJECT_ACCESS_MODE, nOldMode);
	if( PR_FAIL(error) )
	{
		m_nReason = ConvertProcessingError(error);
		if( m_nReason == NCREASON_NONE )
			m_nReason = NCREASON_OBJECTNOTFOUND;
	}

	return nResult;
}	

enActions CScanContext::AskPassword(cAskObjectPassword &pRequest)
{
	if( m_fTreatsProcessing )
		if( m_Session->m_pAVS->GetPassword(pRequest.m_qwUniqueId, pRequest.m_strPassword) )
			return ACTION_OK;

	enActions nResult = m_Session->AskObjectAction(cAskObjectPassword::eIID, pRequest, true);

	if( nResult == ACTION_OK )
		m_Session->m_pAVS->RegisterPassword(pRequest.m_qwUniqueId, pRequest.m_strPassword);

	return nResult;
}

bool CScanContext::CheckActiveDetection(CObjectContext& pCtx)
{
	if( !m_Session->m_pAVS->m_settings.m_bUseActiveDisinfection )
		return false;

	if( m_Settings->m_bActiveDisinfection || m_Settings->m_nScanAction == SCAN_ACTION_DETECT )
		return false;

	cStrObj sObjName;
	if( !pCtx.CheckMemoryModule(false, &sObjName) )
	{
		if( !m_pNative || m_pNative->m_fArchive )
			return false;

		if( !m_Session->m_pAVS->CheckActiveDetection(m_pNative->m_strObjectName) )
		{
			cStrObj strLongPath;
			if( PR_FAIL(strLongPath.assign(m_pNative->m_pObject, plOBJECT_NATIVE_LONG_FULLNAME)) )
				return false;

			if( !strLongPath.compare(0, 4, "\\\\?\\", 0) )
				strLongPath.erase(0, 4);

			if( !m_Session->m_pAVS->CheckActiveDetection(strLongPath) )
				return false;
		}
		sObjName = m_pNative->m_strObjectName;
	}

	cAskObjectAction askAction;
	pCtx.InitAskAction(askAction, ACTION_OK|ACTION_CANCEL);
	askAction.m_strObjectName = sObjName;

	enActions nAction = ACTION_CANCEL;
	if( m_Settings->m_nScanAction == SCAN_ACTION_DISINFECT && m_Settings->m_bTryDisinfect )
		nAction = ACTION_OK;
	else if( m_Settings->m_nScanAction != SCAN_ACTION_REPORT )
		nAction = AskAction(cAskObjectAction::ACTIVE_DETECT, askAction);

	if( nAction != ACTION_OK )
		return false;

	askAction.m_nDescription = m_Settings->m_nScanAction;
	AskAction(cAskObjectAction::ACTIVE_DISINFECT, askAction);
	return true;
}

bool CScanContext::CheckExcludeByVerdict(CObjectContext& pCtx)
{
	cInfectedObjectInfo* pInfo = pCtx.m_InfCtx;
	if( !pInfo )
		return false;

	cStrObj strTemp(pInfo->m_strObjectName);

	bool bSkip = false;
	for(CObjectContext *ctx = &pCtx; ctx && !bSkip; ctx = ctx->m_pPrev)
	{
		if( !ctx->m_fProcessing )
			continue;

		if( ctx != &pCtx )
			pInfo->m_strObjectName = ctx->m_strObjectName;

		if( bSkip = PR_SUCC(m_Session->m_pAVS->MatchExcludesEx(pInfo)) )
			break;

		if( ctx == m_pNative )
		{
			if( PR_SUCC(pInfo->m_strObjectName.assign(ctx->m_pObject, plOBJECT_NATIVE_SHORT_FULLNAME)) )
			{
				if( bSkip = PR_SUCC(m_Session->m_pAVS->MatchExcludesEx(pInfo, true)) )
					break;
			}

			if( PR_SUCC(pInfo->m_strObjectName.assign(ctx->m_pObject, plOBJECT_NATIVE_LONG_FULLNAME)) )
			{
				if( !pInfo->m_strObjectName.compare(0, 4, "\\\\?\\", 0) )
					pInfo->m_strObjectName.erase(0, 4);

				if( bSkip = PR_SUCC(m_Session->m_pAVS->MatchExcludesEx(pInfo)) )
					break;
			}
		}
	}

	pInfo->m_strObjectName = strTemp;
	return bSkip;
}

bool CScanContext::RegisterPassword(cStringObj& strPswd)
{
	tDWORD nCount = m_vPasswords.size();
	for(tDWORD i = 0; i < nCount; i++)
		if( m_vPasswords[i] == strPswd )
			return true;

	m_vPasswords.push_back() = strPswd;
	return true;
}

bool CScanContext::GetPassword(cStringObj& strPswd, tDWORD* pCookie)
{
	tDWORD nCount = m_vPasswords.size();
	if( *pCookie >= nCount )
		return false;

	strPswd = m_vPasswords[nCount - *pCookie - 1];
	(*pCookie)++;
	return true;
}

tDWORD CScanContext::GetTimeDelta()
{
#ifdef _WIN32
/*
	tQWORD qwTime0 = GetTickCount();
	if( !m_dtTimeStamp )
	{
		m_dtTimeStamp = qwTime0;
		return 0;
	}

	tDWORD ret = (tDWORD)((qwTime0 - m_dtTimeStamp)*10000);
	m_dtTimeStamp = qwTime0;
	return ret;
*/

	tQWORD qwTime0 = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&qwTime0);
	if( !m_dtTimeStamp )
	{
		m_dtTimeStamp = qwTime0;
		return 0;
	}

	static tQWORD qwFreg = 0;
	if( !qwFreg )
		QueryPerformanceFrequency((LARGE_INTEGER*)&qwFreg);
	if( !qwFreg )
		return 0;

	tDWORD ret = (tDWORD)((qwTime0 - m_dtTimeStamp)*10000000/qwFreg);
	m_dtTimeStamp = qwTime0;
	return ret;

#else
	return 0;
#endif
}

// -------------------------------------------

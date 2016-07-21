// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- avs.cpp
// -------------------------------------------

#define IMPEX_IMPORT_LIB

#include "avs.h"
#include "scan.h"

#include <Prague/prague.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/i_hash.h>

#include <Extract/iface/i_mailmsg.h>

#include <MemModScan/io.h>
#include <Hash/MD5/plugin_hash_md5.h>

#include <string.h>
#include <stdio.h>

// -------------------------------------------

CObjectContext::CObjectContext(hOBJECT pObject)
{
	int nOffset = offsetof(CObjectContext, m_pScanCtx);
	memset((tBYTE*)this + nOffset, 0, sizeof(CObjectContext)-nOffset);
	m_pObject = pObject;
}

CObjectContext::~CObjectContext()
{
	m_pPrev = NULL;
	InitContext();
}

tERROR CObjectContext::OnMsgRecieve(tDWORD msg_cls, tDWORD msg_id)
{
	tERROR error = errOK;
	switch( msg_cls )
	{
		case pmc_PROCESSING:	error = OnEngineProcessing(msg_id); break;
		case pmc_ENGINE_ACTION:	error = OnEngineAction(msg_id); break;
		case pmc_ENGINE_IO:		error = OnEngineIO(msg_id); break;
		case pmc_IO:			error = OnProcessIO(msg_id); break;
		case pmc_STARTUPENUM2:  error = OnStartupMessage(msg_id); break;
		case pmc_OS_VOLUME:     error = OnOsVolume(msg_id); break;
	}
	return error;
}

tERROR CObjectContext::OnEngineProcessing(tDWORD msg_id)
{
	if( msg_id != pm_PROCESSING_YIELD )
		return errOK;

	tERROR nError = m_pScanCtx->CheckState(true);
	if( PR_FAIL(nError) )
		return nError;

	if( m_pScanCtx->CheckTimeLimit() )
		return errOK;

	SendReport(pmc_PROTECTION_EVENTS_NOTIFY, OBJSTATUS_NOTPROCESSED, NPREASON_TIME);
	IncrementStatistic(STATISTIC_OFFSET(m_nNumNotProcessed));
	m_pScanCtx->ProcessStop();
	return errOPERATION_CANCELED;
}

tERROR CObjectContext::OnEngineAction(tDWORD msg_id)
{
	tERROR nError = m_pScanCtx->CheckState();
	if( PR_FAIL(nError) )
		return nError;

	tDWORD dwClass = 0;
	if( m_pBuff && m_pBuffLen && *m_pBuffLen == sizeof(tDWORD) )
		dwClass = *(tDWORD*)m_pBuff;
	
	if( msg_id != ENGINE_ACTION_TYPE_NO_ACTION && dwClass == ENGINE_ACTION_CLASS_VIRUS_SCAN )
	{
		tERROR nResult = errOK;

		if( nResult == errOPERATION_CANCELED )
			return nResult;

		if( nResult == errOK_DECIDED || !IsShouldBeScan() )
		{
			if( !m_fSkippedByType )
				m_pObject->propSetBool(m_propSkipThisOne, cTRUE);
			return errENGINE_ACTION_SKIP;
		}
	}
		
	nError = errOK_DECIDED;
	switch( msg_id )
	{
		case ENGINE_ACTION_TYPE_SECTOR_OVERWRITE :
			nError = ProcessSectorOverwriteRequest();
			break;

		case ENGINE_ACTION_TYPE_DISINFECT :
			nError = ProcessDisinfectRequest();
			break;

	}
	return nError;
}

tERROR CObjectContext::OnEngineIO(tDWORD msg_id)
{
	tERROR nError = m_pScanCtx->CheckState();

	if( msg_id != pm_ENGINE_IO_DONE && PR_FAIL(nError) )
		return nError;

	switch( msg_id )
	{
	case pm_ENGINE_IO_BEGIN:
		if( PID_AVPMGR == m_pContext->propGetDWord(pgPLUGIN_ID) )
			ProcessScanBegin();
		break;
	case pm_ENGINE_IO_DONE:
		if( PID_AVPMGR == m_pContext->propGetDWord(pgPLUGIN_ID) )
			ProcessScanDone();
		break;
	case pm_ENGINE_IO_NAME_CHECK:	break;
	case pm_ENGINE_IO_OS_RECOGNIZE: 
	case pm_ENGINE_IO_OS_CHECK:		nError = IsOSShouldBeScan(msg_id==pm_ENGINE_IO_OS_CHECK); break;
	case pm_ENGINE_IO_OS_ENTER:		nError = ProcessOSEnter(NULL); break;
	case pm_ENGINE_IO_OS_LEAVE:		break;
	case pm_ENGINE_IO_OS_PRECLOSE:  nError = ProcessOSPreClose(NULL); break;
	case pm_ENGINE_IO_SUB_IO_ENTER:	nError = ProcessSubIOEnter(); break;
	case pm_ENGINE_IO_SUB_IO_LEAVE: break;
	case pm_ENGINE_IO_ENCRYPTED:	nError = ProcessEncrypted(); break;
	case pm_ENGINE_IO_PASSWORD_PROTECTED: nError = ProcessPasswordProtected(); break;
	case pm_ENGINE_IO_CORRUPTED:	nError = ProcessCorrupted(); break;
	case pm_ENGINE_IO_DETECT:		nError = ProcessVirusDetect(); break;
	case pm_ENGINE_IO_DISINFECTION_IMPOSSIBLE:
	case pm_ENGINE_IO_DISINFECTION_FAILED: nError = ProcessDisinfectImpossible(); break;
	case pm_ENGINE_IO_DISINFECTED:	nError = ProcessDisinfected(); break;
	case pm_ENGINE_IO_PROCESSING_ERROR: nError = ProcessProcessingError(); break;
	case pm_ENGINE_IO_GET_ORIGINAL_BOOT: nError = errNOT_SUPPORTED; break;
	case pm_ENGINE_IO_GET_ORIGINAL_MBR:  nError = errNOT_SUPPORTED; break;
	case pm_ENGINE_IO_HC_CLEAN : break;
	case pm_ENGINE_IO_HC_DETECT : nError = ProcessVirusDetect(true); break;
	case pm_ENGINE_IO_HC_OVERWRITED : nError = ProcessDisinfected(); break;
	case pm_ENGINE_IO_HC_OVERWRITE_FAILED : nError = ProcessDisinfectImpossible(); break;
	case pm_ENGINE_IO_HC_MARKED_FOR_DELETE : break;
	case pm_ENGINE_IO_FALSE_ALARM : m_fFalseAlarm = 1; break;
	}			

	return nError;
}

tERROR CObjectContext::OnProcessIO(tDWORD msg_id)
{
	tERROR nError = errOK;
	switch( msg_id )
	{
		case pm_IO_DELETE_ON_CLOSE_FAILED:  nError = ProcessDeleteOnCloseFailed(); break;
		case pm_IO_DELETE_ON_CLOSE_SUCCEED: nError = ProcessDeleteOnClose(); break;
		case pm_IO_SET_ATTRIBUTES_FAILED:	nError = ProcessSetAttributesFailed(); break;
		case pm_IO_CREATE_FAILED:			nError = ProcessCreateFailed(); break;
		case pm_IO_PASSWORD_REQUEST:		nError = ProcessPasswordRequest(); break;
	}
	return nError;
}

tERROR CObjectContext::OnStartupMessage(tDWORD msg_id)
{
	if( msg_id != mc_FILE_FOUNDED )
		return errOK;

	pACTION_MESSAGE_DATA pData = (pACTION_MESSAGE_DATA)m_pBuff;

	cDetectObjectInfo pInfo;
	pInfo = *m_InfCtx;

	switch( pData->m_dAction )
	{
	case FileToDel:
		{
			cIOObj pFileIO(m_pObject, cAutoString (cStringObj(pData->m_sRoot)), fACCESS_RW);
			if( PR_FAIL(pFileIO.last_error()) )
				return pFileIO.last_error();

			pFileIO->propSetBool(pgOBJECT_DELETE_ON_CLOSE, cTRUE);
		}
		return errOK;
	case DelKey:
	case DelThisKey: pInfo.m_nObjectStatus = OBJSTATUS_DELETED; break;
	case DelData:
	case CutData:
	case RestDef:    pInfo.m_nObjectStatus = OBJSTATUS_CURED; break;
	default: return errOK;
	}

	pInfo.m_tmTimeStamp = cDateTime::now_utc();
	pInfo.m_strObjectName = pData->m_sRoot;
	pInfo.m_nObjectType = OBJTYPE_STARTUPOBJECT;
	if( pData->m_sValue || pData->m_sSection )
	{
		pInfo.m_strObjectName.check_last_slash();
		if( pData->m_ObjType == OBJECT_INI_TYPE )
		{
			pInfo.m_strObjectName += pData->m_sSection ? pData->m_sSection : L"*";
			pInfo.m_strObjectName.check_last_slash();
			pInfo.m_strObjectName += pData->m_sValue ? pData->m_sValue : L"*";
		}
		else
			pInfo.m_strObjectName += pData->m_sValue ? pData->m_sValue : pData->m_sSection;
	}

	return m_pScanCtx->m_Session->SendReport(pmc_PROTECTION_EVENTS_FIX, &pInfo, NULL);
}

tERROR CObjectContext::OnOsVolume(tDWORD msg_id)
{
	if( msg_id == pm_OS_VOLUME_HASH )
	{
		if( !m_pBuff )
			return errPARAMETER_INVALID;

		tQWORD hash = *(tQWORD*)m_pBuff;
		tDWORD pos = m_pPrev ? m_pPrev->m_nHashPos : 0;
		if( !pos )
			return errOK;

		CInfectedHashData& pData = m_pScanCtx->m_vHashData[pos-1];
		if( hash != pData.m_qwHash && hash != (pData.m_qwHash^1) )
			pData.m_vVolumes.push_back(hash);
	}

	return errOK;
}

CAVSEngine * CObjectContext::GetEngine()
{
	return m_pCurEngine ? m_pCurEngine : m_pScanCtx->m_Engine;
}

CObjectContext * CObjectContext::GetNextContext(hOBJECT pObject)
{
	CObjectContext * pObjCtx = m_pNext;
	if( !pObjCtx )
	{
		pObjCtx = m_pNext = new CObjectContext(pObject);
		pObjCtx->m_pScanCtx = m_pScanCtx;
		pObjCtx->m_pPrev = this;
	}
	else
		pObjCtx->m_pObject = pObject;

	pObjCtx->InitContext();
	pObject->propSetPtr(m_propContext, pObjCtx);
	return pObjCtx;
}

inline void CObjectContext::InitContext()
{
	m_InfCtx.clear();

	if( m_pvOffsets )
		delete m_pvOffsets, m_pvOffsets=NULL;

	cProtectionStatistics::init();
	int nOffset = offsetof(CObjectContext, m_nStatus);
	memset((tBYTE*)this + nOffset, 0, sizeof(CObjectContext)-nOffset);

	if( m_pPrev )
	{
		if( m_pPrev->m_nObjectType && !m_pPrev->m_fTreatsProcessing )
			m_nObjectType = m_pPrev->m_nObjectType;
		else
			m_nObjectType = OBJTYPE_GENERICIO;
		m_nObjectScope = m_pPrev->m_nObjectScope;
	}

	m_nDetectDanger = DETDANGER_UNKNOWN;
	m_nDetectType = DETYPE_UNKNOWN;
}

void CObjectContext::InitObjectInfo()
{
	m_nSessionId = m_pScanCtx->m_ProcessInfo->m_nActionSessionId;
	m_nPID = m_pScanCtx->m_ProcessInfo->m_nActionPID;
	m_strTaskType = m_pScanCtx->m_Session->m_strTaskType;
	m_nTaskId = m_pScanCtx->m_Session->m_nTaskId;

	cUserInfo* pUserCtx = m_pScanCtx->m_ProcessInfo->m_pUserCtx;
	if( pUserCtx )
	{
		m_strUserName = pUserCtx->m_sUserName;
		m_strMachineName = pUserCtx->m_sMachineName;
	}

	if( !m_pPrev )
		m_pScanCtx->GetTimeDelta();

	if( m_fPreProcessing )
	{
		m_strObjectName = (hSTRING)m_pObject;
		return;
	}

	if( !m_pPrev && m_pScanCtx->m_fTreatsProcessing )
		return;

	if( !m_pObject )
		return;

	m_nObjectOrigin = m_pScanCtx->m_Session->m_nOrigin;

// get object type
	tORIG_ID idOrigin = OID_ANY;
	m_pObject->propGet(0, pgOBJECT_ORIGIN, &idOrigin, sizeof(idOrigin));

	if( idOrigin == OID_ANY )
	{
		hOBJECT hObject = m_pObject->propGetObj(pgOBJECT_BASED_ON);
		if( hObject )
			hObject->propGet(0, pgOBJECT_ORIGIN, &idOrigin, sizeof(idOrigin));
	}
	else if( idOrigin == OID_AVP3_DOS_MEMORY )
	{
		char dosMemory [] = "DOS Memory";
		m_pObject->propSetStr(0, m_propVirtualName, dosMemory, 0, cCP_ANSI);
	}

	enObjectType nObjectType = idOrigin != OID_ANY ? (enObjectType)idOrigin : OBJTYPE_GENERICIO;
	if( nObjectType != OBJTYPE_GENERICIO || !m_nObjectType )
		m_nObjectType = nObjectType;

	if( m_nObjectType == OBJTYPE_QUARANTINED )
		m_pScanCtx->m_fQuarantineProcessing = 1;

// get object name
	tERROR error = m_strObjectName.assign(m_pObject, m_propVirtualName);
	if( PR_FAIL(error) )
		error = m_strObjectName.assign(m_pObject, pgOBJECT_FULL_NAME);

	if( m_pObject->propSize(plNATIVE_HANDLE) )
		m_fNative = 1;
}

bool CObjectContext::InitCureOnReboot()
{
	if( m_pObject->propGetBool(plIS_REMOTE) ||
			!m_pScanCtx->m_Settings->m_bTryCureOnReboot )
		return false;

	cInfectedObjectInfo* pInfInfo = m_InfCtx;
	pInfInfo->m_strArchiveObject.assign(m_pObject, plNATIVE_PATH);
	return true;
}

bool CObjectContext::InitExecutableParent(bool bExecutable)
{
	if( !m_fPragueObj && m_pPrev && !m_pPrev->m_fUnpacked && !m_pPrev->m_fEmbedded )
		return false;

	if( m_pObject->propGetObj(m_propExecutableParent) )
		return false;

	if( !m_pPrev || m_fPragueObj )
	{
		if( bExecutable )
			m_pObject->propSetObj(m_propExecutableParent, m_pObject);
	}
	else
		m_pObject->propSetObj(m_propExecutableParent, m_pPrev->m_pObject);

	return true;
}

inline enActions GetDefaultAction(int nActionMask)
{
	for(int i = 0; i < 32; i++)
		if( nActionMask & (1 << i) )
		   return (enActions)(1 << i);
	return ACTION_REPORTONLY;
}

inline void CObjectContext::InitAskAction(cAskObjectAction& pInfo, int nActionMask)
{
	if( m_InfCtx )
	{
		(cInfectedObjectInfo&)pInfo = *m_InfCtx;
		pInfo.m_nObjectStatus = m_nObjectStatus;
	}
	else
		*(cObjectInfo*)&pInfo = *this;

	pInfo.m_nActionsMask = nActionMask | ACTION_CANCEL;
	pInfo.m_nDefaultAction = GetDefaultAction(nActionMask);
	pInfo.m_tmTimeStamp = cDateTime::now_utc();
	pInfo.m_bSystemCritical = cFALSE;

	if( nActionMask & (ACTION_OK|ACTION_ALLOW) )
	{
		pInfo.m_nActionsAll = nActionMask;
		return;
	}

	pInfo.m_nActionsAll = ACTION_DELETE | ACTION_CANCEL;
	if( nActionMask & ACTION_DELETE_ARCHIVE )
		pInfo.m_nActionsAll |= ACTION_DELETE_ARCHIVE;

	if( pInfo.m_nDetectDanger != DETDANGER_HIGH )
		pInfo.m_nExcludeAction = ACTION_CANCEL;

	if( pInfo.m_nDetectStatus == DSTATUS_SURE )
		pInfo.m_nActionsAll |= ACTION_DISINFECT;
	else
		pInfo.m_nActionsAll |= ACTION_QUARANTINE;
}

bool CObjectContext::InitInfectedInfo()
{
	cAVSSettings& avsSettings = m_pScanCtx->m_Session->m_pAVS->m_settings;

	if( !m_pCurEngine && avsSettings.m_bDebugMode & 0x10 )
		return false;

	if( m_fSkippedByVerdict )
		return false;

	if( !m_InfCtx.init(*this) )
		return false;

	if( m_fExternalDetectProcessing )
	{
		m_InfCtx->m_nDetectStatus = m_DetectInfo->m_nDetectStatus;
		m_InfCtx->m_nDetectType = m_DetectInfo->m_nDetectType;
		m_InfCtx->m_nDetectDanger = m_DetectInfo->m_nDetectDanger;
		m_InfCtx->m_strDetectName = m_DetectInfo->m_strDetectName;
		m_InfCtx->m_nDetectBehaviour = m_DetectInfo->m_nDetectBehaviour;
	}
	else
	{
		m_InfCtx->m_nDetectStatus = GetEngine()->GetDetectStatus(*this);
		m_InfCtx->m_nDetectType = GetEngine()->GetDetectType(*this);
		m_InfCtx->m_nDetectDanger = GetEngine()->GetDetectDanger(*this);
		m_InfCtx->m_nDetectBehaviour = GetEngine()->GetDetectBehaviour(*this);
		GetEngine()->GetDetectName(*this, m_InfCtx->m_strDetectName);

		cAVP3BasesInfo pBasesInfo;
		GetEngine()->GetBasesInfo(pBasesInfo);
		m_InfCtx->m_tmScanningBases = cDateTime(&pBasesInfo.m_dtBasesDate);
	}

	m_InfCtx->m_nObjectStatus = OBJSTATUS_UNKNOWN;
	m_InfCtx->m_strDetectObject = m_strObjectName;

	cUserInfo* pInfo = m_pScanCtx->m_ProcessInfo->m_pOwnerCtx;
	if( pInfo )
	{
		m_InfCtx->m_strUserName = pInfo->m_sUserName;
		m_InfCtx->m_strMachineName = pInfo->m_sMachineName;
	}

	m_nObjectStatus = (m_InfCtx->m_nDetectStatus == DSTATUS_SURE) ? 
		OBJSTATUS_INFECTED : OBJSTATUS_SUSPICION;

	enDetectReason nDetectReason = DTREASON_NONE;
	tDWORD    nMsgClass = pmc_PROTECTION_EVENTS_SPAM;
	enActions nAction = ACTION_UNKNOWN;

	tDWORD nMask, nBehaviour = m_InfCtx->m_nDetectBehaviour;

	if( !nBehaviour )
		nBehaviour = 100;


	nMask = avsSettings.m_nDetectTypeMask[nBehaviour/100-1];

	if( !(m_InfCtx->m_nDetectDanger & avsSettings.m_nDetectMask) ||
		!(nMask & (1 << ((nBehaviour%100)+1))) )
	{
		m_fSkippedByVerdict = 1;
		nDetectReason = DTREASON_INFORMATION;
	}
	else if( !m_pScanCtx->m_fQuarantineProcessing &&
		m_pScanCtx->m_Settings->m_bUseExcludes &&
		m_pScanCtx->CheckExcludeByVerdict(*this) )
	{
		m_fSkippedByExclude = m_fSkippedByVerdict = 1;
		nDetectReason = DTREASON_INFORMATION;
	}
	else if( m_pScanCtx->m_Settings->m_nScanAction == SCAN_ACTION_REPORT )
	{
		m_fSkippedByVerdict = 1;
		nMsgClass = pmc_PROTECTION_EVENTS_IMPORTANT;
		nDetectReason = DTREASON_INFORMATION;
	}
	else if( m_pScanCtx->m_Settings->m_nScanAction == SCAN_ACTION_ASKDETECT )
	{
		cAskObjectAction askAction;
		InitAskAction(askAction, ACTION_ALLOW|ACTION_DENY);
		askAction.m_nDefaultAction = ACTION_DENY;

		nAction = m_pScanCtx->AskAction(cAskObjectAction::DISINFECT, askAction);
		if( nAction == ACTION_ALLOW )
		{
			nMsgClass = pmc_PROTECTION_EVENTS_IMPORTANT;
			m_fSkippedByVerdict = 1;

			InitDetectedObjectId();
			UpdateDetectedInfo();
			m_nStatus |= cScanProcessInfo::SOME_ALLOWED;
		}
	}

	if( m_fSkippedByVerdict )
	{
		IncrementStatistic(STATISTIC_OFFSET(m_nNumNotProcessed));
		SendReport(nMsgClass, m_nObjectStatus, nDetectReason, &m_InfCtx->m_strDetectName);

		if( m_fSkippedByExclude )
		{
			SendReport(pmc_PROTECTION_EVENTS_SPAM, OBJSTATUS_NOTPROCESSED, NPREASON_MASKS);
			TraceOut(".mask");
		}

		if( nAction == ACTION_ALLOW )
		{
			SendReport(pmc_PROTECTION_EVENTS_FIX, OBJSTATUS_ALLOWED);
			TraceOut(".allowed");
		}

		m_InfCtx.clear();
		return false;
	}

	for(CObjectContext *ctx = m_pPrev; ctx; ctx = ctx->m_pPrev)
		if( ctx->m_fProcessing || ctx->m_fOSObject )
		{
			ctx->m_InfCtx.init(*ctx);
			ctx->m_InfCtx->m_nObjectStatus = OBJSTATUS_UNKNOWN;
			ctx->m_InfCtx->m_strDetectObject = m_InfCtx->m_strDetectObject;
			CopyInfectedInfo(ctx->m_InfCtx);
		}
	return true;
}

void CObjectContext::CopyInfectedInfo(cDetectObjectInfo* pData)
{
	pData->m_nDetectStatus = m_InfCtx->m_nDetectStatus;
	pData->m_nDetectType = m_InfCtx->m_nDetectType;
	pData->m_nDetectDanger = m_InfCtx->m_nDetectDanger;
	pData->m_strDetectName = m_InfCtx->m_strDetectName;
	pData->m_strUserName = m_InfCtx->m_strUserName;
	pData->m_strMachineName = m_InfCtx->m_strMachineName;
}

bool CObjectContext::InitSimpleObjectId(tQWORD& qwObjectId)
{
	cAutoObj<cHash> hHash;
	tERROR error = g_root->sysCreateObjectQuick((hOBJECT*)&hHash, IID_HASH, PID_HASH_MD5);
	if( PR_FAIL(error) )
		return false;

	if( m_strObjectName.size() )
	{
		cStrObj strData(m_strObjectName);
		strData.toupper();
		error = hHash->Update((tBYTE*)strData.data(), sizeof(tWCHAR)*strData.size());
	}

	tBYTE pHashData[32];
	if( PR_SUCC(error) )
		error = hHash->GetHash(pHashData, sizeof(pHashData));

	if( PR_SUCC(error) )
	{
		qwObjectId = *(tQWORD*)(&pHashData[0]);
		qwObjectId^= *(tQWORD*)(&pHashData[8]);
	}
	return PR_SUCC(error);
}

bool CObjectContext::InitDetectedObjectId()
{
	if( m_fObjectIdInited )
		return true;

	m_fObjectIdInited = 1;

	cInfectedObjectInfo* pInfInfo = m_InfCtx;
	if( m_pScanCtx->m_Session->m_nOrigin != OBJECT_ORIGIN_GENERIC ||
		m_pScanCtx->m_fStreamProcessing /*||
		m_pScanCtx->m_fExternalDetectProcessing*/ )
	{
		if( !InitSimpleObjectId(pInfInfo->m_qwUniqueId) )
			return false;

		UpdateDetectedInfo();
		return true;
	}

	CObjectContext* pParent = NULL, *pReopenable = NULL, *ctx;

	if( m_pPrev )
		m_pPrev->InitDetectedObjectId();

	// get reopen data for this or some parent
	if( !pInfInfo )
		return false;

	tERROR error = errOK;
	for(ctx = m_pPrev; ctx; ctx = ctx->m_pPrev)
	{
		if( !pReopenable && ctx->m_fReopenanable )
			pReopenable = ctx;

		if( !pParent && ctx->m_fProcessing )
			pParent = ctx;

		if( ctx->m_pCurEngine )
			error = errNOT_MATCHED;
	}

	if( pParent )
		pInfInfo->m_qwParentId = pParent->m_InfCtx->m_qwUniqueId;

	if( PR_SUCC(error) )
		error = m_pScanCtx->m_Engine->GetReopenData(*this, pInfInfo->m_pReopenData);

	if( PR_FAIL(error) )
	{
		if( !pReopenable )
		{
			PR_TRACE((m_pObject, prtFATAL, "AVS\tCannot get reopen data (%S)", m_strObjectName.data()));
			return false;
		}

		pInfInfo->m_pReopenData = pReopenable->m_InfCtx->m_pReopenData;
		error = errOK;
	}

	m_fReopenanable = 1;

	if( PR_SUCC(error) )
		CalcObjectHash(pInfInfo, pReopenable);

	UpdateDetectedInfo();
	return PR_SUCC(error);
}

bool CObjectContext::CalcObjectHash(cInfectedObjectInfo* pInfo, CObjectContext* pReopenable)
{
	cAutoObj<cHash> hHash;
	tBYTE pHashData[32];
	tERROR error = m_pObject->sysCreateObjectQuick((hOBJECT*)&hHash, IID_HASH, PID_HASH_MD5);

	if( PR_SUCC(error) )
		error = hHash->Update((tBYTE *) pInfo->m_pReopenData.data (), pInfo->m_pReopenData.size());

CalcUnique:
	if( PR_SUCC(error) )
		error = hHash->GetHash(pHashData, sizeof(pHashData));

	if( PR_SUCC(error) )
	{
		pInfo->m_qwUniqueId = *(tQWORD*)(&pHashData[0]);
		pInfo->m_qwUniqueId^= *(tQWORD*)(&pHashData[8]);
	}

	if( PR_SUCC(error) && m_fReopenanable && pReopenable && pReopenable->m_InfCtx->m_qwUniqueId == pInfo->m_qwUniqueId )
	{
		cStringObj strTail;
		strTail.assign(pInfo->m_strObjectName, pReopenable->m_strObjectName.size());
		if( strTail.size() )
			error = hHash->Update((tBYTE*)strTail.data(), strTail.memsize(cCP_UNICODE));
		else
		{
			tPID nObjectPid = m_pObject->propGetDWord(pgPLUGIN_ID);
			error = hHash->Update((tBYTE*)&nObjectPid, sizeof(tPID));
		}

		if( PR_SUCC(error) )
			error = hHash->Update((tBYTE*)&pInfo->m_qwParentId, sizeof(tQWORD));

		m_fReopenanable = 0;
		goto CalcUnique;
	}

	if( pInfo->m_qwParentId == pInfo->m_qwUniqueId )
		PR_TRACE((m_pObject, prtFATAL, "AVS\tParentId is equal UniqueId (%S)", m_strObjectName.data()));
	else
	{
		PR_TRACE((m_pObject, prtFATAL, "AVS\tReopen data for (%S) ReopenSize(%d) UniqueId(" PRINTF_LONGLONG ")",
			m_strObjectName.data(), pInfo->m_pReopenData.size(), pInfo->m_qwUniqueId));
	}
	return PR_SUCC(error);
}

bool CObjectContext::ChooseAction(int& nActionMask, enNotCuredReason nReason, int& nAction)
{
	nActionMask |= ACTION_CANCEL;
	nAction = GetDefaultAction(nActionMask);

	if( m_InfCtx )
		m_InfCtx->m_nNonCuredReason = nReason;

	if( m_pScanCtx->m_Settings->m_nScanAction == SCAN_ACTION_POSTPONE )
	{
		nAction = ACTION_CANCEL;
		return true;
	}

	if( !m_nObjectScope && m_pScanCtx->m_Settings->m_bActiveDisinfection )
	{
		nActionMask &= ACTION_DELETE;
		nActionMask |= ACTION_REPORTONLY;
		nAction = GetDefaultAction(nActionMask);
		return true;
	}

	if( m_pScanCtx->m_Settings->m_nScanAction == SCAN_ACTION_ASKUSER )
	{
		nActionMask &= m_pScanCtx->m_Settings->m_nAskActions;
		nAction = GetDefaultAction(nActionMask);
		return false;
	}

	nActionMask |= ACTION_REPORTONLY;

	if( !m_pScanCtx->m_Settings->m_bTryDisinfect )
		nActionMask &= ~(ACTION_DISINFECT|ACTION_QUARANTINE);

	if( !m_pScanCtx->m_Settings->m_bTryDelete )
		nActionMask &= ~ACTION_DELETE;

	nAction = GetDefaultAction(nActionMask);

	if( m_pScanCtx->m_Settings->m_bTryDeleteContainer &&
			(nActionMask & ACTION_DELETE_ARCHIVE) && (nAction == ACTION_REPORTONLY) )
		nAction = ACTION_DELETE_ARCHIVE;
	return true;
}

enNotCuredReason CObjectContext::ConvertProcessingError(tERROR nProcessingError)
{
	enNotCuredReason nReason = NCREASON_NONE;
	switch( nProcessingError )
	{
	case errWRITE_PROTECT:	            nReason = NCREASON_WRITEPROTECT; break;
	case errOBJECT_WRITE:	            nReason = NCREASON_WRITEERROR; break;
	case errOBJECT_READ:	            nReason = NCREASON_READERROR; break;
	case errACCESS_DENIED:	            nReason = NCREASON_NORIGHTS; break;
	case errOUT_OF_SPACE:               nReason = NCREASON_OUTOFSPACE; break;
	case errNOT_READY:                  nReason = NCREASON_DEVICENOTREADY; break;
	case errOBJECT_NOT_FOUND:           nReason = NCREASON_OBJECTNOTFOUND; break;
	case errOBJECT_WRITE_NOT_SUPPORTED: nReason = NCREASON_WRITENOTSUPPORTED; break;
	case errLOCKED:			            if( m_pScanCtx->m_pNative ) { nReason = NCREASON_LOCKED; break; }
	case errOBJECT_READ_ONLY:           nReason = NCREASON_NONCURABLE; break;
	}

	return nReason;
}

enNotCuredReason CObjectContext::GetNotProcessingReason()
{
	if( m_pScanCtx->m_nReason != NCREASON_NONE )
	{
		PR_TRACE((g_root, prtIMPORTANT, "avs\t%S - context reason(%d)", m_strObjectName.data(), m_pScanCtx->m_nReason));
		return m_pScanCtx->m_nReason;
	}

	tERROR nProcessingError = errOK;

	if( m_pBuff && *m_pBuffLen == sizeof(tERROR) )
		nProcessingError = *(tERROR*)m_pBuff;

	if( PR_SUCC(nProcessingError) )
		nProcessingError = m_pObject->propGetDWord(m_propSetWriteAccess);

	if( PR_SUCC(nProcessingError) )
		nProcessingError = m_pObject->propGetDWord(m_propReadonlyError);

	PR_TRACE((g_root, prtIMPORTANT, "avs\t%S - prague error(%terr)", m_strObjectName.data(), nProcessingError));
	return ConvertProcessingError(nProcessingError);
}

bool CObjectContext::CheckExcludeByMasks(bool bCheckAbsolute)
{
	if( !m_fProcessing )
		ProcessScanBegin();

	bool bSkip = false;
	if( m_pScanCtx->m_Settings->m_bUseExcludes &&
		PR_SUCC(m_pScanCtx->m_Session->m_pAVS->MatchExcludes((cDetectObjectInfo*)this)) )
	{
		m_fSkippedByExclude = 1;
		bSkip = true;
	}
	else if( m_pScanCtx->m_Settings->m_bExcludeByMask || m_pScanCtx->m_Settings->m_bIncludeByMask )
	{
		cProtectionSettings& set = *m_pScanCtx->m_Settings;
		CMaskChecker pChecker(m_strObjectName);

		int i, nCount;
		if( set.m_bExcludeByMask )
		{
			nCount = set.m_aExcludeList.size();
			for(i = 0; i < nCount && !bSkip; i++)
			{
				cObjectPathMask& pMask = set.m_aExcludeList[i];
				if( pChecker.MatchPath(pMask.m_strMask, !!pMask.m_bRecurse, bCheckAbsolute) )
					bSkip = true;
			}
		}

		if( !bSkip && set.m_bIncludeByMask )
		{
			nCount = set.m_aIncludeList.size();

			bool bFound = !nCount, bChecked = !nCount;
			for(i = 0; i < nCount && !bSkip; i++)
			{
				cObjectPathMask& pMask = set.m_aIncludeList[i];
				if( !bCheckAbsolute && pChecker.IsAbsolute(pMask.m_strMask) )
					continue;

				bChecked = true;
				if( pChecker.MatchPath(pMask.m_strMask, !!pMask.m_bRecurse, bCheckAbsolute) )
				{
					bFound = true;
					break;
				}
			}

			if( !bFound && bChecked )
				bSkip = true;
		}
	}

	if( bSkip )
	{
		SendReport(pmc_PROTECTION_EVENTS_SPAM, OBJSTATUS_NOTPROCESSED, NPREASON_MASKS);
		TraceOut(".mask");
		IncrementStatistic(STATISTIC_OFFSET(m_nNumNotProcessed));
	}

	return bSkip;
}

bool CObjectContext::CheckExcludeByType()
{
	hOBJECT obj = m_pObject->propGetObj(m_propExecutableParent);
	if( obj && obj != m_pObject )
		return false;

	if( GetEngine()->IsShouldBeScan(*this) )
		return false;

	SendReport(pmc_PROTECTION_EVENTS_SPAM, OBJSTATUS_NOTPROCESSED, NPREASON_TYPE);
	TraceOut(".type");
//	IncrementStatistic(STATISTIC_OFFSET(m_nNumNotProcessed));
	m_fSkippedByType = 1;
	return true;
}

bool CObjectContext::CheckExcludeBySFDB()
{
	if( !m_pScanCtx->CheckByIChecker(m_nICheckerCtx, (cObj*)GetCached(), PID_ICHECKER2) )
		return false;

	if( this == m_pScanCtx )
		m_pScanCtx->ICUpdate(PID_ICHECKERSA, cFALSE);

	SendReport(pmc_PROTECTION_EVENTS_NOTIFY, OBJSTATUS_OK, OKREASON_ISFDB);
	TraceOut(".sfdb");
	IncrementStatistic(STATISTIC_OFFSET(m_nNumScaned));
	m_fSkippedBySFDB = 1;
	return true;
}

bool CObjectContext::CheckDetectByMask()
{
	if( m_pScanCtx->m_Settings->m_nScanAction == SCAN_ACTION_DETECT )
		return false;

	enActions& nAction = m_pScanCtx->m_Settings->m_nDetectByMaskAction;
	if( !nAction )
		return false;

	tDetectMasksList& aMasks = m_pScanCtx->m_Settings->m_aDetectMasksList;
	tDWORD nCount = aMasks.size();
	if( !nCount )
		return false;

	ProcessScanBegin();
	CMaskChecker pChecker(m_strObjectName);
	for(tDWORD i = 0; i < nCount; i++)
	{
		cEnabledStrItem& pMask = aMasks[i];
		if( !pMask.m_bEnabled || !pChecker.MatchPath(pMask.m_sName, false, false) )
			continue;

		switch( nAction )
		{
		case ACTION_DELETE: MarkDeleteOnClose(); break;
		case ACTION_RENAME: RenameObject(); break;
		}
		return true;
	}
	
	return false;
}

bool CObjectContext::CheckMemoryModule(bool bCleanUp, cStrObj* strName)
{
	cObjPtr* hPtr = (cObjPtr*)m_pObject->sysGetParent(IID_OBJPTR);
	if( !hPtr )
		return false;

	if( PR_FAIL(m_pObject->sysCheckObject((cObj*)hPtr, IID_OBJPTR, PID_MEMMODSCAN)) )
		return false;

	if( strName )
		strName->assign(hPtr, pgOBJECT_PATH);

	if( !bCleanUp || m_nObjectType == OBJTYPE_MEMORYPROCESSMODULE )
		return true;

/*	cAutoObj<cIO> hModule;
	if( PR_FAIL(hPtr->IOCreate(&hModule, 0, fACCESS_RW, fOMODE_OPEN_IF_EXIST )) )
		return false;
*/
	return true;
}

bool CObjectContext::IsShouldBeScan(bool bPreProcess)
{
	if( m_fOSObject || m_pScanCtx->m_fQuarantineProcessing ||
		m_pScanCtx->m_fTreatsProcessing || m_DetectInfo )
		return true;

	if( bPreProcess && m_pScanCtx->CheckExcludeByStreams() )
		return false;

	if( !m_fMasksChecked && !m_fPreProcessed )
	{
		m_fMasksChecked = 1;
		if( CheckExcludeByMasks(bPreProcess) )
			return false;
	}

	if( !bPreProcess && CheckExcludeByType() )
		return false;

	if( !m_fSFDBChecked )
	{
		m_fSFDBChecked = 1;
		if( CheckExcludeBySFDB() )
			return false;
	}

	return true;
}

tERROR CObjectContext::IsOSShouldBeScan(bool bCheckOS)
{
	if( bCheckOS )
	{
		if( m_pScanCtx->CheckByIChecker(m_nOSICheckerCtx, m_pContext, PID_ICHECKER2) )
		{
			SendReport(pmc_PROTECTION_EVENTS_NOTIFY, OBJSTATUS_OK, OKREASON_ISFDB);
			TraceOut(".sfdb os");
			m_fSkippedBySFDB = 1;
			return errENGINE_ACTION_SKIP;
		}
		return errOK_DECIDED;
	}

	if( m_pBuff && *(tDWORD*)m_pBuff && m_pScanCtx->m_Settings->m_nScanAction == SCAN_ACTION_DETECT )
		return errENGINE_ACTION_SKIP; // don't scan multivolume archives

	if( PR_FAIL(CALL_SYS_ObjectCheck(m_pObject, m_pObject, IID_IO, PID_ANY, 0, cTRUE)) )
		return errOK_DECIDED;

	if( !m_pScanCtx->m_Settings->m_nSizeLimit.m_on )
		return errOK_DECIDED;

	tQWORD nSizeLimit = m_pScanCtx->m_Settings->m_nSizeLimit.m_val * 0x100000;
	if( !nSizeLimit )
		nSizeLimit = 0x19000;

	tQWORD nObjectSize = 0;
	CALL_IO_GetSize((hIO)m_pObject, &nObjectSize, IO_SIZE_TYPE_EXPLICIT);

	if( nSizeLimit > nObjectSize )
		return errOK_DECIDED;

	cStringObj strPakerName;
	GetEngine()->GetPakerName(*this, strPakerName);
	if( strPakerName == "Embedded" && nObjectSize < 0x200000 )
		return errOK_DECIDED;

	SendReport(pmc_PROTECTION_EVENTS_SPAM, OBJSTATUS_NOTPROCESSED, NPREASON_SIZE);
	TraceOut(".size");
	IncrementStatistic(STATISTIC_OFFSET(m_nNumNotProcessed));

	if( m_pScanCtx->m_Settings->m_nScanAction == SCAN_ACTION_DETECT )
	{
		m_pScanCtx->ProcessStop();
		return errOPERATION_CANCELED;
	}

	return errENGINE_ACTION_SKIP;
}

tERROR CObjectContext::PlaceInBackup(bool bForQuarantine)
{
	if( !(m_pScanCtx->m_Settings->m_nAskActions & ACTION_BACKUP) )
		return errOK;

	if( m_pScanCtx->m_fLockedProcessing )
		return errOK;

	CObjectContext* pContext = this;
	cAutoObj<cIO> hFile;
	hOBJECT hBackupIO = NULL;
	if( m_pScanCtx->m_fQuarantineProcessing )
	{
		pContext = m_pScanCtx;
	}
	else if( m_pScanCtx->m_pNative )
	{
		if( !m_pScanCtx->m_pNative->m_fMailBase )
		{
			pContext = m_pScanCtx->m_pNative;

			tDWORD pos = pContext->m_strObjectName.find_last_of(L"\\/:");
			if( pos != cStrObj::npos && pContext->m_strObjectName[pos] == ':' ) // stream
			{
				cStrObj sFileName;
				sFileName.assign(pContext->m_strObjectName, 0, pos);

				cIOObj file(m_pObject, cAutoString(sFileName), fACCESS_READ);
				if( PR_SUCC(file.last_error()) )
				{
					hFile = file.relinquish();
					hBackupIO = hFile;
				}
			}
		}
		else
		{
			for(CObjectContext *ctx = this; ctx; ctx = ctx->m_pPrev)
			{
				if( !ctx->m_fProcessing )
					continue;

				if( ctx->m_nObjectType != OBJTYPE_MAILMSGBODY &&
					ctx->m_nObjectType != OBJTYPE_MAILMSGATTACH )
					break;

				pContext = ctx;
			}
		}
	}

	if( !hBackupIO )
		hBackupIO = pContext->m_pObject;

	if( bForQuarantine && hBackupIO == m_pObject )
		return errOK;

	if( !hBackupIO || pContext->m_fBackuped )
		return errOK;

	pContext->m_fBackuped = 1;

	if( pContext->m_nObjectType == OBJTYPE_LOGICALDRIVE ||
		pContext->m_nObjectType == OBJTYPE_PHYSICALDISK )
		return errOK;

	if( hBackupIO->propGetBool(m_propBackupFlag) )
		return errOK;

	hBackupIO->propSetBool(m_propBackupFlag, cTRUE);

	cAskQBAction askAction;
	pContext->InitAskAction(askAction, ACTION_CANCEL|ACTION_OK);
	if( m_InfCtx->m_nDetectStatus == DSTATUS_SURE )
		askAction.m_nObjectStatus = OBJSTATUS_INFECTED;
	else
		askAction.m_nObjectStatus = OBJSTATUS_SUSPICION;
	askAction.m_hSource = (hIO)hBackupIO;

	if( hBackupIO != pContext->m_pObject )
		askAction.m_strObjectName.assign(hBackupIO, pgOBJECT_FULL_NAME);

	enActions nAction = m_pScanCtx->AskAction(cAskQBAction::BACKUP, askAction, fACCESS_READ);
	if( nAction != ACTION_OK )
	{
		SendReport(pmc_PROTECTION_EVENTS_CRITICAL, OBJSTATUS_NOTBACKUPPED, askAction.m_nNonCuredReason);

		if( m_pScanCtx->m_Settings->m_nScanAction == SCAN_ACTION_ASKUSER )
		{
			cAskObjectAction newAction;
			InitAskAction(newAction, 0);
			newAction.m_nNonCuredReason	= NCREASON_CANNOTBACKUP;

			m_pScanCtx->AskAction(cAskObjectAction::DISINFECT, newAction, fACCESS_NONE);
		}
		return errUNEXPECTED;
	}

	pContext->m_InfCtx->m_qwQBObjectId = askAction.m_qwQBObjectId;
	SendReport(pmc_PROTECTION_EVENTS_SPAM, OBJSTATUS_BACKUPPED);
	return errOK;
}

tERROR CObjectContext::PlaceInQuarantine()
{
	if( m_fQuarantined )
		return errOK;

	m_fQuarantined = 1;

	cAskQBAction askAction;
	InitAskAction(askAction, ACTION_CANCEL|ACTION_OK);
	askAction.m_nObjectStatus = OBJSTATUS_SUSPICION;
	askAction.m_hSource = (hIO)m_pObject;

	enActions nAction = m_pScanCtx->AskAction(cAskQBAction::QUARANTINE, askAction);
	if( nAction != ACTION_OK )
	{
		IncrementStatistic(STATISTIC_OFFSET(m_nNumAbandoned));
		SendReport(pmc_PROTECTION_EVENTS_CRITICAL, OBJSTATUS_NOTQUARANTINED, askAction.m_nNonCuredReason);

		if( m_pScanCtx->m_Settings->m_nScanAction != SCAN_ACTION_ASKUSER )
			return errUNEXPECTED;

		cAskObjectAction newAction;
		InitAskAction(newAction, ACTION_DELETE);
		newAction.m_nNonCuredReason	= askAction.m_nNonCuredReason;

		nAction = m_pScanCtx->AskAction(cAskObjectAction::DISINFECT, newAction, fACCESS_NONE);
		if( nAction != ACTION_DELETE )
			return errUNEXPECTED;
	}

	m_InfCtx->m_qwQBObjectId = askAction.m_qwQBObjectId;
	MarkDeleteOnClose();
	return errOK;
}

tERROR CObjectContext::CureOnReboot()
{
	cOS* hNativeOS = m_pScanCtx->m_Session->m_pAVS->m_nos;

	cStringObj strCopyName = m_strObjectName;
	strCopyName += ".kav";

	tERROR error = hNativeOS->Copy(cAutoString (m_strObjectName), cAutoString (strCopyName), cTRUE);
	if( PR_SUCC(error) )
	{
		cIOObj hFileIO(*m_pScanCtx->m_Session, cAutoString (strCopyName), fACCESS_RW);
		error = hFileIO.last_error();
		if( PR_SUCC(error) )
		{
			m_strObjectName.copy(hFileIO, CObjectContext::m_propVirtualName);
			m_hCopyIO = hFileIO.relinquish();
		}
	}

	if( PR_FAIL(error) )
	{
		hNativeOS->Delete(cAutoString (strCopyName));
		SendReport(pmc_PROTECTION_EVENTS_CRITICAL, OBJSTATUS_DISINFECT_ON_REBOOT_FAILED);
		return CureOnRebootFailed();
	}

	m_pScanCtx->ProcessStop();
	return errOK;
}

tERROR CObjectContext::CureOnRebootFailed()
{
TryCureAgain:
	int nActionMask = ACTION_DELETE|ACTION_CANCEL|ACTION_TRYAGAIN, nAction = ACTION_DELETE;

	if( !ChooseAction(nActionMask, NCREASON_COPYFAILED, nAction) )
	{
		cAskObjectAction askAction;
		InitAskAction(askAction, nActionMask);

		nAction = m_pScanCtx->AskAction(cAskObjectAction::DISINFECT, askAction);
	}

	switch( nAction )
	{
	case ACTION_TRYAGAIN:		goto TryCureAgain;
	case ACTION_DELETE:         return DeleteOnReboot();
	}
	return errUNEXPECTED;
}

tERROR CObjectContext::PostCureOnReboot()
{
	cProtectionSettings Settings;
	Settings = *m_pScanCtx->m_Settings;
	Settings.m_bTryDisinfect = true;
	Settings.m_bTryDelete = true;
	Settings.m_nScanAction = SCAN_ACTION_DISINFECT;

	cScanProcessInfo ProcessInfo;

	CScanContext ctx((hOBJECT)m_hCopyIO, m_pScanCtx->m_Session, &Settings, &ProcessInfo);
	ctx.m_fLockedProcessing = 1;
	ctx.Process();

	bool bDisinfected = false;
	if( !(ProcessInfo.m_nProcessStatusMask & cScanProcessInfo::DELETED) )
	{
		if( ProcessInfo.m_nProcessStatusMask & cScanProcessInfo::DISINFECTED )
		{
			tERROR errorFS = errUNEXPECTED;
			tERROR errorPr = errUNEXPECTED;
			cAutoString strNative(m_InfCtx->m_strArchiveObject);

			if( FSMoveFileEx )
			{
				cStringObj strCopyName; strCopyName.assign(m_hCopyIO, plNATIVE_PATH);
				errorFS = FSMoveFileEx(cAutoString(strCopyName), strNative, 0);
			}

			if( PrMoveFileEx )
				errorPr = PrMoveFileEx((hOBJECT)m_hCopyIO, strNative, fPR_FILE_DELAY_UNTIL_REBOOT | fPR_FILE_REPLACE_EXISTING);

			PR_TRACE((g_root, prtIMPORTANT, "avs\tCObjectContext::PostCureOnReboot() - errorFS = (%terr), errorPr = (%terr)", errorFS, errorPr));

			if( PR_SUCC(errorFS) || PR_SUCC(errorPr) )
			{
				IncrementStatistic(STATISTIC_OFFSET(m_nNumDisinfected));
				SendReport(pmc_PROTECTION_EVENTS_FIX, OBJSTATUS_DISINFECTED_ON_REBOOT);
				m_pScanCtx->m_Session->m_pAVS->LockedObjectProcessed(m_pScanCtx->m_Session,
					m_InfCtx, OBJSTATUS_DISINFECTED_ON_REBOOT);

				bDisinfected = true;
			}
		}

		if( !bDisinfected )
			m_hCopyIO->propSetBool(pgOBJECT_DELETE_ON_CLOSE, cTRUE);

		m_hCopyIO->sysCloseObject();
	}

	if( !bDisinfected )
		CureOnRebootFailed();

	m_hCopyIO = NULL;
	return errOK;
}

tERROR CObjectContext::DeleteOnReboot(bool bForQuarantine)
{
	tERROR errorFS = errUNEXPECTED;
	tERROR errorPr = errUNEXPECTED;
	cAutoString strNative(m_InfCtx->m_strArchiveObject);

	if( FSMoveFileEx )
		errorFS = FSMoveFileEx(strNative, NULL, 0);

	if( PrMoveFileEx )
		errorPr = PrMoveFileEx(strNative, NULL, fPR_FILE_DELAY_UNTIL_REBOOT);

	PR_TRACE((g_root, prtIMPORTANT, "avs\tCObjectContext::DeleteOnReboot() - errorFS = (%terr), errorPr = (%terr)", errorFS, errorPr));

	if( PR_FAIL(errorFS) && PR_FAIL(errorPr) )
	{
		SendReport(pmc_PROTECTION_EVENTS_CRITICAL, OBJSTATUS_DELETE_ON_REBOOT_FAILED);
		return errUNEXPECTED;
	}

	enObjectStatus nStatus;
	if( bForQuarantine )
		IncrementStatistic(STATISTIC_OFFSET(m_nNumQuarantined)), nStatus = OBJSTATUS_QUARANTINED_ON_REBOOT;
	else
		IncrementStatistic(STATISTIC_OFFSET(m_nNumDeleted)), nStatus = OBJSTATUS_DELETED_ON_REBOOT;

	SendReport(pmc_PROTECTION_EVENTS_FIX, nStatus);
	m_pScanCtx->m_Session->m_pAVS->LockedObjectProcessed(m_pScanCtx->m_Session, m_InfCtx, nStatus);

	CleanUp();
	m_pScanCtx->ProcessStop();
	return errOK;
}

tERROR CObjectContext::CleanUp()
{
	CheckMemoryModule(true);

	m_pScanCtx->m_Session->CleanupStartupObject(m_strObjectName,
		m_pObject, m_InfCtx->m_strDetectName);

	return errOK;
}

tERROR CObjectContext::DeleteTryAgain()
{
	tERROR error = m_pScanCtx->m_Session->m_pAVS->m_nos->Delete(cAutoString (m_strObjectName));
	if( PR_SUCC(error) )
		ProcessDeleteOnClose();
	return error;
}

tERROR CObjectContext::MarkDeleteOnClose()
{
	if( m_fNative )
		CleanUp();

	return m_pObject->propSetBool(pgOBJECT_DELETE_ON_CLOSE, cTRUE);
}

tERROR CObjectContext::RenameObject()
{
	if( m_nObjectType != OBJTYPE_MAILMSGATTACH )
		return errOK;

	cStringObj strAttachName;
	if( PR_FAIL(strAttachName.assign(m_pObject, pgMESSAGE_PART_FILE_NAME)) )
		return errOK;

	tDWORD nSize = strAttachName.size();
	if( nSize )
	{
		strAttachName.erase(nSize-1, 1);
		strAttachName += '_';
		strAttachName.copy(m_pObject, pgMESSAGE_PART_FILE_NAME);

		cStringObj strContentId;
		strContentId.assign(m_pObject, pgMESSAGE_PART_CONTENT_ID);

		if( !strContentId.empty() )
		{
			strContentId = L"XXX";
			strContentId.copy(m_pObject, pgMESSAGE_PART_CONTENT_ID);
		}

		cStringObj strContentType;
		strContentType.assign(m_pObject, pgMESSAGE_PART_CONTENT_TYPE);

		if( !strContentType.empty() )
		{
			strContentType = L"application/octet-stream";
			strContentType.copy(m_pObject, pgMESSAGE_PART_CONTENT_TYPE);
		}

		if( PR_SUCC(((cIO*)m_pObject)->Flush()) )
			SendReport(pmc_PROTECTION_EVENTS_FIX, OBJSTATUS_RENAMED);
		else
			SendReport(pmc_PROTECTION_EVENTS_IMPORTANT, OBJSTATUS_RENAME_FAILED);
	}
	return errOK;
}

void CObjectContext::IncrementStatistic(tDWORD nOffset, tINT nNum)
{
	for(CObjectContext *ctx = this; ctx; ctx = ctx->m_pPrev)
		(*(tLONG*)((tBYTE*)(cProtectionStatistics*)ctx + nOffset))+=nNum;

	if( !m_pScanCtx->m_fTreatsProcessing ||
			nOffset == STATISTIC_OFFSET(m_nNumDisinfected) ||
			nOffset == STATISTIC_OFFSET(m_nNumQuarantined) ||
			nOffset == STATISTIC_OFFSET(m_nNumDeleted) ||
			nOffset == STATISTIC_OFFSET(m_nNumUntreated) && nNum < 0 )
		m_pScanCtx->m_Session->IncrementStatistics(m_nObjectScope, nOffset, nNum);
}

void CObjectContext::UpdateStatus(bool bForDelete)
{
	tDWORD nDisinfected = m_nNumDisinfected + m_nNumQuarantined + m_nNumDeleted;
	tINT nInfectedNum =  m_nNumDetected - nDisinfected, nInfectedCounter;

	if( m_pObject && PR_SUCC(m_pObject->propGet(NULL, m_propInfectedCounter, &nInfectedCounter, sizeof(tDWORD))) )
		if( nInfectedNum > 0 && !nInfectedCounter && !bForDelete )
		{
			IncrementStatistic(STATISTIC_OFFSET(m_nNumDisinfected), nInfectedNum-nInfectedCounter);
			nInfectedNum = nInfectedCounter;
		}

	if( m_nNumNotProcessed || m_nNumAbandoned || m_nNumPswProtected || m_nNumScanErrors )
		m_nStatus |= cScanProcessInfo::SOME_SKIPPED;

	if( m_nNumDetected )
	{
		m_nStatus |= cScanProcessInfo::DETECTED;
		if( m_nNumDetectedExact )
			m_nStatus |= cScanProcessInfo::DETECTED_SURE;
		if( nDisinfected )
			m_nStatus |= cScanProcessInfo::SOME_CHANGES;

		if( bForDelete || nInfectedNum <= 0 )
			m_nStatus |= cScanProcessInfo::DISINFECTED;
	}

	if( nInfectedNum > 0 )
	{
		if( !bForDelete )
			m_nStatus &= ~cScanProcessInfo::DISINFECTED;
		else
			IncrementStatistic(STATISTIC_OFFSET(m_nNumDisinfected), nInfectedNum);
	}

	if( !bForDelete && m_pObject && m_pObject->propGetBool(pgOBJECT_DELETE_ON_CLOSE) )
		m_nStatus |= cScanProcessInfo::DELETED;
}

bool CObjectContext::UpdateDetectedInfo(bool bForDelete)
{
	UpdateStatus(bForDelete);

	if( !m_InfCtx /*|| !m_InfCtx->m_qwUniqueId*/ )
		return false;

	enObjectStatus nOldStatus = m_InfCtx->m_nObjectStatus;
	enObjectStatus nNewStatus;

	if( m_nStatus & cScanProcessInfo::DISINFECTED )
	{
		if( m_nStatus & cScanProcessInfo::DELETED )
		{
			if( m_fQuarantined )
				nNewStatus = OBJSTATUS_QUARANTINED;
			else
				nNewStatus = OBJSTATUS_DELETED;
		}
		else
			nNewStatus = OBJSTATUS_DISINFECTED;
	}
	else
	{
		if( m_fSkippedByExclude )
			nNewStatus = OBJSTATUS_ADDEDTOEXCLUDE;
		else
			nNewStatus = OBJSTATUS_NOTDISINFECTED;
	}

	if( nNewStatus == nOldStatus )
		return true;

	if( !!(m_nStatus & cScanProcessInfo::DELETED) != bForDelete )
		return true;

	m_InfCtx->m_nObjectStatus = nNewStatus;

	// BF 28362: для m_bIsRemote надо и в локальном AVS увеличить
	// счётчик и послать информацию в удалённый AVS
	if( m_pScanCtx->m_Session->m_pAVS->m_bIsRemote ||
		m_pScanCtx->m_fQuarantineProcessing || !m_pScanCtx->m_Settings->m_bRegisterThreats ||
		m_pScanCtx->m_fLockedProcessing || !m_fProcessing )
	{
		if( m_fDetected )
			if( !IS_INFECTED(nOldStatus) && IS_INFECTED(nNewStatus) )
			{
				IncrementStatistic(STATISTIC_OFFSET(m_nNumThreats));
				IncrementStatistic(STATISTIC_OFFSET(m_nNumUntreated));
			}
			else if( IS_INFECTED(nOldStatus) && !IS_INFECTED(nNewStatus) )
				IncrementStatistic(STATISTIC_OFFSET(m_nNumUntreated), -1);

		if (!m_pScanCtx->m_Session->m_pAVS->m_bIsRemote)
			return true;
	}

	m_InfCtx->m_nDescription = m_fDetectedByHash ? DTREASON_BYHASH : (m_fDetected ? 0 :
		(m_fArchive ? DTREASON_INFORMATION : DTREASON_NONE));

	bool bIsUntreatable = (m_InfCtx->m_nDetectType == DETYPE_VULNERABILITY) ||
		(nNewStatus == OBJSTATUS_NOTDISINFECTED) && 
		((m_pScanCtx->m_Session->m_nOrigin != OBJECT_ORIGIN_GENERIC) /*&& !m_fSkippedByVerdict*/);

	if( bIsUntreatable )
		m_InfCtx->m_nObjectStatus = OBJSTATUS_INFECTED;

	bool bResult = m_pScanCtx->m_Session->UpdateDetectedInfo(m_InfCtx);

	if( bIsUntreatable )
		m_InfCtx->m_nObjectStatus = OBJSTATUS_NOTDISINFECTED;

	return bResult;
}

tERROR CObjectContext::SendReport(tDWORD nMsgClass, enObjectStatus nStatus, int nDesc, cStringObj *strDesc)
{
	if( !m_fProcessing )
	{
		ProcessScanBegin();
		m_fProcessing = 0;
	}

	m_tmTimeStamp = cDateTime::now_utc();
	m_tmTimeDelta = m_pScanCtx->GetTimeDelta();

	m_nObjectStatus = nStatus;
	m_nDescription = nDesc;

	if( strDesc )
		m_strDetectName = *strDesc;
	else
		m_strDetectName.clear();

	if( nStatus == OBJSTATUS_OK )
	{
		m_nDetectType = DETYPE_UNKNOWN;
		m_nDetectDanger = DETDANGER_UNKNOWN;
	}
	else if( m_InfCtx )
	{
		m_nDetectType = m_InfCtx->m_nDetectType;
		m_nDetectDanger = m_InfCtx->m_nDetectDanger;
	}

	tERROR error = m_pScanCtx->m_Session->SendReport(nMsgClass, this,
		nMsgClass != pmc_EVENTS_NOTIFY ? m_pObject : NULL);

	if( error == errOPERATION_CANCELED )
		m_pScanCtx->ProcessStop();

	return error;
}

tERROR CObjectContext::ProcessScanBegin()
{
	if( m_fPostProcessing )
		return errOK;

	if( m_pPrev )
		InitContext();

	InitObjectInfo();
//	TraceOut(".begin", prtIMPORTANT);

	if( !m_fPreProcessing && m_fNative && !m_pScanCtx->m_pNative )
	{
		m_pScanCtx->m_pNative = this;
		m_pScanCtx->m_nReason = NCREASON_NONE;
	}

	if( !m_fTreatsProcessing && !m_pScanCtx->m_pScanTop )
		m_pScanCtx->m_pScanTop = this;

	m_fProcessing = 1;

	if( m_fPreProcessing )
		m_pScanCtx->m_ProcessInfo->m_nProcessStatusMask |= cScanProcessInfo::PRE_PROCESSED;
	else
		m_pScanCtx->m_Session->SetCurrentObject(*this);

	if( m_pPrev )
		m_pScanCtx->m_Session->m_task->sysSendMsg(pmc_AVS_OBJECT, 0, GetCached(),
			(cObjectInfo*)this, SER_SENDMSG_PSIZE);

	return errOK;
}

tERROR CObjectContext::ProcessScanDone()
{
	if( m_fPostProcessing )
		return errOK;

	m_fPostProcessing = 1;
	GetEngine()->PostProcessObject(*this);
	m_fPostProcessing = 0;

	if( m_DetectInfo && !m_nNumDetected )
	{
		m_fExternalDetectProcessing = 1;
		ProcessVirusDetect();
		ProcessDisinfectRequest();
	}

	TraceOut(".done");

	if( m_pScanCtx->m_pNative == this )
	{
		if( m_hCopyIO )
			PostCureOnReboot();

		m_pScanCtx->m_pNative = NULL;
		m_pScanCtx->m_nReason = NCREASON_NONE;
	}

	if( PR_SUCC(m_pScanCtx->CheckState()) && !m_fStopProcessing )
	{
		UpdateDetectedInfo();
		IncrementStatistic(STATISTIC_OFFSET(m_nNumScaned));

		if( !(m_nStatus & cScanProcessInfo::DELETED) )
		{
			tBOOL bUpdateStatus = cTRUE, bDeleteStatus = cFALSE;

			if( !(m_nStatus & cScanProcessInfo::DETECTED) )
			{
				if( m_nStatus & cScanProcessInfo::SOME_SKIPPED )
					bUpdateStatus = cFALSE;
				else if( !m_fSkippedByType && !m_fSkippedBySFDB && !m_fOkProcessed )
				{
					if( m_fArchiveSkipped )
						SendReport(pmc_PROTECTION_EVENTS_SPAM, OBJSTATUS_NOTPROCESSED, NPREASON_TYPE);
					else
						SendReport(pmc_PROTECTION_EVENTS_NOTIFY, OBJSTATUS_OK, OKREASON_NONE);
					m_fOkProcessed = 1;
				}
			}
			else
			{
				if( m_fDetected && (m_nStatus & cScanProcessInfo::DISINFECTED) )
				{
					if( !m_fFixProcessed )
						SendReport(pmc_PROTECTION_EVENTS_FIX, OBJSTATUS_CURED);
					if( !m_pScanCtx->m_fLockedProcessing )
						SendReport(pmc_PROTECTION_EVENTS_FIX, OBJSTATUS_DISINFECTED);
				}

				bDeleteStatus = cTRUE;

				if( PR_SUCC(g_root->sysCheckObject(m_pObject, IID_IO)) )
					((cIO*)m_pObject)->Flush();
			}

			if( bUpdateStatus )
			{
				if( m_nICheckerCtx )
					m_pScanCtx->ICUpdate(PID_ICHECKER2, bDeleteStatus, m_nICheckerCtx, (cObj*)GetCached());

				if( !m_pPrev )
					m_pScanCtx->ICUpdate(PID_ICHECKERSA, bDeleteStatus);
			}
		}
	}

	if( m_pScanCtx->m_pScanTop == this )
		m_pScanCtx->m_pScanTop = NULL;

	if( m_nICheckerCtx )
		GetEngine()->ICFree(m_nICheckerCtx);

	return errOK;
}

tERROR CObjectContext::ProcessArchiveBegin(tQWORD qwOffset, tDWORD* pPlugins)
{
	cAutoObj<cIO> dmap;
	cAutoObj<cOS> os;
	cIO* io = GetCached();

	tERROR error = ProcessOSOffset(qwOffset);
	if( PR_FAIL(error) )
		return error;

	if( qwOffset )
	{
		tQWORD qwSize = 0;
		error = io->GetSize(&qwSize, IO_SIZE_TYPE_EXPLICIT);
		if( qwOffset >= qwSize )
			return errOK;

		qwSize -= qwOffset;

		error = io->sysCreateObject(dmap, IID_IO, DMAP_ID);
		if( PR_SUCC(error) ) error = dmap->propSetDWord(ppMAP_AREA_ORIGIN, (tDWORD)io);
		if( PR_SUCC(error) ) error = dmap->propSetQWord(ppMAP_AREA_START, qwOffset);
		if( PR_SUCC(error) ) error = dmap->propSetQWord(ppMAP_AREA_SIZE, qwSize);
		if( PR_SUCC(error) ) error = dmap->propSetDWord(pgOBJECT_ACCESS_MODE, fACCESS_NO_CHANGE_MODE|fACCESS_READ);
		if( PR_SUCC(error) ) error = dmap->sysCreateObjectDone();

		if( PR_FAIL(error) )
			return errOK;

		dmap->propSetDWord(m_propReadonlyError, errOBJECT_WRITE_NOT_SUPPORTED);
		io = dmap;
	}

	tDWORD nMask = m_pScanCtx->m_nClassMask;

	for( ;*pPlugins; pPlugins += 3 )
	{
		if( !(pPlugins[2] & nMask) )
			continue;

		if( PR_FAIL(io->sysRecognize(IID_OS, pPlugins[0], pPlugins[1])) )
			continue;

		tERROR error = io->sysCreateObject(os, IID_OS, pPlugins[0], 0);

		if( PR_SUCC(error) )
			os->propSetBool(pgMULTIVOL_AS_SINGLE_SET, cTRUE);
		
		if( PR_SUCC(error) )
		{
			tQWORD qwHash = os->propGetQWord(pgOBJECT_HASH);
			if( qwHash && PR_FAIL(ProcessArchiveCheck(qwHash, qwOffset)) )
				return errOPERATION_CANCELED;
		}

		if( PR_SUCC(error) )
			error = os->sysCreateObjectDone();

		if( PR_FAIL(error) )
		{
			os.relinquish();
			continue;
		}

		CObjectContext* pOsCtx = GetNextContext(os);
		ProcessOSEnter(os);
		pOsCtx->ProcessOS();

		if( !!dmap && dmap->propGetBool(pgOBJECT_DELETE_ON_CLOSE) )
			io->propSetBool(pgOBJECT_DELETE_ON_CLOSE, cTRUE);

		ProcessArchiveDone(os);
		return errOPERATION_CANCELED;
	}

	return errOK;
}

tERROR CObjectContext::ProcessArchiveCheck(tQWORD qwHash, tQWORD qwOffset)
{
	if( !qwHash )
		return IsOSShouldBeScan(false);

	tERROR error = errOK;
	if( m_pObject->propGetObj(CObjectContext::m_propExecutableParent) )
		qwHash ^= 1;

	cVector<CInfectedHashData>& vHash = m_pScanCtx->m_vHashData;
	tDWORD i, j, num = vHash.size();
	for(i = 0; i < num; i++)
	{
		bool bVolumeFound = false;

		cVector<tQWORD>& v = vHash[i].m_vVolumes;
		for(j = 0; j < v.size(); j++)
			if( qwHash == v[j] ) // volume found
			{
				bVolumeFound = true;
				break;
			}

		if( !bVolumeFound && vHash[i].m_qwHash != qwHash )
			continue;

		if( !vHash[i].m_bWasDone )
			break;

		error = errOPERATION_CANCELED;
		if( !vHash[i].m_nNumDetected )
			break;

		m_pHash = &vHash[i];
		m_pBuff = m_pHash->m_hIo;
		m_pObject->propSetDWord(m_propInfectedCounter, m_pHash->m_nNumDetected);

		tERROR ret = ProcessVirusDetect(true);
		m_pBuff = NULL;

		if( ret == errOK_DECIDED )
		{
			cObj* pDeletable = NULL;
			if( m_pHash->m_hIo )
			{
				tERROR error = GetEngine()->SetWriteAccess(*this, pDeletable);
				if( PR_SUCC(error) )
					error = PrCopy((cObj*)m_pObject, (cObj*)m_pHash->m_hIo, 0);

				if( PR_SUCC(error) )
					ProcessDisinfected();
				else
					ProcessDisinfectImpossible();
			}
			else
			{
				if( !qwOffset )
					error = errOK;
			}
		}

		m_pHash = NULL;
		break;
	}

	m_nHashPos = i + 1;
	if( i == num )
	{
		CInfectedHashData& pData = vHash.push_back();
		pData.m_qwHash = qwHash;
	}

	if( PR_SUCC(error) )
		error = IsOSShouldBeScan(false);

	return error;
}

tERROR CObjectContext::ProcessOSOffset(tQWORD qwOffset)
{
	if( m_pvOffsets )
	{
		for(tDWORD i = 0; i < m_pvOffsets->size(); i++)
			if( qwOffset == m_pvOffsets->at(i) )
				return errOBJECT_ALREADY_EXISTS;
	}

	if( !m_pvOffsets && !(m_pvOffsets = new cVector<tQWORD>) )
		return errNOT_ENOUGH_MEMORY;

	m_pvOffsets->push_back(qwOffset);

	if( !qwOffset || !m_pPrev || !m_pPrev->m_fUnpacked )
		return errOK;

	tQWORD qwSize = 0, qwPSize = 0;
	GetCached()->GetSize(&qwSize, IO_SIZE_TYPE_EXPLICIT);
	m_pPrev->GetCached()->GetSize(&qwPSize, IO_SIZE_TYPE_EXPLICIT);
	qwSize -= qwOffset;
	if( qwPSize > qwSize )
	{
		tQWORD qwPOffs = qwPSize - qwSize, qwP = 0, qw = 0;
		GetCached()->SeekRead(NULL, qwOffset, &qw, sizeof(qw));
		m_pPrev->GetCached()->SeekRead(NULL, qwPOffs, &qwP, sizeof(qwP));
		if( qwP == qw )
			m_pPrev->ProcessOSOffset(qwPOffs);
	}

	return errOK;
}

tERROR CObjectContext::ProcessArchiveDone(cOS* pOS)
{
	ProcessOSPreClose(pOS);

	if( !m_nHashPos )
		return errOK;

	cVector<CInfectedHashData>& vHash = m_pScanCtx->m_vHashData;
	CInfectedHashData& pData = vHash[m_nHashPos-1];

	pData.m_bWasDone = cTRUE;
	if( m_InfCtx )
	{
		pData.m_nNumDetected = m_nNumDetected;
		pData.m_dwType = m_InfCtx->m_nDetectType;
		pData.m_dwStatus = m_InfCtx->m_nDetectStatus;
		pData.m_dwDanger = m_InfCtx->m_nDetectDanger;
		pData.m_sName = m_InfCtx->m_strDetectName;

		if( !pData.m_hIo && (m_nStatus & cScanProcessInfo::SOME_CHANGES) )
		{
			tERROR err = m_pScanCtx->m_pObject->sysCreateObjectQuick(
					(cObj**)&pData.m_hIo, IID_IO, PID_TMPFILE);

			if( PR_SUCC(err) )
				err = PrCopy((cObj*)pData.m_hIo, (cObj*)m_pObject, 0);

			if( PR_FAIL(err) && pData.m_hIo )
			{
				pData.m_hIo->sysCloseObject();
				pData.m_hIo = NULL;
			}
		}
	}

	m_nHashPos = 0;
	return errOK;
}

tERROR CObjectContext::ProcessOS()
{
	m_strObjectName.assign(m_pObject, pgOBJECT_NAME);
	m_pContext = m_pObject;
	m_fOSObject = 1;

	tERROR error = IsOSShouldBeScan(true);
	if( PR_FAIL(error) )
	{
		if( !m_nNumNotProcessed )
			IncrementStatistic(STATISTIC_OFFSET(m_nNumScaned));

		UpdateStatus();
		return errOK;
	}

	cAutoObj<cObjPtr> hObjPtr;
	error = ((cOS*)m_pObject)->PtrCreate(&hObjPtr, NULL);

	if( PR_SUCC(error) )
		ProcessObjPtr(hObjPtr);

	ProcessOSPreClose((cOS*)m_pObject);
	return errOK;
}

tERROR CObjectContext::ProcessObjPtr(cObjPtr* obj)
{
	while( PR_SUCC(obj->Next()) )
	{
		if( obj->propGetBool(pgIS_FOLDER) )
		{
			cAutoObj<cObjPtr> hClone;
			if( PR_SUCC(obj->Clone(&hClone)) && PR_SUCC(hClone->StepDown()) )
				ProcessObjPtr(hClone);
			continue;
		}

		cAutoObj<cIO> hObj;
		tERROR err = obj->IOCreate(&hObj, 0, fACCESS_READ, fOMODE_OPEN_IF_EXIST);

		if( PR_SUCC(err) )
			ProcessSubIO(hObj, NULL, true);
		else
		{
			cStrObj sName(m_pPrev->m_strObjectName), sPtrName;
			sName += "/";
			sPtrName.assign(obj, pgOBJECT_FULL_NAME);
			sName += sPtrName;
			sName.copy(obj, m_propVirtualName);

			CObjectContext* pCtx = GetNextContext((cObj*)obj);

			if( err == errOBJECT_PASSWORD_PROTECTED )
				pCtx->ProcessPasswordProtected();
		}
	}
	return errOK;
}

tERROR CObjectContext::ProcessOSEnter(cOS* pOS)
{
	if( pOS )
		m_sPackerName.assign(pOS, pgINTERFACE_COMMENT);
	else
		GetEngine()->GetPakerName(*this, m_sPackerName);

	SendReport(pmc_PROTECTION_EVENTS_SPAM, OBJSTATUS_ARCHIVE, 0, &m_sPackerName);
	IncrementStatistic(STATISTIC_OFFSET(m_nNumArchived));

	if( m_sPackerName != "Embedded" )
		m_fArchive = 1;
	else
		m_fEmbedded = 1;

	if( m_pBuff && (*(tDWORD*)m_pBuff & ENGINE_ACTION_CLASS_MAIL_DATABASE) )
		m_fMailBase = 1;

	if( m_fDetectedByHash )
		IncrementStatistic(STATISTIC_OFFSET(m_nNumDetected),  -m_nNumDetected);
	return errOK;
}

tERROR CObjectContext::ProcessOSPreClose(cOS* pOS)
{
	if( pOS )
		m_pContext = (cObj*)pOS;

	UpdateStatus();

	if( m_nOSICheckerCtx )
	{
		if( !(m_nStatus & cScanProcessInfo::DELETED) )
		{
			tBOOL bUpdateStatus = cTRUE, bDeleteStatus = cFALSE;

			if( m_nStatus & cScanProcessInfo::DETECTED )
				bDeleteStatus = cTRUE;
			else if( m_nStatus & cScanProcessInfo::SOME_SKIPPED )
				bUpdateStatus = cFALSE;

			if( bUpdateStatus )
				m_pScanCtx->ICUpdate(PID_ICHECKER2, bDeleteStatus, m_nOSICheckerCtx, m_pContext);
		}

		GetEngine()->ICFree(m_nOSICheckerCtx);
	}

	if( !(m_nStatus & cScanProcessInfo::DETECTED) || (m_nStatus & cScanProcessInfo::DELETED) )
		return errOK;

	if( !m_pContext )
		return errOK;

	cStringObj strOldSubject;
	if( PR_FAIL(strOldSubject.assign(m_pContext, pgMESSAGE_SUBJECT)) )
		return errOK;

	static const tCHAR *strSubjects[] = {
		"Message is infected : ",
		"Message has been disinfected : ",
		"Message has a suspicious part : ",
		"Suspicious part has been deleted : ",
		"Suspicious part has been quarantined : ",
	};

	const tCHAR * strPrefix = NULL;
	if( m_nStatus & cScanProcessInfo::DISINFECTED )
	{
		if( m_nStatus & cScanProcessInfo::DETECTED_SURE )
			strPrefix = strSubjects[1];
		else
			strPrefix = strSubjects[m_nNumQuarantined ? 4 : 3];
	}
	else
	{
		if( m_nStatus & cScanProcessInfo::DETECTED_SURE )
			strPrefix = strSubjects[0];
		else
			strPrefix = strSubjects[2];
	}

	cStringObj strTemp = strOldSubject;

	tDWORD idx;
	for( int i = 0; i < 4; i++ )
		while( (idx = strTemp.find(strSubjects[i])) != cStringObj::npos )
			strTemp.erase(idx, strlen(strSubjects[i]));

	cStringObj strNewSubject = strPrefix;
	strNewSubject += strTemp;
	if( strNewSubject != strOldSubject )
		strNewSubject.copy(m_pContext, pgMESSAGE_SUBJECT);
	return errOK;
}

tERROR CObjectContext::ProcessSubIO(hIO obj, tPTR ioCtx, bool bOs, bool bPswd)
{
	cStringObj strObjName;
	strObjName.assign(obj, pgOBJECT_FULL_NAME);

	cStringObj strVirtualName;
	strVirtualName = (m_pPrev && bOs) ? m_pPrev->m_strObjectName : m_strObjectName;
	if( !strVirtualName.empty() && !strObjName.empty() )
		strVirtualName += bOs ? "/" : "//";
	strVirtualName += strObjName;

	strVirtualName.copy(obj, m_propVirtualName);

	CObjectContext* pCtx = GetNextContext((cObj*)obj);
	pCtx->m_pIOCtx = ioCtx;
	if( bPswd )
		pCtx->ProcessPasswordProtected();
	else
		pCtx->GetEngine()->ProcessObject(*pCtx);
	pCtx->m_pIOCtx = NULL;

	if( obj->propGetBool(pgOBJECT_DELETE_ON_CLOSE) )
		return errOK;

	pCtx->CheckDetectByMask();
	return errOK;
}

tERROR CObjectContext::ProcessSubIOEnter()
{
	GetEngine()->GetPakerName(*this, m_sPackerName);
	SendReport(pmc_PROTECTION_EVENTS_SPAM, OBJSTATUS_PACKED, 0, &m_sPackerName);
	IncrementStatistic(STATISTIC_OFFSET(m_nNumPacked));
	m_fUnpacked = 1;
	return errOK;
}

tERROR CObjectContext::ProcessEncrypted()
{
	SendReport(pmc_PROTECTION_EVENTS_SPAM, OBJSTATUS_ENCRYPTED);
	return errOK;
}

tERROR CObjectContext::ProcessPasswordProtected()
{
	SendReport(pmc_PROTECTION_EVENTS_IMPORTANT, OBJSTATUS_PASSWORD_PROTECTED);
	IncrementStatistic(STATISTIC_OFFSET(m_nNumPswProtected));
	return errOK;
}

tERROR CObjectContext::ProcessCorrupted()
{
	SendReport(pmc_PROTECTION_EVENTS_SPAM, OBJSTATUS_CORRUPTED);
	IncrementStatistic(STATISTIC_OFFSET(m_nNumCorrupted));
	return errOK;
}

tERROR CObjectContext::ProcessVirusDetect(bool fByHash)
{
	if( !m_fProcessing )
		ProcessScanBegin();

	m_fDetected = 1;
	if( !InitInfectedInfo() )
		return errENGINE_ACTION_SKIP;

	if( m_InfCtx->m_nDetectType == DETYPE_VULNERABILITY )
	{
		InitDetectedObjectId();
		IncrementStatistic(STATISTIC_OFFSET(m_nNumScaned));
		SendReport(pmc_PROTECTION_EVENTS_IMPORTANT, m_nObjectStatus, DTREASON_NONE, &m_InfCtx->m_strDetectName);
		return errOPERATION_CANCELED;
	}

	enScanAction nScanAction = m_pScanCtx->m_Settings->m_nScanAction;
	if( nScanAction == SCAN_ACTION_DETECT || nScanAction == SCAN_ACTION_ASKDETECT )
	{
		TraceOut("*** detected");
		m_pScanCtx->m_nStatus |= cScanProcessInfo::DETECTED;
		IncrementStatistic(STATISTIC_OFFSET(m_nNumScaned));
		IncrementStatistic(STATISTIC_OFFSET(m_nNumDetected));

		if( m_nObjectOrigin != OBJECT_ORIGIN_GENERIC || nScanAction == SCAN_ACTION_ASKDETECT )
		{
			SendReport(pmc_PROTECTION_EVENTS_IMPORTANT, m_nObjectStatus, DTREASON_NONE, &m_InfCtx->m_strDetectName);
			SendReport(pmc_PROTECTION_EVENTS_FIX, OBJSTATUS_DENIED);
			InitDetectedObjectId();
		}

		if( !(m_InfCtx->m_nDetectDanger == DETDANGER_LOW && m_nObjectOrigin == OBJECT_ORIGIN_GENERIC) )
		{
			m_pScanCtx->ProcessStop();
			return errOPERATION_CANCELED;
		}
	}

	tDWORD nNumDetected = 1;
	if( fByHash )
	{
//		if( m_pBuff )
			m_fArchive = 1;
		m_fDetectedByHash = 1;
		nNumDetected = m_pObject->propGetDWord(m_propInfectedCounter);
	}

	long nStatOffset = 0; enObjectStatus nObjStatus;
	switch( m_InfCtx->m_nDetectStatus )
	{
	case DSTATUS_SURE:
		nStatOffset = STATISTIC_OFFSET(m_nNumDetectedExact);
		nObjStatus = OBJSTATUS_INFECTED;
		break;

	case DSTATUS_HEURISTIC:
	case DSTATUS_PARTIAL:
	case DSTATUS_PROBABLY:
	default:
		nStatOffset = STATISTIC_OFFSET(m_nNumDetectedSusp);
		nObjStatus = OBJSTATUS_SUSPICION;
		break;
	}

	tDWORD nDescr = fByHash ? DTREASON_BYHASH : DTREASON_NONE;
	SendReport(pmc_PROTECTION_EVENTS_IMPORTANT, nObjStatus, nDescr, &m_InfCtx->m_strDetectName);

	TraceOut("*** infected");
	IncrementStatistic(nStatOffset, nNumDetected);
	IncrementStatistic(STATISTIC_OFFSET(m_nNumDetected), nNumDetected);

	InitDetectedObjectId();

	if( nScanAction == SCAN_ACTION_POSTPONE )
		return errENGINE_ACTION_SKIP;

	if( fByHash )
	{
		if( !m_pBuff )
			return ProcessDisinfectRequest();

		PlaceInBackup();
	}

	return errOK_DECIDED;
}

tERROR CObjectContext::ProcessDisinfectImpossible(enNotCuredReason nNonCuredReason)
{
	TraceOut("*** disinfect impossible");
	ProcessDisinfectRequest(nNonCuredReason);
	return errOK;
}

tERROR CObjectContext::ProcessDisinfected()
{
	TraceOut("*** disinfected");

	enObjectStatus nFixStatus = OBJSTATUS_UNKNOWN;
	if( m_fDetectedByHash )
		nFixStatus = OBJSTATUS_OVERWRITED;
	else
		nFixStatus = OBJSTATUS_CURED;

	if( nFixStatus != OBJSTATUS_UNKNOWN )
	{
		SendReport(pmc_PROTECTION_EVENTS_FIX, nFixStatus, 0, &m_InfCtx->m_strDetectName);
		m_fFixProcessed = 1;
	}

	IncrementStatistic(STATISTIC_OFFSET(m_nNumDisinfected), m_fDetectedByHash ? m_nNumDetected : 1);
	UpdateDetectedInfo();
	return errOK;
}

tERROR CObjectContext::ProcessProcessingError(tERROR* pError)
{
	tERROR dwError = pError ? *pError : m_pBuff ? *(tERROR*)m_pBuff : errUNEXPECTED;
	switch( dwError )
	{
		case errOBJECT_CANNOT_BE_INITIALIZED:
		case errOBJECT_NOT_CREATED:
			return errOK;
		case errOPERATION_CANCELED:
			return errOPERATION_CANCELED;
	}

	TraceOut("*** process error ");
	enNotCuredReason nReason = GetNotProcessingReason();
	if( nReason != NCREASON_NONE && m_fProcessing && m_pObject->propGetDWord(m_propInfectedCounter) )
	{
		ProcessDisinfectImpossible(nReason);
		return errOK;
	}

	nReason = (enNotCuredReason)dwError;
	SendReport(pmc_PROTECTION_EVENTS_CRITICAL, OBJSTATUS_PROCESSING_ERROR, nReason);
	IncrementStatistic(STATISTIC_OFFSET(m_nNumScanErrors));
	return errOK;
}

tERROR CObjectContext::ProcessNonCurable(enNotCuredReason nReason)
{
	tDWORD nMsg = pmc_PROTECTION_EVENTS_CRITICAL;
	if( nReason == NCREASON_CANCELLED )
	{
		if( m_pScanCtx->m_Settings->m_nScanAction == SCAN_ACTION_POSTPONE )
			nReason = NCREASON_POSTPONED;

//		nMsg = pmc_PROTECTION_EVENTS_NOTIFY;
	}

	SendReport(nMsg, OBJSTATUS_NOTDISINFECTED, nReason);
	IncrementStatistic(STATISTIC_OFFSET(m_nNumAbandoned));
	return errOK;
}

tERROR CObjectContext::ProcessDisinfectRequest(enNotCuredReason nNonCuredReason, enActions* nRetAction)
{
	enScanAction nScanAction = m_pScanCtx->m_Settings->m_nScanAction;
	if( nScanAction == SCAN_ACTION_DETECT || nScanAction == SCAN_ACTION_REPORT || m_fStreamProcessing )
		return errENGINE_ACTION_SKIP;

	if( !m_InfCtx && !InitInfectedInfo() )
		return errENGINE_ACTION_SKIP;

	if( m_InfCtx->m_nDetectStatus != DSTATUS_SURE )
	{
		if( m_pScanCtx->m_fQuarantineProcessing && !(m_pScanCtx->m_Settings->m_nAskActions & ACTION_QUARANTINE) )
			return errENGINE_ACTION_SKIP;
	}

	CObjectContext* pArchiveObj = NULL, *pActionObj = this;
	enNotCuredReason  nReason;
	hOBJECT hDeletable = NULL;

	int  nActionMask = 0, nAction = 0;
	bool bByDefault = true, bTryAgain = false;
	bool bWriteAccess, bLocked, bDisinfectable, bQuarantinable;
	bool bNonCurable = nNonCuredReason != NCREASON_NONE;

lbSetWriteAccess:
	bWriteAccess = PR_SUCC(m_pScanCtx->m_Engine->SetWriteAccess(*this, hDeletable));
	bDisinfectable = bLocked = false;
	bQuarantinable = !m_pScanCtx->m_fQuarantineProcessing && m_InfCtx->m_nDetectStatus != DSTATUS_SURE;

	TraceOut("*** disinfect request");
	nReason = GetNotProcessingReason();

	if( nNonCuredReason == NCREASON_OUTOFSPACE )
		nReason = NCREASON_OUTOFSPACE;

	CObjectContext* pNative = m_pScanCtx->m_pNative;
	if( pNative )
	{
		TraceOut("*** get native error...");
		enNotCuredReason nTopReason = pNative->GetNotProcessingReason();

		switch( nTopReason )
		{
		case NCREASON_WRITEPROTECT:
			if( !hDeletable )
			{
				nReason = NCREASON_WRITEPROTECT;
				hDeletable = m_pObject;
			}
			break;

		case NCREASON_LOCKED:
			if( !pNative->InitCureOnReboot() )
			{
				nReason = NCREASON_LOCKED;
				bDisinfectable = true;
			}
			else
			{
				cInfectedObjectInfo* pInfInfo = pNative->m_InfCtx;
				AVSImpl* pAVS = m_pScanCtx->m_Session->m_pAVS;
				if( !bTryAgain && pAVS->IsLockedObjectProcessed(pInfInfo->m_qwUniqueId) )
				{
					pAVS->LockedObjectProcessed(m_pScanCtx->m_Session, pInfInfo, OBJSTATUS_UNKNOWN);
					return errENGINE_ACTION_SKIP;
				}

				nReason = NCREASON_NONE;
			}

			if( !pNative->m_fArchive || m_fExternalDetectProcessing )
			{
				nActionMask |= ACTION_DELETE;

				if( bQuarantinable )
					nActionMask |= ACTION_QUARANTINE;
			}

			bLocked = true;
			hDeletable = NULL;
			pActionObj = pNative;
			break;

		case NCREASON_NORIGHTS:
		case NCREASON_READERROR:
		case NCREASON_WRITEERROR:
		case NCREASON_DEVICENOTREADY:
		case NCREASON_OBJECTNOTFOUND:
			nReason = nTopReason;
			hDeletable = NULL;
			break;
		}
	}

	switch( m_nObjectType )
	{
	case OBJTYPE_AVP3DOSMEMORY:
		bByDefault = false;
		nActionMask |= ACTION_DISINFECT;
		break;

	case OBJTYPE_MEMORYPROCESS:
	case OBJTYPE_MEMORYPROCESSMODULE:
		bByDefault = false;
		{
			tDWORD dwSize = sizeof(tDWORD);
			tDWORD dwPid = m_pObject->propGetDWord(plIO_PID);

			PR_TRACE((g_root, prtIMPORTANT, "avs\tmemory module %S(%d) is infected",
				m_strObjectName.data(), dwPid));

			if( hDeletable && m_pObject->sysSendMsg(pmc_REMOTE_GLOBAL, pm_AVS_VERIFY_PID_TO_KILL,
					NULL, &dwPid, &dwSize) != errOK_DECIDED )
				nActionMask |= ACTION_DELETE;
		}
		break;

	case OBJTYPE_MAILMSGMIME:
	case OBJTYPE_MAILMSGREF:
		if( bNonCurable )
		{
			if( nReason == NCREASON_WRITEERROR || nReason == NCREASON_NORIGHTS )
				nActionMask |= ACTION_DELETE;
			bByDefault = false;
		}
		break;

	case OBJTYPE_LOGICALDRIVE:
		if( nReason == NCREASON_WRITEERROR )
		{
			tDWORD nMediaType = m_pObject->propGetDWord(plMediaType);
			if( nMediaType == cMEDIA_TYPE_REMOVABLE )
			{
				tDWORD nDeviceType = m_pObject->propGetDWord(plDriveType);
				if( nDeviceType == cFILE_DEVICE_DISK )
					nReason = NCREASON_WRITEPROTECT;
			}
			else if( nMediaType && nMediaType != cMEDIA_TYPE_FIXED )
				nReason = NCREASON_WRITEPROTECT;
		}
		if( bNonCurable )
			bByDefault = false;
		break;
	}

	if( nReason == NCREASON_WRITEPROTECT )
	{
		if( bNonCurable )
			hDeletable = NULL;
		bDisinfectable = true;
	}

	if( bByDefault )
	{
		if( !bNonCurable && m_InfCtx->m_nDetectStatus == DSTATUS_SURE && (bWriteAccess || bDisinfectable || bLocked) )
		{
			if( GetEngine()->GetDisinfectability(*this) )
				nActionMask |= ACTION_DISINFECT;
		}

		if( !bLocked )
		{
			hOBJECT hActionObj = m_pObject;
			hOBJECT hExecutable = m_pObject->propGetObj(m_propExecutableParent);

			if( hDeletable )
			{
				hExecutable = hDeletable->propGetObj(m_propExecutableParent);

				if( (m_fArchive || hDeletable != m_pObject) && !hExecutable )
				{
					pArchiveObj = (CObjectContext*)hDeletable->propGetPtr(m_propContext);
					nActionMask |= ACTION_DELETE_ARCHIVE;
					if( pArchiveObj->m_fActionProcessed )
						return errENGINE_ACTION_SKIP;
				}
				else
				{
					if( bQuarantinable )
						nActionMask |= ACTION_QUARANTINE;

					if( m_pScanCtx->m_fQuarantineProcessing && m_InfCtx->m_nDetectStatus != DSTATUS_SURE &&
							m_pScanCtx->m_Settings->m_nScanAction == SCAN_ACTION_DISINFECT )
						nReason = NCREASON_REPONLY;
					else
						nActionMask |= ACTION_DELETE;

					hActionObj = hDeletable;
				}
			}

			if( hExecutable )
				hActionObj = hExecutable;

			pActionObj = (CObjectContext*)hActionObj->propGetPtr(m_propContext);
		}
	}

	if( pActionObj->m_fDisinfectProcessed && (nActionMask & ACTION_DISINFECT) )
		return errOK_DECIDED;

	if( pActionObj->m_fActionProcessed )
		return errENGINE_ACTION_SKIP;

	if( nReason == NCREASON_NONE && !(nActionMask & ACTION_DISINFECT) )
		nReason = NCREASON_NONCURABLE;

	if( bNonCurable )
		SendReport(pmc_PROTECTION_EVENTS_CRITICAL, OBJSTATUS_NOTDISINFECTED, nReason);

	if( pArchiveObj )
		m_InfCtx->m_strArchiveObject = pArchiveObj->m_strObjectName;

	PR_TRACE((g_root, prtIMPORTANT, "avs\t%S, reason(%d) - selecting action...",
		m_strObjectName.data(), nReason));

	if( !bTryAgain && !pActionObj->ChooseAction(nActionMask, nReason, nAction) || (bDisinfectable && bTryAgain) )
	{
		cAskObjectAction askAction;
		pActionObj->InitAskAction(askAction, nActionMask);

		nAction = m_pScanCtx->AskAction(cAskObjectAction::DISINFECT, askAction, fACCESS_NONE, !bDisinfectable);

		if( bDisinfectable && !(nAction & ACTION_CANCEL) )
		{
			bTryAgain = true;
			goto lbSetWriteAccess;
		}
	}

	PR_TRACE((g_root, prtIMPORTANT, "avs\t%S - selected action(%d)", m_strObjectName.data(), nAction));

	bool bReportOnlyUserSettings = 
		!m_pScanCtx->m_Settings->m_bTryDelete && !m_pScanCtx->m_Settings->m_bTryDisinfect;
	if (nAction == ACTION_REPORTONLY && (bReportOnlyUserSettings || nReason == NCREASON_NONE))
		nReason = NCREASON_REPONLY;

	if( bDisinfectable )
		nAction = ACTION_REPORTONLY;

	if( nAction == ACTION_DELETE_ARCHIVE )
	{
		pActionObj = pArchiveObj;
		nAction = ACTION_DELETE;
	}

	if( nRetAction )
		*nRetAction = (enActions)nAction;

	switch( nAction )
	{
	case ACTION_DELETE:

		if( !m_fExternalDetectProcessing && m_pScanCtx->CheckActiveDetection(*this) )
			return errENGINE_ACTION_SKIP;

		if( m_pScanCtx->m_nReason != NCREASON_NONE )
			ProcessDeleteOnCloseFailed();
		else
		{
			if( PR_FAIL(pActionObj->PlaceInBackup()) )
				return errENGINE_ACTION_SKIP;

			if( !bLocked )
				pActionObj->MarkDeleteOnClose();
			else
				pActionObj->DeleteOnReboot();
		}
		break;

	case ACTION_DISINFECT:
		if( m_pScanCtx->m_nReason != NCREASON_NONE && !bNonCurable )
			ProcessDisinfectImpossible();
		else
		{
			if( PR_FAIL(pActionObj->PlaceInBackup()) )
				return errENGINE_ACTION_SKIP;

			if( !bLocked )
			{
				pActionObj->m_fDisinfectProcessed = 1;
				return errOK_DECIDED;
			}
			else
				pActionObj->CureOnReboot();
		}
		break;

	case ACTION_QUARANTINE:
		if( PR_FAIL(pActionObj->PlaceInBackup(true)) )
			return errENGINE_ACTION_SKIP;

		if( PR_FAIL(pActionObj->PlaceInQuarantine()) )
			return errENGINE_ACTION_SKIP;

		if( bLocked )
			pActionObj->DeleteOnReboot(true);
		break;

	case ACTION_CANCEL:
		ProcessNonCurable(NCREASON_CANCELLED);
		break;

	default:
		ProcessNonCurable(nReason);
	}

	pActionObj->m_fActionProcessed = 1;
	return errENGINE_ACTION_SKIP;
}

tERROR CObjectContext::ProcessSectorOverwriteRequest()
{
	if( m_nObjectType == OBJTYPE_LOGICALDRIVE )
	{
		if(m_pObject->propGetBool(plIsPartition))
		{
			ProcessNonCurable(NCREASON_NONOVERWRITABLE);
			return errENGINE_ACTION_SKIP;
		}
	}
	return errOK_DECIDED;
}

tERROR CObjectContext::ProcessSetAttributesFailed()
{
//	SendReport(pmc_PROTECTION_EVENTS_SPAM, OBJSTATUS_CANTCHANGEATTR, GetNotProcessingReason());
	return errOK;
}

tERROR CObjectContext::ProcessDeleteOnCloseFailed()
{
	TraceOut("*** delete failed");

	if( m_fNative )
		m_pScanCtx->m_pNative = this;

	enNotCuredReason nReason = GetNotProcessingReason();

	int nAction, nActionMask;
	enObjectStatus nStatus = m_fQuarantined ? OBJSTATUS_NOTQUARANTINED : OBJSTATUS_CANTBEDELETED;

DelAgain :
	SendReport(pmc_PROTECTION_EVENTS_CRITICAL, nStatus, nReason);

	if( m_nObjectType == OBJTYPE_MAILMSGATTACH && m_nDetectType == DETYPE_UNKNOWN &&
		m_nDetectStatus == DSTATUS_UNKNOWN && m_nDetectDanger == DETDANGER_UNKNOWN )
	{
		return errOK;
	}

	nActionMask = 0;
	if( nReason == NCREASON_LOCKED && InitCureOnReboot() )
		nActionMask |= ACTION_DELETE;

	if( !ChooseAction(nActionMask, nReason, nAction) )
	{
		cAskObjectAction askAction;
		InitAskAction(askAction, nActionMask);

		if( m_fNative && !m_fArchive )
			askAction.m_nActionsMask |= ACTION_TRYAGAIN;

		if( nAction != ACTION_DELETE )
			nAction = m_pScanCtx->AskAction(cAskObjectAction::DISINFECT, askAction);
	}

	tERROR nError = errUNEXPECTED;
	switch( nAction )
	{
	case ACTION_TRYAGAIN:
		if( PR_FAIL(nError = DeleteTryAgain()) )
			goto DelAgain;
		break;

	case ACTION_DELETE:
		nError = DeleteOnReboot();
		break;
	}

	if( PR_FAIL(nError) )
		IncrementStatistic(STATISTIC_OFFSET(m_nNumAbandoned));

	m_pScanCtx->m_pNative = NULL;
	UpdateDetectedInfo();
	return errOK;
}

tERROR CObjectContext::ProcessDeleteOnClose()
{
	TraceOut("*** deleted");

	enObjectStatus nStatus;
	if( m_fQuarantined )
		IncrementStatistic(STATISTIC_OFFSET(m_nNumQuarantined)), nStatus = OBJSTATUS_QUARANTINED;
	else
		IncrementStatistic(STATISTIC_OFFSET(m_nNumDeleted)), nStatus = OBJSTATUS_DELETED;

	int nDesc = 0;
	if( GetNotProcessingReason() == NCREASON_OUTOFSPACE )
		nDesc = NCREASON_OUTOFSPACE;

	if( !m_pScanCtx->m_fLockedProcessing || m_pPrev )
		SendReport(pmc_PROTECTION_EVENTS_FIX, nStatus, nDesc);

	UpdateDetectedInfo(true);
	return errOK;
}

tERROR CObjectContext::ProcessCreateFailed()
{
	tERROR nNativeError = m_pObject->propGetDWord(pgNATIVE_ERR);
	if( nNativeError == errACCESS_DENIED )
		SendReport(pmc_PROTECTION_EVENTS_SPAM, OBJSTATUS_NOTPROCESSED, NPREASON_NORIGHTS);
	return errOK;
}

tERROR CObjectContext::ProcessPasswordRequest()
{
	PR_TRACE((m_pObject, prtIMPORTANT, "avs\tProcessPasswordRequest enter"));

	cStrObj sPswd;
	if( m_pScanCtx->GetPassword(sPswd, &m_nPswdPos) )
	{
		sPswd.copy((hSTRING)m_pContext);
		return errOK_DECIDED;
	}

	if( !m_pScanCtx->m_Settings->m_bAskPassword || m_pScanCtx->m_fPasswordSkipped )
		return errOPERATION_CANCELED;

	if( !m_pContext )
		return errPARAMETER_INVALID;

	CObjectContext* pHashCtx = this;
	while( !pHashCtx->m_fProcessing && pHashCtx->m_pPrev )
		pHashCtx = pHashCtx->m_pPrev;

	if( !m_fProcessing )
	{
		ProcessScanBegin();
		m_fProcessing = 0;
	}

	cAskObjectPassword askAction;
	*(cObjectInfo*)&askAction = *this;
	askAction.m_nActionsMask = ACTION_OK | ACTION_CANCEL;
	askAction.m_strArchiveObject = pHashCtx->m_strObjectName;
	pHashCtx->InitSimpleObjectId(askAction.m_qwUniqueId);

	PR_TRACE((m_pObject, prtIMPORTANT, "avs\tProcessPasswordRequest ask password for %S", m_strObjectName.data()));

	enActions nAction = m_pScanCtx->AskPassword(askAction);

	if( nAction != ACTION_OK )
	{
		m_pScanCtx->m_fPasswordSkipped = 1;
		return errOPERATION_CANCELED;
	}

	hSTRING hPswdStr = (hSTRING)m_pContext;

	if( askAction.m_strPassword == hPswdStr )
		return errOPERATION_CANCELED;

	askAction.m_strPassword.copy(hPswdStr);
	return errOK_DECIDED;
}

// -------------------------------------------

tPROPID CObjectContext::m_propContext;
tPROPID	CObjectContext::m_propActionClassMask;
tPROPID	CObjectContext::m_propVirtualName;
tPROPID	CObjectContext::m_propInfectedCounter;
tPROPID	CObjectContext::m_propReadonlyError;
tPROPID	CObjectContext::m_propReadonlyObject;
tPROPID	CObjectContext::m_propSessionObject;
tPROPID	CObjectContext::m_propSkipThisOne;
tPROPID	CObjectContext::m_propExecutableParent;
tPROPID	CObjectContext::m_propSetWriteAccess;
tPROPID	CObjectContext::m_propTreatUserData;
tPROPID	CObjectContext::m_propIntegralParent;
tPROPID	CObjectContext::m_propIStreamsCtx;
tPROPID	CObjectContext::m_propBackupFlag;
tPROPID	CObjectContext::m_propSpecialCure;

tERROR CObjectContext::InitCustomProps(hROOT root)
{
	tERROR error = root->RegisterCustomPropId(&m_propContext, "npSCAN_OBJECT_CONTEXT", pTYPE_PTR | pCUSTOM_HERITABLE);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propActionClassMask, npENGINE_OBJECT_PARAM_ACTION_CLASS_MASK_tDWORD, pTYPE_DWORD | pCUSTOM_HERITABLE);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propVirtualName, npENGINE_VIRTUAL_OBJECT_NAME, pTYPE_STRING);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propInfectedCounter, npENGINE_OBJECT_DETECT_STATE_tDWORD, pTYPE_DWORD);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propReadonlyError, npENGINE_OBJECT_READONLY_tERROR, pTYPE_ERROR | pCUSTOM_HERITABLE);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propReadonlyObject, npENGINE_OBJECT_READONLY_hOBJECT, pTYPE_OBJECT | pCUSTOM_HERITABLE);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propSessionObject, npENGINE_OBJECT_SESSION_hOBJECT, pTYPE_OBJECT | pCUSTOM_HERITABLE);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propSkipThisOne, npENGINE_OBJECT_SKIP_THIS_ONE_tBOOL, pTYPE_BOOL);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propExecutableParent, npENGINE_OBJECT_EXECUTABLE_PARENT_IO_hOBJECT, pTYPE_OBJECT | pCUSTOM_HERITABLE);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propSetWriteAccess, npENGINE_OBJECT_SET_WRITE_ACCESS_tERROR, pTYPE_ERROR);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propTreatUserData, "propid_reopen_user_data", pTYPE_DWORD| pCUSTOM_HERITABLE);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propIntegralParent, npENGINE_INTEGRAL_PARENT_IO_hOBJECT, pTYPE_OBJECT);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propIStreamsCtx, "propid_istreams_ctx", pTYPE_BINARY);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propBackupFlag, "npSCAN_OBJECT_BCKFLAG", pTYPE_BOOL);
	if( PR_SUCC(error) ) error = root->RegisterCustomPropId(&m_propSpecialCure, "avp1_has_special_cure", pTYPE_BOOL |pCUSTOM_HERITABLE );
	return error;
}

// -------------------------------------------

// -------------------------------------------
#include "bl.h"
#include <NetDetails/plugin/p_netdetails.h>
#include <NetDetails/structs/s_netdetails.h>
#include "..\..\Components\Updater\KeepUp2Date\Client\helper\localize.h"
	
// -------------------------------------------
// -------------------------------------------

struct CObjectConverter
{
	CObjectConverter(cBLNotification& _pEvent) :
		pEvent(_pEvent), nEvtDbType(dbRTP),
		pObjectName(NULL),
		nObjectNameSize(0),
		sDetectName(NULL),
		pEventId(NULL),
		nReportTray(0)
	{}

	BlImpl*          bl;
	eReportDatabases nEvtDbType;
	cBLNotification& pEvent;
	enNotification   nNotification;
	tDWORD           nReportTray;
	tDWORD           nMsgCls;
	tDWORD           nMsgId;

	tPTR             pObjectName;
	tDWORD           nObjectNameSize;
	
	cStrObj*         sDetectName;

	cStrObj          m_strTmp1;
	cStrObj          m_strTmp2;
	cStrObj          m_strTmp3;
	tQWORD*          pEventId;
	cChunkBuff       pBuff;

	static bool serialize(const cSer * ser, tDWORD ser_id, cMemChunk& buf)
	{
		buf.reserve(buf.size(), true);
		
		tDWORD nSize = 0;
		tERROR err = ::g_root->StreamSerialize(ser, ser_id, (tBYTE *)buf.ptr(), buf.used(), &nSize);
		if( err == errBUFFER_TOO_SMALL )
		{
			buf.reserve(nSize, true);
			if( PR_FAIL(::g_root->StreamSerialize(ser, ser_id, (tBYTE *)buf.ptr(), buf.used(), &nSize)) )
				buf.m_used = 0;
			else
				buf.m_used = nSize;
		}
		else if( PR_FAIL(err) )
			buf.m_used = 0;
		else
			buf.m_used = nSize;

		return !!buf.used();
	}
	
	enDecisionReason avsDescr2reason(long nDescription)
	{
		switch( nDescription )
		{
		case NCREASON_NONCURABLE        : return eNONCURABLE;
		case NCREASON_LOCKED            : return eLOCKED;
		case NCREASON_REPONLY           : return eREPORTONLY;
		case NCREASON_NORIGHTS          : return eNORIGHTS;
		case NCREASON_CANCELLED         : return eUSER;
		case NCREASON_WRITEPROTECT      : return eWRITEPROTECT;
		case NCREASON_TASKSTOPPED       : return eTASKSTOPPED;
		case NCREASON_POSTPONED         : return ePOSTPONED;
		case NCREASON_NONOVERWRITABLE   : return eNONOVERWRITABLE;
		case NCREASON_COPYFAILED        : return eCOPYFAILED;
		case NCREASON_WRITEERROR        : return eWRITEERROR;
		case NCREASON_OUTOFSPACE        : return eOUTOFSPACE;
		case NCREASON_READERROR         : return eREADERROR;
		case NCREASON_DEVICENOTREADY    : return eDEVICENOTREADY;
		case NCREASON_OBJECTNOTFOUND    : return eNOTFOUND;
		case NCREASON_WRITENOTSUPPORTED : return eWRITENOTSUPPORTED;
		case NCREASON_CANNOTBACKUP      : return eCANNOTBACKUP;
		}

		return (enDecisionReason)0;
	}

	enDecisionReason avsDescr2reason2(long nDescription)
	{
		switch( nDescription )
		{
		case NPREASON_SIZE:     return eSIZE;
		case NPREASON_TYPE:     return eTYPE;
		case NPREASON_MASKS:    return eEXCLUDE;
		case NPREASON_TIME:     return eTIME;
		case NPREASON_NORIGHTS: return eNORIGHTS;
		case NPREASON_NOTFOUND: return eNOTFOUND;
		case NPREASON_SKIPPED:  return eUSER;
		case NPREASON_LOCKED:   return eLOCKED;
		}
		return (enDecisionReason)0;
	}

	enRDetectType avsDetType2RDetType(enDetectType nDetectType)
	{
		switch( nDetectType )
		{
		case DETYPE_VIRWARE:          return eVirware;
		case DETYPE_TROJWARE:         return eTrojware;
		case DETYPE_MALWARE:          return eMalware;
		case DETYPE_ADWARE:           return eAdware;
		case DETYPE_PORNWARE:         return ePornware;
		case DETYPE_RISKWARE:         return eRiskware;
		case DETYPE_XFILES:           return eXFiles;
		case DETYPE_SOFTWARE:         return eSoftware;
		case DETYPE_ATTACK:           return eAttack;
		case DETYPE_REGISTRY:         return eRegistryAccess;
		case DETYPE_SUSPICACTION:     return eSuspicionActivity;
		case DETYPE_VULNERABILITY:    return eVulnerability;
		case DETYPE_PHISHING:         return ePhishing;
		}

		return (enRDetectType)0;
	}
		
	enObjType avsObjectType2ObjType(enObjectType nObjectType)
	{
		switch( nObjectType )
		{
		case OBJTYPE_LOGICALDRIVE:        return eLogSector;
		case OBJTYPE_PHYSICALDISK:        return ePhysSector;
		case OBJTYPE_MEMORYPROCESS:       return eProcess;
		case OBJTYPE_MEMORYPROCESSMODULE: return eModule;
		case OBJTYPE_MAILMSGBODY:
		case OBJTYPE_MAILMSGMIME:
		case OBJTYPE_MAILMSGREF:          return eMailMessage;
		case OBJTYPE_MAILMSGATTACH:       return eMailAttach;
		case OBJTYPE_FOLDER_OBJECT:       return eDirectory;
		case OBJTYPE_NETWORK:             return ePacket;
		}
		
		return eFile;
	}

	inline enVerdict avsObjectStatus2Verdict(enObjectStatus nObjectStatus)
	{
		return (enVerdict)nObjectStatus;
	}

	void TaskType2TaskId(cStrObj& strTaskType, tTaskId nTaskId = TASKID_UNK)
	{
		// eTASK_GROUP_PROTECTION
		if( strTaskType == AVP_TASK_SELFPROT )
		{
			pEvent.m_TaskGroup = eTASK_GROUP_PROTECTION;
			pEvent.m_TaskID = eTASK_SELFPROT;
		}
		
		// eTASK_GROUP_HIPS
		else if( strTaskType == AVP_TASK_HIPS )
		{
			pEvent.m_TaskGroup = eTASK_GROUP_HIPS;
			pEvent.m_TaskID = eTASK_HIPS;
		}
		else if( strTaskType == AVP_TASK_SYSTEM_WATCHER)
		{
			pEvent.m_TaskGroup = eTASK_GROUP_HIPS;
			pEvent.m_TaskID = eTASK_SW;
		}
		else if( strTaskType == AVP_TASK_PDM)
		{
			pEvent.m_TaskGroup = eTASK_GROUP_HIPS;
			pEvent.m_TaskID = eTASK_PDM;
		}
		
		// eTASK_GROUP_AV
		else if( strTaskType == AVP_TASK_ONACCESSSCANNER )
		{
			pEvent.m_TaskGroup = eTASK_GROUP_AV;
			pEvent.m_TaskID = eTASK_FAV;
		}
		else if( strTaskType == AVP_TASK_MAILCHECKER )
		{
			pEvent.m_TaskGroup = eTASK_GROUP_AV;
			pEvent.m_TaskID = eTASK_MAV;
		}
		else if( strTaskType == AVP_TASK_WEBMONITOR ||
			strTaskType == AVP_TASK_SCRIPTCHECKER ||
			strTaskType == AVP_PROFILE_HTTPSCAN ||
			strTaskType == AVP_TASK_WEBMON_URLFLT )
		{
			pEvent.m_TaskGroup = eTASK_GROUP_AV;
			pEvent.m_TaskID = eTASK_HTTP;
		}
		
		// eTASK_GROUP_OS
		else if( strTaskType == AVP_TASK_ANTIDIAL )
		{
			pEvent.m_TaskGroup = eTASK_GROUP_OS;
			pEvent.m_TaskID = eTASK_AD;
		}
		else if( strTaskType == AVP_TASK_ANTIPHISHING )
		{
			pEvent.m_TaskGroup = eTASK_GROUP_OS;
			pEvent.m_TaskID = eTASK_AP;
		}
		else if( strTaskType == AVP_TASK_INTRUSIONDETECTOR )
		{
			pEvent.m_TaskGroup = eTASK_GROUP_OS;
			pEvent.m_TaskID = eTASK_IDS;
		}
		
		// eTASK_GROUP_UC
		else if( strTaskType == AVP_TASK_ANTIBANNER )
		{
			pEvent.m_TaskGroup = eTASK_GROUP_UC;
			pEvent.m_TaskID = eTASK_AB;
		}
		else if( strTaskType == AVP_TASK_ANTISPAM )
		{
			pEvent.m_TaskGroup = eTASK_GROUP_UC;
			pEvent.m_TaskID = eTASK_AS;
		}
		else if( strTaskType == AVP_TASK_PARENTALCONTROL )
		{
			pEvent.m_TaskGroup = eTASK_GROUP_UC;
			pEvent.m_TaskID = eTASK_PC;
		}
		
		// eTASK_GROUP_SCAN
		else if( strTaskType == AVP_TASK_ONDEMANDSCANNER )
		{
			nEvtDbType = dbScan;

			pEvent.m_TaskGroup = eTASK_GROUP_SCAN;
			pEvent.m_TaskID = eTASK_SCAN;
			pEvent.m_ExtraInfoSub1 = nTaskId;
		}
		
		// eTASK_GROUP_UPDATER
		else if( strTaskType == AVP_TASK_UPDATER )
		{
			nEvtDbType = dbUpdate;

			pEvent.m_TaskGroup = eTASK_GROUP_UPDATER;
			pEvent.m_TaskID = eTASK_UPDATER;
			pEvent.m_ExtraInfoSub1 = nTaskId;
		}
		else if( strTaskType == AVP_TASK_ROLLBACK )
		{
			nEvtDbType = dbUpdate;

			pEvent.m_TaskGroup = eTASK_GROUP_UPDATER;
			pEvent.m_TaskID = eTASK_ROLLBACK;
			pEvent.m_ExtraInfoSub1 = nTaskId;
		}
	}
	
	void convert(cDetectObjectInfo* pData)
	{
		pEvent.m_Verdict       = avsObjectStatus2Verdict(pData->m_nObjectStatus);
		pEvent.m_DetectType    = avsDetType2RDetType(pData->m_nDetectType);
		pEvent.m_ObjectType    = avsObjectType2ObjType(pData->m_nObjectType);
		pEvent.m_DetectDanger  = pData->m_nDetectDanger;
		pEvent.m_IsExact       = pData->m_nDetectStatus;
		pEvent.m_PID           = pData->m_nPID;
		pEvent.m_ExtraInfoSub2 = pData->m_tmTimeDelta;

		pObjectName = (tPTR)pData->m_strObjectName.data();
		nObjectNameSize = pData->m_strObjectName.size() * sizeof(tWCHAR);
		
		sDetectName = &pData->m_strDetectName;

		nNotification = (pData->m_nDetectDanger == DETDANGER_HIGH) ?
			eNtfInfected : eNtfRiskwareDetected;

		switch( pData->m_nObjectStatus )
		{
		case OBJSTATUS_OK:
			switch( pData->m_nDescription )
			{
			case OKREASON_ISFDB      : pEvent.m_DecisionReason = eSFDB; break;
			case OKREASON_ISTREAMS   : pEvent.m_DecisionReason = eISWIFT; break;
			}
			nNotification = eNtfNone;
			break;
		case OBJSTATUS_SUSPICION:
			nNotification = eNtfSuspicious;
		case OBJSTATUS_INFECTED:
			pEvent.m_Verdict = eDETECTED;
			switch( pData->m_nDescription )
			{
			case DTREASON_BYHASH        : pEvent.m_DecisionReason = eDETECT_BYHASH; break;
			case DTREASON_INFORMATION   : pEvent.m_DecisionReason = eDETECT_INFORMATION; break;
			}

			bl->m_ChartStatistics.on_detected(pData);
			break;
		case OBJSTATUS_PASSWORD_PROTECTED:
			nNotification = eNtfPasswordProtected;
			break;
		case OBJSTATUS_DELETED:
			nNotification = eNtfInfectedDeleted;
			break;
		case OBJSTATUS_QUARANTINED:
			nNotification = eNtfQuarantined;
			break;
		case OBJSTATUS_NOTPROCESSED:
			nNotification = eNtfNotImportant;
			pEvent.m_DecisionReason = avsDescr2reason2(pData->m_nDescription);
			break;
		case OBJSTATUS_NOTDISINFECTED:
			nNotification = eNtfDisinfectionImpossible;
			pEvent.m_DecisionReason = avsDescr2reason(pData->m_nDescription);
			break;
		case OBJSTATUS_DISINFECTED:
		case OBJSTATUS_OVERWRITED:
		case OBJSTATUS_RENAMED:
		case OBJSTATUS_RESTORED:
		case OBJSTATUS_ADDEDTOEXCLUDE:
			nNotification = eNtfInfectedDisinfected;
			break;
		case OBJSTATUS_DENIED:
			nNotification = eNtfSuspicious;
			break;
		case OBJSTATUS_DELETED_ON_REBOOT:
		case OBJSTATUS_DISINFECTED_ON_REBOOT:
		case OBJSTATUS_QUARANTINED_ON_REBOOT:
			nNotification = eNtfInfectedDisinfected;
			bl->m_State.m_ThreatsState |= eThreatsNeedReboot;
			break;
		case OBJSTATUS_ALLOWED:
			nNotification = eNtfOtherImportantEvents;
			break;
/*		case OBJSTATUS_CURED:
		case OBJSTATUS_PROCESSING_ERROR:
		nNotification = eNtfNone;
		break;
*/		}

		if( pData->m_strTaskType == AVP_PROFILE_HTTPSCAN ||
			pData->m_strTaskType == AVP_TASK_WEBMON_URLFLT )
		{
			pEvent.m_ObjectType = eURL;
		}
		else if( pData->m_strTaskType == AVP_TASK_SCRIPTCHECKER )
		{
			pEvent.m_ObjectType = eScript;
		}
		else if( pData->m_strTaskType == AVP_TASK_ANTIBANNER )
		{
			if( pData->m_nObjectStatus == OBJSTATUS_CORRUPTED )
				nNotification = eNtfDatabaseCorrupted;
			else
				pEvent.m_ObjectType = eURL;
		}
		else if( pData->m_strTaskType == AVP_TASK_ANTIPHISHING )
		{
			if( pData->m_nObjectStatus == OBJSTATUS_CORRUPTED )
				nNotification = eNtfDatabaseCorrupted;
			else
				pEvent.m_ObjectType = eURL;
		}
		else if( pData->m_strTaskType == AVP_TASK_ANTIDIAL )
		{
			pEvent.m_ObjectType = eDialStr;
		}
	}

	void convert(cIDSEventReport* pData)
	{
		nNotification = eNtfAttack;

		pEvent.m_DetectType = avsDetType2RDetType(pData->m_DetectType);
		pEvent.m_ObjectType = avsObjectType2ObjType(pData->m_ObjectType);
		pEvent.m_Verdict = avsObjectStatus2Verdict(pData->m_ObjectStatus);
		
		sDetectName = &pData->m_AttackDescription;

		cReportAttackInfo _info;
		_info.m_Proto         = pData->m_Proto;
		_info.m_LocalPort     = pData->m_LocalPort;
		_info.m_ResponseFlags = pData->m_ResponseFlags;
		_info.m_AttackerIP    = pData->m_AttackerIP;

		if( serialize(&_info, cReportAttackInfo::eIID, pBuff) )
		{
			pObjectName = (tPTR)pBuff.ptr();
			nObjectNameSize = pBuff.used();
		}
	}

	// void convert(cPrivacyControlAskObjectAction* pData)
	// {
	// 	if( nMsgId == cPrivacyControlAskObjectAction::TERMINATE_FAILED ||
	// 		nMsgId == cPrivacyControlAskObjectAction::QUARANTINE_FAILED)
	// 	{
	// 		bl->m_tm->AskAction(0, nMsgId, pData);
	// 		nReportTray = eReportTrayNo;
	// 	}
	// 
	// 	switch(pData->m_ReportEvent)
	// 	{
	// 	case cPrivacyControlAskObjectAction::REPORT_EVENT_ALLOW:
	// 		nNotification = eNtfInfectedDisinfected;
	// 		nReportTray = eReportTrayNo;
	// 		break;
	// 	case cPrivacyControlAskObjectAction::REPORT_EVENT_QUARANTINE_RESULT:
	// 		nNotification = (pData->m_ReportStatus == cPrivacyControlAskObjectAction::REPORT_STATUS_CRITICAL)
	// 			? eNtfDisinfectionImpossible : eNtfQuarantined;
	// 		break;
	// 	case cPrivacyControlAskObjectAction::REPORT_EVENT_DENY:
	// 		nNotification = eNtfInfectedDisinfected;
	// 		break;
	// 	case cPrivacyControlAskObjectAction::REPORT_EVENT_PROCESS_TERMINATED:
	// 		nNotification = (pData->m_ReportStatus == cPrivacyControlAskObjectAction::REPORT_STATUS_CRITICAL)
	// 			? eNtfDisinfectionImpossible : eNtfInfectedDisinfected;
	// 		break;
	// 	}
	// 	PR_TRACE((g_root, prtIMPORTANT, "bl\tprocess_notification: cPrivacyControlAskObjectAction"));
	// }

	// void convert(cAskObjectAction_PDM_BASE* pData)
	// {
	// 	if( nMsgId == cAskObjectAction_PDM_BASE::TERMINATE_FAILED ||
	// 		nMsgId == cAskObjectAction_PDM_BASE::ROLLBACK_RESULT ||
	// 		nMsgId == cAskObjectAction_PDM_BASE::QUARANTINE_FAILED )
	// 	{
	// 		bl->m_tm->AskAction(0/*pPdmEvt->m_nTaskId*/, nMsgId, pData);
	// 		nReportTray = eReportTrayNo;
	// 	}
	// 	
	// 	if( pData->m_ReportInfo.m_nEvent != cPdmReportInfo::ROLLBACK_RESULT &&
	// 		pData->m_ReportInfo.m_nEvent != cPdmReportInfo::TERMINATE_PROCESS &&
	// 		pData->m_ReportInfo.m_nEvent != cPdmReportInfo::QUARANTINE_RESULT )
	// 	{
	// 		nReportTray = eReportNone;
	// 		return;
	// 	}
	// 
	// 	switch( pData->m_ReportInfo.m_nEvent )
	// 	{
	// 	case cPdmReportInfo::DETECT:
	// 		if( pData->isBasedOn(cAskObjectAction_REG::eIID) )
	// 		{
	// 			nNotification = eNtfSuspicious;
	// 			nReportTray = eReportTrayNo;
	// 		}
	// 		break;
	// 	case cPdmReportInfo::QUARANTINE_RESULT:
	// 		nNotification = eNtfQuarantined;
	// 		break;
	// 	case cPdmReportInfo::ALLOW_PROCESS_ACTION:
	// 		nNotification = eNtfInfectedDisinfected;
	// 		nReportTray = eReportTrayNo;
	// 		break;
	// 	case cPdmReportInfo::ROLLBACK_RESULT:
	// 	case cPdmReportInfo::TERMINATE_PROCESS:
	// 	case cPdmReportInfo::DENY_PROCESS_ACTION:
	// 		nNotification = eNtfInfectedDisinfected;
	// 		break;
	// 	}
	// 	
	// 	PR_TRACE((g_root, prtIMPORTANT, "bl\tprocess_notification: cAskObjectAction_PDM_BASE"));
	// }

	void convert(cAskObjectAction_PDM2* pData)
	{
		switch( pData->m_Action )
		{
		case cAskObjectAction_PDM2::DETECT: // ничего не делаем, pmc_UPDATE_THREATS_LIST делает это за нас
			break;
		case cAskObjectAction_PDM2::ASK:
		case cAskObjectAction_PDM2::ROLLBACK_RESULT:
		case cAskObjectAction_PDM2::TERMINATE_INFO:
			nNotification = eNtfInfectedDisinfected;
			break;
		case cAskObjectAction_PDM2::TERMINATE_FAILED:
		case cAskObjectAction_PDM2::QUARANTINE_FAILED:
			nNotification = eNtfDisinfectionImpossible;
			break;
		}

		pEvent.m_PID = pData->m_CurrentProcess.m_ProcessId;
		pEvent.m_Action = evtUnk;
		pEvent.m_ObjectType = 0;

		PR_TRACE((g_root, prtIMPORTANT, "bl\tprocess_notification: cAskObjectAction_PDM2"));
	}

	// void convert(CAskRegVerdict* pData)
	// {
	// 	switch( pData->m_nResultAction )
	// 	{
	// 	case ACTION_REPORTONLY:
	// 		nNotification = eNtfSuspicious;
	// 		nReportTray = eReportTrayNo;
	// 		break;
	// 	case ACTION_ALLOW:
	// 		nNotification = eNtfInfectedDisinfected;
	// 		nReportTray = eReportTrayNo;
	// 		break;
	// 	case ACTION_DENY:
	// 		nNotification = eNtfInfectedDisinfected;
	// 		break;
	// 	}
	// 
	// 	PR_TRACE((g_root, prtIMPORTANT, "bl\tprocess_notification: CAskRegVerdict"));
	// }

	void convert(cUserBanItem* pData)
	{
		nNotification = eNtfBan;
	}

	void convert(cReportEventEx* pData)
	{
		convert((cReportEvent*)pData);
		pEventId = &pData->m_nEventId;
		if (!pEvent.m_ObjectID && pData->m_sObjectName.length())
		{
			pObjectName = (tPTR)pData->m_sObjectName.data();
			nObjectNameSize = pData->m_sObjectName.length() * sizeof(tWCHAR);
		}
	}

	void convert(cReportEvent* pData)
	{
		cReportEvent* _pEvent = &pEvent;
		*_pEvent = *pData;
	}

	void convert(cTaskReportEvent* pData)
	{
		convert(&pData->m_Event);
	}

	void convert(cAntispamStatisticsEx * pData)
	{
		pEvent.m_TaskGroup = eTASK_GROUP_UC;
		pEvent.m_TaskID = eTASK_AS;
		pEvent.m_ObjectType = eMailMessage;
		pEvent.m_PID = pData->m_dwPID;

		m_strTmp1 = pData->m_szLastMessageSender;
		m_strTmp1 += L": ";
		m_strTmp1 += pData->m_szLastObjectName;

		pObjectName = (tPTR)m_strTmp1.data();
		nObjectNameSize = m_strTmp1.size() * sizeof(tWCHAR);

		switch( pData->m_dwLastReason )
		{
		case asdr_NOTHING:	break;
		case asdr_Bayesian:	pEvent.m_DecisionReason = eBAYES; break;
		case asdr_GSG:		pEvent.m_DecisionReason = eGSG; break;
		case asdr_PDB:		pEvent.m_DecisionReason = ePDB; break;
		default:			pEvent.m_DecisionReason = eANTISPAM_BASE + pData->m_dwLastReason;
		}
		
		switch( pData->m_dwLastMessageCategory )
		{
		case cAntispamStatisticsEx::IsDeleted:
			pEvent.m_Verdict = eDELETED;
			break;
		
		case cAntispamStatisticsEx::IsHam:
			pEvent.m_DetectType = eHam;
			pEvent.m_Verdict = eALLOWED;
			break;
		
		case cAntispamStatisticsEx::IsProbableSpam:
			pEvent.m_DetectType = eProbableSpam;
			pEvent.m_Verdict = eDENIED;
			break;
		
		case cAntispamStatisticsEx::IsSpam:
			pEvent.m_DetectType = eSpam;
			pEvent.m_Verdict = eDENIED;
			break;
		}

		pEvent.m_dExtraInfo = pData->m_dLastResult;
	}

	void convert(cUrlFltReportEvent* pData)
	{
		nNotification = eNtfNotImportant;
		switch( nMsgCls )
		{
		case pmc_EVENTS_CRITICAL:	nNotification = eNtfParCtlCritical; break;
		case pmc_EVENTS_IMPORTANT:	nNotification = eNtfParCtlImportant; break;
		}

		switch( pData->m_nURLDetectType )
		{
		case cUrlFltReportEvent::eDtNone:      pEvent.m_DecisionReason = 0; break;
		case cUrlFltReportEvent::eDtTimeLimit: pEvent.m_DecisionReason = eTIME; break;
		case cUrlFltReportEvent::eDtWhiteList: pEvent.m_DecisionReason = eUSER_WHITE_LIST; break;
		case cUrlFltReportEvent::eDtBlackList: pEvent.m_DecisionReason = eUSER_BLACK_LIST; break;
		case cUrlFltReportEvent::eDtDatabase:  pEvent.m_DecisionReason = eDATABASE; break;
		case cUrlFltReportEvent::eDtHeuristic: pEvent.m_DecisionReason = eHEURISTIC; break;
		}

		if( pData->m_nURLDetectType != cUrlFltReportEvent::eDtNone &&
			pData->m_nURLDetectType != cUrlFltReportEvent::eDtTimeLimit )
			pEvent.m_DetectType = eUnwantedWebContent;
		
		switch( pData->m_nEvent )
		{
		case cUrlFltReportEvent::eSwitchProfile:	pEvent.m_Verdict = ePARCTL_SWITCHPROFILE; break;
		case cUrlFltReportEvent::eHeuristicStatus:	pEvent.m_Verdict = ePARCTL_HEURISTICSTATUS; break;
		}
		
		pEvent.m_PID = pData->m_nPID;
		pEvent.m_ExtraInfoSub1 = pData->m_nUfProfileId;
		pEvent.m_ExtraInfoSub2 = pData->m_ErrorCode;

		if( pData->m_ErrorCode )
			pEvent.m_DecisionReason = eERROR;
		//pData->m_strUserId

		if( pData->m_nResultAction == ACTION_DENY )
			pEvent.m_Verdict = eDENIED;
		else
			pEvent.m_Verdict = eALLOWED;
		
		pEvent.m_ObjectType = eURL;

		if( pData->m_strDetectObject.size() )
		{
			pObjectName = (tPTR)pData->m_strDetectObject.data();
			nObjectNameSize = pData->m_strDetectObject.size() * sizeof(tWCHAR);
		}
	}

	void convert(cTrafficAskAction* pData)
	{
		nNotification = eNtfProtocoller;
	}

	void convert(cUpdaterEvent* pData)
	{
		pEvent.m_Severity = eNotifySeverityNotImportant;
		switch( nMsgCls )
		{
		case pmc_UPDATER_CRITICAL_REPORT:	pEvent.m_Severity = eNotifySeverityCritical; break;
		case pmc_UPDATER_REPORT:	        pEvent.m_Severity = eNotifySeverityImportant; break;
		}

		if( (pData->m_qwEvent >> 32) == pm_REP_UPDATE_END )
			nReportTray = 0;
		else if( pData->m_errError == errTASK_ALREADY_RUNNING || (tWORD)pData->m_errError < 100 )
			pEvent.m_Verdict = eUPDATER_ERROR;
		else
		{
			pEvent.m_Verdict = eUPDATER + (tWORD)pData->m_errError;
			if( (tWORD)pData->m_errError == KLUPD::EVENT_TaskStarted )
				nReportTray = 0;
		}
		
		pEvent.m_Error = pData->m_errError;
		pEvent.m_ExtraInfoSub2 = pData->m_dwFileSize;
		
		if( pData->m_parameter2.size() )
		{
			pObjectName = (tPTR)pData->m_parameter2.data();
			nObjectNameSize = pData->m_parameter2.size() * sizeof(tWCHAR);
			pEvent.m_ObjectType = eURL;
		}
		else if( pData->m_parameter1.size() )
		{
			pObjectName = (tPTR)pData->m_parameter1.data();
			nObjectNameSize = pData->m_parameter1.size() * sizeof(tWCHAR);
			pEvent.m_ObjectType = eFile;
		}
	}

	void convert(cHipsAskObjectAction* pData)
	{
		nNotification = eNtfNotImportant;

		pEvent.m_ObjectType = pData->m_nObjectType;
		pEvent.m_DetectType = pData->m_nDetectType;
		pEvent.m_Action = pData->m_Action;

		PR_TRACE((g_root, prtIMPORTANT, "bl\tprocess_notification: cHipsAskObjectAction"));
	}

	void convert(cProtectionNotify* pData)
	{
		pEvent.m_PID = pData->m_nPID;
		pEvent.m_Verdict = eDENIED;
		pEvent.m_Action = evtModify;

		switch(pData->m_eNotifyType)
		{
		case _espAccessToProcess:
			pEvent.m_ObjectID = pData->m_nOurPID;
			pEvent.m_ObjectType = eProcess;
			pEvent.m_Action = evtOpen;
		case _espAccessToFile:
			pObjectName = (tPTR)pData->m_FileName.data();
			nObjectNameSize = pData->m_FileName.size() * sizeof(tWCHAR);
			pEvent.m_ObjectType = eFile;
			break;
		case _espAccessToReg:
			pObjectName = (tPTR)pData->m_RegKey.data();
			nObjectNameSize = pData->m_RegKey.size() * sizeof(tWCHAR);
			pEvent.m_ObjectType = eRegKey;
		}
	}

	void convert(cProfileBase* pData)
	{
		if( nNotification == eNtfRTPTaskOnOff )
		{
			nReportTray = 0;
			return;
		}

		if( !pData->isTask() )
		{
			nReportTray = 0;
			return;
		}

		enVerdict nVerdict = eUNKNOWN;
		switch( pData->m_nState )
		{
		case PROFILE_STATE_RUNNING:
			nVerdict = eTASK_STARTED;
			break;
		
		case PROFILE_STATE_STOPPED:
		case PROFILE_STATE_DISABLED:
		case PROFILE_STATE_COMPLETED:
			nVerdict = eTASK_STOPPED;
			break;
		
		case PROFILE_STATE_FAILED:
		case PROFILE_STATE_RUNNING_MALFUNCTION:
			nVerdict = eTASK_FAILED;
			break;
		}

		if( nNotification == eNtfTaskCanNotExecute || PR_FAIL((tERROR)pData->m_lastErr) )
			nVerdict = eTASK_FAILED;

		if( nVerdict == eUNKNOWN )
		{
			nReportTray = 0;
			return;
		}

		cBLNotification pEvt;
		CObjectConverter cnv(pEvt); cnv.TaskType2TaskId(pData->m_sType);
		
		if( pData->m_sType == AVP_TASK_SCRIPTCHECKER ||
			pData->m_sType == AVP_PROFILE_HTTPSCAN ||
			pData->m_sType == AVP_TASK_WEBMON_URLFLT )
			return;

		if( !pEvt.m_TaskID && pData->m_sType != AVP_PROFILE_PROTECTION )
		{
			nReportTray = 0;
			return;
		}
		
		pEvent.m_TaskGroup = pEvt.m_TaskGroup;
		pEvent.m_TaskID = pEvt.m_TaskID;
		
		pEvent.m_Severity = nVerdict == eTASK_FAILED ? eNotifySeverityCritical : eNotifySeverityNotImportant;
		pEvent.m_Verdict = nVerdict;
		pEvent.m_PID = -1;
		pEvent.m_ExtraInfoSub1 = pData->m_nRuntimeId;
		pEvent.m_ExtraInfoSub2 = pData->m_lastErr;

		if( (tERROR)pData->m_lastErr )
			pEvent.m_DecisionReason = eERROR;

		pEvent.m_ObjectType = eTask;

//		pObjectName = (tPTR)pData->m_sName.data();
//		nObjectNameSize = pData->m_sName.size() * sizeof(tWCHAR);
	}
};

// -------------------------------------------
// -------------------------------------------

void BlImpl::process_notification(enNotification nNotification, cSerializable * pInfo, tDWORD nStatusId, tDWORD nMsgCls, tDWORD nMsgId, tDWORD nTrayMode)
{
	if( !pInfo )
		return;

	{
		cTaskHeader* pData = (cTaskHeader*)pInfo;
		if( !pData->isBasedOn(cTaskHeader::eIID) )
			pData = NULL;
		
		if( pData && (pData->m_strProfile == AVP_PROFILE_AVZ_SCRIPT || pData->m_strProfile == AVP_PROFILE_SCAN_VULNERABILITIES) )
			return;
		
		PR_TRACE((g_root, prtIMPORTANT, "bl\tprocess_notification: iid = 0x%08x, type = \"%S\"", pInfo->getIID(), pData ? pData->m_strTaskType.data() : L"unk"));
	}

	if( NOTIFICATION_ID(nNotification) == eNotifyBases && m_bWaitForUpdate )
	{
		m_tm->SetProfileState(AVP_PROFILE_UPDATER, TASK_REQUEST_RUN, NULL, 0);
		return;
	}

	if( pInfo->isBasedOn(cSnifferFrameSummary::eIID) )
	{
		cSnifferFrameSummary* pSummary = (cSnifferFrameSummary*)pInfo;
		
		tSnifferFrameReport* ptEvent = (tSnifferFrameReport*)pSummary;

		m_pReport->FindObject(dbNetworkPackets, ePacket, NULL, 0, cTRUE, 0, &ptEvent->m_ObjectID);

		if( PR_SUCC(m_pReport->AddEvent(dbNetworkPackets, ptEvent, sizeof(tSnifferFrameReport), NULL)) )
		{
			tPTR pData = (tPTR)pSummary->m_Data.data();
			tDWORD dwSize = pSummary->m_Data.size();
			m_pReport->SetObjectData(ptEvent->m_ObjectID, pData, dwSize, 0);
		}

		return;
	}

	cBLNotification pEvent;

	CObjectConverter cnv(pEvent);
	cnv.bl = this;
	cnv.nNotification = nNotification;
	cnv.nReportTray = nTrayMode;
	cnv.nMsgCls = nMsgCls;
	cnv.nMsgId = nMsgId;

	if( pInfo->isBasedOn(cDetectObjectInfo::eIID) )
		cnv.convert((cDetectObjectInfo*)pInfo);

	if( pInfo->isBasedOn(cIDSEventReport::eIID) )
		cnv.convert((cIDSEventReport*)pInfo);
// 	else if( pInfo->isBasedOn(cAskObjectAction_PDM_BASE::eIID) )
// 		cnv.convert((cAskObjectAction_PDM_BASE*)pInfo);
// 	else if(pInfo->isBasedOn(cPrivacyControlAskObjectAction::eIID))
// 		cnv.convert((cPrivacyControlAskObjectAction*)pInfo);
	else if( pInfo->isBasedOn(cAskObjectAction_PDM2::eIID) )
		cnv.convert((cAskObjectAction_PDM2*)pInfo);
// 	else if( pInfo->isBasedOn(CAskRegVerdict::eIID) )
// 		cnv.convert((CAskRegVerdict*)pInfo);
	else if( pInfo->isBasedOn(cUserBanItem::eIID) )
		cnv.convert((cUserBanItem*)pInfo);
	else if( pInfo->isBasedOn(cAntispamStatisticsEx::eIID) )
		cnv.convert((cAntispamStatisticsEx*)pInfo);
	else if( pInfo->isBasedOn(cUrlFltReportEvent::eIID) )
		cnv.convert((cUrlFltReportEvent*)pInfo);
	else if( pInfo->isBasedOn(cTrafficAskAction::eIID) )
		cnv.convert((cTrafficAskAction*)pInfo);
	else if( pInfo->isBasedOn(cUpdaterEvent::eIID) )
		cnv.convert((cUpdaterEvent*)pInfo);
	else if( pInfo->isBasedOn(cHipsAskObjectAction::eIID) )
		cnv.convert((cHipsAskObjectAction*)pInfo);
	else if( pInfo->isBasedOn(cReportEventEx::eIID) )
		cnv.convert((cReportEventEx*)pInfo);
	else if( pInfo->isBasedOn(cReportEvent::eIID) )
		cnv.convert((cReportEvent*)pInfo);
	else if( pInfo->isBasedOn(cTaskReportEvent::eIID) )
		cnv.convert((cTaskReportEvent*)pInfo);
	else if( pInfo->isBasedOn(cProtectionNotify::eIID) )
		cnv.convert((cProtectionNotify*)pInfo);
	else if( pInfo->isBasedOn(cProfileBase::eIID) )
		cnv.convert((cProfileBase*)pInfo);

	if( pInfo->isBasedOn(cTaskHeader::eIID) )
	{
		cTaskHeader* pData = (cTaskHeader*)pInfo;
		cnv.TaskType2TaskId(pData->m_strTaskType, pData->m_nTaskId);
	}
	else if( nStatusId )
	{
		pEvent.m_Verdict = ePROTECTION + nStatusId;
		pEvent.m_PID = -1;

		switch( NOTIFICATION_ID(nNotification) )
		{
		case eNotifySelfProtection:			pEvent.m_TaskID = eTASK_SELFPROT; break;
		case eNotifyLicensing:				pEvent.m_TaskID = eTASK_LIC; break;
		case eNotifyUpdater:
		case eNotifyUpdateNeedReboot:		pEvent.m_TaskID = eTASK_UPDATER; break;
		case eNotifyMyComputerNotScaned:	pEvent.m_TaskID = eTASK_SCAN; break;
		}
	}

	static cReportEvent g_pNullEvt;
	if( !memcmp((tReportEvent *)&pEvent, (tReportEvent *)&g_pNullEvt, sizeof(tReportEvent)) )
		return;

	if( NOTIFICATION_SERVERITY(cnv.nNotification) != eNotifySeverityNone && cnv.nReportTray )
		cnv.nReportTray |= fReportTray;
	
	if( !cnv.nReportTray )
		return;

	pEvent.m_Severity = NOTIFICATION_SERVERITY(cnv.nNotification);
	pEvent.m_EventID = cnv.nNotification;

	if( pEvent.m_Severity == eNotifySeverityNone )
	{
		switch( nMsgCls )
		{
		case pmc_EVENTS_CRITICAL:   pEvent.m_Severity = eNotifySeverityCritical; break;
		case pmc_EVENTS_IMPORTANT:  pEvent.m_Severity = eNotifySeverityImportant; break;
		case pmc_EVENTS_NOTIFY:     pEvent.m_Severity = eNotifySeverityNotImportant; break;
		}
	}

	cBLNotifySettings * pCfg = m_settings.m_NSettings.find(
		(enNotifySeverity)pEvent.m_Severity, (enNotifyIds)NOTIFICATION_ID(pEvent.m_EventID));

	if( pCfg )
		pEvent.m_bSound = pCfg->m_bSound;
	pEvent.m_Timestamp = (tDWORD)cDateTime::now_utc();

	//  онвертаци€ в уникальный PID
	if( pEvent.m_PID )
	{
		if( pEvent.m_PID == -1 )
			pEvent.m_AppID = -1;
		else if( m_hProcMon )
		{
			cProcessPidInfo sPidInfo;
			tERROR err = m_hProcMon->GetProcessInfoEx(pEvent.m_PID, &sPidInfo);
			PR_TRACE((g_root, prtIMPORTANT, "bl\tprocess_notification: GetProcessInfo: err = %08x, native PID = %u, our internal PID = %016I64x", err, pEvent.m_PID, sPidInfo.m_UniqPid));
			if( PR_SUCC(err) )
			{
				pEvent.m_PID = sPidInfo.m_UniqPid;
				pEvent.m_AppID = sPidInfo.m_AppId;
			}
		}

		if( !pEvent.m_AppID )
			pEvent.m_AppID = -2;
	}

	if( (cnv.nReportTray & fReportEvent) && m_pReport && (!m_settings.m_Ins_InitMode || pEvent.m_TaskID == eTASK_SCAN || pEvent.m_TaskID == eTASK_UPDATER) )
	{
		if( cnv.pObjectName && cnv.nObjectNameSize )
		{
			bool bAdded = false;
/*			if( pEvent.m_TaskID == eTASK_SCAN )
				bAdded = PR_SUCC(m_pReport->FindObject(dbGlobalObjects, pEvent.m_ObjectType,
					cnv.pObjectName, cnv.nObjectNameSize, cFALSE, RF_DONT(RF_MAP_SUBST), &pEvent.m_ObjectID));
*/
			if( !bAdded )
				m_pReport->FindObject(pEvent.m_TaskID == eTASK_FAV ? dbGlobalObjects : cnv.nEvtDbType, pEvent.m_ObjectType,
					cnv.pObjectName, cnv.nObjectNameSize, cTRUE, 0, &pEvent.m_ObjectID);
		}

		if( cnv.sDetectName && !cnv.sDetectName->empty() )
			m_pReport->FindObject(dbGlobalObjects, eDetectName,
				(tPTR)cnv.sDetectName->data(), cnv.sDetectName->size() * sizeof(tWCHAR), cTRUE, 0, &pEvent.m_DetectName);

		m_pReport->AddEvent(cnv.nEvtDbType, (tReportEvent*)&pEvent, sizeof(tReportEvent), cnv.pEventId);
	}

	if( !m_settings.m_bEnableNotifications )
		return;

	if( pCfg && pCfg->m_bTray && (cnv.nReportTray & fReportTray) )
		sysSendMsg(pmc_PRODUCT_NOTIFICATION, 0, NULL, &pEvent, SER_SENDMSG_PSIZE);
	
	if( !(cnv.nReportTray & fReportEvent) )
		return;

	bool bMail = pCfg && pCfg->m_bMail && !m_settings.m_SMTPServer.empty();
	bool bELog = pCfg && !!pCfg->m_bELog;

	if( !bMail && !bELog )
		return;

	cStrObj strText;
	get_event_text(pEvent, strText, NULL);

	if( bMail )
	{
		{
			// Bug 28148 fix: падение из-за несинхронизированной работы с переменной m_strMailText 
			cAutoCS cs(m_lock, true);

			if( m_strMailText.empty() )
				get_mail_header(m_strMailText);

			m_strMailText += "\t";
			if( cBlLoc::init() )
			{
				m_strMailText += m_pRoot->GetFormatedText(cStrObj(),
					m_pRoot->GetString(tString(), PROFILE_LOCALIZE, NULL, "ProductEventSeverityText"), &pEvent);

				m_strMailText += m_pRoot->GetFormatedText(cStrObj(),
					"datetime(ReportTime) + \"\t\"", &pEvent);
			}
			m_strMailText += strText;
		}
		
		if( !m_settings.m_nSendMode )
			send_mail_notification();
	}

	if( bELog )
		report_to_system_log((enNotifySeverity)pEvent.m_Severity,
			strText, !!m_settings.m_bSystemEventLog);
}

void BlImpl::check_status_changed(enNotification nNotification,
	tDWORD* pStatePtr, tDWORD nOldState, tDWORD nState, bool bTrayForce)
{
	if( !bTrayForce )
	{
		*pStatePtr |= nState;
		if( (nOldState & nState) )
			return;
	}
	
	process_notification(nNotification, (cBLStatistics*)this,
		get_statusid(*pStatePtr, nState), 0, 0, fReportTray|(bTrayForce ? 0 : fReportEvent));
}

// -------------------------------------------
// -------------------------------------------

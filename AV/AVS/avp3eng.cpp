// -------------------------------------------

#include "avs.h"

#define MAX_NAME_LEN 0x400

// -------------------------------------------

class CAVP3Engine : public CAVSEngine
{
	virtual tERROR Load(cAVSUpdateInfo* pInfo);
	virtual tERROR Init();
	virtual tERROR Unload();

	virtual tERROR GetBasesInfo(cAVP3BasesInfo& pInfo);

	virtual tDWORD InitClassMask(cScanSettings& pSettings);

	virtual tERROR ICGetData(cObj* pObj, tPID pPid, tPTR& pICdata);
	virtual tERROR ICCheckData(tPTR pICdata, tQWORD pBitData, tDWORD pMandatoryScanDays);
	virtual tERROR ICUpdate(cObj* pObj, tPID pPid, tPTR pICdata, tQWORD pBitData, tBOOL pDeleteStatus);
	virtual tERROR ICFree(tPTR& pICdata);

	virtual tERROR ProcessObject(CObjectContext& pCtx);
	virtual tERROR PostProcessObject(CObjectContext& pCtx);

	virtual tERROR SetWriteAccess(CObjectContext& pCtx, cObj*& pDeletable);
	virtual tERROR AddReopenData(cIO* pIO, tPTR pData, tDWORD nPos, tDWORD pUserData);

	virtual tERROR CheckEIP(tPTR eip);

	virtual enDetectStatus GetDetectStatus(CObjectContext& pCtx);
	virtual enDetectType   GetDetectType(CObjectContext& pCtx);
	virtual enDetectDanger GetDetectDanger(CObjectContext& pCtx);
	virtual tERROR GetDetectName(CObjectContext& pCtx, cStringObj& strName);
	virtual tERROR GetPakerName(CObjectContext& pCtx, cStringObj& strName);
	virtual tERROR GetReopenData(CObjectContext& pCtx, cCharVector& pData);
	virtual bool   GetDisinfectability(CObjectContext& pCtx);
	virtual bool   IsShouldBeScan(CObjectContext& pCtx);

private:
	cEngine*      m_hEngine;
};

// -------------------------------------------

class CAVP3EngineHnd : public CAVSEngineHnd
{
public:
	CAVP3EngineHnd(AVSImpl* pAvs) : m_avs(pAvs)
	{
		m_avs->sysRegisterMsgHandler(pmc_ENGINE_LOAD, rmhLISTENER, m_avs, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
		m_avs->sysRegisterMsgHandler(pmc_ENGINE_OPTIONS, rmhLISTENER, m_avs, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	}

	CAVSEngine* CreateEngine(enEngineType eType){ return new CAVP3Engine(); };

	tERROR OnMsgReceive(tDWORD dwMsg, tDWORD dwMsgId, cObj* pObj, cObj* pCtx, tPTR pBuff, tDWORD* pSize)
	{
		switch( dwMsg )
		{
		case pmc_ENGINE_LOAD:
			switch(dwMsgId)
			{
			case pm_ENGINE_LOAD_BASE_INTEGRITY_FAILED:
			case pm_ENGINE_LOAD_BASE_ERROR_OPEN:
				m_avs->m_statistics.m_nBasesState |= bsInvalid;
				break;
			}
			return errOK;

		case pmc_ENGINE_OPTIONS:
			switch(dwMsgId)
			{
			case pm_ENGINE_OPTIONS_USE_MULTITHREAD_DRIVER:
				*(tBOOL*)pBuff = m_avs->m_settings.m_bMultiThreaded ? cTRUE : cFALSE;
				break;
			case pm_ENGINE_OPTIONS_USE_SIGN_CHECK:
				*(tBOOL*)pBuff = cFALSE;
				break;
			}
			return errOK;
		}
		return errOK;
	}

private:
	AVSImpl* m_avs;
};

// -------------------------------------------

CAVSEngineHnd* CreateEngineHnd_AVP3(AVSImpl* pAvs)
{
	return new CAVP3EngineHnd(pAvs);
}

// -------------------------------------------

tERROR CAVP3Engine::Load(cAVSUpdateInfo* pInfo)
{
	cAutoObj<cEngine>   engine;
	cAutoObj<cRegistry> reg;
	tRegKey root_key, path_key;

	tERROR error = m_avs->sysCreateObjectQuick((hOBJECT*)&engine, IID_ENGINE, PID_AVPMGR);

	if( PR_SUCC(error) && m_avs->m_settings.m_nStackSizeToCheck )
		error = engine->propSetDWord(plENGINE_STACK_PROBE, m_avs->m_settings.m_nStackSizeToCheck);

	if( PR_SUCC(error) )
		error = m_avs->sysCreateObject((hOBJECT*)&reg, IID_REGISTRY, PID_DTREG);

	if( PR_SUCC(error) )
	{
		reg->propSetDWord(pgOBJECT_RO, cFALSE);
		reg->propSetDWord(pgSAVE_RESULTS_ON_CLOSE, cFALSE);

		cStringObj config_path = m_avs->m_settings.m_sBaseFolder;
		config_path += "engine.dt";
		error = config_path.copy(reg, pgOBJECT_NAME);

		if( PR_SUCC(error) )
			error = reg->sysCreateObjectDone();

		if( PR_FAIL(error) )
			PR_TRACE(( m_avs, prtERROR, "avs\tEngineCreate: cannot open registry '%s', %terr", (tSTRING)config_path.c_str(cCP_ANSI), error));
	}

	if( PR_SUCC(error) )
		error = reg->OpenKey(&path_key, cRegRoot, "Version2\\Engines\\PID_AVP1", cTRUE);

	if( PR_SUCC(error) )
	{
		cStringObj avp_set_path = m_avs->m_settings.m_sBaseFolder;
		avp_set_path += m_avs->m_settings.m_sSetName;

		error = reg->SetValue(path_key, "SetPath", tid_STRING, avp_set_path.c_str(cCP_ANSI), avp_set_path.memsize(cCP_ANSI), cTRUE);

		if( PR_FAIL(error) )
			PR_TRACE(( m_avs, prtERROR, "avs\tEngineCreate: cannot set value SetPath=%s, %terr", (tSTRING)avp_set_path.c_str(cCP_ANSI), error));
	}

	if( PR_SUCC(error) )
		error = reg->OpenKey(&root_key, cRegRoot, "", cFALSE);

	if( PR_SUCC(error) )
	{
		error = engine->LoadConfiguration(*reg, root_key);

		if( PR_FAIL(error) )
			PR_TRACE(( m_avs, prtERROR, "avs\tEngineCreate: engine->LoadConfiguration failed with %terr", error));
		else if( m_avs->m_statistics.m_nBasesState & bsInvalid )
			PR_TRACE((m_avs, prtERROR, "avs\twarnAVS_LOAD_BASES"));
	}

	if( PR_FAIL(error) )
	{
		PR_TRACE((m_avs, prtERROR, "avs\tEngineCreate failed %x", error));
		return error;
	}

	m_hCtxObj = engine;
	m_hEngine = engine.relinquish();
	return errOK;
}

tERROR CAVP3Engine::Init()
{
	if( m_iChecker )
		m_hEngine->propSetObj(plENGINE_ICHECKER, (cObj*)m_iChecker);
	if( m_iSwift )
		m_hEngine->propSetObj(plENGINE_ICHECKER_STREAM, (cObj*)m_iSwift);
	return errOK;
}

tERROR CAVP3Engine::Unload()
{
	if( m_hEngine )
	{
		m_hEngine->sysCloseObject();
		m_hEngine = NULL;
	}
	return errOK;
}

tERROR CAVP3Engine::GetBasesInfo(cAVP3BasesInfo& pInfo)
{
	tERROR error = errOK;
	m_hEngine->get_pgENGINE_DB_TIME(&pInfo.m_dtBasesDate);
	pInfo.m_dwViruseCount = m_hEngine->get_pgENGINE_DB_COUNT();
	return error;
}

tDWORD CAVP3Engine::InitClassMask(cScanSettings& pSettings)
{
	tDWORD nMask = 0;
	nMask |= ENGINE_ACTION_CLASS_VIRUS_SCAN|ENGINE_ACTION_CLASS_OBJECT_MODIFY;
	switch( pSettings.m_nScanFilter )
	{
	default:
	case SCAN_FILTER_ALL_EXTENSION:  break;
	case SCAN_FILTER_ALL_INFECTABLE: nMask |= ENGINE_ACTION_CLASS_SMART_MASK; break;
	case SCAN_FILTER_ALL_OBJECTS:    nMask |= ENGINE_ACTION_CLASS_SMART_MASK | ENGINE_ACTION_CLASS_ALL_FILES; break;
	}

	if( pSettings.m_bScanPacked )		nMask |= ENGINE_ACTION_CLASS_PACKED;
	if( pSettings.m_bScanArchived )		nMask |= ENGINE_ACTION_CLASS_ARCHIVE;
	if( pSettings.m_bScanSFXArchived )	nMask |= ENGINE_ACTION_CLASS_ARCHIVE_SFX;
	if( pSettings.m_bScanMailBases )	nMask |= ENGINE_ACTION_CLASS_MAIL_DATABASE;
	if( pSettings.m_bScanPlainMail )	nMask |= ENGINE_ACTION_CLASS_MAIL_PLAIN;
	if( pSettings.m_bScanOLE )			nMask |= ENGINE_ACTION_CLASS_OLE_EMBEDDED|ENGINE_ACTION_CLASS_OLE_STREAM;
/*	if( pSettings.m_bCodeAnalyzer )*/   nMask |= ENGINE_ACTION_CLASS_VIRUS_HEURISTIC;
	return nMask;
}

tERROR CAVP3Engine::CheckEIP(tPTR eip)
{
	tDWORD size = sizeof(tPTR);
	if( errOK_DECIDED == m_avs->sysSendMsg(pmc_DIAGNOSTIC, pm_DIAGNOSTIC_IS_SAFE_IP,
			NULL, &eip, &size) )
		return errOK;
	return errNOT_OK;
}

tERROR CAVP3Engine::ICGetData(cObj* pObj, tPID pPid, tPTR& pICdata)
{
	if( !Engine_ICGetData )
		return errNOT_IMPLEMENTED;

	return Engine_ICGetData(m_hEngine, pObj, pPid, &pICdata, 0, 0);
}

tERROR CAVP3Engine::ICCheckData(tPTR pICdata, tQWORD pBitData, tDWORD pMandatoryScanDays)
{
	return Engine_ICCheckData(m_hEngine, pICdata, pBitData, pMandatoryScanDays);
}

tERROR CAVP3Engine::ICUpdate(cObj* pObj, tPID pPid, tPTR pICdata, tQWORD pBitData, tBOOL pDeleteStatus)
{
	return Engine_ICUpdate(m_hEngine, pObj, pPid, pICdata, pBitData, pDeleteStatus);
}

tERROR CAVP3Engine::ICFree(tPTR& pICdata)
{
	tERROR error = Engine_ICFreeData(m_hEngine, pICdata);
	pICdata = NULL;
	return error;
}

tERROR CAVP3Engine::ProcessObject(CObjectContext& pCtx)
{
	return m_hEngine->Process(pCtx.m_pObject);
}

tERROR CAVP3Engine::PostProcessObject(CObjectContext& pCtx)
{
	if( pCtx.m_fFalseAlarm || !pCtx.m_pScanCtx->m_Settings->m_bEmulEnable )
		return errOK;

	if( pCtx.m_nNumDetected ||
			(!pCtx.m_fToPostProcessing && (pCtx.m_fArchive || pCtx.m_fUnpacked)) )
		return errOK;

	tERROR error = m_avs->LockEngine(ENGINE_EMUL, &pCtx.m_pCurEngine);
	if( PR_SUCC(error) )
	{
		error = pCtx.GetEngine()->ProcessObject(pCtx);
		m_avs->UnlockEngine(pCtx.m_pCurEngine);
		pCtx.m_pCurEngine = NULL;
	}
	return error;
}

tERROR CAVP3Engine::SetWriteAccess(CObjectContext& pCtx, cObj*& pDeletable)
{
	return Engine_SetWriteAccess(pCtx.m_pObject, &pDeletable);
}

tERROR CAVP3Engine::AddReopenData(cIO* pIO, tPTR pData, tDWORD nPos, tDWORD pUserData)
{
	if( !Engine_AddReopenData )
		return errUNEXPECTED;

	return Engine_AddReopenData(pIO, pData, pUserData);
}

enDetectStatus CAVP3Engine::GetDetectStatus(CObjectContext& pCtx)
{
	tDWORD dwCertanity = 0;
	if( pCtx.m_pContext )
		CALL_Engine_GetAssociatedInfo(m_hEngine, pCtx.m_pObject, ENGINE_DETECT_CERTANITY,
			0, &dwCertanity, sizeof(tDWORD), 0);

	return (enDetectStatus)dwCertanity;
}

enDetectType   CAVP3Engine::GetDetectType(CObjectContext& pCtx)
{
	enDetectType dwResult = DETYPE_UNKNOWN;
	if( pCtx.m_pContext )
	{
		tDWORD dwType;
		if( PR_SUCC(CALL_Engine_GetAssociatedInfo(m_hEngine, pCtx.m_pObject, ENGINE_DETECT_TYPE,
			0, (tCHAR*)&dwType, sizeof(dwType), 0)) )
			dwResult = (enDetectType)dwType;
	}
	return dwResult;
}

enDetectDanger CAVP3Engine::GetDetectDanger(CObjectContext& pCtx)
{
	tDWORD dwDanger = 0;
	if( pCtx.m_pContext )
		CALL_Engine_GetAssociatedInfo(m_hEngine, pCtx.m_pObject, ENGINE_DETECT_DANGER,
			0, (tCHAR*)&dwDanger, sizeof(dwDanger), 0);
	switch( dwDanger )
	{
	default:
	case ENGINE_DETECT_DANGER_HIGH: return DETDANGER_HIGH;
	case ENGINE_DETECT_DANGER_MEDIUM: return DETDANGER_MEDIUM;
	case ENGINE_DETECT_DANGER_LOW: return DETDANGER_LOW;
	case ENGINE_DETECT_DANGER_INFORMATIONAL: return DETDANGER_INFORMATIONAL;
	}
	return DETDANGER_UNKNOWN;
}

tERROR CAVP3Engine::GetDetectName(CObjectContext& pCtx, cStringObj& strName)
{
	tERROR error = errUNEXPECTED;
	tCHAR lpVirusName[MAX_NAME_LEN] = "";
	if( pCtx.m_pContext )
		error = CALL_Engine_GetAssociatedInfo(m_hEngine, pCtx.m_pObject, ENGINE_DETECT_NAME,
			cCP_ANSI, lpVirusName, sizeof(lpVirusName), 0);

	if( PR_SUCC(error) )
		error = strName.assign(lpVirusName, cCP_ANSI);

	return error;
}

tERROR CAVP3Engine::GetPakerName(CObjectContext& pCtx, cStringObj& strName)
{
	tERROR error = errUNEXPECTED;
	tCHAR strPackerName[MAX_NAME_LEN] = "";
	if( pCtx.m_pContext )
		error = CALL_Engine_GetAssociatedInfo(m_hEngine, pCtx.m_pObject, ENGINE_OBTAINED_OS_NAME,
			cCP_ANSI, strPackerName, sizeof(strPackerName), 0);

	if( PR_SUCC(error) )
		error = strName.assign(strPackerName, cCP_ANSI);

	return error;
}

tERROR CAVP3Engine::GetReopenData(CObjectContext& pCtx, cCharVector& pData)
{
	tDWORD dwSize = 0;
	tERROR error = CALL_Engine_GetAssociatedInfo(m_hEngine,
		pCtx.m_pObject, ENGINE_REOPEN_DATA, 0, NULL, 0, (tDWORD*)&dwSize);

	if( PR_SUCC(error) )
	{
		pData.resize(dwSize);
		error = CALL_Engine_GetAssociatedInfo(m_hEngine,
			pCtx.m_pObject, ENGINE_REOPEN_DATA, 0, pData.data (), dwSize, NULL);
	}
	return error;
}

bool CAVP3Engine::GetDisinfectability(CObjectContext& pCtx)
{
	tDWORD dwDisinfectability = 0;
	CALL_Engine_GetAssociatedInfo(m_hEngine, pCtx.m_pObject, ENGINE_DETECT_DISINFECTABILITY,
		0, &dwDisinfectability, sizeof(tDWORD), 0);

	if( dwDisinfectability != ENGINE_DETECT_DISINFECTABILITY_YES_BY_DELETE &&
		dwDisinfectability != ENGINE_DETECT_DISINFECTABILITY_NO )
		return true;

	return false;
}

bool CAVP3Engine::IsShouldBeScan(CObjectContext& pCtx)
{
	bool bSkip = false;

	tDWORD dwResult;
	switch( pCtx.m_pScanCtx->m_Settings->m_nScanFilter )
	{
	case SCAN_FILTER_ALL_EXTENSION:
		if( AVLIB_IsExecutableExtension && PR_SUCC(AVLIB_IsExecutableExtension(pCtx.GetCached(), &dwResult)) && !dwResult )
			bSkip = true;
		break;

	case SCAN_FILTER_ALL_INFECTABLE:
		if( AVLIB_IsProgram && PR_SUCC(AVLIB_IsProgram(pCtx.GetCached(), &dwResult)) && !dwResult )
			bSkip = true;
		break;

	case SCAN_FILTER_ALL_OBJECTS:
		break;
	}

	return !bSkip;
};

// -------------------------------------------

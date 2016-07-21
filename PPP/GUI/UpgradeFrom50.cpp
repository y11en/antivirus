#include "StdAfx.h"
#if defined (_WIN32)
#include <structs/s_mc.h>
#include <structs/s_httpscan.h>
#include <structs/s_oas.h>
#include <common/updater.h>
#include "Kav50Settings/ss.h"
#include <assert.h>

namespace Kav50Settings
{

void InheritRtpSettings(CProfile* pProfiles, pxnode pnCfg, bool& bCustom)
{
	bCustom = false;

	assert(pProfiles && pnCfg);
	if (!pProfiles || !pnCfg)
		return;

	CProfile* pProtectionPrf = pProfiles->GetProfile(AVP_PROFILE_PROTECTION);
	CProfile* pOasPrf = pProfiles->GetProfile(AVP_PROFILE_FILEMONITOR);
	CProfile* pMcPrf = pProfiles->GetProfile(AVP_PROFILE_MAILMONITOR);
	CProfile* pWebPrf = pProfiles->GetProfile(AVP_PROFILE_WEBMONITOR);
	CProfile* pOgPrf = pProfiles->GetProfile(AVP_PROFILE_OFFICEGUARD);
	
	pxnode pRtp = xtree_get_node(pnCfg, L"REALTIME_PROT_SETTINGS\\dat");
	if (pRtp) // у FS этого значения нет, а настройки есть
	{
		tDWORD t = 0;
		
		/* Нефига не работает, где-то после upgrade эти настройки перебиваются.
		if (pProtectionPrf && xtree_query_val(pRtp, L"KLRTP_SE_RTP_DISABLE", t))
			pProtectionPrf->Enable(t == 0);
		if (pOasPrf && xtree_query_val(pRtp, L"KLRTP_SE_ONACESS_DISABLE", t))
			pOasPrf->Enable(t == 0);	
		if (pMcPrf && xtree_query_val(pRtp, L"KLRTP_SE_MAILCHK_DISABLE", t))
			pMcPrf->Enable(t == 0);	
		if (pWebPrf && xtree_query_val(pRtp, L"KLRTP_SE_SCRIPTCHK_DISABLE", t))	
			pWebPrf->Enable(t == 0);	
		if (pOgPrf && xtree_query_val(pRtp, L"KLRTP_SE_OFFICEGRD_DISABLE", t))
			pOgPrf->Enable(t == 0);
		*/
			
		if (xtree_query_val(pRtp, L"KLRTP_SE_TASK_LEVEL", t))
		{
			switch (t)
			{
			case 0x0: // PROT_HIGH_SECUR
				if (pOasPrf) pOasPrf->ResetSettingsLevel(3);
				if (pMcPrf) pMcPrf->ResetSettingsLevel(3);
				if (pWebPrf) pWebPrf->ResetSettingsLevel(3);
				return;
				
			case 0x1: // PROT_RECOMMENDED
				if (pOasPrf) pOasPrf->ResetSettingsLevel(SETTINGS_LEVEL_DEFAULT);
				if (pMcPrf) pMcPrf->ResetSettingsLevel(SETTINGS_LEVEL_DEFAULT);
				if (pWebPrf) pWebPrf->ResetSettingsLevel(SETTINGS_LEVEL_DEFAULT);
				return;
				
			case 0x2: // PROT_HIGH_SPEED
				if (pOasPrf) pOasPrf->ResetSettingsLevel(2);
				if (pMcPrf) pMcPrf->ResetSettingsLevel(2);
				if (pWebPrf) pWebPrf->ResetSettingsLevel(2);
				return;
			}
		}
	}
	bCustom = true;
	if (pOasPrf) pOasPrf->ResetSettingsLevel(SETTINGS_LEVEL_CUSTOM);
	if (pMcPrf) pMcPrf->ResetSettingsLevel(SETTINGS_LEVEL_CUSTOM);
	if (pWebPrf) pWebPrf->ResetSettingsLevel(SETTINGS_LEVEL_CUSTOM);
}

void InheritQbSettings(CProfile* pProfiles, pxnode pnCfg)
{
	assert(pProfiles && pnCfg);
	if (!pProfiles || !pnCfg)
		return;

	cBLSettings* pSet = (cBLSettings*) pProfiles->settings(AVP_PROFILE_PRODUCT, cBLSettings::eIID);
	if (!pSet)
		return;
	
	pxnode pBl = xtree_get_node(pnCfg, L"BL/dat");
	pxnode pQb = xtree_get_node(pnCfg, L"Quarantine & Backup/dat");
	if (!pBl || !pQb)
		return;
	
	tDWORD t = 0;

	if (xtree_query_val(pBl, L"KLBL_SE_FULL_REPORT", t))
		pSet->m_bEnableInfoEvents = t != 0;
	if (xtree_query_val(pBl, L"KLBL_SE_REPORT_CHECK_LIFETIME", t))
		pSet->m_ReportSpan.m_on = t != 0;
	if (xtree_query_val(pBl, L"KLBL_SE_REPORT_LIFETIME", t))
		pSet->m_ReportSpan.m_val = t;

	if (xtree_query_val(pQb, L"KLQB_SE_BACKUP_CHECK_OBJ_LIFE", t))
		pSet->m_QBSpan.m_on = t != 0;
	if (xtree_query_val(pQb, L"KLQB_SE_OBJ_LIFE_BACKUP", t))
		pSet->m_QBSpan.m_val = t;

	if (xtree_query_val(pQb, L"KLQB_SE_AUTO_SCAN_ON_UPDATE", t))
	{
		cUpdaterSettings *pSet = (cUpdaterSettings*) pProfiles->settings(AVP_PROFILE_UPDATER, cUpdaterSettings::eIID);
		if (pSet && pSet->m_pTaskBLSettings->isBasedOn(cUpdaterProductSpecific::eIID))
		{
			cUpdaterProductSpecific* p = (cUpdaterProductSpecific*) pSet->m_pTaskBLSettings.ptr_ref();
			p->m_bRescanQuarantineAfterUpdate = t != 0;
		}
	}
}

void InheritGuiSettings(CProfile* pProfiles, pxnode pnCfg)
{
	assert(pProfiles && pnCfg);
	if (!pProfiles || !pnCfg)
		return;
	
	cGuiSettings* pSet = (cGuiSettings*) pProfiles->settings(AVP_TASK_GUI, cGuiSettings::eIID);
	if (!pSet)
		return;
	
	pxnode pBl = xtree_get_node(pnCfg, L"BL\\dat");
	if (!pBl)
		return;
	
	tDWORD t = 0;
	
	if (xtree_query_val(pBl, L"KLBL_SE_GUI_NO_AVS_LOAD", t))
		pSet->m_bEnableTrayIconAnimation = t == 0;
}

void InheritAvsSettings(CProfile* pProfiles, pxnode pnCfg)
{
	assert(pProfiles && pnCfg);
	if (!pProfiles || !pnCfg)
		return;
	
	cAVSSettings* pAvsSet = (cAVSSettings*) pProfiles->settings(AVP_SERVICE_AVS, cAVSSettings::eIID);
	if (!pAvsSet)
		return;

	pxnode pAvs = xtree_get_node(pnCfg, L"AVS\\dat");
	pxnode pBl = xtree_get_node(pnCfg, L"BL\\dat");
	if (!pAvs || !pBl)
		return;
	
	tDWORD bUseRiskware = FALSE;
	xtree_query_val(pAvs, L"KLAVS_SE_USE_RISKWARE_BASES", bUseRiskware);
	pAvsSet->m_nDetectMask = 1 | (bUseRiskware ? 6 : 0);

	pxnode pRWs = xtree_get_node(pBl, L"KLBL_SE_ALLOWED_RISKWARE_LIST");
	if (pRWs)
	{
		for (pxnode pRW = pRWs->first_child; pRW; pRW = pRW->next)
		{
			cDetectExclude exclude;
			exclude.m_nTriggers = cDetectExclude::fObjectMask;
			exclude.m_Object.m_bRecurse = cFALSE;
			xtree_query_val(pRW, L"KLBL_TP_RISKWARE_PATH_MASK", exclude.m_Object.m_strMask);
			xtree_query_val(pRW, L"KLBL_TP_RISKWARE_VERDICT_MASK", exclude.m_strVerdictMask);
			
			if( !exclude.m_strVerdictMask.empty() )
				exclude.m_nTriggers |= cDetectExclude::fVerdictMask;
			
			if( !exclude.m_Object.m_strMask.empty() )
				exclude.m_Object.m_bRecurse = exclude.m_Object.m_strMask[exclude.m_Object.m_strMask.length()-1] == '\\' ? cTRUE : cFALSE;
			
			pAvsSet->m_aExcludes.push_back(exclude);
		}
	}	
}

void InheritExcludeProcessSettings(CProfile* pProfiles, pxnode pnCfg)
{
	assert(pProfiles && pnCfg);
	if (!pProfiles || !pnCfg)
		return;
	
	cBLTrustedApps* pSet = (cBLTrustedApps*) pProfiles->settings(AVP_SERVICE_PROCESS_MONITOR, cBLTrustedApps::eIID);
	if (!pSet)
		return;
	
	pxnode pBl = xtree_get_node(pnCfg, L"BL\\dat");
	if (!pBl)
		return;
	
	tDWORD bEnabled = cFALSE;
	xtree_query_val(pBl, L"KLBL_SE_ENABLE_EXCLUDE_PROCESSES", bEnabled);

	pxnode pExcludes = xtree_get_node(pBl, L"KLBL_SE_EXCLUDED_PROCESSES_LIST");
	if (!pExcludes)
		return;

	for (pxnode pExclude = pExcludes->first_child; pExclude; pExclude = pExclude->next)
	{
		cBLTrustedApp tapp;
		tapp.m_bEnabled = bEnabled;
		cStrObj dir, name;
		xtree_query_val(pExclude, L"KLBL_TP_EXCLUDE_PROCESS_DIR", dir);
		xtree_query_val(pExclude, L"KLBL_TP_EXCLUDE_PROCESS_NAME", name);
		dir.check_last_slash(true);
		tapp.m_sImagePath = dir;
		tapp.m_sImagePath += name;
		tapp.m_Hash = 0; // требуется пересчёт		

		pSet->m_aItems.push_back(tapp);
	}
}

void AddExcludes(pxnode pExcludes, cVector<cDetectExclude>& excludes, const char* pszProfile)
{
	if (!pExcludes)
		return;

	for (pxnode pExclude = pExcludes->first_child; pExclude; pExclude = pExclude->next)
	{
		if (pExclude->type != SBVT_STRING || !pExclude->pdata)
			continue;
		
		cStrObj path;
		path.assign(pExclude->pdata, cCP_UNICODE, pExclude->data_size);
		
		bool bFound = false;
		for (tDWORD i = 0; i < excludes.size(); ++i)
		{
			cDetectExclude& exclude = excludes[i];
			if (exclude.m_bEnable
				&& (exclude.m_nTriggers & cDetectExclude::fObjectMask)
				&& exclude.m_Object.m_strMask == path)
			{		
				if (exclude.m_aTaskList.find(pszProfile) == exclude.m_aTaskList.npos)
					exclude.m_aTaskList.push_back(pszProfile);
				bFound = true;
				break;
			}
		}
		if (bFound)
			continue;					
		
		cDetectExclude exclude;
		exclude.m_nTriggers |= cDetectExclude::fTaskList;
		exclude.m_Object.m_bRecurse = (!path.empty() && (path[path.length()-1] == '\\')) ? cTRUE : cFALSE;
		exclude.m_Object.m_strMask = path;
		exclude.m_aTaskList.push_back(pszProfile);
		excludes.push_back(exclude);
	}
}

void InheritOasSettings(pxnode pnCfg, cOASSettings* pSet, cAVSSettings* pAvsSet)
{
	assert(pnCfg && pSet);
	if (!pnCfg || !pSet)
		return;
	
	tDWORD t = 0;
	
	pxnode pnParams = xtree_get_node(pnCfg, L"TASK_ADDITIONAL_PARAMS");
	if (pnParams)
	{
		if (xtree_query_val(pnParams, L"KLOAS_SE_INFECTED_OBJECT_ACTION", t))
		{
			switch (t)
			{
			case 0: /*AskUser*/
				pSet->m_nScanAction = SCAN_ACTION_ASKUSER;
				break;
			case 1: /*Skip*/
				pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
				pSet->m_bTryDisinfect = false;
				pSet->m_bTryDelete = false;
				break;
			case 2: /*Cure*/
				pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
				pSet->m_bTryDisinfect = true;
				pSet->m_bTryDelete = true;
				break;
			case 3: /*Delete*/
				pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
				pSet->m_bTryDisinfect = false;
				pSet->m_bTryDelete = true;
				break;
			}
		}		

		if (xtree_query_val(pnParams, L"KLOAS_SE_SCAN_TYPE", t))
		{
			switch (t)
			{
			case 0: // AllInfectable
				pSet->m_nScanFilter = SCAN_FILTER_ALL_INFECTABLE;
				break;
			case 1: // AllObjects
				pSet->m_nScanFilter = SCAN_FILTER_ALL_OBJECTS;
				break;
			case 3: // AllByExtension			
				pSet->m_nScanFilter = SCAN_FILTER_ALL_EXTENSION;
				break;
			}
		}

		if (xtree_query_val(pnParams, L"KLOAS_SE_SCAN_ARCHIVED", t))
			pSet->m_bScanArchived = t != 0;
		if (xtree_query_val(pnParams, L"KLOAS_SE_SCAN_SFX_ARCHIVED", t))
			pSet->m_bScanSFXArchived = t != 0;
		if (xtree_query_val(pnParams, L"KLOAS_SE_SCAN_OLE", t))
			pSet->m_bScanOLE = t != 0;
		if (xtree_query_val(pnParams, L"KLOAS_SE_SCAN_PLAIN_MAIL", t))
			pSet->m_bScanPlainMail = t != 0;

		tDWORD bScanRemovable = cTRUE, bScanFixed = cTRUE, bScanNetwork = cTRUE;
		xtree_query_val(pnParams, L"KLOAS_SE_SCAN_REMOVABLE_DRIVE", bScanRemovable);
		xtree_query_val(pnParams, L"KLOAS_SE_SCAN_FIXED_DRIVE", bScanFixed);
		xtree_query_val(pnParams, L"KLOAS_SE_SCAN_NETWORK_DRIVE", bScanNetwork);

		for (tDWORD i = 0; i < pSet->m_aScanObjects.size(); ++i)
		{
			switch (pSet->m_aScanObjects[i].m_nObjType)
			{
			case OBJECT_TYPE_ALL_FIXED_DRIVES:
				pSet->m_aScanObjects[i].m_bEnabled = bScanFixed;
				break;
			case OBJECT_TYPE_ALL_REMOVABLE_DRIVES:
				pSet->m_aScanObjects[i].m_bEnabled = bScanRemovable;
				break;
			case OBJECT_TYPE_ALL_NETWORK_DRIVES:
				pSet->m_aScanObjects[i].m_bEnabled = bScanNetwork;
				break;
			}
		}


		if (pAvsSet && xtree_query_val(pnParams, L"KLOAS_SE_CHECK_EXCLUDE_LIST", t) && t != 0)
		{
			AddExcludes(xtree_get_node(pnParams, L"KLOAS_SE_EXCLUDE_MASKS")
				, pAvsSet->m_aExcludes
				, AVP_TASK_ONACCESSSCANNER);
		}
	}
}

void InheritMcSettings(pxnode pnCfg, cMCSettings* pSet, cAVSSettings* pAvsSet)
{
	assert(pnCfg && pSet);
	if (!pnCfg || !pSet)
		return;
	
	tDWORD t = 0;
	
	pxnode pnParams = xtree_get_node(pnCfg, L"TASK_ADDITIONAL_PARAMS");
	if (pnParams)
	{
		if (xtree_query_val(pnParams, L"KLMC_SE_INFECTED_MAIL_ACTION", t))
		{
			switch (t)
			{
			case 1: /*Cure*/
				pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
				pSet->m_bTryDisinfect = true;
				pSet->m_bTryDelete = true;
				break;
			case 2: /*Delete*/
				pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
				pSet->m_bTryDisinfect = false;
				pSet->m_bTryDelete = true;
				break;
			}
		}		
		
		if (xtree_query_val(pnParams, L"KLMC_SE_SCAN_POP3_MAIL", t))
			pSet->m_bCheckIncomingMessagesOnly = t == 0;
		if (xtree_query_val(pnParams, L"KLMC_SE_SCAN_ARCHIVES", t))
			pSet->m_bScanArchived = t != 0;
		if (xtree_query_val(pnParams, L"KLMC_SE_CHECK_TIME_LIMIT", t))
			pSet->m_nTimeLimit.m_on = t != 0;
		if (xtree_query_val(pnParams, L"KLMC_SE_TIME_LIMIT", t))
			pSet->m_nTimeLimit.m_val = t;

		if (pAvsSet && xtree_query_val(pnParams, L"KLMC_SE_CHECK_EXCLUDE_LIST", t) && t != 0)
		{
			AddExcludes(xtree_get_node(pnParams, L"KLMC_SE_EXCLUDE_LIST")
				, pAvsSet->m_aExcludes
				, AVP_TASK_MAILCHECKER);
		}
	}
}

void InheritSchedule(pxnode pnCfg, cTaskSchedule& schedule)
{
	enum ScheduleType
	{
        SCH_TYPE_NONE,			///< Тип расписания не определен.
		SCH_TYPE_MILLISECONS,	///< Расписание с периодом, задаваемым числом msec.
		SCH_TYPE_EVERYDAY,		///< Задача запускается каждый день.
		SCH_TYPE_EVERYWEEK,		///< Задача запускается каждую неделю.
		SCH_TYPE_EVERYMONTH,	///< Задача запускается каждый месяц.
		SCH_TYPE_SECONDS		///< Расписание с периодом, задаваемым числом секунд.
    };  
	
	tDWORD dwScheduleType = SCH_TYPE_NONE;
	xtree_query_val(pnCfg, L"TASKSCH_TYPE", dwScheduleType);
	
	if (dwScheduleType == SCH_TYPE_NONE)
	{
		cStrObj evType;
		xtree_query_val(pnCfg, L"EVENT_TYPE", evType);
		if (evType == "KLBLAG_EV_ONAPPSTART")
		{
			schedule.m_eMode = schmEnabled;
			schedule.m_eType = schtOnStartup;
		}
		else
			schedule.m_eMode = schmManual;
	}
	else
	{
		schedule.m_eMode = schmEnabled;
		
		tDWORD bRunMissed = FALSE;
		xtree_query_val(pnCfg, L"TASKSCH_RUN_MISSED_FLAG", bRunMissed);
		
		tDWORD dwPeriod = 1;
		xtree_query_val(pnCfg, L"TASKSCH_MS_PERIOD", dwPeriod);
		
		time_t now = time(NULL);
		tm t;
		memcpy(&t, localtime(&now), sizeof(t));		
		t.tm_isdst = 0;
		
		switch (dwScheduleType)
		{
		case SCH_TYPE_MILLISECONS:
			schedule.m_eType = schtMinutely;
			schedule.m_nMinutes = dwPeriod / (60 * 1000);
			break;
		case SCH_TYPE_EVERYDAY:
			schedule.m_eType = schtDaily;
			schedule.m_eEvery = schEveryDays;
			schedule.m_nDays = dwPeriod;
			schedule.m_bUseTime = cTRUE;
			
			xtree_query_val(pnCfg, L"TASKSCH_ED_HOURS", (tDWORD&)t.tm_hour);
			xtree_query_val(pnCfg, L"TASKSCH_ED_MINS", (tDWORD&)t.tm_min);
			xtree_query_val(pnCfg, L"TASKSCH_ED_SECS", (tDWORD&)t.tm_sec);
			schedule.m_nFirstRunTime = mktime(&t) - _timezone;
			break;
		case SCH_TYPE_EVERYWEEK:
			{
				schedule.m_eType = schtWeekly;
				memset(schedule.m_bWeekDays, 0, sizeof(schedule.m_bWeekDays));
				tDWORD nDay = 1;
				xtree_query_val(pnCfg, L"TASKSCH_EW_DAY", nDay);
				schedule.m_bWeekDays[nDay == 0 ? 6 : (nDay - 1)] = cTRUE;
				
				schedule.m_bUseTime = cTRUE;
				xtree_query_val(pnCfg, L"TASKSCH_EW_HOURS", (tDWORD&)t.tm_hour);
				xtree_query_val(pnCfg, L"TASKSCH_EW_MINS", (tDWORD&)t.tm_min);
				xtree_query_val(pnCfg, L"TASKSCH_EW_SECS", (tDWORD&)t.tm_sec);
				schedule.m_nFirstRunTime = mktime(&t) - _timezone;
			}
			break;
		case SCH_TYPE_EVERYMONTH:
			schedule.m_eType = schtMonthly;
			schedule.m_nDays = 1;
			xtree_query_val(pnCfg, L"TASKSCH_EM_DAY", schedule.m_nDays);
			schedule.m_bUseTime = cTRUE;
			xtree_query_val(pnCfg, L"TASKSCH_EM_HOURS", (tDWORD&)t.tm_hour);
			xtree_query_val(pnCfg, L"TASKSCH_EM_MINS", (tDWORD&)t.tm_min);
			xtree_query_val(pnCfg, L"TASKSCH_EM_SECS", (tDWORD&)t.tm_sec);
			schedule.m_nFirstRunTime = mktime(&t) - _timezone;
			break;
		case SCH_TYPE_SECONDS:
			schedule.m_eType = schtMinutely;
			schedule.m_nMinutes = dwPeriod / 60;
			if (schedule.m_nMinutes % 60 == 0)
			{
				schedule.m_nHours = schedule.m_nMinutes / 60;
				schedule.m_eType = schtHourly;
			}
			break;
		}
	}
}

void InheritOdsSettings(pxnode pnCfg,
						CProfile* pPrf,
						cAVSSettings* pAvsSet,
						bool bInheritObjects,
						CRootSink* pSink)
{
	assert(pnCfg);
	if (!pnCfg || !pPrf)
		return;

	cODSSettings* pSet = (cODSSettings*) pPrf->settings();
	if (!pSet || pSet->getIID() != cODSSettings::eIID)
		return;
	
	tDWORD t = 0;
	
	pxnode pnParams = xtree_get_node(pnCfg, L"TASK_ADDITIONAL_PARAMS");
	if (pnParams)
	{
		// KLODS_SE_TASK_LEVEL всегда PROT_RECOMMENDED,
		// очень похоже на то, что 5.0 вычисляет уровень налету по настройкам
		pPrf->ResetSettingsLevel(SETTINGS_LEVEL_CUSTOM);
		
		if (xtree_query_val(pnParams, L"KLODS_SE_INFECTED_OBJECT_ACTION", t))
		{
			switch (t)
			{
			case 0: /*AskUser*/
				pSet->m_nScanAction = SCAN_ACTION_ASKUSER;
				break;
			case 1: /*Skip*/
				pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
				pSet->m_bTryDisinfect = false;
				pSet->m_bTryDelete = false;
				break;
			case 2: /*Cure*/
			case 5: /*Recommended*/	
				pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
				pSet->m_bTryDisinfect = true;
				pSet->m_bTryDelete = true;
				break;
			case 3: /*Delete*/
				pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
				pSet->m_bTryDisinfect = false;
				pSet->m_bTryDelete = true;
				break;
			case 4: /*AskUserOnFinish*/
				pSet->m_nScanAction = SCAN_ACTION_POSTPONE;
				break;
			}
		}		
		
		if (xtree_query_val(pnParams, L"KLODS_SE_SCAN_TYPE", t))
		{
			switch (t)
			{
			case 0: // AllInfectable
				pSet->m_nScanFilter = SCAN_FILTER_ALL_INFECTABLE;
				break;
			case 1: // AllObjects
				pSet->m_nScanFilter = SCAN_FILTER_ALL_OBJECTS;
				break;
			case 3: // AllByExtension			
				pSet->m_nScanFilter = SCAN_FILTER_ALL_EXTENSION;
				break;
			}
		}
		
		if (xtree_query_val(pnParams, L"KLODS_SE_CHECK_TIME_LIMIT", t))
			pSet->m_nTimeLimit.m_on = t != 0;
		if (xtree_query_val(pnParams, L"KLODS_SE_TIME_LIMIT", t))
			pSet->m_nTimeLimit.m_val = t;
		if (xtree_query_val(pnParams, L"KLODS_SE_CHECK_SIZE_LIMIT", t))
			pSet->m_nSizeLimit.m_on = t != 0;
		if (xtree_query_val(pnParams, L"KLODS_SE_SIZE_LIMIT", t))
			pSet->m_nSizeLimit.m_val = t / (1024*1024);
		
		if (xtree_query_val(pnParams, L"KLODS_SE_SCAN_ARCHIVED", t))
			pSet->m_bScanArchived = t != 0;
		if (xtree_query_val(pnParams, L"KLODS_SE_SCAN_OLE", t))
			pSet->m_bScanOLE = t != 0;
		if (xtree_query_val(pnParams, L"KLODS_SE_SCAN_PLAIN_MAIL", t))
			pSet->m_bScanPlainMail = t != 0;
		if (xtree_query_val(pnParams, L"KLODS_SE_ASK_ARCHIVE_PWD", t))
			pSet->m_bAskPassword = t != 0;
		
		if (xtree_query_val(pnParams, L"KLODS_SE_USE_ICHECKER", t))
			pSet->m_bUseIChecker = t != 0;
		if (xtree_query_val(pnParams, L"KLODS_SE_USE_ISTREAMS", t))
			pSet->m_bUseIStreams = t != 0;
		
		if (pPrf->isItYou(AVP_PROFILE_FULLSCAN)
			&& pAvsSet
			&& xtree_query_val(pnParams, L"KLODS_SE_CHECK_EXCLUDE_LIST", t) && t != 0)
		{
			AddExcludes(xtree_get_node(pnParams, L"KLODS_SE_EXCLUDE_MASKS")
				, pAvsSet->m_aExcludes
				, AVP_TASK_ONDEMANDSCANNER);
		}

		if (bInheritObjects)
		{
			pSet->m_bScanRemovable = false;
			pSet->m_bScanFixed = false;
			pSet->m_bScanNetwork = false;
			pSet->m_bScanSectors = false;
			pSet->m_bScanMemory = false;
			pSet->m_bScanStartup = false;
			pSet->m_bScanMail = false;
			pSet->m_aScanObjects.clear();
			
			pxnode pnObjects = xtree_get_node(pnParams, L"KLODS_SE_SCAN_OBJECTS");
			if (pnObjects)
			{
				for (pxnode pnObj = pnObjects->first_child; pnObj; pnObj = pnObj->next)
				{
					tDWORD t = 0;				
					if (!xtree_query_val(pnObj, L"KLODS_TP_OBJECT_ENABLED", t) || t == 0)
						continue;
					
					cStrObj sObjType, sObjName;
					if (!xtree_query_val(pnObj, L"KLODS_TP_OBJECT_TYPE", sObjType))
						continue;
					
					xtree_query_val(pnObj, L"KLODS_TP_OBJECT_NAME", sObjName);
					
					cScanObject obj;
					obj.m_strObjName = sObjName;
					obj.m_bEnabled = true;

					if (sObjType == L"KLODSCN - My computer")
					{
						obj.m_nObjType = OBJECT_TYPE_MY_COMPUTER;
						pSet->m_bScanFixed = true;
						pSet->m_bScanSectors = true;
					}
					else if (sObjType == L"KLODSCN - Fixed Drives")
					{
						obj.m_nObjType = OBJECT_TYPE_ALL_FIXED_DRIVES;
						pSet->m_bScanFixed = true;
					}
					else if (sObjType == L"KLODSCN - Removable Drives")
					{
						obj.m_nObjType = OBJECT_TYPE_ALL_REMOVABLE_DRIVES;
						pSet->m_bScanRemovable = true;
					}
					else if (sObjType == L"KLODSCN - Network Drives")
					{
						obj.m_nObjType = OBJECT_TYPE_ALL_NETWORK_DRIVES;
						pSet->m_bScanNetwork = true;
					}
					else if (sObjType == L"KLODSCN - Mail")
					{
						obj.m_nObjType = OBJECT_TYPE_MAIL;
						pSet->m_bScanMail = true;
					}
					else if (sObjType == L"KLODSCN - Startup")
					{
						obj.m_nObjType = OBJECT_TYPE_STARTUP;
						pSet->m_bScanStartup = true;
					}
					else if (sObjType == L"KLODSCN - Memory")
					{
						obj.m_nObjType = OBJECT_TYPE_MEMORY;
						pSet->m_bScanMemory = true;
					}
					else if (sObjType == L"KLODSCN - Drive")
					{
						obj.m_nObjType = OBJECT_TYPE_DRIVE;
						obj.m_bRecursive = true;
					}
					else if (sObjType == L"KLODSCN - Folder")
					{
						obj.m_nObjType = OBJECT_TYPE_FOLDER;
						obj.m_bRecursive = true;
					}
					else if (sObjType == L"KLODSCN - File")
					{
						obj.m_nObjType = OBJECT_TYPE_FILE;
						obj.m_bRecursive = false;
					}
					else
					{
						assert(!"Unsupported object type");
						continue;
					}
					pSet->m_aScanObjects.push_back(obj);
				}
			}	
		}		

		// Run as...
		cStrObj user, pwd;
		xtree_query_val(pnParams, L"klprts-TaskAccountUser", user);
		xtree_query_val(pnParams, L"klprts-TaskAccountPassword", pwd);
		if (!user.empty())
		{
			cTaskRunAs& runas = pPrf->m_cfg.run_as();
			runas.m_enable = cTRUE;
			runas.m_sAccount = user;
			pSink->CryptDecrypt(pwd, true);
			runas.m_sPassword = pwd;
		}
	}

	InheritSchedule(pnCfg, pPrf->schedule());
}

void InheritUpdaterSettings(pxnode pnCfg, CProfile* pPrf, CProfile* pProfiles, CRootSink* pSink)
{
	assert(pnCfg && pPrf && pProfiles);
	if (!pnCfg || !pPrf || !pProfiles)
		return;

	cUpdaterSettings* pSet = (cUpdaterSettings*) pPrf->settings(pPrf->m_sName.c_str(cCP_ANSI), cUpdaterSettings::eIID);
	if (!pSet)
		return;

	InheritSchedule(pnCfg, pPrf->schedule());

	pxnode pnParams = xtree_get_node(pnCfg, L"TASK_ADDITIONAL_PARAMS");
	if (pnParams)
	{
		cBLSettings* pBlSet = (cBLSettings*) pProfiles->settings(AVP_PROFILE_PRODUCT, cBLSettings::eIID);
		if (pBlSet)
		{
			cProxySettings& proxy = pBlSet->m_ProxySettings;

			proxy.m_bypassProxyServerForLocalAddresses = cFALSE;

			xtree_query_val(pnParams, L"proxy",                   proxy.m_bUseProxy);
			xtree_query_val(pnParams, L"use_ie_proxy",            proxy.m_bUseIEProxySettings);
			tBOOL bUseNTLM, bUseAuth;
			xtree_query_val(pnParams, L"proxy_ntlm_access_token", bUseNTLM);
			xtree_query_val(pnParams, L"proxy_authorisation",     bUseAuth);

			proxy.m_bProxyAuth = bUseAuth && !bUseNTLM;

			xtree_query_val(pnParams, L"proxy_login",             proxy.m_strProxyLogin);
			xtree_query_val(pnParams, L"proxy_port",              proxy.m_nProxyPort);
			xtree_query_val(pnParams, L"proxy_pwd",               proxy.m_strProxyPassword);
			pSink->CryptDecrypt(proxy.m_strProxyPassword, true);
			xtree_query_val(pnParams, L"proxy_url",               proxy.m_strProxyHost);
		}

		tDWORD bUpdateFromKL = cTRUE, bUpdateFromAK = cFALSE, bTryInet = cTRUE;
		xtree_query_val(pnParams, L"inet_update", bUpdateFromKL);
		xtree_query_val(pnParams, L"asrv_update", bUpdateFromAK);
		bool bUpdateFromUserURL = !bUpdateFromAK && !bUpdateFromKL;
		xtree_query_val(pnParams, L"try_inet", bTryInet);
		if (bUpdateFromUserURL && bTryInet)
			bUpdateFromKL = true;
		cStrObj sUserUrl;
		xtree_query_val(pnParams, L"update_srv_url", sUserUrl);

		cStringObj strProductType = g_hGui->ExpandEnvironmentString("%ProductType%");
		pSet->m_blAKServerUse =
			strProductType == "wks" || strProductType == "fs" ? bUpdateFromAK : false;
		pSet->m_bUseInternalSources = bUpdateFromKL;
		pSet->m_aSources.clear();
		if (!sUserUrl.empty())
		{
			cUpdaterSource src;
			src.m_bEnable = bUpdateFromUserURL;
			src.m_strPath = sUserUrl;
			pSet->m_aSources.push_back(src);
		}

		// Retranslation
		tDWORD bUseRetr = cFALSE;
		xtree_query_val(pnParams, L"updates_retranslation", bUseRetr);
		pSet->m_bUseRetr = bUseRetr;
		cStrObj retrPath;
		xtree_query_val(pnParams, L"retranslation_dir", retrPath);
		if (!retrPath.empty())
			pSet->m_strRetrPath = retrPath;

/* TODO: А собственно, что ретранслировать-то?
		pSet->m_RetranslationFilters.?

		BOOL "retranslate_bases" download Bases
		BOOL "KLUPD_RETRANSLATE_UPDATES" download Modules
		BOOL "retranslate_urgent_updates" ? Urgent Modules : All Modules;
		BOOL "retranslate_only_listed_apps", filter ?
*/

		// Run as...
		cStrObj user, pwd;
		xtree_query_val(pnParams, L"klprts-TaskAccountUser", user);
		xtree_query_val(pnParams, L"klprts-TaskAccountPassword", pwd);
		if (!user.empty())
		{
			cTaskRunAs& runas = pPrf->m_cfg.run_as();
			runas.m_enable = cTRUE;
			runas.m_sAccount = user;
			pSink->CryptDecrypt(pwd, true);
			runas.m_sPassword = pwd;
		}
	}
}

}

void InheritOld50Configuration(CProfile * pProfilesList, pxnode ss_root, CRootSink* pSink)
{
	using namespace Kav50Settings;

	pxnode pnRootSettings = xtree_get_node(ss_root, L"settings");
	if (!pnRootSettings)
		return;
	pxnode pnSettings = xtree_find_node(pnRootSettings, L"5.0.0.0", true);
	if (!pnSettings)
		return;

	bool bRtpCustomSettings = false;
	InheritRtpSettings(pProfilesList, pnSettings, bRtpCustomSettings);
	InheritQbSettings(pProfilesList, pnSettings);
	InheritGuiSettings(pProfilesList, pnSettings);
	InheritAvsSettings(pProfilesList, pnSettings);
	InheritExcludeProcessSettings(pProfilesList, pnSettings);

	pxnode pnTasksCfg = xtree_get_node(ss_root, L"tasks");
	if( !pnTasksCfg )
		return;

	cAVSSettings* pAvsSet = (cAVSSettings*) pProfilesList->settings(AVP_SERVICE_AVS, cAVSSettings::eIID);

	for (pxnode pmTask = pnTasksCfg->first_child; pmTask; pmTask = pmTask->next)
	{
		pxnode pnCfg = xtree_get_node(pmTask, L"PRTS/1.0/TASKS/dat");
		if (!pnCfg)
			continue;
		cStrObj taskName, taskId, taskKey;
		if (!xtree_query_val(pnCfg, L"TASK_NAME", taskName))
			continue;
		if (!xtree_query_val(pnCfg, L"TASK_INFO_PARAMS/TASK_INFO_TYPE", taskId)
			&& !xtree_query_val(pnCfg, L"TASK_ADDITIONAL_PARAMS/KLBL_TP_TASK_KEY", taskKey))
			continue;
		
		if (bRtpCustomSettings && taskName == L"KLOAS_TA_MONITOR_TASK")
			InheritOasSettings(pnCfg, (cOASSettings *)pProfilesList->settings(AVP_PROFILE_FILEMONITOR, cOASSettings::eIID), pAvsSet);
		else if (bRtpCustomSettings && taskName == L"KLMC_TA_MONITOR_TASK")
			InheritMcSettings(pnCfg, (cMCSettings*)pProfilesList->settings(AVP_PROFILE_MAILMONITOR, cMCSettings::eIID), pAvsSet);
		else if (taskName == L"KLODS_TA_SCAN_TASK")
		{
			if (taskId == L"ods-fullscan-async-id" || taskKey == L"scan-computer")
				InheritOdsSettings(pnCfg, pProfilesList->GetProfile(AVP_PROFILE_FULLSCAN), pAvsSet, false, pSink);
			else if (taskId == L"ods-scanonstartup-async-id")
				InheritOdsSettings(pnCfg, pProfilesList->GetProfile(AVP_PROFILE_SCAN_STARTUP), pAvsSet, false, pSink);
			else if (taskKey == L"scan-critical")
				InheritOdsSettings(pnCfg, pProfilesList->GetProfile("Scan_Critical_Areas"), pAvsSet, false, pSink);
			else if (taskId == L"unknown-async-id")
			{
				cStrObj taskDispName;
				if (xtree_query_val(pnCfg, L"TASK_INFO_PARAMS/DisplayName", taskDispName))
				{
					CProfile* pProfile = pProfilesList->GetProfile(AVP_PROFILE_SCANOBJECTS);
					if (!pProfile || !g_hTM 
						|| g_pProduct->IsProfilesLimit(pProfile->m_sType.c_str(cCP_ANSI)))
						continue;
					
					CProfile l_NewProfile; l_NewProfile.Assign(*pProfile);
					l_NewProfile.m_eOrigin = cProfileEx::poUser;
					l_NewProfile.m_sDescription = taskDispName;
					l_NewProfile.m_sName = AVP_PROFILE_SCANOBJECTS "_Imported";

					tCHAR str_id[10]; pr_sprintf( str_id, sizeof(str_id), "$%p", pmTask);
					l_NewProfile.m_sName.append(str_id);
					
					InheritOdsSettings(pnCfg, &l_NewProfile, pAvsSet, true, pSink);

					if (PR_FAIL(g_hTM->CloneProfile(NULL, &l_NewProfile, 0, *g_hGui)))
						continue;
				}
			}
		}
		else if (taskName == L"KLUPD_TA_UPDATE_TASK" && (taskId == L"update-async-id" || taskKey == L"update-bases"))
			InheritUpdaterSettings(pnCfg, pProfilesList->GetProfile(AVP_PROFILE_UPDATER), pProfilesList, pSink);
	}
}
#endif //_WIN32

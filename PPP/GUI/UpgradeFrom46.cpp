#include "StdAfx.h"
#if defined (_WIN32)
#include <structs/s_mc.h>
#include <structs/s_httpscan.h>
#include <structs/s_oas.h>
#include "wizards.h"

#define _DATAONLY_
#include "Kav46Settings/Windows/Personal 4.6/CC/LoadSettings/LoadKAV46Settings.h"
#include "Kav46Settings/Windows/Personal 4.6/CC/AVPMan/AVPAgent.exe/CCTypes.h" 
#include <assert.h>

namespace Kav46Settings
{

void InheritQbSettings(CProfile* pProfiles, const CUserSettings& settings)
{
	assert(pProfiles);
	if (!pProfiles)
		return;

	cBLSettings* pSet = (cBLSettings*) pProfiles->settings(AVP_PROFILE_PRODUCT, cBLSettings::eIID);
	if (!pSet)
		return;
	
	PR_TRACE((g_hGui, prtNOTIFY, "gui\tInherit QB settings"));

	pSet->m_bEnableInfoEvents = !settings.m_GUISettings.m_bOkInfoEventsOff;
	pSet->m_ReportSpan.m_on = settings.m_GUISettings.m_bRestrictReportLife;
	pSet->m_ReportSpan.m_val = settings.m_GUISettings.m_nReportOld;

	pSet->m_QBSpan.m_on = settings.m_BackupSettings.m_bMaxTimeOn;
	pSet->m_QBSpan.m_val = settings.m_BackupSettings.m_nMaxTime;

	cUpdaterSettings *pUpd = (cUpdaterSettings *) pProfiles->settings(AVP_PROFILE_UPDATER, cUpdaterSettings::eIID);
	if (pUpd && pUpd->m_pTaskBLSettings->isBasedOn(cUpdaterProductSpecific::eIID))
	{
		cUpdaterProductSpecific* p = (cUpdaterProductSpecific*) pUpd->m_pTaskBLSettings.ptr_ref();
		p->m_bRescanQuarantineAfterUpdate = settings.m_QuarantineSettings.m_bAutoScan;
	}
}

void InheritGuiSettings(CProfile* pProfiles, const CUserSettings& settings)
{
	assert(pProfiles);
	if (!pProfiles)
		return;
	
	cGuiSettings* pSet = (cGuiSettings*) pProfiles->settings(AVP_TASK_GUI, cGuiSettings::eIID);
	if (!pSet)
		return;
	
	PR_TRACE((g_hGui, prtNOTIFY, "gui\tInherit GUI settings"));

	pSet->m_bEnableTrayIconAnimation = settings.m_GUISettings.m_bIconAnimation;
}

void InheritAvsSettings(CProfile* pProfiles,
						const CUserSettings& settings,
						EKav46Type type,
						CKav46SettingsLoader* pLoader)
{
	assert(pProfiles);
	if (!pProfiles)
		return;
	
	cAVSSettings* pAvsSet = (cAVSSettings*) pProfiles->settings(AVP_SERVICE_AVS, cAVSSettings::eIID);
	if (!pAvsSet)
		return;

	PR_TRACE((g_hGui, prtNOTIFY, "gui\tInherit AVS settings"));

	pAvsSet->m_nDetectMask = 1 | (settings.m_AVSettings.m_AVDBType != CAVSettings::eNormal ? 6 : 0);

	if (type == Wks || type == Sos)
	{
		for (size_t i = 0; i < settings.m_ExcludeList.m_RiskwareList.size(); ++i)
		{
			const CRiskware& rw = settings.m_ExcludeList.m_RiskwareList[i];

			cDetectExclude exclude;
			exclude.m_nTriggers = cDetectExclude::fObjectMask | cDetectExclude::fTaskList;
			exclude.m_Object.m_bRecurse = rw.m_sMask.empty()
				|| *rw.m_sMask.rbegin() == '\\' || *rw.m_sMask.rbegin() == '/';
			exclude.m_Object.m_strMask = rw.m_sMask.c_str();
			if (!rw.m_sVerdict.empty())
			{
				exclude.m_nTriggers |= cDetectExclude::fVerdictMask;
				exclude.m_strVerdictMask = rw.m_sVerdict.c_str();
			}

			if (pLoader->IsTaskUseRiskware(settings.m_ExcludeList, CC_TASK_UID_FULLSCAN, false))
				exclude.m_aTaskList.push_back(AVP_TASK_ONDEMANDSCANNER);
			if (pLoader->IsTaskUseRiskware(settings.m_ExcludeList, CC_TASK_UID_ON_ACCESS, false))
				exclude.m_aTaskList.push_back(AVP_TASK_ONACCESSSCANNER);
			if (pLoader->IsTaskUseRiskware(settings.m_ExcludeList, CC_TASK_UID_MAILCHECKER, false))
				exclude.m_aTaskList.push_back(AVP_TASK_MAILCHECKER);

			if (!exclude.m_aTaskList.empty())
				pAvsSet->m_aExcludes.push_back(exclude);
		}
	}
	
	for (size_t i = 0; i < settings.m_ExcludeList.m_List.size(); ++i)
	{
		const CExclude& excl = settings.m_ExcludeList.m_List[i];
		if (excl.m_Owner != ExcludeType_User)
			continue;
		
		cDetectExclude exclude;
		exclude.m_nTriggers = cDetectExclude::fObjectMask;
		exclude.m_Object.m_bRecurse = excl.m_sMask.empty()
			|| *excl.m_sMask.rbegin() == '\\' || *excl.m_sMask.rbegin() == '/';
		exclude.m_Object.m_strMask = excl.m_sMask.c_str();
		if (!excl.m_sVerdict.empty())
		{
			exclude.m_nTriggers |= cDetectExclude::fVerdictMask;
			exclude.m_strVerdictMask = excl.m_sVerdict.c_str();
		}

		if (!excl.m_bAllTasks)
		{
			exclude.m_nTriggers |= cDetectExclude::fTaskList;
			
			if (pLoader->IsTaskUseIt(settings.m_ExcludeList, excl, CC_TASK_UID_FULLSCAN, false))
				exclude.m_aTaskList.push_back(AVP_TASK_ONDEMANDSCANNER);
			if (pLoader->IsTaskUseIt(settings.m_ExcludeList, excl, CC_TASK_UID_ON_ACCESS, false))
				exclude.m_aTaskList.push_back(AVP_TASK_ONACCESSSCANNER);
			if (pLoader->IsTaskUseIt(settings.m_ExcludeList, excl, CC_TASK_UID_MAILCHECKER, false))
				exclude.m_aTaskList.push_back(AVP_TASK_MAILCHECKER);
			if (!exclude.m_aTaskList.empty())
				pAvsSet->m_aExcludes.push_back(exclude);
		}
		else
			pAvsSet->m_aExcludes.push_back(exclude);
	}
}

void InheritExcludeProcessSettings(CProfile* pProfiles, const CUserSettingsOAS& oasset)
{
	assert(pProfiles);
	if (!pProfiles)
		return;
	
	cBLTrustedApps* pSet = (cBLTrustedApps*) pProfiles->settings(AVP_SERVICE_PROCESS_MONITOR, cBLTrustedApps::eIID);
	if (!pSet)
		return;

	PR_TRACE((g_hGui, prtNOTIFY, "gui\tInherit ExcludeProcess settings"));

	for (size_t i = 0; i < oasset.m_ExcludedProcesses.m_Processes.size(); ++i)
	{
		cBLTrustedApp tapp;
		tapp.m_bEnabled = oasset.m_bEnableExcludedProcesses;
		tapp.m_sImagePath = oasset.m_ExcludedProcesses.m_Processes[i].m_ProcessDir.c_str();
		tapp.m_sImagePath.check_last_slash(true);
		tapp.m_sImagePath += oasset.m_ExcludedProcesses.m_Processes[i].m_ProcessName.c_str();
		tapp.m_Hash = 0; // требуется пересчёт		
		
		pSet->m_aItems.push_back(tapp);
	}
}

void InheritOasSettings(const CUserSettingsOAS& oasset,
						cOASSettings* pSet,
						CProfile* pOasPrf,
						CKav46SettingsLoader* pLoader)
{
	assert(pSet && pOasPrf);
	if (!pSet || !pOasPrf)
		return;

	PR_TRACE((g_hGui, prtNOTIFY, "gui\tInherit OAS settings"));
	
	if (pLoader->PPro_IsMaximalProtection(oasset))
		pOasPrf->ResetSettingsLevel(3);
	else if (pLoader->PPro_IsRecommended(oasset))
		pOasPrf->ResetSettingsLevel(SETTINGS_LEVEL_DEFAULT);
	else if (pLoader->PPro_IsMaximalSpeed(oasset))
		pOasPrf->ResetSettingsLevel(2);
	else
	{
		pOasPrf->ResetSettingsLevel(SETTINGS_LEVEL_CUSTOM);
		switch (oasset.m_eScanType)
		{
		case eScanType_All:
			pSet->m_nScanFilter = SCAN_FILTER_ALL_OBJECTS;
			break;
		case eScanType_Infectable:
			pSet->m_nScanFilter = SCAN_FILTER_ALL_INFECTABLE;
			break;
		case eScanType_InfectableExt:
			pSet->m_nScanFilter = SCAN_FILTER_ALL_EXTENSION;
			break;
		}
		
		pSet->m_bScanArchived = oasset.m_bScanPacked;
		pSet->m_bScanSFXArchived = oasset.m_bScanSfxArchived;
		pSet->m_bScanOLE = oasset.m_bScanOle;
		
		for (size_t i = 0; i < pSet->m_aScanObjects.size(); ++i)
		{
			switch (pSet->m_aScanObjects[i].m_nObjType)
			{
			case OBJECT_TYPE_ALL_FIXED_DRIVES:
				pSet->m_aScanObjects[i].m_bEnabled = oasset.m_bScanFixed;
				break;
			case OBJECT_TYPE_ALL_REMOVABLE_DRIVES:
				pSet->m_aScanObjects[i].m_bEnabled = oasset.m_bScanRemovable;
				break;
			case OBJECT_TYPE_ALL_NETWORK_DRIVES:
				pSet->m_aScanObjects[i].m_bEnabled = oasset.m_bScanNetwork;
				break;
			}
		}
	}
	
	switch (oasset.m_eUserAction)
	{
	case CUserSettingsOAS::eUserAction_DenyAccessAskUser:
		pSet->m_nScanAction = SCAN_ACTION_ASKUSER;
		break;
	case CUserSettingsOAS::eUserAction_DenyAccessDoNotAskUser:
		pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
		pSet->m_bTryDisinfect = false;
		pSet->m_bTryDelete = false;
		break;
	case CUserSettingsOAS::eUserAction_AutoDel:
		pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
		pSet->m_bTryDisinfect = false;
		pSet->m_bTryDelete = true;
		break;
	case CUserSettingsOAS::eUserAction_Recommended:
	case CUserSettingsOAS::eUserAction_CureDeleteIfNotCured:
		pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
		pSet->m_bTryDisinfect = true;
		pSet->m_bTryDelete = true;
		break;
	}
}

void InheritMcSettings(const CUserSettingsMC& mcset,
					   cMCSettings* pSet,
					   CProfile* pMcPrf,
					   CKav46SettingsLoader* pLoader)
{
	assert(pSet && pMcPrf);
	if (!pSet || !pMcPrf)
		return;

	PR_TRACE((g_hGui, prtNOTIFY, "gui\tInherit MC settings"));
	
	if (pLoader->PPro_IsMaximalProtection(mcset))
		pMcPrf->ResetSettingsLevel(3);
	else if (pLoader->PPro_IsRecommended(mcset))
		pMcPrf->ResetSettingsLevel(SETTINGS_LEVEL_DEFAULT);
	else if (pLoader->PPro_IsMaximalSpeed(mcset))
		pMcPrf->ResetSettingsLevel(2);
	else
	{
		pMcPrf->ResetSettingsLevel(SETTINGS_LEVEL_CUSTOM);
		pSet->m_bCheckIncomingMessagesOnly = !mcset.m_bScanPop3;
		pSet->m_bScanArchived = mcset.m_bScanArchives;
		pSet->m_nTimeLimit.m_on = mcset.m_bScanTimeLimit;
		pSet->m_nTimeLimit.m_val = mcset.m_nMaxScanTimeSec;
	}
	
	switch (mcset.m_eInfectedMailAction)
	{
	case CUserSettingsMC::eMCInfectedAction_Disinfect:
		pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
		pSet->m_bTryDisinfect = true;
		pSet->m_bTryDelete = true;
		break;
	case CUserSettingsMC::eMCInfectedAction_Delete:
		pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
		pSet->m_bTryDisinfect = false;
		pSet->m_bTryDelete = true;
		break;
	}
}

void InheritRtpSettings(CProfile* pProfiles,
						const CUserSettingsRTP& rtpset,
						EKav46Type type,
						CKav46SettingsLoader* pLoader)
{
	if (type == Sos)
		return;

	assert(pProfiles);
	if (!pProfiles)
		return;

	PR_TRACE((g_hGui, prtNOTIFY, "gui\tInherit RTP settings"));

	bool bCustom = false;

	CProfile* pProtectionPrf = pProfiles->GetProfile(AVP_PROFILE_PROTECTION);
	CProfile* pOasPrf = pProfiles->GetProfile(AVP_PROFILE_FILEMONITOR);
	CProfile* pMcPrf = pProfiles->GetProfile(AVP_PROFILE_MAILMONITOR);
	CProfile* pWebPrf = pProfiles->GetProfile(AVP_PROFILE_WEBMONITOR);
	CProfile* pOgPrf = pProfiles->GetProfile(AVP_PROFILE_OFFICEGUARD);

	/* Нефига не работает, где-то после upgrade эти настройки перебиваются.
	pProtectionPrf->Enable(rtpset.m_bRTPDisable);
	pOasPrf->Enable(!rtpset.m_bMonitorDisable);	
	pMcPrf->Enable(!rtpset.m_bMCDisable);	
	pWebPrf->Enable(!rtpset.m_bSCDisable);	
	pOgPrf->Enable(!rtpset.m_bOGDisable);
	*/

	if (type == Personal)
	{
		switch (rtpset.m_eScanLevel)
		{
		case CUserSettingsRTP::eScanLevel_Max:
			if (pOasPrf) pOasPrf->ResetSettingsLevel(3);
			if (pMcPrf) pMcPrf->ResetSettingsLevel(3);
			if (pWebPrf) pWebPrf->ResetSettingsLevel(3);
			break;
			
		case CUserSettingsRTP::eScanLevel_Rec:
			if (pOasPrf) pOasPrf->ResetSettingsLevel(SETTINGS_LEVEL_DEFAULT);
			if (pMcPrf) pMcPrf->ResetSettingsLevel(SETTINGS_LEVEL_DEFAULT);
			if (pWebPrf) pWebPrf->ResetSettingsLevel(SETTINGS_LEVEL_DEFAULT);
			break;
			
		case CUserSettingsRTP::eScanLevel_Min:
			if (pOasPrf) pOasPrf->ResetSettingsLevel(2);
			if (pMcPrf) pMcPrf->ResetSettingsLevel(2);
			if (pWebPrf) pWebPrf->ResetSettingsLevel(2);
			break;
		}
	}
	else
	{
		InheritOasSettings(rtpset.m_OAS,
			(cOASSettings *)pProfiles->settings(AVP_PROFILE_FILEMONITOR, cOASSettings::eIID),
			pOasPrf,
			pLoader);

		InheritMcSettings(rtpset.m_MC,
			(cMCSettings*)pProfiles->settings(AVP_PROFILE_MAILMONITOR, cMCSettings::eIID),
			pMcPrf,
			pLoader);

		// TODO: IDS
	}
}

void InheritSchedule(const CScheduleSettings& sch46, cTaskSchedule& schedule)
{
	if (!sch46.m_bUseSchedule)
		schedule.m_eMode = schmManual;
	else
	{
		schedule.m_eMode = schmEnabled;
		
		tm t;
		t.tm_isdst = 0;
		t.tm_year = sch46.m_stLocalStartTime.wYear - 1900;
		t.tm_mon = sch46.m_stLocalStartTime.wMonth - 1;
		t.tm_mday = sch46.m_stLocalStartTime.wDay;
		t.tm_hour = sch46.m_stLocalStartTime.wHour;
		t.tm_min = sch46.m_stLocalStartTime.wMinute;
		t.tm_sec = sch46.m_stLocalStartTime.wSecond;
		schedule.m_nFirstRunTime = mktime(&t) - _timezone;
		
		switch (sch46.m_eScanInterval)
		{
		case CScheduleSettings::eOnStartup:
			schedule.m_eType = schtOnStartup;
			break;
		case CScheduleSettings::eEveryHour:
			schedule.m_nHours = sch46.m_nFrequent;
			schedule.m_eType = schtHourly;
			break;
		case CScheduleSettings::eEveryDay:
			schedule.m_eType = schtDaily;
			schedule.m_eEvery = schEveryDays;
			schedule.m_nDays = sch46.m_nFrequent;
			schedule.m_bUseTime = cTRUE;
			break;
		case CScheduleSettings::eEveryWeek:
			schedule.m_eType = schtWeekly;
			schedule.m_bWeekDays[0] = sch46.m_WeekDays.bMonday;
			schedule.m_bWeekDays[1] = sch46.m_WeekDays.bTuesday;
			schedule.m_bWeekDays[2] = sch46.m_WeekDays.bWednesday;
			schedule.m_bWeekDays[3] = sch46.m_WeekDays.bThursday;
			schedule.m_bWeekDays[4] = sch46.m_WeekDays.bFriday;
			schedule.m_bWeekDays[5] = sch46.m_WeekDays.bSaturday;
			schedule.m_bWeekDays[6] = sch46.m_WeekDays.bSunday;
			schedule.m_bUseTime = cTRUE;
			break;
		}
	}
}

void InheritOdsSettings(const CUserSettingsODS& odsset,
						const CScheduleSettings* pSchedule,
						CProfile* pPrf,
						const CCODSScanObjects* pObjectsToScan,
						CRootSink* pSink,
						EKav46Type type,
						CKav46SettingsLoader* pLoader,
						const CLogonCredentials* pLogon)
{
	if (!pPrf)
		return;

	cODSSettings* pSet = (cODSSettings*) pPrf->settings();
	if (!pSet || pSet->getIID() != cODSSettings::eIID)
		return;

	PR_TRACE((g_hGui, prtNOTIFY, "gui\tInherit ODS settings"));

	if (type == Personal)
	{
		switch (odsset.m_ScanLevel)
		{
		case CUserSettingsODS::eScanLevel_Max: pPrf->ResetSettingsLevel(3); break;			
		case CUserSettingsODS::eScanLevel_Rec: pPrf->ResetSettingsLevel(SETTINGS_LEVEL_DEFAULT); break;			
		case CUserSettingsODS::eScanLevel_Min: pPrf->ResetSettingsLevel(2); break;
		}
		pSet->m_bScanArchived = odsset.m_blScanArchives;
		pSet->m_bScanPlainMail = odsset.m_blScanMail;
		pSet->m_bAskPassword = !odsset.m_bDoNotAskPassword;
	}
	else
	{
		if (pLoader->PPro_IsMaximalProtection(odsset))
			pPrf->ResetSettingsLevel(3);
		else if (pLoader->PPro_IsRecommended(odsset))
			pPrf->ResetSettingsLevel(SETTINGS_LEVEL_DEFAULT);
		else if (pLoader->PPro_IsMaximalSpeed(odsset))
			pPrf->ResetSettingsLevel(2);
		else
		{
			pPrf->ResetSettingsLevel(SETTINGS_LEVEL_CUSTOM);
			
			switch (odsset.m_eScanType)
			{
			case eScanType_All:
				pSet->m_nScanFilter = SCAN_FILTER_ALL_OBJECTS;
				break;
			case eScanType_Infectable:
				pSet->m_nScanFilter = SCAN_FILTER_ALL_INFECTABLE;
				break;
			case eScanType_InfectableExt:
				pSet->m_nScanFilter = SCAN_FILTER_ALL_EXTENSION;
				break;
			}
			
			pSet->m_nTimeLimit.m_on = odsset.m_bUseTimeLimit;
			pSet->m_nTimeLimit.m_val = odsset.m_dwTimeLimit;
			pSet->m_nSizeLimit.m_on = odsset.m_bUseSizeLimit;
			pSet->m_nSizeLimit.m_val = odsset.m_dwSizeLimit / (1024*1024);
			pSet->m_bScanArchived = odsset.m_blScanArchives;
			pSet->m_bScanPlainMail = odsset.m_blScanMail;
			pSet->m_bAskPassword = !odsset.m_bDoNotAskPassword;
			pSet->m_bScanOLE = odsset.m_bScanOle;
			
			pSet->m_bUseIChecker = odsset.m_eUseIChecker != eICheckerType_Off;
			pSet->m_bUseIStreams = odsset.m_eUseIStreams != eICheckerType_Off;
		}
	}
	
	switch (odsset.m_UserAction)
	{
	case CUserSettingsODS::eUserAction_Ask:
		pSet->m_nScanAction = SCAN_ACTION_ASKUSER;
		break;
	case CUserSettingsODS::eUserAction_ReportOnly:
		pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
		pSet->m_bTryDisinfect = false;
		pSet->m_bTryDelete = false;
		break;
	case CUserSettingsODS::eUserAction_CureDeleteIfNotCured:
	case CUserSettingsODS::eUserAction_AutoRec:
		pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
		pSet->m_bTryDisinfect = true;
		pSet->m_bTryDelete = true;
		break;
	case CUserSettingsODS::eUserAction_AutoDel:
		pSet->m_nScanAction = SCAN_ACTION_DISINFECT;
		pSet->m_bTryDisinfect = false;
		pSet->m_bTryDelete = true;
		break;
	case CUserSettingsODS::eUserAction_TIF:
		pSet->m_nScanAction = SCAN_ACTION_POSTPONE;
		break;
	}

	if (pObjectsToScan)
	{
		pSet->m_bScanRemovable = false;
		pSet->m_bScanFixed = false;
		pSet->m_bScanNetwork = false;
		pSet->m_bScanSectors = false;
		pSet->m_bScanMemory = false;
		pSet->m_bScanStartup = false;
		pSet->m_bScanMail = false;
		pSet->m_aScanObjects.clear();
		
		
		for (std::list<CCODSScanObject>::const_iterator it = pObjectsToScan->m_List.begin()
			; it != pObjectsToScan->m_List.end(); ++it)
		{
			cScanObject obj;
			obj.m_strObjName = it->m_Name.c_str();
			obj.m_bEnabled = true;
			
			switch (it->m_Type)
			{
			case AVP_OT_MYCOMPUTER:
				obj.m_nObjType = OBJECT_TYPE_MY_COMPUTER;
				pSet->m_bScanFixed = true;
				pSet->m_bScanSectors = true;
				break;				
			case AVP_OT_HARDDRIVES:
				obj.m_nObjType = OBJECT_TYPE_ALL_FIXED_DRIVES;
				pSet->m_bScanFixed = true;
				break;
			case AVP_OT_REMOVABLE:
				obj.m_nObjType = OBJECT_TYPE_ALL_REMOVABLE_DRIVES;
				pSet->m_bScanRemovable = true;
				break;
			case AVP_OT_NETDRIVES:
				obj.m_nObjType = OBJECT_TYPE_ALL_NETWORK_DRIVES;
				pSet->m_bScanNetwork = true;
				break;
			case AVP_OT_BOOT_SECTORS:
				obj.m_nObjType = OBJECT_TYPE_BOOT_SECTORS;
				pSet->m_bScanSectors = true;
				break;
			case AVP_OT_MAIL:
				obj.m_nObjType = OBJECT_TYPE_MAIL;
				pSet->m_bScanMail = true;
				break;
			case AVP_OT_STARTUP:
				obj.m_nObjType = OBJECT_TYPE_STARTUP;
				pSet->m_bScanStartup = true;
				break;
			case AVP_OT_MEMORY:
				obj.m_nObjType = OBJECT_TYPE_MEMORY;
				pSet->m_bScanMemory = true;
				break;
			case AVP_OT_DRIVE:
				obj.m_nObjType = OBJECT_TYPE_DRIVE;
				obj.m_bRecursive = true;
				break;
			case AVP_OT_FOLDER:
				obj.m_nObjType = OBJECT_TYPE_FOLDER;
				obj.m_bRecursive = true;
				break;
			case AVP_OT_FILE:
				obj.m_nObjType = OBJECT_TYPE_FILE;
				obj.m_bRecursive = false;
				break;
			default:
				assert(!"Unsupported object type");
				continue;
				break;
			}
			pSet->m_aScanObjects.push_back(obj);
		}
	}		
	
	// Run as...
	if (pLogon)
	{
		cTaskRunAs& runas = pPrf->m_cfg.run_as();
		runas.m_enable = pLogon->m_bUseCredentials;
		runas.m_sAccount = (pLogon->m_Domain + (pLogon->m_Domain.empty() ? "" : "\\") + pLogon->m_UserName).c_str();

		char buf[1024];
		if (!pLogon->m_EncryptedPassword.empty()
			&& pLoader->DecryptPwd(&pLogon->m_EncryptedPassword.front(), pLogon->m_EncryptedPassword.size(), buf, sizeof(buf)))
		{
			runas.m_sPassword = buf;
			pSink->CryptDecrypt(runas.m_sPassword, true);
		}
		else
			runas.m_enable = cFALSE;
	}
	
	if (pSchedule)
		InheritSchedule(*pSchedule, pPrf->schedule());
}

void InheritUpdaterSettings(const CUserSettingsUpdater& updset,
							CProfile* pPrf,
							CRootSink* pSink,
							CKav46SettingsLoader* pLoader,
							const CLogonCredentials* pLogon)
{
	assert(pPrf);
	if (!pPrf)
		return;

	cUpdaterSettings* pSet = (cUpdaterSettings *) pPrf->settings(pPrf->m_sName.c_str(cCP_ANSI), cUpdaterSettings::eIID);
	if (!pSet)
		return;

	PR_TRACE((g_hGui, prtNOTIFY, "gui\tInherit Updater settings"));

	InheritSchedule(updset.m_TaskSchedule, pPrf->schedule());

	for (std::vector<CCUpdateSource>::const_iterator it = updset.m_UpdateSources.begin()
		, end = updset.m_UpdateSources.end(); it != end; ++it)
	{
		switch (it->m_SourceType)
		{
		case CCUpdateSource::UST_AKServer:
		case CCUpdateSource::UST_AKMServer:
			{
				cStringObj strProductType = g_hGui->ExpandEnvironmentString("%ProductType%");
				if (strProductType == "wks" || strProductType == "fs")
					pSet->m_blAKServerUse = it->m_bActive;
			}
			break;

		case CCUpdateSource::UST_KLServer:
			pSet->m_bUseInternalSources = it->m_bActive;
			break;
		case CCUpdateSource::UST_UserURL:
			cUpdaterSource src;
			src.m_bEnable = it->m_bActive;
			src.m_strPath = it->m_URL.c_str();
			pSet->m_aSources.push_back(src);
			break;
		}
	}
	
	// Retranslation
	pSet->m_bUseRetr = updset.m_retranslationModeOptions.m_enabled;
	if (!updset.m_retranslationModeOptions.m_folder.empty())
		pSet->m_strRetrPath = updset.m_retranslationModeOptions.m_folder.c_str();
	
	// Run as...
	if (pLogon)
	{
		cTaskRunAs& runas = pPrf->m_cfg.run_as();
		runas.m_enable = pLogon->m_bUseCredentials;
		runas.m_sAccount = (pLogon->m_Domain + (pLogon->m_Domain.empty() ? "" : "\\") + pLogon->m_UserName).c_str();

		char buf[1024];
		if (!pLogon->m_EncryptedPassword.empty()
			&& pLoader->DecryptPwd(&pLogon->m_EncryptedPassword.front(), pLogon->m_EncryptedPassword.size(), buf, sizeof(buf)))
		{
			runas.m_sPassword = buf;
			pSink->CryptDecrypt(runas.m_sPassword, true);
		}
		else
			runas.m_enable = cFALSE;
	}
}

void InheritProxySettings(CProfile* pProfiles,
						  const CUserSettingsUpdater& updset,
						  CRootSink* pSink,
						  CKav46SettingsLoader* pLoader)
{
	assert(pProfiles);
	if (!pProfiles)
		return;

	cBLSettings* pSet = (cBLSettings*) pProfiles->settings(AVP_PROFILE_PRODUCT, cBLSettings::eIID);
	if (!pSet)
		return;

	PR_TRACE((g_hGui, prtNOTIFY, "gui\tInherit proxy settings"));

	cProxySettings& proxy = pSet->m_ProxySettings;

	proxy.m_bUseProxy = cTRUE;
	proxy.m_bUseIEProxySettings = updset.m_ProxySettings.m_bAutoDetect;
	proxy.m_bypassProxyServerForLocalAddresses = cFALSE;
	proxy.m_strProxyHost = updset.m_ProxySettings.m_szHost.c_str();
	proxy.m_nProxyPort = updset.m_ProxySettings.m_sPort;

	const CLogonCredentials& proxyLogon = updset.m_ProxySettings.m_LogonCredentials;
	proxy.m_bProxyAuth = proxyLogon.m_bUseCredentials;
	proxy.m_strProxyLogin = (proxyLogon.m_Domain + (proxyLogon.m_Domain.empty() ? "" : "\\")
		+ proxyLogon.m_UserName).c_str();

	char proxyPwd[1024];
	if (!proxyLogon.m_EncryptedPassword.empty()
		&& pLoader->DecryptPwd(&proxyLogon.m_EncryptedPassword.front(), proxyLogon.m_EncryptedPassword.size(), proxyPwd, sizeof(proxyPwd)))
	{
		proxy.m_strProxyPassword = proxyPwd;
		pSink->CryptDecrypt(proxy.m_strProxyPassword, true);
	}
}

}

void InheritOld46Configuration(CProfile * pProfilesList,
							   const cStrObj& sPolicyDat,
							   EKav46Type type,
							   CRootSink* pSink)
{
	using namespace Kav46Settings;

	CKav46SettingsLoader* pLoader;

	typedef void* (*pfnGetLoader)();
	typedef void (*pfnFreeLoader)(void*);

	PR_TRACE((g_hGui, prtIMPORTANT, "gui\tInherit of 4.6 settings begin"));

	HMODULE hLib = LoadLibrary(_T("Load46St.dll"));
	if (hLib == NULL)
	{
		PR_TRACE((g_hGui, prtERROR, "gui\tFailed to load Load46St.dll"));
		return;
	}

	pfnGetLoader GetLoader = (pfnGetLoader) GetProcAddress(hLib, "GetLoader");
	pfnFreeLoader FreeLoader = (pfnFreeLoader) GetProcAddress(hLib, "FreeLoader");
	if (!GetLoader || !FreeLoader || !(pLoader = (CKav46SettingsLoader*) GetLoader()))
	{
		FreeLibrary(hLib);
		return;
	}

	CSettings* pSettings = pLoader->LoadKav46Settings(sPolicyDat.c_str(cCP_ANSI));

	if (!pSettings)
	{
		PR_TRACE((g_hGui, prtERROR, "gui\tFailed to load settings"));
		FreeLoader(pLoader);
		FreeLibrary(hLib);
		return;
	}

	InheritRtpSettings(pProfilesList, pSettings->m_UserSettings.m_RTPSettings, type, pLoader);
	InheritQbSettings(pProfilesList, pSettings->m_UserSettings);
	InheritGuiSettings(pProfilesList, pSettings->m_UserSettings);
	InheritAvsSettings(pProfilesList, pSettings->m_UserSettings, type, pLoader);
	if (type != Personal && type != Sos)
		InheritExcludeProcessSettings(pProfilesList, pSettings->m_UserSettings.m_RTPSettings.m_OAS);

	if (type == Personal || type == Pro)
	{
		InheritUpdaterSettings(pSettings->m_UserSettings.m_UpdaterSettings, pProfilesList->GetProfile(AVP_PROFILE_UPDATER), pSink, pLoader, NULL);
		InheritProxySettings(pProfilesList, pSettings->m_UserSettings.m_UpdaterSettings, pSink, pLoader);
	}
	
	InheritOdsSettings(pSettings->m_UserSettings.m_ODSSettings, NULL, pProfilesList->GetProfile(AVP_PROFILE_SCANOBJECTS), NULL, pSink, type, pLoader, NULL);
	if (type == Personal)
	{
		InheritOdsSettings(pSettings->m_UserSettings.m_ODSSettings,
			&pSettings->m_UserSettings.m_ScheduleSettings, pProfilesList->GetProfile(AVP_PROFILE_FULLSCAN), NULL, pSink, type, pLoader, NULL);
		InheritOdsSettings(pSettings->m_UserSettings.m_ODSSettings, NULL, pProfilesList->GetProfile(AVP_PROFILE_SCAN_STARTUP), NULL, pSink, type, pLoader, NULL);
	}
	else
	{
		for (int i = 0; i < pLoader->GetSize(pSettings->m_arTasks); ++i)
		{
			const CTask& task = pLoader->GetAt(pSettings->m_arTasks, i);
			const std::string& sTaskName = pLoader->GetNameStdStr(task);
			if (task.m_eTaskType == CTask::eUserTask)
			{
				if (sTaskName == CC_TASK_UID_FULLSCAN)
					InheritOdsSettings(pLoader->GetBaseODSSettings(task.m_UserTaskSettings)
						, pLoader->GetBaseScheduleSettings(task.m_UserTaskSettings)
						, pProfilesList->GetProfile(AVP_PROFILE_FULLSCAN), NULL, pSink, type
						, pLoader, &task.m_LogonCredentials);
				else if (sTaskName == CC_TASK_UID_STARTUPSCAN)
					InheritOdsSettings(pLoader->GetBaseODSSettings(task.m_UserTaskSettings)
						, pLoader->GetBaseScheduleSettings(task.m_UserTaskSettings)
						, pProfilesList->GetProfile(AVP_PROFILE_SCAN_STARTUP), NULL, pSink, type
						, pLoader, &task.m_LogonCredentials);
				else if (sTaskName == CC_TASK_UID_CRITICALAREASSCAN)
					InheritOdsSettings(pLoader->GetBaseODSSettings(task.m_UserTaskSettings)
						, pLoader->GetBaseScheduleSettings(task.m_UserTaskSettings)
						, pProfilesList->GetProfile("Scan_Critical_Areas"), NULL, pSink, type
						, pLoader, &task.m_LogonCredentials);
				else if (sTaskName == CC_TASK_UID_QUARANTINESCAN)
				{
					// эти задачи не будем импортировать
				}
				else if (!task.m_UserTaskSettings.m_bAdminTask)
				{
					CProfile* pProfile = pProfilesList->GetProfile(AVP_PROFILE_SCANOBJECTS);
					if (!pProfile || !g_hTM 
						|| g_pProduct->IsProfilesLimit(pProfile->m_sType.c_str(cCP_ANSI)))
						continue;
					
					CProfile l_NewProfile; l_NewProfile.Assign(*pProfile);
					l_NewProfile.m_eOrigin = cProfileEx::poUser;
					l_NewProfile.m_sDescription = task.m_UserTaskSettings.m_sUserTaskName.c_str();
					l_NewProfile.m_sName = AVP_PROFILE_SCANOBJECTS "_Imported";

					tCHAR str_id[10]; pr_sprintf( str_id, sizeof(str_id), "$%p", &task);
					l_NewProfile.m_sName.append(str_id);

					CCODSScanObjects* pObjectsToScan = !task.m_StartSettings.m_Param.empty()
						? pLoader->GetScanObjects(&task.m_StartSettings.m_Param.front(), task.m_StartSettings.m_Param.size())
						: NULL;
					
					InheritOdsSettings(pLoader->GetBaseODSSettings(task.m_UserTaskSettings)
						, pLoader->GetBaseScheduleSettings(task.m_UserTaskSettings)
						, &l_NewProfile, pObjectsToScan, pSink, type, pLoader
						, &task.m_LogonCredentials);

					pLoader->FreeScanObjects(pObjectsToScan);

					if (PR_FAIL(g_hTM->CloneProfile(NULL, &l_NewProfile, 0, *g_hGui)))
						continue;
				}				
			}
			else if (task.m_eTaskType == CTask::eUpdaterTask && sTaskName == CC_TASK_UID_UPDATER)
			{
				InheritUpdaterSettings(task.m_UpdaterTaskSettings.m_BaseUpdaterSettings
					, pProfilesList->GetProfile(AVP_PROFILE_UPDATER), pSink, pLoader, &task.m_LogonCredentials);
				InheritProxySettings(pProfilesList, task.m_UpdaterTaskSettings.m_BaseUpdaterSettings, pSink, pLoader);
			}
		}
	}

	pLoader->FreeSettings(pSettings);
	FreeLoader(pLoader);
	FreeLibrary(hLib);
}
#endif // _WIN32

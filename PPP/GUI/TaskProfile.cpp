// TaskProfile.cpp: implementation of the CTaskProfile1 class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TaskProfile.h"

cTaskManager * g_hTM = NULL;

//////////////////////////////////////////////////////////////////////
// CProfile

CProfile::CProfile() :
	m_bSettingsDirty(0),
	m_bOriginalInited(0),
	m_pParent(NULL),
	m_pRoot(NULL),
	m_pData(NULL),
	m_pOriginal(NULL),
	m_bSettingsExist(2),
	m_bStatisticsExist(2),
	m_bCustom(0)
{
}

CProfile::~CProfile()
{
	clear();
	if( m_pOriginal )
		m_pOriginal->release(), m_pOriginal = NULL;
}

//////////////////////////////////////////////////////////////////////

tERROR CProfile::SendMsg(tDWORD nMsgClass, tDWORD nMsgId, cSerializable* pData, bool fAsync)
{
	return g_hTM ? g_hTM->sysSendMsg(nMsgClass, nMsgId, NULL, pData, SER_SENDMSG_PSIZE) : errOK;
}

bool CProfile::Enable(bool bEnable)
{
	return g_hTM ? PR_SUCC(g_hTM->EnableProfile(m_sName.c_str(cCP_TCHAR), bEnable ? cTRUE : cFALSE, cFALSE, g_hGui->Obj())) : false;
}

bool CProfile::SetState(tTaskRequestState nState, bool bPrevId, tERROR * pErr)
{
	if( !g_hTM )
		return false;
	
	if( nState == TASK_REQUEST_RUN && statistics() && !isProtection() )
	{
		cSerializableObj pStatInit;
		pStatInit.init(statistics()->getIID());
		if( pStatInit )
			statistics()->assign(*pStatInit, true);
	}

	tERROR err = g_hTM->SetProfileState(m_sName.c_str(cCP_TCHAR), nState, g_hGui->Obj(), bPrevId ? cREQUEST_RESTART_PREVIOUS_TASK : 0/*, &m_nRuntimeId*/);
	if( pErr )
		*pErr = err;
	return PR_SUCC(err);
}

tERROR CProfile::SetResumeTime(tDWORD nResumeTime)
{
	if( !nResumeTime )
		return errOK;
	
	if( !g_hTM )
		return errNOT_OK;
	return g_hTM->SetProfileState(m_sName.c_str(cCP_TCHAR), TASK_REQUEST_RUN, g_hGui->Obj(), nResumeTime, NULL);
}

tERROR CProfile::SetInfo(cSerializable* pInfo)
{
	return g_hTM ? g_hTM->SetProfileInfo(m_sName.size() ? (LPCSTR)m_sName.c_str(cCP_TCHAR) : NULL, pInfo, g_hGui->Obj(), 0) : errUNEXPECTED;
}

bool CProfile::GetInfo(cSerializable* pInfo, tGetProfileInfoSource eSrc/*, bool bFixRtId*/)
{
	if( !g_hTM )
		return false;
	
	if( PR_FAIL(g_hTM->GetProfileInfo(m_sName.c_str(cCP_TCHAR), pInfo)) )
		return false;
	
	return true;
}

tERROR CProfile::AskAction(tDWORD action_id, cSerializable* pInfo)
{
	return g_hTM ? g_hTM->AskTaskAction(m_nRuntimeId, action_id, pInfo) : errUNEXPECTED;
}

bool CProfile::GetSettingsByLevelTo(tDWORD lev, cSerializable * pSettings)
{
	if( !g_hTM )
		return false;
	return PR_SUCC(g_hTM->GetSettingsByLevel(m_sName.c_str(cCP_TCHAR), lev, cFALSE, pSettings));
}

bool CProfile::GetSettingsByLevel(tDWORD lev, bool bRecursive)
{
	if( !g_hTM )
		return false;

	if(bRecursive)
		for(tDWORD i = 0; i < m_aChildren.size(); i++)
			GetChild(i)->GetSettingsByLevel(lev, bRecursive);

	settings();
	cCfg _cfg(*this);
	if( !GetSettingsByLevelTo(lev, _cfg.m_settings) )
		return false;

	Assign(_cfg);
	
	m_cfg.m_dwLevel = lev == SETTINGS_LEVEL_RESET ? SETTINGS_LEVEL_DEFAULT : lev;
	return true;
}

bool CProfile::Update(bool bFull)
{
	cProfileEx l_Profile;
	if( !bFull )
		l_Profile.flags(l_Profile.flags() & ~(cCfg::fSettings|cProfile::fStatistics));
	l_Profile.m_sName = m_sName;
	GetInfo(&l_Profile, gpi_FROM_BOTH);

	return Update(&l_Profile);
}

void CProfile::UpdateSchedulTime(bool bRecursive)
{
	cTaskSchedule& pSch = schedule();

	if( !persistent() &&
		!pSch.m_bUseTime &&
		//pSch.m_eMode != schmAuto &&
		( pSch.m_eType == schtDaily ||
		  pSch.m_eType == schtWeekly ||
		  pSch.m_eType == schtMonthly ) )
	{
		pSch.m_nFirstRunTime = cDateTime::now_local();
		SetSettingsDirty();
	}

	if( !bRecursive )
		return;

	for(DWORD i = 0; i < GetChildrenCount(); i++)
		GetChild(i)->UpdateSchedulTime(bRecursive);
}

//////////////////////////////////////////////////////////////////////

void CProfile::OnPrepareSettings()
{
	GetInfo(m_cfg.m_settings, gpi_FROM_BOTH);
}

void CProfile::OnPrepareStatistics()
{
	GetInfo(m_statistics, gpi_FROM_BOTH);
}

//////////////////////////////////////////////////////////////////////

bool CProfile::IsEnableDirtyOff(bool bRecursive)
{
	if( !m_pOriginal )
		return false;
	
	if( m_pOriginal->m_cfg.m_bEnabled != m_cfg.m_bEnabled )
		return !m_cfg.m_bEnabled;

	if( bRecursive )
	{
		for(DWORD i = 0; i < GetChildrenCount(); i++)
			if( GetChild(i)->IsEnableDirtyOff(bRecursive) )
				return true;
	}

	return false;
}

void CProfile::SetSettingsDirty(bool bDirty, bool bRecursive)
{
	m_bSettingsDirty = bDirty;
		
	if( bRecursive )
	{
		for(DWORD i = 0; i < GetChildrenCount(); i++)
			GetChild(i)->SetSettingsDirty(bDirty, bRecursive);
	}
}

bool CProfile::GetSettings()
{
	cCfg l_Cfg;
	if( !GetInfo(&l_Cfg) )
		return false;
	
	return Assign(l_Cfg);
}

static void _StoreSettings_Trace(cCfgEx& _Cfg, bool bRecursive)
{
	PR_TRACE((g_root, prtIMPORTANT, "gui\tprofile \"%S\": enabled(%u)", _Cfg.m_sName.data(), _Cfg.enabled()));
	
// 	if( _Cfg.m_settings )
// 	{
// 		if( _Cfg.m_settings->isBasedOn(cPDMSettings::eIID) )
// 			PR_TRACE((g_root, prtIMPORTANT, "gui\tprofile \"%S\": bRegMonitoring_Enabled(%u)", _Cfg.m_sName.data(), ((cPDMSettings *)_Cfg.m_settings.ptr_ref())->m_bRegMonitoring_Enabled));
// 	}
	
	if( bRecursive )
	{
		for(tDWORD i = 0; i < _Cfg.m_aChildren.size(); i++)
			_StoreSettings_Trace(_Cfg.m_aChildren[i], bRecursive);
	}
}

tERROR CProfile::StoreSettings(bool bRecursive)
{
	tERROR l_err;
	cCfgEx l_Cfg;

	if( bRecursive )
	{
		if( l_err = l_Cfg.assignProfile(*this, true) )
			return l_err;
	}
	else
	{
		if( !isSettingsDirty() )
			return warnFALSE;
		
		(cCfg&)l_Cfg = m_cfg;
		l_Cfg.m_sName.assign(m_sName);
		l_Cfg.m_sType.assign(m_sType);
	}

	PR_TRACE((g_root, prtIMPORTANT, "gui\tStoreSettings begin..."));
	_StoreSettings_Trace(l_Cfg, bRecursive);
	
	l_err = SetInfo(&l_Cfg);
	
	PR_TRACE((g_root, prtIMPORTANT, "gui\tStoreSettings end with code %terr", l_err));
	SetSettingsDirty(false, bRecursive);
	return l_err;
}

bool CProfile::GetData()
{
	cProfile l_Info;
	if( !GetInfo(&l_Info) )
		return false;

	return Assign(l_Info);
}

void CProfile::Disconnect()
{
	for(tDWORD i = 0; i < m_aChildren.size(); i++)
		GetChild(i)->Disconnect();
		
	m_nState = PROFILE_STATE_ENABLED;
	g_pRoot->SendEvent(pmPROFILE_STATE_CHANDED, this);
}

void CProfile::ResetSettingsLevelTo(tDWORD lev, cCfg& _Cfg)
{
	settings();
	
	if( &_Cfg != &m_cfg )
	{
		_Cfg = m_cfg;
		_Cfg.m_nFlags = m_cfg.m_nFlags;
	}
	
	_Cfg.m_dwLevel = lev;
	GetSettingsByLevelTo(lev, &_Cfg);

	if( lev == SETTINGS_LEVEL_RESET )
	{
		cSerializable * pSett = _Cfg.m_settings;
		if( pSett && pSett->isBasedOn(cTaskSettingsPerUser::eIID) )
		{
			cSerializable * pDefUserSett = ((cTaskSettingsPerUser *)pSett)->settings_byuser(cStringObj());
			cSerializable * pUserSett = GetUserSettings(pSett);
			if( pDefUserSett && pUserSett )
				pUserSett->assign(*pDefUserSett, false);
		}
	}
}

bool CProfile::ResetSettingsLevel(tDWORD lev, bool bRecursive)
{
	if( bRecursive )
		for(tDWORD i = 0; i < m_aChildren.size(); i++)
			GetChild(i)->ResetSettingsLevel(lev, bRecursive);

	ResetSettingsLevelTo(lev, m_cfg);
	if( lev == SETTINGS_LEVEL_RESET )
		UpdateSchedulTime(false);
	
	SetSettingsDirty();
	return true;
}

bool CProfile::Assign(const cProfile& _Data)
{
	cSerializable * pSettOld = m_cfg.m_settings.relinquish();
	cSerializable * pStatOld = m_statistics.relinquish();
	
	cProfile::assign(_Data, false);
	
	{
		cSerializableObj l_New; l_New.ptr_ref() = m_cfg.m_settings.relinquish();
		m_cfg.m_settings.ptr_ref() = pSettOld;
		Assign(_Data.m_cfg);
	}

	if( pStatOld )
	{
		cSerializableObj l_New; l_New.ptr_ref() = m_statistics.relinquish();
		m_statistics.ptr_ref() = pStatOld;
		if( l_New )
			pStatOld->assign(*(cSerializable *)l_New, true);
	}
	
	return true;
}

bool CProfile::Assign(const cCfg& _Data)
{
	cSerializable * pOld = m_cfg.m_settings.relinquish();
	m_cfg.assign(_Data, true);
	
	if( m_pOriginal )
		UpdateSchedulTime(false);

	if( !pOld )
		return true;

	cSerializableObj l_New; l_New.ptr_ref() = m_cfg.m_settings.relinquish();
	m_cfg.m_settings.ptr_ref() = pOld;
	
	if( !l_New )
		return true;

	cSerializableObj pOldUserSett;
	if( m_cfg.m_settings->isBasedOn(cTaskSettingsPerUser::eIID) )
	{
		cTaskSettingsPerUserItem * pUsrItem = ((cTaskSettingsPerUser *)m_cfg.m_settings.ptr_ref())->item_byuser(g_hGui->GetUserId(), true);
		if( pUsrItem )
			pOldUserSett.ptr_ref() = pUsrItem->m_settings.relinquish();
	}
	
	m_cfg.m_settings->assign(*(cSerializable *)l_New, true);

	if( !pOldUserSett )
		return true;

	cTaskSettingsPerUserItem * pUsrItem = ((cTaskSettingsPerUser *)m_cfg.m_settings.ptr_ref())->item_byuser(g_hGui->GetUserId(), true);
	if( pUsrItem )
	{
		cSerializableObj pNewUserSett;
		pNewUserSett.assign(pUsrItem->m_settings, true);
		pUsrItem->m_settings.assign(pOldUserSett, true);

		if( pNewUserSett )
			pOldUserSett->assign(*(cSerializable *)pNewUserSett, true);
	}
	
	return true;
}

bool CProfile::AssignEx(CProfile * pProfile, bool bRecursive)
{
	if( pProfile->isSettingsDirty() )
	{
		Assign(pProfile->m_cfg);
		SetSettingsDirty(true);

		pProfile->SetSettingsDirty(false);
	}

	if( bRecursive )
	{
		for(tDWORD i = 0, n = pProfile->GetChildrenCount(); i < n; i++)
		{
			CProfile * pSrc = pProfile->GetChild(i);
			CProfile * pChild = GetProfile(pSrc->m_sName.c_str(cCP_ANSI));
			if( pChild )
				pChild->AssignEx(pSrc, true);
		}
	}
	
	return true;
}

bool CProfile::Update(const cProfileEx * pProfile)
{
	if( !pProfile )
		return false;
	
	if( !m_aChildren.size() )
		for(tDWORD i = 0; i < pProfile->m_aChildren.size(); i++)
			AddProfile();

	if( !Assign(*pProfile) )
		return false;
		
//	if( !m_cfg.m_settings )
//		m_cfg.m_settings.init(pProfile->m_nSerIdSettings);
	
//	if( !m_statistics )
//	{
//		tDWORD nStatId = pProfile->m_statistics ?
//			pProfile->m_statistics->getIID() : pProfile->m_nSerIdStatistics;
//		m_statistics.init(nStatId);
//	}
	
	m_aReports = pProfile->m_aReports;
	
	for(int i = 0, n = min(pProfile->m_aChildren.size(), m_aChildren.size()); i < n; i++)
		if( !GetChild(i)->Update(pProfile->m_aChildren.operator[](i)) )
			return false;

	return true;
}

bool CProfile::Copy(const CProfile * pProfile, bool bFull, bool bRecursive)
{
	if( !pProfile )
		return false;
	
	tDWORD nOldFlags = flags();
	if( !bFull )
		flags(nOldFlags & ~(cCfg::fSettings|cProfile::fStatistics));
	bool bRes = Assign(*pProfile);
	flags(nOldFlags);
	
	if( !bRes )
		return false;
	m_aReports = pProfile->m_aReports;
	m_pOriginal = (CProfile *)pProfile, m_pOriginal->addRef();
	m_pRoot = pProfile->m_pRoot;
	
	if( bRecursive )
		for(tDWORD i = 0; i < pProfile->m_aChildren.size(); i++)
		{
			CProfile * pChild = AddProfile();
			if( !pChild || !GetChild(i)->Copy(pProfile->GetChild(i), bFull, bRecursive) )
				return false;
		}

	return true;
}

tDWORD CProfile::GetUserCount(LPCSTR strType)
{
	tDWORD nCount = 0;

	for(DWORD i = 0; i < GetChildrenCount(); i++)
	{
		CProfile * pProfile = GetChild(i);
		if( !pProfile->IsVisibleOnGui() )
			continue;
		
		if( strType && pProfile->m_sType != strType )
			continue;
		
		nCount++;
	}
	
	return nCount;
}

bool CProfile::IsProtectionTask()
{
	if( !persistent() )
		return false;
	if( isService() )
		return false;
	return true;
}

bool CProfile::IsProfilesLimit(LPCSTR strType)
{
	DWORD nCount = GetUserCount(strType);
	
	if( !strType )
		return nCount >= (MAX_PROFILE_ODS_COUNT + MAX_PROFILE_UPD_COUNT);

	sswitch(strType)
	scase(AVP_TASK_ONDEMANDSCANNER)
		return nCount >= MAX_PROFILE_ODS_COUNT;
		sbreak;
	scase(AVP_TASK_UPDATER)
		return nCount >= MAX_PROFILE_UPD_COUNT;
		sbreak;
	send;

	return false;
}

bool CProfile::IsScan(bool& bUserProfile)
{
	if( m_sType != AVP_TASK_ONDEMANDSCANNER )
		return false;
	bUserProfile = IsUser();
	return true;
}

bool CProfile::IsScanPredefined()
{
	bool bUser;
	if( !IsScan(bUser) )
		return false;

	if( bUser )
		return false;
	return m_sName != (tSTRING)AVP_PROFILE_SCANOBJECTS;
}

bool CProfile::IsScanProfile(LPCTSTR strProfile, bool& bUserProfile)
{
	CProfile * pProfile = GetProfile(strProfile);
	if( !pProfile )
		return false;
	return pProfile->IsScan(bUserProfile);
}

bool CProfile::IsTaskExist(LPCTSTR strTaskType)
{
	if( m_sType == (tSTRING)strTaskType )
		return true;

	for(DWORD i = 0; i < GetChildrenCount(); i++)
		if( GetChild(i)->IsTaskExist(strTaskType) )
			return true;
	
	return false;
}

bool CProfile::IsSettExist()
{
	if( m_bSettingsExist == 2 )
	{
		if( m_nSerIdSettings &&
			m_nSerIdSettings != cSerializable::eIID &&
			cSerializableObj::getDescriptor(m_nSerIdSettings) )
			m_bSettingsExist = 1;
		else
			m_bSettingsExist = 0;
	}
	
	return !!m_bSettingsExist;
}

bool CProfile::IsStatExist()
{
	if( m_bStatisticsExist == 2 )
	{
		if( m_nSerIdStatistics &&
			m_nSerIdStatistics != cSerializable::eIID &&
			cSerializableObj::getDescriptor(m_nSerIdStatistics) )
			m_bStatisticsExist = 1;
		else
			m_bStatisticsExist = 0;
	}
	
	return !!m_bStatisticsExist;
}

CProfile * CProfile::GetProfileByName(LPCTSTR strName, LPCSTR strType)
{
	for(DWORD i = 0; i < GetChildrenCount(); i++)
	{
		CProfile * pProfile = GetChild(i);
		if( pProfile->m_sType != strType )
			continue;

		tString strDescr;
		g_pRoot->GetFormatedText(strDescr, g_pRoot->LoadLocString(tString(), STRID_PROFILEDESCRIPTION), pProfile);
		if( !_stricmp(strDescr.c_str(), strName) )
			return pProfile;
	}

	return NULL;
}

CProfile * CProfile::AddProfile(cProfileBase * pProfile, tDWORD nClassId)
{
	CProfile * pChild = pProfile ? GetProfile(pProfile->m_sName.c_str(cCP_ANSI)) : NULL;
	if( pChild )
		return pChild;
	
	if( nClassId == cSerializable::eIID )
		nClassId = GetCreateIID();
	m_aChildren.push_back().init(nClassId);
	
	pChild = GetChild(m_aChildren.size() - 1);
	if( !pChild )
		return NULL;

	pChild->m_pParent = this;
	pChild->m_pRoot   = m_pRoot;
	pChild->m_nState  = PROFILE_STATE_ENABLED;
	
	if( pProfile )
	{
		pChild->m_sName = pProfile->m_sName;
		pChild->Update();
	}
	
	return pChild;
}

void CProfile::Customize(LPCSTR strName, tDWORD nSettingsId, tDWORD nStatisticsId)
{
	m_sName = strName;
	m_nSerIdSettings = nSettingsId;
	m_nSerIdStatistics = nStatisticsId;
	m_bCustom = 1;
}

void CProfile::UpdateProfile(tDWORD nEventId, cSerializable * pData, /*bool bFixRtId, */bool bAddDel)
{
	if( !pData || !pData->isBasedOn(cProfileBase::eIID) )
		return;

	cProfileBase * pProfileInfo = (cProfileBase *)pData;
	if( nEventId == pmPROFILE_CREATE )
	{
		if( bAddDel )
			AddProfile(pProfileInfo);
		return;
	}
	
	CProfile * pProfile = GetProfile(pProfileInfo->m_sName.c_str(cCP_TCHAR));
	if( !pProfile )
		return;
	
	bool bFixRtId = isBasedOn(CTaskProfile::eIID);
	
	if( bFixRtId && (pProfile == this) && (pProfile->m_nRuntimeId != pProfileInfo->m_nRuntimeId) )
		return;
	
	switch( nEventId )
	{
	case pmPROFILE_DELETE:
		if( bAddDel )
			DelProfile(pProfile->m_sName.c_str(cCP_TCHAR));
		break;
	
	case pmPROFILE_STATE_CHANDED:
		pProfile->UpdateState(pProfileInfo);
		if( !bFixRtId || (pProfile != this) )
			pProfile->m_nRuntimeId = pProfileInfo->m_nRuntimeId;
		break;
	
	case pmPROFILE_MODIFIED:
		{
		tProfileState nState = pProfile->m_nState;
		if( pProfileInfo->isBasedOn(cProfile::eIID) )
			pProfile->Assign(*(cProfile *)pProfileInfo);
		else
			pProfile->GetData();
		pProfile->m_nState = nState; // dirty hack
		}
		break;
	
	case pmPROFILE_SETTINGS_MODIFIED:
		if( !pProfile->m_cfg.m_settings )
			break;
		
		if( pProfileInfo->isBasedOn(cProfile::eIID) )
			pProfile->Assign(((cProfile *)pProfileInfo)->m_cfg);
		else
			pProfile->GetSettings();
		break;
	}
}

bool CProfile::ApplyToAllScanSettings(CProfile* pProfile)
{
	bool bUserProfile;

	cODSSettings* pSettings = static_cast<cODSSettings*>(pProfile->settings());
	if( IsScan(bUserProfile) && pSettings )
	{
		cProtectionSettings pSS( *static_cast<cProtectionSettings*>(pSettings) );
		pSS.m_pTaskBLSettings.clear();
		
		cTaskSettings * pDst = settings();
		cSerializable * pTaskBLSettingsOld = pDst->m_pTaskBLSettings.relinquish();
		pDst->assign(pSS, false);

		pDst->m_pTaskBLSettings.ptr_ref() = pTaskBLSettingsOld;	// return back BL settings
		// apply rootkit scan setting
		if( cODSSettings* pODSSettings = dynamic_cast<cODSSettings*>(pDst) )
		{
			pODSSettings->m_bRootkitScan = pSettings->m_bRootkitScan;
			pODSSettings->m_bDeepRootkitScan = pSettings->m_bDeepRootkitScan;
		}
		
		m_cfg.m_dwLevel = pProfile->m_cfg.m_dwLevel;
		m_cfg.m_runas = pProfile->m_cfg.m_runas;
		m_bSettingsDirty = cTRUE;
	}

	for(DWORD i = 0; i < GetChildrenCount(); i++)
		GetChild(i)->ApplyToAllScanSettings(pProfile);
	return true;
}

tERROR CProfile::SaveSettings(bool bRecursive)
{
	SetSettingsDirty(true, bRecursive);

	if( !m_pOriginal )
		return StoreSettings(bRecursive);
	
	m_pOriginal->AssignEx(this, bRecursive);
	return errOK;
}

void CProfile::PopSettings()
{
	settings();
	
	for(DWORD i = 0; i < GetChildrenCount(); i++)
		GetChild(i)->PopSettings();
}

CProfile& CProfile::operator =(const CProfile& src)
{
	return Copy(&src, true, false), *this;
}

CProfile * CProfile::GetProfileByType(LPCSTR strType)
{
	if( m_sType == (tSTRING)strType )
		return this;

	for(DWORD i = 0; i < GetChildrenCount(); i++)
		if( CProfile *pProfile = GetChild(i)->GetProfileByType(strType) )
			return pProfile;
	
	return NULL;
}

CProfile * CProfile::GetProfile(LPCTSTR strProfile)
{
	return (CProfile*)find_profile(strProfile, true, true);
}

CProfile * CProfile::GetChild(LPCTSTR strProfile) const
{
	return (CProfile*)((CProfile*)this)->find_profile(strProfile, false, false);
}

CProfile * CProfile::GetRootProfile()
{
	if( !m_pParent )
		return this;
	return m_pParent->GetRootProfile();
}

//////////////////////////////////////////////////////////////////////

cTaskSettings* CProfile::settings(LPCSTR strProfile, tDWORD nSettingsId)
{
	if( strProfile && *strProfile )
	{
		CProfile * pProfile = GetProfile(strProfile);
		return pProfile ? pProfile->settings(NULL, nSettingsId) : NULL;
	}
	
	if( !m_cfg.m_settings && IsSettExist() )
	{
		m_cfg.m_settings.init(m_nSerIdSettings);

		if( m_pOriginal )
		{
			m_pOriginal->settings();
			Assign(m_pOriginal->m_cfg);
		}
		else
			OnPrepareSettings();
	}

	if( !m_cfg.m_settings || !m_cfg.m_settings->isBasedOn(nSettingsId) )
		return NULL;
	
	return (cTaskSettings *)m_cfg.m_settings.ptr_ref();
}

cSerializable* CProfile::GetUserSettings(cSerializable * pSettings, tDWORD nSettingsId)
{
	if( !pSettings || !pSettings->isBasedOn(cTaskSettingsPerUser::eIID) )
		return NULL;

	cSerializable * pSett = ((cTaskSettingsPerUser *)pSettings)->settings_byuser(g_hGui->GetUserId(), true);
	if( !pSett || !pSett->isBasedOn(nSettingsId) )
		return NULL;
	return pSett;
}

bool CProfile::IsCfgEqual(cCfg& _cfg1, cCfg& _cfg2)
{
	tMemChunk _users1;
	cTaskSettingsPerUser * pUsers1 = NULL;
	if( _cfg1.m_settings && _cfg1.m_settings->isBasedOn(cTaskSettingsPerUser::eIID) )
	{
		pUsers1 = (cTaskSettingsPerUser *)_cfg1.m_settings.ptr_ref();
		memcpy(&_users1, &pUsers1->m_users, sizeof(_users1));
		memset(&pUsers1->m_users, 0, sizeof(_users1));
	}

	tMemChunk _users2;
	cTaskSettingsPerUser * pUsers2 = NULL;
	if( _cfg2.m_settings && _cfg2.m_settings->isBasedOn(cTaskSettingsPerUser::eIID) )
	{
		pUsers2 = (cTaskSettingsPerUser *)_cfg2.m_settings.ptr_ref();
		memcpy(&_users2, &pUsers2->m_users, sizeof(_users2));
		memset(&pUsers2->m_users, 0, sizeof(_users2));
	}

	bool eq = true;
	if( !_cfg1.m_schedule.isEqual(&_cfg2.m_schedule) )
		eq = false;
	if( !_cfg1.m_runas.isEqual(&_cfg2.m_runas) )
		eq = false;
	
	if( _cfg1.m_settings || _cfg2.m_settings )
	{
		if( !_cfg1.m_settings || !_cfg2.m_settings )
			eq = false;
		else if( !_cfg1.m_settings->isEqual(_cfg2.m_settings) )
			eq = false;
	}

	if( pUsers1 )
		memcpy(&pUsers1->m_users, &_users1, sizeof(_users1));

	if( pUsers2 )
		memcpy(&pUsers2->m_users, &_users2, sizeof(_users2));

	if( eq && pUsers1 && pUsers2 )
	{
		cSerializable * pUserCfg1 = GetUserSettings(pUsers1);
		cSerializable * pUserCfg2 = GetUserSettings(pUsers2);

		if( pUserCfg1 && pUserCfg2 )
			if( !pUserCfg1->isEqual(pUserCfg2) )
				eq = false;
	}

	return eq;
}

cSerializable* CProfile::settings_user(LPCSTR strProfile, tDWORD nSettingsId)
{
	cTaskSettingsPerUser * pUser = (cTaskSettingsPerUser *)settings(strProfile, cTaskSettingsPerUser::eIID);
	if( !pUser )
		return NULL;
	
	return GetUserSettings(pUser, nSettingsId);
}

cTaskStatistics* CProfile::statistics(LPCSTR strProfile, tDWORD nStatisticsId)
{
	if( strProfile && *strProfile )
	{
		CProfile * pProfile = GetProfile(strProfile);
		return pProfile ? pProfile->statistics(NULL, nStatisticsId) : NULL;
	}
	
	if( !m_statistics && IsStatExist() )
	{
		m_statistics.init(m_nSerIdStatistics);

		if( m_pOriginal && (m_nRuntimeId == m_pOriginal->m_nRuntimeId) )
		{
			m_pOriginal->statistics();
			m_statistics->assign(*m_pOriginal->m_statistics, true);
		}
		else
			OnPrepareStatistics();
	}

	if( !m_statistics || !m_statistics->isBasedOn(nStatisticsId) )
		return NULL;
	
	if( m_statistics->isBasedOn(cTaskStatisticsPerUser::eIID) )
	{
		cStrObj& strUser = ((cTaskStatisticsPerUser *)m_statistics.ptr_ref())->m_sUserId;
		if( strUser.empty() )
			strUser = g_hGui->GetUserId();
	}
	
	return (cTaskStatistics *)m_statistics.ptr_ref();
}

void CProfile::ClearUnusedData()
{
	for(DWORD i = 0; i < GetChildrenCount(); i++)
		GetChild(i)->ClearUnusedData();

	int nSettRef = m_cfg.m_settings ? m_cfg.m_settings->getRef() : 0;
	if( nSettRef )
	{
		if( nSettRef == 1 )
			m_cfg.m_settings.clear();
		else
			PR_TRACE((g_root, prtNOT_IMPORTANT, "gui\tSettings 0x%08X of profile \"%S\" ref: %u", m_nSerIdSettings, m_sName.data(), nSettRef));
	}

	int nStatRef = m_statistics ? m_statistics->getRef() : 0;
	if( nStatRef )
	{
		if( nStatRef == 1 )
			m_statistics.clear();
		else
			PR_TRACE((g_root, prtNOT_IMPORTANT, "gui\tStatistics 0x%08X of profile \"%S\" ref: %u", m_nSerIdStatistics, m_sName.data(), nStatRef));
	}
}

bool CProfile::CopyFrom(const cProfileEx& _Data, bool bAddUserProfilesOnly, CProfile* pTopProfile)
{
	if (pTopProfile == NULL)
		pTopProfile = this;
	Assign(*(const cProfile *)&_Data);
	
	for(tUINT i = 0; i < _Data.count(); i++)
	{
		const cProfileEx *pSrc = _Data[i];
		CProfile *pDst = pTopProfile->GetProfile(pSrc->m_sName.c_str(cCP_ANSI));
		if( !pDst )
		{
			if( !bAddUserProfilesOnly || (bAddUserProfilesOnly && pSrc->m_eOrigin == poUser) )
				pDst = AddProfile();
		}		
		if( pDst )
			pDst->CopyFrom(*pSrc, bAddUserProfilesOnly, pTopProfile);
	}
	
	return true;
}

bool CProfile::CopyTo(cProfileEx &oDest, bool bRecursive)
{
	*(cProfile *)&oDest = *(cProfile *)this;

	oDest.m_statistics.clear();
	
	if( bRecursive )
	{
		for(tUINT i = 0; i < GetChildrenCount(); i++)
		{
			CProfile *pSrcChild = GetChild(i);
			if( pSrcChild->m_eOrigin != cProfileBase::poPrivate )
			{
				cProfileExPtr &pChild = oDest.m_aChildren.push_back();
				pChild.init(cProfileEx::eIID);
				pSrcChild->CopyTo(*(cProfileEx *)pChild);
			}
		}
	}
	return true;
}

bool CProfile::ApplyPolicy()
{
	if( !g_hTM )
		return false;

	m_cfg.m_nFlags |= cCfg::fApplyPolicy;
	g_hTM->GetProfileInfo(m_sName.c_str(cCP_ANSI), &m_cfg);
	m_cfg.m_nFlags &= ~cCfg::fApplyPolicy;

	for(tUINT i = 0; i < GetChildrenCount(); i++)
		GetChild(i)->ApplyPolicy();

	return true;
}

//////////////////////////////////////////////////////////////////////
// CTaskProfile

bool CTaskProfile::GetInfo(cSerializable* pInfo, tGetProfileInfoSource eSrc/*, bool bFixRtId*/)
{
	if( !g_hTM )
		return false;
	
	if( PR_FAIL(g_hTM->GetTaskInfo(m_nRuntimeId, pInfo)) )
		return false;
	
	return true;
}

void CTaskProfile::KeepTempOn()
{
	if( m_eOrigin == poTemporary /*&& !(m_nState & STATE_FLAG_ACTIVE)*/ )
	{
		cProfileBase _Dummy;
		GetInfo(&_Dummy);
	}
}

//////////////////////////////////////////////////////////////////////

#undef  __pr_serializable_h
#define __DECLARE_SERIALIZABLE

#include <pr_serializable.h>

IMPLEMENT_SERIALIZABLE_BEGIN( CProfile, 0 )
	SER_BASE( cProfileEx, 0 )
IMPLEMENT_SERIALIZABLE_END()

IMPLEMENT_SERIALIZABLE_BEGIN( CTaskProfile, 0 )
	SER_BASE( CProfile, 0 )
IMPLEMENT_SERIALIZABLE_END()

#undef __DECLARE_SERIALIZABLE

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

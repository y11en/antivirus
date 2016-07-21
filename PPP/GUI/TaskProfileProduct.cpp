// TaskProfileProduct.cpp: implementation of the CProduct class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TaskProfileProduct.h"

#define REFRESH_STAT_TIMIOUT  1500

//////////////////////////////////////////////////////////////////////
// CProfile

void CProfile::UpdateStatistics(/*bool bFixRtId, */bool bFresh)
{
	if( !statistics() || m_nState != PROFILE_STATE_RUNNING )
		return;

	if( !m_bOriginalInited )
	{
		CProfile* pOriginal = g_pProduct->GetProfile(m_sName.c_str(cCP_ANSI));
		if( pOriginal != this && pOriginal )
			m_pOriginal = pOriginal, m_pOriginal->addRef();
		m_bOriginalInited = cTRUE;
	}

	if( !m_pRoot )
		return;
	
	if( m_pOriginal || bFresh )
		GetInfo(statistics(), gpi_FROM_BOTH/*, bFixRtId*/);
	else
	{
		m_pRoot->NeedStatistics(m_pOriginal ? m_pOriginal : this);
		if( m_pOriginal && m_pOriginal->statistics() )
			statistics()->assign(*m_pOriginal->statistics(), true);
	}
}

void CProfile::UpdateState(cProfileBase* pInfo)
{
	m_nState = pInfo->m_nState;
	m_nRuntimeId = pInfo->m_nRuntimeId;
	m_lastErr = pInfo->m_lastErr;
	GetInfo(statistics(), gpi_FROM_BOTH/*, true*/);

	if( m_pRoot )
		m_pRoot->ReinitStatistics(this);
}

void CProfile::DelProfile(LPCTSTR strProfile)
{
	CProfile * pProfile = GetProfile(strProfile);
	if( !pProfile )
		return;

	if( pProfile->m_pRoot )
		pProfile->m_pRoot->DeleteData(pProfile);

	pProfile->m_pParent = NULL;
	m_aChildren.remove(strProfile);
}

//////////////////////////////////////////////////////////////////////
// CProduct

DWORD CProduct::GetChildrenCount() const
{
	return CProfile::GetChildrenCount() + (m_local ? 1 : 0);
}

CProfile * CProduct::GetChild(int idx) const
{
	if( CProfile::GetChildrenCount() == idx )
		return (CProfile *)(const CProfile *)m_local;
	return CProfile::GetChild(idx);
}

CProfile * CProduct::GetProfile(LPCTSTR strProfile)
{
	CProfile * pProfile = CProfile::GetProfile(strProfile);
	if( pProfile )
		return pProfile;

	return m_local ? m_local->GetProfile(strProfile) : NULL;
}

CProduct& CProduct::operator =(const CProduct& src)
{
	CProfile::operator =(src);
	m_local.init(src.m_local->getIID());
	m_local->Copy(src.m_local);
	return *this;
}

//////////////////////////////////////////////////////////////////////
// CGuiSubProfile

void CGuiSubProfile::OnPrepareSettings()
{
	g_hGui->UpdateInternalProfile(m_sName.c_str(cCP_ANSI));
}

void CGuiSubProfile::OnPrepareStatistics()
{
	g_hGui->UpdateInternalProfile(m_sName.c_str(cCP_ANSI));
}

//////////////////////////////////////////////////////////////////////
// CGuiProfile

bool CGuiProfile::GetSettings()
{
	if( !m_cfg.m_settings )
		m_cfg.m_settings.init(m_nSerIdSettings);
	return g_pRoot->GetRegSettings("settings", m_cfg.m_settings);
}

tERROR CGuiProfile::StoreSettings(bool bRecursive)
{
	if( !isSettingsDirty() )
		return errOK;
	
	cGuiSettings * settings = (cGuiSettings*)m_cfg.m_settings.ptr_ref();
	if( !settings )
		return errOK;

	cSerObj<cGuiSettings> pOldGuiCfg; pOldGuiCfg.init(m_nSerIdSettings);
	g_pRoot->GetRegSettings("settings", pOldGuiCfg);
	
	if( pOldGuiCfg )
	{
		bool bChangePath = pOldGuiCfg->m_bEnableSkinPath != settings->m_bEnableSkinPath ||
			pOldGuiCfg->m_strSkinPath != settings->m_strSkinPath;

		tDWORD nFlags = 0;
		if( pOldGuiCfg->m_bEnableStandartGuiStyle != settings->m_bEnableStandartGuiStyle ||
			pOldGuiCfg->m_nTransparency != settings->m_nTransparency ||
			pOldGuiCfg->m_bEnableTransparentWindows != settings->m_bEnableTransparentWindows )
			nFlags |= REFRESH_PARAMS;

		if( pOldGuiCfg->m_bEnableStandartGuiStyle != settings->m_bEnableStandartGuiStyle )
			nFlags |= REFRESH_OPEN_DIALOGS;

		if( nFlags || bChangePath )
		{
			if( bChangePath )
				g_pRoot->InitRoot(settings->m_bEnableSkinPath && settings->m_strSkinPath.size() ? settings->m_strSkinPath.data() : (LPCWSTR)NULL);
			else
				g_pRoot->Refresh(nFlags);
		}
	}

	g_hGui->SetRegSettings("settings", settings);

	cProfileBase _Profile; _Profile.assign(*this, true);
	g_pRoot->SendEvent(pmPROFILE_SETTINGS_MODIFIED, &_Profile);
	return errOK;
}

//////////////////////////////////////////////////////////////////////
// CProfileRoot

void CProfileRoot::Init()
{
	m_stamp = 0;
	m_dirty = false;

	cSerObj<CProduct>::init(CProduct::eIID);
	m_Stat.init(cTaskStatisticsList::eIID);
	m_ser->m_pRoot = this;

	m_lock = NULL;
	g_root->sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);
}

void CProfileRoot::Start()
{
	cThreadPoolBase::init(g_root, 1, 100);
}

void CProfileRoot::Stop()
{
	cThreadPoolBase::de_init();
}

void CProfileRoot::Done()
{
	if( m_lock )
		m_lock->sysCloseObject();

	clear();

	for(size_t i = 0; i < m_Data.size(); i++)
		delete m_Data[i];
	m_Data.clear();
	m_Stat.clear();
}

void CProfileRoot::ReinitStatistics(CProfile* pProfile)
{
	cAutoCS locker(m_lock, true);

	CProfileData* pData = (CProfileData*)pProfile->m_pData;
	if( pData && pData->m_data )
		PR_ASSERT(pProfile->m_statistics); // есть падения, где pProfile->m_statistics NULL, надо бы отловить как такое получается
	if( pData && pData->m_data && pProfile->m_statistics )
		pData->m_data->assign(*pProfile->m_statistics, true);
}

void CProfileRoot::NeedStatistics(CProfile* pProfile)
{
	cAutoCS locker(m_lock, true);

	cTaskStatistics * pStat = pProfile->statistics();
	CProfileData*& pData = (CProfileData*&)pProfile->m_pData;
	if( !pData )
	{
		m_Data.push_back(pData = new CProfileData());

		pData->m_pProfile = pProfile;
		pData->m_data = pStat;
		pData->m_dataOrig.assign(pStat, true);
		m_dirty = true;
	}
	else
	{
		if( pData->m_data && pStat )
			pStat->assign(*pData->m_data, true);
	}
	pData->m_stamp = GetTickCount();
}

void CProfileRoot::DeleteData(CProfile* pProfile)
{
	cAutoCS locker(m_lock, true);

	CProfileData* pData = (CProfileData*)pProfile->m_pData;
	if( !pData )
		return;

	pProfile->m_pData = NULL;

	int nCount = m_Data.size();
	for(int i = 0; i < nCount; i++)
		if( m_Data[i] == pData )
		{
			m_Data.erase(m_Data.begin() + i);
			delete pData;
			break;
		}
}

void CProfileRoot::SyncData(bool bIn)
{
	cAutoCS locker(m_lock, true);

	if( bIn )
	{
		m_Stat->m_aItems.clear();
		m_dirty = false;
	}
	else if( m_Stat->m_aItems.empty() )
		return;

	int nCount = m_Data.size();
	for(int i = 0; i < nCount; i++)
	{
		CProfileData& pData = *m_Data[i];
		if( bIn )
		{
			if( m_stamp - pData.m_stamp > REFRESH_STAT_TIMIOUT )
			{
				DeleteData(pData.m_pProfile); i--, nCount--;
				continue;
			}

			cTaskStatisticsListItem& item = m_Stat->m_aItems.push_back();
			item.m_sProfile = pData.m_pProfile->m_sName;
			item.m_statistics = pData.m_data;
			pData.m_pos = m_Stat->m_aItems.size() - 1;
		}
		else
		{
			if( pData.m_pos == -1 )
				continue;

			cTaskStatisticsListItem& item = m_Stat->m_aItems.at(pData.m_pos);
			pData.m_data = item.m_statistics;
		}
	}
}

tERROR CProfileRoot::on_idle()
{
	if( !g_hTM )
		return errOK;

	tLONG stamp = GetTickCount();
	if( m_dirty || stamp - m_stamp > REFRESH_STAT_TIMIOUT )
		m_stamp = stamp, SyncData(true);
	
	if( m_Stat->m_aItems.size() && PR_SUCC(g_hTM->GetProfileInfo(AVP_PROFILE_PRODUCT, m_Stat)) )
		SyncData(false);

	return errOK;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#undef  __pr_serializable_h
#define __DECLARE_SERIALIZABLE

#include <pr_serializable.h>

IMPLEMENT_SERIALIZABLE_BEGIN( CProduct, 0 )
	SER_BASE( CProfile, 0 )
IMPLEMENT_SERIALIZABLE_END()

IMPLEMENT_SERIALIZABLE_BEGIN( CGuiSubProfile, 0 )
	SER_BASE( CProfile, 0 )
IMPLEMENT_SERIALIZABLE_END()

IMPLEMENT_SERIALIZABLE_BEGIN( CGuiProfile, 0 )
	SER_BASE( CProfile, 0 )
IMPLEMENT_SERIALIZABLE_END()

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiRequest, 0 )
	SER_VALUE_PTR_M(pData,  cSerializable::eIID)
	SER_VALUE_M(request_id, tid_DWORD)
IMPLEMENT_SERIALIZABLE_END()

#undef __DECLARE_SERIALIZABLE

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

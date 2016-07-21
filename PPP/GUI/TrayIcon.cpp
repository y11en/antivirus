// TrayIcon.cpp: implementation of the CTrayIcon class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TrayIcon.h"

#define AVS_QUERYING_INTERVAL                1000 // in milliseconds, determines how often we will ask for AVS load the more the interval, the slower is the tray icon blinking
#define AVS_LOWER_LIMIT                      0 // in percents. If the working AVS-load is less than this number, then the load is not reported

#define ICONID_TRAY_PROTECTION_OFF          "Tray"
#define ICONID_TRAY_PROTECTION_ON           "TrayProtectOn"
#define ICONID_TRAY_MAILACTIVE              "TrayMailActive"
#define ICONID_TRAY_SCANACTIVE              "TrayScanActive"
#define ICONID_TRAY_SCRIPTACTIVE            "TrayScriptActive"
#define ICONID_TRAY_UPDATEACTIVE            "TrayUpdateActive"
#define ICONID_TRAY_YELLOWEXCLAMATION       "TrayWarn"
#define ICONID_TRAY_NEEDREBOOT              "TrayNeedReboot"

#define ITEMID_SCAN_MY_COMPUTER             "Scan_My_Computer"
#define ITEMID_UPDATER                      "Updater"
#define ITEMID_DISABLEPROTECT               "DisableProtect"
#define ITEMID_RUNNINGTASK                  "RunningTask"

#define TRAY_STATE_PROTECTION_ON            0x01
#define TRAY_STATE_SCANACTIVE               0x02
#define TRAY_STATE_MAILACTIVE               0x04
#define TRAY_STATE_SCRIPTACTIVE             0x08
#define TRAY_STATE_UPDATING                 0x10
#define TRAY_STATE_YELLOWXCLAMATION         0x20
#define TRAY_STATE_NEEDREBOOT               0x40

#define STRID_TIPSLIST                      "ProductIcoTip"

//////////////////////////////////////////////////////////////////////
// CTrayMenu

CTrayMenu::CTrayMenu() :
	CTaskProfileView(g_pProduct, fStat)
{}

bool CTrayMenu::OnMenu(CPopupMenuItem * pMenu, CItemBase * pItem)
{
	BOOL bDisconected = !g_hTM;
	EFunctionalityLevel nLicLevel = (EFunctionalityLevel)g_hGui->m_LicInfo.m_dwFuncLevel;
	
	PR_TRACE((g_root, prtIMPORTANT, "gui\tGUI::Tray activate status: conected=%u, functionalitylevel=%u (0 = Unknown, 1 = NoFeatures, 2 = OnlyUpdates, 3 = FunctionWithoutUpdates, 4 = FullFunctionality)", !bDisconected, nLicLevel));

	CItemBase * pTaskTpl = pMenu->GetItem(ITEMID_RUNNINGTASK);
	if( pTaskTpl && pTaskTpl->m_pParent )
	{
		for(DWORD i = 0; i < g_pProduct->GetChildrenCount(); i++)
		{
			CProfile * pProfile = g_pProduct->GetChild(i);
			
			bool bUser;
			if( !pProfile->IsScan(bUser) && pProfile->m_sType != AVP_TASK_UPDATER )
				continue;
			if( !IS_TASK_IN_PROGRESS(pProfile->m_nState) )
				continue;
			
			LPCSTR strProfileName = pProfile->m_sName.c_str(cCP_TCHAR);
			if( pMenu->GetItem(strProfileName) )
				continue;
			
			if( CItemBase * pTask = pTaskTpl->m_pParent->CloneItem(pTaskTpl, strProfileName, true) )
			{
				pTaskTpl->m_pParent->MoveItem(pTaskTpl, pTask);
				pTask->Show(true);
			}
		}
	}

	RefreshStatistics();

	if (bDisconected)
	{
		pMenu->Enable("KAV",                 false);
		pMenu->Enable("Settings",            false);
		pMenu->Enable("Activate",            false);
	}
	if (bDisconected || (nLicLevel != eflFullFunctionality && nLicLevel != eflFunctionWithoutUpdates))
	{
		pMenu->Enable(ITEMID_DISABLEPROTECT, false);
		pMenu->Enable("ScanCustom",          false);
	}

	CProfile * pUpdater  = g_pProduct->GetProfile(AVP_PROFILE_UPDATER);
	if (pUpdater)
		pMenu->Enable(ITEMID_UPDATER, !bDisconected && (nLicLevel == eflFullFunctionality || nLicLevel == eflOnlyUpdates || (pUpdater->m_nState & STATE_FLAG_ACTIVE)));
	else
		pMenu->Show(ITEMID_UPDATER, false);

	CProfile * pFullScan = g_pProduct->GetProfile(AVP_PROFILE_FULLSCAN);
	if (pFullScan)
		pMenu->Enable(ITEMID_SCAN_MY_COMPUTER, !bDisconected && (nLicLevel == eflFullFunctionality || nLicLevel == eflFunctionWithoutUpdates || (pFullScan->m_nState & STATE_FLAG_ACTIVE)));
	else
		pMenu->Show(ITEMID_SCAN_MY_COMPUTER, false);

	return true;
}

//////////////////////////////////////////////////////////////////////
// CTray

CTray::CTray() :
	m_pIcon(NULL),
	m_nIconState(0),
	m_pAVSProfile(NULL),
	m_fUpdateActive(0),
	m_bEnableTrayIconAnimation(1)
{
}

void CTray::OnCreate()
{
	CDialogSink::OnCreate();

	Item()->AddDataSource(g_pProduct->GetProfile(AVP_PROFILE_PROTECTION));
	Item()->AddDataSource(g_hGui->m_statistics);

	if( !m_pAVSProfile )
		if( m_pAVSProfile = g_pProduct->GetProfile(AVP_SERVICE_AVS) )
		{
			cAVSStatistics * pStat = (cAVSStatistics *)m_pAVSProfile->statistics(NULL, cAVSStatistics::eIID);
			if( pStat )
			{
				pStat->m_nObjectOrigin = OBJECT_ORIGIN_UNKNOWN;
				Item()->AddDataSource(pStat, AVP_SERVICE_AVS);
			}
		}
}

void CTray::OnDestroy()
{
	if( m_pIcon )
		m_pIcon->Destroy(), m_pIcon = NULL;

	CDialogSink::OnDestroy();
}

void CTray::OnInit()
{
	if( cGuiSettings * pGuiSett = (cGuiSettings *)g_pProduct->settings(AVP_SOURCE_GUI, cGuiSettings::eIID) )
		m_bEnableTrayIconAnimation = !!pGuiSett->m_bEnableTrayIconAnimation;
}

void CTray::OnEvent(tDWORD nEventId, cSerializable* pData)
{
	switch(nEventId)
	{
	case pmPROFILE_STATE_CHANDED:
		Item()->UpdateData(true);
		break;

	case pm_PRODUCT_STATE_CHANGED:
		if( m_pAVSProfile )
			m_pAVSProfile->UpdateStatistics();
		Item()->UpdateData(true);
		break;
	
	case pmPROFILE_SETTINGS_MODIFIED:
		if( pData && pData->isBasedOn(cProfileBase::eIID) && ((cProfileBase *)pData)->m_sName == (tSTRING)AVP_SOURCE_GUI )
			if( cGuiSettings * pGuiSett = (cGuiSettings *)g_pProduct->settings(AVP_SOURCE_GUI, cGuiSettings::eIID) )
				m_bEnableTrayIconAnimation = !!pGuiSett->m_bEnableTrayIconAnimation;
		break;
	}
}

void CTray::OnTimerRefresh(tDWORD nTimerSpin)
{
	tDWORD nCurTick = nTimerSpin % (AVS_QUERYING_INTERVAL/TIMER_REFRESH_INT);

	if( m_bEnableTrayIconAnimation && m_pAVSProfile )
	{
		if( cAVSStatistics * pStat = (cAVSStatistics *)m_pAVSProfile->statistics(NULL, cAVSStatistics::eIID) )
		{
			if( !nCurTick )
			{
				// time to get AVS load and change states
				m_pAVSProfile->UpdateStatistics();
				CStructData data (pStat);
				Item()->UpdateData(true, &data);
				
				if( !pStat->m_nPerformance )
					pStat->m_nObjectOrigin = OBJECT_ORIGIN_UNKNOWN;
				else if( pStat->m_nPerformance < 20 )
					pStat->m_nPerformance = 20;
				else if( pStat->m_nPerformance > 90 )
					pStat->m_nPerformance = 90;
				
				m_nTicksActiveState = (tDWORD)(((pStat->m_nPerformance/100.) * (AVS_QUERYING_INTERVAL/TIMER_REFRESH_INT)));
			}
			else if( nCurTick >= m_nTicksActiveState )
				pStat->m_nObjectOrigin = OBJECT_ORIGIN_UNKNOWN;
		}
	}
	
	m_fUpdateActive = nCurTick < 9;

	UpdateIcon();
}

void CTray::UpdateIcon()
{
	DWORD nIconState = 0;
	
	if( CProfile * pProtection = g_pProduct->GetProfile(AVP_PROFILE_PROTECTION) )
	{
		if( pProtection->m_nState & STATE_FLAG_OPERATIONAL )
		{
			if( g_hGui->m_statistics->m_State.m_ProtectionState & eTasksMalfunction )
				nIconState |= TRAY_STATE_YELLOWXCLAMATION;
			else
				nIconState |= TRAY_STATE_PROTECTION_ON;
		}
	}
	
	if( m_bEnableTrayIconAnimation )
	{
		if( m_fUpdateActive )
		{
			for(DWORD i = 0, c = g_pProduct->GetChildrenCount(); i < c; ++i)
			{
				CProfile *pProfile = g_pProduct->GetChild(i);
				if((pProfile->m_nState & STATE_FLAG_OPERATIONAL) && pProfile->m_sType == AVP_TASK_UPDATER)
				{
//					pProfile->UpdateStatistics();
//					cUpdater2005Statistics * pUpdStat = (cUpdater2005Statistics *)pProfile->statistics(NULL, cUpdater2005Statistics::eIID);
//					if( pUpdStat && pUpdStat->m_qwDownloadedBytesCount )
					{
						nIconState |= TRAY_STATE_UPDATING;
						break;
					}
				}				
			}
		}

		cAVSStatistics * pStat = m_pAVSProfile ? (cAVSStatistics *)m_pAVSProfile->statistics(NULL, cAVSStatistics::eIID) : NULL;

		if( pStat && !(nIconState & TRAY_STATE_UPDATING) )
		{
			switch( pStat->m_nObjectOrigin )
			{
//			case OBJECT_ORIGIN_GENERIC:  nIconState |= TRAY_STATE_SCANACTIVE; break;
			case OBJECT_ORIGIN_MAIL:     nIconState |= TRAY_STATE_MAILACTIVE; break;
//			case OBJECT_ORIGIN_SCRIPT:   nIconState |= TRAY_STATE_SCRIPTACTIVE; break;
			}
		}
	}

	if( g_hGui->m_statistics && (g_hGui->m_statistics->m_State.m_UpdateState & eUpdateNeedReboot) )
		nIconState |= TRAY_STATE_NEEDREBOOT;

	if( m_nIconState == nIconState && m_pIcon )
		return;
	m_nIconState = nIconState;

	CIcon * pOldIcon = m_pIcon;
	m_pIcon = NULL;
	if( CIcon * pBase = g_pRoot->GetIcon(ICONID_TRAY_PROTECTION_OFF) )
		m_pIcon = g_pRoot->CreateIcon(pBase->IconHnd());

	if( m_pIcon )
	{
		if( m_nIconState & TRAY_STATE_PROTECTION_ON )
			m_pIcon->Merge(g_pRoot->GetIcon(ICONID_TRAY_PROTECTION_ON));

		if( m_nIconState & TRAY_STATE_YELLOWXCLAMATION )
			m_pIcon->Merge(g_pRoot->GetIcon(ICONID_TRAY_YELLOWEXCLAMATION));
		else if( m_nIconState & TRAY_STATE_SCANACTIVE )
			m_pIcon->Merge(g_pRoot->GetIcon(ICONID_TRAY_SCANACTIVE));
		else if( m_nIconState & TRAY_STATE_MAILACTIVE )
			m_pIcon->Merge(g_pRoot->GetIcon(ICONID_TRAY_MAILACTIVE));
		else if( m_nIconState & TRAY_STATE_SCRIPTACTIVE )
			m_pIcon->Merge(g_pRoot->GetIcon(ICONID_TRAY_SCRIPTACTIVE));
		else if( m_nIconState & TRAY_STATE_UPDATING )
			m_pIcon->Merge(g_pRoot->GetIcon(ICONID_TRAY_UPDATEACTIVE));

		if( m_nIconState & TRAY_STATE_NEEDREBOOT )
			m_pIcon->Merge(g_pRoot->GetIcon(ICONID_TRAY_NEEDREBOOT));

		m_pItem->SetIcon(m_pIcon);
	}

	if( pOldIcon )
		pOldIcon->Destroy();
}

//////////////////////////////////////////////////////////////////////
// CTrayFlash

CTrayFlash::CTrayFlash(LPCSTR strIcon1, LPCSTR strIcon2) :
	m_bShow1st(true),
	m_pIcon1(NULL),
	m_pIcon2(NULL),
	m_strIcon1(strIcon1),
	m_strIcon2(strIcon2)
{
}

void CTrayFlash::OnInit()
{
	CIcon * pBase;
	if( !m_strIcon1.empty() )
	{
		pBase = g_pRoot->GetIcon(m_strIcon1.c_str());
		if( pBase )
			m_pIcon1 = g_pRoot->CreateIcon(pBase->IconHnd());
	}
	if( !m_strIcon2.empty() )
	{
		pBase = g_pRoot->GetIcon(m_strIcon2.c_str());
		if( pBase )
			m_pIcon2 = g_pRoot->CreateIcon(pBase->IconHnd());
	}
	Item()->m_nFlags &= ~STYLE_DESTROY_ICON;
	TBase::OnInit();
}

void CTrayFlash::OnDestroy()
{
	if( m_pIcon1 )
		m_pIcon1->Destroy(), m_pIcon1 = NULL;
	if( m_pIcon2 )
		m_pIcon2->Destroy(), m_pIcon2 = NULL;
	TBase::OnDestroy();
}

void CTrayFlash::Show(bool bShow)
{
	if( !bShow )
		Item()->SetIcon((CIcon *)NULL);
	TBase::Show(NULL, bShow);
}

void CTrayFlash::OnTimerRefresh(tDWORD nTimerSpin)
{
	if( GUI_TIMERSPIN_INTERVAL(800) )
		UpdateIcon();

	TBase::OnTimerRefresh(nTimerSpin);
}

void CTrayFlash::UpdateIcon()
{
	if( m_bShow1st && m_pIcon1 )
		m_pItem->SetIcon(m_pIcon1);
	else if( m_pIcon2 )
		m_pItem->SetIcon(m_pIcon2);
	m_bShow1st = !m_bShow1st;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

// ServiceSettings.cpp: implementation of the SettingsDlgs class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ServiceSettings.h"

//#define IMPEX_IMPORT_LIB
//	#include <plugin/p_pdm.h>

//////////////////////////////////////////////////////////////////////
// CNotifyEventsList

CNotifyEventsList::CNotifyEventsList(CItemBase *pItem) : m_pRoot(NULL)
{
	if( pItem->m_strItemType == GUI_ITEMT_LISTCTRL )
		((CListItem *)pItem)->Bind(OnGetType());
}

bool CNotifyEventsList::OnSetValue(const cVariant &pValue)
{
	if( !m_pRoot && (m_pRoot = (cBLNotifySettings *)pValue.c_ser(cBLNotifySettings::eIID)) )
		InitView(m_pRoot);
	return true;
}

cSer * CNotifyEventsList::GetRowData(tIdx nItem)
{
	return (tIdx)m_mapIdx.size() > nItem ? m_mapIdx[nItem] : NULL;
}

void CNotifyEventsList::InitView(cBLNotifySettings *pItem, cRowNode *pNode)
{
	switch( pItem->m_EventID )
	{
	case eNotifyRuleTriggered:
	case eNotifyAttackBlocked:
		if( !g_pProduct->GetProfile(AVP_PROFILE_FIREWALL) )
			return;
		break;

	case eNotifyPopups:
		if( !g_pProduct->GetProfile(AVP_PROFILE_POPUPBLOCKER) )
			return;
		break;

	case eNotifyProtocoller:
	case eNotifySSL:
		if( !g_pProduct->GetProfile(AVP_PROFILE_TRAFFICMONITOR) )
			return;
		break;
	}

	if( !pNode )
		pNode = (CListItem *)Item();

	cRowNode * pChildNode = pNode;
	if( pItem->m_EventID == eNotifyNone )
	{
		pChildNode = pNode->AddNode(m_mapIdx.size(), cRowNode::fSkinGroup|cRowNode::fCheckGroup);
		pChildNode->Expand(true);
	}
	else
	{
		tIdx i = m_mapIdx.size();
		pNode->AddRows(1, &i);
	}
	m_mapIdx.push_back(pItem);
	
	for(tDWORD n = pItem->m_vChilds.size(), i = 0; i < n; i++)
		InitView(&pItem->m_vChilds[i], pChildNode);
}

/*//////////////////////////////////////////////////////////////////////
// CNotifySettDlg

bool CNotifySettDlg::OnAddDataSection(LPCTSTR strSect)
{
	if( !CProfileSettingsDlg::OnAddDataSection(strSect) )
		return false;

	if( strSect && (*strSect) )
		return true;
	
	if( CProfile * pBl = Profile() )
		if( cBLSettings * pSettings = (cBLSettings *)pBl->settings(NULL, cBLSettings::eIID) )
			pBl->m_cfg.m_schedule = pSettings->m_mail_schedule;

	return true;
}

void CNotifySettDlg::OnInit()
{
	CProfileSettingsDlg::OnInit();
	
	if( cBLSettings *pSett = (cBLSettings *)Ser(NULL, cBLSettings::eIID) )
	{
		m_NLst.Bind(&pSett->m_NSettings);
		if( IsBlockedByPolicy(pSett, &pSett->m_NSettings) )
			m_NLst->Freeze(true);
	}
}

bool CNotifySettDlg::OnOk()
{
	if( CProfile * pBl = Profile() )
		if( cBLSettings * pSettings = (cBLSettings *)pBl->settings(NULL, cBLSettings::eIID) )
			pSettings->m_mail_schedule = pBl->m_cfg.m_schedule;

	return CProfileSettingsDlg::OnOk();
}

//////////////////////////////////////////////////////////////////////
// CPswrdProtectSettDlg

static const char* g_szPwdNotChanged = "SaMePsWd";
static const char* g_szPwdNotChanged2 = "********";

void CPswrdProtectSettDlg::OnInited()
{
	CProfileSettingsDlg::OnInited();

	bool bIsAK = GetItem("OldPswdEdit") == NULL;

	cBLSettings* pSettings = (cBLSettings*)Ser();
	m_bProtected = !bIsAK && pSettings && !pSettings->m_PrtctPassword.empty();

	m_pOldPswd = GetItem("OldPswdEdit");
	m_pNewPswd = GetItem("NewPswdEdit");
	m_pConfirmPswd = GetItem("ConfPswdEdit");

	m_b1 = !(Root()->m_nGuiFlags & GUIFLAG_NOCRYPT);

	if( !m_b1 || IsBlockedByPolicy(pSettings, &pSettings->m_PrtctPassword) )
	{
		if( m_pOldPswd )
			m_pOldPswd->Enable(false);
		if( m_pNewPswd )
			m_pNewPswd->Enable(false);
		if( m_pConfirmPswd )
			m_pConfirmPswd->Enable(false);
	}

	if (bIsAK && pSettings && !pSettings->m_PrtctPassword.empty())
	{
		m_passwords.m_sNewPswrd = g_szPwdNotChanged;
		m_passwords.m_sNewPswrd2 = g_szPwdNotChanged2;
		CStructData data(NULL, NULL);
		m_pBinding->UpdateData(true, data);
	}

	if (m_pOldPswd)
		OnChangedData(m_pOldPswd);
}

bool CPswrdProtectSettDlg::OnAddDataSection(LPCTSTR strSect)
{
	if( !CProfileSettingsDlg::OnAddDataSection(strSect) )
		return false;
	
	if( !strSect || !(*strSect) )
		m_pBinding->AddDataSource(&m_passwords);
	return true;
}

void CPswrdProtectSettDlg::OnChangedData(CItemBase * pItem)
{
	CProfileSettingsDlg::OnChangedData(pItem);

	if( pItem != m_pOldPswd )
		return;

	if( m_bProtected )
		m_bProtected = !((CRootSink *)(Root()->m_pSink))->CheckPasswords(((cBLSettings*)Ser())->m_PrtctPassword, m_passwords.m_sOldPswrd);

	if( !m_bProtected )
	{
		m_pOldPswd->Enable(false);
		Item()->SendEvent(EVENTID_PASSWORD_ENTERED, NULL, false);
	}

	if( m_b1 && m_pNewPswd )
	{
		m_pNewPswd->Enable(!m_bProtected);
		if( !m_bProtected )
			m_pNewPswd->Focus(true);
	}

	if( m_b1 && m_pConfirmPswd )
		m_pConfirmPswd->Enable(!m_bProtected);
}

bool CPswrdProtectSettDlg::OnOk()
{
	bool bIsAK = m_pOldPswd == NULL;

	if( m_b1 )
	{
		if( m_bProtected )
		{
			MsgBox(*this, "ErrOldPswrdWrong", NULL, MB_OK|MB_ICONERROR);
			return false;
		}

		if( m_passwords.m_sNewPswrd.empty() || m_passwords.m_sNewPswrd2.empty() )
		{
			MsgBox(*this, "ErrNoPasswordEntered", NULL, MB_OK|MB_ICONERROR);
			return false;
		}

		if (m_passwords.m_sNewPswrd != g_szPwdNotChanged
			&& (!m_passwords.m_sNewPswrd.empty() || !m_passwords.m_sNewPswrd2.empty()))
		{
			if( m_passwords.m_sNewPswrd2 != m_passwords.m_sNewPswrd )
			{
				MsgBox(*this, "ErrDifferPswrds", NULL, MB_OK|MB_ICONERROR);
				return false;
			}

			if( !bIsAK && m_passwords.m_sNewPswrd == m_passwords.m_sOldPswrd )
			{
				MsgBox(*this, "ErrSamePswrds", NULL, MB_OK|MB_ICONERROR);
				if (m_pNewPswd) m_pNewPswd->Focus(true);
				return false;
			}

			cBLSettings * pBlSett = ((cBLSettings*)Ser());
			if( pBlSett )
			{
				pBlSett->m_PrtctPassword = m_passwords.m_sNewPswrd;
				((CRootSink *)(Root()->m_pSink))->CryptDecrypt(pBlSett->m_PrtctPassword, true);
			}
		}
	}
	
	CProfileSettingsDlg::OnOk();
	return true;
}

//////////////////////////////////////////////////////////////////////
// CUPswrdProtectSettDlg

void CUPswrdProtectSettDlg::OnInited()
{
	CProfileSettingsDlg::OnInited();

	m_pUninstPswd = GetItem("UninstallPswdEdit");
	m_pConfUninstPswd = GetItem("ConfUnPswdEdit");

	cBLSettings* pSettings = (cBLSettings*)Ser();

	m_b1 = !(Root()->m_nGuiFlags & GUIFLAG_NOCRYPT);

	if (pSettings && !pSettings->m_UninstallPwd.empty())
	{
		m_passwords.m_sNewPswrd = g_szPwdNotChanged;
		m_passwords.m_sNewPswrd2 = g_szPwdNotChanged2;
		CStructData data(NULL, NULL);
		m_pBinding->UpdateData(true, data);
	}

	if( !m_b1 || IsBlockedByPolicy(pSettings, &pSettings->m_UninstallPwd) )
	{
		if( m_pUninstPswd )
			m_pUninstPswd->Enable(false);
		if( m_pConfUninstPswd )
			m_pConfUninstPswd->Enable(false);
	}
}

bool CUPswrdProtectSettDlg::OnAddDataSection(LPCTSTR strSect)
{
	if( !CProfileSettingsDlg::OnAddDataSection(strSect) )
		return false;

	if( !strSect || !(*strSect) )
		m_pBinding->AddDataSource(&m_passwords);
	return true;
}

bool CUPswrdProtectSettDlg::OnOk()
{
	if( m_b1 && m_passwords.m_sNewPswrd != g_szPwdNotChanged)
	{
		if (m_passwords.m_sNewPswrd != m_passwords.m_sNewPswrd2)
		{
			MsgBox(*this, "ErrDifferPswrds", NULL, MB_OK | MB_ICONERROR);
			if (m_pUninstPswd) m_pUninstPswd->Focus(true);
			return false;
		}

		cBLSettings * pBlSett = ((cBLSettings*)Ser());
		if( pBlSett )
		{
			pBlSett->m_UninstallPwd = m_passwords.m_sNewPswrd;
			((CRootSink *)(Root()->m_pSink))->CryptDecrypt(pBlSett->m_UninstallPwd, true);
		}
	}

	CProfileSettingsDlg::OnOk();
	return true;
}

//////////////////////////////////////////////////////////////////////
// CPswrdCheckDlg

CPswrdCheckDlg::CPswrdCheckDlg(ePasswordConfirmReason eReason, cStrObj& strPswd) :
	TBase(this),
	m_sPswrd(strPswd)
{
	m_nActionsMask |= DLG_ACTION_OKCANCEL|DLG_MODE_ALWAYSMODAL;
	m_eReason = eReason;
	m_strSection = "SettingsWindow.CheckPassword";
}

bool CPswrdCheckDlg::OnOk()
{
	if( ((CRootSink *)(Root()->m_pSink))->CheckPasswords(m_sPswrd, m_sOldPswrd) )
		return TBase::OnOk();

	MsgBox(*this, "ErrPswrdWrong", NULL, MB_OK|MB_ICONERROR);
	m_sOldPswrd.erase();
	UpdateData(true);
	return false;
}*/

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

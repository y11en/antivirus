// OESettings.cpp: implementation of the COESettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OESettings.h"
#include "Utility.h"
#include "OERules.h"
#include "Tracing.h"

namespace
{
	LPCTSTR KL_OE_KEY = _T(VER_PRODUCT_REGISTRY_PATH "\\OEAntiSpam");

	LPCTSTR SPAMACTION_VALUE = _T("SpamAction");
	LPCTSTR SPAMMARKREAD_VALUE = _T("SpamMarkAsRead");
	LPCTSTR SPAMFOLDERID_VALUE = _T("SpamFolderID");
	LPCTSTR PROBSPAMACTION_VALUE = _T("ProbSpamAction");
	LPCTSTR PROBSPAMMARKREAD_VALUE = _T("ProbSpamMarkAsRead");
	LPCTSTR PROBSPAMFOLDERID_VALUE = _T("ProbSpamFolderID");
	LPCTSTR INSTALLED_VALUE = _T("Installed");

	LPCTSTR SPAMRULE_NAME = _T("233");
	LPCTSTR PROBSPAMRULE_NAME = _T("234");
	LPCTSTR SPAMRULE_DISPLAY_NAME = _T("KL Spam rule");
	LPCTSTR PROBSPAMRULE_DISPLAY_NAME = _T("KL Probable Spam rule");

	LPCTSTR SUBJ_SPAM = _T("[!! SPAM]");
	LPCTSTR SUBJ_PROBSPAM = _T("[?? Probable Spam]");
}

int	COESettings::m_nIsOEPluginInstalled = -1;

COESettings::COESettings() :
	m_ruleSpam(SPAMRULE_NAME, SPAMRULE_DISPLAY_NAME),
	m_ruleProbSpam(PROBSPAMRULE_NAME, PROBSPAMRULE_DISPLAY_NAME)
{
	m_nSpamAction = SA_LEAVEASIS;
	m_bSpamMarkRead = false;
	m_dwSpamFolderID = 0;
	
	m_nProbSpamAction = SA_LEAVEASIS;
	m_bProbSpamMarkRead = false;
	m_dwProbSpamFolderID = 0;

	m_ruleSpam.AddSubjCriteria(SUBJ_SPAM);
	m_ruleProbSpam.AddSubjCriteria(SUBJ_PROBSPAM);
}

bool COESettings::IsOEPluginInstalled()
{
	if (m_nIsOEPluginInstalled == 0)
		return false;

	if (m_nIsOEPluginInstalled == 1)
		return true;
	
	tstring strOEKey = KL_OE_KEY;
	strOEKey += _T("\\");
	strOEKey += COERule::GetCurrentIdentityName();

	CRegKey key;
	if (key.Open(HKEY_CURRENT_USER, strOEKey.c_str(), KEY_READ) != ERROR_SUCCESS)
		return false;

	DWORD dwInstalled = 0;
	key.QueryDWORDValue(INSTALLED_VALUE, dwInstalled);

	m_nIsOEPluginInstalled = dwInstalled;

	return (dwInstalled != FALSE) ? true : false;
}

bool COESettings::LoadSettings()
{
	tstring strOEKey = KL_OE_KEY;
	strOEKey += _T("\\");
	strOEKey += COERule::GetCurrentIdentityName();

	CRegKey key;
	if (key.Open(HKEY_CURRENT_USER, strOEKey.c_str(), KEY_READ) != ERROR_SUCCESS)
		return false;

	DWORD dwValue = 0;

	// Spam settings
	if (key.QueryDWORDValue(SPAMACTION_VALUE, dwValue) == ERROR_SUCCESS)
	{
		if ((dwValue >= SA_FIRST) && (dwValue <= SA_LAST))
			m_nSpamAction = (SpamActions) dwValue;
		else
			m_nSpamAction = SA_LEAVEASIS;
	}

	if (key.QueryDWORDValue(SPAMMARKREAD_VALUE, dwValue) == ERROR_SUCCESS)
		m_bSpamMarkRead = (dwValue != 0) ? true : false;

	if (key.QueryDWORDValue(SPAMFOLDERID_VALUE, dwValue) == ERROR_SUCCESS)
		m_dwSpamFolderID = dwValue;

	// Probable spam settings
	if (key.QueryDWORDValue(PROBSPAMACTION_VALUE, dwValue) == ERROR_SUCCESS)
	{
		if ((dwValue >= SA_FIRST) && (dwValue <= SA_LAST))
			m_nProbSpamAction = (SpamActions) dwValue;
		else
			m_nProbSpamAction = SA_LEAVEASIS;
	}
	
	if (key.QueryDWORDValue(PROBSPAMMARKREAD_VALUE, dwValue) == ERROR_SUCCESS)
		m_bProbSpamMarkRead = (dwValue != 0) ? true : false;
	
	if (key.QueryDWORDValue(PROBSPAMFOLDERID_VALUE, dwValue) == ERROR_SUCCESS)
		m_dwProbSpamFolderID = dwValue;

	SetSpamSettings(m_nSpamAction, m_bSpamMarkRead, m_dwSpamFolderID);
	SetProbSpamSettings(m_nProbSpamAction, m_bProbSpamMarkRead, m_dwProbSpamFolderID);
	
	return true;
}

bool COESettings::SaveSettings() const
{
	tstring strOEKey = KL_OE_KEY;
	strOEKey += _T("\\");
	strOEKey += COERule::GetCurrentIdentityName();

	CRegKey key;
	if (key.Create(HKEY_CURRENT_USER, strOEKey.c_str(), NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE) 
		!= ERROR_SUCCESS)
	{
		ASTRACE(_T("Error creating plugin options reg key"));
		return false;
	}

	key.SetDWORDValue(SPAMACTION_VALUE, m_nSpamAction);
	key.SetDWORDValue(SPAMMARKREAD_VALUE, m_bSpamMarkRead ? TRUE : FALSE);
	key.SetDWORDValue(SPAMFOLDERID_VALUE, (DWORD)m_dwSpamFolderID);
	
	key.SetDWORDValue(PROBSPAMACTION_VALUE, m_nProbSpamAction);
	key.SetDWORDValue(PROBSPAMMARKREAD_VALUE, m_bProbSpamMarkRead ? TRUE : FALSE);
	key.SetDWORDValue(PROBSPAMFOLDERID_VALUE, (DWORD)m_dwProbSpamFolderID);

	key.SetDWORDValue(INSTALLED_VALUE, 1);

	m_nIsOEPluginInstalled = 1;

	UpdateRules();
	return true;
}

bool COESettings::UpdateRules() const
{
	m_ruleSpam.SaveRule();
	m_ruleProbSpam.SaveRule();
	return true;
}

void COESettings::SetDefaultSettings(__int64 dwSpamFolderID, __int64 dwProbSpamFolderID)
{
	m_nSpamAction = SA_LEAVEASIS;
	m_bSpamMarkRead = false;
	m_dwSpamFolderID = dwSpamFolderID;
	
	m_nProbSpamAction = SA_LEAVEASIS;
	m_bProbSpamMarkRead = false;
	m_dwProbSpamFolderID = dwProbSpamFolderID;
}

bool COESettings::Initialize()
{
	m_nIsOEPluginInstalled = -1;
	m_ruleSpam.Initialize();
	m_ruleProbSpam.Initialize();
	return true;
}

void COESettings::SetSpamSettings(SpamActions nAction, bool bMarkAsRead, __int64 dwFolderID)
{
	m_nSpamAction = nAction;
	m_bSpamMarkRead = bMarkAsRead;
	m_dwSpamFolderID = dwFolderID;

	m_ruleSpam.ClearActions();
	switch (nAction)
	{
	case SA_LEAVEASIS:
		break;

	case SA_MOVETOFOLDER:
		m_ruleSpam.AddAction(COERule::RA_MOVETO, dwFolderID);
		break;

	case SA_COPYTOFOLDER:
		m_ruleSpam.AddAction(COERule::RA_COPYTO, dwFolderID);
		break;

	case SA_DELETE:
		m_ruleSpam.AddAction(COERule::RA_DELETE);
		break;
	}

	if (bMarkAsRead)
		m_ruleSpam.AddAction(COERule::RA_MARKREAD);
}

void COESettings::SetProbSpamSettings(SpamActions nAction, bool bMarkAsRead, __int64 dwFolderID)
{
	m_nProbSpamAction = nAction;
	m_bProbSpamMarkRead = bMarkAsRead;
	m_dwProbSpamFolderID = dwFolderID;

	m_ruleProbSpam.ClearActions();
	switch (nAction)
	{
	case SA_LEAVEASIS:
		break;
		
	case SA_MOVETOFOLDER:
		m_ruleProbSpam.AddAction(COERule::RA_MOVETO, dwFolderID);
		break;
		
	case SA_COPYTOFOLDER:
		m_ruleProbSpam.AddAction(COERule::RA_COPYTO, dwFolderID);
		break;
		
	case SA_DELETE:
		m_ruleProbSpam.AddAction(COERule::RA_DELETE);
		break;
	}
	
	if (bMarkAsRead)
		m_ruleProbSpam.AddAction(COERule::RA_MARKREAD);
}

COESettings& COESettings::operator=(const COESettings& rhs)
{
	SetSpamSettings(rhs.m_nSpamAction, rhs.m_bSpamMarkRead, rhs.m_dwSpamFolderID);
	SetProbSpamSettings(rhs.m_nProbSpamAction, rhs.m_bProbSpamMarkRead, rhs.m_dwProbSpamFolderID);
	return *this;
}

bool COESettings::DeleteRules()
{
	bool bSpam = m_ruleSpam.DeleteRule();
	bool bProb = m_ruleProbSpam.DeleteRule();

	return (bSpam && bProb);
}

bool COESettings::operator==(const COESettings& rhs) const
{
	return (
		(m_nSpamAction == rhs.m_nSpamAction) &&
		(m_bSpamMarkRead == rhs.m_bSpamMarkRead) &&
		(m_dwSpamFolderID == rhs.m_dwSpamFolderID) &&
		(m_nProbSpamAction == rhs.m_nProbSpamAction) &&
		(m_bProbSpamMarkRead == rhs.m_bProbSpamMarkRead) &&
		(m_dwProbSpamFolderID == rhs.m_dwProbSpamFolderID)
		);
}

bool COESettings::operator!=(const COESettings& rhs) const
{
	return !operator==(rhs);
}

tstring COESettings::GetSettingsRegName()
{
	tstring strOEKey = KL_OE_KEY;
	strOEKey += _T("\\");
	strOEKey += COERule::GetCurrentIdentityName();
	return strOEKey;
}

void COESettings::SetEnabled(bool bEnabled)
{
	m_ruleSpam.SetEnabled(bEnabled);
	m_ruleProbSpam.SetEnabled(bEnabled);
}

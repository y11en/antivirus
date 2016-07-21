// OESettings.h: interface for the COESettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OESETTINGS_H__1402592C_0153_4C17_966F_BAFFA13CEFC7__INCLUDED_)
#define AFX_OESETTINGS_H__1402592C_0153_4C17_966F_BAFFA13CEFC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OERules.h"

class COESettings  
{
public:
	static bool IsOEPluginInstalled();

	enum SpamActions
	{
		SA_FIRST = 0,
		SA_LEAVEASIS = 0,
		SA_MOVETOFOLDER = 1,
		SA_COPYTOFOLDER = 2,
		SA_DELETE = 3,
		SA_UNKNOWN = 4,
		SA_LAST = SA_UNKNOWN
	};

	COESettings();
	COESettings& operator=(const COESettings& rhs);
	bool operator==(const COESettings& rhs) const;
	bool operator!=(const COESettings& rhs) const;

	bool Initialize();
	void SetDefaultSettings(__int64 dwSpamFolderID, __int64 dwProbSpamFolderID);
	void SetSpamSettings(SpamActions nAction, bool bMarkAsRead, __int64 dwFolderID);
	void SetProbSpamSettings(SpamActions nAction, bool bMarkAsRead, __int64 dwFolderID);

	void GetSpamSettings(SpamActions& nAction, bool& bMarkAsRead, __int64& dwFolderID) const
	{
		nAction = m_nSpamAction;
		bMarkAsRead = m_bSpamMarkRead;
		dwFolderID = m_dwSpamFolderID;
	}
	
	void GetProbSpamSettings(SpamActions& nAction, bool& bMarkAsRead, __int64& dwFolderID) const
	{
		nAction = m_nProbSpamAction;
		bMarkAsRead = m_bProbSpamMarkRead;
		dwFolderID = m_dwProbSpamFolderID;
	}

	void SetEnabled(bool bEnabled = false);

	bool LoadSettings();	// from registry
	bool SaveSettings() const;	// to registry

	bool UpdateRules() const;
	bool DeleteRules();	// from registry

	static tstring GetSettingsRegName();

private:
	static int	m_nIsOEPluginInstalled;

	SpamActions	m_nSpamAction;
	bool m_bSpamMarkRead;	// mark the message as read?
	__int64 m_dwSpamFolderID;	// for "Move to..." and "Copy to..." actions

	SpamActions	m_nProbSpamAction;
	bool m_bProbSpamMarkRead;
	__int64 m_dwProbSpamFolderID;

	COERule m_ruleSpam;
	COERule m_ruleProbSpam;
};

#endif // !defined(AFX_OESETTINGS_H__1402592C_0153_4C17_966F_BAFFA13CEFC7__INCLUDED_)

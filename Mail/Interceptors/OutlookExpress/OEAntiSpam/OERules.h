// OERules.h: interface for the COERules class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OERULES_H__CD9F159F_3B0F_43B9_8AC0_00CF60DD9744__INCLUDED_)
#define AFX_OERULES_H__CD9F159F_3B0F_43B9_8AC0_00CF60DD9744__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class COERule
{
public:
	COERule(LPCTSTR szRuleName, LPCTSTR szDisplayName);

	static tstring GetMainOEKey();
	static tstring GetCurrentIdentityName();

	enum RuleActions
	{
		RA_COPYTO	= 0x01,	// folder ID
		RA_MOVETO	= 0x06,	// folder ID
		RA_DELETE	= 0x07,	
		RA_HILIGHT	= 0x0A,	// color ID (0 - N)
		RA_STOPPROC	= 0x0C,
		RA_MARKREAD	= 0x0D
	};

	bool Initialize();
	
	// for now, we support only one - subject - criteria
	bool AddSubjCriteria(LPCTSTR szSubjString);
	bool AddAction(RuleActions nAction, OPTIONAL __int64 dwFolderID = 0, OPTIONAL DWORD dwColorID = 0);
	void ClearActions()
	{
		m_arrActions.swap(ACTIONS_ARRAY());
	}
	
	bool LoadRule();	// from registry
	bool SaveRule() const;	// to registry
	bool DeleteRule();	// from all identities

	void SetEnabled(bool bEnabled = true) {m_bEnabled = bEnabled; }

	static bool ReadInternalValuesVista();

private:
	tstring	m_strRuleName;
	tstring m_strDisplayName;
	
	struct TAction
	{
		RuleActions m_nType;
		__int64	m_dwFolderID;
		DWORD	m_dwColorID;
	};

	struct TCriteria
	{		
		tstring m_strSubjString;
	};

	typedef std::vector<TAction> ACTIONS_ARRAY;
	ACTIONS_ARRAY m_arrActions;
	TCriteria m_Criteria;

	bool m_bEnabled;

	mutable DWORD	m_dwInternal1;
	mutable DWORD	m_dwInternal2;
	mutable bool	m_bInternalsInited;

	static DWORD	m_dwInternal1Vista;
	static DWORD	m_dwInternal2Vista;

	bool _ReadInternalValues() const;
	tstring _SaveAction(CRegKey& key, DWORD dwActionIndex, const TAction& action) const;
	bool _ClearRuleInIdentity(LPCTSTR szIdentityName);
};

#endif // !defined(AFX_OERULES_H__CD9F159F_3B0F_43B9_8AC0_00CF60DD9744__INCLUDED_)

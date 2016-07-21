// SettingsWindow.h: interface for the SettingsWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined _SETTINGSDLGSADM_H_
#define _SETTINGSDLGSADM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ListsView.h"

//////////////////////////////////////////////////////////////////////////
// CLicMgrDlg

struct cLicMgrSettings;

class CLicMgrDlg : public CTaskProfileView
{
	typedef CTaskProfileView TBase;

public:
	CLicMgrDlg(CProfile *pProfileList);
	
protected:
	void OnInited();
	bool OnAddDataSection(LPCTSTR strSect);
	bool OnClicked(CItemBase* pItem);
	bool OnOk();
	void OnEvent(tDWORD nEventId, cSerializable *pData);
	bool BrowseForKeyFile();
	
protected:
	enum tCheckMode { kfCheckFromFile, kfCheckFromMemory };
	tERROR CheckAndLoadKeyFile(tCheckMode eMode);

	cLicMgrSettings *   m_pSett;
	cGuiLicCheckKeyInfo m_KeyInfo;
};

//////////////////////////////////////////////////////////////////////
// CPolicyTree

class CPolicyTree :
	public CListBindingSink,
	public CIndentListAutocheckHelper
{
public:
	CPolicyTree(CProfileAdm *pProfileList) : m_pProfileList(pProfileList){};

protected:
	void OnInited();

protected:
	struct CProfilePolicy
	{
		CProfileAdm*      m_pProfile;
		tDWORD            m_nLevel;
		CLockButtonHolder m_LbHolder;
		bool              m_bLocked;
	};

	struct CConfigSink : public CItemSink
	{
		CConfigSink(CPolicyTree* pTree) : m_pThis(pTree){}
		bool OnClicked(CItemBase * pItem) { return m_pThis->OnConfigure(); }

		CPolicyTree* m_pThis;
	};

	friend struct CConfigSink;

	CProfileAdm* FindProfile(CProfileAdm* pProfileList, LPCSTR strProfile, tDWORD& nLevel);

	bool GetRecord(CRecord &pRecord);
	long GetRecordsCount();
	bool UpdateRecord(CRecord &pRecord);
	bool OnListEvent(int nItem, int nColumn, int nEvent);
	void OnClickIcon(int nItem);
	void OnChangedState(CItemBase * pItem, tDWORD nStateMask = ISTATE_ALL);
	bool OnDblClicked(CItemBase * pItem){ return OnConfigure(); }
	bool OnConfigure();

	tDWORD OnGetRecordIndent(tDWORD nSrcPos);

	CListItem * IndentListAutocheckHelper_OnGetList() { return Item(); }

	CProfileAdm*            m_pProfileList;
	cVector<CProfilePolicy> m_aItems;

	int                     m_icoUnlocked;
	int                     m_icoLocked;

	CItemBase*              m_btnConfig;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // _SETTINGSDLGSADM_H_

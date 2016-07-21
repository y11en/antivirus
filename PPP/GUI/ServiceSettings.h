// ServiceSettings.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICESETTINGS_H__AA90CADE_8429_4816_AA5B_1C67B4785355__INCLUDED_)
#define AFX_SERVICESETTINGS_H__AA90CADE_8429_4816_AA5B_1C67B4785355__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ListsView.h"

//////////////////////////////////////////////////////////////////////
// CNotifyEventsList

class CNotifyEventsList : public CListItemSink
{
public:
	CNotifyEventsList(CItemBase *pItem);

	tDWORD   OnBindingFlags()                    { return ITEMBIND_F_VALUE; }
	tTYPE_ID OnGetType()                         { return (tTYPE_ID)cBLNotifySettings::eIID; }
	bool     OnSetType(tTYPE_ID nType)           { return false; }
	bool     OnGetValue(cVariant &pValue)        { return false; }
	bool     OnSetValue(const cVariant &pValue);

	cSer *   GetRowData(tIdx nItem);
	void     InitView(cBLNotifySettings *pItem, cRowNode *pNode = NULL);

private:
	cVector<cBLNotifySettings *> m_mapIdx;
	cBLNotifySettings *          m_pRoot;
};


/*//////////////////////////////////////////////////////////////////////
// CNotifySettDlg

class CNotifySettDlg : public CProfileSettingsDlg
{
public:
	CNotifySettDlg(CProfile * pProfilesList, CProfile * pProfile) :
		CProfileSettingsDlg(pProfilesList, pProfile) {}

protected:
	SINK_MAP_BEGIN()
		SINK_STATIC("EventList", m_NLst)
	SINK_MAP_END(CProfileSettingsDlg)

	bool OnAddDataSection(LPCTSTR strSect);
	void OnInit();
	bool OnOk();

	CNotifyEventsList m_NLst;
};

//////////////////////////////////////////////////////////////////////
// CPswrdProtectSettDlg

class CPswrdProtectSettDlg : public CProfileSettingsDlg
{
public:
	CPswrdProtectSettDlg(CProfile * pProfilesList, CProfile * pProfile) :
		CProfileSettingsDlg(pProfilesList, pProfile), m_b1(false) {}

protected:
	void OnInited();
	bool OnAddDataSection(LPCTSTR strSect);
	void OnChangedData(CItemBase * pItem);
	bool OnOk();

private:
	bool         m_bProtected;
	CItemBase*   m_pOldPswd;
	CItemBase*   m_pNewPswd;
	CItemBase*   m_pConfirmPswd;
	cGuiPswrdProtection	m_passwords;
	bool         m_b1;
};

//////////////////////////////////////////////////////////////////////
// CUPswrdProtectSettDlg

class CUPswrdProtectSettDlg : public CProfileSettingsDlg
{
public:
	CUPswrdProtectSettDlg(CProfile * pProfilesList, CProfile * pProfile) :
	  CProfileSettingsDlg(pProfilesList, pProfile), m_b1(false) {}

protected:
	void OnInited();
	bool OnAddDataSection(LPCTSTR strSect);
	bool OnOk();

private:
	CItemBase*   m_pUninstPswd;
	CItemBase*   m_pConfUninstPswd;
	cGuiPswrdProtection	m_passwords;
	bool         m_b1;
};

//////////////////////////////////////////////////////////////////////
// CPswrdCheckDlg

class CPswrdCheckDlg :
	public CDialogBindingViewT<>,
	public cGuiPswrdProtection
{
public:
	typedef CDialogBindingViewT<> TBase;
	CPswrdCheckDlg(ePasswordConfirmReason eReason, cStrObj& strPswd);

protected:
	bool OnOk();

private:
	cStrObj m_sPswrd;
};*/

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SERVICESETTINGS_H__AA90CADE_8429_4816_AA5B_1C67B4785355__INCLUDED_)

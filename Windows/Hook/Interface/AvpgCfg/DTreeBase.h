#if !defined(AFX_DTREEBASE_H__59CBF98A_7418_4395_BF2B_8319C8E667E3__INCLUDED_)
#define AFX_DTREEBASE_H__59CBF98A_7418_4395_BF2B_8319C8E667E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DTreeBase.h : header file
//
#include <AVPCONTROLS\ControlTree.h>
#include "FilterTreeInfo.h"
#include "EnumProcess.h"
/////////////////////////////////////////////////////////////////////////////
// CDTreeBase dialog

class CDTreeBase : public CDialog
{
// Construction
public:
	CDTreeBase(CControlTree* pTreeCtrl, DWORD IDD, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDTreeBase)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDTreeBase)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDTreeBase)
	afx_msg LRESULT	OnWindowShutdown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCTTBeginLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCTTEndLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCTTPostLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCTTCancelLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCTTLoadComboContents(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCTTLoadUpDownContext(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCTTDoExternalEditing(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCTTCheckChanging(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCTTLoadIpAddress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCTTEndIpEdit(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	virtual void AddFilterItem(CCTItemsArray* pItems, PFILTER_TRANSMIT pFilter);
	virtual void AddMajFunc(CCTItemsArray* pItems, PFILTER_TRANSMIT pFilter, bool bExpand = false);
	virtual void AddMinFunc(CCTItemsArray* pItems, PFILTER_TRANSMIT pFilter, bool bExpand = false);
	virtual void AddParam(CCTItemsArray* pItems, PFILTER_TRANSMIT pFilter, bool bExpand = false);

	virtual void RemoveAllTreeInfoItems();
	PFILTER_TRANSMIT m_pFilterTransmit;

	char m_pFilterBuffer[FILTER_BUFFER_SIZE];
	PFILTER_TRANSMIT m_pFilterTransmitSave;

	virtual void ItemChanged();
	virtual void UpdateFilterDescribe(int Index, CFilterTreeInfo* pFilterTreeInfo);
	virtual void UpdateSubTree(CFilterTreeInfo* pFilterTreeInfo, int nItemIndex);
	virtual void DeleteSubTreeItemInfo(int nItemIndex);
private:
	CControlTree* m_pTreeCtrl;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DTREEBASE_H__59CBF98A_7418_4395_BF2B_8319C8E667E3__INCLUDED_)

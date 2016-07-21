// guiDlg.h : header file
//

#pragma once

#include "ListCtrlEx.h"
#include "ListRul.h"
#include "ListApp.h"

#include "Cont.h"
//#include "XListCtrl.h"

// CguiDlg dialog



class CguiDlg : public CDialog
{
// Construction
public:
	CguiDlg(CWnd* pParent = NULL);	// standard constructor
	
	void CguiDlg::FillListCtrl(CXListCtrl * pList);
	LRESULT CguiDlg::OnCheckbox(WPARAM nItem, LPARAM nSubItem);
	void CguiDlg::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);

	LRESULT CguiDlg::OnComboSelection(WPARAM nItem, LPARAM nSubItem);
	

	void CguiDlg::InitListsCtrl(list_selector *pls);

	int CguiDlg::TextToNum(WCHAR* str);
	
	//void CguiDlg::OnBnClickedLIST_APL();
	
	//CListCtrlEx	m_List_Rul;
	//CListCtrlEx	m_List_Apl;
	
	CListRul ListRul;
	CListApp ListApp;

	void CguiDlg::RefreshListRul ( int AppItem );
	
	
	list_selector LS;

	CStringArray m_sa1, m_sa2;

	CCont cont;

	//CXListCtrl	m_List;
	
// Dialog Data
	enum { IDD = IDD_GUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	//afx_msg void OnBnClickedBtnAddApl();
	//afx_msg void OnLvnItemchangedListApl(NMHDR *pNMHDR, LRESULT *pResult);
//	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnEnChangeEdit1();
protected:
//	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBAddApl();
	afx_msg void OnLvnItemchangedListRul(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListApl(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnBnClickedBAddDel();
	afx_msg void OnBnClickedBDel();
	afx_msg void OnBnClickedBApply();
	afx_msg void OnBnClickedBAddRul();
	afx_msg void OnBnClickedBAddRulM();
};

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifndef AFX_DLGEDITORSTRING_H__83FBD0A9_FC6B_48BF_83F3_D1B67C79F516__INCLUDED_
#define AFX_DLGEDITORSTRING_H__83FBD0A9_FC6B_48BF_83F3_D1B67C79F516__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
class DlgEditorString : public CDialog
{
public:
	DlgEditorString(CWnd* pParent = NULL);   
	~DlgEditorString();

	//{{AFX_DATA(DlgEditorString)
	enum { IDD = IDD_EDITOR_STRING };
	CString	m_strValueName;
	//}}AFX_DATA

	tTYPE_ID	m_nType;
	BYTE*		m_pBuf;
	DWORD		m_dwSize;
	BOOL		m_bModified;

	//{{AFX_VIRTUAL(DlgEditorString)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	//}}AFX_VIRTUAL

protected:

	BYTE*	m_pInnerBuf;
	DWORD	m_dwInnerBufLen;

	void AllocateInnerBuf(BYTE* i_pBufSrc, DWORD i_dwLen);
	void DeallocateInnerBuf();

	virtual void OnOK();
	void UpdateValue(BOOL i_bSave = TRUE);

	//{{AFX_MSG(DlgEditorString)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#endif // 

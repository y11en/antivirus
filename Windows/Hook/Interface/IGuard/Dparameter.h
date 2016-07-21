#if !defined(AFX_DPARAMETER_H__BE333080_664E_4A60_BD8D_7790389F8D75__INCLUDED_)
#define AFX_DPARAMETER_H__BE333080_664E_4A60_BD8D_7790389F8D75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dparameter.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDParameter dialog

class CDParameter : public CDialog
{
// Construction
public:
	PFILTER_PARAM m_pSingleParamWork;
	CDParameter(PFILTER_PARAM pSingleParam, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDParameter)
	enum { IDD = IDD_DIALOG_PARAMETER };
	CIPAddressCtrl	m_IP4;
	CComboBox	m_cbOperationType;
	CComboBox	m_cbParameterID;
	CString	m_eValue;
	BOOL	m_chCachable;
	BOOL	m_chCalcupReg;
	BOOL	m_chInverseOp;
	BOOL	m_chLowerCase;
	DWORD	m_eWord;
	BOOL	m_chMustExist;
	BYTE	m_eByte;
	UINT	m_eOffsetHi;
	UINT	m_eOffsetLo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDParameter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDParameter)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboParameterId();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void AddParam(PCHAR ParamName, DWORD ParamID);
	void AddOp(PCHAR OpName, DWORD ParamID);

	PFILTER_PARAM m_pSingleParam;
	BYTE m_pFilterBuffer[FILTER_BUFFER_SIZE];
	
	void SwitchParamInput(BOOL bNew);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DPARAMETER_H__BE333080_664E_4A60_BD8D_7790389F8D75__INCLUDED_)

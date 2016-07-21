// LLDiskIOTestDlg.h : header file
//

#if !defined(AFX_LLDISKIOTESTDLG_H__F0BC7080_7770_4354_9493_24F0775AC94C__INCLUDED_)
#define AFX_LLDISKIOTESTDLG_H__F0BC7080_7770_4354_9493_24F0775AC94C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLLDiskIOTestDlg dialog

class CLLDiskIOTestDlg : public CDialog
{
// Construction
public:
	CLLDiskIOTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLLDiskIOTestDlg)
	enum { IDD = IDD_LLDISKIOTEST_DIALOG };
	CComboBox	m_DeviceType;
	CComboBox	m_cbLetter;
	UINT	m_BytesPerSector;
	double	m_Cylinders;
	UINT	m_MediaType;
	UINT	m_SectorsPerTrack;
	UINT	m_TracksPerCylinder;
	double	m_StartingOffset;
	double	m_PartitionLength;
	DWORD	m_HiddenSectors;
	DWORD	m_BootIndicator;
	DWORD	m_PartitionNumber;
	UINT	m_PartitionType;
	UINT	m_RecognizedPartition;
	UINT	m_RewritePartition;
	UINT	m_ReadThisSector;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLLDiskIOTestDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLLDiskIOTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonGetpartitioninfo();
	afx_msg void OnButtonReadThisSector();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LLDISKIOTESTDLG_H__F0BC7080_7770_4354_9493_24F0775AC94C__INCLUDED_)

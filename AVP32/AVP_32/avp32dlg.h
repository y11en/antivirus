// Avp32Dlg.h : header file
//
#include "LogCtrl.h"
#include "TabPages.h"
#include "Cust.h"
#include <ScanAPI/ScanObj.h>
#include <StuffMFC/newtoolbar.h>	// Added by ClassView

//{{AFX_INCLUDES()
#include "reportviewctrl.h"
#include "aboutviewctrl.h"
//}}AFX_INCLUDES


/////////////////////////////////////////////////////////////////////////////
// CAvp32Dlg dialog

#define LVIS_OVERLAY 0x0F00

extern	OSVERSIONINFO os;

extern		DWORD propByTimer;
extern		DWORD propByEvent;
extern		DWORD propEvent;
extern		DWORD propInterval;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAvp32DlgAutoProxy;

/////////////////////////////////////////////////////////////////////////////
// CAvp32Dlg dialog

class CAvp32Dlg : public CDialog
{
	DECLARE_DYNAMIC(CAvp32Dlg);
	friend class CAvp32DlgAutoProxy;

//class CAvp32Dlg : public CDialog
//{
// Construction
public:
	BOOL SaveCustom(const char* name);
	int AVPRegistryChecker(void);
	BOOL DisableControlsVirlist(BOOL dis);
	BOOL OpenReportFile();
	CPoint MinSize;
	CMenu popMenu;
	CAvp32Dlg(CWnd* pParent = NULL);	// standard constructor
	~CAvp32Dlg();	// standard destructor

	CTime BaseTime;
	CString AppName;
	CString CommandLine;
	BOOL clStart;
	BOOL clQuit;
	BOOL clMinimize;
	BOOL clListDelete;
	CString clList;
	CStringArray ScanList;

	BYTE* BinBOOT;
	BYTE* BinMBR;
	BOOL PutReportString(const char* s);
	BOOL SectorWarn;
	int MethodDelete;
	BOOL SelfScan;
	CMenu Menu;
	void ObjectMessage(AVPScanObject* ScanObject,const char* s, BOOL noCheckDone=0, DWORD IpcMess=0);
	DWORD VirCount;
	CDWordArray VirCountArray;
	BOOL UpdateData( BOOL bSaveAndValidate = TRUE );
	UINT ScanAllPaths();
	CStringArray PathArray;
	BOOL InsertTabPage(CTabCtrl* _tab, CWnd* _page);
	int PutLogString( const char* objname, const char* message);
	void StatusLine( const char* newstring );


	CAvpCustomize cust;
	CTabPage1 tPage1;
	CTabPage2 tPage2;
	CTabPage3 tPage3;
//	CTabPage4 tPage4;
	CTabPage5 tPage5;
	CTabPage6 tPage6;

	DWORD functionality;
	UINT Timer;
/*	BOOL	m_CA;
	BOOL	m_Disinfect;
	BOOL	m_Extract;
	BOOL	m_MailBases;
	BOOL	m_MailPlain;
	UINT	m_LicenceAlarm;
*/
/*
	BOOL	m_NOVELL;
	BOOL	m_DOS;
	BOOL	m_DOSLITE;
	BOOL	m_WIN31;
	BOOL	m_WIN95;
	BOOL	m_OS2;
	BOOL	m_WINNT;
*/
/*
	BOOL	m_RemoteLaunch;
	BOOL	m_RemoteScan;
	CString	m_Seller;
	CString	m_Support;
	CString	m_CopyInfo;
	BOOL	m_Unpack;
	CTime	m_LicenceDate;
*/
	CString	m_Name;
	CString	m_Org;
/*
	CString	m_Regnumber;
	BOOL	m_Every;
	BOOL	m_CRC;
	BOOL	m_Monitor;
*/

// Dialog Data
	//{{AFX_DATA(CAvp32Dlg)
	enum { IDD = IDD_AVP32_DIALOG };
	CProgressCtrl	m_Progress;
	CAnimateCtrl	m_AnimateScan;
//	CLogCtrl	m_List;
	CTabCtrl	m_Tab1;
	CString	m_sLine;
	BOOL	m_Track;
	CReportViewCtrl	m_RepView;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvp32Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI );
	//}}AFX_VIRTUAL
	

	
public:
	void CancelProcess();
	CString m_FPSuspFolder;
	CString m_FPInfectFolder;
	int SignCheckFile(const char* filename);
	int SignReportMB(const char* filename, int ret, int mode);
	int sign_check;
//	CStringArray ExcludeArray;
	BOOL SendStatisticsByEvent(DWORD event);
	const char* MakeReport(CString &s);
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAvpReloadbases();
	HANDLE hReportFile;
	HANDLE hVirlist;
	BOOL ReportWithTiming;
	BOOL HiResCounterSupported;
	BOOL DisableControls(BOOL dis);
	CWinThread* ScanThreadPtr;
	CWinThread* VirThreadPtr;
	
// Implementation
protected:
	CAvp32DlgAutoProxy* m_pAutoProxy;
	CDlgToolBar m_tbToolbar;
	int ApplyToAll;
	int LastAction;
	BOOL CopyToFolder(AVPScanObject* ScanObject, const char *folder);
	BOOL SaveProfile(const char* name);
	BOOL LoadProfile(const char* name);
	CTime StartTime;
	BOOL CanCancel;
	HANDLE hFileMapT;
	UINT ScanPath( const char* path ); //Thread generation
	CheckBoot(CString& Path, DWORD type);
	CheckMBRS();
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAvp32Dlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAvpChangetab();
	afx_msg void OnAvpScanEnd();
	afx_msg LONG OnAVPMessage( UINT uParam, LONG lParam);
	afx_msg LONG OnAvpupd( UINT uParam, LONG lParam);
	afx_msg LONG OnUpdateData(UINT uParam, LONG lParam);
	afx_msg void OnClose();
	afx_msg void OnAvpStart();
	afx_msg void OnHelpAboutavp32();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnScanNow();
	afx_msg void OnStop();
	afx_msg void OnScanNowButton();
	afx_msg void OnFileLoadprofile();
	afx_msg void OnFileSaveprofile();
	afx_msg void OnFileSaveprofileasdefault();
	afx_msg void OnFind();
	afx_msg void OnFindagain();
	afx_msg void OnTrack();
	afx_msg void OnAddDisks();
	afx_msg void OnPopMark();
	afx_msg void OnPopScan();
	afx_msg void OnPopUnmark();
	afx_msg void OnToolsUpdatenow();
	afx_msg void OnHelpAvpontheweb();
	afx_msg void OnCustomize();
	afx_msg void OnVirlist();
	afx_msg void OnRVCGetLocalisedString(long nLocaleID, BSTR FAR* pString);
	afx_msg void OnRVCGetFindContext(BSTR FAR* pContext, long FAR* pdwFlags, BOOL FAR* pbDone);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};
/////////////////////////////////////////////////////////////////////////////
// AboutDlg dialog

class AboutDlg : public CDialog
{
// Construction
public:
	AboutDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AboutDlg)
	enum { IDD = IDD_ABOUT_DIALOG };
	CAboutViewCtrl	m_avcAbout;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBitmap m_David;

	// Generated message map functions
	//{{AFX_MSG(AboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnABTGetLocalisedStringAvpaboutview(long nStringID, BSTR FAR* pString);
	afx_msg void OnABTGetGeneralSupportInfoAvpaboutview(BSTR FAR* pSupportInfo);
	afx_msg void OnABTDisplayContextHelpAvpaboutview(LONG id);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CSupportDlg dialog

class CSupportDlg : public CDialog
{
// Construction
public:
	CSupportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSupportDlg)
	enum { IDD = IDD_SUPPORT_DIALOG };
	CString	m_Support;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSupportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSupportDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

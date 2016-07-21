// DLog.cpp : implementation file
//

#include "stdafx.h"
#include "avpgcfg.h"
#include "DLog.h"

#include "AVPRptID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLog dialog


CDLog::CDLog(CWnd* pParent /*=NULL*/)
	: CDialog(CDLog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLog)
	m_chAutoTracking = TRUE;
	//}}AFX_DATA_INIT
}


void CDLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLog)
	DDX_Control(pDX, IDC_COMBO_SESSIONS, m_cbSessions);
	DDX_Control(pDX, IDC_AVPREPORTVIEW, m_axReportViewCtrl);
	DDX_Check(pDX, IDC_CHECK_AUTOTRACKING, m_chAutoTracking);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLog, CDialog)
	//{{AFX_MSG_MAP(CDLog)
	ON_WM_SIZE()
	ON_MESSAGE(WU_STARTINIT, OnStartInit)
	ON_CBN_SELCHANGE(IDC_COMBO_SESSIONS, OnSelchangeComboSessions)
	ON_BN_CLICKED(IDC_CHECK_AUTOTRACKING, OnCheckAutotracking)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLog message handlers

BOOL CDLog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	DATA_Init_Library(malloc, free);
//	KLDT_Init_Library(malloc, free);
	DTUT_Init_Library(malloc, free);

//	LoadIO();

	SetWindowLong(m_axReportViewCtrl, GWL_ID, IDC_AVPREPORTVIEW);

	PostMessage(WU_STARTINIT);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDLog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindow(m_axReportViewCtrl.m_hWnd))
	{
		static RULE rules[] = {    // Action    Act-on                   Relative-to           Offset
		// ------    ------                   -----------           ------
		{lSTRETCH, lRIGHT(IDC_COMBO_SESSIONS),			lRIGHT(lPARENT)				,-1},
		{lSTRETCH, lRIGHT(IDC_AVPREPORTVIEW),			lRIGHT(lPARENT)				,-1},
		{lSTRETCH, lBOTTOM(IDC_AVPREPORTVIEW),			lBOTTOM(lPARENT)			,-1},

		{lEND}};

		Layout_ComputeLayout( GetSafeHwnd(), rules);
	}	
}

void CDLog::SaveColumnWidth(void)
{
	HDATA hRoot;
	if (!m_axReportViewCtrl.GetHeaderColumnWidths((long*) &hRoot))
	{
		HPROP hProp = DATA_Find_Prop(hRoot, 0, 0);
		if (hProp)
		{
			int count = PROP_Arr_Count(hProp);
			DWORD Width;
			CString strtmp;
			for (int cou = 0; cou < count; cou++)
			{
				Width = DTUT_GetPropArrayNumericValueAsDWord(hProp, cou);
				strtmp.Format("Column %d", cou);
				g_pReestr->SetValue(strtmp, Width);
			}
		}
		DATA_Remove(hRoot, 0);
	}
}

void CDLog::RestoreColumnWidth(void)
{
	AvpRPTCortegeFlags Flags;
	Flags.m_nFlags = 0;
	Flags.m_nType = RPT_CRTG_HEADER;	
	
	AvpRPTDomainFlags DFlags;
	
//#define _STR_LOG_FMT "\t%s\t%s\t%s\t%s - %s"
	CString Cortege, strtmp;
	Cortege.Format("<%x", Flags.m_nFlags);
	int cou = 0;
	strtmp = "Column 0";
#define colcount 6
	if (!g_pReestr->IsKey(strtmp, REG_DWORD))
	{
		g_pReestr->SetValue("Column 0", 100/colcount);
		g_pReestr->SetValue("Column 1", 100/colcount);
		g_pReestr->SetValue("Column 2", 100/colcount);
		g_pReestr->SetValue("Column 3", 100/colcount);
		g_pReestr->SetValue("Column 4", 100/colcount);
	}
	while (g_pReestr->IsKey(strtmp, REG_DWORD))
	{
		DFlags.m_nFlags = 0;
		DFlags.m_nHWidthPercent = g_pReestr->GetValue(strtmp, 33);
		DFlags.m_nHAlignType = RPT_ALGN_LEFT;
		switch (cou)
		{
		case 0:
			DFlags.m_nHStringId = IDS_REPORT_DATETIME;
			break;
		case 1:
			DFlags.m_nHStringId = IDS_REPORT_APPNAME;
			break;
		case 2:
			DFlags.m_nHStringId = IDS_REPORT_FUNCTION;
			break;
		case 3:
			DFlags.m_nHStringId = IDS_REPORT_INFO;
			break;
		case 4:
			DFlags.m_nHStringId = IDS_REPORT_ACTION;
			break;
		default:
			DFlags.m_nHStringId = IDS_ERRORMSG;
			break;
		}
		strtmp.Format(".%x", DFlags.m_nFlags);
		Cortege += strtmp;
		strtmp.Format("Column %d", ++cou);
	}
	Cortege += ">";

	m_axReportViewCtrl.SetReportCortege(Cortege);
}

BEGIN_EVENTSINK_MAP(CDLog, CDialog)
    //{{AFX_EVENTSINK_MAP(CDLog)
	ON_EVENT(CDLog, IDC_AVPREPORTVIEW, 1 /* RVCGetCortegeIcon */, OnRVCGetCortegeIcon, VTS_I4 VTS_PI4)
	ON_EVENT(CDLog, IDC_AVPREPORTVIEW, 5 /* RVCGetLocalisedString */, OnRVCGetLocalisedString, VTS_I4 VTS_PBSTR)
	ON_EVENT(CDLog, IDC_AVPREPORTVIEW, 6 /* RVCGetFindContext */, OnRVCGetFindContext, VTS_PBSTR VTS_PI4 VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CDLog::OnRVCGetCortegeIcon(long nIconID, long FAR* phIcon) 
{
	// TODO: Add your control notification handler code here
	*phIcon = (long) LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(ID_ICON_LOG));	
}

void CDLog::OnRVCGetLocalisedString(long nLocaleID, BSTR FAR* pString) 
{
	// TODO: Add your control notification handler code here
	CString rcText;
	if (g_pLocalize->GetLocalizeString(nLocaleID, &rcText))
	{
		::SysFreeString( *pString );
		*pString = rcText.AllocSysString();
	}	
}

void CDLog::OnRVCGetFindContext(BSTR FAR* pContext, long FAR* pdwFlags, BOOL FAR* pbDone) 
{
	// TODO: Add your control notification handler code here
	
}

long CDLog::OnStartInit(WPARAM wParam, LPARAM lParam)
{
	RestoreColumnWidth();

	m_cbSessions.ResetContent();

	HDATA hRoot = 0;
	if (!m_axReportViewCtrl.GetSessionHeadlines((long*)&hRoot, AppReg.m_LogFileName))
	{
		if (hRoot != NULL)
		{
			HPROP hProp = DATA_Find_Prop(hRoot, 0, 0);
			if (hProp != NULL)
			{
				int count = PROP_Arr_Count(hProp);
				char *pch;
				int idx;
				for (int cou = 0; cou < count; cou++)
				{
					pch = DTUT_GetPropArrayValueAsString(hProp, cou, NULL);
					idx = m_cbSessions.AddString(pch);
					if (idx != CB_ERR  && idx != CB_ERRSPACE)
						m_cbSessions.SetItemData(idx, cou);
					free(pch);
				}
			}
			DATA_Remove(hRoot, 0);
		}
		m_cbSessions.SetCurSel(m_cbSessions.GetCount() - 1);
	}

	m_axReportViewCtrl.SetAutoTracking(m_chAutoTracking);
	m_axReportViewCtrl.AcceptReportFile(AppReg.m_LogFileName);
	
	return 0;
}

void CDLog::OnSelchangeComboSessions() 
{
	// TODO: Add your control notification handler code here
	int idx = m_cbSessions.GetCurSel();
	long Session = m_cbSessions.GetItemData(idx);
	if ((m_cbSessions.GetCount() - 1) == Session)
		Session = -1;
	m_axReportViewCtrl.SetViewedSessionNumber(Session);	
}

void CDLog::OnCheckAutotracking() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_axReportViewCtrl.SetAutoTracking(m_chAutoTracking);	
}

BOOL CDLog::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	SaveColumnWidth();

	return CDialog::DestroyWindow();
}

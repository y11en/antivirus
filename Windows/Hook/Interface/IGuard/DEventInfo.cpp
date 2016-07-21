// DEventInfo.cpp : implementation file
//

#include "stdafx.h"
#include "iguard.h"
#include "DEventInfo.h"
#include "DHexInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDEventInfo dialog


CDEventInfo::CDEventInfo(PEVENT_TRANSMIT pEvt, CWnd* pParent /*=NULL*/)
	: CDialog(CDEventInfo::IDD, pParent)
{
	m_pEvt = pEvt;
	//{{AFX_DATA_INIT(CDEventInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDEventInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDEventInfo)
	DDX_Control(pDX, IDC_LIST_PARAMS, m_lParams);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDEventInfo, CDialog)
	//{{AFX_MSG_MAP(CDEventInfo)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PARAMS, OnDblclkListParams)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDEventInfo message handlers

BOOL CDEventInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strtmp;
	strtmp.Format("%d", m_pEvt->m_FilterID);
	GetDlgItem(IDC_STATIC_FILTER_ID)->SetWindowText(strtmp);

	DWORD dwIdx;

	PCHAR pFunc = FillFuncList(NULL, m_pEvt->m_HookID, m_pEvt->m_FunctionMj, m_pEvt->m_FunctionMi, NULL, &dwIdx);
	PCHAR pFuncMj = FillMajList(NULL, m_pEvt->m_HookID, m_pEvt->m_FunctionMj, m_pEvt->m_FunctionMi, NULL, &dwIdx);
	PCHAR pFuncMi = FillMinList(NULL, m_pEvt->m_HookID, m_pEvt->m_FunctionMj, m_pEvt->m_FunctionMi, NULL, &dwIdx);

	if (pFunc == NULL) pFunc = "<unknown>";
	if (pFuncMj == NULL) pFuncMj = "<empty>";
	if (pFuncMi == NULL) pFuncMi = "<empty>";

	strtmp.Format("%s - %s - %s", pFunc, pFuncMj, pFuncMi);
	GetDlgItem(IDC_STATIC_INTERCEPT_INFO)->SetWindowText(strtmp);

	int Item = 0;
	CHAR EventInfo[4096];
	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) m_pEvt->m_Params;
	
	m_lParams.InsertItem(Item, m_pEvt->m_ProcName);
	m_lParams.SetItemData(Item++, NULL);
	for (DWORD cou =  0; (cou < m_pEvt->m_ParamsCount); cou++)
	{
		GetSingleParameterDescription(pSingleParam, EventInfo);
		
		m_lParams.SetItemData(m_lParams.InsertItem(Item++, EventInfo), (DWORD) pSingleParam);

		pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
	}

	SetForegroundWindow();

	RestoreWindowPosition();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDEventInfo::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindow(m_lParams.m_hWnd))
	{
		static RULE rules[] = {
			// Action    Act-on							Relative-to				Offset
			// ------    ------							-----------				------
			{lMOVE,		lBOTTOM(IDOK),								lBOTTOM(lPARENT)			, -3},
			{lMOVE,		lRIGHT(IDOK),								lRIGHT(lPARENT)			, -3},
			{lSTRETCH,	lRIGHT(IDC_LIST_PARAMS),				lRIGHT(lPARENT)			, -3},
			{lSTRETCH,	lBOTTOM(IDC_LIST_PARAMS),				lTOP(IDOK)					, -5},
			{lSTRETCH,	lRIGHT(IDC_STATIC_INTERCEPT_INFO),	lRIGHT(lPARENT)			, -3},
			{lEND}
		};
		Layout_ComputeLayout(m_hWnd, rules);	
	}
}

void CDEventInfo::StoreWindowPosition()
{
	WINDOWPLACEMENT wpl;
	GetWindowPlacement(&wpl);
	
	g_pRegStorage->PutValue("EventInfoWindow", (BYTE*) &wpl, sizeof(wpl));
}

void CDEventInfo::RestoreWindowPosition()
{
	WINDOWPLACEMENT wpl;
	DWORD dw = sizeof(wpl);
	if (g_pRegStorage->GetValue("EventInfoWindow", (BYTE*)&wpl, dw))
		SetWindowPlacement(&wpl);
}

BOOL CDEventInfo::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	StoreWindowPosition();
	
	return CDialog::DestroyWindow();
}

void CDEventInfo::OnDblclkListParams(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_lParams.GetFirstSelectedItemPosition();		
	
	int Item;
	PSINGLE_PARAM pSingleParam;
	while (pos != NULL)
	{
		Item = m_lParams.GetNextSelectedItem(pos);
		pSingleParam = (PSINGLE_PARAM) m_lParams.GetItemData(Item);
		if (pSingleParam != NULL)
		{
			CDHexInfo HexInfo;
			HexInfo.SetHexData(pSingleParam->ParamValue, pSingleParam->ParamSize);
			HexInfo.DoModal();
		}
	}
	
	*pResult = 0;
}

// DApplications.cpp : implementation file
//

#include "stdafx.h"
#include "AllowedRun.h"
#include "DApplications.h"
#include "DApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static RULE gRulesApps[] = {
	// Action    Act-on							Relative-to					Offset
	// ------    ------							-----------					------
	{lSTRETCH,	lRIGHT(IDC_LIST_APPS),			lRIGHT(lPARENT)				, -1},
	{lSTRETCH,	lBOTTOM(IDC_LIST_APPS),			lBOTTOM(lPARENT)			, -1},
	{lEND},
};
/////////////////////////////////////////////////////////////////////////////
// CDApplications dialog


CDApplications::CDApplications(CWnd* pParent /*=NULL*/)
	: CDialog(CDApplications::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDApplications)
	//}}AFX_DATA_INIT
	m_hAccel = NULL;
	m_bShow = FALSE;
}


void CDApplications::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDApplications)
	DDX_Control(pDX, IDC_LIST_APPS, m_lsApps);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDApplications, CDialog)
	//{{AFX_MSG_MAP(CDApplications)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(IDA_ENTER, OnEnter)
	ON_MESSAGE(WU_ADDNEW, OnAddnew)
	ON_MESSAGE(WU_DELETE, OnDelete)
	ON_MESSAGE(WU_EDIT, OnEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_APPS, OnDblclkListApps)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDApplications message handlers
BOOL CDApplications::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if((LOWORD(wParam) == IDCANCEL) || (LOWORD(wParam) == IDOK))
		return TRUE;
	
	return CDialog::OnCommand(wParam, lParam);
}

void CDApplications::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	m_bShow = bShow;
	
	if (bShow)
	{
		if (IsWindow(m_lsApps))
		{
			Refresh();
			m_lsApps.SetFocus();
		}
	}
}

void CDApplications::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (IsWindow(m_lsApps))
		Layout_ComputeLayout(GetSafeHwnd(), gRulesApps);
}


long CDApplications::OnAddnew(WPARAM wParam, LPARAM lparam) 
{
	CString Process;
	Process.Empty();
	
	CDApp DApp(&Process, this);
	if (DApp.DoModal() != IDOK)
		return 0;

	if (!ExistNameInListCtrl(&m_lsApps, &Process))
		AddAppFilter(Process);

	Refresh();

	return 0;
}

long CDApplications::OnDelete(WPARAM wParam, LPARAM lparam) 
{
	POSITION pos = m_lsApps. GetFirstSelectedItemPosition();
	BOOL bFailed = FALSE;
	if (pos != NULL)
	{
		if ((m_lsApps.GetSelectedCount()) == 1 ||
			(MessageBox(_T("Are you sure?"), _T("Delete applications filters?"), MB_ICONQUESTION | MB_YESNO) == IDYES))
		{
			PFILTER_TRANSMIT pFilter;
			int Item;
			while (pos != NULL)
			{
				Item = m_lsApps.GetNextSelectedItem(pos);
				
				pFilter = (PFILTER_TRANSMIT) m_lsApps.GetItemData(Item);
				if (pFilter != NULL)
				{
					if (gpFilters->DeleteFilter(pFilter) == false)
						bFailed = TRUE;
				}
			}
		}
		if (bFailed == TRUE)
			MessageBox(_T("Sorry - can't delete some filters"), _T("Warning"), MB_ICONINFORMATION);

		Refresh();
	}
	
	return 0;
}

void CDApplications::Refresh()
{
	gpFilters->ReloadFiltersArray();

	m_lsApps.DeleteAllItems();
	
	PFILTER_TRANSMIT pFilter;

	CString FilterDescription;
	int Count = gpFilters->m_FiltersArray.GetSize();
	if (Count != 0)
	{
		int ItemID;

		for (int cou = 0; cou < Count; cou++)
		{
			pFilter = gpFilters->m_FiltersArray.ElementAt(cou);
			if (pFilter == NULL)
				continue;
			FilterDescription = pFilter->m_ProcName;
			if (FilterDescription != _T("*"))
			{
				ItemID = m_lsApps.InsertItem(cou, FilterDescription);
				if (ItemID != -1)
					m_lsApps.SetItemData(ItemID, (DWORD) pFilter);
			}
		}
	}
}

BOOL CDApplications::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR_COMMON));
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDApplications::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_bShow && m_hAccel != NULL)
	{
		if (TranslateAccelerator(this->m_hWnd, m_hAccel, pMsg))
			return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDApplications::OnEnter() 
{
	// TODO: Add your command handler code here
	OnAddnew(0, 0);
}

void CDApplications::OnDblclkListApps(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnEdit(0, 0);
	*pResult = 0;
}

long CDApplications::OnEdit(WPARAM wParam, LPARAM lParam)
{
	if (m_lsApps.GetSelectedCount() > 1)
		return 0;

	POSITION pos = m_lsApps.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return 0;

	PFILTER_TRANSMIT pFilter;

	int Item = m_lsApps.GetNextSelectedItem(pos);
	pFilter = (PFILTER_TRANSMIT) m_lsApps.GetItemData(Item);

	if (pFilter == NULL)
		return 0;
			
	CString Process = m_lsApps.GetItemText(Item, 0);
	CString ProcessOld = Process;

	CDApp DApp(&Process, this);
	if (DApp.DoModal() != IDOK)
		return 0;

	if (ProcessOld != Process)
	{
		BOOL bFailed = FALSE;
		if (gpFilters->DeleteFilter(pFilter) == false)
			bFailed = TRUE;

		if (!ExistNameInListCtrl(&m_lsApps, &Process))
			AddAppFilter(Process);

		Refresh();

		if (bFailed == TRUE)
			MessageBox(_T("Sorry - can't modify some filters"), _T("Warning"), MB_ICONINFORMATION);
	}

	return 0;
}

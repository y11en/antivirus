// DFolders.cpp : implementation file
//

#include "stdafx.h"
#include "AllowedRun.h"
#include "DFolders.h"
#include "DFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static RULE gRulesFolders[] = {
	// Action    Act-on							Relative-to					Offset
	// ------    ------							-----------					------
	{lSTRETCH,	lRIGHT(IDC_LIST_FOLDERS),		lRIGHT(lPARENT)				, -1},
	{lSTRETCH,	lBOTTOM(IDC_LIST_FOLDERS),		lBOTTOM(lPARENT)			, -1},
	{lEND},
};
/////////////////////////////////////////////////////////////////////////////
// CDFolders dialog


CDFolders::CDFolders(CWnd* pParent /*=NULL*/)
	: CDialog(CDFolders::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDFolders)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hAccel = NULL;
	m_bShow = FALSE;
}


void CDFolders::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDFolders)
	DDX_Control(pDX, IDC_LIST_FOLDERS, m_lsFolders);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDFolders, CDialog)
	//{{AFX_MSG_MAP(CDFolders)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(IDA_ENTER, OnEnter)
	ON_MESSAGE(WU_ADDNEW, OnAddnew)
	ON_MESSAGE(WU_DELETE, OnDelete)
	ON_MESSAGE(WU_EDIT, OnEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FOLDERS, OnDblclkListFolders)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFolders message handlers

BOOL CDFolders::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if((LOWORD(wParam) == IDCANCEL) || (LOWORD(wParam) == IDOK))
		return TRUE;
	
	return CDialog::OnCommand(wParam, lParam);
}

void CDFolders::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (IsWindow(m_lsFolders))
		Layout_ComputeLayout(GetSafeHwnd(), gRulesFolders);
}

void CDFolders::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	m_bShow = bShow;
	
	if (bShow)
	{
		if (IsWindow(m_lsFolders))
		{
			Refresh();
			m_lsFolders.SetFocus();
		}
	}
}

BOOL CDFolders::DeleteFiltersFromList(CString *pFolder)
{
	BOOL bRet = TRUE;
	int Count = gpFilters->m_FiltersArray.GetSize();
	if (Count == 0)
		return bRet;

	PFILTER_TRANSMIT pFilter;
	PFILTER_PARAM pParam;

	CString folder;
	for (int cou = 0; cou < Count; cou++)
	{
		pFilter = (PFILTER_TRANSMIT) gpFilters->m_FiltersArray.ElementAt(cou);
		if (pFilter == NULL)
			continue;

		pParam = IDriverGetFilterParam(pFilter, _PARAM_OBJECT_URL_W);
		if (pParam != NULL)
		{
			folder = (WCHAR*) pParam->m_ParamValue;
			if (folder == *pFolder)
			{
				if (gpFilters->DeleteFilter(pFilter, true) == false)
					bRet = FALSE;
			}
		}
	}

	return bRet;
}

long CDFolders::OnAddnew(WPARAM wParam, LPARAM lparam) 
{
	CString folder;
	CDFolder DFolder(&folder, this);
	folder.Empty();

	if (DFolder.DoModal() == IDOK)
	{
		if (!ExistNameInListCtrl(&m_lsFolders, &folder))
			AddFolderFilter(folder);
		Refresh();
	}

	return 0;
}

long CDFolders::OnDelete(WPARAM wParam, LPARAM lparam) 
{
	POSITION pos = m_lsFolders. GetFirstSelectedItemPosition();		
	BOOL bFailed = FALSE;

	if (pos != NULL)
	{
		if ((m_lsFolders.GetSelectedCount()) == 1 ||
			(MessageBox(_T("Are you sure?"), _T("Delete folders filters?"), MB_ICONQUESTION | MB_YESNO) == IDYES))
		{
			int Item;
			CString folder;
			
			PFILTER_TRANSMIT pFilter;
			PFILTER_PARAM pParam;
			while (pos != NULL)
			{
				Item = m_lsFolders.GetNextSelectedItem(pos);
				
				pFilter = (PFILTER_TRANSMIT) m_lsFolders.GetItemData(Item);
				if (pFilter == NULL)
					continue;

				pParam = IDriverGetFilterParam(pFilter, _PARAM_OBJECT_URL_W);
				if (pParam != NULL)
				{
					folder = (WCHAR*) pParam->m_ParamValue;
					if (DeleteFiltersFromList(&folder) == FALSE)
						bFailed = TRUE;
				}
				else
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

void CDFolders::Refresh()
{
	gpFilters->ReloadFiltersArray();

	m_lsFolders.DeleteAllItems();
	
	PFILTER_TRANSMIT pFilter;

	CString FilterDescription;
	int Count = gpFilters->m_FiltersArray.GetSize();
	if (Count == 0)
		return;

	int ItemID;

	PFILTER_PARAM pParam;
	for (int cou = 0; cou < Count; cou++)
	{
		pFilter = gpFilters->m_FiltersArray.ElementAt(cou);
		if (pFilter == NULL)
			continue;
		
		pParam = IDriverGetFilterParam(pFilter, _PARAM_OBJECT_URL_W);
		if (pParam == NULL)
			m_lsFolders.InsertItem(cou, _T("unknown filter"));
		else
		{
			FilterDescription = (PWCHAR) pParam->m_ParamValue;
			if (FilterDescription == _T("*"))
				continue;

			if (!ExistNameInListCtrl(&m_lsFolders, &FilterDescription))
			{
				ItemID = m_lsFolders.InsertItem(cou, FilterDescription);
				if (ItemID != -1)
					m_lsFolders.SetItemData(ItemID, (DWORD) pFilter);
			}
		}
	}
}

BOOL CDFolders::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR_COMMON));
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDFolders::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_bShow && m_hAccel != NULL)
	{
		if (TranslateAccelerator(this->m_hWnd, m_hAccel, pMsg))
			return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDFolders::OnEnter() 
{
	// TODO: Add your command handler code here
	OnAddnew(0, 0);
}

void CDFolders::OnDblclkListFolders(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnEdit(0, 0);
	
	*pResult = 0;
}

long CDFolders::OnEdit(WPARAM wParam, LPARAM lParam)
{
	POSITION pos = m_lsFolders. GetFirstSelectedItemPosition();		
	BOOL bFailed = FALSE;

	if (pos != NULL)
	{
		int Item;
		CString folder;
		
		PFILTER_TRANSMIT pFilter;
		PFILTER_PARAM pParam;
		while (pos != NULL)
		{
			Item = m_lsFolders.GetNextSelectedItem(pos);
			
			pFilter = (PFILTER_TRANSMIT) m_lsFolders.GetItemData(Item);
			if (pFilter == NULL)
				continue;

			pParam = IDriverGetFilterParam(pFilter, _PARAM_OBJECT_URL_W);
			if (pParam != NULL)
			{
				folder = (WCHAR*) pParam->m_ParamValue;
				CString newfolder = folder;

				if (newfolder.Right(1) == _T("*"))
					newfolder.SetAt(newfolder.GetLength() - 1, 0);

				CDFolder DFolder(&newfolder, this);
				
				if (DFolder.DoModal() == IDOK)
				{
					if (newfolder.Right(1) != _T("*"))
						newfolder += _T("*");
					if (newfolder != folder)
					{
						if (DeleteFiltersFromList(&folder) == FALSE)
							bFailed = TRUE;

						if (!ExistNameInListCtrl(&m_lsFolders, &newfolder))
							AddFolderFilter(newfolder);
					}
				}
			}
			else
			{
				if (gpFilters->DeleteFilter(pFilter) == false)
					bFailed = TRUE;
			}
		}
		
		Refresh();
	}
	if (bFailed == TRUE)
	{
		MessageBox(_T("Sorry - can't modify some filters"), _T("Warning"), MB_ICONINFORMATION);
	}
	Refresh();

	return 0;
}

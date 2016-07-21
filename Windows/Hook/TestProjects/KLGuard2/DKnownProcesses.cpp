// DKnownProcesses.cpp : implementation file
//

#include "stdafx.h"
#include "klguard2.h"
#include "DKnownProcesses.h"
#include "ListColumnPositions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define _L_ProcName					0
#define _L_ImagePath				1
#define _L_Version					2
#define _L_FileDescription			3

typedef struct _ColumnSort
{
	CListCtrl* m_pCtrl;
	bool m_bSortAscending;
	int m_ColumnIdx;
}ColumnSort;

static int CALLBACK 
KnownProcessCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// lParamSort contains a pointer to the list view control.
	_ColumnSort* pSort = (_ColumnSort*) lParamSort;
	CString* strItem1 = (CString*) lParam1;
	CString* strItem2 = (CString*) lParam2;
	
	int cmp = lstrcmp(strItem2->GetBuffer(0), strItem1->GetBuffer(0));
	strItem1->ReleaseBuffer();
	strItem2->ReleaseBuffer();
	if (!pSort->m_bSortAscending)
		return cmp;
	
	return -cmp;
}

/////////////////////////////////////////////////////////////////////////////
// CDKnownProcesses dialog


CDKnownProcesses::CDKnownProcesses(CKnownProcessList* pKnownList, CWnd* pParent /*=NULL*/)
	: CDialog(CDKnownProcesses::IDD, pParent)
{
	m_pKnownList = pKnownList;
	//{{AFX_DATA_INIT(CDKnownProcesses)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bSortAscending = true;
}

void CDKnownProcesses::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDKnownProcesses)
	DDX_Control(pDX, IDC_BUTTON_REMOVE, m_btRemove);
	DDX_Control(pDX, IDC_LIST_KNOWNLIST, m_lKnownProcesses);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDKnownProcesses, CDialog)
	//{{AFX_MSG_MAP(CDKnownProcesses)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_KNOWNLIST, OnItemchangedListKnownlist)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_ADDNEW, OnButtonAddnew)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_KNOWNLIST, OnColumnclickListKnownlist)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_KNOWNLIST, OnDeleteitemListKnownlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDKnownProcesses message handlers

BOOL CDKnownProcesses::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	ListView_SetExtendedListViewStyle(m_lKnownProcesses.m_hWnd, LVS_EX_FULLROWSELECT);

	m_lKnownProcesses.InsertColumn(_L_ProcName, _T("Process name"), LVCFMT_LEFT, 100);
	m_lKnownProcesses.InsertColumn(_L_ImagePath, _T("Image path"), LVCFMT_LEFT, 340);
	m_lKnownProcesses.InsertColumn(_L_Version, _T("Version"), LVCFMT_LEFT, 140);
	m_lKnownProcesses.InsertColumn(_L_FileDescription, _T("FileDescription"), LVCFMT_LEFT, 140);
	
	RestorePosition();
	ReloadList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDKnownProcesses::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI)
{
	CWnd::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x = 490;
	lpMMI->ptMinTrackSize.y = 240;
}

void CDKnownProcesses::SavePosition()
{
	//	TRACE("\nOwner save pos\n");
	WINDOWPLACEMENT wpl;
	GetWindowPlacement(&wpl);

	g_pReestr->SetBinaryValue(_T("KnownPosition"), (DWORD)&wpl, sizeof(wpl));
}

void CDKnownProcesses::RestorePosition()
{
	WINDOWPLACEMENT wpl;
	UINT dw = 0;

	if (g_pReestr->GetBinaryValue(_T("KnownPosition"), (DWORD) &wpl, sizeof(wpl)))
		SetWindowPlacement(&wpl);

	CListColumnPositions list_columns(_T("KnownProcess_List"), &m_lKnownProcesses);
	list_columns.RestorePositions();
}

void CDKnownProcesses::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindow(m_lKnownProcesses.m_hWnd))
	{
		static RULE rules[] = {
			// Action    Act-on									Relative-to					Offset
			// ------    ------									-----------					------
			{lMOVE,		lRIGHT(IDOK),							lRIGHT(lPARENT)				, -7},
			{lMOVE,		lBOTTOM(IDOK),							lBOTTOM(lPARENT)			, -7},
			{lMOVE,		lLEFT(IDC_BUTTON_ADDNEW),				lLEFT(lPARENT)				,  7},
			{lMOVE,		lBOTTOM(IDC_BUTTON_ADDNEW),				lBOTTOM(lPARENT)			, -7},
			{lMOVE,		lLEFT(IDC_BUTTON_REMOVE),				lRIGHT(IDC_BUTTON_ADDNEW)	,  7},
			{lMOVE,		lBOTTOM(IDC_BUTTON_REMOVE),				lBOTTOM(lPARENT)			, -7},

			

			{lSTRETCH,	lRIGHT(IDC_LIST_KNOWNLIST),				lRIGHT(lPARENT)				, -7},
			{lSTRETCH,	lBOTTOM(IDC_LIST_KNOWNLIST),			lTOP(IDOK)					, -5},
			{lEND}
		};
		Layout_ComputeLayout(m_hWnd, rules);	
	}		
}

BOOL CDKnownProcesses::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	SavePosition();
	CListColumnPositions list_columns(_T("KnownProcess_List"), &m_lKnownProcesses);
	list_columns.SavePositions();
	
	return CDialog::DestroyWindow();
}

//+ ------------------------------------------------------------------------------------------

void CDKnownProcesses::ReloadList()
{
	m_lKnownProcesses.DeleteAllItems();

	_KnownProcList* pKnownList = m_pKnownList->QueryAndLockList();
	if (pKnownList == NULL)
		return;

	KnownProcess Known;
	_KnownProcList::iterator _it;

	int cou = 0;

	int len;
	int lenmax;
	CString strtmp;
	int idx;
	for (_it = pKnownList->begin(); _it != pKnownList->end(); ++_it)
	{
		Known = *_it;

		lenmax = lstrlen(Known.m_pwchImagePath);
		for (len = lenmax; len > 0; len--)
		{
			if (Known.m_pwchImagePath[len] == L'\\')
			{
				len++;
				break;
			}
		}

		strtmp = Known.m_pwchImagePath;
		if (len != 0)
		{
			PWCHAR buftmp = strtmp.GetBuffer(lenmax + 1);
			if (buftmp != NULL)
			{
				lstrcpy(buftmp, Known.m_pwchImagePath + len);
				strtmp.ReleaseBuffer();
			}
		}

		idx = m_lKnownProcesses.InsertItem(LVIF_TEXT, cou, strtmp, 0, 0, 0, 0);
		m_lKnownProcesses.SetItemData(idx, (DWORD)(new CString(strtmp)));

		m_lKnownProcesses.SetItemText(cou, _L_ImagePath, Known.m_pwchImagePath);

		if (!m_WinFileInfo.QueryInfo(Known.m_pwchImagePath))
		{
			m_lKnownProcesses.SetItemText(cou, _L_Version, m_WinFileInfo.GetProductVersion());
			m_lKnownProcesses.SetItemText(cou, _L_FileDescription, m_WinFileInfo.GetFileDescription());
		}
		
		cou++;
	}

	m_pKnownList->ReleaseLockedList();

	m_btRemove.EnableWindow(FALSE);

	_ColumnSort sort;
	sort.m_pCtrl = &m_lKnownProcesses;
	sort.m_bSortAscending = m_bSortAscending;
	sort.m_ColumnIdx = 0;

	m_lKnownProcesses.SortItems(KnownProcessCompareFunc, (LPARAM) &sort);
}

void CDKnownProcesses::OnItemchangedListKnownlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_btRemove.EnableWindow();
	
	*pResult = 0;
}

void CDKnownProcesses::OnButtonRemove() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_lKnownProcesses.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int nItem = m_lKnownProcesses.GetNextSelectedItem(pos);
		
		CString strtmp = m_lKnownProcesses.GetItemText(nItem , _L_ImagePath);
		PWCHAR pwchImagePath = strtmp.GetBuffer(0);
		
		m_pKnownList->RemoveByName(pwchImagePath);
		strtmp.ReleaseBuffer();
	}

	ReloadList();
}

void CDKnownProcesses::OnButtonAddnew() 
{
	// TODO: Add your control notification handler code here
	CString title = _T("Select executable file");
	CString strpath;
	if (BrowseForFolder(this->m_hWnd, title, strpath, TRUE, NULL))
	{
		PWCHAR pwch = strpath.GetBuffer(strpath.GetLength() + sizeof(TCHAR));
		if (pwch != NULL)
		{
			CProcessRecognizer recognizer(pwch);
			m_pKnownList->RegisterNewProcess(pwch, &recognizer);
		
			strpath.ReleaseBuffer();
		}
		else
		{
			//! error during add new process
		}
	}
	
	ReloadList();
}

void CDKnownProcesses::OnColumnclickListKnownlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	m_bSortAscending = !m_bSortAscending;

	_ColumnSort sort;
	sort.m_pCtrl = &m_lKnownProcesses;
	sort.m_bSortAscending = m_bSortAscending;
	sort.m_ColumnIdx = pNMListView->iSubItem;

	m_lKnownProcesses.SortItems(KnownProcessCompareFunc, (LPARAM) &sort);
	*pResult = 0;
}

void CDKnownProcesses::OnDeleteitemListKnownlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CString* ptmp = (CString*) m_lKnownProcesses.GetItemData(pNMListView->iItem);
	delete ptmp;
	
	*pResult = 0;
}

// DFilters.cpp : implementation file
//

#include "stdafx.h"
#include "IGuard.h"
#include "DFilters.h"
#include "DFilter.h"
#include "DWarningNoFilters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDFilters dialog


CDFilters::CDFilters(CWnd* pParent /*=NULL*/)
	: CDialog(CDFilters::IDD, pParent)
{
	m_hAccel = NULL;
	//{{AFX_DATA_INIT(CDFilters)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pFiltersStorage = new CFiltersStorage(g_hDrvDevice);
}


void CDFilters::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDFilters)
	DDX_Control(pDX, IDC_LIST_FILTERS, m_lFilters);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDFilters, CDialog)
	//{{AFX_MSG_MAP(CDFilters)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDA_DEL, OnDel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILTERS, OnDblclkListFilters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFilters message handlers

BOOL CDFilters::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	delete m_pFiltersStorage;
	
	return CDialog::DestroyWindow();
}

void CDFilters::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindow(m_lFilters.m_hWnd))
	{
		static RULE rules[] = {
			// Action    Act-on							Relative-to				Offset
			// ------    ------							-----------				------
			{lSTRETCH,	lRIGHT(IDC_LIST_FILTERS),		lRIGHT(lPARENT)			, -1},
			{lSTRETCH,	lBOTTOM(IDC_LIST_FILTERS),		lBOTTOM(lPARENT)			, -1},
			{lEND}
		};
		Layout_ComputeLayout(m_hWnd, rules);	
	}		
}

BOOL CDFilters::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR_FILTERS));
	
//	AddProProt(m_pFiltersStorage);

	Refresh();

	if (m_pFiltersStorage->m_FiltersArray.GetSize() == 0)
	{
		DWORD dwWarn = 0;
		g_pRegStorage->GetValue("NoFilterWarning", dwWarn);
		if (dwWarn == 0)
		{
			CDWarningNoFilters DWarningNoFilters;
			DWarningNoFilters.DoModal();
		}
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDFilters::Refresh(void)
{
	m_lFilters.DeleteAllItems();

	m_pFiltersStorage->ReloadFiltersArray();
	
	PFILTER_TRANSMIT pFilter;
	CString FilterDescription;
	int Count = m_pFiltersStorage->m_FiltersArray.GetSize();

	int ItemID;
	for (int cou = 0; cou < Count; cou++)
	{
		pFilter = m_pFiltersStorage->m_FiltersArray.ElementAt(cou);
		if (pFilter != NULL)
		{
			GetFilterDesciptions(pFilter, &FilterDescription);
			ItemID = m_lFilters.InsertItem(cou, FilterDescription);
			if (ItemID != -1)
				m_lFilters.SetItemData(ItemID, (DWORD) pFilter);
		}
	}
	
}

void CDFilters::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	HMENU hSysMenu;
	
	hSysMenu = LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_POPUP));
	if (hSysMenu != NULL)
	{
		POINT pt;
		HMENU hSubMenu;
		CWnd* pForeground = GetForegroundWindow();

		POSITION pos = m_lFilters.GetFirstSelectedItemPosition();
		
		GetCursorPos(&pt);
		SetForegroundWindow();
		hSubMenu = GetSubMenu(hSysMenu,0);
		if (hSubMenu != NULL)
		{
			int SelCount = m_lFilters.GetSelectedCount();
			if (SelCount == 0)
				EnableMenuItem(hSubMenu, IDMI_DEL_SELECTED_FILTER, MF_BYCOMMAND | MF_GRAYED);
			
			if (SelCount != 1)
			{
				EnableMenuItem(hSubMenu, IDMI_CHANGE_FILTER, MF_BYCOMMAND | MF_GRAYED);
				EnableMenuItem(hSubMenu, IDMI_ENABLE_FILTER, MF_BYCOMMAND | MF_GRAYED);
			}
			
			int nSelection = TrackPopupMenu(hSubMenu,TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x,pt.y, 0, this->m_hWnd,NULL);
			switch (nSelection)
			{
			case IDMI_REFRESH:
				break;
			case IDMI_CHANGE_FILTER:
				{
					LRESULT Result;
					OnDblclkListFilters(NULL, &Result);
				}
				break;
			case IDMI_ADD_NEW_FILTER:
				InsertNewFilter();
				break;
			case IDMI_DEL_SELECTED_FILTER:
				OnDel();
				break;
			case IDMI_ENABLE_FILTER:
				Enable_DisableFilter();
				break;
			}

			Refresh();
		}
		
		DestroyMenu(hSysMenu);
		
		if (pForeground != NULL)
			pForeground->SetForegroundWindow();
	}
}

BOOL CDFilters::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_hAccel != NULL)
	{
		if (TranslateAccelerator(m_hWnd, m_hAccel, pMsg))		
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDFilters::OnDel()
{
	// TODO: Add your command handler code here
	POSITION pos = m_lFilters.GetFirstSelectedItemPosition();		
	if (pos != NULL)
	{
		if (MessageBox("Are you sure?", "Delete filter(s)", MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			PFILTER_TRANSMIT pFilter;
			int Item;
			while (pos != NULL)
			{
				Item = m_lFilters.GetNextSelectedItem(pos);
				
				pFilter = (PFILTER_TRANSMIT) m_lFilters.GetItemData(Item);
				if (pFilter != NULL)
				{
					if (m_pFiltersStorage->DeleteFilter(pFilter) == false)
						MessageBox("Sorry - cant delete filter", "Warning", MB_ICONINFORMATION);
				}
			}
		}
		Refresh();
	}
}

/*void CDFilters::OnInsert()
{

}*/

void CDFilters::OnDblclkListFilters(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	PFILTER_TRANSMIT pFilter;
	int Item;
	
	POSITION pos = m_lFilters.GetFirstSelectedItemPosition();		
	
	while (pos != NULL)
	{
		Item = m_lFilters.GetNextSelectedItem(pos);
		pFilter = (PFILTER_TRANSMIT) m_lFilters.GetItemData(Item);
		
		CString Caption(L"Change filter's property");
		CDFilter DFilter(pFilter, &Caption, this);
		if (DFilter.DoModal() == IDOK)
		{
			if (m_pFiltersStorage->AddNewFilter(DFilter.m_FilterWork, &pFilter->m_FilterID) == false)
				MessageBox("Sorry - cant add new filter", "Warning", MB_ICONINFORMATION);
			if (m_pFiltersStorage->DeleteFilter(pFilter) == false)
				MessageBox("Sorry - cant delete filter", "Warning", MB_ICONINFORMATION);
		}
	}
	if (pNMHDR != NULL)
		Refresh();
	
	*pResult = 0;
}

void CDFilters::Enable_DisableFilter()
{
	PFILTER_TRANSMIT pFilter;
	int Item;
	
	POSITION pos = m_lFilters.GetFirstSelectedItemPosition();
	
	while (pos != NULL)
	{
		Item = m_lFilters.GetNextSelectedItem(pos);
		pFilter = (PFILTER_TRANSMIT) m_lFilters.GetItemData(Item);

		if (pFilter->m_Flags & FLT_A_DISABLED)
			pFilter->m_FltCtl = FLTCTL_ENABLE_FILTER;
		else
			pFilter->m_FltCtl = FLTCTL_DISABLE_FILTER;
		
		m_pFiltersStorage->FilterTransmit(m_pFiltersStorage->m_hDevice, pFilter, pFilter);
	}
	Refresh();
}

BOOL CDFilters::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if((LOWORD(wParam) == IDCANCEL) || (LOWORD(wParam) == IDOK))
		return TRUE;
	
	return CDialog::OnCommand(wParam, lParam);
}

void CDFilters::InsertNewFilter(void)
{
	// TODO: Add your command handler code here
	CString Caption(L"Add new filter");
	CDFilter DFilter(NULL, &Caption, this);
	
	PFILTER_TRANSMIT pFilter;
	int Item;
	
	POSITION pos = m_lFilters.GetFirstSelectedItemPosition();		
			
	if (pos != NULL)
	{
		Item = m_lFilters.GetNextSelectedItem(pos);
		pFilter = (PFILTER_TRANSMIT) m_lFilters.GetItemData(Item);
						
		DFilter.SetAddPos(pFilter->m_HookID, pFilter->m_FunctionMj, pFilter->m_FunctionMi);
	}
	else
		pFilter = NULL;
	
	
	if (DFilter.DoModal() == IDOK)
	{
		DWORD dwSite = 0;
		DWORD* pSite = NULL;
		if ((pFilter == NULL) || (DFilter.m_rPos == 0))
			pSite = &dwSite;
		else
		{
			if (DFilter.m_rPos == 2)
				pSite = &pFilter->m_FilterID;
		}
		
		if (m_pFiltersStorage->AddNewFilter(DFilter.m_FilterWork, pSite) == false)
			MessageBox("Sorry - cant add new filter", "Warning", MB_ICONINFORMATION);
		
		Refresh();
	}
}

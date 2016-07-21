#include "stdafx.h"
#include "gui.h"
#include "guiDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CguiDlg dialog

CguiDlg::CguiDlg(CWnd* pParent /*=NULL*/)
: CDialog(CguiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CguiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_RUL, ListRul.m_List_Rul);
	DDX_Control(pDX, IDC_LIST_APL, ListApp.m_List_Apl);
}

BEGIN_MESSAGE_MAP(CguiDlg, CDialog)

	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CguiDlg::OnBnClickedOk)
	//	ON_BN_CLICKED(IDC_LIST_RUL, &CguiDlg::OnBnClickedLIST_APL)
	//	ON_BN_CLICKED(IDC_BTN_ADD_APL, &CguiDlg::OnBnClickedBtnAddApl)

	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_RUL, OnColumnClick)
	ON_REGISTERED_MESSAGE(WM_XLISTCTRL_CHECKBOX_CLICKED, OnCheckbox)
	ON_REGISTERED_MESSAGE(WM_XLISTCTRL_COMBO_SELECTION, OnComboSelection)


	ON_BN_CLICKED(IDC_B_ADD_APL, &CguiDlg::OnBnClickedBAddApl)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RUL, &CguiDlg::OnLvnItemchangedListRul)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_APL, &CguiDlg::OnLvnItemchangedListApl)
	//ON_BN_CLICKED(IDC_B_ADD_DEL, &CguiDlg::OnBnClickedBAddDel)
	ON_BN_CLICKED(IDC_B_DEL, &CguiDlg::OnBnClickedBDel)
	ON_BN_CLICKED(IDC_B_APPLY, &CguiDlg::OnBnClickedBApply)
	ON_BN_CLICKED(IDC_B_ADD_RUL, &CguiDlg::OnBnClickedBAddRul)
	ON_BN_CLICKED(IDC_B_ADD_RUL_M, &CguiDlg::OnBnClickedBAddRulM)
END_MESSAGE_MAP()

// CguiDlg message handlers

BOOL CguiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	DWORD dwExStyle = LVS_EX_FULLROWSELECT
		/*| LVS_EX_TRACKSELECT*/;	// for hot tracking


	LS=LIST_NULL;
	//ListRul.m_List_Rul.pls=&LS;
	//ListApp.m_List_Apl.pls=&LS;

	InitListsCtrl(&LS);

	cont.Init();

	//ListRul.m_List_Rul.SetExtendedStyle(dwExStyle);
	//ListApp.m_List_Apl.SetExtendedStyle(dwExStyle);

	// call EnableToolTips to enable tooltip display
	//ListRul.m_List_Rul.EnableToolTips(TRUE);
	//ListApp.m_List_Apl.EnableToolTips(TRUE);

	//InitListRul (&ListRul.m_List_Rul);
	//InitListApl (&ListApp.m_List_Apl);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CguiDlg::InitListsCtrl( list_selector *pls )
{
	ListApp.InitList(pls);
	ListRul.InitList(pls);
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CguiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CguiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CguiDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
/*
void CguiDlg::OnBnClickedBtnAddApl()
{
// TODO: Add your control notification handler code here
//CCheckListBox *pcl;
FillListCtrl(&(ListRul.m_List_Rul));
//pcl=(CCheckListBox*)GetDlgItem(IDC_LIST_APL);

}*/



void CguiDlg::FillListCtrl(CXListCtrl * pList)
{

	/*static TCHAR * szCityNames[] = 
	{
	_T("Tokyo"),
	_T("Mexico City"),
	_T("Seoul"),
	_T("New York"),
	_T("Sao Paulo"),
	_T("Bombay"),
	_T("Delhi"),
	_T("Shanghai"),
	_T("Los Angeles"),
	_T("Osaka"),
	_T("Jakarta"),
	_T("Calcutta"),
	_T("Cairo"),
	_T("Manila"),
	_T("Karachi"),
	_T("Moscow"),
	_T("Buenos Aires"),
	_T("Dacca"),
	_T("Rio de Janeiro"),
	_T("Beijing"),
	_T("London"),
	_T("Tehran"),
	_T("Istanbul"),
	_T("Lagos"),
	_T("Shenzhen"),
	_T("Paris"),
	_T("Chicago"),
	_T("Canton"),
	_T("Chungking"),
	_T("Wuhan"),
	_T("Lima"),
	_T("Bangkok"),
	_T("Bogota"),
	_T("Washington"),
	_T("Nagoya"),
	_T("Madras"),
	_T("Lahore"),
	_T("Hong Kong"),
	_T("Johannesburg"),
	_T("San Francisco"),
	_T("Bangalore"),
	_T("Kinshasa"),
	_T("Taipei"),
	_T("Hyderabad"),
	_T("Tientsin"),
	_T("Dallas"),
	_T("Philadelphia"),
	_T("Santiago"),
	_T("Detroit"),
	_T("Ruhr"),
	_T("Boston"),
	_T("Khartoum"),
	_T("Ahmadabad"),
	_T("Belo Horizonte"),
	_T("Madrid"),
	_T("Baghdad"),
	_T("Miami"),
	_T("Houston"),
	_T("Ho Chi Minh City"),
	_T("Toronto"),
	_T("St. Petersburg"),
	_T("Atlanta"),
	_T("Alexandria"),
	_T("Caracas"),
	_T("Singapore"),
	_T("Riyadh"),
	_T("Shenyang"),
	_T("Rangoon"),
	_T("Poona"),
	_T("Guadalajara"),
	_T("Sydney"),
	_T("Chittagong"),
	_T("Kuala Lumpur"),
	_T("Berlin"),
	_T("Algiers"),
	_T("Porto Alegre"),
	_T("Abidjan"),
	_T("Monterrey"),
	_T("Phoenix"),
	_T("Casablanca"),
	_T("Milan"),
	_T("Barcelona"),
	_T("Ankara"),
	_T("Recife"),
	_T("Seattle"),
	_T("Busan"),
	_T("Melbourne"),
	_T("Surat"),
	_T("Montreal"),
	_T("Bras=?ia"),
	_T("Pyongyang"),
	_T("Sian"),
	_T("Athens"),
	_T("Durban"),
	_T("Fortaleza"),
	_T("Nanking"),
	_T("Salvador"),
	_T("Medellin"),
	_T("Harbin"),
	_T("Minneapolis"),
	NULL
	};

	static TCHAR * szCityPopulations[] = 
	{
	_T("34,200,000"),
	_T("22,800,000"),
	_T("22,300,000"),
	_T("21,900,000"),
	_T("20,200,000"),
	_T("19,850,000"),
	_T("19,700,000"),
	_T("18,150,000"),
	_T("18,000,000"),
	_T("16,800,000"),
	_T("16,550,000"),
	_T("15,650,000"),
	_T("15,600,000"),
	_T("14,950,000"),
	_T("14,300,000"),
	_T("13,750,000"),
	_T("13,450,000"),
	_T("13,250,000"),
	_T("12,150,000"),
	_T("12,100,000"),
	_T("12,000,000"),
	_T("11,850,000"),
	_T("11,500,000"),
	_T("11,100,000"),
	_T("10,700,000"),
	_T("9,950,000"),
	_T("9,750,000"),
	_T("9,550,000"),
	_T("9,350,000"),
	_T("9,100,000"),
	_T("8,550,000"),
	_T("8,450,000"),
	_T("8,350,000"),
	_T("8,150,000"),
	_T("8,050,000"),
	_T("7,600,000"),
	_T("7,550,000"),
	_T("7,400,000"),
	_T("7,400,000"),
	_T("7,250,000"),
	_T("7,100,000"),
	_T("7,000,000"),
	_T("6,950,000"),
	_T("6,700,000"),
	_T("6,350,000"),
	_T("6,000,000"),
	_T("6,000,000"),
	_T("5,900,000"),
	_T("5,800,000"),
	_T("5,800,000"),
	_T("5,700,000"),
	_T("5,650,000"),
	_T("5,600,000"),
	_T("5,600,000"),
	_T("5,600,000"),
	_T("5,550,000"),
	_T("5,550,000"),
	_T("5,400,000"),
	_T("5,400,000"),
	_T("5,400,000"),
	_T("5,250,000"),
	_T("5,100,000"),
	_T("5,000,000"),
	_T("4,700,000"),
	_T("4,650,000"),
	_T("4,550,000"),
	_T("4,550,000"),
	_T("4,500,000"),
	_T("4,450,000"),
	_T("4,350,000"),
	_T("4,350,000"),
	_T("4,300,000"),
	_T("4,300,000"),
	_T("4,200,000"),
	_T("4,150,000"),
	_T("4,150,000"),
	_T("4,100,000"),
	_T("3,950,000"),
	_T("3,900,000"),
	_T("3,850,000"),
	_T("3,850,000"),
	_T("3,800,000"),
	_T("3,750,000"),
	_T("3,750,000"),
	_T("3,750,000"),
	_T("3,700,000"),
	_T("3,700,000"),
	_T("3,700,000"),
	_T("3,650,000"),
	_T("3,600,000"),
	_T("3,600,000"),
	_T("3,600,000"),
	_T("3,500,000"),
	_T("3,500,000"),
	_T("3,500,000"),
	_T("3,500,000"),
	_T("3,500,000"),
	_T("3,450,000"),
	_T("3,400,000"),
	_T("3,400,000"),
	NULL
	};*/

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	//
	// Note - the string arrays must persist for life of list control
	//
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////

	// a list of some of my favorite things - note that this list
	// is not sorted
	m_sa1.RemoveAll();
	m_sa1.Add(_T("banana"));
	m_sa1.Add(_T("strawberry"));
	m_sa1.Add(_T("tangerine"));
	m_sa1.Add(_T("cherry"));
	m_sa1.Add(_T("plum"));
	m_sa1.Add(_T("cranberry"));
	m_sa1.Add(_T("blueberry"));
	m_sa1.Add(_T("lime"));
	m_sa1.Add(_T("pear"));
	m_sa1.Add(_T("lemon"));
	m_sa1.Add(_T("orange"));
	m_sa1.Add(_T("peach"));
	m_sa1.Add(_T("apple"));
	m_sa1.Add(_T("raspberry"));
	m_sa1.Add(_T("grape"));

	// a list of some more of my favorite things
	/*m_sa2.RemoveAll();
	m_sa2.Add(_T("Aidi"));
	m_sa2.Add(_T("Akbas"));
	m_sa2.Add(_T("Ariegeois"));
	m_sa2.Add(_T("Boerboel"));
	m_sa2.Add(_T("Boolomo"));
	m_sa2.Add(_T("Borzoi"));
	m_sa2.Add(_T("Broholmer"));
	m_sa2.Add(_T("Caniche"));
	m_sa2.Add(_T("Charplaninatz"));
	m_sa2.Add(_T("Drok"));
	m_sa2.Add(_T("Elo"));
	m_sa2.Add(_T("Farou"));
	m_sa2.Add(_T("Godo"));
	m_sa2.Add(_T("Groenendael"));
	m_sa2.Add(_T("Hahoavu"));
	m_sa2.Add(_T("Heidewachtel"));
	m_sa2.Add(_T("Hovawart"));
	m_sa2.Add(_T("Juzak"));
	m_sa2.Add(_T("Kai"));
	m_sa2.Add(_T("Kangal"));
	m_sa2.Add(_T("Karabash"));
	m_sa2.Add(_T("Kishu"));
	m_sa2.Add(_T("Kuvasz"));
	m_sa2.Add(_T("Landseer"));
	m_sa2.Add(_T("Levesque"));
	m_sa2.Add(_T("Licaon"));
	m_sa2.Add(_T("Maliki"));
	m_sa2.Add(_T("Mechelaar"));
	m_sa2.Add(_T("Mirigung"));
	m_sa2.Add(_T("Mudi"));
	m_sa2.Add(_T("Noggum"));
	m_sa2.Add(_T("Ocherese"));
	m_sa2.Add(_T("Phalene"));
	m_sa2.Add(_T("Pocadan"));
	m_sa2.Add(_T("Podhalan"));
	m_sa2.Add(_T("Poitevin"));
	m_sa2.Add(_T("Saluki"));
	m_sa2.Add(_T("Samojedskaja"));
	m_sa2.Add(_T("Samoyed"));
	m_sa2.Add(_T("Spion"));
	m_sa2.Add(_T("Stichelhaar"));
	m_sa2.Add(_T("Taygan"));
	m_sa2.Add(_T("Telomian"));
	m_sa2.Add(_T("Tornjak"));
	m_sa2.Add(_T("Virelade"));
	m_sa2.Add(_T("Warrigal"));*/


	pList->LockWindowUpdate();	// ***** lock window updates while filling list *****

	pList->DeleteAllItems();

	CString str = _T("");
	int nItem, nSubItem;

	// insert the items and subitems into the list
	for (nItem = 0; nItem < 20; nItem++)
	{
		for (nSubItem = 0; nSubItem < 4; nSubItem++)
		{
			str = _T("");

			if (nSubItem == 0)				// checkbox
				str = _T(" ");
			else if (nSubItem == 1)			// combo
				str = _T("");
			else if (nSubItem == 2)			// progress
				str = _T("Complete");
			else if (nSubItem == 3)			// checkbox
				str = _T("OK");
			//else if (nSubItem == 4)			// city name
			//	str = szCityNames[nItem];
			//else if (nSubItem == 5)			// city population
			//	str = szCityPopulations[nItem];

			// show background coloring
			if (nSubItem == 0)
				pList->InsertItem(nItem, str);
			else if (nItem == 0 && nSubItem == 4)
				pList->SetItemText(nItem, nSubItem, str, RGB(0,0,0), RGB(210,221,244));
			else if (nItem == 3 && nSubItem == 1)
				pList->SetItemText(nItem, nSubItem, str, RGB(0,0,0), RGB(255,230,162));
			else if (nItem == 8 && nSubItem == 3)
				pList->SetItemText(nItem, nSubItem, str, RGB(0,0,0), RGB(194,209,166));
			else if (nItem == 1 && nSubItem == 2)
				pList->SetItemText(nItem, nSubItem, str, RGB(0,0,0), RGB(247,202,202));
			else
				pList->SetItemText(nItem, nSubItem, str);

			if (nSubItem == 1 /*&& ((nItem & 1) == 1)*/)
				pList->SetComboBox(nItem, nSubItem, 
				TRUE,		// enable combobox
				&m_sa1,		// pointer to persistent CStringArray
				5,			// size of dropdown
				0,			// initial combo selection
				TRUE);		// sort CStringArray
			//if (nSubItem == 1 && ((nItem & 1) == 0))
			//	pList->SetComboBox(nItem, nSubItem, 
			//	TRUE,		// enable combobox
			//	&m_sa2,		// pointer to persistent CStringArray
			//	10,			// size of dropdown
			//	3,			// initial combo selection
			//	TRUE);		// sort CStringArray
			/*if (nSubItem == 0 || nSubItem == 3)
			pList->SetCheckbox(nItem, nSubItem, 1);
			if (nSubItem == 4)
			pList->SetEdit(nItem, nSubItem);*/
		}
	}

#ifndef NO_XLISTCTRL_TOOL_TIPS

	// set tool tips in second row
	/*nItem = 1;
	for (nSubItem = 0; nSubItem < 4; nSubItem++)
	{
	str.Format(_T("This is %d,%d"), nItem, nSubItem);
	pList->SetItemToolTipText(nItem, nSubItem, str);
	}*/

#endif

	pList->UnlockWindowUpdate();	// ***** unlock window updates *****
}


LRESULT CguiDlg::OnCheckbox(WPARAM nItem, LPARAM nSubItem)
{

	//GetDlgItem(IDC_LIST_APL)->m_hWnd;
	/*if (IsWindow(GetDlgItem(IDC_LIST_APL)->m_hWnd))
	{
	int a;
	a=0;
	}*/
	int a=GetDlgCtrlID();
	if (a==IDC_LIST_RUL)
	{
		int a;
		a=0;
	}

	if (LS==LIST_APL)
	{
		APPL_ITEM appl_item;
		memset( &appl_item, 0, sizeof(APPL_ITEM) );
		CString path=ListApp.m_List_Apl.GetItemText((int)nItem,0);

		wsprintfW( appl_item.path,L"%s",path);
		if (nSubItem==2)
		{
			appl_item.path_ch=1;
		}
		if (nSubItem==1)
		{
			appl_item.hash_ch=1;
		}
		cont.ChangeApplItem(&appl_item, (int)nItem);

	}

	return 0;
}

void CguiDlg::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	//GetDlgItem(IDC_LIST_APL)->m_hWnd;
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->hdr.idFrom==IDC_LIST_RUL)
	{
		int b;
		b=0;
	}

	//return 0;
}


void CguiDlg::OnBnClickedBAddApl()
{
	// TODO: Add your control notification handler code here
	//FillListCtrl(&ListRul.m_List_Rul);
	//return;

	CFileDialog cf(true, L"*.*", NULL, OFN_EXPLORER | OFN_OVERWRITEPROMPT, L"сеРDайлыР(*.*)|*.*|", this);
	if(cf.DoModal()!=IDOK)
		return ;

	//DWORD error;
	CString drvname=cf.GetFileTitle();
	CString srcpath=cf.GetPathName();

	APPL_ITEM appl_item;

	memset (&appl_item, 0, sizeof(APPL_ITEM));

	wsprintfW(appl_item.path,L"%s",cf.GetPathName());


	cont.AddAppl(&appl_item);

	if (!ListApp.AddApl(&appl_item))
	{
		MessageBoxW(L"шибкаР4обавленияР2РAписокР?риложенийЮ");
	}




}

void CguiDlg::OnLvnItemchangedListRul(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	ListRul.selected=pNMLV->iItem;
	*pResult = 0;
}

void CguiDlg::RefreshListRul ( int AppItem )
{
	ListRul.ClearRul();

	RUL_ARRAY *prul_array;
	prul_array=cont.GetRulArray(AppItem);
	if (!prul_array)
	{
		return;
	}

	RUL_ARRAY::iterator iter;//=prul_array->begin();
	for (iter=prul_array->begin();iter!=prul_array->end(); iter++)
	{
		ListRul.AddRul(&(*iter));
	}
}

void CguiDlg::OnLvnItemchangedListApl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	ListApp.selected=pNMLV->iItem;

	ListRul.ClearRul();

	RUL_ARRAY *prul_array;
	prul_array=cont.GetRulArray(pNMLV->iItem);
	if (!prul_array)
	{

		//MessageBoxW(L"шибкаР?олученияРAпискаР?равилТ);
		*pResult = 0;
		return;
	}

	RUL_ARRAY::iterator iter;//=prul_array->begin();
	for (iter=prul_array->begin();iter!=prul_array->end(); iter++)
	{
		ListRul.AddRul(&(*iter));
	}

	//if (!ListRul.AddRul(&rul_item))
	//{
	//	//MessageBoxW(L"шибкаР4обавленияР2РAписокР?равилЮ");
	//}

	*pResult = 0;
}

int CguiDlg::TextToNum(WCHAR* str)
{
	if ( memcmp(str, L"allow", sizeof(L"allow")-sizeof(WCHAR))==0 ) 
		return 0;

	if ( memcmp(str, L"ask", sizeof(L"ask")-sizeof(WCHAR))==0 ) 
		return 1;

	if ( memcmp(str, L"deny", sizeof(L"deny")-sizeof(WCHAR))==0 ) 	
		return 2;

	if ( memcmp(str, L"0", sizeof(L"0")-sizeof(WCHAR))==0 ) 	
		return 0;

	if ( memcmp(str, L"1", sizeof(L"1")-sizeof(WCHAR))==0 ) 	
		return 1;

	if ( memcmp(str, L"2", sizeof(L"2")-sizeof(WCHAR))==0 ) 	
		return 2;

	return -1;
}



LRESULT CguiDlg::OnComboSelection(WPARAM nItem, LPARAM nSubItem)
{

	if (LS==LIST_RUL)
	{
		CString strText = ListRul.m_List_Rul.GetItemText((int)nItem, 0);
		RUL_ITEM rul_item;
		memset( &rul_item, 0, sizeof(RUL_ITEM) );
		wsprintf(rul_item.symbolLink,L"%s",strText);

		strText=ListRul.m_List_Rul.GetItemText((int)nItem, 1);
		rul_item.w_3st=TextToNum(strText.GetBuffer());

		strText=ListRul.m_List_Rul.GetItemText((int)nItem, 2);
		rul_item.r_3st=TextToNum(strText.GetBuffer());

		strText=ListRul.m_List_Rul.GetItemText((int)nItem, 3);
		rul_item.e_3st=TextToNum(strText.GetBuffer());

		strText=ListRul.m_List_Rul.GetItemText((int)nItem, 4);
		rul_item.logFlag=(BOOLEAN)TextToNum(strText.GetBuffer());

		strText=ListRul.m_List_Rul.GetItemText((int)nItem, 5);
		rul_item.blockID=TextToNum(strText.GetBuffer());

		cont.ChangeRulItem(&rul_item, ListApp.selected, (int)nItem);
	}

	if (LS==LIST_APL)
	{

	}


	return 0;
}
void CguiDlg::OnBnClickedBDel()
{
	// TODO: Add your control notification handler code here


	cont.DelAppl(ListApp.selected);

	//ListRul.m_List_Rul.DeleteAllItems();
	//ListRul.ClearRul();
	ListApp.m_List_Apl.DeleteItem(ListApp.selected);





}

void CguiDlg::OnBnClickedBApply()
{
	// TODO: Add your control notification handler code here
	cont.InsertApplRulesToDriver();
}

void CguiDlg::OnBnClickedBAddRul()
{
	// TODO: Add your control notification handler code here


	CFileDialog cf(true, L"*.*", NULL, OFN_EXPLORER | OFN_OVERWRITEPROMPT, L"сеРDайлыР(*.*)|*.*|", this);
	if(cf.DoModal()!=IDOK)
		return ;

	//DWORD error;
	CString drvname=cf.GetFileTitle();
	CString srcpath=cf.GetPathName();

	RUL_ITEM rul_item;
	memset( &rul_item, 0, sizeof(RUL_ITEM) );

	wsprintfW( rul_item.symbolLink,L"File (%s) ", cf.GetPathName());
	wsprintfW( rul_item.deviceName,L"%s",cf.GetPathName() );

	cont.AddRulItem( &rul_item, ListApp.selected);

	RefreshListRul(ListApp.selected);
	//ListView_SetCheckState(ListRul.m_List_Rul.m_hWnd,ListApp.selected,TRUE);
}

#include "AddMaskDlg.h"
void CguiDlg::OnBnClickedBAddRulM()
{
	// TODO: Add your control notification handler code here
	CAddMaskDlg addDlg;
	if ( addDlg.DoModal()!=IDOK )
		return;

	if (!addDlg.pMask)
		return;

	if ( lstrlen(addDlg.pMask)==0 )
		return;

	RUL_ITEM rul_item;
	memset( &rul_item, 0, sizeof(RUL_ITEM) );

	wsprintfW( rul_item.symbolLink,L"Mask (%s|%s ) ", addDlg.pMask, (addDlg.pValueName) ? addDlg.pValueName : (L"#NO_VALUE#")  );
	wsprintfW( rul_item.deviceName,L"%s", addDlg.pMask );
	if ( addDlg.pValueName )
		wsprintfW( rul_item.ValueName,L"%s", addDlg.pValueName );

	cont.AddRulItem( &rul_item, ListApp.selected);

	RefreshListRul(ListApp.selected);


}
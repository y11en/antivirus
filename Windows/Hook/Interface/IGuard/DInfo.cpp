// DInfo.cpp : implementation file
//

#include "stdafx.h"
#include "iguard.h"
#include "DInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD g_FuncLevel = 0;
/////////////////////////////////////////////////////////////////////////////
// CDInfo dialog


CDInfo::CDInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDInfo)
	DDX_Control(pDX, IDC_LIST_KEYSINFO, m_lKeysInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDInfo, CDialog)
	//{{AFX_MSG_MAP(CDInfo)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_KEYSINFO, OnItemchangedListKeysinfo)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDInfo message handlers
CHAR CurrentFolder[MAX_PATH * 2];
BOOL CDInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//
	GetCurrentDirectory(sizeof(CurrentFolder), CurrentFolder);
	BOOL bRet = KLKR_FindFirstKeyFile(CurrentFolder);

	struct _tHeaderItem
	{
		CString	m_ItemText;
		int		m_Width;
		_tHeaderItem()
		{
			m_ItemText.Empty();
			m_Width = 1;
		};
		_tHeaderItem (char* ItemText, int Width)
		{
			m_ItemText = ItemText;
			m_Width = Width;
		};
	}_HeaderItem;

	_tHeaderItem nObjectsHeaderIDTable[] = {
		_tHeaderItem("Key number", 30),
		_tHeaderItem("Is trial", 10),
		_tHeaderItem("Has support", 15),
		_tHeaderItem("Expiration date", 25),
		_tHeaderItem("License info", 30),
	};

	m_lKeysInfo.SetShowSelectionAlways(true);
	
	m_lKeysInfo.SetProportionalResizeHeader();
	ListView_SetExtendedListViewStyle(m_lKeysInfo.m_hWnd, LVS_EX_FULLROWSELECT /*| LVS_EX_CHECKBOXES*/);
	
	int nObjectsHeaderIDTableCount = sizeof(nObjectsHeaderIDTable) / sizeof(nObjectsHeaderIDTable[0]);

	CCLColumnsArray *pColumnsArray = new CCLColumnsArray(true);
	for (int i=0; i<nObjectsHeaderIDTableCount; i++ )
	{
		CCheckListColumn *pColumn = new CCheckListColumn();
		CString rString;
		pColumn->SetName(nObjectsHeaderIDTable[i].m_ItemText);
		pColumn->SetAlignment(CCheckListColumn::ca_Left);
		pColumn->SetWidthPercent(nObjectsHeaderIDTable[i].m_Width);
		pColumn->SetExternalPercent();
		pColumnsArray->Add(pColumn);
	}
	
	m_lKeysInfo.SetColumnsArray( pColumnsArray );	

	CCLItemsArray *pItems = new CCLItemsArray( true );
	CStringList lSubTexts;
	CString strtmp;

	BOOL		bTrial;
	BOOL		bHasSupport;
	CHAR		pSerialNumb[MAX_PATH];
	CHAR		pExpDate[MAX_PATH];
	DWORD		bExp;
	DWORD		Level;

	CHAR		pLicenseInfo[MAX_PATH];
	PCHAR		pSupportInfo;
	
	while (bRet == TRUE)
	{
		Level = KLKR_GetKeyHasProductI(AVP_APID_GUARD);
		if (Level > 0)
		{
			lSubTexts.RemoveAll();
			CCheckListItem *item = new CCheckListItem;
			
			KLKR_GetKeySerialNumberSL(pSerialNumb, sizeof(pSerialNumb));
			strtmp = pSerialNumb;

			item->SetText(strtmp);

			bTrial = KLKR_GetKeyIsTrialI();
			if (bTrial == TRUE)
				lSubTexts.AddTail("yes");
			else
				lSubTexts.AddTail("");

			bHasSupport = KLKR_GetKeyHasSupportI();
			if (bHasSupport == TRUE)
				lSubTexts.AddTail("yes");
			else
				lSubTexts.AddTail("");

			KLKR_GetKeyExpDateExExSL(pExpDate, sizeof(pExpDate), &bExp, FALSE);
			strtmp = pExpDate;
			if (bExp) strtmp += " - expired";
			else
			{
				if (g_FuncLevel < Level)
					g_FuncLevel = Level;
			}
			lSubTexts.AddTail(strtmp);

			KLKR_GetKeyLicenceInfoSL(pLicenseInfo, sizeof(pLicenseInfo));
			strtmp = pLicenseInfo;
			lSubTexts.AddTail(strtmp);

			pSupportInfo = (PCHAR) _MM_Alloc(MAX_PATH * 2);
			KLKR_GetKeySupportInfoSL(pSupportInfo, MAX_PATH * 2);

			item->SetSubItemsTexts(lSubTexts);
			item->SetData(pSupportInfo, false);
			
			pItems->Add( item );
		}
		
		bRet = KLKR_FindNextKeyFile();
	}	

	m_lKeysInfo.SetItemsArray(pItems);
	m_lKeysInfo.SetItemSelected(0);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDInfo::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindow(m_lKeysInfo.m_hWnd))
	{
		static RULE rules[] = {
			// Action    Act-on							Relative-to				Offset
			// ------    ------							-----------				------
			{lMOVE,		lLEFT(IDC_LIST_KEYSINFO),			lRIGHT(IDC_STATIC_BMP)		, 3},
			{lSTRETCH,	lRIGHT(IDC_LIST_KEYSINFO),			lRIGHT(lPARENT)				, -1},
			{lSTRETCH,	lBOTTOM(IDC_LIST_KEYSINFO),		lBOTTOM(IDC_STATIC_BMP)		, 0},
			{lSTRETCH,	lRIGHT(IDC_EDIT_SUPPORTINFO),		lRIGHT(lPARENT)				, -1},
			{lMOVE,		lLEFT(IDC_EDIT_SUPPORTINFO),		lRIGHT(IDC_STATIC_BMP)		, 3},
			{lMOVE,		lTOP(IDC_EDIT_SUPPORTINFO),		lBOTTOM(IDC_LIST_KEYSINFO)	, 3},
			{lSTRETCH,	lBOTTOM(IDC_EDIT_SUPPORTINFO),	lBOTTOM(lPARENT)				, -3},
			{lEND}
		};
		Layout_ComputeLayout(m_hWnd, rules);	
	}
}

void CDInfo::OnItemchangedListKeysinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_FOCUSED) && (!pNMListView->uOldState))
	{
		CCLItemsArray* pItems = m_lKeysInfo.GetItemsArray();
		int nItem = m_lKeysInfo.GetSelectedIndex();
		if (nItem != -1)
		{
			CCheckListItem *pItem = (*pItems)[nItem];
			PCHAR pInfo = (PCHAR) pItem->GetData();
			if (pInfo != NULL)
				SetDlgItemText(IDC_EDIT_SUPPORTINFO, pInfo);
		}
	}	
	*pResult = 0;
}

void CDInfo::OnDestroy() 
{
	CDialog::OnDestroy();
	
	CCLItemsArray* pItems = m_lKeysInfo.GetItemsArray();
	int Cou = pItems->GetSize();
	for (int cou = 0; cou < Cou; cou++)
	{
		CCheckListItem *pItem = (*pItems)[cou];
		PCHAR pInfo = (PCHAR) pItem->GetData();
		if (pInfo != NULL)
		{
			_MM_Free(pInfo);
			pItem->SetData(NULL, false);
		}
	}
}

BOOL CDInfo::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if((LOWORD(wParam) == IDCANCEL) || (LOWORD(wParam) == IDOK))
		return TRUE;
	
	return CDialog::OnCommand(wParam, lParam);
}

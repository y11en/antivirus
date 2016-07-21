#include "StdAfx.h"
#include "ListRul.h"

CListRul::CListRul(void)
{
	selected=0;
}

CListRul::~CListRul(void)
{
}


void CListRul::InitList( list_selector *pls )
{
	//ASSERT(pList);
	//if (!pList)
	//	return;

	// set column width according to window rect

	m_List_Rul.pls=pls;

	DWORD dwExStyle = LVS_EX_FULLROWSELECT;

	m_List_Rul.SetExtendedStyle(dwExStyle);
	m_List_Rul.EnableToolTips(TRUE);

	CRect rect;
	m_List_Rul.GetWindowRect(&rect);

	int w = rect.Width() - 2;

	TCHAR *	lpszHeaders[] = { _T("Symbol link"),
		_T("Write"),
		_T("Read"),
		_T("Enumerate"),
		_T("logFlag"),
		_T("blockId"),

		NULL };
	int i;
	int total_cx = 0;
	LV_COLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));

	w = w - ::GetSystemMetrics(SM_CXVSCROLL);	// width of vertical scroll bar

	// add columns
	for (i = 0; ; i++)
	{
		if (lpszHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = LVCFMT_LEFT /* LVCFMT_CENTER*/;
		//lvcolumn.fmt = (i == 5) ? LVCFMT_RIGHT : lvcolumn.fmt;
		lvcolumn.pszText = lpszHeaders[i];
		lvcolumn.iSubItem = i;

		if (i==0)
			lvcolumn.cx = 300;
		else if (i==1 || i==2)
			lvcolumn.cx = 70;
		else if (i==3)
			lvcolumn.cx = 70;
		//lvcolumn.cx = (lpszHeaders[i+1] == NULL) ? w - total_cx - 2 : (w * m_nColWidths[i]) / 64;
		//total_cx += lvcolumn.cx;
		m_List_Rul.InsertColumn(i, &lvcolumn);
	}

	// iterate through header items and attach the image list
	HDITEM hditem;

	for (i = 0; i < m_List_Rul.m_HeaderCtrl.GetItemCount(); i++)
	{
		hditem.mask = HDI_IMAGE | HDI_FORMAT;
		m_List_Rul.m_HeaderCtrl.GetItem(i, &hditem);
		hditem.fmt |=  HDF_IMAGE;
		/*if (i == 1 )
			hditem.iImage = XHEADERCTRL_UNCHECKED_IMAGE;
		else*/
		hditem.iImage = XHEADERCTRL_NO_IMAGE;

		m_List_Rul.m_HeaderCtrl.SetItem(i, &hditem);
	}

	memset(&lvcolumn, 0, sizeof(lvcolumn));

	// set the format again - must do this twice or first column does not get set
	for (i = 0; ; i++)
	{
		if (lpszHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM;
		lvcolumn.fmt = (i == 1) ? LVCFMT_LEFT : LVCFMT_CENTER;
		//lvcolumn.fmt = (i == 5) ? LVCFMT_RIGHT : lvcolumn.fmt;
		lvcolumn.iSubItem = i;
		m_List_Rul.SetColumn(i, &lvcolumn);
	}
}

void CListRul::ClearRul()
{
	m_List_Rul.LockWindowUpdate();	// ***** lock window updates while filling list *****
	
	m_List_Rul.DeleteAllItems();
	
	m_List_Rul.UnlockWindowUpdate();	// ***** unlock window updates *****
}



BOOL CListRul::AddRul( PRUL_ITEM pRulItem)
{
	
	cmbBoxArr.RemoveAll();
	cmbBoxArr.Add(_T("allow"));
	cmbBoxArr.Add(_T("ask"));
	cmbBoxArr.Add(_T("deny"));

	cmbBoxLogFlag.RemoveAll();
	cmbBoxLogFlag.Add(_T("0"));
	cmbBoxLogFlag.Add(_T("1"));
	
	cmbBoxBlockID.RemoveAll();
	cmbBoxBlockID.Add(_T("0"));
	cmbBoxBlockID.Add(_T("1"));
	cmbBoxBlockID.Add(_T("2"));

	m_List_Rul.LockWindowUpdate();	// ***** lock window updates while filling list *****

	//m_List_Rul.DeleteAllItems();
	

	

	CString str = _T("");
	int nItem, nSubItem;

	
	
	nItem=m_List_Rul.GetItemCount();
	for (nSubItem = 0; nSubItem < 6; nSubItem++)
	{
		str = _T("");

		if (nSubItem == 0)
			str = pRulItem->symbolLink;
		else if (nSubItem == 1)
			str = _T("");
		else if (nSubItem == 2)
			str = _T("");
		else if (nSubItem == 3)
			str = _T("");
		else if (nSubItem == 4)
			str = _T("");
		else if (nSubItem == 5)
			str = _T("");

		if (nSubItem == 0)
			if (m_List_Rul.InsertItem(nItem, str)==-1) 
				return false;
			else
			{
				if (nItem>=m_List_Rul.GetItemCount())
					return false;
				m_List_Rul.SetItemText(nItem, nSubItem, pRulItem->symbolLink);
			}

		if (nSubItem == 1)
			m_List_Rul.SetComboBox(nItem, nSubItem, 
			TRUE,		// enable combobox
			&cmbBoxArr,		// pointer to persistent CStringArray
			10,			// size of dropdown
			pRulItem->w_3st,			// initial combo selection
			FALSE);		// sort CStringArray
		else if (nSubItem == 2)
			m_List_Rul.SetComboBox(nItem, nSubItem, 
			TRUE,		// enable combobox
			&cmbBoxArr,		// pointer to persistent CStringArray
			10,			// size of dropdown
			pRulItem->r_3st,			// initial combo selection
			FALSE);		// sort CStringArray

		else if (nSubItem == 3)
			m_List_Rul.SetComboBox(nItem, nSubItem, 
			TRUE,		// enable combobox
			&cmbBoxArr,		// pointer to persistent CStringArray
			10,			// size of dropdown
			pRulItem->e_3st,			// initial combo selection
			FALSE);		// sort CStringArray
		else if (nSubItem == 4)
			m_List_Rul.SetComboBox(nItem, nSubItem, 
			TRUE,		// enable combobox
			&cmbBoxLogFlag,		// pointer to persistent CStringArray
			10,			// size of dropdown
			(pRulItem->logFlag==TRUE)?1:0,			// initial combo selection
			FALSE);
		else if (nSubItem == 5)
			m_List_Rul.SetComboBox(nItem, nSubItem, 
			TRUE,		// enable combobox
			&cmbBoxBlockID,		// pointer to persistent CStringArray
			10,			// size of dropdown
			pRulItem->blockID,			// initial combo selection
			FALSE);

	}
	
	m_List_Rul.UnlockWindowUpdate();	// ***** unlock window updates *****

	return TRUE;
}
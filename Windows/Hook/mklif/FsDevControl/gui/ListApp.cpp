#include "StdAfx.h"
#include "ListApp.h"

CListApp::CListApp(void)
{
	//itemCount=0;
	selected=0;
}

CListApp::~CListApp(void)
{
}

void CListApp::InitList( list_selector *pls )
{
	//ASSERT(pList);
	//if (!pList)
	//	return;

	// set column width according to window rect
	m_List_Apl.pls=pls;

	DWORD dwExStyle = LVS_EX_FULLROWSELECT;

	m_List_Apl.SetExtendedStyle(dwExStyle);
	m_List_Apl.EnableToolTips(TRUE);

	CRect rect;
	m_List_Apl.GetWindowRect(&rect);

	int w = rect.Width() - 2;

	TCHAR *	lpszHeaders[] = { _T("Путь к приложению"),
		_T("Hash"),
		_T("Путь"),
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
		//lvcolumn.fmt = (i == 0) ? LVCFMT_LEFT : LVCFMT_CENTER;
		lvcolumn.fmt = LVCFMT_LEFT;
		//lvcolumn.fmt = (i == 5) ? LVCFMT_RIGHT : lvcolumn.fmt;
		lvcolumn.pszText = lpszHeaders[i];
		lvcolumn.iSubItem = i;

		if (i==0)
			lvcolumn.cx = 300;
		else if (i==1 || i==2)
			lvcolumn.cx = 50;
		else if (i==3)
			lvcolumn.cx = 70;
		//lvcolumn.cx = (lpszHeaders[i+1] == NULL) ? w - total_cx - 2 : (w * m_nColWidths[i]) / 64;
		//total_cx += lvcolumn.cx;
		m_List_Apl.InsertColumn(i, &lvcolumn);
	}

	// iterate through header items and attach the image list
	HDITEM hditem;

	for (i = 0; i < m_List_Apl.m_HeaderCtrl.GetItemCount(); i++)
	{
		hditem.mask = HDI_IMAGE | HDI_FORMAT;
		m_List_Apl.m_HeaderCtrl.GetItem(i, &hditem);
		hditem.fmt |=  HDF_IMAGE;
		//if (i == 0 || i == 3)
		//	hditem.iImage = XHEADERCTRL_UNCHECKED_IMAGE;
		//else
		hditem.iImage = XHEADERCTRL_NO_IMAGE;

		m_List_Apl.m_HeaderCtrl.SetItem(i, &hditem);
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
		m_List_Apl.SetColumn(i, &lvcolumn);
	}
}

BOOL
CListApp::AddApl(PAPPL_ITEM pAplItem)
{
	
	m_List_Apl.LockWindowUpdate();	// ***** lock window updates while filling list *****

	//m_List_Apl.DeleteAllItems();

	CString str = _T("");
	int nItem, nSubItem;

	// insert the items and subitems into the list
	//for (nItem = 0; nItem < 50; nItem++)
	{
		nItem=m_List_Apl.GetItemCount();
		
		for (nSubItem = 0; nSubItem < 3; nSubItem++)
		{
			str = _T("");

			if (nSubItem == 0)				// checkbox
				str = pAplItem->path;
			else if (nSubItem == 1)			// combo
				str = _T("");
			else if (nSubItem == 2)			// progress
				str = _T("");
			

			// show background coloring
			if (nSubItem == 0)
				m_List_Apl.InsertItem(nItem, str);
			else
				m_List_Apl.SetItemText(nItem, nSubItem, str);

			if (nSubItem == 1 )
				m_List_Apl.SetCheckbox(nItem, nSubItem, pAplItem->hash_ch);
			if (nSubItem == 2)
				m_List_Apl.SetCheckbox(nItem, nSubItem, pAplItem->path_ch);
		}
		//m_List_Apl.SetItemCount(nItem+1);
		//itemCount++;
	}
	
	m_List_Apl.UnlockWindowUpdate();
	return TRUE;
}
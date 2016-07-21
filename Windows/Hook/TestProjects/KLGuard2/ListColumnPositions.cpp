// ListColumnPositions.cpp: implementation of the CListColumnPositions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ListColumnPositions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListColumnPositions::CListColumnPositions(PTCHAR ptchName, CListCtrl* pListCtrl)
{
	m_ptchName = ptchName;
	m_pListCtrl = pListCtrl;
}

CListColumnPositions::~CListColumnPositions()
{

}

void CListColumnPositions::SavePositions()
{
	TCHAR strname[260];
	TCHAR strbuf[260];
	HDITEM HeaderItem;

	CHeaderCtrl* pHeader = m_pListCtrl->GetHeaderCtrl();
	
	HeaderItem.mask = HDI_TEXT | HDI_WIDTH;
	HeaderItem.pszText = strbuf;

	for (int cou = pHeader->GetItemCount() - 1; cou >=0; cou--)
	{
		HeaderItem.cchTextMax = sizeof(strbuf) / sizeof(strbuf[0]);

		pHeader->GetItem(cou, &HeaderItem);
		wsprintf(strname, _T("%s_%s"), m_ptchName, HeaderItem.pszText);

		g_pReestr->SetValue(strname, (DWORD) HeaderItem.cxy);
	}
}

void CListColumnPositions::RestorePositions()
{
	TCHAR strname[260];
	TCHAR strbuf[260];
	HDITEM HeaderItem;

	CHeaderCtrl* pHeader = m_pListCtrl->GetHeaderCtrl();
	
	HeaderItem.pszText = strbuf;

	for (int cou = pHeader->GetItemCount() - 1; cou >=0; cou--)
	{
		HeaderItem.mask = HDI_TEXT;
		HeaderItem.cchTextMax = sizeof(strbuf) / sizeof(strbuf[0]);

		pHeader->GetItem(cou, &HeaderItem);
		wsprintf(strname, _T("%s_%s"), m_ptchName, HeaderItem.pszText);

		HeaderItem.cxy = g_pReestr->GetValue(strname, 0);
		if (HeaderItem.cxy != 0)
		{		
			HeaderItem.mask = HDI_WIDTH;
			pHeader->SetItem(cou, &HeaderItem);
		}
	}
}

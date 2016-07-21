// BaseListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "common.h"
#include "BaseListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCBaseListCtrl

CCBaseListCtrl::CCBaseListCtrl()
{
}

CCBaseListCtrl::~CCBaseListCtrl()
{
}


BEGIN_MESSAGE_MAP(CCBaseListCtrl, CListCtrl)
//{{AFX_MSG_MAP(CCBaseListCtrl)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCBaseListCtrl message handlers

void CCBaseListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	//for this notification, the structure is actually a
	// NMLVCUSTOMDRAW that tells you what's going on with the custom
	// draw action. So, we'll need to cast the generic pNMHDR pointer.
	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	
	switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW;          // ask for subitem notifications.
		break;
		
	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		break;
		
	case CDDS_ITEMPREPAINT|CDDS_SUBITEM:
		{
			*pResult = CDRF_DODEFAULT;

			LVITEM lv;
			ZeroMemory(&lv, sizeof (lv));
			
			lv.mask = LVIF_IMAGE | LVIF_STATE;
			lv.iItem = lplvcd->nmcd.dwItemSpec;
			lv.iSubItem = lplvcd->iSubItem;

			if ( !GetItem(&lv) )
				return;

			CRect rcText(0,0,0,0), rcImage(0,0,0,0), rcItem(0,0,0,0);
			GetTextIconRect(lv.iItem, lv.iSubItem, rcText, rcImage, rcItem);

			::ExtTextOut(lplvcd->nmcd.hdc, 0, 0, ETO_OPAQUE, &rcItem, NULL, 0, NULL);

			if ( !rcImage.IsRectEmpty  () )
			{
				CImageList* pImageList = GetImageList(LVSIL_SMALL);
				
				if (pImageList)
				{
					if (lv.iImage == I_IMAGECALLBACK)
					{
						//callback
					}
					else if ( lv.iImage < pImageList->GetImageCount  () )
					{
						HICON hIcon = pImageList->ExtractIcon  (lv.iImage);

						if (hIcon)
						{
							::DrawIconEx(lplvcd->nmcd.hdc, rcImage.left, rcImage.top, hIcon, 16, 16, 0, NULL, DI_NORMAL );
							DestroyIcon(hIcon);
						}
					}
				}
			}
			
			CString sItem = GetItemText(lv.iItem, lv.iSubItem);
			
			if ( !sItem.IsEmpty  () )
				::DrawText  (lplvcd->nmcd.hdc, sItem, sItem.GetLength  (), rcText, DT_LEFT | DT_VCENTER | DT_PATH_ELLIPSIS);
			
			*pResult= CDRF_SKIPDEFAULT;
			break;
		}
		
	default:// it wasn't a notification that was interesting to us.
		*pResult = CDRF_DODEFAULT;
	}
}

BOOL CCBaseListCtrl::GetTextIconRect( int iRow, int iCol, CRect &rectText, CRect &rectIcon, CRect &rectItem )
{

	if(iCol)
		return GetSubItemRect(iRow, iCol, LVIR_BOUNDS, rectItem) && GetSubItemRect(iRow, iCol, LVIR_LABEL, rectText);

		
	
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();

	if(pHeaderCtrl && pHeaderCtrl->GetItemCount()== 1)
	{
		GetItemRect(iRow, rectItem, LVIR_BOUNDS);
		GetItemRect(iRow, rectIcon, LVIR_ICON);
		GetItemRect(iRow, rectText, LVIR_LABEL);
//		rectText.right = rectItem.right;
		return TRUE;
	}
		
	
	iCol = 1;
	CRect rCol1;
	if(!GetSubItemRect(iRow, iCol, LVIR_BOUNDS, rCol1))
		return FALSE;
	
	if(!GetItemRect(iRow, rectItem, LVIR_LABEL))
		return FALSE;
	
	rectItem.right = rCol1.left;

	GetItemRect(iRow, rectIcon, LVIR_ICON);
	GetItemRect(iRow, rectText, LVIR_LABEL);
	
	return TRUE;
}




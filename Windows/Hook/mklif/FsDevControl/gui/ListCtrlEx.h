#pragma once


// CListCtrlEx
#include "XListCtrl.h"


typedef enum _list_selector
{
	
	LIST_NULL=0,
	LIST_APL=1,
	LIST_RUL=2

}list_selector;

class CListCtrlEx : public CXListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)

public:
	CListCtrlEx();
	//virtual ~CListCtrlEx();
	list_selector *pls;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};



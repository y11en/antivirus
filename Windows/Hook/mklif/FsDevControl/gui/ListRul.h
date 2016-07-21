#pragma once


#include "ListCtrlEx.h"

//typedef struct _RUL_ITEM 
//{
//	WCHAR	symbolLink[260];
//	int		w_3st;
//	int		r_3st; 
//	int		e_3st; 
//}RUL_ITEM, *PRUL_ITEM;
#include "Cont.h"

class CListRul
{
public:
	CListCtrlEx	m_List_Rul;
	int selected;

	CListRul(void);
	~CListRul(void);

	void CListRul::InitList(list_selector *pls);
	BOOL CListRul::AddRul( PRUL_ITEM pRulItem);
	void CListRul::ClearRul();


	CStringArray cmbBoxArr;
	CStringArray cmbBoxBlockID;
	CStringArray cmbBoxLogFlag;
};

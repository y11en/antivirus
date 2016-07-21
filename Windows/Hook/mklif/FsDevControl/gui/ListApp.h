#pragma once

#include "ListCtrlEx.h"

//typedef struct _APPL_ITEM 
//{
//	WCHAR	path[260];
//	BOOL	hash_ch; 
//	BOOL	path_ch;
//}APPL_ITEM, *PAPPL_ITEM;
#include "Cont.h"

class CListApp
{
public:
	CListCtrlEx	m_List_Apl;
	int selected;

	CListApp(void);
	~CListApp(void);
	
	void CListApp::InitList(list_selector *pls);

	BOOL AddApl(PAPPL_ITEM pAplItem);
protected:
	//int itemCount;
};

#ifndef CTXMENU_H
#define CTXMENU_H

#include "BCMenu.h"

extern HINSTANCE hLocalizationLibrary;

class CContextMenu : public BCMenu
{
public:
	CContextMenu();
	CWnd* m_pwndParent, *m_pwndCtrl;

	CContextMenu(CWnd* pwndParent, CWnd* pwndCtrl);
	~CContextMenu();
	
	BOOL LoadMenu(UINT nMenuResourceID, UINT nTBResourceID);
	int TrackMenu(int nID, int nMethod);

	virtual void SetMenuDefaultItem(UINT iCmd);
};

#endif
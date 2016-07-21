#include "stdafx.h"
#include "NewCtxMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CContextMenu::CContextMenu()
{
	m_pwndCtrl = NULL;
	m_pwndParent = NULL;
}

CContextMenu::CContextMenu(CWnd* pwndParent, CWnd* pwndCtrl)
{
	m_pwndCtrl = pwndCtrl;
	m_pwndParent = pwndParent;
}

int CContextMenu::TrackMenu(int nID, int nMethod)
{
	int rc = 0;

	try
	{
		CRect R;
		CPoint P;
		GetCursorPos(&P);
		
		switch (nMethod) {
			case 0:	// Mouse or kbd
			case 2:	// Kbd
				if (m_pwndCtrl)
				{
					m_pwndCtrl->GetClientRect(&R);
					m_pwndCtrl->ClientToScreen(&R);
					if (!R.PtInRect(P))
					{
						P.x = R.left;
						P.y = R.top;
					}
				}
			break;
			case 1:	// Mouse
			break;
		}

		BCMenu* pMenu = (BCMenu*)GetSubMenu(nID);
		if (pMenu)
		{
			pMenu->SetMenuDefaultItem(m_iDefaultItem);
			rc = pMenu->TrackPopupMenu(0, P.x, P.y, m_pwndParent);
		}
		
		if (m_pwndParent) m_pwndParent->PostMessage(WM_NULL, 0, 0);

		DestroyMenu();
	}
	catch(...)
	{
	}

	return rc;
}

CContextMenu::~CContextMenu()
{
	DestroyMenu();
}

BOOL CContextMenu::LoadMenu(UINT nMenuResourceID, UINT nTBResourceID)
{
	try
	{
		if (!BCMenu::LoadMenu(hLocalizationLibrary, MAKEINTRESOURCE(nMenuResourceID)))
		{
			if (!BCMenu::LoadMenu(NULL, MAKEINTRESOURCE(nMenuResourceID))) return FALSE;
		}
		
		LoadToolbar(nTBResourceID);

		return TRUE;
	}
	catch(...)
	{
	}

	return FALSE;
}

void CContextMenu::SetMenuDefaultItem(UINT iCmd)
{
	m_iDefaultItem = iCmd;
}

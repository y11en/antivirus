
#pragma warning ( disable : 4786 )

#include "IeStatusBar.h"
#include "ver_mod.h"

#include <Prague/pr_remote.h>
#include <ProductCore/guiloader.h>

#include <Prague/plugin/p_win32_reg.h>

#include <Exdisp.h>
#include <commctrl.h>

#include "scplugins.h"


#define ICONID_MAIN            "Main"
#define ICONID_MAINOFF         "MainOff"
#define ICONID_POPUPBLOCKED    "PopupBlocked"
#define ICONID_POPUPALLOWED    "PopupAllowed"

#define SECTID_IEPLUGINMENU    "IePluginMenu"

#define STRID_ADDTORULES       "AddToRules"
#define STRID_BLOCKNEVER       "BlockNever"

extern HINSTANCE    g_hInstance;
hROOT               g_root = NULL;
cWndSubclasser      g_WndList;
cLocalizedResources g_LocResources;

LRESULT __stdcall IeStatusBarWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////
// cPragueLoader

typedef tERROR (*LPPRGetAPI)(PRRemoteAPI *api);

class cPragueLoader : public PRRemoteAPI
{
public:
	cPragueLoader() : 
		m_hPrRemote(NULL)
	{}
	
	virtual ~cPragueLoader()
	{
		Unload();
	}

	bool Load(HINSTANCE hInst)
	{
		if(!m_hPrRemote)
		{
			if (::GetModuleHandle(PR_REMOTE_MODULE_NAME))
				m_hPrRemote = LoadLibrary(PR_REMOTE_MODULE_NAME);

			if( !m_hPrRemote )
			{
				char szPrRemoteLoader[MAX_PATH + 20];
				GetModuleFileName(hInst, szPrRemoteLoader, countof(szPrRemoteLoader));
				char *pFile = strrchr(szPrRemoteLoader, '\\');
				if(pFile)
					strcpy(pFile + 1, PR_REMOTE_MODULE_NAME);

				m_hPrRemote = ::LoadLibraryEx(szPrRemoteLoader, NULL, LOAD_WITH_ALTERED_SEARCH_PATH) ;
			}

			if(!m_hPrRemote)
				return false;

			LPPRGetAPI pPRGetAPI = (LPPRGetAPI)::GetProcAddress(m_hPrRemote, "PRGetAPI") ;
			if(!pPRGetAPI)
				return false;

			if(PR_FAIL(pPRGetAPI(this)))
				return false;
			
			if(PR_FAIL(Initialize(PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH,NULL)))
				return false;

			if(PR_FAIL(GetRoot(&g_root)) || !g_root)
				return false;

			tCHAR* plugins_list[] = {"nfio.ppl"};
			for (int i = 0; i < countof(plugins_list); i++)
			{
				hPLUGIN ppl;
				if(PR_FAIL(g_root->LoadModule(&ppl, plugins_list[i], (tDWORD)strlen(plugins_list[i]), cCP_ANSI)))
					return false;
			}
		}
		return true;
	}
	
	void Unload()
	{
		if(m_hPrRemote)
		{
			Deinitialize(), g_root = NULL;
			::FreeLibrary(m_hPrRemote), m_hPrRemote = NULL;
		}
	}
	
private:
	HMODULE         m_hPrRemote;
};

//////////////////////////////////////////////////////////////////////////
// cLocalizedResources

cLocalizedResources::cLocalizedResources() :
	m_bInited(false)
{
	memset(m_hIcons, sizeof(m_hIcons), 0);
}

cLocalizedResources::~cLocalizedResources()
{
	for(int i = 0; i < countof(m_hIcons); i++)
		if(m_hIcons[i])
			DestroyIcon(m_hIcons[i]), m_hIcons[i] = NULL;

	if(m_sMenuBlockNever)
		free(m_sMenuBlockNever), m_sMenuBlockNever = NULL;
	
	if(m_sMenuAddToRules)
		free(m_sMenuAddToRules), m_sMenuAddToRules = NULL;
}


bool cLocalizedResources::Load()
{
#ifdef _DEBUG
	return false;
#endif
	if(!m_bInited)
	{
		cPragueLoader PrLoader;
		if(PrLoader.Load(g_hInstance))
		{
			CGuiLoader GuiLoader;
			if(CRootItem *pRoot = GuiLoader.CreateAndInitRoot(g_root))
			{
				m_bInited = true;

				tString str;
#if _BASEGUI_VER >= 0x0200
				pRoot->GetString(str, PROFILE_LOCALIZE, SECTID_IEPLUGINMENU, STRID_BLOCKNEVER);
#else
				str = pRoot->GetStringBuf(PROFILE_LOCALIZE, SECTID_IEPLUGINMENU, STRID_BLOCKNEVER);
#endif
				if(!str.empty())
					m_sMenuBlockNever = _strdup(str.c_str());
				
#if _BASEGUI_VER >= 0x0200
				pRoot->GetString(str, PROFILE_LOCALIZE, SECTID_IEPLUGINMENU, STRID_ADDTORULES);
#else
				str = pRoot->GetStringBuf(PROFILE_LOCALIZE, SECTID_IEPLUGINMENU, STRID_ADDTORULES);
#endif
				if(!str.empty())
					m_sMenuAddToRules = _strdup(str.c_str());

				CIcon* pIcon;
				if(pIcon = pRoot->GetIcon(ICONID_MAIN))         m_hIcons[0] = CopyIcon(pIcon->IconHnd());
				if(pIcon = pRoot->GetIcon(ICONID_MAINOFF))      m_hIcons[1] = CopyIcon(pIcon->IconHnd());
				if(pIcon = pRoot->GetIcon(ICONID_POPUPBLOCKED)) m_hIcons[2] = CopyIcon(pIcon->IconHnd());
				if(pIcon = pRoot->GetIcon(ICONID_POPUPALLOWED)) m_hIcons[3] = CopyIcon(pIcon->IconHnd());

				GuiLoader.Unload();
			}

			PrLoader.Unload();
		}
	}
	return m_bInited;
}

//////////////////////////////////////////////////////////////////////////
// Global functions

BOOL IsEqualDomains(WCHAR* szUrl1, WCHAR* szUrl2) 
{ 
        WCHAR c1, c2; 
        if (!szUrl1 || !szUrl2) 
                return FALSE; 
        do 
        { 
                c1 = *szUrl1++; 
                c2 = *szUrl2++; 
                if (c1 >= 'A' && c1 <= 'Z') 
                        c1 |= 0x20; // lcase 
                if (c2 >= 'A' && c2 <= 'Z') 
                        c2 |= 0x20; // lcase 
                if (c1 == ':' && szUrl1[0] == '/' && szUrl2[1] == '/' && c2 == ':' && szUrl2[0]=='/' && szUrl2[1]=='/') 
                { 
                        // skip protocol delimiter 
                        szUrl1+=2; 
                        szUrl2+=2; 
                } 
                if (c1 == '/') 
                        c1 = 0; 
                if (c2 == '/') 
                        c2 = 0; 
                if (c1 != c2) 
                        return FALSE; 
        } while (c1);
        return TRUE; 
}

void IeStatusBarSetState(HWND hWnd, UINT nPart, DWORD nState, IWebBrowser2 *pWebBrowser2, WCHAR *sSourceUrl)
{
	if (!hWnd)
		return;

	g_LocResources.Load();
	
	cWndSubclasser::WND_LIST *pItem = g_WndList.GetWndListItem(hWnd);
	if(!pItem)
	{
		g_WndList.Subclass(hWnd, IeStatusBarWndProc);
		pItem = g_WndList.GetWndListItem(hWnd);
	}
	if(pItem)
	{
		LRESULT nTotalParts = SendMessage(hWnd, SB_GETPARTS, 0, 0);
		if ((LRESULT)nPart+1 >= nTotalParts)
			nPart--;
		pItem->m_nPart = nPart;
		if(nState & (PBSTATE_BLOCKED|PBSTATE_ALLOWED|PBSTATE_IDLE))
		{
			pItem->m_pWebBrowser2 = pWebBrowser2;
			if(pItem->m_sSourceUrl)
				free(pItem->m_sSourceUrl), pItem->m_sSourceUrl = NULL;
			if(sSourceUrl)
				pItem->m_sSourceUrl = _wcsdup(sSourceUrl);
		}

		DWORD nNewState = pItem->m_nState;
		
		if(nState & 0x0f)
		{
			nNewState &= 0x0f;
			nNewState |= nState & 0x0f;
		}
		if(nState & 0xf0)
		{
			nNewState &= 0xf0;
			nNewState |= nState & 0xf0;
		}
		if(nState & SCSTATE_IDLE)
			nNewState &= ~0x0f;
		if(nState & PBSTATE_IDLE)
			nNewState &= ~0xf0;

		if(pItem->m_nState != nNewState)
		{
			pItem->m_nState = nNewState;

			RECT partrect;
			SendMessage(hWnd, SB_GETRECT, pItem->m_nPart, (LPARAM)&partrect);
			InvalidateRect(hWnd, &partrect, FALSE);
		}
	}
}

bool IsPopupsTemoraryAllowed(HWND hWnd, WCHAR *sUrl)
{
	cWndSubclasser::WND_LIST *pItem = g_WndList.GetWndListItem(hWnd);
	if (!pItem)
		return false;
	if (pItem->m_bTemporaryAllowDomain)
		return !!IsEqualDomains(pItem->m_sTemporaryAllowedUrl, sUrl);
	return (0 == _wcsicmp(pItem->m_sTemporaryAllowedUrl, sUrl));
}

//void IeStatusBarResetState(HWND hWnd)
//{
//	cWndSubclasser::WND_LIST *pItem = g_WndList.GetWndListItem(hWnd);
//	if(pItem)
//		pItem->m_bPopupsAllowed = false;
//}

BOOL CALLBACK FindIEServerEnumChildProc(HWND hwnd, LPARAM lParam)
{
	char szClass[0x100];
	if (GetClassName(hwnd, szClass, countof(szClass)))
	{
		szClass[countof(szClass)-1] = 0;
		if (!_stricmp(szClass, "Internet Explorer_Server"))
		{
			*(HWND*)lParam = hwnd;
			return FALSE;
		}
	}
	return TRUE;
}

HWND FindIEServer(HWND hWnd)
{
	HWND hWndIEServer = NULL;
	while (hWnd)
	{
		EnumChildWindows(hWnd, FindIEServerEnumChildProc, (LPARAM)&hWndIEServer);
		hWnd = GetParent(hWnd);
	}
	return hWndIEServer;
}

bool IeStatusBarTemoraryAllow(HWND hWnd, WCHAR* pSourceUrl, bool bThisPageOnly)
{
	cWndSubclasser::WND_LIST *pItem = g_WndList.GetWndListItem(hWnd);
	if (!pItem)
		return false;
	ODS(("IeStatusBarTemoraryAllow: pItem=%08X", pItem));
	if(pItem->m_sTemporaryAllowedUrl)
		free(pItem->m_sTemporaryAllowedUrl), pItem->m_sTemporaryAllowedUrl = NULL;
	ODS(("IeStatusBarTemoraryAllow: pSourceUrl=%S", pSourceUrl));
	if (pSourceUrl)
	{
		pItem->m_sTemporaryAllowedUrl = _wcsdup(pSourceUrl);
		pItem->m_bTemporaryAllowDomain = !bThisPageOnly;
	}
	ODS(("IeStatusBarTemoraryAllow: pItem->m_pWebBrowser2=%08X", pItem->m_pWebBrowser2));
	if (pItem->m_pWebBrowser2)
	{
		HWND hWndIEServer = FindIEServer(hWnd);
		ODS(("IeStatusBarTemoraryAllow: pWebBrowser2->Reresh()..."));
		PostMessage(hWndIEServer, WM_KEYDOWN, VK_F5, 0);
		//pItem->m_pWebBrowser2->Refresh();
		ODS(("IeStatusBarTemoraryAllow: pWebBrowser2->Reresh() done"));
	}
	return true;
}


LRESULT __stdcall IeStatusBarWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT res = 0;

	switch(uMsg)
	{
	case WM_PAINT:
		{
			cWndSubclasser::WND_LIST *pItem = g_WndList.GetWndListItem(hWnd);
			if(pItem)// && pItem->m_nState)
			{
				RECT partrect;
				SendMessage(hWnd, SB_GETRECT, pItem->m_nPart, (LPARAM)&partrect);

				HDC hDC = GetDC(hWnd);

				HRGN hRgn = CreateRectRgnIndirect(&partrect);
				SelectClipRgn(hDC, hRgn);
				
				res = g_WndList.CallWindowProc(hWnd, uMsg, wParam, lParam);
				
				SelectClipRgn(hDC, NULL);
				DeleteObject((HGDIOBJ)hRgn);

				HICON hIcon = NULL;

				if(pItem->m_nState & SCSTATE_ACTIVE)
					hIcon = g_LocResources.m_hIcons[0];
				else
				if(pItem->m_nState & SCSTATE_INACTIVE)
					hIcon = g_LocResources.m_hIcons[1];
				else
				if(pItem->m_nState & PBSTATE_BLOCKED)
					hIcon = g_LocResources.m_hIcons[2];
				else
				if(pItem->m_nState & PBSTATE_ALLOWED)
					hIcon = g_LocResources.m_hIcons[3];

				if(hIcon)
					DrawIconEx(hDC, partrect.left + 1, partrect.top + 1, hIcon, 16, 16, 0, NULL, DI_NORMAL);
				
				ReleaseDC(hWnd, hDC);
			}
			else
				res = g_WndList.CallWindowProc(hWnd, uMsg, wParam, lParam);
		}
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		{
			cWndSubclasser::WND_LIST *pItem = g_WndList.GetWndListItem(hWnd);
			if(pItem)
			{
				RECT partrect;
				SendMessage(hWnd, SB_GETRECT, pItem->m_nPart, (LPARAM)&partrect);
				
				POINT pt; pt.x = LOWORD(lParam); pt.y = HIWORD(lParam);
				
				if(!(pt.x >= partrect.left && pt.x <= partrect.right && pt.y >= partrect.top && pt.y <= partrect.bottom))
				{
					res = g_WndList.CallWindowProc(hWnd, uMsg, wParam, lParam);
					break;
				}
				if(g_LocResources.Load())
				{
					HMENU hMenu;
					if(g_LocResources.m_sMenuBlockNever && g_LocResources.m_sMenuAddToRules && (hMenu = CreatePopupMenu()))
					{
						bool bAlreadyBlock = pItem->m_sTemporaryAllowedUrl && pItem->m_sSourceUrl && !wcscmp(pItem->m_sTemporaryAllowedUrl, pItem->m_sSourceUrl);
						
						AppendMenu(hMenu, MF_STRING | (bAlreadyBlock ? MF_DISABLED | MF_GRAYED : 0), 1, g_LocResources.m_sMenuBlockNever);
						AppendMenu(hMenu, MF_STRING, 2, g_LocResources.m_sMenuAddToRules);
						
						ClientToScreen(hWnd, &pt);
						UINT nSelPos = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL);
						switch(nSelPos)
						{
						case 1:
							if(pItem->m_sTemporaryAllowedUrl)
								free(pItem->m_sTemporaryAllowedUrl), pItem->m_sTemporaryAllowedUrl = NULL;
							pItem->m_sTemporaryAllowedUrl = _wcsdup(pItem->m_sSourceUrl);
							
							if(pItem->m_pWebBrowser2)
								pItem->m_pWebBrowser2->Refresh();
							pItem->m_nState &= ~0xf0;
							pItem->m_nState |= PBSTATE_ALLOWED;
							InvalidateRect(hWnd, &partrect, FALSE);
							break;
						
						case 2:
							ScPluginsCheckPopupURL(SCP_ACTION_POPUP_ADD_TRUSTED_SITE, pItem->m_sSourceUrl, NULL, NULL, NULL, NULL);
							if(!pItem->m_sTemporaryAllowedUrl && pItem->m_pWebBrowser2)
								pItem->m_pWebBrowser2->Refresh();
							break;
						}
						DestroyMenu(hMenu);
					}
				}
			}
		}
		break;

	case WM_DESTROY:
		res = g_WndList.CallWindowProc(hWnd, uMsg, wParam, lParam);
		g_WndList.Unsubclass(hWnd);
		break;
	
	default:
		res = g_WndList.CallWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return res;
}

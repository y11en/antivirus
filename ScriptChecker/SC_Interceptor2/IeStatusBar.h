#pragma once

#ifndef WINVER 
#define WINVER 0x0500
#endif

#include <windows.h>
#include <stdio.h>
#include "debug.h"

#define SCSTATE_ACTIVE   0x01
#define SCSTATE_INACTIVE 0x02
#define SCSTATE_IDLE     0x08

#define PBSTATE_BLOCKED  0x10
#define PBSTATE_ALLOWED  0x20
#define PBSTATE_IDLE     0x80


interface IWebBrowser2;

void IeStatusBarSetState(HWND hWnd, UINT nPart, DWORD nState, IWebBrowser2 *pWebBrowser, WCHAR *sSourceUrl);
//void IeStatusBarResetState(HWND hWnd, WCHAR *sUrl);
bool IsPopupsTemoraryAllowed(HWND hWnd, WCHAR *sUrl);
bool IeStatusBarTemoraryAllow(HWND hWnd, WCHAR* pSourceUrl, bool bThisPageOnly);

class cCS : protected CRITICAL_SECTION
{
public:
	cCS()        { InitializeCriticalSection(this); }
	~cCS()       { DeleteCriticalSection(this); }
	void Enter() { EnterCriticalSection(this); }
	void Leave() { LeaveCriticalSection(this); }
};

class cAutoCSNative
{
public:
	cAutoCSNative(cCS &cs) : m_cs(cs) { m_cs.Enter(); }
	~cAutoCSNative()                  { m_cs.Leave(); }

protected:
	cCS & m_cs;
};


class cWndSubclasser
{
public:
	struct WND_LIST
	{
		WND_LIST() :
			m_hWnd(NULL),
			m_pOriginalWndProc(NULL),
			m_nState(SCSTATE_IDLE),
//			m_bPopupsAllowed(FALSE),
			m_nPart(0),
			m_sSourceUrl(NULL),
			m_sTemporaryAllowedUrl(NULL),
			m_bTemporaryAllowDomain(TRUE),
			m_pWebBrowser2(NULL),
			m_pNext(NULL)
		{}
		~WND_LIST()
		{
			if(m_sSourceUrl)
				free(m_sSourceUrl), m_sSourceUrl = NULL;

			if(m_sTemporaryAllowedUrl)
				free(m_sTemporaryAllowedUrl), m_sTemporaryAllowedUrl = NULL;
		}

		HWND           m_hWnd;
		WNDPROC        m_pOriginalWndProc;
		DWORD          m_nState;
//		BOOL           m_bPopupsAllowed;
		UINT           m_nPart;
		WCHAR        * m_sSourceUrl;
		WCHAR        * m_sTemporaryAllowedUrl;
		BOOL           m_bTemporaryAllowDomain;
		IWebBrowser2 * m_pWebBrowser2;
		WND_LIST     * m_pNext;
	};


public:
	cWndSubclasser() :
		m_pRoot(NULL)
	{}
	
	~cWndSubclasser()
	{
		cAutoCSNative locker(m_cs);
		for(WND_LIST *pItem = m_pRoot; pItem; )
		{
			WND_LIST *pItem2Delete = pItem;
			pItem = pItem->m_pNext;
			delete pItem2Delete;
		}
	}

	void Subclass(HWND hWnd, WNDPROC pNewWndProc)
	{
		WND_LIST *pItem = GetWndListItemEx(hWnd);
		if(pItem && !pItem->m_pOriginalWndProc)
			pItem->m_pOriginalWndProc = (WNDPROC)::SetWindowLong(hWnd, GWLP_WNDPROC, (LONG)pNewWndProc);
	}
	
	void Unsubclass(HWND hWnd)
	{
		WND_LIST *pItem = GetWndListItem(hWnd);
		if(pItem)
		{
			::SetWindowLong(hWnd, GWLP_WNDPROC, (LONG)pItem->m_pOriginalWndProc);
			RemoveWndListItem(hWnd);
		}
	}

	LRESULT CallWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WND_LIST *pItem = GetWndListItem(hWnd);
		if(pItem)
			return ::CallWindowProc(pItem->m_pOriginalWndProc, hWnd, uMsg, wParam, lParam);
		return 0;
	}

	WND_LIST *GetWndListItem(HWND hWnd)
	{
		cAutoCSNative locker(m_cs);
		for(WND_LIST *pItem = m_pRoot; pItem; pItem = pItem->m_pNext)
			if(pItem->m_hWnd == hWnd)
				return pItem;
		return NULL;
	}

	WND_LIST *GetWndListItemEx(HWND hWnd)
	{
		cAutoCSNative locker(m_cs);
		WND_LIST *pItem = GetWndListItem(hWnd);
		if(!pItem)
		{
			pItem = new WND_LIST();
			if(pItem)
			{
				pItem->m_hWnd = hWnd;
				pItem->m_pNext = m_pRoot;
				m_pRoot = pItem;
			}
		}
		return pItem;
	}

	void RemoveWndListItem(HWND hWnd)
	{
		cAutoCSNative locker(m_cs);
		for(WND_LIST *pItem = m_pRoot, *pPrev = NULL; pItem; pPrev = pItem, pItem = pItem->m_pNext)
		{
			if(pItem->m_hWnd == hWnd)
			{
				if(pPrev)
					pPrev->m_pNext = pItem->m_pNext;
				else
					m_pRoot = pItem->m_pNext;
				delete pItem;
				break;
			}
		}
	}

protected:
	WND_LIST * m_pRoot;
	cCS        m_cs;
};

class cLocalizedResources
{
public:
	cLocalizedResources();
	~cLocalizedResources();

	bool Load();

	HICON   m_hIcons[4];
	char  * m_sMenuBlockNever;
	char  * m_sMenuAddToRules;

protected:
	bool    m_bInited;
};

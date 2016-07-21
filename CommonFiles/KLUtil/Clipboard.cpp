#include "stdafx.h"
#include "Clipboard.h"
#include "UnicodeConv.h"

namespace KLUTIL
{
	void EmptyClipboard(HWND hWnd /*= NULL*/)
	{
		if (!::OpenClipboard(hWnd))
			return;
		::EmptyClipboard();
		::CloseClipboard();
	}

	bool CopyToClipboard(LPCTSTR lpszText, HWND hWnd /*= NULL*/)
	{
		bool bResult = true;

		if (!::OpenClipboard(hWnd))
			return false;
		
		::EmptyClipboard();

		// ≈сли hWnd равна NULL, то после EmptyClipboard() нужно переоткрыть
		if (!hWnd)
		{
			::CloseClipboard();
			if (!::OpenClipboard(hWnd))
				return false;
		}

		int nRet = 0;
		int nLenght = _tcslen(lpszText);
		HGLOBAL hCopy = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, nLenght + 1);
		if (hCopy)
		{
			LPSTR pszCopy = (LPSTR)::GlobalLock(hCopy);
			strcpy(pszCopy, __LPCSTR(lpszText));
			pszCopy[nLenght] = '\0';
			::GlobalUnlock(hCopy);
			if (::SetClipboardData(CF_TEXT, hCopy))
			{
				//  роме самого текста необходимо сохранить €зык дл€
				// корректного извлечени€ русского некоторыми приложени€ми
				hCopy = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, sizeof(LCID));
				if (hCopy)
				{
					LCID* pLangID = (LCID*)::GlobalLock(hCopy);
					*pLangID = ::GetUserDefaultLCID();
					::GlobalUnlock(hCopy);
					if (!SetClipboardData(CF_LOCALE, hCopy))
						::GlobalFree(hCopy);
				}
			}
			else
			{
				::GlobalFree(hCopy);
				bResult = false;
			}
		}
		else
			bResult = false;

		::CloseClipboard();

		return bResult;
	}
} // namespace KLUTIL
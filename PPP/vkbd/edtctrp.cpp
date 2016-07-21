#include <windows.h>

#include "edtctrp.h"

typedef DWORD ICH, *PICH, *LPICH;
typedef PVOID PWND;		// tmp

BOOL ValidateWindowClass(HWND hWnd)
{
	WCHAR wcClassName[512];
	DWORD dwResult = GetClassNameW(hWnd, wcClassName, _countof(wcClassName));
	if (dwResult)
	{
		if (_wcsicmp(wcClassName, L"edit") == 0)
			return TRUE;
	}

	return FALSE;
}

#ifndef _WIN64
//+ this code for win32 only ----------------------------------------------------------------------------

#include <pshpack4.h>

typedef struct tagED {
	HANDLE  hText;             /* Block of text we are editing */
	ICH     cchAlloc;          /* Number of chars we have allocated for hText
							   */
	ICH     cchTextMax;        /* Max number bytes allowed in edit control
							   */
	ICH     cch;               /* Current number of bytes of actual text
							   */
	ICH     cLines;            /* Number of lines of text */

	ICH     ichMinSel;         /* Selection extent.  MinSel is first selected
							   char */
	ICH     ichMaxSel;         /* MaxSel is first unselected character */
	ICH     ichCaret;          /* Caret location. Caret is on left side of
							   char */
	ICH     iCaretLine;        /* The line the caret is on. So that if word
							   * wrapping, we can tell if the caret is at end
							   * of a line of at beginning of next line...
							   */
	ICH     ichScreenStart;    /* Index of left most character displayed on
							   * screen for sl ec and index of top most line
							   * for multiline edit controls
							   */
	ICH     ichLinesOnScreen;  /* Number of lines we can display on screen */
	UINT    xOffset;           /* x (horizontal) scroll position in pixels
							   * (for multiline text horizontal scroll bar)
							   */
	UINT    charPasswordChar;  /* If non null, display this character instead
							   * of the real text. So that we can implement
							   * hidden text fields.
							   */
	int     cPasswordCharWidth;/* Width of password char */

	HWND    hwnd;              /* Window for this edit control */
	PWND    pwnd;              /* Pointer to window */
	RECT    rcFmt;             /* Client rectangle */
	HWND    hwndParent;        /* Parent of this edit control window */
} ED, *PED, **PPED;

#include <poppack.h>

PED GetWindowED(HWND hWnd)
{
	PED pED = (PED)(ULONG_PTR)GetWindowLongPtrW(hWnd, 0);
	if (pED)
	{
		__try
		{
			if (pED->hwnd == hWnd)
				return pED;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	return NULL;
}

BOOL ValidatePatchMethod(HWND hWnd)
{
	return FALSE;
}

HRESULT PatchEditControl(HWND hWnd, PWCHAR wcPatchStr)
{
	HRESULT hResult = E_FAIL;

	BOOL bResult = ValidateWindowClass(hWnd);
	if (bResult)
	{
		PED pED = GetWindowED(hWnd);

		if (pED)
		{
			HANDLE hTextMem = NULL;

			__try
			{
// valid handle
				if (pED->hText)
				{
					BOOL bUnicode = IsWindowUnicode(hWnd);
					DWORD cbChar = bUnicode ? sizeof(WCHAR) : sizeof(CHAR);

					DWORD cchPatchStr = (DWORD)wcslen(wcPatchStr)+1;
					DWORD cbPatchStr = cbChar*cchPatchStr;

					DWORD cbPreAlloc = (DWORD)LocalSize(pED->hText);
					if (cbPreAlloc)
					{
						if (cbPreAlloc < cbPatchStr)
						{
							DWORD cbRealloc = (cbPatchStr+0x10) & 0xfffffff0;
							HANDLE hReallocMem = LocalReAlloc(pED->hText, cbRealloc, 0);
							if (hReallocMem)
							{
								pED->hText = hReallocMem;
								pED->cchAlloc = cbRealloc/cbChar;
								hTextMem = hReallocMem;
							}
						}
						else
							hTextMem = pED->hText;
					}

// valid memory
					if (hTextMem)
					{
						PVOID pTextMem = LocalLock(hTextMem);
						if (pTextMem)
						{
							PVOID pMemToCopy = wcPatchStr;
							DWORD cbToCopy = cbPatchStr;

// translate to ansi
							if (!bUnicode)
							{
								pMemToCopy = NULL;
								cbToCopy = 0;

								PVOID PatchStr = HeapAlloc(GetProcessHeap(), 0, cbPatchStr);
								if (PatchStr)
								{
									DWORD dwResult = WideCharToMultiByte(CP_ACP, 0 ,wcPatchStr, cchPatchStr, (PCHAR)PatchStr,
										cbPatchStr, NULL, NULL);
									if (dwResult)
										pMemToCopy = PatchStr;
									else
										HeapFree(GetProcessHeap(), 0, PatchStr);
								}
							}

							if (pMemToCopy)
							{
								memcpy(pTextMem, pMemToCopy, cbToCopy);
								pED->cch = cchPatchStr-1;
								SendMessage(hWnd, EM_SETMODIFY, 1, 0);
								hResult = S_OK;

								if (pMemToCopy != wcPatchStr)
									HeapFree(GetProcessHeap(), 0, pMemToCopy);
							}

							LocalUnlock(hTextMem);
						}
					}
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}
	}

	return hResult;
}

VOID DeleteSelection( HWND hWnd, PED pED )
{
	if ( !pED )
		return;

	HANDLE hTextMem = NULL;
	__try
	{
		if ( pED->ichMinSel < pED->ichMaxSel )
		{
			DWORD cbChar = IsWindowUnicode( hWnd ) ? sizeof(WCHAR) : sizeof(CHAR);

			hTextMem = pED->hText;
			if ( hTextMem )
			{
				PVOID pTextMem = LocalLock( hTextMem );

				if ( pTextMem )
				{
					// has selection
					PVOID pSrc = (PCHAR) pTextMem + cbChar * pED->ichMaxSel;
					PVOID pDst = (PCHAR) pTextMem + cbChar * pED->ichMinSel;
					DWORD nSize = cbChar * ( pED->cch - pED->ichMaxSel );

					memmove(
						pDst,
						pSrc,
						nSize
						);

					pED->cch = pED->cch - ( pED->ichMaxSel - pED->ichMinSel );
					pED->ichCaret = pED->ichMaxSel = pED->ichMinSel;
				}
			}
		}
	}
	__finally
	{
		if ( hTextMem )
			LocalUnlock( hTextMem );
	}
}

HRESULT PatchEditControl(HWND hWnd, WCHAR wc)
{
	HRESULT hResult = E_FAIL;

	BOOL bResult = ValidateWindowClass(hWnd);
	if (bResult)
	{
		PED pED = GetWindowED(hWnd);

		if (pED)
		{
			HANDLE hTextMem = NULL;

			// delete selection if present
			DeleteSelection( hWnd, pED );

			__try
			{
				// valid handle
				if (pED->hText)
				{
					BOOL bUnicode = IsWindowUnicode(hWnd);
					DWORD cbChar = bUnicode ? sizeof(WCHAR) : sizeof(CHAR);

					DWORD cchPatchStr = pED->cch+1;
					DWORD cbPatchStr = cbChar*cchPatchStr;

					DWORD cbPreAlloc = (DWORD)LocalSize(pED->hText);
					if (cbPreAlloc)
					{
						if (cbPreAlloc < cbPatchStr)
						{
							DWORD cbRealloc = (cbPatchStr+0x10) & 0xfffffff0;
							HANDLE hReallocMem = LocalReAlloc(pED->hText, cbRealloc, 0);
							if (hReallocMem)
							{
								pED->hText = hReallocMem;
								pED->cchAlloc = cbRealloc/cbChar;
								hTextMem = hReallocMem;
							}
						}
						else
							hTextMem = pED->hText;
					}

					// valid memory
					if (hTextMem)
					{
						PVOID pTextMem = LocalLock(hTextMem);
						if (pTextMem)
						{
							PVOID pSrc = ((PBYTE)pTextMem) + cbChar*pED->ichCaret;
							PVOID pDst = ((PBYTE)pSrc) + cbChar;
							DWORD nSize = cbChar*(pED->cch - pED->ichCaret);

							memmove(pDst, pSrc, nSize);

							if (bUnicode)
								*(PWCHAR)pSrc = wc;
							else
								WideCharToMultiByte(CP_ACP, 0, &wc, 1, (PCHAR)pSrc, 1, NULL, NULL);

							pED->ichCaret++;
							pED->ichMaxSel = pED->ichCaret;
							pED->ichMinSel = pED->ichCaret;
							pED->cch = cchPatchStr;
							SendMessage(hWnd, EM_SETMODIFY, 1, 0);
							InvalidateRect(hWnd, NULL, FALSE);
							hResult = S_OK;

							LocalUnlock(hTextMem);
						}
					}
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}
	}

	if( SUCCEEDED( hResult) )
	{
		HWND hParent;

		// send update notifications, fix bug 30051
		hParent = GetParent( hWnd );
		if ( hParent )
		{
			DWORD WndId;

			WndId = GetDlgCtrlID( hWnd );

			SendMessage(
				hParent,
				WM_COMMAND,
				(DWORD) MAKELONG( WndId, EN_UPDATE ),
				(LPARAM) hWnd
				);
			SendMessage(
				hParent,
				WM_COMMAND,
				(DWORD) MAKELONG( WndId, EN_CHANGE ),
				(LPARAM) hWnd
				);
		}
	}

	return hResult;
}
//- this code for win32 only ----------------------------------------------------------------------------
#else
//+ win64 stubs -----------------------------------------------------------------------------------------
HRESULT PatchEditControl(HWND hWnd, PWCHAR wcPatchStr)
{
	return E_NOTIMPL;
}

HRESULT PatchEditControl(HWND hWnd, WCHAR wc)
{
	return E_NOTIMPL;
}
//- win64 stubs -----------------------------------------------------------------------------------------
#endif
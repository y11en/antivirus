#include <windows.h>

#include "edtctrp.h"

typedef DWORD ICH, *PICH, *LPICH;
typedef PVOID PWND;		// tmp

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

CHAR Unicode2AnsiChar(WCHAR wcChar)
{
	WCHAR wcStr[2];
	CHAR ansiStr[2];

	wcStr[0] = wcChar;
	wcStr[1] = 0;
	DWORD dwResult = WideCharToMultiByte(CP_ACP, 0 ,wcStr, 1,  ansiStr, 1, NULL, NULL);
	if (dwResult)
		return ansiStr[0];

	return 0;
}

HRESULT EditInsertChar(HWND hWnd, WCHAR wcInsertChar)
{
	HRESULT hResult = E_FAIL;

	BOOL bResult = ValidateWindowClass(hWnd);
	if (bResult)
	{
		PED pED = GetWindowED(hWnd);

		if (pED)
		{
			HANDLE hTextMem = pED->hText;

			__try
			{
// valid handle
				if (hTextMem)
				{
					BOOL bUnicode = IsWindowUnicode(hWnd);
					DWORD cbChar = bUnicode ? sizeof(WCHAR) : sizeof(CHAR);
					CHAR ansiInsertChar = 0;
					DWORD cch = pED->cch;
					DWORD cchCursor = pED->ichCaret;

					if (!bUnicode)
						ansiInsertChar = Unicode2AnsiChar(wcInsertChar);

					DWORD cbPreAlloc = (DWORD)LocalSize(hTextMem);
					if (cbPreAlloc)
					{
// plus terminating character
						DWORD cbOldData = cbChar*(pED->cch+1);
// new size
						DWORD cbNeeded = cbOldData+cbChar;

// no need to realloc memory
						if (cbPreAlloc >= cbNeeded)
						{
							PVOID pTextMem = LocalLock(hTextMem);
							if (pTextMem)
							{
// shift string and insert our character
								memmove((PCHAR)pTextMem+cbChar*(cchCursor+1), (PCHAR)pTextMem+cbChar*cchCursor, cbChar*(cch-cchCursor+1));
								if (bUnicode)
									((PWCHAR)pTextMem)[cchCursor] = wcInsertChar;
								else
									((PCHAR)pTextMem)[cchCursor] = ansiInsertChar;

								LocalUnlock(hTextMem);

								hResult = S_OK;
							}

						}
						else
						{
// we need to realloc memory
// take old data copy since it can be destroyed on realloc
							PVOID pOldDataCopy = NULL;
							PVOID pTextMem = LocalLock(hTextMem);
							if (pTextMem)
							{
								pOldDataCopy = HeapAlloc(GetProcessHeap(), 0, cbOldData);
								if (pOldDataCopy)
									memcpy(pOldDataCopy, pTextMem, cbOldData);

								LocalUnlock(hTextMem);
							}

							if (pOldDataCopy)
							{
// new allocation size
								DWORD cbRealloc = (cbNeeded+0x10) & 0xfffffff0;
								HANDLE hReallocMem = LocalReAlloc(hTextMem, cbRealloc, 0);
								if (hReallocMem)
								{
									pED->hText = hReallocMem;
									pED->cchAlloc = cbRealloc/cbChar;
									hTextMem = hReallocMem;
								
									PVOID pNewMem = LocalLock(hTextMem);
									if (pNewMem)
									{
										memcpy(pNewMem, pOldDataCopy, cbChar*cchCursor);
										if (bUnicode)
											((PWCHAR)pNewMem)[cchCursor] = wcInsertChar;
										else
											((PCHAR)pNewMem)[cchCursor] = ansiInsertChar;
										memcpy((PCHAR)pNewMem+cbChar*(cchCursor+1), (PCHAR)pOldDataCopy+cbChar*cchCursor, cbChar*(cch-cchCursor+1));

										LocalUnlock(hTextMem);

										hResult = S_OK;
									}
								}


								HeapFree(GetProcessHeap(), 0, pOldDataCopy);
							}

						}

						if (SUCCEEDED(hResult))
						{
							pED->cch++;
							pED->ichCaret++;
							if (pED->ichMaxSel == pED->ichMinSel)
							{
								pED->ichMaxSel++;
								pED->ichMinSel++;
							}
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
HRESULT EditSetStr(HWND hWnd, PWCHAR wcPatchStr)
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
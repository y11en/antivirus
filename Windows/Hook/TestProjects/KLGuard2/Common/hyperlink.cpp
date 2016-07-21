#include "../stdafx.h"
#include "hyperlink.h"

#include <windowsx.h>
#include <debugging.h>

//+ ----------------------------------------------------------------------------------------
BOOL IsCursorInRect(HWND hwndDlg, LPRECT pRect)
{
	POINT sPoint;
	
	if(GetCursorPos(&sPoint))
	{
		ScreenToClient(hwndDlg, &sPoint);
		return PtInRect(pRect, sPoint);
	}
	
	return FALSE;
}

BOOL IsCursorInRect(HWND hwndDlg, LPRECT pRect, LPARAM lParam)
{
	POINT sPoint;
	sPoint.x = GET_X_LPARAM(lParam); 
	sPoint.y = GET_Y_LPARAM(lParam); 
	
	return PtInRect(pRect, sPoint);
}

//+ ----------------------------------------------------------------------------------------

CHyperLink* CreateHyperlinkControl(HWND hwndParent, int nControlID, int nControlTextID, HINSTANCE hLoc, HINSTANCE hModule, int nRectID, HFONT hFont)
{
	CHyperLink* pHL = new CHyperLink;
	if(!pHL)
		return 0;
	
	LPHYPERLINKINF pHLI = new HYPERLINKINF;
	ZeroMemory(pHLI, sizeof(CHyperLink));
	
	HWND hwndRect = GetDlgItem(hwndParent, nRectID);
	ASSERT(hwndRect != 0);
	
	GetWindowRect(hwndRect, &pHLI->ClientRect);
	
	ScreenToClient(hwndParent, (LPPOINT)&(pHLI->ClientRect));
	ScreenToClient(hwndParent, ((LPPOINT)&(pHLI->ClientRect)) + 1);
	
	pHLI->hInst = hLoc;
	pHLI->hModule = hModule;
	pHLI->hParent = hwndParent;
	
	pHLI->hFont = hFont;
	
	pHLI->crTextColor = RGB(0, 0, 175);
	pHLI->crTextColorHover = RGB(0, 0, 255);
	
	pHLI->dwStyle = WS_CHILD|WS_TABSTOP|WS_VISIBLE;
	
	if(pHL->Create(pHLI, nControlID))
	{
		TCHAR Buff[100];
		Buff[0] = 0;
		LoadString(hLoc, nControlTextID, Buff, 100);
		if(Buff[0] != 0)
		{
			SendMessage(pHL->m_hWnd, WM_SETTEXT, 0, (LPARAM)Buff);
			ShowWindow(pHL->m_hWnd, SW_SHOW);
			return pHL;
		}
	}
	
	delete pHL;
	return 0;
}

CHyperLink::CHyperLink()
{
	m_hWnd = 0;
	m_pszText = 0;
	m_dwState = STATE_MOUSE_LEAVE;
	m_pParam = 0;
	
	ZeroMemory(&m_sFocusedRect, sizeof(RECT));
	
	m_hPrevCursor = 0;
	
	m_dwBkColor = COLOR_DIALOG_BACKGR;
	
	m_nDrawTextFormat = DT_VCENTER|DT_WORDBREAK;
}

CHyperLink::~CHyperLink()
{
	if (m_pszText)
	{
		delete m_pszText;
		m_pszText = 0;
	}
	if(m_pParam)
	{
		if(m_pParam->hFont)
			DeleteObject(m_pParam->hFont);
		
		delete m_pParam;
		m_pParam = 0;
	}
	
}

ATOM CHyperLink::MyRegisterClass(LPHYPERLINKINF pHLI, PTCHAR pszWindowClass)
{
	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX); 
	
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= pHLI->hModule;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= 0;
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= pszWindowClass;
	wcex.hIconSm		= 0;
	
	return RegisterClassEx(&wcex);
}


BOOL CHyperLink::Create(LPHYPERLINKINF pInfo, int nCtrlID)
{
	MyRegisterClass(pInfo, (PTCHAR) g_cszHyperlinkClassName);
	
	
	if(m_pParam == 0)
		m_pParam = pInfo;
	
	m_hWnd = CreateWindow(g_cszHyperlinkClassName, 0, pInfo->dwStyle,
		pInfo->ClientRect.left, 
		pInfo->ClientRect.top, 
		pInfo->ClientRect.right - pInfo->ClientRect.left,
		pInfo->ClientRect.bottom - pInfo->ClientRect.top,
		pInfo->hParent, 0, pInfo->hModule, this);
	
	if(m_hWnd != 0 && IsWindow(m_hWnd))
	{
		SetWindowLong(m_hWnd, GWL_ID, nCtrlID);
		return TRUE;
	}
	
	return FALSE;
}


BOOL CALLBACK CHyperLink::WndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CHyperLink* pWnd = (CHyperLink*)GetWindowLong(hwndDlg, GWL_USERDATA);
	
	if(uMsg == nSetBkColor && lParam == TRUE)
	{
		if(pWnd)
		{
			pWnd->m_dwBkColor = wParam;
			return TRUE;
		}
		
		return FALSE;
	}
	
	switch (uMsg) 
	{
	case WM_ERASEBKGND:
		return TRUE;
	case WM_MOUSEMOVE:
		{
			if(pWnd)
			{
				DWORD dwState = pWnd->GetState();
				
				if((dwState&STATE_MOUSE_LBTN_DOWN) == STATE_MOUSE_LBTN_DOWN)
				{
					if(IsCursorInRect(hwndDlg, &pWnd->m_sFocusedRect, lParam))
					{
						pWnd->SetState(dwState|STATE_MOUSE_ENTER);
						SetTimer(hwndDlg, TIMER_CHECK_VIEW, 20, 0);
					}
					else
					{
						pWnd->RemoveState(STATE_MOUSE_ENTER);
					}
					
					
					if(dwState != pWnd->GetState())
						pWnd->RedrawText(hwndDlg);
				}
			}
			
		}
		return 0;
	case WM_KEYDOWN:
		{
			if(wParam == VK_SPACE)
			{
				if(pWnd)
					return pWnd->SendParentPress(hwndDlg);
			}
		}
		return TRUE;
	case WM_TIMER:
		{
			if(wParam == TIMER_CHECK_VIEW)
			{
				if(pWnd)
				{
					if(!IsCursorInRect(hwndDlg, &pWnd->m_sFocusedRect))
					{
						pWnd->RemoveState(STATE_MOUSE_ENTER);
						pWnd->RedrawText(hwndDlg);
						
						if((pWnd->GetState()&STATE_MOUSE_LBTN_DOWN) != STATE_MOUSE_LBTN_DOWN)
							if(pWnd->m_hPrevCursor)
								SetCursor(pWnd->m_hPrevCursor);
							
							KillTimer(hwndDlg, TIMER_CHECK_VIEW);
					}
				}
			}
		}
		return TRUE;
	case WM_LBUTTONDOWN:
		{
			SetFocus(hwndDlg);
			SetCapture(hwndDlg);
			
			if(pWnd)
			{
				pWnd->SetState(pWnd->GetState()|STATE_MOUSE_LBTN_DOWN);
				pWnd->RedrawText(hwndDlg);
			}
		}
		return 0;
	case WM_LBUTTONUP:
		{
			BOOL bReleased = FALSE;
			
			if(pWnd )
			{
				if(IsCursorInRect(hwndDlg, &pWnd->m_sFocusedRect))
					if((pWnd->GetState()&STATE_MOUSE_LBTN_DOWN) == STATE_MOUSE_LBTN_DOWN && (pWnd->GetState()&STATE_MOUSE_ENTER) == STATE_MOUSE_ENTER)
					{
						bReleased = TRUE;
						ReleaseCapture();
						pWnd->SendParentPress(hwndDlg);
					}
					
					pWnd->RemoveState(STATE_MOUSE_LBTN_DOWN);
					
			}
			
			if(!bReleased)
				ReleaseCapture();
			
		}
		return 0;
	case WM_CAPTURECHANGED:
		{
			pWnd->RemoveState(STATE_MOUSE_LBTN_DOWN);
			ReleaseCapture();
			
		}
		return TRUE;
	case WM_SETCURSOR:
		{
			if(pWnd)
			{
				POINT sPoint;
				if(GetCursorPos(&sPoint) && ScreenToClient(hwndDlg, &sPoint))
					if(PtInRect(&pWnd->m_sFocusedRect, sPoint))
					{
						if((pWnd->GetState()&STATE_MOUSE_ENTER) != STATE_MOUSE_ENTER)
						{
							SetTimer(hwndDlg, TIMER_CHECK_VIEW, 20, 0);
							pWnd->SetState(pWnd->GetState()|STATE_MOUSE_ENTER);
							
							pWnd->RedrawText(hwndDlg);
						}
						
						// если курсор будет моргать то его нужно прописать в классе окна/ and don't forget set previous cursor on kill focus/
						HCURSOR hPrevCursor = 0;
						HCURSOR hCur = LoadCursor(0, MAKEINTRESOURCE(32649));
						if(hCur)
							hPrevCursor = SetCursor(hCur);
						
						if(hPrevCursor != hCur)
							pWnd->m_hPrevCursor = hPrevCursor;
						
					}
					
					
					
			}
			
		}
		return TRUE;
	case WM_SETFOCUS:
		{
			if(pWnd)
			{
				pWnd->SetState(pWnd->GetState()|STATE_KEYBOARD_FOCUS);
				pWnd->RedrawText(hwndDlg);
			}
			
		}
		return TRUE;
	case WM_KILLFOCUS:
		{
			if(pWnd)
			{
				pWnd->RemoveState(STATE_KEYBOARD_FOCUS|STATE_MOUSE_LBTN_DOWN);
				pWnd->RedrawText(hwndDlg);
			}
		}
		return TRUE;
	case WM_SETTEXT:
		{
			if(pWnd)
			{
				pWnd->SetText(lParam);
			}
		}
		return TRUE;
	case WM_GETTEXT:
		{
			if(pWnd)
			{
				int nTextSize = pWnd->GetText((PTCHAR) lParam, (int)wParam);
				SetWindowLong(hwndDlg, DWL_MSGRESULT, nTextSize);
			}
		}
		return TRUE;
	case WM_DESTROY:
		{
			if(pWnd)
			{
				delete pWnd;
			}
		}
		return DefWindowProc(hwndDlg, uMsg, wParam, lParam);
	case WM_CREATE:
		{
			LPCREATESTRUCT pCS = (LPCREATESTRUCT)lParam;
			if(pCS)
			{
				if(hwndDlg && IsWindow(hwndDlg))
					SetWindowLong(hwndDlg, GWL_USERDATA, (long)pCS->lpCreateParams);
				
				CHyperLink* pWnd = (CHyperLink*)pCS->lpCreateParams;
				if(pWnd)
				{
					if(pWnd->m_hWnd == 0)
						pWnd->m_hWnd = hwndDlg;
				}
			}
		}
		return TRUE; 
	case WM_PAINT:
		{
			DefWindowProc(hwndDlg, uMsg, wParam, lParam);
			HDC hDC = GetDC(hwndDlg);
			if(hDC)
			{
				RECT sRect;
				GetClientRect(hwndDlg, &sRect);
				if(pWnd)
				{
					pWnd->DrawItem(hDC, &sRect);
				}
				
				ReleaseDC(hwndDlg, hDC);
			}
		}
		return TRUE;
	default:
		return DefWindowProc(hwndDlg, uMsg, wParam, lParam);
	} 
	
	
	return FALSE;
}


BOOL CHyperLink::SetText(LPARAM lParam)
{
	if(m_pszText != 0)
	{
		delete m_pszText;
		m_pszText = 0;
	}
	
	m_pszText = _wcsdup((PTCHAR) lParam);
	
	return (BOOL)m_pszText;
}

int CHyperLink::GetText(PTCHAR szCopyTo, int nSize)
{
	if(m_pszText == 0)
		return 0;
	
	int nTextSize = lstrlenW(m_pszText) + sizeof(TCHAR);
	
	if(szCopyTo == 0 || nSize < nTextSize)
		return nTextSize;
	
	if(nSize < nTextSize)
	{
		szCopyTo[0] = 0;
		return 0;
	}
	
	lstrcpy(szCopyTo, m_pszText);
	
	return nTextSize;
}

int CHyperLink::DrawText(HDC hDC, LPRECT lpRect)
{
	if(m_pszText)
	{
		int nBuffSize = lstrlen(m_pszText);
		if(nBuffSize)
		{
			int nBkMode = SetBkMode(hDC,TRANSPARENT);
			
			HWND hParent = 0;
			if(m_hWnd)
				FillRect(hDC,&m_sFocusedRect,GetSysColorBrush(m_dwBkColor));
			
			if((GetState()&STATE_KEYBOARD_FOCUS) == STATE_KEYBOARD_FOCUS)
			{
				DrawFocusRect(hDC,lpRect);
			}
			
			// текст пишется не от нуля потму что от нуля рисуется фокус рект.
			lpRect->left += 1;
			lpRect->top += 1;
			
			COLORREF Color = 0;
			if(m_pParam)
			{
				if((m_dwState&STATE_MOUSE_ENTER) == STATE_MOUSE_ENTER)
				{
					if(m_pParam->crTextColorHover)
						Color =	SetTextColor(hDC, m_pParam->crTextColorHover );
				}
				else
					if(m_pParam->crTextColor)
						Color =	SetTextColor(hDC, m_pParam->crTextColor);
			}
			
			// отрисовка текста
			::DrawText(hDC,m_pszText,nBuffSize,lpRect,m_nDrawTextFormat);
			
			if(Color)
				SetTextColor(hDC, Color);
			
			if(nBkMode)// restore BK mode
				SetBkMode(hDC,nBkMode);
		}
	}
	
	return TRUE;
}

int CHyperLink::DrawItem(HDC hDC, LPRECT lpRect)
{
	HFONT hPrevFont = 0;
	
	if(m_pParam && m_pParam->hFont == 0)
	{
		if(m_hWnd)
		{
			HWND hParent = GetParent(m_hWnd);
			if(hParent)
				m_pParam->hFont = GetWindowFont(hParent);
		}
		
		if(m_pParam->hFont)
		{
			LOGFONT sLog;
			if(GetObject(m_pParam->hFont,sizeof(LOGFONT),(LPVOID)&sLog))
			{
				sLog.lfUnderline = TRUE;
				m_pParam->hFont = ::CreateFontIndirect(&sLog);
			}
		}
		else
		{
			NONCLIENTMETRICS ncm;
			memset(&ncm, 0, sizeof(ncm));
			ncm.cbSize = sizeof(ncm);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
			
			ncm.lfMessageFont.lfUnderline = TRUE;
			
			m_pParam->hFont = ::CreateFontIndirect(&ncm.lfMessageFont);
		}
		
	}
	
	// select proper font
	if(m_pParam->hFont)
		hPrevFont = (HFONT)SelectObject(hDC,m_pParam->hFont);
	
	// set background transparent
	
	int nSizeBuff = 0;
	
	nSizeBuff = GetText(0, 0);
	if(nSizeBuff)
	{
		PTCHAR pBuff = new TCHAR[nSizeBuff + sizeof(TCHAR)];
		if (pBuff != 0)
		{
			if(GetText(pBuff,nSizeBuff))
			{
				// поправим lpRect чтобы рамка фокуса была чуть больше текста
				memcpy(&m_sFocusedRect,lpRect,sizeof(RECT));
				
				// получим размер текста
				::DrawText(hDC,pBuff, nSizeBuff - 1,&m_sFocusedRect, m_nDrawTextFormat | DT_CALCRECT);
				
				// center text vertically
				long mid = 0;
				//int nStrFix = 4;
				
				if(lpRect->bottom && lpRect->bottom > m_sFocusedRect.bottom )//&& (lpRect->bottom > m_sFocusedRect.bottom*2)
				{
					mid = (long)(lpRect->bottom/2) - (long)(m_sFocusedRect.bottom/2);
					mid -= 2;
					if(mid < 0)
						mid = 0;
				}
				
				m_sFocusedRect.top += mid;
				m_sFocusedRect.right+=2;
				m_sFocusedRect.bottom+=2+mid;
				
				RECT sRect;
				memcpy(&sRect,&m_sFocusedRect,sizeof(RECT));
				DrawText(hDC,&sRect);
			}
			delete pBuff;
		}
	}
	
	if(hPrevFont)
		SelectObject(hDC,hPrevFont);
	
	return TRUE;
}


void CHyperLink::SetState(DWORD dwState)
{
	m_dwState = dwState;
}

DWORD CHyperLink::GetState()
{
	return m_dwState;
}

void CHyperLink::RemoveState(DWORD dwState)
{
	m_dwState &= ~dwState;
}

int CHyperLink::SendParentPress(HWND hwndDlg)
{
	HWND hParent = GetParent(hwndDlg);
	if(hParent)
	{
		SendMessage(hParent, nHyperlinkPressed, GetWindowLong(hwndDlg,GWL_ID),0);
		return 0;
	}
	
	return TRUE;
}

void CHyperLink::RedrawText(HWND hwndDlg)
{
	HDC hDC = GetDC(hwndDlg);
	if(hDC)
	{
		HFONT hPrevFont;
		if(m_pParam->hFont)
			hPrevFont = (HFONT)SelectObject(hDC,m_pParam->hFont);
		
		RECT sRect;
		memcpy(&sRect,&m_sFocusedRect,sizeof(RECT));
		DrawText(hDC,&sRect);
		
		if(hPrevFont)
			SelectObject(hDC,hPrevFont);
		
		ReleaseDC(hwndDlg,hDC);
	}
}

void CHyperLink::SetDrawTestFormat(UINT nDrawTextFormat)
{
	m_nDrawTextFormat = nDrawTextFormat;
}
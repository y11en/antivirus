// RichText.h: interface for the CRichText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RICHTEXT_H__BC51764A_D16B_4262_9E6E_BBB826768BD8__INCLUDED_)
#define AFX_RICHTEXT_H__BC51764A_D16B_4262_9E6E_BBB826768BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
-------Begining-----------------------
1) Создать в темплейте диалога RichEdit контрол
2) Настроить нужные проперти и в ресурсном файле заменить название класса окна на RICHEDIT20A
Было:
CONTROL         "",IDC_RICHEDIT1,"RICHEDIT",ES_AUTOHSCROLL | WS_BORDER | 
WS_TABSTOP, ...
Должно быть:	
CONTROL         "",IDC_RICHEDIT1,"RICHEDIT20A",ES_AUTOHSCROLL | WS_BORDER | 
WS_TABSTOP, ...

  3) Использовать функцию SetHyperText для установки текста
  
	-------Формат записи текста-----------
	[regular text] [<r|b|l[:color][,id]; formated text>]
	
	  форматирование заключается в угловые кавычки '<' '>'
	  [] - необязательные параметры
	  r - обычный текст
	  b - bold
	  l - link
	  
		-------------------Added 11.07.2003-----------------
		
		  Теперь в параметре color можно описывать сжатие пути: p45, p - path и без пробелов - до какого размера жать(в символах)
			
			  -------------------End 11.07.2003-------------------
			  
				
				  --------Получение нотификации от линков-------
				  
					В функцию обработки сообщений, вставить обработчик WM_NOTIFY
					  
						BOOL CALLBACK DlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
						{ 
						
						  switch (message) 
						  { 
						  case WM_NOTIFY:
						  
							if ( IsLinkClicked ( hwndDlg, IDC_RICHEDIT1, 2, wParam, lParam ) )
							MessageBox( hwndDlg, "Link clicked", "Click", MB_OK );
							
*/


/*---------------------------PUBLIC PART------------------------------------

  
	void CRichText::SetHyperTextFormat  ( HWND hwndDlg, DWORD dwCtrlID, DWORD dwTextHigh, LPCTSTR szFontName );
	
	  bool CRichText::IsLinkClicked ( HWND hwndDlg, DWORD dwCtrlID, DWORD lnkID, WPARAM, LPARAM lParam );
	  
		bool CRichText::SetHyperText( HWND hwndDlg, DWORD dwCtrlID, DWORD dwTextID );
		
		  bool CRichText::SetHyperText( HWND hWnd, LPCTSTR szText );
		  
-----------------------------------------------------------------------------*/

//-----------------------------PRIVATE PART---------------------------------
#include "defs.h"

#define LINK_NOT_PRESSED			-1
#define __DBL_BRACKET(szText)		(CRichText::dbl_bracket_buffer(static_cast<LPCTSTR>(szText)).get())

extern COMMONEXPORT UINT UM_CANCANCELNOW;


class COMMONEXPORT CRichText  
{
public:
	struct dbl_bracket_buffer
	{
		explicit dbl_bracket_buffer(LPCTSTR szText) : m_pStr(_tcslen(szText) + 1)
		{
			if (!szText)
				return;

			_tcsncpy ( &m_pStr[0], szText, m_pStr.size  () );
			
			for (std::vector<TCHAR>::iterator i = m_pStr.begin  (); i != m_pStr.end  ();)
			{
				if ( (*i == _T('<'))  || (*i == _T('>')) )
				{
					i = m_pStr.insert  (i, *i);
					++i;
				}
				++i;
			}
		}

		LPCTSTR get(){return (LPCTSTR)&m_pStr[0];}

	protected:
		std::vector<TCHAR> m_pStr;

	};
	

	static void SetHyperTextFormat  ( HWND hCtrlWnd, DWORD dwTextHigh, LPCTSTR szFontName );
	static DWORD GetLinkID ( HWND hCtrlWnd, WPARAM, LPARAM lParam );
	static DWORD GetOnBtnDownLinkID ( HWND hCtrlWnd, WPARAM, LPARAM lParam );
	static bool SetHyperText( HWND hWnd, LPCTSTR szText );
	static CString GetPlainFromHyper( LPCTSTR szHyperText );
	
	static void SetDragMode(HWND hWnd, bool bDrag);
	static void Print2DC(HWND hWnd, CDC* pDC, CRect rect);
	

	CRichText ();
	virtual ~CRichText ();
};

#endif // !defined(AFX_RICHTEXT_H__BC51764A_D16B_4262_9E6E_BBB826768BD8__INCLUDED_)

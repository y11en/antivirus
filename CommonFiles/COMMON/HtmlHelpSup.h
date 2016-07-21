//==================================================================================
//  Макросы для поддержки HTML Help в приложение MFC                                 
//
//==================================================================================
// Last changes: 12.10.2000

#ifndef __HTMLHELPSUP_H
#define __HTMLHELPSUP_H

#include <htmlhelp.h>

//******************************************************************************
// 
//******************************************************************************
class CCAppHtmlHelpSupport : public CWinApp
{
public:
     CCAppHtmlHelpSupport ()
     :    m_pszContextHelpFilePath ( NULL )
     {
     }

     ~CCAppHtmlHelpSupport ()
     {
          free ( ( void * ) m_pszContextHelpFilePath );
     }
          
     void InitHtmlHelp ( LPCTSTR szMainHelpFile = NULL, LPCTSTR szContextHelpFile = NULL, bool bHtmlHelp = true )
     {
          m_bHtmlHelp = bHtmlHelp;
          
          free ( ( void * ) m_pszHelpFilePath );
          m_pszHelpFilePath = NULL;

          if   ( szMainHelpFile )
          {
               m_pszHelpFilePath = _tcsdup ( szMainHelpFile );
          }
          if   ( szContextHelpFile )
          {
               m_pszContextHelpFilePath = _tcsdup ( szContextHelpFile );
          }
          if   ( szMainHelpFile == NULL || szContextHelpFile == NULL )
          {
               TCHAR szBuff [ _MAX_PATH ];
               VERIFY ( ::GetModuleFileName ( m_hInstance, szBuff, _MAX_PATH ) );
               LPTSTR lpszExt = _tcsrchr ( szBuff, '.' );
               *lpszExt = 0;
               
               if   ( m_pszHelpFilePath == NULL )
               {
                    _tcscpy ( lpszExt, ( m_bHtmlHelp ? _T(".CHM") : _T(".HLP") ) );
                    m_pszHelpFilePath = _tcsdup ( szBuff );
                    *lpszExt = '\0';
               }
               if   ( m_pszContextHelpFilePath == NULL )
               {
                    _tcscpy ( lpszExt, _T(".HLP") );
                    m_pszContextHelpFilePath = _tcsdup ( szBuff );
               }
          }
     }

     void DestroyHtmlHelp ()
     {
          free ( ( void * ) m_pszContextHelpFilePath );
     }

     LPCTSTR m_pszContextHelpFilePath;
     bool m_bHtmlHelp;
};

//------------------------------------------------------------------------------
// Объявление использования HtmlHelp в окне
//------------------------------------------------------------------------------
#define DECLARE_USE_WND_HTMLHELP()                                              \
     virtual void HtmlHelp ( DWORD dwData, UINT nCmd = HH_HELP_CONTEXT, bool bIsContext = false );       \
     void WinHelp ( DWORD dwData, UINT nCmd = HELP_CONTEXT, bool bIsContext = false );                   \
     afx_msg BOOL OnHelpInfo ( HELPINFO * );                                    \
     void OnHelpCmd ( HELPINFO *);												\
     void ShowDefContextHelp ();                                                \
     virtual DWORD __GetDefaultHelpContextID () const;                          \

//------------------------------------------------------------------------------
// Объявление для объявления использования HtmlHelp в порожденном классе
//------------------------------------------------------------------------------
#define DECLARE_USE_DERIVED_WND_HTMLHELP()                                      \
     virtual DWORD __GetDefaultHelpContextID () const;                                  \

//------------------------------------------------------------------------------
// Определение HelpID по умолчанию при нажатии F1
//------------------------------------------------------------------------------
#define DEFINE_DEF_HELPCONTEXT(_theWndClass,id)                                 \
DWORD _theWndClass::__GetDefaultHelpContextID () const { return id; }

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
#define DEFINE_DEF_HELPCONTEXT_FUNC(_theWndClass,Func)                          \
DWORD _theWndClass::__GetDefaultHelpContextID () const { return Func () + HIDC_CONST_DIAL; }

//------------------------------------------------------------------------------
// Определение нулевого HelpID по умолчанию
//------------------------------------------------------------------------------
#define DEFINE_EMPTY_DEF_HELPCONTEXT(_theWndClass)                              \
DWORD _theWndClass::__GetDefaultHelpContextID () const { return 0; }

//------------------------------------------------------------------------------
// Добавление в карту сообщений поддержки для HTMLHelp
//------------------------------------------------------------------------------
#define HTMLHELP_MSGS()                                                         \
     ON_WM_HELPINFO()                                                           \
     /*ON_WM_CONTEXTMENU()*/

#ifdef _DEBUG
#define SHOWS_HELP_FAIL() AfxMessageBox ( AFX_IDP_FAILED_TO_LAUNCH_HELP );
#else
#define SHOWS_HELP_FAIL() {}
#endif

//------------------------------------------------------------------------------
// Реализация всех функций, кроме OnHelpCmd
//------------------------------------------------------------------------------
#define IMPL_NO_ONHELPCMD(_theWndClass)                            \
void _theWndClass::HtmlHelp ( DWORD dwData, UINT nCmd, bool bIsContext )     \
{                                                                               \
     CCAppHtmlHelpSupport * pApp = ( CCAppHtmlHelpSupport * ) AfxGetApp ();     \
                                                                                \
	 LPCSTR szHelpFile = bIsContext ? pApp -> m_pszContextHelpFilePath : pApp -> m_pszHelpFilePath; \
                                                                                \
     ASSERT ( szHelpFile != NULL );												\
     CWaitCursor wait;                                                          \
                                                                                \
     if   ( IsFrameWnd () )                                                     \
     {                                                                          \
          CFrameWnd * pFrameWnd = ( CFrameWnd * ) this;                         \
          pFrameWnd -> ExitHelpMode ();                                         \
     }                                                                          \
     SendMessage ( WM_CANCELMODE );                                             \
     SendMessageToDescendants ( WM_CANCELMODE, 0, 0, TRUE, TRUE );              \
     CWnd * pWnd = GetTopLevelParent ();                                        \
     pWnd -> SendMessage ( WM_CANCELMODE );                                     \
     pWnd -> SendMessageToDescendants ( WM_CANCELMODE, 0, 0, TRUE, TRUE );      \
     HWND hWndCapture = ::GetCapture ();                                        \
     if   ( hWndCapture != NULL )                                               \
     {                                                                          \
          ::SendMessage ( hWndCapture, WM_CANCELMODE, 0, 0 );                   \
     }                                                                          \
     TRACE3 ( "HtmlHelp: pszHelpFile = '%s', dwData: $%lx, fuCommand: %d.\n",   \
          szHelpFile, dwData, nCmd );                            \
																				\
	 if ( m_hWnd && IsWindow(m_hWnd))											\
	 {																			\
		SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); \
	 }																			\
					                                                            \
     if   ( !::HtmlHelp ( /*pWnd -> m_hWnd*/NULL, szHelpFile, nCmd, dwData ) )  \
     {                                                                          \
		if ( !::HtmlHelp ( NULL, szHelpFile, HH_HELP_FINDER, 0 ) )	\
		{																		\
          SHOWS_HELP_FAIL ()                                                    \
		}                                                                       \
	}                                                                           \
}																				\
                                                                                \
void _theWndClass::WinHelp ( DWORD dwData, UINT nCmd, bool bIsContext )         \
{                                                                               \
     CCAppHtmlHelpSupport * pApp = ( CCAppHtmlHelpSupport * ) AfxGetApp ();     \
                                                                                \
     if   ( pApp -> m_bHtmlHelp )                                               \
     {                                                                          \
          const static DWORD hcMap [ 5 ][ 2 ] = {                               \
               { HELP_CONTENTS, HH_DISPLAY_TOC },                               \
               { HELP_CONTEXT, HH_HELP_CONTEXT },                               \
               { HELP_KEY, HH_DISPLAY_INDEX },                                  \
               { HELP_PARTIALKEY, HH_DISPLAY_INDEX },                           \
               { HELP_FINDER, HH_HELP_FINDER } };                               \
                                                                                \
          for  ( int i = 0; i < 5; ++i )                                        \
          {                                                                     \
               if   ( hcMap [ i ][ 0 ] == nCmd )                                \
               {                                                                \
                    HtmlHelp ( dwData, hcMap [ i ][ 1 ], bIsContext );          \
                    return;                                                     \
               }                                                                \
          }                                                                     \
     }                                                                          \
     else                                                                       \
     {                                                                          \
          CWnd::WinHelp ( dwData, nCmd );                                       \
     }                                                                          \
}                                                                               \
                                                                                \
BOOL _theWndClass::OnHelpInfo ( HELPINFO * pHelpInfo )                          \
{                                                                               \
     OnHelpCmd ( pHelpInfo );													\
     return TRUE;                                                               \
}                                                                               \
                                                                                \
void _theWndClass::ShowDefContextHelp ()                                        \
{                                                                               \
     if   ( __GetDefaultHelpContextID () )                                      \
     {                                                                          \
          WinHelp ( __GetDefaultHelpContextID (), HELP_CONTEXT, true );			\
     }                                                                          \
}                                                                               \
                                                                                \
//------------------------------------------------------------------------------
// Стандартный обработчик OnHelpCmd
//------------------------------------------------------------------------------
#define IMPL_STD_ONHELPCMD(_theWndClass)                                        \
void _theWndClass::OnHelpCmd ( HELPINFO * pHelpInfo )				            \
{                                                                               \
     ShowDefContextHelp ();														\
}                                                                               \

//------------------------------------------------------------------------------
// Реализация HtmlHelp в окне по умолчанию
//------------------------------------------------------------------------------
#define IMPL_USE_WND_HTMLHELP(_theWndClass)                                     \
     IMPL_NO_ONHELPCMD(_theWndClass)                                            \
     IMPL_STD_ONHELPCMD(_theWndClass)

#endif // __HTMLHELPSUP_H

//==================================================================================

//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_XPTHEMEHELPER_H
#define __COMMON_XPTHEMEHELPER_H

#include "defs.h"
#include "uxtheme.h"
#include "TmSchema.h"

//******************************************************************************
// 
//******************************************************************************
class COMMONEXPORT CCXPThemeHelper
{
public:
	CCXPThemeHelper ();
	~CCXPThemeHelper ();
	
	static bool IsCurrentThemed();


	static BOOL GetUseXPStyle ()
	{
		return m_bUseXPStyle;
	}

	static BOOL SetUseXPStyle ( BOOL bNewValue )
	{
		BOOL bRetVal = m_bUseXPStyle;
		m_bUseXPStyle = bNewValue;
		return bRetVal;
	}

	static BOOL CanApplyXPStyle ()
	{
		return m_bCanApplyXPStyle && m_IsThemeActive ();
	}

	static BOOL __TrackMouseEvent ( LPTRACKMOUSEEVENT lpEventTrack )
	{
		return ( m_TrackMouseEvent && m_bUseXPStyle ) ? m_TrackMouseEvent ( lpEventTrack ) : FALSE;
	}

	static HTHEME OpenThemeData ( HWND hwnd, LPCWSTR pszClassList )
	{
		return ( m_OpenThemeData && m_bUseXPStyle ) ? m_OpenThemeData ( hwnd, pszClassList ) : NULL;
	}

     static HRESULT CloseThemeData ( HTHEME hTheme )
	{
		return ( m_CloseThemeData && m_bUseXPStyle ) ? m_CloseThemeData ( hTheme ) : E_FAIL;
	}
	
     static BOOL IsAppThemed ()
	{
		return ( m_IsAppThemed && m_bUseXPStyle ) ? m_IsAppThemed () : FALSE;
	}
	
     static HRESULT EnableThemeDialogTexture ( HWND hWnd, DWORD dwFlags )
	{
		return ( m_EnableThemeDialogTexture && m_bUseXPStyle ) ? m_EnableThemeDialogTexture ( hWnd, dwFlags ) : E_FAIL;
	}
	
     static HRESULT DrawThemeBackground ( HTHEME hTheme, HDC hdc, 
		int iPartId, int iStateId, const RECT * pRect, const RECT * pClipRect )
	{
		return ( m_DrawThemeBackground && m_bUseXPStyle ) ?
			m_DrawThemeBackground ( hTheme, hdc, iPartId, iStateId, pRect, pClipRect ) : E_FAIL;
	}
	
     static HRESULT GetThemeBackgroundContentRect ( HTHEME hTheme, HDC hdc,
		int iPartId, int iStateId, const RECT * pBoundingRect, RECT * pContentRect )
	{
		return ( m_DrawThemeBackground && m_bUseXPStyle ) ?
			m_GetThemeBackgroundContentRect ( hTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect ) : E_FAIL;
	}
	
     static HRESULT GetThemeBackgroundExtent ( HTHEME hTheme, HDC hdc,
		int iPartId, int iStateId, const RECT * pBoundingRect, RECT * pExtentRect )
	{
		return ( m_DrawThemeBackground && m_bUseXPStyle ) ?
			m_GetThemeBackgroundExtent ( hTheme, hdc, iPartId, iStateId, pBoundingRect, pExtentRect ) : E_FAIL;
	}
	
     static HRESULT DrawThemeText ( HTHEME hTheme, HDC hdc,
		int iPartId, int iStateId, LPCWSTR pszText, int iCharCount,
		DWORD dwTextFlags, DWORD dwTextFlags2, const RECT * pRect )
	{
		return ( m_DrawThemeText && m_bUseXPStyle ) ?
			m_DrawThemeText ( hTheme, hdc, iPartId, iStateId, pszText, iCharCount,
			dwTextFlags, dwTextFlags2, pRect ) : E_FAIL;
	}
	
     static HRESULT DrawThemeIcon ( HTHEME hTheme, HDC hdc,
          int iPartId, int iStateId, const RECT * pRect, HIMAGELIST himl, int iImageIndex )
     {
          return ( m_DrawThemeIcon && m_bUseXPStyle ) ?
               m_DrawThemeIcon ( hTheme, hdc, iPartId, iStateId, pRect, himl, iImageIndex ) : E_FAIL;
     }
     
     static HRESULT DrawThemeEdge ( HTHEME hTheme, HDC hdc,
          int iPartId, int iStateId, const RECT * pRect, UINT uEdge, UINT uFlags, RECT * pContentRect )
     {
          return ( m_DrawThemeEdge && m_bUseXPStyle ) ?
               m_DrawThemeEdge ( hTheme, hdc, iPartId, iStateId, pRect, uEdge, uFlags, pContentRect ) : E_FAIL;
     }
     
     static DWORD GetThemeAppProperties ()
	{
		return ( m_GetThemeAppProperties && m_bUseXPStyle ) ? m_GetThemeAppProperties () : 0;
	}
	
     static BOOL IsThemeActive ()
	{
		return ( m_IsThemeActive && m_bUseXPStyle ) ? m_IsThemeActive () : 0;
	}

     static BOOL GetThemeTextExtent ( HTHEME hTheme, HDC hdc,
          int iPartId, int iStateId, LPCWSTR pszText, int iCharCount,
          DWORD TextFlags, const RECT * pBoundingRect, RECT * pExtentRect )
     {
          if   ( m_GetThemeTextExtent && m_bUseXPStyle )
          {
               return m_GetThemeTextExtent ( hTheme, hdc, iPartId, iStateId, pszText, iCharCount,
                    TextFlags, pBoundingRect, pExtentRect );
          }
          return FALSE;
     }

     static HRESULT DrawThemeParentBackground ( HWND hwnd, HDC hdc, RECT * prc )
     {
          return ( m_DrawThemeEdge && m_bUseXPStyle ) ? m_DrawThemeParentBackground ( hwnd, hdc, prc ) : E_FAIL;
     }
     
private:

	static int m_iCount;

	static BOOL m_bCanApplyXPStyle;			// возможно ли применение XP стиля
	static BOOL m_bUseXPStyle;

     static HMODULE m_hThemeLib;
	static HMODULE m_hUser32Lib;

	typedef BOOL ( WINAPI *fnTrackMouseEvent ) ( LPTRACKMOUSEEVENT lpEventTrack );
	static fnTrackMouseEvent m_TrackMouseEvent;

	typedef HTHEME ( WINAPI * fnOpenThemeData ) ( HWND hwnd, LPCWSTR pszClassList );
	static fnOpenThemeData m_OpenThemeData;
	
     typedef HRESULT ( WINAPI * fnCloseThemeData ) ( HTHEME hTheme );
	static fnCloseThemeData m_CloseThemeData;
	
     typedef BOOL ( WINAPI * fnIsAppThemed ) ();
	static fnIsAppThemed m_IsAppThemed;
	
     typedef HRESULT ( WINAPI * fnEnableThemeDialogTexture ) ( HWND hWnd, DWORD dwFlags );
	static fnEnableThemeDialogTexture m_EnableThemeDialogTexture;
	
     typedef HRESULT ( WINAPI * fnDrawThemeBackground ) ( HTHEME hTheme, HDC hdc, 
		int iPartId, int iStateId, const RECT * pRect, const RECT * pClipRect );
	static fnDrawThemeBackground m_DrawThemeBackground;
	
     typedef HRESULT ( WINAPI * fnGetThemeBackgroundContentRect ) ( HTHEME hTheme, HDC hdc,
		int iPartId, int iStateId, const RECT * pBoundingRect, RECT * pContentRect );
	static fnGetThemeBackgroundContentRect m_GetThemeBackgroundContentRect;
	
     typedef HRESULT ( WINAPI * fnGetThemeBackgroundExtent ) ( HTHEME hTheme, HDC hdc,
		int iPartId, int iStateId, const RECT * pContentRect, RECT * pExtentRect );
	static fnGetThemeBackgroundExtent m_GetThemeBackgroundExtent;
	
     typedef HRESULT ( WINAPI * fnDrawThemeText ) ( HTHEME hTheme, HDC hdc,
		int iPartId, int iStateId, LPCWSTR pszText, int iCharCount,
		DWORD dwTextFlags, DWORD dwTextFlags2, const RECT * pRect );
	static fnDrawThemeText m_DrawThemeText;

     typedef HRESULT ( WINAPI * fnDrawThemeIcon ) ( HTHEME hTheme, HDC hdc,
          int iPartId, int iStateId, const RECT * pRect, HIMAGELIST himl, int iImageIndex );
     static fnDrawThemeIcon m_DrawThemeIcon;
     
     typedef HRESULT ( WINAPI * fnDrawThemeEdge ) ( HTHEME hTheme, HDC hdc,
          int iPartId, int iStateId, const RECT * pRect, UINT uEdge, UINT uFlags, RECT * pContentRect );
     static fnDrawThemeEdge m_DrawThemeEdge;
     
     typedef DWORD ( WINAPI * fnGetThemeAppProperties ) ();
	static fnGetThemeAppProperties m_GetThemeAppProperties;

     typedef BOOL ( WINAPI * fnIsThemeActive ) ();
     static fnIsThemeActive m_IsThemeActive;
     
     typedef BOOL ( WINAPI * fnGetThemeTextExtent ) ( HTHEME hTheme, HDC hdc,
          int iPartId, int iStateId, LPCWSTR pszText, int iCharCount,
          DWORD TextFlags, const RECT * pBoundingRect, RECT * pExtentRect );
     static fnGetThemeTextExtent m_GetThemeTextExtent;

     typedef HRESULT ( WINAPI * fnDrawThemeParentBackground ) ( HWND hwnd, HDC hdc, RECT * prc );
     static fnDrawThemeParentBackground m_DrawThemeParentBackground;
};

#endif // __COMMON_XPTHEMEHELPER_H

//==============================================================================

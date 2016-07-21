//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#include "stdafx.h"
#include "XPThemeHelper.h"

int CCXPThemeHelper::m_iCount = 0;

BOOL CCXPThemeHelper::m_bCanApplyXPStyle = FALSE;
BOOL CCXPThemeHelper::m_bUseXPStyle = TRUE;
HMODULE CCXPThemeHelper::m_hThemeLib = NULL;
HMODULE CCXPThemeHelper::m_hUser32Lib = NULL;

CCXPThemeHelper::fnTrackMouseEvent CCXPThemeHelper::m_TrackMouseEvent;
CCXPThemeHelper::fnOpenThemeData CCXPThemeHelper::m_OpenThemeData;
CCXPThemeHelper::fnCloseThemeData CCXPThemeHelper::m_CloseThemeData;
CCXPThemeHelper::fnIsAppThemed CCXPThemeHelper::m_IsAppThemed;
CCXPThemeHelper::fnEnableThemeDialogTexture CCXPThemeHelper::m_EnableThemeDialogTexture;
CCXPThemeHelper::fnDrawThemeBackground CCXPThemeHelper::m_DrawThemeBackground;
CCXPThemeHelper::fnGetThemeBackgroundContentRect CCXPThemeHelper::m_GetThemeBackgroundContentRect;
CCXPThemeHelper::fnGetThemeBackgroundExtent CCXPThemeHelper::m_GetThemeBackgroundExtent;
CCXPThemeHelper::fnDrawThemeText CCXPThemeHelper::m_DrawThemeText;
CCXPThemeHelper::fnDrawThemeIcon CCXPThemeHelper::m_DrawThemeIcon;
CCXPThemeHelper::fnDrawThemeEdge CCXPThemeHelper::m_DrawThemeEdge;
CCXPThemeHelper::fnGetThemeAppProperties CCXPThemeHelper::m_GetThemeAppProperties;
CCXPThemeHelper::fnIsThemeActive CCXPThemeHelper::m_IsThemeActive;
CCXPThemeHelper::fnGetThemeTextExtent CCXPThemeHelper::m_GetThemeTextExtent;
CCXPThemeHelper::fnDrawThemeParentBackground CCXPThemeHelper::m_DrawThemeParentBackground;

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CCXPThemeHelper::CCXPThemeHelper ()
{
	if   ( m_iCount++ == 0 )
	{
		m_hThemeLib = LoadLibrary ( "uxtheme.dll" );

          if   ( m_hThemeLib )
		{
			m_IsAppThemed = ( fnIsAppThemed ) GetProcAddress ( m_hThemeLib, "IsAppThemed" );
			m_OpenThemeData = ( fnOpenThemeData ) GetProcAddress ( m_hThemeLib, "OpenThemeData" );
			m_CloseThemeData = ( fnCloseThemeData ) GetProcAddress ( m_hThemeLib, "CloseThemeData" );
			m_DrawThemeBackground = ( fnDrawThemeBackground ) GetProcAddress ( m_hThemeLib, "DrawThemeBackground" );
			m_EnableThemeDialogTexture = 
					( fnEnableThemeDialogTexture ) GetProcAddress ( m_hThemeLib, "EnableThemeDialogTexture" );
			m_GetThemeBackgroundContentRect =
					( fnGetThemeBackgroundContentRect ) GetProcAddress ( m_hThemeLib, "GetThemeBackgroundContentRect" );
			m_GetThemeBackgroundExtent =
					( fnGetThemeBackgroundExtent ) GetProcAddress ( m_hThemeLib, "GetThemeBackgroundExtent" );
			m_DrawThemeText = ( fnDrawThemeText ) GetProcAddress ( m_hThemeLib, "DrawThemeText" );
               m_DrawThemeIcon = ( fnDrawThemeIcon ) GetProcAddress ( m_hThemeLib, "DrawThemeIcon" );
               m_DrawThemeEdge = ( fnDrawThemeEdge ) GetProcAddress ( m_hThemeLib, "DrawThemeEdge" );
               m_GetThemeAppProperties = ( fnGetThemeAppProperties ) GetProcAddress ( m_hThemeLib, "GetThemeAppProperties" );
			m_IsThemeActive = ( fnIsThemeActive ) GetProcAddress ( m_hThemeLib, "IsThemeActive" );
               m_GetThemeTextExtent = ( fnGetThemeTextExtent ) GetProcAddress ( m_hThemeLib, "GetThemeTextExtent" );
               m_DrawThemeParentBackground = ( fnDrawThemeParentBackground ) GetProcAddress ( m_hThemeLib, "DrawThemeParentBackground" );

			m_bCanApplyXPStyle = ( m_OpenThemeData && m_CloseThemeData &&
				m_DrawThemeBackground && m_EnableThemeDialogTexture && 
				m_GetThemeBackgroundContentRect &&
				m_GetThemeBackgroundExtent &&
				m_DrawThemeText && m_DrawThemeIcon && m_DrawThemeEdge &&
				m_GetThemeAppProperties &&
				m_IsThemeActive &&
				m_IsAppThemed &&
                    m_DrawThemeParentBackground &&
                    m_GetThemeTextExtent );
		}
		else
		{
			m_IsAppThemed  = NULL;
			m_OpenThemeData = NULL;
			m_CloseThemeData = NULL;
			m_DrawThemeBackground = NULL;
			m_GetThemeBackgroundContentRect = NULL;
			m_GetThemeBackgroundExtent = NULL;
			m_DrawThemeText = NULL;
			m_GetThemeAppProperties = NULL;
			m_IsThemeActive = NULL;
			m_EnableThemeDialogTexture = NULL;
               m_DrawThemeParentBackground = NULL;
		}

		m_hUser32Lib = LoadLibrary ( "user32.dll" );

          if   ( m_hUser32Lib )
          {
               m_TrackMouseEvent = ( fnTrackMouseEvent ) GetProcAddress ( m_hUser32Lib, "TrackMouseEvent" );
          }
		else
          {
               m_TrackMouseEvent = NULL;
          }
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CCXPThemeHelper::~CCXPThemeHelper ()
{
	if   ( --m_iCount == 0 )
	{
		if   ( m_hThemeLib )
          {
               FreeLibrary ( m_hThemeLib );
          }
		if   ( m_hUser32Lib )
          {
               FreeLibrary ( m_hUser32Lib );
          }
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool CCXPThemeHelper::IsCurrentThemed()
{
	bool bRet = false;
	if( m_bUseXPStyle && m_IsAppThemed && m_IsThemeActive && m_IsAppThemed() && m_IsThemeActive())
	{
		HMODULE hMod = LoadLibrary("comctl32.dll");
		HRESULT (WINAPI* fnDllGetVersion)(DLLVERSIONINFO*);
		(void*&)fnDllGetVersion = hMod ? GetProcAddress(hMod, "DllGetVersion") : NULL;;

		DLLVERSIONINFO dvi = {sizeof dvi, 0};
		bRet = fnDllGetVersion && fnDllGetVersion(&dvi) == NOERROR && dvi.dwMajorVersion >= 6;

		if (hMod)
			FreeLibrary(hMod);
	}

	return bRet;
}

//==============================================================================

#include "stdafx.h"
#include "LocalizeMenu.h"

// ---
static void LocalizeOneMenuItem( HMENU hMenu, int nPos, UINT nLocaleID, UINT nIDOrMenu, bool bPopup, LocalizeString pLocFunction, void *pUserData ) {
	TCHAR pBuffer[256] = {0};
	::GetMenuString( hMenu, nPos, pBuffer, sizeof(pBuffer), MF_BYPOSITION );    
/*
	if ( !*pBuffer ) {
		MENUITEMINFO rcMii;
		::ZeroMemory( &rcMii, sizeof(rcMii) );
		rcMii.cbSize = sizeof(rcMii);
		rcMii.dwTypeData = pBuffer;
		rcMii.cch = sizeof(pBuffer);
		rcMii.fMask = 0x00000040;//MIIM_STRING;

		::GetMenuItemInfo( hMenu, nPos, TRUE, &rcMii );          
	}
*/
	if ( pLocFunction )
		pLocFunction( nLocaleID, pBuffer, pUserData );

	MENUITEMINFO rcMii;
	::ZeroMemory( &rcMii, sizeof(rcMii) );
	rcMii.cbSize = sizeof(rcMii);
	rcMii.fMask = MIIM_STATE;

	::GetMenuItemInfo( hMenu, nPos, TRUE, &rcMii );          

	::ModifyMenu( hMenu, nPos, MF_BYPOSITION | (bPopup ? MF_POPUP : 0), nIDOrMenu, pBuffer );

	::SetMenuItemInfo( hMenu, nPos, TRUE, &rcMii );          
}


// ---
static void ScanSubMenuForLocalize( HMENU hMenu, LocalizeString pLocFunction, void *pUserData ) {
	if ( hMenu ) {
		for ( int i=::GetMenuItemCount(hMenu)-1; i>=0; i-- ) {
      int nId = ::GetMenuItemID( hMenu, i );
      if ( nId >= 0 ) {
				if ( nId > 0 ) 
					LocalizeOneMenuItem( hMenu, i, nId, nId, false, pLocFunction, pUserData );
      }
      else {
        HMENU hChildMenu = ::GetSubMenu( hMenu, i );
        if ( hChildMenu ) {
					MENUITEMINFO rcMii;
					::ZeroMemory( &rcMii, sizeof(rcMii) );
					rcMii.cbSize = sizeof(rcMii);
					rcMii.fMask = MIIM_ID;

					::GetMenuItemInfo( hMenu, i, TRUE, &rcMii );          

					LocalizeOneMenuItem( hMenu, i, rcMii.wID, (UINT)hChildMenu, true, pLocFunction, pUserData );

          ScanSubMenuForLocalize( hChildMenu, pLocFunction, pUserData );
				}
      }     
		}
	}
}


// ---
void LocalizeMenu( HMENU hMenu, LocalizeString pLocFunction, void *pUserData ) {
  ScanSubMenuForLocalize( hMenu, pLocFunction, pUserData );
}  

#ifndef __LICALIZEMENU_H__
#define __LICALIZEMENU_H__

typedef void (* LocalizeString)( UINT nLocaleId, LPTSTR pBuffer, void *pUserData );

void LocalizeMenu( HMENU hMenu, LocalizeString pLocFunction, void *pUserData );

#endif
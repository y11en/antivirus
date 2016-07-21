#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>


// ---
WCHAR * IOSFindFileExtentionW( const WCHAR *pszFileName, bool realExt ) {
#if defined(_UNICODE)
	return IOSFindFileExtention( pszFileName, realExt );
#else
  if ( pszFileName && *pszFileName ) {
    WCHAR *ext = (WCHAR *)pszFileName + (wcslen(pszFileName) - 1);
    while ( ext && ext != pszFileName ) {
      if ( *ext == L'.' )
        return ext + !!realExt;
			WCHAR nNext = *ext;
      if ( nNext == L'\\' || nNext == L'/' )
        break;
      ext = ext - 1;
    }
  }
  return NULL;
#endif	
}

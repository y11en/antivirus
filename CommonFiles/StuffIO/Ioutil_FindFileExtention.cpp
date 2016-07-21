#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>


// ---
TCHAR * IOSFindFileExtention( const TCHAR *pszFileName, bool realExt ) {
  if ( pszFileName && *pszFileName ) {
    TCHAR *ext = _tcsninc((TCHAR *)pszFileName, _tcsclen(pszFileName) - 1);
    while ( ext && ext != pszFileName ) {
      if ( _tcsnextc(ext) == L'.' )
        return _tcsninc(ext, !!realExt);
			int nNext = _tcsnextc(ext);
      if ( nNext == L'\\' || nNext == L'/' )
        break;
      ext = _tcsdec( pszFileName, ext );
    }
  }
  return NULL;
}


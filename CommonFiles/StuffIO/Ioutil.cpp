#include <windows.h>
#include <tchar.h>
#include <stuff/pathstr.h>
#include <stuff/pathstra.h>
#include <stuff/pathstrw.h>
#include <stuff/csystem.h>
#include <stuffio/xfile.h>
#include <stuffio/ioutil.h>


#if defined(_UNICODE) || defined(UNICODE)
#define TCSCLEN(p) (p ? _tcslen(p) : 0)
#else
#define TCSCLEN(p) (p ? strlen(p) : 0)
#endif


// ---
TCHAR* DuplicateString( const TCHAR* s ) {
  return s ? _tcscpy( new TCHAR[TCSCLEN(s) + 1], s ) : 0;
}


// ---
TCHAR* DuplicateFileName( const TCHAR* s ) {
  TCHAR *newName = ::DuplicateString( s );
  if ( newName ) {
    while ( _tcsnextc(newName) != L'\0' && _tcsnextc(newName) == L' ' ) 
      _tcscpy( newName, _tcsinc(newName) );
    TCHAR *p = _tcsninc(newName, _tcsclen(newName) - 1);
    while ( p && p != newName && _tcsnextc(p) == L' ' ) {
      *p = L'\0'; 
			p = _tcsdec(newName, p);
		}
  }
  return newName;
}




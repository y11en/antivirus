#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>


// ---
WCHAR* DuplicateStringW( const WCHAR* s ) {
  return s ? wcscpy( new WCHAR[wcslen(s) + 1], s ) : 0;
}


// ---
WCHAR* DuplicateFileNameW( const WCHAR* s ) {
  WCHAR *newName = ::DuplicateStringW( s );
  if ( newName ) {
    while ( *newName != L'\0' && *newName == L' ' ) 
      wcscpy( newName, ++newName );
    WCHAR *p = newName + wcslen(newName) - 1;
    while ( p && p != newName && *p == L' ' ) {
      *p = L'\0'; 
			p = p - 1;
		}
  }
  return newName;
}




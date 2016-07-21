#if defined (_WIN32)

#include <string.h>
#include <new>

#include "direnum.h"

#if !defined (_WIN32_WCE)
# define USE_CHAR_API
#endif

namespace KLSysNS {

#ifdef USE_CHAR_API
  bool match ( const char* mask, const char* s )
  {
    const char* cp=0;
    const char* mp=0;
    for (; *s && ( *mask != '*' ); mask++,s++ ) 
      if ( ( *mask != *s ) && ( *mask!=tolower(*s) ) && ( *mask != '?' ) )
        return false;
    for (;;) {
      if ( !*s ) {
        while ( *mask == '*' ) 
          mask++; 
        return !*mask; 
      }
      if ( *mask == '*' ) { 
        if ( !*++mask ) 
          return true; 
        mp = mask; 
        cp = s + 1; 
        continue; 
      }
      if ( ( *mask==*s ) || ( *mask==tolower(*s) ) || ( *mask=='?' ) ) { 
        mask++; 
        s++;
        continue; 
      }
      mask=mp; s=cp++;
    }
    return false;
  }
#endif // USE_CHAR_API

  bool match_w ( const wchar_t* mask, const wchar_t* s )
  {
    const wchar_t* cp=0;
    const wchar_t* mp=0;
    for (; *s && ( *mask != L'*' ); mask++,s++ ) 
      if ( ( *mask != *s ) && ( *mask != towlower(*s) ) &&( *mask != L'?' ) )
        return false;
    for (;;) {
      if ( !*s ) {
        while ( *mask == L'*' ) 
          mask++; 
        return !*mask; 
      }
      if ( *mask == L'*' ) { 
        if ( !*++mask ) 
          return true; 
        mp = mask; 
        cp = s + 1; 
        continue; 
      }
      if ( ( *mask==*s ) || ( *mask==towlower(*s) ) || ( *mask==L'?' ) ) { 
        mask++; 
        s++;
        continue; 
      }
      mask=mp; s=cp++;
    }
    return false;
  }


#ifdef USE_CHAR_API
  inline bool isAuxiliaryDirectory ( const char* aDirectory )
  {
    return ( aDirectory &&  
             ( ( ( aDirectory [ 0 ] == '.' ) && 
                 ( aDirectory [ 1 ] == 0 ) ) ||
               ( ( aDirectory [ 0 ] == '.' ) && 
                 ( aDirectory [ 1 ] == '.' ) && 
                 ( aDirectory [ 2 ] == 0 ) ) ) );
  }
#endif // USE_CHAR_API

  inline bool isAuxiliaryDirectory_w ( const wchar_t* aDirectory ) 
  {
    return ( aDirectory &&  
             ( ( ( aDirectory [ 0 ] == L'.' ) && 
                 ( aDirectory [ 1 ] == 0 ) ) ||
               ( ( aDirectory [ 0 ] == L'.' ) && 
                 ( aDirectory [ 1 ] == L'.' ) && 
                 ( aDirectory [ 2 ] == 0 ) ) ) );
  }

  DirectoryEnumerator::DirectoryEnumerator ()
    : theDirectory ( INVALID_HANDLE_VALUE ),
      isFirstEntry ( true ),
      isUnicode ( false )
  {}
  
#ifdef USE_CHAR_API
  DirectoryEnumerator::DirectoryEnumerator ( const char* aPath )
    : theDirectory ( INVALID_HANDLE_VALUE ),
      isFirstEntry ( true ),
      isUnicode ( false )
  {
    open ( aPath );
  }
#endif // USE_CHAR_API

  DirectoryEnumerator::DirectoryEnumerator ( const wchar_t* aPath )
    : theDirectory ( INVALID_HANDLE_VALUE ),
      isFirstEntry ( true ),
      isUnicode ( true )
  {
    open ( aPath );
  }

  
  DirectoryEnumerator::~DirectoryEnumerator ()
  {
    close ();
  }
  
  bool DirectoryEnumerator::isOpened () const
  {
    return theDirectory != INVALID_HANDLE_VALUE;
  }
  
#ifdef USE_CHAR_API
  bool DirectoryEnumerator::open ( const char* aPath ) 
  {
    close ();
    DWORD anAttributes = GetFileAttributesA ( aPath );
    if ( ( anAttributes == INVALID_FILE_ATTRIBUTES ) || 
         !( anAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) 
      return false;
    char aPattern [ MAX_PATH ] = {0};
    strncat ( aPattern, aPath, sizeof ( aPattern ) - 1 );
    strncat ( aPattern, "\\*", sizeof ( aPattern ) - strlen ( aPath ) - 1 );  
    WIN32_FIND_DATAA* aDirEntry = reinterpret_cast<WIN32_FIND_DATAA*> ( theDataPlace );
    theDirectory = FindFirstFileA ( aPattern, aDirEntry );
    if ( theDirectory == INVALID_HANDLE_VALUE )
      return false;
    isFirstEntry = true;
    isUnicode = false;
    return true;
  }
#endif // USE_CHAR_API

  bool DirectoryEnumerator::open ( const wchar_t* aPath ) 
  {
    close ();
	DWORD anAttributes = GetFileAttributesW ( aPath );
    if ( ( anAttributes == INVALID_FILE_ATTRIBUTES ) || 
         !( anAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) 
      return false;
    wchar_t aPattern [ MAX_PATH ] = {0};
    wcsncat ( aPattern, aPath, sizeof ( aPattern ) / sizeof ( wchar_t ) - 1 );
    wcsncat ( aPattern, L"\\*", sizeof ( aPattern ) / sizeof ( wchar_t ) - wcslen ( aPath ) - 1 );    
    WIN32_FIND_DATAW* aDirEntry = reinterpret_cast<WIN32_FIND_DATAW*> ( theDataPlace );
    theDirectory = FindFirstFileW ( aPattern, aDirEntry );
    if ( theDirectory == INVALID_HANDLE_VALUE )
      return false;
    isFirstEntry = true;
    isUnicode = true;
    return true;
  }

  void DirectoryEnumerator::close ()
  {
    if ( theDirectory == INVALID_HANDLE_VALUE )
      return;
    FindClose ( theDirectory );
    theDirectory = INVALID_HANDLE_VALUE;
    isFirstEntry = true;
  }

#ifdef USE_CHAR_API
  bool DirectoryEnumerator::nextEntry ( const char* aPattern, bool aDirectoryPattern )
  {
    if ( theDirectory == INVALID_HANDLE_VALUE )
      return false;
    if ( isUnicode )
      return false;
    WIN32_FIND_DATAA* aDirEntry = reinterpret_cast<WIN32_FIND_DATAA*> ( theDataPlace );
    if ( isFirstEntry ) {
      isFirstEntry = false;
      if ( !isAuxiliaryDirectory ( aDirEntry->cFileName ) &&
           ( ( !aPattern || match ( aPattern, aDirEntry->cFileName ) ) || 
             ( !aDirectoryPattern && isDirectory () ) ) )
        return true;
    }
    while ( FindNextFileA ( theDirectory, aDirEntry ) ) {
      if ( isAuxiliaryDirectory ( aDirEntry->cFileName ) )
        continue;
      if ( !aPattern || match ( aPattern, aDirEntry->cFileName ) )
        return true;
      if ( !aDirectoryPattern && isDirectory () ) 
        return true;
    };
    close ();
    return false;
  }  
#endif // USE_CHAR_API
  
  bool DirectoryEnumerator::nextEntry ( const wchar_t* aPattern, bool aDirectoryPattern )
  {
    if ( theDirectory == INVALID_HANDLE_VALUE )
      return false;
    if ( !isUnicode )
      return false;
    WIN32_FIND_DATAW* aDirEntry = reinterpret_cast<WIN32_FIND_DATAW*> ( theDataPlace );
    if ( isFirstEntry ) {
      isFirstEntry = false;
      if ( !isAuxiliaryDirectory_w ( aDirEntry->cFileName ) &&
           ( ( !aPattern || match_w ( aPattern, aDirEntry->cFileName ) ) || 
             ( !aDirectoryPattern && isDirectory () ) ) )
        return true;
    }
    while ( FindNextFileW ( theDirectory, aDirEntry ) ) {
      if ( isAuxiliaryDirectory_w ( aDirEntry->cFileName ) )
        continue;
      if ( !aPattern || match_w ( aPattern, aDirEntry->cFileName ) )
        return true;
      if ( !aDirectoryPattern && isDirectory () ) 
        return true;
    };
    close ();
    return false;
  }  
  
#ifdef USE_CHAR_API
  const char* DirectoryEnumerator::getFileName () const
  {
    if ( theDirectory == INVALID_HANDLE_VALUE )
      return 0;
    if ( isUnicode )
      return 0;
    return reinterpret_cast<const WIN32_FIND_DATAA*> ( theDataPlace )->cFileName;
  }
#endif // USE_CHAR_API

  const wchar_t* DirectoryEnumerator::getFileNameW () const
  {
    if ( theDirectory == INVALID_HANDLE_VALUE )
      return 0;
    if ( !isUnicode )
      return 0;
    return reinterpret_cast<const WIN32_FIND_DATAW*> ( theDataPlace )->cFileName;
  }
    
  bool DirectoryEnumerator::isDirectory () const
  {
    if ( theDirectory == INVALID_HANDLE_VALUE )
      return false;
    if ( isUnicode ) 
      return ( reinterpret_cast<const WIN32_FIND_DATAW*> ( theDataPlace )->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0;
    else
      return ( reinterpret_cast<const WIN32_FIND_DATAA*> ( theDataPlace )->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0;
  }

}

#endif // _WIN32

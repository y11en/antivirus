#include <windows.h>
#include <tchar.h>
#include <stuffio/xfile.h>
#include <stuffio/ioutil.h>


// ---
IOS_ERROR_CODE IOSCheckCreatableFile( const TCHAR *pszFileName ) {

  uint uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );

  WIN32_FIND_DATA w32fd;

  HANDLE hFinder = ::FindFirstFile( pszFileName, &w32fd );
  if ( hFinder != INVALID_HANDLE_VALUE ) {
    ::FindClose( hFinder );
  }


  XFile *inputFile = new XFile( pszFileName, FIOAM_WRITE | FIOCD_OPEN_ALWAYS );

  IOS_ERROR_CODE result = ::IOSGetLastError();

  uint32 dwLength;

  if ( result == IOS_ERR_OK ) {
    inputFile->XLength( &dwLength );
    inputFile->XSetTime( &w32fd.ftCreationTime, &w32fd.ftLastAccessTime, &w32fd.ftLastWriteTime ); 	
  }  

  delete  inputFile;

  if ( result == IOS_ERR_OK && !dwLength )
    ::IOSDeleteFile( pszFileName );

  ::SetErrorMode( uiOldMode );

  return result;  
}


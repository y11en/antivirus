#include <windows.h>
#include <tchar.h>
#include <stuffio/ioutil.h>

//---
IOS_ERROR_CODE  IOSCheckWritableFile( const TCHAR * pszFileName, bool fCheckDirectory ) {
  if ( pszFileName ) {

    ::IOSSetLastError( IOS_ERR_OK );

    TCHAR szDirName[_MAX_PATH];

    uint32 dwAttributes = ::GetFileAttributes( pszFileName );
  
    // If file exists, check it for readonly state
    if ( 0xFFFFFFFF != dwAttributes ) {       // Exists...
      if ( (dwAttributes & FILE_ATTRIBUTE_READONLY) )      // ...and readonly
        return IOS_ERR_WRITE_PROTECT;
    }


    ::IOSDirExtract( pszFileName, szDirName, _MAX_PATH );

    // Well, file either does not exist or does not have the readonly attribute set;
    // Let's check whether the whole directory can be accessed for write.

    return !fCheckDirectory 
           ? IOS_ERR_OK
           : ::IOSCheckWritableDir( szDirName );
  }
  return IOS_ERR_INVALID_FILE_NAME;
}



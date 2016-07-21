#include <windows.h>
#include <tchar.h>
#include <stuff/pathstr.h>
#include <stuff/pathstra.h>
#include <stuff/pathstrw.h>
#include <stuff/csystem.h>
#include <stuffio/xfile.h>
#include <stuffio/ioutil.h>


// ---
IOS_ERROR_CODE IOSGetFileTitle( const TCHAR *pszFileName, TCHAR *szBuffer, uint uicchBufferLen ) {
  if ( pszFileName && szBuffer ) {
    if ( ::GetFileTitle(pszFileName, szBuffer, WORD(uicchBufferLen)) >= 0 )
      return IOS_ERR_OK;
  }  
  return IOS_ERR_INVALID_FILE_NAME;
} 

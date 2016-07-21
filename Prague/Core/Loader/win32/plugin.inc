#if !defined (_PLUGIN_WIN32_INC_)
#define _PLUGIN_WIN32_INC_

#include <windows.h>
#include <HOOK/FSSync.h>

// ---
tERROR ModuleDATA::lock() {
	if ( !m_protector )
		return errOK;
  DWORD wait_result = ::WaitForSingleObject( m_protector, PLUGIN_LOAD_TIMEOUT );
  if ( wait_result == WAIT_TIMEOUT ) {
    PR_TRACE(( this, prtDANGER, "Timeout expired for plugin \"%S\"", m_name.data() ));
    return errSYNCHRONIZATION_TIMEOUT;
  }
  else if ( wait_result != WAIT_OBJECT_0 ) {
    PR_TRACE(( this, prtDANGER, "Synchronization failed for plugin \"%S\"", m_name.data() ));
    return errSYNCHRONIZATION_TIMEOUT;
  }
	return errOK;
}



// ---
tERROR ModuleDATA::unlock() {
	if ( m_protector )
		::ReleaseMutex( m_protector );
	return errOK;
}



tERROR createProtector( HANDLE& aProtector, const tWCHAR* name ) {
#if defined( SINGLE_PLUGIN_PROTECTOR ) 
  aProtector = ::g_plugin_protector;       // synchro object to protect plugin's load process
#else
	#if defined(_DEBUG_N)
		tBYTE  buf[0x400];
		wsprintf( (tCHAR*)buf, "pr_plugin_protector_%S", name );
		for( tCHAR*p=(tCHAR*)buf; *p; ++p ) {
			if ( (*p == '\\') || (*p == ':') || (*p == '.') )
				*p = '_';
		}
		aProtector = ::CreateMutex( 0, FALSE, (tCHAR*)buf );
	#else //_DEBUG
		aProtector = ::CreateMutex( 0, FALSE, 0 );
	#endif //_DEBUG
  if ( !aProtector )
    return errOBJECT_CANNOT_BE_INITIALIZED;
#endif //SINGLE_PLUGIN_PROTECTOR
  return errOK;
}


void destroyProtector( HANDLE& aProtector ) 
{
#if !defined( SINGLE_PLUGIN_PROTECTOR )
  if ( aProtector )
    ::CloseHandle( aProtector );
#endif //SINGLE_PLUGIN_PROTECTOR
  aProtector = 0;
}

tERROR getFileTime( const cStrObj& aName, tDT* aDateTime )
{
  SYSTEMTIME       st;
  WIN32_FIND_DATAW fd;
  HANDLE           fh;

  cStrBuff fName(aName,CP);

  if ( ::g_bUnderNT ) {
    if ( INVALID_HANDLE_VALUE == (fh=FindFirstFileW(UNI(fName),&fd)) )
      return  errMODULE_NOT_FOUND;
  }
  else if ( INVALID_HANDLE_VALUE == (fh=FindFirstFileA(MB(fName),(WIN32_FIND_DATA*)&fd)) ) 
    return  errMODULE_NOT_FOUND;

  FindClose( fh );
  FileTimeToSystemTime( &fd.ftLastWriteTime, &st );
  return DTSet( (tDT*)aDateTime, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds*1000000 );
}


#endif //_PLUGIN_WIN32_INC_

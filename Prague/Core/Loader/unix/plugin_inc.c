#if !defined (_PLUGIN_UNIX_INC_)
#define _PLUGIN_UNIX_INC_

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

tERROR lockProtector( pthread_mutex_t& theProtector, cObj* obj, const tVOID* name, tCODEPAGE cp ) {
  pthread_mutex_lock ( &theProtector );
  return errOK;
}

tERROR unlockProtector( pthread_mutex_t& theProtector, cObj* obj, const tVOID* name, tCODEPAGE cp ) {
  pthread_mutex_unlock ( &theProtector );  
  return errOK;
}

tERROR pr_call initPluginProtection() {
	return errOK;
}

tVOID pr_call deinitPluginProtection() {}

inline        cPluginProtector::cPluginProtector() {}

inline tERROR cPluginProtector::init()                  
{ 
	if(pthread_mutex_init(&m_prot, 0))
		return errOBJECT_CANNOT_BE_INITIALIZED;                  
	return errOK;
}

inline tERROR cPluginProtector::deinit()                
{ 
	pthread_mutex_destroy(&m_prot);
	return errOK;	
}

inline tERROR cPluginProtector::get( tProtector& prot ) { prot = m_prot; return errOK; }
inline tVOID  cPluginProtector::release()               {}

inline tERROR     cPluginProtector::lock( ModuleDATA& plugin ) {
	return lockProtector( m_prot, plugin, plugin.name().data(), cCP_UNICODE );
}

inline tERROR     cPluginProtector::unlock( ModuleDATA& plugin ) {
	return unlockProtector( m_prot, plugin, plugin.name().data(), cCP_UNICODE );
}

tERROR getFileTime ( const cStrObj& aName, tDT* aDateTime )
{
  if ( !DTSet )
    return errUNEXPECTED;

  struct stat aStatus;
  cStrBuff aBuffName ( aName, cCP );       
  if ( stat ( aBuffName, &aStatus ) )
    return errMODULE_NOT_FOUND;

  struct tm aTm;
  
  if ( !localtime_r ( &aStatus.st_mtime, &aTm ) )
    return errUNEXPECTED;    

  return DTSet ( aDateTime, aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec, 0 );
}


#endif //_PLUGIN_UNIX_INC_

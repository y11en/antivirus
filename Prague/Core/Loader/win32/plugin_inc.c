#if !defined (_PLUGIN_WIN32_INC_)
#define _PLUGIN_WIN32_INC_

#include <windows.h>
#include <HOOK/FSSync.h>



// ---
#if defined( __cplusplus )
	extern "C"
#endif

// ---
tERROR lockProtector( HANDLE theProtector, cObj* obj, const tVOID* name, tCODEPAGE cp ) {
	PR_TRACE_VARS;
	if ( !theProtector )
		return errOK;
  DWORD wait_result = ::WaitForSingleObject( theProtector, 200000 /*PLUGIN_LOAD_TIMEOUT*/ );
  if ( wait_result == WAIT_TIMEOUT ) {
    PR_TRACE(( obj, prtDANGER, "ldr\tTimeout expired for plugin \"%s%S\"", PR_TRACE_PREPARE(name) ));
    return errSYNCHRONIZATION_TIMEOUT;
  }
	if ( wait_result == WAIT_FAILED ) {
    PR_TRACE(( obj, prtDANGER, "ldr\tSynchronization failed for plugin \"%s%S\"", PR_TRACE_PREPARE(name) ));
    return errSYNCHRONIZATION_TIMEOUT;
	}
  if ( wait_result != WAIT_OBJECT_0 ) {
    PR_TRACE(( obj, prtDANGER, "ldr\tSynchronization result unknown for plugin \"%s%S\"", PR_TRACE_PREPARE(name) ));
    return errSYNCHRONIZATION_TIMEOUT;
  }
	return errOK;
}



// ---
#if defined( __cplusplus )
	extern "C"
#endif

tERROR unlockProtector( HANDLE theProtector, cObj* obj, const tVOID* name, tCODEPAGE cp ) {
	if ( theProtector )
		::ReleaseMutex( theProtector );
	return errOK;
}



// ---
#if defined( __cplusplus )
	extern "C"
#endif

tERROR createProtector( HANDLE& aProtector, const tWCHAR* name ) {
	#if defined(__DEBUG) // changed by Mezhuev 19.10.2005
		tBYTE  buf[0x400];
		wsprintf( (tCHAR*)buf, "pr_plugin_protector_%S", name );
		for( tCHAR*p=(tCHAR*)buf; *p; ++p ) {
			if ( (*p == '\\') || (*p == ':') || (*p == '.') )
				*p = '_';
		}
		aProtector = ::PrCreateMutex( 0, FALSE, (tCHAR*)buf );
	#else //_DEBUG
		aProtector = ::PrCreateMutex( 0, FALSE, 0 );
	#endif //_DEBUG
  if ( !aProtector )
    return errOBJECT_CANNOT_BE_INITIALIZED;
  return errOK;
}


// ---
void destroyProtector( HANDLE& aProtector ) {
  if ( aProtector )
    ::CloseHandle( aProtector );
  aProtector = 0;
}



#if defined( ADVANCED_PLUGIN_PROTECTOR )
	
	CRITICAL_SECTION cPluginProtector::g_cs; // synchro object to protect plugin's load process
	struct cMutexEl {
		tBOOL      m_busy;
		tProtector m_prot;
	};

	#define MUTEX_POOL_CLUSTER 20
	static tUINT     g_mutex_pool_count = 0;
	static cMutexEl  g_mutex_pool_static[MUTEX_POOL_CLUSTER];
	static cMutexEl* g_mutex_pool = g_mutex_pool_static;
	static tBOOL     g_protector_inited = cFALSE;

	// ---
	#if defined( __cplusplus )
		extern "C"
	#endif

	tERROR pr_call initPluginProtection() {
		if ( !g_protector_inited ) {
			g_protector_inited = cTRUE;
			InitializeCriticalSection( &cPluginProtector::g_cs );
		}
		return errOK;
	}

	// ---
	#if defined( __cplusplus )
		extern "C"
	#endif

	tVOID pr_call deinitPluginProtection() {
		tUINT i;
		cMutexEl* me;
		EnterCriticalSection( &cPluginProtector::g_cs );
		me = g_mutex_pool;
		for( i=0; i<g_mutex_pool_count; ++i,++me ) {
			if ( me->m_prot )
				destroyProtector( me->m_prot );
			me->m_busy = cFALSE;
		}
		g_mutex_pool_count = 0;
		if ( g_mutex_pool != g_mutex_pool_static ) {
			::HeapFree( GetProcessHeap(), 0, g_mutex_pool );
			g_mutex_pool = g_mutex_pool_static;
		}
		g_protector_inited = cFALSE;
		LeaveCriticalSection( &cPluginProtector::g_cs );
		DeleteCriticalSection( &cPluginProtector::g_cs );
	}

	// ---
	inline        cPluginProtector::cPluginProtector() : m_prot_counter(0), m_prot(0) {  }
	inline tERROR cPluginProtector::init()   { return errOK; }
	inline tERROR cPluginProtector::deinit() { return errOK; }


	// ---
	inline tERROR cPluginProtector::get( tProtector& prot ) {
		cERROR err;
		EnterCriticalSection( &g_cs );
		if ( !m_prot_counter ) {
			cMutexEl* me   = 0;
			cMutexEl* curr = g_mutex_pool;
			tUINT     i;
			for( i=0; !me && (i<g_mutex_pool_count); ++i,++curr ) {
				if ( curr->m_prot && !curr->m_busy )
					me = curr;
			}
			for( curr=g_mutex_pool,i=0; !me && (i<g_mutex_pool_count); ++i,++curr ) {
				if ( !curr->m_prot )
					me = curr;
			}
			if ( !me ) {
				if ( g_mutex_pool_count && !(g_mutex_pool_count % MUTEX_POOL_CLUSTER) ) {
					HANDLE heap = GetProcessHeap();
					cMutexEl* tmp = (cMutexEl*)::HeapAlloc( heap, HEAP_ZERO_MEMORY, (g_mutex_pool_count+MUTEX_POOL_CLUSTER)*sizeof(cMutexEl) );
					if ( tmp ) {
						memcpy( tmp, g_mutex_pool, g_mutex_pool_count*sizeof(cMutexEl) );
						if ( g_mutex_pool != g_mutex_pool_static )
							::HeapFree( heap, 0, g_mutex_pool );
						g_mutex_pool = tmp;
					}
					else
						err = errNOT_ENOUGH_MEMORY;
				}
				if ( PR_SUCC(err) )
					me = g_mutex_pool + (g_mutex_pool_count++);
			}
			if ( PR_SUCC(err) && !me->m_prot )
				err = createProtector( me->m_prot, 0 );
			if ( PR_SUCC(err) ) {
				m_prot = me->m_prot;
				me->m_busy = cTRUE;
			}
		}
		if ( PR_SUCC(err) )
			++m_prot_counter;
		LeaveCriticalSection( &g_cs );

		prot = m_prot;
		return err;
	}

	// ---
	inline tVOID cPluginProtector::release() {
		EnterCriticalSection( &g_cs );
		if ( m_prot_counter ) {
			--m_prot_counter;
			if ( !m_prot_counter ) {
				tUINT     i;
				cMutexEl* curr = g_mutex_pool;
				for( i=0; i<g_mutex_pool_count; ++i,++curr ) {
					if ( curr->m_prot == m_prot ) {
						curr->m_busy = cFALSE;
						m_prot = 0;
						break;
					}
				}
				if ( m_prot )
					destroyProtector( m_prot );
			}
		}
		LeaveCriticalSection( &g_cs );
	}

	// ---
	inline tERROR cPluginProtector::lock( ModuleDATA& plugin ) {
		tProtector prot;
		cERROR err = get( prot );
		if ( PR_SUCC(err) )
			err = lockProtector( prot, plugin, plugin.name().data(), cCP_UNICODE );
		return err;
	}

	// ---
	inline tERROR cPluginProtector::unlock( ModuleDATA& plugin ) {
		cERROR err = unlockProtector( m_prot, plugin, plugin.name().data(), cCP_UNICODE );
		release();
		return err;
	}

#elif defined( GLOBAL_PLUGIN_PROTECTOR )

	tProtector cPluginProtector::g_prot; // synchro object to protect plugin's load process

	// ---
	tERROR pr_call initPluginProtection() {
		if ( cPluginProtector::g_prot )
			return errOK;
		return createProtector( cPluginProtector::g_prot, L"plugin_protector" );
	}

	// ---
	tVOID pr_call deinitPluginProtection() {
		destroyProtector( cPluginProtector::g_prot );
	}

	inline        cPluginProtector::cPluginProtector() {}
	inline tERROR cPluginProtector::init()                  { return errOK; }
	inline tERROR cPluginProtector::deinit()                { return errOK; }
	inline tERROR cPluginProtector::get( tProtector& prot ) { prot = g_prot; return errOK; }
	inline tVOID  cPluginProtector::release()               { }

	// ---
	inline tERROR     cPluginProtector::lock( ModuleDATA& plugin ) {
		return lockProtector( g_prot, plugin, plugin.name().data(), cCP_UNICODE );
	}
	
	// ---
	inline tERROR     cPluginProtector::unlock( ModuleDATA& plugin ) {
		return unlockProtector( g_prot, plugin, plugin.name().data(), cCP_UNICODE );
	}

#else

	// ---
	tERROR pr_call initPluginProtection() {
		return errOK;
	}

	// ---
	tVOID pr_call deinitPluginProtection() {
	}

	inline        cPluginProtector::cPluginProtector() : m_prot(0) {}
	inline tERROR cPluginProtector::init()                  { return createProtector( m_prot, 0 ); }
	inline tERROR cPluginProtector::deinit()                { destroyProtector( m_prot ); return errOK;	}
	inline tERROR cPluginProtector::get( tProtector& prot ) { prot = m_prot; return errOK; }
	inline tVOID  cPluginProtector::release()               { }

	// ---
	inline tERROR     cPluginProtector::lock( ModuleDATA& plugin ) {
		return lockProtector( m_prot, plugin, plugin.name().data(), cCP_UNICODE );
	}

	
	// ---
	inline tERROR     cPluginProtector::unlock( ModuleDATA& plugin ) {
		return unlockProtector( m_prot, plugin, plugin.name().data(), cCP_UNICODE );
	}


#endif


// ---
tERROR getFileTime( const cStrObj& aName, tDT* aDateTime ) {
  SYSTEMTIME       st;
  WIN32_FIND_DATAW fd;
  HANDLE           fh;

  cStrBuff fName(aName,cCP);

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

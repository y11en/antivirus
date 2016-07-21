// unix_sync.cpp
//
//

#if defined (__unix__)
#include <time.h>

#include <klavsys_unix.h>

////////////////////////////////////////////////////////////////
// Mutex

KLAV_Unix_Mutex::KLAV_Unix_Mutex (KLAV_Alloc* alloc, klav_bool_t recursive) 
    : m_alloc(alloc)
{
    if ( !recursive ) {
      pthread_mutex_init ( &m_mutex, 0 );
      return;
    }
    pthread_mutexattr_t l_attributes;
    pthread_mutexattr_init ( &l_attributes );
    pthread_mutexattr_settype ( &l_attributes, PTHREAD_MUTEX_RECURSIVE );
    pthread_mutex_init ( &m_mutex, &l_attributes );
    pthread_mutexattr_destroy ( &l_attributes );
}

KLAV_Unix_Mutex::~KLAV_Unix_Mutex ()
{
    pthread_mutex_destroy ( &m_mutex );
}

KLAV_ERR KLAV_CALL KLAV_Unix_Mutex::lock ()
{
	return pthread_mutex_lock ( &m_mutex ) ? KLAV_Get_System_Error () : KLAV_OK;
}
        
KLAV_ERR KLAV_CALL KLAV_Unix_Mutex::unlock ()
{
	return pthread_mutex_unlock ( &m_mutex ) ? KLAV_Get_System_Error () : KLAV_OK;
}
	
void KLAV_CALL KLAV_Unix_Mutex::destroy () 
{
	KLAV_DELETE (this, m_alloc);
}

////////////////////////////////////////////////////////////////
// RWLock

KLAV_Unix_RWLock::KLAV_Unix_RWLock (KLAV_Alloc* alloc) 
    : m_alloc(alloc)
{
	//    pthread_rwlock_init ( &m_rwlock, 0 );
	pthread_mutexattr_t l_attributes;
	pthread_mutexattr_init ( &l_attributes );
	pthread_mutexattr_settype ( &l_attributes, PTHREAD_MUTEX_RECURSIVE );
	pthread_mutex_init ( &m_mutex, &l_attributes );
	pthread_mutexattr_destroy ( &l_attributes );
}

KLAV_Unix_RWLock::~KLAV_Unix_RWLock ()
{
	//    pthread_rwlock_destroy ( &m_rwlock );
	pthread_mutex_destroy ( &m_mutex );
}

KLAV_ERR KLAV_CALL KLAV_Unix_RWLock::lock_read ()
{
	//    return pthread_rwlock_rdlock ( &m_rwlock ) ? KLAV_Get_System_Error () : KLAV_OK;
	return pthread_mutex_lock ( &m_mutex ) ? KLAV_Get_System_Error () : KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Unix_RWLock::lock_write ()
{
	//    return pthread_rwlock_wrlock ( &m_rwlock ) ? KLAV_Get_System_Error () : KLAV_OK;
	return pthread_mutex_lock ( &m_mutex ) ? KLAV_Get_System_Error () : KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Unix_RWLock::unlock()
{
	//    pthread_rwlock_unlock ( &m_rwlock ) ? KLAV_Get_System_Error () : KLAV_OK;
	return pthread_mutex_unlock ( &m_mutex ) ? KLAV_Get_System_Error () : KLAV_OK;
}

void KLAV_CALL KLAV_Unix_RWLock::destroy () 
{
	KLAV_DELETE (this, m_alloc);
}


////////////////////////////////////////////////////////////////
// CondVar

KLAV_Unix_CondVar::KLAV_Unix_CondVar (KLAV_Alloc* alloc)
   : m_alloc(alloc)
{
	pthread_mutex_init (&m_mutex,0);
	pthread_cond_init (&m_condvar, 0);

}
KLAV_Unix_CondVar::~KLAV_Unix_CondVar ()
{
	pthread_mutex_destroy (&m_mutex);
	pthread_cond_destroy (&m_condvar);
}

KLAV_ERR KLAV_CALL KLAV_Unix_CondVar::lock ()
{
	return pthread_mutex_lock ( &m_mutex ) ? KLAV_Get_System_Error () : KLAV_OK;
}
        
KLAV_ERR KLAV_CALL KLAV_Unix_CondVar::unlock ()
{
	return pthread_mutex_unlock ( &m_mutex ) ? KLAV_Get_System_Error () : KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Unix_CondVar::broadcast ()
{
	return pthread_cond_broadcast (&m_condvar) ? KLAV_Get_System_Error () : KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Unix_CondVar::wait ()
{
	return pthread_cond_wait (&m_condvar, &m_mutex) ? KLAV_Get_System_Error () : KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Unix_CondVar::timedwait (uint32_t reltime)
{
#if defined(KL_PLATFORM_MACOSX)
	return KLAV_ENOIMPL;
#else
	timespec l_ts;
	if (clock_gettime(CLOCK_REALTIME, &l_ts))
		return KLAV_Get_System_Error ();
	l_ts.tv_nsec += ( reltime % 1000 ) * 1000;
	l_ts.tv_sec += reltime / 1000 + l_ts.tv_nsec % 1000000 ;
	l_ts.tv_nsec %= 1000000;
	return pthread_cond_timedwait (&m_condvar, &m_mutex, &l_ts) ? KLAV_Get_System_Error () : KLAV_OK;	
#endif //KL_PLATFORM_MACOSX	
}

void KLAV_Unix_CondVar::destroy ()
{
	KLAV_DELETE (this, m_alloc);
}

////////////////////////////////////////////////////////////////
// Sync factory

KLAV_Unix_Sync_Factory::KLAV_Unix_Sync_Factory (KLAV_Alloc* alloc)
    : m_alloc (alloc)
{
	if (m_alloc == 0)
		m_alloc = KLAV_Get_System_Allocator ();
}

KLAV_Unix_Sync_Factory::~KLAV_Unix_Sync_Factory ()
{
}

KLAV_Mutex* KLAV_CALL KLAV_Unix_Sync_Factory::create_mutex (klav_bool_t recursive)
{
	return KLAV_NEW (m_alloc) KLAV_Unix_Mutex (m_alloc, recursive);
}

KLAV_RWLock* KLAV_CALL KLAV_Unix_Sync_Factory::create_rwlock ()
{
	return KLAV_NEW (m_alloc) KLAV_Unix_RWLock (m_alloc);
}

KLAV_CondVar* KLAV_CALL KLAV_Unix_Sync_Factory::create_condvar ()
{
	return KLAV_NEW (m_alloc) KLAV_Unix_CondVar (m_alloc);
}
////////////////////////////////////////////////////////////////
// API wrappers

hKLAV_Sync_Factory KLAV_CALL KLAVSYS_Get_Sync_Factory ()
{
	static KLAV_Unix_Sync_Factory g_sync_factory (0);
	return & g_sync_factory;
}

#endif//__unix__

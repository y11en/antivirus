// win_sync.cpp
//

#if defined (_WIN32)

#include <klava/klavsys_windows.h>

////////////////////////////////////////////////////////////////
// Mutex

KLAV_Win32_Mutex::KLAV_Win32_Mutex (KLAV_Alloc* alloc) : m_alloc(alloc)
{
	InitializeCriticalSection (&m_cs);
}

KLAV_Win32_Mutex::~KLAV_Win32_Mutex ()
{
	DeleteCriticalSection (&m_cs);
}

KLAV_ERR KLAV_CALL KLAV_Win32_Mutex::lock ()
{
	EnterCriticalSection (& m_cs);
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Win32_Mutex::unlock ()
{
	LeaveCriticalSection(&m_cs);
	return KLAV_OK;
}
	
void KLAV_CALL KLAV_Win32_Mutex::destroy () 
{
	KLAV_DELETE (this, m_alloc);
}

////////////////////////////////////////////////////////////////
// RWLock (implemented just as mutex)

KLAV_Win32_RWLock::KLAV_Win32_RWLock (KLAV_Alloc* alloc) : 
		m_alloc(alloc)
{
	InitializeCriticalSection (& m_cs);
}

KLAV_Win32_RWLock::~KLAV_Win32_RWLock ()
{
	DeleteCriticalSection (& m_cs);
}

KLAV_ERR KLAV_CALL KLAV_Win32_RWLock::lock_read ()
{
	EnterCriticalSection (&m_cs);
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Win32_RWLock::lock_write ()
{
	EnterCriticalSection (&m_cs);
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Win32_RWLock::unlock()
{
	LeaveCriticalSection (&m_cs);
	return KLAV_OK;
}

void KLAV_CALL KLAV_Win32_RWLock::destroy () 
{
	KLAV_DELETE (this, m_alloc);
};

////////////////////////////////////////////////////////////////
// CondVar

KLAV_Win32_CondVar::KLAV_Win32_CondVar (KLAV_Alloc* alloc)
	: m_alloc(alloc),
	  m_mutex(NULL),
	  m_event(NULL)
{
	m_mutex = CreateMutex (NULL, FALSE, NULL);
	m_event = CreateEvent (NULL, TRUE, FALSE, NULL);

}
KLAV_Win32_CondVar::~KLAV_Win32_CondVar ()
{
	CloseHandle(m_mutex);
	CloseHandle(m_event);
}

KLAV_ERR KLAV_CALL KLAV_Win32_CondVar::lock ()
{
	switch (WaitForSingleObject(m_mutex, INFINITE)) {
	case WAIT_OBJECT_0: return KLAV_OK;
	case WAIT_FAILED: return KLAV_Get_System_Error ();
	default: return KLAV_EUNEXPECTED;
	}
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Win32_CondVar::unlock ()
{
	return ReleaseMutex(m_mutex)? KLAV_OK : KLAV_Get_System_Error ();
}

KLAV_ERR KLAV_CALL KLAV_Win32_CondVar::broadcast ()
{
	return PulseEvent(m_event) ? KLAV_OK : KLAV_Get_System_Error ();
}

KLAV_ERR KLAV_CALL KLAV_Win32_CondVar::wait ()
{
	KLAV_ERR error = KLAV_OK;
#ifdef KL_PLATFORM_WINCE
	unlock();
	switch (WaitForSingleObject(m_event, INFINITE)) {
#else
	switch (SignalObjectAndWait(m_mutex, m_event, INFINITE, FALSE)) {
#endif
	case WAIT_OBJECT_0: error = KLAV_OK; break;
	case WAIT_FAILED: error = KLAV_Get_System_Error (); break;
	default:
		error = KLAV_EUNEXPECTED;
	}
	lock();
	return error;
}

KLAV_ERR KLAV_CALL KLAV_Win32_CondVar::timedwait (uint32_t reltime)
{
	KLAV_ERR error = KLAV_OK;
#ifdef KL_PLATFORM_WINCE
	unlock();
	switch (WaitForSingleObject(m_event, reltime)) {
#else
	switch (SignalObjectAndWait(m_mutex, m_event, reltime, FALSE)) {
#endif
	case WAIT_OBJECT_0: error = KLAV_OK; break;
	case WAIT_TIMEOUT: error = KLAV_ETIMEOUT; break;
	case WAIT_FAILED: error = KLAV_Get_System_Error (); break;
	default:
		error = KLAV_EUNEXPECTED;
	}
	lock();
	return error;
}

void KLAV_Win32_CondVar::destroy ()
{
	KLAV_DELETE (this, m_alloc);
}

////////////////////////////////////////////////////////////////
// SyncFactory

KLAV_Win32_Sync_Factory::KLAV_Win32_Sync_Factory (KLAV_Alloc* alloc)
	: m_alloc(alloc)
{
	if (m_alloc == 0)
		m_alloc = KLAV_Get_System_Allocator ();
}

KLAV_Win32_Sync_Factory::~KLAV_Win32_Sync_Factory ()
{
}

KLAV_Mutex * KLAV_CALL KLAV_Win32_Sync_Factory::create_mutex (int recursive)
{
	return KLAV_NEW (m_alloc) KLAV_Win32_Mutex (m_alloc);
}

KLAV_RWLock * KLAV_CALL KLAV_Win32_Sync_Factory::create_rwlock ()
{
	return KLAV_NEW (m_alloc) KLAV_Win32_RWLock (m_alloc);
}

KLAV_CondVar * KLAV_CALL KLAV_Win32_Sync_Factory::create_condvar ()
{
	return KLAV_NEW (m_alloc) KLAV_Win32_CondVar (m_alloc);
}

KLAV_Sync_Factory * KLAV_CALL KLAVSYS_Get_Sync_Factory ()
{
	static KLAV_Win32_Sync_Factory g_sync_factory (0);
	return & g_sync_factory;
}

#endif // _WIN32


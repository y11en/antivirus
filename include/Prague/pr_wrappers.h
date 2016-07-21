#ifndef __PRWRAPPERS_H__
#define __PRWRAPPERS_H__

#if defined (_WIN32)
struct PRTls
{
	PRTls()  { m_idx = TlsAlloc(); }
	~PRTls() { TlsFree(m_idx); }

	operator DWORD() const{ return m_idx; }

	DWORD m_idx;
};

struct PRLocker
{
	PRLocker()    { InitializeCriticalSection(&m_lock); }
	~PRLocker()   { DeleteCriticalSection(&m_lock); }

	void lock()   { EnterCriticalSection(&m_lock); }
	void unlock() { LeaveCriticalSection(&m_lock); }

	CRITICAL_SECTION m_lock;
};

#elif defined (__unix__)

#include <pthread.h>

typedef tDWORD PRTls;

struct PRLocker
{
	PRLocker()
	{
		pthread_mutexattr_t anAttr;
		pthread_mutexattr_init ( &anAttr );
		pthread_mutexattr_settype ( &anAttr, PTHREAD_MUTEX_RECURSIVE );        
		pthread_mutex_init ( &m_lock, &anAttr );
		pthread_mutexattr_destroy( &anAttr ); 
	}
	~PRLocker()   { pthread_mutex_destroy(&m_lock); }

	void lock()   { pthread_mutex_lock(&m_lock); }
	void unlock() { pthread_mutex_unlock(&m_lock); }

	pthread_mutex_t m_lock;
};

#else
#error "Implementation is needed"
#endif

class PRAutoLocker
{
public:
	PRAutoLocker(PRLocker& pCS) : m_pcs(pCS) { m_pcs.lock(); }
	~PRAutoLocker() { m_pcs.unlock(); }

private:
	PRLocker& m_pcs;
};

#endif //__PRWRAPPERS_H__

// klavsys_unix.h
//

#ifndef klavsys_unix_h_INCLUDED
#define klavsys_unix_h_INCLUDED

#if defined (__unix__)

#include <klavcore.h>
#include <klavsys.h>

#if defined (__cplusplus)

#include <pthread.h>

#if defined (__linux__)
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif

class KLAV_Unix_CondVar;

class KLAV_Unix_Mutex : public KLAV_IFACE_IMPL(KLAV_Mutex)
{
	friend class KLAV_Unix_CondVar;
public:
	KLAV_Unix_Mutex (KLAV_Alloc* alloc, klav_bool_t recursive);
	virtual ~KLAV_Unix_Mutex ();

	virtual KLAV_ERR KLAV_CALL lock ();
	virtual KLAV_ERR KLAV_CALL unlock ();
	virtual void KLAV_CALL destroy ();

private:
	KLAV_Alloc* m_alloc;
	pthread_mutex_t  m_mutex;
};

class KLAV_Unix_RWLock : public KLAV_IFACE_IMPL(KLAV_RWLock)
{
public:
	KLAV_Unix_RWLock (KLAV_Alloc* alloc);
	virtual ~KLAV_Unix_RWLock ();

	virtual KLAV_ERR KLAV_CALL lock_read ();
	virtual KLAV_ERR KLAV_CALL lock_write ();
	virtual KLAV_ERR KLAV_CALL unlock ();
	virtual void destroy ();

private:
	KLAV_Alloc* m_alloc;
	//  pthread_rwlock_t m_rwlock;
	pthread_mutex_t m_mutex;
};

class KLAV_Unix_CondVar : public KLAV_IFACE_IMPL(KLAV_CondVar)
{
public:
	KLAV_Unix_CondVar (KLAV_Alloc* alloc);
	virtual ~KLAV_Unix_CondVar ();

	virtual KLAV_ERR KLAV_CALL lock ();
	virtual KLAV_ERR KLAV_CALL unlock ();
	virtual KLAV_ERR KLAV_CALL broadcast ();
	virtual KLAV_ERR KLAV_CALL wait ();
	virtual KLAV_ERR KLAV_CALL timedwait (uint32_t reltime);
	virtual void destroy ();

private:
	KLAV_Alloc* m_alloc;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_condvar;
};

class KLAV_Unix_Sync_Factory : public KLAV_IFACE_IMPL(KLAV_Sync_Factory)
{
public:
	KLAV_Unix_Sync_Factory (KLAV_Alloc* alloc) ;
	virtual ~KLAV_Unix_Sync_Factory ();	

	virtual KLAV_Mutex* KLAV_CALL create_mutex (klav_bool_t recursive);
	virtual KLAV_RWLock* KLAV_CALL create_rwlock();
	virtual KLAV_CondVar* KLAV_CALL create_condvar();

private:
	KLAV_Alloc* m_alloc;
};

#endif // __cplusplus

#endif //__unix__

#endif //klavsys_unix_h_INCLUDED

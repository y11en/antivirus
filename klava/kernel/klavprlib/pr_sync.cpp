// prgsync.cpp
//
// Prague-based synchronization primitives
//

#include <klava/klav_prague.h>

////////////////////////////////////////////////////////////////
// Mutex

KLAV_Pr_Mutex::KLAV_Pr_Mutex (hMUTEX impl, KLAV_Alloc* alloc) :
		m_impl(impl),
		m_alloc(alloc)
{
}

KLAV_Pr_Mutex::~KLAV_Pr_Mutex ()
{
}
	
KLAV_ERR KLAV_CALL KLAV_Pr_Mutex::lock ()
{
	tERROR err = m_impl->Lock(cSYNC_INFINITE);
	if (PR_FAIL(err))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_Mutex::unlock ()
{
	tERROR err = m_impl->Release();
	if (PR_FAIL(err))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}
	
void KLAV_CALL KLAV_Pr_Mutex::destroy () 
{
	m_impl->sysCloseObject ();
	m_alloc->free (this);
};

void KLAV_Pr_Mutex::destroy_obsolete () 
{
	m_impl->sysCloseObject ();
	m_alloc->free (this);
};

////////////////////////////////////////////////////////////////
// RWLock

KLAV_Pr_RWLock::KLAV_Pr_RWLock (hMUTEX impl, KLAV_Alloc* alloc) : 
	m_impl(impl),
	m_alloc(alloc)
{
}

KLAV_Pr_RWLock::~KLAV_Pr_RWLock ()
{
}

KLAV_ERR KLAV_CALL KLAV_Pr_RWLock::lock_read ()
{
	tERROR err = m_impl->Lock (cSYNC_INFINITE);
	if (PR_FAIL(err))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_RWLock::lock_write ()
{
	tERROR err = m_impl->Lock (cSYNC_INFINITE);
	if (PR_FAIL(err))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_RWLock::unlock ()
{
	tERROR err = m_impl->Release();
	if (PR_FAIL(err))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

void KLAV_CALL KLAV_Pr_RWLock::destroy () 
{
	m_impl->sysCloseObject();
	m_alloc->free(this);
}

////////////////////////////////////////////////////////////////
// SyncFactory

KLAV_Pr_Sync_Factory::KLAV_Pr_Sync_Factory (hOBJECT root, KLAV_Alloc* alloc)
	: m_root(root),
	  m_alloc(alloc)
{
}

KLAV_Pr_Sync_Factory::~KLAV_Pr_Sync_Factory ()
{
}

KLAV_Mutex * KLAV_CALL KLAV_Pr_Sync_Factory::create_mutex (int recursive)
{
	hMUTEX impl = 0;
	tERROR res = m_root->sysCreateObjectQuick (reinterpret_cast<hOBJECT*>(&impl), IID_MUTEX);
	if (PR_FAIL(res))
		return 0;

	return KLAV_NEW (m_alloc) KLAV_Pr_Mutex (impl, m_alloc);
}

KLAV_RWLock * KLAV_CALL KLAV_Pr_Sync_Factory::create_rwlock ()
{
	hMUTEX impl = 0;
	tERROR res = m_root->sysCreateObjectQuick (reinterpret_cast<hOBJECT*>(&impl), IID_MUTEX);
	if (PR_FAIL(res))
		return 0;

	return KLAV_NEW (m_alloc) KLAV_Pr_RWLock (impl, m_alloc);
}

KLAV_CondVar * KLAV_CALL KLAV_Pr_Sync_Factory::create_condvar ()
{
	return 0;
}

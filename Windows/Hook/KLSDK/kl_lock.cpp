#include "kl_lock.h"
#pragma hdrstop

CKl_Lock::CKl_Lock() : CKl_Object()
{    
    AllocateSpinLock(&Lock);
    bState = LOCK_RELEASED;
}

CKl_Lock::~CKl_Lock()
{
    bState = LOCK_DESTROYED;
}

int CKl_Lock::Acquire()
{
    AcquireSpinLock(&Lock, &OldIrql);
    bState = LOCK_ACQUIRED;
    
    return 0;
}

int CKl_Lock::Release()
{
    ReleaseSpinLock(&Lock, OldIrql);
    bState = LOCK_RELEASED;
    return 0;
}

//-------------------------------------------------------

CKl_AutoLock::CKl_AutoLock(SPIN_LOCK* Lock) : m_pLock( Lock )
{
    if ( m_pLock )
    {
        AcquireSpinLock(m_pLock, &m_OldIrql);
    }    
}

CKl_AutoLock::~CKl_AutoLock()
{
    if ( m_pLock )
    {
        ReleaseSpinLock(m_pLock, m_OldIrql);
    }    
}
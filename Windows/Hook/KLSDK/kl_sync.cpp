#include "kl_sync.h"

CKl_SyncObject::CKl_SyncObject()
{
}

CKl_SyncObject::~CKl_SyncObject()
{
}

ULONG
CKl_SyncObject::Wait( KL_TIME WaitTime )
{
    KlWaitEvent( m_pSyncObject );

    return 0;
}
//--------------------------------------------------------------
//--------EVENT-------------------------------------------------
//--------------------------------------------------------------
CKl_Event::CKl_Event()
{
    m_pSyncObject = &m_Event;
    KlInitializeEvent(m_pSyncObject, NotificationEvent, FALSE );    
}

CKl_Event::~CKl_Event()
{

}

void
CKl_Event::Set()
{
    KlSetEvent( m_pSyncObject );
}

void
CKl_Event::Clear()
{
    KlClearEvent( m_pSyncObject );
}

void
CKl_Event::isSignalled()
{
}


//--------------------------------------------------------------
//--------Semaphore --------------------------------------------
//--------------------------------------------------------------

CKl_Semaphore::CKl_Semaphore( ULONG Count ) : m_SemCount( Count )
{
    m_pSyncObject = &m_Event;
    KlInitializeEvent(m_pSyncObject, NotificationEvent, FALSE );
}

CKl_Semaphore::~CKl_Semaphore()
{
}

/*
 *  Увеличивает число Count-ов
 *	Возвращает кол-во Count-ов до Release
 */
ULONG
CKl_Semaphore::Release()
{
    ULONG   count;

    count = InterlockedIncrement( (LONG*)&m_SemCount );

    KlClearEvent( m_pSyncObject );

    return count;
}

ULONG
CKl_Semaphore::Wait( KL_TIME WaitTime )
{
    ULONG   count;

    count = InterlockedDecrement( (LONG*)&m_SemCount );

    if ( m_SemCount == 0 )
    {
        KlWaitEvent( m_pSyncObject );
    }
    
    return count;
}
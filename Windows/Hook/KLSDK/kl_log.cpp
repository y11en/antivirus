#include "kl_log.h"

CKl_Log::CKl_Log(void* LogMethod) : m_LogMethod(LogMethod)
{
    m_Continue  = true;    
    m_LogList   = new CKl_List<CKl_LogItem>();

    KlInitializeEvent( &m_LogEvent, SynchronizationEvent, FALSE );
    
    if ( m_LogThread = new CKl_Thread(&m_StopEvent) )
    {
        m_LogThread->Start( (PKSTART_ROUTINE)CKl_Log::LogRoutine, this );        
    }
}

CKl_Log::~CKl_Log()
{
    m_Continue      = false;    
    KlSetEvent( &m_LogEvent );
	
    m_LogThread->WaitForExit();
    
    delete m_LogThread;
}

KLSTATUS
CKl_Log::AddItem(CKl_LogItem*  Item)
{
    if ( m_Continue )
    {
        m_LogList->InsertTail(Item);
        
        KlSetEvent(&m_LogEvent);
    }
    else
    {
        delete Item;
    }
    
    return KL_SUCCESS;
}

KLSTATUS
CKl_Log::OnThreadStart(PVOID Context)
{
    CKl_LogItem*      Item = NULL;     

    while ( m_Continue )
    {
        KlWaitEvent( &m_LogEvent ); 
        KlClearEvent(&m_LogEvent );
        
        while ( Item = m_LogList->InterlockedRemoveHead() )
        {
            if ( KL_SUCCESS == Item->Process() )
            {
                delete Item;
            }
            else
            {
                m_LogList->InsertHead( Item ) ;
            }
        }
    }
    
    DbgPrint("Log Thread exited\n");
    return KL_SUCCESS;
}

void
CKl_Log::LogRoutine(PVOID Context)
{    
    CKl_Log*        _This					=   (CKl_Log*)Context;    
    CKl_Thread*     _ThisThread				= _This->m_LogThread;    
    CKl_List<CKl_LogItem>*    _ThisLogList	= _This->m_LogList;

    _This->OnThreadStart(NULL);

            
    CKl_LogItem*  Item = NULL;
    while ( Item = _ThisLogList->InterlockedRemoveHead() )
        delete Item;
    delete _ThisLogList;    
}
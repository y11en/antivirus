#include "kl_timer.h"

CKl_TimerObject::CKl_TimerObject( PVOID context, ULONG timeout )
{    
}

CKl_NT_Timer::CKl_NT_Timer(PVOID context, ULONG timeout ): CKl_TimerObject( context, timeout )
{
    InitializeTimer();    
}


#ifdef USER_MODE 

DWORD WINAPI MyThread( LPVOID lpParameter   // thread data
)
{
    DWORD RetVal;
    CKl_NT_Timer* tt = ( CKl_NT_Timer* )lpParameter;

    while ( TRUE )
    {       
        RetVal = WaitForSingleObject( tt->m_Timer, INFINITE );
        
        if ( WAIT_OBJECT_0 != RetVal )
            break;
        
        tt->OnTimerFunc();
    }

    return 0;
}

void
CKl_NT_Timer::InitializeTimer()
{
    m_Timer = CreateWaitableTimer( NULL, FALSE, NULL );
    CreateThread( NULL, 0, MyThread, this, 0, &m_ThreadID  );
}

void
CKl_NT_Timer::SetTimer( ULONG timeout )
{
    BOOL Res;
    LARGE_INTEGER time;
    
    time.QuadPart = -10000 * (LONGLONG)timeout;    

    Res = SetWaitableTimer( m_Timer, &time, 0, NULL, 0, FALSE );
}

BOOLEAN
CKl_NT_Timer::CancelTimer()
{
    BOOLEAN bTimerCancelled;

    bTimerCancelled = CancelWaitableTimer( m_Timer );

    return bTimerCancelled;
}

#else

VOID
__stdcall
MyNdisTimer(
            IN PVOID SystemSpecific1, 
            IN PVOID FunctionContext, 
            IN PVOID SystemSpecific2, 
            IN PVOID SystemSpecific3 )
{
    CKl_TimerObject* MyTimer = (CKl_TimerObject*)FunctionContext;

    if ( MyTimer )
    {
        MyTimer->OnTimerFunc();
    }
}

void
CKl_NT_Timer::InitializeTimer()
{
    NdisInitializeTimer( &m_Timer, MyNdisTimer, this );
}

void
CKl_NT_Timer::SetTimer( ULONG timeout )
{
    NdisSetTimer( &m_Timer, timeout );
}

BOOLEAN
CKl_NT_Timer::CancelTimer()
{
    BOOLEAN bTimerCancelled;

    NdisCancelTimer( &m_Timer, &bTimerCancelled );

    return bTimerCancelled;
}

#endif
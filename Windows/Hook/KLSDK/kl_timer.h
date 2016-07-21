#ifndef _KL_TIMER_OBJECTS_H__
#define _KL_TIMER_OBJECTS_H__

#include "kl_object.h"

class CKl_TimerObject
{    
public :
    CKl_TimerObject( PVOID context, ULONG timeout );    
    
    virtual void    InitializeTimer() = 0;
    virtual BOOLEAN    CancelTimer() = 0;
    virtual void    SetTimer( ULONG timeout ) = 0;

    virtual void    OnTimerFunc() = 0;
};

#ifndef USER_MODE

class CKl_NT_Timer : public CKl_TimerObject
{
    NDIS_TIMER m_Timer;
public:
    CKl_NT_Timer( PVOID context, ULONG timeout );    

    virtual void    InitializeTimer();
    virtual BOOLEAN    CancelTimer();
    virtual void    SetTimer( ULONG timeout );

    virtual void    OnTimerFunc() = 0;
};

#else

class CKl_NT_Timer : public CKl_TimerObject
{
    
public:
    HANDLE  m_Timer;
    DWORD   m_ThreadID;
    CKl_NT_Timer( PVOID context, ULONG timeout );

    virtual void    InitializeTimer();
    virtual BOOLEAN    CancelTimer();
    virtual void    SetTimer( ULONG timeout );
};


#endif

#endif
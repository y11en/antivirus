#include "klopevent.h"

#ifdef ISWIN9X
void 
WaitUlong(ULONG* pEvent)
{
    Begin_Nest_Exec();
    while ( *pEvent == 0 )
    {
        Resume_Exec();
        Time_Slice_Sleep(0);
    }
    End_Nest_Exec();   
}
#endif

CKl_KlopEvent::CKl_KlopEvent()
{
	KlInitializeEvent( &m_Event, NotificationEvent, FALSE );
	Clear();	
}

CKl_KlopEvent::~CKl_KlopEvent()
{
}

void
CKl_KlopEvent::Set()
{
#ifdef ISWIN9X
	m_Event9x = 1;
#else
	KlSetEvent(&m_Event);
#endif
}

void
CKl_KlopEvent::Wait()
{
#ifdef ISWIN9X
	WaitUlong(&m_Event9x);
#else
	KlWaitEvent(&m_Event);
#endif
}

void
CKl_KlopEvent::Clear()
{
#ifdef ISWIN9X
	m_Event9x = 0;
#else
	KlClearEvent(&m_Event);
#endif
}
#include "g_thread.h"
#pragma hdrstop

CKl_List<CKl_Thread>*   pThreadList = NULL;

CKl_Thread::CKl_Thread(KL_EVENT*	Event) : CKl_Object(), m_started(FALSE), m_terminated(FALSE)
{
    /*
	m_pTermEvent = Event;

	if ( m_pTermEvent )
		KlInitializeEvent(m_pTermEvent, NotificationEvent, FALSE);
*/
}

CKl_Thread::~CKl_Thread()
{    
}

#ifdef USER_MODE
/*   ///////////////////////////////////////////////////
 *	 //////////// USER MODE definitions ////////////////
 */  ///////////////////////////////////////////////////
#include <process.h>    /* _beginthread, _endthread */

VOID CreateThreadWrapper(PSTART_PARAMS pSP)
{
	pSP->StartRoutine(pSP->StartContext);
	KL_MEM_FREE ( pSP );

    _endthread();
}

void
CKl_Thread::WaitForExit()
{
    // KlWaitEvent(m_pObject);
    KlSleep(5);
}


HANDLE CKl_Thread::Start(
                            PKSTART_ROUTINE ThreadMain, 
                            PVOID           ThreadParam)
{
    if ( PSTART_PARAMS   pSP = (PSTART_PARAMS)KL_MEM_ALLOC( sizeof (START_PARAMS) ) )
    {
        m_parameter       = ThreadParam;
        pSP->StartContext = ThreadParam;
        pSP->StartRoutine = ThreadMain;		

        m_Handle = (PVOID)_beginthread( (PKSTART_ROUTINE)CreateThreadWrapper, 0, pSP );
    }
    return m_Handle;
}

VOID CKl_Thread::Terminate(KLSTATUS status)
{
    _endthread();
}
#else // USER_MODE

#ifndef ISWIN9X

//-------------------------------------------
//------------- WINNT	definitions ---------
//-------------------------------------------

VOID CreateThreadWrapper(PSTART_PARAMS pSP)
{
	PVOID*	object = (void**)pSP->pObject;

	pSP->StartRoutine(pSP->StartContext);
	KL_MEM_FREE ( pSP );

//	ObDereferenceObject(*object);
    PsTerminateSystemThread(STATUS_SUCCESS);
}

void
CKl_Thread::WaitForExit()
{
    KlWaitEvent(m_pObject);
}

HANDLE CKl_Thread::Start(
                            PKSTART_ROUTINE ThreadMain, 
                            PVOID           ThreadParam)
{	
    if ( PSTART_PARAMS   pSP = (PSTART_PARAMS)KL_MEM_ALLOC( sizeof ( START_PARAMS ) ) )
    {
        OBJECT_ATTRIBUTES   oa;

        m_parameter         = ThreadParam;
        pSP->StartContext   = ThreadParam;
        pSP->StartRoutine   = ThreadMain;        
		pSP->pObject		= &m_pObject;

        InitializeObjectAttributes(&oa, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);
        
        if ( NT_SUCCESS( PsCreateSystemThread( &m_Handle, THREAD_ALL_ACCESS, &oa, NULL, NULL, (PKSTART_ROUTINE)CreateThreadWrapper, pSP )) )
        {
            ObReferenceObjectByHandle( m_Handle, THREAD_ALL_ACCESS, NULL, KernelMode, &m_pObject, NULL );
			ZwClose(m_Handle);
            return m_pObject;
        }
    }

	return NULL;
}

VOID CKl_Thread::Terminate(KLSTATUS status)
{
//    PsTerminateSystemThread(STATUS_SUCCESS);
	ObDereferenceObject(m_pObject);
}

#else // ISWIN9X

//-------------------------------------------
//------------- WIN9x	definitions ---------
//-------------------------------------------
VOID __fastcall CreateThreadWrapper(DWORD dwReserved,PSTART_PARAMS pSP)
{
	THREADHANDLE  Handle = Get_Cur_Thread_Handle();	

	pSP->StartRoutine(pSP->StartContext);
	KL_MEM_FREE ( pSP );

    VMMTerminateThread(Handle);	
}

void
CKl_Thread::WaitForExit()
{
    KlWaitEvent((KL_EVENT*)&m_pObject);
}

HANDLE	StartThread(PVOID	FuncAddr, PVOID	Context)
{
	PSTART_PARAMS	pSP;
	PTCB			Handle;
	BOOLEAN			Result;
	ULONG			WinVer;
	PVOID			r3Handle;
		
	WinVer = Get_VMM_Version(NULL);		

    if ( pSP = (PSTART_PARAMS)KL_MEM_ALLOC ( sizeof (START_PARAMS) ) )
    {
        pSP->StartRoutine   = (PKSTART_ROUTINE)FuncAddr;
        pSP->StartContext   = Context;   // Parameter for CompleteRoutine        
        
        if (WinVer == 0x45A ||		// WinME
            WinVer == 0x40A)		// Win98
        {		
            if ( Handle	= (PTCB)VMMCreateThreadEx (0,0,0,0,0,0, 0x10121973, (THREADINITPROC)CreateThreadWrapper, (DWORD)pSP ) )
            {                
                // thread is created. So Set thread priority now
                Result = Set_Thread_Win32_Pri((THREADHANDLE)Handle , 0x8);
            }
            else
            {
                // thread not started
            }
        }
        else
        {
            Handle = (PTCB)VWIN32_CreateRing0Thread(4096, (ULONG)Context, (PVOIDFUNCOFDW)FuncAddr, FALSE, &r3Handle);
        }
    }
    
    return Handle;
}

HANDLE CKl_Thread::Start(PKSTART_ROUTINE ThreadMain, PVOID ThreadParam /* = NULL */)
{
    m_parameter         = ThreadParam;
    KlInitializeEvent((KL_EVENT*)&m_pObject, NotificationEvent, FALSE);

    m_Handle = StartThread( ThreadMain, ThreadParam );

    if ( pThreadList )
        pThreadList->InsertTail( this );

    return m_pObject;
}

#endif // ISWIN9X
#endif // USER_MODE
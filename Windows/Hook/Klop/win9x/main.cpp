#define   DEVICE_MAIN
#include  "main.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(KLOP)

#include "kldef.h"
#include "klstatus.h"
#include "g_thread.h"
#include "kl_registry.h"
#include "hook.h"

DefineControlHandler(DEVICE_INIT,			    OnDeviceInit);
DefineControlHandler(W32_DEVICEIOCONTROL,	    OnW32Deviceiocontrol);
DefineControlHandler(SYS_DYNAMIC_DEVICE_INIT,   OnSysDynamicDeviceInit);
DefineControlHandler(SYS_DYNAMIC_DEVICE_EXIT,   OnSysDynamicDeviceExit);
DefineControlHandler(DESTROY_THREAD,            OnDestroyThread);


BOOL __cdecl ControlDispatcher(
							   DWORD dwControlMessage,
							   DWORD EBX,
							   DWORD EDX,
							   DWORD ESI,
							   DWORD EDI,
							   DWORD ECX)
{
	START_CONTROL_DISPATCH	
		
	ON_W32_DEVICEIOCONTROL      ( OnW32Deviceiocontrol   );
	ON_DEVICE_INIT              ( OnDeviceInit           );	
    ON_SYS_DYNAMIC_DEVICE_INIT  ( OnSysDynamicDeviceInit );
    ON_SYS_DYNAMIC_DEVICE_EXIT  ( OnSysDynamicDeviceExit );
    ON_DESTROY_THREAD           ( OnDestroyThread        );
	
    END_CONTROL_DISPATCH
        
    return TRUE;
}

/*
 *	Чтобы рулить потоками, нужно знать, когда уничтожается наш поток.
 *  Сделаем список потоков, и если находим собственный поток, то удаляем из списка и ставим Event.
 */

BOOL
HookNetwork();

bool
isWinME()
{
	DWORD dwVersion = Get_VMM_Version( NULL );

	return ( dwVersion == 0x45A ) ? true : false;
}

VOID OnDestroyThread(THREADHANDLE hThread)
{
    if ( pThreadList )
    {
        CKl_Thread* MyThread = pThreadList->Find_PTR( FIELD_OFFSET(CKl_Thread, m_Handle), (void*)hThread );
        
        if ( MyThread )
        {
            pThreadList->InterlockedRemove( MyThread );            
            KlSetEvent((KL_EVENT*)&MyThread->m_pObject);
        }
    }    
}

BOOL
Initialize()
{
	BOOL	Res = TRUE;
	//CKl_Reg Reg;
	
	/*
	if ( KL_SUCCESS == Reg.OpenKey(L"\\registry\\machine\\system\\CurrentControlSet\\Services\\VxD\\KL1") )
	{
	wchar_t	Buffer[100];	
	ULONG	Size = sizeof ( Buffer );
	ULONG	Type = REG_SZ;
	wchar_t	Name[100];
	ULONG   NameSize = sizeof ( Name );
	
	  Reg.CreateSubKey(L"SubKey");
	  Reg.EnumValues(2, (PWCHAR)Name, &NameSize );
	  Reg.QueryValue(L"TraceFile", &Type, Buffer, &Size );
	  Reg.CloseKey();
	  }
	*/
	// NdisInitializeWrapper(&WrapperHandle, NULL, NULL, NULL );
	
	//if ( isWinME() )
	
	{
		isWin98 = TRUE;
	}	
	
	
	AllocateSpinLock( &g_PCP_Lock );
	
	Res = HookNdis();
	
	return Res;
}

BOOL OnSysDynamicDeviceInit()
{   
	return Initialize();
}


BOOL OnSysDynamicDeviceExit()
{
	return FALSE;
	
    if ( pThreadList )
    {
        delete pThreadList;
        pThreadList = NULL;
    }

	PCP_InterlockedDone();
	CP_Done();
    
	// NdisTerminateWrapper( WrapperHandle, NULL );
	return TRUE;
}


BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{	
	return Initialize();
}

DWORD OnW32Deviceiocontrol(PIOCTLPARAMS pDIOCParams)
{
    DWORD   ret                 = 0;
    *pDIOCParams->dioc_bytesret = 0;
	
    switch( pDIOCParams->dioc_IOCtlCode ) 
    {
    case DIOC_OPEN:
		break;
    case DIOC_CLOSEHANDLE:
        break;
		
    default:
        {
            switch ( KlopDispatch( pDIOCParams->dioc_IOCtlCode,
								   pDIOCParams->dioc_InBuf, 
								   pDIOCParams->dioc_cbInBuf, 
								   pDIOCParams->dioc_OutBuf, 
								   pDIOCParams->dioc_cbOutBuf, 
								   pDIOCParams->dioc_bytesret ) )
            {
            case KL_SUCCESS :
                ret = DEVIOCTL_NOERROR;
                break;
            case KL_BUFFER_TOO_SMALL:
                ret = 1;
                break;
                
            default:
                ret = DEVIOCTL_NOERROR;
                break;
            }
			
        }
        break;
		
    }
	return ret;
}

#define   DEVICE_MAIN
#include  "kl1.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device( KL1 )

DefineControlHandler(DEVICE_INIT,			    OnDeviceInit);
DefineControlHandler(W32_DEVICEIOCONTROL,	    OnW32Deviceiocontrol);
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
		
	ON_W32_DEVICEIOCONTROL  ( OnW32Deviceiocontrol  );
	ON_DEVICE_INIT          ( OnDeviceInit          );
    ON_DESTROY_THREAD       ( OnDestroyThread       );
	
    END_CONTROL_DISPATCH
    
    return TRUE;
}

VOID OnDestroyThread(THREADHANDLE hThread)
{
    if ( pThreadList )
    {
        CKl_Thread* MyThread = pThreadList->Find_PTR( FIELD_OFFSET(CKl_Thread, m_Handle), (PVOID)hThread );
        
        if ( MyThread )
        {
            pThreadList->Remove( MyThread );
            KlSetEvent( (KL_EVENT*)&MyThread->m_pObject );
        }
    }
}

BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{
    LoaderInitialize();

    LoadBoot0();
	
	return TRUE;
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
    case KL1_GET_VERSION :
        {   
            ULONG	Version;
            
            Version = KL1_BUILDNUM;
            RtlCopyMemory (pDIOCParams->dioc_OutBuf, &Version, sizeof(Version));				
            *pDIOCParams->dioc_bytesret = sizeof(Version);                    
            
            break;
        }

    case KL1_LOAD_MODULE:
        {
            PKL1_MODULE_CONTEXT pContext = (PKL1_MODULE_CONTEXT)pDIOCParams->dioc_InBuf;
            
            KlLoadModule( pContext->ModuleName, pContext->ModulePath, false );
        }
        
        break;
    case KL1_UNLOAD_MODULE:
        {
            PKL1_MODULE_CONTEXT pContext = (PKL1_MODULE_CONTEXT)pDIOCParams->dioc_InBuf;
            
            KlUnloadModule( pContext->ModuleName );
        }            
        break;
		
    default:
        break;
    }
	return ret;
}

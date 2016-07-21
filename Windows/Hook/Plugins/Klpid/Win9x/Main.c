#define   DEVICE_MAIN
#include  "precomp.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(KLPID)

NTSTATUS	MySetupDriver();

DefineControlHandler(DEVICE_INIT,			OnDeviceInit);
DefineControlHandler(W32_DEVICEIOCONTROL,	OnW32Deviceiocontrol);
DefineControlHandler(KERNEL32_INITIALIZED,	OnKernel32Init);

BOOL __cdecl ControlDispatcher(
							   DWORD dwControlMessage,
							   DWORD EBX,
							   DWORD EDX,
							   DWORD ESI,
							   DWORD EDI,
							   DWORD ECX)
{
	START_CONTROL_DISPATCH	
	
	ON_W32_DEVICEIOCONTROL(OnW32Deviceiocontrol);
	ON_DEVICE_INIT(OnDeviceInit);
	ON_KERNEL32_INITIALIZED(OnKernel32Init);
	
	END_CONTROL_DISPATCH
		
		return TRUE;
}

VOID
OnKernel32Init()
{	
	return;
}

BOOL OnDeviceInit(VMHANDLE hVM, PCHAR CommandTail)
{	
	ULONG			Status = 0;

    InitializeGlobalStore();

    InitializeStealthMode();

    MySetupDriver();

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
    
    default:
        {
            switch ( MyIoctlHandler( 
                            pDIOCParams->dioc_IOCtlCode,
                            pDIOCParams->dioc_InBuf,
                            pDIOCParams->dioc_cbInBuf,
                            pDIOCParams->dioc_OutBuf,
                            pDIOCParams->dioc_cbOutBuf,
                            pDIOCParams->dioc_bytesret ) )
            {
            case KL_SUCCESS :
                ret = 0;
                break;
            default:
                ret = 0;
                break;
            }

        }
        break;
    }
	return ret;
}

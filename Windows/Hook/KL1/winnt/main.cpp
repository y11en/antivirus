/********************************************************************/
/**                     Kaspersky Lab  PE  Loader                  **/
/**               Copyright(c) Kaspersky Lab., 2003-20004          **/
/********************************************************************/

// Драйвер - загрузчик PE-можулей в память.
// Файл содержит стартовые функции. DriverEntry, Dispatcher
// 
//    
//

//  Author - Eugene Roschin     21.01.2005

#include "..\loader.h"
#include "..\..\HOOK\kl1_api.h"
#include "kl_registry.h"
#include "main.h"

#pragma warning ( disable: 4273 )

#define g_ntContext myctx
extern NT_CONTEXT*  g_ntContext;

UCHAR   NotHooked = 2;
BOOLEAN   TcpipLoaded = FALSE;

t_ZwOpenFile r_ZwOpenFile;

#define DbPrint(_x_, _y_, _z_)

PDRIVER_OBJECT  pMyDrvObj = NULL;

#define SAFE_MODE_KEY L"\\registry\\machine\\system\\CurrentControlSet\\Control"

KMUTEX  g_LoadMutex;

bool
isSafeMode()
{
	bool    isSafe = false;
    CKl_Reg Reg;
    
	if ( KL_SUCCESS == Reg.OpenKey( SAFE_MODE_KEY ) )
	{
		wchar_t BootOptions[512];
		ULONG   Type = REG_SZ;
		ULONG   Size = sizeof ( BootOptions );

		if ( KL_SUCCESS == Reg.QueryValue( L"SystemStartOptions", &Type, BootOptions, &Size ) )
		{
			isSafe = ( NULL != wcsstr(BootOptions, L"SAFEBOOT") );
		}

		Reg.CloseKey();
	}

    return isSafe;
}
ULONG dw_OsMajor, dw_OsMinor;

bool
ShouldNotPatch()
{
    if ( sizeof (PVOID) == 0x8 )
        return true;

    if ( dw_OsMajor == 0x6 )
        return true;

    return FALSE;
}

bool
InWinNT40()
{
    if ( dw_OsMajor * 10 + dw_OsMinor  == 40 )
        return true;

    return false;
}

NTSTATUS
hZwOpenFile(
            OUT	PHANDLE				FileHandle,
            IN	ACCESS_MASK			DesiredAccess,
            IN	POBJECT_ATTRIBUTES	ObjectAttributes,
            OUT PIO_STATUS_BLOCK	IoStatusBlock,
            IN	ULONG				ShareAccess,
            IN	ULONG				OpenOptions
            )
{    
    if ( NotHooked )
    {
        if ( ObjectAttributes->ObjectName->Buffer )
        {
            ULONG   Length1 = wcslen ( L"\\SystemRoot\\System32\\DRIVERS\\" );
            ULONG   Length  = wcslen ( ObjectAttributes->ObjectName->Buffer );

            if ( Length >= Length1 )
            {
                UNICODE_STRING  us1, us2, tcpip;
                // wchar_t buf1[100];                 

                // RtlZeroMemory ( buf1, sizeof ( buf1 ) );
                // RtlCopyMemory ( buf1, ObjectAttributes->ObjectName->Buffer, Length1 * 2 );

                // RtlInitUnicodeString(&us1, buf1);
                // RtlInitUnicodeString(&us2, L"\\SystemRoot\\System32\\DRIVERS\\");
                RtlInitUnicodeString(&tcpip, L"\\SystemRoot\\System32\\DRIVERS\\tcpip.sys");

                /*
                if ( TcpipLoaded && RtlEqualUnicodeString( &us1, &us2, TRUE ) )
                {
                    if ( NotHooked == 1 )                        
                    {   
                        LoadBoot0();
                    }

                    NotHooked--;
                }
                */
                // DbgPrint("Load %ls\n", ObjectAttributes->ObjectName->Buffer );

                /*
                if ( TcpipLoaded && RtlEqualUnicodeString( &us1, &us2, TRUE ) )
                {
                    // загрузка драйвера следующего после tcpip.sys
                    LoadBoot0();
                    NotHooked = 0;
                }
                */

                if ( !TcpipLoaded && RtlEqualUnicodeString( ObjectAttributes->ObjectName, &tcpip, TRUE ) )
                {
                    TcpipLoaded = TRUE;

                    LoadBoot0();
                    NotHooked = 0;
                }
            }
        }        
    }

    return r_ZwOpenFile(  FileHandle,DesiredAccess,ObjectAttributes,IoStatusBlock,ShareAccess, OpenOptions );
}

NTSTATUS	
Dispatch(
		   IN PDEVICE_OBJECT	DeviceObject,
		   IN PIRP				Irp
)
{
	NTSTATUS			ntStatus	= STATUS_SUCCESS;
	PIO_STACK_LOCATION	IrpSp		= IoGetCurrentIrpStackLocation(Irp);
	PVOID				ioBuffer	= Irp->AssociatedIrp.SystemBuffer;
	ULONG               inBufLength = IrpSp->Parameters.DeviceIoControl.InputBufferLength;
    ULONG               outBufLength= IrpSp->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG               IOCTL_Code	= IrpSp->Parameters.DeviceIoControl.IoControlCode;

    PDRIVER_OBJECT      DriverObject = FindDriver(DeviceObject);

    // Вызов Dispatcher соответствующего модуля.
    
    if ( DriverObject )
    {        
        ntStatus = DriverObject->MajorFunction[IrpSp->MajorFunction](DeviceObject, Irp);        
        return ntStatus;
        
    }

    if ( IrpSp->MajorFunction == IRP_MJ_DEVICE_CONTROL )
    {
        switch( IOCTL_Code ) 
        {
        case KL1_GET_VERSION :
            {   
                ULONG	Version;
                CHECK_OUT_BUFFER_SIZE( sizeof( ULONG ) );
                
                Version = KL1_BUILDNUM;
                RtlCopyMemory (ioBuffer, &Version, sizeof(Version));				
                Irp->IoStatus.Information = sizeof(Version);                    
                
                break;
            }
        case KL1_LOAD_MODULE:
            {
                CHECK_IN_BUFFER_SIZE( sizeof ( KL1_MODULE_CONTEXT ) );

                PKL1_MODULE_CONTEXT pContext = (PKL1_MODULE_CONTEXT)ioBuffer;

                KeWaitForSingleObject( &g_LoadMutex, Executive, KernelMode, FALSE, NULL );

                KlLoadModule( pContext->ModuleName, pContext->ModulePath, false);

                KeReleaseMutex( &g_LoadMutex, FALSE );
            }
            
        	break;
        case KL1_UNLOAD_MODULE:
            {
                CHECK_IN_BUFFER_SIZE( sizeof ( KL1_MODULE_CONTEXT ) );

                PKL1_MODULE_CONTEXT pContext = (PKL1_MODULE_CONTEXT)ioBuffer;

                KlUnloadModule( pContext->ModuleName );
            }            
            break;
        case 0:
            {
                // Это нужно специально для того, чтоб были зависимости от NDIS.sys и TDI.sys
				NDIS_STATUS Status;
                NDIS_HANDLE Handle;
                NdisAllocatePacketPool(&Status, &Handle, 1, 16);
                NdisFreePacketPool( Handle );

                TdiMapUserRequest( DeviceObject, Irp, IrpSp );
            }
        default:
            DbgPrint ("IOCTL\n");
            break;
        }
    }
    

    Irp->IoStatus.Status = ntStatus;    
	IoCompleteRequest(Irp, IO_NO_INCREMENT);	

	return ntStatus;
}

extern "C" 
NTSTATUS 
DriverEntry ( 
			 PDRIVER_OBJECT		DriverObject, 
			 PUNICODE_STRING	RegistryPath 
)
{
	NTSTATUS                ntStatus;
    UNICODE_STRING          DeviceName, DeviceLinkName;
    PDEVICE_OBJECT          DeviceObject;  
/*
    DbgBreakPoint();

    CKl_Reg reg;

    if ( KL_SUCCESS == reg.OpenKey( RegistryPath->Buffer ) )
    {
        DWORD val = 1;
        reg.SetValue( L"Start", REG_DWORD, &val, sizeof ( DWORD ) );
        reg.CloseKey();
        return STATUS_UNSUCCESSFUL;
    }
*/
	if ( isSafeMode() )
	{
		return STATUS_UNSUCCESSFUL;
	}

    PsGetVersion( &dw_OsMajor, &dw_OsMinor, 0, 0 );

    pMyDrvObj = DriverObject;
    
    RtlInitUnicodeString(&DeviceName,		NTDEVICE_STRING);
	RtlInitUnicodeString(&DeviceLinkName,	LINKNAME_STRING);

    if ( !NT_SUCCESS( ntStatus = IoCreateDevice( 
                                                DriverObject, 
                                                0, 
                                                &DeviceName, 
                                                FILE_DEVICE_UNKNOWN, 
                                                0, 
                                                FALSE, 
                                                &DeviceObject )))
    {
        return ntStatus;
    }

	for ( int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i )
	{
		DriverObject->MajorFunction[i] = Dispatch;
	}

	DriverObject->DriverUnload = NULL;

	if ( !NT_SUCCESS(ntStatus = IoCreateSymbolicLink(&DeviceLinkName, &DeviceName)))
    {
        IoDeleteDevice(DeviceObject);
        return ntStatus;
    }

    KeInitializeMutex( &g_LoadMutex, 0 );

	g_ntContext = (NT_CONTEXT*)ExAllocatePoolWithTag(NonPagedPool, sizeof ( NT_CONTEXT ), 'gpll' );

    if ( g_ntContext )
    {
        g_ntContext->DriverObject = DriverObject;
        g_ntContext->RegPath      = *RegistryPath;
        
        LoaderInitialize(  );
    }

	DbgPrint ( "Kl1 DriverObject = %x, %x\n", DriverObject, g_ntContext );

    /*
     *	Here is some botva happends ...
     *  If you touch a file during boot=0 using \??\ notation
     *  there is a problem with chkdsk
     *  Solution : patch NtOpenFile. And check, if the first driver is loading, load our drivers and continue.
     */

    if ( InWinNT40() )    
        NotHooked = 5;
    
#ifndef _WIN64

    if ( ShouldNotPatch() )
    {
        LoadBoot0();
    }
    else
    {
        __asm { // снимаем защиту памяти от записи 
            mov  ebx , cr0
            push ebx
            and  ebx , ~0x10000
            mov  cr0 , ebx
        }

        ULONG   ii        = 0;
        ULONG   NumbOfSrv = 0x150;
        PVOID   TableFunc = NULL;
        PVOID   MyFunc    = NtOpenFile;          // this jmp to import. 
        PVOID   MyZwFunc    = ZwOpenFile;       
        ULONG   FuncID = 0;

        MyFunc = *(PVOID*)( (char*)MyFunc + 2 ); // skip jmp command
        MyFunc = *(PVOID*)( MyFunc );            // and get func addr

        MyZwFunc = *(PVOID*)( (char*)MyZwFunc + 2 ); // skip jmp command
        MyZwFunc = *(PVOID*)( MyZwFunc );            // and get func addr   

        if ( ((unsigned char*)MyZwFunc)[0] == (unsigned char)0xb8 )
        {
            FuncID = ((unsigned char*)MyZwFunc)[1];
        }

        r_ZwOpenFile = (t_ZwOpenFile) *( *KeServiceDescriptorTable + FuncID );
        *( *KeServiceDescriptorTable + FuncID ) = ( ULONG )hZwOpenFile;

        __asm {	
            pop   ebx 	
            mov   cr0 , ebx    
        }
    }
    
#else
    LoadBoot0();
#endif

    return STATUS_SUCCESS;
}
#include "loader.h"
#include "stdlib.h"
#include "hook\kl1_api.h"

#include "winnt\sign_check.h"

#pragma warning ( disable: 4273 )

/*
 *	 Философия :
 *
 *   В данной модели драйвера загружаются не операционной системой ( IoManager ), нашим загрузчиком.
 *   Возникают попросы создания объектов, использующих DRIVER_OBJECT. Понятно, что при создании DeviceObject
 *   Необходимо использовать DriverObject загрузчика ( поскольку только он зарегистрирован в системе. )
 *   С другой стороны перебивать Dispatch - функции DRIVER_OBJECT загрузчика - есть вещь неправильная.
 *   Для того чтобы все было хорошо, создаем собственный DRIVER_OBJECT с особый типом.
 *   В загружаемый модуль передаем этот DRIVER_OBJECT. В нем необходимо будет сделать проверку :
 *   Если тип DRIVER_OBJECT новый, то по смещению sizeof ( DRIVER_OBJECT ) + sizeof ( DRIVER_EXTENSION ) лежит NT_CONTEXT,
 *   содержащий указатель на DRIVER_OBJECT драйвера загрузчика. И DeviceObject нужно создавать через него.
 *   Если же тип обычный, то DeviceObject нужно создавать "обычным" способом.
 */

#define CKl_PEModule			Module
#define InitDriverObject		Init
#define Start					Begin
#define Stop					End
#define IopInvalidDeviceRequest Dispatch1
#define g_ntContext				myctx
#define PEModuleList			MyData

#define PARAMETERS_KEY L"\\Parameters\\"


NTSTATUS
IopInvalidDeviceRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    UNREFERENCED_PARAMETER( DeviceObject );

    //
    // Simply store the appropriate status, complete the request, and return
    // the same status stored in the packet.
    //

    Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    return STATUS_INVALID_DEVICE_REQUEST;
}

class Module;

NT_CONTEXT*			g_ntContext; // глобальный контекст Loader-а
CKl_List<Module>*   PEModuleList;

#define  ZeroMemory RtlZeroMemory

wchar_t*
GetSysName( wchar_t* ImageName, wchar_t* OutBuffer )
{    
    ULONG offset = wcslen ( ImageName );

    if ( ImageName )
    {
        while ( offset )
        {
            if ( ImageName[offset] == L'\\' )
            {
                wchar_t* ext;
                //return ImageName + offset + 1;
                
                wcscpy ( OutBuffer, ImageName + offset + 1 );

                ext = wcschr( OutBuffer, L'.' );
                if ( ext )
                {
                    ext[1] = L's';
                    ext[2] = L'y';
                    ext[3] = L's';
                }

                return OutBuffer;
            }
            offset--;
        }
    }

    return ImageName;
}


class Module : public CKl_Object
{
public:
    QUEUE_ENTRY;
    
    PDRIVER_OBJECT          m_DriverObject;
    char*                   m_ModuleName;
    wchar_t*                m_ModulePath;
    PVOID                   m_ModuleLoadBase;
    CKl_ModLoader*          ldr;
    ULONG                   m_CallCount;        // Число заходов внутрь по IOCTL
    
    Module(char*  Name, wchar_t* Path, bool Boot );

    void        InitDriverObject();

    NTSTATUS    Start();    
    NTSTATUS    Stop();

    virtual ~Module();
};

Module::Module(char*  Name, wchar_t* Path, bool Boot ) : ldr(NULL), m_DriverObject(NULL), m_ModuleLoadBase(NULL), m_ModuleName(NULL), m_ModulePath(NULL)
{   
    HANDLE              hFile;
    NTSTATUS            ntStatus;
    OBJECT_ATTRIBUTES   ObjAttr;	
    UNICODE_STRING      FileName;
    IO_STATUS_BLOCK     ioStatus;
    FILE_STANDARD_INFORMATION   fi;
    ULONG               FileSize = 0;
    wchar_t             Buf[1000];
    
    if ( m_ModuleName = (char*)KL_MEM_ALLOC ( strlen( Name) + 1 ) )
    {
        strcpy ( m_ModuleName, Name );
    }
    
    if ( m_ModulePath = (wchar_t*)KL_MEM_ALLOC ( (wcslen( Path) + 1) << 1 ) )
    {
        wcscpy ( m_ModulePath, Path );            
    }
    
    if ( Boot )
    {
        swprintf ( Buf, L"\\SystemRoot\\System32\\DRIVERS\\%s", GetModuleName(m_ModulePath) );
    }
    else
    {
        swprintf ( Buf, L"\\??\\%s", m_ModulePath);
    }
    
    RtlInitUnicodeString(&FileName, Buf);
    
    InitializeObjectAttributes(&ObjAttr, &FileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	ntStatus = ZwCreateFile( 
					&hFile,  
					FILE_READ_DATA | SYNCHRONIZE, 
					&ObjAttr,  
					&ioStatus, 
					NULL,  
					FILE_ATTRIBUTE_NORMAL,
					FILE_SHARE_READ, 
					FILE_OPEN, 
					FILE_SYNCHRONOUS_IO_NONALERT, 
					NULL,
					0 );
    
    if ( STATUS_SUCCESS != ntStatus )
    {   
        if ( Boot )
        {
            swprintf ( Buf, L"\\??\\%s", m_ModulePath );
            
            RtlInitUnicodeString( &FileName, Buf );
        }
        
        ntStatus = ZwCreateFile( 
					&hFile,  
					FILE_READ_DATA | SYNCHRONIZE,
					&ObjAttr,  
					&ioStatus, 
					NULL,  
					FILE_ATTRIBUTE_NORMAL,
					FILE_SHARE_READ, 
					FILE_OPEN, 
					FILE_SYNCHRONOUS_IO_NONALERT, 
					NULL,
					0 );
    }
    
    
    if ( STATUS_SUCCESS == ntStatus )
    {
		ntStatus = ZwQueryInformationFile(
									hFile, 
									&ioStatus, 
									&fi, 
									sizeof (fi), 
									FileStandardInformation);
		
        if ( NT_SUCCESS( ntStatus ) )
        {
            FileSize =  fi.EndOfFile.LowPart;
        }
        
        if ( FileSize )
        {
            if ( m_ModuleLoadBase = KL_MEM_ALLOC( FileSize ) ) 
            {   
				ntStatus = ZwReadFile( 
									hFile, 
									NULL, 
									NULL, 
									NULL, 
									&ioStatus, 
									m_ModuleLoadBase, 
									FileSize, 
									0, 
									NULL);

                // unXOR loaded file
                static unsigned char KL1Key[] = KL1_KEY;
                for (unsigned long i = 0; i < FileSize; ++i)
                {
                    ((char*)m_ModuleLoadBase)[i] ^= KL1Key[i % KL1_KEY_SIZE];
                }
                
				if ( NT_SUCCESS ( ntStatus ) )
                {
                    wchar_t SysName[20];
                    // Файл может быть либо подписан старой подписью, либо нет.
                    // Проверим оба варианта. Если подпись совпадет хотя бы 
                    // в 1-м случа, то считаем, что все хорошо.
					if (
                        SignCheck( (PCHAR)m_ModuleLoadBase, FileSize, GetSysName( Path, SysName ) )
                        // SignCheck( (PCHAR)m_ModuleLoadBase, FileSize - OLD_SIGN_SIZE, PubKey, strlen(PubKey) )
                        
                        // TRUE
                        )
					{
						ldr = new CKl_ModLoader( m_ModuleLoadBase );
						
						if ( ldr )
						{
							InitDriverObject();
							
							ldr->PrepareForExec( (PVOID*)&m_DriverObject->DriverInit );
						}
					}                     
					else
					{
						ntStatus = STATUS_UNSUCCESSFUL;
					}                     
                }
            }
        }
        
        ZwClose( hFile );
    }
}

CKl_PEModule::~CKl_PEModule()
{
    if ( m_ModuleName )
        KL_MEM_FREE ( m_ModuleName );
    
    if ( m_DriverObject )
        KL_MEM_FREE ( m_DriverObject );
    
    if ( m_ModuleLoadBase )
        KL_MEM_FREE ( m_ModuleLoadBase );
    
    if ( ldr )
        delete ldr;
}

void    
CKl_PEModule::InitDriverObject()
{
    ULONG   dwSize = sizeof ( DRIVER_OBJECT ) + sizeof (DRIVER_EXTENSION) + sizeof ( NT_MOD_CONTEXT );
    
	m_DriverObject = (DRIVER_OBJECT*)KL_MEM_ALLOC( dwSize );

	if ( m_DriverObject )
	{
		RtlZeroMemory ( m_DriverObject ,  dwSize );
		
		m_DriverObject->DriverExtension = (PDRIVER_EXTENSION) (m_DriverObject + 1);
		m_DriverObject->DriverExtension->DriverObject = m_DriverObject;
		
		for ( int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++ )
		{
			m_DriverObject->MajorFunction[i] = IopInvalidDeviceRequest;
		}
		
		m_DriverObject->Type            = 0; // !!! По нашей архитектуре должно быть 0 !
		m_DriverObject->Size            = sizeof( DRIVER_OBJECT );        
		m_DriverObject->DriverSection   = NULL;
		m_DriverObject->DriverStart     = ldr->m_base;
		m_DriverObject->DriverSize      = 0;
		
		NT_MOD_CONTEXT* ModCtx = (NT_MOD_CONTEXT*)( PDRIVER_EXTENSION( m_DriverObject + 1 ) + 1 );
		ModCtx->pg_ntContext = g_ntContext;
	}    
}

NTSTATUS    
CKl_PEModule::Start()
{
    NTSTATUS    ntStatus;
    
    if ( m_DriverObject )
    {
		ntStatus = m_DriverObject->DriverInit ( m_DriverObject, &g_ntContext->RegPath );

        if ( ntStatus == STATUS_SUCCESS )
        {
			NT_MOD_CONTEXT* ModCtx = (NT_MOD_CONTEXT*)( PDRIVER_EXTENSION( m_DriverObject + 1 ) + 1 );
			ULONG			i      = 0;
			
			while ( i < MAX_DEV_OBJ && ModCtx->DevObj[i] )
			{
				ModCtx->DevObj[i++]->Flags &= ~DO_DEVICE_INITIALIZING;
			}
			
			PEModuleList->InsertTail( this );
        }
        
        return ntStatus;
    }
	
    return STATUS_UNSUCCESSFUL;
}

NTSTATUS    
CKl_PEModule::Stop()
{
    if ( m_DriverObject )
    {
        if ( m_DriverObject->DriverUnload )
        {
            PEModuleList->InterlockedRemove( this );
            
            if ( m_DriverObject->DriverUnload )
			{
				m_DriverObject->DriverUnload( m_DriverObject );
				return STATUS_SUCCESS;
			}			
            
            return STATUS_UNSUCCESSFUL;
        }
    }
    
    
    return STATUS_UNSUCCESSFUL;
}


//--------------------------------------------------------------------------------
PDRIVER_OBJECT  
FindDriver(PDEVICE_OBJECT DevObj)
{
    CKl_PEModule*   Module = NULL;
    
    PDRIVER_OBJECT  ModDrvObj = NULL;
    PDEVICE_OBJECT  ModDevObj = NULL;
    
    // Перебираем все драйвера
	while ( Module = PEModuleList->Next(Module) )
    {
        if ( ModDrvObj = Module->m_DriverObject )
        {
            // Если драйвер содержит указанный DevObj, значт это наш драйвер
			if ( FindDevice( DevObj, ModDrvObj ) )
                return ModDrvObj;
        }
    }
    
    return NULL;
}

KLSTATUS
LoaderInitialize()
{
    BootState   = BOOT0;

    PEModuleList = new CKl_List<CKl_PEModule>();
    
    PUNICODE_STRING OldRegPath = &g_ntContext->RegPath;

    wchar_t* Buffer = (wchar_t*)KL_MEM_ALLOC ( OldRegPath->MaximumLength + 2 );
	
    if ( Buffer )
    {
        RtlZeroMemory ( Buffer, OldRegPath->MaximumLength + 2);
        RtlCopyMemory ( Buffer, OldRegPath->Buffer, OldRegPath->MaximumLength );
        RtlInitUnicodeString( &g_ntContext->RegPath, Buffer );
		
		return KL_SUCCESS;
    }

	return KL_UNSUCCESSFUL;
}

VOID
LoaderDeinitialize()
{
}

KLSTATUS
KlLoadModule( char* ModName, wchar_t* ModPath, bool Boot )
{
    CKl_PEModule* Mod = new CKl_PEModule( ModName, ModPath, Boot );

    if ( Mod )
    {
        if ( STATUS_SUCCESS != Mod->Start() )
        {
            delete Mod;
            return KL_UNSUCCESSFUL;
        }

        return KL_SUCCESS;
        
    }

    return KL_UNSUCCESSFUL;
}

KLSTATUS
KlUnloadModule( char* ModName )
{
    CKl_PEModule*   Mod= NULL;

    PEModuleList->Lock();
    while ( Mod = PEModuleList->Next(Mod) )
    {        
        if ( 0 == strcmp ( ModName, Mod->m_ModuleName ) )
        {
            break;
        }
    }
    PEModuleList->Unlock();

    if ( Mod )
    {
        Mod->Stop();
        delete Mod;

        return KL_SUCCESS;
    }

    return KL_UNSUCCESSFUL;
}

KLSTATUS
LoadBoot0()
{   
    NTSTATUS            ntStatus;
    ULONG               i = 0;
    
    wchar_t*            ParamKeyBuffer;
    UNICODE_STRING      KeyStr;
    
    HANDLE              hKey;
    OBJECT_ATTRIBUTES   ObjAttr;
    PKEY_VALUE_PARTIAL_INFORMATION KeyBuffer = NULL;
    ULONG               KeyBufferSize = g_ntContext->RegPath.MaximumLength + 100;

    // инициализируем ключ, где лежат mod1, mod2...modx
    if ( ParamKeyBuffer = (wchar_t*)ExAllocatePoolWithTag( NonPagedPool, KeyBufferSize, COMMON_TAG ) )
    {
        RtlZeroMemory ( ParamKeyBuffer, KeyBufferSize );
        RtlCopyMemory ( ParamKeyBuffer, g_ntContext->RegPath.Buffer, g_ntContext->RegPath.Length );
        RtlCopyMemory ( (char*)ParamKeyBuffer + g_ntContext->RegPath.Length, PARAMETERS_KEY, (wcslen ( PARAMETERS_KEY ) + 1) << 1 );

        RtlInitUnicodeString( &KeyStr , ParamKeyBuffer );
        InitializeObjectAttributes( &ObjAttr, &KeyStr, OBJ_CASE_INSENSITIVE, NULL, NULL );	
        
        if ( STATUS_SUCCESS == (ntStatus = ZwOpenKey ( &hKey, KEY_READ, &ObjAttr )) )
        {   
            // выделяем буфер, куда считаем значение.
            ULONG   RetSize = 300;
            if ( KeyBuffer = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePoolWithTag( NonPagedPool, 300, COMMON_TAG ) )
            {
                for ( i = 0; TRUE; i++ )
                {   
                    RtlZeroMemory ( KeyBuffer, 300 );
                    
                    if ( STATUS_SUCCESS != (ntStatus = ZwEnumerateValueKey (hKey, i, KeyValuePartialInformation, KeyBuffer, 300, &RetSize )))
                    {
                        break;
                    }
                    
                    KlLoadModule( "un", (wchar_t*)KeyBuffer->Data, true );
                }
                
                ExFreePool ( KeyBuffer );
            }

            ZwClose( hKey );
        }
        ExFreePool( ParamKeyBuffer );
    }

    BootState = BOOT1;
    
    return KL_SUCCESS;
}
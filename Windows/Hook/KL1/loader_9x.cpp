#include "loader.h"

class CKl_VXDModule : public CKl_Object
{
public:
    QUEUE_ENTRY;
    char*   m_ModName;
    PDDB    m_pDDB;
    PDEVICEINFO m_DevID;

    CKl_VXDModule(char* Name) : m_ModName(NULL), m_pDDB(NULL), m_DevID(NULL)
    {
        if ( Name )
        {
            if ( m_ModName = (char*)KL_MEM_ALLOC( strlen( Name ) + 1 ) )
            {
                strcpy( m_ModName, Name );
            }
        }
    }
    virtual ~CKl_VXDModule()
    {
        if ( m_ModName )
            KL_MEM_FREE ( m_ModName );
    }
};

CKl_List<CKl_VXDModule>* ModuleList = NULL;

KLSTATUS
LoaderInitialize()
{
    BootState   = BOOT0;

    ModuleList = new CKl_List<CKl_VXDModule>();

    return KL_SUCCESS;
}

VOID
LoaderDeinitialize()
{
}

KLSTATUS
KlLoadModule(char* ModName, wchar_t* ModPath, bool Boot )
{
    PDEVICEINFO		pHandle;
    PDDB			pDDB;
    ULONG			err;
    CKl_VXDModule*  Module = NULL;
    
    DbgPrint ( "Load %s from %ls\n", ModName, ModPath );

    char*   Path = (char*)KL_MEM_ALLOC( wcslen (ModPath) + 1 );

    Unicode2Ansi(Path, ModPath);

    ModuleList->Lock();
    
    while ( Module = ModuleList->Next( Module ) )
    {
        if ( BootState != BOOT0 && 0 == strcmp ( ModName, Module->m_ModName ) )
        {
            ModuleList->Unlock();
            return KL_UNSUCCESSFUL;
        }
    }
    
    ModuleList->Unlock();

	err = VXDLDR_LoadDevice(Path, VXDLDR_INIT_DEVICE, &pHandle, &pDDB );
	
	KL_MEM_FREE ( Path );

    if ( ERROR_SUCCESS == err )
    {
		Module = new CKl_VXDModule( ModName );
        
		if ( Module )
        {
            Module->m_pDDB  = pDDB;
            Module->m_DevID = pHandle;
            ModuleList->InsertTail( Module );
			
			return KL_SUCCESS;
        }
    }
    
    return KL_UNSUCCESSFUL;
}

KLSTATUS
KlUnloadModule( char* ModName )
{
    CKl_VXDModule*  Module = NULL;
    
    if ( ModName )
    {
        ModuleList->Lock();

        while ( Module = ModuleList->Next( Module ) )
        {
            if ( 0 == strcmp ( ModName, Module->m_ModName ) )
            {
				ModuleList->Remove( Module );
                break;
            }
        }

        ModuleList->Unlock();

        if ( Module )
        {            
            VXDLDR_UnloadDevice( -1, Module->m_DevID->DI_ModuleName );
            
            delete Module;

            return KL_SUCCESS;
        }
    }
    
    return KL_UNSUCCESSFUL;
}

KLSTATUS
LoadBoot0()
{
    HKEY hKey;
    ULONG   i = 0;
    
    if ( ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\Vxd\\Kl1\\Parameters", &hKey))
    {
        char    ValueName[100];
        DWORD   ValueNameSize;
        DWORD   ValueType;
        char    Data[300];
        DWORD   DataSize;
        
        for ( i = 0; i < 02000; i++ )
        {            
            RtlZeroMemory ( ValueName, sizeof ( ValueName ) );
            RtlZeroMemory ( Data, sizeof (Data) );

            DataSize = sizeof( Data );
            ValueNameSize = sizeof ( ValueName );

            if ( ERROR_NO_MORE_ITEMS != RegEnumValue(hKey, i, ValueName, &ValueNameSize, NULL, &ValueType, (BYTE*)Data, &DataSize) )
            {
                if ( ValueType == REG_SZ && DataSize > 5 )
                {
                    wchar_t*    Path = (wchar_t*)KL_MEM_ALLOC( ( strlen ( Data ) + 1 ) << 1 );

                    Ansi2Unicode(Path, Data);

                    KlLoadModule("unloadable", Path, true );

                    KL_MEM_FREE ( Path );
                }
            }
            else
            { 
                break;
            }
        }
    }

    BootState = BOOT1;

    return KL_SUCCESS;
}
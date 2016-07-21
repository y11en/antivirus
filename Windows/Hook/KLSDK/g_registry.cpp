#include "g_registry.h"

CKl_Registry::CKl_Registry(HANDLE hKey, TCHAR* tszKeyStr /* = NULL */, ULONG KeyStrSize )
{
    m_hKey = NULL;

    if ( tszKeyStr )
    {
        OpenKey( tszKeyStr, KeyStrSize );
    }
}

CKl_Registry::~CKl_Registry()
{
    CloseKey();
}


#ifdef USER_MODE

KLSTATUS
CKl_Registry::OpenKey(TCHAR* tszKeyStr, ULONG KeyStrSize /* = 0  */)
{
    if ( m_hKey )
        RegCloseKey ( (HKEY)m_hKey );

    if ( ERROR_SUCCESS == RegOpenKey ( HKEY_LOCAL_MACHINE, tszKeyStr, (PHKEY)&m_hKey ) )
    {
        return KL_SUCCESS;
    }
    
    return KL_UNSUCCESSFUL;
}

void
CKl_Registry::CloseKey()
{
    if ( m_hKey )
    {
        RegCloseKey( (HKEY)m_hKey );
        m_hKey = NULL;
    }
}

#else // USER_MODE

#ifndef ISWIN9X
//  ----------------------------------------------
//	-------   WinNT Section ----------------------
//  ----------------------------------------------


KLSTATUS
CKl_Registry::OpenKey( TCHAR* tszKeyStr, ULONG KeyStrSize )
{
    OBJECT_ATTRIBUTES   ObjAttr;
    UNICODE_STRING      KeyStr;
    NTSTATUS            ntStatus;

    TCHAR*      Str = (TCHAR*)KL_MEM_ALLOC ( KeyStrSize + 2 );
    RtlZeroMemory ( Str, KeyStrSize + 2 );
    RtlCopyMemory ( Str, tszKeyStr, KeyStrSize );
    
    if ( m_hKey )
        CloseKey();

    RtlInitUnicodeString( &KeyStr , Str );

	// открываем ключик
	InitializeObjectAttributes( &ObjAttr, &KeyStr, OBJ_CASE_INSENSITIVE, NULL, NULL );	
	ntStatus = ZwCreateKey ( &m_hKey, KEY_READ, &ObjAttr, 0, NULL, REG_OPTION_NON_VOLATILE, NULL );

    KL_MEM_FREE ( Str );

    return KL_SUCCESS;
}

/*
 *	Получает значение ключа с именем tszValue.
 *  Значение записывается в буффер Value, который вызывающий предварительно выделил.
 *  Если размера не хватает, то возвращается статус BUFFER_TOO_SMALL и указывается сколько
 *  Памяти требуется выделить.
 */
KLSTATUS
CKl_Registry::QueryValue(TCHAR* tszSubKey, TCHAR* tszValue, PVOID Value, ULONG* pValueSize)
{
    NTSTATUS                                ntStatus;
    PKEY_VALUE_PARTIAL_INFORMATION			Buffer = NULL;
    UNICODE_STRING							KeyValue;
    ULONG                                   RetSize;

    // выделяем буффер для значения
    Buffer = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePoolWithTag (NonPagedPool, *pValueSize + sizeof(ULONG)*3, COMMON_TAG);

    if ( Buffer == NULL )
        return KL_UNSUCCESSFUL;

    RtlInitUnicodeString(&KeyValue , tszValue);

    // смотрим, что там за значение
    ntStatus = ZwQueryValueKey(m_hKey, &KeyValue, KeyValuePartialInformation, Buffer, *pValueSize + sizeof(ULONG)*3, &RetSize);

    if ( ntStatus == STATUS_BUFFER_TOO_SMALL )
    {
        *pValueSize = RetSize - sizeof(ULONG) * 3;
        return KL_BUFFER_TOO_SMALL;
    }

    if ( ntStatus == STATUS_SUCCESS )
    {
        RtlCopyMemory( Value, (PCHAR)Buffer + sizeof(ULONG) * 3, RetSize - sizeof(ULONG) * 3 );
    }

    return ntStatus;
}

void 
CKl_Registry::CloseKey()
{
    if ( m_hKey )
    {
        ZwClose( m_hKey );
        m_hKey = NULL;

    }
}


#else // ISWIN9X
// -----------------------------------------------
// --------  Win9x Section -----------------------	
// -----------------------------------------------

KLSTATUS    
CKl_Registry::OpenKey     (TCHAR* tszKeyStr, ULONG KeyStrSize /* = 0  */)
{
    return KL_SUCCESS;
}

void
CKl_Registry::CloseKey()
{
}

#endif // ISWIN9X
#endif // USER_MODE


//------------------------------------------------------------------------------------------
/*
CKl_ServRegistry::CKl_ServRegistry(PUNICODE_STRING RegStr)
{
    NTSTATUS            ntStatus;
    OBJECT_ATTRIBUTES   ObjAttr;

    InitializeObjectAttributes( &ObjAttr, RegStr, OBJ_CASE_INSENSITIVE, NULL, NULL );	
    
    ntStatus = ZwCreateKey ( &m_hRootKey, KEY_READ, &ObjAttr, 0, NULL, REG_OPTION_NON_VOLATILE, NULL );
}

CKl_ServRegistry::~CKl_ServRegistry()
{
    NTSTATUS ntStatus;

    ntStatus = ZwClose ( m_hRootKey );
}

KLSTATUS
CKl_ServRegistry::GetValue(TCHAR* ValName, PVOID ValueBuffer, ULONG* ValueSize, ULONG* ValType)
{
    NTSTATUS            ntStatus;
    ULONG               RetSize;
    ULONG               BufferSize  = *pValueSize + sizeof( KEY_VALUE_PARTIAL_INFORMATION );
    PKEY_VALUE_PARTIAL_INFORMATION Buffer = (PKEY_VALUE_PARTIAL_INFORMATION)KL_MEM_ALLOC ( BufferSize );

    UNICODE_STRING      KeyName;

    RtlInitUnicodeString( &KeyName, ValName );

    while ( Buffer )
    {
        ntStatus = ZwQueryValueKey(
                        m_hRootKey, 
                        &KeyName, 
                        KeyValuePartialInformation, 
                        Buffer, 
                        BufferSize, 
                        &RetSize);

        if ( ntStatus == STATUS_SUCCESS )
        {            
            RtlCopyMemory( ValueBuffer, Buffer->Data, RetSize - sizeof(ULONG) * 3 );
            *pValueSize = RetSize - sizeof(ULONG) * 3;
            
            KL_MEM_FREE ( Buffer );
            return KL_SUCCESS;
        }
        
        if ( ntStatus == STATUS_BUFFER_OVERFLOW )
        {
            BufferSize = *RetSize + sizeof ( ULONG ) * 3;
            KL_MEM_FREE ( Buffer );
            Buffer = KL_MEM_ALLOC( BufferSize );
            
            continue;
        }

        KL_MEM_FREE ( Buffer );
        return KL_UNSUCCESSFUL;
    }

    return KL_SUCCESS;
}

KLSTATUS
CKl_ServRegistry::GetDWORD(TCHAR* ValName, ULONG* Value)
{
    return KL_SUCCESS;
}

KLSTATUS
CKl_ServRegistry::GetWORD(TCHAR* ValName, USHORT* Value)
{
    return KL_SUCCESS;
}

KLSTATUS
CKl_ServRegistry::GetBYTE(TCHAR* ValName, UCHAR* Value)
{
    return KL_SUCCESS;
}
*/
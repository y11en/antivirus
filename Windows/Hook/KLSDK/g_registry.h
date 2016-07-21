#ifndef _G_REGISTRY_H
#define _G_REGISTRY_H

#include "kl_object.h"
#include "kldef.h"
#include "klstatus.h"

class CKl_Registry : public CKl_Object {
    HANDLE      m_hKey;
public: 
    
    KLSTATUS    OpenKey     ( TCHAR* tszKeyStr, ULONG   KeyStrSize = 0 );

    KLSTATUS    QueryValue  ( TCHAR* tszSubKey, TCHAR*    tszValue, PVOID Value, ULONG*    pValueSize);

    void        CloseKey    ();

    KLSTATUS    CreateKey   ( TCHAR* tszSubKey, HANDLE*  hResult);

    CKl_Registry( HANDLE  hKey, TCHAR* tszKeyStr = NULL, ULONG   KeyStrSize = 0 );
    virtual ~CKl_Registry();
};

/*
// класс дл€ получени€ значений из собственной ветки драйвера
class CKl_ServRegistry : public CKl_Object
{
    HANDLE      m_hRootKey;
public:
    CKl_ServRegistry(PUNICODE_STRING RegStr);
    
    virtual ~CKl_ServRegistry();

    KLSTATUS    GetValue    ( TCHAR* ValName, PVOID ValueBuffer, ULONG* ValueSize, ULONG* ValType );

    KLSTATUS    GetDWORD    ( TCHAR* ValName, ULONG* Value );

    KLSTATUS    GetWORD     ( TCHAR* ValName, USHORT* Value );

    KLSTATUS    GetBYTE     ( TCHAR* ValName, UCHAR* Value );
};
*/

#endif
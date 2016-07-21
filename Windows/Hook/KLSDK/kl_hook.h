#ifndef _KL_HOOK_H_
#define _KL_HOOK_H_

#include "kl_object.h"
#include "kldef.h"
#include "klstatus.h"

#ifndef ISWIN9X
#ifndef USER_MODE
#include "pe.h"
#endif
#endif

#define DB(_x_)  *(UCHAR*)(_x_)
#define DW(_x_)  *(USHORT*)(_x_)
#define DD(_x_)  *(ULONG*)(_x_)

#ifndef ISWIN9X

typedef struct _DRIVER_INFO 
{
	DWORD_PTR   Unknown1;
	PVOID	BaseAddress;
	ULONG	Size;
	ULONG   Unknown3;
	ULONG	Index;
	ULONG   Unknown4;
	CHAR	PathName[0x104];
}DRIVER_INFO,*PDRIVER_INFO;


typedef struct _SYSTEM_INFO_DRIVERS 
{
	ULONG	NumberOfDrivers;
	DWORD_PTR	Reserved;
	DRIVER_INFO Drivers[0x100];
}SYSTEM_INFO_DRIVERS,*PSYSTEM_INFO_DRIVERS;

// Класс для работы с PE модулями.
// Получение адресов функций, хуки экспортов
#define CKl_HookModule  Module1
#define GetBase         Get1
#define GetModuleSize   Get2
#define GetExportByName Get3
#define HookFunction    Mark1

class CKl_HookModule : public CKl_Object
{
public:
    char*       m_base; // начало модуля
    ULONG       m_size; // Размер модуля

    PVOID       GetBase         ( char* ModuleName  );    // получает указатель на начало модуля
    ULONG       GetModuleSize   ( PVOID ModuleBase  );    // получает размер модуля
    PVOID       GetExportByName ( char* fName       );    // получает адрес экспортируемой функции по ее имени.

    KLSTATUS    HookFunction    ( char* fName, PVOID NewFunc, PVOID* pOldFunc );
    bool        ContainPtr      ( void* ptr );
    bool        isPE();
    
    CKl_HookModule(PVOID    base = NULL);
    CKl_HookModule(PCHAR    ModuleName);

    virtual ~CKl_HookModule();
};

#define CKl_ModLoader   Module2
#define PrepareForExec  Mark2

// класс для загрузки PE модулей в память и исполнения StartEntry
class CKl_ModLoader : public CKl_Object
{
public:
    char*       m_base;
    char*       m_DriverBase;

    void        PrepareForExec(PVOID  *StartEntry); // подготавливает модуль для исполнения

    CKl_ModLoader(PVOID ModuleLoadBase = NULL);
    virtual ~CKl_ModLoader();
};
#endif // ISWIN9X


#endif // _KL_HOOK_H_
#ifndef _KL_LOADER_MODULE_H_
#define _KL_LOADER_MODULE_H_

#include "kldef.h"

// Глобальный контекст Loader-а
typedef struct _NT_CONTEXT 
{
    PDRIVER_OBJECT  DriverObject;   // DriverObject  Loader-а
    UNICODE_STRING  RegPath;        // Ветка реестра Loader-а    
} NT_CONTEXT, *PNT_CONTEXT;

#define MAX_DEV_OBJ	20

typedef struct _NT_MOD_CONTEXT
{
    PNT_CONTEXT    pg_ntContext;
    PDEVICE_OBJECT DevObj[MAX_DEV_OBJ];     // Список зарегистрированных DevObj для данного модуля
} NT_MOD_CONTEXT, *PNT_MOD_CONTEXT;

#define G_CTX(_DrvObj_) ((PNT_MOD_CONTEXT) ( (PDRIVER_EXTENSION)((_DrvObj_) + 1) + 1 ))->pg_ntContext


#define SafeDriverObject			Safe
#define KlRegisterDeviceObject		insert1
#define KlUnregisterDeviceObject	remove1
#define FindDevice					MyFind1

PDRIVER_OBJECT  
SafeDriverObject		( PDRIVER_OBJECT DrvObj );

void    
KlRegisterDeviceObject  ( PDEVICE_OBJECT DeviceObject, PDRIVER_OBJECT DrvObj );

void 
KlUnregisterDeviceObject( PDEVICE_OBJECT DeviceObject, PDRIVER_OBJECT DrvObj );

BOOLEAN 
FindDevice				( PDEVICE_OBJECT DeviceObject, PDRIVER_OBJECT DrvObj );

#endif
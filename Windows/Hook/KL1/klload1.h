#ifndef _KL_LOADER1_MODULE_H_
#define _KL_LOADER1_MODULE_H_

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

//#define SafeDriverObject			Safe
//#define KlRegisterDeviceObject		insert1
//#define KlUnregisterDeviceObject	remove1
//#define FindDevice					MyFind1

#define SafeDriverObject(DrvObj) ((DrvObj)->Type ? (DrvObj) : G_CTX(DrvObj)->DriverObject)

#define KlRegisterDeviceObject(DeviceObject, DrvObj) \
{ \
	if ( !DrvObj->Type ) { \
		PNT_MOD_CONTEXT mc = (PNT_MOD_CONTEXT) ( (PDRIVER_EXTENSION)(DrvObj + 1) + 1 ); \
		ULONG i; \
		for ( i = 0; i < MAX_DEV_OBJ; i++ )	{ \
			if ( mc->DevObj[i] == NULL ) { \
				mc->DevObj[i] = DeviceObject; \
				DbgPrint ( "Registered %x->%d for %x\n", DeviceObject, i, DrvObj ); \
				break; \
			} \
		} \
	} \
}

#define KlUnregisterDeviceObject(DeviceObject, DrvObj) \
{ \
	if ( !DrvObj->Type ) { \
		ULONG i; \
		PNT_MOD_CONTEXT mc = (PNT_MOD_CONTEXT) ( (PDRIVER_EXTENSION)(DrvObj + 1) + 1 ); \
		for ( i = 0; i < MAX_DEV_OBJ; i++ ) { \
			if ( mc->DevObj[i] == DeviceObject ) { \
				mc->DevObj[i] = NULL; \
				DbgPrint ( "Unregistered %x->%d for %x\n", DeviceObject, i, DrvObj ); \
				break; \
			} \
		} \
	} \
}

#endif

#include "klload.h"

PDRIVER_OBJECT  SafeDriverObject(PDRIVER_OBJECT DrvObj)
{
    PDRIVER_OBJECT  LoaderDrvObj = NULL;    

    LoaderDrvObj = DrvObj->Type ? DrvObj : G_CTX(DrvObj)->DriverObject;
    
    return LoaderDrvObj;
}


void    KlRegisterDeviceObject(PDEVICE_OBJECT DeviceObject, PDRIVER_OBJECT DrvObj)
{
    if ( !DrvObj->Type )
    {
        PNT_MOD_CONTEXT mc = (PNT_MOD_CONTEXT) ( (PDRIVER_EXTENSION)(DrvObj + 1) + 1 );

        ULONG i;         
        for ( i = 0; i < MAX_DEV_OBJ; i++ )
        {
            if ( mc->DevObj[i] == NULL )
            {
                mc->DevObj[i] = DeviceObject;
                DbgPrint ( "Registered %x->%d for %x\n", DeviceObject, i, DrvObj );
                break;
            }
        }
    }
}

void KlUnregisterDeviceObject(PDEVICE_OBJECT DeviceObject, PDRIVER_OBJECT   DrvObj)
{
    if ( !DrvObj->Type )
    {   
        ULONG i;
        PNT_MOD_CONTEXT mc = (PNT_MOD_CONTEXT) ( (PDRIVER_EXTENSION)(DrvObj + 1) + 1 );

        for ( i = 0; i < MAX_DEV_OBJ; i++ )
        {
            if ( mc->DevObj[i] == DeviceObject )
            {
                mc->DevObj[i] = NULL;
                DbgPrint ( "Unregistered %x->%d for %x\n", DeviceObject, i, DrvObj );
                break;
            }
        }
    }
}

BOOLEAN FindDevice(PDEVICE_OBJECT DeviceObject, PDRIVER_OBJECT   DrvObj)
{
    if ( !DrvObj->Type )
    {        
        ULONG i; 
        PNT_MOD_CONTEXT mc = (PNT_MOD_CONTEXT) ( (PDRIVER_EXTENSION)(DrvObj + 1) + 1 );

        for ( i = 0; i < MAX_DEV_OBJ; i++ )
        {
            if ( mc->DevObj[i] == DeviceObject )
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}
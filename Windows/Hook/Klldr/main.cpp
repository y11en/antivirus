#include "precomp.h"
#pragma hdrstop

CKl_ModLoader*   ldr = NULL;

typedef
NTSTATUS
(*START_ENTRY)(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);

START_ENTRY se;

extern "C" NTSTATUS DriverEntry ( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath )
{
    NTSTATUS					ntStatus;
    
    CKl_File    ff;

    BOOLEAN bWasError = FALSE;

    __asm int 3
    
    if ( KL_SUCCESS == ff.Open("C:\\debug\\klick.sys") )
    {        
        ULONG   ModuleSize  = ff.GetSize();
        ULONG   bRead       = 0;
        
        PVOID   ModuleLoadBase = KL_MEM_ALLOC( ModuleSize );
        ASSERT ( ModuleLoadBase );
        
        // 1.
        ff.Read((char*)ModuleLoadBase, 0, ModuleSize, &bRead);
        
        if ( ModuleSize == bRead )
        {            
            
            ldr = new CKl_ModLoader( ModuleLoadBase );
            if ( ldr )
            {
                ldr->PrepareForExec((PVOID*)&se);
                
                NTSTATUS    retStatus = se(NULL, NULL);
            }
            
//            if ( ModuleLoadBase )
//                KL_MEM_FREE ( ModuleLoadBase );
        }
        
        ff.Close();
    }
    return STATUS_SUCCESS;
}

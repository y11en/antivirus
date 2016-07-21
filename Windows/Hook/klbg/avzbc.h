#ifndef		__AVZBC_H
#define		__AVZBC_H
//--------------------------------------------------------------------
#include <fltkernel.h>

#ifdef __cplusplus
extern "C" {
#endif

NTSTATUS ExecuteQuarantine(IN PUNICODE_STRING pusRegistryPath, IN BOOLEAN DeteteKey);
NTSTATUS ExecuteCopyFiles(IN PUNICODE_STRING pusRegistryPath, IN BOOLEAN DeteteKey);
NTSTATUS ExecuteDeleteFiles(IN PUNICODE_STRING pusRegistryPath, IN BOOLEAN DeteteKey);
NTSTATUS ExecuteDeleteDrivers(IN PUNICODE_STRING pusRegistryPath, IN BOOLEAN DeteteKey);
NTSTATUS ExecuteREGOper(IN PUNICODE_STRING pusRegistryPath, IN BOOLEAN DeteteKey);

VOID AVZBCInit( __in PUNICODE_STRING pusRegistryPath );
VOID AVZBCUnInit();

#ifdef __cplusplus
}
#endif
//--------------------------------------------------------------------
#endif
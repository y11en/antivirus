#include "klstatus.h"
#include "kldef.h"
#include "kl_lock.h"


extern "C" NTSTATUS DriverEntry ( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath )
{
    NTSTATUS					ntStatus;

    DbgPrint ("Hello World\n");

    CKl_Lock    lock;

    lock.Acquire();

    DbgPrint ("Interlocked Hello World\n");

    lock.Release();

    return STATUS_UNSUCCESSFUL;
}
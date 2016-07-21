#ifndef _KLOP_MAIN_H_
#define _KLOP_MAIN_H_

#define MYDRV "KLOP"

#define LINKNAME_STRING     L"\\DosDevices\\Global\\Klop"
#define LINKNAME_STRING2    L"\\DosDevices\\Klop"
#define NTDEVICE_STRING     L"\\Device\\Klop"


#include "kldef.h"
#include "klstatus.h"

NTSTATUS	
Dispatch(
         IN PDEVICE_OBJECT	DeviceObject,
         IN PIRP				Irp
         );
                
#endif
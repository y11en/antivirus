#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

$$IF(PLATFORM_NT)
#include <ntddk.h>
#include <tdi.h>
$$ENDIF
$$IF(PLATFORM_9X)
#define USE_NDIS
#include <vtoolsc.h>       
#define $$ROOT$$_Major			1
#define $$ROOT$$_Minor			0
#define $$ROOT$$_DeviceID		UNDEFINED_DEVICE_ID
#define $$ROOT$$_Init_Order		VXDLDR_INIT_ORDER + 4
$$ENDIF
#include "..\g_precomp.h"

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)

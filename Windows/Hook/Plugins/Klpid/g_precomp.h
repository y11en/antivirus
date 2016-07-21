#include "kldef.h"
#include "klstatus.h"
#include "tchar.h"


#ifdef ISWIN9X
/*
 *	WIN9X Section 
 */

#define KLICK_BASE_DRV          "KLICK   "
#define KLIN_BASE_DRV           "KLIN    "
#else
   /*
    *	WINNT Section
    */

    #include <ndis.h>     
    #define DWORD   ULONG
    #define BYTE    UCHAR

#define KLIN_BASE_DRV       L"\\Device\\klin"
#define KLICK_BASE_DRV      L"\\Device\\klick"

#endif

#include "registry.h"
#include "plugAPI.h"
#include "..\klick\klick_api.h"
#include "..\..\..\hook\hook\avpgcom.h"
#include "..\..\..\hook\hook\pidexternalapi.h"
#include "..\..\..\hook\hook\SingleParam.h"
#include "..\..\..\hook\hook\hookspec.h"
#include "..\..\..\hook\hook\NETID.h"
#include "handlers.h"

#include "..\..\TRUE\FilterEvent.h"

#include "attacks.h"
#include "STEALTH.H"

#include "..\reg_plugin.h"
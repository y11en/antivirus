#include "kldef.h"
#include "klstatus.h"


#ifdef ISWIN9X

#else
   /*
    *	WINNT Section
    */

    #include <ntddk.h> 
    #include <TDIKRNL.H>
#endif

#include "plugAPI.h"
#include "..\klin\klin_api.h"
#include "..\..\..\hook\hook\klpf_api.h"
#include "..\..\..\hook\hook\avpgcom.h"
#include "..\..\..\hook\hook\SingleParam.h"
#include "..\..\..\hook\hook\hookspec.h"
#include "..\..\..\hook\hook\NETID.h"
#include "handlers.h"

#include "..\reg_plugin.h"
#ifdef _WIN64
#pragma pack ( push, 8 )
#endif

#include <ndis.h>

#ifdef _WIN64
#pragma pack ( pop )
#endif

#include "filteruser.h"
#include "flt_dbg.h" 
#include "filter.h"
#include "..\klim_dispatcher.h"
#include "..\..\hook\klick_api.h"

#include "klim6_pkt_recv.h"
#include "klim6_pkt_send.h"

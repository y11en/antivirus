#ifdef _WIN64
#pragma pack ( push, 8 )
#endif

#include "ndis.h"
#include "ntddk.h"


#ifdef _WIN64
#pragma pack ( pop )
#endif

#include "passthru.h"
#include "..\klim_dispatcher.h"
#include "klim_pkt_recv.h"
#include "klim_buff_recv.h"
#include "klim_pkt_send.h"
#include "sendpkt.h"
#include "..\..\hook\klick_api.h"

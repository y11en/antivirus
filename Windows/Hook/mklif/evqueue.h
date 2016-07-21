#ifndef _EVQUEUE_H
#define _EVQUEUE_H

#include "../hook/avpgcom.h"
#include "inc/commdata.h"

PMKLIF_EVENT_TRANSMIT
QueueAllocEventTr (
	PFILTER_EVENT_PARAM pEventParam,
	PFILTER_SUB pFilterSub,
	ULONG EventFlags
	);

#endif // _EVQUEUE_H
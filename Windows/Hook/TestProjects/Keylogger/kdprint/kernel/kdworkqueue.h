#ifndef _KDWORKQUEUE_H
#define _KDWORKQUEUE_H
//-------------------------------------------------------------------------------------------------------------------------
#include <ntifs.h>

NTSTATUS KDInitWorkQueue();
VOID KDUnInitWorkQueue();
VOID KDQueueWorkItem(IN PWORK_QUEUE_ITEM pWorkItem, IN ULONG Unused OPTIONAL);

//-------------------------------------------------------------------------------------------------------------------------
#endif
#ifndef __WATCHER_H
#define __WATCHER_H
//----------------------------------------------------------------------
#include "ctlcodes.h"

#ifndef __WATCHER_C
extern ERESOURCE g_erServiceListLock;
extern LIST_ENTRY g_ServiceListHead;
#endif

VOID ServiceEnumInit();
VOID ServiceEnumCleanup();
NTSTATUS EnumServices();
VOID ComputeMd5OnServiceImages();

#include <pshpack1.h>

typedef struct _SERVICE_ENUM_ENTRY
{
	LIST_ENTRY	m_List;

	ULONG		m_Type;
	ULONG		m_Start;
	BOOLEAN		m_Md5Computed;
	UCHAR		m_Md5Hash[16];
	ULONG		m_cbServiceName;	// total len of service name & image path including terminators
	ULONG		m_ImagePathOffset;	// offset from m_ServiceName
	WCHAR		m_ServiceName[1];	// service name & image path
} SERVICE_ENUM_ENTRY, *PSERVICE_ENUM_ENTRY;

#include <poppack.h>

//----------------------------------------------------------------------
#endif
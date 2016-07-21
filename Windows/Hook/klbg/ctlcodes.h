#ifndef __CTLCODES_H
#define __CTLCODES_H
//-------------------------------------------------------------------------------

#define CTL_DEVICE_NAME		L"\\Device\\klbg007"
#define SYMLINK_NAME		L"\\??\\klbg007"
#define DOS_DEVICE_NAME		L"\\\\.\\klbg007"

#define IOCTL_SW_GETFIRST	CTL_CODE( FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS )
#define IOCTL_SW_GETNEXT	CTL_CODE( FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS )

#include <pshpack1.h>

typedef struct _SERVICE_ENUM_ENTRY_OUT
{
	ULONG		m_Type;
	ULONG		m_Start;
	BOOLEAN		m_Md5Computed;
	UCHAR		m_Md5Hash[16];
	ULONG		m_cbServiceName;	// total len of service name & image path including terminators
	ULONG		m_ImagePathOffset;	// offset from m_ServiceName
	WCHAR		m_ServiceName[1];	// service name & image path
} SERVICE_ENUM_ENTRY_OUT, *PSERVICE_ENUM_ENTRY_OUT;

#include <poppack.h>

//-------------------------------------------------------------------------------
#endif

#ifndef _KDCTL_H
#define _KDCTL_H
//------------------------------------------------------------------------------------------------------------------------
#define K_CONTROL_DEVICE_NAME(var)		(L"\\Device\\KdPrintDevice"L#var)
#define K_CONTROL_SYMLINK_NAME(var)		(L"\\??\\KdPrintDevice"L#var)
#define U_CONTROL_DEVICE_PREFIX			"\\\\.\\KdPrintDevice"
#define U_CONTROL_DEVICE_NAME(var)		(U_CONTROL_DEVICE_PREFIX#var)

// имена секции и мьютекса для юзер-модового шаренья
#define U_FILEMAPPING_NAME				"KdPrintFileMapping"
#define U_MUTEX_NAME					"KdPrintMutex"

#define FN_INIT_KDPRINT					0x801

#define IOCTL_INIT_KDPRINT				CTL_CODE(FILE_DEVICE_UNKNOWN, FN_INIT_KDPRINT, METHOD_BUFFERED, FILE_ANY_ACCESS)

#include <pshpack4.h>

typedef
struct _KDPRINT_INIT_INPARAM
{
	HANDLE		m_hSectionMutant;
	HANDLE		m_hSection;
} KDPRINT_INIT_INPARAM, *PKDPRINT_INIT_INPARAM;

#include <poppack.h>

//------------------------------------------------------------------------------------------------------------------------
#endif
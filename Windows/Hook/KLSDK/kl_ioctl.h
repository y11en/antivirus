/*
 *	Класс - хелпер для облегчения операций взаимодействия между драйверами.
 *  
 */

#ifndef _KL_IOCTL_H_
#define _KL_IOCTL_H_

#include "kldef.h"
#include "klstatus.h"
#include "kl_object.h"

class CKl_Device
{	
public:
    PVOID	m_DevicePtr;
    PVOID	m_DeviceFO;		// FileObject

	CKl_Device();
	CKl_Device(PCHAR  DeviceName);
	CKl_Device(PWCHAR DeviceName);
	CKl_Device(PVOID  DevicePtr);
	
	KLSTATUS SendIoctl( ULONG ioctl, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, DWORD* dwRet );	

	PVOID	FindDeviceByName( PCHAR Name );
	PVOID	FindDeviceByName( PWCHAR Name );
};

#endif
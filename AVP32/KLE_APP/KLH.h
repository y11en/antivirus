// KLH.H
// KL Engine Helper API for test application

#ifndef __KLH_h__
#define __KLH_h__
#define KLH_DEVICE_NAME_NT "\\\\.\\KLH_SYSDevice0"
#define KLH_DEVICE_NAME_9X "KLH_VXD"

#define KLH_IOCTL_LOAD_BASES           CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define KLH_IOCTL_SCAN_FILE            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define KLH_IOCTL_GET_REP_INFO         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct KLH_REP_INFO{
	unsigned Param1;
	unsigned Param2;
    char  String1[0x200];
    char  String2[0x200];
	char  DebugStr[0x200];
	struct KLH_REP_INFO* Next;
}KLH_REP_INFO;

#endif

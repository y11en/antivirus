
#ifndef _KLPF_API_
#define _KLPF_API_

#define KLPF_BUILDNUM 3

#define KLPF_NAME	L"KLPF"
#define KLPF_NAME_9x "KLPF    "

#define KLPF_FILE_DEVICE_SECTION  0x00008005

#define KLPF_IOCTL_INDEX  0x739

#define EVENT_CONNECT			0x1
#define EVENT_DISCONNECT		0x2
#define EVENT_RECEIVE			0x3
#define EVENT_SEND				0x4
#define EVENT_OPEN_PORT			0x5
#define EVENT_CLOSE_PORT		0x6
#define EVENT_LISTEN			0x7

//  [7/31/2002]
#define	EVENT_UDP_SEND				0x8
#define EVENT_UDP_RECEIVE			0x9

#define ID_FULL_APP_PATH		0x04000001

#define ID_REMOTE_IP			0x04000002
#define ID_REMOTE_PORT			0x04000003
#define ID_LOCAL_PORT			0x04000004

#define ID_CONNECT_OUTGOING		0
#define ID_CONNECT_INCOMING		1
#define ID_INITIATOR			0x04000005

#define ID_LOCAL_IP				0x04000006
#define ID_FILE_CHECKSUM		0x04000007

#define ID_IMAGE_NAME	   	    0x04000008

//-------------------------------------------------------------------------------
// Общие IOCTL ... вызовы, существенные для системной работы
#define KLPF_GET_VERSION		CTL_CODE(KLPF_FILE_DEVICE_SECTION,				\
								KLPF_IOCTL_INDEX,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define KLPF_FIND_AVPG_IOCTL	CTL_CODE(KLPF_FILE_DEVICE_SECTION,				\
								KLPF_IOCTL_INDEX+5,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define KLPF_START_FILTER		CTL_CODE(KLPF_FILE_DEVICE_SECTION,				\
								KLPF_IOCTL_INDEX+6,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define KLPF_STOP_FILTER		CTL_CODE(KLPF_FILE_DEVICE_SECTION,				\
								KLPF_IOCTL_INDEX+7,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)
//-------------------------------------------------------------------------------
// Функциональные вызовы
#define USERAPPNAMELEN  260
#define PROCNAMELEN		32

#pragma pack(1)
typedef struct _IO_LOCAL_TA_INFO	{
	unsigned long	Handle;	
	unsigned long	Protocol;
	unsigned long	LocalIp;
	unsigned short	LocalPort;	
	unsigned long	PID;
	unsigned char	ProcessName[USERAPPNAMELEN];
	unsigned char	UserAppName[USERAPPNAMELEN];
} IO_LOCAL_TA_INFO, *PIO_LOCAL_TA_INFO;
#pragma pack()
#define KLPF_GET_LOCAL_TA		CTL_CODE(KLPF_FILE_DEVICE_SECTION,				\
								KLPF_IOCTL_INDEX+8,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#pragma pack(1)
typedef struct _IO_CONN_INFO	{
	unsigned long	Handle;	
	unsigned long	Protocol;
	unsigned long	LocalIp;
	unsigned short	LocalPort;	
	unsigned long	RemoteIp;
	unsigned short	RemotePort;	
	unsigned long	PID;
	unsigned char	ProcessName[USERAPPNAMELEN];
	unsigned char	UserAppName[USERAPPNAMELEN];
	unsigned long	State;
	unsigned char	isIncoming;	
} IO_CONN_INFO, *PIO_CONN_INFO;
#pragma pack()
#define KLPF_GET_CONNECTIONS	CTL_CODE(KLPF_FILE_DEVICE_SECTION,				\
								KLPF_IOCTL_INDEX+9,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)


#pragma pack(1)
typedef struct _MAKE_DISCONNECT_STRUCT	{
	unsigned char	Protocol;
	unsigned long	LocalIp;
	unsigned short	LocalPort;
	unsigned long	RemoteIp;
	unsigned short	RemotePort;	
} MAKE_DISCONNECT_STRUCT, *PMAKE_DISCONNECT_STRUCT;
#pragma pack()
#define KLPF_MAKE_DISCONNECT	CTL_CODE(KLPF_FILE_DEVICE_SECTION,				\
								KLPF_IOCTL_INDEX+10,							\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// 1-DWORD - число IP-адресов, потом набор этих адресов.
#define KLPF_SET_LOCAL_IP_LIST	CTL_CODE(KLPF_FILE_DEVICE_SECTION,				\
								KLPF_IOCTL_INDEX+11,							\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)


//-------------------------------------------------------------------------------
// Отладочные вызовы 

#ifndef IOCTL_DEBUG__
#define IOCTL_DEBUG__
// DWORD in
#define IOCTL_SET_DEBUG_LEVEL		CTL_CODE(KLPF_FILE_DEVICE_SECTION,			\
									KLPF_IOCTL_INDEX+100,						\
									METHOD_BUFFERED,							\
									FILE_ANY_ACCESS)
// DWORD out
#define IOCTL_GET_DEBUG_LEVEL		CTL_CODE(KLPF_FILE_DEVICE_SECTION,			\
									KLPF_IOCTL_INDEX+101,						\
									METHOD_BUFFERED,							\
									FILE_ANY_ACCESS)

// DWORD in
#define IOCTL_SET_DEBUG_MASK		CTL_CODE(KLPF_FILE_DEVICE_SECTION,			\
									KLPF_IOCTL_INDEX+102,						\
									METHOD_BUFFERED,							\
									FILE_ANY_ACCESS)

// DWORD out
#define IOCTL_GET_DEBUG_MASK		CTL_CODE(KLPF_FILE_DEVICE_SECTION,			\
									KLPF_IOCTL_INDEX+103,						\
									METHOD_BUFFERED,							\
									FILE_ANY_ACCESS)
#endif // IOCTL_DEBUG__

#endif //_KLPF_API_

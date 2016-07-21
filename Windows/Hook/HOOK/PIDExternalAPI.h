/*++
Description:

Definition of ioctls
    
Author:    Eugene Roschin         
Time  :    01.08.2001

--*/

//
// Define the various device type values.  Note that values used by Microsoft
// Corporation are in the range 0-32767, and 32768-65535 are reserved for use
// by customers.
//

#define KLPID_BUILDNUM 5
#define KLPID_IDS_VERSION	6
#define KLPID_NAME L"KLPID"
#define KLPID_NAME_9x "KLPID   "
#define KLPID_NAME_EX L"KLPID_"		// For imsamp version for NT4.0
#define KLPID_NAME_SIZE 6			// For imsamp version for NT4.0

#define PID_FILE_DEVICE_SECTION  0x00008005

#pragma pack (push)
#pragma warning( push )

#pragma pack(1)
#pragma warning( disable : 4200 )

typedef struct tagDebugExchange{
	ULONG m_uID;
	ULONG m_uDbgValue;
}DebugExchange, *PDebugExchange;

typedef struct _ADAPTER_INFO {
	HANDLE				hAdapter;		//хэндл адаптера (указатель на внутреннюю структуру ADAPTER)
	BOOLEAN				bIsHooked;		// мы перехватываем этот Адаптер ??? Да\нет
	// NDIS_MEDIUM
	ULONG				Medium;			// среда распространения пакетов (ethernet, token ring ...)
	DWORD				dwNameLength;	//String length with null terminated
	USHORT				szAdapterName[0];	// Null terminated!
}ADAPTER_INFO, *PADAPTER_INFO;

#define IS_VALID_BUF_PTR(buf, size, ptr) \
(((PCH)buf + size)>(PCH)ptr)?TRUE:FALSE

//IOCTL_SEND_BUFFER_AS_FRAME
typedef struct _FRAME{
	HANDLE	hAdapter; //hAdapter from ADAPTER_INFO
	BOOLEAN	bDirection; //TRUE - to network, FALSE - to user
	ULONG	uFrameSize;
	UCHAR	Frame[0];
}FRAME, *LPFRAME;

#pragma pack (pop)

//
// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//

#define PID_IOCTL_INDEX  0x739

//
// Define our own private IOCTL
//

#define PID_IOCTL_GETVERSION	CTL_CODE(PID_FILE_DEVICE_SECTION ,				 \
														 PID_IOCTL_INDEX,    \
														 METHOD_BUFFERED,\
														 FILE_ANY_ACCESS)
// OutBuffer=&BuildNum OutBufferSize=sizeof(ULONG)


#define PID_IOCTL_START_FILTER  CTL_CODE(PID_FILE_DEVICE_SECTION ,				 \
                                                         PID_IOCTL_INDEX+1,  \
                                                         METHOD_BUFFERED,\
                                                         FILE_ANY_ACCESS)
// Find AVPG and Start filtering

#define PID_IOCTL_STOP_FILTER	CTL_CODE(PID_FILE_DEVICE_SECTION,				 \
                                                         PID_IOCTL_INDEX+2,  \
                                                         METHOD_BUFFERED,\
                                                         FILE_ANY_ACCESS)
// Stop Filtering

#define PID_IOCTL_FIND_DRIVER	  CTL_CODE(PID_FILE_DEVICE_SECTION,				 \
														PID_IOCTL_INDEX+3,  \
														METHOD_BUFFERED,\
														FILE_ANY_ACCESS)
// Just Find a driver

#define PID_IOCTL_START_LOG	CTL_CODE(PID_FILE_DEVICE_SECTION,				 \
														PID_IOCTL_INDEX+4,  \
														METHOD_BUFFERED,\
														FILE_ANY_ACCESS)

#define PID_IOCTL_STOP_LOG	CTL_CODE(PID_FILE_DEVICE_SECTION,				 \
														PID_IOCTL_INDEX+5,  \
														METHOD_BUFFERED,\
														FILE_ANY_ACCESS)

#define PID_IOCTL_SETUP		CTL_CODE(PID_FILE_DEVICE_SECTION,				 \
														PID_IOCTL_INDEX+6,  \
														METHOD_BUFFERED,\
														FILE_ANY_ACCESS)

typedef struct _ATTACK_OPTIONS {
	ULONG	Version;
	struct {
		ULONG	Land;
		ULONG	PingOfDeath;
		ULONG	SynFlood;
		ULONG	TcpScan;
		ULONG	UdpScan;
		ULONG	UdpFlood;
		ULONG	IcmpFlood;
		ULONG	Helkern;
		ULONG	SmbDie;
        ULONG   LoveSan;
//        ULONG   Witty;

		ULONG	StealthModeOn;		
		ULONG	IDS_Enable;		
	} AttackSwitch;
	
	ULONG	SynFloodCount;
	ULONG	SynFloodTime;
	ULONG	UdpFloodCount;
	ULONG	UdpFloodTime;
	ULONG	IcmpFloodCount;
	ULONG	IcmpFloodTime;

	ULONG	FloodListCount;
	
	ULONG	BanTime;
	ULONG	MaxOpenedTcpPorts;
	ULONG	MaxOpenedUdpPorts;	
	ULONG	TcpRefreshTime;
	ULONG	UdpRefreshTime;	
} ATTACK_OPTIONS, *PATTACK_OPTIONS;
#define PID_IOCTL_GET_ATTACK_OPTIONS	CTL_CODE(PID_FILE_DEVICE_SECTION,				 \
										PID_IOCTL_INDEX+7,  \
										METHOD_BUFFERED,\
										FILE_ANY_ACCESS)

#define PID_IOCTL_SET_ATTACK_OPTIONS	CTL_CODE(PID_FILE_DEVICE_SECTION,				 \
										PID_IOCTL_INDEX+8,  \
										METHOD_BUFFERED,\
										FILE_ANY_ACCESS)

typedef struct _REG_PARAM {
	ULONG	ParamType;
	CHAR	ParamName[256];
	ULONG	ParamSize;
	CHAR	Param[0];
} REG_PARAM, *PREG_PARAM;



#define PID_IOCTL_QUERY_REGISTRY		CTL_CODE(PID_FILE_DEVICE_SECTION,				 \
										PID_IOCTL_INDEX+9,  \
										METHOD_BUFFERED,\
										FILE_ANY_ACCESS)

#define PID_IOCTL_WRITE_REGISTRY		CTL_CODE(PID_FILE_DEVICE_SECTION,				 \
										PID_IOCTL_INDEX+10,  \
										METHOD_BUFFERED,\
										FILE_ANY_ACCESS)

#define PID_IOCTL_RESET_BAN				CTL_CODE(PID_FILE_DEVICE_SECTION,				 \
										PID_IOCTL_INDEX+11,  \
										METHOD_BUFFERED,\
										FILE_ANY_ACCESS)

//-----------------------------------------------------------------------------------------------------
#define PID_IOCTL_SET_STEALTH_MODE		CTL_CODE(PID_FILE_DEVICE_SECTION,			\
										PID_IOCTL_INDEX+12,							\
										METHOD_BUFFERED,							\
										FILE_ANY_ACCESS)

#define PID_IOCTL_GET_STEALTH_MODE		CTL_CODE(PID_FILE_DEVICE_SECTION,			\
										PID_IOCTL_INDEX+13,							\
										METHOD_BUFFERED,							\
										FILE_ANY_ACCESS)


// InBuffer = &DebugTraceLevel
#ifndef IOCTL_DEBUG__
#define IOCTL_DEBUG__
// DWORD in.
#define IOCTL_SET_DEBUG_LEVEL		CTL_CODE(PID_FILE_DEVICE_SECTION ,				 \
									PID_IOCTL_INDEX +100,    \
									METHOD_BUFFERED,\
									FILE_ANY_ACCESS)
// DWORD out.
#define IOCTL_GET_DEBUG_LEVEL		CTL_CODE(PID_FILE_DEVICE_SECTION ,				 \
									PID_IOCTL_INDEX +101,    \
									METHOD_BUFFERED,\
									FILE_ANY_ACCESS)

// DWORD in.
#define IOCTL_SET_DEBUG_MASK		CTL_CODE(PID_FILE_DEVICE_SECTION ,				 \
									PID_IOCTL_INDEX +102,    \
									METHOD_BUFFERED,\
									FILE_ANY_ACCESS)

// DWORD out.
#define IOCTL_GET_DEBUG_MASK		CTL_CODE(PID_FILE_DEVICE_SECTION ,				 \
									PID_IOCTL_INDEX +103,    \
									METHOD_BUFFERED,\
									FILE_ANY_ACCESS)

//-----------------------------------------------------------------------------------
#endif

//Add additional IOCTL's by Melekh Boris 30.04.2002
//To include this file into GUI project include before 
//#include <NTSecAPI.h>
//#include <ntddndis.h>


#define IOCTL_GET_ADAPTER_LIST		CTL_CODE(PID_FILE_DEVICE_SECTION ,				 \
									PID_IOCTL_INDEX +200,    \
									METHOD_BUFFERED,\
									FILE_ANY_ACCESS)

#define IOCTL_SEND_BUFFER_AS_FRAME	CTL_CODE(PID_FILE_DEVICE_SECTION ,				 \
									PID_IOCTL_INDEX +201,    \
									METHOD_BUFFERED,\
									FILE_ANY_ACCESS)

#define IOCTL_SET_ADAPTER_PROPERTY	CTL_CODE(PID_FILE_DEVICE_SECTION ,				 \
									PID_IOCTL_INDEX +202,    \
									METHOD_BUFFERED,\
									FILE_ANY_ACCESS)

#define IOCTL_GET_STATISTICS		CTL_CODE(PID_FILE_DEVICE_SECTION ,				 \
									PID_IOCTL_INDEX +203,    \
									METHOD_BUFFERED,\
									FILE_ANY_ACCESS)


//-----------------------------------------------------------------------------------


#pragma warning( pop )


// Собковские ID-шники 

#define MJ_NET_ATTACK				0x3

#define ID_NET_ATTACK				0x05000001

enum ATTACK_ID {
    ATTACK_LAND_ID = 0x1,
    ATTACK_PING_OF_DEATH_ID,
    ATTACK_SYN_FLOOD_ID,
    ATTACK_UDP_FLOOD_ID,
    ATTACK_ADDRESS_SCAN,
    ATTACK_TCP_SCAN,
    ATTACK_UDP_SCAN,
    ATTACK_ICMP_FLOOD,
    ATTACK_SMBDIE_ID,
    ATTACK_HELKERN_ID,
    ATTACK_LOVESAN_ID,
    ATTACK_WITTY_ID,
    ATTACK_SASSER_ID
};

/*
#define ATTACK_LAND_ID				0x1
#define ATTACK_PING_OF_DEATH_ID		0x2
#define ATTACK_SYN_FLOOD_ID			0x3
#define ATTACK_UDP_FLOOD_ID			0x4
#define ATTACK_ADDRESS_SCAN			0x5
#define ATTACK_TCP_SCAN				0x6
#define ATTACK_UDP_SCAN				0x7
#define ATTACK_ICMP_FLOOD			0x8
#define ATTACK_SMBDIE_ID			0x9
#define ATTACK_HELKERN_ID			0xa
#define ATTACK_LOVESAN_ID			0xb
#define ATTACK_WITTY_ID			    0xc
*/

#define ATTACK_DDOS					0xFFFFFFFF // Пипец всему живому !

//--------- ключи в реестре  --------------------------------------------------------------
#define SETUP_STRING_KEY		L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\klpid\\Parameters"
#define PARAMETERS_LEVEL_KEY	L"DebugLevel"
#define PARAMETERS_MASK_KEY		L"DebugMask"

#define PARAMETERS_MAX_OPENED_TCP_PORTS_KEY		_T("MaxOpenedTCPPorts")
#define PARAMETERS_MAX_OPENED_UDP_PORTS_KEY		_T("MaxOpenedUDPPorts")
#define PARAMETERS_BAN_TIME_KEY					_T("BanTime")
#define PARAMETERS_REFRESH_TIME_KEY				_T("RefreshTime")
#define PARAMETERS_TCP_REFRESH_TIME_KEY			_T("TcpRefreshTime")
#define PARAMETERS_UDP_REFRESH_TIME_KEY			_T("UdpRefreshTime")

#define PARAMETERS_SYN_FLOOD_COUNT_KEY			_T("MaxSynCount")
#define PARAMETERS_SYN_FLOOD_TIME_KEY			_T("SynFloodTime")
#define PARAMETERS_UDP_FLOOD_COUNT_KEY			_T("MaxUdpFloodCount")
#define PARAMETERS_UDP_FLOOD_TIME_KEY			_T("UdpFloodTime")
#define PARAMETERS_ICMP_FLOOD_TIME_KEY			_T("IcmpFloodTime")
#define PARAMETERS_ICMP_FLOOD_COUNT_KEY			_T("IcmpFloodCount")

#define PARAMETERS_LAND_PROTECT_KEY				_T("LandProtect")
#define PARAMETERS_PING_OF_DEATH_PROTECT_KEY	_T("PingOfDeathProtect")
#define PARAMETERS_SYN_FLOOD_PROTECT_KEY		_T("SynFloodProtect")
#define PARAMETERS_TCP_SCAN_PROTECT_KEY			_T("TcpScanProtect")
#define PARAMETERS_UDP_SCAN_PROTECT_KEY			_T("UdpScanProtect")
#define PARAMETERS_UDP_FLOOD_PROTECT_KEY		_T("UdpFloodProtect")
#define PARAMETERS_ICMP_FLOOD_PROTECT_KEY		_T("IcmpFloodProtect")
#define PARAMETERS_IDS_ENABLE_PROTECT_KEY		_T("IDS_Enable")

#define PARAMETERS_IDS_OPTIONS_KEY		_T("Ids")

#define PARAMETERS_TRUE_LEVEL_KEY L"TrueDebugLevel"
#define PARAMETERS_TRUE_MASK_KEY  L"TrueDebugMask"

//-----------------------------------------------------------------------------------
extern IS_NEED_FILTER_EVENT		IsNeedFiltering;
extern FILTER_EVENT_FUNC		FilterEventExt;
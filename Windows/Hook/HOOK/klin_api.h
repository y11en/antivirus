/*
 *	API для драйвера KLIN.sys
 *  Определения вызовов IOCTL, а также структур, передаваемых в эти вызовы.
 *
 *  Автор : Рощин Е.Е.
 *
 */

#ifndef _KLIN_API_
#define _KLIN_API_

#pragma warning( disable : 4200 )
#include "..\KLSDK\baseapi.h"
#include "..\KLSDK\klstatus.h"
#include "..\KLSDK\kl_ipaddr.h"

#define KLIN_API_VERSION                 0x1

#define KLIN_FILE_DEVICE_SECTION    0x00008005

#define KLIN_IOCTL_INDEX            0x841

#define KL_PROCNAMELEN              16
#define KL_USERAPPNAMELEN	        1100
#define KL_HASH_SIZE	            16

#define KLIN_NAME      L"KLIN"
#define KLIN_NAME_9x   "KLIN    "

typedef struct _MAKE_DISCONNECT_ITEM MAKE_DISCONNECT_ITEM, *PMAKE_DISCONNECT_ITEM;

typedef
KLSTATUS
(*QueryHandleHandler)(PVOID Handle, ULONG   InformationId, PCHAR    pBuffer, ULONG Size);

typedef
KLSTATUS
(*GetListHandler)(PVOID pBuffer, ULONG BufferSize, ULONG*   BytesNeeded);

typedef
KLSTATUS
(*MakeDisconnectHandler)(PMAKE_DISCONNECT_ITEM  discItem);

typedef
KLSTATUS
(*RedirectHandler)( PVOID Handle, ip_addr_t* Ip, USHORT Port, char*  HelloBuffer, ULONG HelloBufferSize);

typedef
KLSTATUS
(*CorrectPortHandler) ( PVOID Handle );

typedef
KLSTATUS
(*SetIDHandler) ( PVOID Handle, __int64 ID );

typedef
KLSTATUS
(*ObtainHandleHandler)( ip_addr_t*  lIp, USHORT lPort, PVOID* pHandle);

typedef
void         
(*f_AssociateConnections)( 
                      ULONG lIp1, USHORT lPort1, ULONG rIp1, USHORT rPort1,
                      ULONG lIp2, USHORT lPort2, ULONG rIp2, USHORT rPort2,
                      ULONG Association );

// Информация о TDI функциях, которые нужны плагину
typedef struct _KLIN_INFO {
    QueryHandleHandler      QueryAddr;
    QueryHandleHandler      QueryConn;
    GetListHandler          GetAddrList;
    GetListHandler          GetConnList;
    MakeDisconnectHandler   MakeDisconnect;    
    RedirectHandler         Redirect;
    CorrectPortHandler      CorrectPort;
    SetIDHandler            SetID;
    ObtainHandleHandler     ObtainHandle;
    GetListHandler          GetAddrList2;
    GetListHandler          GetConnList2;

    GetListHandler          GetAddrHandleList;
    GetListHandler          GetConnHandleList;

    GetListHandler          GetAddrList3;
    GetListHandler          GetConnList3;

    GetListHandler          GetAddrList4;
    GetListHandler          GetConnList4;

	void	(*f_AcquireAddrListLock)	();
	void	(*f_ReleaseAddrListLock)	();

	void	(*f_AcquireConnListLock)	();
	void	(*f_ReleaseConnListLock)	();

	QueryHandleHandler      f_QueryAddrNoLock;
    QueryHandleHandler      f_QueryConnNoLock;

	PVOID	(*f_GetAddrByHandle)( PVOID Handle );
	PVOID	(*f_GetConnByHandle)( PVOID Handle );

    ULONG   (*f_GetConnectionCount) ( void );
    ULONG   (*f_GetOpenPortCount) ( void );

    f_AssociateConnections    AssociateCR;

    KLSTATUS (*f_SetTrusted )  ( PVOID Handle, BOOLEAN Value );

} KLIN_INFO, *PKLIN_INFO;

enum QUERY_TYPE
{
    QueryLocalIp   =   1,   // ip_addr_t
    QueryLocalPort,
    QueryRemoteIp,          // ip_addr_t
    QueryRemotePort,
    QueryPID,
    QueryProtocol,
    QueryProcess,
    QueryImage,
    QueryHash,
    QueryThreadID,
    QueryInBytes,
    QueryOutBytes,
    QueryOpenFromTime,
    QueryCommandLine,
	QueryConnIsRedirected,
    QueryIsTrusted,
    QueryZone,
    QueryID
};


// Получить версию API драйвера.
// outBuffer = *DWORD
#define KLIN_GET_VERSION		BASE_GET_VERSION

// Начать перехват
#define KLIN_START_FILTER		BASE_START_FILTER

// Прекратить перехват
#define KLIN_STOP_FILTER		BASE_STOP_FILTER

#define KLIN_PAUSE_FILTER		BASE_PAUSE_FILTER

#define KLIN_RESUME_FILTER		BASE_RESUME_FILTER

// Зарегистрировать Callback для плагина.
// inBuffer = PluginInitEntry (DWORD), Context(DWORD)
#define KLIN_REGISTER_PLUGIN	BASE_REGISTER_PLUGIN

// InBuffer = PluginID, полученный при инициализации
#define KLIN_DEREGISTER_PLUGIN	BASE_DEREGISTER_PLUGIN

// Запрос для накопления запросов.
// Если требуется передать информацию юзеру - выполняем Complete запроса.
#define KLIN_ACCUMULATE_REQUEST	CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+100,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#pragma pack( push, 1)
typedef struct _ADDR_LIST_ITEM {    
    unsigned long	Protocol;                       // TCP, UDP, IP ....
    unsigned long	LocalIp;
    unsigned short	LocalPort;	
    unsigned long	PID;                            // PID процесса
    wchar_t	        UserAppName[KL_USERAPPNAMELEN]; // исполняемый модуль     
} ADDR_LIST_ITEM, *PADDR_LIST_ITEM;


// outBuffer = ULONG (count), ADDR_LIST_ITEM's * count
#define KLIN_GET_ADDR_LIST	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+101,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)


typedef struct _CONN_LIST_ITEM {    
    unsigned long	Protocol;                       // TCP, UDP, IP ....
    unsigned long	LocalIp;
    unsigned short	LocalPort;	
    unsigned long	RemoteIp;
	unsigned short	RemotePort;	
    unsigned long	PID;                            // PID процесса    
    unsigned char	isIncoming;	
    wchar_t	        UserAppName[KL_USERAPPNAMELEN]; // исполняемый модуль    
} CONN_LIST_ITEM, *PCONN_LIST_ITEM;

// outBuffer = ULONG (count), CONN_LIST_ITEM's * count
#define KLIN_GET_CONN_LIST	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+102,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)


typedef struct _MAKE_DISCONNECT_ITEM	{
	unsigned char	Protocol;
	ip_addr_t	    LocalIp;
	unsigned short	LocalPort;
	ip_addr_t	    RemoteIp;
	unsigned short	RemotePort;	
} MAKE_DISCONNECT_ITEM, *PMAKE_DISCONNECT_ITEM;


// inBuffer = MAKE_DISCONNECT_ITEM
#define KLIN_MAKE_DISCONNECT    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+103,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// InBuffer = *DebugLevel, *DebugMask
#define KLIN_SET_DEBUG	        CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX + 104,						    \
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)
// OutBuffer = *DebugLevel, *DebugMask
#define KLIN_GET_DEBUG	        CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX + 105,						    \
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)


typedef struct _ADDR_LIST_ITEM2 {    
    unsigned long	Protocol;                       // TCP, UDP, IP .... ??
    unsigned long	LocalIp;
    unsigned short	LocalPort;	
    unsigned long	PID;                            // PID процесса        
    unsigned long   ActiveTime;                     // Время открытости порта ( в секундах )
    __int64         inBytes;                        // количество полученных байт
    __int64         outBytes;                       // количество отправленных байт
    wchar_t	        UserAppName[KL_USERAPPNAMELEN]; // исполняемый модуль
} ADDR_LIST_ITEM2, *PADDR_LIST_ITEM2;

// outBuffer = ULONG (count), ADDR_LIST_ITEM's * count
#define KLIN_GET_ADDR_LIST2	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+106,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

typedef struct _CONN_LIST_ITEM2 {    
    unsigned long	Protocol;                       // TCP, UDP, IP ....
    unsigned long	LocalIp;
    unsigned short	LocalPort;	
    unsigned long	RemoteIp;
	unsigned short	RemotePort;	
    unsigned long	PID;                            // PID процесса    
    unsigned char	isIncoming;	
    unsigned long   ActiveTime;                     // Время активности соединения ( в секундах )
    __int64         inBytes;                        // количество полученных байт
    __int64         outBytes;                       // количество отправленных байт
    wchar_t	        UserAppName[KL_USERAPPNAMELEN]; // исполняемый модуль    
} CONN_LIST_ITEM2, *PCONN_LIST_ITEM2;

// outBuffer = ULONG (count), CONN_LIST_ITEM's * count
#define KLIN_GET_CONN_LIST2	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+107,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

typedef struct _ADDR_LIST_ITEM3 {    
    unsigned long	Protocol;                       // TCP, UDP, IP .... ??
    unsigned long	LocalIp;
    unsigned short	LocalPort;	
    unsigned long	PID;                            // PID процесса        
    unsigned long   ActiveTime;                     // Время открытости порта ( в секундах )
    __int64         inBytes;                        // количество полученных байт
    __int64         outBytes;                       // количество отправленных байт
    wchar_t	        UserAppName[KL_USERAPPNAMELEN]; // исполняемый модуль
    wchar_t	        CommandLine[KL_USERAPPNAMELEN]; // исполняемый модуль
} ADDR_LIST_ITEM3, *PADDR_LIST_ITEM3;

// outBuffer = ULONG (count), ADDR_LIST_ITEM's * count
#define KLIN_GET_ADDR_LIST3	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+108,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

typedef struct _CONN_LIST_ITEM3 {    
    unsigned long	Protocol;                       // TCP, UDP, IP ....
    unsigned long	LocalIp;
    unsigned short	LocalPort;	
    unsigned long	RemoteIp;
	unsigned short	RemotePort;	
    unsigned long	PID;                            // PID процесса    
    unsigned char	isIncoming;	
    unsigned long   ActiveTime;                     // Время активности соединения ( в секундах )
    __int64         inBytes;                        // количество полученных байт
    __int64         outBytes;                       // количество отправленных байт
    wchar_t	        UserAppName[KL_USERAPPNAMELEN]; // исполняемый модуль    
    wchar_t	        CommandLine[KL_USERAPPNAMELEN]; // исполняемый модуль    
} CONN_LIST_ITEM3, *PCONN_LIST_ITEM3;

// outBuffer = ULONG (count), CONN_LIST_ITEM's * count
#define KLIN_GET_CONN_LIST3	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+109,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define KLIN_DISABLE_DEBUG	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX + 110,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define KLIN_ENABLE_DEBUG	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX + 111,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)


typedef struct _KLIF_TEST_PKT_EVENT {    
    ULONG isIncoming : 1;
    ULONG isIncomingStream : 1;
    ULONG isDispatchLevel : 1;

    ULONG rIp;
    ULONG lIp;
    USHORT rPort;
    USHORT lPort;
    UCHAR Protocol;
} KLIF_TEST_PKT_EVENT, *PKLIF_TEST_PKT_EVENT;
#define KLIN_CHECK_KLIF_STATUS  CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX + 112,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)


typedef struct _ADDR_LIST_ITEM4 {        
    unsigned long	Protocol;                       // TCP, UDP, IP .... ??
    ip_addr_t         LocalIp;
    unsigned short	LocalPort;	
    unsigned long	PID;                            // PID процесса        
    unsigned long   ActiveTime;                     // Время открытости порта ( в секундах )
    __int64         inBytes;                        // количество полученных байт
    __int64         outBytes;                       // количество отправленных байт
    wchar_t	        UserAppName[KL_USERAPPNAMELEN]; // исполняемый модуль
    wchar_t	        CommandLine[KL_USERAPPNAMELEN]; // command line
} ADDR_LIST_ITEM4, *PADDR_LIST_ITEM4;

// outBuffer = ULONG (count), ADDR_LIST_ITEM's * count
#define KLIN_GET_ADDR_LIST4	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+113,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

typedef struct _CONN_LIST_ITEM4 {
    unsigned long	Protocol;                       // TCP, UDP, IP ....

    ip_addr_t         LocalIp;
    ip_addr_t         RemoteIp;

    unsigned short	LocalPort;
    unsigned short	RemotePort;	
    unsigned long	PID;                            // PID процесса    
    unsigned char	isIncoming;	
    unsigned long   ActiveTime;                     // Время активности соединения ( в секундах )
    __int64         inBytes;                        // количество полученных байт
    __int64         outBytes;                       // количество отправленных байт
    wchar_t	        UserAppName[KL_USERAPPNAMELEN]; // исполняемый модуль    
    wchar_t	        CommandLine[KL_USERAPPNAMELEN]; // command line
} CONN_LIST_ITEM4, *PCONN_LIST_ITEM4;

// outBuffer = ULONG (count), CONN_LIST_ITEM's * count
#define KLIN_GET_CONN_LIST4	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+114,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

/*
typedef struct _KL_PEER {
    HANDLE  Context;
    ULONG   Ip;
    USHORT  Port;
    UCHAR   DataBuffer[256];
} KL_PEER, *PKL_PEER;
// InBuffer = KL_PEER
// OutBuffer = HANDLE(Context)
#define KLIN_TDI_OPEN_PORT	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+115,							\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// InBuffer = KL_PEER
#define KLIN_TDI_CONNECT	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+116,							\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// InBuffer = KL_PEER
#define KLIN_TDI_SEND	        CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+117,							\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// InBuffer = HANDLE
#define KLIN_TDI_DISCONNECT	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+118,							\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)
// InBuffer = HANDLE
#define KLIN_TDI_CLOSE_PORT	    CTL_CODE(KLIN_FILE_DEVICE_SECTION,				\
								KLIN_IOCTL_INDEX+119,							\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)
*/

#pragma pack( pop )


#endif //_KLIN_API_

/*
 *	API для драйвера KLICK.sys
 *  Определения вызовов IOCTL, а также структур, передаваемых в эти вызовы.
 *
 *  Автор : Рощин Е.Е.
 *
 */

#ifndef _KLICK_API_
#define _KLICK_API_

#pragma warning( disable : 4200 )
#include "../KLSDK/baseapi.h"
#include "../KLSDK/klstatus.h"
#include "../klsdk/kl_ipaddr.h"
#define KLICK_API_VERSION  0x1

#define KLICK_FILE_DEVICE_SECTION    BASE_FILE_DEVICE_SECTION

#define KLICK_IOCTL_INDEX            BASE_IOCTL_INDEX

#define KLICK_NAME      L"KLICK"
#define KLICK_NAME_9x   "KLICK   "

#define KLICK_MJ_ATTACK_NOTIFY		0x1
#define KLICK_MJ_NDIS_PACKET_IN		0xa
#define KLICK_MJ_NDIS_PACKET_OUT	0xb

#define KLICK_MJ_LICENCE_BUFFER		0xc

typedef enum _PacketInfoType {
    pktINFO_PacketSize,
    pktINFO_PacketType,
    pktINFO_Protocol,
    pktINFO_Ihl,
    pktINFO_IpVer,
    
    pktINFO_srcIp,  // ip_addr_t
    pktINFO_dstIp,  // ip_addr_t

    pktINFO_IpDataOffset,
    
    pktINFO_srcPort,
    pktINFO_dstPort,

    pktINFO_Identification,
    pktINFO_FragmentOffset,
    
    pktINFO_SeqNumber,
    pktINFO_AckNumber,

    pktINFO_isFragmented,
    pktINFO_isBroadCast,
    
    pktINFO_bMF,
    pktINFO_bDF,
    pktINFO_bReserved,

    pktINFO_isIncoming,
    pktINFO_isIncomingStream,
    
    pktINFO_TrustedFlags,

    pktINFO_fin,
    pktINFO_syn,
    pktINFO_rst,
    pktINFO_psh,
    pktINFO_ack,
    pktINFO_urg,
    pktINFO_ece,
    pktINFO_cwr,

    pktINFO_IcmpType,
    pktINFO_IcmpCode,

    pktINFO_StreamHandle,
    pktINFO_Time,
    pktINFO_Zone
} PacketInfoType;

#define TS_PKT_IN               0x1
#define TS_PKT_OUT              0x2
#define TS_PKT_SET_STATUS       0x4
#define TS_APP_IN               0x8
#define TS_APP_OUT              0x10

// версия таблицы.
// В случае когда добавляется функция или меняется функционал, версию поднять на 1
#define KLICK_TABLE_VERSION 0x1

// Таблица функций, экспортируемых KLICK
typedef struct _KLICK_INFO {
    // ULONG           Version;
    
    unsigned long   (*GetLinearBuffer)      ( void*  pktContext, void* buffer, ULONG from, ULONG count );
    unsigned long   (*GetPacketSize)        ( void*  pktContext );
    
    KLSTATUS        (*GetDword)             ( void*  pktContext, ULONG*  val,  ULONG  offset );
    KLSTATUS        (*GetWord)              ( void*  pktContext, USHORT* val,  ULONG  offset );
    KLSTATUS        (*GetByte)              ( void*  pktContext, UCHAR*  val,  ULONG  offset );
    
    BOOLEAN         (*IsIncoming)           ( void*  pktContext );
    KLSTATUS        (*ReceivePacket)        ( void*  pktContext );
    KLSTATUS        (*SendPacket)           ( void*  pktContext );
    unsigned long   (*RefPacket)            ( void*  pktContext );
    unsigned long   (*DerefPacket)          ( void*  pktContext );    
    void            (*SetStatus)            ( void*  pktContext, KLSTATUS klStatus );
    
    BOOLEAN         (*IsIncomingStream)     ( void*  pktContext );
    void*           (*GetStreamHandle)      ( void*  pktContext );
    
    KLSTATUS        (*AddStream)            ( ip_addr_t* srcIp, USHORT srcPort, ip_addr_t* dstIp, USHORT dstPort, UCHAR Protocol, BOOLEAN isIncoming );
    KLSTATUS        (*RemoveStream)         ( ip_addr_t* srcIp, USHORT srcPort, ip_addr_t* dstIp, USHORT dstPort, UCHAR Protocol );    

    KLSTATUS        (*f_StreamSetTrusted )  ( void*  pktContext, ULONG Flags );

    KLSTATUS        (*f_StreamSetTrusted2 )  ( 
                                            IN ip_addr_t*    srcIp, 
                                            IN USHORT   srcPort, 
                                            IN ip_addr_t*    dstIp, 
                                            IN USHORT   dstPort, 
                                            IN UCHAR    Protocol, 
                                            IN ULONG    Flags );

    KLSTATUS        (*GetHostStatistics)    ( PVOID StatBuffer, ULONG StatBufferSize, ULONG* StatVer );

    KLSTATUS        (*GetHostStatistics2)    ( PVOID StatBuffer, ULONG StatBufferSize, ULONG* StatVer );

    KLSTATUS        (*GetStreamInfo)        (  
                                            IN ip_addr_t*        srcIp, 
                                            IN USHORT       srcPort, 
                                            IN ip_addr_t*        dstIp, 
                                            IN USHORT       dstPort, 
                                            IN UCHAR        Protocol,
                                            OUT PVOID*      pStreamHandle, 
                                            OUT ULONG*      pIsIncoming,
											OUT ULONG*      pInSeqNumber,
											OUT ULONG*      pOutSeqNumber,
                                            OUT ULONG*      pTrustedFlags );

    KLSTATUS        (*GetStreamInfo2)   ( 
                                            IN  void*       pktContext,
                                            OUT PVOID*      pStreamHandle, 
                                            OUT ULONG*      pIsIncoming,
											OUT ULONG*      pInSeqNumber,
											OUT ULONG*      pOutSeqNumber,
                                            OUT ULONG*      pTrustedFlags);
	
	KLSTATUS		(*f_GetAdapterHandleList)( IN void*	Buffer, IN ULONG BufferSize, OUT ULONG* HandleCount );

	KLSTATUS		(*f_GetAdapterInfo)		( IN ULONG AdapterHandle, IN ULONG ID, IN PVOID Buffer, IN ULONG BufferSize );

	KLSTATUS		(*f_SetAdapterInfo)		( IN ULONG AdapterHandle, IN ULONG ID, IN PVOID Buffer, IN ULONG BufferSize );

	BOOLEAN			(*f_isFromStealthedAdapter)(void* pktContext);	

    KLSTATUS		(*f_QueryPacketInfo )( void* pktContext, PacketInfoType InfoType, PVOID Buffer, ULONG BufferSize );

    PVOID           reserved[4];

} KLICK_INFO, *PKLICK_INFO;

// KID = Klick ID
enum ADAPTER_QUERY_TYPE
{
    KID_MacAddress = 1,
	KID_QueryName,
	KID_Stealthed
};

#define HOST_STAT_VER 0x1

#pragma pack( push, 1)
typedef struct _HostStat
{
    __int64 inBytes;
    __int64 outBytes;
} HostStat, *PHostStat;

typedef struct _HostStatItem 
{
    ULONG       HostIp;

    HostStat    TotalStat;
    HostStat    TcpStat;
    HostStat    UdpStat;
    HostStat    BroadcastStat; 

    // KL_TIME     LastPktTime;
} HostStatItem, *PHostStatItem;

typedef struct _HostStatItem2
{    
    ip_addr_t  HostIp;

    HostStat    TotalStat;
    HostStat    TcpStat;
    HostStat    UdpStat;
    HostStat    BroadcastStat; 

    // KL_TIME     LastPktTime;
} HostStatItem2, *PHostStatItem2;

typedef struct _HostStatTable {
    ULONG           Version;
    ULONG           ItemCount;
    // HostStat        TotalTraffic;
    HostStatItem    Item[0];
} HostStatTable, *PHostStatTable;

typedef struct _HostStatTable2 {    
    ULONG           ItemCount;
    // HostStat        TotalTraffic;
    HostStatItem2    Item[0];
} HostStatTable2, *PHostStatTable2;
#pragma pack( pop )

// Получить версию API драйвера.
// outBuffer = *DWORD
#define KLICK_GET_VERSION		BASE_GET_VERSION

// Начать перехват
#define KLICK_START_FILTER		BASE_START_FILTER

// Прекратить перехват
#define KLICK_STOP_FILTER		BASE_STOP_FILTER

#define KLICK_PAUSE_FILTER		BASE_PAUSE_FILTER

#define KLICK_RESUME_FILTER		BASE_RESUME_FILTER

// Зарегистрировать Callback для плагина.
// inBuffer = PREGISTER_BUFFER
#define KLICK_REGISTER_PLUGIN	BASE_REGISTER_PLUGIN

// InBuffer = PluginID, полученный при инициализации
#define KLICK_DEREGISTER_PLUGIN	BASE_DEREGISTER_PLUGIN

// данное сообщение необходимо доставить плагину
// inBuffer     = PPLUGIN_MESSAGE
// outBuffer    = PPLUGIN_MESSAGE
#define KLICK_MESSAGE           BASE_DELIVER_MESSAGE

// InBuffer = *DebugLevel, *DebugMask
#define KLICK_SET_DEBUG	        CTL_CODE(KLICK_FILE_DEVICE_SECTION,				\
								KLICK_IOCTL_INDEX + 100,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)
// OutBuffer = *DebugLevel, *DebugMask
#define KLICK_GET_DEBUG	        CTL_CODE(KLICK_FILE_DEVICE_SECTION,				\
								KLICK_IOCTL_INDEX + 101,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define KLICK_DISABLE_DEBUG	    CTL_CODE(KLICK_FILE_DEVICE_SECTION,				\
								KLICK_IOCTL_INDEX + 102,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define KLICK_ENABLE_DEBUG	    CTL_CODE(KLICK_FILE_DEVICE_SECTION,				\
								KLICK_IOCTL_INDEX + 103,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)


typedef struct _StreamTable
{
    ULONG           Count;
    
	struct CStream
    {
        ULONG       srcIp;        
        ULONG       dstIp;
        USHORT      srcPort;
        USHORT      dstPort;
		ULONG		isIncoming;
		ULONG		InSeqNum;
		ULONG		OutSeqNum;
    } tbl[0];
} StreamTable;
#define  KLICK_GET_TCP_STREAMS	CTL_CODE(KLICK_FILE_DEVICE_SECTION,				\
								KLICK_IOCTL_INDEX + 200,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define  KLICK_GET_UDP_STREAMS	CTL_CODE(KLICK_FILE_DEVICE_SECTION,				\
								KLICK_IOCTL_INDEX + 201,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

typedef struct _IDS_PACKET {        
    ULONG   isIncoming;
    ULONG   PacketSize;
    char    PacketBuffer[0];
} IDS_PACKET, *PIDS_PACKET;

// inBuffer = IDS_PACKET, outBuffer = KLSTATUS ( KL_SUCCESS, KL_UNSUCCESSFUL )
#define  KLICK_PROCESS_PACKET   CTL_CODE(KLICK_FILE_DEVICE_SECTION,				\
								KLICK_IOCTL_INDEX + 202,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)


#define  KLICK_PROCESS_PACKET_ARRAY   CTL_CODE(KLICK_FILE_DEVICE_SECTION,		\
								KLICK_IOCTL_INDEX + 203,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define  KLICK_PROCESS_INIT     CTL_CODE(KLICK_FILE_DEVICE_SECTION,		\
								KLICK_IOCTL_INDEX + 204,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define  KLICK_PROCESS_DONE     CTL_CODE(KLICK_FILE_DEVICE_SECTION,		\
								KLICK_IOCTL_INDEX + 205,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define  KLICK_RESET_TRUSTED_STREAMS	CTL_CODE(KLICK_FILE_DEVICE_SECTION,		\
								KLICK_IOCTL_INDEX + 206,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)
/*
typedef struct _StreamTable2
{
    ULONG           Count;
    
	struct CStream
    {
        UCHAR       srcIpVer;
        union {
            ULONG       srcIp;
            USHORT      srcIpv6;
        };
        
        UCHAR       dstIpVer;
        union {
            ULONG       dstIp;
            USHORT      dstIpv6;
        };

        USHORT      srcPort;
        USHORT      dstPort;
		ULONG		isIncoming;
		ULONG		InSeqNum;
		ULONG		OutSeqNum;
    } tbl[0];
} StreamTable2;
*/
#define  KLICK_GET_TCP_STREAMS2	CTL_CODE(KLICK_FILE_DEVICE_SECTION,				\
								KLICK_IOCTL_INDEX + 207,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define  KLICK_GET_UDP_STREAMS2	CTL_CODE(KLICK_FILE_DEVICE_SECTION,				\
								KLICK_IOCTL_INDEX + 208,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define  KLICK_RESET_STREAMS	CTL_CODE(KLICK_FILE_DEVICE_SECTION,		        \
								KLICK_IOCTL_INDEX + 209,						\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)



#define KLICK_GET_INFO   CTL_CODE(KLICK_FILE_DEVICE_SECTION,			\
								KLICK_IOCTL_INDEX + 255,				\
								METHOD_BUFFERED,						\
								FILE_ANY_ACCESS)

#define KLICK_SET_INFO   CTL_CODE(KLICK_FILE_DEVICE_SECTION,			\
								KLICK_IOCTL_INDEX + 256,				\
								METHOD_BUFFERED,						\
								FILE_ANY_ACCESS)

// inBuffer : char*  - имя IM девайса.
#define KLICK_ATTACH_IM   CTL_CODE(KLICK_FILE_DEVICE_SECTION,			\
								KLICK_IOCTL_INDEX + 257,				\
								METHOD_BUFFERED,						\
								FILE_ANY_ACCESS)

#endif // _KLICK_API_

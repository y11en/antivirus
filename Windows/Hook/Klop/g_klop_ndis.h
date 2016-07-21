#ifndef _KLOP_NDIS_H__
#define _KLOP_NDIS_H__

#include "kldef.h"
#include "klstatus.h"
#include "kl_list.h"
#include "klopevent.h"
#include "..\hook\klop_api.h"
#include "g_cp_proto.h"

#define     IP_HEADER_SIZE  20

//
// This macro will compare network addresses.
//
//  A - Is a network address.
//
//  B - Is a network address.
//
//  Result - The result of comparing two network address.
//
//  Result != 0 Implies inequality.
//  Result == 0 Implies equality.
//
//
#define ETH_COMPARE_NETWORK_ADDRESSES_EQ(_A,_B, _Result)		\
{																\
	if ((*(ULONG UNALIGNED *)&(_A)[2] ==						\
			*(ULONG UNALIGNED *)&(_B)[2]) &&					\
		 (*(USHORT UNALIGNED *)(_A) ==							\
			*(USHORT UNALIGNED *)(_B)))							\
	{															\
		*(_Result) = 0;											\
	}															\
	else														\
	{															\
		*(_Result) = 1;											\
	}															\
}

#ifndef NDIS_API
#define  NDIS_API
#endif

#ifndef OID_GEN_PHYSICAL_MEDIUM
#define OID_GEN_PHYSICAL_MEDIUM                 0x00010202
#endif

typedef enum _NDIS_PHYSICAL_MEDIUM
{
    NdisPhysicalMediumUnspecified,
    NdisPhysicalMediumWirelessLan,
    NdisPhysicalMediumCableModem,
    NdisPhysicalMediumPhoneLine,
    NdisPhysicalMediumPowerLine,
    NdisPhysicalMediumDSL,      // includes ADSL and UADSL (G.Lite)
    NdisPhysicalMediumFibreChannel,
    NdisPhysicalMedium1394,
    NdisPhysicalMediumWirelessWan,
    NdisPhysicalMediumMax       // Not a real physical type, defined as an upper-bound
} NDIS_PHYSICAL_MEDIUM, *PNDIS_PHYSICAL_MEDIUM;

#pragma pack( push, 1)



#ifdef ISWIN9X
//
//	PnP and PM event codes that can be indicated up to transports
//	and clients.
//
typedef enum _NET_PNP_EVENT_CODE
{
	NetEventSetPower,
	NetEventQueryPower,
	NetEventQueryRemoveDevice,
	NetEventCancelRemoveDevice,
	NetEventReconfigure,
	NetEventBindList,
	NetEventMaximum
} NET_PNP_EVENT_CODE, *PNET_PNP_EVENT_CODE;

//
//	Networking PnP event indication structure.
//
typedef struct _NET_PNP_EVENT
{
	//
	//	Event code describing action to take.
	//
	NET_PNP_EVENT_CODE	NetEvent;
	
	//
	//	Event specific data.
	//
	PVOID				Buffer;
	
	//
	//	Length of event specific data.
	//
	ULONG				BufferLength;
	
	//
	//	Reserved values are for use by respective components only.
	//	
	UCHAR				NdisReserved[16];
	UCHAR				TransportReserved[16];
	UCHAR				TdiReserved[16];
	UCHAR				TdiClientReserved[16];
} NET_PNP_EVENT, *PNET_PNP_EVENT;

// WinME functions -------------------------------------------------------------------
typedef
INT
(*RECEIVE_PACKET_HANDLER)(
                IN	NDIS_HANDLE				ProtocolBindingContext,
                IN	PNDIS_PACKET			Packet
                        );

typedef
NDIS_STATUS
(*PNP_EVENT_HANDLER)(
                IN	NDIS_HANDLE				ProtocolBindingContext,
                IN	PNET_PNP_EVENT			NetPnPEvent
                );

typedef
VOID
(*BIND_HANDLER)(
				OUT	PNDIS_STATUS			Status,
				IN	NDIS_HANDLE				BindContext,
				IN	PNDIS_STRING			DeviceName,
				IN	PVOID					SystemSpecific1,
				IN	PVOID					SystemSpecific2
				);

typedef
VOID
(*UNBIND_HANDLER)(
                OUT	PNDIS_STATUS			Status,
                IN	NDIS_HANDLE				ProtocolBindingContext,
                IN	NDIS_HANDLE				UnbindContext
                );

#endif // ISWIN9X

typedef struct _NDIS_PCHARS {
    UCHAR MajorNdisVersion;
    UCHAR MinorNdisVersion;

    USHORT  Filler;

    UINT Reserved;

    OPEN_ADAPTER_COMPLETE_HANDLER           OpenAdapterCompleteHandler;
    CLOSE_ADAPTER_COMPLETE_HANDLER          CloseAdapterCompleteHandler;
    SEND_COMPLETE_HANDLER                   SendCompleteHandler;
    TRANSFER_DATA_COMPLETE_HANDLER          TransferDataCompleteHandler;
    RESET_COMPLETE_HANDLER                  ResetCompleteHandler;
    REQUEST_COMPLETE_HANDLER                RequestCompleteHandler;
    RECEIVE_HANDLER                         ReceiveHandler;
    RECEIVE_COMPLETE_HANDLER                ReceiveCompleteHandler;
    STATUS_HANDLER                          StatusHandler;
    STATUS_COMPLETE_HANDLER                 StatusCompleteHandler;
    NDIS_STRING     Name;
//
// MajorNdisVersion must be set to 0x04 or 0x05
// with any of the following members.
//
    RECEIVE_PACKET_HANDLER                  ReceivePacketHandler;
    BIND_HANDLER                            BindAdapterHandler;
    UNBIND_HANDLER                          UnbindAdapterHandler;
    PNP_EVENT_HANDLER                       PnPEventHandler;
    UNLOAD_PROTOCOL_HANDLER                 UnloadHandler;

} NDIS_PCHARS, *PNDIS_PCHARS;

typedef struct _NDIS_PCHARS_9X {
    UCHAR MajorNdisVersion;
    UCHAR MinorNdisVersion;
    ULONG Reserved;
    OPEN_ADAPTER_COMPLETE_HANDLER           OpenAdapterCompleteHandler;
    CLOSE_ADAPTER_COMPLETE_HANDLER          CloseAdapterCompleteHandler;
    SEND_COMPLETE_HANDLER                   SendCompleteHandler;
    TRANSFER_DATA_COMPLETE_HANDLER          TransferDataCompleteHandler;
    RESET_COMPLETE_HANDLER                  ResetCompleteHandler;
    REQUEST_COMPLETE_HANDLER                RequestCompleteHandler;
    RECEIVE_HANDLER                         ReceiveHandler;
    RECEIVE_COMPLETE_HANDLER                ReceiveCompleteHandler;
    STATUS_HANDLER                          StatusHandler;
    STATUS_COMPLETE_HANDLER                 StatusCompleteHandler;
    BIND_HANDLER                            BindAdapterHandler;
    UNBIND_HANDLER                          UnbindAdapterHandler;
    UNLOAD_PROTOCOL_HANDLER                 UnloadProtocolHandler;
    NDIS_STRING Name;
} NDIS_PCHARS_9X, *PNDIS_PCHARS_9X;

typedef
PVOID
(NDIS_API *REGISTER_PROTOCOL)(PNDIS_STATUS, PNDIS_HANDLE, PNDIS_PCHARS, UINT);

typedef
PVOID
(NDIS_API *DEREGISTER_PROTOCOL)(PNDIS_STATUS, NDIS_HANDLE);

typedef
PVOID
(NDIS_API *OPEN_ADAPTER)(PNDIS_STATUS, PNDIS_STATUS, PNDIS_HANDLE, PUINT, PNDIS_MEDIUM, UINT, NDIS_HANDLE, NDIS_HANDLE, PNDIS_STRING, UINT, PSTRING);

typedef
PVOID
(NDIS_API *CLOSE_ADAPTER)(PNDIS_STATUS, NDIS_HANDLE);

typedef
PVOID
(NDIS_API *MY_REQUEST_HANDLER)(PNDIS_STATUS, NDIS_HANDLE, PNDIS_REQUEST);

typedef
PVOID
(NDIS_API *MY_RESET_HANDLER)(PNDIS_STATUS, NDIS_HANDLE);

typedef
PVOID
(NDIS_API *RETURN_PACKET)(PNDIS_PACKET*, UINT);

VOID
NDIS_API
HNdisRegisterProtocol(OUT   PNDIS_STATUS    Status,
					  OUT   PNDIS_HANDLE    NdisProtocolHandle,
					  IN    PNDIS_PCHARS    ProtocolCharacteristics,
					  IN    UINT            CharacteristicsLength
					  );

VOID
NDIS_API
HNdisOpenAdapter(
				OUT PNDIS_STATUS	Status,
				OUT PNDIS_STATUS	OpenErrorStatus,
				OUT PNDIS_HANDLE	NdisBindingHandle,
				OUT PUINT			SelectedMediumIndex,
				IN PNDIS_MEDIUM		MediumArray,
				IN UINT				MediumArraySize,
				IN NDIS_HANDLE		NdisProtocolHandle,
				IN NDIS_HANDLE		ProtocolBindingContext,
				IN PNDIS_STRING		AdapterName,
				IN UINT				OpenOptions,
				IN PSTRING			AddressingInformation  OPTIONAL
				);

VOID
NDIS_API
HNdisCloseAdapter(
				  OUT PNDIS_STATUS  Status,
				  IN NDIS_HANDLE  NdisBindingHandle
				  );
VOID
NDIS_API
HNdisDeregisterProtocol(
						OUT PNDIS_STATUS  Status,
						IN NDIS_HANDLE  NdisProtocolHandle
						);

VOID
OnUnbindAdapter(
				OUT PNDIS_STATUS  Status,
				IN NDIS_HANDLE  ProtocolBindingContext,
				IN NDIS_HANDLE  UnbindContext
				);

NDIS_STATUS
SendRequest(
                NDIS_HANDLE         ProtocolBindingContext, 
                NDIS_REQUEST_TYPE   Type,
                NDIS_OID            Oid,
                PVOID               Buffer, 
                ULONG               BufferSize);

VOID 
NDIS_API
OnRequestComplete( IN NDIS_HANDLE ProtocolBindingContext, 
			   IN PNDIS_REQUEST NdisRequest, 
			   IN NDIS_STATUS Status ) ;

NDIS_STATUS
GetMacAddr(NDIS_HANDLE ProtocolBindingContext);

VOID 
NDIS_API
OnOpenAdapterComplete( IN NDIS_HANDLE ProtocolBindingContext, 
				   IN NDIS_STATUS Status, 
				   IN NDIS_STATUS OpenErrorStatus ) ;

VOID 
NDIS_API
OnCloseAdapterComplete( IN NDIS_HANDLE ProtocolBindingContext, 
				    IN NDIS_STATUS Status );

VOID 
NDIS_API
OnSendComplete( IN NDIS_HANDLE ProtocolBindingContext, 
		    IN PNDIS_PACKET pPacket, 
			IN NDIS_STATUS Status );

VOID 
NDIS_API
OnTransferDataComplete ( IN NDIS_HANDLE     ProtocolBindingContext, 
					 IN PNDIS_PACKET    NdisPacket, 
					 IN NDIS_STATUS     Status, 
					 IN UINT            BytesTransfered ) ;

VOID 
NDIS_API
OnResetComplete( IN NDIS_HANDLE ProtocolBindingContext, 
				  IN NDIS_STATUS Status ) ;

NDIS_STATUS 
NDIS_API
OnReceiveSmallPacket( 
               IN NDIS_HANDLE ProtocolBindingContext, 
			   IN NDIS_HANDLE MacReceiveContext, 
			   IN PVOID HeaderBuffer, 
			   IN UINT HeaderBufferSize, 
			   IN PVOID LookAheadBuffer, 
			   IN UINT LookaheadBufferSize, 
			   UINT PacketSize ) ;

NDIS_STATUS 
NDIS_API
OnReceive( IN NDIS_HANDLE ProtocolBindingContext, 
              IN NDIS_HANDLE MacReceiveContext, 
              IN PVOID HeaderBuffer, 
			   IN UINT HeaderBufferSize, 
			   IN PVOID LookAheadBuffer, 
			   IN UINT LookaheadBufferSize, 
			   UINT PacketSize ) ;

VOID 
NDIS_API
OnReceiveComplete( IN NDIS_HANDLE ProtocolBindingContext ) ;

VOID 
NDIS_API
OnStatus( IN NDIS_HANDLE ProtocolBindingContext, 
		  IN NDIS_STATUS Status, 
		  IN PVOID StatusBuffer, 
		  IN UINT StatusBufferSize ) ;

VOID 
NDIS_API
OnStatusComplete( IN NDIS_HANDLE ProtocolBindingContext ) ;

VOID 
OnBindAdapter(
			  OUT PNDIS_STATUS Status,
			  IN NDIS_HANDLE  BindContext,
			  IN PNDIS_STRING  DeviceName,
			  IN PVOID  SystemSpecific1,
			  IN PVOID  SystemSpecific2
			  );

VOID
OnUnbindAdapter(
				OUT PNDIS_STATUS  Status,
				IN NDIS_HANDLE  ProtocolBindingContext,
				IN NDIS_HANDLE  UnbindContext
				);


#ifdef ISWIN9X
void 
WaitUlong(ULONG* pEvent);
#endif

#define OI_BUFFER_POOL  0x1
#define OI_PACKET_POOL  0x2
#define OI_ADAPTER_NAME 0x4
#define OI_INITIALIZED	0x8
#define OI_HAS_MAC		0x10
#define OI_ADAPTER_OPENED 0x20

class CKl_OI_Request : public CKl_Object
{
public:
	CKl_OI_Request(NDIS_HANDLE Pbc) : m_Pbc(Pbc){ };
	virtual ~CKl_OI_Request() {};

	QUEUE_ENTRY;
	
	NDIS_REQUEST    m_Request;
    NDIS_HANDLE     m_Pbc;
    NDIS_STATUS     m_FunalStatus;    
	CKl_KlopEvent	m_Event;
};

class CKl_SendPkt : public CKl_Object
{
public:	
	PNDIS_PACKET	m_Packet;
	NDIS_STATUS		m_Status;

	CKl_SendPkt(PNDIS_PACKET pkt) : m_Packet(pkt)
	{
		*(PVOID*)m_Packet->ProtocolReserved = this;	
	}
};

class CKl_OpenInstance : public CKl_Object 
{
public:
	CKl_OpenInstance( PVOID AdapterName, ULONG AdapterNameSize );
	virtual ~CKl_OpenInstance();	
    
    QUEUE_ENTRY;

	CKl_List<CKl_OI_Request>	m_RequestList;

    NDIS_HANDLE			AdapterHandle;  /* returned from ndisOpenAdapter */
    
	NDIS_HANDLE         mPacketPoolH;    
    NDIS_HANDLE			mBufferPoolH;

	NDIS_STATUS         Status; /* for async status */
	UINT                Medium;
	ULONG				PhysicalMedium;

    NDIS_REQUEST        Request;

	NDIS_STATUS         SendStatus; /* for async status */	
    
	wchar_t*            pAdapterName;
	ULONG				NameSize;

    ULONG               Flags;

    unsigned char       CurrentMacAddr[6];
    unsigned long       CurrentIP;
	unsigned long		MaxFrameSize;
	unsigned long		MacOptions;

	unsigned long		AdapterState;		// NDIS_PACKET_TYPE_PROMISCUOUS
    
    LIST_ENTRY          BufferList;
    CKl_Lock*           BufferListLock;

	CKl_KlopEvent		AdapterEvent;
};


#pragma pack( pop )


extern UCHAR		isWin98;
extern NDIS_HANDLE	WrapperHandle;
extern NDIS_HANDLE  ProtocolHandle;
extern bool			StopPacketProcessing;

extern CKl_List<CKl_OpenInstance>* OI_List;
extern CKl_List<CKl_OpenInstance>* OI_BindList;

extern REGISTER_PROTOCOL    rNdisRegisterProtocol;
extern OPEN_ADAPTER	     rNdisOpenAdapter;
extern DEREGISTER_PROTOCOL	 rNdisDeregisterProtocol;
extern CLOSE_ADAPTER	     rNdisCloseAdapter;

extern USHORT	dwLastID;

#endif
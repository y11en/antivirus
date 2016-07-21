typedef struct TDI_DISPATCH_TABLE {
	TDI_STATUS		(*TdiOpenAddressEntry)(
											PTDI_REQUEST, 
											PTRANSPORT_ADDRESS, 
											ULONG, 
											PVOID
											);
	
	TDI_STATUS		(*TdiCloseAddressEntry)(
											PTDI_REQUEST
											);

	TDI_STATUS		(*TdiOpenConnectionEntry)(
											PTDI_REQUEST, 
											PVOID
											);

	TDI_STATUS		(*TdiCloseConnectionEntry)(
											PTDI_REQUEST
											);

	TDI_STATUS		(*TdiAssociateAddressEntry)(
											PTDI_REQUEST, HANDLE
											);

	TDI_STATUS		(*TdiDisAssociateAddressEntry)(
											PTDI_REQUEST
											);

	TDI_STATUS		(*TdiConnectEntry)(
											PTDI_REQUEST, 
											PVOID,
											PTDI_CONNECTION_INFORMATION,
											PTDI_CONNECTION_INFORMATION
											);

	TDI_STATUS		(*TdiDisconnectEntry)(
											PTDI_REQUEST, 
											PVOID, 
											USHORT,
											PTDI_CONNECTION_INFORMATION, 
											PTDI_CONNECTION_INFORMATION
											);

	TDI_STATUS		(*TdiListenEntry)(
											PTDI_REQUEST, 
											USHORT, 
											PTDI_CONNECTION_INFORMATION, 
											PTDI_CONNECTION_INFORMATION
											);

	TDI_STATUS		(*TdiAcceptEntry)(
											PTDI_REQUEST, 
											PTDI_CONNECTION_INFORMATION,
											PTDI_CONNECTION_INFORMATION
											);
	
	TDI_STATUS		(*TdiReceiveEntry)(	
											PTDI_REQUEST, 
											USHORT *, 
											ULONG *, 
											PNDIS_BUFFER
											);

	TDI_STATUS		(*TdiSendEntry)(
											PTDI_REQUEST, 
											USHORT, 
											ULONG, 
											PNDIS_BUFFER
											);

	TDI_STATUS		(*TdiSendDatagramEntry)(
											PTDI_REQUEST, 
											PTDI_CONNECTION_INFORMATION,
											ULONG, 
											ULONG *, 
											PNDIS_BUFFER
											);

	TDI_STATUS		(*TdiReceiveDatagramEntry)(
											PTDI_REQUEST, 
											PTDI_CONNECTION_INFORMATION,
											PTDI_CONNECTION_INFORMATION, 
											ULONG, 
											ULONG *, 
											PNDIS_BUFFER
											);

	TDI_STATUS		(*TdiSetEventEntry)(
											PVOID, 
											ULONG, 
											PVOID, 
											PVOID
											);

	TDI_STATUS		(*TdiQueryInformationEntry)(
											PTDI_REQUEST, 
											ULONG,
											PNDIS_BUFFER, 
											ULONG*, 
											ULONG
											);

	TDI_STATUS		(*TdiSetInformationEntry)(
											PTDI_REQUEST, 
											ULONG, 
											PNDIS_BUFFER, 
											ULONG, 
											ULONG
											);
	TDI_STATUS		(*TdiActionEntry)(
											PTDI_REQUEST, 
											ULONG, 
											PNDIS_BUFFER, 
											ULONG
											);

//* Next two entries still under construction - reserved for future use.....
	TDI_STATUS		(*TdiQueryInformationExEntry)(PTDI_REQUEST, struct TDIObjectID *, PNDIS_BUFFER, ULONG *, void *);
	TDI_STATUS		(*TdiSetInformationExEntry)(PTDI_REQUEST, 	struct TDIObjectID *, void *, ULONG);

} TDI_DISPATCH_TABLE, *PTDI_DISPATCH_TABLE;

typedef void    (*CTEReqCmpltRtn)(void *Context, TDI_STATUS FinalStatus, ULONG ByteCount); 

typedef struct ConnectEventInfo {
	CTEReqCmpltRtn  cei_rtn;
	PVOID  cei_context;
	PTDI_CONNECTION_INFORMATION  cei_acceptinfo;
	PTDI_CONNECTION_INFORMATION  cei_conninfo;
} ConnectEventInfo;

typedef struct EventRcvBuffer {
	PNDIS_BUFFER  erb_buffer;
	ULONG  erb_size;
	CTEReqCmpltRtn  erb_rtn;
	PVOID  erb_context;
	USHORT  *erb_flags;
} EventRcvBuffer;

typedef
TDI_STATUS
(*PTDI_IND_CONNECT)(
				PVOID  EventContext, 
				ULONG  AddressLength,  
				PTRANSPORT_ADDRESS  Address, 
				ULONG  UserDataLength, 
				PVOID  UserData, 
				ULONG  OptionsLength, 
				PVOID  Options,  
				PVOID  *AcceptingID, 
				ConnectEventInfo  *EventInfo
				);

typedef
TDI_STATUS
(*PTDI_IND_DISCONNECT)(
				PVOID  EventContext, 
				PVOID  ConnectionContext, 
				ULONG  DisconnectDataLength, 
				PVOID  DisconnectData, 
				ULONG  OptionsLength, 
				PVOID  Options, 
				ULONG  Flags
				);

typedef
TDI_STATUS
(*PTDI_IND_RECEIVE)(
				PVOID  EventContext, 
				PVOID  ConnectionContext,  
				ULONG  Flags, 
				ULONG  Indicated, 
				ULONG  Available, 
				ULONG  *Taken, 
				UCHAR  *Data, 
				EventRcvBuffer  *Buffer
				);

typedef
TDI_STATUS
(*PTDI_IND_RECEIVE_DATAGRAM)(
				PVOID  EventContext, 
				ULONG  AddressLength,  
				PTRANSPORT_ADDRESS  Address, 
				ULONG  OptionsLength, 
				PVOID  Options,  
				ULONG  Flags, 
				ULONG  Indicated, 
				ULONG  Available,  
				ULONG  *Taken, 
				UCHAR  *Data, 
				EventRcvBuffer  **Buffer);
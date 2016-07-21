#define htons(_x_) (USHORT)(((_x_)<<8)|((_x_)>>8))
#define htonl(_x_) (ULONG)(_x_<<24) | (((_x_)<<8)&0x00FF0000)|(((_x_)>>8)&0x0000FF00)|(((_x_)>>24)&0x000000FF)

#define ETH_HEADER_LENGTH	0xE

#define ETH_P_LOOP	0x0060		/* Ethernet Loopback packet	*/
#define ETH_P_ECHO	0x0200		/* Ethernet Echo packet		*/
#define ETH_P_PUP	0x0400		/* Xerox PUP packet		*/
#define ETH_P_IP	0x0800		/* Internet Protocol packet	*/
#define ETH_P_X25	0x0805		/* CCITT X.25			*/
#define ETH_P_ARP	0x0806		/* Address Resolution packet	*/
#define	ETH_P_BPQ	0x08FF		/* G8BPQ AX.25 Ethernet Packet	[ NOT AN OFFICIALLY REGISTERED ID ] */
#define ETH_P_DEC       0x6000          /* DEC Assigned proto           */
#define ETH_P_DNA_DL    0x6001          /* DEC DNA Dump/Load            */
#define ETH_P_DNA_RC    0x6002          /* DEC DNA Remote Console       */
#define ETH_P_DNA_RT    0x6003          /* DEC DNA Routing              */
#define ETH_P_LAT       0x6004          /* DEC LAT                      */
#define ETH_P_DIAG      0x6005          /* DEC Diagnostics              */
#define ETH_P_CUST      0x6006          /* DEC Customer use             */
#define ETH_P_SCA       0x6007          /* DEC Systems Comms Arch       */
#define ETH_P_RARP      0x8035		/* Reverse Addr Res packet	*/
#define ETH_P_ATALK	0x809B		/* Appletalk DDP		*/
#define ETH_P_AARP	0x80F3		/* Appletalk AARP		*/
#define ETH_P_IPX	0x8137		/* IPX over DIX			*/
#define ETH_P_IPV6	0x86DD		/* IPv6 over bluebook		*/
#define ETH_P_PPP_DISC	0x8863		/* PPPoE discovery messages     */
#define ETH_P_PPP_SES	0x8864		/* PPPoE session messages	*/
#define ETH_P_ATMMPOA	0x884c		/* MultiProtocol Over ATM	*/
#define ETH_P_ATMFATE	0x8884		/* Frame-based ATM Transport*/

#pragma pack(1)
typedef struct _ETH_HEADER {
	UCHAR	dstMac[6];	/* destination eth addr	*/
	UCHAR	srcMac[6];	/* source ether addr	*/
	USHORT	Type;		/* packet type ID field	*/
} ETH_HEADER, *PETH_HEADER;

typedef struct	_IP_HEADER {	
	UCHAR	Ihl		: 4;
	UCHAR	Version : 4;
	
	UCHAR	TypeOfService;
	USHORT	TotalLength;
	USHORT  Identification;
	USHORT	Flg_FragOffs;
	UCHAR	TimeToLive;
	UCHAR	Protocol;
	USHORT	HeaderChecksum;
	ULONG	srcIP;
	ULONG	dstIP;	
} IP_HEADER, *PIP_HEADER;

typedef struct _TCP_HEADER {
	USHORT	srcPort;
	USHORT	dstPort;
	ULONG	SeqNumber;
	ULONG	AckNumber;

	USHORT	res1:4;
	USHORT	DataOffset:4;
	USHORT	fin:1;
	USHORT	syn:1;
	USHORT	rst:1;
	USHORT	psh:1;
	USHORT	ack:1;
	USHORT	urg:1;
	USHORT	ece:1;
	USHORT	cwr:1;
	
	USHORT	Window;
	USHORT	CheckSum;
	USHORT	UrgPointer;
} TCP_HEADER, *PTCP_HEADER;

typedef struct _UDP_HEADER {
	USHORT	srcPort;
	USHORT	dstPort;
	USHORT	Length;
	USHORT	CheckSum;
} UDP_HEADER, *PUDP_HEADER;


#define SESSION_REQUEST 0x81
#define SESSION_MESSAGE 0x00

#define SMB_NEGOTIATE_PROTOCOL 0x72
#define SMB_SESSION_SETUP_ANDX 0x73
#define SMB_TREE_CONNECT_ANDX 0x75
#define SMB_COM_TRANSACTION 0x25

typedef struct _SMB_HEADER
{
	UCHAR	server_component[4];
	UCHAR	command;
	UCHAR	error_class;
	UCHAR	reserved1;
	USHORT	error_code;
	UCHAR	flags;
	USHORT	flags2;
	UCHAR	reserved2[12];
	USHORT	tree_id;
	USHORT	proc_id;
	USHORT	user_id;
	USHORT	mpex_id;
} SMB_HEADER, *PSMB_HEADER;

typedef struct _TRANSACTION_REQUEST
{
	/* wct: word count */
	UCHAR	wct;
	USHORT	total_param_cnt;
	USHORT	total_data_cnt;
	USHORT	max_param_cnt;
	USHORT	max_data_cnt;
	UCHAR	max_setup_cnt;
	UCHAR	reserved1;
	USHORT	flags;
	ULONG	timeout;
	USHORT	reserved2;
	USHORT	param_cnt;
	USHORT	param_offset;
	USHORT	data_cnt;
	USHORT	data_offset;
	UCHAR	setup_count;
	UCHAR	reserved3;
	/* bcc: byte count */
	USHORT bcc;
} TRANSACTION_REQUEST, *PTRANSACTION_REQUEST;

typedef struct _SMB_PARAMETERS
{
	USHORT	function_code;
	UCHAR	param_descriptor[6];
	UCHAR	return_descriptor[7];
	USHORT	detail_level;
	USHORT	recv_buffer_len;
} SMB_PARAMETERS, *PSMB_PARAMETERS;


typedef struct NBT_PACKET 
{
	UCHAR	type;
	UCHAR	flags;
	USHORT	length;
	UCHAR	called[34];
	UCHAR	calling[34];
} NBT_PACKET, *PNBT_PACKET;

#define ICMP_ECHOREPLY		0	/* Echo Reply			*/
#define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
#define ICMP_SOURCE_QUENCH	4	/* Source Quench		*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO		8	/* Echo Request			*/
#define ICMP_TIME_EXCEEDED	11	/* Time Exceeded		*/
#define ICMP_PARAMETERPROB	12	/* Parameter Problem		*/
#define ICMP_TIMESTAMP		13	/* Timestamp Request		*/
#define ICMP_TIMESTAMPREPLY	14	/* Timestamp Reply		*/
#define ICMP_INFO_REQUEST	15	/* Information Request		*/
#define ICMP_INFO_REPLY		16	/* Information Reply		*/
#define ICMP_ADDRESS		17	/* Address Mask Request		*/
#define ICMP_ADDRESSREPLY	18	/* Address Mask Reply		*/

typedef struct _ICMP_HEADER {
	UCHAR	Type;
	UCHAR	Code;
	USHORT	CheckSum;
	union 
	{
		struct 
		{
			USHORT	Id;
			USHORT	Sequence;
		} echo;
		struct
		{
			ULONG	Address;
			ULONG	enh_hdr;
		} redirect;


	}	Params;	
} ICMP_HEADER, *PICMP_HEADER;
#pragma pack()

#ifndef ISWIN9X
#pragma pack(1)
typedef struct _NOTIFY_ATTACK_CONTEXT {
	LIST_ENTRY			Linkage;
	ULONG				AttackID;	
	ULONG				AttackerIp;
	NDIS_WORK_ITEM		WorkQueueItem;
}NOTIFY_ATTACK_CONTEXT, *PNOTIFY_ATTACK_CONTEXT;
#pragma pack()
#endif

#define ONE_SECOND_TIME				(KL_TIME)0x989680
#define HALF_SECOND_TIME			(KL_TIME)0x4C4B40

#ifndef isWIN9x
#define MAX_IP_STATISTICS			1000
#else
#define MAX_IP_STATISTICS			500
#endif

#define DEFAULT_MAX_OPENED_TCP_PORTS		15
#define DEFAULT_MAX_OPENED_UDP_PORTS		30

#define DEFAULT_BAN_TIME					60 * 60 // seconds
#define DEFAULT_REFRESH_TIME				60 * 2

#define DEFAULT_TCP_REFRESH_TIME			5
#define DEFAULT_UDP_REFRESH_TIME			5

#define DEFAULT_SYN_FLOOD_COUNT				300
#define DEFAULT_SYN_FLOOD_TIME				5 // seconds

#define DEFAULT_UDP_FLOOD_COUNT				1000
#define DEFAULT_UDP_FLOOD_TIME				2

#define DEFAULT_ICMP_FLOOD_COUNT			300
#define DEFAULT_ICMP_FLOOD_TIME				2

#define DEFAULT_IDS_ENABLE					1
#define DEFAULT_ENABLE						1
#define DEFAULT_DISABLE						0

#define DEFAULT_FLOOD_LIST_COUNT			1000

#define DEFAULT_LAND_NOTIFY_TIME			HALF_SECOND_TIME		
#define	DEFAULT_TRUSTED_LIST_REFRESH_TIME	ONE_SECOND_TIME * 120

#define IDS_STAT_ELEMENT	'tats'
#define IDS_PORT_ELEMENT    'etsp'
#define IDS_FLOOD_ELEMENT   'lelf'
#define IDS_BAN_REASON      'rnab'
#define IDS_NOTIFY_CONTEXT	'xtca'

#pragma pack(1)
typedef struct _STAT_PORT_ELEMENT	{	// статистика по отдельному порту 
	LIST_ENTRY	Linkage;
	USHORT		PortValue;
	UCHAR		Protocol;	
	KL_TIME		TimeStamp;
} STAT_PORT_ELEMENT, *PSTAT_PORT_ELEMENT;

typedef struct _STAT_FLOOD_ELEMENT {
	LIST_ENTRY	Linkage;	
	KL_TIME		TimeStamp;
} STAT_FLOOD_ELEMENT, *PSTAT_FLOOD_ELEMENT;

typedef struct _BAN_REASON {
	LIST_ENTRY	Linkage;
	ULONG		Reason;
	KL_TIME		StartTime;
	KL_TIME		StopTime;

}BAN_REASON, *PBAN_REASON;


typedef struct _STAT_ELEMENT {
	LIST_ENTRY		Linkage;			// Связь всех элементов такого типа
	ULONG			IpAddr;				// адрес внешней машины под подозрением
	ULONG			isRegistered;		// можно ли верить статистике ?
	
//	LIST_ENTRY		FloodList;			// Здесь храним STAT_FLOOD_ELEMENTs ...
//	ULONG			FloodListCount;
	
	LIST_ENTRY		TcpPortList;		// Список портов, на которые удаленная машина ломится	
	LIST_ENTRY		TcpPortListFree;	
	ULONG			TcpPortCount;		// Количество портов, выделенное для рассмотрения
	LIST_ENTRY		SynList;	
	
	LIST_ENTRY		UdpPortList;		// Список портов, на которые удалнная машина ломится		
	LIST_ENTRY		UdpPortListFree;
	ULONG			UdpPortCount;		// Количество портов, выделенное для рассмотрения
	LIST_ENTRY		UdpList;	
	
	LIST_ENTRY		IcmpList;	
	
	KL_TIME			StartTime;			// Время, когда элемент был создан
	KL_TIME			TimeStamp;			// Время, когда пришел Последний запрос.		
	
	LIST_ENTRY		BanReasonList;
} STAT_ELEMENT, *PSTAT_ELEMENT;
#pragma pack()

#define TRUST_ELEMENT_ALONE         0x0
#define TRUST_ELEMENT_INSERTED      0x1
#define TRUST_ELEMENT_MARKED_DELETE 0x2

typedef struct _TRUST_ELEMENT	{
	LIST_ENTRY		Linkage;
	ULONG			IpAddr;		// remote ip
	USHORT			PortValue;	// local port
    USHORT          Status;
	KL_TIME			TimeStamp;
    
} TRUST_ELEMENT, *PTRUST_ELEMENT;

extern ATTACK_OPTIONS	AttackOptions;

extern KL_EVENT			IDS_ResetEvent;
extern SPIN_LOCK		IDS_Lock;
extern BOOLEAN			IDS_ResetFlag;

extern LIST_ENTRY		IdsTrustList;
extern SPIN_LOCK		IdsTrustListLock;

extern BOOLEAN			bBlockALL;
extern KL_TIME			BlockALL_TimeStamp;
extern KL_TIME			BlockALL_StopTime;

VERDICT		QuickFilter(PVOID Packet);
VOID		DeregisterAllElements();

VERDICT	
FilterEthernet(PETH_HEADER	EthernetHeader);

BOOLEAN	
isMulticast(PETH_HEADER	EthernetHeader);

PTRUST_ELEMENT	
CheckTrusted(ULONG	IpAddr, USHORT	PortValue);

VOID	MakeTrusted(PETH_HEADER	EthernetHeader);

VOID	
RefreshTrustedList();

VOID InitializeHelkern();

typedef struct _STRUCT_STORE {			  // структура для хранения данных.
	ULONG					Tag;		  

	PDEVICE_OBJECT			MyDeviceObject; // Main DeviceObject
	PDRIVER_OBJECT			MyDriverObject; // current DriverObject 

	LIST_ENTRY				ProtocolList;
	LIST_ENTRY				AdapterList;
	
	LIST_ENTRY				FreeAdapterList;	// сюда будем складывать адаптеры после закрытия.
	NDIS_SPIN_LOCK			FreeAdapterListLock;
	
	PDEVICE_OBJECT			pKLIFObject;     //  Мы ведь работаем с KLIF.sys !!! запомним его ...

	PVOID					outPlainPacket;

	KEVENT					InPacketsEvent;
	KEVENT					outPacketsEvent;

	LIST_ENTRY				WorkItemList;
	NDIS_SPIN_LOCK			WorkItemListLock;
	ULONG					WorkItemListCount;

	NDIS_HANDLE				NdisPacketListHandle;
	LIST_ENTRY				NdisPacketList;
	NDIS_SPIN_LOCK			NdisPacketListLock;
	
	NDIS_HANDLE				NdisBufferListHandle;
	LIST_ENTRY				NdisBufferList;
	NDIS_SPIN_LOCK			NdisBufferListLock;
	
	struct 
	{
		LIST_ENTRY		ProtectList;
		ULONG			ProtectListCount;		// сколько элементов в листе на данный момент
		NDIS_SPIN_LOCK	ProtectListLock;		
	} AttacksStorage;
	struct 
	{
		LIST_ENTRY		NotifyAttackList;
		SPIN_LOCK		NotifyAttackListLock;

		LIST_ENTRY		ProtectList;
		ULONG			ProtectListCount;		// сколько элементов в листе на данный момент
		NDIS_SPIN_LOCK	ProtectListLock;		

        LIST_ENTRY		FloodProtectList;
        ULONG			FloodProtectListCount;	// сколько элементов в листе на данный момент
        SPIN_LOCK		FloodProtectListLock;		
		
		LIST_ENTRY		BlockIpList;
		NDIS_SPIN_LOCK	BlockIpListLock;

		KL_TIME			LandAttackNotificationTime;
		KL_TIME			DDOSAttackNotificationTime;
		KL_TIME			PingOfDeathNotificationTime;
		KL_TIME			SynFloodNotificationTime;
		KL_TIME			UdpFloodNotificationTime;
		KL_TIME			TcpScanNotificationTime;
		KL_TIME			UdpScanNotificationTime;
		KL_TIME			IcmpFloodNotificationTime;
		KL_TIME			SmbDieNotificationTime;
		KL_TIME			HelkernNotificationTime;
        KL_TIME			LoveSanNotificationTime;
	} Attacks;	
} STRUCT_STORE, *PSTRUCT_STORE;

typedef struct _ATTACK_OPTIONS6
{
	ULONG	Version; // = 6
	struct
	{
		ULONG	Land;
		ULONG	PingOfDeath;
		ULONG	SynFlood;
		ULONG	TcpScan;
		ULONG	UdpScan;
		ULONG	UdpFlood;
		ULONG	IcmpFlood;
		ULONG	Helkern;
		ULONG	SmbDie;
		ULONG	LoveSan;

		ULONG	StealthModeOn;		
		ULONG	IDS_Enable;		
	} AttackSwitch;
	
	ULONG	SynFloodCount;
	ULONG	SynFloodTime;
	ULONG	 UdpFloodCount;
	ULONG	 UdpFloodTime;
	ULONG	IcmpFloodCount;
	ULONG	IcmpFloodTime;

	ULONG	FloodListCount;
	
	ULONG	BanTime;
	ULONG	MaxOpenedTcpPorts;
	ULONG	MaxOpenedUdpPorts;	
	ULONG	TcpRefreshTime;
	ULONG	UdpRefreshTime;	
} ATTACK_OPTIONS6, *PATTACK_OPTIONS6;

typedef struct _ATTACK_OPTIONS5
{
	ULONG	Version; // = 5
	struct
	{
		ULONG	Land;
		ULONG	PingOfDeath;
		ULONG	SynFlood;
		ULONG	TcpScan;
		ULONG	UdpScan;
		ULONG	UdpFlood;
		ULONG	IcmpFlood;
		ULONG	Helkern;
		ULONG	SmbDie;

		ULONG	StealthModeOn;		
		ULONG	IDS_Enable;		
	} AttackSwitch;
	
	ULONG	SynFloodCount;
	ULONG	SynFloodTime;
	ULONG	 UdpFloodCount;
	ULONG	 UdpFloodTime;
	ULONG	IcmpFloodCount;
	ULONG	IcmpFloodTime;

	ULONG	FloodListCount;
	
	ULONG	BanTime;
	ULONG	MaxOpenedTcpPorts;
	ULONG	MaxOpenedUdpPorts;	
	ULONG	TcpRefreshTime;
	ULONG	UdpRefreshTime;	
} ATTACK_OPTIONS5, *PATTACK_OPTIONS5;

typedef struct _ATTACK_OPTIONS4
{
	ULONG	Version; // = 4
	struct
	{
		ULONG	Land;
		ULONG	PingOfDeath;
		ULONG	SynFlood;
		ULONG	TcpScan;
		ULONG	UdpScan;
		ULONG	UdpFlood;
		ULONG	IcmpFlood;

		ULONG	StealthModeOn;		
		ULONG	IDS_Enable;		
	} AttackSwitch;
	
	ULONG	SynFloodCount;
	ULONG	SynFloodTime;
	ULONG	 UdpFloodCount;
	ULONG	 UdpFloodTime;
	ULONG	IcmpFloodCount;
	ULONG	IcmpFloodTime;

	ULONG	FloodListCount;
	
	ULONG	BanTime;
	ULONG	MaxOpenedTcpPorts;
	ULONG	MaxOpenedUdpPorts;	
	ULONG	TcpRefreshTime;
	ULONG	UdpRefreshTime;	
} ATTACK_OPTIONS4, *PATTACK_OPTIONS4;

typedef union _UNI_ATTACK_OPTIONS
{
	ATTACK_OPTIONS4 ao4;
	ATTACK_OPTIONS5 ao5;
	ATTACK_OPTIONS6 ao6;
} UNI_ATTACK_OPTIONS, *PUNI_ATTACK_OPTIONS;
#pragma pack()

extern STRUCT_STORE			GlobalStore;

ULONG	InitializeGlobalStore( VOID );
#ifndef _FILTER_EVENT_
#define _FILTER_EVENT_

#define TRUE_PID		0x00010000

#ifdef _DEBUG
//Debug level
typedef enum tagDBL_LEVELS{
	DL_FATAL_ERROR,
	DL_ERROR,		//_T("ERROR")
	DL_SILENCE,		//_T("SILENCE")
	DL_PARSE_MISS,	//_T("PARSE MISS")
	DL_BRIEF,		//_T("BRIEF")
	DL_INFO,		//_T("INFO")
	DL_FULL_INFO	//_T("FULL INFO")
}DBL_LEVELS;

//Debug mask
#define DM_UNWIND		(1L<<1)		//_T("Unwind info")
#define DM_ADD_PARAM	(1L<<2)		//_T("Added items")
#define DM_FLT_EVENT	(1L<<3)		//_T("FILTER_EVENT info")
#define DM_ETH			(1L<<5)	//_T("Ethernet Info")
#define DM_IPv4			(1L<<6)	//_T("IPv4 Info")
#define DM_TCP			(1L<<7)	//_T("TCP Info")
#define DM_UDP			(1L<<8)	//_T("UDP Info")
#define DM_ICMP			(1L<<9)	//_T("ICMP Info")
#define DM_ARP			(1L<<10)	//_T("ARP Info")

#define DM_ANY			0xFFFFFFFF

//extern ULONG		DebugTrueMask;  
//extern ULONG		DebugTrueLevel;
#endif //_DEBUG

//NIH (struct NetInfoHeader) m_dwMediaType 
#define MT_ANY				(ULONG)0x00000000 //Any medium (default)
#define MT_Eth				(ULONG)0x00000001 //Ethernet II or 802.3
#define MT_802_5			(ULONG)0x00000008 //Token Ring
#define MT_FDDI				(ULONG)0x00000010 //FDDI
#define MT_PPP				(ULONG)0x00000020 //Point to Point Protocol



/*NIH (struct NetInfoHeader) flags (m_dwFlag)*/
/* m_dwMediaType format*/
#define FL_MT_NULL			(USHORT)0x00000001 
#define FL_MT_SINGLE		(USHORT)0x00000002 
#define FL_MT_MULTI			(USHORT)0x00000004 

/* LOGIC TYPE */
#define FL_BINARY			(USHORT)0x00000008 
#define FL_TERNARY			(USHORT)0x00000010 

/* WITH RECOVERY OR NOT*/
#define FL_STRAIGHTFORWARD	(USHORT)0x00000020 
#define FL_RECOVERY			(USHORT)0x00000040 

/* WHERE FIND */
#define FL_FIND_FAR			(USHORT)0x00000080 
#define FL_FIND_LOCAL		(USHORT)0x00000100




//NIH (struct NetInfoHeader) m_dwDirection
#define PACKET_ANY_DIRECTION	(USHORT)0x00000000 //default
#define PACKET_OUT				(USHORT)0x00000001 //For outgoing packet
#define PACKET_IN				(USHORT)0x00000002 //For incoming packet
#define PACKET_THROUGH			(USHORT)0x00000004 //For pass-through packet


#ifndef _DECLARE_DISABLE

#ifdef __cplusplus
#pragma warning( disable: 4200 )       
#endif
#pragma pack( push, 1)
typedef struct tagNetInfoHeader{
	ULONG	m_dwMediaType; //MT_ANY, MT_802_3, MT_802_5, MT_FDDI...
	USHORT	m_wDirection; //PACKET_IN, PACKET_OUT, PACKET_THROUGH
	USHORT	m_wFlag;		//NIH_xxx flags
	__int64	m_qwTime;		//arriving time
	ULONG	m_dwAction;		//error code if failes of action type
	PDEVICE_OBJECT	pAVPGObject;
	ULONG	m_dwPacketSize; //whole packet chunk size in bytes( without this header)
	BYTE*	m_lpPacket; //beginning chunk of data
}NetInfoHeader, *LPNetInfoHeader;
#pragma pack( pop )

#ifdef __cplusplus
extern "C"{
#endif 
	VERDICT __stdcall FilterEvent(LPNetInfoHeader pNetInfoHeader);
#ifndef _CONSOLE
	NTSTATUS __stdcall
		KLDeviceIoControl(PDEVICE_OBJECT	Device,
		ULONG		IoControlCode,
		PVOID		InData,
		ULONG		InDataSize,
		PVOID		OutData,
		ULONG		OutDataSize,
		PULONG		Returned);
#endif _CONSOLE
	
#ifdef __cplusplus
}
#endif 

#endif //_DECLARE_DISABLE


#endif //_FILTER_EVENT_
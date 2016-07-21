extern ULONG StealthModeOn;

typedef struct _STEALTH_ELEMENT {
	LIST_ENTRY		Linkage;
	ULONG			IpAddr;			// Удаленный адрес
	USHORT			PortValue;		// локальный порт, к которому разрешено обращение
	KL_TIME			TimeStamp;		// Время, когда ушел последний пакет на этот IP
} STEALTH_ELEMENT, *PSTEALTH_ELEMENT;

/*
	Список IP-адресов, на которые ушел FTP-запрос
*/
typedef struct _FTP_STEALTH_ELEMENT {
	LIST_ENTRY		Linkage;
	ULONG			IpAddr;			// Удаленный адрес
	KL_TIME			TimeStamp;		// Время, когда ушел последний пакет на этот IP
} FTP_STEALTH_ELEMENT, *PFTP_STEALTH_ELEMENT;

NTSTATUS	InitializeStealthMode();

VERDICT	
StealthProcessingInPacket(PETH_HEADER pEthernetHeader);

VERDICT 
StealthProcessingOutPacket(PETH_HEADER pEthernetHeader);

NTSTATUS	
InitializeStealthMode();

PSTEALTH_ELEMENT 
isStealthElementExist(PLIST_ENTRY	ListHead, ULONG	IpAddr, USHORT	PortValue);

VERDICT 
isCheckElement(PLIST_ENTRY	ListHead, SPIN_LOCK * pLock, ULONG	IpAddr, USHORT PortValue);

PSTEALTH_ELEMENT    
AllocateStealthElement(ULONG    IpAddr, USHORT	PortValue);

VOID    
FreeStealthElement(PSTEALTH_ELEMENT Element);


VOID RegisterStealthElement(ULONG IpAddr, USHORT PortValue, PLIST_ENTRY pListHead, SPIN_LOCK * pLock, ULONG * pKillCounter);

VOID RegisterTcpStealthElement(ULONG IpAddr, USHORT PortValue);

VOID RegisterUdpStealthElement(ULONG IpAddr, USHORT PortValue);

VOID DeregisterStealthElement(PSTEALTH_ELEMENT Element);



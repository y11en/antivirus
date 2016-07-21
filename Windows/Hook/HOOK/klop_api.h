#ifndef _KLOP_API_H_
#define _KLOP_API_H_

#pragma warning( disable : 4200 )

#pragma pack( push, 1)

#define KLOP_FILE_DEVICE_SECTION    0x00008005

#define KLOP_IOCTL_INDEX            0x567

#define KLOP_API_VERSION	0x2

// Получить версию API драйвера.
// outBuffer = *DWORD
#define KLOP_GET_VERSION		CTL_CODE(KLOP_FILE_DEVICE_SECTION,		\
								KLOP_IOCTL_INDEX,						\
								METHOD_BUFFERED,						\
								FILE_ANY_ACCESS)

typedef struct _CP_INIT_INFO {
	ULONG	KeySize;
	char	KeyBuffer[0];
} CP_INIT_INFO, *PCP_INIT_INFO;
// Инициализация протокола лицензирования
// InBuffer = CP_INIT_INFO
#define KLOP_CP_INIT            CTL_CODE(KLOP_FILE_DEVICE_SECTION,		\
								KLOP_IOCTL_INDEX + 1,					\
								METHOD_BUFFERED,						\
								FILE_ANY_ACCESS)

typedef struct _CP_RANGE {
	ULONG	StartOffset;
	ULONG	BytesCount;
} CP_RANGE, *PCP_RANGE;

typedef struct _CP_RANGE_LIST {
	ULONG		Count;
	CP_RANGE	Range[0];
} CP_RANGE_LIST, *PCP_RANGE_LIST;

typedef struct _CP_ADAPTER_INFO {
    ULONG			Version;    // Версия AdapterInfo.
	ULONG			LocalIp;
	ULONG			AdapterNameSize;
	char			AdapterBuffer[0];
} CP_ADAPTER_INFO, *PCP_ADAPTER_INFO;

// Открывает адаптер для прослушки / отправки пакетов.
// InBuffer = CP_ADAPTER_INFO. InBufferSize = sizeof ( CP_ADAPTER_INFO ) + AdapterNameSize;
#define  KLOP_CP_ADD_ADAPTER    CTL_CODE(KLOP_FILE_DEVICE_SECTION,		\
								KLOP_IOCTL_INDEX + 2,					\
								METHOD_BUFFERED,						\
								FILE_ANY_ACCESS)

typedef struct _CP_IFACE_INFO {
	char	MacAddr[6];
	ULONG	IpAddr;
} CP_IFACE_INFO, *PCP_IFACE_INFO;

// pInBuffer = CP_IFACE_INFO[], InBufferSize = sizeof ( CP_IFACE_INFO ) * Count
#define  KLOP_CP_SET_BROADCAST  CTL_CODE(KLOP_FILE_DEVICE_SECTION,		\
								KLOP_IOCTL_INDEX + 3,					\
								METHOD_BUFFERED,						\
								FILE_ANY_ACCESS)

// Останавливает всяческий перехват.
#define  KLOP_CP_DONE			CTL_CODE(KLOP_FILE_DEVICE_SECTION,		\
								KLOP_IOCTL_INDEX + 4,					\
								METHOD_BUFFERED,						\
								FILE_ANY_ACCESS)

#define  KLOP_CP_SEND_PACKET    CTL_CODE(KLOP_FILE_DEVICE_SECTION,		\
								KLOP_IOCTL_INDEX + 5,					\
								METHOD_BUFFERED,						\
								FILE_ANY_ACCESS)

// отсылает в сеть широковещательный пакет о своем ключе.
// inBuffer = Key, InBufferSize = KeySize
#define  KLOP_CP_ACTIVATE       CTL_CODE(KLOP_FILE_DEVICE_SECTION,		\
								KLOP_IOCTL_INDEX + 6,					\
								METHOD_BUFFERED,						\
								FILE_ANY_ACCESS)

// проверяет количество ключей, отозвавшихся в сети.
#define  KLOP_CP_CHECK			CTL_CODE(KLOP_FILE_DEVICE_SECTION,		\
								KLOP_IOCTL_INDEX + 7,					\
								METHOD_BUFFERED,						\
								FILE_ANY_ACCESS)

typedef struct _KlopKeyItem {
    char    MacAddr[6];
} KlopKeyItem, *PKlopKeyItem;
// Получает список отозвавшихся маков.
#define  KLOP_CP_GETMACS		CTL_CODE(KLOP_FILE_DEVICE_SECTION,		\
								KLOP_IOCTL_INDEX + 8,					\
								METHOD_BUFFERED,						\
								FILE_ANY_ACCESS)


#pragma pack( pop )

#endif // _KLOP_API_H_
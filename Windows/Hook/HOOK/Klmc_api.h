#ifndef _KLMC_API_
#define _KLMC_API_

#define KLMC_BUILDNUM 1

#define KLMC_NAME	L"KLMC"
#define KLMC_NAME_9x "KLMC    "

#define KLMC_FILE_DEVICE_SECTION	0x00008005
#define KLMC_IOCTL_INDEX			0x739
#define	ID_MCMESSAGETYPE			0x90001 //Parametr ID

// OUT *ULONG.
#define KLMC_GET_VERSION		CTL_CODE(KLMC_FILE_DEVICE_SECTION,				\
								KLMC_IOCTL_INDEX,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define KLMC_FIND_KLIF			CTL_CODE(KLMC_FILE_DEVICE_SECTION,				\
								KLMC_IOCTL_INDEX+1,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// данные хранятся в прямом порядке
typedef struct
{
	unsigned short	m_sSMTPServerPort;
	unsigned short	m_sSMTPProxyPort;
	unsigned short	m_sPOP3ServerPort;
	unsigned short	m_sPOP3ProxyPort;	

	unsigned long	m_ulPID;		

	unsigned long	m_dwProxyPID;

	unsigned char m_bCheckSMTPTraffic;
	unsigned char	m_bCheckPOP3Traffic;
} ConnectionParams;

#define _DEFAULT_SMTP_SERVER_PORT	25		//Default value for SMTP-server port

#define _DEFAULT_POP3_SERVER_PORT	110		//Default value for POP3-server port

#define net_DEFAULT_SMTP_SERVER_PORT	htons(_DEFAULT_SMTP_SERVER_PORT)

#define net_DEFAULT_POP3_SERVER_PORT	htons(_DEFAULT_POP3_SERVER_PORT)


#define KLMC_START_FILTER		CTL_CODE(KLMC_FILE_DEVICE_SECTION,				\
								KLMC_IOCTL_INDEX+2,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define KLMC_STOP_FILTER		CTL_CODE(KLMC_FILE_DEVICE_SECTION,				\
								KLMC_IOCTL_INDEX+3,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define KLMC_SET_PARAMS			CTL_CODE(KLMC_FILE_DEVICE_SECTION,				\
								KLMC_IOCTL_INDEX+4,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define MJ_CONN_ACTION		0x07000001


#define KLMC_MODE_NORMAL        0x1  // дефолтное значение. обычная, бесхитростная обработка
#define KLMC_MODE_REDIRECT      0x2  // в этом режиме KAVSEND присылвает 4-м параметром замаскированный порт. Port ^ 1000

// inBuffer = ULONG
#define KLMC_SET_WORK_MODE		CTL_CODE(KLMC_FILE_DEVICE_SECTION,				\
								KLMC_IOCTL_INDEX+5,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// outBuffer = ULONG
#define KLMC_GET_WORK_MODE		CTL_CODE(KLMC_FILE_DEVICE_SECTION,				\
								KLMC_IOCTL_INDEX+6,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)




#endif  _KLMC_API_ // _KLMC_API_
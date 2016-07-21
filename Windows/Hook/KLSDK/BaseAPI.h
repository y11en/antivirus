#ifndef _BASE_API_H_
#define _BASE_API_H_

#pragma warning( disable : 4200 )

//  ---------------------------------------------------------
//  ---------    API дл€ любого базового драйвера -----------
//  ---------------------------------------------------------
#define BASE_FILE_DEVICE_SECTION    0x00008005

#define BASE_IOCTL_INDEX            0x841

// ѕолучить версию API драйвера.
// outBuffer = *DWORD
#define BASE_GET_VERSION		CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
								BASE_IOCTL_INDEX,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// Ќачать перехват
#define BASE_START_FILTER		CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
								BASE_IOCTL_INDEX+1,							    \
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// ѕрекратить перехват
#define BASE_STOP_FILTER		CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
								BASE_IOCTL_INDEX+2,							    \
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

typedef struct _REGISTER_BUFFER {
    void*		PluginInitRoutine;
    void*		plContext;
} REGISTER_BUFFER, *PREGISTER_BUFFER;
// «арегистрировать Callback дл€ плагина.
// inBuffer = PluginInitEntry (DWORD), Context(DWORD)
#define BASE_REGISTER_PLUGIN	CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
								BASE_IOCTL_INDEX+3,							    \
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// InBuffer = PluginID, полученный при инициализации
#define BASE_DEREGISTER_PLUGIN	CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
								BASE_IOCTL_INDEX+4,							    \
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#define PLUGIN_DESCRIPTION_SIZE         100

// Ёта структура не должна мен€тьс€ ! иначе не будет совместимости со старой компонентой.
typedef struct _PLUGIN_MESSAGE {
    char            PluginName[PLUGIN_DESCRIPTION_SIZE];     // Plugin Name to send message        
	unsigned long	plContext;		//   !!! раньше там был void* 
									//   сейчас ULONG дл€ совместимости с 64-бит
    unsigned long   SenderID;       // от кого
    unsigned long   MessageID;      // что за сообщение
    unsigned long   MessageSize;    // размер сообщени€ без заголовка
    char            Message[0];     // —ообщение
} PLUGIN_MESSAGE, *PPLUGIN_MESSAGE;

// данное сообщение необходимо доставить плагину
// inBuffer     = PPLUGIN_MESSAGE
// outBuffer    = PPLUGIN_MESSAGE
#define BASE_DELIVER_MESSAGE	CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
								BASE_IOCTL_INDEX+6,								\
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// ѕриостановить перехват перехват
#define BASE_PAUSE_FILTER		CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
								BASE_IOCTL_INDEX+7,							    \
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// ¬озобновить перехват перехват
#define BASE_RESUME_FILTER		CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
								BASE_IOCTL_INDEX+8,							    \
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

// ¬озобновить перехват перехват
#define BASE_GET_EXPORT_TABLE   CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
                                BASE_IOCTL_INDEX+9,							    \
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

typedef struct _BASE_PLUGIN_MESSAGE  {
    unsigned long           dwIoctl;
    PLUGIN_MESSAGE  message;
} BASE_PLUGIN_MESSAGE, *PBASE_PLUGIN_MESSAGE;
// прием плагинного запроса. аналог PLUG_IOCTL
// *InBuffer = BASE_PLUGIN_MESSAGE
#define BASE_RECEIVE_PLUGIN_REQUEST   CTL_CODE(BASE_FILE_DEVICE_SECTION,				\
								BASE_IOCTL_INDEX+10,							    \
								METHOD_BUFFERED,								\
								FILE_ANY_ACCESS)

#endif
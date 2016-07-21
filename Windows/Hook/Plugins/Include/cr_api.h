#ifndef _CR_API_H_
#define _CR_API_H_

#include "../../klsdk/kl_ipaddr.h"

#pragma warning (disable : 4200)

/*  
 *	    Connection redirector plugin API 
 *  
 *      Автор       : Рощин Е.Е.
 *      Updated     : 16/02/2005
 *
 *      
 *
 *      Модель перенаправления соединений подразумевает собой существование "прокси сервера" ( возможно нескольких ),
 *      куда направляются исходящие соединения с определенных портов.
 *
 *      Если прокси сервер "хочет" получать и обрабатывать потоки с определенных портов, то необходимо зарегистрироваться
 *      в драйвере редиректора с помощью посылки PLUGIN_MESSAGE с командой MSG_CR_INITIALIZE. В ответ прокси получает
 *      хэндл драйвера, и в будущем работает с этим хэндлом. Данный хэндл валиден только для данной прокси и не может быть
 *      использован для другой прокси, где бы она не находилась. При этом хэндл можно использовать из разных потоков.
 *
 *      Для добавления перехвата потока по определенному порту необходимо послать PLUGIN_MESSAGE с командой 
 *      MSG_CR_ADD_PORT. Если порт уже используется для перехвата, то вернется ошибка.
 *      Для исключения порта из перехвата необходимо послать PLUGIN_MESSAGE с командой MSG_CR_REMOVE_PORT
 *
 *      При завершении работы, прокси сервер должен послать PLUGIN_MESSAGE с командой MSG_CR_DONE, для того, чтоб
 *      прекратился перехват всех портов, которые использовал прокси сервер.
 * 
 *      Когда соединение перенаправилось на прокси сервер, драйвер сперва пошлет строку размером в 500 байт в виде :
 *      sprintf( HelloBuffer, "KAVSEND <%s> <%d> <%d> <%d>\r\n\0", Ip, htons ( rPort ), PID, htons ( rMaskedPort ) );
 *
 *      Перехват также прекратится в случае выгрузки плагина-редиректора или в случае 
 *      перевода плагина в режим PAUSE.
 */

#define REDIRECTOR_PLUGIN_NAME        "Connection redirector Plugin"

// ID сообщений, приходящих в плагин

#pragma pack(1)

typedef struct _CrInitInfo {
    unsigned short  m_ListenPort;   // прослушивающий порт    
    unsigned long   m_ProxyPID;     // PID прокси
} CrInitInfo;

typedef struct _CrInitInfo2 {
    unsigned short  m_ListenPort;   // прослушивающий порт    
    unsigned short  m_ServPort;     // вспомогательный порт.
    unsigned long   m_ProxyPID;     // PID прокси
} CrInitInfo2;

typedef struct _CrAskConnection {    
    unsigned long   m_Ver;          // тип сообщения
    unsigned long   m_Size;         // размер сообщения

    _ip_addr_t      m_rIp;
    unsigned short  m_rPort;
    unsigned long   m_PID;
    __int64         m_ID;           // ID соединения. заполняется драйвером.
} CrAskConnection ;

typedef struct _CrReplyConnection {
    KLSTATUS        m_Status;       // статус соединения с сервером.
    __int64         m_ID;           // ID соединения.
} CrReplyConnection;

typedef struct _CrInitInfoRet {
    unsigned long   m_Status;       // 0 в случае успеха.
    unsigned long   m_Handle;       // хэндл, возвращающийся в случае успеха. это DWORD даже на 64-битной машине.
} CrInitInfoRet;

typedef struct _CrPortInfo {
    unsigned long  m_Handle;        // хэндл прокси
    unsigned short m_PortValue;     // значение порта для перенаправления
} CrPortInfo;

typedef struct _CrPortListInfo {
    unsigned long   m_TotalPortCount;
    unsigned long   m_PortCount;
    unsigned short  m_Port[0];
} CrPortListInfo;

typedef struct _CrExcludeInfo {    
    unsigned long   m_Host;
    unsigned short  m_Port;    
    unsigned char   m_Hash[16];
} CrExcludeInfo;

typedef struct _CrExcludeInfo2 {    
    ip_addr_t       m_Host;
    unsigned short  m_Port;    
    unsigned char   m_Hash[16];
} CrExcludeInfo2;


#define CR_ANY_APP  L"*"

// InBuffer = PPLUGIN_MESSAGE(CrInitInfo); OutBuffer = CrInitInfoRet
#define MSG_CR_INITIALIZE       0x1

// *InBuffer = PPLUGIN_MESSAGE(m_Handle)
#define MSG_CR_DONE             0x2

// InBuffer = PPLUGIN_MESSAGE(CrPortInfo);
#define MSG_CR_ADD_PORT         0x3

// InBuffer = PPLUGIN_MESSAGE(CrPortInfo);
#define MSG_CR_REMOVE_PORT      0x4

// *InBuffer = PPLUGIN_MESSAGE(m_Handle); OutBuffer = CrPortListInfo
#define MSG_CR_GET_PORTS        0x5

// *InBuffer = PPLUGIN_MESSAGE(CrExcludeInfo); *OutBuffer = Handle (ULONG)
#define MSG_ADD_EXCLUDE         0x6

// *inBuffer = ExcludeHandle
#define MSG_REMOVE_EXCLUDE      0x7

typedef struct _CrAssociateStruct {
    ULONG   lIp1;
    ULONG   rIp1;
    USHORT  lPort1;
    USHORT  rPort1; 

    ULONG   lIp2;
    ULONG   rIp2;
    USHORT  lPort2;
    USHORT  rPort2; 
} CrAssociateStruct;

// *InBuffer = CrAssociateStruct
#define MSG_CR_ASSOCIATE_CONN      0x8

// *InBuffer = PPLUGIN_MESSAGE(CrExcludeInfo2); *OutBuffer = Handle (ULONG)
#define MSG_ADD_EXCLUDE2         0x9

typedef struct _CrAssociateStruct2 {
    ip_addr_t   lIp1;
    ip_addr_t   rIp1;
    USHORT  lPort1;
    USHORT  rPort1; 

    ip_addr_t   lIp2;
    ip_addr_t   rIp2;
    USHORT  lPort2;
    USHORT  rPort2; 
} CrAssociateStruct2;

// *InBuffer = CrAssociateStruct
#define MSG_CR_ASSOCIATE_CONN2      0xA

// InBuffer = PPLUGIN_MESSAGE(CrInitInfo2); OutBuffer = CrInitInfoRet
#define MSG_CR_INITIALIZE2          0xB

// Получает кол-во соединений, ушедших в проксю и не вернувшихся обратно
// OutBuffer = ULONG
#define MSG_CR_GET_CONNCOUNT        0xC

// OutBuffer = DWORD
#define MSG_CR_FLAGS               0xD

    #define CR_FLAG_INITIALIZED         0x1     // прошла инициализация
    #define CR_FLAG_CLOSING             0x2     // вызвался деструктор
    #define CR_FLAG_CLOSING_DONE        0x4     // конец деструктора
    #define CR_HAS_CONNECTIONS          0x8     // Есть висячие соединения
    #define CR_QUIT_SET                 0x10    // QuitEvent - сигнализирован

// Дает команду драйверу, что мы выходим. ( SetEvent ( QuitEvent ) )
#define MSG_PREPARE_EXIT               0xE

// получает имя евента, который показывает на наличие потоков,
// которые нужно редиректить
// InBuffer = PPLUGIN_MESSAGE( DWORD )
#define MSG_ASSIGN_EVENT              0xF

// получает свободный контекст с параметрами, куда необходимо законнектиться
// InBuffer = PPLUGIN_MESSAGE()
// OutBuffer = CrAskConnection
#define MSG_GET_CONTEXT               0x10

// после соединения выставляет контексту вердикт ( продолжить/блокировать/не редиректить )
// InBuffer = PPLUGIN_MESSAGE ( CrReplyConnection )
#define MSG_SET_CONTEXT_VERDICT       0x11

#pragma pack()

#endif
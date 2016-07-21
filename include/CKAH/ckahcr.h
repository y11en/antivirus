#if !defined(CKAHREDIRECTOR_H__INCLUDED_)
#define CKAHREDIRECTOR_H__INCLUDED_

// Connection redirector
namespace CKAHCR
{
    typedef LPVOID HPROXY;
    typedef LPVOID HPORTLIST;
    typedef LPVOID HEXCLUDE;

	// запускает Redirector
	CKAHUMMETHOD Start ();

	// останавливает Redirector
	CKAHUMMETHOD Stop ();

	// приостанавливает Redirector (при запуске плагин приостановлен)
	CKAHUMMETHOD Pause ();

	// возобновляет работу Redirector (при запуске плагин приостановлен)
	CKAHUMMETHOD Resume ();

    // устанавливает время, через которое Redirector должен быть отключен, если вызов не будет повторен
    CKAHUMMETHOD SetWatchdogTimeout (DWORD timeout_sec);

    // регистрирует прокси
    CKAHUMMETHOD RegisterProxy(USHORT port, DWORD pid, OUT HPROXY * phproxy);

    // регистрирует прокси
    CKAHUMMETHOD RegisterProxy2(USHORT port, USHORT serv_port, DWORD pid, OUT HPROXY * phproxy);

    // повторение структур CrAskConnection и СкReplyConnection, чтобы пользователи компоненты
    // не включали хедер драйвера напрямую.
#pragma pack( push, 1 )
    typedef struct _AskConnection {    
        unsigned long   m_Ver;          // тип сообщения
        unsigned long   m_Size;         // размер сообщения

        _ip_addr_t      m_rIp;
        unsigned short  m_rPort;
        unsigned long   m_PID;
    } AskConnection ;

    typedef struct _ReplyConnection {
        unsigned long   m_Status;       // статус соединения с сервером. ( KL_STATUS )
        __int64         m_ID;           // ID соединения. ( для ассоциирования установленного и редиректнутого )
    } ReplyConnection;

#pragma pack( pop )

    // разрегистрирует прокси
    CKAHUMMETHOD Proxy_Unregister(IN HPROXY hproxy);

    // добавляет порт в список портов перенаправления
    CKAHUMMETHOD Proxy_AddPort (IN HPROXY hproxy, USHORT port);

    // удаляет порт из списка порта перенаправления
    CKAHUMMETHOD Proxy_RemovePort (IN HPROXY hproxy, USHORT port);

    // получить текущий список портов перенаправления
    CKAHUMMETHOD Proxy_GetPortList ( IN HPROXY hproxy, OUT HPORTLIST * phlist );

    // удалить хэндл списка портов перенаправления
    CKAHUMMETHOD PortList_Delete ( IN HPORTLIST hlist );

    // получить размер списка портов перенаправления
    CKAHUMMETHOD PortList_GetSize ( IN HPORTLIST hlist, OUT int * psize );

    // получить элемент списка портов перенаправления
    CKAHUMMETHOD PortList_GetItem ( IN HPORTLIST hlist, IN int n, OUT USHORT * pport );
    
    // добавляет исключение проверки 
    CKAHUMMETHOD Proxy_AddExclude     ( IN CKAHUM::IP* host, IN USHORT port, IN wchar_t* ImagePath, IN ULONG ImagePathSize, OUT HEXCLUDE* phex);

    // удаляет исключение проверки
    CKAHUMMETHOD Proxy_RemoveExclude  ( IN HEXCLUDE hex );

    CKAHUMMETHOD Associate_Conn( 
                            IN ULONG lIp1, IN USHORT lPort1, IN ULONG rIp1, IN USHORT rPort1,
                            IN ULONG lIp2, IN USHORT lPort2, IN ULONG rIp2, IN USHORT rPort2 );

    CKAHUMMETHOD Drv_GetFlags( ULONG* flags );

    CKAHUMMETHOD Drv_SetQuitEvent();

};



#endif // !defined(CKAHREDIRECTOR_H__INCLUDED_)

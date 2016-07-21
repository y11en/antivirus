#if !defined(_CKAHPORTS_H__INCLUDED_)
#define _CKAHPORTS_H__INCLUDED_

// Firewall
namespace CKAHFW
{
    const int APPNAMELEN = 1100;
    const int CMDLINELEN = 1100;
    // открытые порты

    struct OpenPort
    {
        UCHAR       Proto;               // протокол
        CKAHUM::IP  LocalIP;             // локальный IP-адрес
        USHORT      LocalPort;           // локальный порт
        ULONG       PID;                 // PID процесса
        WCHAR       AppName[APPNAMELEN]; // название процесса (нуль-терминированная юникод-строка)
        WCHAR       CmdLine[CMDLINELEN]; // командная строка (нуль-терминированная юникод-строка)
        ULONG       ActiveTime;          // время открытости порта (в секундах)
        __int64     InBytes;             // количество входящих байт
        __int64     OutBytes;            // количество исходящих байт                           
    };

    typedef LPVOID HOPENPORTLIST;

    // получить список открытых портов
    CKAHUMMETHOD GetOpenPortList ( OUT HOPENPORTLIST * phlist );

    // получить размер списка открытых портов
    CKAHUMMETHOD OpenPortList_GetSize ( IN HOPENPORTLIST hlist, OUT int * psize );

    // получить элемент списка открытых портов
    CKAHUMMETHOD OpenPortList_GetItem ( IN HOPENPORTLIST hlist, IN int n, OUT OpenPort * pport );

    // удалить список открытых портов
    CKAHUMMETHOD OpenPortList_Delete ( IN HOPENPORTLIST hlist );

    
    // установленные соединения

    struct Connection
    {
        UCHAR       Proto;               // протокол
        CKAHUM::IP  LocalIP;             // локальный IP-адрес
        CKAHUM::IP  RemoteIP;            // удаленный IP-адрес
        USHORT      LocalPort;           // локальный порт
        USHORT      RemotePort;          // удаленный порт
        ULONG       PID;                 // PID процесса
        UCHAR       IsIncoming;          // входящее ли соединение (1 - входящее, 0 - исходящее)
        WCHAR       AppName[APPNAMELEN]; // название процесса (нуль-терминированная юникод-строка)
        WCHAR       CmdLine[CMDLINELEN]; // командная строка (нуль-терминированная юникод-строка)
        ULONG       ActiveTime;          // время открытости порта (в секундах)
        __int64     InBytes;             // количество входящих байт
        __int64     OutBytes;            // количество исходящих байт                           
    };

    typedef LPVOID HCONNLIST;

    // получить список установленных соединений
    CKAHUMMETHOD GetConnectionList ( OUT HCONNLIST * phlist );

    // получить размер списка установленных соединений
    CKAHUMMETHOD ConnectionList_GetSize ( IN HCONNLIST hlist, OUT int * psize );

    // получить элемент списка установленных соединений
    CKAHUMMETHOD ConnectionList_GetItem ( IN HCONNLIST hlist, IN int n, OUT Connection * pconn );

    // удалить список установленных соединений
    CKAHUMMETHOD ConnectionList_Delete ( IN HCONNLIST hlist );

    // разорвать установленное соединение
    CKAHUMMETHOD BreakConnection ( IN UCHAR Proto, IN CKAHUM::IP *pLocalIP, IN USHORT LocalPort, IN CKAHUM::IP *pRemoteIP, IN USHORT RemotePort );
    
    struct HostStat
    {
        __int64 inBytes;
        __int64 outBytes;
    };
    
    struct HostStatItem
    {
        CKAHUM::IP  HostIp;
        
        HostStat    TotalStat;
        HostStat    TcpStat;
        HostStat    UdpStat;
        HostStat    BroadcastStat; 
    };
    
    typedef LPVOID HHOSTSTATLIST;

    // получить список статистики хостов
    CKAHUMMETHOD GetHostStatistics      ( OUT HHOSTSTATLIST * hlist );

    // получить размер списка статистики хостов
    CKAHUMMETHOD HostStatistics_GetSize ( IN HHOSTSTATLIST hlist, OUT int * psize );

    // получить элемент списка статистики хостов
    CKAHUMMETHOD HostStatistics_GetItem ( IN HHOSTSTATLIST hlist, IN int n, OUT HostStatItem * pconn );

    // удалить список статистики хостов
    CKAHUMMETHOD HostStatistics_Delete  ( IN HHOSTSTATLIST hlist );

};

#endif // !defined(_CKAHPORTS_H__INCLUDED_)

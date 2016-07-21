unit IPHelperDef;

interface
uses Windows, SysUtils;

Const
  // ******* Из iptypes.h *******
  MAX_HOSTNAME_LEN               = 128;
  MAX_DOMAIN_NAME_LEN            = 128;
  MAX_INTERFACE_NAME_LEN         = 256;
  MAX_SCOPE_ID_LEN               = 256;
  MAX_ADAPTER_NAME_LENGTH        = 256;
  MAX_ADAPTER_DESCRIPTION_LENGTH = 128;
  MAX_ADAPTER_ADDRESS_LENGTH     = 8;
  DEFAULT_MINIMUM_ENTITIES       = 32;
  IF_TYPE_ETHERNET_CSMACD        = 6;
  MAXLEN_PHYSADDR                = 8;
  MAXLEN_IFDESCR                 = $100;

  // Node Type
  BROADCAST_NODETYPE    = 1;
  PEER_TO_PEER_NODETYPE = 2;
  MIXED_NODETYPE        = 4;
  HYBRID_NODETYPE       = 16;

   // Adapter Type
  IF_OTHER_ADAPTERTYPE       =     0;
  IF_ETHERNET_ADAPTERTYPE    =     1;
  IF_TOKEN_RING_ADAPTERTYPE  =     2;
  IF_FDDI_ADAPTERTYPE        =     3;
  IF_PPP_ADAPTERTYPE         =     4;
  IF_LOOPBACK_ADAPTERTYPE    =     5;
  IF_SLIP_ADAPTERTYPE        =     6;


Type
  TIPAddressString = Array[0..4*4-1] of Char;
  PIPAddrString = ^TIPAddrString;
  // Описатель адреса
  TIPAddrString = packed Record
    Next      : PIPAddrString;
    IPAddress : TIPAddressString;
    IPMask    : TIPAddressString;
    Context   : Integer;
  End;

  PFixedInfo = ^TFixedInfo;
  TFixedInfo = packed Record { FIXED_INFO }
    HostName         : Array[0..MAX_HOSTNAME_LEN+3] of Char;
    DomainName       : Array[0..MAX_DOMAIN_NAME_LEN+3] of Char;
    CurrentDNSServer : PIPAddrString;
    DNSServerList    : TIPAddrString;
    NodeType         : Integer;
    ScopeId          : Array[0..MAX_SCOPE_ID_LEN+3] of Char;
    EnableRouting    : Integer;
    EnableProxy      : Integer;
    EnableDNS        : Integer;
  End;

 // Информация об адаптере
 PIPAdapterInfo = ^TIPAdapterInfo;
 TIPAdapterInfo = packed Record { IP_ADAPTER_INFO }
    Next                : PIPAdapterInfo;
    ComboIndex          : Integer;
    AdapterName         : Array[0..MAX_ADAPTER_NAME_LENGTH+3] of Char;
    Description         : Array[0..MAX_ADAPTER_DESCRIPTION_LENGTH+3] of Char;
    AddressLength       : Integer;
    Address             : Array[1..MAX_ADAPTER_ADDRESS_LENGTH] of Byte;
    Index               : Integer;
    _Type               : Integer;
    DHCPEnabled         : Integer;
    CurrentIPAddress    : PIPAddrString;
    IPAddressList       : TIPAddrString;
    GatewayList         : TIPAddrString;
    DHCPServer          : TIPAddrString;
    HaveWINS            : LongBool;
    PrimaryWINSServer   : TIPAddrString;
    SecondaryWINSServer : TIPAddrString;
    LeaseObtained       : Integer;
    LeaseExpires        : Integer;
  End;

  // ************** Структуры для MIB ************************
  // Статистика TCP
  TMIB_TCPSTATS = packed record
    dwRtoAlgorithm : DWORD;    // time-out algorithm
    dwRtoMin       : DWORD;    // minimum time-out
    dwRtoMax       : DWORD;    // maximum time-out
    dwMaxConn      : DWORD;    // maximum connections
    dwActiveOpens  : DWORD;    // active opens
    dwPassiveOpens : DWORD;    // passive opens
    dwAttemptFails : DWORD;    // failed attempts
    dwEstabResets  : DWORD;    // established connections reset
    dwCurrEstab    : DWORD;    // established connections
    dwInSegs       : DWORD;    // segments received
    dwOutSegs      : DWORD;    // segment sent
    dwRetransSegs  : DWORD;    // segments retransmitted
    dwInErrs       : DWORD;    // incoming errors
    dwOutRsts      : DWORD;    // outgoing resets
    dwNumConns     : DWORD;    // cumulative connections
  end;
  PMIB_TCPSTATS = ^TMIB_TCPSTATS;

  // ************* MIB - описатели соединения ************
  // ** Стандартные описатели **

  // Описание соединения TCP/IP
  TMIB_TCPROW = packed record
   dwState      : DWORD;   // state of the connection
   dwLocalAddr  : DWORD;   // address on local computer
   dwLocalPort  : DWORD;   // port number on local computer
   dwRemoteAddr : DWORD;   // address on remote computer
   dwRemotePort : DWORD;   // port number on remote computer
  end;
  PMIB_TCPROW = ^TMIB_TCPROW;

  // Таблица соединений TCP/IP
  TMIB_TCPTABLE = packed record
   dwNumEntries : DWORD;                             // number of entries in the table
   MIB_TCPROW   : packed array[0..1] of TMIB_TCPROW; // array of TCP connections
  end;
  PMIB_TCPTABLE = ^TMIB_TCPTABLE;

  // Описание соединения TCP/UDP
  TMIB_UDPROW = packed record
   dwLocalAddr  : DWORD;   // address on local computer
   dwLocalPort  : DWORD;   // port number on local computer
  end;
  PMIB_UDPROW = ^TMIB_UDPROW;

  // Таблица соединений TCP/UDP
  TMIB_UDPTABLE = packed record
   dwNumEntries : DWORD;                             // number of entries in the table
   MIB_UDPROW   : packed array[0..1] of TMIB_UDPROW; // array of UDP connections
  end;
  PMIB_UDPTABLE = ^TMIB_UDPTABLE;

  // ** Расширенные описатели **
  // Описание соединения TCP/IP - расширенный вариант
  TMib_TcpExRow = packed record
    dwState       : DWORD;  // Состояние
    dwLocalAddr   : DWORD;  // Локальный адрес
    dwLocalPort   : DWORD;  // Локальный порт
    dwRemoteAddr  : DWORD;  // Удаленный адрес
    dwRemotePort  : DWORD;  // Удаленный порт
    dwProcessId   : DWORD;  // PID процесса
  end;
  PMib_TcpExRow = ^TMib_TcpExRow;

  // Таблица соединений TCP/IP - расширенный вариант
  TMib_TcpExTable = packed record
    dwNumEntries : DWORD;
    MIB_TCPROW   : array[0..1] of TMib_TcpExRow;
  end;
  PMib_TcpExTable = ^TMib_TcpExTable;

  // Описание соединения TCP/UDP - расширенный вариант с PID процесса
  TMib_UdpExRow = packed record
    dwLocalAddr : DWORD;
    dwLocalPort : DWORD;
    dwProcessId : DWORD;
  end;
  PMib_UdpExRow = ^TMib_UdpExRow;

  // Таблица соединений TCP/UDP
  TMib_UdpExTable = packed record
    dwNumEntries : DWORD;
    MIB_UDPROW   : array[0..1] of TMib_UdpExRow;
  end;
  PMib_UdpExTable = ^TMib_UdpExTable;

  // Данные об интерфейсе
  TMIB_IFROW = packed record
   wszName       : packed array [0 .. 255] of WideChar;
   dwIndex       : DWORD;    // индекс интерфейса
   dwType        : DWORD;    // тип интерфейса
   dwMtu         : DWORD;    // max transmission unit
   dwSpeed       : DWORD;    // speed of the interface
   dwPhysAddrLen : DWORD;    // length of physical address
   bPhysAddr     : packed array [0 .. 7] of CHAR; // physical address of adapter
   dwAdminStatus : DWORD;    // administrative status
   dwOperStatus  : DWORD;    // operational status
   dwLastChange  : DWORD;    // last time operational status changed
   dwInOctets    : DWORD;    // octets received
   dwInUcastPkts : DWORD;    // unicast packets received
   dwInNUcastPkts : DWORD;   // non-unicast packets received
   dwInDiscards   : DWORD;   // received packets discarded
   dwInErrors     : DWORD;   // erroneous packets received
   dwInUnknownProtos : DWORD;// unknown protocol packets received
   dwOutOctets     : DWORD;  // octets sent
   dwOutUcastPkts  : DWORD;  // unicast packets sent
   dwOutNUcastPkts : DWORD;  // non-unicast packets sent
   dwOutDiscards   : DWORD;  // outgoing packets discarded
   dwOutErrors     : DWORD;  // erroneous packets sent
   dwOutQLen       : DWORD;  // output queue length
   dwDescrLen      : DWORD;  // length of bDescr member
   bDescr          : packed array[0 ..255] of Char;  // interface description
  end;
 PMIB_IFROW = ^TMIB_IFROW;

 TMIB_IFTABLE = packed record
  dwNumEntries : DWORD;
  MIB_IFROW    : array[0..20] of TMIB_IFROW;
 end;
 PMIB_IFTABLE = ^TMIB_IFTABLE;

 TGetNetworkParams     = function (FI : PFixedInfo; Var BufLen : Integer)     : DWORD;StdCall;
 TGetAdaptersInfo      = function (AI : PIPAdapterInfo; Var BufLen : Integer) : DWORD;StdCall;
 TGetTcpStatistics     = function (ST : PMIB_TCPSTATS) : DWORD;StdCall;
 TGetTcpTable          = function (pTcpTable : PMIB_TCPTABLE; Size : PDWORD; Order : Bool) : DWORD;StdCall;
 TGetUDPTable          = function (pUDPTable : PMIB_UDPTABLE; Size : PDWORD; Order : Bool) : DWORD;StdCall;
 TGetIfEntry           = function (pIfRow    : PMIB_IFROW) : DWORD;StdCall;
 TGetIfTable           = function (pIfTable : PMIB_IFTABLE; pdwSize : PULONG; bOrder : BOOL) : DWORD;StdCall;
 TAllocateAndGetTcpExTableFromStack = function(pTcpExTable: PMib_TcpExTable; bOrder: BOOL; heap: THandle; dwZero, dwFlags: DWORD): DWORD; stdcall;
 TAllocateAndGetUdpExTableFromStack = function(pUdpExTable: PMib_UdpExTable; bOrder: BOOL; heap: THandle; dwZero, dwFlags: DWORD): DWORD; stdcall;

 // Получение сетевых параметров
 Function GetNetworkParams(FI : PFixedInfo; Var BufLen : Integer) : DWORD;StdCall;
 // Получение информации о адаптере
 Function GetAdaptersInfo(AI : PIPAdapterInfo; Var BufLen : Integer) : DWORD;StdCall;
 // Получение статистика TCP/IP
 Function GetTcpStatistics(ST : PMIB_TCPSTATS) : DWORD;StdCall;
 // Получение списка соединений TCP
 Function GetTcpTable(pTcpTable : PMIB_TCPTABLE; Size : PDWORD; Order : Bool) : DWORD;StdCall;
 // Получение списка соединений UDP
 Function GetUdpTable(pUDPTable : PMIB_UDPTABLE; Size : PDWORD; Order : Bool) : DWORD;StdCall;
 // Получение данных об интерфейсе
 Function GetIfEntry(pIfRow    : PMIB_IFROW) : DWORD;StdCall;
 // Получение данных об интерфейсе
 Function GetIfTable(pIfTable : PMIB_IFTABLE; pdwSize : PULONG; bOrder : BOOL) : DWORD;StdCall;
 Function AllocateAndGetTcpExTableFromStack(pTcpExTable: PMib_TcpExTable; bOrder: BOOL; heap: THandle; dwZero, dwFlags: DWORD): DWORD; stdcall;
 Function AllocateAndGetUdpExTableFromStack(pUdpExTable: PMib_UdpExTable; bOrder: BOOL; heap: THandle; dwZero, dwFlags: DWORD): DWORD; stdcall;

 //В качестве первого значения массив, второе значение, размер данных в массиве
 function GetMAC(MIB_IFROW   : TMIB_IFROW): String;
 // Проверка, доступно API
 Function CheckIPHelperLoaded : boolean;
 // Проверка, доступно ли расширенное API
 Function CheckIPHelperExLoaded : boolean;
implementation
var
  hIPHelper : THandle;  // Handle на библиотеку
  // Указатели на функции
  _GetNetworkParams  : TGetNetworkParams;
  _GetAdaptersInfo   : TGetAdaptersInfo;
  _GetTcpStatistics  : TGetTcpStatistics;
  _GetTcpTable       : TGetTcpTable;
  _GetUDPTable       : TGetUDPTable;
  _GetIfEntry        : TGetIfEntry;
  _GetIfTable        : TGetIfTable;
  _AllocateAndGetTcpExTableFromStack : TAllocateAndGetTcpExTableFromStack;
  _AllocateAndGetUdpExTableFromStack : TAllocateAndGetUdpExTableFromStack;

Function CheckIPHelperLoaded : boolean;
begin
 Result := false;
 // Библиотека еще на зегружена - пробуем ее загрузить
 if hIPHelper = 0 then begin
  hIPHelper := LoadLibrary('iphlpapi.dll');
  // Ошибка загрузки ??
  if hIPHelper < 32 then begin
   hIPHelper := 0;
   Result := False;
   Exit;
  end;
  // Получаем указатели
  @_GetNetworkParams  := GetProcAddress(hIPHelper,  'GetNetworkParams');
  @_GetAdaptersInfo   := GetProcAddress(hIPHelper,  'GetAdaptersInfo');
  @_GetTcpStatistics  := GetProcAddress(hIPHelper,  'GetTcpStatistics');
  @_GetTcpTable       := GetProcAddress(hIPHelper,  'GetTcpTable');
  @_GetUDPTable       := GetProcAddress(hIPHelper,  'GetUdpTable');
  @_GetIfEntry        := GetProcAddress(hIPHelper,  'GetIfEntry');
  @_GetIfTable        := GetProcAddress(hIPHelper,  'GetIfTable');
  @_AllocateAndGetTcpExTableFromStack := nil;
  @_AllocateAndGetTcpExTableFromStack := GetProcAddress(hIPHelper,  'AllocateAndGetTcpExTableFromStack');
  @_AllocateAndGetUdpExTableFromStack := nil;
  @_AllocateAndGetUdpExTableFromStack := GetProcAddress(hIPHelper,  'AllocateAndGetUdpExTableFromStack');
  Result := True;
 end else Result := True;
end;

Function CheckIPHelperExLoaded : boolean;
begin
 Result := CheckIPHelperLoaded and
          Assigned(_AllocateAndGetTcpExTableFromStack) and
          Assigned(_AllocateAndGetUdpExTableFromStack);
end;

//В качестве первого значения массив, второе значение, размер данных в массиве
function GetMAC(MIB_IFROW   : TMIB_IFROW): String;
var
  i: Integer;
begin
  if MIB_IFROW.dwPhysAddrLen = 0 then Result := '00:00:00:00:00:00' else
  begin
    Result := '';
    for i:= 0 to MIB_IFROW.dwPhysAddrLen - 1 do begin
     if Result <> '' then
      Result := Result + ':';
     Result := Result + IntToHex(byte(MIB_IFROW.bPhysAddr[i]),2);
    end;
  end;
end;

Function GetNetworkParams(FI : PFixedInfo; Var BufLen : Integer) : DWORD;
begin
 if CheckIPHelperLoaded then
  Result := _GetNetworkParams(FI, BufLen)
   else Result := ERROR_NOT_SUPPORTED;
end;

Function GetAdaptersInfo(AI : PIPAdapterInfo; Var BufLen : Integer) : DWORD;
begin
 if CheckIPHelperLoaded then
  Result := _GetAdaptersInfo(AI, BufLen)
   else Result := ERROR_NOT_SUPPORTED;
end;

Function GetTcpStatistics(ST : PMIB_TCPSTATS) : DWORD;StdCall;
begin
 if CheckIPHelperLoaded then
  Result := _GetTcpStatistics(ST)
   else Result := ERROR_NOT_SUPPORTED;
end;

Function GetTcpTable(pTcpTable : PMIB_TCPTABLE; Size : PDWORD; Order : Bool) : DWORD;StdCall;
begin
 if CheckIPHelperLoaded then
  Result := _GetTcpTable(pTcpTable, Size, Order)
   else Result := ERROR_NOT_SUPPORTED;
end;

Function GetUDPTable(pUdpTable : PMIB_UDPTABLE; Size : PDWORD; Order : Bool) : DWORD;StdCall;
begin
 if CheckIPHelperLoaded then
  Result := _GetUdpTable(pUdpTable, Size, Order)
   else Result := ERROR_NOT_SUPPORTED;
end;

Function GetIfEntry(pIfRow    : PMIB_IFROW) : DWORD;
begin
 if CheckIPHelperLoaded then
  Result := _GetIfEntry(pIfRow)
   else Result := ERROR_NOT_SUPPORTED;
end;

// Получение данных об интерфейсе
Function GetIfTable(pIfTable : PMIB_IFTABLE; pdwSize : PULONG; bOrder : BOOL) : DWORD;StdCall;
begin
 if CheckIPHelperLoaded then
  Result := _GetIfTable(pIfTable, pdwSize, bOrder)
   else Result := ERROR_NOT_SUPPORTED;
end;

// Получение расширенной таблицы портов и соединений TCP
Function AllocateAndGetTcpExTableFromStack(pTcpExTable: PMib_TcpExTable; bOrder: BOOL; heap: THandle; dwZero, dwFlags: DWORD): DWORD;
begin
 if CheckIPHelperExLoaded then
  Result := _AllocateAndGetTcpExTableFromStack(pTcpExTable, bOrder, heap, dwZero, dwFlags)
   else Result := ERROR_NOT_SUPPORTED;
end;

// Получение расширенной таблицы портов и соединений UDP
Function AllocateAndGetUdpExTableFromStack(pUdpExTable: PMib_UdpExTable; bOrder: BOOL; heap: THandle; dwZero, dwFlags: DWORD): DWORD; stdcall;
begin
 if CheckIPHelperExLoaded then
  Result := _AllocateAndGetUdpExTableFromStack(pUdpExTable, bOrder, heap, dwZero, dwFlags)
   else Result := ERROR_NOT_SUPPORTED;
end;

initialization
 hIPHelper := 0;

finalization
 if hIPHelper <> 0 then
  FreeLibrary(hIPHelper);
end.

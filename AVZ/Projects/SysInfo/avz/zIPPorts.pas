unit zIPPorts;

interface
uses Windows, Classes, NetInfo, IPHelperDef,
  Winsock, SystemMonitor, ntdll,
  zAntivirus, zStringCompressor;
type
  // Информация по порту или соединению
  TIPConnection = record
   LocalHost   : string;
   LocalPort   : word;
   RemoteHost  : string;
   RemotePort  : word;
   State       : string;  // Состояние
   StateID     : integer;
   ProcessID   : dword;
   ProcessBin  : string;  // Имя EXE файла
   Prim        : string;  // Примечания с результатами анализа
   CheckResult : integer; // Результат проверки по базе безопасных программ
   Flags       : byte;
  end;

  TIPPortInfo = array of TIPConnection;

  // Описание порта
  TPortDescr = record
   Port  : word;
   Flags : byte;
   Cmt, ExeFile : string;
  end;

  // Информационный класс
  TIPPorts = class
  private
   FSysProcessInfo : TSysProcessInfo;
   // Сравнение двух элементов по полю N (0-равны, 1 - a>b, -1 - a<b)
   function CompareItems(a, b : TIPConnection; AFieldN : integer) : integer;
   function CreateNativeProcessList(ALines: TStrings): boolean;
  public
   UDPConnections, TCPConnections : TIPPortInfo;
   PortDescrList : array of TPortDescr;
   constructor Create;
   destructor Destroy; override;
   // Загрузка базы описания портов
   function LoadPortDescr(AFileName : string) : boolean;
   // Построение списка портов TCP
   function RefresTCPConnections: boolean;
   // Построение списка портов TCP через расширенный API
   function RefresTCPExConnections: boolean;
   // Построение списка портов UDP
   function RefresUDPConnections: boolean;
   // Построение списка портов UDP через расширенный API
   function RefresUDPExConnections: boolean;
   // Обновление спиков портов (метод выбирается автоматом исходя из доступности)
   function Refresh : boolean;
   // Добавление описания
   function CommentPorts(IPPortInfo : TIPPortInfo; IsTcp : boolean) : boolean;
   // Сортировка массива по полю номер N
   function SotrTable(ATable : TIPPortInfo; AFieldN : integer) : boolean;
   // Декодирование битов категории
   function DecodeCatBits(B : byte) : string;
  end;

implementation
uses orautil;
{ TIPPorts }

// Сравнение элементов по коду поля
function TIPPorts.CompareItems(a, b: TIPConnection;
  AFieldN: integer): integer;
begin
 Result := 0;
 case AFieldN of
  0 : begin
       if a.LocalHost > b.LocalHost then Result := 1;
       if a.LocalHost < b.LocalHost then Result := -1;
      end;
  1 : begin
       if a.LocalPort > b.LocalPort then Result := 1;
       if a.LocalPort < b.LocalPort then Result := -1;
      end;
  2 : begin
       if a.RemoteHost > b.RemoteHost then Result := 1;
       if a.RemoteHost < b.RemoteHost then Result := -1;
      end;
  3 : begin
       if a.RemotePort > b.RemotePort then Result := 1;
       if a.RemotePort < b.RemotePort then Result := -1;
      end;
  4 : begin
       if a.State > b.State then Result := 1;
       if a.State < b.State then Result := -1;
      end;
  5 : begin
       if a.ProcessBin > b.ProcessBin then Result := 1;
       if a.ProcessBin < b.ProcessBin then Result := -1;
      end;
  6 : begin
       if a.Prim > b.Prim then Result := 1;
       if a.Prim < b.Prim then Result := -1;
      end;
 end;
end;

constructor TIPPorts.Create;
begin
 PortDescrList := nil;
 if ISNT then
  FSysProcessInfo := TPSAPIProcessInfo.Create else
   FSysProcessInfo := TToolHelpProcessInfo.Create;
end;

destructor TIPPorts.Destroy;
begin
 FSysProcessInfo.Free;
 FSysProcessInfo := nil;
 PortDescrList   := nil;
 inherited;
end;

function TIPPorts.Refresh: boolean;
begin
 Result := false;
 // Проверка, доступно ли API
 if not(CheckIPHelperLoaded) then begin
  exit;
 end;
 try
  // Построение списка TCP портов
  if CheckIPHelperExLoaded then RefresTCPExConnections
   else RefresTCPConnections;
  // Построение списка UDP портов
  if CheckIPHelperExLoaded then RefresUDPExConnections
   else RefresUDPConnections;
 except
 end;  
 Result := true;
end;

function TIPPorts.RefresTCPConnections: boolean;
var
 Res : integer;
 i, Size : DWORD;
 MIB_TCPTABLE : PMIB_TCPTABLE;
 addr : TInAddr;
 s : string;
begin
 Result := false;
 MIB_TCPTABLE   := nil;
 TCPConnections := nil;
 try
   Size := 10000;
   repeat
    GetMem(MIB_TCPTABLE, Size);
    ZeroMemory(MIB_TCPTABLE, Size);
    Res := GetTcpTable(MIB_TCPTABLE, @Size, true);
    // Увеличение размера буфера
    if Res = ERROR_INSUFFICIENT_BUFFER then begin
     FreeMem(MIB_TCPTABLE, Size);
     Size := Size * 2;
    end;
   until Res <> ERROR_INSUFFICIENT_BUFFER;
   // Ошибка ?? Тогда выход
   if Res <> NO_ERROR then begin
    FreeMem(MIB_TCPTABLE, Size);
    exit;
   end;
   // Логический контроль
   if MIB_TCPTABLE^.dwNumEntries > $FFFF then begin
    FreeMem(MIB_TCPTABLE, Size);
    exit;
   end;
   SetLength(TCPConnections, MIB_TCPTABLE^.dwNumEntries);
   for i := 0 to MIB_TCPTABLE^.dwNumEntries-1 do begin
    s := '';
    {$R-}
    case MIB_TCPTABLE^.MIB_TCPROW[i].dwState of
      0 : s := '???';
      1 : s := 'CLOSED';
      2 : s :=  'LISTENING';
      3 : s :=  'SYN_SENT';
      4 : s :=  'SYN_RECEIVED';
      5 : s :=  'ESTABLISHED';
      6 : s :=  'FIN_WAIT';
      7 : s :=  'FIN_WAIT2';
      8 : s :=  'CLOSE_WAIT';
      9 : s :=  'CLOSING';
      10 : s := 'LAST_ACK';
      11 : s := 'TIME_WAIT';
     end;
    TCPConnections[i].State := s;
    TCPConnections[i].StateID := MIB_TCPTABLE^.MIB_TCPROW[i].dwState;
    TCPConnections[i].ProcessBin := '';
    TCPConnections[i].ProcessID := 0;
    addr.S_addr := MIB_TCPTABLE^.MIB_TCPROW[i].dwLocalAddr;
    TCPConnections[i].LocalHost := String(inet_ntoa(addr));
    TCPConnections[i].LocalPort := htons(MIB_TCPTABLE^.MIB_TCPROW[i].dwLocalPort);
    addr.S_addr := u_long(MIB_TCPTABLE^.MIB_TCPROW[i].dwRemoteAddr);
    TCPConnections[i].RemoteHost := String(inet_ntoa(addr));
    TCPConnections[i].RemotePort := htons(MIB_TCPTABLE^.MIB_TCPROW[i].dwRemotePort);
    TCPConnections[i].CheckResult := -1;
    {$R+}
   end;
   FreeMem(MIB_TCPTABLE, Size);
 finally
 end;
 CommentPorts(TCPConnections, true);
end;

function TIPPorts.RefresTCPExConnections: boolean;
var
 Res : integer;
 i, j : DWORD;
 Mib_TcpExTable : PMib_TcpExTable;
 addr : TInAddr;
 s : string;
 FProcessList, FNativeProcessList : TStringList;
begin
 TCPConnections := nil;
 FProcessList       := TStringList.Create;
 FNativeProcessList := TStringList.Create;
 try
   // Создание списка процессов
   FSysProcessInfo.CreateProcessList(FProcessList);
   if ISNT then
    CreateNativeProcessList(FNativeProcessList);
   Res := AllocateAndGetTcpExTableFromStack(@Mib_TcpExTable, True, GetProcessHeap, 0, 2);
   // Ошибка ?? Тогда выход
   if Res <> NO_ERROR then begin
    exit;
   end;
   // Логический контроль
   if Mib_TcpExTable^.dwNumEntries > $FFFF then exit;
   // Задание длинны таблицы
   SetLength(TCPConnections,  Mib_TcpExTable^.dwNumEntries);
   for i := 0 to Mib_TcpExTable^.dwNumEntries-1 do begin
    s := '';
    {$R-}
    case Mib_TcpExTable^.MIB_TCPROW[i].dwState of
      0 : s := '???';
      1 : s := 'CLOSED';
      2 : s :=  'LISTENING';
      3 : s :=  'SYN_SENT';
      4 : s :=  'SYN_RECEIVED';
      5 : s :=  'ESTABLISHED';
      6 : s :=  'FIN_WAIT';
      7 : s :=  'FIN_WAIT2';
      8 : s :=  'CLOSE_WAIT';
      9 : s :=  'CLOSING';
      10 : s := 'LAST_ACK';
      11 : s := 'TIME_WAIT';
     end;
    TCPConnections[i].StateID := Mib_TcpExTable^.MIB_TCPROW[i].dwState;
    TCPConnections[i].State := s;
    addr.S_addr := Mib_TcpExTable^.MIB_TCPROW[i].dwLocalAddr;
    TCPConnections[i].LocalHost := String(inet_ntoa(addr));
    TCPConnections[i].LocalPort := htons(Mib_TcpExTable^.MIB_TCPROW[i].dwLocalPort);
    addr.S_addr := u_long(Mib_TcpExTable^.MIB_TCPROW[i].dwRemoteAddr);
    TCPConnections[i].RemoteHost := String(inet_ntoa(addr));
    TCPConnections[i].RemotePort := htons(Mib_TcpExTable^.MIB_TCPROW[i].dwRemotePort);
    TCPConnections[i].CheckResult := -1;
    TCPConnections[i].ProcessID   := Mib_TcpExTable^.MIB_TCPROW[i].dwProcessId;
    TCPConnections[i].ProcessBin := '';
    // Поиск процесса по его PID
    if FProcessList.Count > 0 then
     for j := 0 to FProcessList.Count - 1 do
      if TProcessInfo(FProcessList.Objects[j]).PID = Mib_TcpExTable^.MIB_TCPROW[i].dwProcessId then begin
       TCPConnections[i].ProcessBin := TProcessInfo(FProcessList.Objects[j]).ExeFileName;
       break;
      end;
    // Не нашли в списке обычных процессов - ищем в расширенном списке
    if TCPConnections[i].ProcessBin = '' then
     for j := 0 to FNativeProcessList.Count - 1 do
      if dword(FNativeProcessList.Objects[j]) = Mib_TcpExTable^.MIB_TCPROW[i].dwProcessId then begin
       TCPConnections[i].ProcessBin := FNativeProcessList[j];
       Break;
      end;
    {$R+}
   end;
   HeapFree(GetProcessHeap, 0, Mib_TcpExTable);
 finally
  FSysProcessInfo.ClearList(FProcessList);
  FProcessList.Free;
  FNativeProcessList.Free;
 end;
 CommentPorts(TCPConnections, true);
end;

function TIPPorts.RefresUDPConnections: boolean;
var
 Res : integer;
 i, Size : DWORD;
 MIB_UDPTABLE : PMIB_UDPTABLE;
 addr : TInAddr;
 s : string;
begin
 UDPConnections := nil;
 try
   Size := 10000;
   repeat
    GetMem(MIB_UDPTABLE, Size);
    ZeroMemory(MIB_UDPTABLE, Size);
    Res := GetUdpTable(MIB_UDPTABLE, @Size, true);
    // Увеличение размера буфера
    if Res = ERROR_INSUFFICIENT_BUFFER then begin
     FreeMem(MIB_UDPTABLE, Size);
     Size := Size * 2;
    end;
   until Res <> ERROR_INSUFFICIENT_BUFFER;
   // Ошибка ?? Тогда выход
   if Res <> NO_ERROR then begin
    FreeMem(MIB_UDPTABLE, Size);
    exit;
   end;
   // Логический контроль
   if MIB_UDPTABLE^.dwNumEntries > $FFFF then begin
    FreeMem(MIB_UDPTABLE, Size);
    exit;
   end;
   SetLength(UDPConnections, MIB_UDPTABLE^.dwNumEntries);
   for i := 0 to MIB_UDPTABLE^.dwNumEntries-1 do begin
    s := '';
    {$R-}
    addr.S_addr := MIB_UDPTABLE^.MIB_UDPROW[i].dwLocalAddr;
    s := String(inet_ntoa(addr));
    UDPConnections[i].LocalHost := s;
    UDPConnections[i].LocalPort :=htons(MIB_UDPTABLE^.MIB_UDPROW[i].dwLocalPort);
    UDPConnections[i].RemoteHost := ''; UDPConnections[i].RemotePort := 0;
    UDPConnections[i].CheckResult := -1;
    UDPConnections[i].ProcessID := 0;
    UDPConnections[i].State := 'LISTENING';
    {$R+}
   end;
   FreeMem(MIB_UDPTABLE, Size);
 finally
 end;
 CommentPorts(UDPConnections, false);
end;

function TIPPorts.RefresUDPExConnections: boolean;
var
 Res : integer;
 i, j : DWORD;
 Mib_UdpExTable : PMib_UdpExTable;
 addr : TInAddr;
 s : string;
 FProcessList, FNativeProcessList : TStringList;
begin
 UDPConnections := nil;
 FProcessList := TStringList.Create;
 FNativeProcessList := TStringList.Create;
 try
   FSysProcessInfo.CreateProcessList(FProcessList);
   if ISNT then
    CreateNativeProcessList(FNativeProcessList);
   Res := AllocateAndGetUdpExTableFromStack(@Mib_UdpExTable, True, GetProcessHeap, 0, 2);
   // Ошибка ?? Тогда выход
   if Res <> NO_ERROR then begin
    exit;
   end;
   // Логический контроль
   if Mib_UdpExTable^.dwNumEntries > $FFFF then exit;
   // Задание длинны таблицы
   SetLength(UDPConnections,  Mib_UdpExTable^.dwNumEntries);
   for i := 0 to Mib_UdpExTable^.dwNumEntries-1 do begin
    s := '';
    {$R-}
    UDPConnections[i].ProcessBin := '';
    addr.S_addr := Mib_UdpExTable^.MIB_UDPROW[i].dwLocalAddr;
    UDPConnections[i].LocalHost := String(inet_ntoa(addr));
    UDPConnections[i].LocalPort := htons(Mib_UdpExTable^.MIB_UDPROW[i].dwLocalPort);
    UDPConnections[i].RemoteHost := '';
    UDPConnections[i].RemotePort := 0;
    UDPConnections[i].CheckResult := -1;
    UDPConnections[i].State := 'LISTENING';
    UDPConnections[i].ProcessID := Mib_UdpExTable^.MIB_UDPROW[i].dwProcessId;
    // Поиск процесса по его PID
    if FProcessList.Count > 0 then
     for j := 0 to FProcessList.Count - 1 do
      if TProcessInfo(FProcessList.Objects[j]).PID = Mib_UdpExTable^.MIB_UDPROW[i].dwProcessId then begin
       UDPConnections[i].ProcessBin := TProcessInfo(FProcessList.Objects[j]).ExeFileName;
       break;
      end;
    // Не нашли в списке обычных процессов - ищем в расширенном списке
    if UDPConnections[i].ProcessBin = '' then
     for j := 0 to FNativeProcessList.Count - 1 do
      if dword(FNativeProcessList.Objects[j]) = Mib_UdpExTable^.MIB_UDPROW[i].dwProcessId then begin
       UDPConnections[i].ProcessBin := FNativeProcessList[j];
       Break;
      end;
    {$R+}
   end;
   HeapFree(GetProcessHeap, 0, Mib_UdpExTable);
 finally
  FSysProcessInfo.ClearList(FProcessList);
  FProcessList.Free;
 end;
 CommentPorts(UDPConnections, false);
end;

function TIPPorts.SotrTable(ATable: TIPPortInfo;
  AFieldN: integer): boolean;
var
 i, j : integer;
 Tmp : TIPConnection;
begin
 Result := false;
 // Таблицы нет ? Тогда выход
 if ATable = nil then exit;
 // Процесс сортировки
 for i := 0 to Length(ATable) - 2 do
  for j := i+1 to Length(ATable) - 1 do
   if CompareItems(ATable[i], ATable[j], AFieldN) > 0 then begin
    Tmp := ATable[i];
    ATable[i] := ATable[j];
    ATable[j] := Tmp;
   end;
end;

function TIPPorts.CreateNativeProcessList(ALines: TStrings): boolean;
var
  NTDLL_DLL : THandle;
  BufSize, ResBufSize: DWORD;
  Buf : Pointer;
  rel, cnt, Res : integer;
  EndOfList : boolean;
  ZwQuerySystemInformation: TNativeQuerySystemInformation;
  S : string;
begin
 Buf := nil;
 ALines.Clear;
 Result := false;
 // Загрузка библиотеки и поиск функции
 NTDLL_DLL := GetModuleHandle(NTDLL_DLL_name);
 // Поиск функции
 if NTDLL_DLL <> 0 then @ZwQuerySystemInformation := GetProcAddress(NTDLL_DLL,'ZwQuerySystemInformation')
  else exit;
 if @ZwQuerySystemInformation = nil then exit;
 try
   // Вывод в протокол адреса функции
   BufSize := 100000;
   // Получение списка процессов
   repeat
    GetMem(Buf, BufSize);
    Res := ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, Buf, BufSize, @ResBufSize);
    if Res = STATUS_INFO_LENGTH_MISMATCH then begin
     FreeMem(Buf, BufSize);
     BufSize := BufSize * 2;
    end;
   until Res <> STATUS_INFO_LENGTH_MISMATCH;
   // Ошибка ??
   if Res <> STATUS_SUCCESS then exit;
   rel := 0; cnt := 0;
   // Перемещение по списку и анализ
   repeat
    EndOfList := TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).NextEntryDelta = 0;
    if TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).ProcessName.Length > 0 then begin
      {SetLength(S, 200);
      hProcess := OpenProcess(SYNCHRONIZE or PROCESS_QUERY_INFORMATION, true, TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).ProcessID);
      Res := GetModuleFileName(hProcess, @s[1], length(S));
      CloseHandle(hProcess);
      S := copy(S, 1, Res);
      if S = '' then}
      S := String(TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).ProcessName.StrData);
      ALines.AddObject(S,
                       pointer(TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).ProcessID));
     end else ;
    inc(cnt);
    // Переход на следующую структуру, описывающую процесс
    rel := rel + TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).NextEntryDelta;
   until  EndOfList;
   Result := true;
 finally
  // Освобождение памяти
  if Buf <> nil then
   FreeMem(Buf, BufSize);
 end;
end;

function TIPPorts.LoadPortDescr(AFileName: string): boolean;
var
 MS    : TMemoryStream;
 Res : boolean;
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 i      : integer;
 buf_b  : byte;
 buf_w  : word;
 S : string;
begin
 PortDescrList := nil;
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if not(Res) then exit;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 SetLength(PortDescrList, VirusBaseHeader.RecCount);
 for i := 0 to VirusBaseHeader.RecCount - 1 do begin
  // Флаги порта
  MS.Read(buf_b, 1);
  PortDescrList[i].Flags := buf_b;
  // Номер порта
  MS.Read(buf_w,  2);
  PortDescrList[i].Port := buf_w;
  // Описание
  MS.Read(buf_w,  2);
  setlength(S, buf_w);
  MS.Read(S[1],  buf_w);
  PortDescrList[i].Cmt := S;
  // Имя EXE файла
  MS.Read(buf_w,  2);
  setlength(S, buf_w);
  if buf_w > 0 then
   MS.Read(S[1],  buf_w);
  PortDescrList[i].ExeFile := S;
 end;
end;

function TIPPorts.CommentPorts(IPPortInfo: TIPPortInfo; IsTcp : boolean): boolean;
var
 i, j : integer;
begin
 if IPPortInfo = nil then exit;
 if PortDescrList = nil then exit;
 for i := 0 to length(IPPortInfo) - 1 do
  for j := 0 to  length(PortDescrList) - 1 do
   if ((((PortDescrList[j].Flags and 1) = 0) and IsTcp) or
       (((PortDescrList[j].Flags and 1) = 1) and not(IsTcp)))
        and (IPPortInfo[i].LocalPort = PortDescrList[j].Port) then begin
          IPPortInfo[i].Prim := PortDescrList[j].Cmt;
          IPPortInfo[i].Flags := PortDescrList[j].Flags;
          Break;
        end;
end;

function TIPPorts.DecodeCatBits(B: byte): string;
begin
 Result := '';
 if (b and 2) > 0 then AddToList(Result, '$AVZ0955',', ');
 if (b and 4) > 0 then AddToList(Result, '$AVZ0999',', ');
 if (b and 8) > 0 then AddToList(Result, 'Trojan',', ');
 if (b and 16) > 0 then AddToList(Result, 'Worm',', ');
 if (b and 32) > 0 then AddToList(Result, 'Backdoor',', ');
 if (b and 64) > 0 then AddToList(Result, 'Riskware',', ');
end;

end.

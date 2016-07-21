unit zNetInt;

interface
uses Windows, zantivirus, SystemMonitor, classes, Sysutils, NetInfo, IPHelperDef, Winsock, forms, registry, md5, RxVerInf;
 // ************** Процессы ***************
 // Создание XML со списком процессов
 Function CreateProcessListXML(ProcessMonitor : TProcessMonitor; AList : TStrings) : boolean;
 // Создание XML со статистикой процессов и подробностями (размер, сигнатуры ...)
 Function CreateProcessStatXML(ProcessMonitor : TProcessMonitor; AList : TStrings) : boolean;
 // Получение информации о процессе
 Function CreateProcessInfoXML(APID : DWORD; ProcessMonitor : TProcessMonitor; AList : TStrings) : boolean;

 // ************** Система ***************
 // Создание XML с переменными окружения
 Function CreateEnvXML(AList : TStrings) : boolean;
 // Информация о системе
 Function CreateSysInfoXML(AList : TStrings) : boolean;
 // Информация о дисках
 Function CreateDiskInfoXML(AList : TStrings) : boolean;
 // Автозагрузка
 Function CreateAutorunXML(AList : TStrings) : boolean;
 // Информация о файле
 Function CreateFileInfoXML(AFileName : string;  ProcessMonitor : TProcessMonitor; AList : TStrings) : boolean;
 // ************** Сеть ***************
 // Расшаренные ресурсы
 Function CreateNetSharesXML(AList : TStrings) : boolean;
 // Подключения
 Function CreateNetConnectionsXML(AList : TStrings) : boolean;
 // Открытые файлы
 Function CreateNetOpenFilesXML(AList : TStrings) : boolean;
 // ************** Сеть - низкоуровневые данные ***************
 // Статистика NIC
 Function CreateNICStatXML(AList : TStrings) : boolean;
 // Статистика TCP
 Function CreateTCPStatXML(AList : TStrings) : boolean;
 // Настройки TCP/IP
 Function CreateTCPSetupXML(AList : TStrings) : boolean;
 // Настройки DNS
 Function CreateDNSSetupXML(AList : TStrings) : boolean;
 // ************** Сеть IP - порты ***************
 // Порты TCP/IP
 Function CreateTCPConnectXML(AList : TStrings) : boolean;
 // Порты UDP/IP
 Function CreateUDPPortXML(AList : TStrings) : boolean;
 // ************* Данные по AVP
 Function CreateAVPXML(AList : TStrings) : boolean;

implementation
uses zutil;

function ISNT : boolean;
var
 Ver  : TOSVersionInfo;
begin
 Result := False;
 Ver.dwOSVersionInfoSize := SizeOf(TOSVersionInfo);
 GetVersionEx(Ver);
 case Ver.dwPlatformId of
  VER_PLATFORM_WIN32_NT      : Result := True;   //Windows NT - подходит
  VER_PLATFORM_WIN32_WINDOWS : Result := False;  //Windows 9x-Me - подходит
  VER_PLATFORM_WIN32s        : Result := False;  //Windows 3.x - не подходит
 end;
end;

Function RegReadString(ARootKey : HKEY; APath, AKey : string) : string;
var
 Reg : TRegistry;
begin
 Result := '';
 try
  Reg := TRegistry.Create;
  Reg.RootKey := ARootKey;
  Reg.Access := KEY_READ;
  if Reg.OpenKey(APath, false) then begin
   Result := Reg.ReadString(AKey);
   Reg.CloseKey;
  end;
  Reg.Free;
 except // Подавление любой ошибки
 end;
end;

Function CreateProcessListXML(ProcessMonitor : TProcessMonitor; AList : TStrings) : boolean;
var
 i : integer;
 ProcessInfo : TProcessInfo;
begin
 AList.Clear;
 AList.Add('<PROCESS>');
 ProcessMonitor.RefreshProcessStat;
 for i := 0 to ProcessMonitor.ProcessList.Count - 1 do begin
  ProcessInfo := ProcessMonitor.ProcessList.Objects[i] as TProcessInfo;
  AList.Add(' <PI '+
           'Name='''+ProcessInfo.ExeFileName+''' '+
           'PID='''+IntToHex(ProcessInfo.PID, 8)+''' '+
           'tc='''+inttostr(ProcessInfo.cntThreads)+''' '+
           'pr='''+inttostr(ProcessInfo.PriorityClass)+''' '+
           '/>'
          );
 end;
 AList.Add('</PROCESS>');
end;

// Получение информации о процессе
Function CreateProcessInfoXML(APID : DWORD; ProcessMonitor : TProcessMonitor; AList : TStrings) : boolean;
var
 i : integer;
 TmpList : TStrings;
 ModuleInfo : TModuleInfo;
begin
 AList.Clear;
 TmpList := TStringList.Create;
 AList.Add('<PROCESS_INFO PID='''+IntToStr(APID)+''' />');
 Result := ProcessMonitor.SysProcessInfo.CreateModuleList(TmpList, APID);
 if not(Result) then exit;
 for i := 0 to TmpList.Count - 1 do begin
  ModuleInfo := TmpList.Objects[i] as TModuleInfo;
  AList.Add(' <PI '+
           'name='''+ModuleInfo.ModuleName+''' '+
           'hModule='''+IntToHex(ModuleInfo.hModule, 8)+''' '+
           '/>'
          );
 end;
 AList.Add('</PROCESS_INFO>');
end;

Function CreateProcessStatXML(ProcessMonitor : TProcessMonitor; AList : TStrings) : boolean;
var
 i, j : integer;
 ProcessStat : TProcessStat;
 S : string;
begin
 AList.Clear;
 AList.Add('<PROCESS>');
 ProcessMonitor.RefreshProcessStat;
 for i := 1 to ProcessMonitor.ProcessListStat.Count - 1 do begin
  ProcessStat := ProcessMonitor.ProcessListStat.Objects[i] as TProcessStat;
  S := '';
  for j := 1 to 4 do
   S := S + 'sg'+inttostr(j)+'=''' + IntToHex(ProcessStat.PEFileInfo.Sign[j], 8)+''' ';
  AList.Add(' <PI '+
           'Name='''+ProcessStat.ExeFileName+''' '+
           'PID='''+IntToHex(ProcessStat.PID, 8)+''' '+
           'Cnt='''+inttostr(ProcessStat.Count)+''' '+
           'Time='''+inttostr(ProcessStat.SummTime)+''' '+
           'StartTime='''+DateTimeToStr(ProcessStat.FirstStart)+''' '+
           'size='''+inttostr(ProcessStat.PEFileInfo.Size)+''' '+
           'ver='''+ProcessStat.PEFileInfo.FileVersion+''' '+
           'cr='''+ProcessStat.PEFileInfo.LegalCopyright+''''+
           ' ' + S+
           '/>'
          );
 end;
 AList.Add('</PROCESS>');
end;

// Создание XML с переменными окружения
Function CreateEnvXML(AList : TStrings) : boolean;
var
  c, i, j :dword;
  b :pchar;
  s : string;
begin
  AList.Clear;
  AList.Add('<VARLIST>');
  c:=1024;
  b:=GetEnvironmentStrings;
  i:=0; j := 1;
  s:='';
  while i<c do begin
    if b[i]<>#0 then
      s:=s+b[i]
    else begin
      if s='' then
        break;
      AList.Add(' <VAR num='''+inttostr(j)+''' val='''+s+''' />');
      inc(j);
      s:='';
    end;
    inc(i);
  end;
  FreeEnvironmentStrings(b);
  AList.Add('</VARLIST>');
end;

Function CreateSysInfoXML(AList : TStrings) : boolean;
var
 SysInfo : TSysInfo;
 MS      : TMemoryStatus;
 OS      : TOSVersionInfo;
begin
 AList.Clear;
 SysInfo := TSysInfo.Create;
 AList.Add('<SYSINFO ComputerName='''+SysInfo.ComputerName+'''>');
 AList.Add(' <DIR WinDir='''+SysInfo.WindowsHomePath+''' SysDir='''+SysInfo.WindowsSystemPath+ '''/>');
 OS := SysInfo.VersionInfo;
 AList.Add(' <VER Major='''+inttostr(OS.dwMajorVersion)+''' '+
           'Minor='''+inttostr(OS.dwMinorVersion)+''' '+
           'Build='''+inttostr(OS.dwBuildNumber)+''' '+
           'Platform='''+inttostr(OS.dwPlatformId)+''' '+
           'CDS='''+OS.szCSDVersion+'''/>');
 MS := SysInfo.MemoryStatus;
 AList.Add(' <RAM TotalPH='''+inttostr(MS.dwTotalPhys div 1024)+''' '+
               'AvailPH='''+inttostr(MS.dwAvailPhys div 1024)+''' '+
               'TotalV='''+inttostr(MS.dwTotalVirtual div 1024)+''' '+
               'AvailV='''+inttostr(MS.dwAvailVirtual div 1024)+''' '+
               'TotalPF='''+inttostr(MS.dwTotalPageFile div 1024)+''' '+
               'AvailPF='''+inttostr(MS.dwAvailPageFile div 1024)+''' '+
           '/>');
 AList.Add(' <SCREEN '+
           'W='''+inttostr(Screen.Width)+''' H='''+inttostr(Screen.Height)+''''+
           ' PPI='''+ inttostr(Screen.PixelsPerInch)+''''+
           ' MonitorCnt='''+ inttostr(Screen.MonitorCount)+''''+
           '/>');
 AList.Add('</SYSINFO>');
end;

Function CreateDiskInfoXML(AList : TStrings) : boolean;
var
 Bufer      : array[0..1024] of char; // Буфер
 RealLen, i : integer;                // Результирующая длина
 S          : string;                 // Временная строка
 dw         : DWORD;                  //
 SerialNum  : dword;
 VolumeName,                          // Имя тома
 FSName  : array [0..255] of char;
 MaximumFNameLength,
 FileSystemFlags     : dword;
 Drive : byte;
 DiskFreeBytes, DiskSizeBytes : int64;
 oldMode: Cardinal;
begin
 AList.Clear;
 try
   oldMode := SetErrorMode(SEM_FAILCRITICALERRORS);
   AList.Add('<DISKINFO>');
   // Составление списка устройств
   RealLen := GetLogicalDriveStrings(SizeOf(Bufer),Bufer);
   i := 0; S := '';
   // Цикл анализа буфера (последний символ не обрабатывается, т.к. он всегда #0)
   while i < RealLen do begin
    if Bufer[i] <> #0 then begin
     S := S + Bufer[i];
     inc(i);
    end else begin
     inc(i); // Пропуск разделяющего #0
     S := UpperCase(Trim(S));
     dw := GetDriveType(PChar(S));
     SerialNum := 0;
     DiskSizeBytes := 0;
     DiskFreeBytes := 0;
     if (dw = DRIVE_FIXED) then begin
      if not GetVolumeInformation(PChar(S), VolumeName, SizeOf(VolumeName),
                            @SerialNum,
                            MaximumFNameLength,
                            FileSystemFlags,
                            FSName, SizeOf(FSName)) then SerialNum := 0;
      // Запрос информации о свобобном месте на диске
      Drive := (ord(s[1]) - ord('A') + 1);
      DiskSizeBytes := DiskSize(Drive);
      DiskFreeBytes := DiskFree(Drive);
     end;
     AList.Add(' <DISK name='''+S+''' type='''+IntToStr(dw)+''' '+
                'SN='''+IntToHex(SerialNum, 10)+''' '+
                'FS_Type='''+Trim(FSName)+''' '+
                'Vol_name='''+Trim(VolumeName)+''' '+
                'FS_mlfn='''+IntToStr(MaximumFNameLength)+''' '+
                'FS_Flags='''+IntToStr(FileSystemFlags)+''' '+
                'FS_Flags='''+IntToStr(FileSystemFlags)+''' '+
                'DiskSize='''+IntToStr(DiskSizeBytes)+''' '+
                'DiskFree='''+IntToStr(DiskFreeBytes)+''' '+
              '/>');
     S := '';
    end;
   end;
 finally
  AList.Add('</DISKINFO>');
  SetErrorMode(oldMode);
 end;
end;

Function CreateNetSharesXML(AList : TStrings) : boolean;
var
 MSNetInfo      : TCustomMSNetInfo;
 NetShareInfo   : TNetShareInfo;
 i              : integer;
 List           : TStringList;
begin
 AList.Clear;
 AList.Add('<NET_SHARES>');
 try
   if ISNT then
    MSNetInfo := TNTMSNetInfo.Create
     else MSNetInfo := T9xMSNetInfo.Create;
   List      := TStringList.Create;
   MSNetInfo.CreateShareList(List);
   for i := 0 to List.Count - 1 do begin
    NetShareInfo := List.Objects[i] as TNetShareInfo;
    AList.Add(' <SHARE '+
             'Name='''+NetShareInfo.NetName+''' '+
             'Remark='''+NetShareInfo.Remark+''' '+
             'Path='''+NetShareInfo.Path+''' '+
             'CurUsers='''+IntToStr(NetShareInfo.CurUsers)+''' '+
             '/>');
   end;
 finally
  AList.Add('</NET_SHARES>');
 end;
end;

Function CreateNetConnectionsXML(AList : TStrings) : boolean;
var
 MSNetInfo      : TCustomMSNetInfo;
 NetSessionInfo : TNetSessionInfo;
 List           : TStrings;
 i              : integer;
begin
 AList.Clear;
 AList.Add('<NET_SESSIONS>');
 try
  if ISNT then
   MSNetInfo := TNTMSNetInfo.Create
    else MSNetInfo := T9xMSNetInfo.Create;
  List      := TStringList.Create;
  MSNetInfo.CreateSessionsList(List);
  for i := 0 to List.Count - 1 do begin
   NetSessionInfo := List.Objects[i] as TNetSessionInfo;
  AList.Add(' <SESSION '+
            'SessionName='''+NetSessionInfo.SessionName + ''' '+
            'UserName='''+NetSessionInfo.UserName + ''' '+
            'NumOpens='''+inttostr(NetSessionInfo.NumOpens) + ''' '+
            'Time='''+inttostr(NetSessionInfo.Time) + ''' '+
            'IdleTime='''+inttostr(NetSessionInfo.IdleTime) + ''' '+
            '/>');
 end;
 finally
  AList.Add('</NET_SESSIONS>');
 end;
end;

Function CreateNetOpenFilesXML(AList : TStrings) : boolean;
var
 MSNetInfo      : TCustomMSNetInfo;
 NetFileInfo    : TNetFileInfo;
 List           : TStrings;
 i              : integer;
begin
 AList.Clear;
 AList.Add('<NET_FILES>');
 try
  if ISNT then
   MSNetInfo := TNTMSNetInfo.Create
    else MSNetInfo := T9xMSNetInfo.Create;
  List      := TStringList.Create;
  MSNetInfo.CreateOpenResList(List);
  for i := 0 to List.Count - 1 do begin
   NetFileInfo := List.Objects[i] as TNetFileInfo;
   AList.Add(' <SESSION '+
             'PathName='''+NetFileInfo.PathName + ''' '+
             'UserName='''+NetFileInfo.UserName + ''' '+
             'ShareName='''+NetFileInfo.ShareName + ''' '+
             'Num_locks='''+IntToStr(NetFileInfo.Num_locks) + ''' '+
             'ID='''+IntToStr(NetFileInfo.ID) + ''' '+
             '/>');
   AList.Add(NetFileInfo.PathName + ' '+NetFileInfo.UserName+' '+inttostr(NetFileInfo.Num_locks))
  end;
 finally
  AList.Add('</NET_FILES>');
 end;
end;

Function CreateNICStatXML(AList : TStrings) : boolean;
var
 MIB_IFTABLE : PMIB_IFTABLE;
 MIB_IFROW   : TMIB_IFROW;
 i, Size, Res : dword;
begin
 AList.Clear;
 AList.Add('<NET_FILES>');
 try
  Size := 10000;
  GetMem(MIB_IFTABLE, Size);
  ZeroMemory(MIB_IFTABLE, Size);
  Res := GetIfTable(MIB_IFTABLE, @Size, false);
  for i := 0 to MIB_IFTABLE^.dwNumEntries - 1 do begin
   {$R-}
   MIB_IFROW := MIB_IFTABLE^.MIB_IFROW[i];
   AList.Add(' <ADAPTER '+
             'Name='''+Trim(MIB_IFROW.bDescr) + ''' '+
             'MAC='''+GetMAC(MIB_IFROW) + ''' '+
             'MTU='''+IntToStr(MIB_IFROW.dwMtu) + ''' '+
             'Speed='''+IntToStr(MIB_IFROW.dwSpeed) + ''' '+
             'Status='''+IntToStr(MIB_IFROW.dwAdminStatus) + ''' '+
             'InOctets='''+IntToStr(MIB_IFROW.dwInOctets) + ''' '+
             'InUcastPkts='''+IntToStr(MIB_IFROW.dwInUcastPkts) + ''' '+
             'InNUcastPkts='''+IntToStr(MIB_IFROW.dwInNUcastPkts) + ''' '+
             'InDiscards='''+IntToStr(MIB_IFROW.dwInDiscards) + ''' '+
             'InErrors='''+IntToStr(MIB_IFROW.dwInErrors) + ''' '+
             'InUnknownProtos='''+IntToStr(MIB_IFROW.dwInUnknownProtos) + ''' '+
             'OutOctets='''+IntToStr(MIB_IFROW.dwOutOctets) + ''' '+
             'OutUcastPkts='''+IntToStr(MIB_IFROW.dwOutUcastPkts) + ''' '+
             'OutNUcastPkts='''+IntToStr(MIB_IFROW.dwOutNUcastPkts) + ''' '+
             'OutDiscards='''+IntToStr(MIB_IFROW.dwOutDiscards) + ''' '+
             'OutErrors='''+IntToStr(MIB_IFROW.dwOutErrors) + ''' '+
             'OutQLen='''+IntToStr(MIB_IFROW.dwOutQLen) + ''' '+
             '/>');
   {$R+}
  end;
 finally
  AList.Add('</NET_FILES>');
 end;
end;

Function CreateTCPStatXML(AList : TStrings) : boolean;
var
 Res : integer;
 ST  : TMIB_TCPSTATS;
begin
 AList.Clear;
 Res := GetTcpStatistics(@ST);
 AList.Add('<TCP_STAT '+
             'RtoAlgorithm='''+IntToStr(ST.dwRtoAlgorithm) + ''' '+
             'RtoMin='''+IntToStr(ST.dwRtoMin) + ''' '+
             'RtoMax='''+IntToStr(ST.dwRtoMax) + ''' '+
             'MaxConn='''+IntToStr(ST.dwMaxConn) + ''' '+
             'ActiveOpens='''+IntToStr(ST.dwActiveOpens) + ''' '+
             'PassiveOpens='''+IntToStr(ST.dwPassiveOpens) + ''' '+
             'AttemptFails='''+IntToStr(ST.dwAttemptFails) + ''' '+
             'EstabResets='''+IntToStr(ST.dwEstabResets) + ''' '+
             'CurrEstab='''+IntToStr(ST.dwCurrEstab) + ''' '+
             'InSegs='''+IntToStr(ST.dwInSegs) + ''' '+
             'OutSegs='''+IntToStr(ST.dwOutSegs) + ''' '+
             'RetransSegs='''+IntToStr(ST.dwRetransSegs) + ''' '+
             'InErrs='''+IntToStr(ST.dwInErrs) + ''' '+
             'OutRsts='''+IntToStr(ST.dwOutRsts) + ''' '+
             'NumConns='''+IntToStr(ST.dwNumConns) + ''' '+
             '/>');
end;

Function CreateTCPSetupXML(AList : TStrings) : boolean;
var
 NetworkInfo : TNetworkInfo;
 NetAdater   : TNetAdapter;
 i           : integer;
 FI   : PFixedInfo;
 Size : Integer;
 Res  : Integer;
 DNS  : PIPAddrString;
begin
 AList.Clear;
 AList.Add('<NET_SETUP>');
 try
   NetworkInfo := TNetworkInfo.Create;
   if NetworkInfo.RefreshNetworkInfo then begin
    for i := 0 to NetworkInfo.AdaptersCount - 1 do begin
     NetAdater := NetworkInfo.Adapters[i];
     AList.Add(' <ADAPTER '+
               'Name='''+Trim(NetAdater.AdapterName) + ''' '+
               'Descr='''+Trim(NetAdater.Description) + ''' '+
               'MACAddress='''+NetAdater.MACAddress + ''' '+
               'IP='''+NetAdater.IPAddressList.CommaText + ''' '+
               'Gateway='''+NetAdater.GatewayList.CommaText + ''' '+
               'Primary_WINS='''+NetAdater.PrimaryWINSServer.AsString + ''' '+
               'Secondary_WINS='''+NetAdater.SecondaryWINSServer.AsString + ''' '+
               'DHCP='''+NetAdater.DHCPServer.AsString + ''' '+
               '/>');
    end;
   end;
 finally
  AList.Add('</NET_SETUP>');
 end;
end;

Function CreateDNSSetupXML(AList : TStrings) : boolean;
var
 Size : Integer;
 Res  : Integer;
 FI   : PFixedInfo;
 DNS  : PIPAddrString;
 CurrDNS, S    : string;
begin
 AList.Clear;
 Size := 1024;
 GetMem(FI,Size);
 Res := GetNetworkParams(FI,Size);
 if (Res <> ERROR_SUCCESS) Then exit;
 If (FI^.CurrentDNSServer <> nil) Then
  CurrDns := FI^.CurrentDNSServer^.IPAddress
   else CurrDns := '';
 S := '';
 DNS := @FI^.DNSServerList;
 Repeat
  if S <> '' then S := S+';';
  S := S + DNS^.IPAddress;
  DNS := DNS^.Next;
 Until (DNS = nil);
 AList.Add('<DNS '+
           'HostName='''+FI^.HostName + ''' '+
           'DomainName='''+FI^.DomainName + ''' '+
           'CurrentDNS='''+CurrDns + ''' '+
           'DNS='''+S + ''' '+
           'Scope ID='''+FI^.ScopeId + ''' '+
           'EnableRouting='''+IntToStr(FI^.EnableRouting) + ''' '+
           'EnableProxy='''+IntToStr(FI^.EnableProxy) + ''' '+
           'EnableDNS='''+IntToStr(FI^.EnableDNS) + ''' '+
          '/>');
 FreeMem(FI);
end;

// Порты TCP/IP
Function CreateTCPConnectXML(AList : TStrings) : boolean;
var
 Res : integer;
 i, Size : DWORD;
 MIB_TCPTABLE : PMIB_TCPTABLE;
 addr : TInAddr;
 s, LocalHost, RemoteHost : string;
begin
 AList.Clear;
 AList.Add('<TCP_CONNECT>');
 try
   Size := 10000;
   GetMem(MIB_TCPTABLE, Size);
   Res := GetTcpTable(MIB_TCPTABLE, @Size, true);
   for i := 0 to MIB_TCPTABLE^.dwNumEntries-1 do begin
    s := '';
    {$R-}
    case MIB_TCPTABLE^.MIB_TCPROW[i].dwState of
      0 : s := '???';
      1 : s := 'CLOSED';
      2 : s :=  'LISTENING';
      3 : s :=  'SYN_SENT';
      4 : s :=  'SEN_RECEIVED';
      5 : s :=  'ESTABLISHED';
      6 : s :=  'FIN_WAIT';
      7 : s :=  'FIN_WAIT2';
      8 : s :=  'CLOSE_WAIT';
      9 : s :=  'CLOSING';
      10 : s := 'LAST_ACK';
      11 : s := 'TIME_WAIT';
     end;
    addr.S_addr := MIB_TCPTABLE^.MIB_TCPROW[i].dwLocalAddr;
    LocalHost := String(inet_ntoa(addr));
    addr.S_addr := u_long(MIB_TCPTABLE^.MIB_TCPROW[i].dwRemoteAddr);
    RemoteHost := String(inet_ntoa(addr));
    AList.Add(' <CONN '+
             'LocalHost='''+ LocalHost + ''' '+
             'LocalPort='''+ IntToStr(htons(MIB_TCPTABLE^.MIB_TCPROW[i].dwLocalPort)) + ''' '+
             'RemoteHost='''+ RemoteHost + ''' '+
             'RemotePort='''+ IntToStr(htons(MIB_TCPTABLE^.MIB_TCPROW[i].dwRemotePort)) + ''' '+
             'State='''+ s + ''' '+
              '/>');
    {$R+}
   end;
   FreeMem(MIB_TCPTABLE, Size);
 finally
  AList.Add('</TCP_CONNECT>');
 end;
end;

// Порты UDP/IP
Function CreateUDPPortXML(AList : TStrings) : boolean;
var
 Res : integer;
 i, Size : DWORD;
 MIB_UDPTABLE : PMIB_UDPTABLE;
 addr : TInAddr;
 s : string;
begin
 AList.Clear;
 AList.Add('<PORT_LIST_UDP>');
 try
   Size := 10000;
   GetMem(MIB_UDPTABLE, Size);
   Res := GetUdpTable(MIB_UDPTABLE, @Size, true);
   for i := 0 to MIB_UDPTABLE^.dwNumEntries-1 do begin
    s := '';
    {$R-}
    addr.S_addr := MIB_UDPTABLE^.MIB_UDPROW[i].dwLocalAddr;
    s := String(inet_ntoa(addr));
    AList.Add(' <PORT '+
             'LocalHost='''+ s + ''' '+
             'LocalPort='''+ IntToStr(htons(MIB_UDPTABLE^.MIB_UDPROW[i].dwLocalPort)) + ''' '+
              '/>');
    {$R+}
   end;
   FreeMem(MIB_UDPTABLE, Size);
 finally
  AList.Add('</PORT_LIST_UDP>');
 end;
end;

// Автозагрузка
Function CreateAutorunXML(AList : TStrings) : boolean;
var
 Res : integer;
 i   : DWORD;
 s   : string;
 AutoRunInfo : TAutoRunInfo;
 AutoRunItem : TAutoRunItem;
 TmpList : TStrings;
begin
 AList.Clear;
 TmpList := TStringList.Create;
 AutoRunInfo := TAutoRunInfo.Create;
 AutoRunInfo.GetAutorunInfo(TmpList);
 AList.Add('<AUTORUN>');
 try
  for i := 0 to TmpList.Count - 1 do begin
   AutoRunItem := TmpList.Objects[i] as TAutoRunItem;
   AList.Add(' <ITEM '+
             'Type='''+ IntToStr(AutoRunItem.RecType) + ''' '+
             'Path='''+ AutoRunItem.Path + ''' '+
             'KeyName='''+ AutoRunItem.KeyName + ''' '+
             'PrgName='''+ AutoRunItem.PrgName + ''' '+
             '/>');
  end;
 finally
  AList.Add('</AUTORUN>');
 end;
end;

Function CreateAVPXML(AList : TStrings) : boolean;
var
 AVPBase : string;
 SR      : TSearchRec;
 Res     : integer;
begin
 AList.Clear;
 AVPBase := RegReadString(HKEY_LOCAL_MACHINE, 'SOFTWARE\KasperskyLab\Components\100', 'Folder');
 AList.Add('<AVP BasePath='''+AVPBase+'''/>');
 if (AVPBase <> '') and DirectoryExists(AVPBase) then begin
  Res := FindFirst(NormalDir(AVPBase)+'*.*', faAnyFile, SR);
  while res = 0 do begin
   if (SR.Attr and faDirectory) = 0 then
    AList.Add(' <FILE Name='''+SR.Name+''' '+
                 'Size='''+IntToStr(SR.Size)+''' '+
                 'Date='''+DateTimeToStr(FileDateToDateTime(SR.Time))+''' '+
                 '/>');
   Res := FindNext(SR);
  end;
  FindClose(SR);
 end;
 AList.Add('</AVP>');
end;

// Информация о файле
Function CreateFileInfoXML(AFileName : string; ProcessMonitor : TProcessMonitor; AList : TStrings) : boolean;
var
 SysInfo   : TSysInfo;
 PE        : TPEFileInfo;
 MD5Digest : TMD5Digest;
 VersionInfo : TVersionInfo;
begin
 SysInfo := TSysInfo.Create;
 AFileName := StringReplace(AFileName, '$WINDOWS$', SysInfo.WindowsHomePath, [rfReplaceAll, rfIgnoreCase]);
 AFileName := StringReplace(AFileName, '$SYSTEM$', SysInfo.WindowsSystemPath, [rfReplaceAll, rfIgnoreCase]);
 ProcessMonitor.SysProcessInfo.GetFileInfo(AFileName, PE);
 MD5Digest := MD5File(AFileName);
 // Определение версии файла
 VersionInfo := TVersionInfo.Create(AFileName);
 PE.FileVersion    := VersionInfo.FileVersion;
 PE.LegalCopyright := VersionInfo.LegalCopyright;
 VersionInfo.Free;
 AList.Clear;
 AList.Add('<FILE_INFO '+
           'Name=''' + PE.FileName + ''' '+
           'Size=''' + inttostr(PE.Size) + ''' '+
           'S1='''   + IntToHex(PE.Sign[1], 8) + ''' '+
           'S2='''   + IntToHex(PE.Sign[2], 8) + ''' '+
           'S3='''   + IntToHex(PE.Sign[3], 8) + ''' '+
           'S4='''   + IntToHex(PE.Sign[4], 8) + ''' '+
           'MS5='''+MD5DigestToStr(MD5Digest)+''' '+
           'Copyright='''+PE.LegalCopyright+''' '+
           'Ver='''+PE.FileVersion+''' '+
           '/>');
end;

end.

unit NetInfo;

interface

uses
  Windows, Messages, SysUtils, Classes, IPHelperDef;

type
  // IP адрес и маска
  TIPAddress = class
   protected
    FIPAddress : string;
    FIPNetmask : string;
  private
    function GetStringInfo: String;
   public
    constructor Create(Addr : PIPAddrString);
    property    AsString : string read GetStringInfo;
  end;

  // Описание сетевого адаптера
  TNetAdapter = class
   private
    FAdapterInfo : PIPAdapterInfo;
    FIPAddressList,
    FGatewayList    : TStrings;
    FAdapterName: string;
    FDescription: string;
    FMACAddress: string;
    FSecondaryWINSServer,
    FPrimaryWINSServer,
    FDHCPServer : TIPAddress;
    FIndex      : Cardinal;
    function MACToStr(ByteArr : PByte; Len : Integer) : String;
    function IpAddrtoStr(Addr : PIPAddrString) : String;
    procedure PopulateAddressList(Addr : PIPAddrString; List: TStrings);
    function GetCurrentIP: TIPAddress;
   public
    constructor Create(AdapterInfo:PIPAdapterInfo);
    function    RefreshAdapterInfo : boolean;
    property    AdapterName : string read FAdapterName;
    property    Description : string read FDescription;
    property    MACAddress  : string read FMACAddress;
    property    IPAddressList : TStrings read FIPAddressList;
    property    GatewayList   : TStrings read FGatewayList;
    property    SecondaryWINSServer : TIPAddress read FSecondaryWINSServer;
    property    PrimaryWINSServer : TIPAddress read FPrimaryWINSServer;
    property    DHCPServer : TIPAddress read FDHCPServer;
    property    AdapterIndex : cardinal read FIndex;
  end;

  // Информация о сети
  TNetworkInfo = class
  protected
   FNetAdapters : TStringList;
  private
    function GetAdapter(index: Integer): TNetAdapter;
    function GetAdaptersCount: integer;
  public
   constructor Create;
   // Обновление информации о сети
   Function RefreshNetworkInfo : boolean;
   // Массив адаптеров
   property Adapters[index: Integer]: TNetAdapter read GetAdapter;
   // Количество адаптеров
   property AdaptersCount : integer read GetAdaptersCount;
  end;

implementation


{ TNetworkInfo }
constructor TNetworkInfo.Create;
begin
 inherited;
 FNetAdapters := TStringList.Create;
end;

function TNetworkInfo.GetAdapter(index: Integer): TNetAdapter;
begin
 Result := nil;
 if (index >= 0) and (index < AdaptersCount) then
  Result := FNetAdapters.Objects[index] as TNetAdapter;
end;

function TNetworkInfo.GetAdaptersCount: integer;
begin
 Result := FNetAdapters.Count;
end;

function TNetworkInfo.RefreshNetworkInfo: boolean;
var
  AI, TekAI : PIPAdapterInfo;
  Size      : integer;
  Res       : integer;
  Adapter   : TNetAdapter;
begin
 Result := false;
 Size := 5120;
 GetMem(AI, Size);
 try
  // Запрос информации об адаптерах
  Res := GetAdaptersInfo(AI, Size);
  // Проверка, не было ли ошибок. Если ошибки есть, то проблемы с загрузкой библиотеки или нет сети
  if (Res <> ERROR_SUCCESS) then
   Exit;
  TekAI := AI;
  repeat
   adapter := TNetAdapter.Create(TekAI);
   FNetAdapters.AddObject('', adapter);
   TekAI := TekAI^.Next;
  until TekAI = nil;
 finally
  FreeMem(AI);
 end;
 Result := true;
end;

{ TAdapter }

constructor TNetAdapter.Create(AdapterInfo: PIPAdapterInfo);
begin
 FAdapterInfo := AdapterInfo;
 FIPAddressList := TStringList.Create;
 FGatewayList   := TStringList.Create;
 RefreshAdapterInfo;
end;

function TNetAdapter.GetCurrentIP: TIPAddress;
begin
 ;
end;

function TNetAdapter.IpAddrtoStr(Addr: PIPAddrString): String;
begin
 Result := '';
 if Addr <> nil then
  Result := Addr^.IPAddress + '/' + Addr^.IPMask;
end;

function TNetAdapter.MACToStr(ByteArr: PByte; Len: Integer): String;
begin
 Result := '';
  while (Len > 0) do begin
   if Result <> '' then
    Result  := Result + ':';
   Result  := Result + IntToHex(ByteArr^,2);
   ByteArr := Pointer(Integer(ByteArr)+SizeOf(Byte));
   Dec(Len);
  end;
end;

procedure TNetAdapter.PopulateAddressList(Addr: PIPAddrString;
  List: TStrings);
begin
 List.Clear;
 while (Addr <> nil) do begin
  List.Add(Addr^.IPAddress + '/' + Addr^.IPMask);
  Addr := Addr^.Next;
 End;
end;

function TNetAdapter.RefreshAdapterInfo: boolean;
begin
 Result := false;
 FAdapterName := FAdapterInfo^.AdapterName;
 FDescription := FAdapterInfo^.Description;
 FMACAddress  :=MACToStr(@FAdapterInfo^.Address, FAdapterInfo^.AddressLength);
 PopulateAddressList(@fadapterInfo^.IPAddressList,FIPAddressList);
 PopulateAddressList(@fadapterInfo^.GatewayList,FGatewayList);

 FDHCPServer          := TIPAddress.Create(@fadapterInfo.DHCPServer);
 FPrimaryWINSServer   := TIPAddress.Create(@fadapterInfo.PrimaryWINSServer);
 FSecondaryWINSServer := TIPAddress.Create(@fadapterInfo.SecondaryWINSServer);
 FIndex               := fadapterInfo^.Index;
{
 if adapterInfo^.DHCPEnabled<>0 then FDHCPEnabled:=True else FDHCPEnabled:=False;
 if adapterInfo^.HaveWINS then FHaveWINS:=True else FHaveWINS:=False;
 }
end;

{ TIPAddress }

constructor TIPAddress.Create(Addr: PIPAddrString);
begin
  if Addr<>nil then begin
    FIPAddress := Addr^.IPAddress;
    FIPNetmask := Addr^.IPMask;
  end
  else begin
    FIPAddress := '';
    FIPNetmask := '';
  end;
end;

function TIPAddress.getStringInfo: string;
begin
 Result := FIPAddress + '/' + FIPNetmask;
end;

end.

unit FileLoader;
// Модуль загрузки файла. Зайцев О.В., 2005-2006
// Последняя модификация: 17.10.2006

interface
type
 // Событие "прогресс" при копировании
 TFileCopyProgress = procedure (Sender : TObject; ASize, AFullSize : integer) of object;
 // Режим загрузки (по настройкам IE, прямое соединение, прокси без авторизации и с авторизацией)
 THTTPDownloadMode = (hdmIESettings, hdmDirectConnection, hdmProxy, hdmProxyAuth, hdmProxyAuthNTLM);
 // Базовый класс загрузчика
 TCustomFileLoader = class
  private
    FLogin: string;
    FPasswd: string;
    FFileCopyProgress: TFileCopyProgress;
    procedure SetLogin(const Value: string);
    procedure SetPasswd(const Value: string);
    procedure SetFileCopyProgress(const Value: TFileCopyProgress);
  public
   LoadStep : integer;
   GetLastErrorCode : Longword;
   constructor Create;
   // Загрузка файла (AFileName - имя файла, ASaveName - имя сохраняемой копии)
   function LoadFile(AFileName, ASaveName : string; AFileSize : integer = 0) : boolean; virtual; abstract;
  published
   property OnFileCopyProgress : TFileCopyProgress read FFileCopyProgress write SetFileCopyProgress;
 end;

 // Загрузчик по HTTP протоколу
 THTTPFileLoader = class(TCustomFileLoader)
  private
    FProxyServerPasswd: string;
    FProxyServer: string;
    FProxyServerLogin: string;
    FDownloadMode: THTTPDownloadMode;
    procedure SetDownloadMode(const Value: THTTPDownloadMode);
    procedure SetProxyServer(const Value: string);
    procedure SetProxyServerLogin(const Value: string);
    procedure SetProxyServerPasswd(const Value: string);
    function LoadFile(AFileName, ASaveName : string; AFileSize : integer = 0) : boolean; override;
  public
   constructor Create;
  published
   property DownloadMode      : THTTPDownloadMode read FDownloadMode write SetDownloadMode;
   property ProxyServer       : string read FProxyServer write SetProxyServer;
   property ProxyServerLogin  : string read FProxyServerLogin write SetProxyServerLogin;
   property ProxyServerPasswd : string read FProxyServerPasswd write SetProxyServerPasswd;
 end;

implementation
uses wininet, windows, sysutils;
{ THTTPFileLoader }

constructor THTTPFileLoader.Create;
begin
 inherited;
 FDownloadMode := hdmIESettings;
 FProxyServer       := '';
 FProxyServerLogin  := '';
 FProxyServerPasswd := '';
end;

function THTTPFileLoader.LoadFile(AFileName, ASaveName: string; AFileSize : integer = 0): boolean;
const
 BufferSize = 2048;
var
 hSession, hURL : HInternet;
 Buffer         : array[1..BufferSize] of Byte;
 BufferLen      : DWORD;
 f              : File;
 sAppName, S    : string;
 dwStatus, dwStatusSize, lpdwReserved : DWORD;
 Res : boolean;
begin
 Result := false;
 LoadStep := 0; GetLastErrorCode := 0;
 // Удаление файла
 DeleteFile(ASaveName);
 sAppName := ExtractFileName(ParamStr(0));
 if Assigned(OnFileCopyProgress) then
  OnFileCopyProgress(Self, 0, AFileSize);
 LoadStep := 1;
 // Открытие соединения (в обход прокси !!)
 case DownloadMode of
  hdmIESettings       :  hSession := InternetOpen(PChar(sAppName), INTERNET_OPEN_TYPE_PRECONFIG,  nil, nil, 0);
  hdmDirectConnection :  hSession := InternetOpen(PChar(sAppName), INTERNET_OPEN_TYPE_DIRECT,  nil, nil, 0);
  hdmProxy            :  hSession := InternetOpen(PChar(sAppName), INTERNET_OPEN_TYPE_PROXY,  PChar(FProxyServer), nil, 0);
  hdmProxyAuth        :  begin
                          hSession := InternetOpen(PChar(sAppName), INTERNET_OPEN_TYPE_PROXY,  PChar(FProxyServer), nil, 0);
                         end;
  hdmProxyAuthNTLM    :  begin
                          hSession := InternetOpen(PChar(sAppName), INTERNET_OPEN_TYPE_PROXY,  PChar(FProxyServer), nil, 0);
                         end;
 end;
 if hSession <> nil then
 try
  LoadStep := 2;
  hURL := nil;
  case DownloadMode of
   hdmProxyAuthNTLM : begin
      // Для NTLM важно поддерживать коннекцию
      hURL := InternetOpenURL(hSession, PChar(AFileName),nil,0,INTERNET_FLAG_PRAGMA_NOCACHE or INTERNET_FLAG_RELOAD or INTERNET_FLAG_NO_UI or INTERNET_FLAG_NO_COOKIES or INTERNET_FLAG_NO_CACHE_WRITE or INTERNET_FLAG_KEEP_CONNECTION, 0);
     end;
   else begin
    // Открытие URL
    hURL := InternetOpenURL(hSession, PChar(AFileName),nil,0,INTERNET_FLAG_PRAGMA_NOCACHE or INTERNET_FLAG_RELOAD or INTERNET_FLAG_NO_UI or INTERNET_FLAG_NO_COOKIES or INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if hURL = nil then begin
     LoadStep := 21;
     GetLastErrorCode := GetLastError;
     hURL := InternetOpenURL(hSession, PChar(AFileName),nil,0,INTERNET_FLAG_RELOAD, 0);
    end;
    if hURL = nil then begin
     GetLastErrorCode := GetLastError;
     exit;
    end;
    end;
  end;
  LoadStep := 3;
  // URL открыт, проверим, нужна ли авторизация
  if DownloadMode = hdmProxyAuth  then begin
   LoadStep := 31;
   dwStatusSize := sizeof(DWORD);
   dwStatus := 0; lpdwReserved := 0;
   HttpQueryInfo(hURL, HTTP_QUERY_FLAG_NUMBER or HTTP_QUERY_STATUS_CODE, @dwStatus, dwStatusSize, lpdwReserved);
   LoadStep := 32;
   if dwStatus = HTTP_STATUS_PROXY_AUTH_REQ then begin
    LoadStep := 33;
     // Имя пользователя
     if ProxyServerLogin <> '' then begin
      S := ProxyServerLogin + #0;
      Res := InternetSetOption(hURL, INTERNET_OPTION_PROXY_USERNAME, PChar(S), length(S));
     end;
    // Пароль
    if ProxyServerPasswd <> '' then begin
     S := ProxyServerPasswd+#0;
     InternetSetOption(hURL, INTERNET_OPTION_PROXY_PASSWORD, PChar(S), length(S));
    end;
    HttpSendRequest(hURL, nil, 0, nil, 0);
   end;
  end;
  if DownloadMode = hdmProxyAuthNTLM  then begin
   LoadStep := 35;
   dwStatusSize := sizeof(DWORD);
   dwStatus := 0; lpdwReserved := 0;
   HttpQueryInfo(hURL, HTTP_QUERY_FLAG_NUMBER or HTTP_QUERY_STATUS_CODE, @dwStatus, dwStatusSize, lpdwReserved);
   LoadStep := 36;
   if dwStatus = HTTP_STATUS_PROXY_AUTH_REQ then begin
    LoadStep := 37;
     // Имя пользователя
     if ProxyServerLogin <> '' then begin
      S := ProxyServerLogin + #0;
      Res := InternetSetOption(hURL, INTERNET_OPTION_PROXY_USERNAME, PChar(S), length(S));
     end;
    // Пароль
    if ProxyServerPasswd <> '' then begin
     S := ProxyServerPasswd+#0;
     InternetSetOption(hURL, INTERNET_OPTION_PROXY_PASSWORD, PChar(S), length(S));
    end;
    HttpSendRequest(hURL, nil, 0, nil, 0);
   end;
   if dwStatus = HTTP_STATUS_DENIED then begin
    LoadStep := 38;
     // Имя пользователя
     if ProxyServerLogin <> '' then begin
      S := ProxyServerLogin + #0;
      Res := InternetSetOption(hURL, INTERNET_OPTION_USERNAME, PChar(S), length(S));
     end;
    // Пароль
    if ProxyServerPasswd <> '' then begin
     S := ProxyServerPasswd+#0;
     InternetSetOption(hURL, INTERNET_OPTION_PASSWORD, PChar(S), length(S));
    end;
    HttpSendRequest(hURL, nil, 0, nil, 0);
   end;
  end;
  try
   AssignFile(f, ASaveName);
   Rewrite(f,1);
   repeat
    LoadStep := 4;
    // Чтение очередного фрагмента в буфер
    InternetReadFile(hURL, @Buffer, SizeOf(Buffer), BufferLen);
    // Запись принятых данных в файл
    BlockWrite(f, Buffer, BufferLen);
    if Assigned(OnFileCopyProgress) then
     OnFileCopyProgress(Self, filepos(f), AFileSize);
   until BufferLen = 0;
   CloseFile(f);
   Result := true;
  finally
   InternetCloseHandle(hURL)
  end;
 finally
  InternetCloseHandle(hSession)
 end else
  GetLastErrorCode := GetLastError;
end;

procedure THTTPFileLoader.SetDownloadMode(const Value: THTTPDownloadMode);
begin
  FDownloadMode := Value;
end;

procedure THTTPFileLoader.SetProxyServer(const Value: string);
begin
  FProxyServer := Value;
end;

procedure THTTPFileLoader.SetProxyServerLogin(const Value: string);
begin
  FProxyServerLogin := Value;
end;

procedure THTTPFileLoader.SetProxyServerPasswd(const Value: string);
begin
  FProxyServerPasswd := Value;
end;

{ TCustomFileLoader }

constructor TCustomFileLoader.Create;
begin
 OnFileCopyProgress := nil;
end;

procedure TCustomFileLoader.SetFileCopyProgress(
  const Value: TFileCopyProgress);
begin
  FFileCopyProgress := Value;
end;

procedure TCustomFileLoader.SetLogin(const Value: string);
begin
  FLogin := Value;
end;

procedure TCustomFileLoader.SetPasswd(const Value: string);
begin
  FPasswd := Value;
end;

end.

unit zUnRar;

interface
uses Windows, UnRAR, SysUtils;

type
 TUnrarArchive = class
 protected
  hArcData: THandle;
 public
  FFileName : string;
  constructor Create(AFileName : string);
  destructor Destroy; override;
  // Открытие архива
  function OpenArchive : boolean;
  // Закрытие архива
  function CloseArchive : boolean;
  // Чтение очередного файла
  function ReadHeader(var HeaderData: RARHeaderDataEx) : integer;
  function SkipFile : integer;
  function ExtractFile(AFileName : string) : integer;
 end;
implementation

{ TUnrarArchive }

function TUnrarArchive.CloseArchive: boolean;
begin
 if hArcData > 0 then begin
  RARCloseArchive(hArcData);
  hArcData := 0;
 end;
 Result := true;
end;

constructor TUnrarArchive.Create(AFileName: string);
begin
 FFileName := AFileName;
 hArcData  := 0;
end;

destructor TUnrarArchive.Destroy;
begin
 CloseArchive;
 inherited;
end;

function TUnrarArchive.ExtractFile(AFileName: string): integer;
begin
 Result := RARProcessFile(hArcData, RAR_EXTRACT, nil, pchar(AFileName+#0));
end;

function TUnrarArchive.OpenArchive: boolean;
var
  CmtBuf : array[0..Pred(16384)] of Char;
  OpenArchiveData: RAROpenArchiveDataEx;
  S : WideString;
begin
  Result := false;
  // Проверка, загружена ли библиотека
  if @RAROpenArchiveEx = nil then exit;
  CloseArchive;
  ZeroMemory(@OpenArchiveData, sizeof(OpenArchiveData));
  OpenArchiveData.ArcName    := PChar(FFilename);
  OpenArchiveData.CmtBuf     := @CmtBuf;
  OpenArchiveData.CmtBufSize := SizeOf(CmtBuf);
  OpenArchiveData.OpenMode   := RAR_OM_EXTRACT;
  hArcData := RAROpenArchiveEx(OpenArchiveData);
  Result := (hArcData <> 0) and (OpenArchiveData.OpenResult = 0);
end;

function TUnrarArchive.ReadHeader(var HeaderData: RARHeaderDataEx): integer;
begin
 Result := -1;
 if hArcData = 0 then exit;
 Result := RARReadHeaderEx(hArcData, HeaderData);
end;

function TUnrarArchive.SkipFile: integer;
begin
 Result := RARProcessFile(hArcData, RAR_SKIP, nil, nil);
end;

end.
 

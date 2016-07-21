unit zEmailUnpacker;

interface
uses windows, Classes, SysUtils, zBase64, zUUE;
type
  TMailPart = record
   Header   : string;
   Body     : string;
   Encoding : string;
   FileName : string;
  end;
  TEmailParser = class
  private
    FFileName: string;
    FTekLine : integer;
    EmailLines, HeaderLines : TStringList;
    procedure SetFileName(const Value: string);
    function GetBoundary(S: string): string;
    function GetHeaderField(AHeaderLines: TStrings;
      AField: string): string;
    function GetNextString: string;
    function IsEof: boolean;
    function ReadHeader(ALines: TStrings): boolean;
    function GetPartFileName(ALines: TStrings): string;
    function ScanSection(ABoundary: string): boolean;
    function GetPartsCount: integer;
  public
    MailParts : array of TMailPart;
    constructor Create;
    Function ParseEmail : boolean;
    Function ExtractToStream(MailPart : TMailPart; FS : TStream) : boolean;
  published
   property FileName : string read FFileName write SetFileName;
   property PartsCount : integer read GetPartsCount;
  end;

implementation

{ TEmailUnpacker }
constructor TEmailParser.Create;
begin
 EmailLines  := TStringList.Create;
 HeaderLines := TStringList.Create;
 FTekLine := 0;
end;

function TEmailParser.ParseEmail: boolean;
var
 S : string;
 Tmp                        : TMailPart;
begin
 Result := false;
 EmailLines.Clear;
 HeaderLines.Clear;
 MailParts := nil;
 EmailLines.LoadFromFile(FFileName);
 // Чтение заголовка
 FTekLine := 0;
 ReadHeader(HeaderLines);
 S := GetHeaderField(HeaderLines, 'Content-Type:');
 if pos('multipart', S) = 1 then
  ScanSection(GetBoundary(S)) else begin
   S := '';
   while not(IsEof) do begin
    S := S + GetNextString;
   end;
   Tmp.Header := HeaderLines.Text;
   Tmp.Body   := S;
   Tmp.Encoding := LowerCase(Trim(GetHeaderField(HeaderLines, 'Content-Transfer-Encoding:')));
   Tmp.FileName := GetPartFileName(HeaderLines);
   if Tmp.FileName = '' then
    Tmp.FileName := 'MainBody';
   SetLength(MailParts, Length(MailParts) + 1);
   MailParts[Length(MailParts) - 1] := Tmp;
  end;
 EmailLines.Clear;
 Result := true;
end;

procedure TEmailParser.SetFileName(const Value: string);
begin
  FFileName := Value;
end;

// Считывание очередной строки
Function TEmailParser.GetNextString : string;
begin
 if FTekLine < EmailLines.Count then begin
  Result := EmailLines[FTekLine];
  inc(FTekLine);
 end else Result := '';
end;

// Получение признака конца файла
Function TEmailParser.IsEof : boolean;
begin
 Result := not(FTekLine < EmailLines.Count);
end;

// Считывание поля по его имени
Function TEmailParser.GetHeaderField(AHeaderLines : TStrings; AField : string) : string;
var
 i : integer;
begin
 Result := '';
 AField := LowerCase(AField);
 for i := 0 to AHeaderLines.Count-1 do
  if Pos(AField, LowerCase(AHeaderLines[i])) = 1 then begin
   Result := trim(copy(AHeaderLines[i], length(AField)+1, length(AHeaderLines[i])));
   exit;
  end;
end;

// Чтение заголовка
Function TEmailParser.ReadHeader(ALines : TStrings) : boolean;
var
 S             : string;
begin
 ALines.Clear;
 while not(IsEof) do begin
  S := GetNextString;
  if S = '' then begin
   Break;
  end else begin
   if ((S[1] in [' ', #9]) and (ALines.Count > 0)) then ALines[ALines.Count-1] := ALines[ALines.Count-1] + Trim(S)
    else ALines.Add(S);
  end;
 end;
 Result := (HeaderLines.Count > 0) and not(IsEof);
end;

// Поиск Boundary в параметрах Content-Type:
function TEmailParser.GetBoundary(S : string) : string;
begin
 Result := '';
 S := trim(S);
 if S = '' then exit;
 if pos('boundary', S) = 0 then exit;
 S := Trim(copy(S, pos('boundary', S)+8, 500));
 if pos('=', S) <> 1 then exit;
 Delete(S, 1, 1);
 if pos(';', S) > 0 then
  Delete(S, pos(';', S), Length(S));
 S := Trim(S);
 if S = '' then exit;
 if copy(S, 1, 1) = '"' then delete(S, 1, 1);
 if copy(S, length(S), 1) = '"' then delete(S, Length(S), 1);
 Result := S;
end;

// Сканироване секции
Function TEmailParser.ScanSection(ABoundary : string) : boolean;
var
 SectionHeader, SectionData : TStringList;
 S                          : string;
 RecordLinesF               : boolean;
 Tmp                        : TMailPart;
begin
 SectionHeader := TStringList.Create;
 SectionData   := TStringList.Create;
 RecordLinesF  := false;
 try
  while not(IsEof) do begin
   // Поиск начала секции
   while not(IsEof) do begin
    S := GetNextString;
    // Нашли начало секции
    if  S = '--' + ABoundary then break;
    // Нашли конец секции
    if  S = '--' + ABoundary + '--' then break;
    if RecordLinesF then
     SectionData.Add(S);
   end;
   if RecordLinesF then begin
    Tmp.Header := SectionHeader.Text;
    Tmp.Body   := SectionData.Text;
    Tmp.Encoding := LowerCase(Trim(GetHeaderField(SectionHeader, 'Content-Transfer-Encoding:')));
    Tmp.FileName := GetPartFileName(SectionHeader);
    SetLength(MailParts, Length(MailParts) + 1);
    MailParts[Length(MailParts) - 1] := Tmp;
    RecordLinesF := false;
   end;
   SectionData.Clear;
   SectionHeader.Clear;
   // Это разделитель секции с признаком конца секции ?
   if  S = '--' + ABoundary + '--' then exit;
   if IsEof then exit;
   // Чтение заголовка секции
   if not(ReadHeader(SectionHeader)) then exit;
   S := Trim(GetHeaderField(SectionHeader, 'Content-Type:'));
   if pos('multipart', S) = 1 then
    ScanSection(GetBoundary(S)) else begin
     SectionData.Clear;
     RecordLinesF := true;
    end;
  end;
 finally
  SectionHeader.Free;
  SectionData.Free;
 end;
end;

function TEmailParser.ExtractToStream(MailPart: TMailPart;
  FS: TStream): boolean;
var
 i, BufSize : integer;
 b : byte;
 S : string;
begin
 // Без кодирования
 if (MailPart.Encoding = '') or (MailPart.Encoding = '7bit') or (MailPart.Encoding = '8bit') or (MailPart.Encoding = 'binary') then begin
  if length(MailPart.Body) > 0 then
   FS.Write(MailPart.Body[1], length(MailPart.Body));
  Result := true;
  exit;
 end;
 // Кодирование quoted-printable
 if MailPart.Encoding = 'quoted-printable' then begin
  i := 1;
  BufSize := length(MailPart.Body);
  while (BufSize > 0) and (byte(MailPart.Body[BufSize]) in [9, 32, 10, 13]) do
   dec(BufSize);
  while i <= BufSize do begin
   if MailPart.Body[i] <> '=' then begin
    FS.Write(MailPart.Body[i], 1);
    inc(i);
   end else begin
    if i+2 <= BufSize then begin
     if not((MailPart.Body[i+1] = #$0D) and (MailPart.Body[i+2] = #$0A)) then begin
      b := StrToIntDef('$'+MailPart.Body[i+1]+MailPart.Body[i+2], 0);
      FS.Write(b, 1);
     end;
     inc(i, 3);
    end else i := BufSize+1;
   end;
  end;
 end;
 // Кодирование Base64
 if MailPart.Encoding = 'base64' then begin
  S := DeCodeStringBase64Ex(MailPart.Body);
  FS.Write(S[1], length(S));
  S := '';
 end;
 // Кодирование x-uue
 if MailPart.Encoding = 'x-uue' then begin
  S := DeCodeStringUUEEx(MailPart.Body);
  FS.Write(S[1], length(S));
  S := '';
 end;
end;

function TEmailParser.GetPartsCount: integer;
begin
 Result := length(MailParts);
end;

function TEmailParser.GetPartFileName(ALines: TStrings): string;
var
 S : string;
begin
 Result := '';
 S := GetHeaderField(ALines, 'Content-Disposition:');
 if pos('filename=', S) > 0 then begin
  // Выделение имени файла
  Result := Trim(copy(S, pos('filename=', S)+8, length(S)));
  if pos('=', Result) > 0 then Delete(Result, 1, pos('=', Result));
  Result := Trim(Result);
  // Удаление кавычек
  if copy(Result, 1, 1) = '"' then Delete(Result, 1, 1);
  if copy(Result, length(Result), 1) = '"' then Delete(Result, length(Result), 1);
 end;
end;

begin
end.

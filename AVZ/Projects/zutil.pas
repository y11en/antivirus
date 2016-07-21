unit zutil;

interface
uses Windows, SysUtils, ShellAPI, Classes, Masks;
 const
 // Массив для перекодировки W-1251 --> DOS
 DOSDecode : array[0..$FF] of byte = (
  $00,$01,$02,$03,$04,$05,$06,$07,$08,$09,$0a,$0b,$0c,$0d,$0e,$0f,$10,$11,$12,$13,$14,
  $15,$16,$17,$18,$19,$1a,$1b,$1c,$1d,$1e,$1f,$20,$21,$22,$23,$24,$25,$26,$27,$28,$29,$2a,$2b,$2c,$2d,
  $2e,$2f,$30,$31,$32,$33,$34,$35,$36,$37,$38,$39,$3a,$3b,$3c,$3d,$3e,$3f,$40,$41,$42,$43,$44,$45,$46,
  $47,$48,$49,$4a,$4b,$4c,$4d,$4e,$4f,$50,$51,$52,$53,$54,$55,$56,$57,$58,$59,$5a,$5b,$5c,$5d,$5e,$5f,
  $60,$61,$62,$63,$64,$65,$66,$67,$68,$69,$6a,$6b,$6c,$6d,$6e,$6f,$70,$71,$72,$73,$74,$75,$76,$77,$78,
  $79,$7a,$7b,$7c,$7d,$7e,$7f,$3f,$3f,$27,$3f,$22,$3a,$c5,$d8,$3f,$25,$3f,$3c,$3f,$3f,$3f,$3f,$3f,$27,
  $27,$22,$22,$07,$2d,$2d,$3f,$54,$3f,$3e,$3f,$3f,$3f,$3f,$ff,$f6,$f7,$3f,$fd,$3f,$b3,$15,$f0,$63,$f2,
  $3c,$bf,$2d,$52,$f4,$f8,$2b,$49,$69,$3f,$e7,$14,$fa,$f1,$fc,$f3,$3e,$3f,$3f,$3f,$f5,$80,$81,$82,$83,
  $84,$85,$86,$87,$88,$89,$8a,$8b,$8c,$8d,$8e,$8f,$90,$91,$92,$93,$94,$95,$96,$97,$98,$99,$9a,$9b,$9c,
  $9d,$9e,$9f,$a0,$a1,$a2,$a3,$a4,$a5,$a6,$a7,$a8,$a9,$aa,$ab,$ac,$ad,$ae,$af,$e0,$e1,$e2,$e3,$e4,$e5,
  $e6,$e7,$e8,$e9,$ea,$eb,$ec,$ed,$ee,$ef);

 MonthsNames : array[1..12] of String = ('январь', 'февраль','март','апрель',
                                         'май','июнь','июль','август','сентябрь',
                                         'октябрь','ноябрь','декабрь');
// Перекодировка строки WIN-1251 --> DOS
Function DecodeToDos(s:string):string;

// Нормализовать путь
function NormalDir(const DirName: string): string;

// Выделить имя директории без '\' в хвосте
function NormalNameDir(const DirName: string): string;

// Получить название месяца
Function GetMonthName(N:integer):string;

// Выполнить файл
function ExecuteFile(const FileName, Params, DefaultDir: string;
  ShowCmd: Integer): THandle;

// Получить дату и время файла по имени
function GetFileDate(AFileName : string) : TDateTime;

// Поиск и замена одного фрагмента на другой
Function Replace(ASrc, AOld : string; ANew : String = ''):String;

// Установить параметры окружения системы
Procedure SetSystemEnv;

// Получить путь к системным папкам
Function GetSystemDirectoryPath : string;   // System
Function GetWindowsDirectoryPath : string;  // Windows
function GetTempDirectoryPath : string;     // Temp

// Преобразование имени раздела реестра в его код. Возврат 0 - ошибка преобразования
function StrToRootKey(ARootKey : string) : HKEY;
// Преобразование кода раздела реестра в имя. Возврат '' - ошибка преобразования
function RootKeyToStr(ARootKey : HKEY) : string;

// Преобразование милисекунд в строку стандартного вида HH24:MI:SS[.MS] (отображение .MS зависит от AShowMs)
function MilliSecToTimeStr(MSec : integer; AShowMs : boolean = false) : string;

// Проверить, совпадает ли файл с маской
function MatchesMask(const Filename: string; AMackList : TStringList): Boolean;

function zForceDirectories(Dir: string): Boolean;

implementation

// Поиск и замена одного фрагмента на другой
Function Replace(ASrc, AOld : string; ANew : String = ''):String;
var
 Poz : integer;
begin
 Result := ASrc;
 // Если образец пуст, или он равен новому значению,
 // или он не содержится в строке, то замена бессмыслена
 if (AOld = '') or (AOld = ASrc) or (Pos(AOld, ASrc) = 0) then exit;
 // Образец содержится хотя бы один раз
 Result := '';
 Repeat
  Poz := Pos(AOld, ASrc);
  if Poz > 0 then begin
   Result := Result + Copy(ASrc, 1, Poz - 1) + ANew;
   Delete(ASrc, 1, Poz + Length(AOld)-1);
  end else Result := Result + ASrc;
  until Poz = 0;
end;

// Получить название месяца по его номеру
Function GetMonthName(N:integer):string;
begin
 Result := '???';
 if N in [1..12] then
  Result := MonthsNames[N];
end;

// Нормализовать путь
function NormalDir(const DirName: string): string;
begin
 Result := DirName;
 // Заменяем все прямые слеши на обратные
 while pos('/',Result)>0 do
  Result[pos('/',Result)] := '\';
 // Создание последнего \
 if (Result <> '') and not (Result[Length(Result)] in ['\']) then
  if (Length(Result) = 1) and (UpCase(Result[1]) in ['A'..'Z']) then
    Result := Result + ':\'    // Буква диска
  else Result := Result + '\'; // Путь
 // Уничтожение \\, возникающих в процессе конкантенации путей
 if copy(Result, 1, 2) = '\\' then
  Result := '\'+Replace(Result, '\\', '\')
   else Result := Replace(Result, '\\', '\');
end;

// Нормализовать путь (но оставить только имя без слеша в хвосте)
function NormalNameDir(const DirName: string): string;
begin
 Result := NormalDir(DirName);
 while copy(Result, length(result), 1) = '\' do
  Delete(Result, length(result), 1);
 while copy(Result, length(result), 1) = '/' do
  Delete(Result, length(result), 1);
end;

// Перекодировка строки WIN-1251 --> DOS
Function DecodeToDos(s:string):string;
var
 i : integer;
begin
 Result := '';
 For i:=1 to Length(s) do
  Result := Result + Char(DOSDecode[Ord(s[i])]);
end;

// Выполнить файл
function ExecuteFile(const FileName, Params, DefaultDir: string;
  ShowCmd: Integer): THandle;
begin
  Result := ShellExecute(0,
                         nil,
                         PChar(FileName),
                         PChar(Params),
                         PChar(DefaultDir),
                         ShowCmd);
end;

// Получить дату и время файла по имени
function GetFileDate(AFileName : string) : TDateTime;
var
 FAge : Longint;
begin
  FAge := FileAge(AFileName);
  if FAge = -1 then
    Result := 0
  else
    Result := FileDateToDateTime(FAge);
end;

// Установить параметры окружения системы
Procedure SetSystemEnv;
begin
 CurrencyString    := 'р.';
 ThousandSeparator := ' ';
 DecimalSeparator  := ',';
 ShortDateFormat   := 'dd.MM.yyyy';
 LongDateFormat    := 'd MMMM yyyy г.';
 DateSeparator     := '.';
 TimeSeparator     := ':';
 ShortTimeFormat   := 'h:mm';
 LongTimeFormat    := 'h:mm:ss';
 CurrencyDecimals  := 2;  // Образец "0,00"
 CurrencyFormat	   := 3;  // Образец "1,00 р."
 NegCurrFormat	   := 8;  // Образец "-1,00 р."
 ListSeparator     := ';';

 TwoDigitYearCenturyWindow := 50;
end;

// Получить путь к системным папкам
Function GetSystemDirectoryPath : string;
begin
 SetLength(Result, 300);
 Result := NormalDir(copy(Result, 1, GetSystemDirectory(@Result[1], length(Result))));
end;

Function GetWindowsDirectoryPath : string;
begin
 SetLength(Result, 300);
 Result := NormalDir(copy(Result, 1, GetWindowsDirectory(@Result[1], length(Result))));
end;

// Преобразование имени раздела реестра в его код
function StrToRootKey(ARootKey : string) : HKEY;
begin
 Result := 0;
 ARootKey := Trim(UpperCase(ARootKey));
 if (ARootKey = 'HKEY_LOCAL_MACHINE') or (ARootKey = 'HKLM') then
  Result := HKEY_LOCAL_MACHINE;
 if (ARootKey = 'HKEY_CLASSES_ROOT')  or (ARootKey = 'HKCR') then
  Result := HKEY_CLASSES_ROOT;
 if (ARootKey = 'HKEY_CURRENT_USER')  or (ARootKey = 'HKCU') then
  Result := HKEY_CURRENT_USER;
 if (ARootKey = 'HKEY_CURRENT_CONFIG') or (ARootKey = 'HKCC') then
  Result := HKEY_CURRENT_CONFIG;
 if ARootKey = 'HKEY_USERS' then
  Result := HKEY_USERS;
 if ARootKey = 'HKEY_PERFORMANCE_DATA' then
  Result := HKEY_PERFORMANCE_DATA;
 if ARootKey = 'HKEY_DYN_DATA' then
  Result := HKEY_DYN_DATA;
end;

// Преобразование имени раздела реестра в его код
function RootKeyToStr(ARootKey : HKEY) : string;
begin
 Result :=  '';
 if (ARootKey = HKEY_CLASSES_ROOT) then
  Result := 'HKEY_CLASSES_ROOT';
 if (ARootKey = HKEY_CURRENT_USER) then
  Result := 'HKEY_CURRENT_USER';
 if (ARootKey = HKEY_LOCAL_MACHINE) then
  Result := 'HKEY_LOCAL_MACHINE';
 if (ARootKey = HKEY_USERS) then
  Result := 'HKEY_USERS';
 if (ARootKey = HKEY_PERFORMANCE_DATA) then
  Result := 'HKEY_PERFORMANCE_DATA';
 if (ARootKey = HKEY_CURRENT_CONFIG) then
  Result := 'HKEY_CURRENT_CONFIG';
 if (ARootKey = HKEY_DYN_DATA) then
  Result := 'HKEY_DYN_DATA';
end;

// Преобразование милисекунд в строку стандартного вида HH24:MI:SS
function MilliSecToTimeStr(MSec : integer; AShowMs : boolean = false) : string;
var
 Hour, Min, Sec : integer;
begin
 Hour := MSec div (3600000); // Часы
 MSec := MSec - Hour * 3600000;
 Min  := MSec div 60000;    // Минуты
 MSec := MSec - Min * 60000;
 Sec  := MSec div 1000;    // Секунды
 MSec := MSec - Sec * 1000;
 Result := FormatFloat('00', Hour)+':'+FormatFloat('00', Min)+':'+FormatFloat('00', Sec);
 if AShowMs then
  Result := Result+'.'+FormatFloat('000', MSec)
end;

// Проверить, совпадает ли файл с маской (список масок передается в AMackList)
function MatchesMask(const Filename: string; AMackList : TStringList): Boolean;
var
 CMask : TMask;
 i     : integer;
begin
 Result := false;
 for i := 0 to AMackList.Count - 1 do begin
  // Сравнение имени
  if LowerCase(Filename) = AMackList[i] then begin Result := true; exit; end;
  CMask := TMask.Create(AMackList[i]);
  try
    if CMask.Matches(Filename) then begin
     Result := true;
     break;
    end;
  finally
    CMask.Free;
  end;
 end;
end;

// Возврящает путь к каталогу для временных файлов (Temp)
function GetTempDirectoryPath : String;
var
 Buf : array[0..1023] of Char;
 Res : integer;
begin
 Res    := GetTempPath(SizeOf(Buf) - 1, Buf);
 Result :=  NormalDir(copy(Buf, 1, Res));
end;

function zForceDirectories(Dir: string): Boolean;
begin
 try
  Result := ForceDirectories(Dir);
 except
  Result := false;
 end;
end;

end.


unit zBackupEngine;
// Система создания резарвных копий
interface
uses windows, registry, Classes,
  zCRC32,
  AbArcTyp,
  AbZipTyp,
  AbUnzPrc,
  AbDfBase,
  AbDfEnc,
  AbDfDec;
const
 // Коды обытий для отката
 brtCreateKey                = 1; // Создать ключ
 brtDeleteParam              = 2; // Удалить параметр
 brtSetParamValREG_DWORD     = 3; // Создать параметр и задать ему значение. Тип параметра REG_DWORD
 brtSetParamValREG_SZ        = 4; // Создать параметр и задать ему значение. Тип параметра REG_SZ
 brtSetParamValREG_EXPAND_SZ = 5; // Создать параметр и задать ему значение. Тип параметра REG_EXPAND_SZ
 brtSetParamValREG_MULTI_SZ  = 6; // Создать параметр и задать ему значение. Тип параметра REG_MULTY_SZ
 brtSetParamValREG_BINARY    = 7; // Создать параметр и задать ему значение. Тип параметра REG_BINARY
 // Массив ключей шифрования базы
 CodeArray      : array [0..1, 0..$0F] of byte = ((0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
                                                 (115,25,67,51,134,92,210,111,6,35,197,241,19,125,230,72));
type
 // Запись в базе бекапа
 TBackupRec = class
  RecType  : word;   // Код типа операции
  X1, X2, X3, X4 : string; // Данные. Интерпретация X* зависит от RecType
 end;
 // Заголовок базы бекапа
 TBackupFileHeader = packed record
  Sign           : packed array [1..6] of char; // Сигнатура BACKUP
  DateTime       : TDateTime;                   // Дата и время создания базы
  MinAvzVersion  : Real48;                      // Код версии базы (для совместимости)
  RecCount       : Longint;                     // Количество записей в базе данных типа 1
  CRC            : DWORD;                       // Контрольная сумма всего файла
  code_num       : byte;                        // Байт для доп. кодирования
 end;
 // Класс, осуществляющий поддержку бекапа
 TZBackupEngine = class
  private
    FBackupId: string;
    procedure SetBackupId(const Value: string);
  public
   // Класс для работы с реестром
   Reg : TRegistry;
   // База бекапа
   BackupDB : TStringList;
   // Создание/разрушение класса
   constructor Create;
   destructor Destroy; override;
   // Очистка базы
   function ClearDB : boolean;
   // Загрузка базы бекапа
   function LoadBackupDB(AStream : TMemoryStream) : boolean;
   // Сохранение базы бекапа
   function SaveBackupDB(AStream : TMemoryStream) : boolean;
   // Выполнение отката (ADeleteItems - удаление эламентов из базы бекапа после выполнения)
   function UndoChanges(ABackupId : string; ADeleteItems : boolean = true) : boolean;
   // Проверка, существует ли бекап для данного ID
   function BackUpExists(ABackupId : string) : boolean;
   // Удаление истории всех из изменений с заданным ID из базы
   function DeleteChanges(ABackupId : string) : boolean;
   // Удаление элемента
   function DeleteItem(AIndx : integer) : boolean;
   // Добавление элемента (данные задаются параметрами функции)
   function AddItem(ARecType : word; ABackupId, AX1, AX2, AX3, AX4 : string) : boolean;
   // Добавление элемента (данные считываются из потока)
   function AddItemFromStream(AStream : TStream) : boolean;
   // Сохранение элемента в поток
   function SaveItemToStream(AStream : TStream; ABackupId : string; AItem : TBackupRec) : boolean;
   // Сохранение строки в поток
   function SaveStringToStream(AStream : TStream; S : string) : boolean;
   // Загрузка строки из потока
   function LoadStringFromStream(AStream : TStream; var S : string) : boolean;
   // ****** Добавление событий в базу ******
   function AddBackupDeleteKey(ARoot, AKey : string) : boolean;
   function AddBackupDeleteParam(ARoot, AKey, AParamName : string) : boolean;
   function AddBackupChangeParam(ARoot, AKey, AParamName : string) : boolean;
   // Резервная копия ключа в базе бекапа
   function BackupRegKey(ARoot, AKey : string) : boolean;
   // ************ Вспомогательные функции ***********
   // Настройка RootKey класса работы с реестром по имени
   function SetRootKeyByName(Reg : TRegistry; AName : string) : boolean;
 published
   property BackupId : string read FBackupId write SetBackupId;
 end;
implementation
uses Math, SysUtils, zutil, zSharedFunctions;

// Декодировать бинарные данные (расширофка и разархивация)
Function DecodeBinData(MS : TMemoryStream) : boolean;
var
 i, ResSize : integer;
 w : word;
 b : byte;
 VirusBaseHeader : TBackupFileHeader;
 CRC, HeaderCRC : DWORD;
 AbDeflateHelper : TAbDeflateHelper;
 PackedStream, UnpackedStream : TMemoryStream;
begin
 Result := false;
 {$R-}
 // Чтение заголовка
 MS.Seek(0,0);
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 // Запись заголовка с CRC = 0
 MS.Seek(0,0);
 HeaderCRC := VirusBaseHeader.CRC;
 VirusBaseHeader.CRC := 0;
 MS.Write(VirusBaseHeader, sizeof(VirusBaseHeader));
 MS.Seek(0,0);
 CRC := 0;
 // Цикл декодирования данных
 for i := sizeof(TBackupFileHeader) to MS.Size - 1 do begin
  w := word(i + i*5 + i*i div 3 + VirusBaseHeader.code_num * 8 + 219);
  b := byte( Pointer(Longint(MS.Memory) + i)^ );
  CRC := DWORD(CRC + b * i);
  b := byte(not(b xor w));
  b := b xor CodeArray[1, i mod $0F];
  asm rol b, 3 end;
  ResSize := 1;
  Move(b, Pointer(Longint(MS.Memory) + i)^, ResSize);
 end;
 // Цикл расчета CRC заголовка
 for i := 0 to sizeof(TBackupFileHeader) - 1 do begin
  b := byte( Pointer(Longint(MS.Memory) + i)^ );
  CRC := CRC + b * i;
 end;
 Result := CRC = HeaderCRC;
 if Result then begin
  PackedStream := nil; UnPackedStream := nil; AbDeflateHelper := nil;
  try
    try
     // Создание структур и классов
     PackedStream    := TMemoryStream.Create;
     UnPackedStream  := TMemoryStream.Create;
     AbDeflateHelper := TAbDeflateHelper.Create;
     // Копирование данных в поток
     MS.Seek(SizeOf(VirusBaseHeader), 0);
     PackedStream.CopyFrom(MS, MS.Size - SizeOf(VirusBaseHeader));
     // Распаковка
     PackedStream.Seek(0,0); UnPackedStream.Seek(0,0);
     Inflate(PackedStream, UnPackedStream, AbDeflateHelper);
     MS.SetSize(SizeOf(VirusBaseHeader));
     UnPackedStream.Seek(0,0);
     MS.CopyFrom(UnPackedStream, UnPackedStream.Size);
    finally
     AbDeflateHelper.Free;
     UnPackedStream.Free;
     PackedStream.Free;
    end;
  except
   Result := false;
  end;
 end;
 {$R+}
end;

// Кодировать (шифровка + архивация)
function CodeBinData(MS: TMemoryStream) : boolean;
var
 i, ResSize : integer;
 w : word;
 b : byte;
 VirusBaseHeader : TBackupFileHeader;
 AbDeflateHelper : TAbDeflateHelper;
 PackedStream, UnpackedStream : TMemoryStream;
begin
 Result := false;
 {$R-}
 // Загрузка данных в буфер
 MS.Seek(0,0);
 MS.Read(VirusBaseHeader, SizeOf(VirusBaseHeader));
 // Создание структур и классов
 PackedStream    := TMemoryStream.Create;
 UnPackedStream  := TMemoryStream.Create;
 AbDeflateHelper := TAbDeflateHelper.Create;
 AbDeflateHelper.PKZipOption := 'x';
 // Копирование данных в поток
 UnPackedStream.CopyFrom(MS, MS.Size - SizeOf(VirusBaseHeader));
 // Сжатие
 UnpackedStream.Seek(0,0); PackedStream.Seek(0,0);
 Deflate(UnpackedStream, PackedStream, AbDeflateHelper);
 PackedStream.Seek(0,0);
 MS.SetSize(0);
 VirusBaseHeader.CRC := 0;
 MS.Write(VirusBaseHeader,  SizeOf(VirusBaseHeader));
 PackedStream.Seek(0,0);
 MS.CopyFrom(PackedStream, PackedStream.Size);
 // Разрушение классов и освобожение памяти
 PackedStream.Free;
 UnPackedStream.Free;
 AbDeflateHelper.Free;
 // Цикл кодирования данных
 for i := sizeof(TBackupFileHeader) to MS.Size - 1 do begin
  b := byte( Pointer(Longint(MS.Memory) + i)^ );
  asm ror b, 3 end;
  w := i + i*5 + i*i div 3 + VirusBaseHeader.code_num * 8 + 219;
  b := b xor CodeArray[1, i mod $0F];
  b := (not b) xor w;
  ResSize := 1;
  Move(b, Pointer(Longint(MS.Memory) + i)^, ResSize);
 end;
 VirusBaseHeader.CRC := 0;
 for i := 0 to MS.Size - 1 do
  VirusBaseHeader.CRC := DWORD(VirusBaseHeader.CRC + byte( Pointer(Longint(MS.Memory) + i)^ ) * i);
 MS.Seek(0,0);
 MS.Write(VirusBaseHeader, SizeOf(VirusBaseHeader));
 MS.Seek(0,0);
 Result := true;
 {$R+}
end;

{ TZBackupEngine }

function TZBackupEngine.AddBackupChangeParam(ARoot, AKey,
  AParamName: string): boolean;
var
 j     : integer;
 ST    : string;
 DataType, DataSize: Integer;
begin
 Result := false;
 if not SetRootKeyByName(Reg, ARoot) then exit;
 if AKey = '' then exit;
 try
   // Попытка открыть ключ
   if Reg.OpenKey(AKey, false) then begin
    // Параметр существует
    if Reg.ValueExists(AParamName) then begin
     case Reg.GetDataType(AParamName) of
      rdString       : AddItem(brtSetParamValREG_SZ, BackupId, ARoot, AKey, AParamName, Reg.ReadString(AParamName));
      rdExpandString : AddItem(brtSetParamValREG_EXPAND_SZ, BackupId, ARoot, AKey, AParamName, Reg.ReadString(AParamName));
      rdInteger      : AddItem(brtSetParamValREG_DWORD, BackupId, ARoot, AKey, AParamName, Reg.ReadString(AParamName));
      rdBinary,
      rdUnknown      : begin
                       DataSize := Reg.GetDataSize(AParamName);
                       SetLength(ST, DataSize);
                       if DataSize > 0 then begin
                        j := Reg.ReadBinaryData(AParamName, ST[1], Length(ST));
                        SetLength(ST, j);
                       end;
                        if (RegQueryValueEx(Reg.CurrentKey, PChar(AParamName), nil, @DataType, nil,
                           @DataSize) = ERROR_SUCCESS) and (DataType = REG_MULTI_SZ) then
                            AddItem(brtSetParamValREG_MULTI_SZ, BackupId, ARoot, AKey, AParamName, ST)
                             else AddItem(brtSetParamValREG_BINARY, BackupId, ARoot, AKey, AParamName, ST);
                       end;
     end;
    end else
     AddItem(brtDeleteParam, BackupId, ARoot, AKey, AParamName, ST); // Такой параметр не существовал - удалим его при откате
    Reg.CloseKey;
   end;
 except
  Result := false;
 end;
end;

function TZBackupEngine.AddBackupDeleteKey(ARoot, AKey: string): boolean;
begin
 Result := false;
 // Переименование root ключа - для экономии места в базе
 ARoot := StringReplace(ARoot, 'HKEY_LOCAL_MACHINE',  'HKLM', [rfIgnoreCase]);
 ARoot := StringReplace(ARoot, 'HKEY_CLASSES_ROOT',   'HKCR', [rfIgnoreCase]);
 ARoot := StringReplace(ARoot, 'HKEY_CURRENT_USER',   'HKCU', [rfIgnoreCase]);
 ARoot := StringReplace(ARoot, 'HKEY_CURRENT_CONFIG', 'HKCC', [rfIgnoreCase]);
 // Если ключ существует - выполним его BackUp
 if SetRootKeyByName(Reg, ARoot) then
  if Reg.KeyExists(AKey) then begin
   BackupRegKey(ARoot, AKey);
  end;
end;

function TZBackupEngine.AddBackupDeleteParam(ARoot, AKey,
  AParamName: string): boolean;
var
 j     : integer;
 ST    : string;
 DataType, DataSize: Integer;
begin
 Result := false;
 if not SetRootKeyByName(Reg, ARoot) then exit;
 if AKey = '' then exit;
 try
   // Попытка открыть ключ
   if Reg.OpenKey(AKey, false) then begin
    // Параметр существует
    if Reg.ValueExists(AParamName) then begin
     case Reg.GetDataType(AParamName) of
      rdString       : AddItem(brtSetParamValREG_SZ, BackupId, ARoot, AKey, AParamName, Reg.ReadString(AParamName));
      rdExpandString : AddItem(brtSetParamValREG_EXPAND_SZ, BackupId, ARoot, AKey, AParamName, Reg.ReadString(AParamName));
      rdInteger      : AddItem(brtSetParamValREG_DWORD, BackupId, ARoot, AKey, AParamName, Reg.ReadString(AParamName));
      rdBinary,
      rdUnknown      : begin
                       DataSize := Reg.GetDataSize(AParamName);
                       SetLength(ST, DataSize);
                       if DataSize > 0 then begin
                        j := Reg.ReadBinaryData(AParamName, ST[1], Length(ST));
                        SetLength(ST, j);
                       end;
                        if (RegQueryValueEx(Reg.CurrentKey, PChar(AParamName), nil, @DataType, nil,
                           @DataSize) = ERROR_SUCCESS) and (DataType = REG_MULTI_SZ) then
                            AddItem(brtSetParamValREG_MULTI_SZ, BackupId, ARoot, AKey, AParamName, ST)
                             else AddItem(brtSetParamValREG_BINARY, BackupId, ARoot, AKey, AParamName, ST);
                       end;
     end;
    end;
    Reg.CloseKey;
   end;
 except
  Result := false;
 end;
end;

// Добавление элемента (данные задаются параметрами функции)
function TZBackupEngine.AddItem(ARecType: word; ABackupId, AX1, AX2, AX3,
  AX4: string): boolean;
var
 Tmp : TBackupRec;
begin
 Result := false;
 // Подготовка идентификатора (верхний регистр, пробелы в начале и конце не допустимы)
 ABackupId := Trim(UpperCase(ABackupId));
 // Создание класса-описателя элмента
 Tmp := TBackupRec.Create;
 // Заполнение полей
 with Tmp do begin
  RecType  := ARecType;
  X1       := AX1;
  X2       := AX2;
  X3       := AX3;
  X4       := AX4;
 end;
 // Добавление элемента в базу
 BackupDB.AddObject(ABackupId, Tmp);
 Result := true;
end;

// Добавление элемента (данные считываются из потока)
function TZBackupEngine.AddItemFromStream(AStream: TStream): boolean;
var
 W   : word;
 Tmp : TBackupRec;
 S   : string;
begin
 Result := false;
 W := 0;
 // Чтение кода типа записи
 if AStream.Read(W, 2) <> 2 then exit;
 // Чтение идентифкатора
 if not(LoadStringFromStream(AStream, S)) then exit;
 Tmp := TBackupRec.Create;
 Tmp.RecType  := W;
 // Загрузка данных
 LoadStringFromStream(AStream, Tmp.X1);
 LoadStringFromStream(AStream, Tmp.X2);
 LoadStringFromStream(AStream, Tmp.X3);
 LoadStringFromStream(AStream, Tmp.X4);
 // Добавление объекта в базу
 BackupDB.AddObject(S, Tmp);
end;

// Проверка, существует ли бекап для данного ID
function TZBackupEngine.BackUpExists(ABackupId: string): boolean;
var
 i : integer;
begin
 Result := false;
 ABackupId := Trim(UpperCase(ABackupId));
 if ABackupId = '' then exit;
 for i := 0 to BackupDB.Count - 1 do
  if BackupDB[i] = ABackupId then begin
   Result := true;
   exit;
  end;
end;

// Резервная копия ключа в базе бекапа
function TZBackupEngine.BackupRegKey(ARoot, AKey: string): boolean;
var
 Lines : TStringList;
 i,j   : integer;
 ST    : string;
 DataType, DataSize: Integer;
 Reg   : TRegistry;
begin
 Result := false;
 if AKey = '' then exit;
 Lines := TStringList.Create;
 Reg   := TRegistry.Create;
 try
  if not SetRootKeyByName(Reg, ARoot) then exit;
  // Добавили элемент для ключа
  if Reg.OpenKey(AKey, false) then begin
   // Добавили запись бекапа самого ключа
   AddItem(brtCreateKey, BackupId, ARoot, AKey, '', '');
   // ************ BackUp параметров *************
   // Получаем список параметров
   Lines.Clear;
   Reg.GetValueNames(Lines);
   for i := 0 to Lines.Count - 1 do begin
    case Reg.GetDataType(Lines[i]) of
     rdString       : AddItem(brtSetParamValREG_SZ, BackupId, ARoot, AKey, Lines[i], Reg.ReadString(Lines[i]));
     rdExpandString : AddItem(brtSetParamValREG_EXPAND_SZ, BackupId, ARoot, AKey, Lines[i], Reg.ReadString(Lines[i]));
     rdInteger      : AddItem(brtSetParamValREG_DWORD, BackupId, ARoot, AKey, Lines[i], Inttostr(Reg.ReadInteger(Lines[i])));
     rdBinary,
     rdUnknown      : begin
                       DataSize := Reg.GetDataSize(Lines[i]);
                       SetLength(ST, DataSize);
                       if DataSize > 0 then begin
                        j := Reg.ReadBinaryData(Lines[i], ST[1], Length(ST));
                        SetLength(ST, j);
                       end;
                       if (RegQueryValueEx(Reg.CurrentKey, PChar(Lines[i]), nil, @DataType, nil,
                          @DataSize) = ERROR_SUCCESS) and (DataType = REG_MULTI_SZ) then
                           AddItem(brtSetParamValREG_MULTI_SZ, BackupId, ARoot, AKey, Lines[i], ST)
                            else AddItem(brtSetParamValREG_BINARY, BackupId, ARoot, AKey, Lines[i], ST);
                      end;
    end;
   end;
   // ************* BackUp вложенных ключей ******
   // Получаем список вложенных ключей
   Lines.Clear;
   Reg.GetKeyNames(Lines);
   if not(AKey[length(AKey)] in ['\', '/']) then
    AKey := AKey + '\';
   for i := 0 to Lines.Count - 1 do
    BackupRegKey(ARoot, AKey + Lines[i]);
   Reg.CloseKey;
  end;
 finally
  Lines.Free;
  Reg.Free;
 end;
 Result := true;
end;

// Очистка базы
function TZBackupEngine.ClearDB: boolean;
var
 i : integer;
begin
 Result := false;
 // Очистка классов, хранящих записи бекапа
 for i := 0 to BackupDB.Count - 1 do
  if BackupDB.Objects[i] <> nil then begin
   if BackupDB.Objects[i] is TBackupRec then
    (BackupDB.Objects[i] as TBackupRec).Free;
   BackupDB.Objects[i] := nil;
  end;
 // Очистка списка записей
 BackupDB.Clear;
 Result := true;
end;

constructor TZBackupEngine.Create;
begin
 BackupDB := TStringList.Create;
 Reg      := TRegistry.Create;
end;

function TZBackupEngine.DeleteChanges(ABackupId: string): boolean;
var
 i : integer;
begin
 Result := false;
 i := 0;
 ABackupId := Trim(UpperCase(ABackupId));
 // Цикл удаление элементов с заданным ID
 while i < BackupDB.Count do begin
  if BackupDB[i] = ABackupId then
   DeleteItem(i)
    else inc(i);
 end;
 Result := true;
end;

function TZBackupEngine.DeleteItem(AIndx: integer): boolean;
begin
 Result := false;
 // Контроль индекса
 if AIndx < 0 then exit;
 if AIndx >= BackupDB.Count then exit;
 // освобождение памяти, занятой элементом
 if BackupDB.Objects[AIndx] is TBackupRec then
  (BackupDB.Objects[AIndx] as TBackupRec).Free;
 BackupDB.Objects[AIndx] := nil;
 // Удаление элемента
 BackupDB.Delete(AIndx);
 Result := true;
end;

destructor TZBackupEngine.Destroy;
begin
 // Освобожение памяти
 ClearDB;
 try Reg.CloseKey; except end;
 Reg.Free;
 BackupDB.Free;
 inherited;
end;

function TZBackupEngine.LoadBackupDB(AStream: TMemoryStream): boolean;
var
 BackupFileHeader : TBackupFileHeader;
 i : integer;
begin
 Result := false;
 // Очистка базы перед загрузкой
 ClearDB;
 AStream.Seek(0,0);
 // Проверка CRC, расшифровка, разархивация
 if not(DecodeBinData(AStream)) then exit;
 AStream.Seek(0,0);
 // Загрузка заголовка
 AStream.Read(BackupFileHeader, SizeOf(BackupFileHeader));
 // Загрузка данных
 for i := 0 to BackupFileHeader.RecCount - 1 do begin
  AddItemFromStream(AStream);
 end;
 Result := true;
end;

// Загрузка строки из потока
function TZBackupEngine.LoadStringFromStream(AStream: TStream;
  var S: string): boolean;
var
 W : Word;
begin
 Result := false;
 S := '';
 // Загрузка длинны строки
 if AStream.Read(W, 2) <> 2 then exit;
 Result := true;
 // Длинна строки больше 0 ? Если да, то выделить буфер и загрузить строку
 if W > 0 then begin
  SetLength(S, W);
  Result := AStream.Read(S[1], W) = W;
 end;
end;

function TZBackupEngine.SaveBackupDB(AStream: TMemoryStream): boolean;
var
 BackupFileHeader : TBackupFileHeader;
 i   : integer;
begin
 // Очистка потока
 AStream.Seek(0,0);
 AStream.Clear;
 AStream.Size := 0;
 AStream.Seek(0,0);
 // Подготовка заголовка
 BackupFileHeader.Sign := 'BACKUP';
 BackupFileHeader.DateTime := Now;
 BackupFileHeader.MinAvzVersion := 4.18;
 BackupFileHeader.RecCount := BackupDB.Count;
 BackupFileHeader.CRC := 0;
 BackupFileHeader.code_num := Random(255);
 // Запись заголовка
 AStream.Write(BackupFileHeader, SizeOf(BackupFileHeader));
 // Запись данных
 for i := 0 to BackupDB.Count - 1 do
  SaveItemToStream(AStream, BackupDB[i], (BackupDB.Objects[i] as TBackupRec));
 // Шифрование, архивация, расчет CRC
 Result := CodeBinData(AStream);
end;

// Сохранение элемента в поток
function TZBackupEngine.SaveItemToStream(AStream: TStream;
  ABackupId : string; AItem: TBackupRec): boolean;
begin
 // Запись кода типа
 AStream.Write(AItem.RecType, 2);
 // Запись ID
 SaveStringToStream(AStream, ABackupId);
 // Запись текстовых полей с данными
 SaveStringToStream(AStream, AItem.X1);
 SaveStringToStream(AStream, AItem.X2);
 SaveStringToStream(AStream, AItem.X3);
 SaveStringToStream(AStream, AItem.X4);
end;

// Сохранение строки в поток
function TZBackupEngine.SaveStringToStream(AStream: TStream;
  S: string): boolean;
var
 W  : word;
begin
 Result := false;
 W := length(S);
 // Запись длинны строки
 AStream.Write(W, 2);
 // Запись самой строки
 if W > 0 then
  AStream.Write(S[1], W);
 Result := true;
end;

procedure TZBackupEngine.SetBackupId(const Value: string);
begin
  FBackupId := Trim(UpperCase(Value));
end;

function TZBackupEngine.SetRootKeyByName(Reg : TRegistry; AName: string): boolean;
begin
 Result := false;
 if StrToRootKey(AName) > 0 then begin
  Reg.RootKey := StrToRootKey(AName);
  Result := true;
 end;
end;

// Выполнение отката (ADeleteItems - удаление эламентов из базы бекапа после выполнения)
function TZBackupEngine.UndoChanges(ABackupId: string;
  ADeleteItems: boolean): boolean;
var
 i, ErrCount, OpCount : integer;
 Tmp : TBackupRec;
 S : string;
begin
 Result := false;
 ABackupId := Trim(UpperCase(ABackupId));
 OpCount := 0; ErrCount := 0;
 // Цикл восстановления - выполняется в обратном порядке
 i := BackupDB.Count - 1;
 while (i >= 0) and (i < BackupDB.Count) do begin
  // Это элемент требуемой группы ? Если да, то выполним его
  if BackupDB[i] = ABackupId then begin
   // Получаем описание элемента
   Tmp := (BackupDB.Objects[i] as TBackupRec);
   // Выполняем откат
   case Tmp.RecType of
    // Создание ключа реестра
    brtCreateKey : begin
                    try
                     if SetRootKeyByName(Reg, Tmp.X1) then
                      Reg.CreateKey(Tmp.X2)
                       else inc(ErrCount);
                    except
                     inc(ErrCount);
                    end;
                   end;
    // Удаление параметра реестра
    brtDeleteParam : begin
                    try
                     if SetRootKeyByName(Reg, Tmp.X1) then
                      if Reg.OpenKey(Tmp.X2, false) then begin
                       if Reg.ValueExists(Tmp.X3) then
                        Reg.DeleteValue(Tmp.X3);
                       Reg.CloseKey;
                      end;
                    except
                     inc(ErrCount);
                    end;
                   end;
    // Создание или установка значение параметру REG_DWORD, REG_SZ
    brtSetParamValREG_DWORD,
    brtSetParamValREG_SZ,
    brtSetParamValREG_EXPAND_SZ : begin
                    try
                     if SetRootKeyByName(Reg, Tmp.X1) then
                      if Reg.OpenKey(Tmp.X2, true) then begin
                       // Удаление текущего параметра
                       if Reg.ValueExists(Tmp.X3) then
                        Reg.DeleteValue(Tmp.X3);
                       // Создание нового параметра с правильным типом и значением
                       // REG_DWORD
                       if Tmp.RecType = brtSetParamValREG_DWORD then
                        Reg.WriteInteger(Tmp.X3, StrToInt(Tmp.X4));
                       // REG_SZ
                       if Tmp.RecType = brtSetParamValREG_SZ then
                        Reg.WriteString(Tmp.X3, Tmp.X4);
                       // REG_EXPAND_SZ
                       if Tmp.RecType = brtSetParamValREG_EXPAND_SZ then
                        Reg.WriteExpandString(Tmp.X3, Tmp.X4);
                       Reg.CloseKey;
                      end;
                    except
                     inc(ErrCount);
                    end;
                   end;
    // Создание или установка значение параметру REG_DWORD, REG_SZ
    brtSetParamValREG_MULTI_SZ,
    brtSetParamValREG_BINARY : begin
                    try
                     if SetRootKeyByName(Reg, Tmp.X1) then
                      if Reg.OpenKey(Tmp.X2, true) then begin
                       // Удаление текущего параметра
                       if Reg.ValueExists(Tmp.X3) then
                        Reg.DeleteValue(Tmp.X3);
                       // Создание нового параметра с правильным типом и значением
                       S := Tmp.X4;
                       if Length(S) > 0 then begin
                        // REG_BINARY
                        if Tmp.RecType = brtSetParamValREG_BINARY then
                         RegSetValueEx(Reg.CurrentKey, PChar(Tmp.X3), 0,
                                       REG_BINARY, @S[1], Length(S));
                        // REG_MULTI_SZ
                        if Tmp.RecType = brtSetParamValREG_MULTI_SZ then
                         RegSetValueEx(Reg.CurrentKey, PChar(Tmp.X3), 0,
                                       REG_MULTI_SZ, @S[1], Length(S));
                       end else begin
                        // REG_BINARY
                        if Tmp.RecType = brtSetParamValREG_BINARY then
                         RegSetValueEx(Reg.CurrentKey, PChar(Tmp.X3), 0,
                                       REG_BINARY, nil, 0);
                        // REG_MULTI_SZ
                        if Tmp.RecType = brtSetParamValREG_MULTI_SZ then
                         RegSetValueEx(Reg.CurrentKey, PChar(Tmp.X3), 0,
                                       REG_MULTI_SZ, nil, 0);
                       end;
                       Reg.CloseKey;
                      end;
                    except
                     inc(ErrCount);
                    end;
                   end;
   end;
  end;
  dec(i);
 end;
 // Чистка базы
 if ADeleteItems then
  DeleteChanges(ABackupId);
 Result := true;
end;

end.

(*
// Код для тестирования системы
var
 BackupEngine : TZBackupEngine;
 MS : TMemoryStream;
begin
 BackupEngine := TZBackupEngine.Create;
 MS           := TMemoryStream.Create;
 {
 BackupEngine.BackupId := 'X5';
 BackupEngine.AddBackupDeleteKey('HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\Windows\CurrentVersion');
 BackupEngine.SaveBackupDB(MS);
 MS.seek(0,0);
 MS.SaveToFile('c:\text.zbk');
 }
 MS.LoadFromFile('c:\text.zbk');
 BackupEngine.LoadBackupDB(MS);
 MS.Free;
 BackupEngine.Free;
end;

*)

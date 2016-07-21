unit zRevizor;

interface
uses Windows, SysUtils, Classes, Dialogs, Forms,
  zCRC32,
  AbArcTyp,
  AbZipTyp,
  AbUnzPrc,
  AbDfBase,
  AbDfEnc,
  AbDfDec;

type
 // Структура "файл"
 TRevizorFile = class
  Name  : string;  // Имя
  Size  : dword;   // Размер
  Attr  : byte;    // Атрибуты
  CRC   : dword;   // Контрольная сумма
  RC    : byte;    // Результат сравнения
 end;
 TRevizorHDR = packed record
  SIGN          : array[1..3] of char;  // Сигнатура
  DateTime      : TDateTime;            // Дата генерации
  Version       : Double;               // Версия
  RootDirCount  : dword;                // Кол-во корневых каталогов
  code_num      : byte;                 // Байт для доп. кодирования
  CRC           : dword;                // Контрольная сумма файла
  Mode          : dword;                // Битовая маска режимов
  DopData       : dword;                // Битовая маска с описанием доп.данных
 end;
 TRevizorDirHDR = packed record
  SIGN          : char;                 // Сигнатура
  FilesCount    : dword;                // Кол-во файлов
  SubdirsCount  : dword;                // Кол-во подкаталогов
 end;
 TRevizorFileHDR = packed record
  Size  : dword;   // Размер
  Attr  : byte;    // Атрибуты
  CRC   : dword;   // Контрольная сумма
 end;

 TRevizor = class;
 // Событие "сканирование файла"
 TRevizorScanEvent = procedure(Sender : TObject; ObjName : string; F1 : TRevizorFile ) of object;
 // Событие "результат сравнения"
 TRevizorCmpEvent  = procedure(Sender : TObject; ObjName : string; ACode : integer; F1, F2 : TRevizorFile) of object;
 // Событие "фильтр"
 TRevizorFltEvent  = function(ObjName : string):boolean of object;

 // Класс "каталог"
 TRevizorDir = class
  FilesArray   : TList;                 // Файлы
  SubDirsArray : array of TRevizorDir;  // Подкаталоги
  Parent       : TRevizorDir;           // Родительский каталог
  DirName      : string;
  RC           : byte;
  private
   FParentRevizor : TRevizor;
   function GetFullDirName: string;
   procedure DoRevizorScanEvent(Sender : TObject; ObjName : string; F1 : TRevizorFile);
   procedure DoRevizorCmpEvent(Sender : TObject; ObjName : string; ACode : integer; F1, F2 : TRevizorFile);
   function DoRevizorFltEvent(ObjName : string) : boolean;
  public
   constructor Create(AParentRevizor : TRevizor);
   destructor  Destroy; override;
   // Очистка (рекурсивная вглубь)
   procedure   Clear;
   // Построение от текущего уровня вглубь
   function    Refresh(ASpeedMode : boolean = false) : boolean;
   // Сравнение
   function    Compare(ASpeedMode : boolean = false) : boolean;
   // Сканирование файла
   function    ScanFile(AFileDir : string; SR : TSearchRec; var Res : TRevizorFile; ASpeedMode : boolean = false) : boolean;
   function    CompareFiles(F1, F2 : TRevizorFile; ASpeedMode : boolean = false) : boolean;
   // Сохранение в поток
   function    SaveToStream(AStream : TStream) : boolean;
   // Загрузка из потока
   function    LoadFromStream(AStream : TStream) : boolean;
  published
   property FullDirName : string read GetFullDirName;
 end;

 // Ревизор диска - строит описание указанных папок
 TRevizor = class
  private
   FOnScanFile: TRevizorScanEvent;
   FOnCmpFile: TRevizorCmpEvent;
   FOnFilterFile: TRevizorFltEvent;
    FScanMask: string;
    FExcludeMask: string;
   procedure SetOnScanFile(const Value: TRevizorScanEvent);
   procedure SetOnCmpFile(const Value: TRevizorCmpEvent);
   procedure SetOnFilterFile(const Value: TRevizorFltEvent);
    procedure SetScanMask(const Value: string);
    procedure SetExcludeMask(const Value: string);
  public
   DirsArray : array of TRevizorDir;  // Контролируемые каталоги
   RevizorHDR : TRevizorHDR;
   LastRefreshMode : Boolean;
   constructor Create;
   destructor Destroy; override;
   // Очистка списка сканируемых каталогов и всего построенного дерева
   procedure  Clear;
   // Добавление вканируемого каталога
   function   AddRootDir(ADirName : string) : boolean;
   // Удаление сканируемого каталога
   function   DeleteRootDir(ADirName : string) : boolean;
   // Построение по текущему состоянию диска
   function   Refresh(ASpeedMode : boolean = false) : boolean;
   // Сравнение
   function   Compare(ASpeedMode : boolean = false) : boolean;
   // Сохранение в поток
   function   SaveToStream(AStream : TmemoryStream) : boolean;
   // Загрузка из потока
   function   LoadFromStream(AStream : TmemoryStream) : boolean;
  published
   property OnScanFile   : TRevizorScanEvent read FOnScanFile write SetOnScanFile;
   property OnCmpFile    : TRevizorCmpEvent read FOnCmpFile write SetOnCmpFile;
   property OnFilterFile : TRevizorFltEvent read FOnFilterFile write SetOnFilterFile;
   property ScanMask     : string read FScanMask write SetScanMask;
   property ExcludeMask  : string read FExcludeMask write SetExcludeMask;
 end;

// Декодировать бинарные данные
Function DecodeRevizorBinData(MS : TMemoryStream) : boolean;
// Кодировать
function CodeRevizorBinData(MS: TMemoryStream) : boolean;

implementation
uses zutil, Math;

function CalkFileCRC(AFileName: string; var CRC,
  Size: DWord): boolean;
var
 FS  : TFileStream;
 Buf : array[0..$FFFF] of byte;
 Res, i, poz : integer;
begin
 Result := false;
 CRC := 0;
 try
 FS := TFileStream.Create(AFileName, fmOpenRead or fmShareDenyNone);
 Size := FS.Size;
 Poz := 0;
 Res := FS.Read(Buf, sizeof(Buf));
 while Res > 0 do begin
  for i :=0 to Res - 1 do begin
   inc(poz);
   {$R-}
   CRC := CRC + Buf[i]*poz;
   {$R+}
  end;
  Res := FS.Read(Buf, sizeof(Buf));
 end;
 FS.Free;
 Result := true;
 except
 end;
end;

function DecodeRevizorBinData(MS: TMemoryStream): boolean;
var
 i, ResSize : integer;
 w : word;
 b : byte;
 RevizorHDR : TRevizorHDR;
 CRC, HeaderCRC : DWORD;
 AbDeflateHelper : TAbDeflateHelper;
 PackedStream, UnpackedStream : TMemoryStream;
begin
 Result := false;
 {$R-}
 // Чтение заголовка
 MS.Seek(0,0);
 MS.Read(RevizorHDR, sizeof(RevizorHDR));
 // Запись заголовка с CRC = 0
 MS.Seek(0,0);
 HeaderCRC := RevizorHDR.CRC;
 RevizorHDR.CRC := 0;
 MS.Write(RevizorHDR, sizeof(RevizorHDR));
 MS.Seek(0,0);
 CRC := 0;
 // Цикл декодирования данных
 for i := sizeof(RevizorHDR) to MS.Size - 1 do begin
  w := word(i*30 + RevizorHDR.code_num * 5 + 753);
  b := byte( Pointer(Longint(MS.Memory) + i)^ );
  CRC := DWORD(CRC + b * i);
  asm rol b, 2 end;
  b := byte(not(b xor w));
  ResSize := 1;
  Move(b, Pointer(Longint(MS.Memory) + i)^, ResSize);
 end;
 // Цикл расчета CRC заголовка
 for i := 0 to sizeof(RevizorHDR) - 1 do begin
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
     MS.Seek(SizeOf(RevizorHDR), 0);
     PackedStream.CopyFrom(MS, MS.Size - SizeOf(RevizorHDR));
     // Распаковка
     PackedStream.Seek(0,0); UnPackedStream.Seek(0,0);
     Inflate(PackedStream, UnPackedStream, AbDeflateHelper);
     MS.SetSize(SizeOf(RevizorHDR));
     UnPackedStream.Seek(0,0);
     MS.CopyFrom(UnPackedStream, UnPackedStream.Size);
    finally
     AbDeflateHelper.Free; AbDeflateHelper := nil;
     UnPackedStream.Free;  UnPackedStream  := nil;
     PackedStream.Free;    PackedStream    := nil;
    end;
  except
   Result := false;
  end;
 end;
 {$R+}
end;

function CodeRevizorBinData(MS: TMemoryStream) : boolean;
var
 i, ResSize : integer;
 w : word;
 b : byte;
 RevizorHDR : TRevizorHDR;
 AbDeflateHelper : TAbDeflateHelper;
 PackedStream, UnpackedStream : TMemoryStream;
begin
 Result := false;
 {$R-}
 // Загрузка данных в буфер
 MS.Seek(0,0);
 MS.Read(RevizorHDR, SizeOf(RevizorHDR));
 // Создание структур и классов
 PackedStream    := TMemoryStream.Create;
 UnPackedStream  := TMemoryStream.Create;
 AbDeflateHelper := TAbDeflateHelper.Create;
 // Копирование данных в поток
 UnPackedStream.CopyFrom(MS, MS.Size - SizeOf(RevizorHDR));
 // Сжатие
 UnpackedStream.Seek(0,0); PackedStream.Seek(0,0);
 Deflate(UnpackedStream, PackedStream, AbDeflateHelper);
 PackedStream.Seek(0,0);
 MS.SetSize(0);
 RevizorHDR.CRC := 0;
 MS.Write(RevizorHDR,  SizeOf(RevizorHDR));
 PackedStream.Seek(0,0);
 MS.CopyFrom(PackedStream, PackedStream.Size);
 // Разрушение классов и освобожение памяти
 PackedStream.Free;
 UnPackedStream.Free;
 AbDeflateHelper.Free;
 // Цикл кодирования данных
 for i := sizeof(RevizorHDR) to MS.Size - 1 do begin
  b := byte( Pointer(Longint(MS.Memory) + i)^ );
  w := word(i*30 + RevizorHDR.code_num * 5 + 753);
  b := (not b) xor w;
  asm ror b, 2 end;
  ResSize := 1;
  Move(b, Pointer(Longint(MS.Memory) + i)^, ResSize);
 end;
 RevizorHDR.CRC := 0;
 for i := 0 to MS.Size - 1 do
  RevizorHDR.CRC := DWORD(RevizorHDR.CRC + byte( Pointer(Longint(MS.Memory) + i)^ ) * i);
 MS.Seek(0,0);
 MS.Write(RevizorHDR, SizeOf(RevizorHDR));
 MS.Seek(0,0);
 Result := true;
 {$R+}
end;

{ TDiskRevizor }
function TRevizor.AddRootDir(ADirName: string): boolean;
var
 i : integer;
begin
 Result := false;
 // Подготовка имени каталога
 ADirName := AnsiLowerCase(NormalDir(Trim(ADirName)));
 // Поиск в списке для блокировки повторного добавления
 for i := 0 to Length(DirsArray) - 1 do
  if pos(ADirName, AnsiLowerCase(DirsArray[i].FullDirName)) = 1 then exit;
 // Добавление
 SetLength(DirsArray, Length(DirsArray) + 1);
 DirsArray[Length(DirsArray) - 1] := TRevizorDir.Create(Self);
 DirsArray[Length(DirsArray) - 1].Parent := nil;
 DirsArray[Length(DirsArray) - 1].DirName := ADirName;
end;

procedure TRevizor.Clear;
var
 i : integer;
begin
 // Очистка классов
 if Length(DirsArray) > 0 then
  for i := 0 to Length(DirsArray) - 1 do
   if DirsArray[i] <> nil then begin
    DirsArray[i].Free;
    DirsArray[i] := nil;
   end;
 // Очистка таблицы
 DirsArray := nil;
end;

constructor TRevizor.Create;
begin
 DirsArray := nil;
 LastRefreshMode := false;
end;

function TRevizor.DeleteRootDir(ADirName: string): boolean;
var
 i : integer;
begin
 Result := false;
 // Подготовка имени каталога
 ADirName := AnsiLowerCase(NormalDir(Trim(ADirName)));
 // Поиск в списке для блокировки повторного добавления
 for i := 0 to Length(DirsArray) - 1 do
  if pos(ADirName, AnsiLowerCase(DirsArray[i].FullDirName)) = 1 then begin
   DirsArray[i].Free;
   DirsArray[i] := nil;
   exit;
  end;
end;

destructor TRevizor.Destroy;
begin
 Clear;
 inherited;
end;

{ TRevizorDir }

procedure TRevizorDir.Clear;
var
 i : integer;
begin
  // Очистка списка файлов
  if FilesArray.Count > 0 then
  for i := 0 to FilesArray.Count - 1 do
   if FilesArray[i] <> nil then begin
    TRevizorFile(FilesArray[i]).Free;
    FilesArray[i] := nil;
   end;
   FilesArray.Clear;
  // Очиска подкаталогов
  if Length(SubDirsArray) > 0 then 
  for i := 0 to Length(SubDirsArray) - 1 do
   if SubDirsArray[i] <> nil then begin
    SubDirsArray[i].Free;
    SubDirsArray[i] := nil;
   end;
  SetLength(SubDirsArray, 0);
  SubDirsArray := nil;
end;

constructor TRevizorDir.Create;
begin
 FParentRevizor := AParentRevizor;
 FilesArray   := TList.Create;
 SubDirsArray := nil;
 Parent       := nil;
 RC := 0;
end;

destructor TRevizorDir.Destroy;
begin
 Clear;
 inherited;
end;

function TRevizorDir.GetFullDirName: string;
begin
 Result := DirName;
 if Parent <> nil then
  Result := Parent.FullDirName + DirName;
 Result := NormalDir(Result);
end;

function TRevizorDir.Refresh(ASpeedMode : boolean = false) : boolean;
var
 SR    : TSearchRec;
 Res   : integer;
 FDir  : string;
 TmpFI : TRevizorFile;
 TmpDI : TRevizorDir;
begin
 // Очистка
 Clear;
 FDir := NormalDir(FullDirName);
 // Сканирование
 Res := FindFirst(FDir+'*.*', faAnyFile ,SR);
 while Res = 0 do begin
  if (SR.Name <> '.') and (SR.Name <> '..') then
   if (SR.Attr and faDirectory) <> 0 then begin
    DoRevizorScanEvent(self, FDir + SR.Name, nil);
    TmpDI := TRevizorDir.Create(FParentRevizor);
    TmpDI.Parent  := self;
    TmpDI.DirName := SR.Name;
    TmpDI.Refresh(ASpeedMode);  
    // Добавление
    SetLength(SubDirsArray, length(SubDirsArray)+1);
    SubDirsArray[length(SubDirsArray)-1] := TmpDI;
    TmpDI := nil;
   end else begin
    if  DoRevizorFltEvent(SR.name) then begin
     TmpFI := TRevizorFile.Create;
     ScanFile(Fdir, SR, TmpFI, ASpeedMode);
     FilesArray.Add(TmpFI);
    end;
   end;
  Res := FindNext(SR);
 end;
 FindClose(SR);
end;

function TRevizorDir.SaveToStream(AStream: TStream): boolean;
var
 S : string;
 i : integer;
 b : byte;
 RevizorDirHDR  : TRevizorDirHDR;
 RevizorFileHDR : TRevizorFileHDR;
begin
 // Запись заголовка
 RevizorDirHDR.SIGN := 'D';
 RevizorDirHDR.FilesCount   := FilesArray.Count;
 RevizorDirHDR.SubdirsCount := length(SubDirsArray);
 AStream.Write(RevizorDirHDR, sizeof(RevizorDirHDR));
 // Запись имени каталога
 S := DirName;
 b := length(S);
 AStream.Write(b, 1);
 AStream.Write(S[1], b);
 // Запись данных о файлах
 for i := 0 to FilesArray.Count - 1 do begin
  S := TRevizorFile(FilesArray[i]).Name;
  b := length(S);
  AStream.Write(b, 1);
  AStream.Write(S[1], b);
  RevizorFileHDR.Size := TRevizorFile(FilesArray[i]).Size;
  RevizorFileHDR.Attr := TRevizorFile(FilesArray[i]).Attr;
  RevizorFileHDR.CRC  := TRevizorFile(FilesArray[i]).CRC;
  AStream.Write(RevizorFileHDR, SizeOf(RevizorFileHDR));
 end;
 // Запись данных о подкаталогах
 for i := 0 to length(SubDirsArray) - 1 do
  SubDirsArray[i].SaveToStream(AStream);
end;

function TRevizorDir.LoadFromStream(AStream: TStream): boolean;
var
 i : integer;
 b : byte;
 RevizorDirHDR  : TRevizorDirHDR;
 RevizorFileHDR : TRevizorFileHDR;  TmpFI : TRevizorFile;
begin
 // Чтение заголовка
 AStream.Read(RevizorDirHDR, sizeof(RevizorDirHDR));
 if RevizorDirHDR.SIGN <> 'D' then exit;
 SetLength(SubDirsArray, RevizorDirHDR.SubdirsCount);
 // Чтение имени каталога
 AStream.Read(b, 1);
 if b > 0 then begin
  SetLength(DirName, b);
  AStream.Read(DirName[1], b);
 end else DirName := '';
 // Чтение данных о файлах
 if RevizorDirHDR.FilesCount > 0 then
  for i := 0 to RevizorDirHDR.FilesCount - 1 do begin
   TmpFI := TRevizorFile.Create;
   AStream.Read(b, 1);
   if b > 0 then begin
    SetLength(TmpFI.Name, b);
    AStream.Read(TmpFI.Name[1], b);
   end else TmpFI.Name := '';
   AStream.Read(RevizorFileHDR, SizeOf(RevizorFileHDR));
   TmpFI.Size := RevizorFileHDR.Size;
   TmpFI.Attr := RevizorFileHDR.Attr;
   TmpFI.CRC  := RevizorFileHDR.CRC;
   FilesArray.Add(TmpFI);
  end;
 // Чтение данных о подкаталогах
 for i := 0 to length(SubDirsArray) - 1 do begin
  SubDirsArray[i] := TRevizorDir.Create(FParentRevizor);
  SubDirsArray[i].Parent := Self;
  SubDirsArray[i].LoadFromStream(AStream);
 end;
end;

function TRevizorDir.ScanFile(AFileDir: string; SR : TSearchRec;
  var Res: TRevizorFile;ASpeedMode : boolean = false): boolean;
var
 CRC,  Size: DWord;
begin
 Res.Name := AnsiLowerCase(SR.Name);
 Res.Size := SR.Size;
 Res.Attr := byte(SR.Attr);
 Res.CRC  := 0;
 // Вычисление CRC
 if not(ASpeedMode) then
  if CalkFileCRC(NormalDir(AFileDir) + SR.Name, CRC, Size) then
   Res.CRC  := CRC;
 DoRevizorScanEvent(Self, NormalDir(AFileDir) + SR.Name, Res);
 Result := true;
end;

function TRevizor.Refresh(ASpeedMode : boolean = false) : boolean;
var
 i : integer;
begin
 Result := true;
 LastRefreshMode := ASpeedMode;
 // Очистка классов
 for i := 0 to Length(DirsArray) - 1 do
  if DirsArray[i] <> nil then
    DirsArray[i].Refresh(ASpeedMode);
end;

function TRevizor.SaveToStream(AStream: TmemoryStream): boolean;
var
 i : integer;
 RevizorHDR : TRevizorHDR;
 w : word;
begin
 Result := true;
 // Очистка потока
 AStream.Seek(0,0);
 AStream.Size := 0;
 // Сохранение заголовка
 RevizorHDR.SIGN := 'ARZ';
 RevizorHDR.DateTime := Now;
 RevizorHDR.Version := 1.00;
 RevizorHDR.DopData := 0;
 RevizorHDR.Mode := 0;
 if LastRefreshMode then
  RevizorHDR.Mode := 1;
 RevizorHDR.RootDirCount := Length(DirsArray);
 AStream.Write(RevizorHDR, SizeOf(RevizorHDR));
 // Сохранение маски сканирования
 w := length(FScanMask);
 AStream.Write(w, 2);
 if w > 0 then
  AStream.Write(FScanMask[1], w);
 // Сохранение маски исключения
 w := length(FExcludeMask);
 AStream.Write(w, 2);
 if w > 0 then
  AStream.Write(FExcludeMask[1], w);
 // Очистка классов
 for i := 0 to Length(DirsArray) - 1 do
  if DirsArray[i] <> nil then
    DirsArray[i].SaveToStream(AStream);
 CodeRevizorBinData(AStream);
end;

function TRevizor.LoadFromStream(AStream: TmemoryStream): boolean;
var
 i : integer;
 w : word;
begin
 clear;
 AStream.Seek(0,0);
 Result := DecodeRevizorBinData(AStream);
 if not(Result) then exit;
 AStream.Seek(0,0);
 AStream.Read(RevizorHDR, sizeof(RevizorHDR));
 // Чтение маски сканирования
 AStream.Read(W, 2);
 if w > 0 then begin
  SetLength(FScanMask, w);
  AStream.Read(FScanMask[1], w);
 end else FScanMask := '';
 // Чтение маски исключения
 AStream.Read(W, 2);
 if w > 0 then begin
  SetLength(FExcludeMask, w);
  AStream.Read(FExcludeMask[1], w);
 end else FExcludeMask := '';
 SetLength(DirsArray, RevizorHDR.RootDirCount);
 for i := 0 to RevizorHDR.RootDirCount - 1 do begin
  DirsArray[i] := TRevizorDir.Create(Self);
  DirsArray[i].Parent := nil;
  DirsArray[i].LoadFromStream(AStream);
 end;
end;

function TRevizorDir.Compare(ASpeedMode : boolean = false) : boolean;
var
 SR    : TSearchRec;
 Res   : integer;
 FDir, LowerName  : string;
 TmpFI : TRevizorFile;
 i     : integer;
 Found : integer;
begin
 TmpFI := TRevizorFile.Create;
 FDir := NormalDir(FullDirName);
 if FilesArray.Count > 0 then
  for i := 0 to FilesArray.Count - 1 do
   TRevizorFile(FilesArray[i]).RC := 0;
 if i > 0 then
  for i := 0 to length(SubDirsArray) - 1 do
   SubDirsArray[i].RC := 0;
 // Сканирование
 Res := FindFirst(FDir+'*.*', faAnyFile ,SR);
 while Res = 0 do begin
  LowerName := AnsiLowerCase(SR.Name);
  if (SR.Name <> '.') and (SR.Name <> '..') then
   if (SR.Attr and faDirectory) <> 0 then begin
    Found := -1;
    for i := 0 to length(SubDirsArray) - 1 do
     if AnsiLowerCase(SubDirsArray[i].DirName) = LowerName then begin
      Found := i;
      SubDirsArray[i].RC := 1;
      SubDirsArray[i].Compare(ASpeedMode); // Мы нашли каталог и сканируем его рекурсивно
     end;
     if Found = -1 then  // Это новый каталог
      DoRevizorCmpEvent(Self, LowerName, 5, nil, nil);
   end else begin
    Found := -1;
    if  DoRevizorFltEvent(SR.name) then begin
      for i := 0 to FilesArray.Count - 1 do begin
       if TRevizorFile(FilesArray[i]).Name = LowerName then begin
        TRevizorFile(FilesArray[i]).RC := 1;
        Found := i;
        Break;
       end;
      end;
      if Found = -1 then begin // Это новый файл, такого еще не было
       DoRevizorCmpEvent(Self, LowerName, 2, nil, nil);
      end else begin           // Такой файл есть в базе
       ScanFile(Fdir, SR, TmpFI, ASpeedMode);
       if CompareFiles(TmpFI, TRevizorFile(FilesArray[Found]), ASpeedMode) then
        DoRevizorCmpEvent(Self, TRevizorFile(FilesArray[Found]).Name, 3, TmpFI, TRevizorFile(FilesArray[Found])); // Файл изменен
      end;
    end;
   end;
  Res := FindNext(SR);
 end;
 // Удаленные файлы
 if FilesArray.Count > 0 then
  for i := 0 to FilesArray.Count - 1 do
   if TRevizorFile(FilesArray[i]).RC = 0 then
    DoRevizorCmpEvent(Self, TRevizorFile(FilesArray[i]).Name, 1, nil, TRevizorFile(FilesArray[i]));
 // Удаленные каталоги
 if length(SubDirsArray) > 0 then
  for i := 0 to length(SubDirsArray) - 1 do
   if SubDirsArray[i].RC = 0 then
    DoRevizorCmpEvent(Self, SubDirsArray[i].DirName, 4, nil, nil);
 FindClose(SR);
 TmpFI.Free;
 TmpFI := nil;
end;

function TRevizor.Compare(ASpeedMode : boolean = false) : boolean;
var
 i : integer;
begin
 Result := true;
 // Сравнение
 if Length(DirsArray) > 0 then
  for i := 0 to Length(DirsArray) - 1 do
   if DirsArray[i] <> nil then
     DirsArray[i].Compare(ASpeedMode);
end;


procedure TRevizor.SetOnScanFile(const Value: TRevizorScanEvent);
begin
 FOnScanFile := Value;
end;

procedure TRevizor.SetOnCmpFile(const Value: TRevizorCmpEvent);
begin
  FOnCmpFile := Value;
end;

procedure TRevizorDir.DoRevizorCmpEvent(Sender: TObject; ObjName: string;
  ACode: integer; F1, F2: TRevizorFile);
begin
 if (FParentRevizor <> nil) and Assigned(FParentRevizor.FOnCmpFile) then
  FParentRevizor.FOnCmpFile(Sender, ObjName, ACode, F1, F2);
end;

procedure TRevizorDir.DoRevizorScanEvent(Sender: TObject; ObjName: string;
  F1 : TRevizorFile);
begin
 if (FParentRevizor <> nil) and Assigned(FParentRevizor.FOnScanFile) then
  FParentRevizor.FOnScanFile(Sender, ObjName, F1);
end;

procedure TRevizor.SetOnFilterFile(const Value: TRevizorFltEvent);
begin
 FOnFilterFile := Value;
end;

function TRevizorDir.DoRevizorFltEvent(ObjName: string): boolean;
begin
 if (FParentRevizor <> nil) and Assigned(FParentRevizor.FOnFilterFile) then
  Result := FParentRevizor.FOnFilterFile(ObjName)
 else Result := true;
end;

procedure TRevizor.SetScanMask(const Value: string);
begin
  FScanMask := Value;
end;

procedure TRevizor.SetExcludeMask(const Value: string);
begin
  FExcludeMask := Value;
end;

function TRevizorDir.CompareFiles(F1, F2: TRevizorFile;
  ASpeedMode: boolean): boolean;
begin
 Result := (F1.Size <> F2.Size);
 if not(ASpeedMode) then
  Result := Result or (F1.CRC <> F2.CRC);
end;
end.

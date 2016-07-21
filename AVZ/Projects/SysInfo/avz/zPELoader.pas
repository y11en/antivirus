unit zPELoader;

interface
uses Windows, classes, sysutils;
const
 // Каталоги данных
 // Каталог экспортируемых объектов
 IMAGE_DIRECTORY_ENTRY_EXPORT  =  0;
 // Каталог импортируемых объектов
 IMAGE_DIRECTORY_ENTRY_IMPORT  =  1;
 // Каталог ресурсов
 IMAGE_DIRECTORY_ENTRY_RESOURCE =  2;
 // Каталог исключений
 IMAGE_DIRECTORY_ENTRY_EXCEPTION = 3;
 // Каталог безопасности
 IMAGE_DIRECTORY_ENTRY_SECURITY = 4;
 // Таблица переадресации
 IMAGE_DIRECTORY_ENTRY_BASERELOC = 5;
 // Отладочный каталог
 IMAGE_DIRECTORY_ENTRY_DEBUG = 6;
 // Строки описания
 IMAGE_DIRECTORY_ENTRY_COPYRIGHT = 7;
 // Машинный значения (MIPS GP)
 IMAGE_DIRECTORY_ENTRY_GLOBALPTR = 8;
 // Каталог TLS ( Thread local storage - локальная память потоков )
 IMAGE_DIRECTORY_ENTRY_TLS = 9;
 // Каталог конфигурации загрузки
 IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG  = 10;

type
  //(C++: IMAGE_FILE_HEADER, )
  _IMAGE_FILE_HEADER = packed record
    PE_Segnature         : DWord; // 4550h ('PE', 00h, 00h)
    Machine              : Word;  // CPU назначения, для 32-bit Intel это 014С (hex)
    NumberOfSections     : Word;  // Число PE-секций
    TimeDateStamp        : DWORD; // Дата и время (число секунд с момента 16:00 31.12.1969) создания файла
    PointerToSymbolTable : DWORD; // Для OBJ-файлов поле указывает на таблицу символов, для DLL - на таблицу с отладочной информацией
    NumberOfSymbols      : DWORD; // Кол-во символов для OBJ
    SizeOfOptionalHeader : Word;  // Размер дополнительной части PE-заголовка
    Characteristics      : Word;  // Различные информационные флаги
  end;
  TImageFileHeader = _IMAGE_FILE_HEADER;

  //(C++: IMAGE_OPTIONAL_HEADER, он есть в Windows.pas - здесь дублирован с комментариями)
  _IMAGE_OPTIONAL_HEADER = packed record
    Magic                   : Word;     // Код, описывающий назначение программы
    MajorLinkerVersion      : Byte;     // Версия линкера (HI)
    MinorLinkerVersion      : Byte;     // Версия линкера (LO)
    SizeOfCode              : DWORD;    // Суммарный виртуальный размер всех секций, содержащих код (используется для отведения памяти)
    SizeOfInitializedData   : DWORD;    // То же для инициализированных данных (не исп. в Win 95 ?)
    SizeOfUninitializedData : DWORD;    // и для неинициализированных
    AddressOfEntryPoint     : DWORD;    // RVA точки входа в PE-файл (для DLL - DllMain)
    BaseOfCode              : DWORD;    // RVA секции с кодом
    BaseOfData              : DWORD;    // RVA секции данных
    ImageBase               : DWORD;    // Предпочтительный базовый адрес загрузки
    SectionAlignment        : DWORD;    // Выравнивание - все виртуальные адреса секций кратны этому числу
    FileAlignment           : DWORD;    // Выравнивание в файле
    MajorOperatingSystemVersion : Word; // Требуемая версия ОС для запуска программы
    MinorOperatingSystemVersion : Word;
    MajorImageVersion        : Word;    // Пользовательский номер версии, заданный линкеру
    MinorImageVersion        : Word;
    MajorSubsystemVersion    : Word;    // Номер версии подсистемы (?)
    MinorSubsystemVersion    : Word;
    Win32VersionValue        : DWORD;   // Зарезервировано
    SizeOfImage              : DWORD;   // Размер области памяти, необходимый для размещения образа PE-файла
    SizeOfHeaders            : DWORD;   // Размер области заголовков.
    CheckSum                 : DWORD;   // Контрольная сумма файла, обычно равна 0 (но используется для SYS файлов !!)
    Subsystem                : Word;    // Для исполнимых файлов – требуемая для работы подсистема
    DllCharacteristics       : Word;    // Свойства DLL. Значения 1,2,4 и 8 зарезервированы; 2000h - WDM-драйвер
    SizeOfStackReserve       : DWORD;   // Память для стека (всего)
    SizeOfStackCommit        : DWORD;   // Память для стека (отводимая сразу при загрузке)
    SizeOfHeapReserve        : DWORD;   // максимально возможный размер Heap
    SizeOfHeapCommit         : DWORD;   // Отводимы при загрузке Heap
    LoaderFlags              : DWORD;   // Флаги загрузчика, устаревшие (более не используется)
    NumberOfRvaAndSizes      : DWORD;   // Количество элементов в каталоге DataDirectory
    //Далее следует массив из NumberOfRvaAndSizes элементов, ссылающиеся на важные структуры данных,
    DataDirectory: packed array[0..IMAGE_NUMBEROF_DIRECTORY_ENTRIES-1] of TImageDataDirectory;
  end;
  TImageOptionalHeader = _IMAGE_OPTIONAL_HEADER;

  // Запись таблицы объектов PE файла
  TImageSectionHeader = packed record
   ObjName         : packed array [0..7] of char; // Имя объекта
   VirtSize        : DWord;
   VirtRVA         : DWord;
   PhSize          : DWord;
   PhOffset        : DWord;
   Reserved        : packed array[1..3] of DWORD;
   Characteristics : ULONG;
  end;

  // Заголовок блока таблицы Relocation Table
  TImageBaseRelocation=packed record
    VirtualAddress  : cardinal; // Базовый RVA
    SizeOfBlock     : cardinal; // Размер блока
  end;

  _IMAGE_EXPORT_DIRECTORY = packed record  //(C++: IMAGE_EXPORT_DIRECTORY)
      Characteristics : DWord;       //Не используется
      TimeDateStamp   : DWord;       //Отметка времени создания таблицы; не всегда содержит верное значение!
      MajorVersion    : Word;        //Не используется
      MinorVersion    : Word;        //Не используется
      Name            : DWord;       //Виртуальный адрес ASCIIZ-строки с именем данной DLL
      Base            : DWord;       //Начальный ordinal (обычно – минимальный из порядковых номеров всех
                                     // экспортируемых символов)
      NumberOfFunctions: DWord;      //Общее число экспортируемых символов
      NumberOfNames: DWord;          //Количество символов, экспортированных по именам
      AddressOfFunctions: DWORD;     //Виртуальный адрес массива адресов функций
      AddressOfNames: DWORD;         //То же для массива виртуальных адресов имен функций
      AddressOfNameOrdinals: DWord;   //То же для массива, задающего соответствие порядковых номеров именам
      ForAligmentBuf : array[1..16] of byte;
  end;

  TImageExportDirectory = _IMAGE_EXPORT_DIRECTORY;
  TImageImportDescriptor=packed record  //(C++: IMAGE_IMPORT_DESCRIPTOR)
    OriginalFirstThunk : DWORD; // Ранее это поле называлось Characteristics; в целях сохранения
    TimeDateStamp      : DWORD; // 0, если импортирование осуществляется без привязки (binding - см. далее)
                                // При импортировании с привязкой содержит отметку времени файла, из которого
    ForwarderChain     : DWORD; //
    Name               : DWORD; // Виртуальный адрес ASCIIZ-строки с именем файла, из которого импортируем
    FirstThunk         : DWORD; // Виртуальный адрес подтаблицы импортируемых символов
  end;
  PImageImportDescriptor=^TImageImportDescriptor;

  _IMAGE_TLS_DIRECTORY32 = packed record
    StartAddressOfRawData : DWORD;
    EndAddressOfRawData   : DWORD;
    AddressOfIndex        : DWORD;    // PDWORD
    AddressOfCallBacks    : DWORD;    // PIMAGE_TLS_CALLBACK *
    SizeOfZeroFill        : DWORD;
    Characteristics       : DWORD;
  end;
  TImageTSLDirectory = _IMAGE_TLS_DIRECTORY32;
  PImageTSLDirectory = ^TImageTSLDirectory;

 // Класс для работы с PE файлом
 TPEImageLoader = class
 protected
  // Загрузка таблицы описания секций файла
  function LoadSectionTable : boolean;
 public
  FS : TMemoryStream;    // Копия PE файла в памяти
  PE_Header_Rel         : DWORD;                        // Смещение на PE заголовок
  ImageFileHeader       : TImageFileHeader;             // Заголовок файла
  ImageOptionalHeader   : TImageOptionalHeader;         // Расширенный заголовок файла
  ImageSections         : array of TImageSectionHeader; // Массив секций
  PELoaded              : boolean;
  constructor Create;
  destructor Destroy; override;
  // Загрузка PE файла в память
  function LoadPEFile(APEFileName : string) : boolean;
  function LoadMemImage(ABuf: pointer; Size: dword): boolean;
  // Выгрузка PE файла из памяти
  function UnLoadPEFile : boolean;
  // Настройка защиты страницы
  function SetupPageProtect : boolean;
  // Выполнение настройки по таблице перемещения
  function ExecuteReallocation(AImageBase : DWORD) : boolean;
  // Поиск секции по RVA (-1 - секция не найдена)
  function GetSectionByRVA(RVA : DWORD) : integer;
  // Поиск секции по имени (-1 - секция не найдена)
  function GetSectionByName(AName : string) : integer;
  // Создание таблицы секции импорта
  function GetImportTable(AList : TStrings) : boolean;
  // Создание таблицы секции импорта
  function GetExportTable(AList : TStrings) : boolean;
  // Очистка памяти для таблицы импорта
  function FreeImportTable(AList : TStrings) : boolean;
  // Поиск функции по ее имени
  function ImportFunctionExists(AList : TStrings; ADLLName, AFunctionName : string) : boolean;
  // Вычисление адреса по RVA и коду секции
  function GetPhAddByRVA(RVA : DWORD; ASectionId : DWORD) : DWORD; overload;
  function GetPhAddByRVA(RVA : DWORD) : DWORD; overload;
  function DWordIsReallocated(Addr : dword) : boolean;
  function CreateCodeImageFS(AFS : TMemoryStream) : boolean;
 end;

 TImageDirectoryEntryToData = function (Base: Pointer; MappedAsImage: ByteBool;
  DirectoryEntry: Word; var Size: DWORD): Pointer; stdcall;

implementation

uses Math;

{ TPEImageLoader }

function TPEImageLoader.GetPhAddByRVA(RVA, ASectionId: DWORD): DWORD;
begin
 Result := RVA - ImageSections[ASectionId].VirtRVA +
           ImageSections[ASectionId].PhOffset;
end;

constructor TPEImageLoader.Create;
begin
 FS := TMemoryStream.Create;
 ImageSections := nil;
 PELoaded := false;
end;

destructor TPEImageLoader.Destroy;
begin
 PELoaded := false;
 ImageSections := nil;
 FS.Clear;
 FS.Free;
 inherited;
end;


function TPEImageLoader.ExecuteReallocation(AImageBase: DWORD): boolean;
var
 ImageBaseRelocation : TImageBaseRelocation;
 RelocSize, ImageBaseDelta : DWORD;
 P         : PWORD;
 RelocSectionId, CorrectSectionId : integer;
 CorrectSectionPhAddr : DWORD;
 StartTablePtr, EndTablePtr, TekTablePtr : pointer;
 ModCount : cardinal;
 i : integer;
begin
 Result := false;
 // Проверка, есть ли таблица секций
 if ImageSections = nil then exit;
 // Проверка, есть ли таблица перемещений (если нет, то выход)
 if ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size = 0 then exit;
 if ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress = 0 then exit;
 // Поиск секции, в которой находится таблица перемещений
 RelocSectionId := GetSectionByRVA(ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
 if RelocSectionId < 0 then exit;
 // Выполнение перемещений
 RelocSize := ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
 // Вычисление указателя на начало таблицы
 StartTablePtr := pointer(dword(FS.Memory) +
                          ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress -
                          ImageSections[RelocSectionId].VirtRVA +
                          ImageSections[RelocSectionId].PhOffset);
 // Вычисление указателя на конец таблицы
 EndTablePtr := pointer(dword(StartTablePtr) + RelocSize);
 TekTablePtr := StartTablePtr;
 // Вычисление поправочниго коэффициента ImageBaseDelta (разница между адресом в заголовке и реальным адресом загрузки)
 ImageBaseDelta := AImageBase - ImageOptionalHeader.ImageBase;
 if ImageBaseDelta = 0 then exit;
 // Цикл по блокам таблицы перемещений
 while dword(TekTablePtr) < dword(EndTablePtr) do begin
  // Чтение заголовка
  Move(TekTablePtr^, ImageBaseRelocation, SizeOf(ImageBaseRelocation));
  // Вычисление указателя на первый WORD в текущем блоке
  P := pointer(dword(TekTablePtr) + SizeOf(ImageBaseRelocation));
  // Вычисление физического смещения в файле
  CorrectSectionId := GetSectionByRVA(ImageBaseRelocation.VirtualAddress);
  if CorrectSectionId >= 0 then
   CorrectSectionPhAddr := dword(FS.Memory) +
                          ImageBaseRelocation.VirtualAddress -
                          ImageSections[CorrectSectionId].VirtRVA +
                          ImageSections[CorrectSectionId].PhOffset
  else
   CorrectSectionPhAddr := 0;
  // Вычисление количества элементов в блоке
  ModCount := (ImageBaseRelocation.SizeOfBlock - SizeOf(ImageBaseRelocation)) div 2;
  for i := 0 to ModCount - 1 do begin
   if (CorrectSectionPhAddr <> 0) and ((P^ and $F000) <> 0) then // Тип ссылки - не IMAGE_REL_BASED_ABSOLUTE(=0)
    Inc(pdword(CorrectSectionPhAddr + (P^ and $0fff))^, ImageBaseDelta);
   inc(P);
  end;
  TekTablePtr := P; // Текущий указатель := последний элемент + 1 (т.е. указатель на очередной заголовок)
 end;
end;

function TPEImageLoader.GetSectionByRVA(RVA: DWORD): integer;
var
 i : integer;
begin
 Result := -1;
 if not(PELoaded) then exit;
 // Поиск секции
 for i := 0 to Length(ImageSections) - 1 do
  if (RVA >= ImageSections[i].VirtRVA) and (RVA < ImageSections[i].VirtRVA + ImageSections[i].VirtSize) then begin
   Result := i;
   Break;
  end;
end;

// Поиск секции по имени (-1 - секция не найдена)
function TPEImageLoader.GetSectionByName(AName: string): integer;
var
 i : integer;
begin
 Result := -1;
 AName := Trim(UpperCase(AName));
 if not(PELoaded) then exit;
 // Поиск секции
 for i := 0 to Length(ImageSections) - 1 do
  if (AName = UpperCase(Trim(ImageSections[i].ObjName))) then begin
   Result := i;
   Break;
  end;
end;

function TPEImageLoader.LoadMemImage(ABuf : pointer; Size : dword): boolean;
var
 Buf            : packed array [0..90000] of byte;
begin
 if Size < 100 then exit;
 Result := false;
 PELoaded := false;
 // Выгрузка предыдущего модуля
 UnLoadPEFile;
 FS.SetSize(Size);
 CopyMemory(FS.Memory, ABuf, Size);
 // Перемещение на начало файла и загрузка заголовка в буфер
 FS.Seek(0,0);
 FS.Read(Buf, SizeOf(Buf));
 if FS.Size < ($3C+4) then begin
  UnLoadPEFile;
  exit;
 end;
 // Поиск смещения до заголовка
 Move(pointer(dword(FS.Memory)+$3C)^, PE_Header_Rel, 4);
 // Чтение основного заголовка
 if FS.Size < (PE_Header_Rel + sizeof(ImageFileHeader)) then begin
  UnLoadPEFile;
  exit;
 end;
 Move(pointer(dword(FS.Memory)+PE_Header_Rel)^, ImageFileHeader, sizeof(ImageFileHeader));
 if ImageFileHeader.PE_Segnature <> 17744 then begin
  UnLoadPEFile;
  exit;
 end;
 // Чтение расширенного заголовка
 Move(pointer(dword(FS.Memory)+PE_Header_Rel+$18)^, ImageOptionalHeader, sizeof(ImageOptionalHeader));
 // Чтение описания секций файла
 LoadSectionTable;
 // Настройка защиты 
 SetupPageProtect;
 PELoaded := true;
 Result := true;
end;

function TPEImageLoader.LoadPEFile(APEFileName: string): boolean;
var
 FS_DLL   : TFileStream;
 Buf            : packed array [0..90000] of byte;
begin
 Result := false;
 PELoaded := false;
 // Выгрузка предыдущего модуля
 UnLoadPEFile;
 // Создание потока, связанного с файлом (только чтение и fmShareDenyNone - иначе не откроет загруженную DLL)
 try
  FS_DLL  := TFileStream.Create(APEFileName, fmOpenRead  or fmShareDenyNone);
 except
  exit;
 end;
 try
  try
   FS.LoadFromStream(FS_DLL);
  except
   exit;
  end;
 finally
  FS_DLL.Free;
  FS_DLL := nil;
 end;
 // Перемещение на начало файла и загрузка заголовка в буфер
 FS.Seek(0,0);
 FS.Read(Buf, SizeOf(Buf));
 if FS.Size < ($3C+4) then begin
  UnLoadPEFile;
  exit;
 end;
 // Поиск смещения до заголовка
 Move(pointer(dword(FS.Memory)+$3C)^, PE_Header_Rel, 4);
 // Чтение основного заголовка
 if FS.Size < (PE_Header_Rel + sizeof(ImageFileHeader)) then begin
  UnLoadPEFile;
  exit;
 end;
 Move(pointer(dword(FS.Memory)+PE_Header_Rel)^, ImageFileHeader, sizeof(ImageFileHeader));
 if ImageFileHeader.PE_Segnature <> 17744 then begin
  UnLoadPEFile;
  exit;
 end;
 // Чтение расширенного заголовка
 Move(pointer(dword(FS.Memory)+PE_Header_Rel+$18)^, ImageOptionalHeader, sizeof(ImageOptionalHeader));
 // Чтение описания секций файла
 LoadSectionTable;
 // Настройка защиты
 SetupPageProtect;
 PELoaded := true;
 Result := true;
end;

function TPEImageLoader.LoadSectionTable: boolean;
begin
 ImageSections := nil;
 // Установка размера массива по числу секций
 SetLength(ImageSections, ImageFileHeader.NumberOfSections);
 // Загрузка массива секций
 Move(pointer(dword(FS.Memory)+PE_Header_Rel + $18 + ImageFileHeader.SizeOfOptionalHeader)^,
      ImageSections[0],
      ImageFileHeader.NumberOfSections * sizeof(TImageSectionHeader));
end;

function TPEImageLoader.UnLoadPEFile: boolean;
begin
 Result := true;
 // Очистка буфера
 FS.SetSize(0);
 ImageSections := nil;
end;

function TPEImageLoader.GetPhAddByRVA(RVA: DWORD): DWORD;
var
 SectionID : integer;
begin
 Result := 0;
 SectionID := GetSectionByRVA(RVA);
 if SectionID < 0 then exit;
 Result := GetPhAddByRVA(RVA, SectionID);
end;

function TPEImageLoader.FreeImportTable(AList: TStrings): boolean;
var
 i : integer;
begin
 For i := 0 to AList.Count-1 do
  if (AList.Objects[i] is TStrings) then begin
   (AList.Objects[i] as TStrings).Free;
   AList.Objects[i] := nil;
  end;
 AList.Clear;
end;

function TPEImageLoader.GetImportTable(AList: TStrings): boolean;
var
 ImportTableSection : integer;
 ImportTableRVA, ImportTableEntryPoint : DWord;
 PImport     : PImageImportDescriptor;
 PRVA_Import : LPDWORD;
 PLibName, PImportedName    : PChar;
 dw : dword;
 Tmp : TStrings;
begin
 FreeImportTable(AList);
 Result := false;
 // 1. Проверка, есть ли каталог импорта
 if ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = 0 then exit;
 // 2. Поиск секции каталога импорта
 ImportTableSection := GetSectionByRVA(ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
 // Если секция не найдена, то выход
 if ImportTableSection < 0 then exit;
 // Загрузка таблицы в память
 if ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size > 100000 then exit;
 ImportTableRVA        := ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
 ImportTableEntryPoint := GetPhAddByRVA(ImportTableRVA);
 FS.Seek(ImportTableEntryPoint, 0);
 PImport := pointer(ImportTableEntryPoint + dword(FS.Memory));
 while PImport.Name <> 0 do begin
  PLibName := pchar(DWord(GetPhAddByRVA(PImport.Name)));
  dw := DWord(GetPhAddByRVA(PImport.Name));
  Tmp := TStringList.Create;
  AList.AddObject(pchar(dw+dword(FS.Memory)), Tmp);
  // Привязка есть ?
  if PImport.TimeDateStamp=0  then PRVA_Import := LPDWORD(GetPhAddByRVA(pImport.FirstThunk) + dword(FS.Memory))
   else PRVA_Import:=LPDWORD(GetPhAddByRVA(pImport.OriginalFirstThunk) + dword(FS.Memory));
  while PRVA_Import^<>0 do begin
   if (dword(PRVA_Import^) and $80000000) = 0 then begin
    PImportedName := pchar(GetPhAddByRVA(PRVA_Import^) + 2 + dword(FS.Memory));
    Tmp.Add(PImportedName);
   end;
   Inc(PRVA_Import);
  end;
  inc(PImport);
 end;
 Result := true;
end;

function TPEImageLoader.ImportFunctionExists(AList: TStrings; ADLLName,
  AFunctionName: string): boolean;
var
 Poz : integer;
 Tmp : TStrings;
begin
 Result := false;
 ADLLName      := Trim(LowerCase(ADLLName));
 AFunctionName := Trim(LowerCase(AFunctionName));
 Poz := AList.IndexOf(ADLLName);
 if Poz < 0 then exit;
 Tmp := (AList.Objects[Poz] as TStrings);
 if Tmp = nil then exit;
 Result := Tmp.IndexOf(AFunctionName) >= 0;
 Tmp := nil;
end;

function TPEImageLoader.GetExportTable(AList: TStrings): boolean;
var
 ExportTableSection : integer;
 ExportTableRVA,
 ExportTableEntryPoint,
 ExportTableSize,
 ExportTableItemsCount,
 ExportTableFunctionsRel,
 ExportTableNameRel, ExportTableOrdinalRel : DWORD;
 dw      : dword;
 i       : integer;
 Buf            : packed array of byte;
 ImageExportDirectory : TImageExportDirectory;
 FunctionName    : string;
 FunctionOrdinal : word;
 FunctionRVA     : dword;
begin
 AList.Clear;
 // Секция для таблицы экспорта отсутствует ? Если да, то выход
 if (ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size = 0) or
    (ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = 0) then
     exit;
 // Запоминаем номер секции
 ExportTableSection := GetSectionByRVA(ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
 if (ExportTableSection <  0) then
   exit;
 // Определение RVA, размера и физического положения таблицы экспорта
 ExportTableRVA        := ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
 ExportTableEntryPoint := GetPhAddByRVA(ExportTableRVA);
 ExportTableSize       := ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
 // Чтение данных из таблицы экспорта в буфер
 SetLength(Buf, ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
 FS.Seek(ExportTableEntryPoint, 0);
 FS.Read(Buf[0], ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
 move(Buf[0], ImageExportDirectory, sizeof(ImageExportDirectory));
 // Чтение количества элементов таблицы экспорта
 ExportTableItemsCount   := ImageExportDirectory.NumberOfFunctions;
 ExportTableFunctionsRel := ImageExportDirectory.AddressOfFunctions - ExportTableRVA;
 // Чтение указателя на на таблицу имен
 ExportTableNameRel    := ImageExportDirectory.AddressOfNames - ExportTableRVA;
 ExportTableOrdinalRel := ImageExportDirectory.AddressOfNameOrdinals - ExportTableRVA;
 // Цикл по таблице имен
 for i := 0 to  ImageExportDirectory.NumberOfNames - 1 do begin
  // Имя функции
  move(Buf[ExportTableNameRel+i*4], dw, 4);
  if (dw - ExportTableRVA) > ExportTableSize then continue;
  FunctionName := Pchar(@Buf[dw - ExportTableRVA]);
  // Ординал
  move(Buf[ExportTableOrdinalRel+i*2], FunctionOrdinal, 2);
  // Адрес
  move(Buf[ExportTableFunctionsRel+FunctionOrdinal*4], dw, 4);
  FunctionRVA := dw;
  AList.AddObject(FunctionName, pointer(FunctionRVA));
 end;
end;


function TPEImageLoader.DWordIsReallocated(Addr: dword): boolean;
begin
 result := true;
end;

function TPEImageLoader.SetupPageProtect: boolean;
var
 NewProtect, OldProtect : cardinal;
begin
 NewProtect := PAGE_EXECUTE_READWRITE;
 VirtualProtect(FS.Memory, FS.Size, NewProtect, OldProtect);
end;

function TPEImageLoader.CreateCodeImageFS(AFS: TMemoryStream): boolean;
var
 i : integer;
begin
 if not(PELoaded) then exit;
 AFS.SetSize(ImageOptionalHeader.SizeOfImage);
 ZeroMemory(AFS.Memory, AFS.Size);
 // Копирование заголовков
 CopyMemory(AFS.Memory, FS.Memory, ImageOptionalHeader.SizeOfHeaders);
 // Копирование секций
 for i := 0 to length(ImageSections)-1 do
  CopyMemory(pointer(dword(AFS.Memory) + ImageSections[i].VirtRVA),
             pointer(dword(FS.Memory)  + ImageSections[i].PhOffset),
             ImageSections[i].PhSize);
end;

end.

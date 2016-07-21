unit zAntivirus;

interface
uses zutil, windows, classes, Sysutils, zStringCompressor, zVBAMacros, activex,
  zCRC32,
  wintrust,
  AbArcTyp,
  AbZipTyp,
  AbUnzPrc,
  AbDfBase,
  AbDfEnc,
  AbDfDec;
const
 AVClasterCount = 128; // Количество кластеров в разбиении базы
 // Массив для приведения символа к нижнему регистру
 LowerCaseArray : array [0..255] of byte = (0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255);
 // Массив ключей для шифрации базы
 CodeArray      : array [0..4, 0..$0F] of byte = ((0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
                                                 (15,225,110,85,113,90,126,133,54,119,118,89,150,243,143,203),
                                                 (139,178,3,180,220,195,226,57,139,69,228,107,120,20,59,185),
                                                 (96,204,135,43,66,59,21,38,113,49,56,142,13,221,191,110),
                                                 (27,182,54,12,207,106,192,9,35,67,161,227, 20, 155, 211, 47));
 // Формат базы (для контроля совместимости)
 MaxFormatCode : Real48 = 4.29;
 // Коды типов баз
 scAVZBaseSignature = 1;
 scAVZScripts       = 2;
 scAVZNeuro         = 3;
var
 // Задержка в сканировании для снижения нагрузки на систему
 SleepScanTime  : integer = 0;
 // Признак, управляющий сканированием папкок относительно каталога AVZ
 ScanAVZFolders : boolean = false;

type
 // Цифровая подпись файла
 TFileSign = record
  FileSize  : DWord; // Размер
  FileCRC   : DWord; // Контрольная сумма
 end;
 TFileSignList = array of TFileSign;

 // Кластер сигнатур файлов
 TFileSignListCluster = record
  SMin, Smax       : dword;
  SignCount        : dword;      // Расчетное кол-во сигнатур
  FileSignList     : TFileSignList;  // Массив подписей сигнатур
 end;

 // Информация о файле
 TAVZFileInfo  = record
  FileName       : String;       // Имя файла
  Size           : integer;      // Размер файла
  FileVersion    : string;       // Версия файла
  LegalCopyright : string;       // Copyright
  IsEXEFile,                     // Признак сигнатуры MZ
  IsPEFile       : boolean;      // Признак сигнатуры PE
  StorageType    : integer;      // Тип хранилица (0-обычный файл, 1...N - некий составной файл)
  PE_EntryPoint  : integer;      // Точка входа в PE файл (ее физическое положение в файле)
  PE_HeaderSize  : integer;      // Точка входа в PE файл (ее физическое положение в файле)
  Sign : array[1..4] of  dword;  // Сигнатуры (первых 4000 байт, 200 байт с позиции 10000,  200 байт с позиции 20000, и последних 4000)
 end;

 // Описание вируса - сигнатуры для режима поиска 1
 TVirusDescr = packed record
  Name : string;                 // Имя вируса
  SizeMin, SizeMax : dword;      // Минимальный и максимальный размер
  Sign : array[1..4] of  dword;  // Сигнатуры (первых 4000 байт, 200 байт с позиции 10000,  200 байт с позиции 20000, и последних 4000)
  ScanFlags : byte;              // Флаги, управляющие сканированием
  Script    : dword;             // Скрипт (его номер)
 end;
 // Массив описаний вируса
 TSignList = array of TVirusDescr;

 // Кластер сигнатур
 TSignListCluster = record
  SizeMinMin, SizeMinMax : dword;      // Минимальный и максимальный размер
  SMin, Smax       : dword;
  SignCount        : dword;      // Расчетное кол-во сигнатур
  SignList         : TSignList;  // Массив сигнатур
 end;

 // Заголовок антивирусной базы
 TVirusBaseHeader  = packed record
  Sign           : packed array [1..3] of char; // Сигнатура
  DateTime       : TDateTime;                   // Дата и время создания базы
  MinAvzVersion  : Real48;                      // Код версии базы (для совместимости)
  RecCount       : Longint;                     // Количество записей в базе данных типа 1
  CRC            : DWORD;                       // Контрольная сумма всего файла
  code_num       : byte;                        // Байт для доп. кодирования
  NextHeaderPtr  : dword;                       // Адрес заголовка первой секции (после основной)
 end;

 // Заголовок секции базы данных (универсальный)
 TSectionHeader = packed record
  MinAvzVersion  : Real48;                      // Код версии базы (для совместимости)
  DataType       : DWord;                       // Код, определяющий тип записей блока
  RecCount       : Longint;                     // Количество записей в базе данных типа 1
  NextHeaderPtr  : dword;                       // Адрес следующего заголовка или 0, если этот последний
 end;

 // Запись таблицы объектов PE файла
 TPEObjTbl = packed record
  ObjName         : packed array [0..7] of char; // Имя объекта
  VirtSize        : DWord;
  VirtRVA         : DWord;
  PhSize          : DWord;
  PhOffset        : DWord;
  Reserved        : packed array[1..3] of DWORD;
  Object_Flags    : DWord;
 end;

 // Скрипт
 TAVZScript = record
  ScriptID       : dword;
  CompessedText  : string;
 end;

 // Нейропрофиль
 TAVZNeuro  = record
  Name          : string; // Имя
  NeuroProfile  : string; // Нейропрофиль
 end;

 // Антивирус - выполняет проверку указанного файла на вирусы
 TAntivirus = class
  FDatabasePath : string;
  private
   FVirusName: string;                  // Имя обнаруженного вируса
   FSignList   : array of TVirusDescr;  // Массив сигнатур типа AVZ-1 (размер + S1..S4)
   FScriptList : array of TAVZScript;   // Массив скриптов лечения
   FNeuroNets  : array of TAVZNeuro;    // Массив нейросетей
   FLastError  : string;
   FEvLevel    : integer;
   FMainDBDate, MaxDbDate : TDateTime;
   FVirusUnregisterDLL: boolean;
   // Очистить базу
   Function ClearDatabase : boolean;
   // Кластеризовать базу
   Function ClasterizateDatabase : boolean;
   // Загрузить бинарную базу данных
   Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
   // Загрузить бинарную базу нейросетей
   Function LoadBinNeuralFile(AFileName : string; AMainBase : boolean) : boolean;
   // Получение количества сигнатур
   function GetSignCount: integer;
   // Добавление ошибки к протоколу
   procedure AddError(s : string);
   procedure SetEvLevel(const Value: integer);
   function GetVirBaseDescription: string;
   function GetCureScrCount: integer;
   function GetNNCount: integer;
 public
  SignListClusters : array[1..AVClasterCount] of TSignListCluster;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Проверка указанного файла
  Function CheckFile(PEFileInfo : TAVZFileInfo; var AVirusDescr : TVirusDescr) : integer;
  // Получение скрипта лечения по его номеру
  Function GetCureScript(AScript : dword) : string;
  // Получение нейропрофиля по имени
  Function GetNeuroProfile(AName : string) : string;
  // Отмена регистрации библиотеки
  Function UnregisterLibrary(AFileName : string) : boolean;
  // Имя вируса
  property VirusName : string read FVirusName;
  // Признак того, что при удалении нужно отменить регистрацию DLL
  property VirusUnregisterDLL : boolean read FVirusUnregisterDLL;
  // Кол-во сигнатур
  property SignCount : integer read GetSignCount;
  property CureScrCount : integer read GetCureScrCount;
  property NNCount : integer read GetNNCount;
  // Имя последней ошибки
  property LastError : string read FLastError;
  // Уровень эвристического анализатора
  property EvLevel : integer read FEvLevel write SetEvLevel;
  // Дата основной базы
  property MainDBDate : TDateTime read FMainDBDate;
  // Описание базы
  property VirBaseDescription : string Read GetVirBaseDescription;
 end;

 // Сканер - анализатор файла. Загружает из файла данные в указанные буфера
 TFileScanner = class
  // Информация о файле
  PEFileInfo : TAVZFileInfo;
  // Буфера
  Buf1, Buf2, Buf3, Buf4, Buf5, Buf6 : packed array[0..4096] of byte;
  // Заголовки EXE
  PEImageFileHeader     : TImageFileHeader;
  PEImageOptionalHeader : TImageOptionalHeader;
  // Сбор данных о файле по его имени
  Function CheckFile(AFileName : string) : boolean; overload;
  // Сбор данных о файле по его Handle
  Function CheckFile(hFile : THandle) : boolean; overload;
 end;

 // Событие "проверка файла по базе безопасных"
 TOnFileSignCheck = procedure(AFileName : string; var AStatus : integer) of object;
 // Класс, предназначенный для проверки цифровой подписи
 TFileSignCheck = class
  FDatabasePath : string;
  // Кластеризовать базу
  Function ClasterizateDatabase : boolean;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
  private
   FLastError: string;
   // Массив подписей файлов
   FSignList     : array of TFileSign;
   // Рабочий буфер
   Buf : array[0..60000] of byte;
   // Кластеризованная база
   FileSignListClusters : array[1..AVClasterCount] of TFileSignListCluster;
   FOnFileSignCheck: TOnFileSignCheck;
   function GetSignCount: integer;
    procedure SetOnFileSignCheck(const Value: TOnFileSignCheck);
 public
  // Создание
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Вычисление CRC файла
  function CalkFileCRC(AFileName : string; var CRC, Size : DWord) : boolean;
  // Вычисление CRC файла + проверка файла по базе
  Function CheckFile(AFileName : string; ACheckFileTrust : boolean = true) : integer;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Имя последней ошибки
  property LastError : string read FLastError;
  // Кол-во сигнатур
  property SignCount : integer read GetSignCount;
  property OnFileSignCheck : TOnFileSignCheck read FOnFileSignCheck write SetOnFileSignCheck;
 end;

 // Класс с настройкой антикейлоггера
 TKeyloggerBase = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
 public
  SysDLL : TStringList;
  constructor Create(ADatabasePath : string);
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
 end;

 // Настрока UserMode анти-Rootkit
 TRootkitDLLSetup = record
   DllName      : string; // Имя DLL
   CheckFunct   : string; // Список проверяемых функций
   NoCheckFunct : string; // Список функций, проверка которых запрещена
   NoCureFunct  : string; // Список функций, восстановление которых запрещено
   DescrFunct   : string; // Описание функций
 end;

 // Глобальные настройки антируткита
 TRootkitSetup = record
   ParamName      : string; // Имя параметра
   ParamVal       : string; // Значение параметра
 end;

 // Класс с настройкой антируткита
 TRootkitBase = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
 public
  // Настройки проверки DLL
  CheckedDLLs : array of TRootkitDLLSetup;
  // Глобальные настройки
  Setup       : array of TRootkitSetup;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  Function GetParam(AName : string) : string;
 end;

 // Класс с KernelMode драйверами
 TDriverRec = record
  Naim     : string;               // Имя драйвера
  LinkName : string;               // Имя ссылки
  BinFile  : packed array of byte; // Бинарный файл
  OS_Code  : dword;                // Код ОС (битовая маска)
 end;
 TKMBase = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
 public
  Driver : array of TDriverRec;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Создание драйвера в указанном месте
  Function CreateDriverFile(ADriver, AFilename: string; ASO_Ver : dword; var ALinkName : string) : boolean;
  // Получение имени линка
  Function GetDriverLink(ADriver: string; ASO_Ver : dword; var ALinkName : string) : boolean;
 end;

 // Сигнатура поиска в файле
 TMemSign = record
  Name      : string;
  ID        : word;
  CRCLength : word;
  CRC       : DWORD;
 end;

 TMemDetectCr = record
  Token   : string;
  CmpCode : byte;
  CmpPar  : byte;
 end;
 TMemDetectCrArray = array of TMemDetectCr;
 TMemDetectRec     = record
  Name : string;
  SMin, SMax : integer;
  MainDetect : TMemDetectCrArray;
  DopDetect  : TMemDetectCrArray;
  LockDetect : TMemDetectCrArray;
 end;

 TOnMemSignFound = procedure(MemSign : TMemSign; Rel : DWORD) of object;
 TOnMemSuspFound = procedure(AVirName : string) of object;
 TMemSignArray = array of TMemSign;
 // Класс с настройкой антируткита
 TMemScanBase = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  Function LoadBinDatabaseFileD(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
  procedure AddSusp(AVirName : string);
  private
   FOnMemSignFound: TOnMemSignFound;
   FOnMemSuspFound: TOnMemSuspFound;
   procedure SetOnMemSignFound(const Value: TOnMemSignFound);
   function CheckDeteckCR(Detect : TMemDetectCrArray) : boolean;
   function GetTokenVal(AName: string; ADefVal: integer): integer;
   procedure SetOnMemSuspFound(const Value: TOnMemSuspFound);
 public
  MemScanBase    : array [0..$FF, 0..$FF] of TMemSignArray;
  MemDetectBase  : array of TMemDetectRec;
  NeuroTokenList  : TStrings;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Очистка массивов
  function ClearArray : boolean;
  // Сканирование буфера памяти
  function ScanBuf(var Buf : array of byte) : boolean;
  // Детект зловредов по набору правил
  function Detect : boolean;
 published
  property OnMemSignFound : TOnMemSignFound read FOnMemSignFound write SetOnMemSignFound;
  property OnMemSuspFound : TOnMemSuspFound read FOnMemSuspFound write SetOnMemSuspFound;
 end;

 // Скрипт
 TAVZSysChkScript = record
  CompressedName : string; // Имя
  CompressedText  : string;
 end;

 TESSystem  = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
  private
    FLastError: string;
 public
  ESScripts : array of TAVZSysChkScript;
  MainDBDate: TDateTime;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Имя последней ошибки
  property LastError : string read FLastError;
 end;

 // Класс с для эвристической проверки системы
 TSystemCheck = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
  private
    FLastError: string;
 public
  CheckScripts : array of TAVZSysChkScript;
  MainDBDate: TDateTime;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Имя последней ошибки
  property LastError : string read FLastError;
 end;

 // Класс проверки ИПУ
 TSystemIPU = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
  private
    FLastError: string;
 public
  CheckScripts : array of TAVZSysChkScript;
  MainDBDate: TDateTime;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Имя последней ошибки
  property LastError : string read FLastError;
 end;

 // Скрипты восстановления
 TSystemRepair = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
  private
    FLastError: string;
 public
  RepairScripts : array of TAVZSysChkScript;
  MainDBDate: TDateTime;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Имя последней ошибки
  property LastError : string read FLastError;
 end;

 // Скрипты бекапа
 TBackupScripts = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
  private
    FLastError: string;
 public
  BackupScripts : array of TAVZSysChkScript;
  MainDBDate: TDateTime;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Имя последней ошибки
  property LastError : string read FLastError;
 end;

 // Стандартные скрипты
 TStdScripts = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
  private
    FLastError: string;
 public
  Scripts : array of TAVZSysChkScript;
  MainDBDate: TDateTime;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Имя последней ошибки
  property LastError : string read FLastError;
 end;

 // Скрипт
 TAVZCustomScript = record
  ID              : dword;  // Номер скрипта
  CompressedName  : string; // Имя
  CompressedText  : string;
 end;
  // Скрипты без определенного назначения (для различных операций в KIS8)
 TCustomScripts = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
  private
    FLastError: string;
 public
  Scripts : array of TAVZCustomScript;
  DBDate  : TDateTime;
  constructor Create;
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase(ADBFilename : string) : boolean;
  // Имя последней ошибки
  property LastError : string read FLastError;
 end;

 // Перевод строки
 TAVZTranslate = record
  ItemID   : word;   // Код элемента
  LocalTxt : string; // Локализаованный текст
 end;

  // Кластер шаблонов перевода
 TTranslateCluster = record
  TranslateCount   : dword;      // Расчетное кол-во шаблонов перевода
  Translates       : array of TAVZTranslate;  // Массив шаблонов
 end;
 // Событие типа "требуется перевод"
 TOnTranslateEvent = function(AStr : string) : string of object;
 // Класс "Переводчик"
 TTranslate = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
  private
   FLastError: string;
   FOnTranslate: TOnTranslateEvent;
   procedure SetOnTranslate(const Value: TOnTranslateEvent);
   // Очистка кластерной таблицы
   procedure ClearClustersTable;
   // Кластеризовать базу
   Function ClasterizateDatabase : boolean;
 public
  Translates   : array of TAVZTranslate;
  TranslatesCL : array [0..$FF] of TTranslateCluster;
  MainDBDate: TDateTime;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase(ALocale : string) : boolean;
  // Перевод
  Function TranslateStr(S : string) : string;
  // Поиск перевода по коду
  Function TranslateByCode(ItemId : integer; var Res : string) : boolean;
  // Имя последней ошибки
  property LastError : string read FLastError;
  property OnTranslate : TOnTranslateEvent read FOnTranslate write SetOnTranslate;
 end;

 // Группа команд (визуальный редактор)
 TAVZCmdGroup = record
  GroupID  : integer; // Код группы
  Naim     : string;  // Наименование
  IsPublic : integer; // Признак "публичности"
 end;

 // Описание команды (визуальный редактор)
 TAVZCmdDescr = record
  ID      : integer;   // Идентификатор команды
  GroupID : integer;   // Идентификатор группы
  PrgType : integer;   // Код типа
  Naim    : string;    // имя
  CmdTxt  : string;    // Код для декларации
  Cmt     : string;    // Текстовое описание-комментарий
  ScriptCmt : string;  // Текстовый комментарий для документирования скрипта
  AVZVer  : single;    // Версия AVZ, с которой поддерживается данная команда
 end;

 // Данные VSE - Visual Script Editor
 TVSEData = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
  private
    FLastError: string;
 public
  CmdGroups : array of TAVZCmdGroup;
  CmdDescr  : array of TAVZCmdDescr;
  MainDBDate: TDateTime;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Имя последней ошибки
  property LastError : string read FLastError;
 end;

 // Класс лечения документов Office и детектирования в них червей
 TOfficeMacroBase = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
 public
  OfficeMacros : TOfficeMacros;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Проверка файла
  Function CheckFile(AFileName : string) : integer;
  // Проверка макроса
  Function CheckMacros(AFileName, AMacroName, AMacroText : string) : integer;
 end;

 // Статистика по правилу
 TRuleStat = record
  ID         : integer; // Идентификатор правила
  GroupID    : integer; // Идентификатор группы правила
  Cnt        : integer; // Количество срабатываний правила
  RF         : byte;
  ArgHashes  : array of DWORD; // Хеши аргументов для обработки повторных вызовов
 end;

 // Система выставления рейтинга - сессия анализа
 TSRSession  = class
 public
  FileName : string;
  // Статистика сработавших правил с рейтингами по ним
  RulesStat : array of TRuleStat;
  // Текущий суммарный рейтинг
  SummRating  : integer;
  // Признак того, что это Malware
  IsMalware   : boolean;
  // Код последнего из сработавших правил
  LastRule    : integer;
  constructor Create;
  destructor  Destroy; override;
  // Запрос количества срабатывания правила
  function GetRuleStatCnt(ARuleID : integer) : integer;
  // Добавление статистики (возвращает 0, если повтор, > 0 - кол-во срабатываний на уникальные аргументы)
  function AddRuleStat(ARuleID, ARuleGroup : integer; AArgHash : dword; var RSIndx : integer) : integer;
  // Запрос статистики по группе (возвращает кол-во срабатываний правила группы X)
  function GetGroupRating(AGroupID : integer) : integer;
 end;

 // Правило системы SR для API функции
 TAPIRule = record
  ID : integer;                        // Уникальный идентификатор правила
  GroupID : integer;                   // Идентификатор группы
  ArgCmpMode  : array[1..3] of byte;   // Коды режима сравнения аргументов (0 - игнорировать, 1-поиск образца, 2-поиск по справочнику)
  ArgCmpData  : array[1..3] of string;  // Данные для сравнения аргументов
  MalwareRating : array[1..3] of byte; // Рейтинги зловредности
  ISMalware : boolean                  // Признак того, что по данному правилу процесс можно считать малварью
 end;

 // Описание API функции и правил для нее
 TSRAPIRec = record
  API_ID   : integer; // Код API функции
  API_Name : string;  // Имя API функции
  Rules    : array of TAPIRule; // Массив правил для данной API функции
 end;

 TSRSpr = record
  ID   : integer;     // Код справочника
  Data : TStringList; // Данные справочника
 end;

 // Система выставления рейтинга
 TSRSystem  = class
  FDatabasePath : string;
  // Загрузить бинарную базу данных
  Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
  Function LoadBinSPRDatabaseFile(AFileName : string) : boolean;
  // Добавление ошибки к протоколу
  procedure AddError(s : string);
  // Проверка правила для аргумента
  function CheckAPIRule(SRSession : TSRSession; ARule : TAPIRule; AArgID : byte; AData : string) : boolean;
  // Вычисление хеша аргуметов
  function CalkArgHash(ARule : TAPIRule; Arg1, Arg2, ArgN : string) : dword;
  private
   FLastError: string;
 public
  MainDBDate: TDateTime;
  // Массив API функций и правил для них
  API_DB : array of TSRAPIRec;
  SPR_DB : array of TSRSpr;
  constructor Create(ADatabasePath : string);
  destructor  Destroy; override;
  procedure ClearDB;
  // Проверка по справочнику
  function CheckBySpr(ASprId : integer; AData :string) : boolean;
  // Загрузка бинарной сжатой/закодированной базы
  Function LoadBinDatabase : boolean;
  // Получение ID функции по ее имени. Если ID < 0, то на данную API функцию нет правил
  function GetFuntionIndx(AFunctionName : string) : integer;
  // Проверка по набору правил
  function GetAPICallRating(SRSession : TSRSession; AFunctIndx : integer; Arg1, Arg2, ArgN : string) : integer;
  // Получение рейтинга по строке данных IMUL
  function CheckAPIEvent(SRSession : TSRSession; AIMULData : string) : integer;
  // Имя последней ошибки
  property LastError : string read FLastError;
 end;

// Декодировать бинарные данные
Function DecodeBinData(MS : TMemoryStream; ACodeIndx : byte = 0) : boolean;
// Кодировать бинарные данные
function CodeBinData(MS: TMemoryStream; ACodeIndx : byte = 0) : boolean;

implementation

{ TAntivirus }
procedure TAntivirus.AddError(s: string);
begin
 if FLastError <> '' then FLastError := FLastError + ';';
 FLastError := FLastError + Trim(s);
end;

function TAntivirus.CheckFile(PEFileInfo : TAVZFileInfo; var AVirusDescr : TVirusDescr): integer;
var
 Clust, i, j, CheckedSign, EqualsSign : integer;
 S : string;
begin
 Result := 0;
 FVirusName := '';
 FVirusUnregisterDLL := false;
 // Методика поиска по сигнатурам вида 1
 {$R-}
 for Clust := 1 to AVClasterCount do
  if (PEFileInfo.Size >= SignListClusters[Clust].SMin) and
     (PEFileInfo.Size <= SignListClusters[Clust].Smax) then begin
   for i := 0 to length(SignListClusters[Clust].SignList) - 1 do begin
    // Проверка размера
    if (PEFileInfo.Size >= SignListClusters[Clust].SignList[i].SizeMin) and
       (PEFileInfo.Size <= SignListClusters[Clust].SignList[i].SizeMax) then begin
      CheckedSign := 0; EqualsSign := 0;
      for j := 1 to 4 do
       if SignListClusters[Clust].SignList[i].Sign[j] <> 0 then begin
        inc(CheckedSign);
        if SignListClusters[Clust].SignList[i].Sign[j] = PEFileInfo.Sign[j] then
         inc(EqualsSign);
       end;
      // Проверка наличия вируса в базе
      if (CheckedSign > 0) and (CheckedSign = EqualsSign) then begin
       Result := 1;
       FVirusName := SignListClusters[Clust].SignList[i].Name;
       AVirusDescr := SignListClusters[Clust].SignList[i];
       exit;
      end else
       if (CheckedSign > 0) and (EqualsSign > 0) and (CheckedSign <> EqualsSign) then begin
        if EqualsSign >= (3 - EvLevel) then
         if (SignListClusters[Clust].SignList[i].ScanFlags and 16) = 0 then begin  // Для этой сигнатуры разрешена эвристика
          Result := 2;
          S := '';
          for j := 1 to 4 do
           S := S + ' ' + IntToHex(PEFileInfo.Sign[j], 8);
          S := S + ' ' + inttostr(PEFileInfo.Size);
          FVirusName := SignListClusters[Clust].SignList[i].Name+ ' (' + S+')';
          AVirusDescr := SignListClusters[Clust].SignList[i];
         end;
        end;
    end;
   end;
 end;
 // Методика 1 вирусов не выявила - переходим к методике 2 - скрипты OLE объекта
 {$R+}
 if PEFileInfo.StorageType = 1 then begin

 end;
end;

function TAntivirus.ClasterizateDatabase: boolean;
var
 i, j, min, max : integer;
 ClusterStep : integer;
begin
 ClusterStep := 2048;
 min := 0;  max := ClusterStep;
 // Очистка и подготовка кластерной базы
 for i := 1 to AVClasterCount do begin
  SignListClusters[i].SizeMinMin := min;
  SignListClusters[i].SizeMinMax := max;
  SignListClusters[i].SMin := $FFFFFFFF;
  SignListClusters[i].SMax := 0;
  min := max + 1;
  max := max + ClusterStep;
  SignListClusters[i].SignCount := 0;
  SignListClusters[i].SignList := nil;
 end;
 SignListClusters[AVClasterCount].SizeMinMax := $FFFFFFFF;
 // Вычисление размеров кластеров
 for i := 0 to length(FSignList) - 1 do
  for j := 1 to AVClasterCount do
   if (FSignList[i].SizeMin >= SignListClusters[j].SizeMinMin) and
      (FSignList[i].SizeMin <= SignListClusters[j].SizeMinMax) then begin
       inc(SignListClusters[j].SignCount);
       break;
      end;
 // Выделение буферов
 for i := 1 to AVClasterCount do begin
  SetLength(SignListClusters[i].SignList, SignListClusters[i].SignCount);
  SignListClusters[i].SignCount := 0;
 end;
 // Заполнение базы
 for i := 0 to length(FSignList) - 1 do
  for j := 1 to AVClasterCount do
   if (FSignList[i].SizeMin >= SignListClusters[j].SizeMinMin) and
      (FSignList[i].SizeMin <= SignListClusters[j].SizeMinMax) then begin
       // Копирование
       SignListClusters[j].SignList[SignListClusters[j].SignCount] := FSignList[i];
       // Настройка размеров
       if SignListClusters[j].SMin > FSignList[i].SizeMin then
        SignListClusters[j].SMin := FSignList[i].SizeMin;
       if SignListClusters[j].SMax < FSignList[i].SizeMax then
        SignListClusters[j].SMax := FSignList[i].SizeMax;
       // Инкремент счетчика
       inc(SignListClusters[j].SignCount);
       break;
      end;
 FSignList := nil;
end;

function TAntivirus.ClearDatabase: boolean;
var
 i : integer;
begin
 FSignList   := nil;
 FNeuroNets  := nil;
 FScriptList := nil;
 for i := 1 to AVClasterCount do
  SignListClusters[i].SignList := nil;
 Result := true;
end;

constructor TAntivirus.Create(ADatabasePath: string);
begin
 FVirusUnregisterDLL := false;
 FDatabasePath  := NormalDir(ADatabasePath);
 FSignList      := nil;
 FScriptList    := nil;
 FNeuroNets     := nil;
end;

function CodeBinData(MS: TMemoryStream; ACodeIndx : byte = 0) : boolean;
var
 i, ResSize : integer;
 w : word;
 b : byte;
 VirusBaseHeader : TVirusBaseHeader;
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
 for i := sizeof(TVirusBaseHeader) to MS.Size - 1 do begin
  b := byte( Pointer(Longint(MS.Memory) + i)^ );
  w := i + i*7 + i*i div 2 + VirusBaseHeader.code_num * 7 + 673;
  b := b xor CodeArray[ACodeIndx, i mod $0F];
  b := (not b) xor w;
  asm ror b, 2 end;
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

function DecodeBinData(MS: TMemoryStream; ACodeIndx : byte = 0): boolean;
var
 i, ResSize : integer;
 w : word;
 b : byte;
 VirusBaseHeader : TVirusBaseHeader;
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
 for i := sizeof(TVirusBaseHeader) to MS.Size - 1 do begin
  w := word(i + i*7 + i*i div 2 + VirusBaseHeader.code_num * 7 + 673);
  b := byte( Pointer(Longint(MS.Memory) + i)^ );
  CRC := DWORD(CRC + b * i);
  asm rol b, 2 end;
  b := byte(not(b xor w));
  b := b xor CodeArray[ACodeIndx, i mod $0F];
  ResSize := 1;
  Move(b, Pointer(Longint(MS.Memory) + i)^, ResSize);
 end;
 // Цикл расчета CRC заголовка
 for i := 0 to sizeof(TVirusBaseHeader) - 1 do begin
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

destructor TAntivirus.Destroy;
var
 i : integer;
begin
 FVirusName := '';
 FLastError  := '';
 FSignList   := nil;
 FScriptList := nil;
 FNeuroNets  := nil;
 // Очистка базы
 for i := 1 to AVClasterCount do
  SignListClusters[i].SignList := nil;
 inherited;
end;

function TAntivirus.GetCureScrCount: integer;
begin
 Result := Length(FScriptList);
end;

function TAntivirus.GetCureScript(AScript: dword): string;
var
 i : integer;
begin
 Result := '';
 if AScript = 0 then exit;
 for i := length(FScriptList) - 1 downto 0 do
  if FScriptList[i].ScriptID = AScript then begin
   Result := DeCompressString(FScriptList[i].CompessedText);
   exit;
  end;
end;

function TAntivirus.GetNeuroProfile(AName: string): string;
var
 i : integer;
begin
 Result := '';
 AName := Trim(LowerCase(AName));
 for i := 0 to length(FNeuroNets) - 1 do
  if LowerCase(FNeuroNets[i].Name) = AName then begin
   Result := DeCompressString(FNeuroNets[i].NeuroProfile);
   exit;
  end;
end;

function TAntivirus.GetNNCount: integer;
begin
 Result := Length(FNeuroNets);
end;

function TAntivirus.GetSignCount: integer;
var
 SummClusterSign, i : integer;
begin
 SummClusterSign := 0;
 // Подсчет сигнатур
 for i := 1 to AVClasterCount do
  SummClusterSign := SummClusterSign + Length(SignListClusters[i].SignList);
 Result := Length(FSignList) + SummClusterSign;
end;

function TAntivirus.GetVirBaseDescription: string;
var
 S : string;
begin
 // Количество сигнатур
 Result :='$AVZ0954 - '+IntToStr(SignCount)+', ';
 // Количество нейропрофилей
 Result := Result+ '$AVZ0493 - '+IntToStr(length(FNeuroNets)) +', ';
 // Микропрограммы лечения
 Result := Result + '$AVZ0426 - '+IntToStr(length(FScriptList))+', '+S;
 Result := Result + '$AVZ0071 ' + FormatDateTime('dd.mm.yyyy hh:nn', MaxDbDate);
end;

function TAntivirus.LoadBinDatabase: boolean;
var
 SR : TSearchRec;
 Res : integer;
begin
 MaxDbDate := 0;
 FMainDBDate := 0;
 SetLength(FSignList, 0);
 ClearDatabase;
 FLastError   := '';
 Result := true;
 Res := FindFirst(NormalDir(FDatabasePath)+'main*.avz', faAnyFile, SR); //#DNL
 while Res = 0 do begin
  Result := Result and LoadBinDatabaseFile(NormalDir(FDatabasePath)+SR.Name, true);
  Res := FindNext(SR);
 end;
 FindClose(SR);
 // Загрузка доп. баз
 if FileExists(NormalDir(FDatabasePath)+'daily.avz') then //#DNL
  LoadBinDatabaseFile(NormalDir(FDatabasePath)+'daily.avz', false); //#DNL
 // Загрузка нейросетей
 if FileExists(NormalDir(FDatabasePath)+'neural.avz') then //#DNL
  LoadBinNeuralFile(NormalDir(FDatabasePath)+'neural.avz', true); //#DNL
 // Кластеризация базы
 ClasterizateDatabase;
end;

function TAntivirus.LoadBinDatabaseFile(AFileName: string; AMainBase : boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 SectionHeader : TSectionHeader;      // Субзаголовок антивирусной базы
 VirusDescr : TVirusDescr;            // Описание вируса
 RecFlags, RecExtFlags, b1, b2   : byte;
 VirName, LastVirName, s : string; // Текущее и предыдущее имя вируса
 buf_dw : dword;
 buf_w  : word;
 i, j, StartPosInBase : integer;
 Res : boolean;
 AVZScript : TAVZScript;
 LastSize : dword;
begin
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit; //#DNL
 // Блокировка по дате
 if not(AMainBase) and (VirusBaseHeader.DateTime < MainDBDate) then exit;
 if VirusBaseHeader.DateTime > MaxDbDate then
  MaxDbDate := VirusBaseHeader.DateTime;
 // Запоминание даты основного файла
 if AMainBase then
  if VirusBaseHeader.DateTime > FMainDBDate then
   FMainDBDate := VirusBaseHeader.DateTime;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 VirName := ''; LastVirName := '';
 // Последний размер для архивации
 LastSize := 0;
 // Чтение данных
 StartPosInBase := length(FSignList);
 // Добавление информации к таблице
 SetLength(FSignList, length(FSignList)+VirusBaseHeader.RecCount);
 for i := 1 to VirusBaseHeader.RecCount do begin
  RecExtFlags := 0;
  // Чтение флагов записи
  MS.Read(RecFlags, 1);
  // Чтение заголовка расширенной компрессии записи (если он есть)
  if (RecFlags and $80) > 0 then
   MS.Read(RecExtFlags, 1);
  // Разархивация имени
  // Бит 1 флагов = 1 - имя совпадает с предыдущим
  if (RecFlags and 3) > 0 then begin
   if (RecFlags and 2) = 2 then
    VirName := LastVirName
   else
    // Бит 0 флагов = 1 - имя частично совпадает с предыдущим
    if (RecFlags and 1) = 1 then begin
     MS.Read(b1, 1); // Длина строки
     MS.Read(b2, 1); // Кол-во повторяющихся байт
     SetLength(s, b1 - 1);
     if b1 > 1 then
      MS.Read(S[1], b1-1);
     VirName := copy(LastVirName, 1, b2) + S;
    end;
  end else begin
   MS.Read(b1, 1); // Длина строки
   SetLength(VirName, b1);
   MS.Read(VirName[1], b1);
  end;
  // *************** Процедура чтения размера и сигнатур для вируса ***********
  // Имя разархивировано, запоминаем его
  VirusDescr.Name := VirName;
  // Запоминаем флаги
  VirusDescr.ScanFlags := RecFlags;
  LastVirName     := VirName;
  // Чтение размера
  if (RecFlags and 4) = 4 then begin // SMin = SMax
   if (RecFlags and 8) = 0 then      // Дельта-кодирование не применено ?
    MS.Read(buf_dw, 4)
   else begin                        // Режим дельта-кодирования
    MS.Read(buf_w, 2);
    buf_dw := buf_w + LastSize;
   end;
   VirusDescr.SizeMin := buf_dw;
   VirusDescr.SizeMax := buf_dw;
  end else begin                     // SMin != SMax
   if (RecFlags and 8) = 0 then      // Дельта-кодирование SMin не применено ?
    MS.Read(buf_dw, 4)
   else begin                        // Дельта-кодирование SMin
    MS.Read(buf_w, 2);
    buf_dw := buf_w + LastSize;
   end;
   VirusDescr.SizeMin := buf_dw;
   if (RecExtFlags and 2) = 0 then
    MS.Read(buf_dw, 4)
   else begin
    MS.Read(buf_w, 2); buf_dw := buf_w;
   end;
   VirusDescr.SizeMax := buf_dw;
  end;
  LastSize := VirusDescr.SizeMin;
  if LastSize = 30212 then begin
   LastSize := LastSize + 2-2;
  end;
  // ***** Загрузка сигнатур *****
  // Сигнатура S1
  if (RecExtFlags and 4) = 0 then begin
   MS.Read(buf_dw, 4); VirusDescr.Sign[1] := buf_dw;
  end else VirusDescr.Sign[1] := 0;
  // Сигнатура S2
  if (RecExtFlags and 8) = 0 then begin
   MS.Read(buf_dw, 4); VirusDescr.Sign[2] := buf_dw;
  end else VirusDescr.Sign[2] := 0;
  // Сигнатура S3
  if (RecExtFlags and 16) = 0 then begin
   MS.Read(buf_dw, 4); VirusDescr.Sign[3] := buf_dw;
  end else VirusDescr.Sign[3] := 0;
  // Сигнатура S4
  if (RecExtFlags and 32) = 0 then begin
   MS.Read(buf_dw, 4); VirusDescr.Sign[4] := buf_dw;
  end else VirusDescr.Sign[4] := 0;
  // Чтение скрипта
  if (RecFlags and 32) > 0 then begin
   MS.Read(VirusDescr.Script, 4);
  end else VirusDescr.Script := 0;
  // Запись считанной строки в память
  FSignList[StartPosInBase] := VirusDescr;
  inc(StartPosInBase);
 end;
 // ******** Загрузка скриптов лечения ******
 if MS.Read(SectionHeader, sizeof(SectionHeader)) =  sizeof(SectionHeader) then begin
  // Чтение данных
  for i := 0 to SectionHeader.RecCount - 1 do begin
   // Чтение кода скрипта
   MS.Read(buf_dw, 4);
   AVZScript.ScriptID := buf_dw;
   // Чтение размера сжатого скрипта
   MS.Read(buf_w, 2);
   SetLength(AVZScript.CompessedText, buf_w);
   MS.Read(AVZScript.CompessedText[1], buf_w);
   StartPosInBase := -1;
   for j := 0 to length(FScriptList)-1 do
    if FScriptList[j].ScriptID = AVZScript.ScriptID then begin
     StartPosInBase := j;
     Break;
    end;
   if StartPosInBase >= 0 then begin
    FScriptList[StartPosInBase] := AVZScript;
   end else begin
    // Добавление информации к таблице
    SetLength(FScriptList, length(FScriptList)+1);
    FScriptList[length(FScriptList)-1] := AVZScript;
   end;
  end;
 end;
 MS.Free;
 Result := true
end;

function TAntivirus.LoadBinNeuralFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 i, StartPosInBase : integer;
 Res : boolean;
 b  : byte;
 w  : word;
begin
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 StartPosInBase := length(FNeuroNets);
 // Добавление информации к таблице
 SetLength(FNeuroNets, length(FNeuroNets)+VirusBaseHeader.RecCount);
 for i := 1 to VirusBaseHeader.RecCount do begin
  // Загрузка имени
  MS.Read(b, 1);
  SetLength(FNeuroNets[StartPosInBase].Name, b);
  MS.Read(FNeuroNets[StartPosInBase].Name[1], b);
  // Загрузка нейропрофиля
  MS.Read(w, 2);
  SetLength(FNeuroNets[StartPosInBase].NeuroProfile, w);
  MS.Read(FNeuroNets[StartPosInBase].NeuroProfile[1], w);
  // Перемещаемся на очередную запись
  inc(StartPosInBase);
 end;
 Result := true;
 MS.Free;
end;

procedure TAntivirus.SetEvLevel(const Value: integer);
begin
 FEvLevel := Value;
end;

function TAntivirus.UnregisterLibrary(AFileName: string): boolean;
type
 TDllUnregisterServerProc = procedure; stdcall;
var
 hLibrary : THandle;
 DllUnregisterServerProc : TDllUnregisterServerProc;
begin
 Result := false;
 // Загрузка баблиотеки
 hLibrary := LoadLibrary(PChar(AFileName));
 // Библиотеку удалось открыть
 if hLibrary = 0 then exit;
 try
  // Поиск функции DllUnregisterServer - если она есть, то возможна отмена регистрации
  @DllUnregisterServerProc := GetProcAddress(hLibrary, 'DllUnregisterServer');
  // Функция найдена - выполняем отмену регистрации
  if Assigned(DllUnregisterServerProc) then begin
   try
    DllUnregisterServerProc;
   except end;
   Result := true;
  end;
 finally
  FreeLibrary(hLibrary);
 end;
end;

{ TFileScanner }

function TFileScanner.CheckFile(AFileName: string): boolean;
var
 HFile : THandle;
begin
 Result := false;
 // Получение имени файла
 PEFileInfo.FileName := LowerCase(ExtractFileName(AFileName));
 // Открытие файла (режим "только чтение")
 hFile := CreateFile(PChar(AFileName),
                     GENERIC_READ, FILE_SHARE_READ, nil,
                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL or FILE_FLAG_RANDOM_ACCESS, 0);

 // Попытка открыть файл провалилась - попробуем усилить права доступа
 if hFile = INVALID_HANDLE_VALUE then
  hFile := CreateFile(PChar(AFileName),
                     READ_CONTROL, 0, nil,
                     OPEN_EXISTING,  FILE_FLAG_BACKUP_SEMANTICS, 0);
 // Файл открылся - запрос информации
 if hFile <> INVALID_HANDLE_VALUE then begin
  try
   Result := CheckFile(hFile);
  except end;
  // Закрытие Handle
  CloseHandle(hFile);
 end;
end;

function TFileScanner.CheckFile(hFile: THandle): boolean;
var
 PE_Header_Rel,
 EntryPointRWA : DWORD;
 SzOfOptHdr, NOfSections : word;
 BytesReaded : Cardinal;
 i    : integer;
 Sign : dword;
 PEObjTbl : TPEObjTbl;
begin
 Result := false;
 PEFileInfo.PE_EntryPoint := 0;
 PEFileInfo.StorageType   := 0;
 PEFileInfo.PE_HeaderSize := 0;
 // Получение размера файла
 PEFileInfo.Size := GetFileSize(hFile, nil);
 // Перемещение на начало файла
 FileSeek(hFile, 0, 0);
 // Чтение сигнатуры 1 (2000 байт с начала)
 if not(ReadFile(HFile, Buf1, 2000, BytesReaded, 0)) then exit;
 // Проверка сигнатуры EXE (MZ или ZM по смещению 00h)
 PEFileInfo.IsEXEFile := ((Buf1[0] = $4D) and (Buf1[1] = $5A)) or ((Buf1[0] = $5A) and (Buf1[1] = $4D));
 // ***** Проверка сигнатуры PE (PE по смещению из dword 3Ch) *****
 if PEFileInfo.IsEXEFile then begin
  // Чтение заголовка EXE файла
  Move(Buf1[0], PEImageFileHeader, SizeOf(PEImageFileHeader));
  // Поиск смещения до заголовка
  Move(Buf1[$3C], PE_Header_Rel, 4);
  if PE_Header_Rel < SizeOf(Buf1) then
   PEFileInfo.IsPEFile := ((Buf1[PE_Header_Rel] = $50) and (Buf1[PE_Header_Rel+1] = $45))
    else PEFileInfo.IsPEFile := false;
  if PEFileInfo.IsPEFile then begin
   if PE_Header_Rel + SizeOF(PEImageOptionalHeader) < SizeOf(Buf1) then
    move(Buf1[PE_Header_Rel+$18], PEImageOptionalHeader, SizeOF(PEImageOptionalHeader))
     else ZeroMemory(@PEImageOptionalHeader, SizeOF(PEImageOptionalHeader));
  end;
 end;
 // проверка, не является ли файл OLE хранилицем
 if (Buf1[0] = $D0) and (Buf1[1] = $CF) and (Buf1[2] = $11) and (Buf1[3] = $E0) and (Buf1[4] = $A1) and (Buf1[5] = $B1) then
  PEFileInfo.StorageType   := 1;
 // Расчет сигнатуры 1
 Sign := 0;
 if BytesReaded > 0 then
 {$R-}{$I-}for i := 0 to BytesReaded - 1 do Sign := Sign + dword(Buf1[i]*i);{$R+}{$I+}
 PEFileInfo.Sign[1] := Sign;
 // Чтение сигнатуры 2 (2000 байт с конца)
 FileSeek(hFile, -2000, 2);
 ReadFile(HFile, Buf2, 2000, BytesReaded, 0);
 // Расчет сигнатуры 2
 Sign := 0;
 if BytesReaded > 0 then
 {$R-}{$I-}for i := 0 to BytesReaded - 1 do Sign := Sign + DWord(Buf2[i]*i);{$R+}{$I+}
 PEFileInfo.Sign[2] := Sign;
 // Чтение сигнатуры 3 (200 байт с позиции 5000)
 FileSeek(hFile, 5000, 0);
 ReadFile(HFile, Buf3, 200, BytesReaded, 0);
 // Расчет сигнатуры 3
 Sign := 0;
 if BytesReaded > 0 then
 {$R-}{$I-}for i := 0 to BytesReaded - 1 do Sign := Sign + DWord(Buf3[i]*i);{$R+}{$I+}
 PEFileInfo.Sign[3] := Sign;
 // Чтение сигнатуры 4 (200 байт с позиции 10000)
 FileSeek(hFile, 10000, 0);
 ReadFile(HFile, Buf4, 200, BytesReaded, 0);
 // Расчет сигнатуры 4
 Sign := 0;
 if BytesReaded > 0 then
 {$R-}{$I-}for i := 0 to BytesReaded - 1 do Sign := Sign + DWord(Buf4[i]*i);{$R+}{$I+}
 PEFileInfo.Sign[4] := Sign;
 {$R-}
 // Чтение фрагмента по точке входа
 if PEFileInfo.IsPEFile then begin
  // Считываем RWA точки входа
  move(Buf1[PE_Header_Rel+$28], EntryPointRWA, 4);
  // Считываем резмер переменной части заголовка
  move(Buf1[PE_Header_Rel+$14], SzOfOptHdr, 2);
  // Считываем кол-во секций
  move(Buf1[PE_Header_Rel+$06], NOfSections, 2);
  // Процесс поиска по секциям
  if NOfSections < 50 then begin
    for i := 0 to NOfSections - 1 do begin
     // Читаем описание секции
     move(Buf1[PE_Header_Rel+$18+SzOfOptHdr+sizeof(PEObjTbl)*i], PEObjTbl, sizeof(PEObjTbl));
     // Проверяем, попало ли RVA в зону секции
     if (EntryPointRWA >= PEObjTbl.VirtRVA) and (EntryPointRWA < PEObjTbl.VirtRVA + PEObjTbl.VirtSize) then begin
      // Начало секции = ([RWA точки входа] - [RWA секции]) + [физ. положение секции на диске]
      PEFileInfo.PE_EntryPoint := EntryPointRWA - PEObjTbl.VirtRVA + PEObjTbl.PhOffset;
      FileSeek(hFile, PEFileInfo.PE_EntryPoint, 0);
      // Читаем 4 КБ кода начиная от точки входа
      ReadFile(HFile, Buf5, 4096, BytesReaded, 0);
      // Завершаем цикл поиска и считывания кода в точке входа
      Break;
     end;
   end;
   // Определение размера файла по заголовку
   PEFileInfo.PE_HeaderSize := 0;
   for i := 0 to NOfSections - 1 do begin
     // Читаем описание секции
     move(Buf1[PE_Header_Rel+$18+SzOfOptHdr+sizeof(PEObjTbl)*i], PEObjTbl, sizeof(PEObjTbl));
     if PEFileInfo.PE_HeaderSize < PEObjTbl.PhSize + PEObjTbl.PhOffset then
      PEFileInfo.PE_HeaderSize :=  PEObjTbl.PhSize + PEObjTbl.PhOffset;
   end;
   // Читаем 4 КБ кода начиная от точки входа - это данные после EXE файла
   if PEFileInfo.PE_HeaderSize < PEFileInfo.Size then begin
    FileSeek(hFile, PEFileInfo.PE_HeaderSize, 0);
    ReadFile(HFile, Buf6, 128, BytesReaded, 0);
   end else
    ZeroMemory(@Buf6[0], 128);
  end;
 end;
 {$R+}
 Result := true;
end;

{ TFileSignCheck }

procedure TFileSignCheck.AddError(s: string);
begin
 if FLastError <> '' then FLastError := FLastError + ';';
 FLastError := FLastError + s;
end;

function TFileSignCheck.CalkFileCRC(AFileName: string; var CRC,
  Size: DWord): boolean;
var
 FS  : TFileStream;
 Res, i, poz : integer;
begin
 Result := false;
 CRC := 0;
 if AFileName = '' then exit;
 FS := nil;
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

function TFileSignCheck.CheckFile(AFileName: string; ACheckFileTrust : boolean = true): integer;
var
 i, Clust : integer;
 CRC, FSize : DWord;
begin
 Result := -1;
 // Задержка (применяется на очень медленных ПК для снижения нагрузки)
 if SleepScanTime > 0 then Sleep(SleepScanTime);
 // Попытка расчета CRC файла
 if not(CalkFileCRC(AFileName, CRC, FSize)) then
  exit;
 // Поиск файла в базе
 for Clust := 1 to AVClasterCount do
  if (FSize >= FileSignListClusters[Clust].SMin) and
     (FSize <= FileSignListClusters[Clust].Smax) then begin
       for i := 0 to Length(FileSignListClusters[Clust].FileSignList) - 1 do
        if FileSignListClusters[Clust].FileSignList[i].FileSize = FSize then
         if FileSignListClusters[Clust].FileSignList[i].FileCRC = CRC then begin
          Result := 0;
          exit;
         end;
     // В найденном кластере файл не найден - выход
     Break;
   end;
 // Проверка по базе безопасных MS
 if (Result <> 0) and (ACheckFileTrust) then
  if CheckFileTrust(AFileName) then
   Result := 0;
 // Вызов события "проверка по базе безопасных"
 if Assigned(FOnFileSignCheck) then
  FOnFileSignCheck(AFileName, Result);
end;

function TFileSignCheck.ClasterizateDatabase: boolean;
var
 i, j, min, max : integer;
 ClusterStep : integer;
begin
 ClusterStep := 4096;
 min := 0;  max := ClusterStep;
 // Очистка и подготовка кластерной базы
 for i := 1 to AVClasterCount do begin
  FileSignListClusters[i].SMin := min;
  FileSignListClusters[i].SMax := max;
  min := max + 1;
  max := max + ClusterStep;
  FileSignListClusters[i].SignCount := 0;
  FileSignListClusters[i].FileSignList := nil;
 end;
 FileSignListClusters[AVClasterCount].SMax := $FFFFFFFF;
 // Вычисление размеров кластеров
 for i := 0 to length(FSignList) - 1 do
  for j := 1 to AVClasterCount do
   if (FSignList[i].FileSize >= FileSignListClusters[j].SMin) and
      (FSignList[i].FileSize <= FileSignListClusters[j].SMax) then begin
       inc(FileSignListClusters[j].SignCount);
       break;
      end;
 // Выделение буферов
 for i := 1 to AVClasterCount do begin
  SetLength(FileSignListClusters[i].FileSignList, FileSignListClusters[i].SignCount);
  FileSignListClusters[i].SignCount := 0;
 end;
 // Заполнение базы
 for i := 1 to length(FSignList) - 1 do
  for j := 1 to AVClasterCount do
   if (FSignList[i].FileSize >= FileSignListClusters[j].SMin) and
      (FSignList[i].FileSize <= FileSignListClusters[j].Smax) then begin
       // Копирование
       FileSignListClusters[j].FileSignList[FileSignListClusters[j].SignCount] := FSignList[i];
       // Инкремент счетчика
       inc(FileSignListClusters[j].SignCount);
       break;
      end;
 // Очистка исходной базы      
 FSignList := nil;
end;

constructor TFileSignCheck.Create(ADatabasePath: string);
begin
 FSignList := nil;
 FOnFileSignCheck := nil;
 FDatabasePath := NormalDir(ADatabasePath);
end;

destructor TFileSignCheck.Destroy;
var
 i : integer;
begin
 FSignList := nil;
 // Очистка базы
 for i := 1 to AVClasterCount do
  FileSignListClusters[i].FileSignList := nil;
 inherited;
end;

function TFileSignCheck.GetSignCount: integer;
var
 SummClusterSign, i : integer;
begin
 SummClusterSign := 0;
 // Подсчет сигнатур
 for i := 1 to AVClasterCount do
  SummClusterSign := SummClusterSign + Length(FileSignListClusters[i].FileSignList);
 Result := Length(FSignList) + SummClusterSign;
end;

function TFileSignCheck.LoadBinDatabase: boolean;
var
 SR  : TSearchRec;
 Res : integer;
 i : integer;
begin
 Result := false;
 SetLength(FSignList, 0);
 // Очистка базы
 FSignList    := nil;
 for i := 1 to AVClasterCount do
  FileSignListClusters[i].FileSignList := nil;
 FLastError   := '';
 Res := FindFirst(FDatabasePath + 'signf*.avz', faAnyFile, SR);
 while Res = 0 do begin
  if (SR.Attr and faDirectory) = 0 then
   if FileExists(FDatabasePath + SR.Name) then
    Result := LoadBinDatabaseFile(FDatabasePath + SR.Name, true);
  Res := FindNext(SR);
 end;
 FindClose(SR);
 // Кластеризация базы
 ClasterizateDatabase;
 Result := true;
end;

function TFileSignCheck.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 i, StartPosInBase : integer;
 Res : boolean;
 b, b1  : byte;
 w  : word;
 dw : dword;
 LastSize : dword;
begin
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 StartPosInBase := length(FSignList);
 // Добавление информации к таблице
 SetLength(FSignList, length(FSignList)+VirusBaseHeader.RecCount);
 LastSize := 0;
 for i := 1 to VirusBaseHeader.RecCount do begin
  MS.Read(b, 1);
  if (b and $C0) = 0 then begin
   FSignList[StartPosInBase].FileSize := b + LastSize;
  end else
  if (b and $C0) = 64 then begin
   MS.Read(b1, 1);
   w := MakeWord(b1, b and not(64+128));
   FSignList[StartPosInBase].FileSize := w + LastSize;
  end else
  if (b and $C0) = 128 then begin
   MS.Read(w, 2);
   dw := MakeLong(w, b and not(64+128));
   FSignList[StartPosInBase].FileSize := dw + LastSize;
  end else begin
   MS.Read(dw, 4);
   FSignList[StartPosInBase].FileSize := dw + LastSize;
  end;

  MS.Read(dw, 4);
  FSignList[StartPosInBase].FileCRC := dw;
  // Запоминаем последний размер
  LastSize := FSignList[StartPosInBase].FileSize;
  inc(StartPosInBase);
 end;
 MS.Free;
 Result := true;
end;

procedure TFileSignCheck.SetOnFileSignCheck(const Value: TOnFileSignCheck);
begin
  FOnFileSignCheck := Value;
end;

{ TKeyloggerBase }

procedure TKeyloggerBase.AddError(s: string);
begin
 ;
end;

constructor TKeyloggerBase.Create(ADatabasePath: string);
begin
 SysDLL := TStringList.Create;
 FDatabasePath := ADatabasePath;
end;

function TKeyloggerBase.LoadBinDatabase: boolean;
begin
 SysDLL.Clear;
 Result := LoadBinDatabaseFile(FDatabasePath + 'keylogger.avz', true);
end;

function TKeyloggerBase.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i : integer;
 Res : boolean;
 buf_b  : byte;
 buf_w  : word;
begin
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 try
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 for i := 1 to VirusBaseHeader.RecCount do begin
  MS.Read(buf_b, 1);
  SetLength(S, buf_b);
  MS.Read(S[1], buf_b);
  MS.Read(buf_w, 2);
  SysDLL.AddObject(Trim(S), pointer(buf_w));
 end;
 MS.Free;
 Result := true;
 except
  Result := false;
 end;
end;

{ TRootkitBase }

procedure TRootkitBase.AddError(s: string);
begin
 ;
end;

constructor TRootkitBase.Create(ADatabasePath: string);
begin
 FDatabasePath := ADatabasePath;
 CheckedDLLs := nil;
 Setup       := nil;
end;

destructor TRootkitBase.Destroy;
begin
  CheckedDLLs := nil;
  Setup := nil;
  inherited;
end;

function TRootkitBase.GetParam(AName: string): string;
var
 i : integer;
begin
 AName := Trim(UpperCase(AName));
 Result := '';
 for i := 0 to length(Setup)-1 do
  if Setup[i].ParamName = AName then begin
   Result := Setup[i].ParamVal;
   exit;
  end;
end;

function TRootkitBase.LoadBinDatabase: boolean;
begin
 CheckedDLLs := nil;
 Setup       := nil;
 Result := LoadBinDatabaseFile(FDatabasePath + 'rootkit.avz', true);
 if not(Result) then
  Result := LoadBinDatabaseFile(FDatabasePath + 'rk.avz', true);
 if not(Result) then
  Result := LoadBinDatabaseFile(FDatabasePath + 'checkrk.avz', true);
end;

function TRootkitBase.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i, StartPosInBase : integer;
 Res : boolean;
 buf_w  : word;
begin
 Result := false;
 try
   // Загрузка файла в память
   MS := TMemoryStream.Create;
   MS.LoadFromFile(AFileName);
   // Декодирование и проверка CRC
   Res := DecodeBinData(MS);
   if Not(Res) then begin
    AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
    exit;
   end;
   MS.Seek(0,0);
   // Чтение заголовка
   MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
   if VirusBaseHeader.Sign <> 'AVZ' then exit;
   // Проверка, поддерживается ли данная версия базы
   if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
    AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
    exit;
   end;
   // Чтение данных
   StartPosInBase := length(CheckedDLLs);
   // Добавление информации к таблице
   SetLength(CheckedDLLs, length(CheckedDLLs)+VirusBaseHeader.RecCount);
   // Чтение данных
   for i := 1 to VirusBaseHeader.RecCount do begin
    // Чтение имени DLL
    MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
    CheckedDLLs[StartPosInBase].DllName := Trim(S);
    // Чтение проверяемых функций
    MS.Read(buf_w, 2);
    S := '';
    if buf_w > 0 then begin
     SetLength(S, buf_w);
     MS.Read(S[1], buf_w);
    end;
    CheckedDLLs[StartPosInBase].CheckFunct := Trim(S);
    // Чтение непроверяемых функций
    MS.Read(buf_w, 2);
    S := '';
    if buf_w > 0 then begin
     SetLength(S, buf_w);
     MS.Read(S[1], buf_w);
    end;
    CheckedDLLs[StartPosInBase].NoCheckFunct := Trim(S);
    // Чтение функций, лечение которых запрещено
    MS.Read(buf_w, 2);
    S := '';
    if buf_w > 0 then begin
     SetLength(S, buf_w);
     MS.Read(S[1], buf_w);
    end;
    CheckedDLLs[StartPosInBase].NoCureFunct := Trim(S);
    // Чтение описания
    MS.Read(buf_w, 2);
    S := '';
    if buf_w > 0 then begin
     SetLength(S, buf_w);
     MS.Read(S[1], buf_w);
    end;
    CheckedDLLs[StartPosInBase].DescrFunct := Trim(S);
    inc(StartPosInBase);
   end;

   // Чтение данных
   StartPosInBase := length(Setup);
   // Добавление информации к таблице
   SetLength(Setup, length(Setup)+VirusBaseHeader.NextHeaderPtr);
   // Чтение настроек
   for i := 1 to VirusBaseHeader.NextHeaderPtr do begin
    // Чтение имени параметра
    MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
    Setup[StartPosInBase].ParamName := UpperCase(Trim(S));
    // Чтение проверяемых функций
    MS.Read(buf_w, 2);
    if buf_w > 0 then begin
     SetLength(S, buf_w);
     MS.Read(S[1], buf_w);
     Setup[StartPosInBase].ParamVal := S;
    end;
    inc(StartPosInBase);
   end;
   Result := true;
  except
   Result := false;
  end;
end;

{ FOfficeMacroBase }

procedure TOfficeMacroBase.AddError(s: string);
begin

end;

function TOfficeMacroBase.CheckFile(AFileName: string): integer;
var
 OLEStorage, OLEStorageMacros, OLEStorageVBA : IStorage;
 Stream      : IStream;
 S           : WideString;
 MS          : TMemoryStream;
 STATSTG     : tagSTATSTG;
 i, lReaded  : longint;
begin
 Result := 0;
 S := AFileName;
 if StgOpenStorage(PWideChar(S), nil,
                STGM_READ or STGM_SHARE_EXCLUSIVE,
                nil, 0, OLEStorage) <> S_OK then begin
                 OLEStorage := nil;
                 exit;
                end;
 if OLEStorage = nil then exit;
 // Открытие папки с макросами
 OLEStorage.OpenStorage('Macros', nil,
                       STGM_READ or STGM_SHARE_EXCLUSIVE,
                       nil, 0, OLEStorageMacros);
 if OLEStorageMacros = nil then begin
  OLEStorage.OpenStorage('_VBA_PROJECT_CUR', nil,
                        STGM_READ or STGM_SHARE_EXCLUSIVE,
                        nil, 0, OLEStorageMacros);
 end;
 // Если такая папка Macros есть, то поиск VBA
 if OLEStorageMacros <> nil then begin
  OLEStorageMacros.OpenStorage('VBA', nil,
                        STGM_READ or STGM_SHARE_EXCLUSIVE,
                        nil, 0, OLEStorageVBA);
  // VBA найден - поиск потока _VBA_PROJECT - оглавления проекта
  if OLEStorageVBA <> nil then begin
    OLEStorageVBA.OpenStream('_VBA_PROJECT', nil, STGM_READ or STGM_SHARE_EXCLUSIVE,
                                0, Stream);
    // Оглавление проекта обнаружено ?
    if Stream <> nil then begin
     // Чтение оглавления
     MS   := TMemoryStream.Create;
     Stream.Stat(STATSTG, STATFLAG_DEFAULT);
     MS.Size := STATSTG.cbSize;
     Stream.Read(MS.Memory, STATSTG.cbSize, @lReaded);
     Stream := nil;
     // Парсинг оглавления
     OfficeMacros.ParseVBAProjectHeader(MS);
     MS.Free;
     // Цикл анализа элементов оглавления
     for i := 0 to length(OfficeMacros.MacrosList)-1 do begin
      // Имя потока объекта известно ?
      S := OfficeMacros.MacrosList[i].Name;
      if S = '' then Continue;
      // Пробуем открыть поток по имени
      OLEStorageVBA.OpenStream(PWideChar(S), nil, STGM_READ or STGM_SHARE_EXCLUSIVE,
                                 0, Stream);
      if Stream = nil then Continue;
      // Поток открыт - читаем данные
      MS   := TMemoryStream.Create;
      Stream.Stat(STATSTG, STATFLAG_DEFAULT);
      MS.Size := STATSTG.cbSize;
      Stream.Read(MS.Memory, STATSTG.cbSize, @lReaded);
      // Проводим декомпрессию макроса
      try
       S := OfficeMacros.DecompressVBA(MS, OfficeMacros.MacrosList[i].Rel);
      except
       AddError(AFileName + ' ' +IntToStr(i)+' $AVZ1168');
      end;
      // Вызываем проверку макроса
      CheckMacros(AFileName, OfficeMacros.MacrosList[i].Name, S);
     end;
   end;
  end;
 end;
 OLEStorage := nil;
end;

constructor TOfficeMacroBase.Create(ADatabasePath: string);
begin
 OfficeMacros := TOfficeMacros.Create;
end;

function TOfficeMacroBase.LoadBinDatabase: boolean;
begin
 Result := true;
end;

function TOfficeMacroBase.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
begin
 Result := true;
end;

function TOfficeMacroBase.CheckMacros(AFileName, AMacroName, AMacroText: string): integer;
var
 Lines : TStrings;
 i     : integer;
begin
 Result := -1;
 Lines := TStringList.Create;
 // Переводим текст макроса к нижнему регистру и вырезаем все немечатаемые знаки до и после
 Lines.Text := LowerCase(AMacroText);
 // Цикл нормализации
 i := 0;
 while i < Lines.Count do begin
  // Убираем пробелы в каждой строке
  Lines[i] := Trim(Lines[i]);
  // Если строка отсутствует - вырезаем ее
  if Lines[i] = '' then begin Lines.Delete(i); continue; end;
  // Вырезаем явные комментарии ??
  if Lines[i][1] = '''' then begin Lines.Delete(i); continue; end;
  inc(i);
 end;
 //Lines.SaveToFile('c:\1\'+ExtractFileName(AFileName)+'_'+AMacroName+'.txt');
end;

{ TSystemCheck }

procedure TSystemCheck.AddError(s: string);
begin
 if FLastError <> '' then FLastError := FLastError + ';';
 FLastError := FLastError + s;
end;

constructor TSystemCheck.Create(ADatabasePath: string);
begin
 FDatabasePath := ADatabasePath;
 CheckScripts  := nil;
end;

destructor TSystemCheck.Destroy;
begin
 CheckScripts := nil;
 inherited;
end;

function TSystemCheck.LoadBinDatabase: boolean;
begin
 Result := false;
 CheckScripts  := nil;
 if FileExists(FDatabasePath + 'syscheck.avz') then
  Result := LoadBinDatabaseFile(FDatabasePath + 'syscheck.avz', true);
end;

function TSystemCheck.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i,StartPosInBase : integer;
 Res : boolean;
 buf_w  : word;
begin
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 StartPosInBase := length(CheckScripts);
 // Добавление информации к таблице
 SetLength(CheckScripts, length(CheckScripts)+VirusBaseHeader.RecCount);
 if AMainBase then
  MainDBDate := VirusBaseHeader.DateTime;
 // Чтение данных
 for i := 1 to VirusBaseHeader.RecCount do begin
  // Чтение имени скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  CheckScripts[StartPosInBase].CompressedName := S;
  // Чтение скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  CheckScripts[StartPosInBase].CompressedText := S;
  inc(StartPosInBase);
 end;
 Result := true;
end;

{ TSystemRepair }

procedure TSystemRepair.AddError(s: string);
begin
 if FLastError <> '' then FLastError := FLastError + ';';
 FLastError := FLastError + s;
end;

constructor TSystemRepair.Create(ADatabasePath: string);
begin
 FDatabasePath := ADatabasePath;
 RepairScripts  := nil;
end;

destructor TSystemRepair.Destroy;
begin
  RepairScripts := nil;
  inherited;
end;

function TSystemRepair.LoadBinDatabase: boolean;
begin
 Result := false;
 RepairScripts  := nil;
 if FileExists(FDatabasePath + 'repair.avz') then
  Result := LoadBinDatabaseFile(FDatabasePath + 'repair.avz', true);
end;

function TSystemRepair.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i, StartPosInBase : integer;
 Res : boolean;
 buf_w  : word;
begin
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 StartPosInBase := length(RepairScripts);
 // Добавление информации к таблице
 SetLength(RepairScripts, length(RepairScripts)+VirusBaseHeader.RecCount);
 if AMainBase then
  MainDBDate := VirusBaseHeader.DateTime;
 // Чтение данных
 for i := 1 to VirusBaseHeader.RecCount do begin
  // Чтение имени скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  RepairScripts[StartPosInBase].CompressedName := S;
  // Чтение скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  RepairScripts[StartPosInBase].CompressedText := S;
  inc(StartPosInBase);
 end;
 Result := true;
end;

{ TMemScanBase }

procedure TMemScanBase.AddError(s: string);
begin
 ;
end;

function TMemScanBase.GetTokenVal(AName: string;
  ADefVal: integer): integer;
var
 BoolMode, QueryMode : boolean;
 Indx, i     : integer;
 S : string;
begin
 Result := ADefVal;
 AName := Trim(UpperCase(AName));
 BoolMode  := false;
 QueryMode := false;
 // Режим округления
 if copy(AName, length(AName), 1) = '#' then begin
  delete(AName, length(AName), 1);
  BoolMode := true;
 end;
 // Режим округления
 if copy(AName, length(AName), 1) = '$' then begin
  delete(AName, length(AName), 1);
  QueryMode := true;
 end;
 if not(QueryMode) then begin
  Indx := NeuroTokenList.IndexOf(AName);
  if Indx < 0 then exit;
  Result := integer(NeuroTokenList.Objects[Indx]);
 end else begin
   S := UpperCase(AName);
   Result := 0;
   for i := 0 to NeuroTokenList.Count - 1 do
    if pos(S, UpperCase(NeuroTokenList[i])) = 1 then
     Result := Result + integer(NeuroTokenList.Objects[i]);
 end;
 if BoolMode then
  if Result <> 0 then Result := 1;
end;

function TMemScanBase.CheckDeteckCR(Detect: TMemDetectCrArray): boolean;
var
 i, x : integer;
 Res : boolean;
begin
  Result := false;
 // Правил нет - тогда
 if length(Detect) = 0 then
  exit;
 for i := 0 to length(Detect) - 1 do begin
  x := GetTokenVal(Detect[i].Token, 0);
  // Проверка условия
  case Detect[i].CmpCode of
   1 : Res := X >= Detect[i].CmpPar;
   2 : Res := X <= Detect[i].CmpPar;
   3 : Res := X >  Detect[i].CmpPar;
   4 : Res := X <  Detect[i].CmpPar;
   5 : Res := X <> Detect[i].CmpPar;
   0, 6 : Res := X =  Detect[i].CmpPar;
   else begin Result := false; exit; end;
  end;
   // Суммирование результата
   if i = 0 then Result := Res else
    Result := Result and Res;
   // Досрочный выход
   if not(Result) then exit;
 end;
end;

function TMemScanBase.ClearArray: boolean;
var
 i, j : integer;
begin
 // База сигнатур
 for i := 0 to $0FF do
  for j := 0 to $0FF do
   MemScanBase[i,j] := nil;
 // база критериев детекта
 for i := 0 to length(MemDetectBase) - 1 do begin
  MemDetectBase[i].MainDetect := nil;
  MemDetectBase[i].DopDetect  := nil;
  MemDetectBase[i].LockDetect := nil;
 end;
 MemDetectBase := nil;
end;

constructor TMemScanBase.Create(ADatabasePath: string);
begin
 FDatabasePath := ADatabasePath;
 NeuroTokenList  := TStringList.Create;
 FOnMemSignFound := nil;
 MemDetectBase   := nil;
 ClearArray;
end;

function TMemScanBase.Detect: boolean;
var
 i : integer;
begin
 for i := 0 to Length(MemDetectBase) - 1 do begin
  if CheckDeteckCR(MemDetectBase[i].MainDetect) then
   if (length(MemDetectBase[i].DopDetect)>0) and CheckDeteckCR(MemDetectBase[i].DopDetect) then
    if (length(MemDetectBase[i].LockDetect)=0) or (not CheckDeteckCR(MemDetectBase[i].LockDetect)) then begin
     AddSusp(MemDetectBase[i].Name);
    end;
 end;
end;

function TMemScanBase.LoadBinDatabase: boolean;
begin
 ClearArray;
 LoadBinDatabaseFile(FDatabasePath  + 'neuralm.avz', true);
 LoadBinDatabaseFileD(FDatabasePath + 'neurald.avz', true);
end;

function TMemScanBase.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i : integer;
 Res : boolean;
 buf_b  : byte;
 buf_w  : word;
 dw : dword;
 LastGroupName : string;
 B1, B2 : byte;
 MemSign : TMemSign;
begin
 Result := false;
 LastGroupName := 'Main';
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 for i := 1 to VirusBaseHeader.RecCount do begin
  MS.Read(buf_b, 1);
  if buf_b > 0 then begin
   SetLength(S, buf_b);
   MS.Read(S[1], buf_b);
   LastGroupName := S;
  end;
  MS.Read(buf_w, 2); // ID
  MemSign.ID   := buf_w;
  // Чтение байт b1 и b2
  MS.Read(B1, 1);
  MS.Read(B2, 1);
  MS.Read(buf_b, 1); // Длина CRC
  MemSign.CRCLength := buf_b;
  MS.Read(dw, 4);    // CRC
  MemSign.CRC := dw;
  MemSign.Name := LastGroupName;
  SetLength(MemScanBase[B1, B2], length(MemScanBase[B1, B2])+1);
  MemScanBase[B1, B2][length(MemScanBase[B1, B2])-1] := MemSign;
 end;
 MS.Free;
 Result := true;
end;

function TMemScanBase.LoadBinDatabaseFileD(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 i : integer;
 Res : boolean;
 buf_b  : byte;
 buf_w  : word;
 buf_dw : dword;
 MemDetectRec    : TMemDetectRec;
 procedure LoadDetectTokens(var Detect : TMemDetectCrArray);
 var
  i : integer;
 begin
  Detect := nil;
  MS.Read(buf_w, 2); //
  SetLength(Detect, buf_w);
  for i := 0 to Length(Detect) - 1 do begin
   // Имя токена
   MS.Read(buf_b, 1);
   SetLength(Detect[i].Token, buf_b);
   MS.Read(Detect[i].Token[1], buf_b);
   MS.Read(buf_b, 1);
   Detect[i].CmpCode := buf_b;
   MS.Read(buf_b, 1);
   Detect[i].CmpPar := buf_b;
  end;
 end;
begin
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 SetLength(MemDetectBase, VirusBaseHeader.RecCount);
 // Чтение данных
 for i := 1 to VirusBaseHeader.RecCount do begin
  MS.Read(buf_b, 1);
  SetLength(MemDetectRec.Name, buf_b);
  MS.Read(MemDetectRec.Name[1], buf_b);

  MS.Read(buf_dw, 4); // SMin
  MemDetectRec.SMin := buf_dw;
  MS.Read(buf_dw, 4); // SMax
  MemDetectRec.SMax := buf_dw;
  LoadDetectTokens(MemDetectRec.MainDetect);
  LoadDetectTokens(MemDetectRec.DopDetect);
  LoadDetectTokens(MemDetectRec.LockDetect);
  MemDetectBase[i-1] := MemDetectRec;
 end;
 MS.Free;
 Result := true;
end;

function TMemScanBase.ScanBuf(var Buf: array of byte): boolean;
var
 i, j, k  : integer;
 CRC, LastCRCLength, X   : dword;
 B1, B2 : byte;
begin
 for i := 0 to Length(buf)-255 do begin
  B1 := LowerCaseArray[buf[i]];
  B2 := LowerCaseArray[buf[i+1]];
  if MemScanBase[B1, B2] <> nil then begin
   LastCRCLength := 0;
   for j := 0 to Length(MemScanBase[B1, B2]) - 1 do begin
    if LastCRCLength <> MemScanBase[B1, B2][j].CRCLength then begin
     LastCRCLength := MemScanBase[B1, B2][j].CRCLength;
     CRC := $FFFFFFFF;
     for k := i+2 to i+2+LastCRCLength-1 do begin
      X := LowerCaseArray[buf[k]] xor (CRC and $0FF);
      CRC := (CRC shr 8) xor CRC32Table[X];
     end;
    end;
    if CRC = MemScanBase[B1, B2][j].CRC then
     if Assigned(FOnMemSignFound) then
      OnMemSignFound(MemScanBase[B1, B2][j], i);
   end;
  end;
 end;
end;

procedure TMemScanBase.SetOnMemSignFound(const Value: TOnMemSignFound);
begin
  FOnMemSignFound := Value;
end;

procedure TMemScanBase.SetOnMemSuspFound(const Value: TOnMemSuspFound);
begin
  FOnMemSuspFound := Value;
end;

procedure TMemScanBase.AddSusp(AVirName: string);
begin
 if Assigned(FOnMemSuspFound) then
  FOnMemSuspFound(AVirName);
end;

destructor TMemScanBase.Destroy;
var
 i, j : integer;
begin
  MemDetectBase := nil;
  // Очистка древовидной базы-индекса
  for i := 0 to $FF do
   for j := 0 to $FF do
    MemScanBase[i, j] := nil;
  inherited;
end;

{ TESSystem }

procedure TESSystem.AddError(s: string);
begin
 if FLastError <> '' then FLastError := FLastError + ';';
 FLastError := FLastError + s;
end;

constructor TESSystem.Create(ADatabasePath: string);
begin
 FDatabasePath := ADatabasePath;
 ESScripts     := nil;
end;

destructor TESSystem.Destroy;
begin
  ESScripts := nil;
  inherited;
end;

function TESSystem.LoadBinDatabase: boolean;
begin
 Result := false;
 ESScripts  := nil;
 if FileExists(FDatabasePath + 'neurale.avz') then
  Result := LoadBinDatabaseFile(FDatabasePath + 'neurale.avz', true);
end;

function TESSystem.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i, StartPosInBase : integer;
 Res : boolean;
 buf_w  : word;
begin
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 StartPosInBase := length(ESScripts);
 // Добавление информации к таблице
 SetLength(ESScripts, length(ESScripts)+VirusBaseHeader.RecCount);
 if AMainBase then
  MainDBDate := VirusBaseHeader.DateTime;
 // Чтение данных
 for i := 1 to VirusBaseHeader.RecCount do begin
  // Чтение имени скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  ESScripts[StartPosInBase].CompressedName := S;
  // Чтение скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  ESScripts[StartPosInBase].CompressedText := S;
  inc(StartPosInBase);
 end;
 Result := true;
end;

{ TStdScripts }

procedure TStdScripts.AddError(s: string);
begin
 if FLastError <> '' then FLastError := FLastError + ';';
 FLastError := FLastError + s;
end;

constructor TStdScripts.Create(ADatabasePath: string);
begin
 FDatabasePath := ADatabasePath;
 Scripts  := nil;
end;

destructor TStdScripts.Destroy;
begin
  Scripts := nil;
  inherited;
end;

function TStdScripts.LoadBinDatabase: boolean;
begin
 Result := false;
 Scripts  := nil;
 if FileExists(FDatabasePath + 'scripts.avz') then
  Result := LoadBinDatabaseFile(FDatabasePath + 'scripts.avz', true);
end;

function TStdScripts.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i, StartPosInBase : integer;
 Res : boolean;
 buf_w  : word;
begin
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 StartPosInBase := length(Scripts);
 // Добавление информации к таблице
 SetLength(Scripts, length(Scripts)+VirusBaseHeader.RecCount);
 if AMainBase then
  MainDBDate := VirusBaseHeader.DateTime;
 // Чтение данных
 for i := 1 to VirusBaseHeader.RecCount do begin
  // Чтение имени скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  Scripts[StartPosInBase].CompressedName := S;
  // Чтение скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  Scripts[StartPosInBase].CompressedText := S;
  inc(StartPosInBase);
 end;
 Result := true;
end;

{ TTranslate }

procedure TTranslate.AddError(s: string);
begin
 ;
end;

function TTranslate.ClasterizateDatabase: boolean;
var
 i : integer;
begin
 // Очистка таблицы кластеров
 ClearClustersTable;
 // Вычисление количества шаблонов перевода в каждом кластере
 for i := 0 to Length(Translates)-1 do
  inc(TranslatesCL[Translates[i].ItemID and $FF].TranslateCount);
 // Выделение памяти
 for i := 0 to $FF do begin
  SetLength(TranslatesCL[i].Translates, TranslatesCL[i].TranslateCount);
  TranslatesCL[i].TranslateCount := 0;
 end;
 // Заполнение кластерной базы
 for i := 0 to Length(Translates)-1 do begin
  TranslatesCL[Translates[i].ItemID and $FF].Translates[TranslatesCL[Translates[i].ItemID and $FF].TranslateCount] := Translates[i];
  inc(TranslatesCL[Translates[i].ItemID and $FF].TranslateCount);
 end;
 // Очистка исходной таблицы
 Translates := nil;
end;

procedure TTranslate.ClearClustersTable;
var
 i : integer;
begin
 for i := 0 to $FF do begin
  TranslatesCL[i].TranslateCount := 0;
  TranslatesCL[i].Translates := nil;
 end;
end;

constructor TTranslate.Create(ADatabasePath: string);
begin
 FDatabasePath := ADatabasePath;
 Translates   := nil;
 FOnTranslate := NIL;
 ClearClustersTable;
end;

destructor TTranslate.Destroy;
begin
  Translates := nil;
  inherited;
end;

function TTranslate.LoadBinDatabase(ALocale : string): boolean;
begin
 // загрузка базы
 Result := LoadBinDatabaseFile(FDatabasePath + 'lang_'+ALocale+'.avz', true);
 // Кластеризация базы
 ClasterizateDatabase;
end;

function TTranslate.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i, StartPosInBase : integer;
 Res : boolean;
 buf_w  : word;
begin
 Result := false;
 try
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 StartPosInBase := length(Translates);
 // Добавление информации к таблице
 SetLength(Translates, length(Translates)+VirusBaseHeader.RecCount);
 if AMainBase then
  MainDBDate := VirusBaseHeader.DateTime;
 // Чтение данных
 for i := 1 to VirusBaseHeader.RecCount do begin
  // Чтение кода строки
  MS.Read(buf_w, 2);  SetLength(S, buf_w);
  Translates[StartPosInBase].ItemID := buf_w;
  // Чтение строки
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  Translates[StartPosInBase].LocalTxt := S;
  inc(StartPosInBase);
 end;
 Result := true;
 except
 end;
end;

procedure TTranslate.SetOnTranslate(const Value: TOnTranslateEvent);
begin
  FOnTranslate := Value;
end;

function TTranslate.TranslateByCode(ItemId: integer;
  var Res: string): boolean;
var
 i, TranslatesCLIndx : integer;
begin
 Result := false;
 // Вычисление номера кластера
 TranslatesCLIndx := ItemId and $FF;
 // Поиск записи в кластере
 for i := 0 to Length(TranslatesCL[TranslatesCLIndx].Translates)-1 do
  if TranslatesCL[TranslatesCLIndx].Translates[i].ItemID = ItemId then begin
   Res := TranslatesCL[TranslatesCLIndx].Translates[i].LocalTxt;
   Result := true;
   exit;
  end;
end;

function TTranslate.TranslateStr(S: string): string;
var
 poz, ItemId : integer;
 ST : string;
begin
 // Если внешний модуль перевода подключен, то применяем его
 if Assigned(FOnTranslate) then begin
  Result := FOnTranslate(S);
  exit;
 end;
 // Перевод по собственной базе
 Result := S;
 poz := pos('$AVZ', Result);
 while poz > 0 do begin
  // В строке нет должного количества знаков
  ItemId := StrToIntDef(copy(Result, poz+4, 4), $FFFF);
  if ItemId = $FFFF then break;
  // Перевод
  if not(TranslateByCode(ItemId, ST)) then
   ST := '[?'+IntToStr(ItemId)+'?]';
  // Замена кода переводом
  Delete(Result, poz, 8);
  insert(ST, Result, poz);
  poz := pos('$AVZ', Result)
 end;
end;

{ TKMBase }

procedure TKMBase.AddError(s: string);
begin
 ;
end;

constructor TKMBase.Create(ADatabasePath: string);
begin
 Driver := nil;
 FDatabasePath := ADatabasePath;
end;

function TKMBase.CreateDriverFile(ADriver, AFilename: string; ASO_Ver : dword; var ALinkName : string): boolean;
var
 i  : integer;
 FS : TFileStream;
begin
 Result    := false;
 ALinkName := '';
 ADriver :=  UpperCase(Trim(ADriver));
 ForceDirectories(ExtractFilePath(AFilename));
 for i := 0 to Length(Driver) - 1 do
  if (Driver[i].Naim = ADriver) then begin
   try
    FS := TFileStream.Create(AFilename, fmCreate);
    FS.Write(Driver[i].BinFile[0], Length(Driver[i].BinFile));
    FS.Free;
   except
    // Подавление ошибок
   end;
   // Настройка имени линка по базе
   ALinkName := Driver[i].LinkName;
   Result := true;
   exit;
  end;
end;

destructor TKMBase.Destroy;
begin
  Driver := nil;
  inherited;
end;

function TKMBase.GetDriverLink(ADriver: string; ASO_Ver: dword;
  var ALinkName: string): boolean;
var
 i  : integer;
begin
 Result    := false;
 ALinkName := '';
 ADriver :=  UpperCase(Trim(ADriver));
 for i := 0 to Length(Driver) - 1 do
  if (Driver[i].Naim = ADriver) then begin
   ALinkName := Driver[i].LinkName;
   Result := true;
   exit;
  end;
end;

function TKMBase.LoadBinDatabase: boolean;
begin
 Driver := nil;
 Result := LoadBinDatabaseFile(FDatabasePath + 'krnldrv.avz', true);
end;

function TKMBase.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i, StartPosInBase : integer;
 Res : boolean;
 buf_b   : byte;
 buf_dw  : DWORD;
begin
 Result := false;
 try
   // Загрузка файла в память
   MS := TMemoryStream.Create;
   MS.LoadFromFile(AFileName);
   // Декодирование и проверка CRC
   Res := DecodeBinData(MS, 1);
   if Not(Res) then begin
    AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
    exit;
   end;
   MS.Seek(0,0);
   // Чтение заголовка
   MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
   if VirusBaseHeader.Sign <> 'AVZ' then exit;
   // Проверка, поддерживается ли данная версия базы
   if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
    AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
    exit;
   end;
   // Чтение данных
   StartPosInBase := length(Driver);
   // Добавление информации к таблице
   SetLength(Driver, length(Driver) + VirusBaseHeader.RecCount);
   // Чтение данных
   for i := 1 to VirusBaseHeader.RecCount do begin
    // Чтение имени драйвера
    MS.Read(buf_b, 1);  SetLength(S, buf_b);  MS.Read(S[1], buf_b);
    Driver[StartPosInBase].Naim := Trim(S);
    // Чтение имени ссылки
    MS.Read(buf_b, 1);  SetLength(S, buf_b);  MS.Read(S[1], buf_b);
    Driver[StartPosInBase].LinkName := Trim(S);
    // Чтение кода ОС
    MS.Read(buf_dw, 4);
    Driver[StartPosInBase].OS_Code := buf_dw;
    // Загрузка драйвера
    MS.Read(buf_dw, 4);
    SetLength(Driver[StartPosInBase].BinFile, buf_dw);
    MS.Read(Driver[StartPosInBase].BinFile[0], buf_dw);
    inc(StartPosInBase);
   end;
   Result := true;
  except
   Result := false;
  end;
end;

{ TSystemCheckIPU }

procedure TSystemIPU.AddError(s: string);
begin
 if FLastError <> '' then FLastError := FLastError + ';';
 FLastError := FLastError + s;
end;

constructor TSystemIPU.Create(ADatabasePath: string);
begin
 FDatabasePath := ADatabasePath;
 CheckScripts  := nil;
end;

destructor TSystemIPU.Destroy;
begin
 CheckScripts := nil;
 inherited;
end;

function TSystemIPU.LoadBinDatabase: boolean;
var
 FName : string;
begin
 Result := false;
 CheckScripts  := nil;
 FName := FDatabasePath + 'sysipu.avz';
 if FileExists(FName) then
  Result := LoadBinDatabaseFile(FName, true);
end;

function TSystemIPU.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i,StartPosInBase : integer;
 Res : boolean;
 buf_w  : word;
begin
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 StartPosInBase := length(CheckScripts);
 // Добавление информации к таблице
 SetLength(CheckScripts, length(CheckScripts)+VirusBaseHeader.RecCount);
 if AMainBase then
  MainDBDate := VirusBaseHeader.DateTime;
 // Чтение данных
 for i := 1 to VirusBaseHeader.RecCount do begin
  // Чтение имени скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  CheckScripts[StartPosInBase].CompressedName := S;
  // Чтение скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  CheckScripts[StartPosInBase].CompressedText := S;
  inc(StartPosInBase);
 end;
 Result := true;
end;

destructor TOfficeMacroBase.Destroy;
begin
  OfficeMacros := nil;
  inherited;
end;

{ TVSEData }

procedure TVSEData.AddError(s: string);
begin

end;

constructor TVSEData.Create(ADatabasePath: string);
begin
 FDatabasePath := ADatabasePath;
 CmdGroups := nil;
 CmdDescr  := nil;
end;

destructor TVSEData.Destroy;
begin
 CmdGroups := nil;
 CmdDescr  := nil;
 inherited;
end;

function TVSEData.LoadBinDatabase: boolean;
begin
 CmdGroups := nil;
 CmdDescr  := nil;
 Result := LoadBinDatabaseFile(FDatabasePath + 'vse_ru.avz', true);
end;

function TVSEData.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i   : integer;
 Res : boolean;
 buf_w  : word;
 buf_b, RecType  : byte;
 buf_fl : Single;
 TmpGr  : TAVZCmdGroup;
 TmpCmd : TAVZCmdDescr;
begin
 Result := false;
 try
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS, 2);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 for i := 1 to VirusBaseHeader.RecCount do begin
  // Чтение кода типа записи
  MS.Read(RecType, 1);
  if RecType = 1 then begin
   // Чтение кода группы
   MS.Read(buf_w, 2);
   TmpGr.GroupID := buf_w;
   // Чтение кода Public
   MS.Read(buf_b, 2);
   TmpGr.IsPublic := buf_b;
   // Чтение имени группы
   MS.Read(buf_w, 2);
   SetLength(S, buf_w);
   MS.Read(S[1], buf_w);
   TmpGr.Naim := S;
   SetLength(CmdGroups, length(CmdGroups) + 1);
   CmdGroups[length(CmdGroups) - 1] := TmpGr;
  end;
  if RecType = 2 then begin
   // Чтение кода команды
   MS.Read(buf_w, 2);
   TmpCmd.ID := buf_w;
   // Чтение кода группы
   MS.Read(buf_w, 2);
   TmpCmd.GroupID := buf_w;
   // Чтение кода типа
   MS.Read(buf_b, 1);
   TmpCmd.PrgType := buf_b;
   // Чтение имени
   MS.Read(buf_w, 2);
   SetLength(S, buf_w);
   if buf_w > 0 then
    MS.Read(S[1], buf_w)
     else S := '';
   TmpCmd.Naim := S;
   // Чтение декларации
   MS.Read(buf_w, 2);
   SetLength(S, buf_w);
   if buf_w > 0 then
    MS.Read(S[1], buf_w)
     else S := '';
   TmpCmd.CmdTxt := S;
   // Чтение комментария
   MS.Read(buf_w, 2);
   SetLength(S, buf_w);
   if buf_w > 0 then
    MS.Read(S[1], buf_w)
     else S := '';
   TmpCmd.Cmt := S;
   // Чтение комментария для документирования скрипта
   MS.Read(buf_w, 2);
   SetLength(S, buf_w);
   if buf_w > 0 then
    MS.Read(S[1], buf_w)
     else S := '';
   TmpCmd.ScriptCmt := S;
   MS.Read(buf_fl, 4);
   TmpCmd.AVZVer := buf_fl;
   SetLength(CmdDescr, length(CmdDescr) + 1);
   CmdDescr[length(CmdDescr) - 1] := TmpCmd;
  end;
 end;
 Result := true;
 except
 end;
end;

{ TBackupScripts }

procedure TBackupScripts.AddError(s: string);
begin
 if FLastError <> '' then FLastError := FLastError + ';';
 FLastError := FLastError + s;
end;

constructor TBackupScripts.Create(ADatabasePath: string);
begin
 FDatabasePath := ADatabasePath;
 BackupScripts := nil;
end;

destructor TBackupScripts.Destroy;
begin
  BackupScripts := nil;
  inherited;
end;

function TBackupScripts.LoadBinDatabase: boolean;
begin
 Result := false;
 BackupScripts  := nil;
 if FileExists(FDatabasePath + 'backup.avz') then
  Result := LoadBinDatabaseFile(FDatabasePath + 'backup.avz', true);
end;

function TBackupScripts.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i, StartPosInBase : integer;
 Res : boolean;
 buf_w  : word;
begin
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS, 3);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 StartPosInBase := length(BackupScripts);
 // Добавление информации к таблице
 SetLength(BackupScripts, length(BackupScripts)+VirusBaseHeader.RecCount);
 if AMainBase then
  MainDBDate := VirusBaseHeader.DateTime;
 // Чтение данных
 for i := 1 to VirusBaseHeader.RecCount do begin
  // Чтение имени скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  BackupScripts[StartPosInBase].CompressedName := S;
  // Чтение скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  BackupScripts[StartPosInBase].CompressedText := S;
  inc(StartPosInBase);
 end;
 Result := true;
end;

{ TCustomScripts }

procedure TCustomScripts.AddError(s: string);
begin
 ;
end;

constructor TCustomScripts.Create;
begin
 Scripts  := nil;
end;

destructor TCustomScripts.Destroy;
begin
  Scripts := nil;
  inherited;
end;

function TCustomScripts.LoadBinDatabase(ADBFilename : string): boolean;
begin
 Result := false;
 Scripts  := nil;
 if FileExists(ADBFilename) then
  Result := LoadBinDatabaseFile(ADBFilename, true);
end;
                        
function TCustomScripts.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i, StartPosInBase : integer;
 Res : boolean;
 buf_w  : word;
begin
 Result := false;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS, 4);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 StartPosInBase := length(Scripts);
 // Добавление информации к таблице
 SetLength(Scripts, length(Scripts)+VirusBaseHeader.RecCount);
 DBDate := VirusBaseHeader.DateTime;
 // Чтение данных
 for i := 1 to VirusBaseHeader.RecCount do begin
  // Чтение номера скрипта
  MS.Read(buf_w, 2);
  Scripts[StartPosInBase].ID := buf_w;
  // Чтение имени скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  Scripts[StartPosInBase].CompressedName := S;
  // Чтение скрипта
  MS.Read(buf_w, 2);  SetLength(S, buf_w);  MS.Read(S[1], buf_w);
  Scripts[StartPosInBase].CompressedText := S;
  inc(StartPosInBase);
 end;
 Result := true;
end;

{ TSRSystem }

procedure TSRSystem.AddError(s: string);
begin

end;

constructor TSRSystem.Create(ADatabasePath: string);
begin
 FDatabasePath := ADatabasePath;
 API_DB := nil;
 SPR_DB := nil;
end;

destructor TSRSystem.Destroy;
begin
 ClearDB;
 inherited;
end;

function TSRSystem.GetAPICallRating(SRSession : TSRSession; AFunctIndx: integer; Arg1, Arg2,
  ArgN: string): integer;
var
 i, CallCnt : integer;
 RSIndx     : integer;
begin
 Result := 0;
 // Логический контроль
 if (SRSession = nil) or (AFunctIndx < 0) then exit;
 // Анализ
 for i := 0 to Length(API_DB[AFunctIndx].Rules) - 1 do begin
  // Проверка условий для аргуметов
  if CheckAPIRule(SRSession, API_DB[AFunctIndx].Rules[i], 1, Arg1) then
   if CheckAPIRule(SRSession, API_DB[AFunctIndx].Rules[i], 2, Arg2) then
    if CheckAPIRule(SRSession, API_DB[AFunctIndx].Rules[i], 3, ArgN) then begin
     // Выставление признака "зловред"
     if API_DB[AFunctIndx].Rules[i].ISMalware then
      SRSession.IsMalware := true;
     // Добавляем статистику и разбираемся, какой по счету это вызов
     CallCnt := SRSession.AddRuleStat(API_DB[AFunctIndx].Rules[i].ID, API_DB[AFunctIndx].Rules[i].GroupID,
                                      CalkArgHash(API_DB[AFunctIndx].Rules[i], Arg1, Arg2, ArgN), RSIndx);
     if (CallCnt = 1) and (SRSession.RulesStat[RSIndx].RF = 0) then begin
      SRSession.SummRating := SRSession.SummRating + API_DB[AFunctIndx].Rules[i].MalwareRating[1];
      Result := API_DB[AFunctIndx].Rules[i].MalwareRating[1];
      SRSession.RulesStat[RSIndx].RF := 1;
     end else if (CallCnt <= 3) and (SRSession.RulesStat[RSIndx].RF = 1) then begin
      SRSession.SummRating := SRSession.SummRating + API_DB[AFunctIndx].Rules[i].MalwareRating[2];
      SRSession.RulesStat[RSIndx].RF := 2;
      Result := API_DB[AFunctIndx].Rules[i].MalwareRating[2];
     end else begin
      if (SRSession.RulesStat[RSIndx].RF = 2) then
       SRSession.SummRating := SRSession.SummRating + API_DB[AFunctIndx].Rules[i].MalwareRating[3];
      SRSession.RulesStat[RSIndx].RF := 3;
      Result := API_DB[AFunctIndx].Rules[i].MalwareRating[3];
     end;
     if SRSession.SummRating > 100 then
      SRSession.SummRating := 100;
     exit;
    end;
 end;
end;

function TSRSystem.GetFuntionIndx(AFunctionName: string): integer;
var
 i : integer;
begin
 Result := -1;
 AFunctionName := LowerCase(AFunctionName);
 // Цикл поиска функции по базе поддерживаемых функций
 for i := 0 to Length(API_DB) - 1 do
  if API_DB[i].API_Name = AFunctionName then begin
   Result := i;
   exit;
  end;
end;

function TSRSystem.CheckAPIEvent(SRSession: TSRSession;
  AIMULData: string): integer;
// Извлечение аргумента из строки
function ExtractArg(var AStr : string) : string;
 var
  i : integer;
 begin
  Result := '';
  AStr := Trim(AStr);
  // параметров нет
  if AStr = '' then exit;
  // Пустой параметр
  if AStr[1] = ',' then begin
   Delete(AStr, 1, 1);
   exit;
  end;
  // Строка в кавычках
  if (AStr[1]  = '"') or (AStr[1]  = '''') then begin
   Delete(AStr, 1, 1);
   i := 1;
   while i <= length(AStr) do
    case AStr[i] of
     '\' :  begin
             if i < length(AStr) then
              if AStr[i+1] in ['x', 'n'] then begin
               Result := Result + AStr[i];
               inc(i);
               continue;
              end;
              if AStr[i+1] in ['\'] then begin
               Result := Result + '\\';
               inc(i,2);
               continue;
              end;
              Result := Result + copy(AStr, i+1, 1);
              inc(i,2);
            end;
     '''',
     '"' :  begin
             Delete(AStr, 1, i);
             AStr := Trim(AStr);
             if copy(AStr, 1, 1) = ',' then
              Delete(AStr, 1, 1);
             exit;
            end;
     else begin
           Result := Result + AStr[i];
           inc(i);
          end;
    end;
  end;
  // Строка без кавычек
  if pos(',', AStr) > 0 then begin
   Result := copy(AStr, 1, pos(',', AStr)-1);
   Delete(AStr, 1, pos(',', AStr));
  end else begin
   Result := AStr;
   AStr := '';
  end;
 end;
 function NormalArg(FData : string) : string;
 var
  i : integer;
  PrevChar : char;
 begin
  FData := LowerCase(FData);
  Result := '';
  for i := 1 to Length(FData) do begin
   if i <= 2 then
    Result := Result + FData[i]
   else begin
    // Парный \
    if (FData[i] = '\') and (FData[i-1] = '\') then continue;
    // Парный пробел
    if (FData[i] = ' ') and (FData[i-1] = ' ') then continue;
    Result := Result + FData[i];
   end;
  end;
 end;
var
 S, APIName, ARG1, ARG2 : string;
 FunctIndx : integer;
begin
 Result := 0;
 SRSession.LastRule := 0;
 S := Trim(AIMULData);
 // Выделение имени функции и удаление скобок
 APIName := LowerCase(Trim(copy(S, 1, pos('(', S)-1)));
 // Проверка, есть ли по данной функции правила
 FunctIndx := GetFuntionIndx(APIName);
 // Если правил нет, то дальнейший анализ нецелесообразен
 if FunctIndx < 0 then begin
  Exit;
 end;
 // Удаление имени функции, скобок и точки с запятой в хвосте строки
 Delete(S, 1, pos('(', S));
 Delete(S, length(S)-1, 2);
 // Извлечение аргументов
 S := trim(S);
 ARG1 := ExtractArg(S);
 ARG2 := ExtractArg(S);
 // Анализ по правилам
 Result := GetAPICallRating(SRSession, FunctIndx, NormalArg(ARG1), NormalArg(ARG2), NormalArg(S));
end;

function TSRSystem.LoadBinDatabase: boolean;
begin
 // Очистка базы перед загрузкой
 ClearDB;
 // Загрузка базы правил
 Result := LoadBinDatabaseFile(FDatabasePath + 'sr.avz', true);
 // Загрузка базы справочников, с которым сравниваются аргументы
 Result := LoadBinSPRDatabaseFile(FDatabasePath + 'srdb.avz');
end;

function TSRSystem.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i, StartPosInBase, APIId : integer;
 Res : boolean;
 buf_w  : word;
 TMPRule : TAPIRule;
 // Загрузка правила для аргумента
 procedure LoadRule(var AR : TAPIRule; ArgID : integer);
 var
  S : string;
 begin
  // Чтение режима
  MS.Read(buf_w, 2);
  AR.ArgCmpMode[ArgID] := buf_w;
  AR.ArgCmpData[ArgID] := '';
  // загрузка параметров сообразно режиму
  case AR.ArgCmpMode[ArgID] of
   0 : exit;  // без анализа
   1 : begin  // анализ образца
        MS.Read(buf_w, 2);
        SetLength(S, buf_w);
        MS.Read(S[1], buf_w);
        AR.ArgCmpData[ArgID] := LowerCase(Trim(S));
       end;
   2 : begin  // анализ по справочнику
        // Загрузка кода справочника
        MS.Read(buf_w, 2);
        AR.ArgCmpData[ArgID] := IntToStr(buf_w);
       end;
  end;
 end;
 // Добавление правила в базу
 procedure AddRule(AR : TAPIRule;  AAPIId : integer);
 var
  i : integer;
 begin
  for i := 0 to Length(API_DB) - 1 do
   if API_DB[i].API_ID = AAPIId then begin
    SetLength(API_DB[i].Rules, Length(API_DB[i].Rules)+1);
    API_DB[i].Rules[Length(API_DB[i].Rules)-1] := AR;
    exit;
   end
 end;
begin
 Result := false;
 try
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 if AMainBase then
  MainDBDate := VirusBaseHeader.DateTime;
 // Создание таблицы API функций
 SetLength(API_DB, VirusBaseHeader.NextHeaderPtr);
 // Чтение набора API функций
 for i := 0 to VirusBaseHeader.NextHeaderPtr-1 do begin
  // Чтение кода функции
  MS.Read(buf_w, 2);
  API_DB[i].API_ID := buf_w;
  API_DB[i].Rules := nil;
  // Чтение имени функции
  MS.Read(buf_w, 2);
  SetLength(S, buf_w);
  MS.Read(S[1], buf_w);
  API_DB[i].API_Name := LowerCase(Trim(S));
 end;
 // Чтение правил
 for i := 1 to VirusBaseHeader.RecCount do begin
  // ID правила
  MS.Read(buf_w, 2);
  TMPRule.ID := buf_w;
  // ID группы
  MS.Read(buf_w, 2);
  TMPRule.GroupID  := buf_w;
  // Код API функции
  MS.Read(buf_w, 2);
  APIId := buf_w;
  // Загрузка правил
  LoadRule(TMPRule, 1);
  LoadRule(TMPRule, 2);
  LoadRule(TMPRule, 3);
  // Загрузка рейтингов
  MS.Read(buf_w, 2); TMPRule.MalwareRating[1]  := buf_w;
  MS.Read(buf_w, 2); TMPRule.MalwareRating[2]  := buf_w;
  MS.Read(buf_w, 2); TMPRule.MalwareRating[3]  := buf_w;
  // Признак "классифицировать как малваре"
  MS.Read(buf_w, 2);
  TMPRule.ISMalware := buf_w = 1;
  // Добавление правила
  AddRule(TMPRule, APIId);
  // Инкремент позиции
  inc(StartPosInBase);
 end;
 Result := true;
 except
 end;
end;

function TSRSystem.CheckAPIRule(SRSession : TSRSession; ARule: TAPIRule; AArgID: byte;
  AData: string): boolean;
var
 SPR_ID, i : integer;
begin
 Result := false;
 case ARule.ArgCmpMode[AArgID] of
  0 : begin // Режим 0 - игнорировать анализ данного аргумента
       Result := true;
       exit;
      end;
  1 : begin // Режим 1 - поиск образца
       Result := pos(ARule.ArgCmpData[AArgID], LowerCase(AData)) > 0;
       exit;
      end;
  2 : begin // Режим 2 - поиск по справочнику
       SPR_ID := StrToIntDef(ARule.ArgCmpData[AArgID], 0);
       Result := CheckBySpr(Spr_ID, AData);
       exit;
      end;
 end;
end;

function TSRSystem.CalkArgHash(ARule: TAPIRule; Arg1, Arg2,
  ArgN: string): dword;
 procedure AddToCRC(s : string);
 var
  i : integer;
 begin
  for i := 1 to Length(S) do
   Result := (Result shr 8) xor CRC32Table[byte(S[i])];
 end;
begin
 Result := $FFFFFFFF;
 if ARule.ArgCmpMode[1] > 0 then
  AddToCRC(Arg1);
 if ARule.ArgCmpMode[2] > 0 then
  AddToCRC(Arg2);
 if ARule.ArgCmpMode[3] > 0 then
  AddToCRC(ArgN);
end;

function TSRSystem.LoadBinSPRDatabaseFile(AFileName: string): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 S : string;
 i, StartPosInBase, GroupId, RecId : integer;
 Res : boolean;
 buf_w  : word;
 // Добавление правила в базу
 procedure AddSPRRow(SprID, RowId : integer; AData : string);
 var
  i : integer;
 begin
  for i := 0 to Length(SPR_DB) - 1 do
   if SPR_DB[i].ID = SprID then begin
    SPR_DB[i].Data.Add(LowerCase(AData));
    exit;
   end
 end;
begin
 Result := false;
 try
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Создание таблицы с перечнем справочников
 SetLength(SPR_DB, VirusBaseHeader.NextHeaderPtr);
 // Чтение набора API функций
 for i := 0 to VirusBaseHeader.NextHeaderPtr-1 do begin
  // Чтение кода справочника
  MS.Read(buf_w, 2);
  SPR_DB[i].ID := buf_w;
  // Чтение кол-ва записей (для контроля, не применяется)
  MS.Read(buf_w, 2);
  // Создание объекта для хранения коллекции строк-образцов
  SPR_DB[i].Data := TStringList.Create;
 end;
 // Чтение правил
 for i := 1 to VirusBaseHeader.RecCount do begin
  // ID группы
  MS.Read(buf_w, 2);
  GroupId := buf_w;
  // ID записи
  MS.Read(buf_w, 2);
  RecId := buf_w;
  // Чтение данных
  MS.Read(buf_w, 2);
  SetLength(S, buf_w);
  MS.Read(s[1], buf_w);
  // Добавление правила
  AddSprRow(GroupId, RecId, S);
  // Инкремент позиции
  inc(StartPosInBase);
 end;
 Result := true;
 except
 end;
end;

procedure TSRSystem.ClearDB;
var
 i : integer;
begin
 for i := 0 to Length(API_DB) - 1 do
  API_DB[i].Rules := nil;
 for i := 0 to Length(SPR_DB) - 1 do begin
  SPR_DB[i].Data.Clear;
  SPR_DB[i].Data.Free;
 end;
 API_DB := nil;
 SPR_DB := nil;
end;

function TSRSystem.CheckBySpr(ASprId: integer; AData: string): boolean;
var
 i, FSprIndx : integer;
begin
 Result := false;
 AData := LowerCase(AData);
 // Поиск справочника
 FSprIndx := -1;
 for i := 0 to Length(SPR_DB)-1 do
  if SPR_DB[i].ID = ASprId then begin
   FSprIndx := i;
   break;
  end;
 // Справочник не найден ?
 if FSprIndx = -1 then exit;
 // проверка по справонику
 for i := 0 to SPR_DB[FSprIndx].Data.Count - 1 do
  if pos(SPR_DB[FSprIndx].Data[i], AData) > 0 then begin
   Result := true;
   exit;
  end;
end;

{ TSRSession }

function TSRSession.AddRuleStat(ARuleID, ARuleGroup: integer;
  AArgHash: dword; var RSIndx : integer): integer;
var
 i, j : integer;
begin
 Result := 0;
 RSIndx := -1;
 LastRule := ARuleID;
 for i := 0 to Length(RulesStat) - 1 do
  if RulesStat[i].ID = ARuleID then begin
   RSIndx := i;
   // Проверка, срабатывало ли правило на такой набор аргументов
   if AArgHash <> $FFFFFFFF then
    for j := 0 to Length(RulesStat[i].ArgHashes) - 1 do
     if RulesStat[i].ArgHashes[j] = AArgHash then exit;
   // На текущий набор аргументов правило еще не срабатывало - инкремент статистики
   RulesStat[i].Cnt := RulesStat[i].Cnt + 1;
   Result := RulesStat[i].Cnt;
   // Добавление хеша аргументов
   if AArgHash <> $FFFFFFFF then begin
    SetLength(RulesStat[i].ArgHashes, Length(RulesStat[i].ArgHashes)+1);
    RulesStat[i].ArgHashes[Length(RulesStat[i].ArgHashes)-1] := AArgHash;
   end;
   exit;
  end;
 // Такое правило еще ни разу не срабатывало - добавим его в статистику
 SetLength(RulesStat, Length(RulesStat)+1);
 i := Length(RulesStat)-1;
 RulesStat[i].ID  := ARuleID;
 RulesStat[i].Cnt := 1;
 RulesStat[i].RF  := 0;
 RulesStat[i].GroupID := ARuleGroup;
 RulesStat[i].ArgHashes := nil;
 RSIndx := i;
 Result := 1;
end;

constructor TSRSession.Create;
begin
 RulesStat := nil;
 SummRating  := 0;
 IsMalware   := false;
end;

destructor TSRSession.Destroy;
var
 i : integer;
begin
 // Принудительная чистка массива хешей аргументов
 for i := 0 to Length(RulesStat) - 1 do
  RulesStat[i].ArgHashes := nil;
 RulesStat := nil;
 inherited;
end;

function TSRSession.GetGroupRating(AGroupID: integer): integer;
var
 i : integer;
begin
 Result := 0;
 for i := 0 to Length(RulesStat) - 1 do
  if RulesStat[i].GroupID = AGroupID then
   Result := Result + RulesStat[i].Cnt;
end;

function TSRSession.GetRuleStatCnt(ARuleID: integer): integer;
var
 i : integer;
begin
 Result := 0;
 for i := 0 to Length(RulesStat) - 1 do
  if RulesStat[i].ID = ARuleID then begin
   Result := RulesStat[i].Cnt;
   exit;
  end;
end;

end.

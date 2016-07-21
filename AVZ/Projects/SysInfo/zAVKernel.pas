unit zAVKernel;

interface
uses
  Types,
  Classes,
  SysUtils,
  Windows,
  zAVZArcKernel,
  // Модули связи с драйверами
  zAVZDriver,  zAVZDriverN,  zAVZDriverRK,  zAVZDriverBC,
  WIZARDNN,
  zAntivirus,
  zVirFileList,
  zESScripting,
  zLogSystem,
  zXMLLogger;

threadvar
 // Антивирус
 MainAntivirus   : TAntivirus;
 // Система проверки сигнатур
 FileSignCheck   : TFileSignCheck;
 // Главная нейросеть
 WizardNeuralNetwork : TWizardNeuralNetwork;
 // Настроки антикейлоггера
 KeyloggerBase : TKeyloggerBase;
 // Найстройки антируткита
 RootkitBase   : TRootkitBase;
 // Макровирусы в документах Office
 OfficeMacroBase : TOfficeMacroBase;
 // Настройки проверки системы
 SystemCheck   : TSystemCheck;
 // Настройки проверки системы - искатель пот. уязвимостей
 SystemIPU     : TSystemIPU;
 // Настройки восстановления системы
 SystemRepair : TSystemRepair;
 // Стандартные скрипты
 StdScripts   : TStdScripts;
 // Скрипты Backup
 BackupScripts : TBackupScripts;
 // База кода kernel-mode
 KMBase       : TKMBase;
 // Драйвер
 AVZDriver     : TAVZDriver;
 // Распаковщик архивов
 AVZDecompress : TAVZDecompress;
 // Модуль переводчика
 TranslateSys  : TTranslate;
 // Эвристический анализатор памяти
 ESSystem    : TESSystem;
 ESUserScript : TESUserScript;
 // База сканера памяти
 MemScanBase : TMemScanBase;
 // Список наденных файлов для индивидуальной обработки
 VirFileList : TVirFileList;
 // XML логгер для доп-данных, собираемых в ходе исследования системы
 XMLLogger   : TXMLLogger;
 // Boot Cleaner
 AVZDriverBC : TAVZDriverBC;

var
 // папка, в которой размещены базы
 AVPath : string;
 // папка, в которой размещен карантин
 QuarantineBaseFolder : string;
 // Признак того, что AVZ работает по сети
 NetWorkMode : boolean = false;
 // Признак того, что требуется перезагрузка
 RebootNeed : boolean;
 DeletedVirusList  : TStrings;
 // папка, в которой размещаются временные файлы
 AVZTempDirectoryPath : string;
 // Режим именования папко для KAV
 KAVQuarantineMode : boolean = false;

 // Инициализация ядра
function InitAVKernel(ALangId : string; AKavMode : byte = 0) : boolean;
// Получение пути к папке карантина
function GetQuarantineDirName(ASubFolder : string; ABaseFolder : boolean = false) : string;

implementation
uses zutil, zTranslate;

function InitAVKernel(ALangId : string; AKavMode : byte = 0) : boolean;
begin
 Result := false;
 ALangId := Trim(UpperCase(ALangId));
 KMBase   := nil;
 // Создание класса "Антивирус"
 MainAntivirus   := TAntivirus.Create(AVPath);//#DNL
 // Система проверки сигнатур
 FileSignCheck   := TFileSignCheck.Create(AVPath);  //#DNL
 // Создание главного нейроэмулятора
 WizardNeuralNetwork := TWizardNeuralNetwork.Create;

 // Режим 0 - драйвера берутся из базы AVZ
 if AKavMode = 0 then begin
  // Инициализация хранилища драйверов
  KMBase   := TKMBase.Create(AVPath);//#DNL
  KMBase.LoadBinDatabase;
  // Создание класса для управления драйвером
  AVZDriver     := TAVZDriver.Create;
  AVZDriver.KMBase := KMBase;
  AVZDriver.DriverPath := GetSystemDirectoryPath+'Drivers\';

  // Настройка AVZDriverSG
  AVZDriverSG.KMBase := KMBase;
  AVZDriverSG.DriverPath := GetSystemDirectoryPath+'Drivers\';

  // Драйвер расширенного мониторинга
  AVZDriverRK := TAVZDriverRK.Create;
  AVZDriverRK.KMBase := KMBase;
  AVZDriverRK.DriverPath := AVZDriverSG.DriverPath;

  // Драйвер Boot Cleaner
  AVZDriverBC := TAVZDriverBC.Create;
  AVZDriverBC.KMBase := KMBase;
  AVZDriverBC.DriverPath := AVZDriverSG.DriverPath;
 end;

 // Режим 1 - драйвер KIS
 if AKavMode = 1 then begin
  // Инициализация хранилища драйверов (у нас его нет - заглушка)
  KMBase   := nil;
  // Создание класса для управления драйвером
  AVZDriver     := TAVZDriverKIS.Create;
  AVZDriver.KMBase := nil;

  // Настройка AVZDriverSG
  AVZDriverSG.KMBase := nil;
  AVZDriverSG.DriverPath := '-';

  // Драйвер Boot Cleaner
  AVZDriverBC := TAVZDriverBCKIS.Create;
  AVZDriverBC.KMBase := nil;
  AVZDriverBC.DriverPath := '-';
 end;

 ESUserScript  := TESUserScript.Create;

 // Создание настроек кейлоггера
 KeyloggerBase := TKeyloggerBase.Create(AVPath);//#DNL
 // Создание настроек антируткита
 RootkitBase   := TRootkitBase.Create(AVPath);//#DNL
 // Создание класса для поиска макровирусов в документах Office
 OfficeMacroBase := TOfficeMacroBase.Create(AVPath);//#DNL
 // Создание класса проверки системы
 SystemCheck   := TSystemCheck.Create(AVPath);//#DNL
 // Создание класса поиска потенциальных уязвимостей
 SystemIPU     := TSystemIPU.Create(AVPath);//#DNL
 // Настройки восстановления системы
 SystemRepair  := TSystemRepair.Create(AVPath);//#DNL
 // База сканирования памяти
 MemScanBase   := TMemScanBase.Create(AVPath);//#DNL
 // База стандартных скриптов
 StdScripts   := TStdScripts.Create(AVPath);//#DNL
 // База скриптов backup
 BackupScripts := TBackupScripts.Create(AVPath);//#DNL

 // Создание класса-декомпрессора
 AVZDecompress := TAVZDecompress.Create(AVPath);//#DNL
 AVZDecompress.LoadBinDatabase;

 // Эвритисческая проверка
 ESSystem    := TESSystem.Create(AVPath);//#DNL
 // Переводчик
 TranslateSys  := TTranslate.Create(AVPath);//#DNL
 TranslateSys.LoadBinDatabase(ALangId);
 LangCode :=  ALangId;

 // Создание класса "Список вирусов"
 VirFileList := TVirFileList.Create;
 DeletedVirusList  := TStringList.Create;

 // Создание XML логгера
 XMLLogger   := TXMLLogger.Create;
end;

// Получение пути к папке карантина
function GetQuarantineDirName(ASubFolder : string; ABaseFolder : boolean = false) : string;
var
 Year, Month, Day : word;
 S : string;
 StrSize : dword;
begin
 if KAVQuarantineMode then begin
  if ASubFolder = 'Quarantine' then ASubFolder := 'AVZ_Quarantine' else
  if ASubFolder = 'Infected' then ASubFolder := 'AVZ_Infected' else
  if ASubFolder = 'Backup' then ASubFolder := 'AVZ_Backup';

 end;
 // Формирование базового пути
 Result := QuarantineBaseFolder + ASubFolder + '\';
 // Добавление в сетевом режиме имени ПК в путь
 if NetWorkMode then begin
  SetLength(S, 200);
  StrSize := length(s);
  GetComputerName(PChar(S), StrSize);
  Result := Result + copy(S, 1, StrSize) + '\';
 end;
 // Формирование динамической части (год-месяц-день)
 DecodeDate(Now, Year, Month, Day);
 if not(ABaseFolder) then
  Result := Result + IntToStr(Year) + '-' +
            FormatFloat('00', Month) + '-' + FormatFloat('00', Day)+'\';
 Result := NormalDir(Result);
end;


end.

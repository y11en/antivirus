unit zAVZKernel;
// Ядро AVZ - основные функции.
// Ядро общее для AVZ и его варианта в виде DLL для AVPTool/KIS, потокобезопасное

interface
uses
 Types,
 Classes,
 Windows,
 SysUtils,
 inifiles,
 math,
 ntdll,
 registry,
 SystemMonitor,
 zAutoruns,
 zLSP,
 zPELoader,
 zKernelMode,
 zSharedFunctions,
 zLogSystem,
 zKeyloggerHunter,
 zDirectFileCopy,
 zServices,
 zAntivirus,
 zStringCompressor,
 zAVKernel,
 // Модули связи с драйверами
 zAVZDriver, zAVZDriverN, zAVZDriverRK,
 // Поддержка архивов и составных файлов
 zAVZArcKernel,
 zVirFileList,
 zUserScripting,
 zScriptingKernel,
 zHTMLTemplate,
 zTranslate,
 zBackupEngine,
 zAVZVersion,
 forms;

 // Вакцинировать функцию AVZ от RootKit - перехватчика адреса
 function ModifyProgrammAPICall(AFunctionStubPtr, AGoodAPIAddr : pointer) : boolean;
 // Нейропроверка DLL как типового кейлоггера
 function NeuroKeyloggerCheck(AFileName : string; APrintLog : boolean = true): double;
 // Отложенное удаление файла
 function DelayDeleteFile(AFileName: string): boolean;
 // Удаление файла "методом AVZ" - обычное + переименование + отложение
 function AVZDeleteFile(AFileName: string): boolean;
 // Занесение файла в Infected
 Function CopyToInfected(AFileName, AVirus, AFolder : string) : boolean;
 // Запись в память процесса с защитой от перехвата этой функции в UserMode
 function zWriteProcessMemory(hProcess: THandle; const lpBaseAddress: Pointer; lpBuffer: Pointer;
   nSize: DWORD; var lpNumberOfBytesWritten: DWORD): BOOL;
 // Расширенный поиск маскирующихся объектов
 function ExtSearchHiddenObjects : boolean;
 // Проверка IAT указанного модуля
 procedure CheckModuleIAT(AImageBase: dword; ACheckedModuleImage, ALibName : string; hLib : Thandle; ALibPELoader : TPEImageLoader; ACheckedFunctions, ANoCheckedFunctions : string; ARestoreCode : boolean);
 // Проверка DLL на руткит-перехваты
 Function CheckDLLforRootKit(ADLLPath, ADLLName : string; ARestoreCode : boolean; ACheckedFunctions, ANoCheckedFunctions : String) : boolean;
 // Проверка SDT на перехваты
 function CheckServiceDescriptorTableforRootKit(ARestoreCode: boolean): boolean;
 // Поиск перехватчиков и руткитов
 function ScanRootKit(ADestroyRootkitUser, ADestroyRootKitKernel : boolean): boolean;
 // Удаление зловреда
 function DeleteVirus(AFileName, AVirus : string; AScript : dword; AUnregDLL : boolean = false) : boolean;
 // Проверка указанной DLL на предмет наличия в ней кейлоггеров
 function CheckDLLforKeyLogger(ModuleInfo: TModuleInfo) : boolean;
 // Поиск кейлоггеров
 function SearchKeyloggers: boolean;
 // Расширенная чистка системы
 function ExtSystemClean(ADeletedVirusList : TStrings)  : boolean;
 // Удаление Winlogon Notify по имени исполняемого файла
 procedure DelWinlogonNotifyByFileName(AFileName: string);
 // Удаление Winlogon Notify по имени ключа
 procedure DelWinlogonNotifyByKeyName(AKeyName: string);
 // Проверка, существует ли такой BHO
 function BHOExists(ACLSID: string): boolean;
 // Проверка, существует ли такой CLSID
 function CLSIDExists(ACLSID: string): boolean;
 // Проверка, существует ли такой CLSID - возвращает имя связанного с CLSID файла
 function CLSIDFileName(ACLSID: string; ARecurLevel : integer = 1): string;
 // Проверка файла по его CLSID - для эвристики
 function EvSysFileCheckByCLSID(ACLSID, AVirusName, AFileMask: string): boolean;
 // Проверка файла по его имени - для эвристики
 function EvSysFileCheckByName(AFileName, AVirusName,  AFileMask: string): boolean;
 // Вызов проверки ИПУ
 function EvSysCheckIPU(AGUILink : TGUILink): boolean;
 // Вызов эвристической проверки системы
 function EvSysCheck(AGUILink : TGUILink): boolean;
 // Запуск визарда в редиме Check/Fix
 function RunWizardScripts(AGUILink : TGUILink; AWizardDB : string; ALogLevel, AFixLevel : integer; AUseBackUp : boolean; AScriptID : integer = -1) : integer;

var
  FKeyloggerAlarmCount        : integer = 0; // Количество подозрений на кейлоггеры
  KernelRootKitFunctionsCount : integer = 0;
  AutoQuarantine              : boolean = false; // Помещать подозрительный в карантин автоматически
  AutoInfected                : boolean = false; // Помещать удаляемые в Infected автоматически
  AvzPath                     : string;       // Путь к AVZ
threadvar
  StopF      : boolean;     // Флаг - признак необходимости остановки
  ScanPauseF : boolean;     // Флаг - признак необходимости паузы
  LastSysCheckXML : string; // Последний XML файл с исследованием 

const
 // Набор расширений, проверяемых AVZ. Сгруппирован по исполняемым файлам (EXT_PE) и прочим (EXT_Data)
 ScanFilesEXT_PE     = '.EXE.DLL.OCX.SCR.SYS.CPL.AX.COM.VXD'; //#DNL
 ScanFilesEXT_Script = '.JS.HTM.HTML.CMD.BAT.INF.CHM.HTA.VBS';  //#DNL
 ScanFilesEXT_Data   = '.JS.HTM.HTML.CHM.HTA.CMD.BAT.DOC.LNK.INF.PIF.CAB.WAV.MP3.BAK.VBS.ZIP.RAR.JAR.TAR.GZ.TGZ.TMP.EML.MSG.MHT.MSI.XPI.MIM.BHX';//#DNL
 ExtToScan           : string = ScanFilesEXT_PE + ScanFilesEXT_Data;

implementation
uses zutil, orautil, uProcessView;

// Вакцинировать функцию AVZ от RootKit - перехватчика адреса
function ModifyProgrammAPICall(AFunctionStubPtr,
  AGoodAPIAddr: pointer): boolean;
var
 Buf1     : packed array [0..10] of byte;
 BytesReaded       : dword;
 dw : dword;
begin
 // Подготовка буфера "EB xx xx xx xx 90"
 Buf1[0] := $E9;
 Buf1[5] := $90;
 // Вычисление смещения для JMP и его запись в байты 2-5 буфера
 dw := dword(AGoodAPIAddr)  - (dword(AFunctionStubPtr) + 5);
 CopyMemory(@Buf1[1], @dw, 4);
 // Запись кода JMP <правильный адрес>; NOP поверх переходника, генерируемого Delphi для статического импорта
 Result := WriteProcessMemory(GetCurrentProcess, AFunctionStubPtr, @Buf1[0], 6, BytesReaded);
end;

// Запись в память процесса с защитой от перехвата этой функции в UserMode
function zWriteProcessMemory(hProcess: THandle; const lpBaseAddress: Pointer; lpBuffer: Pointer;
  nSize: DWORD; var lpNumberOfBytesWritten: DWORD): BOOL;
type
 TZwWriteVirtualMemory = function (hProcess: THandle; const lpBaseAddress: Pointer; lpBuffer: Pointer;
                                    nSize: DWORD; var lpNumberOfBytesWritten: DWORD): BOOL; stdcall;
var
 PEImageLoader   :  TPEImageLoader; // Класс - загрузчик PE
 ExportFunctions :  TStrings;
 ZwWriteVirtualMemory : TZwWriteVirtualMemory;
begin
 Result := WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
 if Result then exit;
 if not(ISNT) then exit;
 PEImageLoader   :=  TPEImageLoader.Create;
 ExportFunctions :=  TStringList.Create;
 try
  // Загрузка DLL. Если ошибка, то выход
  if not(PEImageLoader.LoadPEFile(GetSystemDirectoryPath + 'ntdll.dll')) then exit; //#DNL
  // Пересчет таблицы релокейшенов
  PEImageLoader.ExecuteReallocation(cardinal(PEImageLoader.FS.Memory));
  // Анализ таблицы импорта
  PEImageLoader.GetExportTable(ExportFunctions);
  // Поиск функции. Если функция не найдена, то выход
  if ExportFunctions.IndexOf('ZwWriteVirtualMemory') < 0 then exit; //#DNL
  @ZwWriteVirtualMemory := pointer(cardinal(PEImageLoader.FS.Memory) + PEImageLoader.GetPhAddByRVA(cardinal(ExportFunctions.Objects[ExportFunctions.IndexOf('ZwWriteVirtualMemory')]))); //#DNL
  Result := ZwWriteVirtualMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
 finally
  PEImageLoader.Free;
  ExportFunctions.Free;
 end;
end;

// Нейропроверка DLL как типового кейлоггера
function NeuroKeyloggerCheck(AFileName : string; APrintLog : boolean = true): double;
var
 PEImageLoader   :  TPEImageLoader; // Класс - загрузчик PE
 ImportList,
 ExportList      :  TStringList;
 S               :  string;
 F1, F2, F3 : boolean;
 i : integer;
 Res : Double;
 Lines : TStringList;
 function BoolToInt(B : boolean) : integer;
 begin
  if b then Result := 1 else Result := 0;
 end;
begin
 Result := -1;
 PEImageLoader   :=  TPEImageLoader.Create;
 ImportList      :=  TStringList.Create;
 ExportList      :=  TStringList.Create;
 try
 try
  if not(PEImageLoader.LoadPEFile(AFileName)) then exit;
  // Поиск нейропрофиля
  S := MainAntivirus.GetNeuroProfile('Keylogger'); //#DNL
  if S = '' then exit;
  // Загрузка нейропрофиля в нейроэмулятор
  Lines := TStringList.Create;
  Lines.Text := S;
  WizardNeuralNetwork.LoadFromWizardFile('', Lines);
  Lines.Free;
  PEImageLoader.GetImportTable(ImportList);
  PEImageLoader.GetExportTable(ExportList);
  WizardNeuralNetwork.InputValues['size'] := PEImageLoader.FS.Size; //#DNL
  WizardNeuralNetwork.InputValues['IC']   := ImportList.Count;      //#DNL
  WizardNeuralNetwork.InputValues['EC']   := ExportList.Count;      //#DNL
  F1 := false; F2 := false; F3 := false;
  for i := 0 to ExportList.Count - 1 do begin
   F1 := F1 or (pos('keyb', LowerCase(ExportList[i])) > 0);     //#DNL
   F2 := F2 or (pos('hook', LowerCase(ExportList[i])) > 0);     //#DNL
   F3 := F3 or (pos('keyboard', LowerCase(ExportList[i])) > 0); //#DNL
  end;
  WizardNeuralNetwork.InputValues['F1']     := BoolToInt(F1);   //#DNL
  WizardNeuralNetwork.InputValues['F2']     := BoolToInt(F2);   //#DNL
  WizardNeuralNetwork.InputValues['F3']     := BoolToInt(F3);   //#DNL
  WizardNeuralNetwork.InputValues['User32'] := BoolToInt(ImportList.IndexOf('user32.dll')>=0); //#DNL
  WizardNeuralNetwork.InputValues['P1']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'KERNEL32.dll', 'WriteProcessMemory'));          // p1  //#DNL
  WizardNeuralNetwork.InputValues['P2']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll', 'keybd_event'));   // p2                  //#DNL
  WizardNeuralNetwork.InputValues['P3']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'KERNEL32.dll', 'TerminateProcess'));            // p3  //#DNL
  WizardNeuralNetwork.InputValues['P4']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'UnhookWindowsHookEx'));         // p4  //#DNL
  WizardNeuralNetwork.InputValues['P5']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'SetWindowsHookExA'));           // p5  //#DNL
  WizardNeuralNetwork.InputValues['P6']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'CallNextHookEx'));              // p6  //#DNL
  WizardNeuralNetwork.InputValues['P7']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'GetKeyboardLayout'));           // p7  //#DNL
  WizardNeuralNetwork.InputValues['P8']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'GetKeyState'));                 // p8  //#DNL
  WizardNeuralNetwork.InputValues['P9']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'GetWindowThreadProcessId'));    // p9  //#DNL
  WizardNeuralNetwork.InputValues['P10']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'GetKeyboardState'));            // p10 //#DNL
  WizardNeuralNetwork.InputValues['P11']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'GetKeyboardType'));             // p11 //#DNL
  WizardNeuralNetwork.InputValues['P12']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'GetForegroundWindow'));         // p12 //#DNL
  WizardNeuralNetwork.InputValues['P13']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'FindWindowA'));                 // p13 //#DNL
  WizardNeuralNetwork.InputValues['P14']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'AttachThreadInput'));           // p14 //#DNL
  WizardNeuralNetwork.InputValues['P15']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'GetActiveWindow'));             // p15 //#DNL
//  WizardNeuralNetwork.InputValues['P16']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'GetWindowTextA'));            // p16 //#DNL
  WizardNeuralNetwork.InputValues['P17']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'ToAscii'));                     // p17 //#DNL
  WizardNeuralNetwork.InputValues['P18']     :=  BoolToInt(PEImageLoader.ImportFunctionExists(ImportList, 'USER32.dll',   'MapVirtualKeyA'));              // p18 //#DNL
  // Вычисление
  WizardNeuralNetwork.Compute;
  Res := WizardNeuralNetwork.OutputValues['RES']; //#DNL
  Res := max(0, Res);
  Res := min(100, Res);
  Result := res;
  if APrintLog then AddToLog(AFileName+ '>>> $AVZ0498 '+FormatFloat('#0.00', Res)+'% $AVZ0792', logAlert);
 except
  Result := -1;
  if APrintLog then AddToLog('$AVZ0497', logError);
 end;
 finally
  ImportList.Free;
  ExportList.Free;
  PEImageLoader.Free;
 end;
end;

// Отложенное удаление файла
function DelayDeleteFile(AFileName: string): boolean;
var
 Lines : TStrings;
 i, poz     : integer;
begin
 Result := false;
 // Метод для Windows 95/98/ME
 if not(ISNT) then begin
  Lines := TStringList.Create;
  if FileExists(GetWindowsDirectoryPath + 'wininit.ini') then    //#DNL
   Lines.LoadFromFile(GetWindowsDirectoryPath + 'wininit.ini');  //#DNL
  poz := -1;
  for i := 0 to Lines.Count - 1 do
  if Trim(LowerCase(Lines[i])) = '[rename]' then begin          //#DNL
    poz := i;
    Break;
   end;
  if poz >= 0 then
   Lines.Insert(poz+1, 'NUL='+AFileName)                        //#DNL
    else begin
     Lines.Add('[rename]');                                     //#DNL
     Lines.Add('NUL='+AFileName);                               //#DNL
    end;
  Lines.SaveToFile(GetWindowsDirectoryPath + 'wininit.ini');    //#DNL
  Lines.Free;
 end;
 // Метод для NT/W2K/XP
 if ISNT then begin
  Result := MoveFileEx(PChar(AFileName), nil, MOVEFILE_DELAY_UNTIL_REBOOT);
 end;
end;

// Удаление файла "методом AVZ" - обычное + переименование + отложение
function AVZDeleteFile(AFileName: string): boolean;
begin
 try
  Result := false;
  // Сброс атрибутов файла
  SetFileAttributes(PChar(AFileName), 0);
  // Удаление файла
  Result := DeleteFile(AFileName);
  // Переименование
  if not(Result) then begin
   // Отложенное удаление
   DelayDeleteFile(AFileName);
   AddToLog('>>>$AVZ0214 '+AFileName+' $AVZ0514', logAlert);
   // Попытка переименования
   Result := RenameFile(AFileName, ChangeFileExt(AFileName, '.bak'));
   // Попытка неуспешна ?? Повторим ее
   if FileExists(AFileName) then
    Result := RenameFile(AFileName, ChangeFileExt(AFileName, '.bak'));
   // Выполняем отложенное удаление
   if FileExists(ChangeFileExt(AFileName, '.bak')) then
    DelayDeleteFile(ChangeFileExt(AFileName, '.bak'));
   RebootNeed := true;
  end;
  except
   on e : exception do
    AddToDebugLog('$AVZ0686 '+e.message);
 end;
 // Дополнительная зачистка WinLogon - выполняется автоматом
 DelWinlogonNotifyByFileName(AFileName);
end;

// Занесение файла в карантин
function CopyToInfected(AFileName, AVirus, AFolder: string): boolean;
var
 InfectedFolder   : string;
 InfectedFileName : string;
 BaseNum, FileSize   : integer;
 INI              : TIniFile;
 FS, FS1          : TFileStream;
 FileDate         : LongInt;
begin
 Result := false;
 AVirus := TranslateStr(AVirus);
 // Задержка (применяется на очень медленных ПК для снижения нагрузки)
 if SleepScanTime > 0 then Sleep(SleepScanTime);
 FS := nil; FS1 := nil;
 try
   if Trim(AFileName) = '' then exit;
   // Блокировка помещения в карантин AVZ
   if pos(AvzPath, Trim(LowerCase(ExtractFilePath(AFileName)))) = 1 then exit;
   // Блокирование помещения "правильного" файла в карантин
   if FileSignCheck.CheckFile(AFileName) = 0 then exit;
   // Формирование базового пути
   InfectedFolder := GetQuarantineDirName(AFolder);
   // Создание папки для инфицированных файлов
   zForceDirectories(InfectedFolder);
   // Создание имени инфицированного файла
   BaseNum := 1;
   // Фаза 1 - быстрый поиск
   while FileExists(InfectedFolder + 'avz' + FormatFloat('#00000', BaseNum)+'.dta') do begin   //#DNL
    INI   :=  TIniFile.Create(InfectedFolder + 'avz' + FormatFloat('#00000', BaseNum)+'.ini'); //#DNL
    if LowerCase(INI.ReadString('InfectedFile', 'Src', '')) = LowerCase(AFileName) then begin  //#DNL
     INI.Free;
     exit;
    end;
    INI.Free;
    inc(BaseNum, 1);
   end;
   InfectedFileName := 'avz' + FormatFloat('#00000', BaseNum)+'.dta'; //#DNL
   // Копирование файла
   try
    try
     FS  := nil; FS1 := nil;
     FS  := TFileStream.Create(AFileName, fmOpenRead or fmShareDenyNone);
     FS1 := TFileStream.Create(InfectedFolder + InfectedFileName, fmCreate);
     FS.Seek(0,0);
     FS1.Seek(0,0);
     FS1.CopyFrom(FS, FS.Size);
     FS.Free;  FS  := nil;
     FileSize := FS1.Size;
     FS1.Free; FS1 := nil;
    except
     AddToLog('$AVZ0663 ('+AFileName+')', logError);
     if DirectCopyFile(AFileName, InfectedFolder + InfectedFileName) then
      AddToLog(' $AVZ0327', logInfo)
       else AddToLog(' $AVZ0328', logError);
    end;
    // Блокировка создания INI файла, если нет файла DTA
    if not(FileExists(InfectedFolder + InfectedFileName)) then exit;
    AddToLog('$AVZ1113 ('+AFileName+')', logInfo);
    // Копирование даты
    try
     FileDate := FileAge(AFileName);
     FileSetDate(InfectedFolder + InfectedFileName, FileDate);
    except end;
    // Запись информации о файле
    INI   :=  TIniFile.Create(InfectedFolder + ChangeFileExt(InfectedFileName, '.ini')); //#DNL
    INI.WriteString('InfectedFile', 'Src', AFileName);             //#DNL
    INI.WriteString('InfectedFile', 'Infected', InfectedFileName); //#DNL
    INI.WriteString('InfectedFile', 'Virus', AVirus);              //#DNL
    INI.WriteDateTime('InfectedFile', 'QDate', Now);               //#DNL
    INI.WriteInteger('InfectedFile', 'Size', FileSize);            //#DNL
    INI.WriteString('InfectedFile', 'MD5', CalkFileMD5(InfectedFolder + InfectedFileName));            //#DNL
    INI.WriteDateTime('InfectedFile', 'FileDate', FileDateToDateTime(FileDate)); //#DNL
    INI.WriteString('InfectedFile', 'AVZVer', VER_NUM); //#DNL
    INI.WriteDateTime('InfectedFile', 'MainAVBase', MainAntivirus.MainDBDate); //#DNL
    INI.Free;
    Result := true;
   except
    FS.Free;
    FS1.Free;
   end;
 except
  Result := false;
 end;
end;

procedure CheckModuleIAT(AImageBase : dword; ACheckedModuleImage, ALibName : string;  hLib : Thandle; ALibPELoader : TPEImageLoader; ACheckedFunctions, ANoCheckedFunctions : string;  ARestoreCode : boolean);
var
 PE_Header_Rel, ImportTableEntryPoint, ImpId, FunctId, EtRva   : dword;
 PRVA_Import : LPDWORD;
 ImageFileHeader : TImageFileHeader;
 ImageOptionalHeader : TImageOptionalHeader;
 PImport     : PImageImportDescriptor;
 PLibName    : PChar;
 PELoader : TPEImageLoader;
 X, LibExpTable : TStringList;
 FunctionName : string;
 EtF : integer;
begin
 PELoader := TPEImageLoader.Create;
 X           := TStringList.Create;
 LibExpTable := TStringList.Create;
 try
  try
     PELoader.LoadPEFile(ACheckedModuleImage);
     ALibPELoader.GetExportTable(LibExpTable);
     PELoader.GetImportTable(X);
     // Поиск смещения до заголовка
     Move(pointer(AImageBase+$3C)^, PE_Header_Rel, 4);
     // Чтение заголовка
     Move(pointer(AImageBase+PE_Header_Rel)^, ImageFileHeader, sizeof(ImageFileHeader));
     if ImageFileHeader.PE_Segnature <> 17744 then begin
      AddToDebugLog('RK - no PE sign');
      exit;
     end;
     // Чтение расширенного заголовка
     Move(pointer(AImageBase+PE_Header_Rel+$18)^, ImageOptionalHeader, sizeof(ImageOptionalHeader));
     // Проверка, есть ли каталог импорта
     if ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = 0 then begin
      AddToDebugLog('RK - IDEI not found');
      exit;
     end;
     // Указатель на таблицу импорта
     ImportTableEntryPoint := ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
     PImport := pointer(ImportTableEntryPoint + AImageBase);
     ImpId := 0;
     // Сканирование таблицы импорта
     while PImport.Name <> 0 do begin
      PLibName := pchar(PImport.Name + AImageBase);
      if LowerCase(PLibName) = LowerCase(ALibName) then begin
       // Привязка есть ?
       if PImport.TimeDateStamp=0  then
        PRVA_Import := LPDWORD(pImport.FirstThunk + AImageBase)
       else
        PRVA_Import:=LPDWORD(pImport.OriginalFirstThunk + AImageBase);
       FunctId := 0;
       while PRVA_Import^<>0 do begin
        if (dword(PRVA_Import^) and $80000000) = 0 then begin
         FunctionName := (X.Objects[ImpId] as TStrings)[FunctId];
         EtF := LibExpTable.IndexOf(FunctionName);
         // Проверка только заданных функций
         if ACheckedFunctions <> '' then
          if pos(LowerCase(FunctionName)+',', ACheckedFunctions) = 0 then EtF := -1;
         if ANoCheckedFunctions <> '' then
          if pos(LowerCase(FunctionName)+',', ANoCheckedFunctions) > 0 then EtF := -1;
         if EtF >= 0 then begin
          EtRva := dword(LibExpTable.Objects[EtF]) + hLib;
          if dword(PRVA_Import^) <> EtRva then begin
           AddToLog('$AVZ0209: '+(X.Objects[ImpId] as TStrings)[FunctId]+' - '+IntToHex(dword(PRVA_Import^), 8)+'<>'+IntToHex(dword(EtRva), 8), logAlert);
           if ARestoreCode then begin
            dword(PRVA_Import^) := EtRva;
            AddToLog('$AVZ0051: '+(X.Objects[ImpId] as TStrings)[FunctId], logInfo);
           end;
          end;
         end;
         Inc(PRVA_Import);
        end;
        Inc(FunctId);
       end;
      end;
      inc(PImport);
      inc(ImpId);
     end;
   except
    on e : exception do
     AddToDebugLog('Creck IAT error: '+e.Message);
   end;
  finally
   PELoader.Free;  PELoader := nil;
   X.Free;
   LibExpTable.Free;
  end;
end;

Function CheckDLLforRootKit(ADLLPath, ADLLName : string; ARestoreCode : boolean; ACheckedFunctions, ANoCheckedFunctions : String) : boolean;
var
 PEImageLoader   :  TPEImageLoader; // Класс - загрузчик PE
 hLib            : THandle;         // Handle обрабатываемой DLL
 ExportTableSection : integer;      // Номер секции, содержащей таблицу экспорта
 ExportTableRVA,                    // RVA таблицы экспорта
 ExportTableEntryPoint,
 ExportTableSize,
 ExportTableItemsCount,
 ExportTableFunctionsRel,
 ExportTableNameRel, ExportTableOrdinalRel : DWORD;
 BytesReaded : Cardinal;
 dw      : dword;
 i, Step    : integer;
 ImageExportDirectory : TImageExportDirectory;
 FunctionName    : string;  // Имя функции
 FunctionOrdinal : word;    // Ординал функции
 FunctionRVA     : dword;   // RVA функции
 FunctionPhAddr  : dword;   // Физический адрес точки входа (т.е. адрес в файле)
 ProcAddress     : Pointer;
 LibLoaded       : boolean; // Признак того, что изучаемая DLL уже загружена моим процессом
 RealCodeRel     : integer;
 S               : string;
 // Буфера для различных операций
 Buf            : packed array of byte;
 Buf1, Buf2     : packed array [0..100] of byte;
 HookAddr       : dword;
begin
 Step := 1;
 // Подготовка параметров - списки проверяемых и непроверяемых функций
 if Trim(LowerCase(ACheckedFunctions)) <> '' then
  ACheckedFunctions   := Trim(LowerCase(ACheckedFunctions))+',';
 if Trim(LowerCase(ANoCheckedFunctions)) <> '' then
  ANoCheckedFunctions := Trim(LowerCase(ANoCheckedFunctions))+',';
 // Загрузка DLL моим методом, через PEImageLoader
 PEImageLoader :=  TPEImageLoader.Create;
 try
   Step := 2;
   PEImageLoader.LoadPEFile(ADLLPath+ADLLName);
   // Загрузка DLL штатным методом, через GetModuleHandle и LoadLibrary
   LibLoaded := false;
   // 1. Эта DLL уже загружена ?
   hLib := GetModuleHandle(PChar(GetSystemDirectoryPath + ADLLName));
   // 2. hLib = 0 - не загружена, загрузим штатным способом
   if hLib = 0 then begin
    Step := 3;
    hLib := LoadLibrary(PChar(GetSystemDirectoryPath + ADLLName));
    LibLoaded := true;
   end;
   // 3. Загрузка не удалась - тогда продолжать нет смысла
   if hLib = 0 then begin
    AddToLog(' $AVZ0479 '+ADLLName, logError);
    exit;
   end;
   // Настройка Reallocation под _реальный_ адрес (это дает право сравнивать машинный код)
   Step := 4;
   PEImageLoader.ExecuteReallocation(hLib);

   Step := 5;
   // Секция для таблицы экспорта отсутствует ?? - тогда выход
   if (PEImageLoader.ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size = 0) or
      (PEImageLoader.ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = 0) then begin
       AddToLog(' $AVZ0481', logError);
       exit;
   end;
   // Поиск секции, содержащей таблицу экспорта
   ExportTableSection := PEImageLoader.GetSectionByRVA(PEImageLoader.ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
   if (ExportTableSection <  0) then begin
     AddToLog(' $AVZ0480', logError);
     exit;
   end;
   AddToLog(' $AVZ0054 '+ADLLName+ ', $AVZ1056 '+PEImageLoader.ImageSections[ExportTableSection].ObjName, logInfo);
   // Определение RVA, размера и физического положения таблицы экспорта
   ExportTableRVA        := PEImageLoader.ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
   ExportTableEntryPoint := PEImageLoader.GetPhAddByRVA(ExportTableRVA);
   ExportTableSize       := PEImageLoader.ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
   // Чтение данных из таблицы экспорта в буфер
   Step := 6;
   SetLength(Buf, PEImageLoader.ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
   PEImageLoader.FS.Seek(ExportTableEntryPoint, 0);
   PEImageLoader.FS.Read(Buf[0], PEImageLoader.ImageOptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
   Step := 7;
   move(Buf[0], ImageExportDirectory, sizeof(ImageExportDirectory));
   Step := 8;
   // Чтение количества элементов таблицы экспорта
   ExportTableItemsCount   := ImageExportDirectory.NumberOfFunctions;
   ExportTableFunctionsRel := ImageExportDirectory.AddressOfFunctions - ExportTableRVA;
   // Чтение указателя на на таблицу имен
   Step := 9;
   ExportTableNameRel    := ImageExportDirectory.AddressOfNames - ExportTableRVA;
   ExportTableOrdinalRel := ImageExportDirectory.AddressOfNameOrdinals - ExportTableRVA;
   // Цикл по таблице имен
   for i := 0 to  ImageExportDirectory.NumberOfNames - 1 do begin
    Step := 10;
    // Имя функции
    move(Buf[ExportTableNameRel+i*4], dw, 4);
    if (dw - ExportTableRVA) > ExportTableSize then continue;
    FunctionName := Pchar(@Buf[dw - ExportTableRVA]);
    Step := 11;
    // Проверка только заданных функций
    if ACheckedFunctions <> '' then
     if pos(LowerCase(FunctionName)+',', ACheckedFunctions) = 0 then Continue;
    if ANoCheckedFunctions <> '' then
     if pos(LowerCase(FunctionName)+',', ANoCheckedFunctions) > 0 then Continue;
    Step := 12;
    // Ординал
    move(Buf[ExportTableOrdinalRel+i*2], FunctionOrdinal, 2);
    Step := 13;
    // Адрес
    move(Buf[ExportTableFunctionsRel+FunctionOrdinal*4], dw, 4);
    FunctionRVA := dw;
    FunctionOrdinal := FunctionOrdinal + ImageExportDirectory.Base;
    Step := 14;
    // Поиск адреса
    ProcAddress := GetProcAddress(hLib, PChar(FunctionName));
    // Пропуск функций, для которых не найдены адреса
    if ProcAddress = nil then
     Continue;
    // Отображение статистики в строке статуса
//!!!!!!!!!!!!!    StatusBar.Panels[0].Text := ADLLName+':'+FunctionName+' ('+IntToStr(FunctionOrdinal)+')';
//!!!!!!!!!!!!!    StatusBar.Repaint;
    //LogMemo.Lines.Add(FunctionName+' ('+IntToStr(FunctionOrdinal)+') = '+IntToStr(FunctionRVA)+ '; Addr = '+  inttostr(integer(ProcAddress)));
    FunctionPhAddr := PEImageLoader.GetPhAddByRVA(FunctionRVA);
    // Пропуск функций, для которых не найдены физические адреса кода
    if FunctionPhAddr = 0 then
     Continue;
    // Чтение 20 байт машинного кода функции из "эталонного образа"
    PEImageLoader.FS.Seek(FunctionPhAddr, 0);
    PEImageLoader.FS.Read(Buf1, 20);
    Step := 15;
    // Чтение изучаемного кода
    CopyMemory(@Buf2[0], ProcAddress, 20);
    // todo - сдалать ReadProcessMemory(GetCurrentProcess, ProcAddress,  @Buf1[0], 15, BytesReaded); и сравнить ...
    Step := 16;
    // Поверка, совпадает ли программный код в первых двух байтах
    if (Buf1[0]<>0) and (Buf2[1]<>0) and ((Buf1[0] <> Buf2[0]) or (Buf1[1] <> Buf2[1])) then begin
      // Проверка, совпадает ли RWA
      if FunctionRVA <> (dword(ProcAddress) - dword(hLib)) then begin
       AddToLog('$AVZ1135 '+ADLLName+':'+FunctionName+' ('+IntToStr(FunctionOrdinal)+') $AVZ0717 ->'+IntToHex(dword(FunctionRVA+dword(hLib)), 6)+'->'+IntToHex(dword(ProcAddress), 6), logAlert);
       // Добавление в XML базу
       XMLLogger.AddItem('RK_UM', '<ITEM DLL="'+ADLLName+'" '+
                              ' FNaim="'+FunctionName+'"'+
                              ' FIndx="'+IntToStr(i)+'"'+
                              ' HookPtr="'+IntToHex(dword(ProcAddress), 6)+'"'+
                              ' HookType="1" '+
                              '/>');
       Buf1[0] := $E9;
       dw := FunctionRVA + dword(hLib) - (dword(ProcAddress) + 5);
       Step := 17;
       if CompareMem(@Buf1[0], @Buf2[0], 5) then
        AddToLog('$AVZ0711 '+ADLLName+':'+FunctionName+' ('+IntToStr(FunctionOrdinal)+') $AVZ1098', logInfo)
       else
        // атака перехватчика на основе подмены адреса - записываем первой командой его кода JMP на нормальную функцию
        if ARestoreCode then begin
         CopyMemory(@Buf1[1], @dw, 4);
         if WriteProcessMemory(GetCurrentProcess, ProcAddress, @Buf1[0], 5, BytesReaded) then
          AddToLog('$AVZ0705 '+ADLLName+':'+FunctionName+' ('+IntToStr(FunctionOrdinal)+') $AVZ0501', logAlert)
           else AddToLog(' >>> $AVZ0340 '+FunctionName+' - $AVZ0678', logError);
         // ***** Прививка от перехвата подменой адреса (работает только на Delphi) *************
         if LowerCase(ADLLName) = 'kernel32.dll' then begin
          if LowerCase(FunctionName) = 'createfilea' then  //#DLS
           if ModifyProgrammAPICall(@CreateFileA, pointer(FunctionRVA + dword(hLib))) then
            AddToLog(' >>> $AVZ1133 '+FunctionName+' - $AVZ0796  !!)', logAlert);
          if LowerCase(FunctionName) = '_lcreat' then //#DLS
           if ModifyProgrammAPICall(@_lcreat, pointer(FunctionRVA + dword(hLib))) then
            AddToLog(' >>> $AVZ1133 '+FunctionName+' - $AVZ0796  !!)', logAlert);
          if LowerCase(FunctionName) = '_lopen' then //#DLS
           if ModifyProgrammAPICall(@_lopen, pointer(FunctionRVA + dword(hLib))) then
            AddToLog(' >>> $AVZ1133 '+FunctionName+' - $AVZ0796  !!)', logAlert);
          if LowerCase(FunctionName) = 'openfile' then //#DLS
           if ModifyProgrammAPICall(@openfile, pointer(FunctionRVA + dword(hLib))) then
            AddToLog(' >>> $AVZ1133 '+FunctionName+' - $AVZ0796  !!)', logAlert);
          if LowerCase(FunctionName) = 'loadlibrarya' then  //#DLS
           if ModifyProgrammAPICall(@loadlibrarya, pointer(FunctionRVA + dword(hLib))) then
            AddToLog(' >>> $AVZ1133 '+FunctionName+' - $AVZ0796  !!)', logAlert);
          if LowerCase(FunctionName) = 'loadlibraryexa' then //#DLS
           if ModifyProgrammAPICall(@loadlibraryexa, pointer(FunctionRVA + dword(hLib))) then
            AddToLog(' >>> $AVZ1133 '+FunctionName+' - $AVZ0796 !!)', logAlert);
        end;
       end;
      end else begin
       Step := 18;
       // Поиск маскировки кода
       RealCodeRel := 0;
       S := '';
       // Пропуск NOP, если таковые есть
       while (Buf2[RealCodeRel] = $90) and (RealCodeRel < 10) do inc(RealCodeRel);
       if RealCodeRel > 0 then
        S := ', $AVZ0384 '+IntToStr(RealCodeRel)+' $AVZ0579';
       // Попытка определения метода по опкоду
       case Buf2[RealCodeRel] of
        $0CC : AddToLog('$AVZ1135 '+ADLLName+':'+FunctionName+' ('+IntToStr(FunctionOrdinal)+') $AVZ0713'+S, logAlert);
        $0E9 : begin
                CopyMemory(@HookAddr, @Buf2[RealCodeRel+1], 4);
                HookAddr := HookAddr - 5 + dword(ProcAddress);
                AddToList(S, '['+IntToHex(HookAddr, 8)+']');
                AddToLog('$AVZ1135 '+ADLLName+':'+FunctionName+' ('+IntToStr(FunctionOrdinal)+') $AVZ0714'+S, logAlert);
               end;
        $06A,
        $068,
        $050,
        $053,
        $056,
        $057,
        $059 : AddToLog('$AVZ1135 '+ADLLName+':'+FunctionName+' ('+IntToStr(FunctionOrdinal)+') $AVZ0715'+S, logAlert);
        else begin
         AddToLog('$AVZ1135 '+ADLLName+':'+FunctionName+' ('+IntToStr(FunctionOrdinal)+') $AVZ0716'+S, logAlert)
        end;
       end;
       // Добавление в XML базу
       XMLLogger.AddItem('RK_UM', '<ITEM DLL="'+ADLLName+'" '+
                              ' FNaim="'+FunctionName+'"'+
                              ' FIndx="'+IntToStr(i)+'"'+
                              ' HookPtr="'+IntToHex(dword(ProcAddress), 6)+'"'+
                              ' HookType="2" '+
                              '/>');
       Step := 19;
       if ARestoreCode then begin
        if WriteProcessMemory(GetCurrentProcess, ProcAddress, @Buf1[0], 20, BytesReaded) then
         AddToLog(' >>> $AVZ0343 '+FunctionName+' $AVZ0501', logAlert)
          else begin
           AddToLog(' >>> $AVZ0343 '+FunctionName+' - $AVZ0682', logAlert);
           if zWriteProcessMemory(GetCurrentProcess, ProcAddress, @Buf1[0], 20, BytesReaded) then
            AddToLog(' >>>> $AVZ0343 '+FunctionName+' - $AVZ0502)', logAlert);
          end;
       end;
      end;
     end;
    end;

   Step := 20;
   // Проверка IAT исполняемого файла
   try
    CheckModuleIAT(GetModuleHandle(nil), ParamStr(0),  ADLLName, hLib, PEImageLoader, ACheckedFunctions, ANoCheckedFunctions, ARestoreCode);
   except
    on e : Exception do
     AddToDebugLog('IAT:'+e.Message);
   end;
   // Проверка модуля Kernel32
  { if LowerCase(ADLLName) = 'ntdll.dll' then
  CheckModuleIAT(GetModuleHandle('kernel32.dll'), ADLLPath+'kernel32.dll', ADLLName, hLib, PEImageLoader, ACheckedFunctions, ANoCheckedFunctions, ARestoreCode);}
 except
  on e : exception do begin
   AddToLog('$AVZ0642 '+ADLLName, logError);
   AddToDebugLog('Error:'+e.Message);
  end;
 end;
 // Осовобождение памяти и выгрузка DLL
 PEImageLoader.Free;
 if LibLoaded then
  FreeLibrary(hLib);
 Buf := nil;
end;

function CheckServiceDescriptorTableforRootKit(
  ARestoreCode: boolean): boolean;
var
 PEImageLoader   :  TPEImageLoader;    // Класс - загрузчик PE
 ExportTable,
 KFNamesList     :  TStringList;
 dw, dw1, dwJMP : dword;
 Buf, Buf1 : packed array[0..50] of byte;
 i, j       : integer;
 S, KernelFileName, HookFileName, FileInfoStr : string;
 KernelBase, KernelSize,
 KeServiceDescriptorTableRVA,
 KESystemDescriptorTableAddress,
 SDTTableAddress,
 SDTTableAddressF,
 SDTTableAddress1F,
 SDTItemsCount,
 SDTItemsCountF,
 FunctionPhAddr,
 SDTCheckedItemsCount,
 RealCodeRel,
 MemCodeHijackRel,
 ProcAddress     : dword;
 FoundSDT1Fl, MemCodeHijack     : boolean;
 StatCheckedFn, StatRootKitFn, StatRestoreFn : integer; // Статистика
 JmpAddr : dword;
 JmpAddrBytes : packed array[0..3] of byte absolute JmpAddr;
 SysenterAddrF : dword;
 IDT : TIDT;
 IDT_GATES : packed array of TIDTGATE;
 IDTSetup, KernelExportSetup : string;
 SI : TSystemInfo;
 NumberOfProcessors, CurrentCPU : integer;
 KernelExport : TStringList;
 // Вычисление адреса Sysenter
 function GetSysenterAddr : dword;
 var
  Section, i  : integer;
  StartAddr, ScanSize, SignAddr : dword;
 begin
  Result := 0;
  Section := PEImageLoader.GetSectionByName('.text');
  if Section = -1 then exit;
  StartAddr := PEImageLoader.ImageSections[Section].PhOffset;
  ScanSize  := PEImageLoader.ImageSections[Section].PhSize;
  SignAddr := 0;
  // Ищем команду mov     ds:KeServiceDescriptorTable, offset off_40DF40
  for i := dword(PEImageLoader.FS.Memory)+StartAddr to dword(PEImageLoader.FS.Memory) + ScanSize  do
   if (byte(pointer(i)^)   = $68) and
      (byte(pointer(i+5)^) = $68) and
      (byte(pointer(i+6)^) = $76) and
      (byte(pointer(i+7)^) = $01) and
      (byte(pointer(i+8)^) = $00) and
      (byte(pointer(i+9)^) = $00) and
      (byte(pointer(i+10)^) = $E8)
      then begin
       SignAddr := i;
       break;
      end;
  if SignAddr > 0 then
   try CopyMemory(@result,  Pointer(SignAddr+1), 4); except end;
 end;
 function GetCmpCallCallBacksPtr : dword;
 var
  Section, i  : integer;
  StartAddr, ScanSize, SignAddr : dword;
 begin
  Result := 0;
  Section := PEImageLoader.GetSectionByName('PAGE');
  if Section = -1 then exit;
  StartAddr := PEImageLoader.ImageSections[Section].PhOffset;
  ScanSize  := PEImageLoader.ImageSections[Section].PhSize;
  SignAddr := 0;
  // Ищем команду mov     ds:KeServiceDescriptorTable, offset off_40DF40
  for i := dword(PEImageLoader.FS.Memory)+StartAddr to dword(PEImageLoader.FS.Memory) + ScanSize  do
   if (byte(pointer(i+0)^) = $08B) and
      (byte(pointer(i+1)^) = $0FF) and
      (byte(pointer(i+2)^) = $055) and
      (byte(pointer(i+3)^) = $08B) and
      (byte(pointer(i+4)^) = $0EC) and
      (byte(pointer(i+15)^) = $0BB) and
      (byte(pointer(i+20)^) = $053) and
      (byte(pointer(i+21)^) = $0E8)
      then begin
       SignAddr := i;
       break;
      end;
  if SignAddr > 0 then
   Result := SignAddr;
 end;
 // Поиск имени модуля ядра
 function GetKernelModuleInfo(Addr : dword) : string;
 begin
  Result := GetKernelModuleByAddr(JmpAddr);
  if  Result <> '' then begin
   if FileSignCheck.CheckFile(NormalProgramFileName(Result, '.sys')) = 0 then
    Result := Result + ', $AVZ0232'
   else
    VirFileList.AddVir(NormalProgramFileName(Result, '.sys'), '$AVZ0707', vrtVirusRootKit);
  end;
 end;
begin
 if StopF then exit;
  // Очистка старых данных
 XMLLogger.ClearByName('RK_KM');

 // Получение имени файла ядра
 KernelFileName := GetKernelFileName;
 AddToDebugLog('GetKernelFileName="'+KernelFileName+'"'); //#DLS
 KernelFileName := NormalProgramFileName(KernelFileName,'.exe');
 AddToDebugLog('KernelFileName="'+KernelFileName+'"');    //#DLS
 if (Trim(KernelFileName) = '') then begin
  AddToLog(' $AVZ0674 ('+S+')', logError);
  exit;
 end;
 // Поиск файла ядра
 if (not(FileExists(KernelFileName))) then
  if (copy(KernelFileName, 1, 1) = '\') or (copy(KernelFileName, 1, 1) = '/') then
   KernelFileName := copy(GetSystemDirectoryPath, 1, 2) + KernelFileName;
 if (not(FileExists(KernelFileName))) then begin
  KernelFileName := copy(GetSystemDirectoryPath, 1, 2) + GetKernelFileName;
  if (not(FileExists(KernelFileName))) then begin
   AddToLog(' $AVZ0632 ('+KernelFileName+')', logError);
   exit;
  end;
 end;
 // Получение адреса ядра в памяти
 KernelBase := GetKernelBase;
 KernelSize := GetKernelSize;
 // Построение списка функций ядра по анализу NTDLL
 KFNamesList := TStringList.Create;
 GetKernelFunctionList(KFNamesList);
 // Загрузка ядра моим методом, через PEImageLoader
 PEImageLoader :=  TPEImageLoader.Create;
 if not(PEImageLoader.LoadPEFile(KernelFileName)) then begin
  AddToLog('>>> $AVZ1227', logAlert);
  // Загрузить не удалось ?
  S := AVZTempDirectoryPath + 'avz_'+IntToStr(GetCurrentProcessId)+'_k.dat';
  if DirectCopyFile(KernelFileName, S) then begin
   PEImageLoader.LoadPEFile(S);
   DeleteFile(S);
  end;
 end;
 // Расчет релокейшенов на реальный адрес ядра в памяти
 PEImageLoader.ExecuteReallocation(KernelBase);
 // Получение таблицы экспорта
 ExportTable := TStringList.Create;
 PEImageLoader.GetExportTable(ExportTable);
 KeServiceDescriptorTableRVA := 0;
 // Поиск KeServiceDescriptorTable
 if ExportTable.IndexOf('KeServiceDescriptorTable') >= 0 then
  KeServiceDescriptorTableRVA := DWORD(ExportTable.Objects[ExportTable.IndexOf('KeServiceDescriptorTable')]);
 if KeServiceDescriptorTableRVA = 0 then begin
  AddToLog(' $AVZ0671 '+ExtractFileName(KernelFileName), logError);
  exit;
 end;
 // Инсталляция и загрузка драйвера
 AVZDriver.InstallDriver;
 AVZDriver.LoadDriver;
 // Контроль - если драйвер не загрузился, то продолжать нет смысла
 if AVZDriver.Loaded then
  AddToLog(' $AVZ0234', logInfo)
 else begin
  AddToLog(' $AVZ0657'+' ['+(IntToHex(AVZDriver.LastStatus, 8))+']', logAlert);
  exit;
 end;
 if not(AVZDriver.CallDriver_IOCTL_Z_SETTRUSTEDPID) then begin
  AddToLog(' $AVZ0666 '+' ['+(IntToHex(AVZDriver.LastStatus, 8))+'] - [1]', logAlert);
  exit;
 end;
 // Логический контроль
 if KernelBase < 1000 then begin
  AddToLog(' $AVZ0673 '+ExtractFileName(KernelFileName)+' - $AVZ0832', logError);
  exit;
 end;
 AddToLog(' $AVZ0026 (RVA='+IntToHex(KeServiceDescriptorTableRVA, 6)+')', logInfo);
 AddToLog(' $AVZ1157 '+ExtractFileName(KernelFileName)+' $AVZ0539 '+IntToHex(KernelBase, 6), logInfo);
 // Опрос драйвера - узнаем текущие адреса KESDT, KiServiceTable и кол-во записей в KESDT
 AddToDebugLog('$AVZ0594');
 KESystemDescriptorTableAddress := AVZDriver.CallDriver_GET_KESDT_PTR;
 SDTTableAddress                := AVZDriver.CallDriver_GET_SDT_PTR;
 SDTItemsCount                  := AVZDriver.CallDriver_GET_SDT_COUNT;
 AddToDebugLog('$AVZ0595');
 // Логический контроль полученных значений
 if (SDTItemsCount < 50) or (SDTTableAddress < KernelBase) or (KESystemDescriptorTableAddress < KernelBase) then begin
  AddToLog('', logError);
  exit;
 end;
 // Поиск KiServiceTable в файле
 SDTTableAddressF := 0;
 // Поиск секции INIT - код размещается в ней. Если не найдено, то поиск от начала файла
 if PEImageLoader.GetSectionByName('init') >=0 then //#DLS
  dw := PEImageLoader.ImageSections[PEImageLoader.GetSectionByName('init')].PhOffset //#DLS
   else dw := 0;
 dw := 0; // контроль скорости поиска - убрать !!!
 // Ищем команду mov     ds:KeServiceDescriptorTable, offset off_40DF40
 // C7 05 xx xx xx yy yy yy yy (где xx xx xx xx = KeServiceDescriptorTable, yy yy yy yy - искомый адрес)
 for i := dword(PEImageLoader.FS.Memory)+dw to dword(PEImageLoader.FS.Memory) + PEImageLoader.FS.Size  do
  if (byte(pointer(i)^) = $C7) and (byte(pointer(i+1)^) = $05) then
   if dword(pointer(i+2)^) = KeServiceDescriptorTableRVA + KernelBase then begin
    SDTTableAddressF := dword(pointer(i+6)^);
    dw := i;
    Break;
   end;
 // Не сработало ? Значит, данной команды в секции INIT нет - продолжать нельзя
 if SDTTableAddressF = 0 then begin
  AddToLog(' $AVZ0672 ', logError);
  exit;
 end;
 // Нашли, проводим самоконтроль - рядом должен быть код указанного вида. Если он есть, то я нашел код инициализации
 // BE 00 C5 48 00                                mov     esi, offset KeServiceDescriptorTable
 // BF C0 C4 48 00                                mov     edi, offset dword_48C4C0
 FoundSDT1Fl       := false;
 SDTTableAddress1F := 0;
 for i := dw to min(dword(PEImageLoader.FS.Memory) + PEImageLoader.FS.Size, dw+200)  do
  if (byte(pointer(i)^) = $0BE) and (dword(pointer(i+1)^) = KeServiceDescriptorTableRVA + KernelBase) and (byte(pointer(i+5)^) = $BF) then begin
   FoundSDT1Fl := true;
   SDTTableAddress1F := dword(pointer(i+6)^); // Запомнили адрес таблицы 1 (?)
   // Считали по этому адресу  собственно адрес SDTTableAddress1F
   PEImageLoader.FS.Seek(PEImageLoader.GetPhAddByRVA(SDTTableAddress1F - KernelBase), 0);
   PEImageLoader.FS.Read(SDTTableAddress1F, 4);
   Break;
  end;
 // Нашли, значит можно попробовать поиск размера SDT - он выше, примерно 90-100 байт
 // INIT:005DAEA3 A1 C0 2F 40 00                                mov     eax, ds:SDT_Size
 SDTItemsCountF := 0;
 if FoundSDT1Fl then begin
  dw := dw - 150; // Реально смещение = 93 байта для XP SP2 ...
  for i := dw to min(dword(PEImageLoader.FS.Memory) + PEImageLoader.FS.Size, dw+200)  do
   if (byte(pointer(i)^) = $0A1) and PEImageLoader.DwordIsReallocated(i+1) then begin
    SDTItemsCountF := dword(pointer(i+1)^);
    PEImageLoader.FS.Seek(PEImageLoader.GetPhAddByRVA(SDTItemsCountF - KernelBase), 0);
    PEImageLoader.FS.Read(SDTItemsCountF, 4);
    AddToDebugLog('KiST Size ($AVZ0403 1.1)= '+IntToStr(SDTItemsCountF));
    break;
   end;
  dw := dw - 150; // Реально смещение = 93 байта для XP SP2 ...
  for i := dw to min(dword(PEImageLoader.FS.Memory) + PEImageLoader.FS.Size, dw+200)  do
   if (byte(pointer(i)^) = $08B) and (byte(pointer(i+1)^) = $15) and PEImageLoader.DwordIsReallocated(i+2) then begin
    SDTItemsCountF := dword(pointer(i+2)^);
    PEImageLoader.FS.Seek(PEImageLoader.GetPhAddByRVA(SDTItemsCountF - KernelBase), 0);
    PEImageLoader.FS.Read(SDTItemsCountF, 4);
    AddToDebugLog('KiST $AVZ0893 ($AVZ0403 1.2)= '+IntToStr(SDTItemsCountF));
    break;
   end;
 end;

 // Вычисление физического адреса KiST в файле
 FunctionPhAddr := PEImageLoader.GetPhAddByRVA(SDTTableAddressF - KernelBase);
 PEImageLoader.FS.Seek(FunctionPhAddr, 0);
 // Попытка вычислить реальный размер KiST анализом адресов - поиск до первого кривого адреса
 i := 0;
 if not(AVZDriver.CallDriver_IOCTL_Z_SETTRUSTEDPID) then begin
  AddToLog(' $AVZ0666 '+' ['+(IntToHex(AVZDriver.LastStatus, 8))+'] - [1]', logAlert);
  exit;
 end;
 while i < SDTItemsCount*5 do begin
  PEImageLoader.FS.Read(dw, 4);
  if PEImageLoader.GetSectionByRVA(dw-KernelBase) < 0 then break;
  inc(i);
 end;
 AddToDebugLog('KiST Size ($AVZ0403 2)= '+IntToStr(i));
 AddToDebugLog('KiST Size ($AVZ0403 3)= '+IntToStr(KFNamesList.Count));

 FunctionPhAddr := PEImageLoader.GetPhAddByRVA(SDTTableAddressF - KernelBase);
 PEImageLoader.FS.Seek(FunctionPhAddr, 0);
 // Вычисление размера SDT
 SDTCheckedItemsCount := SDTItemsCount;
 if (KFNamesList.Count > 50) and (KFNamesList.Count < SDTCheckedItemsCount) then
  SDTCheckedItemsCount := KFNamesList.Count;
 StatCheckedFn := 0; StatRootKitFn := 0; StatRestoreFn := 0;
 AddToLog('   SDT = '+IntToHex(KESystemDescriptorTableAddress, 6), logInfo); //#DLS
 AddToLog('   KiST = '+IntToHex(SDTTableAddress, 6)+' ('+IntToStr(SDTItemsCount)+')', logInfo); //#DLS
 // Анализ перемещения таблицы
 if SDTTableAddress <> SDTTableAddressF then begin
  AddToLog(' >>> $AVZ0125 ('+IntToHex(SDTTableAddressF, 6)+'('+IntToStr(SDTCheckedItemsCount)+')'+'->'+IntToHex(SDTTableAddress, 6)+'('+IntToStr(SDTItemsCount)+')'+')', logAlert);
 end;
 ZProgressClass.AddProgressMax(SDTCheckedItemsCount);
 for i := 0 to SDTCheckedItemsCount - 1 do begin
  if StopF then Break;
  ZProgressClass.ProgressStep;
  inc(StatCheckedFn);
  PEImageLoader.FS.Read(dw, 4);
  dw1 := AVZDriver.CallDriver_GET_SDT_ENTRY(i);
  if dw1 = 0 then begin
   AddToLog(' $AVZ0666 '+' ['+(IntToHex(AVZDriver.LastStatus, 8))+'] - [2]', logAlert);
   exit;
  end;
  //AddToLog(GetKernelFunctionName(KFNamesList, i)+#9+IntToHex(i, 8)+' = '+inttohex(dw1, 8));
  // Адреса не совпадают ??
  if dw <> dw1 then begin
   inc(StatRootKitFn);
   // Поиск дравера-перехватчика по адресу
   HookFileName := GetKernelModuleByAddr(dw1);
   if HookFileName <> '' then begin
    HookFileName := NormalProgramFileName(HookFileName, '.sys');
    VirFileList.AddVir(HookFileName, '$AVZ0707', vrtVirusRootKit);
    S := ', $AVZ0706 '+HookFileName;
    // Проверка по базе безопасных
    if FileSignCheck.CheckFile(HookFileName) = 0 then
     S := S + ', $AVZ0232';
   end else S := ', $AVZ0710';
   AddToLog('$AVZ1135 '+GetKernelFunctionName(KFNamesList, i, true)+' ('+IntToHex(i,2)+') $AVZ0712 ('+IntToHex(dw, 6)+'->'+IntToHex(dw1, 6)+')'+S, logAlert);
   // Добавление в XML базу
   FileInfoStr := FormatXMLFileInfo(HookFileName, true);
   XMLLogger.AddItem('RK_KM', '<ITEM File="'+HookFileName+'" '+
                              ' FNaim="'+GetKernelFunctionName(KFNamesList, i, true)+'"'+
                              ' FIndx="'+IntToStr(i)+'"'+
                              ' HookPtr="'+IntToHex(dw1, 6)+'"'+
                              ' HookType="1" '+
                              FileInfoStr+
                              '/>');
   // Восстановление адреса
   if ARestoreCode then begin
    if AVZDriver.CallDriver_SET_SDT_ENTRY(i, dw) then begin
     AddToLog('>>> $AVZ1136 !', logAlert);
     Buf[0] := $E9;
     Buf[5] := $90;
     // Вычисление смещения для JMP и его запись в байты 2-5 буфера
     dwJMP := dword(dw)  - (dword(dw1) + 5);
     CopyMemory(@Buf[1], @dwJMP, 4);
     ZeroMemory(@buf1[0], 5);
     AVZDriver.CallDriver_READ_MEMORY(@buf1[0], dw1, 5);
     if (buf1[0] = buf[0]) and (buf1[1] = buf[1]) and (buf1[2] = buf[2]) and (buf1[3] = buf[3]) and (buf1[4] = buf[4]) then
      AddToLog('>>> $AVZ0342', logAlert)
     else
      if AVZDriver.CallDriver_WRITE_MEMORY(@buf[0], dw1, 5) then
       AddToLog('>>> $AVZ0341', logAlert)
      else
       AddToLog('>>> $AVZ0665', logAlert);
     dw1 := dw;
     inc(StatRestoreFn);
    end;
   end;
  end;
  // Адреса совпали (или адрес восстановлен) ?? Если да, то сравним код
  if dw = dw1 then begin
   // Чтение реального кода из памяти
   if AVZDriver.CallDriver_READ_MEMORY(@Buf[0], dw, 20) then begin
    // Чтение кода из файла
    CopyMemory(@Buf1[0],
               Pointer(DWORD(PEImageLoader.FS.Memory)+PEImageLoader.GetPhAddByRVA(dw - KernelBase)), 20);
    MemCodeHijack := false;
    MemCodeHijackRel := 0;
    for j := 0 to 19 do
     if Buf[j] <> Buf1[j] then begin
      if MemCodeHijackRel = 0 then
       MemCodeHijackRel := j;
      MemCodeHijack := true;
      AddToDebugLog(IntToStr(j)+' '+IntToHex(Buf1[j], 2)+' -> '+IntToHex(Buf[j], 2));
     end;
    // Машинный код изменен ?
    if MemCodeHijack then begin
     RealCodeRel := 0;
     HookFileName := '';
     S := '';
     // пропуск совпадающих байт
     while (Buf[RealCodeRel] = Buf1[RealCodeRel]) and (RealCodeRel < 15) do inc(RealCodeRel);
     // пропуск NOP
     while (Buf[RealCodeRel] = $90) and (RealCodeRel < 15) do inc(RealCodeRel);
     //AddToList(S, inttohex(dword(pointer(@Buf2[RealCodeRel+1])^), 6));
     // Попытка определения метода перехвата
     case Buf[RealCodeRel] of
      $0CC : S := '$AVZ0404';
      $0E9 : begin
              S := '$AVZ0405';
              JmpAddrBytes[0] := Buf[RealCodeRel + 1 + 0];
              JmpAddrBytes[1] := Buf[RealCodeRel + 1 + 1];
              JmpAddrBytes[2] := Buf[RealCodeRel + 1 + 2];
              JmpAddrBytes[3] := Buf[RealCodeRel + 1 + 3];
              JmpAddr := dword(JmpAddr+(dw+RealCodeRel+5));
              // Поиск драйвера-перехватчика
              S := S + ' jmp '+IntToHex(JmpAddr, 6)+GetKernelModuleInfo(JmpAddr);
              HookFileName := GetKernelModuleByAddr(JmpAddr);
             end;
      $06A,
      $068,
      $050,
      $053,
      $056,
      $057,
      $059 : S := '$AVZ0406';
      else S:= '$AVZ0408';
     end;
     if RealCodeRel > 0 then
      S := S+', $AVZ0113 '+IntToStr(RealCodeRel);
     AddToLog('$AVZ1135 '+GetKernelFunctionName(KFNamesList, i, true)+' ('+IntToHex(i,2)+') - $AVZ0428 '+S, logAlert);
     // Добавление в XML базу
     FileInfoStr := FormatXMLFileInfo(HookFileName, true);
     XMLLogger.AddItem('RK_KM', '<ITEM File="'+HookFileName+'" '+
                              ' FNaim="'+GetKernelFunctionName(KFNamesList, i, true)+'"'+
                              ' FIndx="'+IntToStr(i)+'"'+
                              ' HookPtr="'+IntToHex(dw1, 6)+'"'+
                              ' HookType="2" '+
                              FileInfoStr+
                              '/>');
     if MemCodeHijackRel < 3 then // !!! Восстанавливаем только внедрение с начала кода
     if ARestoreCode then begin
      if AVZDriver.CallDriver_WRITE_MEMORY(@Buf1[0], dw, 15) then begin
       AddToLog('>>> $AVZ1136 !', logAlert);
       inc(StatRestoreFn);
      end;
     end
    end;
   end else begin
    AddToLog('$AVZ1135 '+GetKernelFunctionName(KFNamesList, i, true)+' $AVZ0687', logError);
   end;
  end;
 end;
 // ************** Проверка функций, экспортируемых ядром *****************
 KernelExport := TStringList.Create;
 PEImageLoader.GetExportTable(KernelExport);
 // Подготовка строки параметров
 KernelExportSetup := LowerCase(RootkitBase.GetParam('NO_SCAN_KRNL_EXP')+',');
 KernelExportSetup := StringReplace(KernelExportSetup, #$0D, '', [rfReplaceAll]);
 KernelExportSetup := StringReplace(KernelExportSetup, #$0A, '', [rfReplaceAll]);
 KernelExportSetup := StringReplace(KernelExportSetup, ' ', '', [rfReplaceAll]);
 //KernelExport.Clear;
 for i := 0 to KernelExport.Count - 1 do
  if pos(LowerCase(KernelExport[i])+',', KernelExportSetup) = 0 then
   if dword(KernelExport.Objects[i]) > 0 then begin
     dw := KernelBase + dword(KernelExport.Objects[i]);
     // Чтение реального кода из памяти
     if PEImageLoader.GetPhAddByRVA(dw - KernelBase) > 1000 then
       if AVZDriver.CallDriver_READ_MEMORY(@Buf[0], dw, 20) then begin
        if i mod 10 = 0 then begin
//!!!!!!!!         StatusBar.Panels[0].Text := ExtractFileName(KernelFileName)+':'+KernelExport[i];
//!!!!!!!!         Application.ProcessMessages;
        end;
        // Чтение кода из файла
        CopyMemory(@Buf1[0],
                   Pointer(DWORD(PEImageLoader.FS.Memory)+PEImageLoader.GetPhAddByRVA(dw - KernelBase)), 20);
        MemCodeHijack := false;
        for j := 0 to 19 do
         if Buf[j] <> Buf1[j] then begin
          MemCodeHijack := true;
          AddToDebugLog(IntToStr(j)+' '+IntToHex(Buf1[j], 2)+' -> '+IntToHex(Buf[j], 2));
         end;
        // Машинный код изменен ?
        if MemCodeHijack then begin
         RealCodeRel := 0;
         S := '';
         HookFileName := '';
         // пропуск совпадающих байт
         while (Buf[RealCodeRel] = Buf1[RealCodeRel]) and (RealCodeRel < 15) do inc(RealCodeRel);
         // пропуск NOP
         while (Buf[RealCodeRel] = $90) and (RealCodeRel < 15) do inc(RealCodeRel);
         // Попытка определения метода перехвата
         case Buf[RealCodeRel] of
          $0CC : S := '$AVZ0404';
          $0E9 : begin
                  S := '$AVZ0405';
                  JmpAddrBytes[0] := Buf[RealCodeRel + 1 + 0];
                  JmpAddrBytes[1] := Buf[RealCodeRel + 1 + 1];
                  JmpAddrBytes[2] := Buf[RealCodeRel + 1 + 2];
                  JmpAddrBytes[3] := Buf[RealCodeRel + 1 + 3];
                  JmpAddr := dword(JmpAddr+(dw+RealCodeRel+5));
                  // Поиск драйвера-перехватчика
                  S := S + ' jmp '+IntToHex(JmpAddr, 6)+' '+GetKernelModuleInfo(JmpAddr);
                  HookFileName := GetKernelModuleByAddr(JmpAddr);
                 end;
          $06A,
          $068,
          $050,
          $053,
          $056,
          $057,
          $059 : S := '$AVZ0406';
          else S:= '$AVZ0408';
         end;
         if RealCodeRel > 0 then
          S := S+', $AVZ0113 '+IntToStr(RealCodeRel);
         AddToLog('$AVZ1135 '+KernelExport[i]+' ('+IntToHex(dw,8)+') - $AVZ0428 '+S, logAlert);
         // Добавление в XML базу
         FileInfoStr := FormatXMLFileInfo(HookFileName, true);
         XMLLogger.AddItem('RK_KM', '<ITEM File="'+HookFileName+'" '+
                              ' FNaim="'+GetKernelFunctionName(KFNamesList, i, true)+'"'+
                              ' FIndx="'+IntToStr(i)+'"'+
                              ' HookPtr="'+IntToHex(dw1, 6)+'"'+
                              ' HookType="3" '+
                              FileInfoStr+
                              '/>');
         if ARestoreCode then begin
          if AVZDriver.CallDriver_WRITE_MEMORY(@Buf1[0], dw, 15) then begin
           AddToLog('>>> $AVZ1136 !', logAlert);
           inc(StatRestoreFn);
          end;
         end;
      end;
   end;
  end;
 AddToLog('$AVZ0807: '+IntToStr(StatCheckedFn)+', $AVZ0718: '+IntToStr(StatRootKitFn)+', $AVZ0149: '+IntToStr(StatRestoreFn), logInfo);
 KernelRootKitFunctionsCount := StatRestoreFn;
 AddToLog('1.3 $AVZ0811', logInfo);
 // Проверка таблицы IDT
 ZeroMemory(@SI, SizeOf(SI));
 GetSystemInfo(SI);
 NumberOfProcessors := SI.dwNumberOfProcessors;
 if NumberOfProcessors = 0 then NumberOfProcessors := 1;
 SysenterAddrF := GetSysenterAddr;
 AddToDebugLog('SysenterAddrF = '+IntToHex(SysenterAddrF, 8));
 for CurrentCPU := 1 to NumberOfProcessors do begin
   AddToLog(' $AVZ0060 '+IntToStr(CurrentCPU), logInfo);
   zSetProcessAffinityMask(GetCurrentProcess, 1 shl (CurrentCPU-1));
   if AVZDriver.CallDriver_GET_IDT(IDT) then begin
    IDT.Size := IDT.Size + 1;
    AddToDebugLog('IDT Size = '+IntToStr(IDT.Size));
    AddToDebugLog('IDT Base = '+IntToHex(IDT.Base, 8));
    IDTSetup := RootkitBase.GetParam('IDT')+',';
    SetLength(IDT_GATES, IDT.Size div sizeof(TIDTGATE));
    AVZDriver.CallDriver_READ_MEMORY(@IDT_GATES[0], IDT.Base, Length(IDT_GATES)*sizeof(TIDTGATE));
    AddToDebugLog('IDT record cnt = '+IntToStr(Length(IDT_GATES)));
    for i := 0 to Length(IDT_GATES) - 1 do
     if IDT_GATES[i].flags and 7 = 6 then begin
      dw := IDT_GATES[i].off1 + IDT_GATES[i].off2 shl 16;
      if not((dw > KernelBase) and (dw < KernelBase + KernelSize)) and (pos(IntToHex(i, 2)+',', IDTSetup)>0) then begin
       S := GetKernelModuleByAddr(dw);
       if S <> '' then S := NormalProgramFileName(S, '.sys');
       if S <> '' then begin
        VirFileList.AddVir(S, '>>> $AVZ0709['+IntToStr(CurrentCPU)+'].IDT['+IntToHex(i,2)+']', vrtVirusRootKit);
        if FileSignCheck.CheckFile(S) = 0 then
         S := S + ', $AVZ0232';
       end;
      AddToLog('>>> $AVZ0574['+IntToStr(CurrentCPU)+'].IDT['+IntToHex(i,2)+'] = ['+inttohex(dw, 6)+']'+' '+S, logAlert);
     end;
    end;
   end else
    AddToLog('$AVZ0675', logError);
   // Проверка SYSEnter
   dw := AVZDriver.CallDriver_GET_CPU_176REG;
   if dw <> 0 then begin
    AddToDebugLog('CPU_176REG (SYSENTER) = '+inttohex(dw, 6));
    if ((SysenterAddrF = 0) and not((dw > KernelBase) and (dw < KernelBase + KernelSize))) or
       ((SysenterAddrF > 0) and (dw <> SysenterAddrF)) then begin
     S := GetKernelModuleByAddr(dw);
     if S <> '' then S := NormalProgramFileName(S, '.sys');
     if S <> '' then begin
      VirFileList.AddVir(S, '>>> $AVZ0708', vrtVirusRootKit);
      if FileSignCheck.CheckFile(S) = 0 then
       S := S + ', $AVZ0232';
     end;
     AddToLog('>>> $AVZ0574['+IntToStr(CurrentCPU)+'].SYSENTER='+inttohex(dw, 6)+' '+S, logAlert);
     if ARestoreCode and (SysenterAddrF > 0) and (SysenterAddrF > KernelBase) and (SysenterAddrF < KernelBase + KernelSize) then
       if AVZDriver.CallDriver_SET_CPU_176REG(SysenterAddrF) then
        AddToLog(' ЦП['+IntToStr(CurrentCPU)+'].$AVZ0028', logInfo);
    end;
   end;
 end;
 // Отключили привязку к ЦП
 zSetProcessAffinityMask(GetCurrentProcess, 0);
 // *********** Нейтрализация колбеков ************
 // CmpCallCallBacks
 if ARestoreCode then begin
   dw := GetCmpCallCallBacksPtr;
   AddToLog('CmpCallCallBacks = '+IntToHex(dw, 8),  logAlert);
   if  dw > 0 then begin
    dw := KernelBase + dword(KernelExport.Objects[i]);
    if AVZDriver.CallDriver_Read_MEMORY(@Buf[0], dw, 19) then
     AddToLog('Code = '+IntToHex(Buf[13], 2),  logAlert);
    // Модикациция кода в памяти
    Buf[0] := $C9; Buf[1] := $C2; Buf[2] := $08; Buf[3] := $00;
    if AVZDriver.CallDriver_Write_MEMORY(@Buf[0], dw, 4) then
     AddToLog('Disable callback OK',  logAlert);
   end;
 end;
 AddToLog(' $AVZ0812', logInfo);
 KFNamesList.Free;
 PEImageLoader.Free;
 ExportTable.Free;
end;

function ExtSearchHiddenObjects: boolean;
var
 PROCESS_LIST : TPROCESS_LIST;
 DRIVER_LIST  : TDRIVER_LIST;
 i, j, poz : integer;
 PL, ProcessList : TStringList;
 S, ST : string;
 dw : dword;
 SysProcessInfo : TSysProcessInfo;
begin
 PL           := TStringList.Create;
 ProcessList  := TStringList.Create;
 AddToLog('1.4 $AVZ0764', logInfo);
 AddToDebugLog('CPL-PM [1]');
 if (AVZDriverRK = nil) or not(AVZDriverRK.Loaded) then begin
  AddToLog(' $AVZ0827', logInfo);
  exit;
 end;
 AddToDebugLog('CPL-PM [2]');
 // Установка нашему процессу права на управление драйвером
 AVZDriverRK.CallDriver_IOCTL_Z_SETTRUSTEDPID;
 if not(AVZDriverRK.IOCTL_Z_GET_PROCESSLIST(PROCESS_LIST)) then
  AddToLog(' $AVZ0662', logInfo)
 else begin
   // Получение списка процессов
   CreateNativeProcessList(PL);
   // Заполнение полных путей к процессу
   if ISNT then
    SysProcessInfo := TPSAPIProcessInfo.Create
     else SysProcessInfo := TToolHelpProcessInfo.Create;
   // Построение списка процессов
   SysProcessInfo.CreateProcessList(ProcessList);
   for i := 0 to PL.Count - 1 do
    for j := 0 to ProcessList.Count - 1 do
     if integer(PL.Objects[i]) = (ProcessList.Objects[j] as TProcessInfo).PID then begin
       PL[i] := NTFileNameToNormalName((ProcessList.Objects[j] as TProcessInfo).ExeFileName);
     end;
   ProcessList.Free;
   // Цикл проверки
   for i := 0 to $FF-1 do
    if PROCESS_LIST[i].PID <> $FFFFFFFF then begin
     poz := -1;
     for j := 0 to PL.Count - 1 do begin
      if integer(PL.Objects[j]) = PROCESS_LIST[i].PID then begin
       poz := j;
       break;
      end;
     end;
    if poz < 0 then begin
      S := '$AVZ0386='+IntToStr(PROCESS_LIST[i].PID)+', $AVZ0301 = "'+PROCESS_LIST[i].NAME+'"';
     if AVZDriverRK.IOCTL_Z_GET_EPROCESS_FULLNAME(i, ST) then
      S := S + ', $AVZ0779 = ' + '"' + ST + '"';
     // Вывод данных о процессе
     AddToLog(S, logAlert);
     // Проверка подмены PID
     AVZDriverRK.IOCTL_Z_GET_EPROCESS_PID(i, dw);
     if dw <> PROCESS_LIST[i].PID then
      AddToLog(' >> $AVZ0536='+ inttostr(dw)+', $AVZ0904 = '+IntToStr(PROCESS_LIST[i].PID)+')', logAlert);
     if AVZDriverRK.IOCTL_Z_GET_EPROCESS_NAME(i, ST) then
      if Trim(ST) <> Trim(PROCESS_LIST[i].NAME) then
       AddToLog(' >> $AVZ0537 = "'+ST+'"', logAlert);
    end else begin
     if AVZDriverRK.IOCTL_Z_GET_EPROCESS_FULLNAME(i, ST) then begin
      ST := Trim(ST);
      if pos(AnsiLowerCase(copy(PL[poz], 3, maxint)), AnsiLowerCase(ST)) = 0 then begin
       AddToLog('$AVZ0104'+IntToStr(PROCESS_LIST[i].PID)+', $AVZ0301 = "'+ST+'"', logAlert);
       AddToLog(' >> $AVZ0537 = "'+PL[poz]+'"', logAlert);
      end;
     end;
    end;
  end;
 end;
 PL.Clear;
 GetKernelModuleList(PL);
 ZeroMemory(@DRIVER_LIST, sizeof(DRIVER_LIST));
 // Установка нашему процессу права на управление драйвером
 AVZDriverRK.CallDriver_IOCTL_Z_SETTRUSTEDPID;
 if not(AVZDriverRK.IOCTL_Z_GET_DRIVERSLIST(DRIVER_LIST)) then
  AddToLog(' $AVZ0661', logInfo)
 else
 AddToDebugLog('CPL-PM [3]');
 for i := 0 to 1023 do
  if DRIVER_LIST[i].Base > 0 then begin
    poz := -1;
    for j := 0 to PL.Count - 1 do
     if dword(PL.Objects[j]) = DRIVER_LIST[i].Base then begin
      poz := j;
      break;
     end;
    if poz < 0 then begin
     S := '>> $AVZ0383: Base='+IntToHex(DRIVER_LIST[i].Base, 8)+
          ', $AVZ0893='+ IntToStr(DRIVER_LIST[i].Size);
     if AVZDriverRK.IOCTL_Z_GET_DRIVER_FULLNAME(DRIVER_LIST[i].Base, ST) then
      S := S + ', $AVZ0301 = "'+ST+'"';
     AddToLog(S, logAlert);
    end;
  end;
 PL.Free;
 AddToLog(' $AVZ0765', logInfo)
 //RtlVolumeDeviceToDosName , IoVolumeDeviceToDosName
end;

function ScanRootKit(ADestroyRootkitUser, ADestroyRootKitKernel : boolean): boolean;
var
 i, j : integer;
 ServiceManager  : TServiceManager;
 PL1, PL2, PL3   : TStringList;
 FSysProcessInfo : TSysProcessInfo;
 SA1a, SA1r, SA2a, SA2r, SA3a, SA3r  : TServiceArray;
 FoundFl  : boolean;
 S : string;
 RootkitBaseLoaded : boolean;
 HiddenProc : TStrings;
 SR : TSearchRec;
 Step, FoundIndex : integer;
begin
 PL1 :=nil; PL2 :=nil; PL3 :=nil;
 ServiceManager := nil; FSysProcessInfo := nil;
   // Очистка старых данных
 XMLLogger.ClearByName('RK_UM');
 try
   Result := false;
   Step := 1;
   ServiceManager := TServiceManager.Create;
   PL1            := TStringList.Create;
   PL2            := TStringList.Create;
   PL3            := TStringList.Create;
   // Загрузка базы антируткита
   RootkitBaseLoaded := RootkitBase.LoadBinDatabase;
   Step := 2;
   AddToDebugLog('$AVZ0257');
   if not RootkitBaseLoaded then begin
    AddToLog('$AVZ0119', logError);
   end;
   AddToDebugLog('$AVZ0310');
   // Создание диспетчера процессов
   if ISNT then
    FSysProcessInfo := TPSAPIProcessInfo.Create else
     FSysProcessInfo := TToolHelpProcessInfo.Create;
   Step := 3;
   if ISNT then begin
    // Построение списка процессов
    FSysProcessInfo.CreateProcessList(PL3);
    AddToDebugLog('$AVZ0762 1');
    CreateNativeProcessList(PL1);
    Step := 4;
    AddToDebugLog('$AVZ0762 2');
    CreateNativeProcessListAntiRootKit(PL2);
    AddToDebugLog('$AVZ0762 3');
    Step := 5;
    if PL2.Count <> PL1.Count then
     AddToLog(' >> $AVZ0570', logAlert);
    for i := 0 to PL2.Count - 1 do begin
     FoundFl := false;
     for j := 0 to PL1.Count - 1 do
      if dword(PL1.Objects[j]) = dword(PL2.Objects[i]) then begin
       FoundFl := true;
       FoundIndex := j;
       break;
      end;
     if not(FoundFl) then begin
      AddToLog(' >>>> $AVZ0534 '+IntToStr(dword(PL2.Objects[i]))+' '+NormalProgramFileName(PL2[i], '.exe'), logAlert);
      Step := 6;
      VirFileList.AddVir(NTFileNameToNormalName(PL2[i]), '$AVZ0729', vrtVirusRootKit);
     end else
      if ExtractFilename(AnsiLowerCase(PL2[i])) <> ExtractFilename(AnsiLowerCase(PL1[FoundIndex])) then
       AddToLog(' >>>> $AVZ0130 '+IntToStr(dword(PL2.Objects[i]))+' '+NTFileNameToNormalName(PL2[i]), logAlert);
    end;
    try
      // Поиск подделки имени файла
      for i := 0 to PL3.Count - 1 do begin
       FoundFl := false;
       for j := 0 to PL2.Count - 1 do
        if (PL3.Objects[i] as TProcessInfo).PID = dword(PL2.Objects[j]) then begin
         FoundFl := true;
         FoundIndex := j;
         break;
        end;
       if FoundFl then
        if ExtractFilename(AnsiLowerCase(PL2[FoundIndex])) <> ExtractFilename(AnsiLowerCase((PL3.Objects[i] as TProcessInfo).ExeFileName)) then
         AddToLog(' >>>> $AVZ0129 '+IntToStr(dword(PL2.Objects[FoundIndex]))+' '+ExtractFileName(AnsiLowerCase((PL3.Objects[i] as TProcessInfo).ExeFileName))+', $AVZ0903 - '+NTFileNameToNormalName(PL2[FoundIndex]), logAlert);
      end;
    except
     on e : exception do
      AddToDebugLog('Error - antirutkit, CFN');
    end;
    Step := 7;
    AddToDebugLog('$AVZ0762 4');
    PL1.Clear;
    PL2.Clear;
   end;
   AddToDebugLog('$AVZ0763 PL1');
   // Построение списка процессов
   FSysProcessInfo.CreateProcessList(PL1);
   Step := 8;
   AddToDebugLog('$AVZ0766');
   for i := 0 to PL1.Count - 1 do
    if (PL1.Objects[i] as TProcessInfo).PID >= 10 then begin
     S :=  ExpandFileNameEx(NTFileNameToNormalName((PL1.Objects[i] as TProcessInfo).ExeFileName));
     if FindFirst(S, faAnyFile, SR) <> 0 then begin
       AddToLog(' >>>> $AVZ0733: '+S, logAlert);
       VirFileList.AddVir(S, '$AVZ0729', vrtVirusRootKit);
      end;
     FindClose(SR);
    end;
   Step := 9;
   // Построение списка сервисов
   if ISNT and (ADestroyRootkitUser or ADestroyRootKitKernel) then begin
    AddToDebugLog('$AVZ0763 SA1a');
    ServiceManager.RefreshServicesByAPI(2,2);
    SA1a := ServiceManager.ServiceArray;
    AddToDebugLog('$AVZ0763 SA1r');
    ServiceManager.RefreshServicesByReg(2,2);
    SA1r := ServiceManager.ServiceArray;
   end;
   Step := 10;
   // Шаг 1.1 - поиск перехватчиков UserMode
   AddToLog('1.1 $AVZ0770', logInfo);
   // Цикл проверки DLL
   ZProgressClass.AddProgressMax(length(RootkitBase.CheckedDLLs) * 10);
   for i := 0 to length(RootkitBase.CheckedDLLs) - 1 do begin
    if StopF then Break;
    ZProgressClass.ProgressStep(10);
    try
     Result := Result or CheckDLLforRootKit(GetSystemDirectoryPath,
                                            RootkitBase.CheckedDLLs[i].DllName,
                                            ADestroyRootkitUser,
                                            RootkitBase.CheckedDLLs[i].CheckFunct,
                                            RootkitBase.CheckedDLLs[i].NoCheckFunct);
    except
     on e : exception do
      AddToDebugLog('Error - usermode antirutkit, '+RootkitBase.CheckedDLLs[i].DllName+', '+e.Message);
    end;
   end;
   // Повторное построение списка сервисов
   if ISNT and (ADestroyRootkitUser or ADestroyRootKitKernel) then begin
    Step := 11;
    AddToDebugLog('$AVZ0762 2');
    ServiceManager.RefreshServicesByAPI(2,2);
    SA2a := ServiceManager.ServiceArray;
    ServiceManager.RefreshServicesByReg(2,2);
    SA2r := ServiceManager.ServiceArray;
    if length(SA1a) <> Length(SA2a) then begin
     AddToLog(' >> $AVZ0571', logAlert);
     for i := 0 to Length(SA2a)-1 do
      if not ServiceManager.ServiceExists(SA2a[i].Name, SA1a) then begin
       AddToLog(' >>>> $AVZ0729 '+SA2a[i].Name+' '+NTFileNameToNormalName(SA2a[i].BinFile), logAlert);
       VirFileList.AddVir(NTFileNameToNormalName(SA2a[i].BinFile), '$AVZ0729', vrtVirusRootKit);
      end;
    end;
    Step := 12;
    if length(SA2r) > Length(SA1r) then begin
     AddToLog(' >> $AVZ0572', logAlert);
     for i := 0 to Length(SA2r)-1 do
      if not(ServiceManager.ServiceExists(SA2r[i].Name, SA1r)) and
         (FileSignCheck.CheckFile(NTFileNameToNormalName(SA2r[i].BinFile)) <> 0) then begin
       AddToLog(' >>>> $AVZ0729 '+SA2r[i].Name+' '+NTFileNameToNormalName(SA2r[i].BinFile), logAlert);
       VirFileList.AddVir(NTFileNameToNormalName(SA2r[i].BinFile), '$AVZ0729', vrtVirusRootKit);
      end;
    end;
   Step := 13;
   // Поиск маскирующихся процессов
   FSysProcessInfo.CreateProcessList(PL2);
   if PL2.Count <> PL1.Count then begin
    AddToLog(' >> $AVZ0570', logAlert);
    for i := 0 to PL2.Count - 1 do begin
     FoundFl := false;
     for j := 0 to PL1.Count - 1 do
      if (PL1.Objects[j] as TProcessInfo).PID = (PL2.Objects[i] as TProcessInfo).PID then begin
       FoundFl := true;
       break;
      end;
     if not(FoundFl) and (FileSignCheck.CheckFile(NTFileNameToNormalName((PL2.Objects[i] as TProcessInfo).ExeFileName)) <> 0) then begin
      AddToLog(' >>>> $AVZ0732 '+IntToStr((PL2.Objects[i] as TProcessInfo).PID)+' '+NTFileNameToNormalName((PL2.Objects[i] as TProcessInfo).ExeFileName), logAlert);
      VirFileList.AddVir(NTFileNameToNormalName((PL2.Objects[i] as TProcessInfo).ExeFileName), '$AVZ0729', vrtVirusRootKit);
     end;
    end;
   end;
   end;
   Step := 14;
   // Поиск перехватчиков API под NT
   if ISNT and not(AVZGuardIsOn) then begin
    AddToLog('1.2 $AVZ0769', logInfo);
    // Вызов поиска перехватчиков
    CheckServiceDescriptorTableforRootKit(ADestroyRootKitKernel);
    Step := 15;
    if ADestroyRootKitKernel then begin
     ServiceManager.RefreshServicesByAPI(2,2);
     SA3a := ServiceManager.ServiceArray;
     ServiceManager.RefreshServicesByReg(2,2);
     SA3r := ServiceManager.ServiceArray;
     FSysProcessInfo.CreateProcessList(PL3);
     if length(SA3r) > Length(SA2r) then begin
      for i := 0 to Length(SA3r)-1 do
       if not(ServiceManager.ServiceExists(SA3r[i].Name, SA2r)) and
          (FileSignCheck.CheckFile(NTFileNameToNormalName(SA3r[i].BinFile)) <> 0) then begin
        if pos('avz.sys', LowerCase(NTFileNameToNormalName(SA3r[i].BinFile))) = 0 then begin
         AddToLog(' >>>> $AVZ0729 '+SA3r[i].Name+' '+NTFileNameToNormalName(SA3r[i].BinFile), logAlert);
         VirFileList.AddVir(NTFileNameToNormalName(SA3r[i].BinFile), '$AVZ0729', vrtVirusRootKit);
        end;
       end;
     end;
    end;
   end;
  Step := 16;
   // 1.3 Поиск маскировки
  HiddenProc  := TStringList.Create;
  SearchFUHiddenProc(HiddenProc);
  for i := 0 to HiddenProc.Count - 1 do
   if integer(HiddenProc.Objects[i]) <> 0 then
    if Trim(HiddenProc[i]) <> '' then begin
     AddToLog(' >>>> $AVZ0534 '+inttostr(integer(HiddenProc.Objects[i])) + ' ' +HiddenProc[i], logAlert);
     VirFileList.AddVir(NTFileNameToNormalName(HiddenProc[i]), '$AVZ0729', vrtVirusRootKit);
    end;
   Step := 17;
   ServiceManager.Free;
   FSysProcessInfo.Free;
   PL1.Free;
   PL2.Free;
   PL3.Free;
   // 1.4 Расширенный поиск маскировки
   ExtSearchHiddenObjects;
 except
  on e : Exception do begin
   AddToLog('$AVZ0643 [' + e.Message+'], $AVZ1147 ['+Inttostr(Step)+']', logError);
   Result := false;
  end;
 end;
end;

function DeleteVirus(AFileName, AVirus : string; AScript : dword; AUnregDLL : boolean = false) : boolean;
var
 UnregDLLRes : boolean;
 S           : string;
begin
 Result := false;
 // Добавление к списку удаленных файлов
 if DeletedVirusList.IndexOf(AFileName) < 0 then
  DeletedVirusList.Add(AFileName);
 // Установка нормальных атрибутов файла
 SetFileAttributes(PChar(AFileName), FILE_ATTRIBUTE_ARCHIVE);
 // Деинсталляция (DllUnregisterServer)
 if AUnregDLL then
  UnregDLLRes := MainAntivirus.UnregisterLibrary(AFileName);
 // Занесение в папку Infected
 if AutoInfected then
  CopyToInfected(AFileName, AVirus, 'Infected'); //#DNL
 Result := AVZDeleteFile(AFileName);
 // Выполнение скрипта лечения
 if AScript > 0 then begin
  S := MainAntivirus.GetCureScript(AScript);
  AvzUserScript.ExecuteScript(AFileName, S);
 end;
end;

// Проверка указанной DLL на предмет наличия в ней кейлоггеров
function CheckDLLforKeyLogger(ModuleInfo: TModuleInfo) : boolean;
var
 Buf           : array[0..1000] of byte;
 dwBytesReaded : DWORD;
 DLLPath       : string;
 hLib          : THandle;
 // Добавление кейлоггера
 procedure AddKeyloggerEv(AFileName, AMsg : string);
 var
  S, FileInfoStr : string;
  Lines : TStrings;
  i : integer;
 begin
  // Проверка по базе безопасных
  if FileSignCheck.CheckFile(NTFileNameToNormalName(AFileName)) = 0 then exit;
  inc(FKeyloggerAlarmCount);
  AddToLog(AFileName + ' --> '+AMsg, logAlert);
  S := GetModuleTraceResult(ModuleInfo.hModule, ModuleInfo.ModuleSize);
  if S <> '' then begin
   Lines := TStringList.Create;
   Lines.Text := S;
   AddToLog(AFileName+ '>>> $AVZ0720: ', logAlert);
   for i := 0 to Lines.Count - 1 do
    AddToLog('  '+inttostr(i+1)+'. '+Lines[i], logAlert);
   Lines.Free;
   // Анализ нейросетью
   NeuroKeyloggerCheck(AFileName);
  end else begin
   AddToLog(AFileName+ '>>> $AVZ0720: ', logAlert);
   AddToLog(' $AVZ1063', logInfo);
  end;
  // Список подозрений
  VirFileList.AddVir(AFileName, AMsg, vrtKeylogger);
  // Добавление в XML базу
  FileInfoStr := FormatXMLFileInfo(AFileName, true);
  XMLLogger.AddItem('KEYLOGGER', '<ITEM File="'+AFileName+'" '+
                              ' Verdict="'+S+'" '+
                              FileInfoStr+
                              '/>');
  // Автоматический карантин
  if AutoQuarantine then
   CopyToInfected(ModuleInfo.ModuleName, '$AVZ0726'+', A='+S, 'Quarantine');
 end;
begin
 Result := false;
 // Это наш собственный процесс
 if UpperCase(ModuleInfo.ModuleName) = UpperCase(ParamStr(0)) then begin
  Result := true;
  exit;
 end;
 hLib := LoadLibrary(PChar(ModuleInfo.ModuleName));
 FreeLibrary(hLib);
 // Опасно - подделка имени DLL
 if hLib <> ModuleInfo.hModule then begin
  AddKeyloggerEv(ModuleInfo.ModuleName, '$AVZ0737');
  exit;
 end;
 // Сравнение имени DLL с известным именем
 if KeyloggerBase.SysDLL.IndexOf(ExtractFileName(ModuleInfo.ModuleName)) >= 0 then begin
  // Читаем ее память
  ReadProcessMemory(GetCurrentProcess, pointer(ModuleInfo.hModule), @Buf, SizeOf(Buf), dwBytesReaded);
  DLLPath := '';
  // Поиск в папке System
  if FileExists(GetSystemDirectoryPath+ExtractFileName(ModuleInfo.ModuleName)) then
   DLLPath := GetSystemDirectoryPath;
  // Поиск в папке Windows
  if FileExists(GetWindowsDirectoryPath+ExtractFileName(ModuleInfo.ModuleName)) then
   DLLPath := GetWindowsDirectoryPath;
  // Поиск в папке с конфликными версиями
  if pos(AnsiLowerCase(NormalDir(GetWindowsDirectoryPath)+'WinSxS\x86_Microsoft'), AnsiLowerCase(ModuleInfo.ModuleName)) = 1 then //#DNL
   DLLPath := ExtractFilePath(ModuleInfo.ModuleName);
  if DLLPath = '' then begin
   AddKeyloggerEv(ModuleInfo.ModuleName, '$AVZ0728');
   Result := true;
   exit;
  end;
  if not(AnsiUpperCase(DLLPath + ExtractFileName(ModuleInfo.ModuleName)) = AnsiUpperCase(ModuleInfo.ModuleName)) then begin
   AddKeyloggerEv(ModuleInfo.ModuleName, '$AVZ0727');
   Result := true;
  end;
 end else begin
  AddKeyloggerEv(ModuleInfo.ModuleName, '$AVZ0726');
  Result := true;
 end;
end;

function SearchKeyloggers: boolean;
var
 SysProcessInfo : TSysProcessInfo;
 Lines, NewList : TStringList;
 i,j            : integer;
 FoundFl        : boolean;
begin
 Result := false;
 // Очистка старых данных
 XMLLogger.ClearByName('KEYLOGGER');
 // Загрузка базы
 if not(KeyloggerBase.LoadBinDatabase) then begin
  exit;
 end;
 // Создание анализатора списка модулей
 if ISNT then
  SysProcessInfo := TPSAPIProcessInfo.Create
   else SysProcessInfo := TToolHelpProcessInfo.Create;
 Lines   := TStringList.Create;
 NewList := TStringList.Create;
 // Первичное пострение списка модулей
 SysProcessInfo.CreateModuleList(Lines, GetCurrentProcessId);
 // "Дразнение" потенциальных кейлоггеров и анализ результатов
 KeyloggerHunter;
 // Повторное построение списка модулей
 SysProcessInfo.CreateModuleList(NewList, GetCurrentProcessId);
 // Анализ первичного списка модулей - все постороннее на подозрение
 for i := 1 to Lines.Count - 1 do begin
  CheckDLLforKeyLogger(TModuleInfo(Lines.Objects[i]));
 end;
 // Сравнение списков
 if NewList.Count <> Lines.Count then
 for i := 0 to NewList.Count - 1 do begin
  FoundFl := false;
  for j := 0 to Lines.Count - 1 do
   if TModuleInfo(NewList.Objects[i]).ModuleName = TModuleInfo(Lines.Objects[j]).ModuleName then begin
    FoundFl := true;
    Break;
   end;
  // Эта DLL не имеет системного имени - значит, это точно троян или нечно "левое"
  if not(FoundFl) then
   CheckDLLforKeyLogger(TModuleInfo(NewList.Objects[i]));
 end;
 SysProcessInfo.ClearList(Lines);
 SysProcessInfo.ClearList(NewList);
 SysProcessInfo.Free;
 if FKeyloggerAlarmCount > 0 then
  AddToLog('$AVZ0449', logInfo);
 Result := true; 
end;

// Расширенная чистка системы
function ExtSystemClean(ADeletedVirusList : TStrings): boolean;
var
 Reg : TRegistry;
 CLSIDList, VirusCLSIDList : TStringList;
 i : integer;
 AutorunManager : TAutorunManager;
 ServiceManager : TServiceManager;
 LSPManager     : TLSPManager;
 // Добавление CLSID списку, если он ссылается на файла
 procedure CheckCLSID(ACLSID, AFileName : string);
 var
  i : integer;
 begin
  if (AFileName = '') then exit;
  if (AFileName<>'') and (AFileName[1] = '"') then Delete(AFileName, 1, 1);
  AFileName    := AnsiLowerCase(NTFileNameToNormalName(Trim(AFileName)));
  // Поиск и добавление
  for i := 0 to ADeletedVirusList.Count - 1 do
   if (AFileName = ADeletedVirusList[i]) then begin
    VirusCLSIDList.Add(ACLSID);
    AddToDebugLog(ACLSID + ' = ' + AFileName);
   end;
 end;
begin
 if ADeletedVirusList.Count = 0 then exit;
 AddToLog('$AVZ0041', logInfo);
 Reg := nil;
 CLSIDList := nil;
 VirusCLSIDList := nil;
 try
  Reg := TRegistry.Create;
  CLSIDList := TStringList.Create;
  VirusCLSIDList := TStringList.Create;
  // Получение списка CLSID
  Reg.RootKey := HKEY_CLASSES_ROOT;
  Reg.OpenKey('CLSID', false);
  Reg.GetKeyNames(CLSIDList);
  Reg.CloseKey;
  // Цикл анализа CLSID
  for i := 0 to CLSIDList.Count - 1 do begin
   if Reg.OpenKey('CLSID\'+CLSIDList[i]+'\InprocServer32', false) then begin //#DNL
    if Reg.ValueExists('') then
     CheckCLSID(CLSIDList[i], Reg.ReadString(''));
    Reg.CloseKey;
   end;
   if Reg.OpenKey('CLSID\'+CLSIDList[i]+'\LocalServer32', false) then begin //#DNL
    if Reg.ValueExists('') then
     CheckCLSID(CLSIDList[i], Reg.ReadString(''));
    Reg.CloseKey;
   end;
   if Reg.OpenKey('CLSID\'+CLSIDList[i]+'\LocalServer', false) then begin  //#DNL
    if Reg.ValueExists('') then
     CheckCLSID(CLSIDList[i], Reg.ReadString(''));
    Reg.CloseKey;
   end;
  end;
  // Цикл удаления CLSID
  for i := 0 to VirusCLSIDList.Count - 1 do
   AvzUserScript.DelCLSID(copy(VirusCLSIDList[i], 2, length(VirusCLSIDList[i])-2));
  // Чистка автозапуска
  AutorunManager := nil;
  try
   AutorunManager := TAutorunManager.Create;
   AutorunManager.Refresh;
   for i := 0 to ADeletedVirusList.Count - 1 do begin
    // Удаление элементов автозапуска
    AutorunManager.DeleteFromAutorun(ADeletedVirusList[i]);
    AutorunManager.DeleteFromAutorun(NTFileNameToNormalName(ADeletedVirusList[i]));
    // Чистка расширений WinLogon
    DelWinlogonNotifyByFileName(ADeletedVirusList[i]);
   end;
   AutorunManager.Free;
   // Чистка сервисов и драйверов
   if ISNT then begin
    ServiceManager := TServiceManager.Create;
    ServiceManager.RefreshServicesByAPI(2,2);
    for i := 0 to ADeletedVirusList.Count - 1 do begin
     ServiceManager.DeleteServiceByFile(ADeletedVirusList[i]);
    end;
    ServiceManager.Free;
   end;
   // Чистка LSP
   LSPManager := TLSPManager.Create;
   LSPManager.Refresh;
   for i := 0 to ADeletedVirusList.Count - 1 do begin
    LSPManager.DeleteNameSpaceByBinaryName(ADeletedVirusList[i], false);
    LSPManager.DeleteProtocolByBinaryName(ADeletedVirusList[i], false);
   end;
  except
  end;
 finally
  Reg.Free;
  CLSIDList.Free;
  VirusCLSIDList.Free;
 end;
end;

procedure DelWinlogonNotifyByFileName(AFileName: string);
var
 Reg   : TRegistry;
 Lines : TStringList;
 i     : integer;
 S     : string;
begin
 Reg   := TRegistry.Create;
 Lines := TStringList.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.OpenKey('SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Notify', false) then begin
  Reg.GetKeyNames(Lines);
  Reg.CloseKey;
  // Цикл поиска ключа
  for i := 0 to Lines.Count - 1 do
   if Reg.OpenKey('SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Notify\'+Lines[i], false) then begin
    if Reg.ValueExists('DllName') then begin
     S :=  LowerCase(Trim(NTFileNameToNormalName(LowerCase(Trim(Reg.ReadString('DllName'))))));
     if ExtractFilePath(S) = '' then begin
      if FileExists(GetSystemDirectoryPath+S) then S := GetSystemDirectoryPath + S;
     end;
     if S = LowerCase(AFileName) then begin
      Reg.DeleteKey('SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Notify\'+Lines[i]);
      AddToLog('$AVZ1072 '+AFileName, logInfo);
     end;
    end;
    try Reg.CloseKey; except end;
   end;
 end;
 Reg.Free;
 Lines.Free;
end;

procedure DelWinlogonNotifyByKeyName(AKeyName: string);
var
 Reg   : TRegistry;
begin
 Reg   := TRegistry.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.DeleteKey('SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Notify\'+AKeyName) then begin
  AddToLog('$AVZ1071 '+AKeyName, logInfo);
 end;
 Reg.Free;
end;

function BHOExists(ACLSID: string): boolean;
var
 Reg   : TRegistry;
 S     : string;
begin
 Result := false;
 if Trim(ACLSID) = '' then exit;
 Reg   := TRegistry.Create;
 try
   Reg.RootKey := HKEY_LOCAL_MACHINE;
   S := '{'+Trim(ACLSID)+'}';
   if Reg.KeyExists('SOFTWARE\Microsoft\Internet Explorer\Explorer Bars\'+S) then begin
    Result := true;
    exit;
   end;
   if Reg.KeyExists('SOFTWARE\Microsoft\Internet Explorer\Extensions'+S) then begin
    Result := true;
    exit;
   end;
   if Reg.OpenKey('SOFTWARE\Microsoft\Internet Explorer\ToolBar', false) then begin
    if Reg.ValueExists(S) then
     Result := true;
    Reg.CloseKey;
   end;
   Reg.RootKey := HKEY_CURRENT_USER;
   S := '{'+Trim(ACLSID)+'}';
   if Reg.KeyExists('SOFTWARE\Microsoft\Internet Explorer\Explorer Bars\'+S) then begin
    Result := true;
    exit;
   end;
   if Reg.KeyExists('SOFTWARE\Microsoft\Internet Explorer\Extensions'+S) then begin
    Result := true;
    exit;
   end;
   if Reg.OpenKey('SOFTWARE\Microsoft\Internet Explorer\ToolBar', false) then begin
    if Reg.ValueExists(S) then
     Result := true;
    Reg.CloseKey;
   end;
 finally
  Reg.Free;
 end;
end;

function CLSIDExists(ACLSID: string): boolean;
var
 Reg   : TRegistry;
begin
 try
  Reg   := TRegistry.Create;
  ACLSID := Trim(ACLSID);
  if ACLSID = '' then exit;
  Reg.RootKey := HKEY_CLASSES_ROOT;
  Result := Reg.KeyExists('CLSID\{'+ACLSID+'}');
 finally
  Reg.Free;
 end;
end;

function CLSIDFileName(ACLSID: string; ARecurLevel : integer = 1): string;
var
 Reg   : TRegistry;
begin
 Result := '';
 // Антизацикливание при рекурсивном поиске
 if ARecurLevel > 10 then exit;
 if Not(CLSIDExists(ACLSID)) then exit;
 try
  Reg   := TRegistry.Create;
   ACLSID := 'CLSID\{'+Trim(ACLSID)+'}';
   Reg.RootKey := HKEY_CLASSES_ROOT;
   // Поиск в ключе InprocServer32
   if Reg.OpenKey(ACLSID+'\InprocServer32', false) then begin
    if Reg.ValueExists('') then
     Result := Reg.ReadString('');
    Reg.CloseKey;
    if Result <> '' then exit;
   end;
   // Поиск в ключе LocalServer32
   if Reg.OpenKey(ACLSID+'\LocalServer32', false) then begin
    if Reg.ValueExists('') then
     Result := Reg.ReadString('');
    Reg.CloseKey;
    if Result <> '' then exit;
   end;
   // Поиск в ключе LocalServer
   if Reg.OpenKey(ACLSID+'\LocalServer', false) then begin
    if Reg.ValueExists('') then
     Result := Reg.ReadString('');
    Reg.CloseKey;
    if Result <> '' then exit;
   end;
   // Итак, не нашли ничего - может, это ссылка ??
   if Reg.OpenKey(ACLSID+'\TreatAs', false) then begin
    if Reg.ValueExists('') then
     Result := Reg.ReadString('');
    Reg.CloseKey;
    if Result <> '' then begin
     Result := StringReplace(Result, '{', '', []);
     Result := Trim(StringReplace(Result, '}', '', []));
     Result := CLSIDFileName(Result, ARecurLevel+1);
    end;
   end;
 finally
  Reg.Free
 end;
end;

function EvSysFileCheckByCLSID(ACLSID, AVirusName,
  AFileMask: string): boolean;
var
 FileName : string;
begin
 try
  Result := false;
  // Поиск файла по CLSID класса
  FileName := CLSIDFileName(ACLSID);
  // Файл не найден
  if FileName = '' then exit;
  FileName := NTFileNameToNormalName(FileName);
  Result :=  EvSysFileCheckByName(FileName, AVirusName, AFileMask);
 except
 end;
end;

function EvSysFileCheckByName(AFileName, AVirusName,
  AFileMask: string): boolean;
var
 ST, FileName : string;
 MaskList : TStringList;
begin
 try
  Result := false;
  FileName := NTFileNameToNormalName( AFileName );
  // Файл не найден
  if FileName = '' then exit;
  // Файл найден ? Если нет, то выход
  if not(FileExists(FileName)) then exit;
  // Файл в базе правильных ? Если да, то перекращаем проверку
  if FileSignCheck.CheckFile(FileName) = 0 then exit;
  // Файл опознан как вирус ? Если да, то перекращаем проверку
  //if VirusScan(AFileMask, cbDeleteVirus.Checked, nil, nil) > 0 then exit;
  // Итак, файл не известен как системный и как вирус
  ST := '';
  if AFileMask <> '' then begin
   AFileMask := StringReplace(AFileMask, ' ', '', [rfReplaceAll]);
   MaskList := TStringList.Create;
   MaskList.CommaText := AFileMask;
   if MatchesMask(ExtractFileName(FileName), MaskList) then
    ST := ' ($AVZ0194)';
   MaskList.Free;
  end;
  // Вывод сообщения
  St := ' $AVZ1155 '+AVirusName + ST;
  AddToLog('>>> '+FileName+ST, logAlert);
  // Карантинные действия
  VirFileList.AddVir(FileName, ST, vrtVirusEv, 0);
  if AutoQuarantine then
   CopyToInfected(FileName, ST, 'Quarantine');
  Result := true;
 except end;
end;

function EvSysCheckIPU(AGUILink : TGUILink ): boolean;
var
 i : integer;
 S, ScriptName : string;
 AvzUserScript : TAvzUserScript;
begin
 ExtMenuScripts := nil;
 // Создание экземпляра скриптдвидка
 AvzUserScript := TAvzUserScript.Create;
 // Загрузка базы IPU
 Result := SystemIPU.LoadBinDatabase;
 try
   AvzUserScript.QurantinePath := GetQuarantineDirName('Quarantine');//#DNL
   AvzUserScript.GUILink   := AGUILink;
   Result := false;
   try
    AvzUserScript.CallGUILink(guiSet, 'MAIN-PROGRESS-POS', '0');
    AvzUserScript.CallGUILink(guiSet, 'MAIN-PROGRESS-MAX', IntToStr(length(SystemIPU.CheckScripts)));
    ZProgressClass.AddProgressMax(length(SystemIPU.CheckScripts));
    for i := 0 to length(SystemIPU.CheckScripts)-1 do begin
     ZProgressClass.ProgressStep;
     // Прерывание операции
     if StopF then exit;
     if ScanPauseF then begin
      AvzUserScript.CallGUILink(guiSet, 'MAIN-STATUSBAR', '$AVZ0701');
      while ScanPauseF do begin
       Application.HandleMessage;
       if StopF then exit;
      end;
      AvzUserScript.CallGUILink(guiSet, 'MAIN-STATUSBAR', '$AVZ0965');
     end;
     // Задержка (применяется на очень медленных ПК для снижения нагрузки)
     if SleepScanTime > 0 then Sleep(SleepScanTime);
     AvzUserScript.CallGUILink(guiSet, 'MAIN-PROGRESS-POS', IntToStr(i));
     // Распаковка имени
     ScriptName := DeCompressString(SystemIPU.CheckScripts[i].CompressedName);
     AvzUserScript.CallGUILink(guiSet, 'MAIN-STATUSBAR', '$AVZ0747 '+ScriptName);
     // Распаковка строки
     S := DeCompressString(SystemIPU.CheckScripts[i].CompressedText);
     // Выполнение скрипта
     if not(AvzUserScript.ExecuteScript('', S, ScriptName)) then
      AddToLog('$AVZ0664 '+IntToStr(i), logError);
     ZeroMemory(@S[1], length(s)); S := '';
     // Отдаем квант времени
     if i mod 5 = 0 then Application.ProcessMessages;
    end;
   except end;
 finally
  // Разрушаем экземпляр скрипт-движка для чистки памяти
  AvzUserScript.Free;
  AvzUserScript := nil;
 end;
end;

function EvSysCheck(AGUILink : TGUILink): boolean;
var
 i : integer;
 S, ScriptName : string;
 AvzUserScript : TAvzUserScript;
begin
 // Создание экземпляра скриптдвидка
 AvzUserScript := TAvzUserScript.Create;
 // Загрузка базы SysCheck
 Result := SystemCheck.LoadBinDatabase;
 try
   AvzUserScript.QurantinePath := GetQuarantineDirName('Quarantine');//#DNL
   AvzUserScript.GUILink   := AGUILink;
   Result := false;
   try
    // Подключение к прогресс-индикации
    AvzUserScript.CallGUILink(guiSet, 'MAIN-PROGRESS-POS', '0');
    AvzUserScript.CallGUILink(guiSet, 'MAIN-PROGRESS-MAX', IntToStr(length(SystemCheck.CheckScripts)));
    ZProgressClass.AddProgressMax(length(SystemCheck.CheckScripts));
    // Цикл по скриптам
    for i := 0 to length(SystemCheck.CheckScripts)-1 do begin
     ZProgressClass.ProgressStep;
     // Прерывание операции
     if StopF then exit;
     if ScanPauseF then begin
      AvzUserScript.CallGUILink(guiSet, 'MAIN-STATUSBAR', '$AVZ0701');
      while ScanPauseF do begin
       Application.HandleMessage;
       if StopF then exit;
      end;
      AvzUserScript.CallGUILink(guiSet, 'MAIN-STATUSBAR', '$AVZ0965');
     end;
     // Задержка (применяется на очень медленных ПК для снижения нагрузки)
     if SleepScanTime > 0 then Sleep(SleepScanTime);
     AvzUserScript.CallGUILink(guiSet, 'MAIN-PROGRESS-POS', IntToStr(i));
     // Распаковка имени
     ScriptName := DeCompressString(SystemCheck.CheckScripts[i].CompressedName);
     AvzUserScript.CallGUILink(guiSet, 'MAIN-STATUSBAR', '$AVZ0747 '+ScriptName);
     // Распаковка строки с текстом скрипта
     S := DeCompressString(SystemCheck.CheckScripts[i].CompressedText);
     // Выполнение скрипта
     if not(AvzUserScript.ExecuteScript('', S, ScriptName)) then
      AddToLog('$AVZ0664 '+IntToStr(i), logError);
     ZeroMemory(@S[1], length(s)); S := '';
     // Отдаем квант времени GUI
     if i mod 5 = 0 then Application.ProcessMessages;
    end;
    AvzUserScript.CallGUILink(guiSet, 'MAIN-STATUSBAR', '');
   except end;
 finally
  // Разрушаем экземпляр скрипт-движка для чистки памяти
  AvzUserScript.Free;
  AvzUserScript := nil;
 end;
end;

// Запуск визарда в редиме Check/Fix
function RunWizardScripts(AGUILink : TGUILink; AWizardDB : string; ALogLevel, AFixLevel : integer; AUseBackUp : boolean; AScriptID : integer = -1) : integer;
var
 FixedFl, i, Detectedlevel, FixCounter : integer;
 S, BackupDBFileName : string;
 AvzUserScript : TAvzUserScript;
 CustomScripts : TCustomScripts;
 BackupEngine  : TZBackupEngine;
 MS : TMemoryStream;
 XMLTagName : string;
begin
 Result := -1;
 XMLTagName := UpperCase(Trim('WIZARD-' + ChangeFileExt(ExtractFileName(AWizardDB), '')));
 // Очистка старых данных
 XMLLogger.ClearByName(XMLTagName);
 if not(ALogLevel in [1..3]) then exit;
 if AFixLevel < 0 then AFixLevel := MaxInt;
 AWizardDB := ChangeFileExt(AWizardDB, '.avz');
 // Подготовка пути к файлу карантина и создание папки
 BackupDBFileName := GetQuarantineDirName('Backup', true);
 BackupDBFileName := NormalDir(BackupDBFileName) + ChangeFileExt(AWizardDB, '.zbk');
 // Сброс счетчика пофиксенных проблем
 FixCounter := 0;
 BackupEngine  := nil;
 // Создание экземпляра скриптдвидка
 AvzUserScript := TAvzUserScript.Create;
 // Загрузка базы CustomScripts, содержащей скрипты данного визарда
 CustomScripts := TCustomScripts.Create;
 // Создание потока для работы с базами
 MS := TMemoryStream.Create;
 // Если бекап поддерживается, то загрузка базы бекапа
 if AUseBackUp then begin
  zForceDirectories(ExtractFilePath(BackupDBFileName));
  // Создали движок бекапа
  BackupEngine  := TZBackupEngine.Create;
  try
    // Пробуем загрузить файл
    MS.LoadFromFile(BackupDBFileName);
    // Файл загрузился - анализ и загрузка его в класс бекапа
    BackupEngine.LoadBackupDB(MS);
  except
  end;
 end;
 try
   // Проверка, загружена ли база
   if not(CustomScripts.LoadBinDatabase(AVPath + AWizardDB)) then exit;
   Result := 0;
   // Настройка скрипт-движка
   AvzUserScript.QurantinePath := GetQuarantineDirName('Quarantine');//#DNL
   AvzUserScript.GUILink   := AGUILink;
   try
    // Подключение к прогресс-индикации
    AvzUserScript.CallGUILink(guiSet, 'MAIN-PROGRESS-POS', '0');
    AvzUserScript.CallGUILink(guiSet, 'MAIN-PROGRESS-MAX', IntToStr(length(CustomScripts.Scripts)));
    ZProgressClass.AddProgressMax(length(CustomScripts.Scripts));
    // Цикл по скриптам
    for i := 0 to length(CustomScripts.Scripts)-1 do
     if (CustomScripts.Scripts[i].ID = AScriptID) or (AScriptID < 0) then begin
     ZProgressClass.ProgressStep;
     // Прерывание операции
     if StopF then exit;
     AvzUserScript.CallGUILink(guiSet, 'MAIN-PROGRESS-POS', inttostr(I));
     // Извлечение из базы скрипта, его декомпрессия и запуск
     S := Trim(DeCompressString(CustomScripts.Scripts[i].CompressedText));
     S := S + #$0D#$0A+' begin if AFilename = ''FIX'' then OutRes := IntToStr(Fix) else OutRes := IntToStr(Check); end.';
     if i mod 5 = 0 then
      AvzUserScript.CallGUILink(guiSet, 'MAIN-STATUSBAR', '$AVZ0747 '+TranslateStr(DeCompressString(CustomScripts.Scripts[i].CompressedName)));
     AvzUserScript.LogEnabled := false;
     // Подключили движок бекапа
     AvzUserScript.BackupEngine := BackupEngine;
     try
       // запуск скрипта
       if AvzUserScript.ExecuteScript('CHECK', S) then begin
        //////AddToDebugLog(AWizardDB + ' = '+IntToStr(CustomScripts.Scripts[i].ID)+ ' = '+AvzUserScript.OutRes);
        Detectedlevel := StrToIntDef(AvzUserScript.OutRes, 0);
        // Проблема детектирована ?? Если да, то добавим ее к списку
        if (Detectedlevel > 0) and (Detectedlevel >= ALogLevel) then begin
         FixedFl := 0;
         // Счетчик найденных объектов
         inc(Result);
         AddToLog(' >> '+TranslateStr(DeCompressString(CustomScripts.Scripts[i].CompressedName)), logAlert);
         // Автоматический фикс
         if (Detectedlevel >= AFixLevel) then begin
          if AvzUserScript.ExecuteScript('FIX', S) then begin
           inc(FixCounter);
           FixedFl := 1;
           AddToLog(' >>> '+TranslateStr(DeCompressString(CustomScripts.Scripts[i].CompressedName)) + ' - $AVZ1600', logAlert);
          end;
         end;
         XMLLogger.AddItem(XMLTagName, '<ITEM ID="'+
                           IntToStr(CustomScripts.Scripts[i].ID)+'" Level="'+AvzUserScript.OutRes+'" Fixed="'+IntToStr(FixedFl)+'" />');
        end;
       end;
     except
      on E : Exception do
       AddToDebugLog('Error, script id = '+IntToStr(CustomScripts.Scripts[i].ID));
     end;
     // Чистка памяти
     if S <> '' then ZeroMemory(@S[1], length(S));
     S := '';
     // Задержка (применяется на очень медленных ПК для снижения нагрузки)
     if SleepScanTime > 0 then Sleep(SleepScanTime);
     AvzUserScript.CallGUILink(guiSet, 'MAIN-PROGRESS-POS', IntToStr(i));
     // Отдаем квант времени GUI
     if i mod 5 = 0 then Application.ProcessMessages;
    end;
   except end;
   // Почистили статус-бар
   AvzUserScript.CallGUILink(guiSet, 'MAIN-STATUSBAR', '');
   // Процесс завершен, смотрим, есть ли несохраненный бекап
   if AUseBackUp and (FixCounter > 0) then begin
    MS.Clear;
    MS.Size := 0;
    BackupEngine.SaveBackupDB(MS);
    try MS.SaveToFile(BackupDBFileName); except end;
   end;
 finally
  // Разрушение потока для работы с базами
  MS.Clear;
  MS.Free;
  MS := nil;
  // Разрушаем экземпляр скрипт-движка для чистки памяти
  AvzUserScript.Free;
  AvzUserScript := nil;
  // Освобожение базы бекапа
  BackupEngine.Free;
  BackupEngine := nil;
  // Освобожение базы скриптов
  CustomScripts.Free;
  CustomScripts := nil;
 end;
end;

begin
 StopF := false;
end.

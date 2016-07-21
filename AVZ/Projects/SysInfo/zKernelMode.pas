unit zKernelMode;

interface
uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls,
  ntdll,  zPELoader;
 // Получение имени файла ядра
 function GetKernelFileName : string;
 // Получение базового адреса ядра
 function GetKernelBase : DWORD;
 // Получение размера ядра в памяти
 function GetKernelSize : DWORD;
 // Получение имения драйвера ядра по адресу
 function GetKernelModuleByAddr(Addr : DWORD) : string;
 // Получение списка драверов ядра
 function GetKernelModuleList(AList : TStrings) : boolean;
 // Получение списка функций ядра
 function GetKernelFunctionList(AList : TStrings) : boolean;
 function GetKernelGUIFunctionList(AList : TStrings) : boolean;
 // Получение имени функций ядра
 function GetKernelFunctionName(AList : TStrings; ID : dword; DecodeZwNt : boolean = false) : string;
implementation
uses ZUtil;

// Получение имени файла ядра
function GetKernelFileName : string;
var
  R: NTSTATUS;
  NTDLL_DLL : THandle;
  BufSize, ResBufSize, lpNumberOfBytesWritten: DWORD;
  Buf : Pointer;
  S : string;
  i,  NumEntries : integer;
  ZwQuerySystemInformation: TNativeQuerySystemInformation;
begin
 // Адрес ядра по умолчанию
 Result := '';
 // Загрузка библиотеки и поиск функции
 NTDLL_DLL := GetModuleHandle(NTDLL_DLL_name);
 // Поиск функции
 if NTDLL_DLL <> 0 then @ZwQuerySystemInformation := GetProcAddress(NTDLL_DLL,'ZwQuerySystemInformation')
  else exit;
 if @ZwQuerySystemInformation = nil then exit;
 try
   Buf := nil;
   try
     // Вывод в протокол адреса функции
     BufSize := 200000;
     repeat
      GetMem(Buf, BufSize);
      // Получение списка процессов
      R := ZwQuerySystemInformation(SystemModuleInformation, Buf, BufSize, @ResBufSize);
      if R = STATUS_INFO_LENGTH_MISMATCH then begin
       FreeMem(Buf, BufSize);
       BufSize := BufSize * 2;
      end;
     until R <> STATUS_INFO_LENGTH_MISMATCH;
     // Ошибка ?? Тогда выход
     if R <> STATUS_SUCCESS then exit;
     NumEntries := dword(Buf^);
     if NumEntries > 0 then
       Result := Trim(TSystemModuleInformation(Ptr(dword(Buf) + 4)^).ImageName);
   finally
    // Освобождение памяти
    if Buf <> nil then
     FreeMem(Buf, BufSize);
   end;
 except
  Result := '';
 end;
end;

// Получение базового адреса ядра
function GetKernelBase : DWORD;
var
  R: NTSTATUS;
  NTDLL_DLL : THandle;
  BufSize, ResBufSize, lpNumberOfBytesWritten: DWORD;
  Buf : Pointer;
  S : string;
  i,  NumEntries : integer;
  ZwQuerySystemInformation: TNativeQuerySystemInformation;
begin
 // Адрес ядра по умолчанию
 Result := 0;
 // Загрузка библиотеки и поиск функции
 NTDLL_DLL := LoadLibrary(NTDLL_DLL_name);
 // Поиск функции
 if NTDLL_DLL <> 0 then @ZwQuerySystemInformation := GetProcAddress(NTDLL_DLL,'ZwQuerySystemInformation')
  else exit;
 if @ZwQuerySystemInformation = nil then exit;
 try
   // Вывод в протокол адреса функции
   BufSize := 200000;
   GetMem(Buf, BufSize);
   // Получение списка процессов
   R := ZwQuerySystemInformation(SystemModuleInformation, Buf, BufSize, @ResBufSize);
   if R <> STATUS_SUCCESS then exit;
   NumEntries := dword(Buf^);
   if NumEntries > 0 then
     Result := TSystemModuleInformation(Ptr(dword(Buf) + 4)^).Base;
   // Освобождение памяти
   FreeMem(Buf, BufSize);
   FreeLibrary(NTDLL_DLL);
 except
  Result := 0;
 end;
end;

function GetKernelSize : DWORD;
var
  R: NTSTATUS;
  NTDLL_DLL : THandle;
  BufSize, ResBufSize, lpNumberOfBytesWritten: DWORD;
  Buf : Pointer;
  S : string;
  i,  NumEntries : integer;
  ZwQuerySystemInformation: TNativeQuerySystemInformation;
begin
 // Адрес ядра по умолчанию
 Result := 0;
 // Загрузка библиотеки и поиск функции
 NTDLL_DLL := LoadLibrary(NTDLL_DLL_name);
 // Поиск функции
 if NTDLL_DLL <> 0 then @ZwQuerySystemInformation := GetProcAddress(NTDLL_DLL,'ZwQuerySystemInformation')
  else exit;
 if @ZwQuerySystemInformation = nil then exit;
 try
   // Вывод в протокол адреса функции
   BufSize := 200000;
   GetMem(Buf, BufSize);
   // Получение списка процессов
   R := ZwQuerySystemInformation(SystemModuleInformation, Buf, BufSize, @ResBufSize);
   if R <> STATUS_SUCCESS then exit;
   NumEntries := dword(Buf^);
   if NumEntries > 0 then
     Result := TSystemModuleInformation(Ptr(dword(Buf) + 4)^).Size;
   // Освобождение памяти
   FreeMem(Buf, BufSize);
   FreeLibrary(NTDLL_DLL);
 except
  Result := 0;
 end;
end;

// Получение имения драйвера ядра по адресу
function GetKernelModuleByAddr(Addr : DWORD) : string;
var
  R: NTSTATUS;
  NTDLL_DLL : THandle;
  BufSize, ResBufSize, lpNumberOfBytesWritten: DWORD;
  Buf : Pointer;
  S : string;
  i,  NumEntries : integer;
  ZwQuerySystemInformation: TNativeQuerySystemInformation;
begin
 Result := '';
 // Загрузка библиотеки и поиск функции
 NTDLL_DLL := LoadLibrary(NTDLL_DLL_name);
 // Поиск функции
 if NTDLL_DLL <> 0 then @ZwQuerySystemInformation := GetProcAddress(NTDLL_DLL,'ZwQuerySystemInformation')
  else exit;
 if @ZwQuerySystemInformation = nil then exit;
 // Вывод в протокол адреса функции
 BufSize := 500000;
 GetMem(Buf, BufSize);
 try
   // Получение списка процессов
   R := ZwQuerySystemInformation(SystemModuleInformation, Buf, BufSize, @ResBufSize);
   if R <> STATUS_SUCCESS then exit;
   NumEntries := dword(Buf^);
   // Перемещение по списку и анализ
   for i := 0 to NumEntries-2 do
    if (TSystemModuleInformation(Ptr(dword(Buf) + i*sizeof(TSystemModuleInformation) + 4)^).Base < Addr) and
       (TSystemModuleInformation(Ptr(dword(Buf) + i*sizeof(TSystemModuleInformation) + 4)^).Base+
        TSystemModuleInformation(Ptr(dword(Buf) + i*sizeof(TSystemModuleInformation) + 4)^).Size > Addr) then begin
         Result := TSystemModuleInformation(Ptr(dword(Buf) + i*sizeof(TSystemModuleInformation) + 4)^).ImageName;
         break;
        end;
 finally
  // Освобождение памяти
  FreeMem(Buf, BufSize);
  FreeLibrary(NTDLL_DLL);
 end;
end;

// Получение списка драверов ядра
function GetKernelModuleList(AList : TStrings) : boolean;
var
  R: NTSTATUS;
  NTDLL_DLL : THandle;
  BufSize, ResBufSize, lpNumberOfBytesWritten: DWORD;
  Buf : Pointer;
  S : string;
  i,  NumEntries : integer;
  ZwQuerySystemInformation: TNativeQuerySystemInformation;
begin
 Result := false;
 // Загрузка библиотеки и поиск функции
 NTDLL_DLL := LoadLibrary(NTDLL_DLL_name);
 // Поиск функции
 if NTDLL_DLL <> 0 then @ZwQuerySystemInformation := GetProcAddress(NTDLL_DLL,'ZwQuerySystemInformation')
  else exit;
 if @ZwQuerySystemInformation = nil then exit;
 // Вывод в протокол адреса функции
 BufSize := 500000;
 try
   try
     GetMem(Buf, BufSize);
     // Получение списка процессов
     R := ZwQuerySystemInformation(SystemModuleInformation, Buf, BufSize, @ResBufSize);
     if R <> STATUS_SUCCESS then exit;
     NumEntries := dword(Buf^);
     // Перемещение по списку и анализ
     for i := 0 to NumEntries-2 do
      AList.AddObject(TSystemModuleInformation(Ptr(dword(Buf) + i*sizeof(TSystemModuleInformation) + 4)^).ImageName,
                      pointer(TSystemModuleInformation(Ptr(dword(Buf) + i*sizeof(TSystemModuleInformation) + 4)^).Base));
     Result := true;
   finally
    // Освобождение памяти
    FreeMem(Buf, BufSize);
    FreeLibrary(NTDLL_DLL);
   end;
 except
  Result := false;
 end;
end;

// Получение списка функций ядра
function GetKernelFunctionList(AList : TStrings) : boolean;
var
 PEImageLoader   :  TPEImageLoader; // Класс - загрузчик PE
 ExportTable     :  TStringList;
 i               :  integer;
 PhPtr, LastId   :  DWORD;
 procedure AddKernelFunction(AFunctionName : string; ID : DWord);
 begin
  if AList.IndexOfObject(pointer(ID)) < 0 then
   AList.AddObject(AFunctionName, pointer(ID));
 end;
begin
 AList.Clear;
 Result := false;
 try
  PEImageLoader   :=  TPEImageLoader.Create;
  LastId := 0;
  // Загрузка NTDLL
  if not(PEImageLoader.LoadPEFile(GetSystemDirectoryPath + 'ntdll.dll')) then exit;
  // Построение списка экспорта
  ExportTable := TStringList.Create;
  PEImageLoader.GetExportTable(ExportTable);
  // Сканирование списка экспорта - поиск ZW функций
  for i := 0 to ExportTable.Count - 1 do
   if pos('zw', LowerCase(ExportTable[i]))=1 then begin
    PhPtr := PEImageLoader.GetPhAddByRVA(Dword(ExportTable.Objects[i]));
    if (PhPtr <= 0) or (PhPtr > PEImageLoader.FS.Size-10) then continue;
    PhPtr := dword(PEImageLoader.FS.Memory) + PhPtr;
    // Проверка, есть ли стандартный код
    if (byte(pointer(PhPtr)^)    = $0B8) and
       (byte(pointer(PhPtr+3)^)  = 0) and
       (byte(pointer(PhPtr+4)^)  = 0)
     then begin
      // Добавление функции с заданным номером
      AddKernelFunction(ExportTable[i], dword(pointer(PhPtr+1)^));
      LastId := dword(pointer(PhPtr+1)^);
     end else begin
      // Добавление функции с номером предыдущей функции + 1
      LastId := LastId + 1;
      AddKernelFunction(ExportTable[i], LastId);
     end;
   end;

  Result := true;
 finally
  ExportTable.Free;
  PEImageLoader.Free;
 end;
end;

function GetKernelGUIFunctionList(AList : TStrings) : boolean;
var
 PEImageLoader   :  TPEImageLoader; // Класс - загрузчик PE
 ExportTable     :  TStringList;
 i               :  integer;
 PhPtr, LastId   :  DWORD;
 procedure AddKernelFunction(AFunctionName : string; ID : DWord);
 begin
  if AList.IndexOfObject(pointer(ID)) < 0 then
   AList.AddObject(AFunctionName, pointer(ID));
 end;
begin
 AList.Clear;
 Result := false;
 try
  PEImageLoader   :=  TPEImageLoader.Create;
  LastId := 0;
  // Загрузка NTDLL
  if not(PEImageLoader.LoadPEFile(GetSystemDirectoryPath + 'user32.dll')) then exit;
  // Построение списка экспорта
  ExportTable := TStringList.Create;
  PEImageLoader.GetExportTable(ExportTable);
  // Сканирование списка экспорта - поиск ZW функций
  for i := 0 to ExportTable.Count - 1 do begin
    PhPtr := PEImageLoader.GetPhAddByRVA(Dword(ExportTable.Objects[i]));
    if (PhPtr <= 0) or (PhPtr > PEImageLoader.FS.Size-10) then continue;
    PhPtr := dword(PEImageLoader.FS.Memory) + PhPtr;
    // Проверка, есть ли стандартный код
    {
    .text:77D8C7CE B8 A1 11 00 00                                mov     eax, 11A1h
    .text:77D8C7D3 BA 00 03 FE 7F                                mov     edx, 7FFE0300h
    .text:77D8C7D8 FF 12                                         call    dword ptr [edx]
    .text:77D8C7DA C2 04 00                                      retn    4
    B8 xx xx 00 00 BA xx xx xx 7F FF 12 C2
    }
    if (byte(pointer(PhPtr)^)    = $0B8)  and
       (byte(pointer(PhPtr+3)^)  = 0)     and
       (byte(pointer(PhPtr+4)^)  = 0)     and
       (byte(pointer(PhPtr+5)^)  = $0BA)  and
       (byte(pointer(PhPtr+10)^)  = $0FF) and
       (byte(pointer(PhPtr+11)^)  = $012) and
       (byte(pointer(PhPtr+12)^)  = $0C2)
     then begin
      // Добавление функции с заданным номером
      AddKernelFunction(ExportTable[i], dword(pointer(PhPtr+1)^));
      LastId := dword(pointer(PhPtr+1)^);
     end else begin
      // Добавление функции с номером предыдущей функции + 1
      LastId := LastId + 1;
      AddKernelFunction(ExportTable[i], LastId);
     end;
  end;
  Result := true;
 finally
  ExportTable.Free;
  PEImageLoader.Free;
 end;
end;

// Получение имени функций ядра
function GetKernelFunctionName(AList : TStrings; ID : dword; DecodeZwNt : boolean = false) : string;
var
 Poz : integer;
begin
 Result := '';
 Poz := AList.IndexOfObject(pointer(ID));
 if Poz < 0 then exit;
 Result := AList[Poz];
 if DecodeZwNt then
  if UpperCase(copy(Result, 1, 2)) = 'ZW' then begin
   Delete(Result, 1, 2);
   Result := 'Nt'+Result;
  end;
end;

end.

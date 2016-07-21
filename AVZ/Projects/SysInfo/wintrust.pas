unit wintrust;

interface

// Проверка подлинности файла
function CheckFileTrust(const AFilename: string): Boolean;
// Загрузка DLL и поиск функций
function LoadWintrustDLL : boolean;
var
 WinTrustLevel : integer = 1; // Уровень контроля

implementation

uses
  Windows,
  SysUtils;

const
  //Kostanten fur die dwUnionChoice in WINTRUST_DATA
  WTD_CHOICE_FILE             = 1;
  WTD_CHOICE_CATALOG          = 2;

  //Konstanten fur dwStateAction
  WTD_STATEACTION_IGNORE      = 0;
  WTD_STATEACTION_VERIFY      = 1;

  //UI Konstanten fur WINTRUST_DATA
  WTD_UI_NONE                 = 2;                //kein UI anzeigen

  //Konstanten zur Prufung auf zuruckgezogene Zertifikate
  WTD_REVOKE_NONE             = 0;                // keine zusatzliche Prufun

  //Konstanten fur TrustProvider
  WTD_SAFER_FLAG              = 256;              // fur Winxp Sp2 benotigt

  //Wintrust Action GUID?s
  WINTRUST_ACTION_GENERIC_VERIFY_V2: TGUID = '{00AAC56B-CD44-11d0-8CC2-00C04FC295EE}';


type

  CATALOG_INFO = record
    cbStruct: DWORD;                              // = sizeof(WINTRUST_CATALOG_INFO)
    sCatalogFile: array[0..MAX_PATH] of WCHAR;    // Dateiname incl. Pfad zur CAT Datei
  end;
  PCATALOG_INFO = ^CATALOG_INFO;


  WINTRUST_CATALOG_INFO = record
    cbStruct: DWORD;                              // = sizeof(WINTRUST_CATALOG_INFO)
    dwCatalogVersion: DWORD;                      // optional
    pcwszCatalogFilePath: LPCWSTR;                // benotigt, Dateiname incl. Pfad zur CAT Datei
    pcwszMemberTag: LPCWSTR;                      // benotigt, tag zum Mitglied im Katalog
    pcwszMemberFilePath: LPCWSTR;                 // benotigt, Dateiname incl. Pfad
    hMemberFile: THANDLE;                         // optional
  end;
  PWINTRUST_CATALOG_INFO = ^WINTRUST_CATALOG_INFO;


  WINTRUST_FILE_INFO = record
    cbStruct: DWORD;                              // = sizeof(WINTRUST_FILE_INFO)
    pcwszFilePath: LPCWSTR;                       // benotigt, Dateiname incl. Pfad
    pgKnownSubject: PGUID;                        // optional
    hFile: THANDLE;                               // optional
  end;
  PWINTRUST_FILE_INFO = ^WINTRUST_FILE_INFO;


  WINTRUST_DATA = packed record
    cbStruct: DWORD;                              // = sizeof(WINTRUST_DATA)
    pPolicyCallbackData: pointer;                 // optional - auf 0 setzen
    pSIPClientData: pointer;                      // optional - auf 0 setzen
    dwUIChoice: DWORD;                            // benotigt, UI auswahl
    fdwRevocationChecks: DWORD;                   // benotigt, auf zuruckgezogene Zertifikate prufen (online ben.)
    dwUnionChoice: DWORD;                         // benotigt, welche Datenstruktur soll verwendet werden
    pWTDINFO: pointer;                            // Pointer zu einer der Wintrust_X_Info Strukturen
    pFake: pointer;                               //Fake Pointer - notig damit der Speicer wieder freigegeben wird
    pFake1: pointer;                              //Fake Pointer - notig damit der Speicer wieder freigegeben wird
    pFake2: pointer;                              //Fake Pointer - notig damit der Speicer wieder freigegeben wird
    pFake3: pointer;                              //Fake Pointer - notig damit der Speicer wieder freigegeben wird
    dwStateAction: DWORD;
    hWVTStateData: THANDLE;
    pwszURLReference: PWChar;
    dwProvFlags: DWORD;
    dwUIContext: DWORD;

  end;
  PWINTRUST_DATA = ^WINTRUST_DATA;

  //Handle und Pointer auf KatalogAdminKontext
  HCatAdmin = THANDLE;
  PHCatAdmin = ^HCatAdmin;

type
  TCryptCATAdminAcquireContext         = function (PHCatAdmin: PHCatAdmin; pgSubsystem: PGUID; dwFlags: DWORD): BOOL; stdcall;
  TCryptCATAdminReleaseContext         = function (HCatAdmin: HCatAdmin; dwFlags: DWORD): BOOL; stdcall;
  TCryptCATAdminCalcHashFromFileHandle = function (hFile: THANDLE; pHashSize: PDWORD; pbHash: PByteArray; dwFlags: DWORD): BOOL; stdcall;
  TCryptCATAdminEnumCatalogFromHash    = function (HCatAdmin: HCatAdmin; pbHash: PByteArray; pHashSize: DWORD; dwFlags: DWORD; phPrevCatInfo: PHandle): THANDLE; stdcall;
  TCryptCATCatalogInfoFromContext      = function (hCatInfo: THANDLE; psCatInfo: PCATALOG_INFO; dwFlags: DWORD): BOOL; stdcall;
  TCryptCATAdminReleaseCatalogContext  = function (HCatAdmin: HCatAdmin; hCatInfo: THANDLE; dwFlags: DWORD): BOOL; stdcall;
  TWinVerifyTrust                      = function (hwnd: THANDLE; pgActionID: PGUID; pWintrustData: PWINTRUST_DATA): Longint; stdcall;

var
  hLibWintrust                : THANDLE = INVALID_HANDLE_VALUE; // Handle wintrust.dll

  // Функции wintrust.dll
  CryptCATAdminAcquireContext         : TCryptCATAdminAcquireContext;
  CryptCATAdminReleaseContext         : TCryptCATAdminReleaseContext;
  CryptCATAdminCalcHashFromFileHandle : TCryptCATAdminCalcHashFromFileHandle;
  CryptCATAdminEnumCatalogFromHash    : TCryptCATAdminEnumCatalogFromHash;
  CryptCATCatalogInfoFromContext      : TCryptCATCatalogInfoFromContext;
  CryptCATAdminReleaseCatalogContext  : TCryptCATAdminReleaseCatalogContext;
  WinVerifyTrust                      : TWinVerifyTrust;

function LoadWintrustDLL : boolean;
begin
 Result := false;
 if (hLibWintrust = INVALID_HANDLE_VALUE)  then begin
  // Загрузка DLL
  hLibWintrust := LoadLibrary('wintrust.dll');
  // Загрузка успешна ?? Если да, то ведем поиск функций
  if hLibWintrust <> INVALID_HANDLE_VALUE then begin
    CryptCATAdminAcquireContext         := GetProcAddress(hLibWintrust, 'CryptCATAdminAcquireContext');
    CryptCATAdminReleaseContext         := GetProcAddress(hLibWintrust, 'CryptCATAdminReleaseContext');
    CryptCATAdminCalcHashFromFileHandle := GetProcAddress(hLibWintrust, 'CryptCATAdminCalcHashFromFileHandle');
    CryptCATAdminEnumCatalogFromHash    := GetProcAddress(hLibWintrust, 'CryptCATAdminEnumCatalogFromHash');
    CryptCATCatalogInfoFromContext      := GetProcAddress(hLibWintrust, 'CryptCATCatalogInfoFromContext');
    CryptCATAdminReleaseCatalogContext  := GetProcAddress(hLibWintrust, 'CryptCATAdminReleaseCatalogContext');
    WinVerifyTrust                      := GetProcAddress(hLibWintrust, 'WinVerifyTrust');
  end;
 end;
 if hLibWintrust > 32 then begin
  Result :=  (@CryptCATAdminAcquireContext <> nil) and
             (@CryptCATAdminReleaseContext <> nil) and
             (@CryptCATAdminCalcHashFromFileHandle <> nil) and
             (@CryptCATAdminCalcHashFromFileHandle <> nil) and
             (@CryptCATCatalogInfoFromContext <> nil) and
             (@CryptCATAdminReleaseCatalogContext <> nil) and
             (@WinVerifyTrust <> nil);

 end else Result := false;
end;

function CheckFileTrust(const AFilename: string): Boolean;
var
  aByteHash                   : array[0..255] of Byte;
  iByteCount                  : Integer;
  hCatAdminContext            : HCatAdmin;
  WTrustData                  : WINTRUST_DATA;
  WTDCatalogInfo              : WINTRUST_CATALOG_INFO;
  WTDFileInfo                 : WINTRUST_FILE_INFO;
  CatalogInfo                 : CATALOG_INFO;
  hFile                       : THANDLE;
  hCatalogContext             : THANDLE;

  swFilename                  : WideString;
  swMemberTag                 : WideString;

  ilRet                       : Longint;
  x                           : Integer;
begin
  Result := False;
  if WinTrustLevel = 0 then exit;
  if not(LoadWintrustDLL) then exit;

  if not(FileExists(AFilename)) then exit;

  // Очистка памяти
  ZeroMemory(@CatalogInfo,    SizeOf(CatalogInfo));
  ZeroMemory(@WTDFileInfo,    SizeOf(WTDFileInfo));
  ZeroMemory(@WTDCatalogInfo, SizeOf(WTDCatalogInfo));
  ZeroMemory(@WTrustData,     SizeOf(WTrustData));

  swFilename := AFilename;

  hCatAdminContext  := INVALID_HANDLE_VALUE;
  hCatAdminContext  := INVALID_HANDLE_VALUE;

  // Открытие каталога безопасностие
  if not(CryptCATAdminAcquireContext(@hCatAdminContext, nil, 0)) then Exit;
  try
    // Открытие файла и получение его HASH
    hFile := CreateFile(PChar(string(AFilename)), GENERIC_READ, FILE_SHARE_READ, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    // Выход, если файл не найден
    if hFile = INVALID_HANDLE_VALUE then Exit;

    //GetFileSize(hFile, @lpFileSize);

    // Размер буфера для хранения HASH
    iByteCount := SizeOf(aByteHash);

    // Вычисление HASH для данного файла
    CryptCATAdminCalcHashFromFileHandle(hFile, @iByteCount, @aByteHash, 0);

    // Побайтное чтение хеша и сохранение его в виде строки Hex чисел
    for x := 0 to iByteCount - 1 do
     swMemberTag := swMemberTag + IntToHex(aByteHash[x], 2);

    CloseHandle(hFile);


    // Поиск в каталоге по HASH
    hCatalogContext := CryptCATAdminEnumCatalogFromHash(hCatAdminContext, @aByteHash, iByteCount, 0, nil);

    // Если Handle = 0 (не найдено), то передает WINTRUST_DATA.dwUnionChoice := WTD_CHOICE_FILE;
    if hCatalogContext = 0 then begin
      exit;
      // Заполнение WINTRUST_FILE_INFO
      WTDFileInfo.cbStruct       := SizeOf(WTDFileInfo);
      WTDFileInfo.pcwszFilePath  := PWideChar(swFilename);
      WTDFileInfo.pgKnownSubject := nil;
      WTDFileInfo.hFile := 0;
      // Заполнение WINTRUST_DATA
      WTrustData.cbStruct      := SizeOf(WTrustData);
      WTrustData.dwUnionChoice := WTD_CHOICE_FILE;
      WTrustData.pWTDINFO      := @WTDFileInfo;      // Указатель на WTDFileInfo
      WTrustData.dwUIChoice    := WTD_UI_NONE;       // Запрет пользовательского интерфейса
      WTrustData.fdwRevocationChecks := WTD_REVOKE_NONE;
      WTrustData.dwStateAction       := WTD_STATEACTION_IGNORE;
      WTrustData.dwProvFlags         := WTD_SAFER_FLAG;   //UI bei XP SP2 unterbinden
      WTrustData.hWVTStateData       := 0;
      WTrustData.pwszURLReference    := nil;
    end
    else
    begin
      //CatalogContext <> 0 also CATALOG_INFO benutzen
      //
      //CATALOG_INFO Struktur fullen
      if WinTrustLevel = 1 then begin
       Result := true;
       exit;
      end;
      CryptCATCatalogInfoFromContext(hCatalogContext, @CatalogInfo, 0);

      //WINTRUST_CATALOG_INFO Struktur initialisieren und fullen
      WTDCatalogInfo.cbStruct := SizeOf(WTDCatalogInfo);
      WTDCatalogInfo.pcwszCatalogFilePath := CatalogInfo.sCatalogFile;
      WTDCatalogInfo.pcwszMemberFilePath := PWideChar(swFilename);
      WTDCatalogInfo.pcwszMemberTag := PWideChar(swMemberTag);

      //WINTRUST_DATA Struktur initialisieren und fullen
      WTrustData.cbStruct := SizeOf(WTrustData);
      WTrustData.dwUnionChoice := WTD_CHOICE_CATALOG; //WINTRUST_CATALOG_INFO Struktur wahlen
      WTrustData.pWTDINFO := @WTDCatalogInfo;     //Pointer zu WINTRUST_CATALOG_INFO
      WTrustData.dwUIChoice := WTD_UI_NONE;
      WTrustData.fdwRevocationChecks := WTD_REVOKE_NONE;
      WTrustData.pPolicyCallbackData := nil;
      WTrustData.pSIPClientData := nil;
      WTrustData.dwStateAction := WTD_STATEACTION_VERIFY;
      WTrustData.dwProvFlags := 0;                //WTD_SAFER_FLAG;   //UI bei XP SP2 unterbinden
      WTrustData.hWVTStateData := 0;
      WTrustData.pwszURLReference := nil;
    end;

    // Вызов WinVerifyTrust для выполнения проверки
    ilRet := WinVerifyTrust(INVALID_HANDLE_VALUE, @WINTRUST_ACTION_GENERIC_VERIFY_V2, @WTrustData);

    // Если возвращается 0, то файл подписан и подпись успешно проверена
    Result := ilRet = 0;
  finally
   // Закрытие каталога безопасности
   CryptCATAdminReleaseCatalogContext(hCatAdminContext, hCatalogContext, 0);
   CryptCATAdminReleaseContext(hCatAdminContext, 0);
  end;
end;



initialization

finalization
 if hLibWintrust >= 32 then
  FreeLibrary(hLibWintrust);
end.

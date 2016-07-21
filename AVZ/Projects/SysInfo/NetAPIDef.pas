unit NetAPIDef;

interface
uses Windows, Dialogs, Sysutils;

type
 TShareInfo50 = packed record
  shi50_netname 	: array [0..12] of Char;
  shi50_type 		: Byte;
  shi50_flags 		: Word;
  shi50_remark 		: PChar;
  shi50_path 		: PChar;
  shi50_rw_password 	: array [0..8] of Char;
  shi50_ro_password 	: array [0..8] of Char;
 end;

 TShareInfo2 = packed record
  shi2_netname 			: PWChar;
  shi2_type			: DWORD;
  shi2_remark 			: PWChar;
  shi2_permissions		: DWORD;
  shi2_max_uses 		: DWORD;
  shi2_current_uses 		: DWORD;
  shi2_path 			: PWChar;
  shi2_passwd 			: PWChar;
 end;
 PShareInfo2 = ^TShareInfo2;
 TShareInfo2Array = array [0..512] of TShareInfo2;
 PShareInfo2Array = ^TShareInfo2Array;

 TSessionInfo50 = packed record
   sesi50_cname       : PChar;
   sesi50_username    : PChar;
   sesi50_key         : Cardinal;
   sesi50_num_conns   : Word;
   sesi50_num_opens   : Word;
   sesi50_time        : Cardinal;
   sesi50_idle_time   : Cardinal;
   sesi50_protocol    : Byte;
   pad1               : Byte;
 end;

 TSessionInfo502 = packed record
   Sesi502_cname	: PWideChar;
   Sesi502_username	: PWideChar;
   Sesi502_num_opens	: DWORD;
   Sesi502_time		: DWORD;
   Sesi502_idle_time	: DWORD;
   Sesi502_user_flags	: DWORD;
   Sesi502_cltype_name	: PWideChar;
   Sesi502_transport	: PWideChar;
 End;
 PSessionInfo502 = ^TSessionInfo502;
 TSessionInfo502Array = array[0..512] of TSessionInfo502;
 PSessionInfo502Array = ^TSessionInfo502Array;

 TFileInfo50 = packed record
   fi50_id          : Cardinal;
   fi50_permissions : WORD;
   fi50_num_locks   : WORD;
   fi50_pathname    : PChar;
   fi50_username    : PChar;
   fi50_sharename   : PChar;
 end;

 TFileInfo3 = packed record
   fi3_id          : DWORD;
   fi3_permissions : DWORD;
   fi3_num_locks   : DWORD;
   fi3_pathname    : PWChar;
   fi3_username    : PWChar;
 end;

 PFileInfo3 = ^TFileInfo3;
 TFileInfo3Array = array[0..512] of TFileInfo3;
 PFileInfo3Array = ^TFileInfo3Array;


 TNetShareEnum9x   = function (pszServer : PChar; sLevel : Cardinal; pbBuffer : PChar; cbBuffer : Cardinal;  pcEntriesRead, pcTotalAvail : Pointer):DWORD; stdcall;
 TNetShareEnumNT   = function (ServerName :PWChar; Level : DWORD; Bufptr : Pointer; Prefmaxlen : DWORD; EntriesRead, TotalEntries, resume_handle:LPDWORD): DWORD; stdcall;

 TNetSessionEnum9x = function (pszServer : PChar; sLevel : DWORD; pbBuffer : Pointer; cbBuffer : DWORD; pcEntriesRead, pcTotalAvial : Pointer):integer; stdcall;
 TNetSessionEnumNT = function (ServerName, UncClientName, Username : PWChar; Level : DWORD; bufptr : Pointer; prefmaxlen : DWORD; entriesread, totalentries,resume_handle : LPDWORD):DWORD; stdcall;

 TNetSessionDel9x = function (pszServer, pszClientName : PChar; sReserved : word):integer; stdcall;
 TNetSessionDelNT = function (ServerName, UncClientName, Username : PWChar):DWORD; stdcall;

 TNetFileEnum9x      = function (pszServer, pszBasePath : PChar; sLevel : DWORD; pbBuffer : Pointer; cbBuffer : DWORD; pcEntriesRead, pcTotalAvail : Pointer):Integer; stdcall;
 TNetFileEnumNT      = function (servername, basepath, username : PWChar; level : DWORD; bufptr : Pointer; prefmaxlen : DWORD; entriesread, totalentries, resume_handle	: LPDWORD):DWORD; stdcall;
 TNetFileClose9x     = function (pszServer	:PChar;	UlFileId	:LongWord):DWORD; stdcall;
 TNetFileCloseNT     = function (ServerName	:PWideChar; 	FileId	:DWORD):DWORD; stdcall;

 function NetShareEnum9x(pszServer : PChar; sLevel : Cardinal; pbBuffer : PChar; cbBuffer : Cardinal;  pcEntriesRead, pcTotalAvail : Pointer):DWORD; stdcall;
 function NetShareEnumNT(ServerName :PWChar; Level	  :DWORD; Bufptr :Pointer; Prefmaxlen	:DWORD; EntriesRead, TotalEntries, resume_handle:LPDWORD): DWORD; stdcall;

 function NetSessionEnum9x(pszServer : PChar; sLevel : DWORD; pbBuffer : Pointer; cbBuffer : DWORD; pcEntriesRead, pcTotalAvial : Pointer):integer;
 function NetSessionEnumNT(ServerName, UncClientName, Username : PWChar; Level : DWORD; bufptr : Pointer; prefmaxlen : DWORD; entriesread, totalentries,resume_handle : LPDWORD):DWORD;

 function NetSessionDel9x(pszServer, pszClientName : PChar; sReserved : SmallInt):DWORD; stdcall;
 function NetSessionDelNT(ServerName, UncClientName, Username : PWChar):DWORD; stdcall;


 function NetFileEnum9x(pszServer, pszBasePath : PChar; sLevel : DWORD; pbBuffer : Pointer; cbBuffer : DWORD; pcEntriesRead, pcTotalAvail : Pointer):Integer; stdcall;
 function NetFileEnumNT(servername, basepath, username : PWChar; level : DWORD; bufptr : Pointer; prefmaxlen : DWORD; entriesread, totalentries, resume_handle	: LPDWORD):DWORD; stdcall;

 function NetFileClose9x(pszServer	:PChar;	UlFileId	:LongWord):DWORD; stdcall;
 function NetFileCloseNT(ServerName	:PWideChar; 	FileId	:DWORD):DWORD; stdcall;

implementation
var
  hNETAPI32 : THandle;  // Handle на библиотеку NETAPI32
  hSVRAPI   : THandle;  // Handle на библиотеку SVRAPI
  _NetShareEnum9x   : TNetShareEnum9x;
  _NetShareEnumNT   : TNetShareEnumNT;
  _NetSessionEnum9x : TNetSessionEnum9x;
  _NetSessionEnumNT : TNetSessionEnumNT;
  _NetSessionDel9x  : TNetSessionDel9x;
  _NetSessionDelNT  : TNetSessionDelNT;
  _NetFileEnum9x    : TNetFileEnum9x;
  _NetFileEnumNT    : TNetFileEnumNT;
  _NetFileClose9x   : TNetFileClose9x;
  _NetFileCloseNT   : TNetFileCloseNT;

function CheckNETAPI32Loaded : boolean;
begin
 Result := false;
 // Библиотека еще на зегружена - пробуем ее загрузить
 if hNETAPI32 = 0 then begin
  hNETAPI32 := LoadLibrary('NETAPI32.DLL');
  // Ошибка загрузки ??
  if hNETAPI32 = 0 then begin
   hNETAPI32 := 0;
   Result := False;
   Exit;
  end;
  @_NetShareEnumNT   := GetProcAddress(hNETAPI32, 'NetShareEnum');
  @_NetSessionEnumNT := GetProcAddress(hNETAPI32, 'NetSessionEnum');
  @_NetSessionDelNT  := GetProcAddress(hNETAPI32, 'NetSessionDel');
  @_NetFileEnumNT    := GetProcAddress(hNETAPI32, 'NetFileEnum');
  @_NetFileCloseNT   := GetProcAddress(hNETAPI32, 'NetFileClose');
  Result := true;
 end else Result := true;
end;

function CheckSVRAPILoaded : boolean;
begin
 Result := false;
 // Библиотека еще на зегружена - пробуем ее загрузить
 if hSVRAPI = 0 then begin
  hSVRAPI := LoadLibrary('SVRAPI.DLL');
  // Ошибка загрузки ??
  if hSVRAPI = 0 then begin
   hSVRAPI := 0;
   Result  := False;
   Exit;
  end;
  @_NetShareEnum9x   :=  GetProcAddress(hSVRAPI, 'NetShareEnum');
  @_NetSessionEnum9x :=  GetProcAddress(hSVRAPI, 'NetSessionEnum');
  @_NetSessionDel9x  :=  GetProcAddress(hSVRAPI, 'NetSessionDel');
  @_NetFileEnum9x    :=  GetProcAddress(hSVRAPI, 'NetFileEnum');
  @_NetFileClose9x   :=  GetProcAddress(hSVRAPI, 'NetFileClose2');
  Result := true;
 end else Result := true;
end;

function NetShareEnum9x(pszServer : PChar; sLevel : Cardinal; pbBuffer : PChar; cbBuffer : Cardinal;  pcEntriesRead, pcTotalAvail : Pointer):DWORD; stdcall;
begin
 if CheckSVRAPILoaded then
  Result :=  _NetShareEnum9x(pszServer, sLevel, pbBuffer, cbBuffer, pcEntriesRead, pcTotalAvail)
   else Result := ERROR_NOT_SUPPORTED;
end;

function NetShareEnumNT(ServerName :PWChar; Level  :DWORD; Bufptr :Pointer; Prefmaxlen	:DWORD; EntriesRead, TotalEntries, resume_handle:LPDWORD): DWORD; stdcall;
begin
 if CheckNETAPI32Loaded then
  Result :=  _NetShareEnumNT(ServerName, Level, Bufptr, Prefmaxlen, EntriesRead, TotalEntries, resume_handle)
   else Result := ERROR_NOT_SUPPORTED;
end;

function NetSessionEnum9x(pszServer : PChar; sLevel : DWORD; pbBuffer : Pointer; cbBuffer : DWORD; pcEntriesRead, pcTotalAvial : Pointer):integer;
begin
 if CheckSVRAPILoaded then
  Result := _NetSessionEnum9x(pszServer, sLevel, pbBuffer, cbBuffer, pcEntriesRead, pcTotalAvial)
   else Result := ERROR_NOT_SUPPORTED;
end;

function NetSessionEnumNT(ServerName, UncClientName, Username : PWChar; Level : DWORD; bufptr : Pointer; prefmaxlen : DWORD; entriesread, totalentries,resume_handle : LPDWORD):DWORD;
begin
 if CheckNETAPI32Loaded then
  Result := _NetSessionEnumNT(ServerName, UncClientName, Username, Level, bufptr, prefmaxlen, entriesread, totalentries, resume_handle)
   else Result := ERROR_NOT_SUPPORTED;
end;

function NetSessionDel9x(pszServer, pszClientName : PChar; sReserved : Smallint):DWORD; stdcall;
begin
 if CheckSVRAPILoaded then
  Result := _NetSessionDel9x(pszServer, pszClientName, sReserved)
   else Result := ERROR_NOT_SUPPORTED;
end;

function NetSessionDelNT(ServerName, UncClientName, Username : PWChar):DWORD; stdcall;
begin
 if CheckNETAPI32Loaded then
  Result := _NetSessionDelNT(ServerName, UncClientName, Username)
   else Result := ERROR_NOT_SUPPORTED;
end;

function NetFileEnum9x(pszServer, pszBasePath : PChar; sLevel : DWORD; pbBuffer : Pointer; cbBuffer : DWORD; pcEntriesRead, pcTotalAvail : Pointer):Integer; stdcall;
begin
 if CheckSVRAPILoaded then
  Result := _NetFileEnum9x(pszServer, pszBasePath, sLevel, pbBuffer, cbBuffer, pcEntriesRead, pcTotalAvail)
   else Result := ERROR_NOT_SUPPORTED;
end;

function NetFileEnumNT(servername, basepath, username : PWChar; level : DWORD; bufptr : Pointer; prefmaxlen : DWORD; entriesread, totalentries, resume_handle	: LPDWORD):DWORD; stdcall;
begin
 if CheckNETAPI32Loaded then
  Result := _NetFileEnumNT(servername, basepath, username, level, bufptr, prefmaxlen, entriesread, totalentries, resume_handle)
   else Result := ERROR_NOT_SUPPORTED;
end;

function NetFileClose9x(pszServer	:PChar;	UlFileId	:LongWord):DWORD; stdcall;
begin
 if CheckSVRAPILoaded then
  Result := _NetFileClose9x(pszServer, UlFileId)
   else Result := ERROR_NOT_SUPPORTED;
end;

function NetFileCloseNT(ServerName	:PWideChar; FileId	:DWORD):DWORD; stdcall;
begin
 if CheckNETAPI32Loaded then
  Result := _NetFileCloseNT(ServerName, FileId)
   else Result := ERROR_NOT_SUPPORTED;
end;

initialization
 hNETAPI32 := 0;
 hSVRAPI   := 0;

finalization
 if hNETAPI32 <> 0 then
  FreeLibrary(hNETAPI32);
 if hSVRAPI <> 0 then
  FreeLibrary(hSVRAPI);
end.

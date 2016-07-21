program AVZTestCons;

{$APPTYPE CONSOLE}

uses
  Windows,
  SysUtils,
  ActiveX;

function AVZ_InitializeExA(var ppAVZAScanKernel : pointer; ADbPath : PChar): DWORD; stdcall; external 'avzkrnl.dll' name 'Z1';
function AVZ_Done(ppAVZAScanKernel : pointer): DWORD; stdcall; external 'avzkrnl.dll' name 'Z3';
function AVZ_GetSomeInfo(ppAVZAScanKernel : pointer; szScript : PWChar; AVZCallBackProc : Pointer; pScriptContext : pointer; Stream : IStream) : DWORD; stdcall; external 'avzkrnl.dll' name 'Z6';
function AVZ_Free(ppAVZAScanKernel : pointer; pPtr : pointer) : DWORD; stdcall;external 'avzkrnl.dll' name 'Z7';


function AVZCallBackProc(pScriptContext : pointer; dwProgressCurr, dwProgressMax : DWORD; szMsg: PWChar; dwMsgSize : dword; AVZMSG : DWORD) : DWORD; stdcall;
begin
 Result := 0;
 if (dwMsgSize > 0) and (szMsg <> nil) and (String(szMsg) <> '') then
  Writeln(String(szMsg));
end;

var
 ppAVZAScanKernel : pointer;
 Stream : IStream;
 ST : string;
 S  : WideString;
begin
 Stream := nil;
 Writeln('AVZ console test. Enter script, end of script - "end." command');
 repeat
  Readln(ST);
  ST := Trim(ST);
  if ST <> '' then
   S := S + #$0D#$0A + ST;
 until UpperCase(ST) = 'END.';
 Writeln('Execite script ...');
 WriteLn('AVZ_InitializeExA = '+inttostr(AVZ_InitializeExA(ppAVZAScanKernel, 'c:\avz4\base\')));
 WriteLn('ppAVZAScanKernel = '+inttostr(dword(ppAVZAScanKernel)));
 WriteLn('AVZ_GetSomeInfo = '+inttohex(dword(AVZ_GetSomeInfo(ppAVZAScanKernel, PWideChar(S), @AVZCallBackProc, nil, Stream)), 6));
 WriteLn('AVZ_Done = '+inttohex(dword(AVZ_Done(ppAVZAScanKernel)), 6));
end.

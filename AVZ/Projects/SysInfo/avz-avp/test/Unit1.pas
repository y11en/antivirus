unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ComCtrls, ActiveX;

type
  TForm1 = class(TForm)
    Button1: TButton;
    ProgressBar1: TProgressBar;
    GroupBox1: TGroupBox;
    mmScript: TMemo;
    GroupBox2: TGroupBox;
    Memo1: TMemo;
    Button2: TButton;
    Button3: TButton;
    Edit1: TEdit;
    Button4: TButton;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
  private
  public
    { Public declarations }
  end;

type
 // CallBack функции.
 // Статистика выполнения скрипта. pScriptContext - контекст скрипта, dwProgressCurr, dwProgressMax - прогресс-индикация; szMsg, dwMsgSize - сообщение для лога; AVZMSG - код типа сообщения
 TAVZCallBackProc = function (pScriptContext : pointer; dwProgressCurr, dwProgressMax : DWORD; szMsg: PWChar; dwMsgSize : dword; AVZMSG : DWORD) : DWORD; stdcall;
 PAVZCallBackProc = ^TAVZCallBackProc;
 // Перевод строки. Строка выделяется функцией
 TGetLocalizedStr = function (szID: PChar; var pszResult : PWChar) : DWORD; stdcall;
 PGetLocalizedStr = ^TGetLocalizedStr;
 // Освобождение буфера переведенной строки
 TFreeLocalizedStr = function (pszResult : PWChar) : DWORD; stdcall;
 PFreeLocalizedStr = ^TFreeLocalizedStr;

function AVZ_InitializeExA(var ppAVZAScanKernel : pointer; ADbPath : PChar): DWORD; stdcall; external 'avzkrnl.dll' name 'Z1';
function AVZ_Done(ppAVZAScanKernel : pointer): DWORD; stdcall; external 'avzkrnl.dll' name 'Z3';
function AVZ_RunScript(ppAVZAScanKernel : pointer; szScript : PWChar; pScriptContext : pointer; Stream : IStream) : DWORD; stdcall; external 'avzkrnl.dll' name 'Z6';
function AVZ_DeserializeXML(ppAVZAScanKernel : pointer; XMLStream : IStream; szTagName, szBufFormat : PWideChar; var ppAVZBuffer : pointer; var BufSize : dword): DWORD; stdcall; external 'avzkrnl.dll' name 'Z9';
function AVZ_Free(ppAVZAScanKernel : pointer; pPtr : pointer) : DWORD; stdcall;external 'avzkrnl.dll' name 'Z7';
function AVZ_SetCallbackProc(ppAVZAScanKernel : pointer; dwCallBackID : dword; AVZCallBackProc : pointer) : DWORD; stdcall;external 'avzkrnl.dll' name 'Z10';

function AVZ_LoadCustomScriptDB(ppAVZAScanKernel : pointer; szDBFilename : PWideChar; var ppDBHandle : pointer) : DWORD; stdcall; external 'avzkrnl.dll' name 'Z13';
function AVZ_GetCustomScriptDBScrCount(ppAVZAScanKernel, ppDBHandle : pointer; var RecCount : dword) : DWORD; stdcall;external 'avzkrnl.dll' name 'Z14';
function AVZ_RunCustomScript(ppAVZAScanKernel, ppDBHandle : pointer; AID, AMode : dword; var Res : dword) : DWORD; stdcall;external 'avzkrnl.dll' name 'Z12';
function AVZ_FreeCustomScriptDB(ppAVZAScanKernel, ppDBHandle : pointer) : DWORD; stdcall; external 'avzkrnl.dll' name 'Z15';

var
  Form1: TForm1;
  StopF : boolean;
  ST    : widestring = '-loc-';

implementation

uses Math;

function GetLocalizedStr(pTaskContext : pointer;szID: PChar; var pszResult : PWChar) : DWORD; stdcall;
begin
 Result :=0;
 pszResult := @St[1];
end;

function FreeLocalizedStr(pTaskContext : pointer;pszResult : PWChar) : DWORD; stdcall;
begin
 Result := 0;
end;

{$R *.dfm}
function AVZCallBackProc(pScriptContext : pointer; dwProgressCurr, dwProgressMax : DWORD; szMsg: PWChar; dwMsgSize : dword; AVZMSG : DWORD) : DWORD; stdcall;
begin
 Result := 0;
 Application.ProcessMessages;
 Form1.ProgressBar1.Max      := dwProgressMax;
 Form1.ProgressBar1.Position := dwProgressCurr;
 if (dwMsgSize > 0) and (szMsg <> nil) and (String(szMsg) <> '') then
  Form1.Memo1.Lines.Add(String(szMsg));
 if StopF then
  Result := 4;
end;

procedure TForm1.Button1Click(Sender: TObject);
var
 ppAVZAScanKernel : pointer;
 Stream : IStream;
 S : WideString;
begin
 S := mmScript.Lines.text;
 StopF := false;
 Stream := nil;
 Memo1.Lines.Add('AVZ_InitializeExA = '+inttostr(AVZ_InitializeExA(ppAVZAScanKernel, 'c:\avz4\base\')));
 Memo1.Lines.Add('ppAVZAScanKernel = '+inttostr(dword(ppAVZAScanKernel)));
// AVZCallBackProc
 AVZ_SetCallbackProc(ppAVZAScanKernel, 0, @AVZCallBackProc);
 AVZ_SetCallbackProc(ppAVZAScanKernel, 1, @GetLocalizedStr);
 AVZ_SetCallbackProc(ppAVZAScanKernel, 2, @FreeLocalizedStr);
 Memo1.Lines.Add('AVZ_RunScript = '+inttohex(dword(AVZ_RunScript(ppAVZAScanKernel, PWideChar(S), nil, Stream)), 6));
 Memo1.Lines.Add('AVZ_Done 1 = '+inttohex(dword(AVZ_Done(ppAVZAScanKernel)), 6));
 Memo1.Lines.Add('AVZ_Done 2 = '+inttohex(dword(AVZ_Done(ppAVZAScanKernel)), 6));
end;

procedure TForm1.Button2Click(Sender: TObject);
begin
 StopF := true;
end;

procedure TForm1.Button3Click(Sender: TObject);
var
 pRecords, ppAVZAScanKernel : pointer;
 Stream   : TStreamadapter;
 S, S1 : WideString;
 MS       : TMemoryStream;
 BufSize  : dword;
begin
 StopF := false;
 Stream := nil;
 MS  := TMemoryStream.Create;
 MS.Clear;
 MS.LoadFromFile('LOG\virusinfo_syscheck.xml');
 S  := 'AVZ\PROCESS';
 S1 := Trim(Edit1.Text);
 Stream := TStreamAdapter.Create(MS);
 Memo1.Lines.Add('AVZ_InitializeExA = '+inttostr(AVZ_InitializeExA(ppAVZAScanKernel, 'c:\avz4\base\')));
 Memo1.Lines.Add('ppAVZAScanKernel = '+inttostr(dword(ppAVZAScanKernel)));
 Memo1.Lines.Add('AVZ_GetSomeInfo = '+inttohex(dword(AVZ_DeserializeXML(ppAVZAScanKernel, Stream, PWideChar(S), PWideChar(S1), pRecords, bufsize)), 6));
 MS.Clear;
 MS.Write(pRecords^, BufSize);
 MS.Seek(0,0);
 MS.SaveToFile('dump.tda');
 Memo1.Lines.Add('pRecords = '+inttohex(dword(pRecords), 6));
 Memo1.Lines.Add('AVZ_Free = '+inttohex(dword(AVZ_Free(ppAVZAScanKernel, pRecords)), 6));
 Memo1.Lines.Add('AVZ_Done 1 = '+inttohex(dword(AVZ_Done(ppAVZAScanKernel)), 6));
 Memo1.Lines.Add('AVZ_Done 2 = '+inttohex(dword(AVZ_Done(ppAVZAScanKernel)), 6));
end;

procedure TForm1.Button4Click(Sender: TObject);
var
 ppAVZAScanKernel, ppAVZDBHandle : pointer;
 Stream : IStream;
 S : WideString;
 Res : dword;
begin
 S := mmScript.Lines.text;
 StopF := false;
 Stream := nil;
 Memo1.Lines.Add('AVZ_InitializeExA = '+inttostr(AVZ_InitializeExA(ppAVZAScanKernel, 'c:\avz4\base\')));
 Memo1.Lines.Add('ppAVZAScanKernel = '+inttostr(dword(ppAVZAScanKernel)));
// AVZCallBackProc
 AVZ_SetCallbackProc(ppAVZAScanKernel, 0, @AVZCallBackProc);
 AVZ_SetCallbackProc(ppAVZAScanKernel, 1, @GetLocalizedStr);
 AVZ_SetCallbackProc(ppAVZAScanKernel, 2, @FreeLocalizedStr);
 S := 'E:\Delphi5\Delphi7\Projects\SysInfo\avz-avp\LOG\test_db.avz';
 Memo1.Lines.Add('AVZ_LoadCustomScriptDB = '+inttohex(dword(AVZ_LoadCustomScriptDB(ppAVZAScanKernel, PWideChar(S), ppAVZDBHandle)), 6));
 Memo1.Lines.Add('AVZ_GetCustomScriptDBScrCount = '+inttohex(dword(AVZ_GetCustomScriptDBScrCount(ppAVZAScanKernel, ppAVZDBHandle, res)), 6));
 Memo1.Lines.Add('  Res = '+inttostr(Res));
 Memo1.Lines.Add('AVZ_RunCustomScript = '+inttohex(dword(AVZ_RunCustomScript(ppAVZAScanKernel, ppAVZDBHandle, 1, 1, res)), 6));
 Memo1.Lines.Add('  Res = '+inttostr(Res));
 Memo1.Lines.Add('AVZ_Done 1 = '+inttohex(dword(AVZ_Done(ppAVZAScanKernel)), 6));
 Memo1.Lines.Add('AVZ_Done 2 = '+inttohex(dword(AVZ_Done(ppAVZAScanKernel)), 6));
end;

end.

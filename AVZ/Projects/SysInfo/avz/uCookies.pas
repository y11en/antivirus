unit uCookies;
// ******************* Поиск Cookie по заданном содержимому ******************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ComCtrls, Buttons, ExtCtrls, zBase64, zUUE, Registry,
  uTextReportView;

type
  TCookiesSearch = class(TForm)
    Panel1: TPanel;
    Label1: TLabel;
    edSearchTest: TEdit;
    mbStart: TBitBtn;
    mbStop: TBitBtn;
    BitBtn3: TBitBtn;
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    LogMemo: TMemo;
    TabSheet2: TTabSheet;
    lvFoundCookies: TListView;
    BitBtn1: TBitBtn;
    TabSheet3: TTabSheet;
    GroupBox1: TGroupBox;
    cbCodeBase64: TCheckBox;
    cbCodeUUE: TCheckBox;
    cbCodeURL: TCheckBox;
    cbCodeQP: TCheckBox;
    GroupBox2: TGroupBox;
    cbCheckExplorer: TCheckBox;
    cbCheckFirefox: TCheckBox;
    procedure mbStartClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
  private
    ScanTxt : TStrings;
    Function DecodeQuotedPrintable(S : string) : string;
    Function DecodeURLCode(S : string) : string;
    Function ScanCookie(ACookie : TStrings) : integer;
    Function GetUserIECookiesPath : string;
    Function GetUserAppDataPath : string;
  public
   // Добавление в протокол
   procedure AddToLog(s : string);
   // Добавление найденного кукиза
   procedure AddFoundCookie(AName, ABrowser, ACookieText : string);
   // Поиск кукизов в IE
   procedure SearchIECookies(APath : string);
   // Поиск кукизов в Firefox
   procedure SearchFirefoxCookies(APath : string);
   // Создание списка масок
   Function CreateMaskList : boolean;
   function Execute : boolean;
  end;

var
  CookiesSearch: TCookiesSearch;

function ExecuteCookiesSearch : boolean;
implementation
uses zutil, zTranslate;
{$R *.dfm}

{ TCookiesSearch }
function ExecuteCookiesSearch : boolean;
begin
 CookiesSearch := nil;
 try
  CookiesSearch := TCookiesSearch.Create(nil);
  Result := CookiesSearch.Execute;
 finally
  CookiesSearch.Free;
  CookiesSearch := nil;
 end;
end;

procedure TCookiesSearch.AddToLog(s: string);
begin
 LogMemo.Lines.Add(TranslateStr(s));
end;

function TCookiesSearch.DecodeQuotedPrintable(S: string): string;
var
 i : integer;
begin
 Result := '';
 i := 1;
 while i < Length(S) do begin
  if (S[i] <> '=') or (Length(S)-i < 3) then begin
   Result := Result + S[i];
   inc(i);
  end else begin
   inc(i);
   Result := Result + chr(StrToIntDef('$'+S[i]+S[i+1], $20));
   inc(i, 2);
  end;
 end;
end;

function TCookiesSearch.ScanCookie(ACookie: TStrings): integer;
var
 i, j : integer;
 S : string;
 SA : array[1..12] of String;
begin
 Result := -1;
 S := Trim(ACookie.Text);
 for j := 1 to 12 do
  SA[j] := '';
 SA[1] := AnsiLowerCase(S);
 // Base64
 if cbCodeBase64.Checked then begin
  SA[2] := AnsiLowerCase(DeCodeStringBase64Ex(S));
  SA[3] := AnsiLowerCase(DeCodeStringBase64Ex(copy(S, 2, MaxInt)));
  SA[4] := AnsiLowerCase(DeCodeStringBase64Ex(copy(S, 3, MaxInt)));
  SA[5] := AnsiLowerCase(DeCodeStringBase64Ex(copy(S, 4, MaxInt)));
 end;
 // UUE
 if cbCodeUUE.Checked then begin
  SA[6] := AnsiLowerCase(DeCodeStringUUEEx(S));
  SA[7] := AnsiLowerCase(DeCodeStringUUEEx(copy(S, 2, MaxInt)));
  SA[8] := AnsiLowerCase(DeCodeStringUUEEx(copy(S, 3, MaxInt)));
  SA[9] := AnsiLowerCase(DeCodeStringUUEEx(copy(S, 4, MaxInt)));
 end;
 // QuotedPrintable
 if cbCodeQP.Checked then
  SA[10] := AnsiLowerCase(DecodeQuotedPrintable(S));
 // URL
 if cbCodeURL.Checked then
  SA[11] := AnsiLowerCase(DecodeURLCode(S));
 for i := 0 to ScanTxt.Count - 1 do
  for j := 1 to 11 do
   if Pos(ScanTxt[i], SA[j]) > 0 then begin
    Result := i;
    exit;
   end;
end;

procedure TCookiesSearch.mbStartClick(Sender: TObject);
begin
 mbStart.Enabled := false;
 mbStop.Enabled  := true;
 try
  Screen.Cursor := crHourGlass;
  // Очистка результатов
  LogMemo.Clear;
  lvFoundCookies.Clear;
  lvFoundCookies.SortType := stNone;
  // Создание списка масок
  CreateMaskList;
  // Поиск в IE
  if cbCheckExplorer.Checked then
   SearchIECookies(GetUserIECookiesPath);
  // Поиск в Firefox
  if cbCheckFirefox.Checked then
   SearchFirefoxCookies(GetUserAppDataPath);
 finally
  Screen.Cursor := crDefault;
  mbStart.Enabled := true;
  mbStop.Enabled  := false;
 end;
end;

procedure TCookiesSearch.SearchFirefoxCookies(APath: string);
var
 SR     : TSearchRec;
 Res, i, cnt : integer;
 S : string;
 Lines, Lines1  : TStringList;
begin
 cnt := 0;
 if APath = '' then exit;
 AddToLog('--- $AVZ0055 ---');
 APath := NormalDir(APath) + 'Mozilla\Firefox\Profiles\';
 Lines  := TStringList.Create;
 Lines1 := TStringList.Create;
 APath := NormalDir(APath);
 Res := FindFirst(APath+'*.*', faAnyFile, SR);
 while Res = 0 do begin
  if (SR.Attr and faDirectory) > 0 then
   try
    Lines.LoadFromFile(APath + SR.Name + '\cookies.txt');
    for i := 0 to Lines.Count - 1 do
     if copy(Trim(Lines[i]), 1, 1) <> '#' then  begin
      S := Lines[i];
      delete(S, 1, pos(#9, S));
      delete(S, 1, pos(#9, S));
      delete(S, 1, pos(#9, S));
      delete(S, 1, pos(#9, S));
      delete(S, 1, pos(#9, S));
      Lines1.Text := S;
      inc(cnt);
      if ScanCookie(Lines1) >= 0  then begin
       AddFoundCookie(copy(Lines1.Text, 1, pos(#9, Lines1.Text)-1), 'Firefox: '+SR.Name, Lines1.Text);
     end;
    end;
   except
  end;
  Res := FindNext(SR);
 end;
 AddToLog('$AVZ0056. $AVZ0865: '+IntToStr(Cnt));
end;

procedure TCookiesSearch.SearchIECookies(APath: string);
var
 SR    : TSearchRec;
 Res   : integer;
 Lines : TStringList;
 Cnt : integer;
begin
 AddToLog('--- $AVZ0057 ---');
 Cnt := 0;
 Lines := TStringList.Create;
 APath := NormalDir(APath);
 Res := FindFirst(APath+'*.txt', faAnyFile, SR);
 while Res = 0 do begin
  try
   Lines.LoadFromFile(APath + SR.Name);
   inc(cnt);
   if cnt mod 20 = 0 then Application.ProcessMessages;
   if ScanCookie(Lines) >= 0  then begin
    AddFoundCookie(SR.Name, 'Internet Explorer', Lines.Text);
   end;
  except
  end;
  Res := FindNext(SR);
 end;
 AddToLog('$AVZ0058. $AVZ0865: '+IntToStr(Cnt));
end;

procedure TCookiesSearch.FormCreate(Sender: TObject);
begin
 ScanTxt := TStringList.Create;
 TranslateForm(Self);
end;

function TCookiesSearch.CreateMaskList: boolean;
var
 s, st : string;
begin
 ScanTxt.Clear;
 S := Trim(edSearchTest.Text);
 S := StringReplace(S, ';', ',', [rfReplaceAll]);
 S := StringReplace(S, ' ', ',', [rfReplaceAll]);
 S := S + ',';
 while pos(',', S) > 0 do begin
  st := AnsiLowerCase(Trim(copy(S, 1, pos(',', S)-1)));
  delete(S, 1, pos(',', S));
  if st <> '' then
   ScanTxt.Add(st);
 end;
end;

function TCookiesSearch.GetUserIECookiesPath: string;
var
 Reg : TRegistry;
begin
 Result := '';
 Reg := TRegistry.Create;
 Reg.RootKey := HKEY_CURRENT_USER;
 Reg.Access := KEY_READ;
 if Reg.OpenKey('Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders', false) then begin
  if Reg.ValueExists('Cookies') then
   Result := Reg.ReadString('Cookies');
  Reg.CloseKey;
 end;
end;

function TCookiesSearch.DecodeURLCode(S: string): string;
var
 i : integer;
 buf : packed array[0..5] of word;
begin
 Result := '';
 i := 1;
 while i < Length(S) do begin
  if (S[i] <> '%') or (Length(S)-i < 3) then begin
   Result := Result + S[i];
   inc(i);
  end else begin
   inc(i);
   if not(S[i] in ['u', 'U']) then begin
    Result := Result + chr(StrToIntDef('$'+S[i]+S[i+1], $20));
    inc(i, 2);
   end else begin
    inc(i);
    ZeroMemory(@Buf, SizeOf(Buf));
    buf[0] := StrToIntDef('$'+copy(S, i, 4), $20);
    Result := Result + WideCharToString(@Buf);
    inc(i, 4);
   end;
  end;
 end;
end;

procedure TCookiesSearch.AddFoundCookie(AName, ABrowser,
  ACookieText: string);
begin
 AddToLog(ABrowser + ' : '+AName);
 with lvFoundCookies.Items.Add do begin
  Caption := AName;
  SubItems.Add(ABrowser);
  SubItems.Add(ACookieText);
 end;
end;

procedure TCookiesSearch.BitBtn1Click(Sender: TObject);
var
 Lines : TStrings;
begin
 if lvFoundCookies.Selected = nil then exit;
 Lines := TStringList.Create;
 Lines.Text := lvFoundCookies.Selected.SubItems[1];
 ExecuteTxtLogView(Lines, '$AVZ1011');
 Lines.Free;
end;

function TCookiesSearch.GetUserAppDataPath: string;
var
 Reg : TRegistry;
begin
 Result := '';
 Reg := TRegistry.Create;
 Reg.RootKey := HKEY_CURRENT_USER;
 Reg.Access := KEY_READ;
 if Reg.OpenKey('Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders', false) then begin
  if Reg.ValueExists('Local AppData') then
   Result := Reg.ReadString('AppData');
  Reg.CloseKey;
 end;
end;

function TCookiesSearch.Execute: boolean;
begin
 Result := false;
 ShowModal;
 Result := ModalResult = mrOk;
end;

end.

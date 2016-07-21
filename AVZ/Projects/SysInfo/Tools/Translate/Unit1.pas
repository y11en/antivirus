unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls, DB, OracleData, Oracle, zDBFDataSet,
  INIFiles;

type
  TForm1 = class(TForm)
    Memo1: TMemo;
    IgnoreList: TMemo;
    OracleSession1: TOracleSession;
    TranslateOracleDataSet: TOracleDataSet;
    Button1: TButton;
    ZDBFDataSet1: TZDBFDataSet;
    Button2: TButton;
    ExpOracleDataSet: TOracleDataSet;
    Button3: TButton;
    ImpTranslateOracleQuery: TOracleQuery;
    Button4: TButton;
    GetTranslateOracleQuery: TOracleQuery;
    OracleQuery1: TOracleQuery;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
  private
    function TranslateSrcDir(ADirName: string): boolean;
    function TranslateSrcFile(AFileName: string): boolean;
    function TranslateDofFile(AFileName: string): boolean;
    function TranslateSrcLines(AFileName: string; Lines: TStrings;
      AID: integer): boolean;
    function TranslateDfmFile(AFileName: string): boolean;
    function TranslateDfmDir(ADirName: string): boolean;
    function TranslateDFM(Sender: TObject; PropName, InStr: string;
      var OutStr: string): boolean;
    function TranslateDFM1(Sender: TObject; PropName, InStr: string;
      var OutStr: string): boolean;
  public
   TekFileName : string;
   procedure AddStrToDB(AStr : string; ASrcFile, X1, X2 : string; ASrcId : integer);
   // Сканирование файла с исходником
   Function ScanSrcDir(ADirName : string) : boolean;
   Function ScanSrcFile(AFileName : string) : boolean;
   Function ScanSrcLines(AFileName : string; Lines : TStrings; AID : integer) : boolean;
   // Сканирование файла с исходником
   Function ScanDfmDir(ADirName : string) : boolean;
   Function ScanDfmFile(AFileName : string) : boolean;
   // Сканирование скриптов в базе
   Function ScanScripts : boolean;
  end;

var
  Form1: TForm1;

implementation
uses orautil, zDFMParser;
{$R *.dfm}

function TForm1.ScanSrcFile(AFileName: string): boolean;
var
 Lines : TStringList;
begin
 Memo1.Lines.Add('********************************'+ExtractFileName(AFileName)+'***********************************');
 Lines := TStringList.Create;
 Lines.LoadFromFile(AFileName);
 ScanSrcLines(AFileName, Lines, 1);
 Lines.Free
end;

function TForm1.TranslateSrcFile(AFileName: string): boolean;
var
 Lines : TStringList;
begin
 Memo1.Lines.Add('********************************'+ExtractFileName(AFileName)+'***********************************');
 Lines := TStringList.Create;
 Lines.LoadFromFile(AFileName);
 TranslateSrcLines(AFileName, Lines, 1);
 Lines.SaveToFile(AFileName);
 Lines.Free
end;

function TForm1.TranslateDFM(Sender: TObject; PropName, InStr: string;
  var OutStr: string): boolean;
var
 S : string;
begin
 if (pos('''', InStr) > 0) or (pos('#', InStr) > 0) then begin
  S := DFMStringToStr(InStr);
  if (UpperCase(S) = AnsiUpperCase(S)) and
     (LowerCase(S) = AnsiLowerCase(S)) then exit;
  S := StringReplace(S, #$0D, '#$0D', [rfReplaceAll]);
  S := StringReplace(S, #$0A, '#$0A', [rfReplaceAll]);
  Memo1.Lines.Add(PropName + ' = ' + S);
  AddStrToDB(Trim(S), '', TekFileName, PropName, 3);
 end;
end;

function TForm1.TranslateDFM1(Sender: TObject; PropName, InStr: string;
  var OutStr: string): boolean;
var
 S, ST : string;
begin
 Result := false;
 if (pos('''', InStr) > 0) or (pos('#', InStr) > 0) then begin
  S := DFMStringToStr(InStr);
  S := StringReplace(S, #$0D, '#$0D', [rfReplaceAll]);
  S := StringReplace(S, #$0A, '#$0A', [rfReplaceAll]);
  if (UpperCase(S) = AnsiUpperCase(S)) and
     (LowerCase(S) = AnsiLowerCase(S)) then exit;
  ST := S;
  ST  := Trim(ST);
//  Memo1.Lines.Add(PropName + ' = ' + ST);
  GetTranslateOracleQuery.SetVariable('RUS_TXT',ST);
  GetTranslateOracleQuery.Execute;
  if Not(GetTranslateOracleQuery.Eof) then begin
   if GetTranslateOracleQuery.FieldAsString('LOCAL_TXT') = '' then exit;
   S := StringReplace(S,
                     ST,
                     GetTranslateOracleQuery.FieldAsString('LOCAL_TXT'),
                     [rfReplaceAll, rfIgnoreCase]);
   if AnsiUpperCase(S) = UpperCase(S) then
    Memo1.Lines.Add('ERROR: '+PropName + ' = ' + ST);
   OutStr := StrToDFMString(S);
   Result := true;
  end else Memo1.Lines.Add('ERROR: '+PropName + ' = ' + ST);
 end;
end;

function TForm1.ScanDfmFile(AFileName: string): boolean;
var
 Lines     : TStringList;
 DFMParser : TDFMParser;
begin
 TekFileName := AFileName;
 Memo1.Lines.Add('------------->'+AFileName);
 Lines   := TStringList.Create;
 Lines.LoadFromFile(AFileName);
 DFMParser := TDFMParser.Create;
 DFMParser.LoadFromLines(Lines);
 DFMParser.Translate(TranslateDFM);
 DFMParser.Free;
 Lines.Free;
end;

function TForm1.TranslateDfmFile(AFileName: string): boolean;
var
 Lines     : TStringList;
 DFMParser : TDFMParser;
begin
 TekFileName := AFileName;
 Memo1.Lines.Add('------------->'+AFileName);
 Lines   := TStringList.Create;
 Lines.LoadFromFile(AFileName);
 DFMParser := TDFMParser.Create;
 DFMParser.LoadFromLines(Lines);
 DFMParser.Translate(TranslateDFM1);
 Lines.Clear;
 DFMParser.SaveToLines(Lines);
 Lines.SaveToFile(AFileName);
 DFMParser.Free;
 Lines.Free;
end;

procedure TForm1.AddStrToDB(AStr, ASrcFile, X1, X2: string;
  ASrcId: integer);
begin
 GetTranslateOracleQuery.SetVariable('RUS_TXT', AStr);
 GetTranslateOracleQuery.Execute;
 if not(GetTranslateOracleQuery.Eof) then exit;
 TranslateOracleDataSet.Append;
 AStr := Trim(AStr);
 AStr := StringReplace(AStr, #$0D, '#$0D', [rfReplaceAll]);
 AStr := StringReplace(AStr, #$0A, '#$0A', [rfReplaceAll]);
 AStr := StringReplace(AStr, #$09, '#$09', [rfReplaceAll]);
 TranslateOracleDataSet['RUS_TXT']  := AStr;
 TranslateOracleDataSet['SRC_CODE'] := ASrcId;
 TranslateOracleDataSet['SRC_FILE'] := ASrcFile;
 TranslateOracleDataSet['X1'] := X1;
 TranslateOracleDataSet['X2'] := X2;
end;

function TForm1.ScanSrcDir(ADirName: string): boolean;
var
 SR : TSearchRec;
 Res : integer;
begin
 Res := FindFirst(ADirName + '*.pas', faAnyFile, SR);
 while Res = 0 do begin
  if (SR.Name <> '..') and (SR.Name <> '.') and FileExists(ADirName + SR.Name) then
   try
    ScanSrcFile(ADirName + SR.Name);
   except
    on e : exception do
     Memo1.Lines.Add(e.Message);
   end;
  Res := FindNext(SR);
 end;
 FindClose(SR);
end;

function TForm1.TranslateSrcDir(ADirName: string): boolean;
var
 SR : TSearchRec;
 Res : integer;
begin
 Res := FindFirst(ADirName + '*.pas', faAnyFile, SR);
 while Res = 0 do begin
  if (SR.Name <> '..') and (SR.Name <> '.') and FileExists(ADirName + SR.Name) then
   try
    TranslateSrcFile(ADirName + SR.Name);
   except
    on e : exception do
     Memo1.Lines.Add(e.Message);
   end;
  Res := FindNext(SR);
 end;
 FindClose(SR);
end;

function TForm1.ScanDfmDir(ADirName: string): boolean;
var
 SR : TSearchRec;
 Res : integer;
begin
 Res := FindFirst(ADirName + '*.dfm', faAnyFile, SR);
 while Res = 0 do begin
  if (SR.Name <> '..') and (SR.Name <> '.') and FileExists(ADirName + SR.Name) then
   try
    ScanDfmFile(ADirName + SR.Name);
   except
    on e : exception do
     Memo1.Lines.Add('ERROR: '+e.Message);
   end;
  Res := FindNext(SR);
 end;
end;

function TForm1.TranslateDfmDir(ADirName: string): boolean;
var
 SR : TSearchRec;
 Res : integer;
begin
 Res := FindFirst(ADirName + '*.dfm', faAnyFile, SR);
 while Res = 0 do begin
  if (SR.Name <> '..') and (SR.Name <> '.') and FileExists(ADirName + SR.Name) then
   try
    TranslateDfmFile(ADirName + SR.Name);
   except
    on e : exception do
     Memo1.Lines.Add('ERROR: '+e.Message);
   end;
  Res := FindNext(SR);
 end;
end;

procedure TForm1.Button1Click(Sender: TObject);
begin
{ ScanSrcDir('E:\Delphi5\Delphi7\Projects\SysInfo\avz\');
 ScanSrcDir('E:\Delphi5\Delphi7\Projects\SysInfo\avz\Driver\avz\');
 ScanSrcDir('E:\Delphi5\Delphi7\Projects\SysInfo\avz\unpack\');
 ScanSrcDir('E:\Delphi5\Delphi7\Projects\SysInfo\avz\unpack\RAR\');
 ScanSrcDir('E:\Delphi5\Delphi7\Projects\SysInfo\avz\unpack\MAIL\');
}
// ScanDfmDir('E:\Delphi5\Delphi7\Projects\SysInfo\avz\');
//ScanSrcDir('E:\Delphi5\Delphi7\Projects\FilterControl\');
 ScanScripts;
end;

function TForm1.ScanSrcLines(AFileName: string; Lines: TStrings; AID : integer): boolean;
var
 i, j, OpenPos, Cnt : integer;
 S, ST : string;
 TranslateActive : boolean;
begin
 Cnt := 0;
 TranslateActive := true;
 for i := 0 to Lines.Count - 1 do begin
  S := Lines[i];
  OpenPos := 0;
  j := 1;
  if pos('#translate=off', LowerCase(S)) > 0 then TranslateActive := false;
  if pos('#translate=on',  LowerCase(S)) > 0 then TranslateActive := true;
  if TranslateActive and
     (pos('do not localize', LowerCase(S)) = 0) and
     (pos('#DLS', S) = 0) and
     (pos('#DNL', S) = 0) then
  while j <= length(S) do begin
   // В текущей позиции - кавычка
   if S[j] = '''' then begin
    // Это две кывычки подряд - пропускаем
    if copy(S, j+1, 1) = '''' then begin
      inc(j,2);
      Continue;
    end;
    if OpenPos > 0 then begin
     ST := Trim(copy(S, OpenPos+1, j - OpenPos-1));
     if (ST <> '') and (IgnoreList.Lines.IndexOf(ST) < 0) and (UpperCase(ST) <> AnsiUpperCase(ST)) then begin
      Memo1.Lines.Add(IntToStr(i+1) + ' "'+  ST+'"');
      AddStrToDB(ST, ExtractFileName(AFileName), inttostr(i),'',AID);
      inc(cnt);
     end;
     OpenPos := 0;
     Inc(j);
     Continue;
    end else begin
     OpenPos := j;
     inc(j);
     Continue;
    end;
   end;
   // Найден комментарий до конца строки
   if S[j] = '/' then
    if (copy(S, j, 2) = '//') and (OpenPos = 0) then Break;
   inc(j);
  end;
 end;
 Caption := inttostr(Cnt);
end;

function TForm1.TranslateSrcLines(AFileName: string; Lines: TStrings; AID : integer): boolean;
var
 i, j, OpenPos, Cnt : integer;
 S, ST, ST1 : string;
 TranslateActive : boolean;
begin
 Cnt := 0;
 TranslateActive := true;
 for i := 0 to Lines.Count - 1 do begin
  S := Lines[i];
  OpenPos := 0;
  j := 1;
  if pos('#translate=off', LowerCase(S)) > 0 then TranslateActive := false;
  if pos('#translate=on',  LowerCase(S)) > 0 then TranslateActive := true;
  if TranslateActive and
     (pos('do not localize', LowerCase(S)) = 0) and
     (pos('#DLS', S) = 0) and
     (pos('#DNL', S) = 0) then
  while j <= length(S) do begin
   // В текущей позиции - кавычка
   if S[j] = '''' then begin
    // Это две кывычки подряд - пропускаем
    if copy(S, j+1, 1) = '''' then begin
      inc(j,2);
      Continue;
    end;
    if OpenPos > 0 then begin
     ST := Trim(copy(S, OpenPos+1, j - OpenPos-1));
     if (ST <> '') and (IgnoreList.Lines.IndexOf(ST) < 0) and (UpperCase(ST) <> AnsiUpperCase(ST)) then begin
      //Memo1.Lines.Add(IntToStr(i+1) + ' "'+  ST+'"');
      // Перевод
      GetTranslateOracleQuery.SetVariable('RUS_TXT',ST);
      GetTranslateOracleQuery.Execute;
      if Not(GetTranslateOracleQuery.Eof) then begin
       ST1 := Lines[i];
       Lines[i] := StringReplace(Lines[i],
                     ST,
                     GetTranslateOracleQuery.FieldAsString('LOCAL_TXT'),
                     []);
       if Lines[i] = ST1 then
        Memo1.Lines.Add('ERR-REPLACE {'+ST +'} ['+ inttostr(i)+']');
       if UpperCase(Lines[i]) <> AnsiUpperCase(Lines[i]) then
        Memo1.Lines.Add('ERR-UNTRANSLATE {'+ST +'} ['+ inttostr(i)+']');
      end else
       Memo1.Lines.Add('ERR-SEARCH {'+ST +'} ['+ inttostr(i)+']');
      inc(cnt);
     end;
     OpenPos := 0;
     Inc(j);
     Continue;
    end else begin
     OpenPos := j;
     inc(j);
     Continue;
    end;
   end;
   // Найден комментарий до конца строки
   if S[j] = '/' then
    if (copy(S, j, 2) = '//') and (OpenPos = 0) then Break;
   inc(j);
  end;
 end;
 Caption := inttostr(Cnt);
end;

function TForm1.ScanScripts: boolean;
var
 Lines : TStringList;
begin
 Lines := TStringList.Create;
 OracleQuery1.Execute;
 while not(OracleQuery1.Eof) do begin
  Lines.Clear;
  Lines.Add(''''+OracleQuery1.Field('NAIM')+'''');
  Lines.Add('');
  Lines.Add('');
  ScanSrcLines(OracleQuery1.Field('TIP'), Lines, 4);
  Lines.Text := OracleQuery1.Field('SCRIPT')+#$0D+#$0A;
  ScanSrcLines(OracleQuery1.Field('TIP'), Lines, 5);
  OracleQuery1.Next;
 end;
 Lines.Free
end;

procedure TForm1.Button2Click(Sender: TObject);
begin
 ZDBFDataSet1.Active := false;
 ZDBFDataSet1.CreateTable('SRC_CODE NUMBER(1,0); RUS_TXT CHARACTER(240); LOCAL_TXT CHARACTER(240); PRIM CHARACTER(240)');
 ZDBFDataSet1.Active := true;
 ExpOracleDataSet.Active := false;
 ExpOracleDataSet.Active := true;
 while not(ExpOracleDataSet.Eof) do begin
  ZDBFDataSet1.Append;
  ZDBFDataSet1['SRC_CODE'] := ExpOracleDataSet['SRC_CODE'];
  ZDBFDataSet1['RUS_TXT']  := ExpOracleDataSet['RUS_TXT'];
  ZDBFDataSet1.Post;
  ExpOracleDataSet.Next;
 end;
 ExpOracleDataSet.Active := false;
end;

procedure TForm1.Button3Click(Sender: TObject);
begin
 ZDBFDataSet1.Active := false;
 ZDBFDataSet1.Active := true;
 while not(ZDBFDataSet1.Eof) do begin
  ImpTranslateOracleQuery.SetVariable('RUS_TXT', ZDBFDataSet1['RUS_TXT']);
  ImpTranslateOracleQuery.SetVariable('LOCAL_TXT', ZDBFDataSet1['LOCAL_TXT']);
  ImpTranslateOracleQuery.Execute;
  if ImpTranslateOracleQuery.RowsProcessed = 0 then begin
   Memo1.Lines.Add(ZDBFDataSet1['RUS_TXT']);
   Memo1.Lines.Add(ZDBFDataSet1['LOCAL_TXT']);
   Memo1.Lines.Add('');
  end;
  ZDBFDataSet1.Next;
 end;
 OracleSession1.Commit;
end;

procedure TForm1.Button4Click(Sender: TObject);
begin
 Memo1.Lines.Clear;
 TranslateOracleDataSet.Active := false;
 TranslateOracleDataSet.Active := true;
 TranslateSrcDir('E:\Delphi5\Delphi7\Projects\SysInfo\avz\');
 TranslateSrcDir('E:\Delphi5\Delphi7\Projects\SysInfo\avz\Driver\avz\');
 TranslateSrcDir('E:\Delphi5\Delphi7\Projects\SysInfo\avz\unpack\');
 TranslateSrcDir('E:\Delphi5\Delphi7\Projects\SysInfo\avz\unpack\RAR\');
 TranslateSrcDir('E:\Delphi5\Delphi7\Projects\SysInfo\avz\unpack\MAIL\');
 TranslateDfmDir('E:\Delphi5\Delphi7\Projects\SysInfo\avz\');
 TranslateSrcFile('E:\Delphi5\Delphi7\Projects\SysInfo\zAutoUpdate.pas');
 TranslateSrcFile('E:\Delphi5\Delphi7\Projects\SysInfo\ZSharedFunctions.pas');
 TranslateSrcFile('E:\Delphi5\Delphi7\Projects\SysInfo\zAntivirus.pas');
 TranslateSrcFile('E:\Delphi5\Delphi7\Projects\SysInfo\avz\avz.dpr');
 TranslateDofFile('E:\Delphi5\Delphi7\Projects\SysInfo\avz\avz.dof');
end;

function TForm1.TranslateDofFile(AFileName: string): boolean;
var
 INI : TIniFile;
 procedure TranslateKey(AKey, AVal : string);
 var
  S, ST : string;
 begin
  S  := INI.ReadString(AKey, AVal, '');
  ST := Trim(S);
  if (ST <> '') and (IgnoreList.Lines.IndexOf(ST) < 0) and (UpperCase(ST) <> AnsiUpperCase(ST)) then begin
   // Перевод
   GetTranslateOracleQuery.SetVariable('RUS_TXT',ST);
   GetTranslateOracleQuery.Execute;
   if Not(GetTranslateOracleQuery.Eof) then begin
    S   := StringReplace(S,
                ST,
                GetTranslateOracleQuery.FieldAsString('LOCAL_TXT'),
                []);
   end else
    Memo1.Lines.Add('ERR-SEARCH {'+ST +'}');
  end;
  INI.WriteString(AKey, AVal, S);
 end;
begin
 INI := TIniFile.Create(AFileName);
 TranslateKey('Version Info Keys', 'CompanyName');
 TranslateKey('Version Info Keys', 'FileDescription');
 TranslateKey('Version Info Keys', 'LegalCopyright');
 TranslateKey('Version Info Keys', 'InternalName');
 TranslateKey('Version Info Keys', 'ProductName');
 INI.Free;
end;

end.

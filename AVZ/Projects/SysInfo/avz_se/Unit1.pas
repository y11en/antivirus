unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, fs_synmemo, ExtCtrls, fs_tree, ComCtrls, XPMan, Menus, StdCtrls,
  Buttons, zAntivirus,
  clipbrd,
  INIFiles,
  zFileSearch, zSharedFunctions,
  fs_iinterpreter,
  fs_ipascal,
  fs_iclassesrtti,
  fs_idialogsrtti,
  fs_itools, RXCtrls,
  zAVKernel,
  zTranslate;

const
 LangEnFl : boolean = false;
 VER_NUM  = '1.20'; //#DNL
 VER_INFO : string = '$AVZ0935 '+VER_NUM+' $AVZ1304 '+
                    '25.09.2007'; //#DNL
type
  TMain = class(TForm)
    StatusBar1: TStatusBar;
    Panel1: TPanel;
    MainMenu1: TMainMenu;
    mmFile: TMenuItem;
    N2: TMenuItem;
    N3: TMenuItem;
    N4: TMenuItem;
    N6: TMenuItem;
    N7: TMenuItem;
    mmHlp: TMenuItem;
    N9: TMenuItem;
    XPManifest: TXPManifest;
    mbScriptWizard: TBitBtn;
    mbAddCmd: TBitBtn;
    Timer1: TTimer;
    mbGenerateTxt: TBitBtn;
    mmSyntax: TfsSyntaxMemo;
    N10: TMenuItem;
    OpenDialog: TOpenDialog;
    SaveDialog: TSaveDialog;
    GroupBox1: TGroupBox;
    mmHelp: TMemo;
    mbChkSyn: TBitBtn;
    Panel2: TPanel;
    sbWebHelp: TSpeedButton;
    lbCmdList: TTextListBox;
    mmEdit: TMenuItem;
    N11: TMenuItem;
    N12: TMenuItem;
    N13: TMenuItem;
    N15: TMenuItem;
    N18: TMenuItem;
    N19: TMenuItem;
    mmService: TMenuItem;
    N21: TMenuItem;
    N22: TMenuItem;
    Utythbhjdfnmntrcn1: TMenuItem;
    N14: TMenuItem;
    procedure FormCreate(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure mbScriptWizardClick(Sender: TObject);
    procedure N6Click(Sender: TObject);
    procedure N2Click(Sender: TObject);
    procedure N3Click(Sender: TObject);
    procedure N4Click(Sender: TObject);
    procedure mbAddCmdClick(Sender: TObject);
    function mmSyntaxCheckUserKeyword(AKeyword: String): Boolean;
    procedure mbChkSynClick(Sender: TObject);
    // Главный обработчик для всех "мелких" команд
    function OnScriptCallMetod(Instance: TObject; ClassType: TClass; const MethodName: String;
                             var Params: Variant): Variant;
    procedure sbWebHelpClick(Sender: TObject);
    procedure N9Click(Sender: TObject);
    procedure mbGenerateTxtClick(Sender: TObject);
    procedure N7Click(Sender: TObject);
    procedure mmSyntaxKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure lbCmdListExit(Sender: TObject);
    procedure lbCmdListKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure FormKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure N19Click(Sender: TObject);
    procedure N11Click(Sender: TObject);
    procedure N12Click(Sender: TObject);
    procedure N13Click(Sender: TObject);
    procedure N15Click(Sender: TObject);
    procedure N16Click(Sender: TObject);
    procedure N17Click(Sender: TObject);
    procedure Utythbhjdfnmntrcn1Click(Sender: TObject);
    procedure lbCmdListDblClick(Sender: TObject);
    procedure N21Click(Sender: TObject);
    procedure N14Click(Sender: TObject);
    procedure N22Click(Sender: TObject);
  private
  public
    LastInsertedCmd, HelpKeyword : string;
    VSEData : TVSEData;
    fsScriptRuntime : TfsScript;
    fsPascal        : TfsPascal;
    fsClassesRTTI   : TfsClassesRTTI;
    // Форматирование текста заданной команды
    function FormatCommandTxt(Indx: integer; AComment : boolean): string;
  end;

var
  Main: TMain;
  INIFileName : string;
implementation

uses uScriptWizard, uAddCmd, uAboutDLG, uTextVizard, uSelectTemplate, Math;

{$R *.dfm}

procedure TMain.FormCreate(Sender: TObject);
var
 INI : TIniFile;
 S, LocaleST : string;
 i : integer;
begin
 // Загружаем опции
 INI := TINIFile.Create(ChangeFileExt(Application.ExeName, '.loc'));
 // Шаг 1. Если в профиле ее нет, то загружаем из параметров (у нее приоритет над профилем)
 for i := 1 to ParamCount do
  if pos('LANG=', UpperCase(ParamStr(i))) = 1 then begin
   LocaleST := Trim(UpperCase(copy(ParamStr(i), 6, maxint)));
   Break;
  end;
 // Шаг 2. Загружаем локализацию из профиля
 if LocaleST = '' then
  LocaleST       := Trim(UpperCase(INI.ReadString('Main', 'Lang', '')));
 // Шаг 3. В профиле и параметре данных нет - берем автоматом
 if LocaleST = '' then begin
  SetLength(S, 20);
  ZeroMemory(@S[1], length(S));
  if GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_ILANGUAGE, @S[1], length(S)) > 0 then
   S := Trim(S);
  case StrToIntDef(S, 0) of
   419 : LocaleST := 'RU';
   409 : LocaleST := 'EN';
   else
    LocaleST := 'EN';
  end;
 end;
 LangLocaleFont        := Trim(INI.ReadString(LocaleST, 'Font', ''));
 LangLocaleFontCharset := INI.ReadInteger(LocaleST, 'Charset', RUSSIAN_CHARSET);
 // Установка ThreadLocale по профилю
 if INI.ReadInteger(LocaleST, 'ThreadLocale', -1) >= 0 then
  SetThreadLocale(INI.ReadInteger(LocaleST, 'ThreadLocale', LOCALE_SYSTEM_DEFAULT));
 TranslateSys  := TTranslate.Create(ExtractFilePath(Application.ExeName)+'Base\');
 TranslateSys.LoadBinDatabase(LocaleST);
 TranslateApp;
 TranslateForm(Self);
 mmHlp.Caption := TranslateStr(mmHlp.Caption);
 mmFile.Caption := TranslateStr(mmFile.Caption);
 mmService.Caption := TranslateStr(mmService.Caption);

 LastInsertedCmd := '';
 fsScriptRuntime := TfsScript.Create(nil);
 fsScriptRuntime.Parent := fsGlobalUnit;
 fsPascal        := TfsPascal.Create(nil);
 fsClassesRTTI   := TfsClassesRTTI.Create(nil);
 mmSyntax.Lines.Clear;
 VSEData := TVSEData.Create(ExtractFilePath(Application.ExeName)+'Base\');
 VSEData.LoadBinDatabase;
 mmFile.Caption := TranslateStr('$AVZ1109');
 mmEdit.Caption := TranslateStr('$AVZ1262');
 mmService.Caption := TranslateStr('$AVZ0945');
 mmHlp.Caption := TranslateStr('$AVZ1041');
  // Настройка окна для режима 640*480
 if Screen.Width < 650 then begin
  Constraints.MinWidth  := Screen.Width  - 10;
  Constraints.MinHeight := Screen.Height - 10;
 end;
 try
  if ParamCount > 0 then
   if FileExists(ParamStr(1)) then
    mmSyntax.Lines.LoadFromFile(ParamStr(1));
 except
 end;   
end;

procedure TMain.Timer1Timer(Sender: TObject);
var
 Cmd, S : string;
 i   : integer;
begin
 if mmSyntax.SelText <> '' then begin
  Cmd := Trim(mmSyntax.SelText);
  if Pos(#$0D, Cmd) > 0  then Delete(Cmd, Pos(#$0D, Cmd), maxint);
  if Pos(' ', Cmd) > 0  then Delete(Cmd, Pos(' ', Cmd), maxint);
  if Pos('(', Cmd) > 0  then Delete(Cmd, Pos('(', Cmd), maxint);
  if Pos('//', Cmd) > 0 then Delete(Cmd, Pos('//', Cmd), maxint);
  if Pos('{', Cmd) > 0 then Delete(Cmd, Pos('{', Cmd), maxint);
  if Pos(';', Cmd) > 0 then Delete(Cmd, Pos(';', Cmd), maxint);
  Cmd := Trim(LowerCase(Cmd));
  S := '';
  for i := 0 to Length(VSEData.CmdDescr) - 1 do
   if Cmd = VSEData.CmdDescr[i].Naim then begin
     S := VSEData.CmdDescr[i].CmdTxt + #$0D#$0A + VSEData.CmdDescr[i].Cmt;
     if VSEData.CmdDescr[i].AVZVer > 0 then
      S := S + #$0D#$0A + '$AVZ1295 '+FormatFloat('0.00', VSEData.CmdDescr[i].AVZVer);
     Break;
   end;
  if S = '' then begin
   mmHelp.Lines.Text := TranslateStr('"'+Cmd + '" - '+'$AVZ1305');
   HelpKeyword := '';
   sbWebHelp.Enabled := false;
  end else begin
   mmHelp.Lines.Text := TranslateStr(S);
   HelpKeyword := Cmd;
   sbWebHelp.Enabled := true;
  end;
 end else begin
  HelpKeyword := '';
  sbWebHelp.Enabled := false;
  mmHelp.Lines.Text := TranslateStr('$AVZ1296');
 end;
end;

procedure TMain.mbScriptWizardClick(Sender: TObject);
var
 Lines : TStringList;
 S : string;
begin
 if ScriptWizard.Execute then begin
  Lines := TStringList.Create;
  mmSyntax.Lines.Clear;
  ScriptWizard.GenerateScript(Lines);
  S := Trim(Lines.Text);
  mmSyntax.Lines.Clear;
  mmSyntax.Lines.Text := S;
  Lines.Free;
 end;
end;

procedure TMain.N6Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ1297', mtConfirmation, [mbYes, mbNo], 0) = mrYes then
  Application.Terminate;
end;

procedure TMain.N2Click(Sender: TObject);
begin
 mmSyntax.Lines.Clear;
end;

procedure TMain.N3Click(Sender: TObject);
begin
 if OpenDialog.Execute and (OpenDialog.FileName <> '') then
  mmSyntax.Lines.LoadFromFile(OpenDialog.FileName);
end;

procedure TMain.N4Click(Sender: TObject);
begin
 if SaveDialog.Execute and (SaveDialog.FileName <> '') then
  mmSyntax.Lines.SaveToFile(SaveDialog.FileName);
end;

procedure TMain.mbAddCmdClick(Sender: TObject);
begin
 if AddCmdDLG.Execute then begin
  LastInsertedCmd  := AddCmdDLG.CommandTxt;
  mmSyntax.SelText := LastInsertedCmd;
 end;
end;

function TMain.mmSyntaxCheckUserKeyword(AKeyword: String): Boolean;
var
 i : integer;
begin
 Result := false;
 AKeyword := LowerCase(AKeyword);
 for i := 0 to Length(VSEData.CmdDescr) - 1 do
  if AKeyword = VSEData.CmdDescr[i].Naim then begin
    Result := true;
    exit;
  end
end;

procedure TMain.mbChkSynClick(Sender: TObject);
var
 i : integer;
 S : string;
 P : Tpoint;
begin
   // Подготовка текста скрипта
   fsScriptRuntime.Clear;
   fsScriptRuntime.Lines.Text := mmSyntax.Lines.Text;
   // Константа с именем скрипта
   fsScriptRuntime.AddConst('AScriptName', 'string', '$AVZ1298');
   // Декларация типа DWord
   fsScriptRuntime.AddType('DWord', fvtVariant);
   fsScriptRuntime.AddConst('AFileName', 'string', '$AVZ0307');
   // Класс для поиска файлов и папок
   with fsScriptRuntime.AddClass(TFileSearch, 'TComponent') do begin
    AddMethod('function FindFirst(AFileMask : string) : boolean', OnScriptCallMetod);
    AddMethod('function FindNext : boolean', OnScriptCallMetod);
    AddMethod('function FindClose : boolean', OnScriptCallMetod);
   end;
 // Декларирование функций
 for i := 0 to Length(VSEData.CmdDescr) - 1 do
  if VSEData.CmdDescr[i].PrgType in [0,1] then begin
   S := Trim(VSEData.CmdDescr[i].CmdTxt);
   if copy(S, length(S), 1) = ';' then
    Delete(S,length(S), 1);
   fsScriptRuntime.AddMethod(S, OnScriptCallMetod);
  end;
 // Контроль
 if fsScriptRuntime.Compile then begin
  zMessageDlg('$AVZ1299', mtInformation, [mbOk], 0);
 end else begin
  p := fsPosToPoint(fsScriptRuntime.ErrorPos);
  mmSyntax.SetPos(p.X, p.Y);
  zMessageDlg('$AVZ0630: "'+fsScriptRuntime.ErrorMsg+'"', mtError, [mbOk], 0);
  SetFocusedControl(mmSyntax);
 end;
end;

function TMain.OnScriptCallMetod(Instance: TObject; ClassType: TClass;
  const MethodName: String; var Params: Variant): Variant;
begin
 ;
end;

procedure TMain.sbWebHelpClick(Sender: TObject);
begin
 if HelpKeyword <> '' then
  WebOpenURLinBrowser('http://z-oleg.com/secur/avz_doc/index.html?script_'+HelpKeyword+'.htm')
end;

procedure TMain.N9Click(Sender: TObject);
begin
 ExecuteAboutDLG;
end;

procedure TMain.mbGenerateTxtClick(Sender: TObject);
begin
 TextWizard.Execute(mmSyntax.Lines.Text);
end;

procedure TMain.N7Click(Sender: TObject);
var
 i, x : integer;
 S : string;
begin
 if SelectTemplateDLG.Execute then begin
  mmSyntax.Lines.Clear;
  mmSyntax.Lines.Text := Trim(SelectTemplateDLG.Script);
  for i := 0 to mmSyntax.Lines.Count - 1 do begin
   X := pos('|', mmSyntax.Lines[i]);
   if Trim(mmSyntax.Lines[i]) = '|' then begin
    S :=  mmSyntax.Lines[i];
    S[x] := ' ';
    mmSyntax.Lines[i] := S;
    mmSyntax.SetPos(X, i+1);
    break;
   end;
  end;
 end;
 mmSyntax.SetFocus;
end;

procedure TMain.mmSyntaxKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
var
 Poz, Off : TPoint;
 i        : integer;
 S        : string;
begin
 // Реакция на Ctrl + ***
 if ssCtrl in Shift then begin
  // Реакция на Ctrl + Space
  if Key = VK_SPACE then begin
   Off := mmSyntax.GetOffset;
   Poz := mmSyntax.GetPos;
   lbCmdList.Clear;
   lbCmdList.Sorted := true;
   for i := 0 to Length(Main.VSEData.CmdDescr) - 1 do begin
    S := Main.VSEData.CmdDescr[i].CmdTxt;
    S := StringReplace(S, 'procedure ', '', [rfIgnoreCase]);
    S := StringReplace(S, 'function ', '', [rfIgnoreCase]);
    lbCmdList.Items.AddObject(S {+ ' - ' +  TranslateMain.VSEData.CmdDescr[i].Cmt},
                                   pointer(i));
   end;
   lbCmdList.Sorted := true;
   // Отображение списка
   lbCmdList.Visible := true;
   // Передача ему фокуса
   lbCmdList.SetFocus;
   // Перерисовка списка
   lbCmdList.Refresh;
   Application.ProcessMessages;
   Key := 0;
  end;
  // Реакция на Ctrl + Enter
  if Key = VK_RETURN then begin
   mbAddCmd.Click;
   Key := 0;
  end;
 end;
end;

procedure TMain.lbCmdListExit(Sender: TObject);
begin
 lbCmdList.Visible := false;
end;

function TMain.FormatCommandTxt(Indx : integer; AComment : boolean): string;
var
 S, ST, ParamList, ParamMask, ParamDef : string;
begin
 Result := '';
 if Indx < 0 then exit;
 // Текст команды
 ST := Main.VSEData.CmdDescr[Indx].CmdTxt;
 // Удаление профикса и пробелов
 if pos('function',  LowerCase(ST)) = 1 then  Delete(ST, 1, 8);
 if pos('procedure', LowerCase(ST)) = 1 then  Delete(ST, 1, 9);
 ST := Trim(ST);
 // Выделение списка параметров
 ParamList := Trim(copy(ST, pos('(',  ST)+1, pos(')',  ST) - pos('(',  ST)-1));
 ParamList := StringReplace(ParamList, ' ', '', [rfReplaceAll]);
 ParamMask := '';
 // Параметры есть - тогда парсинг
 if ParamList <> '' then begin
  ParamList := ParamList + ';';
  // Поиск групп параметров одного типа
  while pos(';', ParamList) > 0 do begin
   ParamDef := copy(ParamList, 1, pos(';', ParamList)-1);
   // Группа найдена, анализ
   if ParamDef <> '' then begin
    ParamDef := ParamDef + ',';
    S := '';
    // Анализ парамтеров в группе
    while pos(',', ParamDef) > 0 do begin
     if Pos(':string', ParamDef) > 0 then
      S := ''' '''
       else S := ' ';
     if ParamMask <> '' then ParamMask := ParamMask + ',';
     ParamMask := ParamMask + S;
    Delete(ParamDef, 1, pos(',', ParamDef));
    end;
   end;
   Delete(ParamList, 1, pos(';', ParamList));
  end;
 end;
 if pos('(',  ST) > 0 then  Delete(ST, pos('(',  ST), maxint);
 if pos(';',  ST) > 0 then  Delete(ST, pos(';',  ST), maxint);
 if pos(':',  ST) > 0 then  Delete(ST, pos(':',  ST), maxint);
 if ParamMask <> '' then
  Result := Trim(ST)+'('+ParamMask+');'
   else Result := Trim(ST)+';';
 if AComment then begin
  S := Trim(TranslateStr(Main.VSEData.CmdDescr[Indx].ScriptCmt));
  if S = '*' then
   S := Trim(TranslateStr(Main.VSEData.CmdDescr[Indx].Cmt));
  if S <> '' then
   Result := '//'+S + #$0D#$0A+' '+ Result;
 end;
 Result := #$0D#$0A+' '+ Result;
end;

procedure TMain.lbCmdListKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
 if Key = VK_RETURN then begin
  lbCmdList.Visible := false;
  try
   LastInsertedCmd  := Trim(FormatCommandTxt(integer(lbCmdList.Items.Objects[lbCmdList.ItemIndex]), false));
   Clipboard.AsText := LastInsertedCmd;
  except
  end;
  mmSyntax.PasteFromClipboard;
  mmSyntax.SetFocus;
 end;
 if Key = VK_ESCAPE then begin
  lbCmdList.Visible := false;
  mmSyntax.SetFocus;
 end;
end;

procedure TMain.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
 if Key = VK_F9 then
  mbChkSyn.Click;
 if Key = VK_Return then
  if ssAlt in Shift then
   if LastInsertedCmd <> '' then
    mmSyntax.SelText := LastInsertedCmd;
end;

procedure TMain.N19Click(Sender: TObject);
begin
 mmSyntax.Undo;
end;

procedure TMain.N11Click(Sender: TObject);
begin
 mmSyntax.CopyToClipboard;
end;

procedure TMain.N12Click(Sender: TObject);
begin
 mmSyntax.CutToClipboard;
end;

procedure TMain.N13Click(Sender: TObject);
begin
 mmSyntax.PasteFromClipboard;
end;

procedure TMain.N15Click(Sender: TObject);
begin
 mmSyntax.SelectAll;
end;

procedure TMain.N16Click(Sender: TObject);
begin
 mbAddCmd.Click;
end;

procedure TMain.N17Click(Sender: TObject);
begin
 mbChkSyn.Click;
end;

procedure TMain.Utythbhjdfnmntrcn1Click(Sender: TObject);
begin
 mbGenerateTxt.Click;
end;

procedure TMain.lbCmdListDblClick(Sender: TObject);
var
 Key : word;
begin
 // Эмуляция Enter по двойному клику
 Key := VK_RETURN;
 lbCmdListKeyDown(lbCmdList, Key, []);
end;

procedure TMain.N21Click(Sender: TObject);
begin
 mbAddCmd.Click;
end;

procedure TMain.N14Click(Sender: TObject);
begin
 mbScriptWizard.Click;
end;

procedure TMain.N22Click(Sender: TObject);
begin
 mbChkSyn.Click;
end;

begin
 INIFileName := ChangeFileExt(Application.ExeName, '.ini');
end.

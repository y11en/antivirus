unit uAutoUpdate;
// ****************** Автоматическое обновление **********************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, StdCtrls, Buttons, ComCtrls, zAutoUpdate, FileLoader,
  Mask, ToolEdit, INIFiles, zAVZDriverRK, zAVKernel, zAVZVersion, ZLogSystem;

type
  TAutoUpdate = class(TForm)
    lbStep1: TLabel;
    lbStep2: TLabel;
    lbStep3: TLabel;
    lbStep4: TLabel;
    lbCurrentFile: TLabel;
    Label1: TLabel;
    pbCurrentOp: TProgressBar;
    pbFullProgress: TProgressBar;
    Timer1: TTimer;
    Label2: TLabel;
    cbSourceURL: TComboBox;
    Panel1: TPanel;
    mbStart: TBitBtn;
    mbExit: TBitBtn;
    Label3: TLabel;
    ceSetup: TComboEdit;
    lbAnalizResult: TLabel;
    procedure mbStartClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure ceSetupButtonClick(Sender: TObject);
  private
    // Настойка текущего шага - отображается визуально на окне
    procedure SetCurrentStep(const Value: integer);
    procedure OnStep(AStep : integer);
    procedure OnFileOperation(AFileName : string; ATek, AMax : integer);
    procedure OnAnalizComplete(DownloadSize : integer);
  public
   FCurrentStep : integer;
   Mode : byte; // Код режима  (0...4, см. расшифровку в GetSetupDescr)
   Proxy, ProxyUser, ProxyPass : string; // Парамеры связи с прокси
   // Загрузка настроек
   Function LoadSetup : boolean;
   // Выполнение диалога
   Function Execute : boolean;
   // Обновление строки с описанием источника обновления
   Function RefreshSetupStr : boolean;
   // Выполнение обновления (NoGUI - перекл. невизуального режима)
   Function ExecuteUpdate(NoGUI : boolean; ServerPath : string; ConnectMode : byte; ProxyServer, ProxyUser, ProxyPass : string) : boolean;
  end;

var
  AutoUpdate: TAutoUpdate;
 // Вызов в диалоговом режиме
 function ExecuteAutoUpdate : boolean;
 // Вызов в автоматическом режиме
 function ExecuteAutoUpdateNoGUI(ASrcCode : integer; ServerPath : string; ConnectMode : byte; ProxyServer, ProxyUser, ProxyPass : string) : boolean;
 // Запрос описания настроек в текстовом виде человекочитаемом (для отображения на окне и логе)
 function GetSetupDescr(ConnectMode : byte; ProxyServer, ProxyUser, ProxyPass : string) : string;
implementation
uses zutil, uAutoUpdateSetup, zTranslate;
{$R *.dfm}

function GetSetupDescr(ConnectMode : byte; ProxyServer, ProxyUser, ProxyPass : string) : string;
begin
 Result := '$AVZ0512';
 case ConnectMode of
  0 : Result := '$AVZ0319';
  1 : Result := '$AVZ0880';
  2 : Result := '$AVZ1140'    + ' "' + ProxyServer +'"';
  3 : Result := '$AVZ1142'       + ' "' + ProxyServer +', '+'$AVZ0781'+'="' + ProxyUser + '"';
  4 : Result := '$AVZ1141'  + ' "' + ProxyServer +', '+'$AVZ0781'+'="' + ProxyUser + '"';
 end;
 Result := TranslateStr(Result);
end;

function ExecuteAutoUpdateNoGUI(ASrcCode : integer; ServerPath : string; ConnectMode : byte; ProxyServer, ProxyUser, ProxyPass : string) : boolean;
begin
 try
  AutoUpdate := nil;
  AutoUpdate := TAutoUpdate.Create(nil);
  Result     := AutoUpdate.ExecuteUpdate(true, ServerPath, ConnectMode, ProxyServer, ProxyUser, ProxyPass);
 finally
  AutoUpdate.Free;
  AutoUpdate := nil;
 end;
end;

function ExecuteAutoUpdate : boolean;
begin
 try
  AutoUpdate := nil;
  AutoUpdate := TAutoUpdate.Create(nil);
  Result := AutoUpdate.Execute;
 finally
  AutoUpdate.Free;
  AutoUpdate := nil;
 end;
end;


procedure TAutoUpdate.SetCurrentStep(const Value: integer);
var
 i : integer;
begin
 FCurrentStep := Value;
 pbFullProgress.Position := Value;
 Application.ProcessMessages;
 for i := 1 to ComponentCount-1 do
  if (Components[i] is TLabel) then
   if (Components[i] as TLabel).Tag > 0 then
    if (Components[i] as TLabel).Tag = FCurrentStep then
     (Components[i] as TLabel).Font.Style := [fsBold]
      else (Components[i] as TLabel).Font.Style := [];
  Application.ProcessMessages;
  Application.ProcessMessages;
  Application.ProcessMessages;
  Application.ProcessMessages;
end;

function TAutoUpdate.Execute: boolean;
begin
 lbAnalizResult.Caption := '';
 LoadSetup;
 RefreshSetupStr;
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TAutoUpdate.mbStartClick(Sender: TObject);
begin
 try
  mbStart.Enabled := false;
  mbExit.Enabled := false;
  if ExecuteUpdate(false, '', Mode, Proxy, ProxyUser, ProxyPass) then ModalResult := mrOk;
 finally
  mbStart.Enabled := true;
  mbExit.Enabled  := true;
 end;
end;

procedure TAutoUpdate.OnStep(AStep: integer);
begin
 SetCurrentStep(AStep);
end;

procedure TAutoUpdate.OnFileOperation(AFileName: string; ATek,
  AMax: integer);
begin
 lbCurrentFile.Caption := AFileName;
 pbCurrentOp.Max       := AMax;
 pbCurrentOp.Position  := ATek;
 Application.ProcessMessages;
end;

procedure TAutoUpdate.FormCreate(Sender: TObject);
begin
 Randomize;
 cbSourceURL.ItemIndex := 0;
 if cbSourceURL.Items.Count > 1 then
  if random(20) > 17 then
   cbSourceURL.ItemIndex := 1;
 Mode := 0;
 Proxy := '';
 ProxyUser := '';
 ProxyPass := '';
 TranslateForm(Self);
end;

function TAutoUpdate.ExecuteUpdate(NoGUI : boolean; ServerPath : string; ConnectMode : byte; ProxyServer, ProxyUser, ProxyPass : string): boolean;
var
 AVAAutoUpdater : TAVZAutoUpdater;
 S              : string;
begin
  lbAnalizResult.Caption := '';
  Result := false;
  ServerPath := trim(ServerPath);
  AVAAutoUpdater := TAVZAutoUpdater.Create;
  AVAAutoUpdater.ServerPath := cbSourceURL.Text;
  // Путь к серверу (URL)
  if ServerPath <> '' then
   AVAAutoUpdater.ServerPath := ServerPath;
  // Режим работы
  case ConnectMode of
   0 : AVAAutoUpdater.HTTPDownloadMode := hdmIESettings;
   1 : AVAAutoUpdater.HTTPDownloadMode := hdmDirectConnection;
   2 : begin
        AVAAutoUpdater.HTTPDownloadMode := hdmProxy;
        AVAAutoUpdater.ProxyServer := ProxyServer;
       end;
   3 : begin
        AVAAutoUpdater.HTTPDownloadMode := hdmProxyAuth;
        AVAAutoUpdater.ProxyServer := ProxyServer;
        AVAAutoUpdater.ProxyUser   := ProxyUser;
        AVAAutoUpdater.ProxyPass   := ProxyPass;
       end;
   4 : begin
        AVAAutoUpdater.HTTPDownloadMode := hdmProxyAuthNTLM;
        AVAAutoUpdater.ProxyServer := ProxyServer;
        AVAAutoUpdater.ProxyUser   := ProxyUser;
        AVAAutoUpdater.ProxyPass   := ProxyPass;
       end;
   else exit;
  end;
  AVAAutoUpdater.TempPath   := AVZTempDirectoryPath+'AVZ\';
  AVAAutoUpdater.DestPath   := ExtractFilePath(Application.ExeName);
  AVAAutoUpdater.OnStep     := OnStep;
  AVAAutoUpdater.OnFileOperation := OnFileOperation;
  AVAAutoUpdater.OnAnalizComplete := OnAnalizComplete;
  AVAAutoUpdater.LocalVer   := StrToFloatDef(VER_NUM, 0);
  // Выполнение обновления
  Result := AVAAutoUpdater.Execute;
  // Вывод сообщения
  if not(NoGUI) then
   if Result then begin
    if AVAAutoUpdater.ResStatus = 1 then S := '$AVZ0552'
     else S := '$AVZ0553';
    if AVAAutoUpdater.CurrentVer > AVAAutoUpdater.LocalVer then
     S := S + ' '#$0D#$0A + '$AVZ0168 '+FloatToStr(AVAAutoUpdater.CurrentVer);
    zMessageDlg('$AVZ0049. '+S, mtInformation, [mbOk], 0)
   end else zMessageDlg('$AVZ0644 '+AVAAutoUpdater.LastError, mtError, [mbOk], 0);
  // Обновление драйвера PM (при условии, что он загружен)
  if Result and AVZDriverRK.Loaded and (AVAAutoUpdater.ResStatus <> 1) then begin
   // Перезагрузка базы
   KMBase.LoadBinDatabase;
   // Реинсталляция драйвера
   AVZDriverRK.InstallDriver;
  end;
  pbCurrentOp.Position    := 0;
  pbFullProgress.Position := 0;
  lbCurrentFile.Caption   := '';
  SetCurrentStep(-1);
  AVAAutoUpdater.Free;
end;

procedure TAutoUpdate.ceSetupButtonClick(Sender: TObject);
begin
 if ExecuteAutoUpdateSetup(Mode, Proxy, ProxyUser, ProxyPass) then
  RefreshSetupStr;
end;

function TAutoUpdate.RefreshSetupStr: boolean;
begin
 ceSetup.Text := GetSetupDescr(Mode, Proxy, ProxyUser, ProxyPass);
end;

procedure TAutoUpdate.OnAnalizComplete(DownloadSize: integer);
begin
 lbAnalizResult.Caption := TranslateStr('$AVZ1069 ') + FormatFloat('0.0',DownloadSize/1024)+TranslateStr(' $AVZ1191)');
 lbAnalizResult.Refresh;
 lbAnalizResult.Repaint;
 Application.ProcessMessages;
end;

function TAutoUpdate.LoadSetup: boolean;
var
 INI : TIniFile;
begin
 Result := false;
 INI := TIniFile.Create(ChangeFileExt(Application.ExeName, '.ini'));
 Mode := Byte(INI.ReadInteger('Update', 'Mode', 0));
 if Mode > 4 then Mode := 0;
 Proxy     := INI.ReadString('Update', 'Proxy', '');
 ProxyUser := INI.ReadString('Update', 'ProxyUser', '');
 ProxyPass := DeCryptPwd(INI.ReadString('Update', 'ProxyPass', ''));
 INI.Free;
 Result := true;
end;

end.

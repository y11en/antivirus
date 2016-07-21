unit uAutoUpdateSetup;
// ******************* настройка апдейтера баз ***********************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls, Buttons, INIFiles, zAutoUpdate;

type
  TAutoUpdateSetup = class(TForm)
    rgMode: TRadioGroup;
    GroupBox1: TGroupBox;
    lbProxy: TLabel;
    edProxy: TEdit;
    lbProxyUser: TLabel;
    lbProxyPasswd: TLabel;
    edProxyUser: TEdit;
    edProxyPasswd: TEdit;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    cbSaveSetup: TCheckBox;
    procedure rgModeClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  public
   Mode : byte;  // Код режима  (0...4, см. расшифровку в GetSetupDescr)
   Proxy, ProxyUser, ProxyPass : string; // Параметры прокси
   function SaveSetup : boolean;
   // Настройка интерфейса
   procedure SetupInterface;
   // Выполнение диалога
   function Execute : boolean;
  end;

var
  AutoUpdateSetup: TAutoUpdateSetup;

// Отображение диалогового окна настройки
function ExecuteAutoUpdateSetup(var AMode : byte; var AProxy, AproxyUser, AProxyPass : string) : boolean;
implementation
uses zTranslate;
{$R *.dfm}

{ TAutoUpdateSetup }

function ExecuteAutoUpdateSetup(var AMode : byte; var AProxy, AproxyUser, AProxyPass : string) : boolean;
begin
 try
  AutoUpdateSetup := nil;
  AutoUpdateSetup := TAutoUpdateSetup.Create(nil);
  AutoUpdateSetup.Mode      := AMode;
  AutoUpdateSetup.Proxy     := AProxy;
  AutoUpdateSetup.ProxyUser := AproxyUser;
  AutoUpdateSetup.ProxyPass := AProxyPass;
  Result := AutoUpdateSetup.Execute;
  if Result then begin
   AMode      := AutoUpdateSetup.Mode;
   AProxy     := AutoUpdateSetup.Proxy;
   AproxyUser := AutoUpdateSetup.ProxyUser;
   AProxyPass := AutoUpdateSetup.ProxyPass;
  end;
 finally
  AutoUpdateSetup.Free;
  AutoUpdateSetup := nil;
 end;
end;

function TAutoUpdateSetup.Execute: boolean;
begin
 rgMode.ItemIndex   := Mode;
 edProxy.Text       := Proxy;
 edProxyUser.Text   := ProxyUser;
 edProxyPasswd.Text := ProxyPass;
 SetupInterface;
 ShowModal;
 Result := ModalResult = mrOk;
 if Result then begin
  Mode  := rgMode.ItemIndex;
  Proxy := edProxy.Text;
  ProxyUser := edProxyUser.Text;
  ProxyPass := edProxyPasswd.Text;
  if cbSaveSetup.Checked then
   SaveSetup;
 end;
end;

procedure TAutoUpdateSetup.SetupInterface;
begin
 edProxy.Enabled       := rgMode.ItemIndex in [2,3,4];
 lbProxy.Enabled       := edProxy.Enabled;
 edProxyUser.Enabled   := rgMode.ItemIndex in [3,4];
 lbProxyUser.Enabled   := edProxyUser.Enabled;
 edProxyPasswd.Enabled := rgMode.ItemIndex in [3,4];
 lbProxyPasswd.Enabled := edProxyPasswd.Enabled;
end;

procedure TAutoUpdateSetup.rgModeClick(Sender: TObject);
begin
 SetupInterface;
end;

function TAutoUpdateSetup.SaveSetup: boolean;
var
 INI : TIniFile;
begin
 Result := false;
 INI := TIniFile.Create(ChangeFileExt(Application.ExeName, '.ini'));
 INI.WriteInteger('Update', 'Mode', Mode);
 INI.WriteString('Update', 'Proxy', Proxy);
 INI.WriteString('Update', 'ProxyUser', ProxyUser);
 INI.WriteString('Update', 'ProxyPass', CryptPwd(ProxyPass));
 INI.Free;
 Result := true;
end;

procedure TAutoUpdateSetup.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.

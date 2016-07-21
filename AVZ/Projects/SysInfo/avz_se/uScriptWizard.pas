unit uScriptWizard;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ExtCtrls, zTranslate;

type
  TScriptWizard = class(TForm)
    GroupBox1: TGroupBox;
    cbBCImportDeletedList: TCheckBox;
    cbBCActivate: TCheckBox;
    cbBCLog: TCheckBox;
    cbBCImportQuarantineList: TCheckBox;
    GroupBox2: TGroupBox;
    cbRootkit: TCheckBox;
    cbAVZGuard: TCheckBox;
    cbExecuteSysClean: TCheckBox;
    cbReboot: TCheckBox;
    cbSaveLog: TCheckBox;
    GroupBox3: TGroupBox;
    cbDeleteFile: TCheckBox;
    cbDelByCLSID: TCheckBox;
    cbDelDriver: TCheckBox;
    cbLSPFix: TCheckBox;
    Panel1: TPanel;
    BitBtn1: TBitBtn;
    GroupBox4: TGroupBox;
    cbAutoquarantine: TCheckBox;
    cbClearQuarantine: TCheckBox;
    cbQuarantineFile: TCheckBox;
    cbCreateQuarantineArchive: TCheckBox;
    cbAVUpdate: TCheckBox;
    cbDelWinlogon: TCheckBox;
    cbClearHosts: TCheckBox;
    cbAddCmt: TCheckBox;
    BitBtn2: TBitBtn;
    cbQrFile: TCheckBox;
    bcBCDeleteFile: TCheckBox;
    cbBCQrSvc: TCheckBox;
    cbBCDeleteSvc: TCheckBox;
    cbTerminateProcess: TCheckBox;
    procedure BitBtn1Click(Sender: TObject);
    procedure cbRootkitClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  public
   function Execute : boolean;
   function GetComments(ACmd : string) : string;
   function GenerateScript(ALines : TStrings) : boolean;
  end;

var
  ScriptWizard: TScriptWizard;

implementation

uses Unit1, zAntivirus;

{$R *.dfm}

{ TScriptWizard }

function TScriptWizard.Execute: boolean;
begin
 ShowModal;
 Result := ModalResult = mrOk;
end;

function TScriptWizard.GenerateScript(ALines: TStrings): boolean;
var
 RebootNeedF : boolean;
 ST : string;
 procedure AddOp(S : string; ALevel : integer = 1);
 var
  Cmt : string;
 begin
  ST := copy('                        ', 1, ALevel);
  if cbAddCmt.Checked then begin
   Cmt := GetComments(S);
   if Cmt <> '' then
    ALines.Add(ST+'// '+Cmt);
  end;
  ALines.Add(ST+S);
 end;
begin
 RebootNeedF := false;
 ALines.Clear;
 AddOp('begin', 0);
 // Очистка карантина
 if cbClearQuarantine.Checked then
  AddOp('ClearQuarantine;');
 // Антируткит
 if cbRootkit.Checked then begin
  AddOp('SearchRootkit(true, true);');
  RebootNeedF := true;
 end;
 // AVZGuard
 if cbAVZGuard.Checked then begin
  AddOp('SetAVZGuardStatus(true);');
  RebootNeedF := true;
 end;

 // Обновление баз
 if cbAVUpdate.Checked then
  AddOp('ExecuteAVUpdate ;');

 // Карантин файла
 if cbQuarantineFile.Checked then
  AddOp('$AVZ1300');
 // Автокарантин
 if cbAutoquarantine.Checked then
  AddOp('ExecuteAutoQuarantine;');
 // Остановка процесса по имени файла
 if cbTerminateProcess.Checked then
  AddOp('$AVZ1301');
 // Удаление файла
 if cbDeleteFile.Checked then
  AddOp('$AVZ1302');

 // Устранение ошибок SPI
 if cbLSPFix.Checked then
  AddOp('AutoFixSPI;');
 if cbClearHosts.Checked then
  AddOp('ClearHostsFile;');
 if cbDelByCLSID.Checked then
  AddOp('DelCLSID('''');');
 if cbDelDriver.Checked then
  AddOp('DeleteService('''', true);');
 if cbDelWinlogon.Checked then
  AddOp('DelWinlogonNotifyByFileName('''');');

  // BC - протоколирование
 if cbBCLog.Checked then
  AddOp('BC_LogFile(GetAVZDirectory + ''boot_clr.log'');');
 // BC - испорт списка удаленных
 if cbBCImportDeletedList.Checked then
  AddOp('BC_ImportDeletedList;');
 // BC - испорт списка удаленных
 if cbBCImportQuarantineList.Checked then
  AddOp('BC_ImportQuarantineList;');
 // Карантин файла
 if cbQrFile.Checked then
  AddOp('BC_QrFile('''');');
 if bcBCDeleteFile.Checked then
  AddOp('BC_DeleteFile('''');');
 if cbBCQrSvc.Checked then
  AddOp('BC_QrSvc('''');');
 if cbBCDeleteSvc.Checked then
  AddOp('BC_DeleteSvc('''');');
 // BCActivate
 if cbBCActivate.Checked then begin
  AddOp('BC_Activate;');
  RebootNeedF := true;
 end;

 // ExecuteSysClean
 if cbExecuteSysClean.Checked then
  AddOp('ExecuteSysClean;');
 // Создание архива с содержимым карантина
 if cbCreateQuarantineArchive.Checked then
  AddOp('CreateQurantineArchive(GetAVZDirectory+''quarantine.zip'');');
 // Сохранение протокола
 if cbSaveLog.Checked then
  AddOp('SaveLog(GetAVZDirectory+''avzlog.txt'');');

 // Перезагрузка - должна стоять последней в скрипте
 if cbReboot.Checked then begin
  AddOp('RebootWindows(true);');
  RebootNeedF := false;
 end;
 AddOp('end.', 0);
end;

function TScriptWizard.GetComments(ACmd: string): string;
var
 i : integer;
begin
 Result := '';
 ACmd := Trim(LowerCase(ACmd));
 if pos('(', ACmd) > 0 then Delete(ACmd, pos('(', ACmd), maxint);
 if pos('//', ACmd) > 0 then Delete(ACmd, pos('//', ACmd), maxint);
 if pos(';', ACmd) > 0 then Delete(ACmd, pos(';', ACmd), maxint);
 if pos(' ', ACmd) > 0 then Delete(ACmd, pos(' ', ACmd), maxint);
 if ACmd = '' then exit;
  for i := 0 to Length(Main.VSEData.CmdDescr) - 1 do
   if ACmd = Main.VSEData.CmdDescr[i].Naim then begin
     Result := Trim(TranslateStr(Main.VSEData.CmdDescr[i].ScriptCmt));
     if Result = '*' then
      Result := Trim(TranslateStr(Main.VSEData.CmdDescr[i].Cmt));
     Break;
   end;
end;

procedure TScriptWizard.BitBtn1Click(Sender: TObject);
begin
 ModalResult := mrOk;
end;

procedure TScriptWizard.cbRootkitClick(Sender: TObject);
begin
 if cbReboot.Checked then exit;
 if (Sender as TCheckBox).Checked then
  if zMessageDlg('$AVZ1303', mtWarning, [mbYes, mbNo], 0) = mrYes then
   cbReboot.Checked := true;
end;

procedure TScriptWizard.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.

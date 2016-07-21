unit uAutoQurantine;
// Автоматический карантин
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ExtCtrls, ComCtrls, uActiveSetup;

type
  TAutoQurantine = class(TForm)
    GroupBox1: TGroupBox;
    cbProcess: TCheckBox;
    cbProcessLib: TCheckBox;
    cbKernelModules: TCheckBox;
    cbAutorun: TCheckBox;
    cbSPI: TCheckBox;
    cbService: TCheckBox;
    cbIEExt: TCheckBox;
    cbOpenPorts: TCheckBox;
    cbExplorerExt: TCheckBox;
    Panel1: TPanel;
    mbStart: TBitBtn;
    cbPrintExt: TCheckBox;
    BitBtn1: TBitBtn;
    cbTaskScheduler: TCheckBox;
    ProgressBar: TProgressBar;
    cbProtocols: TCheckBox;
    cbServiceMode: TComboBox;
    cbDPF: TCheckBox;
    cbCPL: TCheckBox;
    cbAS: TCheckBox;
    procedure mbStartClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  public
   function Execute : boolean;
  end;

var
  AutoQurantine: TAutoQurantine;
  // Выполнение автокарантина в диалоговом режиме
  function ExecuteAutoQurantine : boolean;
  // Выполнение автокарантина в автоматическом режиме - для скриптов
  function ExecuteAutoQurantineAuto : boolean;
implementation

uses uProcessView, zutil, ntdll, uKernelObjectsView, uServiceView,
  uAutoruns, uSPIView, uBHOManager, uOpenPorts, uExplorerExtManager,
  uPrintExtManager, uTaskScheduler, uProtocolManager, uDPFManager,
  uCPLManager, zLogSystem, zTranslate;

{$R *.dfm}

function ExecuteAutoQurantine : boolean;
begin
 AutoQurantine := nil;
 try
  AutoQurantine := TAutoQurantine.Create(nil);
  AutoQurantine.Execute;
 finally
  AutoQurantine.Free;
  AutoQurantine := nil;
 end;
end;

function ExecuteAutoQurantineAuto : boolean;
begin
 AutoQurantine := nil;
 try
  AutoQurantine := TAutoQurantine.Create(nil);
  AutoQurantine.mbStart.Click;
 finally
  AutoQurantine.Free;
  AutoQurantine := nil;
 end;
end;

procedure TAutoQurantine.mbStartClick(Sender: TObject);
begin
 ProgressBar.Position := 0;
 ProgressBar.Max := 13;
 ZProgressClass.AddProgressMax(ProgressBar.Max * 100);
 ProgressBar.Position := ProgressBar.Position + 1;
 // Процессы и библиотеки
 Application.ProcessMessages;
 if cbProcess.Checked then
  try
   ProcessView := nil;
   ProcessView := TProcessView.Create(nil);
   ProcessView.CreateProcessList;
   ProcessView.AddAllToQurantine;
   ProgressBar.Position := ProgressBar.Position + 1;
   ZProgressClass.ProgressStep(100);
  finally
   ProcessView.Free;
   ProcessView := nil;
  end;
 // Модули ядра
 Application.ProcessMessages;
 if IsNT and cbKernelModules.Checked then
  try
   KernelObjectsView := nil;
   KernelObjectsView := TKernelObjectsView.Create(nil);
   KernelObjectsView.RefreshModuleList;
   KernelObjectsView.AddAllToQurantine;
   ProgressBar.Position := ProgressBar.Position + 1;
   ZProgressClass.ProgressStep(100);
  finally
   KernelObjectsView.Free;
   KernelObjectsView := nil;
  end;
 // Службы и драйверы
 Application.ProcessMessages;
 if IsNT and cbService.Checked then
  try
   ServiceView := nil;
   ServiceView := TServiceView.Create(nil);
   ServiceView.cbServiceMode.ItemIndex := 2;
   case cbServiceMode.ItemIndex of
    0 : ServiceView.cbServiceStatus.ItemIndex := 0;
    1 : ServiceView.cbServiceStatus.ItemIndex := 2;
   end;
   ServiceView.CreateServiceList;
   ServiceView.AddAllToQurantine;
   ProgressBar.Position := ProgressBar.Position + 1;
   ZProgressClass.ProgressStep(100);
  finally
   ServiceView.Free;
   ServiceView := nil;
  end;
 // Автозапуск
 Application.ProcessMessages;
 if cbAutorun.Checked then
  try
   Autoruns := nil;
   Autoruns := TAutoruns.Create(nil);
   Autoruns.RefreshItems(true);
   Autoruns.AddAllToQurantine;
   ProgressBar.Position := ProgressBar.Position + 1;
   ZProgressClass.ProgressStep(100);
  finally
   Autoruns.Free;
   Autoruns := nil;
  end;
 // Расширения IE
 Application.ProcessMessages;
 if cbIEExt.Checked then
  try
   BHOManager := nil;
   BHOManager := TBHOManager.Create(nil);
   BHOManager.RefreshBHOList;
   BHOManager.AddAllToQurantine;
   ProgressBar.Position := ProgressBar.Position + 1;
   ZProgressClass.ProgressStep(100);
  finally
   BHOManager.Free;
   BHOManager := nil;
  end;
 // Расширения проводника
 Application.ProcessMessages;
 if cbExplorerExt.Checked then
  try
   ExplorerExtManager := nil;
   ExplorerExtManager := TExplorerExtManager.Create(nil);
   ExplorerExtManager.RefreshExplorerExtList;
   ExplorerExtManager.AddAllToQurantine;
   ProgressBar.Position := ProgressBar.Position + 1;
   ZProgressClass.ProgressStep(100);
  finally
   ExplorerExtManager.Free;
   ExplorerExtManager := nil;
  end;
 // Расширения печати
 Application.ProcessMessages;
 if cbPrintExt.Checked then
  try
   PrintExtManager := nil;
   PrintExtManager := TPrintExtManager.Create(nil);
   PrintExtManager.RefreshPrintExtList;
   PrintExtManager.AddAllToQurantine;
   ProgressBar.Position := ProgressBar.Position + 1;
   ZProgressClass.ProgressStep(100);
  finally
   PrintExtManager.Free;
   PrintExtManager := nil;
  end;
 // Планировщик задач
 if cbTaskScheduler.Checked then
  try
   TaskScheduler := nil;
   TaskScheduler := TTaskScheduler.Create(nil);
   TaskScheduler.RefreshTaskSchedulerJobs;
   TaskScheduler.AddAllToQurantine;
   ProgressBar.Position := ProgressBar.Position + 1;
   ZProgressClass.ProgressStep(100);
  finally
   TaskScheduler.Free;
   TaskScheduler := nil;
  end;
 // Порты
 Application.ProcessMessages;
 if cbOpenPorts.Checked then
  try
   NetOpenPorts := nil;
   NetOpenPorts := TNetOpenPorts.Create(nil);
   NetOpenPorts.RefresConnections;
   NetOpenPorts.AddAllToQurantine;
   ProgressBar.Position := ProgressBar.Position + 1;
   ZProgressClass.ProgressStep(100);
  finally
   NetOpenPorts.Free;
   NetOpenPorts := nil;
  end;
  // Protocols
 Application.ProcessMessages;
 if cbProtocols.Checked then
  try
   ProtocolManager := nil;
   ProtocolManager := TProtocolManager.Create(nil);
   ProtocolManager.RefreshList;
   ProgressBar.Position := ProgressBar.Position + 1;
   ProtocolManager.AddAllToQurantine;
   ZProgressClass.ProgressStep(100);
  finally
   ProtocolManager.Free;
   ProtocolManager := nil;
  end;
  // DPF
 Application.ProcessMessages;
 if cbDPF.Checked then
  try
   DPFManager := nil;
   DPFManager := TDPFManager.Create(nil);
   DPFManager.RefreshDPFList;
   ProgressBar.Position := ProgressBar.Position + 1;
   ZProgressClass.ProgressStep(100);
   DPFManager.AddAllToQurantine;
  finally
   DPFManager.Free;
   DPFManager := nil;
  end;
  // CPL
 Application.ProcessMessages;
 if cbDPF.Checked then
  try
   CPLManager := nil;
   CPLManager := TCPLManager.Create(nil);
   CPLManager.RefreshCPLList;
   CPLManager.AddAllToQurantine;
   ProgressBar.Position := ProgressBar.Position + 1;
   ZProgressClass.ProgressStep(100);
  finally
   CPLManager.Free;
   CPLManager := nil;
  end;
  // Active Setup
 Application.ProcessMessages;
 if cbAS.Checked then
  try
   ActiveSetupManager := nil;
   ActiveSetupManager := TActiveSetupManager.Create(nil);
   ActiveSetupManager.RefreshASList;
   ActiveSetupManager.AddAllToQurantine;
   ProgressBar.Position := ProgressBar.Position + 1;
   ZProgressClass.ProgressStep(100);
  finally
   ActiveSetupManager.Free;
   ActiveSetupManager := nil;
  end;
 ProgressBar.Position := 0;
 if Visible then
  zMessageDlg('$AVZ0358', mtInformation, [mbYes], 0);
end;

function TAutoQurantine.Execute: boolean;
begin
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TAutoQurantine.FormCreate(Sender: TObject);
begin
 cbServiceMode.ItemIndex := 0;
 TranslateForm(Self);
end;

end.

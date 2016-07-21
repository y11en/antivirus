program sysInfo;

uses
  Forms,
  Unit1 in 'Unit1.pas' {Form1},
  kernlutl in 'kernlutl.pas',
  SystemMonitor in 'SystemMonitor.pas',
  NetAPIDef in 'NetAPIDef.pas',
  zAntivirus in 'zAntivirus.pas',
  zNetInt in 'zNetInt.pas';

{$R *.RES}

begin
  Application.Initialize;
  //Application.ShowMainForm := false;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.

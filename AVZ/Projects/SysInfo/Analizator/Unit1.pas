unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Registry, ntdll;

type
  TTestThreas = class (TThread)
    procedure Execute; override;
  end;
  TForm1 = class(TForm)
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Button5: TButton;
    procedure FormCreate(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
  private
    { Private declarations }
  public
    TestThreas : array[1..20] of TTestThreas;
  end;
var
  Form1: TForm1;

implementation

{$R *.dfm}

procedure TForm1.FormCreate(Sender: TObject);
var
 reg : TRegistry;
begin
 LoadLibrary('zsa.dll');
 reg := TRegistry.Create;
 reg.RootKey := HKEY_CLASSES_ROOT;
 reg.CreateKey('software\test');
 reg.Free;
end;

procedure TForm1.Button1Click(Sender: TObject);
var
  si : Tstartupinfo;
  p  : Tprocessinformation;
begin
 FillChar( Si, SizeOf( Si ) , 0 );
 with Si do begin
  cb := SizeOf( Si);
  dwFlags := startf_UseShowWindow;
  wShowWindow := 4;
 end;
 // Создание процесса
 CreateProcess(nil,'c:\avz4\avz.exe', nil,nil,false,Create_default_error_mode,nil,nil,si,p);
// CreateProcess(nil,'E:\Delphi5\Delphi7\Projects\SysInfo\research\Revizor\revizor.exe', nil,nil,false,Create_default_error_mode,nil,nil,si,p);
end;

{ TTestThreas }

procedure TTestThreas.Execute;
var
 reg : TRegistry;
begin
 while (true) do begin
  reg := TRegistry.Create;
  reg.RootKey := HKEY_CLASSES_ROOT;
  try reg.CreateKey('software\test'); reg.CloseKey; except end;
  reg.CloseKey;
  reg.Free;
 end;
end;

procedure TForm1.Button2Click(Sender: TObject);
var
 i          : integer;
begin
 for i := 1 to 7 do
  TestThreas[i] := TTestThreas.Create(false);
end;

procedure TForm1.Button3Click(Sender: TObject);
var
 reg : TRegistry;
begin
 while (true) do begin
  reg := TRegistry.Create;
  reg.RootKey := HKEY_CLASSES_ROOT;
  try reg.CreateKey('software\test'); reg.CloseKey; except end;
  reg.CloseKey;
  reg.Free;
  Sleep(25);
 end;
end;
procedure TForm1.Button4Click(Sender: TObject);
begin
 LoadServiceManagerAPI;
 zCreateService(0, '', '', 0, 0, 0, 0, '','',0,'','','');
end;

procedure TForm1.Button5Click(Sender: TObject);
var
 SI : TStartupInfo;
 lpProcessInformation: TProcessInformation;
begin
 FillChar( Si, SizeOf( Si ) , 0 );
 with Si do begin
  cb := SizeOf( Si);
  dwFlags := startf_UseShowWindow;
  wShowWindow := 4;
 end;
 CreateProcess(nil, 'notepad.exe',
  nil, nil,
  false, 0, nil,
  nil, SI,
  lpProcessInformation);
end;

end.

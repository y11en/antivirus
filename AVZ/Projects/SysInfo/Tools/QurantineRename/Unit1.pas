unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Mask, ToolEdit, INIFiles;

type
  TForm1 = class(TForm)
    deDir: TDirectoryEdit;
    Button1: TButton;
    cbRecurse: TCheckBox;
    procedure FormCreate(Sender: TObject);
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
   function ProcessDir(ADirName : string; ARecusive : boolean = false) : boolean;
  end;

var
  Form1: TForm1;

implementation
uses zutil;
{$R *.dfm}

procedure TForm1.FormCreate(Sender: TObject);
begin
 if ParamCount > 0 then begin
  deDir.Text := ParamStr(1);
  cbRecurse.Checked := (UpperCase(paramstr(2)) = '/R') or (UpperCase(paramstr(2)) = '\R');
  Button1.Click;
  Application.Terminate;
 end;
end;

function TForm1.ProcessDir(ADirName: string; ARecusive : boolean = false): boolean;
var
 SR : TSearchRec;
 Res : integer;
 Path, S, OldName, NewName : string;
 WorkDir : string;
 INI : TIniFile;
 Lines : TStringList;
begin
 Lines := TStringList.Create;
 Path := NormalDir(ADirName);
 // Переименование файлов карантина AVZ
 Res := FindFirst(Path + '*.ini', faAnyFile, SR);
 while res = 0 do begin
  if FileExists(Path + SR.Name) then begin
   INI := TIniFile.Create(Path + SR.Name);
   S := Trim(INI.ReadString('InfectedFile','Src', ''));
   OldName := ChangeFileExt(Path + SR.Name, '.dta');
   NewName := Path + copy(S, 4, 500);
   ForceDirectories(ExtractFilePath(NewName));
   if RenameFile(OldName, NewName) then
    DeleteFile(Path + SR.Name);
   INI.Free;
  end;
  Res := FindNext(SR);
 end;
 FindClose(SR);
 // Переименование файлов с сайта
 Res := FindFirst(Path + 'vf*.dta', faAnyFile, SR);
 while res = 0 do begin
  if FileExists(Path + SR.Name) then begin
   WorkDir := NormalDir(Path + ChangeFileExt(ExtractFileName(SR.Name), ''));
   ForceDirectories(WorkDir);
   // Копирование описания
   RenameFile(ChangeFileExt(Path + SR.Name, '.txt'), WorkDir+ExtractFileName(ChangeFileExt(Path + SR.Name, '.txt')));
   // Копирование файла
   try
    Lines.LoadFromFile(WorkDir+ExtractFileName(ChangeFileExt(Path + SR.Name, '.txt')));
    NewName := Lines.Values['FName'];
    RenameFile(Path + SR.Name, WorkDir+NewName);
   except
   end;
  end;
  Res := FindNext(SR);
 end;
 FindClose(SR);

 // Рекурсивный анализ вложенных каталогов
 if ARecusive then begin
   Res := FindFirst(Path + '*.*', faAnyFile, SR);
   while res = 0 do begin
    if (SR.Attr and faDirectory <> 0) and (SR.Name <> '.') and (SR.Name <> '..') then
     ProcessDir(Path + Sr.Name, ARecusive);
    Res := FindNext(SR);
   end;
   FindClose(SR);
 end;  
 Lines.Free;
end;


procedure TForm1.Button1Click(Sender: TObject);
begin
 ProcessDir(deDir.Text, cbRecurse.Checked);
end;

end.

unit uIESettings;
// Менеджер настроек IE (не активен - заготовка !!)
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, zBHOManager;

type
  TIESettings = class(TForm)
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    { Private declarations }
  public
   BHOManager : TBHOManager;
   function Execute : boolean;
  end;

var
  IESettings: TIESettings;

Function ExecuteIESettings : boolean;

implementation

{$R *.dfm}

{ TIESettings }

function TIESettings.Execute: boolean;
begin
 BHOManager.Refresh;
 ShowModal;
 Result := ModalResult = mrOk;
end;

Function ExecuteIESettings : boolean;
begin
 IESettings := nil;
 try
  IESettings := TIESettings.Create(nil);
  IESettings.Execute;
 finally
  IESettings.Free;
  IESettings := nil;
 end;
end;

procedure TIESettings.FormCreate(Sender: TObject);
begin
 BHOManager := TBHOManager.Create;
end;

procedure TIESettings.FormDestroy(Sender: TObject);
begin
 BHOManager.Free;
end;

end.

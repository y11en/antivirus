unit uGUIShared;
// ********** Содержит общие для GUI форм компоненты ************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ImgList;

type
  TGUIShared = class(TForm)
    StateImageList: TImageList;
    HTMLSaveDialog: TSaveDialog;
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  GUIShared: TGUIShared;

implementation

{$R *.dfm}

begin
 if GUIShared = nil then
  GUIShared := TGUIShared.Create(nil);
end.

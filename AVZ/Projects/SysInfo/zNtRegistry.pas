unit zNtRegistry;

interface
uses Windows, Classes, SysUtils;
type
 TNtRegDataType = (rdUnknown, rdString, rdExpandString, rdInteger, rdBinary);


 TNtRegDataInfo = record
   RegData: TNtRegDataType;
   DataSize: Integer;
 end;
 TNTRegistry = class(TObject)
  private
    FRootKey      : HKEY;
    procedure SetRootKey(const Value: HKEY);
  public
    constructor Create; overload;
    destructor Destroy; override;
    procedure CloseKey;
    function CreateKey(const Key: string): Boolean;
    function DeleteKey(const Key: string): Boolean;
    function DeleteValue(const Name: string): Boolean;
    procedure GetKeyNames(Strings: TStrings);
    procedure GetValueNames(Strings: TStrings);
    function KeyExists(const Key: string): Boolean;
    function OpenKey(const Key: string; CanCreate: Boolean): Boolean;
    function OpenKeyReadOnly(const Key: String): Boolean;
    property RootKey: HKEY read FRootKey write SetRootKey;
  end;


implementation

{ TNTRegistry }

{ TNTRegistry }

procedure TNTRegistry.CloseKey;
begin

end;

constructor TNTRegistry.Create;
begin

end;

function TNTRegistry.CreateKey(const Key: string): Boolean;
begin

end;

function TNTRegistry.DeleteKey(const Key: string): Boolean;
begin

end;

function TNTRegistry.DeleteValue(const Name: string): Boolean;
begin

end;

destructor TNTRegistry.Destroy;
begin

  inherited;
end;

procedure TNTRegistry.GetKeyNames(Strings: TStrings);
begin

end;

procedure TNTRegistry.GetValueNames(Strings: TStrings);
begin

end;

function TNTRegistry.KeyExists(const Key: string): Boolean;
begin

end;

function TNTRegistry.OpenKey(const Key: string;
  CanCreate: Boolean): Boolean;
begin

end;

function TNTRegistry.OpenKeyReadOnly(const Key: String): Boolean;
begin

end;

procedure TNTRegistry.SetRootKey(const Value: HKEY);
begin
  FRootKey := Value;
end;

end.

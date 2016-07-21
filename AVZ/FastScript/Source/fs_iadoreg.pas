
{******************************************}
{                                          }
{             FastScript v1.9              }
{            Registration unit             }
{                                          }
{  (c) 2003-2005 by Alexander Tzyganenko,  }
{             Fast Reports Inc             }
{                                          }
{******************************************}

unit fs_iadoreg;

{$i fs.inc}

interface


procedure Register;

implementation

uses
  Classes
{$IFNDEF Delphi6}
, DsgnIntf
{$ELSE}
, DesignIntf
{$ENDIF}
, fs_iadortti;

{-----------------------------------------------------------------------}

procedure Register;
begin
  RegisterComponents('FastScript', [TfsADORTTI]);
end;

end.


//1e6391b54c81ea5a8e8ccbf16c5b604c
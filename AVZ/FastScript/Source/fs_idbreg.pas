
{******************************************}
{                                          }
{             FastScript v1.9              }
{            Registration unit             }
{                                          }
{  (c) 2003-2005 by Alexander Tzyganenko,  }
{             Fast Reports Inc             }
{                                          }
{******************************************}

unit fs_idbreg;

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
, fs_idbrtti, fs_idbctrlsrtti;

{-----------------------------------------------------------------------}

procedure Register;
begin
  RegisterComponents('FastScript', [TfsDBRTTI, TfsDBCtrlsRTTI]);
end;

end.


//1e6391b54c81ea5a8e8ccbf16c5b604c
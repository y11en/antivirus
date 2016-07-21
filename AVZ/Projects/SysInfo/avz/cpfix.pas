unit CPFix;
interface

{$IFNDEF VER150}
{$IF Trunc(RTLVersion) <> 15}
{$MESSAGE ERROR 'This unit for Delphi 7 only.'}
{$IFEND}
{$ENDIF}

function FixDefaultUserCodePage:Boolean;

implementation
uses
  Windows;

function FixDefaultUserCodePage:Boolean;
type
  TCode=packed record
    CallOpA:Byte;//$E8
    OffsetA:Integer;//GetThreadLocale
    CallOpB:Byte;//$E8
    OffsetB:Integer;//LCIDToCodePage
    MovOp:Byte;//$A3
    VarAddr:PInteger;//DefaultUserCodePage
  end;
  TCodes=packed record
    Code1:TCode;//NT4
    JMP:Word;//Short jump
    Code2:TCode;//Win95/98/ME
  end;
  PImportStub=^TImportStub;
  TImportStub=packed record
    JMP:Word;//$25ff
    Addr:PPointer;//Real addr
  end;
var
  Code:^TCodes;
  Count:Integer;
  MemInfo:TMemoryBasicInformation;
  P:Pointer;
begin
  Result:=False;
  P:=@TObject.Free;
  if PImportStub(P).JMP=$25FF then P:=PImportStub(P).Addr^;
  if (VirtualQuery(P,MemInfo,SizeOf(MemInfo))<SizeOf(MemInfo))
    or(MemInfo.State<>MEM_COMMIT) then Exit;
  Code:=MemInfo.BaseAddress;
  Count:=MemInfo.RegionSize;
  while Count>SizeOf(TCodes) do begin
    with Code^ do
      if (Code1.CallOpA=$E8)and(Code1.CallOpB=$E8)and(Code1.MovOp=$A3)
        and(Code2.CallOpA=$E8)and(Code2.CallOpB=$E8)and(Code2.MovOp=$A3)
        and(Code1.OffsetA-Code1.OffsetB=Code2.OffsetA-Code2.OffsetB)
        and(Code1.VarAddr=Code2.VarAddr) then begin
        Code1.VarAddr^:=GetACP;
        Result:=True;
        Break;
      end;
    Inc(PByte(Code));
    Dec(Count);
  end;
end;

initialization
  FixDefaultUserCodePage;
end.

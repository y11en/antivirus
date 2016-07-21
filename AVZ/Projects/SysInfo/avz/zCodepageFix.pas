unit zCodepageFix;

interface

function GetDefaultUserCodePageAddr:PInteger;

implementation

uses Windows;
function GetDefaultUserCodePageAddr:PInteger;
type
  TCode=packed record
    CallOpA:Byte;
    OffsetA:Integer;
    CallOpB:Byte;
    OffsetB:Integer;
    MovOp:Byte;
    VarAddr:Pointer;
  end;
  TCodes=packed record
    Code1:TCode;
    JMP:Word;
    Code2:TCode;
  end;
var
  Code:^TCodes;
  Count:Integer;
  MemInfo:TMemoryBasicInformation;
begin
  Result:=nil;
  if (VirtualQuery(TObject,MemInfo,SizeOf(MemInfo))<SizeOf(MemInfo))
    or(MemInfo.State<>MEM_COMMIT) then Exit;
  Code:=MemInfo.BaseAddress;
  Count:=MemInfo.RegionSize;
  while Count>0 do begin
    with Code^ do
      if (Code1.CallOpA=$E8)and(Code1.CallOpB=$E8)and(Code1.MovOp=$A3)
        and(Code2.CallOpA=$E8)and(Code2.CallOpB=$E8)and(Code2.MovOp=$A3)
        and(Code1.OffsetA-Code1.OffsetB=Code2.OffsetA-Code2.OffsetB)
        and(Code1.VarAddr=Code2.VarAddr) then begin
        Result:=Code1.VarAddr;
        Break;
      end;
    Inc(PByte(Code));
  end;
end;

end.

unit zProcessMemory;

interface
uses Windows, Classes, SysUtils, registry, PSAPI;

type
 TMemoryRegionInfo = record
  MemoryBasicInformation : TMemoryBasicInformation;
 end;

 TMemoryRegionInfoArray = array of TMemoryRegionInfo;

 TProcessMemoryInfo = class
  MemoryRegionInfoArray : TMemoryRegionInfoArray;
  MemAllocGran          : DWord; // Гранулярность заполнения памяти
  constructor Create;
  // Обновление информации
  function ResreshInfo(hProcess : THandle) : boolean;
  function GetMappedFileName(hProcess: THandle; lpv: Pointer) : string;
 end;

implementation

{ TProcessMemoryInfo }

constructor TProcessMemoryInfo.Create;
var
 SI : TSystemInfo;
begin
 MemoryRegionInfoArray := nil;
 // Определение гранулярности заполнения памяти
 GetSystemInfo(SI);
 MemAllocGran := SI.dwAllocationGranularity;
end;

function TProcessMemoryInfo.GetMappedFileName(hProcess: THandle; lpv: Pointer): string;
var
 Res : integer;
begin
 SetLength(Result, 300);
 Res := PSAPI.GetMappedFileName(hProcess, lpv, @Result[1], 300);
 Result := copy(Result, 1, Res);
end;

function TProcessMemoryInfo.ResreshInfo(hProcess : THandle): boolean;
var
 MemoryBasicInformation : TMemoryBasicInformation;
 lpAddress : Pointer;
 Res       : DWORD;
begin
 MemoryRegionInfoArray := nil;
 // Первый запрос
 lpAddress := nil;
 Res := VirtualQueryEx(hProcess, lpAddress, MemoryBasicInformation, sizeof(MemoryBasicInformation));
 while Res > 0 do begin
  // Добавление региона
  SetLength(MemoryRegionInfoArray, length(MemoryRegionInfoArray)+1);
  MemoryRegionInfoArray[Length(MemoryRegionInfoArray)-1].MemoryBasicInformation := MemoryBasicInformation;
  // Переход на новый регион
  lpAddress := pointer(dword(MemoryBasicInformation.BaseAddress) + MemoryBasicInformation.RegionSize);
  Res := VirtualQueryEx(hProcess, lpAddress, MemoryBasicInformation, sizeof(MemoryBasicInformation));
 end;
end;

end.
 

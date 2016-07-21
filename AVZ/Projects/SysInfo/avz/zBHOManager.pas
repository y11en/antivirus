unit zBHOManager;

interface
uses Windows, SysUtils, winsock, registry, Variants, Classes, zSharedFunctions,
  RxVerInf, zutil;
type
 // Информация о BHO
 TBHOInfo = record
   BinFileName     : string;  // Имя исполняемого файла
   Descr           : string;  // Описание (если есть)
   BinFileExists   : boolean; // Признак существования файла на диске
   BinFileInfo     : string;  // Информация о исполняемом файле
   CheckResults    : boolean; // Результаты проверки
   RegRoot         : HKEY;    // Узел реестра
   RegKEY          : string;  // Ключ реестра
   RegValue        : string;  // Ключ или параметр реестра, идентифицирующий BHO
   RegType         : integer; // Тип ключа BHO
 end;

 // Менеджер BHO
 TBHOManager = class
   // Списки пространств имен
   BHOListReg  : array of  TBHOInfo;
   constructor Create;
   // Обновление списков
   function Refresh : boolean;
 end;
implementation

{ TBHOManager }

constructor TBHOManager.Create;
begin
 BHOListReg := nil;
end;

function TBHOManager.Refresh: boolean;
begin
 BHOListReg := nil;
end;

end.

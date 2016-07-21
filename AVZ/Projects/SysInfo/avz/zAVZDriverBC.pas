unit zAVZDriverBC;

interface
uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, winsvc, StdCtrls, ntdll, AclAPI, AccCtrl, zSharedFunctions, zAntivirus,
  registry;
type

 TAVZDriverBC = class
  private
    FDriverPath, FDriverName: string;
    ServiceAPILoaded : boolean;
    procedure SetDriverPath(const Value: string);
  public
   KMBase       : TKMBase;
   LastStatus : DWORD;
   DriverSvcName, DriverLinkName : string;
   // Конструктор
   constructor Create; virtual;
   // Инсталляция драйвера и его настройка
   function InstallDriver(AFiles, AKeys, ASvcReg, ASvc, ADisableSvc, AQrFiles,  AQrSvc, ACpFiles : TStrings; ALogFile : string)   : boolean; virtual;
   // Загрузка драйвера
   function LoadDriver      : boolean; virtual;
   // Деинсталляция драйвера
   function UnInstallDriver : boolean; virtual;
  published
   Property DriverPath : string read FDriverPath write SetDriverPath;
 end;

 TAVZDriverBCKIS = class(TAVZDriverBC)
   // Конструктор
   constructor Create; override;
   // Инсталляция драйвера и его настройка
   function InstallDriver(AFiles, AKeys, ASvcReg, ASvc, ADisableSvc, AQrFiles,  AQrSvc, ACpFiles : TStrings; ALogFile : string)   : boolean; override;
   // Загрузка драйвера
   function LoadDriver      : boolean; override;
   // Деинсталляция драйвера
   function UnInstallDriver : boolean; override;
 end;

implementation
uses  zAVKernel, zutil;
{ TAVZDriverN }


constructor TAVZDriverBC.Create;
begin
 ServiceAPILoaded := LoadServiceManagerAPI;
 // Имя файла
 FDriverName := 'AVZBC.sys';
 FDriverName := GenerateRandomName(8, 4) + '.sys';
 // Имя службы
 DriverSvcName := 'AVZBC';
 DriverSvcName := GenerateRandomName(8, 4);
end;

function TAVZDriverBC.InstallDriver(AFiles, AKeys, ASvcReg, ASvc, ADisableSvc, AQrFiles,  AQrSvc, ACpFiles : TStrings; ALogFile : string): boolean;
var
 Reg     : TRegistry;
 KeyName,  Group : String;
 i       : integer;
begin
 if KMBase <> nil then
  KMBase.CreateDriverFile('AVZBC', DriverPath+FDriverName, 0, DriverLinkName); //#DNL
 LastStatus := zSharedFunctions.InstallDriver(DriverPath+FDriverName,
                                          DriverSvcName, 'AVZ-BC Kernel Driver', 1); //#DNL
 Result := LastStatus = STATUS_SUCCESS;
 if Result then begin
  Group := '';
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  // Определение группы
  if Reg.OpenKey('SYSTEM\CurrentControlSet\Control\ServiceGroupOrder', false) then begin
   try
    SetLength(Group, 4096);
    i := Reg.ReadBinaryData('List', Group[1], Length(Group));
    SetLength(Group, i);
    Delete(Group, 1, pos(#0, Group));
    Group := Trim(copy(Group, 1, pos(#0, Group)-1));
   except
    Group := '';
   end;
  end;
  KeyName := 'SYSTEM\CurrentControlSet\Services\'+DriverSvcName; //#DNL
  Reg.CloseKey;
  if Reg.OpenKey(KeyName, false) then begin
   if Group <> '' then
    Reg.WriteString('Group', Group); //#DNL
   Reg.WriteInteger('Tag', 1);       //#DNL

   if Trim(ALogFile) <> '' then
    Reg.WriteString('LogFile', '\??\'+Trim(ALogFile)); //#DNL
   for i := 0 to AFiles.Count-1 do
    Reg.WriteString('DelFile'+IntToStr(i+1), '\??\'+AFiles[i]); //#DNL
   for i := 0 to AQrFiles.Count-1 do
    Reg.WriteString('QrFile'+IntToStr(i+1), '\??\'+AQrFiles[i]); //#DNL
   for i := 0 to ACpFiles.Count-1 do begin
    Reg.WriteString('CpFileF'+IntToStr(i+1), '\??\'+copy(ACpFiles[i], 1, pos('||', ACpFiles[i])-1)); //#DNL
    Reg.WriteString('CpFileT'+IntToStr(i+1), '\??\'+copy(ACpFiles[i], pos('||', ACpFiles[i])+2, maxint)); //#DNL
   end;
   for i := 0 to AKeys.Count-1 do
    Reg.WriteString('DelRegKey'+IntToStr(i+1), RegKeyToSystemRegKey(AKeys[i])); //#DNL
   for i := 0 to ASvcReg.Count-1 do
    Reg.WriteString('DelSvcReg'+IntToStr(i+1), ASvcReg[i]); //#DNL
   for i := 0 to ASvc.Count-1 do
    Reg.WriteString('DelSvc'+IntToStr(i+1), ASvc[i]); //#DNL
   for i := 0 to AQrSvc.Count-1 do
    Reg.WriteString('QrSvc'+IntToStr(i+1), AQrSvc[i]); //#DNL
   for i := 0 to ADisableSvc.Count-1 do
    Reg.WriteString('DisableSvc'+IntToStr(i+1), ADisableSvc[i]); //#DNL
   // Создание пути для сохранения файлов, внесенных в карантин
   if (AQrSvc.Count > 0) or (AQrFiles.Count > 0) then begin
    Reg.WriteString('QrPath', '\??\'+GetQuarantineDirName('Quarantine')); //#DNL
    zForceDirectories(GetQuarantineDirName('Quarantine'));  //#DNL
   end;
  end;
  Reg.CloseKey;
  Reg.Free;
 end;
end;

function TAVZDriverBC.LoadDriver: boolean;
begin
 Result := false;
 LastStatus := zSharedFunctions.LoadDriver(DriverSvcName);
 Result := (LastStatus = STATUS_SUCCESS) or (LastStatus = STATUS_IMAGE_ALREADY_LOADED);
end;

procedure TAVZDriverBC.SetDriverPath(const Value: string);
begin
  FDriverPath := Value;
end;

function TAVZDriverBC.UnInstallDriver: boolean;
begin
 LastStatus := zSharedFunctions.UnInstallDriver(DriverSvcName);
 DeleteFile(FDriverPath+FDriverName);
 Result := LastStatus = STATUS_SUCCESS;
end;

{ TAVZDriverBCKIS }

constructor TAVZDriverBCKIS.Create;
begin
 ServiceAPILoaded := LoadServiceManagerAPI;
 // Имя файла
 FDriverName := 'AVZBC.sys';
 // Имя службы
 DriverSvcName := 'AVZBC';
end;

function TAVZDriverBCKIS.InstallDriver(AFiles, AKeys, ASvcReg, ASvc,
  ADisableSvc, AQrFiles, AQrSvc, ACpFiles: TStrings;
  ALogFile: string): boolean;
begin

end;

function TAVZDriverBCKIS.LoadDriver: boolean;
begin

end;

function TAVZDriverBCKIS.UnInstallDriver: boolean;
begin

end;

initialization
finalization
end.

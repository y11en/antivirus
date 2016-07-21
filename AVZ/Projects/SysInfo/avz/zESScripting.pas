unit zESScripting;
// Скрипт-машина для скриптов эвристического обследования процессов
interface
uses Windows, Messages, SysUtils, Classes, Forms, ComCtrls,
     fs_iinterpreter,
     fs_ipascal,
     fs_iclassesrtti,
     zLSP, zSharedFunctions,
     zAntivirus,
     uInfectedView,
     uSysCheck,
     Registry, INIFiles, ZLogSystem;
type
 TESUserScript = class
  private
    FLogEnabled: boolean;
    procedure SetLogEnabled(const Value: boolean);
  protected
   fsScriptRuntime : TfsScript;
   fsPascal        : TfsPascal;
   fsClassesRTTI   : TfsClassesRTTI;
   // Обработчик "моих" команд
   function OnScriptCallMetod(Instance: TObject; ClassType: TClass; const MethodName: String;
                             var Params: Variant): Variant;
  public
   NeuroTokenList  : TStrings;
   ReadOnlyMode  : boolean;
   constructor Create;
   procedure AddToLog(s : string);
   function  GetTokenVal(AName : string; ADefVal : integer) : integer;
   function  SetTokenVal(AName : string; AVal : integer) : integer;
   function  ExecuteScript(AScript : string; AScriptName : string='') : boolean;
   destructor Destroy; override;
   property LogEnabled : boolean read FLogEnabled write SetLogEnabled;
 end;

implementation
uses WizardNN, zAVKernel;
{ TESUserScript }

procedure TESUserScript.AddToLog(s: string);
begin
 ZLogSystem.AddToLog('[ES]:'+s, logInfo);
end;

constructor TESUserScript.Create;
begin
 fsScriptRuntime := TfsScript.Create(nil);
 fsScriptRuntime.Parent := fsGlobalUnit;
 fsPascal        := TfsPascal.Create(nil);
 fsClassesRTTI   := TfsClassesRTTI.Create(nil);
 ReadOnlyMode := false;
 FLogEnabled := true;
end;


destructor TESUserScript.Destroy;
begin
  fsClassesRTTI.Free;
  fsPascal.Free;
  fsScriptRuntime.Free;
  inherited;
end;

function TESUserScript.ExecuteScript(AScript,
  AScriptName: string): boolean;
begin
 Result := false;
 // Выполнение скрипта лечения
 if Trim(AScript) = '' then exit;
 // Подготовка текста скрипта
 fsScriptRuntime.Clear;
 fsScriptRuntime.Lines.Text := AScript;

 fsScriptRuntime.AddConst('AScriptName', 'string', AScriptName);
 fsScriptRuntime.AddMethod('procedure AddToLog(S : String)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function TokenExists(S : String) : boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure AddToken(ATokenName : String; Cnt : integer)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function QueryTokenCnt(ATokenName : String) : integer', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function QueryTokenVal(ATokenName : String; ADefVal : integer) : integer', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function QueryTokenGrCnt(ATokenGrName : String) : integer', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function ExecuteNeuro(ANetName : String) : boolean', OnScriptCallMetod);

 // Компиляция и выполнение
 if fsScriptRuntime.Compile then begin
  fsScriptRuntime.Execute;
  Result := true;
 end else begin
  AddToLog('$AVZ0630: '+AScriptName+' '+fsScriptRuntime.ErrorMsg+', $AVZ0745 ['+fsScriptRuntime.ErrorPos+']');
  AddToLog(AScript);
 end;
end;

function TESUserScript.GetTokenVal(AName: string;
  ADefVal: integer): integer;
var
 BoolMode : boolean;
 Indx     : integer;
begin
 Result := ADefVal;
 AName := Trim(UpperCase(AName));
 BoolMode := false;
 if copy(AName, length(AName), 1) = '#' then begin
  delete(AName, length(AName), 1);
  BoolMode := true;
 end;
 Indx := NeuroTokenList.IndexOf(AName);
 if Indx < 0 then exit;
 Result := integer(NeuroTokenList.Objects[Indx]);
 if BoolMode then
  if Result <> 0 then Result := 1;
end;

function TESUserScript.OnScriptCallMetod(Instance: TObject;
  ClassType: TClass; const MethodName: String;
  var Params: Variant): Variant;
var
 s       : string;
 i       : integer;
 Res     : integer;
 Lines   : TStringList;
begin
 Application.ProcessMessages;
 try
   // Добавление в протокол
   if MethodName = 'ADDTOLOG' then begin
    AddToLog(Params[0]);
    exit;
   end;
   // Проверка существования токена
   if MethodName = 'TOKENEXISTS' then begin
    Result := NeuroTokenList.IndexOf(Params[0]) >= 0;
    exit;
   end;
   // Проверка существования токена
   if MethodName = 'QUERYTOKENCNT' then begin
    Res := 0;
    S := UpperCase(Params[0]);
    for i := 0 to NeuroTokenList.Count - 1 do
     if pos(S, UpperCase(NeuroTokenList[i])) = 1 then
      Res := Res + integer(NeuroTokenList.Objects[i]);
     Result := Res;
    exit;
   end;
   // Проверка существования токена
   if MethodName = 'QUERYTOKENVAL' then begin
    Result := GetTokenVal(Params[0], Params[1]);
    exit;
   end;
   // Проверка существования токена
   if MethodName = 'QUERYTOKENGRCNT' then begin
    Res := 0;
    S := UpperCase(Params[0]);
    for i := 0 to NeuroTokenList.Count - 1 do
     if pos(S, UpperCase(NeuroTokenList[i])) = 1 then
      Res := Res + 1;
     Result := Res;
    exit;
   end;
   // Добавление токена
   if MethodName = 'ADDTOKEN' then begin
    SetTokenVal(Params[0], Params[1]);
    exit;
   end;
   // Выполнить нейросеть
   if MethodName = 'EXECUTENEURO' then begin
    Result := false;
    S := MainAntivirus.GetNeuroProfile(Params[0]);
    if S = '' then exit;
    // Загрузка нейропрофиля в нейроэмулятор
    Lines := TStringList.Create;
    Lines.Text := S;
    WizardNeuralNetwork.LoadFromWizardFile('', Lines);
    // Загрузка параметров в эмулятор
    for i := 0 to WizardNeuralNetwork.InputsList.Count - 1 do
     WizardNeuralNetwork.InputValues[WizardNeuralNetwork.InputsList[i]] := GetTokenVal(WizardNeuralNetwork.InputsList[i], 0);
    // Вычисление
    WizardNeuralNetwork.Compute;
    for i := 0 to WizardNeuralNetwork.OutputsList.Count - 1 do
     SetTokenVal(WizardNeuralNetwork.OutputsList[i], round(WizardNeuralNetwork.OutputValues[WizardNeuralNetwork.OutputsList[i]]*100));
    Lines.Free;
    exit;
   end;
 except
  on e : exception do
   AddToLog('$AVZ0654 '+MethodName+', $AVZ0631');
 end;
end;

procedure TESUserScript.SetLogEnabled(const Value: boolean);
begin

end;

function TESUserScript.SetTokenVal(AName: string; AVal: integer): integer;
var
 BoolMode : boolean;
 Indx     : integer;
begin
 Result := AVal;
 AName := Trim(UpperCase(AName));
 BoolMode := false;
 if copy(AName, length(AName), 1) = '#' then begin
  delete(AName, length(AName), 1);
  BoolMode := true;
 end;
 if BoolMode then
  if Result <> 0 then Result := 1;
 Indx := NeuroTokenList.IndexOf(AName);
 if Indx < 0 then
  NeuroTokenList.AddObject(AName, pointer(Result))
   else NeuroTokenList.Objects[Indx] := pointer(Result);
end;

end.

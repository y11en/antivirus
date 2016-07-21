unit WizardNN;
// Нейроэмулятор

interface
uses
  Classes,IniFiles,SysUtils;

const  // типы нормализации для входов и выходов
  ntMID = 0;  // линейная нормализация
  ntEXP = 1;  // экспоненциальная нормализация
  ntNONE = 3; // без нормализации
  ntAUTO = 2; // автоматическая нормализация

type
  TNeuralNetwork = class (TObject)
  private
    fW : array of array of array of double; // массив коэффициентов нейросети
    fDW: array of array of array of double; // массив приращений коэффициентов
    fWT:array of array of double          ; // массив смещения начальной точки
    fDWT:array of array of double         ; // массив приращений начальной точки
    fEA,                                    // производная ошибки
    fEI: array of double                  ; // скорость изменения производной
    fConfig : array of integer            ; // конфигурация слоев
    fLayerOutput: array of array of double; // выходы слоев(fLayerOutput[0]-вход)
    fTS,                                    // скорость обучения
    fMiu: double;                           // коэффициент инерции
    fLC:integer;                            // число слоев
    fMaxNeurons:integer;                    // максимальное число нейронов
    // работа с конфигурацией сети
    procedure fSetLC(LC:integer);
    function fGetL(Layer:integer):integer;
    procedure fSetL(Layer,NeuronNum:integer);
    // работа со входными значениями
    function fGetIn(i:integer):double;
    procedure fSetIn(i:integer;val:double);
    // работа с выходными значениями
    function fGetOut(i:integer):double;
    procedure fSetTS(TS:double);
    function fGetW(i,j,k:integer):double;
    procedure fSetW(i,j,k:integer;W:double);
    function fGetWT(i,k:integer):double;
    procedure fSetWT(i,k:integer;W:double);
    //
  public
    Alpha:double;
    Epoch : integer;                       // счетчик эпох
    property LayerCount:integer read fLC write fSetLC; // число слоев
    property Layers[i:integer]:integer read fGetL write fSetL;// число нейронов в слое
    property TeachSpeed:double read fTS write fSetTS; // скорость обучения
    property Miu:double read fMiu write fMiu;// инерционность обучения
    property W[i,j,k:integer]:double read fGetW write fSetW;
    property WT[i,k:integer]:double read fGetWT write fSetWT;
    // вектор входных значений
    property Input[i:integer]:double read fGetIN write fSetIN;
    // вектор выходных значений
    property Output[i:integer]:double read fGetOUT;
    // сброс слоя нейросети
    procedure ResetLayer(Layer:integer);
    // Рассчет
    procedure Compute;
    // обучение сети
    procedure Teach(Dest:array of double);
    // шоковая встряска сети
    procedure Shock;
    // чтение нейросети из внутреннего формата
    procedure RestoreNet(fname:string);
    // сохранение нейросети во внутреннем формате
    procedure StoreNet(fname:string);
    // подсчет корректировочных значений для нейросети
    procedure CountError(Dest:array of double);
    // коррекция весовых коэффициентов
    procedure CorrectW;
    constructor Create;
  end;
  TNeuralNetworkINI = class(TNeuralNetwork)
  public
    procedure LoadFromIni(var Ini:TMemIniFile;SectionName:string);
    procedure SaveToIni(var Ini:TIniFile;SectionName:string);
    constructor Create;
  end;
  TNetworkInputDef = class(TObject)
  public
    InputName : string;  // имя входа
    NormType  : integer; // способ нормализации
    Min,Max   : double;  // минимальное и максимальное значение для лин. нормализации
    alpha     : double;  // параметр сигмоидальной нормализации
    Mid,Disp  : double;  // среднее и дисперсия для автонормализации
    Value     : double;  // значение входа
    constructor Create;
  end;

  TNetworkInput = class(TNetworkInputDef)
  public
    number : integer;         // номер поля в НД
    Values : array of double; // массив значений
  end;

  TWizardNeuralNetwork = class (TNeuralNetworkINI)
  private
    fError       : double;
    fInputsList  : TStringList;
    fOutputsList : TStringList;
    function fGetInputValues(InputName:string):double;
    procedure fSetInputValues(InputName:string;InputValue:double);
    function fGetOutputValues(OutputName:string):double;
    procedure fSetOutputValues(OutputName:string;OutputValue:double);
  public
    property Error : double read fError;  // ошибка при обучении
    constructor Create;
    // установка входов и выходов нейросети
    procedure AddInput(InputDef:TNetworkInputDef);
    procedure DelInput(InputName:string);
    procedure AddOutput(OutputDef:TNetworkInputDef);
    procedure DelOutput(OutputName:string);
    // процедуры сохранения и чтения нейросети из файла
    procedure LoadFromWizardFile(FileName:string; ALines : TStrings);virtual;
    procedure StoreToFile(FileName:string);virtual;
    // значение входов и выходов нейросети
    property InputValues[inputName:string]:double read fGetInputValues write fSetInputValues;
    property OutputValues[inputName:string]:double read fGetOutputValues write fSetOutputValues;
    property InputsList:TStringList read fInputsList;
    property OutputsList:TStringList read fOutputsList;
    procedure Compute; // рассчет нейросетью
    procedure Learn;   // обучение нейросети
  end;

  function Sigmoid(inp:double;parm:double):double;
  function DSigmoid(inp:double;parm:double):double;

implementation
// реализация бозовых функций НС
// сброс слоя нейросети
procedure TNeuralNetwork.ResetLayer;
var
  xJ,xK,xLayer:integer;
begin
  for xLayer:=0 to fLC-2 do
  begin
    if (fConfig[xLayer]=0) or ((fConfig[xLayer+1]=0)) then exit;
    for xJ:=0 to High(fW[xLayer]) do
      for xK:=0 to High(fW[Layer,xJ]) do
        fW[xLayer,xJ,xK] := (Random(99)+1)/100;
  end;
end;

// установка скорости обучения
procedure TNeuralNetwork.fSetTS(TS:double);
begin
  fTS := TS;
end;

// работа с конфигурацией
procedure TNeuralNetwork.fSetLC(LC:integer);
begin
  Epoch := 0;
  fLC := LC;
  SetLength(fW,0);
  SetLength(fDW,0);
  SetLength(fConfig,0);
  SetLength(fLayerOutput,0);
  SetLength(fW,LC-1);
  SetLength(fDW,LC-1);
  SetLength(fConfig,LC);
  SetLength(fLayerOutput,LC);
  SetLength(fWT,LC-1);
  SetLength(fDWT,LC-1);
end;

procedure TNeuralNetwork.fSetL;
var
 xJ,xK:integer;
begin
  // проверка диапазона
  if (Layer < 0) or (Layer > fLC-1) then
    raise Exception.Create('$AVZ0527');
  fConfig[Layer]:=NeuronNum;
  if NeuronNum > fMaxNeurons then
  begin
    fMaxNeurons := NeuronNum;
    SetLength(fEA,0);
    SetLength(fEI,0);
    SetLength(fEA,fMaxNeurons);
    SetLength(fEI,fMaxNeurons);
  end;
  Epoch := 0;
  //  конфигурируем матрицу коэффициентов для слоя
  if (Layer>0)  then
    if (fConfig[Layer-1]>0) then
    begin
      // слой не входной.меняем число нейронов в этом слое
      SetLength(fW[Layer-1],0);
      SetLength(fDW[Layer-1],0);
      SetLength(fWT[Layer-1],0);
      SetLength(fDWT[Layer-1],0);
      SetLength(fW[Layer-1],fConfig[Layer-1],fConfig[Layer]);
      SetLength(fDW[Layer-1],fConfig[Layer-1],fConfig[Layer]);
      SetLength(fWT[Layer-1],fConfig[Layer]);
      SetLength(fDWT[Layer-1],fConfig[Layer]);
      // сбрасываем слой
      for xJ:=0 to fConfig[Layer-1]-1 do
        for xK:=0 to fConfig[Layer]-1 do
        begin
          fW[Layer-1,xJ,xK] := (Random(99)+1)/100;
        end;
      for xJ:=0 to fConfig[Layer]-1 do
        fWT[Layer-1,xJ] := (Random(99)+1)/100;
    end;
  // меняем число выходных нейронов в сотв. слое
  SetLength(fLayerOutput[Layer],0);
  SetLength(fLayerOutput[Layer],NeuronNum);
  if (Layer<fLC-1) then
    if (fConfig[Layer+1]>0) then
    begin
      // слой не выходной. меняем число нейронов в следующем слое
      SetLength(fW[Layer],0);
      SetLength(fDW[Layer],0);
      SetLength(fW[Layer],fConfig[Layer],fConfig[Layer+1]);
      SetLength(fDW[Layer],fConfig[Layer],fConfig[Layer+1]);
      // сбрасываем следующий слой
      for xJ:=0 to fConfig[Layer]-1 do
        for xK:=0 to fConfig[Layer+1]-1 do
        begin
          fW[Layer,xJ,xK] := (Random(99)+1)/100;
        end;
    end;
end;

function TNeuralNetwork.fGetL;
begin
  // проверка диапазона
  if (Layer < 0) or (Layer > fLC-1) then
    raise Exception.Create('$AVZ0527');
  result := fConfig[Layer];
end;


// работа со входными значениями
function TNeuralNetwork.fGetIn(i:integer):double;
begin
  if fLC = 0 then
    raise Exception.Create('$AVZ0950');
  if (i<0) or (i > fConfig[0]-1) then
    raise Exception.Create('$AVZ0491');
  result := fLayerOutput[0,i];
end;

procedure TNeuralNetwork.fSetIn(i:integer;val:double);
begin
  if fLC = 0 then
    raise Exception.Create('$AVZ0950');
  if (i<0) or (i > fConfig[0]-1) then
    raise Exception.Create('$AVZ0491');
  fLayerOutput[0,i] := val;
end;

// работа с выходными значеиями
function TNeuralNetwork.fGetOut(i:integer):double;
begin
  if fLC = 0 then
    raise Exception.Create('$AVZ0950');
  if (i<0) or (i > fConfig[fLC-1]-1) then
    raise Exception.Create('$AVZ0491');
  result := fLayerOutput[fLC-1,i];
end;

// работа с нейросетью
// Рассчет
procedure TNeuralNetwork.Compute;
var
  xI,xJ,xK,xTmp :integer;
  xSum :double;
begin
  // рассчитываем по слоям сети
  for xI:=0 to fLC-2 do
  begin
    // рассчитываем по I-му слою
    // считаем по нейронам слоя
    xTmp := fConfig[xI]-1;
    for xK:=0 to fConfig[xI+1]-1 do
    begin
      // считаем взвешенный вход K-го нейрона в I-м слое
      xSum := 0;
      // xTmp - число входов xI-го слоя НС
      for xJ:=0 to xTmp do
        xSum := xSum + fLayerOutput[xI,xJ]*fW[xI,xJ,xK];
      // добавляем точку смещения
      xSum := xSum+fWT[xI,xK];
      // считаем K-й выход слоя
      fLayerOutput[xI+1,xK] := Sigmoid(xSum,Alpha);
    end;
  end;
end;

// обучение сети
procedure TNeuralNetwork.Teach;
var
  xI,xJ,xK: integer;
  xTmp: integer;
  xSum: double;
begin
  // формируем массив ошибки для выходного слоя
  xTmp:=fConfig[fLC-1];
  dec(xTmp);
  for xK:=0 to xTmp do
    fEA[xK]:=fLayerOutput[fLC-1,xK]-Dest[xK];
  // корректируем послойно сеть
  for xI:=fLC-2 downto 0 do
  begin
    // рассчитываем вектор скорости изменения ошибки
    xTmp := fConfig[xI+1]-1;
    // xTmp - число нейронов в xI - м слое НС
    // xK - номер текущего нейрона слоя xI
    for xK:=0 to xTmp do
    begin
      fEI[xK] := fEA[xK]*DSigmoid(fLayerOutput[xI+1,xK],Alpha);
      fEA[xK] := 0;
    end;
    // определяем приращение коэффициентов
    // xJ - номер текущего входа слоя xI
    for xJ:=0 to fConfig[xI]-1 do
    begin
      xSum := 0;
      for xK:=0 to xTmp do
      begin
        xSum:=xSum + fEI[xK]*fW[xI,xJ,xK];
        fDW[xI,xJ,xK]:=-fTS*fEI[xK]*fLayerOutput[xI,xJ]+fMiu*fDW[xI,xJ,xK];
        fW[xI,xJ,xK]:=fW[xI,xJ,xK]+fDW[xI,xJ,xK];
        // считаем изменение точки смещения
        if xJ=0 then
        begin
          fDWT[xI,xK] := -fTS*fEI[xK]+fMiu*fDWT[xI,xK];
          fWT[xI,xK] := fWT[xI,xK]+fDWT[xI,xK];
        end;
      end;
      // распространяем производную на предыдущий слой
      fEA[xJ]:=xSum;
    end;
  end;
end;

constructor TNeuralNetwork.Create;
begin
  inherited Create;
  fTS := 0.95;
  fMiu := 0.1;
  fMaxNeurons:=0;
  Alpha := 0.5;
end;

// шоковая встряска сети
procedure TNeuralNetwork.Shock;
var
  xI,xJ,xK: integer;
begin
  for xI:=0 to fLC-2 do
    for xJ:=0 to fConfig[xI]-1 do
      for xK:=0 to fConfig[xI+1]-1 do
        fW[xI,xJ,xK] := fW[xI,xJ,xK]+Random*0.5-0.25;
end;

// сохранение нейросети в файле
procedure TNeuralNetwork.StoreNet;
var
  xFile: TFileStream;
  xI,xJ,xK: integer;
  xTmp: integer;
  xVal: double;
begin
  xFile :=  nil;
  try
    if FileExists(fname) then
      DeleteFile(fname);
    // открываем файл
    xFile := TFileStream.Create(fname,fmCreate or fmOpenWrite);
    // записываем в файл сигнатуру
    xFile.Write('Neural network',15);
    // записываем в файл число слоев сети
    xFile.Write(fLC,sizeof(integer));
    // записываем конфигурацию слоев
    for xI:=0 to fLC-1 do
    begin
      xTmp := fConfig[xI];
      xFile.Write(xTmp,sizeof(xTmp));
    end;
    // записываем слои в файл
    for xI:=0 to fLC-2 do
      for xJ:=0 to fConfig[xI]-1 do
        for xK:=0 to fConfig[xI+1]-1 do
        begin
          xVal := fW[xI,xJ,xK];
          xFile.Write(xVal,sizeof(xVal));
        end;
    // записываем точность
    xVal := fTS;
    xFile.Write(xVal,sizeof(xVal));
    // записываем Мю
    xVal := fMiu;
    xFile.Write(xVal,sizeof(xVal));
    // записываем смещения рабочей точки
    for xI:=0 to fLC-2 do
      for xJ:=0 to fConfig[xI+1]-1 do
      begin
        xVal := fWT[xI,xJ];
        xFile.Write(xVal,sizeof(xVal));
      end;
    // пишем параметр Alpha
    xVal := Alpha;
    xFile.Write(xVal,sizeof(xVal));  
    xFile.Destroy;
  except
    if xFile<> nil then xFile.Destroy;
  end;
end;

// чтение нейросети из файла
procedure TNeuralNetwork.RestoreNet;
var
  xFile: TFileStream;
  xI,xJ,xK: integer;
  xTmp: integer;
  xVal: double;
  xSign: string[16];
begin
  xFile := nil;
  try
    // открываем файл
    xFile := TFileStream.Create(fname,fmOpenRead);
    // читаем сигнатуру
    xFile.Read(xSign,15);
    // читаем из файла число слоев сети
    xFile.Read(xTmp,sizeof(xTmp));
    LayerCount := xTmp;
    // читаем информацию о слоях сети
    for xI:=0 to fLC-1 do
    begin
      xFile.Read(xTmp,sizeof(xTmp));
      Layers[xI] := xTmp
    end;
    // читаем слои из файла
    for xI:=0 to fLC-2 do
      for xJ:=0 to fConfig[xI]-1 do
        for xK:=0 to fConfig[xI+1]-1 do
        begin
          xFile.Read(xVal,sizeof(xVal));
          fW[xI,xJ,xK] := xVal;
        end;
    // читаем точность
    xFile.Read(xVal,sizeof(xVal));
    fTS := xVal;
    // читаем Мю
    xFile.Read(xVal,sizeof(xVal));
    fMiu := xVal;
    // читаем смещение рабочей точки
    for xI:=0 to fLC-2 do
      for xJ:=0 to fConfig[xI+1]-1 do
      begin
        xFile.Read(xVal,sizeof(xVal));
        fWT[xI,xJ] := xVal;
      end;
    // читаем параметр Alpha
    xFile.Read(xVal,sizeof(xVal));
    Alpha := xVal;
    xFile.Destroy;
  except
    if xFile<> nil then xFile.Destroy;
  end;
end;

function TNeuralNetwork.fGetW(i,j,k:integer):double;
begin
  result := fW[i,j,k];
end;

procedure TNeuralNetwork.fSetW(i,j,k:integer;W:double);
begin
  fW[i,j,k]:=W;
end;

function TNeuralNetwork.fGetWT(i,k:integer):double;
begin
  result := fWT[i,k];
end;

procedure TNeuralNetwork.fSetWT(i,k:integer;W:double);
begin
  fWT[i,k]:=W;
end;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// рассчет коррекции по ошибке
procedure TNeuralNetwork.CountError;
var
  xI,xJ,xK: integer;
  xTmp: integer;
  xSum: double;
begin
  // формируем массив ошибки для выходного слоя
  xTmp:=fConfig[fLC-1];
  dec(xTmp);
  for xK:=0 to xTmp do
    fEA[xK]:=fLayerOutput[fLC-1,xK]-Dest[xK];
  // корректируем послойно сеть
  for xI:=fLC-2 downto 0 do
  begin
    // рассчитываем вектор скорости изменения ошибки
    xTmp := fConfig[xI+1]-1;
    // xTmp - число нейронов в xI - м слое НС
    // xK - номер текущего нейрона слоя xI
    for xK:=0 to xTmp do
    begin
      fEI[xK] := fEA[xK]*DSigmoid(fLayerOutput[xI+1,xK],Alpha);
      fEA[xK] := 0;
    end;
    // определяем приращение коэффициентов
    // xJ - номер текущего входа слоя xI
    for xJ:=0 to fConfig[xI]-1 do
    begin
      xSum := 0;
      for xK:=0 to xTmp do
      begin
        xSum:=xSum + fEI[xK]*fW[xI,xJ,xK];
        fDW[xI,xJ,xK]:=-fTS*fEI[xK]*fLayerOutput[xI,xJ]+fMiu*fDW[xI,xJ,xK];
        // считаем изменение точки смещения
        if xJ=0 then
        begin
          fDWT[xI,xK] := -fTS*fEI[xK]+fMiu*fDWT[xI,xK];
        end;
      end;
      // распространяем производную на предыдущий слой
      fEA[xJ]:=xSum;
    end;
  end;
end;
// изменение весов
procedure TNeuralNetwork.CorrectW;
var
  xI,xJ,xK : integer;
begin
  // корректируем послойно сеть
  for xI:=fLC-2 downto 0 do
  begin
    for xJ:=0 to fConfig[xI]-1 do
    begin
      for xK:=0 to fConfig[xI+1]-1 do
      begin
        fW[xI,xJ,xK]:=fW[xI,xJ,xK]+fDW[xI,xJ,xK];
        fDW[xI,xJ,xK]:=0;
        // считаем изменение точки смещения
        if xJ=0 then
        begin
          fWT[xI,xK] := fWT[xI,xK]+fDWT[xI,xK];
          fDWT[xI,xK]:=0;
        end;
      end;
    end;
  end;
end;

function Sigmoid(inp:double;parm:double):double;
begin
  result := 1/(1+exp(-parm*inp));
end;

function DSigmoid(inp:double;parm:double):double;
begin
  result := parm*inp*(1-inp);
end;

// реализация класса сохраняемой нейросети
constructor TNeuralNetworkINI.Create;
begin
  inherited Create;
  Miu := 0.9;
  TeachSpeed := 0.1;
  LayerCount := 3;
  Layers[0] := 10;
  Layers[1] := 5;
  Layers[2] := 1;
end;

procedure TNeuralNetworkINI.SaveToIni;
var
  xI,xJ,xK:integer;
begin
  // пишем параметры НС
  Ini.EraseSection(SectionName);
  Ini.WriteFloat(SectionName,'TeachSpeed',TeachSpeed);
  Ini.WriteFloat(SectionName,'Miu',Miu);
  Ini.WriteFloat(SectionName,'Alpha',Alpha);
  Ini.WriteInteger(SectionName,'Epoch',Epoch);
  // сохраняем конфигурацию НС
  Ini.WriteInteger(SectionName,'CountLayers',LayerCount);
  // сохраняем информацию о слоях
  for xI:=0 to LayerCount-1 do
    Ini.WriteInteger(SectionName,'Layer_'+IntToStr(xI),Layers[xI]);
  // сохраняем информацию о весах
  for xI:=0 to LayerCount-2 do // по всем слоям сети
    for xJ:=0 to Layers[xI]-1 do
      for xK:=0 to Layers[xI+1]-1 do
        Ini.WriteFloat(SectionName,'W_'+IntToStr(xI)+'_'+IntToStr(xJ)+'_'+
                        IntToStr(xK),W[xI,xJ,xK]);
  // пишем точки смещения
  for xI:=0 to LayerCount-2 do
    for xK:=0 to Layers[xI+1]-1 do
      Ini.WriteFloat(SectionName,'WT_'+IntToStr(xI)+'_'+
                      IntToStr(xK),WT[xI,xK]);
end;

procedure TNeuralNetworkINI.LoadFromIni(var Ini:TMemIniFile;SectionName:string);
var
  xI,xJ,xK:integer;
begin
  // читаем параметры НС
  TeachSpeed :=  Ini.ReadFloat(SectionName,'TeachSpeed',0);
  Miu :=  Ini.ReadFloat(SectionName,'Miu',0);
  Alpha :=  Ini.ReadFloat(SectionName,'Alpha',0);
  // читаем конфигурацию НС
  LayerCount := Ini.ReadInteger(SectionName,'CountLayers',0);
  // читаем информацию о слоях
  for xI:=0 to LayerCount-1 do
    Layers[xI] := Ini.ReadInteger(SectionName,'Layer_'+IntToStr(xI),0);
  // читаем информацию о весах
  for xI:=0 to LayerCount-2 do // по всем слоям сети
    for xJ:=0 to Layers[xI]-1 do
      for xK:=0 to Layers[xI+1]-1 do
        W[xI,xJ,xK] := Ini.ReadFloat(SectionName,'W_'+IntToStr(xI)+'_'+
                       IntToStr(xJ)+'_'+IntToStr(xK),0);
  // читаем точки смещения
  for xI:=0 to LayerCount-2 do
    for xK:=0 to Layers[xI+1]-1 do
      WT[xI,xK] := Ini.ReadFloat(SectionName,'WT_'+IntToStr(xI)+'_'+
                   IntToStr(xK),0);
  Epoch :=  Ini.ReadInteger(SectionName,'Epoch',0);
end;
// нормализуем вход в диапазон 0...1
function NormalizeCoeff(InpDef:TNetworkInputDef):double;
var
  xTmp:double;
begin
  with InpDef do
  begin
    xTmp := Value;
    // нормализуем
    case NormType of
      ntMID : xTmp := (xTmp-Min)/(Max-Min);
      ntEXP : xTmp := 1/(1+exp(-Alpha*xTmp));
      ntAUTO: xTmp := 1/(1+exp(-(xTmp-Mid)/Disp));
    end;
  end;
  result := xTmp;
end;
// денормализация в исходный масштаб
function DenormalizeCoeff(InpDef:TNetworkInputDef):double;
var
  xTmp : double;
begin
  with InpDef do
  begin
    xTmp := Value;
    case NormType of
      ntMID: xTmp := xTmp*(Max-Min)+Min;
      ntEXP: xTmp := -ln(1/xTmp-1)/Alpha;
      ntAUTO:
      begin
        xTmp := -ln(1/xTmp-1);
        xTmp := xTmp*Disp+Mid;
      end;
    end;
  end;
  result := xTmp;
end;

constructor TNetworkInputDef.Create;
begin
  inherited Create;
end;

constructor TWizardNeuralNetwork.Create;
begin
  inherited Create;
  fInputsList := TStringList.Create;
  fOutputsList := TStringList.Create;
end;

function TWizardNeuralNetwork.fGetInputValues;
begin
  if fInputsList.IndexOf(InputName)=-1 then
    raise Exception.Create('$AVZ0163 '+InputName+' $AVZ0488');
  result := (fInputsList.Objects[fInputsList.IndexOf(InputName)] as TNetworkInputDef)
             .Value;
end;

procedure TWizardNeuralNetwork.fSetInputValues(InputName:string;InputValue:double);
begin
  if fInputsList.IndexOf(InputName)=-1 then
    raise Exception.Create('$AVZ0163 '+InputName+' $AVZ0488');
  (fInputsList.Objects[fInputsList.IndexOf(InputName)] as TNetworkInputDef).Value :=
  InputValue;
end;

function TWizardNeuralNetwork.fGetOutputValues(OutputName:string):double;
begin
  if fOutputsList.IndexOf(OutputName)=-1 then
    raise Exception.Create('$AVZ0163 '+OutputName+' $AVZ0488');
  result := (fOutputsList.Objects[fOutputsList.IndexOf(OutputName)] as TNetworkInputDef)
             .Value;
end;

procedure TWizardNeuralNetwork.fSetOutputValues(OutputName:string;OutputValue:double);
begin
  if fOutputsList.IndexOf(OutputName)=-1 then
    raise Exception.Create('$AVZ0163 '+OutputName+' $AVZ0488');
  (fOutputsList.Objects[fOutputsList.IndexOf(OutputName)] as TNetworkInputDef).Value :=
  OutputValue;
end;

procedure TWizardNeuralNetwork.AddOutput;
begin
  if fOutputsList.IndexOf(OutputDef.InputName)<>-1 then
    raise Exception.Create('$AVZ0196 '+OutputDef.InputName+' $AVZ1099');
  fOutputsList.AddObject(OutputDef.InputName,OutputDef);
  // перестраиваем нейросеть
  Layers[LayerCount-1] := fOutputsList.Count;
end;

procedure TWizardNeuralNetwork.DelOutput;
begin
  if fOutputsList.IndexOf(OutputName)=-1 then
    raise Exception.Create('$AVZ0163 '+OutputName+' $AVZ0488');
  fOutputsList.Delete(fOutputsList.IndexOf(OutputName));
  // перстраиваем нейросеть
  Layers[LayerCount-1] := fOutputsList.Count;
end;

procedure TWizardNeuralNetwork.AddInput;
begin
  if fInputsList.IndexOf(InputDef.InputName)<>-1 then
    raise Exception.Create('$AVZ0163 '+InputDef.InputName+' $AVZ1099');
  fInputsList.AddObject(InputDef.InputName,InputDef);
  // перестраиваем нейросеть
  Layers[0] := fInputsList.Count;
end;

procedure TWizardNeuralNetwork.DelInput;
begin
  if fInputsList.IndexOf(InputName)=-1 then
    raise Exception.Create('$AVZ0163 '+InputName+' $AVZ0488');
  fInputsList.Delete(fInputsList.IndexOf(InputName));
  // перстраиваем нейросеть
  Layers[0] := fInputsList.Count;
end;


procedure TWizardNeuralNetwork.LoadFromWizardFile(FileName:string; ALines : TStrings);
var
  xCount,xI,xFieldType : integer;
  xFieldsInfo : TNetworkInputDef;
  xIniFile: TMemIniFile;
  begin
  xIniFile := TMemIniFile.Create(FileName);
  // читаем из файла, который создан Wizard-ом
  if ALines <> nil then begin
   xIniFile.Clear;
   xIniFile.SetStrings(ALines);
  end;
  // восстанавливаем число доступных полей
  xCount:= xIniFile.ReadInteger('Phase2','AvailableFieldsCount',0);
  // Читаем поля
  fInputsList.Clear;
  fOutputsList.Clear;
  for xI:=0 to xCount-1 do
  begin
    xFieldsInfo := TNetworkInputDef.Create;
    xFieldType := xIniFile.ReadInteger('Phase2','FieldType_'+
                                IntToStr(xI),0);
    xFieldsInfo.InputName := xIniFile.ReadString('Phase2','FieldName_'+
                                IntToStr(xI),'');
    xFieldsInfo.NormType := xIniFile.ReadInteger('Phase2','NormType_'+
                               IntToStr(xI),0);
    xFieldsInfo.Max := xIniFile.ReadFloat('Phase2','MAX_'+IntToStr(xI),0);
    xFieldsInfo.Min := xIniFile.ReadFloat('Phase2','MIN_'+IntToStr(xI),0);
    xFieldsInfo.Mid := xIniFile.ReadFloat('Phase2','MID_'+IntToStr(xI),0);
    xFieldsInfo.Disp := xIniFile.ReadFloat('Phase2','DISP_'+IntToStr(xI),0);
    xFieldsInfo.Alpha := xIniFile.ReadFloat('Phase2','ALPHA_'+IntToStr(xI),0);
    case xFieldType of
      0: AddInput(xFieldsInfo);
      1: AddOutput(xFieldsInfo);
    end;
  end;
  // восстанавливаем результат конфигурации
  // читаем число скрытых слоев НС
  LayerCount := xIniFile.ReadInteger('Phase3','HideLayers',1)+2;
  Layers[0] := fInputsList.Count;
  // читаем конфигурацию слоев НС
  for xI:=1 to LayerCount-2 do
    Layers[xI] := xIniFile.ReadInteger('Phase3','Layer_'+IntToStr(xI),5);
  Layers[LayerCount-1] := fOutputsList.Count;
  // читаем параметр сигмодиды
  Alpha := xIniFile.ReadFloat('Phase3','Alpha',1);
  // продолжим чтение коэффициентов сети
  LoadFromIni(xIniFile,'Network');
  // закрываемся
  xIniFile.Free;
end;

procedure TWizardNeuralNetwork.StoreToFile;
var
  xCount,xI   : integer;
  xFieldsInfo : TNetworkInputDef;
  IniFile     : TIniFile;
begin
  // пробуем сохранить данные, в формате, совместимом с визардом :)
  IniFile := TIniFile.Create(FileName);
  // записываем число доступных полей
  xCount := fInputsList.Count+fOutputsList.Count;
  IniFile.WriteInteger('Phase2','AvailableFieldsCount',xCount);
  // cохраняем входы
  for xI:=0 to fInputsList.Count-1 do
  begin
    xFieldsInfo := fInputsList.Objects[xI] as TNetworkInputDef;
    IniFile.WriteInteger('Phase2','FieldType_'+
                                IntToStr(xI),0);
    IniFile.WriteString('Phase2','FieldName_'+IntToStr(xI),xFieldsInfo.InputName);
    IniFile.WriteInteger('Phase2','NormType_'+IntToStr(xI),xFieldsInfo.NormType);
    IniFile.WriteFloat('Phase2','MAX_'+IntToStr(xI),xFieldsInfo.Max);
    IniFile.WriteFloat('Phase2','MIN_'+IntToStr(xI),xFieldsInfo.Min);
    IniFile.WriteFloat('Phase2','ALPHA_'+IntToStr(xI),xFieldsInfo.Alpha);
  end;
  // сохраняем выходы
  for xI:=fInputsList.Count to fInputsList.Count+fOutputsList.Count-1 do
  begin
    xFieldsInfo := fInputsList.Objects[xI-fInputsList.Count] as TNetworkInputDef;
    IniFile.WriteInteger('Phase2','FieldType_'+
                                IntToStr(xI),1);
    IniFile.WriteString('Phase2','FieldName_'+IntToStr(xI),xFieldsInfo.InputName);
    IniFile.WriteInteger('Phase2','NormType_'+IntToStr(xI),xFieldsInfo.NormType);
    IniFile.WriteFloat('Phase2','MAX_'+IntToStr(xI),xFieldsInfo.Max);
    IniFile.WriteFloat('Phase2','MIN_'+IntToStr(xI),xFieldsInfo.Min);
    IniFile.WriteFloat('Phase2','ALPHA_'+IntToStr(xI),xFieldsInfo.Alpha);
  end;
  // пишем число скрытых слоев НС
  IniFile.WriteInteger('Phase3','HideLayers',LayerCount-2);
  // пишем конфигурацию слоев НС
  for xI:=1 to LayerCount-1 do
    IniFile.WriteInteger('Phase3','Layer_'+IntToStr(xI),Layers[xI]);
  // пишем параметр сигмодиды
  IniFile.WriteFloat('Phase3','Alpha',Alpha);
  // продолжим чтение коэффициентов сети
  SaveToIni(IniFile,'Network');
  IniFile.Free;
end;

procedure TWizardNeuralNetwork.Compute;
var
  xI:integer;
begin
  // заполняем входы нейросети
  for xI:=0 to fInputsList.Count-1 do
  begin
    Input[xI] := NormalizeCoeff((fInputsList.Objects[xI] as TNetworkInputDef))*2-1;
  end;
  inherited Compute;
  // читаем выходы нейросети
  for xI:=0 to fOutputsList.Count-1 do
  begin
   (fOutputsList.Objects[xI] as TNetworkInputDef).Value := Output[xI];
   (fOutputsList.Objects[xI] as TNetworkInputDef).Value :=
      DenormalizeCoeff((fOutputsList.Objects[xI] as TNetworkInputDef));
  end;
end;

procedure TWizardNeuralNetwork.Learn;
var
  xI:integer;
    xTmp: array of double;
begin
  // заполняем входы нейросети
  for xI:=0 to fInputsList.Count-1 do
  begin
    Input[xI] := NormalizeCoeff((fInputsList.Objects[xI] as TNetworkInputDef))*2-1;
  end;
  // определяем выходы
  SetLength(xTmp,fOutputsList.Count);
  for xI:=0 to fOutputsList.Count-1 do
  begin
    xTmp[xI] := NormalizeCoeff((fOutputsList.Objects[xI] as TNetworkInputDef));
  end;
  // рассчитаем выходы сети
  Compute;
  // рассчитаем ошибку
  fError := 0;
  for xI:=0 to fOutputsList.Count-1 do
    fError := fError + sqr(xTmp[xI]-Output[xI]);
  for xI:=0 to fOutputsList.Count-1 do
  begin
   (fOutputsList.Objects[xI] as TNetworkInputDef).Value := Output[xI];
   (fOutputsList.Objects[xI] as TNetworkInputDef).Value :=
      DenormalizeCoeff((fOutputsList.Objects[xI] as TNetworkInputDef));
  end;
  // и обучим ее
  Teach(xTmp);
end;

initialization
  DecimalSeparator := '.';
end.

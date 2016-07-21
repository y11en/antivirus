unit zFilterCmp;
// Компонент для визуальной фильтрации данных
// (C) Зайцев О.В., 2004-2006
interface
uses classes, stdctrls, Forms, extctrls, Sysutils, Graphics, controls, dialogs, buttons;

type

 // Типы данных
 TFilterEditType = (fetNumber, fetNumberList, fetNumberDiapazon, fetString, fetDate, fetDateDiapazon, fetNone);
 // Режимы работы фильтра
 TFilterEditMode = (femNumber, femNumberCode, femString, femDate, femCustom);

 // Элемент фильтра
 TFilterItem = class
  Caption   : string;           // Заголовок
  EditType  : TFilterEditType;  // Тип радактора для ввода
  WhereMask : string;           // Маска для формирования условия
  Help      : string;           // Подсказка
  Flags     : byte;             // Битовая маска, управляющая поведением редактора
  {                             Бит   Вес    Назначение
                                 0     1     Видимость полей редактирования
                                 1     2     Видимость полей редактирования
                                 2     4     Признак того, что в этом режиме не нужен автоконтроль
                                 3     8
                                 4    16
                                 5    32
  }
 end;

 TOnButtonClickEvent = procedure(Sender : TObject; ButtonId : byte) of object;
 TFilterEditTranslateEvent = function(Sender : TObject; AStr : string) : string of object;
 // Злемент ввода
 TFilterEdit = class(TCustomPanel)
  private
    FFilterEditMode: TFilterEditMode;
    FFilterCaption: String;
    FFilterCaptionWidth: integer;
    FFilterComboWidth: integer;
    FFieldName: string;
    FAutoValidate: boolean;
    FDefaultFilterItem: integer;
    FFilterItemsNull: boolean;
    FFilterReadOnly: boolean;
    FHiddenFilterText1: string;
    FHiddenFilterText2: string;
    FHiddenValuesMode: boolean;
    FButtonsVisible: boolean;
    FOnButtonClick: TOnButtonClickEvent;
    FButtonsHint: string;
    FFilterEditTranslateEvent: TFilterEditTranslateEvent;
    procedure SetFilterEditMode(const Value: TFilterEditMode);
    procedure SetFilterCaption(const Value: String);
    procedure SetFilterCaptionWidth(const Value: integer);
    procedure SetFilterComboWidth(const Value: integer);
    function  GetFilterOn: boolean;
    // Настройка состояния включено/выключено
    procedure SetFilterOn(const Value: boolean);
    // Реакция на включение/выключени фильтрации
    procedure FOnCaptionLabelClick(Sender: TObject);
    // Реакция на смену условия фильтрации
    procedure FOnComboBoxClick(Sender: TObject);
    // Реакция на редактирование условия
    procedure FOnEditChange(Sender: TObject);
    // Получение текущего элемента списка
    function  GetTekFilterItem: TFilterItem;
    // Проверить числовое значение
    function  CheckNumberValue(S : string) : boolean;
    // Проверить список числовых значений
    function  CheckNumberValues(S : string) : boolean;
    // Проверить числовое значение
    function  CheckDateValue(S : string) : boolean;
    // Проверить числовое значение
    function  CheckDateValues(S : string) : boolean;
    // Установить режим автопроверки
    procedure SetAutoValidate(const Value: boolean);
    function GetButtonVisible: boolean;
    function GetTextStateDescr: string;
    procedure SetTextStateDescr(const Value: string);
    function GetTekFilterItemIndex: integer;
    function GetFilterText1: string;
    function GetFilterText2: string;
    procedure SetFilterText1(const Value: string);
    procedure SetFilterText2(const Value: string);
    procedure SetDefaultFilterItem(const Value: integer);
    procedure SetFilterItemsNull(const Value: boolean);
    procedure SetFilterReadOnly(const Value: boolean);
    procedure SetHiddenFilterText1(const Value: string);
    procedure SetHiddenFilterText2(const Value: string);
    procedure SetHiddenValuesMode(const Value: boolean);
    procedure SetButtonsVisible(const Value: boolean);
    procedure SetOnButtonClick(const Value: TOnButtonClickEvent);
    procedure SpeedButtonClick(Sender: TObject);
    procedure SetButtonsHint(const Value: string);
    procedure SetFilterEditTranslateEvent(
      const Value: TFilterEditTranslateEvent);
  protected
   FEdit1OK, FEdit2OK : boolean; // Результаты проверки значения 1 и 2
   FFilterEdit1b : TSpeedButton; // Кнопка редактора 1
   FFilterEdit2b : TSpeedButton; // Кнопка редактора 2
   FDopLabel     : TLabel;       // Метка с изображением ... для разделения полей диапазона
   // Выполнить выравнивание
   Procedure   AlignElements;
   // Рисование (применяется для вывода подчеркивания)
   procedure Paint; override;
   // Масштабирование
   procedure Resize; override;
   // Рисование красной волнистой линии под указанным компонентом
   procedure UnderlineComponent(ACmp : TControl);
   function  DoTranslate(S : string) : string;
  public
   FFilterCombo  : TComboBox;    // Условие
   FCaptionLabel : TCheckBox;    // Переключатель-метка
   FFilterEdit1  : TEdit;        // Редактор для ввода текста 1
   FFilterEdit2  : TEdit;        // Редактор для ввода текста 2
   ErrorMessage : string; // Сообщение об ошибке
   // Создание класса
   Constructor Create(AOwner: TComponent); override;
   // Разрушение класса
   Destructor  Destroy; override;
   // Добавление варианта фильтрации
   Procedure   AddFilterItem(ACaption: string; AEditType: TFilterEditType; AWhereMask : string; AFlags : byte; AComment : string = '');
   // Удаление элемента фильтра
   Procedure   DeleteFilterItems;
   // Проверка значения
   Function    ValidateValue : boolean;
   // Формирования SQL условия согласно стандарту Oracle
   Function    GetSQL : string;
   // Признак видимости кнопок
   property    ButtonVisible : boolean read GetButtonVisible;
   // Текстовое описание
   property    TextStateDescr : string read GetTextStateDescr write SetTextStateDescr;
  published
   // Режим фильтра
   property FilterEditMode  : TFilterEditMode read FFilterEditMode write SetFilterEditMode;
   // Заголовок
   property Caption : String read FFilterCaption write SetFilterCaption;
   // Ширина заголовка
   property FilterCaptionWidth : integer     read FFilterCaptionWidth write SetFilterCaptionWidth;
   // Ширина списка вариантов фильтрации
   property FilterComboWidth   : integer     read FFilterComboWidth   write SetFilterComboWidth;
   // Состояние фильтра (включено-выключено)
   property FilterOn           : boolean     read GetFilterOn         write SetFilterOn;
   // Указатель на текущий элемент или nil, если текущего элемента нет
   property TekFilterItem      : TFilterItem read GetTekFilterItem;
   // Указатель на текущий элемент или nil, если текущего элемента нет
   property TekFilterItemIndex : integer read GetTekFilterItemIndex;
   // Имя поля или выражение, считаемое источником для фильтра
   property FieldName          : string read FFieldName write FFieldName;
   // Прямой доступ к тексту строк с образцом фильтрации на чтение и запись
   property FilterText1        : string read GetFilterText1 write SetFilterText1;
   property FilterText2        : string read GetFilterText2 write SetFilterText2;
   // Доступ к скрытым значениям (они используются вместо видимых при включении HiddenValuesMode)
   property HiddenFilterText1  : string read FHiddenFilterText1 write SetHiddenFilterText1;
   property HiddenFilterText2  : string read FHiddenFilterText2 write SetHiddenFilterText2;
   // Режим "Только чтение" для полей фильтра. Полезно, если образцы вносятся в поля каким-то особым способом
   property FilterReadOnly     : boolean read FFilterReadOnly write SetFilterReadOnly;
   // Автопроверка (при каждом изменении)
   property AutoValidate : boolean read FAutoValidate write SetAutoValidate;
   // Индекс типа фильтрации по умолчанию (индекс отсчитывается с 0)
   property DefaultFilterItem  : integer read FDefaultFilterItem write SetDefaultFilterItem;
   // Включает дополнительнеы условия для NULL с стандартных фильтрах
   property FilterItemsNull    : boolean read FFilterItemsNull write SetFilterItemsNull;
   // Режим "скрытых значений"  - в нем вместо видимых используются скрытые значения. Удобно для справочников
   property HiddenValuesMode   : boolean read FHiddenValuesMode write SetHiddenValuesMode;
   // Признак того, что кнопки видимы
   property ButtonsVisible     : boolean read FButtonsVisible write SetButtonsVisible;
   // Подсказка для кнопок
   property ButtonsHint        : string read FButtonsHint write SetButtonsHint;
   // Событие "нажатие на кнопку"
   property OnButtonClick      : TOnButtonClickEvent read FOnButtonClick write SetOnButtonClick;
   // Событие "требуется локализация строки"
   property  FilterEditTranslateEvent : TFilterEditTranslateEvent read FFilterEditTranslateEvent write SetFilterEditTranslateEvent;
   // Стандартные свойства - рамка и HINT
   property BevelInner;
   property BevelOuter;
   property ShowHint;
   property Anchors;
 end;

procedure Register;
implementation

procedure Register;
begin
  RegisterComponents('Data Controls', [TFilterEdit]);
end;

// Создание класса
constructor TFilterEdit.Create(AOwner: TComponent);
begin
 inherited;
 Height := 32;
 Width  := 400;
 FFilterEditTranslateEvent := nil;
 FFilterEdit1b  := nil;
 FFilterEdit2b  := nil;
 FOnButtonClick := nil;
 FFilterReadOnly := false;
 FHiddenFilterText1 := '';
 FHiddenFilterText2 := '';
 FHiddenValuesMode  := false;
 FButtonsVisible    := false;
 FFilterCaptionWidth  := 70;
 FFilterComboWidth    := 150;
 BorderWidth := 0;
 BorderStyle := bsNone;
 FAutoValidate := false;
 FFilterItemsNull := false;
 // Создание вложенных компонент
 FCaptionLabel := TCheckBox.Create(self);
 FFilterCombo  := TComboBox.Create(self);
 FFilterEdit1  := TEdit.Create(self);
 FFilterEdit2  := TEdit.Create(self);
 FDopLabel     := TLabel.Create(self);

 // Инициализация надписи с чекбоксом
 FCaptionLabel.Visible  := true;
 FCaptionLabel.Caption  := FFilterCaption;
 FCaptionLabel.Parent   := self;
 FCaptionLabel.OnClick  := FOnCaptionLabelClick;
 FCaptionLabel.Name     := 'FilterCaption';

 // Инициализация списка вариантов фильтра
 FFilterCombo.Visible  := true;
 FFilterCombo.Parent   := self;
 FFilterCombo.Style    := csDropDownList;
 FFilterCombo.OnClick  := FOnComboBoxClick;
 FFilterCombo.Top      := 1;
 FFilterCombo.Name     := 'FFilterCombo';

 // Инициализация редактора текста 1
 FFilterEdit1.Visible  := true;
 FFilterEdit1.Parent   := self;
 FFilterEdit1.OnChange := FOnEditChange;
 FFilterEdit1.Name     := 'FFilterEdit';
 FFilterEdit1.Top      := 1;
 FFilterEdit1.Text     := '';

 // Инициализация редактора текста 2
 FFilterEdit2.Parent   := self;
 FFilterEdit2.OnChange := FOnEditChange;
 FFilterEdit2.Name     := 'FFilterEdit1';
 FFilterEdit2.Top      := 1;
 FFilterEdit2.Text     := '';
 FFilterEdit2.Visible  := false;

 FDopLabel.Name     := 'FDopLabel';
 FDopLabel.Visible  := false;
 FDopLabel.AutoSize := true;
 FDopLabel.Caption  := '...';
 FDopLabel.Parent   := self;


 inherited Caption := '';
 // Выравнивание элементов
 AlignElements;
end;

// Разрушение класса
destructor TFilterEdit.Destroy;
begin
  inherited;
end;

procedure TFilterEdit.AlignElements;
var
 BtnWidth : integer;
begin
 // Настройка метки
 FCaptionLabel.Left  := 2; FCaptionLabel.Top := FFilterCombo.Height - FCaptionLabel.Height+3;
 FCaptionLabel.Width := FilterCaptionWidth;

 // Настройка списка
 FFilterCombo.Left  := FFilterCaptionWidth+1;
 FFilterCombo.Width := FFilterComboWidth;
 // Ширина кнопки (для режима с кнопкой справа от полей редактирования
 BtnWidth := 0;
 // Создание/разрушение кнопок по мере необходимости
 if FButtonsVisible then begin
  BtnWidth := 20;
  // Первая кнопка
  if not(Assigned(FFilterEdit1b)) then begin
   FFilterEdit1b := TSpeedButton.Create(Self);
   FFilterEdit1b.Visible := FFilterEdit1.Visible;
   FFilterEdit1b.Height  := FFilterEdit1.Height-3;
   FFilterEdit1b.Top     := FFilterEdit1.Top+2;
   FFilterEdit1b.Width   := BtnWidth;
   FFilterEdit1b.Caption := '...'; FFilterEdit1b.Font.Style := [fsBold];
   FFilterEdit1b.Enabled := FCaptionLabel.Checked;
   FFilterEdit1b.Parent  := Self;
   FFilterEdit1b.Hint    := ButtonsHint;
   FFilterEdit1b.ShowHint := true;
   FFilterEdit1b.OnClick := SpeedButtonClick;
  end;
  // Вторая кнопка
  if not(Assigned(FFilterEdit2b)) then begin
   FFilterEdit2b := TSpeedButton.Create(Self);
   FFilterEdit2b.Visible := FFilterEdit2.Visible;
   FFilterEdit2b.Height  := FFilterEdit2.Height-3;
   FFilterEdit2b.Top     := FFilterEdit2.Top+2;
   FFilterEdit2b.Width   := BtnWidth;
   FFilterEdit2b.Caption := '...'; FFilterEdit2b.Font.Style := [fsBold];
   FFilterEdit2b.Enabled := FCaptionLabel.Checked;
   FFilterEdit2b.Parent := Self;
   FFilterEdit2b.Hint    := ButtonsHint;
   FFilterEdit2b.ShowHint := true;
   FFilterEdit2b.OnClick := SpeedButtonClick;
  end;
 end else begin
  // Если кнопки не требуются, то они разрушаются, чтобы не потреблять ресурсы
  BtnWidth := 0;
  FFilterEdit1b.Free; FFilterEdit1b := nil;
  FFilterEdit2b.Free; FFilterEdit2b := nil;
 end;
 // Настройка редактора для ввода значения
 if not(FFilterEdit2.Visible) then begin  // Один редактор
  FDopLabel.Visible := false;
  FFilterEdit2.Left := -1000;
  FFilterEdit1.Left   := FFilterCombo.Left + FFilterComboWidth+2;
  FFilterEdit1.Height := FFilterCombo.Height-1;
  FFilterEdit1.Width  := Width - FFilterEdit1.Left - 3 - BtnWidth;
  if FButtonsVisible then
   FFilterEdit1b.Left := FFilterEdit1.Left + FFilterEdit1.Width+1;
 end else begin                        // Два редактора
   FDopLabel.Visible  := true;
   FDopLabel.Font.Style := [fsBold];
   FFilterEdit1.Left   := FFilterCombo.Left + FFilterComboWidth+2;
  if not(FButtonsVisible) then begin    // без кнопок
   FFilterEdit1.Width  := (Width - FFilterEdit1.Left) div 2  - FDopLabel.Width + 3;
   FDopLabel.Left     := FFilterEdit1.Left + FFilterEdit1.Width+1;
   FDopLabel.Top      := FFilterEdit1.Height - FDopLabel.Height+1;
   FFilterEdit2.Left  := FDopLabel.Left + FDopLabel.Width + 1;
   FFilterEdit2.Width := FFilterEdit1.Width;
   FFilterEdit1.Height := FFilterCombo.Height-1;
   FFilterEdit2.Height := FFilterCombo.Height-1;
  end else begin                      // С кнопками
   FFilterEdit1.Width  := (Width - FFilterEdit1.Left - BtnWidth*2) div 2  - FDopLabel.Width + 3;
   FFilterEdit1b.Left  :=  FFilterEdit1.Left + FFilterEdit1.Width+1;
   FDopLabel.Left     := FFilterEdit1b.Left + FFilterEdit1b.Width+1;
   FDopLabel.Top      := FFilterEdit1.Height - FDopLabel.Height+1;
   FFilterEdit2.Left  := FDopLabel.Left + FDopLabel.Width + 1;
   FFilterEdit2.Width := FFilterEdit1.Width;
   FFilterEdit1.Height := FFilterCombo.Height-1;
   FFilterEdit2.Height := FFilterCombo.Height-1;
   FFilterEdit2b.Left  :=  FFilterEdit2.Left + FFilterEdit2.Width+1;
   FFilterEdit2b.Visible := true;
  end;
 end;
 // Перерисовка после выравнивания
 Repaint;
end;

procedure TFilterEdit.SetFilterCaption(const Value: String);
begin
  FFilterCaption := Value;
  FCaptionLabel.Caption := FFilterCaption;
end;

procedure TFilterEdit.SetFilterEditMode(const Value: TFilterEditMode);
begin
  FFilterEditMode := Value;
  DeleteFilterItems;
  case FFilterEditMode of
   femNumber : begin
                DeleteFilterItems;
                AddFilterItem('$AVZ0887',     fetNumberList,     '{FLD} IN ({X})',     1, '$AVZ0230');
                AddFilterItem('$AVZ0485', fetNumberList,     '{FLD} NOT IN ({X})', 1, '$AVZ0230');
                AddFilterItem('$AVZ0079',    fetNumber,         '{FLD} > {X}',        1);
                AddFilterItem('$AVZ0402',    fetNumber,         '{FLD} < {X}',        1);
                AddFilterItem('$AVZ0088',   fetNumberDiapazon, '{FLD} BETWEEN {X1} AND {X2}',     2);
                AddFilterItem('$AVZ0112', fetNumberDiapazon, '{FLD} NOT BETWEEN {X1} AND {X2}', 2);
                if FFilterItemsNull then begin
                 AddFilterItem('$AVZ0273',   fetNumber, '{FLD} IS NOT NULL', 0 + 4);
                 AddFilterItem('$AVZ0474', fetNumber, '{FLD} IS NULL', 0 + 4);
                end;
               end;
   femNumberCode : begin
                DeleteFilterItems;
                AddFilterItem('$AVZ0887',     fetNumberList,     '{FLD} IN ({X})',     1, '$AVZ0230');
                AddFilterItem('$AVZ0485', fetNumberList,     '{FLD} NOT IN ({X})', 1, '$AVZ0230');
                if FFilterItemsNull then begin
                 AddFilterItem('$AVZ0273',   fetNumber, '{FLD} IS NOT NULL', 0 + 4);
                 AddFilterItem('$AVZ0474', fetNumber, '{FLD} IS NULL', 0 + 4);
                end;
               end;
   femString : begin
                DeleteFilterItems;
                AddFilterItem('$AVZ1012',             fetString, 'UPPER({FLD}) LIKE UPPER(''%''||''{X}''||''%'')', 1);
                AddFilterItem('$AVZ0486',          fetString, 'UPPER({FLD}) NOT LIKE UPPER(''%''||''{X}''||''%'')', 1);
                AddFilterItem('$AVZ0467',     fetString, 'UPPER({FLD}) LIKE UPPER(''{X}''||''%'')', 1);
                AddFilterItem('$AVZ0265 ...', fetString, 'UPPER({FLD}) LIKE UPPER(''%''||''{X}'')', 1);
                AddFilterItem('$AVZ0886',                fetString, 'UPPER({FLD}) LIKE UPPER(''{X}'')', 1);
                AddFilterItem('$AVZ0484',             fetString, 'UPPER({FLD}) LIKE UPPER(''{X}'')', 1);
                if FFilterItemsNull then begin
                 AddFilterItem('$AVZ0273',   fetString, 'LTRIM({FLD}) IS NOT NULL', 4);
                 AddFilterItem('$AVZ0474', fetString, 'LTRIM({FLD}) IS NULL', 4);
                end;
               end;
   femDate : begin
                DeleteFilterItems;
                AddFilterItem('$AVZ0887',     fetDate, 'TRUNC({FLD}) =  TO_DATE(''{X}'',''DD.MM.YYYY'')', 1, '$AVZ1132');
                AddFilterItem('$AVZ0485', fetDate, 'TRUNC({FLD}) != TO_DATE(''{X}'',''DD.MM.YYYY'')', 1, '$AVZ1132');
                AddFilterItem('$AVZ0079',    fetDate, 'TRUNC({FLD}) > TO_DATE(''{X}'',''DD.MM.YYYY'')', 1);
                AddFilterItem('$AVZ0402',    fetDate, 'TRUNC({FLD}) < TO_DATE(''{X}'',''DD.MM.YYYY'')', 1);
                AddFilterItem('$AVZ0088',   fetDateDiapazon, 'TRUNC({FLD}) BETWEEN TO_DATE(''{X1}'',''DD.MM.YYYY'') AND TO_DATE(''{X2}'',''DD.MM.YYYY'')', 2);
                AddFilterItem('$AVZ0112', fetDateDiapazon, 'TRUNC({FLD}) NOT BETWEEN TO_DATE(''{X1}'',''DD.MM.YYYY'') AND TO_DATE(''{X2}'',''DD.MM.YYYY'')', 2);
                if FFilterItemsNull then begin
                 AddFilterItem('$AVZ0273',   fetDate, '{FLD} IS NOT NULL', 4);
                 AddFilterItem('$AVZ0474', fetDate, '{FLD} IS NULL', 4);
                end;
               end;
   femCustom : begin
                 DeleteFilterItems;
               end;
  end;
  // Выбор элемента по умолчанию
  if (FFilterCombo.Items.Count > 0) and
     (FFilterCombo.ItemIndex < 0) then begin
      FFilterCombo.ItemIndex := 0;
      if DefaultFilterItem > 0 then
       try FFilterCombo.ItemIndex := DefaultFilterItem; except end;
     end;
end;

procedure TFilterEdit.SetFilterCaptionWidth(const Value: integer);
begin
 FFilterCaptionWidth := Value;
 AlignElements;
end;

procedure TFilterEdit.SetFilterComboWidth(const Value: integer);
begin
 FFilterComboWidth := Value;
 AlignElements;
end;

function TFilterEdit.GetFilterOn: boolean;
begin
 Result := FCaptionLabel.Checked;
end;

procedure TFilterEdit.SetFilterOn(const Value: boolean);
begin
 FCaptionLabel.Checked := Value;
 FFilterCombo.Enabled  := Value;
 FFilterEdit1.Enabled   := Value;
 FFilterEdit2.Enabled  := Value;
 if FButtonsVisible then begin
  if FFilterEdit1b <> nil then
   FFilterEdit1b.Enabled   := Value;
  if FFilterEdit2b <> nil then
   FFilterEdit2b.Enabled  := Value;
 end;
 Repaint;
end;

// Реакция на включение/выключени фильтрации
procedure TFilterEdit.FOnCaptionLabelClick(Sender: TObject);
begin
 SetFilterOn(FCaptionLabel.Checked);
end;

// Добавление варианта фильтрации
procedure TFilterEdit.AddFilterItem(ACaption: string; AEditType: TFilterEditType; AWhereMask : string;  AFlags : byte; AComment : string = '');
var
 Tmp : TFilterItem;
begin
 // Создание экземпляра класса "фильтр"
 Tmp := TFilterItem.Create;
 ACaption := DoTranslate(ACaption);
 //  Заполнение полей
 Tmp.Caption   := ACaption;
 Tmp.EditType  := AEditType;
 Tmp.WhereMask := AWhereMask;
 Tmp.Flags     := AFlags;
 // Добавление
 FFilterCombo.Items.AddObject(ACaption, Tmp);
end;

procedure TFilterEdit.DeleteFilterItems;
begin
 FFilterCombo.Items.Clear;
end;

// Получение текущего элемента списка
function TFilterEdit.GetTekFilterItem: TFilterItem;
begin
 Result := nil;
 // Ошибки при получении указателя игнорируются
 try
  if (FFilterCombo.Items.Count > 0) and (FFilterCombo.ItemIndex >= 0) then
   Result := TFilterItem(FFilterCombo.Items.Objects[FFilterCombo.ItemIndex]);
 except end;
end;

procedure TFilterEdit.FOnComboBoxClick(Sender: TObject);
begin
 // Настройка видимости элементов
 if (TekFilterItem = nil) then begin
  FFilterEdit2.Visible := false;
 end else begin
  case (TekFilterItem.Flags and 3) of
   0 : begin                               // Поля ввода не требуются
        FFilterEdit2.Visible := false;
        FFilterEdit1.Visible  := false;
       end;
   1 : begin                               // Одно поле ввода
        FFilterEdit2.Visible := false;
        FFilterEdit1.Visible  := true;
       end;
   2 : begin                               // Два поля ввода
        FFilterEdit2.Visible := true;
        FFilterEdit1.Visible  := true;
       end;
  end;
 end;
 // Выравнивание и настройка внешнего вида
 AlignElements;
 // Автопроверка
 if AutoValidate then
  ValidateValue;
 Repaint;
end;

function TFilterEdit.ValidateValue: boolean;
var
 Res : boolean;
begin
 try
   Result := false;
   FEdit1OK := false; FEdit2OK := false;
   ErrorMessage := '';
   // Тип фильтрации
   if (TekFilterItem = nil) then  begin
    ErrorMessage := '$AVZ0471';
    exit;
   end;
   // Блокиратор контроля полей
   if (TekFilterItem.Flags and 4) > 0 then begin
    Result := true;
    exit;
   end;
   // Строка
   if TekFilterItem.EditType = fetString then begin
    if not(Trim(FilterText1) <> '') then begin
     ErrorMessage := '$AVZ0469';
     exit;
    end;
   end;
   // Одно число
   if TekFilterItem.EditType = fetNumber then begin
    if not CheckNumberValue(FilterText1) then begin
     ErrorMessage := '$AVZ0510';
     exit;
    end;
   end;
   // Список чисел
   if TekFilterItem.EditType = fetNumberList then begin
    if not CheckNumberValues(FilterText1) then begin
     ErrorMessage := '$AVZ0511';
     exit;
    end;
   end;
   // Диапазон чисел
   if TekFilterItem.EditType = fetNumberDiapazon then begin
    Res := true;
    if not CheckNumberValue(FilterText1) then begin
     ErrorMessage := '$AVZ0507';
     Res := false;
    end else FEdit1OK := true;
    if not CheckNumberValue(FilterText2) then begin
     ErrorMessage := '$AVZ0506';
     Res := false;
    end else FEdit2OK := true;
    if not(res) then exit;
    if StrToFloat(Trim(FilterText1)) > StrToFloat(Trim(FilterText2)) then begin
     ErrorMessage := '$AVZ0354';
     FEdit1OK := false; FEdit2OK := false;
    end;
   end;
   // Одна дата
   if TekFilterItem.EditType = fetDate then begin
    if not CheckDateValue(FilterText1) then begin
     ErrorMessage := '$AVZ0503';
     exit;
    end;
   end;
   // Диапазон дат
   if TekFilterItem.EditType = fetDateDiapazon then begin
    if not CheckDateValue(FilterText1) then begin
     ErrorMessage := '$AVZ0505';
     exit;
    end else FEdit1OK := true;
    if not CheckDateValue(FilterText2) then begin
     ErrorMessage := '$AVZ0504';
     exit;
    end else FEdit2OK := true;
   end;
   Result := true;
   FEdit1OK := true; FEdit2OK := true;
 finally
  ErrorMessage := DoTranslate(ErrorMessage);
  FFilterEdit1.Hint := ErrorMessage;
  FFilterEdit2.Hint := ErrorMessage;
 end;
end;

// Проверить числовое значение
function TFilterEdit.CheckNumberValue(S: string): boolean;
begin
 result := false;
 S := trim(s);
 // Проверка заполенения
 if S = '' then exit;
 // Проверка корректности
 try
  S := StringReplace(S, '.', DecimalSeparator, []);
  StrToFloat(S);
  Result := true;
 except
  // Исключение возникнет в случае ошибки конвертации строки в число
 end;
end;

// Проверить список числовых значений
function TFilterEdit.CheckNumberValues(S: string): boolean;
var
 st : string;
begin
 Result := false;
 s := trim(s) + ',';
 while pos(',', s) > 0 do begin
  st := copy(s, 1, pos(',', s)-1);
  if not CheckNumberValue(st) then
   exit;
  Delete(s, 1, pos(',', s));
 end;
 Result := true;
end;

// Сформировать условие
function TFilterEdit.GetSQL: string;
var
 Text1, Text2 : string;
begin
 Result := '';
 if not(FCaptionLabel.Checked) or not(ValidateValue) then exit;
 result := TekFilterItem.WhereMask;
 Text1 := Trim(FilterText1);
 Text2 := Trim(FilterText2);
 if FilterEditMode =  femString then begin
  Text1 := StringReplace(Text1, '*', '%',[rfReplaceAll]); Text1 := StringReplace(Text1, '?', '_', [rfReplaceAll]);
  Text2 := StringReplace(Text2, '*', '%',[rfReplaceAll]); Text2 := StringReplace(Text2, '?', '_', [rfReplaceAll]);
 end;
 // Подстановка полей
 Result := StringReplace(Result, '{FLD}', FieldName, [rfReplaceAll, rfIgnoreCase]);
 Result := StringReplace(Result, '{X}',  Text1, [rfReplaceAll, rfIgnoreCase]);
 Result := StringReplace(Result, '{X1}', Text1, [rfReplaceAll, rfIgnoreCase]);
 Result := StringReplace(Result, '{X2}', Text2, [rfReplaceAll, rfIgnoreCase]);
end;

// Рисование (применяется для вывода подчеркивания)
procedure TFilterEdit.Paint;
begin
  inherited;
  if not(FilterOn) then exit;
  if not FEdit1OK then
   UnderlineComponent(FFilterEdit1);
  if not FEdit2OK then
   UnderlineComponent(FFilterEdit2);
end;

// Рисование красной волнистой линии под указанным компонентом
procedure TFilterEdit.UnderlineComponent(ACmp: TControl);
var
 i, x, y, dy, line_width : integer;
begin
 // Для невидимого компонента рисование бессмыслено
 if not ACmp.Visible then exit;
 Canvas.pen.Color := clRed;
 line_width := ACmp.Width;
 y := ACmp.Height + ACmp.Top + 2;
 dy := 2;
 // Рисование линии
 for i := 1 to line_width div 4 do begin
  x := ACmp.Left + i*4;
  Canvas.MoveTo(x - 4, y + dy);
  Canvas.LineTo(x - 2, y - dy);
  Canvas.LineTo(x, y + dy);
 end;
end;

procedure TFilterEdit.FOnEditChange(Sender: TObject);
begin
 if csLoading in ComponentState then exit;
 if AutoValidate then begin
  ValidateValue;
  Repaint;
 end;
end;

procedure TFilterEdit.Resize;
begin
  inherited;
  AlignElements;
end;

procedure TFilterEdit.SetAutoValidate(const Value: boolean);
begin
  FAutoValidate := Value;
  if FAutoValidate then
   ValidateValue;
  Repaint;
end;

function TFilterEdit.CheckDateValue(S: string): boolean;
var
 DayS, MonS, YearS : string;
begin
 Result := false;
 S := Trim(S);
 // Проверка заполенения
 if S = '' then exit;
 S := S + '....';
 DayS  := copy(S, 1, pos('.',S)-1); delete(S, 1, pos('.',S));
 MonS  := copy(S, 1, pos('.',S)-1); delete(S, 1, pos('.',S));
 YearS := copy(S, 1, pos('.',S)-1); delete(S, 1, pos('.',S));
 if Not(CheckNumberValue(DayS) and CheckNumberValue(MonS) and CheckNumberValue(YearS)) then
  exit;
 if (StrToInt(YearS) < 1800) or (StrToInt(YearS) > 2200) then
  exit;
 try
  EncodeDate(StrToInt(YearS), StrToInt(MonS), StrToInt(DayS));
  Result := true;
 except
  result := true;
 end;
end;

function TFilterEdit.CheckDateValues(S: string): boolean;
var
 st : string;
begin
 Result := false;
 s := trim(s) + ',';
 while pos(',', s) > 0 do begin
  st := copy(s, 1, pos(',', s)-1);
  if not CheckDateValue(st) then
   exit;
  Delete(s, 1, pos(',', s));
 end;
 Result := true;
end;

function TFilterEdit.GetButtonVisible: boolean;
begin
 Result := true;
end;

function TFilterEdit.GetTextStateDescr: string;
var
 Lines : TStringList;
begin
 Lines := TStringList.Create;
 if FilterOn then
  Lines.add('on=1') else Lines.add('on=0');
 Lines.add('mode='+inttostr(FFilterCombo.ItemIndex));
 Lines.add('ed1='+FilterText1);
 Lines.add('ed2='+FilterText2);
 Result := Lines.CommaText;
end;

procedure TFilterEdit.SetTextStateDescr(const Value: string);
var
 Lines : TStringList;
begin
 Lines := TStringList.Create;
 try
  Lines.CommaText := Value;
  FilterOn := Lines.Values['on'] = '1';
  FFilterCombo.ItemIndex := StrToInt(Lines.Values['mode']);
  FOnComboBoxClick(FFilterCombo);
  FilterText1  := Lines.Values['ed1'];
  FilterText2 := Lines.Values['ed2'];
 except end;
end;

function TFilterEdit.GetTekFilterItemIndex: integer;
begin
 Result := FFilterCombo.ItemIndex;
end;

function TFilterEdit.GetFilterText1: string;
begin
 if FHiddenValuesMode then
  Result := FHiddenFilterText1
 else
  Result := FFilterEdit1.Text;
end;

function TFilterEdit.GetFilterText2: string;
begin
 if FHiddenValuesMode then
  Result := FHiddenFilterText2
 else
  Result := FFilterEdit2.Text;
end;

procedure TFilterEdit.SetFilterText1(const Value: string);
begin
 FFilterEdit1.Text := value;
 if FAutoValidate then
  ValidateValue;
end;

procedure TFilterEdit.SetFilterText2(const Value: string);
begin
 FFilterEdit2.Text := value;
 if FAutoValidate then
  ValidateValue;
end;

procedure TFilterEdit.SetDefaultFilterItem(const Value: integer);
begin
  FDefaultFilterItem := Value;
  SetFilterEditMode(FFilterEditMode);
end;

procedure TFilterEdit.SetFilterItemsNull(const Value: boolean);
begin
  FFilterItemsNull := Value;
  SetFilterEditMode(FFilterEditMode);
end;

procedure TFilterEdit.SetFilterReadOnly(const Value: boolean);
begin
 FFilterReadOnly := Value;
 FFilterEdit1.ReadOnly := Value;
 FFilterEdit2.ReadOnly := Value;
end;

procedure TFilterEdit.SetHiddenFilterText1(const Value: string);
begin
 FHiddenFilterText1 := Value;
 if FAutoValidate then
  ValidateValue;
end;

procedure TFilterEdit.SetHiddenFilterText2(const Value: string);
begin
 FHiddenFilterText2 := Value;
 if FAutoValidate then
  ValidateValue;
end;

procedure TFilterEdit.SetHiddenValuesMode(const Value: boolean);
begin
 FHiddenValuesMode := Value;
 if FAutoValidate then
  ValidateValue;
end;

procedure TFilterEdit.SetButtonsVisible(const Value: boolean);
begin
 FButtonsVisible := Value;
 AlignElements;
end;

procedure TFilterEdit.SetOnButtonClick(const Value: TOnButtonClickEvent);
begin
  FOnButtonClick := Value;
end;

procedure TFilterEdit.SpeedButtonClick(Sender: TObject);
begin
 if not(Assigned(FOnButtonClick)) then exit;
 if Sender = FFilterEdit1b then
  FOnButtonClick(Self, 1);
 if Sender = FFilterEdit2b then
  FOnButtonClick(Self, 2);
end;

procedure TFilterEdit.SetButtonsHint(const Value: string);
begin
 FButtonsHint := Value;
 if FButtonsVisible then begin
  if FFilterEdit1b <> nil then  FFilterEdit1b.Hint := Value;
  if FFilterEdit2b <> nil then  FFilterEdit2b.Hint := Value;
 end;
end;

procedure TFilterEdit.SetFilterEditTranslateEvent(
  const Value: TFilterEditTranslateEvent);
begin
  FFilterEditTranslateEvent := Value;
end;

function TFilterEdit.DoTranslate(S: string): string;
begin
 Result := S;
 if Assigned(FFilterEditTranslateEvent) then
  Result := FFilterEditTranslateEvent(Self, S);
end;

end.

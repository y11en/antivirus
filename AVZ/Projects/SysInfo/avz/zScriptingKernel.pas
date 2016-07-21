unit zScriptingKernel;
// Скрипт-ядро
interface
uses
  Types,
  SysUtils,
  Windows,
  zUserScripting;
threadvar
 // Скрипт-машина
 AvzUserScript : TAvzUserScript;

// Инициализация ядра
function InitScriptingKernel : boolean;

implementation

// Инициализация ядра
function InitScriptingKernel : boolean;
begin
 // Создание класса скрипт-машина
 AvzUserScript := TAvzUserScript.Create;
end;

end.
 

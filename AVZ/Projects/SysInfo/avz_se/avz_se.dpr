program avz_se;

uses
  Forms,
  windows,
  SysUtils,
  ntdll,
  Unit1 in 'Unit1.pas' {Main},
  uScriptWizard in 'uScriptWizard.pas' {ScriptWizard},
  uAddCmd in 'uAddCmd.pas' {AddCmdDLG},
  uAboutDLG in 'uAboutDLG.pas' {AboutDLG},
  uTextVizard in 'uTextVizard.pas' {TextWizard},
  uSelectTemplate in 'uSelectTemplate.pas' {SelectTemplateDLG};

{$R *.res}

begin
  if UpperCase(ParamStr(1)) = 'RU' then
   ZSetThreadLocale(LANG_RUSSIAN)
    else ZSetThreadLocale(LOCALE_SYSTEM_DEFAULT);
  Application.Initialize;
  if UpperCase(ParamStr(1)) = 'RU' then
   ZSetThreadLocale(LANG_RUSSIAN)
    else ZSetThreadLocale(LOCALE_SYSTEM_DEFAULT);
  Application.Title := 'Редактор скриптов AVZ';
  Application.CreateForm(TMain, Main);
  Application.CreateForm(TScriptWizard, ScriptWizard);
  Application.CreateForm(TAddCmdDLG, AddCmdDLG);
  Application.CreateForm(TAboutDLG, AboutDLG);
  Application.CreateForm(TTextWizard, TextWizard);
  Application.CreateForm(TSelectTemplateDLG, SelectTemplateDLG);
  Application.Run;
end.

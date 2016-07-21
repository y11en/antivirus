object Form1: TForm1
  Left = 139
  Top = 79
  Width = 870
  Height = 640
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 16
    Top = 8
    Width = 833
    Height = 433
    Lines.Strings = (
      '1'
      '2'
      '3')
    ScrollBars = ssBoth
    TabOrder = 0
    WordWrap = False
  end
  object IgnoreList: TMemo
    Left = 336
    Top = 480
    Width = 185
    Height = 89
    Lines.Strings = (
      '.'
      '..'
      ':'
      ';'
      '/'
      '\'
      '?'
      '!'
      '>>>>>'
      '>>>'
      ';'
      '-'
      '00'
      '\'
      '%'
      '%)'
      '"'
      ','
      'CLSID'
      'CLSID\'
      '\InprocServer32'
      '\LocalServer32'
      '\LocalServer'
      '='
      '<>'
      '!='
      '.bak'
      'msconfig.exe'
      'regedit.exe'
      '#0.00'
      'RES'
      'MapVirtualKeyA'
      'user32.dll'
      'kernel32.dll'
      't_main_1'
      'Quarantine'
      'UDP -'
      'TCP -'
      '('
      ')'
      ' ('
      ' )'
      'avz.sys'
      'Scripting.FileSystemObject'
      'Infected'
      'LISTENING'
      '-->'
      '->'
      'jmp'
      '['
      ']'
      '')
    TabOrder = 1
  end
  object Button1: TButton
    Left = 16
    Top = 464
    Width = 225
    Height = 25
    Caption = #1055#1086#1089#1090#1088#1086#1077#1085#1080#1077' '#1073#1072#1079#1099
    TabOrder = 2
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 16
    Top = 496
    Width = 225
    Height = 25
    Caption = #1069#1082#1089#1087#1086#1088#1090' '#1074' DBF'
    TabOrder = 3
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 16
    Top = 528
    Width = 225
    Height = 25
    Caption = #1048#1084#1087#1086#1088#1090' DBF'
    TabOrder = 4
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 16
    Top = 560
    Width = 225
    Height = 25
    Caption = #1055#1077#1088#1077#1074#1086#1076
    TabOrder = 5
    OnClick = Button4Click
  end
  object OracleSession1: TOracleSession
    LogonUsername = 'avz'
    LogonPassword = 'aes254'
    LogonDatabase = 'VIRLAB'
    Connected = True
    Left = 192
    Top = 56
  end
  object TranslateOracleDataSet: TOracleDataSet
    SQL.Strings = (
      'SELECT  T.ROWID, T.*'
      'FROM AVZ.AVZT_TRANSLATE T')
    QBEDefinition.QBEFieldDefs = {
      0400000006000000070000005255535F545854010000000000090000004C4F43
      414C5F545854010000000000080000005352435F434F44450100000000000800
      00005352435F46494C4501000000000002000000583101000000000002000000
      5832010000000000}
    Session = OracleSession1
    Active = True
    Left = 232
    Top = 56
  end
  object ZDBFDataSet1: TZDBFDataSet
    FileName = 'translate.dbf'
    ReadOnly = False
    CodePage = cp866
    CalcFieldAfterFilter = False
    Left = 320
    Top = 64
  end
  object ExpOracleDataSet: TOracleDataSet
    SQL.Strings = (
      'SELECT distinct SRC_CODE, RUS_TXT'
      'FROM AVZ.AVZT_TRANSLATE')
    Session = OracleSession1
    Left = 232
    Top = 96
  end
  object ImpTranslateOracleQuery: TOracleQuery
    SQL.Strings = (
      'UPDATE AVZ.AVZT_TRANSLATE'
      'SET    LOCAL_TXT = :LOCAL_TXT'
      'WHERE rtrim(RUS_TXT)   = rtrim(:RUS_TXT)')
    Session = OracleSession1
    Variables.Data = {
      03000000020000000A0000003A4C4F43414C5F54585405000000000000000000
      0000080000003A5255535F545854050000000000000000000000}
    Left = 232
    Top = 144
  end
  object GetTranslateOracleQuery: TOracleQuery
    SQL.Strings = (
      'SELECT REPLACE(LOCAL_TXT, '#39#39#39#39', '#39#39#39#39#39#39') LOCAL_TXT'
      'FROM AVZ.AVZT_TRANSLATE'
      'WHERE RTRIM(LTRIM(RUS_TXT)) = RTRIM(LTRIM(:RUS_TXT))')
    Session = OracleSession1
    Variables.Data = {
      0300000001000000080000003A5255535F545854050000000000000000000000}
    Left = 272
    Top = 144
  end
  object OracleQuery1: TOracleQuery
    SQL.Strings = (
      'SELECT NAIM, SCRIPT, '#39'REPAIR'#39' TIP FROM AVZT_SCRIPT_REPAIR'
      'union all'
      'SELECT NAIM, SCRIPT, '#39'STDOP'#39' TIP FROM AVZT_SCRIPT_STDOP'
      'union all'
      'SELECT NAIM, SCRIPT, '#39'SYSCHECK'#39' TIP FROM AVZT_SCRIPT_SYSCHECK'
      'union all'
      'SELECT NAIM, SCRIPT, '#39'TOKENS'#39'   TIP FROM AVZT_TOKENS_SCR'
      '')
    Session = OracleSession1
    Left = 288
    Top = 64
  end
end

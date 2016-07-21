object Main: TMain
  Left = 192
  Top = 114
  Width = 759
  Height = 637
  Caption = #1040#1085#1072#1083#1080#1079#1072#1090#1086#1088' '#1074#1088#1077#1076#1086#1085#1086#1089#1085#1099#1093' URL'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 16
  object pcMain: TPageControl
    Left = 0
    Top = 0
    Width = 751
    Height = 498
    ActivePage = tsData
    Align = alClient
    TabOrder = 0
    OnChange = pcMainChange
    object tsData: TTabSheet
      Caption = #1041#1072#1079#1072' URL'
      object DBGrid2: TDBGrid
        Left = 0
        Top = 0
        Width = 743
        Height = 467
        Align = alClient
        DataSource = DataSource1
        Options = [dgEditing, dgTitles, dgIndicator, dgColumnResize, dgColLines, dgRowLines, dgTabs, dgCancelOnExit]
        TabOrder = 0
        TitleFont.Charset = DEFAULT_CHARSET
        TitleFont.Color = clWindowText
        TitleFont.Height = -13
        TitleFont.Name = 'MS Sans Serif'
        TitleFont.Style = []
        Columns = <
          item
            Expanded = False
            FieldName = 'ID'
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'URL'
            Width = 212
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'NAIM'
            Width = 296
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'ADD_DATE'
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'LAST_CHECK'
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'FILE_DATE'
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'FILE_SIZE'
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'AUTO_CHECK'
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'BAD_LINK'
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'LAST_RESP_CODE'
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'REAL_FILE_SIZE'
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'REAL_SIGN'
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'UPD_CNT'
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'LAST_AVZ_DETECT'
            Width = 147
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'REF_CNT'
            Visible = True
          end
          item
            Expanded = False
            FieldName = 'URL_TYPE'
            Visible = True
          end>
      end
    end
    object tsFilter: TTabSheet
      Caption = #1060#1080#1083#1100#1090#1088
      ImageIndex = 1
      object FilterEdit1: TFilterEdit
        Left = 2
        Top = 3
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femNumber
        Caption = #1053#1086#1084#1077#1088' '#1079#1072#1087#1080#1089#1080
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'ID'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object FilterEdit2: TFilterEdit
        Left = 0
        Top = 35
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femString
        Caption = 'URL'
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'URL'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object FilterEdit3: TFilterEdit
        Left = -1
        Top = 67
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femString
        Caption = #1053#1072#1080#1084#1077#1085#1086#1074#1072#1085#1080#1077
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'NAIM'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object FilterEdit4: TFilterEdit
        Left = -1
        Top = 99
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femDate
        Caption = #1044#1072#1090#1072' '#1076#1086#1073#1072#1074#1083#1077#1085#1080#1103
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'ADD_DATE'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object FilterEdit5: TFilterEdit
        Left = -1
        Top = 131
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femDate
        Caption = #1044#1072#1090#1072' '#1087#1088#1086#1074#1077#1088#1082#1080
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'LAST_CHECK'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object FilterEdit6: TFilterEdit
        Left = -1
        Top = 163
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femDate
        Caption = #1044#1072#1090#1072' '#1092#1072#1081#1083#1072
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'FILE_DATE'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object FilterEdit7: TFilterEdit
        Left = -1
        Top = 195
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femNumber
        Caption = #1056#1072#1079#1084#1077#1088' '#1092#1072#1081#1083#1072
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'FILE_SIZE'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object FilterEdit8: TFilterEdit
        Left = -1
        Top = 227
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femNumber
        Caption = #1056#1077#1072#1083#1100#1085#1099#1081' '#1088#1072#1079#1084#1077#1088' '#1092#1072#1081#1083#1072
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'REAL_FILE_SIZE'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object FilterEdit9: TFilterEdit
        Left = -1
        Top = 259
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femNumber
        Caption = #1050#1086#1083'-'#1074#1086' '#1086#1073#1085#1086#1074#1083#1077#1085#1080#1081
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'UPD_CNT'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object FilterEdit10: TFilterEdit
        Left = -1
        Top = 291
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femString
        Caption = #1057#1080#1075#1085#1072#1090#1091#1088#1072
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'REAL_SIGN'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object FilterEdit11: TFilterEdit
        Left = -1
        Top = 323
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femNumber
        Caption = #1055#1086#1089#1083#1077#1076#1085#1080#1081' '#1082#1086#1076' HTTP'
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'LAST_RESP_CODE'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object cbExcludeBadLink: TCheckBox
        Left = 0
        Top = 443
        Width = 217
        Height = 17
        Caption = #1048#1089#1082#1083#1102#1095#1080#1090#1100' "'#1073#1080#1090#1099#1077'" '#1089#1089#1099#1083#1082#1080
        Checked = True
        State = cbChecked
        TabOrder = 11
      end
      object cbExcludeNoAutocheck: TCheckBox
        Left = 232
        Top = 443
        Width = 273
        Height = 17
        Caption = #1048#1089#1082#1083#1102#1095#1080#1090#1100' '#1089#1089#1099#1083#1082#1080' '#1073#1077#1079' '#1072#1074#1090#1086#1087#1088#1086#1074#1077#1088#1082#1080
        TabOrder = 12
      end
      object FilterEdit12: TFilterEdit
        Left = -1
        Top = 356
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femString
        Caption = #1056#1077#1079#1091#1083#1100#1090#1072#1090' '#1076#1077#1090#1077#1082#1090#1072' AVZ'
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'LAST_AVZ_DETECT'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object FilterEdit13: TFilterEdit
        Left = -1
        Top = 388
        Width = 687
        Height = 32
        HelpContext = 22
        FilterEditMode = femNumber
        Caption = #1050#1086#1083'-'#1074#1086' '#1086#1073#1085#1072#1088#1091#1078#1077#1085#1080#1081
        FilterCaptionWidth = 210
        FilterComboWidth = 150
        FilterOn = False
        FieldName = 'REF_CNT'
        FilterReadOnly = False
        AutoValidate = True
        DefaultFilterItem = 0
        FilterItemsNull = True
        HiddenValuesMode = False
        ButtonsVisible = False
      end
      object Button7: TButton
        Left = 544
        Top = 432
        Width = 169
        Height = 25
        Caption = #1054#1073#1085#1086#1074#1080#1090#1100
        TabOrder = 15
        OnClick = Button7Click
      end
    end
    object TabSheet3: TTabSheet
      Caption = #1055#1088#1086#1090#1086#1082#1086#1083
      ImageIndex = 2
      object Memo1: TMemo
        Left = 0
        Top = 0
        Width = 743
        Height = 410
        Align = alClient
        Lines.Strings = (
          'Memo1')
        TabOrder = 0
      end
      object Memo2: TMemo
        Left = 0
        Top = 410
        Width = 743
        Height = 57
        Align = alBottom
        Lines.Strings = (
          'Memo2')
        TabOrder = 1
      end
    end
    object TabSheet1: TTabSheet
      Caption = #1054#1087#1077#1088#1072#1094#1080#1080
      ImageIndex = 3
      object Button2: TButton
        Left = 8
        Top = 9
        Width = 201
        Height = 25
        Caption = #1055#1086#1087#1086#1083#1085#1077#1085#1080#1077' '#1073#1072#1079#1099' '#1080#1079' DBF'
        TabOrder = 0
        OnClick = Button2Click
      end
    end
    object TabSheet2: TTabSheet
      Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1080
      ImageIndex = 4
      object cbClipbrdCHM: TCheckBox
        Left = 8
        Top = 11
        Width = 209
        Height = 17
        Caption = #1057#1083#1077#1078#1077#1085#1080#1077' '#1079#1072' Clipboard'
        TabOrder = 0
      end
      object cbSetVirName: TCheckBox
        Left = 8
        Top = 35
        Width = 369
        Height = 17
        Caption = #1055#1077#1088#1077#1079#1072#1087#1080#1089#1099#1074#1072#1090#1100' '#1080#1084#1103' '#1074#1080#1088#1091#1089#1072' '#1087#1086' '#1087#1086#1089#1083#1077#1076#1085#1077#1084#1091' '#1076#1077#1090#1077#1082#1090#1091
        TabOrder = 1
      end
      object GroupBox1: TGroupBox
        Left = 8
        Top = 64
        Width = 193
        Height = 105
        Caption = ' '#1040#1074#1090#1086#1089#1090#1072#1090#1091#1089'  '
        TabOrder = 2
        object cbAS404: TCheckBox
          Left = 8
          Top = 24
          Width = 97
          Height = 17
          Caption = '404 --> 1'
          TabOrder = 0
        end
        object cbAS_1: TCheckBox
          Left = 8
          Top = 40
          Width = 97
          Height = 17
          Caption = '-1 --> 1'
          TabOrder = 1
        end
        object cbAS300: TCheckBox
          Left = 8
          Top = 56
          Width = 97
          Height = 17
          Caption = '300..303 --> 3'
          TabOrder = 2
        end
        object cbAS200: TCheckBox
          Left = 8
          Top = 76
          Width = 97
          Height = 17
          Caption = '200 --> 0'
          TabOrder = 3
        end
      end
    end
    object TabSheet4: TTabSheet
      Caption = #1048#1089#1082#1072#1090#1077#1083#1100' URL'
      ImageIndex = 5
      object RxDBGrid1: TRxDBGrid
        Left = 0
        Top = 0
        Width = 743
        Height = 433
        Align = alTop
        DataSource = DataSource2
        TabOrder = 0
        TitleFont.Charset = DEFAULT_CHARSET
        TitleFont.Color = clWindowText
        TitleFont.Height = -13
        TitleFont.Name = 'MS Sans Serif'
        TitleFont.Style = []
        Columns = <
          item
            Expanded = False
            FieldName = 'X'
            Width = 503
            Visible = True
          end>
      end
      object Button8: TButton
        Left = 8
        Top = 440
        Width = 75
        Height = 25
        Caption = 'Google'
        TabOrder = 1
        OnClick = Button8Click
      end
      object ceHunterFilter: TComboEdit
        Left = 240
        Top = 440
        Width = 385
        Height = 21
        GlyphKind = gkEllipsis
        ButtonWidth = 17
        NumGlyphs = 1
        TabOrder = 2
        Text = 'ceHunterFilter'
        OnButtonClick = ceHunterFilterButtonClick
      end
      object Button9: TButton
        Left = 88
        Top = 440
        Width = 75
        Height = 25
        Caption = 'Yahoo'
        TabOrder = 3
        OnClick = Button9Click
      end
    end
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 584
    Width = 751
    Height = 19
    Panels = <>
    SimplePanel = True
  end
  object Panel1: TPanel
    Left = 0
    Top = 498
    Width = 751
    Height = 86
    Align = alBottom
    TabOrder = 2
    object Button1: TButton
      Left = 8
      Top = 61
      Width = 145
      Height = 25
      Caption = #1054#1073#1085#1086#1074#1080#1090#1100' '#1087#1086' '#1074#1089#1077#1084
      TabOrder = 0
      OnClick = Button1Click
    end
    object Button3: TButton
      Left = 8
      Top = 37
      Width = 145
      Height = 25
      Caption = #1054#1073#1085#1086#1074#1080#1090#1100' '#1090#1077#1082#1091#1097#1080#1081
      TabOrder = 1
      OnClick = Button3Click
    end
    object Button4: TButton
      Left = 160
      Top = 35
      Width = 185
      Height = 25
      Caption = #1055#1077#1088#1077#1079#1072#1075#1088#1091#1079#1080#1090#1100' '#1090#1077#1082#1091#1097#1080#1081
      TabOrder = 2
      OnClick = Button4Click
    end
    object ceURL: TComboEdit
      Left = 8
      Top = 9
      Width = 729
      Height = 21
      GlyphKind = gkEllipsis
      NumGlyphs = 1
      TabOrder = 3
      OnButtonClick = ceURLButtonClick
    end
    object Button5: TButton
      Left = 360
      Top = 37
      Width = 75
      Height = 25
      Caption = #1057#1090#1086#1087
      TabOrder = 4
      OnClick = Button5Click
    end
    object Button6: TButton
      Left = 160
      Top = 61
      Width = 185
      Height = 25
      Caption = #1055#1077#1088#1077#1079#1072#1075#1088#1091#1079#1080#1090#1100' '#1087#1086' '#1074#1089#1077#1084
      TabOrder = 5
      OnClick = Button6Click
    end
    object cbExtScan: TCheckBox
      Left = 464
      Top = 40
      Width = 193
      Height = 17
      Caption = #1056#1072#1089#1096#1080#1088#1077#1085#1085#1072#1103' '#1087#1088#1086#1074#1077#1088#1082#1072
      TabOrder = 6
    end
    object cbDel2: TCheckBox
      Left = 480
      Top = 61
      Width = 273
      Height = 17
      Caption = #1053#1077' '#1072#1085#1072#1083#1080#1079#1080#1088#1086#1074#1072#1090#1100' '#1077#1089#1083#1080' '#1076#1077#1090#1077#1082#1090#1080#1088#1091#1077#1090
      TabOrder = 7
    end
    object Button10: TButton
      Left = 360
      Top = 64
      Width = 75
      Height = 17
      Caption = 'Button10'
      TabOrder = 8
      OnClick = Button10Click
    end
  end
  object OracleSession1: TOracleSession
    LogonUsername = 'avz'
    LogonPassword = 'aes254'
    LogonDatabase = 'VIRLAB'
    Connected = True
    Left = 216
    Top = 80
  end
  object MainDataSet: TOracleDataSet
    SQL.Strings = (
      'SELECT MU.ROWID, MU.*'
      'FROM AVZ.AVZT_URL MU'
      '/* END_REFRESH */'
      'where :where_st'
      '  and url_type=0'
      
        'order by upd_cnt DESC NULLS LAST, last_check NULLS FIRST, REF_CN' +
        'T DESC NULLS LAST')
    Variables.Data = {
      0300000001000000090000003A57484552455F53540100000004000000313D31
      0000000000}
    QBEDefinition.QBEFieldDefs = {
      04000000100000000200000049440100000000000300000055524C0100000000
      00040000004E41494D010000000000080000004144445F444154450100000000
      000A0000004C4153545F434845434B0100000000000900000046494C455F4441
      54450100000000000900000046494C455F53495A450100000000000A00000041
      55544F5F434845434B010000000000080000004241445F4C494E4B0100000000
      000E0000004C4153545F524553505F434F44450100000000000E000000524541
      4C5F46494C455F53495A45010000000000090000005245414C5F5349474E0100
      00000000070000005550445F434E540100000000000F0000004C4153545F4156
      5A5F444554454354010000000000070000005245465F434E5401000000000008
      00000055524C5F54595045010000000000}
    QueryAllRecords = False
    CountAllRecords = True
    RefreshOptions = [roBeforeEdit, roAfterInsert, roAfterUpdate, roAllFields]
    Session = OracleSession1
    Active = True
    AfterInsert = MainDataSetAfterInsert
    BeforeDelete = MainDataSetBeforeDelete
    ProviderOptions = [opNoKeyFields, opNoIndexDefs, opNoDefaultOrder, opNoCommit]
    Left = 248
    Top = 80
  end
  object DataSource1: TDataSource
    DataSet = MainDataSet
    Left = 280
    Top = 80
  end
  object AddChkOracleQuery: TOracleQuery
    SQL.Strings = (
      'UPDATE AVZ.AVZT_URL'
      'SET REF_CNT = nvl(REF_CNT, 0)+1'
      'where rtrim(lower(url)) = rtrim(lower(:url))')
    Session = OracleSession1
    Variables.Data = {0300000001000000040000003A55524C050000000000000000000000}
    Left = 248
    Top = 112
  end
  object ZDBFDataSet1: TZDBFDataSet
    ReadOnly = False
    CodePage = cp1251
    CalcFieldAfterFilter = False
    Left = 312
    Top = 80
  end
  object CheckFileOracleQuery: TOracleQuery
    SQL.Strings = (
      'SELECT  NAIM'
      'FROM AVZ.AVZT_MAIN'
      'WHERE (:FSIZE >= SIZEMIN AND :FSIZE <= SIZEMAX)'
      ' AND (S1 = 0 OR S1 = :S1)'
      ' AND (S2 = 0 OR S2 = :S2)'
      ' AND (S3 = 0 OR S3 = :S3)'
      ' AND (S4 = 0 OR S4 = :S4)'
      ''
      '')
    Session = OracleSession1
    Variables.Data = {
      0300000005000000030000003A5331050000000000000000000000030000003A
      5332050000000000000000000000030000003A53330500000000000000000000
      00030000003A5334050000000000000000000000060000003A4653495A450500
      00000000000000000000}
    Left = 36
    Top = 195
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 300
    OnTimer = Timer1Timer
    Left = 444
    Top = 115
  end
  object Timer2: TTimer
    Interval = 300
    OnTimer = Timer2Timer
    Left = 484
    Top = 115
  end
  object GoogleHunterDataSet: TOracleDataSet
    SQL.Strings = (
      '/*'
      'SELECT DISTINCT URL_EXTRACT_HOST(url) x'
      '  FROM avz.AVZT_URL m'
      
        'WHERE LOWER(URL) LIKE LOWER('#39'%'#39'||:x||'#39'%'#39') and url_type=0 and nai' +
        'm is not null '
      'minus'
      'SELECT s.url x'
      'FROM AVZ.AVZT_URL_SEARCH s'
      'where search_sys=3'
      '*/'
      ''
      '/*'
      'union '
      'SELECT DISTINCT URL_EXTRACT_FILE(url) x'
      '  FROM avz.AVZT_URL m'
      'WHERE LOWER(URL) LIKE LOWER('#39'%'#39'||:x||'#39'%'#39') and naim is not null'
      'minus'
      'SELECT s.url x'
      'FROM AVZ.AVZT_URL_SEARCH s'
      'where search_sys=1'
      '*/'
      ''
      ''
      'select x from xxx'
      'where :x = :x or 1=1'
      'minus'
      'SELECT s.url x'
      'FROM AVZ.AVZT_URL_SEARCH s'
      ''
      '/*'
      'select distinct substr(guid, 2, 36) x from xxxx'
      'where (:x = :x or 1=1) and upper(status) like '#39'X%'#39
      'minus'
      'SELECT s.url x'
      'FROM AVZ.AVZT_URL_SEARCH s'
      '*/'
      '')
    Variables.Data = {0300000001000000020000003A58050000000000000000000000}
    QBEDefinition.QBEFieldDefs = {04000000010000000100000058010000000000}
    Session = OracleSession1
    Active = True
    Left = 436
    Top = 83
  end
  object DataSource2: TDataSource
    DataSet = GoogleHunterDataSet
    Left = 476
    Top = 83
  end
  object SearchStatDataSet: TOracleDataSet
    SQL.Strings = (
      'SELECT s.rowid, s.*'
      'FROM AVZ.AVZT_URL_SEARCH s')
    QBEDefinition.QBEFieldDefs = {
      040000000500000007000000444154455F4F500100000000000300000055524C
      0100000000000A0000005345415243485F53595301000000000009000000464F
      554E445F434E54010000000000070000004144445F434E54010000000000}
    Session = OracleSession1
    Active = True
    Left = 444
    Top = 155
  end
end

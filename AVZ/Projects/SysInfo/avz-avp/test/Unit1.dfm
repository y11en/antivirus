object Form1: TForm1
  Left = 192
  Top = 114
  Width = 870
  Height = 527
  Caption = #1058#1077#1089#1090#1077#1088' '#1103#1076#1088#1072' AVZ'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    862
    493)
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 8
    Top = 462
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = #1055#1091#1089#1082
    TabOrder = 0
    OnClick = Button1Click
  end
  object ProgressBar1: TProgressBar
    Left = 0
    Top = 433
    Width = 862
    Height = 17
    Align = alTop
    TabOrder = 1
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 862
    Height = 241
    Align = alTop
    Caption = ' '#1057#1082#1088#1080#1087#1090'  '
    TabOrder = 2
    object mmScript: TMemo
      Left = 2
      Top = 15
      Width = 858
      Height = 224
      Align = alClient
      Lines.Strings = (
        'var'
        ' AVZLogDir : string;'
        'begin'
        
          ' // '#1040#1085#1090#1080#1088#1091#1090#1082#1080#1090' '#1074' '#1088#1077#1078#1080#1084#1077' '#1085#1077#1081#1090#1088#1072#1083#1080#1079#1072#1094#1080#1080' '#1087#1077#1088#1077#1093#1074#1072#1090#1086#1074'                ' +
          '                                                                ' +
          '                         '
        ' SearchRootkit(true, true);'
        
          ' // '#1069#1074#1088#1080#1089#1090#1080#1095#1077#1089#1082#1072#1103' '#1087#1088#1086#1074#1077#1088#1082#1072' '#1089#1080#1089#1090#1077#1084#1099'                              ' +
          '                                              '
        ' ExecuteSysChkEV;    '
        
          ' // '#1069#1074#1088#1080#1089#1090#1080#1095#1077#1089#1082#1072#1103' '#1087#1088#1086#1074#1077#1088#1082#1072' '#1089#1080#1089#1090#1077#1084#1099' - '#1048#1055#1059'                        ' +
          '                                                                ' +
          '            '
        ' ExecuteSysChkIPU;'
        ' // '#1055#1086#1076#1075#1086#1090#1086#1074#1082#1072' '#1088#1072#1073#1086#1095#1077#1081' '#1087#1072#1087#1082#1080
        ' AVZLogDir := GetAVZDirectory + '#39'LOG\'#39';'
        ' CreateDirectory(AVZLogDir);'
        ' // '#1042#1099#1087#1086#1083#1085#1077#1085#1080#1077' '#1080#1089#1089#1083#1077#1076#1086#1074#1072#1085#1080#1103' '#1089#1080#1089#1090#1077#1084#1099
        ' ExecuteSysCheck(AVZLogDir+'#39'virusinfo_syscheck.htm'#39');'
        'end.')
      TabOrder = 0
    end
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 241
    Width = 862
    Height = 192
    Align = alTop
    Caption = ' '#1055#1088#1086#1090#1086#1082#1086#1083'  '
    TabOrder = 3
    object Memo1: TMemo
      Left = 2
      Top = 15
      Width = 858
      Height = 175
      Align = alClient
      TabOrder = 0
      WordWrap = False
    end
  end
  object Button2: TButton
    Left = 88
    Top = 462
    Width = 75
    Height = 25
    Caption = #1057#1090#1086#1087
    TabOrder = 4
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 288
    Top = 462
    Width = 185
    Height = 25
    Caption = #1055#1072#1088#1089#1080#1085#1075' XML'
    TabOrder = 5
    OnClick = Button3Click
  end
  object Edit1: TEdit
    Left = 472
    Top = 456
    Width = 385
    Height = 21
    TabOrder = 6
    Text = 
      'ftPWChar,; File=ftPWChar,; Path=ftPWChar,; NetName=ftPWChar,; Su' +
      'pplier=ftPWChar,; Type=ftPWChar,; Status=ftPWChar,; GUID=ftPWCha' +
      'r,; CheckResult=ftPWChar,; Producer=ftPWChar,; URL=ftPWChar,; Re' +
      'gPath=ftPWChar,; Neuro=ftPWChar,; Analyzer=ftPWChar,; Descriptio' +
      'n=ftPWChar,; BaseAddr=ftPWChar,; MemSize=ftPWChar,; Sessions=ftP' +
      'WChar,; RemoteHost=ftPWChar,; RemotePort=ftPWChar,; User=ftPWCha' +
      'r,; Resources=ftPWChar,; ConnectionTime=ftPWChar,; DelayTime=ftP' +
      'WChar,; Depends=ftPWChar,; PID=ftPWChar,; MD5=ftPWChar,; Camoufl' +
      'age=ftPWChar,; CommandLine=ftPWChar,; Handle=ftPWChar,; Visibili' +
      'ty=ftWord,0; Coords=ftPWChar,; '
  end
  object Button4: TButton
    Left = 168
    Top = 464
    Width = 75
    Height = 25
    Caption = #1058#1077#1089#1090' CDB'
    TabOrder = 7
    OnClick = Button4Click
  end
end

object SetupDlg: TSetupDlg
  Left = 192
  Top = 114
  Width = 613
  Height = 251
  Caption = 'AVZ TheBat Plugin. '#1053#1072#1089#1090#1088#1086#1081#1082#1072
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  Scaled = False
  PixelsPerInch = 96
  TextHeight = 16
  object Panel1: TPanel
    Left = 0
    Top = 180
    Width = 605
    Height = 37
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    DesignSize = (
      605
      37)
    object BitBtn1: TBitBtn
      Left = 440
      Top = 6
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      TabOrder = 0
      Kind = bkOK
    end
    object BitBtn2: TBitBtn
      Left = 520
      Top = 6
      Width = 81
      Height = 25
      Anchors = [akTop, akRight]
      Caption = #1054#1090#1084#1077#1085#1072
      TabOrder = 1
      Kind = bkCancel
    end
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 605
    Height = 89
    Align = alTop
    Caption = #1056#1072#1079#1084#1077#1097#1077#1085#1080#1077' AV '#1073#1072#1079
    TabOrder = 1
    DesignSize = (
      605
      89)
    object rbDefPath: TRadioButton
      Left = 8
      Top = 21
      Width = 321
      Height = 17
      Caption = #1055#1072#1087#1082#1072' Base '#1088#1072#1079#1084#1077#1097#1072#1077#1090#1089#1103' '#1074' '#1087#1072#1087#1082#1077' '#1089' '#1087#1083#1072#1075#1080#1085#1086#1084
      Checked = True
      TabOrder = 0
      TabStop = True
    end
    object rbUserPath: TRadioButton
      Left = 8
      Top = 42
      Width = 161
      Height = 17
      Caption = #1042' '#1086#1090#1076#1077#1083#1100#1085#1086#1081' '#1087#1072#1087#1082#1077
      TabOrder = 1
    end
    object deUserPath: TDirectoryEdit
      Left = 32
      Top = 60
      Width = 566
      Height = 21
      NumGlyphs = 1
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 2
    end
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 89
    Width = 605
    Height = 80
    Align = alTop
    Caption = ' '#1044#1086#1087#1086#1083#1085#1080#1090#1077#1083#1100#1085#1099#1077' '#1085#1072#1089#1090#1088#1086#1081#1082#1080' '
    TabOrder = 2
    object cbLogMode: TCheckBox
      Left = 8
      Top = 24
      Width = 169
      Height = 17
      Caption = #1055#1088#1086#1090#1086#1082#1086#1083#1080#1088#1086#1074#1072#1085#1080#1077
      TabOrder = 0
      OnClick = cbLogModeClick
    end
    object cbLogMalware: TCheckBox
      Left = 24
      Top = 43
      Width = 545
      Height = 17
      Caption = 
        #1042#1085#1086#1089#1080#1090#1100' '#1074' '#1087#1088#1086#1090#1086#1082#1086#1083' '#1080#1085#1092#1086#1088#1084#1072#1094#1080#1102' '#1086#1073' '#1086#1073#1085#1072#1088#1091#1078#1077#1085#1085#1099#1093' '#1074#1088#1077#1076#1086#1085#1086#1089#1085#1099#1093' '#1087#1088#1086#1075#1088#1072 +
        #1084#1084#1072#1093
      TabOrder = 1
    end
  end
end

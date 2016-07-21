object Form1: TForm1
  Left = 192
  Top = 114
  Width = 661
  Height = 116
  Caption = #1055#1077#1088#1077#1080#1084#1077#1085#1086#1074#1072#1085#1080#1077' '#1082#1072#1088#1072#1085#1090#1080#1085#1072' AVZ'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  DesignSize = (
    653
    82)
  PixelsPerInch = 96
  TextHeight = 13
  object deDir: TDirectoryEdit
    Left = 8
    Top = 8
    Width = 639
    Height = 21
    NumGlyphs = 1
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
    Text = 'deDir'
  end
  object Button1: TButton
    Left = 8
    Top = 56
    Width = 75
    Height = 25
    Caption = #1055#1091#1089#1082
    TabOrder = 1
    OnClick = Button1Click
  end
  object cbRecurse: TCheckBox
    Left = 8
    Top = 32
    Width = 145
    Height = 17
    Caption = #1056#1077#1082#1091#1088#1089#1080#1074#1085#1099#1081' '#1072#1085#1072#1083#1080#1079
    TabOrder = 2
  end
end

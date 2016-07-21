object Main: TMain
  Left = 192
  Top = 114
  Width = 870
  Height = 475
  Caption = 'Anti L2M'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 0
    Top = 0
    Width = 862
    Height = 385
    Align = alTop
    Lines.Strings = (
      'Memo1')
    TabOrder = 0
  end
  object Button1: TButton
    Left = 8
    Top = 392
    Width = 75
    Height = 25
    Caption = #1055#1091#1089#1082
    TabOrder = 1
    OnClick = Button1Click
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 422
    Width = 862
    Height = 19
    Panels = <>
    SimplePanel = True
  end
end

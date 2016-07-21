object SysRepair: TSysRepair
  Left = 216
  Top = 101
  Width = 567
  Height = 434
  Caption = '$AVZ0143'
  Color = clBtnFace
  Constraints.MinHeight = 200
  Constraints.MinWidth = 400
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 16
  object cbRepairScriptList: TCheckListBox
    Left = 0
    Top = 0
    Width = 559
    Height = 369
    Align = alClient
    ItemHeight = 16
    TabOrder = 0
  end
  object Panel1: TPanel
    Left = 0
    Top = 369
    Width = 559
    Height = 31
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    DesignSize = (
      559
      31)
    object mbClose: TBitBtn
      Left = 464
      Top = 5
      Width = 89
      Height = 24
      Anchors = [akTop, akRight]
      Caption = '$AVZ0266'
      TabOrder = 0
      Kind = bkCancel
    end
    object mbRunRepair: TBitBtn
      Left = 4
      Top = 3
      Width = 269
      Height = 25
      Caption = '$AVZ0187'
      TabOrder = 1
      OnClick = mbRunRepairClick
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000010000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFF717FFFFFFFFFFFFF111FFFFFFFFFFFFF717FFFFFFFFFFFF
        FFFF00000000007FFF1F08BFBFB8070FF7177000000000FFF111F08888880FFF
        F111F0FBFBFB07FFF111F70F8888F07FF111FF70BFBFBF0FF111FFF088888807
        FF1FFF00000000007FFFFF08BFBFB8070FFFFFF000000000FFFF}
    end
  end
end

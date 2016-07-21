object TextWizard: TTextWizard
  Left = 192
  Top = 114
  Width = 611
  Height = 482
  Caption = '$AVZ1290'
  Color = clBtnFace
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 16
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 603
    Height = 34
    Align = alTop
    TabOrder = 0
    object Label1: TLabel
      Left = 8
      Top = 12
      Width = 61
      Height = 16
      Caption = '$AVZ1291'
    end
    object cbTemplate: TComboBox
      Left = 88
      Top = 4
      Width = 513
      Height = 24
      Style = csDropDownList
      ItemHeight = 16
      TabOrder = 0
      OnChange = cbTemplateChange
    end
  end
  object mmRes: TMemo
    Left = 0
    Top = 34
    Width = 603
    Height = 373
    Align = alClient
    Lines.Strings = (
      'Memo1')
    ScrollBars = ssBoth
    TabOrder = 1
    WordWrap = False
  end
  object Panel2: TPanel
    Left = 0
    Top = 407
    Width = 603
    Height = 41
    Align = alBottom
    TabOrder = 2
    DesignSize = (
      603
      41)
    object BitBtn1: TBitBtn
      Left = 8
      Top = 8
      Width = 241
      Height = 25
      Caption = '$AVZ1292'
      TabOrder = 0
      OnClick = BitBtn1Click
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000010000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFF444444444FFFFFFF4FFFFFFF4FFFFFFF4F0000
        0F4F0000004FFFFFFF4F0FFFFF4F00000F4F0F00004FFFFFFF4F0FFFFF4F00F4
        444F0F00004FFFF4F4FF0FFFFF4FFFF44FFF0F00F0444444FFFF0FFFF0F0FFFF
        FFFF0FFFF00FFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFF}
    end
    object BitBtn2: TBitBtn
      Left = 501
      Top = 8
      Width = 93
      Height = 25
      Anchors = [akTop, akRight]
      Caption = '$AVZ0266'
      TabOrder = 1
      Kind = bkOK
    end
  end
end

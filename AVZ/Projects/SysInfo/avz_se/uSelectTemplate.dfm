object SelectTemplateDLG: TSelectTemplateDLG
  Left = 192
  Top = 114
  Width = 416
  Height = 640
  Caption = '$AVZ1289'
  Color = clBtnFace
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 565
    Width = 408
    Height = 41
    Align = alBottom
    TabOrder = 0
    DesignSize = (
      408
      41)
    object BitBtn1: TBitBtn
      Left = 215
      Top = 8
      Width = 91
      Height = 25
      Anchors = [akTop, akRight]
      TabOrder = 0
      Kind = bkOK
    end
    object BitBtn2: TBitBtn
      Left = 311
      Top = 8
      Width = 89
      Height = 25
      Anchors = [akTop, akRight]
      Caption = '$AVZ0612'
      TabOrder = 1
      Kind = bkCancel
    end
  end
  object lbTemplates: TListBox
    Left = 0
    Top = 0
    Width = 408
    Height = 565
    Align = alClient
    ItemHeight = 13
    Sorted = True
    TabOrder = 1
  end
end

object FileDeleteModeDLG: TFileDeleteModeDLG
  Left = 192
  Top = 114
  Width = 397
  Height = 190
  BorderIcons = [biSystemMenu]
  Caption = '$AVZ0926'
  Color = clBtnFace
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
  object Panel2: TPanel
    Left = 0
    Top = 121
    Width = 389
    Height = 35
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    DesignSize = (
      389
      35)
    object BitBtn1: TBitBtn
      Left = 201
      Top = 6
      Width = 91
      Height = 25
      Anchors = [akTop, akRight]
      TabOrder = 0
      Kind = bkOK
    end
    object BitBtn2: TBitBtn
      Left = 296
      Top = 6
      Width = 91
      Height = 25
      Anchors = [akTop, akRight]
      Caption = '$AVZ0612'
      TabOrder = 1
      Kind = bkCancel
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 91
    Width = 389
    Height = 30
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object cbQuarantine: TCheckBox
      Left = 8
      Top = 2
      Width = 369
      Height = 17
      Caption = '$AVZ0374'
      TabOrder = 0
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 0
    Width = 389
    Height = 91
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 2
    object rbDelete: TRadioButton
      Left = 8
      Top = 8
      Width = 193
      Height = 17
      Caption = '$AVZ1082'
      TabOrder = 0
    end
    object rbHDelete: TRadioButton
      Left = 8
      Top = 32
      Width = 257
      Height = 41
      Caption = '$AVZ1083'
      Checked = True
      TabOrder = 1
      TabStop = True
      WordWrap = True
    end
  end
end

object SelectDeletedFile: TSelectDeletedFile
  Left = 192
  Top = 130
  Width = 556
  Height = 120
  BorderIcons = [biSystemMenu]
  Caption = '$AVZ0611'
  Color = clBtnFace
  Constraints.MinHeight = 120
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnCreate = FormCreate
  DesignSize = (
    548
    86)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 24
    Width = 54
    Height = 13
    Caption = '$AVZ1109:'
  end
  object feFile: TFilenameEdit
    Left = 48
    Top = 16
    Width = 497
    Height = 21
    OnAfterDialog = feFileAfterDialog
    Anchors = [akLeft, akTop, akRight]
    NumGlyphs = 1
    TabOrder = 0
    Text = 'feFile'
  end
  object BitBtn1: TBitBtn
    Left = 8
    Top = 54
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    TabOrder = 1
    Kind = bkOK
  end
  object BitBtn2: TBitBtn
    Left = 88
    Top = 54
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = '$AVZ0612'
    TabOrder = 2
    Kind = bkCancel
  end
end

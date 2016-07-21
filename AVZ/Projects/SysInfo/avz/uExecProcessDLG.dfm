object ExecProcessDLG: TExecProcessDLG
  Left = 192
  Top = 114
  Width = 631
  Height = 143
  BorderIcons = [biSystemMenu]
  Caption = '$AVZ0278'
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
  DesignSize = (
    623
    109)
  PixelsPerInch = 96
  TextHeight = 16
  object Label1: TLabel
    Left = 8
    Top = 16
    Width = 64
    Height = 16
    Caption = '$AVZ0797:'
  end
  object Label2: TLabel
    Left = 8
    Top = 44
    Width = 64
    Height = 16
    Caption = '$AVZ0351:'
  end
  object feProgramm: TFilenameEdit
    Left = 136
    Top = 8
    Width = 481
    Height = 21
    Filter = '$AVZ0801'
    Anchors = [akLeft, akTop, akRight]
    NumGlyphs = 1
    TabOrder = 0
    Text = 'feProgramm'
    OnChange = feProgrammChange
  end
  object edCmdLine: TEdit
    Left = 136
    Top = 32
    Width = 481
    Height = 24
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 1
    Text = 'edCmdLine'
  end
  object Panel1: TPanel
    Left = 0
    Top = 73
    Width = 623
    Height = 36
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 2
    DesignSize = (
      623
      36)
    object mbOk: TBitBtn
      Left = 440
      Top = 8
      Width = 83
      Height = 25
      Anchors = [akTop, akRight]
      TabOrder = 0
      Kind = bkOK
    end
    object BitBtn2: TBitBtn
      Left = 528
      Top = 8
      Width = 89
      Height = 25
      Anchors = [akTop, akRight]
      Caption = '$AVZ0612'
      TabOrder = 1
      Kind = bkCancel
    end
  end
end

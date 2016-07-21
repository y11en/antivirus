object AutoUpdateSetup: TAutoUpdateSetup
  Left = 192
  Top = 114
  Width = 414
  Height = 320
  BorderIcons = [biSystemMenu]
  Caption = '$AVZ0466'
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
    406
    286)
  PixelsPerInch = 96
  TextHeight = 16
  object rgMode: TRadioGroup
    Left = 0
    Top = 0
    Width = 406
    Height = 121
    Align = alTop
    Caption = ' $AVZ0911 '
    ItemIndex = 0
    Items.Strings = (
      '$AVZ0319'
      '$AVZ0880'
      '$AVZ1140'
      '$AVZ1142'
      '$AVZ1141')
    TabOrder = 0
    OnClick = rgModeClick
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 121
    Width = 406
    Height = 105
    Align = alTop
    Caption = ' $AVZ0695 '
    TabOrder = 1
    DesignSize = (
      406
      105)
    object lbProxy: TLabel
      Left = 8
      Top = 26
      Width = 60
      Height = 16
      Caption = 'Proxy:port'
    end
    object lbProxyUser: TLabel
      Left = 8
      Top = 51
      Width = 61
      Height = 16
      Caption = '$AVZ0300'
    end
    object lbProxyPasswd: TLabel
      Left = 8
      Top = 74
      Width = 64
      Height = 16
      Caption = '$AVZ0698 '
    end
    object edProxy: TEdit
      Left = 117
      Top = 16
      Width = 280
      Height = 24
      Hint = '$AVZ0305'
      Anchors = [akLeft, akTop, akRight]
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      Text = 'edProxy'
    end
    object edProxyUser: TEdit
      Left = 117
      Top = 42
      Width = 280
      Height = 24
      Anchors = [akLeft, akTop, akRight]
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      Text = 'Edit1'
    end
    object edProxyPasswd: TEdit
      Left = 117
      Top = 68
      Width = 280
      Height = 24
      Anchors = [akLeft, akTop, akRight]
      ParentShowHint = False
      PasswordChar = '*'
      ShowHint = True
      TabOrder = 2
      Text = 'Edit1'
    end
  end
  object BitBtn1: TBitBtn
    Left = 235
    Top = 258
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    TabOrder = 2
    Kind = bkOK
  end
  object BitBtn2: TBitBtn
    Left = 315
    Top = 258
    Width = 89
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = '$AVZ0612'
    TabOrder = 3
    Kind = bkCancel
  end
  object cbSaveSetup: TCheckBox
    Left = 8
    Top = 232
    Width = 185
    Height = 17
    Caption = '$AVZ0274'
    TabOrder = 4
  end
end

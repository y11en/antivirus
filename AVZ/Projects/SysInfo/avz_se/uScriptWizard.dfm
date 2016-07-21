object ScriptWizard: TScriptWizard
  Left = 192
  Top = 114
  Width = 569
  Height = 593
  Caption = '$AVZ1266'
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
  object GroupBox1: TGroupBox
    Left = 0
    Top = 349
    Width = 561
    Height = 164
    Align = alTop
    Caption = '$AVZ1267  '
    TabOrder = 0
    object cbBCImportDeletedList: TCheckBox
      Left = 8
      Top = 58
      Width = 337
      Height = 17
      Caption = '$AVZ1268'
      TabOrder = 0
    end
    object cbBCActivate: TCheckBox
      Left = 8
      Top = 22
      Width = 185
      Height = 17
      Caption = '$AVZ1269'
      TabOrder = 1
      OnClick = cbRootkitClick
    end
    object cbBCLog: TCheckBox
      Left = 8
      Top = 40
      Width = 249
      Height = 17
      Caption = '$AVZ1270'
      TabOrder = 2
    end
    object cbBCImportQuarantineList: TCheckBox
      Left = 8
      Top = 75
      Width = 337
      Height = 17
      Caption = '$AVZ1271'
      TabOrder = 3
    end
    object cbQrFile: TCheckBox
      Left = 8
      Top = 92
      Width = 337
      Height = 17
      Caption = '$AVZ1272'
      TabOrder = 4
    end
    object bcBCDeleteFile: TCheckBox
      Left = 8
      Top = 109
      Width = 225
      Height = 17
      Caption = '$AVZ1273'
      TabOrder = 5
    end
    object cbBCQrSvc: TCheckBox
      Left = 8
      Top = 126
      Width = 297
      Height = 17
      Caption = '$AVZ1274'
      TabOrder = 6
    end
    object cbBCDeleteSvc: TCheckBox
      Left = 8
      Top = 142
      Width = 297
      Height = 17
      Caption = '$AVZ1275'
      TabOrder = 7
    end
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 0
    Width = 561
    Height = 156
    Align = alTop
    Caption = ' $AVZ1276 '
    TabOrder = 1
    object cbRootkit: TCheckBox
      Left = 8
      Top = 20
      Width = 465
      Height = 17
      Caption = '$AVZ0499'
      TabOrder = 0
      OnClick = cbRootkitClick
    end
    object cbAVZGuard: TCheckBox
      Left = 8
      Top = 36
      Width = 169
      Height = 17
      Caption = '$AVZ1277'
      TabOrder = 1
      OnClick = cbRootkitClick
    end
    object cbExecuteSysClean: TCheckBox
      Left = 8
      Top = 68
      Width = 297
      Height = 17
      Caption = '$AVZ1144'
      TabOrder = 2
    end
    object cbReboot: TCheckBox
      Left = 8
      Top = 134
      Width = 137
      Height = 17
      Caption = '$AVZ0703'
      TabOrder = 3
    end
    object cbSaveLog: TCheckBox
      Left = 8
      Top = 117
      Width = 193
      Height = 17
      Caption = '$AVZ1027'
      TabOrder = 4
    end
    object cbLSPFix: TCheckBox
      Left = 8
      Top = 85
      Width = 353
      Height = 17
      Caption = '$AVZ1278'
      TabOrder = 5
    end
    object cbAVUpdate: TCheckBox
      Left = 8
      Top = 52
      Width = 161
      Height = 17
      Caption = '$AVZ0550'
      TabOrder = 6
    end
    object cbClearHosts: TCheckBox
      Left = 8
      Top = 101
      Width = 345
      Height = 17
      Caption = '$AVZ0629'
      TabOrder = 7
    end
  end
  object GroupBox3: TGroupBox
    Left = 0
    Top = 242
    Width = 561
    Height = 107
    Align = alTop
    Caption = ' $AVZ1279 '
    TabOrder = 2
    object cbDeleteFile: TCheckBox
      Left = 8
      Top = 19
      Width = 146
      Height = 17
      Caption = '$AVZ1081'
      TabOrder = 0
    end
    object cbDelByCLSID: TCheckBox
      Left = 8
      Top = 35
      Width = 298
      Height = 17
      Caption = '$AVZ1280'
      TabOrder = 1
    end
    object cbDelDriver: TCheckBox
      Left = 8
      Top = 52
      Width = 377
      Height = 17
      Caption = '$AVZ1079'
      TabOrder = 2
    end
    object cbDelWinlogon: TCheckBox
      Left = 8
      Top = 69
      Width = 385
      Height = 17
      Caption = '$AVZ1281'
      TabOrder = 3
    end
    object cbTerminateProcess: TCheckBox
      Left = 8
      Top = 86
      Width = 385
      Height = 17
      Caption = '$AVZ1282'
      TabOrder = 4
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 518
    Width = 561
    Height = 41
    Align = alBottom
    TabOrder = 3
    DesignSize = (
      561
      41)
    object BitBtn1: TBitBtn
      Left = 384
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'OK'
      Default = True
      TabOrder = 0
      OnClick = BitBtn1Click
      Glyph.Data = {
        DE010000424DDE01000000000000760000002800000024000000120000000100
        0400000000006801000000000000000000001000000000000000000000000000
        80000080000000808000800000008000800080800000C0C0C000808080000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        3333333333333333333333330000333333333333333333333333F33333333333
        00003333344333333333333333388F3333333333000033334224333333333333
        338338F3333333330000333422224333333333333833338F3333333300003342
        222224333333333383333338F3333333000034222A22224333333338F338F333
        8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
        33333338F83338F338F33333000033A33333A222433333338333338F338F3333
        0000333333333A222433333333333338F338F33300003333333333A222433333
        333333338F338F33000033333333333A222433333333333338F338F300003333
        33333333A222433333333333338F338F00003333333333333A22433333333333
        3338F38F000033333333333333A223333333333333338F830000333333333333
        333A333333333333333338330000333333333333333333333333333333333333
        0000}
      NumGlyphs = 2
    end
    object cbAddCmt: TCheckBox
      Left = 8
      Top = 8
      Width = 217
      Height = 17
      Caption = '$AVZ1283'
      Checked = True
      State = cbChecked
      TabOrder = 1
    end
    object BitBtn2: TBitBtn
      Left = 464
      Top = 8
      Width = 89
      Height = 25
      Anchors = [akTop, akRight]
      Caption = '$AVZ0612'
      TabOrder = 2
      Kind = bkCancel
    end
  end
  object GroupBox4: TGroupBox
    Left = 0
    Top = 156
    Width = 561
    Height = 86
    Align = alTop
    Caption = ' $AVZ1284 '
    TabOrder = 4
    object cbAutoquarantine: TCheckBox
      Left = 8
      Top = 32
      Width = 530
      Height = 17
      Caption = '$AVZ1285'
      TabOrder = 0
    end
    object cbClearQuarantine: TCheckBox
      Left = 8
      Top = 16
      Width = 169
      Height = 17
      Caption = '$AVZ1286'
      TabOrder = 1
    end
    object cbQuarantineFile: TCheckBox
      Left = 8
      Top = 48
      Width = 273
      Height = 17
      Caption = '$AVZ1287'
      TabOrder = 2
    end
    object cbCreateQuarantineArchive: TCheckBox
      Left = 8
      Top = 64
      Width = 393
      Height = 17
      Caption = '$AVZ1288'
      TabOrder = 3
    end
  end
end

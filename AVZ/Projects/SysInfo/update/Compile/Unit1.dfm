object Main: TMain
  Left = 192
  Top = 114
  Width = 696
  Height = 456
  Caption = #1043#1077#1085#1077#1088#1072#1090#1086#1088' '#1072#1087#1076#1077#1081#1090#1086#1074
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 16
  object feBaseDir: TFilenameEdit
    Left = 8
    Top = 8
    Width = 665
    Height = 21
    NumGlyphs = 1
    TabOrder = 0
    Text = 'C:\avz4\'
  end
  object Button1: TButton
    Left = 8
    Top = 392
    Width = 169
    Height = 25
    Caption = #1043#1077#1085#1077#1088#1080#1088#1086#1074#1072#1090#1100' '#1072#1087#1076#1077#1081#1090
    TabOrder = 1
    OnClick = Button1Click
  end
  object mbFTPUpd: TButton
    Left = 192
    Top = 392
    Width = 193
    Height = 25
    Caption = #1054#1073#1085#1086#1074#1080#1090#1100' '#1087#1086' FTP'
    TabOrder = 2
    OnClick = mbFTPUpdClick
  end
  object PageControl1: TPageControl
    Left = 8
    Top = 32
    Width = 673
    Height = 353
    ActivePage = TabSheet1
    TabOrder = 3
    object TabSheet1: TTabSheet
      Caption = #1055#1088#1086#1090#1086#1082#1086#1083
      object Memo1: TMemo
        Left = 0
        Top = 0
        Width = 665
        Height = 322
        Align = alClient
        ScrollBars = ssBoth
        TabOrder = 0
      end
    end
    object TabSheet2: TTabSheet
      Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1080
      ImageIndex = 1
      object Label1: TLabel
        Left = 16
        Top = 176
        Width = 47
        Height = 16
        Caption = #1041#1091#1092#1077#1088':'
      end
      object GroupBox1: TGroupBox
        Left = 8
        Top = 8
        Width = 265
        Height = 129
        Caption = ' '#1057#1077#1088#1074#1077#1088#1072' '
        TabOrder = 0
        object cb_S1: TCheckBox
          Left = 8
          Top = 16
          Width = 193
          Height = 17
          Caption = 'ftp://z-olegcom.57.com1.ru'
          Checked = True
          State = cbChecked
          TabOrder = 0
        end
        object cb_S2: TCheckBox
          Left = 8
          Top = 34
          Width = 169
          Height = 17
          Caption = 'ftp://avz.virusinfo.info'
          Checked = True
          State = cbChecked
          TabOrder = 1
        end
        object cb_S3: TCheckBox
          Left = 8
          Top = 51
          Width = 121
          Height = 17
          Caption = '172.20.97.200'
          Checked = True
          State = cbChecked
          TabOrder = 2
        end
        object cb_S4: TCheckBox
          Left = 8
          Top = 68
          Width = 121
          Height = 17
          Caption = '172.20.99.20'
          Checked = True
          State = cbChecked
          TabOrder = 3
        end
        object cb_S5: TCheckBox
          Left = 8
          Top = 85
          Width = 121
          Height = 17
          Caption = '172.20.100.200'
          Checked = True
          State = cbChecked
          TabOrder = 4
        end
        object cb_S6: TCheckBox
          Left = 8
          Top = 101
          Width = 121
          Height = 17
          Caption = '172.20.98.200'
          Checked = True
          State = cbChecked
          TabOrder = 5
        end
      end
      object cbPassiveM: TCheckBox
        Left = 16
        Top = 144
        Width = 201
        Height = 17
        Caption = #1055#1072#1089#1089#1080#1074#1085#1099#1081' '#1088#1077#1078#1080#1084' FTP'
        TabOrder = 1
      end
      object RxSpinEdit1: TRxSpinEdit
        Left = 72
        Top = 168
        Width = 121
        Height = 24
        Value = 32768.000000000000000000
        TabOrder = 2
      end
    end
  end
  object IdFTP: TIdFTP
    MaxLineAction = maException
    ReadTimeout = 0
    OnWork = IdFTPWork
    ProxySettings.ProxyType = fpcmNone
    ProxySettings.Port = 0
    Left = 320
    Top = 96
  end
end

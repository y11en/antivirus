object NetShares: TNetShares
  Left = 192
  Top = 114
  Width = 686
  Height = 480
  Caption = '$AVZ0949'
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
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 16
  object Panel1: TPanel
    Left = 0
    Top = 416
    Width = 678
    Height = 30
    Align = alBottom
    TabOrder = 0
    DesignSize = (
      678
      30)
    object BitBtn4: TBitBtn
      Left = 600
      Top = 3
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'OK'
      ModalResult = 1
      TabOrder = 0
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
  end
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 678
    Height = 416
    ActivePage = TabSheet1
    Align = alClient
    TabOrder = 1
    object TabSheet1: TTabSheet
      Caption = '$AVZ0562'
      object dgNetShares: TRxDrawGrid
        Left = 0
        Top = 0
        Width = 670
        Height = 385
        Align = alClient
        ColCount = 4
        FixedCols = 0
        TabOrder = 0
        OnDrawCell = dgNetSharesDrawCell
        FixedButtons = True
        OnFixedCellClick = dgNetSharesFixedCellClick
        ColWidths = (
          149
          240
          256
          64)
      end
    end
    object TabSheet2: TTabSheet
      Caption = '$AVZ0944'
      ImageIndex = 1
      object dgNetSessions: TRxDrawGrid
        Left = 0
        Top = 0
        Width = 670
        Height = 349
        Align = alClient
        FixedCols = 0
        TabOrder = 0
        OnDrawCell = dgNetSessionsDrawCell
        FixedButtons = True
        OnFixedCellClick = dgNetSessionsFixedCellClick
        ColWidths = (
          175
          199
          93
          127
          136)
      end
      object Panel2: TPanel
        Left = 0
        Top = 349
        Width = 670
        Height = 36
        Align = alBottom
        TabOrder = 1
        object mbCloseSession: TButton
          Left = 7
          Top = 6
          Width = 137
          Height = 25
          Caption = '$AVZ0267'
          TabOrder = 0
          OnClick = mbCloseSessionClick
        end
      end
    end
    object TabSheet3: TTabSheet
      Caption = '$AVZ0607 '
      ImageIndex = 2
      object dgNetFiles: TRxDrawGrid
        Left = 0
        Top = 0
        Width = 670
        Height = 349
        Align = alClient
        ColCount = 3
        FixedCols = 0
        TabOrder = 0
        OnDrawCell = dgNetFilesDrawCell
        FixedButtons = True
        OnFixedCellClick = dgNetFilesFixedCellClick
        ColWidths = (
          229
          198
          218)
      end
      object Panel3: TPanel
        Left = 0
        Top = 349
        Width = 670
        Height = 36
        Align = alBottom
        TabOrder = 1
        object mbCloaseFile: TButton
          Left = 7
          Top = 6
          Width = 137
          Height = 25
          Caption = '$AVZ0268'
          TabOrder = 0
          OnClick = mbCloaseFileClick
        end
      end
    end
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 56
    Top = 144
  end
end

object NetOpenPorts: TNetOpenPorts
  Left = 192
  Top = 114
  Width = 743
  Height = 482
  Caption = '$AVZ0606'
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
  object PageControl1: TPageControl
    Left = 0
    Top = 32
    Width = 735
    Height = 416
    ActivePage = TabSheet1
    Align = alClient
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = '$AVZ0784'
      object dgTCPList: TRxDrawGrid
        Left = 0
        Top = 0
        Width = 727
        Height = 385
        Align = alClient
        ColCount = 7
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goDrawFocusSelected, goColSizing, goEditing]
        TabOrder = 0
        OnDrawCell = dgTCPListDrawCell
        ColWidths = (
          98
          110
          110
          106
          286
          397
          250)
      end
    end
    object TabSheet2: TTabSheet
      Caption = '$AVZ0786'
      ImageIndex = 1
      object dgUDPList: TRxDrawGrid
        Left = 0
        Top = 0
        Width = 727
        Height = 384
        Align = alClient
        ColCount = 4
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goDrawFocusSelected, goColSizing, goEditing]
        TabOrder = 0
        OnDrawCell = dgUDPListDrawCell
        ColWidths = (
          85
          316
          353
          475)
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 735
    Height = 32
    Align = alTop
    TabOrder = 1
    object SpeedButton2: TSpeedButton
      Left = 30
      Top = 4
      Width = 22
      Height = 22
      Hint = '$AVZ1032'
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000010000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
        FFFFFF0000000000000FF03300000088030FF03300000088030FF03300000088
        030FF03300000000030FF03333333333330FF03300000000330FF03088888888
        030FF03088888888030FF03088888888030FF03088888888030FF03088888888
        000FF03088888888080FF00000000000000FFFFFFFFFFFFFFFFF}
      ParentShowHint = False
      ShowHint = True
      OnClick = SpeedButton2Click
    end
    object mbRefresh: TBitBtn
      Left = 5
      Top = 4
      Width = 22
      Height = 22
      Hint = '$AVZ0544'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnClick = mbRefreshClick
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000010000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FF0000000000
        000FFF0FFFFFFFFFFF0FFF0FFFFF2FFFFF0FFF0FFFF22FFFFF0FFF0FFF22222F
        FF0FFF0FFFF22FF2FF0FFF0FFFFF2FF2FF0FFF0FF2FFFFF2FF0FFF0FF2FF2FFF
        FF0FFF0FF2FF22FFFF0FFF0FFF22222FFF0FFF0FFFFF22FFFF0FFF0FFFFF2FF0
        000FFF0FFFFFFFF0F0FFFF0FFFFFFFF00FFFFF0000000000FFFF}
    end
  end
  object Timer1: TTimer
    Interval = 15000
    OnTimer = Timer1Timer
    Left = 80
    Top = 200
  end
end

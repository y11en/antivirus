object HostsManager: THostsManager
  Left = 216
  Top = 101
  Width = 700
  Height = 457
  Caption = '$AVZ0401'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
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
  object dgHosts: TRxDrawGrid
    Left = 0
    Top = 32
    Width = 692
    Height = 391
    Align = alClient
    ColCount = 1
    FixedCols = 0
    TabOrder = 0
    OnDrawCell = dgHostsDrawCell
    FixedButtons = True
    ColWidths = (
      669)
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 692
    Height = 32
    Align = alTop
    TabOrder = 1
    object sbSaveProcLog: TSpeedButton
      Left = 5
      Top = 5
      Width = 23
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
      OnClick = sbSaveProcLogClick
    end
    object mbDeleteItem: TBitBtn
      Left = 33
      Top = 5
      Width = 22
      Height = 22
      Hint = '$AVZ1096'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnClick = mbDeleteItemClick
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000010000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0FFFFF0FFFFFFFFFFFFFF000FFFFFFF
        0FFFFF000FFFFFF0FFFFFFF000FFFF00FFFFFFFF000FF00FFFFFFFFFF00000FF
        FFFFFFFFFF000FFFFFFFFFFFF00000FFFFFFFFFF000FF0FFFFFFFF0000FFFF00
        FFFFF0000FFFFFF00FFFF00FFFFFFFFF00FFFFFFFFFFFFFFFFFF}
    end
  end
end

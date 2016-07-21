object KeyloggerManager: TKeyloggerManager
  Left = 236
  Top = 167
  Width = 764
  Height = 459
  Caption = '$AVZ0394'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Panel3: TPanel
    Left = 0
    Top = 0
    Width = 756
    Height = 31
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object sbCopyToQurantine: TSpeedButton
      Left = 60
      Top = 6
      Width = 22
      Height = 22
      Hint = '$AVZ0360'
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFF1
        1111FFFFFFFFFFF11F11FFFFFF4444411F11FFFFFF4FFFF11111FFFFFF4F7771
        FFF10000004FFFFF111F0FFFFF4F77777F4F0F77774FFFFFFF4F0FFFFF4F77F4
        444F0F77774FFFF4F4FF0FFFFF4FFFF44FFF0F77F0444444FFFF0FFFF0F0FFFF
        FFFF0FFFF00FFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFF}
      ParentShowHint = False
      ShowHint = True
      OnClick = sbCopyToQurantineClick
    end
    object SpeedButton5: TSpeedButton
      Left = 84
      Top = 6
      Width = 22
      Height = 22
      Hint = '$AVZ0365'
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFF1
        1111FFFFFFFFFFF11F11FFFFFF4444411F11FFFFFF4FFFF11111FFFFFF4F7771
        FFF10000004FFFFF111F0FFFFF4F77777F4F0F77774FFFFFFF4F0FFFFF4F77F4
        444F0F77774FFFF4F4FF0FFFFF4FFFF44FFF0F77F0444444FFFF0FFFF0F0FFFF
        FFFF0FFFF00FFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFF}
      ParentShowHint = False
      ShowHint = True
      OnClick = SpeedButton5Click
    end
    object sbRefresh: TSpeedButton
      Left = 4
      Top = 5
      Width = 22
      Height = 22
      Hint = '$AVZ1188'
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FF0000000000
        000FFF0FFFFFFFFFFF0FFF0FFFFF2FFFFF0FFF0FFFF22FFFFF0FFF0FFF22222F
        FF0FFF0FFFF22FF2FF0FFF0FFFFF2FF2FF0FFF0FF2FFFFF2FF0FFF0FF2FF2FFF
        FF0FFF0FF2FF22FFFF0FFF0FFF22222FFF0FFF0FFFFF22FFFF0FFF0FFFFF2FF0
        000FFF0FFFFFFFF0F0FFFF0FFFFFFFF00FFFFF0000000000FFFF}
      ParentShowHint = False
      ShowHint = True
      OnClick = sbRefreshClick
    end
    object sbSaveLog: TSpeedButton
      Left = 29
      Top = 5
      Width = 23
      Height = 22
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
      OnClick = sbSaveLogClick
    end
  end
  object dgKeyloggerModules: TRxDrawGrid
    Left = 0
    Top = 31
    Width = 756
    Height = 394
    Align = alClient
    ColCount = 4
    FixedCols = 0
    PopupMenu = pmKernelTablePopUp
    TabOrder = 1
    OnClick = dgKeyloggerModulesClick
    OnDrawCell = dgKeyloggerModulesDrawCell
    FixedButtons = True
    ColWidths = (
      320
      398
      95
      64)
  end
  object pmKernelTablePopUp: TPopupMenu
    Left = 192
    Top = 152
    object mmQurantine: TMenuItem
      Caption = '$AVZ0360'
      OnClick = sbCopyToQurantineClick
    end
    object MenuItem4: TMenuItem
      Caption = '-'
    end
    object MenuItem5: TMenuItem
      Caption = '$AVZ0752'
      OnClick = MenuItem5Click
    end
    object MenuItem6: TMenuItem
      Caption = '$AVZ0754'
      OnClick = MenuItem6Click
    end
    object MenuItem7: TMenuItem
      Caption = '$AVZ0753'
      OnClick = MenuItem7Click
    end
  end
end

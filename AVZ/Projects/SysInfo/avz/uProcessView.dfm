object ProcessView: TProcessView
  Left = 236
  Top = 111
  Width = 691
  Height = 494
  Caption = '$AVZ0211'
  Color = clBtnFace
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 16
  object pcMain: TPageControl
    Left = 0
    Top = 0
    Width = 683
    Height = 460
    ActivePage = tsProcess
    Align = alClient
    TabOrder = 0
    OnChange = pcMainChange
    object tsProcess: TTabSheet
      Caption = '$AVZ0879'
      object Splitter1: TSplitter
        Left = 0
        Top = 255
        Width = 675
        Height = 3
        Cursor = crVSplit
        Align = alBottom
      end
      object pbDetail: TPageControl
        Left = 0
        Top = 258
        Width = 675
        Height = 171
        ActivePage = TabSheet3
        Align = alBottom
        Constraints.MinHeight = 100
        TabOrder = 0
        OnChange = pbDetailChange
        object TabSheet3: TTabSheet
          Caption = '$AVZ0320'
          ImageIndex = 1
          object dgDLL: TRxDrawGrid
            Left = 0
            Top = 0
            Width = 639
            Height = 140
            Align = alClient
            ColCount = 7
            DefaultRowHeight = 16
            FixedCols = 0
            PopupMenu = pmDLLPopUp
            TabOrder = 0
            OnClick = dgDLLClick
            OnDrawCell = dgDLLDrawCell
            FixedButtons = True
            OnFixedCellClick = dgDLLFixedCellClick
            ColWidths = (
              112
              108
              299
              277
              197
              276
              64)
          end
          object Panel2: TPanel
            Left = 639
            Top = 0
            Width = 28
            Height = 140
            Align = alRight
            BevelOuter = bvNone
            TabOrder = 1
            object sbCopyDllToQurantine: TSpeedButton
              Left = 5
              Top = 30
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
              OnClick = sbCopyDllToQurantineClick
            end
            object sbCopyAllDllToQurantine: TSpeedButton
              Left = 5
              Top = 54
              Width = 22
              Height = 22
              Hint = '$AVZ0361'
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
              OnClick = sbCopyAllDllToQurantineClick
            end
            object sbRefreshDLL: TSpeedButton
              Left = 5
              Top = 5
              Width = 22
              Height = 22
              Hint = '$AVZ0545'
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
              OnClick = sbRefreshDLLClick
            end
            object sbSaveDLLLog: TSpeedButton
              Left = 4
              Top = 80
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
              OnClick = sbSaveDLLLogClick
            end
            object sbCreateDump: TSpeedButton
              Left = 4
              Top = 108
              Width = 23
              Height = 22
              Hint = '$AVZ1007'
              Glyph.Data = {
                F6000000424DF600000000000000760000002800000010000000100000000100
                0400000000008000000000000000000000001000000010000000000000000000
                8000008000000080800080000000800080008080000080808000C0C0C0000000
                FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
                FFFFFFFFFFFFFF0FFFFFFFF000000000FFFFFFFFBFBFBF0B0FFFFFFB4BFBFB0B
                0FFFFFFF44BFBF0BB0FF4444444BFBF0B0FF44444444BFB0BB0F444444444BFB
                000044444444BFBF01104444444BFBFB000FFFFF44BFBF000FFFFFFB4BFBFB0F
                0FFFFFFFBFBFBF00FFFFFFF00000000FFFFFFFFFFFFFFFFFFFFF}
              ParentShowHint = False
              ShowHint = True
              OnClick = sbCreateDumpClick
            end
          end
        end
        object tsWindowsList: TTabSheet
          Caption = '$AVZ0565'
          ImageIndex = 1
          object dgWindowsList: TRxDrawGrid
            Left = 0
            Top = 0
            Width = 667
            Height = 140
            Align = alClient
            ColCount = 6
            DefaultRowHeight = 16
            FixedCols = 0
            TabOrder = 0
            OnDrawCell = dgWindowsListDrawCell
            FixedButtons = True
            ColWidths = (
              66
              259
              122
              142
              197
              440)
          end
        end
      end
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 675
        Height = 255
        Align = alClient
        TabOrder = 1
        object dgProcessList: TRxDrawGrid
          Left = 1
          Top = 1
          Width = 643
          Height = 253
          Align = alClient
          ColCount = 8
          Constraints.MinHeight = 100
          FixedCols = 0
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goDrawFocusSelected, goColSizing, goRowSelect]
          PopupMenu = pmProcessPopUp
          TabOrder = 0
          OnClick = dgProcessListClick
          OnDrawCell = dgProcessListDrawCell
          FixedButtons = True
          OnFixedCellClick = dgProcessListFixedCellClick
          ColWidths = (
            112
            48
            299
            277
            197
            440
            121
            64)
        end
        object Panel3: TPanel
          Left = 644
          Top = 1
          Width = 30
          Height = 253
          Align = alRight
          BevelOuter = bvNone
          TabOrder = 1
          object sbCopyProcToQurantine: TSpeedButton
            Left = 6
            Top = 78
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
            OnClick = sbCopyProcToQurantineClick
          end
          object SpeedButton5: TSpeedButton
            Left = 5
            Top = 102
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
          object sbRefreshPr: TSpeedButton
            Left = 5
            Top = 5
            Width = 22
            Height = 22
            Hint = '$AVZ0544 $AVZ1038'
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
            OnClick = sbRefreshPrClick
          end
          object sbTerminateProcess: TSpeedButton
            Left = 5
            Top = 30
            Width = 22
            Height = 22
            Hint = '$AVZ0243'
            Glyph.Data = {
              F6000000424DF600000000000000760000002800000010000000100000000100
              0400000000008000000000000000000000001000000010000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
              FFFFFFFFFFFFFFFFF1FFFFF1FFFFFFFFFFFFFF111FFFFFFF1FFFFF111FFFFFF1
              FFFFFFF111FFFF11FFFFFFFF111FF11FFFFFFFFFF11111FFFFFFFFFFFF111FFF
              FFFFFFFFF11111FFFFFFFFFF111FF1FFFFFFFF1111FFFF11FFFFF1111FFFFFF1
              1FFFF11FFFFFFFFF11FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
            ParentShowHint = False
            ShowHint = True
            OnClick = sbTerminateProcessClick
          end
          object sbSaveProcLog: TSpeedButton
            Left = 4
            Top = 136
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
        end
      end
    end
    object TabSheet1: TTabSheet
      Caption = '$AVZ0695'
      ImageIndex = 1
      object cbCalkMD5Summ: TCheckBox
        Left = 2
        Top = 5
        Width = 449
        Height = 17
        Caption = '$AVZ0197'
        TabOrder = 0
        OnClick = cbCalkMD5SummClick
      end
      object cbCheckFile: TCheckBox
        Left = 2
        Top = 24
        Width = 449
        Height = 17
        Caption = '$AVZ0844'
        Checked = True
        State = cbChecked
        TabOrder = 1
      end
      object cbCheckWinTrust: TCheckBox
        Left = 2
        Top = 43
        Width = 391
        Height = 17
        Caption = '$AVZ0845'
        Checked = True
        State = cbChecked
        TabOrder = 2
      end
    end
  end
  object pmProcessPopUp: TPopupMenu
    Left = 232
    Top = 128
    object N1: TMenuItem
      Caption = '$AVZ0243'
      OnClick = N1Click
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object N3: TMenuItem
      Caption = '$AVZ0360'
      OnClick = N3Click
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object pmiGoogle: TMenuItem
      Caption = '$AVZ0752'
      OnClick = pmiGoogleClick
    end
    object pmiYandex: TMenuItem
      Caption = '$AVZ0754'
      OnClick = pmiYandexClick
    end
    object pmiRambler: TMenuItem
      Caption = '$AVZ0753'
      OnClick = pmiRamblerClick
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object N6: TMenuItem
      Caption = '$AVZ0755'
      OnClick = N6Click
    end
    object N7: TMenuItem
      Caption = '$AVZ0756'
      OnClick = N7Click
    end
    object N12: TMenuItem
      Caption = '-'
    end
    object pmAVZGuard1: TMenuItem
      Caption = '$AVZ0006'
      Visible = False
      OnClick = pmAVZGuard1Click
    end
  end
  object pmDLLPopUp: TPopupMenu
    Left = 224
    Top = 336
    object MenuItem3: TMenuItem
      Caption = '$AVZ0360'
      OnClick = MenuItem3Click
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
    object N8: TMenuItem
      Caption = '-'
    end
    object N9: TMenuItem
      Caption = '$AVZ0755'
      OnClick = N9Click
    end
    object N10: TMenuItem
      Caption = '$AVZ0756'
      OnClick = N10Click
    end
    object N11: TMenuItem
      Caption = '-'
    end
    object DLL1: TMenuItem
      Caption = '$AVZ0803'
      OnClick = DLL1Click
    end
  end
end

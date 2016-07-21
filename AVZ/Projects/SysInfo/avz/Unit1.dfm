object Main: TMain
  Left = 275
  Top = 232
  Width = 704
  Height = 565
  HelpType = htKeyword
  HelpContext = 1
  BiDiMode = bdLeftToRight
  Caption = '$AVZ0063'
  Color = clBtnFace
  Constraints.MinHeight = 475
  Constraints.MinWidth = 700
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  Menu = MainMenu1
  OldCreateOrder = False
  ParentBiDiMode = False
  Position = poScreenCenter
  Scaled = False
  ShowHint = True
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 16
  object GroupBox1: TGroupBox
    Left = 0
    Top = 281
    Width = 696
    Height = 210
    Align = alClient
    Caption = ' $AVZ0868 '
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    DesignSize = (
      696
      210)
    object SpeedButton2: TSpeedButton
      Left = 667
      Top = 16
      Width = 24
      Height = 23
      Hint = '$AVZ1032'
      Anchors = [akTop, akRight]
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000010000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
        FFFFFF0000000000000FF03300000088030FF03300000088030FF03300000088
        030FF03300000000030FF03333333333330FF03300000000330FF03088888888
        030FF03088888888030FF03088888888030FF03088888888030FF03088888888
        000FF03088888888080FF00000000000000FFFFFFFFFFFFFFFFF}
      OnClick = SpeedButton2Click
    end
    object sbDetailLogView: TSpeedButton
      Left = 667
      Top = 42
      Width = 24
      Height = 23
      Hint = '$AVZ0861'
      Anchors = [akTop, akRight]
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000010000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
        FFFFF000FFFF000FFFFF08E80FF08E80FFFF0E8E0FF0E8E0FFFF0FE80FF0FE80
        FFFF0EFE0FF0EFE0FFFF7000800F0007FFFFB087FFFFFFF0FFF07F07BF0FFFFF
        0FF0FBF7FF0FFFFFF0F0777FBFBFFFFFFF0FFB7BF07FFFFFFFFFF7F7BFF7FFFF
        FFFF7FF7FFFFFFFFFFFFBFF7BFFFFFFFFFFFFFFFFFFFFFFFFFFF}
      OnClick = sbDetailLogViewClick
    end
    object SpeedButton1: TSpeedButton
      Left = 667
      Top = 68
      Width = 24
      Height = 23
      Hint = '$AVZ0626'
      Anchors = [akTop, akRight]
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000010000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFF00000000000FFFFF0FFFFFFFFF0FFFFF0FFFFFFFFF
        0FFFFF0FFFFFFFFF0FFFFF0FFFFFFFFF0FFFFF0FFFFFFFFF0FFFFF0FFFFFFFFF
        0FFFFF0FFFFFFFFF0FFFFF0FFFFFFFFF0FFFFF0FFFFFF0000FFFFF0FFFFFF0F0
        FFFFFF0FFFFFF00FFFFFFF00000000FFFFFFFFFFFFFFFFFFFFFF}
      OnClick = SpeedButton1Click
    end
    object sbAutoUpdate: TSpeedButton
      Left = 667
      Top = 100
      Width = 24
      Height = 23
      Hint = '$AVZ0550'
      Anchors = [akTop, akRight]
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000010000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFF0FFFF
        FFFFFFFFFF00FFFFFFFFFFFFF0E00000FFFFFFFF0EEEEEE0FFFFFFF70EEEEEE0
        FFFFFF0F20E00000FFFFF0F2F0002F0FFFFF7F2F06C0F2F70FFF02F06C6C0F27
        00FF000ECEC600000E0F0EECECEC0EEEEEE00EC00ECE0EEEEEE07E0F200C0000
        0E0FF0F2F2F0CE0F00FFFF0F2F2F00FF0FFFFFF700007FFFFFFF}
      OnClick = sbAutoUpdateClick
    end
    object LogMemo: TRichEdit
      Left = 2
      Top = 18
      Width = 661
      Height = 190
      Anchors = [akLeft, akTop, akRight, akBottom]
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      HideSelection = False
      HideScrollBars = False
      ParentFont = False
      PlainText = True
      ReadOnly = True
      ScrollBars = ssBoth
      TabOrder = 0
      WordWrap = False
    end
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 491
    Width = 696
    Height = 20
    Panels = <
      item
        Width = 400
      end
      item
        Alignment = taCenter
        Width = 120
      end
      item
        Width = 150
      end>
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 696
    Height = 281
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 2
    DesignSize = (
      696
      281)
    object mbPause: TSpeedButton
      Left = 471
      Top = 255
      Width = 81
      Height = 25
      AllowAllUp = True
      Anchors = [akTop, akRight, akBottom]
      GroupIndex = 1
      Caption = '$AVZ0699'
      Enabled = False
      Glyph.Data = {
        E6080000424DE60800000000000036040000280000003C000000140000000100
        080000000000B004000000000000000000000001000000000000000000000000
        80000080000000808000800000008000800080800000C0C0C000C0DCC000F0CA
        A6000020400000206000002080000020A0000020C0000020E000004000000040
        20000040400000406000004080000040A0000040C0000040E000006000000060
        20000060400000606000006080000060A0000060C0000060E000008000000080
        20000080400000806000008080000080A0000080C0000080E00000A0000000A0
        200000A0400000A0600000A0800000A0A00000A0C00000A0E00000C0000000C0
        200000C0400000C0600000C0800000C0A00000C0C00000C0E00000E0000000E0
        200000E0400000E0600000E0800000E0A00000E0C00000E0E000400000004000
        20004000400040006000400080004000A0004000C0004000E000402000004020
        20004020400040206000402080004020A0004020C0004020E000404000004040
        20004040400040406000404080004040A0004040C0004040E000406000004060
        20004060400040606000406080004060A0004060C0004060E000408000004080
        20004080400040806000408080004080A0004080C0004080E00040A0000040A0
        200040A0400040A0600040A0800040A0A00040A0C00040A0E00040C0000040C0
        200040C0400040C0600040C0800040C0A00040C0C00040C0E00040E0000040E0
        200040E0400040E0600040E0800040E0A00040E0C00040E0E000800000008000
        20008000400080006000800080008000A0008000C0008000E000802000008020
        20008020400080206000802080008020A0008020C0008020E000804000008040
        20008040400080406000804080008040A0008040C0008040E000806000008060
        20008060400080606000806080008060A0008060C0008060E000808000008080
        20008080400080806000808080008080A0008080C0008080E00080A0000080A0
        200080A0400080A0600080A0800080A0A00080A0C00080A0E00080C0000080C0
        200080C0400080C0600080C0800080C0A00080C0C00080C0E00080E0000080E0
        200080E0400080E0600080E0800080E0A00080E0C00080E0E000C0000000C000
        2000C0004000C0006000C0008000C000A000C000C000C000E000C0200000C020
        2000C0204000C0206000C0208000C020A000C020C000C020E000C0400000C040
        2000C0404000C0406000C0408000C040A000C040C000C040E000C0600000C060
        2000C0604000C0606000C0608000C060A000C060C000C060E000C0800000C080
        2000C0804000C0806000C0808000C080A000C080C000C080E000C0A00000C0A0
        2000C0A04000C0A06000C0A08000C0A0A000C0A0C000C0A0E000C0C00000C0C0
        2000C0C04000C0C06000C0C08000C0C0A000F0FBFF00A4A0A000808080000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD040404
        040404FDFDFDFDFDFDFDFDFDFDFDFDFDFDA4A4A4A4A4A4FDFDFDFDFDFDFDFDFD
        FDFDFDFDFD040404040404FDFDFDFDFDFDFDFDFDFDFDFD040404040404040404
        04FDFDFDFDFDFDFDFDFDFDA4A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFDFDFDFD04
        040404040404040404FDFDFDFDFDFDFDFDFD0404C0C0C0C0C0C0C0040404FDFD
        FDFDFDFDFDFDA4A4A4A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFDFD0404C0C0C0C0
        C0C0C0040404FDFDFDFDFDFDFDFCC0C0C0C0C0C0C0C0C0C0C00404FDFDFDFDFD
        FDA4A4A4A4A4A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFCC0C0C0C0C0C0C0C0C0C0
        C00404FDFDFDFDFD04C0C0C0C0C0C0C0C0C0C0C0C0C00404FDFDFDFDA4A4A4A4
        A4A4A4A4A4A4A4A4A4A4A4A4FDFDFDFD04C0C0C0C0C0C0C0C0C0C0C0C0C00404
        FDFDFDFD04C0C0040404040404040404C0C00404FDFDFDFDA4A4A4A4A4A4A4A4
        A4A4A4A4A4A4A4A4FDFDFDFD04C0C0040404040404040404C0C00404FDFDFDFC
        C0C0C004FFFFFF04FFFFFF04C0C0C00404FDFDF7A4A4A4A4FFFFFFA4FFFFFFA4
        A4A4A4A4A4FDFDFCC0C0C004FBFBFB04FBFBFB04C0C0C00404FDFDFCC0C0C004
        FFFFFF04FFFFFF04C0C0C00404FDFDF7A4A4A4A4FFFFFFA4FFFFFFA4A4A4A4A4
        A4FDFDFCC0C0C004FBFBFB04FBFBFB04C0C0C00404FDFDFCC0C0C004FFFFFF04
        FFFFFF04C0C0C00404FDFDF7A4A4A4A4FFFFFFA4FFFFFFA4A4A4A4A4A4FDFDFC
        C0C0C004FBFBFB04FBFBFB04C0C0C00404FDFDFCC0C0C004FFFFFF04FFFFFF04
        C0C0C00404FDFDF7A4A4A4A4FFFFFFA4FFFFFFA4A4A4A4A4A4FDFDFCC0C0C004
        FBFBFB04FBFBFB04C0C0C00404FDFDFCC0C0C004FFFFFF04FFFFFF04C0C0C004
        04FDFDF7A4A4A4A4FFFFFFA4FFFFFFA4A4A4A4A4A4FDFDFCC0C0C004FBFBFB04
        FBFBFB04C0C0C00404FDFDFCFCC0C004FFFFFF04FFFFFF04C0C0C00404FDFDF7
        A4A4A4A4FFFFFFA4FFFFFFA4A4A4A4A4A4FDFDFCFCC0C004FBFBFB04FBFBFB04
        C0C0C00404FDFDFDFCC0C004FFFFFF04FFFFFF04C0C0C004FDFDFDFDF7A4A4A4
        FFFFFFA4FFFFFFA4A4A4A4A4FDFDFDFDFCC0C004FBFBFB04FBFBFB04C0C0C004
        FDFDFDFDFCFCC0040404040404040404C0C00404FDFDFDFDF7A4A4A4A4A4A4A4
        A4A4A4A4A4A4A4A4FDFDFDFDFCFCC0040404040404040404C0C00404FDFDFDFD
        FDFCFCC0C0C0C0C0C0C0C0C0C00404FDFDFDFDFDFDF7F7A4A4A4A4A4A4A4A4A4
        A4A4A4FDFDFDFDFDFDFCFCC0C0C0C0C0C0C0C0C0C00404FDFDFDFDFDFDFDFCFC
        C0FCFCC0C0C0C0C00404FDFDFDFDFDFDFDFDF7F7A4A4A4A4A4A4A4A4A4A4FDFD
        FDFDFDFDFDFDFCFCC0FCFCC0C0C0C0C00404FDFDFDFDFDFDFDFDFDFCFCC0C0C0
        C0C0C0FCFCFDFDFDFDFDFDFDFDFDFDF7A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFD
        FDFDFDFCFCC0C0C0C0C0C0FCFCFDFDFDFDFDFDFDFDFDFDFDFDFCFCFCFCFCFCFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDF7F7F7F7F7F7FDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFCFCFCFCFCFCFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFD}
      NumGlyphs = 3
      OnClick = mbPauseClick
    end
    object mbStopScript: TBitBtn
      Left = 388
      Top = 255
      Width = 217
      Height = 25
      Hint = '$AVZ0600'
      Anchors = [akTop, akRight]
      Caption = '$AVZ0795'
      TabOrder = 4
      Visible = False
      OnClick = mbStopScriptClick
      Glyph.Data = {
        E6080000424DE60800000000000036040000280000003C000000140000000100
        080000000000B004000000000000000000000001000000000000000000000000
        80000080000000808000800000008000800080800000C0C0C000C0DCC000F0CA
        A6000020400000206000002080000020A0000020C0000020E000004000000040
        20000040400000406000004080000040A0000040C0000040E000006000000060
        20000060400000606000006080000060A0000060C0000060E000008000000080
        20000080400000806000008080000080A0000080C0000080E00000A0000000A0
        200000A0400000A0600000A0800000A0A00000A0C00000A0E00000C0000000C0
        200000C0400000C0600000C0800000C0A00000C0C00000C0E00000E0000000E0
        200000E0400000E0600000E0800000E0A00000E0C00000E0E000400000004000
        20004000400040006000400080004000A0004000C0004000E000402000004020
        20004020400040206000402080004020A0004020C0004020E000404000004040
        20004040400040406000404080004040A0004040C0004040E000406000004060
        20004060400040606000406080004060A0004060C0004060E000408000004080
        20004080400040806000408080004080A0004080C0004080E00040A0000040A0
        200040A0400040A0600040A0800040A0A00040A0C00040A0E00040C0000040C0
        200040C0400040C0600040C0800040C0A00040C0C00040C0E00040E0000040E0
        200040E0400040E0600040E0800040E0A00040E0C00040E0E000800000008000
        20008000400080006000800080008000A0008000C0008000E000802000008020
        20008020400080206000802080008020A0008020C0008020E000804000008040
        20008040400080406000804080008040A0008040C0008040E000806000008060
        20008060400080606000806080008060A0008060C0008060E000808000008080
        20008080400080806000808080008080A0008080C0008080E00080A0000080A0
        200080A0400080A0600080A0800080A0A00080A0C00080A0E00080C0000080C0
        200080C0400080C0600080C0800080C0A00080C0C00080C0E00080E0000080E0
        200080E0400080E0600080E0800080E0A00080E0C00080E0E000C0000000C000
        2000C0004000C0006000C0008000C000A000C000C000C000E000C0200000C020
        2000C0204000C0206000C0208000C020A000C020C000C020E000C0400000C040
        2000C0404000C0406000C0408000C040A000C040C000C040E000C0600000C060
        2000C0604000C0606000C0608000C060A000C060C000C060E000C0800000C080
        2000C0804000C0806000C0808000C080A000C080C000C080E000C0A00000C0A0
        2000C0A04000C0A06000C0A08000C0A0A000C0A0C000C0A0E000C0C00000C0C0
        2000C0C04000C0C06000C0C08000C0C0A000F0FBFF00A4A0A000808080000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD040404
        040404FDFDFDFDFDFDFDFDFDFDFDFDFDFDA4A4A4A4A4A4FDFDFDFDFDFDFDFDFD
        FDFDFDFDFD040404040404FDFDFDFDFDFDFDFDFDFDFDFD040404040404040404
        04FDFDFDFDFDFDFDFDFDFDA4A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFDFDFDFD04
        040404040404040404FDFDFDFDFDFDFDFDFD0404C0C0C0C0C0C0C0040404FDFD
        FDFDFDFDFDFDA4A4A4A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFDFD0404C0C0C0C0
        C0C0C0040404FDFDFDFDFDFDFDFCC0C0C0C0C0C0C0C0C0C0C00404FDFDFDFDFD
        FDA4A4A4A4A4A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFCC0C0C0C0C0C0C0C0C0C0
        C00404FDFDFDFDFD04C0C0C0C0C0C0C0C0C0C0C0C0C00404FDFDFDFDA4A4A4A4
        A4A4A4A4A4A4A4A4A4A4A4A4FDFDFDFD04C0C0C0C0C0C0C0C0C0C0C0C0C00404
        FDFDFDFD04C0C0040404040404040404C0C00404FDFDFDFDA4A4A4A4A4A4A4A4
        A4A4A4A4A4A4A4A4FDFDFDFD04C0C0040404040404040404C0C00404FDFDFDFC
        C0C0C004FFFFFF04FFFFFF04C0C0C00404FDFDF7A4A4A4A4FFFFFFA4FFFFFFA4
        A4A4A4A4A4FDFDFCC0C0C004FBFBFB04FBFBFB04C0C0C00404FDFDFCC0C0C004
        FFFFFF04FFFFFF04C0C0C00404FDFDF7A4A4A4A4FFFFFFA4FFFFFFA4A4A4A4A4
        A4FDFDFCC0C0C004FBFBFB04FBFBFB04C0C0C00404FDFDFCC0C0C004FFFFFF04
        FFFFFF04C0C0C00404FDFDF7A4A4A4A4FFFFFFA4FFFFFFA4A4A4A4A4A4FDFDFC
        C0C0C004FBFBFB04FBFBFB04C0C0C00404FDFDFCC0C0C004FFFFFF04FFFFFF04
        C0C0C00404FDFDF7A4A4A4A4FFFFFFA4FFFFFFA4A4A4A4A4A4FDFDFCC0C0C004
        FBFBFB04FBFBFB04C0C0C00404FDFDFCC0C0C004FFFFFF04FFFFFF04C0C0C004
        04FDFDF7A4A4A4A4FFFFFFA4FFFFFFA4A4A4A4A4A4FDFDFCC0C0C004FBFBFB04
        FBFBFB04C0C0C00404FDFDFCFCC0C004FFFFFF04FFFFFF04C0C0C00404FDFDF7
        A4A4A4A4FFFFFFA4FFFFFFA4A4A4A4A4A4FDFDFCFCC0C004FBFBFB04FBFBFB04
        C0C0C00404FDFDFDFCC0C004FFFFFF04FFFFFF04C0C0C004FDFDFDFDF7A4A4A4
        FFFFFFA4FFFFFFA4A4A4A4A4FDFDFDFDFCC0C004FBFBFB04FBFBFB04C0C0C004
        FDFDFDFDFCFCC0040404040404040404C0C00404FDFDFDFDF7A4A4A4A4A4A4A4
        A4A4A4A4A4A4A4A4FDFDFDFDFCFCC0040404040404040404C0C00404FDFDFDFD
        FDFCFCC0C0C0C0C0C0C0C0C0C00404FDFDFDFDFDFDF7F7A4A4A4A4A4A4A4A4A4
        A4A4A4FDFDFDFDFDFDFCFCC0C0C0C0C0C0C0C0C0C00404FDFDFDFDFDFDFDFCFC
        C0FCFCC0C0C0C0C00404FDFDFDFDFDFDFDFDF7F7A4A4A4A4A4A4A4A4A4A4FDFD
        FDFDFDFDFDFDFCFCC0FCFCC0C0C0C0C00404FDFDFDFDFDFDFDFDFDFCFCC0C0C0
        C0C0C0FCFCFDFDFDFDFDFDFDFDFDFDF7A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFD
        FDFDFDFCFCC0C0C0C0C0C0FCFCFDFDFDFDFDFDFDFDFDFDFDFDFCFCFCFCFCFCFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDF7F7F7F7F7F7FDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFCFCFCFCFCFCFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFD}
      NumGlyphs = 3
    end
    object psScanZone: TPageControl
      Left = 1
      Top = 0
      Width = 381
      Height = 280
      ActivePage = tsSelectDisk
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      object tsSelectDisk: TTabSheet
        Caption = '$AVZ0531'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        DesignSize = (
          373
          249)
        object ZShellTreeView: TZShellTreeView
          Left = 0
          Top = 0
          Width = 373
          Height = 180
          Align = alTop
          Anchors = [akLeft, akTop, akRight, akBottom]
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          Indent = 19
          ParentFont = False
          PopupMenu = pmDirTreeMenu
          ReadOnly = True
          TabOrder = 0
          OnKeyPress = ZShellTreeViewKeyPress
          OnTreeNodeStateIconClick = ZShellTreeViewTreeNodeStateIconClick
          OnNewTreeNodeSetState = ZShellTreeViewNewTreeNodeSetState
        end
        object cbCheckProcess: TCheckBox
          Left = 2
          Top = 199
          Width = 257
          Height = 17
          Hint = '$AVZ0814'
          Anchors = [akLeft, akBottom]
          Caption = '$AVZ0840 '
          Checked = True
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          State = cbChecked
          TabOrder = 1
        end
        object cbEvSysCheck: TCheckBox
          Left = 2
          Top = 216
          Width = 257
          Height = 17
          Hint = '$AVZ1149'
          Anchors = [akLeft, akBottom]
          Caption = '$AVZ1150 '
          Checked = True
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          State = cbChecked
          TabOrder = 2
        end
        object pbScanProgress: TProgressBar
          Left = 0
          Top = 185
          Width = 373
          Height = 10
          Anchors = [akLeft, akRight, akBottom]
          Step = 100
          TabOrder = 3
        end
        object cbEvSysIPU: TCheckBox
          Left = 2
          Top = 232
          Width = 343
          Height = 17
          Caption = '$AVZ0773'
          Checked = True
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          State = cbChecked
          TabOrder = 4
        end
      end
      object TabSheet1: TTabSheet
        Caption = '$AVZ1064'
        ImageIndex = 1
        DesignSize = (
          373
          249)
        object Label10: TLabel
          Left = 326
          Top = 223
          Width = 61
          Height = 16
          Caption = '$AVZ1190'
        end
        object rgFileTypes: TRadioGroup
          Left = 5
          Top = -1
          Width = 365
          Height = 71
          Hint = '$AVZ0791'
          Anchors = [akLeft, akTop, akRight]
          Caption = ' $AVZ1064 '
          Items.Strings = (
            '$AVZ0790'
            '$AVZ0158'
            '$AVZ1127')
          TabOrder = 0
          OnClick = rgFileTypesClick
        end
        object cbIncludeFiles: TCheckBox
          Left = 7
          Top = 70
          Width = 242
          Height = 17
          Hint = '$AVZ1100'
          Caption = '$AVZ0107:'
          TabOrder = 1
          OnClick = cbIncludeFilesClick
        end
        object edIncludeFiles: TEdit
          Left = 32
          Top = 86
          Width = 339
          Height = 24
          Hint = '$AVZ1035'
          Anchors = [akLeft, akTop, akRight]
          Enabled = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 2
        end
        object edExcludeFiles: TEdit
          Left = 32
          Top = 127
          Width = 339
          Height = 24
          Hint = '$AVZ1035'
          Anchors = [akLeft, akTop, akRight]
          Enabled = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 3
        end
        object cbExcludeFiles: TCheckBox
          Left = 7
          Top = 110
          Width = 282
          Height = 17
          Hint = '$AVZ1101'
          Caption = '$AVZ0313:'
          TabOrder = 4
          OnClick = cbExcludeFilesClick
        end
        object cbRepGood: TCheckBox
          Left = 7
          Top = 153
          Width = 212
          Height = 17
          Hint = '$AVZ0111'
          Caption = '$AVZ0624'
          TabOrder = 5
          OnClick = cbRepGoodClick
        end
        object cbCheckArchives: TCheckBox
          Left = 7
          Top = 206
          Width = 153
          Height = 17
          Hint = '$AVZ0106'
          Caption = '$AVZ0839'
          Checked = True
          State = cbChecked
          TabOrder = 6
          OnClick = cbCheckArchivesClick
        end
        object cbMaxArcSize: TCheckBox
          Left = 24
          Top = 224
          Width = 225
          Height = 17
          Hint = '$AVZ0078'
          Caption = '$AVZ0483'
          Checked = True
          State = cbChecked
          TabOrder = 7
        end
        object seMaxArcSize: TRxSpinEdit
          Left = 255
          Top = 215
          Width = 65
          Height = 24
          Hint = '$AVZ0380 '
          MaxValue = 100.000000000000000000
          MinValue = 1.000000000000000000
          Value = 10.000000000000000000
          TabOrder = 8
        end
        object cbRepGoodCheck: TCheckBox
          Left = 24
          Top = 171
          Width = 345
          Height = 17
          Hint = '$AVZ0847'
          Caption = '$AVZ0846'
          Enabled = False
          TabOrder = 9
        end
        object cbCheckNTFSStream: TCheckBox
          Left = 7
          Top = 189
          Width = 185
          Height = 17
          Hint = '$AVZ0851'
          Caption = '$AVZ0842'
          Checked = True
          State = cbChecked
          TabOrder = 10
        end
      end
      object TabSheet2: TTabSheet
        Caption = '$AVZ0697'
        ImageIndex = 1
        object GroupBox2: TGroupBox
          Left = 0
          Top = 2
          Width = 367
          Height = 74
          Caption = ' $AVZ1151 '
          TabOrder = 0
          DesignSize = (
            367
            74)
          object Label7: TLabel
            Left = 65
            Top = 19
            Width = 64
            Height = 16
            Caption = '$AVZ1060:'
          end
          object lbEvLevel: TLabel
            Left = 72
            Top = 35
            Width = 282
            Height = 16
            Anchors = [akLeft, akTop, akRight, akBottom]
            AutoSize = False
            Caption = 'lbEvLevel'
          end
          object Label2: TLabel
            Tag = 7
            Left = 351
            Top = 10
            Width = 11
            Height = 20
            Cursor = crHelp
            Anchors = [akTop, akRight]
            Caption = '?'
            Font.Charset = RUSSIAN_CHARSET
            Font.Color = clBlue
            Font.Height = -16
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
            OnClick = lbHelpLabelClick
          end
          object tbEvLevel: TTrackBar
            Left = 10
            Top = 14
            Width = 42
            Height = 55
            Hint = '$AVZ0908'
            Max = 3
            Orientation = trVertical
            PageSize = 1
            TabOrder = 0
            TickMarks = tmBoth
            OnChange = tbEvLevelChange
          end
          object cbExtEv: TCheckBox
            Left = 67
            Top = 53
            Width = 168
            Height = 17
            Caption = '$AVZ0901'
            TabOrder = 1
          end
        end
        object GroupBox3: TGroupBox
          Left = 0
          Top = 76
          Width = 367
          Height = 69
          Caption = ' Anti-RootKit '
          TabOrder = 1
          DesignSize = (
            367
            69)
          object Label1: TLabel
            Tag = 8
            Left = 351
            Top = 10
            Width = 11
            Height = 20
            Cursor = crHelp
            Anchors = [akTop, akRight]
            Caption = '?'
            Font.Charset = RUSSIAN_CHARSET
            Font.Color = clBlue
            Font.Height = -16
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
            OnClick = lbHelpLabelClick
          end
          object cbCheckRootKit: TCheckBox
            Left = 8
            Top = 16
            Width = 313
            Height = 17
            Caption = '$AVZ0210'
            Checked = True
            State = cbChecked
            TabOrder = 0
            OnClick = cbCheckRootKitClick
          end
          object cbAntiRootKitSystemUser: TCheckBox
            Left = 24
            Top = 32
            Width = 329
            Height = 17
            Caption = '$AVZ0077'
            TabOrder = 1
          end
          object cbAntiRootKitSystemKernel: TCheckBox
            Left = 24
            Top = 48
            Width = 329
            Height = 17
            Hint = '$AVZ0126'
            Caption = '$AVZ0076'
            Font.Charset = RUSSIAN_CHARSET
            Font.Color = clRed
            Font.Height = -13
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            TabOrder = 2
          end
        end
        object GroupBox4: TGroupBox
          Left = 1
          Top = 147
          Width = 367
          Height = 55
          Caption = ' Winsock  Service Provider '
          TabOrder = 2
          DesignSize = (
            367
            55)
          object Label9: TLabel
            Tag = 9
            Left = 351
            Top = 11
            Width = 11
            Height = 20
            Cursor = crHelp
            Anchors = [akTop, akRight]
            Caption = '?'
            Font.Charset = RUSSIAN_CHARSET
            Font.Color = clBlue
            Font.Height = -16
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
            OnClick = lbHelpLabelClick
          end
          object cbCheckLSP: TCheckBox
            Left = 8
            Top = 19
            Width = 313
            Height = 17
            Caption = '$AVZ0841'
            Checked = True
            State = cbChecked
            TabOrder = 0
            OnClick = cbCheckLSPClick
          end
          object cbAutoRepairLSP: TCheckBox
            Left = 24
            Top = 35
            Width = 321
            Height = 17
            Caption = '$AVZ0042'
            TabOrder = 1
          end
        end
        object cbKeyloggerSearch: TCheckBox
          Left = 3
          Top = 205
          Width = 358
          Height = 17
          Caption = '$AVZ0761'
          Checked = True
          State = cbChecked
          TabOrder = 3
        end
        object cbSearchTrojanPorts: TCheckBox
          Left = 3
          Top = 222
          Width = 342
          Height = 17
          Caption = '$AVZ0772'
          TabOrder = 4
        end
      end
    end
    object gbSetup: TGroupBox
      Left = 386
      Top = 0
      Width = 309
      Height = 251
      Anchors = [akTop, akRight]
      Caption = ' $AVZ0409 '
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      object lbClassVir: TLabel
        Tag = 1
        Left = 12
        Top = 45
        Width = 64
        Height = 16
        Cursor = crHelp
        Caption = '$AVZ0105:'
        Color = clBtnFace
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clBlue
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsUnderline]
        ParentColor = False
        ParentFont = False
        OnClick = lbHelpLabelClick
      end
      object Label3: TLabel
        Tag = 2
        Left = 12
        Top = 70
        Width = 60
        Height = 16
        Cursor = crHelp
        Caption = 'AdvWare:'
        Color = clBtnFace
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clBlue
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsUnderline]
        ParentColor = False
        ParentFont = False
        OnClick = lbHelpLabelClick
      end
      object Label4: TLabel
        Tag = 3
        Left = 12
        Top = 95
        Width = 84
        Height = 16
        Cursor = crHelp
        Caption = 'Spy/Spyware:'
        Color = clBtnFace
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clBlue
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsUnderline]
        ParentColor = False
        ParentFont = False
        OnClick = lbHelpLabelClick
      end
      object Label5: TLabel
        Tag = 5
        Left = 12
        Top = 143
        Width = 63
        Height = 16
        Cursor = crHelp
        Caption = 'HackTool:'
        Color = clBtnFace
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clBlue
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsUnderline]
        ParentColor = False
        ParentFont = False
        OnClick = lbHelpLabelClick
      end
      object Label6: TLabel
        Tag = 6
        Left = 12
        Top = 168
        Width = 63
        Height = 16
        Cursor = crHelp
        Caption = 'RiskWare:'
        Color = clBtnFace
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clBlue
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsUnderline]
        ParentColor = False
        ParentFont = False
        OnClick = lbHelpLabelClick
      end
      object Label8: TLabel
        Tag = 4
        Left = 12
        Top = 119
        Width = 104
        Height = 16
        Cursor = crHelp
        Caption = 'Dialer/PornWare:'
        Color = clBtnFace
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clBlue
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsUnderline]
        ParentColor = False
        ParentFont = False
        OnClick = lbHelpLabelClick
      end
      object cbDeleteVirus: TCheckBox
        Left = 6
        Top = 20
        Width = 251
        Height = 17
        Caption = '$AVZ0193:'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        OnClick = cbDeleteVirusClick
      end
      object cbModeVirus: TComboBox
        Left = 124
        Top = 37
        Width = 183
        Height = 24
        Style = csDropDownList
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 16
        ParentFont = False
        TabOrder = 1
        Items.Strings = (
          '$AVZ1067'
          '$AVZ1097'
          '$AVZ1042')
      end
      object cbModeAdvWare: TComboBox
        Left = 124
        Top = 62
        Width = 183
        Height = 24
        Style = csDropDownList
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 16
        ParentFont = False
        TabOrder = 2
        Items.Strings = (
          '$AVZ1067'
          '$AVZ1097'
          '$AVZ1042')
      end
      object cbModeSpy: TComboBox
        Left = 124
        Top = 87
        Width = 183
        Height = 24
        Style = csDropDownList
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 16
        ParentFont = False
        TabOrder = 3
        Items.Strings = (
          '$AVZ1067'
          '$AVZ1097'
          '$AVZ1042')
      end
      object cbModeHackTools: TComboBox
        Left = 124
        Top = 135
        Width = 183
        Height = 24
        Style = csDropDownList
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 16
        ParentFont = False
        TabOrder = 4
        Items.Strings = (
          '$AVZ1067'
          '$AVZ0379'
          '$AVZ1042')
      end
      object cbModeRiskWare: TComboBox
        Left = 124
        Top = 160
        Width = 183
        Height = 24
        Style = csDropDownList
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 16
        ParentFont = False
        TabOrder = 5
        Items.Strings = (
          '$AVZ1067'
          '$AVZ1097'
          '$AVZ1042')
      end
      object cbInfected: TCheckBox
        Left = 13
        Top = 212
        Width = 292
        Height = 17
        Caption = '$AVZ0373'
        Enabled = False
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 6
      end
      object cbQuarantine: TCheckBox
        Left = 13
        Top = 228
        Width = 292
        Height = 17
        Caption = '$AVZ0372'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 7
      end
      object cbModePornWare: TComboBox
        Left = 124
        Top = 111
        Width = 183
        Height = 24
        Style = csDropDownList
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 16
        ParentFont = False
        TabOrder = 8
        Items.Strings = (
          '$AVZ1067'
          '$AVZ1097'
          '$AVZ1042')
      end
      object cbExtFileDelete: TCheckBox
        Left = 13
        Top = 195
        Width = 252
        Height = 17
        Caption = '$AVZ1153'
        Checked = True
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        State = cbChecked
        TabOrder = 9
      end
    end
    object mbStart: TBitBtn
      Left = 386
      Top = 255
      Width = 81
      Height = 25
      Hint = '$AVZ0280'
      Anchors = [akTop, akRight]
      Caption = '$AVZ0882'
      TabOrder = 2
      OnClick = mbStartClick
      Glyph.Data = {
        E6080000424DE60800000000000036040000280000003C000000140000000100
        080000000000B004000000000000000000000001000000000000000000000000
        80000080000000808000800000008000800080800000C0C0C000C0DCC000F0CA
        A6000020400000206000002080000020A0000020C0000020E000004000000040
        20000040400000406000004080000040A0000040C0000040E000006000000060
        20000060400000606000006080000060A0000060C0000060E000008000000080
        20000080400000806000008080000080A0000080C0000080E00000A0000000A0
        200000A0400000A0600000A0800000A0A00000A0C00000A0E00000C0000000C0
        200000C0400000C0600000C0800000C0A00000C0C00000C0E00000E0000000E0
        200000E0400000E0600000E0800000E0A00000E0C00000E0E000400000004000
        20004000400040006000400080004000A0004000C0004000E000402000004020
        20004020400040206000402080004020A0004020C0004020E000404000004040
        20004040400040406000404080004040A0004040C0004040E000406000004060
        20004060400040606000406080004060A0004060C0004060E000408000004080
        20004080400040806000408080004080A0004080C0004080E00040A0000040A0
        200040A0400040A0600040A0800040A0A00040A0C00040A0E00040C0000040C0
        200040C0400040C0600040C0800040C0A00040C0C00040C0E00040E0000040E0
        200040E0400040E0600040E0800040E0A00040E0C00040E0E000800000008000
        20008000400080006000800080008000A0008000C0008000E000802000008020
        20008020400080206000802080008020A0008020C0008020E000804000008040
        20008040400080406000804080008040A0008040C0008040E000806000008060
        20008060400080606000806080008060A0008060C0008060E000808000008080
        20008080400080806000808080008080A0008080C0008080E00080A0000080A0
        200080A0400080A0600080A0800080A0A00080A0C00080A0E00080C0000080C0
        200080C0400080C0600080C0800080C0A00080C0C00080C0E00080E0000080E0
        200080E0400080E0600080E0800080E0A00080E0C00080E0E000C0000000C000
        2000C0004000C0006000C0008000C000A000C000C000C000E000C0200000C020
        2000C0204000C0206000C0208000C020A000C020C000C020E000C0400000C040
        2000C0404000C0406000C0408000C040A000C040C000C040E000C0600000C060
        2000C0604000C0606000C0608000C060A000C060C000C060E000C0800000C080
        2000C0804000C0806000C0808000C080A000C080C000C080E000C0A00000C0A0
        2000C0A04000C0A06000C0A08000C0A0A000C0A0C000C0A0E000C0C00000C0C0
        2000C0C04000C0C06000C0C08000C0C0A000F0FBFF00A4A0A000808080000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD020202
        020202FDFDFDFDFDFDFDFDFDFDFDFDFDFDA4A4A4A4A4A4FDFDFDFDFDFDFDFDFD
        FDFDFDFDFD020202020202FDFDFDFDFDFDFDFDFDFDFDFD020202020202020202
        02FDFDFDFDFDFDFDFDFDFDA4A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFDFDFDFD02
        020202020202020202FDFDFDFDFDFDFDFDFD020228282828282828020202FDFD
        FDFDFDFDFDFDA4A4F7F7F7F7F7F7F7F7A4A4FDFDFDFDFDFDFDFD020228282828
        282828020202FDFDFDFDFDFDFD0228282810102828282828280202FDFDFDFDFD
        FDA4F7F7F7A4A4F7F7F7F7F7F7A4A4FDFDFDFDFDFD0228282810102828282828
        280202FDFDFDFDFD022828282818FF182828282828280202FDFDFDFDA4F7F7F7
        F7A4FFA4F7F7F7F7F7F7A4A4FDFDFDFD022828282818FB182828282828280202
        FDFDFDFD022828282818FFFF1828282828280202FDFDFDFDA4F7F7F7F7A4FFFF
        A4F7F7F7F7F7A4A4FDFDFDFD022828282818FBFB1828282828280202FDFDFD02
        282828282818FFFFFF1828282828280202FDFD07F7F7F7F7F7A4FFFFFFA4F7F7
        F7F7F7A4A4FDFD02282828282818FBFBFB1828282828280202FDFD2828282828
        2818FFFFFFFF18282828280202FDFDF7F7F7F7F7F7A4FFFFFFFFA4F7F7F7F7A4
        A4FDFD28282828282818FBFBFBFB18282828280202FDFD28282828282818FFFF
        FFFFFF182828280202FDFD07F7F7F7F7F7A4FFFFFFFFFFA4F7F7F7A4A4FDFD28
        282828282818FBFBFBFBFB182828280202FDFD28282828282818FFFFFFFFFFFF
        1828280202FDFD07F7F7F7F7F7A4FFFFFFFFFFFFA4F7F7A4A4FDFD2828282828
        2818FBFBFBFBFBFB1828280202FDFD28282828282818FFFFFFFFFF1828282802
        02FDFD07F7F7F7F7F7A4FFFFFFFFFFA4F7F7F7A4A4FDFD28282828282818FBFB
        FBFBFB182828280202FDFD28282828282818FFFFFFFF18282828280202FDFD07
        F7F7F7F7F7A4FFFFFFFFA4F7F7F7F7A4A4FDFD28282828282818FBFBFBFB1828
        2828280202FDFDFD282828282818FFFFFF18282828282802FDFDFDFD07F7F7F7
        F7A4FFFFFFA4F7F7F7F7F7A4FDFDFDFD282828282818FBFBFB18282828282802
        FDFDFDFD283028282818FFFF1828282828280202FDFDFDFDF7F7F7F7F7A4FFFF
        A4F7F7F7F7F7A4A4FDFDFDFD283028282818FBFB1828282828280202FDFDFDFD
        FD2830282818FF1828282828280202FDFDFDFDFDFD07F7F7F7A4FFA4F7F7F7F7
        F7A4A4FDFDFDFDFDFD2830282818FB1828282828280202FDFDFDFDFDFDFD2830
        28181828282828280202FDFDFDFDFDFDFDFD07F7F7A4A4F7F7F7F7F7A4A4FDFD
        FDFDFDFDFDFD283028181828282828280202FDFDFDFDFDFDFDFDFD2830282828
        2828282828FDFDFDFDFDFDFDFDFDFD07F7F7F7F7F7F7F7F7A4FDFDFDFDFDFDFD
        FDFDFD28302828282828282828FDFDFDFDFDFDFDFDFDFDFDFD282828282828FD
        FDFDFDFDFDFDFDFDFDFDFDFDFDF707070707F7FDFDFDFDFDFDFDFDFDFDFDFDFD
        FD282828282828FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFD}
      NumGlyphs = 3
    end
    object mbStop: TBitBtn
      Left = 559
      Top = 255
      Width = 81
      Height = 25
      Hint = '$AVZ0601'
      Anchors = [akTop, akRight]
      Caption = '$AVZ1053'
      Enabled = False
      TabOrder = 3
      OnClick = mbStopClick
      Glyph.Data = {
        E6080000424DE60800000000000036040000280000003C000000140000000100
        080000000000B004000000000000000000000001000000000000000000000000
        80000080000000808000800000008000800080800000C0C0C000C0DCC000F0CA
        A6000020400000206000002080000020A0000020C0000020E000004000000040
        20000040400000406000004080000040A0000040C0000040E000006000000060
        20000060400000606000006080000060A0000060C0000060E000008000000080
        20000080400000806000008080000080A0000080C0000080E00000A0000000A0
        200000A0400000A0600000A0800000A0A00000A0C00000A0E00000C0000000C0
        200000C0400000C0600000C0800000C0A00000C0C00000C0E00000E0000000E0
        200000E0400000E0600000E0800000E0A00000E0C00000E0E000400000004000
        20004000400040006000400080004000A0004000C0004000E000402000004020
        20004020400040206000402080004020A0004020C0004020E000404000004040
        20004040400040406000404080004040A0004040C0004040E000406000004060
        20004060400040606000406080004060A0004060C0004060E000408000004080
        20004080400040806000408080004080A0004080C0004080E00040A0000040A0
        200040A0400040A0600040A0800040A0A00040A0C00040A0E00040C0000040C0
        200040C0400040C0600040C0800040C0A00040C0C00040C0E00040E0000040E0
        200040E0400040E0600040E0800040E0A00040E0C00040E0E000800000008000
        20008000400080006000800080008000A0008000C0008000E000802000008020
        20008020400080206000802080008020A0008020C0008020E000804000008040
        20008040400080406000804080008040A0008040C0008040E000806000008060
        20008060400080606000806080008060A0008060C0008060E000808000008080
        20008080400080806000808080008080A0008080C0008080E00080A0000080A0
        200080A0400080A0600080A0800080A0A00080A0C00080A0E00080C0000080C0
        200080C0400080C0600080C0800080C0A00080C0C00080C0E00080E0000080E0
        200080E0400080E0600080E0800080E0A00080E0C00080E0E000C0000000C000
        2000C0004000C0006000C0008000C000A000C000C000C000E000C0200000C020
        2000C0204000C0206000C0208000C020A000C020C000C020E000C0400000C040
        2000C0404000C0406000C0408000C040A000C040C000C040E000C0600000C060
        2000C0604000C0606000C0608000C060A000C060C000C060E000C0800000C080
        2000C0804000C0806000C0808000C080A000C080C000C080E000C0A00000C0A0
        2000C0A04000C0A06000C0A08000C0A0A000C0A0C000C0A0E000C0C00000C0C0
        2000C0C04000C0C06000C0C08000C0C0A000F0FBFF00A4A0A000808080000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD040404
        040404FDFDFDFDFDFDFDFDFDFDFDFDFDFDA4A4A4A4A4A4FDFDFDFDFDFDFDFDFD
        FDFDFDFDFD040404040404FDFDFDFDFDFDFDFDFDFDFDFD040404040404040404
        04FDFDFDFDFDFDFDFDFDFDA4A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFDFDFDFD04
        040404040404040404FDFDFDFDFDFDFDFDFD0404C0C0C0C0C0C0C0040404FDFD
        FDFDFDFDFDFDA4A4A4A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFDFD0404C0C0C0C0
        C0C0C0040404FDFDFDFDFDFDFDFCC0C0C0C0C0C0C0C0C0C0C00404FDFDFDFDFD
        FDA4A4A4A4A4A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFCC0C0C0C0C0C0C0C0C0C0
        C00404FDFDFDFDFD04C0C004040404040404040404C00404FDFDFDFDA4A4A4A4
        A4A4A4A4A4A4A4A4A4A4A4A4FDFDFDFD04C0C004040404040404040404C00404
        FDFDFDFD04C0C004FFFFFFFFFFFFFFFF04C00404FDFDFDFDA4A4A4A4FFFFFFFF
        FFFFFFFFA4A4A4A4FDFDFDFD04C0C004FBFBFBFBFBFBFBFB04C00404FDFDFDFC
        C0C0C004FFFFFFFFFFFFFFFF04C0C00404FDFDF7A4A4A4A4FFFFFFFFFFFFFFFF
        A4A4A4A4A4FDFDFCC0C0C004FBFBFBFBFBFBFBFB04C0C00404FDFDFCC0C0C004
        FFFFFFFFFFFFFFFF04C0C00404FDFDF7A4A4A4A4FFFFFFFFFFFFFFFFA4A4A4A4
        A4FDFDFCC0C0C004FBFBFBFBFBFBFBFB04C0C00404FDFDFCC0C0C004FFFFFFFF
        FFFFFFFF04C0C00404FDFDF7A4A4A4A4FFFFFFFFFFFFFFFFA4A4A4A4A4FDFDFC
        C0C0C004FBFBFBFBFBFBFBFB04C0C00404FDFDFCC0C0C004FFFFFFFFFFFFFFFF
        04C0C00404FDFDF7A4A4A4A4FFFFFFFFFFFFFFFFA4A4A4A4A4FDFDFCC0C0C004
        FBFBFBFBFBFBFBFB04C0C00404FDFDFCC0C0C004FFFFFFFFFFFFFFFF04C0C004
        04FDFDF7A4A4A4A4FFFFFFFFFFFFFFFFA4A4A4A4A4FDFDFCC0C0C004FBFBFBFB
        FBFBFBFB04C0C00404FDFDFCFCC0C004FFFFFFFFFFFFFFFF04C0C00404FDFDF7
        A4A4A4A4FFFFFFFFFFFFFFFFA4A4A4A4A4FDFDFCFCC0C004FBFBFBFBFBFBFBFB
        04C0C00404FDFDFDFCC0C004FFFFFFFFFFFFFFFF04C0C004FDFDFDFDF7A4A4A4
        FFFFFFFFFFFFFFFFA4A4A4A4FDFDFDFDFCC0C004FBFBFBFBFBFBFBFB04C0C004
        FDFDFDFDFCFCC004040404040404040404C00404FDFDFDFDF7A4A4A4A4A4A4A4
        A4A4A4A4A4A4A4A4FDFDFDFDFCFCC004040404040404040404C00404FDFDFDFD
        FDFCFCC0C0C0C0C0C0C0C0C0C00404FDFDFDFDFDFDF7F7A4A4A4A4A4A4A4A4A4
        A4A4A4FDFDFDFDFDFDFCFCC0C0C0C0C0C0C0C0C0C00404FDFDFDFDFDFDFDFCFC
        C0FCFCC0C0C0C0C00404FDFDFDFDFDFDFDFDF7F7A4A4A4A4A4A4A4A4A4A4FDFD
        FDFDFDFDFDFDFCFCC0FCFCC0C0C0C0C00404FDFDFDFDFDFDFDFDFDFCFCC0C0C0
        C0C0C0FCFCFDFDFDFDFDFDFDFDFDFDF7A4A4A4A4A4A4A4A4A4FDFDFDFDFDFDFD
        FDFDFDFCFCC0C0C0C0C0C0FCFCFDFDFDFDFDFDFDFDFDFDFDFDFCFCFCFCFCFCFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDF7F7F7F7F7F7FDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFCFCFCFCFCFCFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFDFD
        FDFDFDFDFDFDFDFDFDFD}
      NumGlyphs = 3
    end
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 144
    Top = 144
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'txt'
    FileName = 'avz_log'
    Filter = '$AVZ1058'
    Title = '$AVZ1027'
    Left = 80
    Top = 143
  end
  object MainMenu1: TMainMenu
    Left = 112
    Top = 144
    object mmFile: TMenuItem
      Caption = '$AVZ1109'
      object mmStart: TMenuItem
        Caption = '$AVZ0284'
        OnClick = mmStartClick
      end
      object mmPause: TMenuItem
        Caption = '$AVZ0700'
        OnClick = mmPauseClick
      end
      object mmStop: TMenuItem
        Caption = '$AVZ0598'
        OnClick = mmStopClick
      end
      object N32: TMenuItem
        Caption = '-'
      end
      object mmSysCheck: TMenuItem
        Caption = '$AVZ0322'
        OnClick = mmSysCheckClick
      end
      object mmRepairSystem: TMenuItem
        Caption = '$AVZ0145'
        OnClick = mmRepairSystemClick
      end
      object mmBackup: TMenuItem
        Caption = '$AVZ1186'
        OnClick = mmBackupClick
      end
      object mmRevizor: TMenuItem
        Caption = '$AVZ0906'
        OnClick = mmRevizorClick
      end
      object mmWizards: TMenuItem
        Caption = '$AVZ1245'
        OnClick = mmWizardsClick
      end
      object N46: TMenuItem
        Caption = '-'
      end
      object mmStdScripts: TMenuItem
        Caption = '$AVZ1048'
        OnClick = mmStdScriptsClick
      end
      object mmRunScript: TMenuItem
        Caption = '$AVZ0189'
        OnClick = mmRunScriptClick
      end
      object N39: TMenuItem
        Caption = '-'
      end
      object mmAutoUpdate: TMenuItem
        Caption = '$AVZ0550'
        OnClick = mmAutoUpdateClick
      end
      object N9: TMenuItem
        Caption = '-'
      end
      object mmExtLogView: TMenuItem
        Caption = '$AVZ0862'
        OnClick = mmExtLogViewClick
      end
      object N10: TMenuItem
        Caption = '$AVZ1032'
        OnClick = N10Click
      end
      object N41: TMenuItem
        Caption = '$AVZ0626'
        OnClick = N41Click
      end
      object N12: TMenuItem
        Caption = '-'
      end
      object mmShowInfected: TMenuItem
        Caption = '$AVZ0859'
        OnClick = mmShowInfectedClick
      end
      object N11: TMenuItem
        Caption = '$AVZ0858'
        OnClick = N11Click
      end
      object N2: TMenuItem
        Caption = '$AVZ0225'
        OnClick = N2Click
      end
      object N38: TMenuItem
        Caption = '$AVZ0039'
        OnClick = N38Click
      end
      object N30: TMenuItem
        Caption = '-'
      end
      object N31: TMenuItem
        Caption = '$AVZ0611'
        OnClick = N31Click
      end
      object N6: TMenuItem
        Caption = '-'
      end
      object mmSaveSetupProfile: TMenuItem
        Caption = '$AVZ1033'
        OnClick = mmSaveSetupProfileClick
      end
      object N52: TMenuItem
        Caption = '$AVZ0256'
        OnClick = N52Click
      end
      object N7: TMenuItem
        Caption = '-'
      end
      object mmExit: TMenuItem
        Caption = '$AVZ0195'
        OnClick = mmExitClick
      end
    end
    object mmService: TMenuItem
      Caption = '$AVZ0945'
      object N22: TMenuItem
        Caption = '$AVZ0211'
        OnClick = N22Click
      end
      object mmServiceView: TMenuItem
        Caption = '$AVZ0212'
        OnClick = mmServiceViewClick
      end
      object mmKernelModules: TMenuItem
        Caption = '$AVZ0430'
        OnClick = mmKernelModulesClick
      end
      object DLL1: TMenuItem
        Caption = '$AVZ0394'
        OnClick = DLL1Click
      end
      object N16: TMenuItem
        Caption = '-'
      end
      object N14: TMenuItem
        Caption = '$AVZ0757'
        OnClick = N14Click
      end
      object N15: TMenuItem
        Caption = '$AVZ0776'
        OnClick = N15Click
      end
      object N42: TMenuItem
        Caption = '$AVZ0748'
        OnClick = N42Click
      end
      object N13: TMenuItem
        Caption = '-'
      end
      object N27: TMenuItem
        Caption = '$AVZ0391'
        OnClick = N27Click
      end
      object IEBHO1: TMenuItem
        Caption = '$AVZ0398'
        OnClick = IEBHO1Click
      end
      object CPL1: TMenuItem
        Caption = '$AVZ0393'
        OnClick = CPL1Click
      end
      object N34: TMenuItem
        Caption = '$AVZ0399'
        OnClick = N34Click
      end
      object N35: TMenuItem
        Caption = '$AVZ0400'
        OnClick = N35Click
      end
      object N36: TMenuItem
        Caption = '$AVZ0395'
        OnClick = N36Click
      end
      object N44: TMenuItem
        Caption = '$AVZ0396'
        OnClick = N44Click
      end
      object DownloadedProgramFiles1: TMenuItem
        Caption = '$AVZ0389'
        OnClick = DownloadedProgramFiles1Click
      end
      object ActiveSetup1: TMenuItem
        Caption = '$AVZ0388'
        OnClick = ActiveSetup1Click
      end
      object N26: TMenuItem
        Caption = '-'
      end
      object mmLSPView: TMenuItem
        Caption = '$AVZ0390'
        OnClick = mmLSPViewClick
      end
      object cbHosts: TMenuItem
        Caption = '$AVZ0401'
        OnClick = cbHostsClick
      end
      object CPUDP1: TMenuItem
        Caption = '$AVZ0606'
        OnClick = CPUDP1Click
      end
      object N17: TMenuItem
        Caption = '$AVZ0563'
        OnClick = N17Click
      end
      object N18: TMenuItem
        Caption = '-'
      end
      object N19: TMenuItem
        Caption = '$AVZ0959'
        object N21: TMenuItem
          Caption = '$AVZ0024'
          OnClick = N21Click
        end
        object MsConfig1: TMenuItem
          Caption = '$AVZ0020'
          OnClick = MsConfig1Click
        end
      end
      object N28: TMenuItem
        Caption = '-'
      end
      object N29: TMenuItem
        Caption = '$AVZ0810'
        OnClick = N29Click
      end
      object mmCheckWintrust: TMenuItem
        Caption = '$AVZ0808'
        OnClick = mmCheckWintrustClick
      end
      object mmCalkMD5: TMenuItem
        Caption = '$AVZ1608'
        OnClick = mmCalkMD5Click
      end
    end
    object mmSG: TMenuItem
      Caption = 'AVZGuard'
      object mmSG_TurnOn: TMenuItem
        Caption = '$AVZ0110'
        OnClick = mmSG_TurnOnClick
      end
      object mmSG_TrustedRun: TMenuItem
        Caption = '$AVZ0283'
        OnClick = mmSG_TrustedRunClick
      end
      object mmSG_TurnOff: TMenuItem
        Caption = '$AVZ0605'
        OnClick = mmSG_TurnOffClick
      end
    end
    object mmPMControl: TMenuItem
      Caption = 'AVZPM'
      object mmPMDriverInstall: TMenuItem
        Caption = '$AVZ1106'
        OnClick = mmPMDriverInstallClick
      end
      object mmPMDriverRemove: TMenuItem
        Caption = '$AVZ1085'
        OnClick = mmPMDriverRemoveClick
      end
      object mmPMDriverUnloadRemove: TMenuItem
        Caption = '$AVZ1086'
        OnClick = mmPMDriverUnloadRemoveClick
      end
    end
    object mmHelp: TMenuItem
      Caption = '$AVZ1041'
      object N23: TMenuItem
        Caption = '$AVZ1041'
        OnClick = N23Click
      end
      object N24: TMenuItem
        Caption = '$AVZ1010'
        OnClick = N24Click
      end
      object N25: TMenuItem
        Caption = '-'
      end
      object N4: TMenuItem
        Caption = '$AVZ0529'
        OnClick = N4Click
      end
    end
  end
  object pmDirTreeMenu: TPopupMenu
    OnPopup = pmDirTreeMenuPopup
    Left = 208
    Top = 144
    object mmSelectHDD: TMenuItem
      Tag = 1
      Caption = '$AVZ0175'
      OnClick = mmSelectHDDClick
    end
    object mmSelectCDROM: TMenuItem
      Tag = 2
      Caption = '$AVZ0174'
      OnClick = mmSelectHDDClick
    end
    object mmSelectFDD: TMenuItem
      Tag = 3
      Caption = '$AVZ0176'
      OnClick = mmSelectHDDClick
    end
    object N48: TMenuItem
      Caption = '-'
    end
    object N49: TMenuItem
      Caption = '$AVZ0217'
      OnClick = N49Click
    end
    object N50: TMenuItem
      Caption = '$AVZ0843'
      OnClick = N50Click
    end
    object N20: TMenuItem
      Caption = '-'
    end
    object mmRefresh: TMenuItem
      Caption = '$AVZ0544'
      ShortCut = 116
      OnClick = mmRefreshClick
    end
  end
  object tRunScan: TTimer
    Enabled = False
    OnTimer = tRunScanTimer
    Left = 144
    Top = 112
  end
  object OpenDialog1: TOpenDialog
    Filter = '$AVZ0159'
    Options = [ofEnableSizing, ofForceShowHidden]
    Left = 248
    Top = 112
  end
  object RxTrayIcon: TRxTrayIcon
    Active = False
    Hint = '$AVZ0063'
    Icon.Data = {
      0000010009001010100001000400280100009600000010100000010008006805
      0000BE010000101000000100200068040000260700002020100001000400E802
      00008E0B00002020000001000800A8080000760E00002020000001002000A810
      00001E170000303010000100040068060000C62700003030000001000800A80E
      00002E2E00003030000001002000A8250000D63C000028000000100000002000
      00000100040000000000C0000000000000000000000010000000000000000000
      000000008000008000000080800080000000800080008080000080808000C0C0
      C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF008080
      0000000000000800000000000000800780078000000000700000780000000080
      0000070000000070080000800000000000800070000000700008000000000070
      000080000000007BBBBB08000000007033030080000000700BB0000800000070
      300B000080000070BBBBB0000800007077777000008000078808770000001FFF
      00001FFF0000007F0000C03F0000C03F0000C01F0000C01F0000C01F0000C01F
      0000C01F0000C00F0000C0070000C0030000C0110000C0190000C21F00002800
      0000100000002000000001000800000000004001000000000000000000000001
      00000000000000000000FFFFFF002222220092929200ABABAB00C5C5C5008585
      8500525252005555550044444400AAAAAA0033FEFF0011111100DDDDDD0000BA
      BA006CFFFF00005D5D005F5F5F00000080000080000000808000800000008000
      800080800000C0C0C000808080000000FF0000FF000000FFFF00FF000000FF00
      FF00FFFF000009090900121212001F1F1F002C2C2C0039393900454545006C6C
      6C00787878009F9F9F00B8B8B800D2D2D200DEDEDE00EBEBEB00F8F8F800F0FB
      FF00A4A0A000C0DCC000F0CAA60000003E0000005D0000007C0000009B000000
      BA000000D9000000F0002424FF004848FF006C6CFF009090FF00B4B4FF000014
      3E00001E5D0000287C0000329B00003CBA000046D9000055F000246DFF004885
      FF006C9DFF0090B5FF00B4CDFF00002A3E00003F5D0000547C0000699B00007E
      BA000093D90000AAF00024B6FF0048C2FF006CCEFF0090DAFF00B4E6FF00003E
      3E00007C7C00009B9B0000D9D90000F0F00024FFFF0048FFFF0090FFFF00B4FF
      FF00003E2A00005D3F00007C5400009B690000BA7E0000D9930000F0AA0024FF
      B60048FFC2006CFFCE0090FFDA00B4FFE600003E1400005D1E00007C2800009B
      320000BA3C0000D9460000F0550024FF6D0048FF85006CFF9D0090FFB500B4FF
      CD00003E0000005D0000007C0000009B000000BA000000D9000000F0000024FF
      240048FF48006CFF6C0090FF9000B4FFB400143E00001E5D0000287C0000329B
      00003CBA000046D9000055F000006DFF240085FF48009DFF6C00B5FF9000CDFF
      B4002A3E00003F5D0000547C0000699B00007EBA000093D90000AAF00000B6FF
      2400C2FF4800CEFF6C00DAFF9000E6FFB4003E3E00005D5D00007C7C00009B9B
      0000BABA0000D9D90000F0F00000FFFF2400FFFF4800FFFF6C00FFFF9000FFFF
      B4003E2A00005D3F00007C5400009B690000BA7E0000D9930000F0AA0000FFB6
      2400FFC24800FFCE6C00FFDA9000FFE6B4003E1400005D1E00007C2800009B32
      0000BA3C0000D9460000F0550000FF6D2400FF854800FF9D6C00FFB59000FFCD
      B4003E0000005D0000007C0000009B000000BA000000D9000000F0000000FF24
      2400FF484800FF6C6C00FF909000FFB4B4003E0014005D001E007C0028009B00
      3200BA003C00D9004600F0005500FF246D00FF488500FF6C9D00FF90B500FFB4
      CD003E002A005D003F007C0054009B006900BA007E00D9009300F000AA00FF24
      B600FF48C200FF6CCE00FF90DA00FFB4E6003E003E005D005D007C007C009B00
      9B00BA00BA00D900D900F000F000FF24FF00FF48FF00FF6CFF00FF90FF00FFB4
      FF002A003E003F005D0054007C0069009B007E00BA009300D900AA00F000B624
      FF00C248FF00CE6CFF00DA90FF00E6B4FF0014003E001E005D0028007C003200
      9B003C00BA000500050000000000000000000000000000040000000000000000
      0000000000000500021105000011050000000000000000001102000000001105
      0000000000000000050002000000001100000000000000001100000D00000021
      0500000000000000000021000D002100110000000000000008022121000D0021
      00000000000000000821212121000D000000000000000000080F0F0F0F0B000D
      000000000000000008210E10210E21000D000000000000000821210B0B212100
      000D00000000000008210E21210F210000000D000000000008020B0B0B0B0B21
      0000000D00000000080209090909090000000000040000000203040500040608
      0000000000001FFF00001FFF0000007F0000C03F0000C03F0000C01F0000C01F
      0000C01F0000C01F0000C01F0000C00F0000C0070000C0030000C0110000C019
      0000C21F00002800000010000000200000000100200000000000400400000000
      0000000000000000000000000000C5C5C5FF000000FFC5C5C5FF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000FFAAAAAAFF000000FF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000C5C5C5FF000000FF222222FF5F5F5FFFC5C5
      C5FF000000FF000000FF5F5F5FFFC5C5C5FF0000000000000000000000000000
      000000000000000000000000000000000000000000005F5F5FFF222222FF0000
      00FF000000FF000000FF000000FF5F5F5FFFC5C5C5FF00000000000000000000
      00000000000000000000000000000000000000000000C5C5C5FF000000FF2222
      22FF000000FF000000FF000000FF000000FF5F5F5FFF00000000000000000000
      000000000000000000000000000000000000000000005F5F5FFF000000FF0000
      00FFDDDDDDFF000000FF000000FF000000FF111111FFC5C5C5FF000000000000
      00000000000000000000000000000000000000000000000000FF000000FF1111
      11FF000000FFDDDDDDFF000000FF111111FF000000FF5F5F5FFF000000000000
      00000000000000000000000000000000000000000000555555FF222222FF1111
      11FF111111FF000000FFDDDDDDFF000000FF111111FF000000FF000000000000
      00000000000000000000000000000000000000000000555555FF111111FF1111
      11FF111111FF111111FF000000FFDDDDDDFF000000FF000000FF000000000000
      00000000000000000000000000000000000000000000555555FF6CFFFFFF6CFF
      FFFF6CFFFFFF6CFFFFFF33FEFFFF000000FFDDDDDDFF000000FF000000000000
      00000000000000000000000000000000000000000000555555FF111111FF00BA
      BAFF005D5DFF111111FF00BABAFF111111FF000000FFDDDDDDFF000000FF0000
      00000000000000000000000000000000000000000000555555FF111111FF1111
      11FF33FEFFFF33FEFFFF111111FF111111FF000000FF000000FFDDDDDDFF0000
      00FF0000000000000000000000000000000000000000555555FF111111FF00BA
      BAFF111111FF111111FF6CFFFFFF111111FF000000FF000000FF000000FFDDDD
      DDFF000000FF00000000000000000000000000000000555555FF222222FF33FE
      FFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF111111FF000000FF000000000000
      00FFDDDDDDFF000000FF000000000000000000000000555555FF222222FF4444
      44FF444444FF444444FF444444FF444444FF000000FF000000FF000000000000
      0000000000FFAAAAAAFF000000000000000000000000222222FF929292FFABAB
      ABFFC5C5C5FF00000000ABABABFF858585FF525252FF000000FF000000000000
      00000000000000000000000000001FFF000008FF0000007F0000C03F0000C01F
      0000C01F0000C01F0000C01F0000C01F0000C00F0000C0070000C0030000C001
      0000C0100000C0180000C21F0000280000002000000040000000010004000000
      0000800200000000000000000000100000000000000000000000000080000080
      00000080800080000000800080008080000080808000C0C0C0000000FF0000FF
      000000FFFF00FF000000FF00FF00FFFF0000FFFFFF0008707800000000000000
      0000000000000787770000000000000000000000000000887000000000000000
      0000000000000788800000070070000000000000000008700000007000070000
      0000000000000000070007000780700000000000000000000070000070000700
      0000000000000000000700070000007000000000000000000070707000000007
      000000000000000008000787000000008000000000000000070070F870000000
      70000000000000008007000F87000000080000000000000070700000F8700000
      0700000000000000008000000F87000000000000000000000700000000F87000
      000000000000000007700000000F87000000000000000000077000000000F870
      0000000000000000077BBBBBBBB00F870000000000000000077BBB000BBB00F8
      700000000000000007700BB0000BB00F8700000000000000077003BB00000000
      F8700000000000000770000BBB0000000F8700000000000007700003BB300000
      00F87000000000000770000003B30000000F87000000000007700B30000BB000
      0000F8700000000007700BB300003B3000000F8700000000077003BBBBBBBBB0
      000000F87000000007700000000000000000000F870000000770077777777770
      00000000F8700000077770000000007700000000077000000700078800887000
      0000000000000000007000000000000700000000000083FFFFFF83FFFFFF83FF
      FFFF81E1FFFF80C0FFFFF0007FFFF8003FFFFC001FFFFC000FFFF80007FFF800
      07FFF00003FFF00003FFF00003FFF00003FFF00003FFF00003FFF00003FFF000
      03FFF00001FFF00000FFF000007FF000003FF000001FF000020FF0000307F000
      0383F00003C1F00003E1F00003F1F00C03FFF1FFE3FF28000000200000004000
      0000010008000000000080040000000000000000000000010000000000000000
      0000FFFFFF0077777700ABABAB00C5C5C5001111110055555500444444005F5F
      5F007878780022222200AAAAAA0045454500DDDDDD0088888800005D5D006CFF
      FF0033FEFF00009B9B00007C7C0048FFFF0000BABA0000008000008000000080
      8000800000008000800080800000C0C0C000808080000000FF0000FF000000FF
      FF00FF000000FF00FF00FFFF000009090900121212001F1F1F002C2C2C003939
      3900525252006C6C6C0085858500929292009F9F9F00B8B8B800D2D2D200DEDE
      DE00EBEBEB00F8F8F800F0FBFF00A4A0A000C0DCC000F0CAA60000003E000000
      5D0000007C0000009B000000BA000000D9000000F0002424FF004848FF006C6C
      FF009090FF00B4B4FF0000143E00001E5D0000287C0000329B00003CBA000046
      D9000055F000246DFF004885FF006C9DFF0090B5FF00B4CDFF00002A3E00003F
      5D0000547C0000699B00007EBA000093D90000AAF00024B6FF0048C2FF006CCE
      FF0090DAFF00B4E6FF00003E3E0000D9D90000F0F00024FFFF0090FFFF00B4FF
      FF00003E2A00005D3F00007C5400009B690000BA7E0000D9930000F0AA0024FF
      B60048FFC2006CFFCE0090FFDA00B4FFE600003E1400005D1E00007C2800009B
      320000BA3C0000D9460000F0550024FF6D0048FF85006CFF9D0090FFB500B4FF
      CD00003E0000005D0000007C0000009B000000BA000000D9000000F0000024FF
      240048FF48006CFF6C0090FF9000B4FFB400143E00001E5D0000287C0000329B
      00003CBA000046D9000055F000006DFF240085FF48009DFF6C00B5FF9000CDFF
      B4002A3E00003F5D0000547C0000699B00007EBA000093D90000AAF00000B6FF
      2400C2FF4800CEFF6C00DAFF9000E6FFB4003E3E00005D5D00007C7C00009B9B
      0000BABA0000D9D90000F0F00000FFFF2400FFFF4800FFFF6C00FFFF9000FFFF
      B4003E2A00005D3F00007C5400009B690000BA7E0000D9930000F0AA0000FFB6
      2400FFC24800FFCE6C00FFDA9000FFE6B4003E1400005D1E00007C2800009B32
      0000BA3C0000D9460000F0550000FF6D2400FF854800FF9D6C00FFB59000FFCD
      B4003E0000005D0000007C0000009B000000BA000000D9000000F0000000FF24
      2400FF484800FF6C6C00FF909000FFB4B4003E0014005D001E007C0028009B00
      3200BA003C00D9004600F0005500FF246D00FF488500FF6C9D00FF90B500FFB4
      CD003E002A005D003F007C0054009B006900BA007E00D9009300F000AA00FF24
      B600FF48C200FF6CCE00FF90DA00FFB4E6003E003E005D005D007C007C009B00
      9B00BA00BA00D900D900F000F000FF24FF00FF48FF00FF6CFF00FF90FF00FFB4
      FF002A003E003F005D0054007C0069009B007E00BA009300D900AA00F000B624
      FF00C248FF00CE6CFF00DA90FF00E6B4FF0014003E001E005D0028007C000004
      0800080400000000000000000000000000000000000000000000000000000008
      0306060800000000000000000000000000000000000000000000000000000000
      0D03060000000000000000000000000000000000000000000000000000000008
      0D0D032500000000000800000800000000000000000000000000000000000004
      0800000A25000000080025000008000000000000000000000000000000000000
      000000070A2500002525000E0D00000000000000000000000000000000000000
      00000000070A250025000E000000250800000000000000000000000000000000
      0000000000070A25000E00000000002500000000000000000000000000000000
      000000000800070A0E0025000000000025080000000000000000000000000000
      000000030007000E0D0E00000000000025000300000000000000000000000000
      0000000807000E00010D0E000000000000250800000000000000000000000000
      00000300000E002500010D0E0000250000250003000000000000000000000000
      000008000E0025252500010D0E00252525002508000000000000000000000000
      000000000D002525252500010D0E002525002500000000000000000000000000
      00000006000A252525252500010D0E0025252500000000000000000000000000
      00000006070A25252525252500010D0E00002500000000000000000000000000
      0000000607252525252525250000010D0E002500000000000000000000000000
      000000060711111111111111110000010D0E0000000000000000000000000000
      00000006071011202525251011112500010D0E00000000000000000000000000
      0000000607252511112525250010112500010D0E000000000000000000000000
      000000060725250F10152525252500252500010D0E0000000000000000000000
      00000006072525252510111425252525250000010D0E00000000000000000000
      0000000607252525250F11110F25252525002500010D0E000000000000000000
      000000060725252525252512111225252500250000010D0E0000000000000000
      00000006072525111325252525101125250025000000010D0E00000000000000
      000000060725251011122525252513110F002500000000010D0E000000000000
      00000006070A250F10111111111111111125250000000000010D0E0000000000
      00000006070A25252525250000000000000025000000000000010D0E00000000
      00000006070A2507070707070707070707002500000000000000010307000000
      0000000606070700000000000000002507072500000000000000000809000000
      0000000200000002030400000403020000002500000000000000000000000000
      00000000020000000000000000000000000200000000000000000000000083FF
      FFFF83FFFFFF83FFFFFF81E1FFFF81C0FFFFF0007FFFF8003FFFFC001FFFFC00
      0FFFF80007FFF80007FFF00003FFF00003FFF00003FFF00003FFF00003FFF000
      03FFF00003FFF00003FFF00001FFF00000FFF000007FF000003FF000001FF000
      020FF0000307F0000383F00003C1F00003E1F00003F1F00C03FFF1FFE3FF2800
      0000200000004000000001002000000000008010000000000000000000000000
      00000000000000000000C5C5C5FF5F5F5FFF000000FF5F5F5FFFC5C5C5FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000005F5F5FFFAAAAAAFF555555FF555555FF5F5F5FFF0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000FFDDDDDDFFAAAAAAFF555555FF000000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000005F5F5FFFDDDDDDFFDDDDDDFFAAAAAAFF111111FF0000
      00FF000000000000000000000000000000005F5F5FFF000000FF000000FF5F5F
      5FFF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000C5C5C5FF5F5F5FFF000000FF000000FF222222FF1111
      11FF0000000000000000000000005F5F5FFF000000FF111111FF000000FF0000
      00FF5F5F5FFF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF444444FF2222
      22FF111111FF000000FF000000FF111111FF111111FF000000FF888888FFDDDD
      DDFF000000FF000000FF00000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000FF4444
      44FF222222FF111111FF000000FF111111FF000000FF888888FF000000FF0000
      00FF000000FF111111FF5F5F5FFF000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF444444FF222222FF111111FF000000FF888888FF000000FF000000FF0000
      00FF000000FF000000FF111111FF000000FF0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000005F5F
      5FFF000000FF444444FF222222FF888888FF000000FF111111FF000000FF0000
      00FF000000FF000000FF000000FF111111FF5F5F5FFF00000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000ABABABFF0000
      00FF444444FF000000FF888888FFDDDDDDFF888888FF000000FF000000FF0000
      00FF000000FF000000FF000000FF111111FF000000FFABABABFF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000005F5F5FFF4444
      44FF000000FF888888FF000000FFFFFFFFFFDDDDDDFF888888FF000000FF0000
      00FF000000FF000000FF000000FF000000FF111111FF5F5F5FFF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000ABABABFF000000FF0000
      00FF888888FF000000FF111111FF000000FFFFFFFFFFDDDDDDFF888888FF0000
      00FF000000FF111111FF000000FF000000FF111111FF000000FFABABABFF0000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000005F5F5FFF000000FF8888
      88FF000000FF111111FF111111FF111111FF000000FFFFFFFFFFDDDDDDFF8888
      88FF000000FF111111FF111111FF111111FF000000FF111111FF5F5F5FFF0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF000000FFDDDD
      DDFF000000FF111111FF111111FF111111FF111111FF000000FFFFFFFFFFDDDD
      DDFF888888FF000000FF111111FF111111FF000000FF111111FF000000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF0000
      00FF222222FF111111FF111111FF111111FF111111FF111111FF000000FFFFFF
      FFFFDDDDDDFF888888FF000000FF111111FF111111FF111111FF000000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF222222FF111111FF111111FF111111FF111111FF111111FF111111FF0000
      00FFFFFFFFFFDDDDDDFF888888FF000000FF000000FF111111FF000000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF111111FF111111FF111111FF111111FF111111FF111111FF111111FF0000
      00FF000000FFFFFFFFFFDDDDDDFF888888FF000000FF111111FF000000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FE
      FFFF000000FF000000FFFFFFFFFFDDDDDDFF888888FF000000FF000000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF6CFFFFFF33FEFFFF00FFFFFF111111FF111111FF111111FF6CFFFFFF33FE
      FFFF33FEFFFF111111FF000000FFFFFFFFFFDDDDDDFF888888FF000000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF111111FF111111FF33FEFFFF33FEFFFF111111FF111111FF111111FF0000
      00FF6CFFFFFF33FEFFFF111111FF000000FFFFFFFFFFDDDDDDFF888888FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF111111FF111111FF005D5DFF6CFFFFFF00BABAFF111111FF111111FF1111
      11FF111111FF000000FF111111FF111111FF000000FFFFFFFFFFDDDDDDFF8888
      88FF000000FF0000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF111111FF111111FF111111FF111111FF6CFFFFFF33FEFFFF48FFFFFF1111
      11FF111111FF111111FF111111FF111111FF000000FF000000FFFFFFFFFFDDDD
      DDFF888888FF000000FF00000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF111111FF111111FF111111FF111111FF005D5DFF33FEFFFF33FEFFFF005D
      5DFF111111FF111111FF111111FF111111FF000000FF111111FF000000FFFFFF
      FFFFDDDDDDFF888888FF000000FF000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF111111FF111111FF111111FF111111FF111111FF111111FF009B9BFF33FE
      FFFF009B9BFF111111FF111111FF111111FF000000FF111111FF000000FF0000
      00FFFFFFFFFFDDDDDDFF888888FF000000FF0000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF111111FF111111FF33FEFFFF007C7CFF111111FF111111FF111111FF1111
      11FF6CFFFFFF33FEFFFF111111FF111111FF000000FF111111FF000000FF0000
      0000000000FFFFFFFFFFDDDDDDFF888888FF000000FF00000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF111111FF111111FF6CFFFFFF33FEFFFF009B9BFF111111FF111111FF1111
      11FF111111FF007C7CFF33FEFFFF005D5DFF000000FF111111FF000000FF0000
      000000000000000000FFFFFFFFFFDDDDDDFF888888FF000000FF000000000000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF222222FF111111FF005D5DFF6CFFFFFF33FEFFFF33FEFFFF33FEFFFF33FE
      FFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF111111FF111111FF000000FF0000
      00000000000000000000000000FFFFFFFFFFDDDDDDFF888888FF000000FF0000
      00000000000000000000000000000000000000000000000000FF555555FF4444
      44FF222222FF111111FF111111FF111111FF111111FF111111FF000000FF0000
      00FF000000FF000000FF000000FF000000FF000000FF111111FF000000FF0000
      0000000000000000000000000000000000FFFFFFFFFFDDDDDDFF888888FF0000
      00FF0000000000000000000000000000000000000000000000FF555555FF4444
      44FF222222FF111111FF444444FF444444FF444444FF444444FF444444FF4444
      44FF444444FF444444FF444444FF444444FF000000FF111111FF000000FF0000
      000000000000000000000000000000000000000000FFFFFFFFFFAAAAAAFF4545
      45FF0000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF444444FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF000000FF111111FF444444FF444444FF111111FF000000FF0000
      00000000000000000000000000000000000000000000000000FF5F5F5FFF7878
      78FF0000000000000000000000000000000000000000000000FF777777FF0000
      00FF000000FF000000FF777777FFABABABFFC5C5C5FF0000000000000000C5C5
      C5FFABABABFF777777FF000000FF000000FF000000FF111111FF000000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF000000FF7777
      77FF000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000777777FF000000FF000000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000083FFFFFF83FFFFFF83FFFFFF81E1FFFF80807FFFF0003FFFF800
      1FFFFC000FFFF80007FFF80007FFF80003FFF00003FFF00003FFF00003FFF000
      03FFF00003FFF00003FFF00003FFF00003FFF00001FFF00000FFF000007FF000
      003FF000001FF000020FF0000307F0000383F00003C1F00003E1F00003F1F00C
      03FFF0FFE3FF2800000030000000600000000100040000000000000600000000
      0000000000001000000000000000000000000000800000800000008080008000
      0000800080008080000080808000C0C0C0000000FF0000FF000000FFFF00FF00
      0000FF00FF00FFFF0000FFFFFF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000008
      7770000000000000000000000000000000000000000000088870000000000000
      0000000000000000000000000000000888700000000000000000000000000000
      0000000000000008888000000000070007000000000000000000000000000000
      0000000000070000000700000000000000000000000000000000000000700000
      0000700000000000000000000000000000077000000000077800070000000000
      0000000000000000000007000000007000000070000000000000000000000000
      0000070000000070000000070000000000000000000000000000007700007700
      0000000070000000000000000000000000000700700700000000000007000000
      0000000000000000000080007007000000000000078000000000000000000000
      00007000077877000000000000700000000000000000000000070000700F8870
      0000000000070000000000000000000000070000700FF8870000000000070000
      0000000000000000008000770000FF8870000000000080000000000000000000
      0070070000000FF88700000000007000000000000000000000700700000000FF
      88700000000070000000000000000000000008000000000FF887000000000000
      00000000000000000007700000000000FF887000000000000000000000000000
      00077000000000000FF887000000000000000000000000000007770000000000
      00FF88700000000000000000000000000007770000000000000FF88700000000
      000000000000000000077700000000000000FF88700000000000000000000000
      000777BBBBBBBBBBBB300FF88700000000000000000000000007773BBB300003
      BBBB00FF887000000000000000000000000777003BB300003BBB300FF8870000
      00000000000000000007770003BB3000003BB000FF8870000000000000000000
      00077700003BB300000000000FF887000000000000000000000777000003BBB0
      0000000000FF887000000000000000000007770000003BBBB0000000000FF887
      000000000000000000077700000003BBB30000000000FF887000000000000000
      00077700000000BBBB30000000000FF887000000000000000007770000000000
      3BB30000000000FF8870000000000000000777000BB3000000BBB3000000000F
      F887000000000000000777000BBB30000003BB3000000000FF88700000000000
      0007770003BBB30000003BB3000000000FF887000000000000077700003BBBBB
      BBBBBBBB0000000000FF8870000000000007770000033BBBBBBBBBBB00000000
      000FF88700000000000777000000000000000000000000000000FF8870000000
      0007770007777777777777770000000000000FF8870000000007770007777777
      7777777700000000000000FF8700000000077777700000000000000777000000
      0000000777000000000770000778880008887700000000000000000000000000
      0007700078000000000008700000000000000000000000000000078000000000
      0000000087000000000000000000F3FFFFFFFFFF0000E1FFFFFFFFFF0000C0FF
      FFFFFFFF000080FFFFFFFFFF0000807FFFFFFFFF0000C03F83FFFFFF0000E01E
      00FFFFFF0000F80C007FFFFF0000FC00003FFFFF0000FE00001FFFFF0000FF00
      000FFFFF0000FF800007FFFF0000FF800003FFFF0000FF000001FFFF0000FF00
      0001FFFF0000FE000000FFFF0000FE000000FFFF0000FC0000007FFF0000FC00
      00007FFF0000FC0000007FFF0000FC0000007FFF0000FC0000007FFF0000FC00
      00007FFF0000FC0000007FFF0000FC0000007FFF0000FC0000007FFF0000FC00
      00007FFF0000FC0000007FFF0000FC0000007FFF0000FC0000003FFF0000FC00
      00001FFF0000FC0000000FFF0000FC00000007FF0000FC00000003FF0000FC00
      000001FF0000FC00000000FF0000FC000000407F0000FC000000603F0000FC00
      0000701F0000FC000000780F0000FC0000007C070000FC0000007E030000FC00
      00007F030000FC0000007F830000FC0000007FC30000FC0038007FFF0000FC03
      FF807FFF0000FC1FFFF07FFF0000280000003000000060000000010008000000
      0000800A00000000000000000000000100000000000000000000FFFFFF007777
      7700ABABAB0011111100C5C5C5005555550044444400454545005F5F5F007878
      780022222200D2D2D200AAAAAA00DDDDDD0088888800005D5D00009B9B0033FE
      FF006CFFFF00007C7C0000F0F00048FFFF0000BABA0000008000008000000080
      8000800000008000800080800000C0C0C000808080000000FF0000FF000000FF
      FF00FF000000FF00FF00FFFF000009090900121212001F1F1F002C2C2C003939
      3900525252006C6C6C0085858500929292009F9F9F00B8B8B800DEDEDE00EBEB
      EB00F8F8F800F0FBFF00A4A0A000C0DCC000F0CAA60000003E0000005D000000
      7C0000009B000000BA000000D9000000F0002424FF004848FF006C6CFF009090
      FF00B4B4FF0000143E00001E5D0000287C0000329B00003CBA000046D9000055
      F000246DFF004885FF006C9DFF0090B5FF00B4CDFF00002A3E00003F5D000054
      7C0000699B00007EBA000093D90000AAF00024B6FF0048C2FF006CCEFF0090DA
      FF00B4E6FF00003E3E0000D9D90024FFFF0090FFFF00B4FFFF00003E2A00005D
      3F00007C5400009B690000BA7E0000D9930000F0AA0024FFB60048FFC2006CFF
      CE0090FFDA00B4FFE600003E1400005D1E00007C2800009B320000BA3C0000D9
      460000F0550024FF6D0048FF85006CFF9D0090FFB500B4FFCD00003E0000005D
      0000007C0000009B000000BA000000D9000000F0000024FF240048FF48006CFF
      6C0090FF9000B4FFB400143E00001E5D0000287C0000329B00003CBA000046D9
      000055F000006DFF240085FF48009DFF6C00B5FF9000CDFFB4002A3E00003F5D
      0000547C0000699B00007EBA000093D90000AAF00000B6FF2400C2FF4800CEFF
      6C00DAFF9000E6FFB4003E3E00005D5D00007C7C00009B9B0000BABA0000D9D9
      0000F0F00000FFFF2400FFFF4800FFFF6C00FFFF9000FFFFB4003E2A00005D3F
      00007C5400009B690000BA7E0000D9930000F0AA0000FFB62400FFC24800FFCE
      6C00FFDA9000FFE6B4003E1400005D1E00007C2800009B320000BA3C0000D946
      0000F0550000FF6D2400FF854800FF9D6C00FFB59000FFCDB4003E0000005D00
      00007C0000009B000000BA000000D9000000F0000000FF242400FF484800FF6C
      6C00FF909000FFB4B4003E0014005D001E007C0028009B003200BA003C00D900
      4600F0005500FF246D00FF488500FF6C9D00FF90B500FFB4CD003E002A005D00
      3F007C0054009B006900BA007E00D9009300F000AA00FF24B600FF48C200FF6C
      CE00FF90DA00FFB4E6003E003E005D005D007C007C009B009B00BA00BA00D900
      D900F000F000FF24FF00FF48FF00FF6CFF00FF90FF00FFB4FF002A003E003F00
      5D0054007C0069009B007E00BA009300D900AA00F000B624FF00C248FF00CE6C
      FF00DA90FF00E6B4FF0014003E001E005D0028007C0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000003060606000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000E03030600000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000E030306000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000E0E0E0327270000000000000000090000000900000000
      00000000000000000000000000000000000000000000000000000000000B0B27
      0000000000090000270000000009000000000000000000000000000000000000
      000000000000000000000000000B0B2700000000090000002700000000000900
      000000000000000000000000000000000000000000000000000000000007070B
      2727000000272727000F0F0E0000000900000000000000000000000000000000
      000000000000000000000000000000070B0B2700002700000F00000000002700
      0900000000000000000000000000000000000000000000000000000000000007
      0B0B2700002700000F0000000000270000090000000000000000000000000000
      0000000000000000000000000000000007070B2727000F0F0000000000000027
      2700090000000000000000000000000000000000000000000000000000000009
      0000070B0B0F0000270000000000000000270009000000000000000000000000
      000000000000000000000000000003090000070B0B0F00002700000000000000
      0027000903000000000000000000000000000000000000000000000000000900
      0707000F0F0E0F0F000000000000000000270000090000000000000000000000
      0000000000000000000000000009090700000F0000010E030F00000000000000
      0000272700090000000000000000000000000000000000000000000000090007
      00000F000001010E030F00000000000000002727000900000000000000000000
      000000000000000000000000030000000F0F0027270001010E030F0000002700
      000027270000030000000000000000000000000000000000000000000900000F
      0000272727270001010E030F0000272727270000270009000000000000000000
      0000000000000000000000000900000F000027272727000001010E030F002727
      272700002700090000000000000000000000000000000000000000000000000E
      00002727272727270001010E030F002727270000270000000000000000000000
      000000000000000000000000000606000B0B272727272727270001010E030F00
      0027272727000000000000000000000000000000000000000000000000060600
      0B0B27272727272727000001010E030F00272727270000000000000000000000
      000000000000000000000000000606070B0B2727272727272727270001010E03
      0F00000027000000000000000000000000000000000000000000000000060607
      2727272727272727272727000001010E030F0000270000000000000000000000
      0000000000000000000000000006060727272727272727272727270000000101
      0E030F0027000000000000000000000000000000000000000000000000060607
      12121212121212121212121211000001010E030F000000000000000000000000
      0000000000000000000000000006060710131213102727272710131212122700
      01010E030F000000000000000000000000000000000000000000000000060607
      272711131314272727001012121211000001010E030F00000000000000000000
      0000000000000000000000000006060727272711121214272727270014131227
      270001010E030F00000000000000000000000000000000000000000000060607
      2727272710131217272727272727002727270001010E030F0000000000000000
      0000000000000000000000000006060727272727271012121327272727270027
      2727000001010E030F0000000000000000000000000000000000000000060607
      27272727272710131212162727272727272700000001010E030F000000000000
      0000000000000000000000000006060727272727272727101212121027272727
      27270000270001010E030F000000000000000000000000000000000000060607
      272727272727272716121213102727272727000027000001010E030F00000000
      0000000000000000000000000006060727272727272727272727111212112727
      272700002700000001010E030F00000000000000000000000000000000060607
      2727271212142727272727271513121027270000270000000001010E030F0000
      0000000000000000000000000006060727272712121314272727272727141213
      1027000027000000000001010E030F0000000000000000000000000000060607
      27272710131212112727272727271412121000002700000000000001010E030F
      000000000000000000000000000606070B0B2727101312121212121212121212
      12122727270000000000000001010E030F000000000000000000000000060607
      0B0B27272710111212121212121212121212272727000000000000000001010E
      030F00000000000000000000000606070B0B2727272727272700000000000000
      000000002700000000000000000001010E030F00000000000000000000060607
      0B0B270707070707070707070707070707070000270000000000000000000001
      010C03070000000000000000000606070B0B2707070707070707070707070707
      0707000027000000000000000000000001010509000000000000000000060606
      0707070000000000000000000000002727070707270000000000000000000000
      0007090A00000000000000000002020000000002020303050000000503030202
      0000000027000000000000000000000000000000000000000000000000020200
      0000020300000000000000000000000302000000270000000000000000000000
      0000000000000000000000000000000203000000000000000000000000000000
      00000302000000000000000000000000000000000000F3FFFFFFFFFF0000E1FF
      FFFFFFFF0000C0FFFFFFFFFF000080FFFFFFFFFF0000807FFFFFFFFF0000C03F
      83FFFFFF0000E01E00FFFFFF0000F80C007FFFFF0000FC00003FFFFF0000FE00
      001FFFFF0000FF00000FFFFF0000FF800007FFFF0000FF800003FFFF0000FF00
      0001FFFF0000FF000001FFFF0000FE000000FFFF0000FE000000FFFF0000FC00
      00007FFF0000FC0000007FFF0000FC0000007FFF0000FC0000007FFF0000FC00
      00007FFF0000FC0000007FFF0000FC0000007FFF0000FC0000007FFF0000FC00
      00007FFF0000FC0000007FFF0000FC0000007FFF0000FC0000007FFF0000FC00
      00003FFF0000FC0000001FFF0000FC0000000FFF0000FC00000007FF0000FC00
      000003FF0000FC00000001FF0000FC00000000FF0000FC000000407F0000FC00
      0000603F0000FC000000701F0000FC000000780F0000FC0000007C070000FC00
      00007E030000FC0000007F030000FC0000007F830000FC0000007FC30000FC00
      38007FFF0000FC03FF807FFF0000FC1FFFF07FFF000028000000300000006000
      0000010020000000000080250000000000000000000000000000000000000000
      0000000000000000000000000000000000FF000000FF00000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000FF000000FF000000FF000000FF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000FFAAAAAAFF555555FF555555FF555555FF000000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000FF000000FFDDDDDDFFAAAAAAFFAAAAAAFF555555FF000000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000FF000000FFDDDDDDFFAAAAAAFFAAAAAAFF555555FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000FFDDDDDDFFDDDDDDFFDDDDDDFFAAAAAAFF111111FF1111
      11FF000000FF0000000000000000000000000000000000000000000000000000
      00005F5F5FFF000000FF000000FF000000FF5F5F5FFF00000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000FF000000FF000000FF000000FF222222FF2222
      22FF111111FF000000FF000000000000000000000000000000005F5F5FFF0000
      00FF000000FF111111FF000000FF000000FF000000FF000000FF5F5F5FFF0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000FF000000FF222222FF2222
      22FF111111FF000000FF000000FF00000000000000005F5F5FFF000000FF0000
      00FF000000FF111111FF000000FF000000FF000000FF000000FF000000FF5F5F
      5FFF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF444444FF4444
      44FF222222FF111111FF111111FF000000FF000000FF000000FF111111FF1111
      11FF111111FF000000FF888888FF888888FFDDDDDDFF000000FF000000FF0000
      00FF5F5F5FFF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000FF0000
      00FF444444FF222222FF222222FF111111FF000000FF000000FF111111FF0000
      00FF000000FF888888FF000000FF000000FF000000FF000000FF000000FF1111
      11FF000000FF5F5F5FFF00000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF444444FF222222FF222222FF111111FF000000FF000000FF111111FF0000
      00FF000000FF888888FF000000FF000000FF000000FF000000FF000000FF1111
      11FF000000FF000000FF5F5F5FFF000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000FF444444FF444444FF222222FF111111FF111111FF000000FF8888
      88FF888888FF000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF111111FF111111FF000000FF5F5F5FFF0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00005F5F5FFF000000FF000000FF444444FF222222FF222222FF888888FF0000
      00FF000000FF111111FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF000000FF111111FF000000FF5F5F5FFF00000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000ABAB
      ABFF5F5F5FFF000000FF000000FF444444FF222222FF222222FF888888FF0000
      00FF000000FF111111FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF000000FF111111FF000000FF5F5F5FFFABABABFF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000005F5F
      5FFF000000FF444444FF444444FF000000FF888888FF888888FFDDDDDDFF8888
      88FF888888FF000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF000000FF111111FF000000FF000000FF5F5F5FFF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000005F5F5FFF5F5F
      5FFF444444FF000000FF000000FF888888FF000000FF000000FFFFFFFFFFDDDD
      DDFFAAAAAAFF888888FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF000000FF000000FF111111FF111111FF000000FF5F5F5FFF0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000005F5F5FFF0000
      00FF444444FF000000FF000000FF888888FF000000FF000000FFFFFFFFFFFFFF
      FFFFDDDDDDFFAAAAAAFF888888FF000000FF000000FF000000FF000000FF0000
      00FF000000FF000000FF000000FF111111FF111111FF000000FF5F5F5FFF0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000ABABABFF000000FF0000
      00FF000000FF888888FF888888FF000000FF111111FF111111FF000000FFFFFF
      FFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF000000FF000000FF000000FF1111
      11FF000000FF000000FF000000FF111111FF111111FF000000FF000000FFABAB
      ABFF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000005F5F5FFF000000FF0000
      00FF888888FF000000FF000000FF111111FF111111FF111111FF111111FF0000
      00FFFFFFFFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF000000FF000000FF1111
      11FF111111FF111111FF111111FF000000FF000000FF111111FF000000FF5F5F
      5FFF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000005F5F5FFF000000FF0000
      00FF888888FF000000FF000000FF111111FF111111FF111111FF111111FF0000
      00FF000000FFFFFFFFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF000000FF1111
      11FF111111FF111111FF111111FF000000FF000000FF111111FF000000FF5F5F
      5FFF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF000000FF0000
      00FFDDDDDDFF000000FF000000FF111111FF111111FF111111FF111111FF1111
      11FF111111FF000000FFFFFFFFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF0000
      00FF111111FF111111FF111111FF000000FF000000FF111111FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF000000FF222222FF222222FF111111FF111111FF111111FF111111FF1111
      11FF111111FF111111FF000000FFFFFFFFFFFFFFFFFFDDDDDDFFAAAAAAFF8888
      88FF000000FF000000FF111111FF111111FF111111FF111111FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF000000FF222222FF222222FF111111FF111111FF111111FF111111FF1111
      11FF111111FF111111FF000000FF000000FFFFFFFFFFFFFFFFFFDDDDDDFFAAAA
      AAFF888888FF000000FF111111FF111111FF111111FF111111FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF222222FF222222FF111111FF111111FF111111FF111111FF1111
      11FF111111FF111111FF111111FF111111FF000000FFFFFFFFFFFFFFFFFFDDDD
      DDFFAAAAAAFF888888FF000000FF000000FF000000FF111111FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF111111FF111111FF111111FF111111FF1111
      11FF111111FF111111FF111111FF111111FF000000FF000000FFFFFFFFFFFFFF
      FFFFDDDDDDFFAAAAAAFF888888FF000000FF000000FF111111FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF111111FF111111FF111111FF111111FF1111
      11FF111111FF111111FF111111FF111111FF000000FF000000FF000000FFFFFF
      FFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF000000FF111111FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FE
      FFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF009B9BFF000000FF0000
      00FFFFFFFFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF000000FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF005D5DFF6CFFFFFF33FEFFFF6CFFFFFF005D5DFF111111FF1111
      11FF111111FF111111FF005D5DFF6CFFFFFF33FEFFFF33FEFFFF33FEFFFF1111
      11FF000000FFFFFFFFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF009B9BFF6CFFFFFF6CFFFFFF007C7CFF1111
      11FF111111FF111111FF000000FF005D5DFF33FEFFFF33FEFFFF33FEFFFF009B
      9BFF000000FF000000FFFFFFFFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF111111FF009B9BFF33FEFFFF33FEFFFF007C
      7CFF111111FF111111FF111111FF111111FF000000FF007C7CFF6CFFFFFF33FE
      FFFF111111FF111111FF000000FFFFFFFFFFFFFFFFFFDDDDDDFFAAAAAAFF8888
      88FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF111111FF111111FF005D5DFF6CFFFFFF33FE
      FFFF00BABAFF111111FF111111FF111111FF111111FF111111FF111111FF0000
      00FF111111FF111111FF111111FF000000FFFFFFFFFFFFFFFFFFDDDDDDFFAAAA
      AAFF888888FF000000FF00000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF111111FF111111FF111111FF005D5DFF33FE
      FFFF33FEFFFF6CFFFFFF111111FF111111FF111111FF111111FF111111FF0000
      00FF111111FF111111FF111111FF000000FF000000FFFFFFFFFFFFFFFFFFDDDD
      DDFFAAAAAAFF888888FF000000FF000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF111111FF111111FF111111FF111111FF005D
      5DFF6CFFFFFF33FEFFFF33FEFFFF48FFFFFF111111FF111111FF111111FF1111
      11FF111111FF111111FF111111FF000000FF000000FF000000FFFFFFFFFFFFFF
      FFFFDDDDDDFFAAAAAAFF888888FF000000FF0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF111111FF111111FF111111FF111111FF1111
      11FF005D5DFF33FEFFFF33FEFFFF33FEFFFF005D5DFF111111FF111111FF1111
      11FF111111FF111111FF111111FF000000FF000000FF111111FF000000FFFFFF
      FFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF000000FF00000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF111111FF111111FF111111FF111111FF1111
      11FF111111FF48FFFFFF33FEFFFF33FEFFFF6CFFFFFF005D5DFF111111FF1111
      11FF111111FF111111FF111111FF000000FF000000FF111111FF000000FF0000
      00FFFFFFFFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF000000FF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF111111FF111111FF111111FF111111FF1111
      11FF111111FF111111FF111111FF009B9BFF33FEFFFF33FEFFFF009B9BFF1111
      11FF111111FF111111FF111111FF000000FF000000FF111111FF000000FF0000
      00FF000000FFFFFFFFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF000000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF111111FF33FEFFFF33FEFFFF007C7CFF1111
      11FF111111FF111111FF111111FF111111FF111111FF00F0F0FF6CFFFFFF33FE
      FFFF005D5DFF111111FF111111FF000000FF000000FF111111FF000000FF0000
      00FF00000000000000FFFFFFFFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF111111FF33FEFFFF33FEFFFF6CFFFFFF007C
      7CFF111111FF111111FF111111FF111111FF111111FF111111FF007C7CFF33FE
      FFFF6CFFFFFF005D5DFF111111FF000000FF000000FF111111FF000000FF0000
      00FF0000000000000000000000FFFFFFFFFFFFFFFFFFDDDDDDFFAAAAAAFF8888
      88FF000000FF0000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF111111FF111111FF111111FF005D5DFF6CFFFFFF33FEFFFF33FE
      FFFF009B9BFF111111FF111111FF111111FF111111FF111111FF111111FF007C
      7CFF33FEFFFF33FEFFFF005D5DFF000000FF000000FF111111FF000000FF0000
      00FF000000000000000000000000000000FFFFFFFFFFFFFFFFFFDDDDDDFFAAAA
      AAFF888888FF000000FF00000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF222222FF222222FF111111FF111111FF005D5DFF6CFFFFFF33FE
      FFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FE
      FFFF33FEFFFF33FEFFFF33FEFFFF111111FF111111FF111111FF000000FF0000
      00FF00000000000000000000000000000000000000FFFFFFFFFFFFFFFFFFDDDD
      DDFFAAAAAAFF888888FF000000FF000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF222222FF222222FF111111FF111111FF111111FF005D5DFF009B
      9BFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FEFFFF33FE
      FFFF33FEFFFF33FEFFFF33FEFFFF111111FF111111FF111111FF000000FF0000
      00FF0000000000000000000000000000000000000000000000FFFFFFFFFFFFFF
      FFFFDDDDDDFFAAAAAAFF888888FF000000FF0000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF222222FF222222FF111111FF111111FF111111FF111111FF1111
      11FF111111FF111111FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF000000FF000000FF000000FF000000FF111111FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000FFFFFF
      FFFFFFFFFFFFDDDDDDFFAAAAAAFF888888FF000000FF00000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF222222FF222222FF111111FF444444FF444444FF444444FF4444
      44FF444444FF444444FF444444FF444444FF444444FF444444FF444444FF4444
      44FF444444FF444444FF444444FF000000FF000000FF111111FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      00FFFFFFFFFFFFFFFFFFD2D2D2FFAAAAAAFF454545FF00000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF444444FF222222FF222222FF111111FF444444FF444444FF444444FF4444
      44FF444444FF444444FF444444FF444444FF444444FF444444FF444444FF4444
      44FF444444FF444444FF444444FF000000FF000000FF111111FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000FFFFFFFFFFFFFFFFFFC5C5C5FF5F5F5FFF00000000000000000000
      00000000000000000000000000000000000000000000000000FF555555FF5555
      55FF555555FF444444FF444444FF444444FF000000FF000000FF000000FF0000
      00FF000000FF000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF111111FF111111FF444444FF444444FF444444FF111111FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      000000000000000000FF454545FF5F5F5FFF787878FF00000000000000000000
      00000000000000000000000000000000000000000000000000FF777777FF7777
      77FF000000FF000000FF000000FF000000FF777777FF777777FFABABABFFABAB
      ABFFC5C5C5FF000000000000000000000000C5C5C5FFABABABFFABABABFF7777
      77FF777777FF000000FF000000FF000000FF000000FF111111FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF777777FF7777
      77FF000000FF000000FF000000FF777777FFABABABFF00000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000ABABABFF777777FF000000FF000000FF000000FF111111FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000FF000000FF0000
      00FF777777FFABABABFF00000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000ABABABFF777777FF000000FF000000FF0000
      00FF000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000F3FF
      FFFFFFFF0000E1FFFFFFFFFF0000C0FFFFFFFFFF000080FFFFFFFFFF0000807F
      FFFFFFFF0000C03F83FFFFFF0000E01E00FFFFFF0000F80C007FFFFF0000FC00
      003FFFFF0000FE00001FFFFF0000FF00000FFFFF0000FF800007FFFF0000FF80
      0003FFFF0000FF000001FFFF0000FF000001FFFF0000FE000000FFFF0000FE00
      0000FFFF0000FC0000007FFF0000FC0000007FFF0000FC0000007FFF0000FC00
      00007FFF0000FC0000007FFF0000FC0000007FFF0000FC0000007FFF0000FC00
      00007FFF0000FC0000007FFF0000FC0000007FFF0000FC0000007FFF0000FC00
      00007FFF0000FC0000003FFF0000FC0000001FFF0000FC0000000FFF0000FC00
      000007FF0000FC00000003FF0000FC00000001FF0000FC00000000FF0000FC00
      0000407F0000FC000000603F0000FC000000701F0000FC000000780F0000FC00
      00007C070000FC0000007E030000FC0000007F030000FC0000007F830000FC00
      00007FC30000FC0038007FFF0000FC03FF807FFF0000FC1FFFF07FFF0000}
    Icons.Icons = {
      020000003E010000000001000100101010000000000028010000160000002800
      000010000000200000000100040000000000C000000000000000000000000000
      0000000000000000000000008000008000000080800080000000800080008080
      000080808000C0C0C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF
      0000FFFFFF000000000000000000000000000000000000000001100000000000
      0011110000000000001111000000000000011000000000000000000000000000
      0001100000000000007117000000000000111100000000000011110000000000
      0011110000000000001111000000000000111100000000000011110000000000
      000110000000FFFF0000FFFF0000FE7F0000FC3F0000FC3F0000FE7F0000FFFF
      0000FE7F0000FC3F0000FC3F0000FC3F0000FC3F0000FC3F0000FC3F0000FC3F
      0000FE7F00003E01000000000100010010101000000000002801000016000000
      2800000010000000200000000100040000000000C00000000000000000000000
      0000000000000000000000000000800000800000008080008000000080008000
      8080000080808000C0C0C0000000FF0000FF000000FFFF00FF000000FF00FF00
      FFFF0000FFFFFF0000000000000000000000000000000990000006C6C6009990
      0002F06000099900002F2F078870900000F2F07EFEF700000F2F008FEFE80E00
      02F0E08FFEF80C00000E707FFFE70E000EECE7078870EC000EC0CE70000ECE00
      000F0CE7ECECE00000F2F00ECECEC000000F2F000CEC0000000002F2F0000000
      0000000000000000FFF90000F8300000E0000000C00100008003000080030000
      00010000000100000001000000010000000100008003000080030000C0070000
      E00F0000F83F0000}
    Interval = 400
    PopupMenu = pmAVZTrayMenu
    OnClick = RxTrayIconClick
    Left = 248
    Top = 144
  end
  object XPManifest1: TXPManifest
    Left = 280
    Top = 113
  end
  object pmAVZTrayMenu: TRxPopupMenu
    Left = 279
    Top = 145
    object pmShow: TMenuItem
      Caption = '$AVZ0891'
      Default = True
      OnClick = pmShowClick
    end
    object N51: TMenuItem
      Caption = '-'
    end
    object pmStart: TMenuItem
      Caption = '$AVZ0882'
      OnClick = pmStartClick
    end
    object pmPause: TMenuItem
      Caption = '$AVZ0699'
      Enabled = False
      OnClick = pmPauseClick
    end
    object pmStop: TMenuItem
      Caption = '$AVZ1053'
      Enabled = False
      OnClick = pmStopClick
    end
    object N37: TMenuItem
      Caption = '-'
    end
    object pmExit: TMenuItem
      Caption = '$AVZ0195'
      OnClick = pmExitClick
    end
  end
  object tRunScript: TTimer
    Enabled = False
    OnTimer = tRunScriptTimer
    Left = 189
    Top = 51
  end
  object tWatchDog: TTimer
    Enabled = False
    OnTimer = tWatchDogTimer
    Left = 221
    Top = 51
  end
  object ProfileSaveDialog: TSaveDialog
    DefaultExt = 'prf'
    FileName = 'avz_profile'
    Filter = '$AVZ0873'
    Title = '$AVZ1028'
    Left = 80
    Top = 311
  end
  object ProfileOpenDialog: TOpenDialog
    DefaultExt = 'prf'
    Filter = '$AVZ0873'
    Options = [ofEnableSizing, ofForceShowHidden]
    Left = 120
    Top = 312
  end
  object FontDialog1: TFontDialog
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Left = 152
    Top = 313
  end
end

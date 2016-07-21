object RevizorDLG: TRevizorDLG
  Left = 322
  Top = 274
  Width = 693
  Height = 435
  Caption = '$AVZ0907'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 16
  object StatusBar: TStatusBar
    Left = 0
    Top = 382
    Width = 685
    Height = 19
    Panels = <
      item
        Width = 500
      end
      item
        Width = 150
      end
      item
        Width = 50
      end>
  end
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 685
    Height = 382
    ActivePage = TabSheet1
    Align = alClient
    TabOrder = 1
    OnChanging = PageControlChanging
    object TabSheet1: TTabSheet
      Caption = '$AVZ1020'
      object GroupBox1: TGroupBox
        Left = 0
        Top = 34
        Width = 282
        Height = 317
        Align = alClient
        Caption = ' $AVZ0531 '
        TabOrder = 0
        object ZShellTreeView: TZShellTreeView
          Left = 2
          Top = 18
          Width = 278
          Height = 297
          Align = alClient
          Indent = 19
          ReadOnly = True
          TabOrder = 0
          OnKeyPress = ZShellTreeViewKeyPress
          OnTreeNodeStateIconClick = ZShellTreeViewTreeNodeStateIconClick
        end
      end
      object GroupBox2: TGroupBox
        Left = 282
        Top = 34
        Width = 395
        Height = 317
        Align = alRight
        Caption = ' $AVZ0695  '
        TabOrder = 1
        DesignSize = (
          395
          317)
        object rgFileTypes: TRadioGroup
          Left = 5
          Top = 23
          Width = 384
          Height = 71
          Hint = '$AVZ0791'
          Anchors = [akLeft, akTop, akRight]
          Caption = ' $AVZ1064 '
          ItemIndex = 0
          Items.Strings = (
            '$AVZ0790'
            '$AVZ0158'
            '$AVZ1127')
          TabOrder = 0
          OnClick = rgFileTypesClick
        end
        object cbIncludeFiles: TCheckBox
          Left = 7
          Top = 94
          Width = 242
          Height = 17
          Hint = '$AVZ1100'
          Caption = '$AVZ0107:'
          TabOrder = 1
          OnClick = cbIncludeFilesClick
        end
        object edIncludeFiles: TEdit
          Left = 32
          Top = 110
          Width = 358
          Height = 24
          Hint = '$AVZ1035'
          Anchors = [akLeft, akTop, akRight]
          Enabled = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 2
        end
        object cbExcludeFiles: TCheckBox
          Left = 7
          Top = 134
          Width = 282
          Height = 17
          Hint = '$AVZ1101'
          Caption = '$AVZ0313:'
          TabOrder = 3
          OnClick = cbExcludeFilesClick
        end
        object edExcludeFiles: TEdit
          Left = 32
          Top = 151
          Width = 358
          Height = 24
          Hint = '$AVZ1035'
          Anchors = [akLeft, akTop, akRight]
          Enabled = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 4
        end
        object rgCreateDBMode: TRadioGroup
          Left = 6
          Top = 176
          Width = 384
          Height = 73
          Caption = ' $AVZ0912 '
          ItemIndex = 0
          Items.Strings = (
            '$AVZ1049'
            '$AVZ0084')
          TabOrder = 5
        end
        object mbStop: TBitBtn
          Left = 85
          Top = 254
          Width = 75
          Height = 25
          Hint = '$AVZ0601'
          Caption = '$AVZ1053'
          Enabled = False
          TabOrder = 6
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
        object mbStart: TBitBtn
          Left = 5
          Top = 254
          Width = 75
          Height = 25
          Hint = '$AVZ0280'
          Caption = '$AVZ0882'
          TabOrder = 7
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
      end
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 677
        Height = 34
        Align = alTop
        TabOrder = 2
        DesignSize = (
          677
          34)
        object Label1: TLabel
          Left = 6
          Top = 12
          Width = 64
          Height = 16
          Caption = '$AVZ1109:'
        end
        object feRevizorFileOUT: TFilenameEdit
          Left = 48
          Top = 6
          Width = 628
          Height = 21
          Filter = '$AVZ1129'
          Anchors = [akLeft, akTop, akRight]
          NumGlyphs = 1
          TabOrder = 0
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = '$AVZ1044'
      ImageIndex = 1
      object Panel2: TPanel
        Left = 0
        Top = 0
        Width = 677
        Height = 73
        Align = alTop
        TabOrder = 0
        object Label2: TLabel
          Left = 6
          Top = 12
          Width = 64
          Height = 16
          Caption = '$AVZ1109:'
        end
        object feRevizorFileIN: TFilenameEdit
          Left = 48
          Top = 6
          Width = 620
          Height = 21
          Filter = '$AVZ1129'
          NumGlyphs = 1
          TabOrder = 0
        end
        object mbStart1: TBitBtn
          Left = 4
          Top = 36
          Width = 75
          Height = 25
          Hint = '$AVZ0280'
          Caption = '$AVZ0882'
          TabOrder = 1
          OnClick = mbStart1Click
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
        object mbStop1: TBitBtn
          Left = 83
          Top = 36
          Width = 75
          Height = 25
          Hint = '$AVZ0601'
          Caption = '$AVZ1053'
          Enabled = False
          TabOrder = 2
          OnClick = mbStop1Click
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
      end
      object pcCompareDiskDB: TPageControl
        Left = 0
        Top = 73
        Width = 677
        Height = 278
        ActivePage = TabSheet5
        Align = alClient
        TabOrder = 1
        object TabSheet5: TTabSheet
          Caption = '$AVZ0461'
          ImageIndex = 2
          object cbExcludeWindowsTrusted: TCheckBox
            Left = 5
            Top = 107
            Width = 435
            Height = 17
            Hint = '$AVZ0838'
            Caption = '$AVZ0316'
            Checked = True
            State = cbChecked
            TabOrder = 0
          end
          object cbExcludeGoodFiles: TCheckBox
            Left = 5
            Top = 89
            Width = 442
            Height = 17
            Caption = '$AVZ0315'
            Checked = True
            State = cbChecked
            TabOrder = 1
          end
          object rgCompareMode: TRadioGroup
            Left = 4
            Top = 8
            Width = 429
            Height = 73
            Caption = ' $AVZ0913 '
            ItemIndex = 0
            Items.Strings = (
              '$AVZ1049'
              '$AVZ0085')
            TabOrder = 2
          end
        end
        object TabSheet3: TTabSheet
          Caption = '$AVZ0868'
          object CompareLogMemo: TMemo
            Left = 0
            Top = 0
            Width = 665
            Height = 277
            Align = alClient
            ReadOnly = True
            ScrollBars = ssBoth
            TabOrder = 0
            WordWrap = False
          end
        end
        object TabSheet4: TTabSheet
          Caption = '$AVZ0457'
          ImageIndex = 1
          DesignSize = (
            669
            247)
          object lvFoundFiles: TListView
            Left = 0
            Top = 0
            Width = 669
            Height = 211
            Align = alTop
            Anchors = [akLeft, akTop, akRight, akBottom]
            Checkboxes = True
            Columns = <
              item
                AutoSize = True
                Caption = '$AVZ1109'
                MinWidth = 400
              end
              item
                AutoSize = True
                Caption = '$AVZ0892'
                MinWidth = 100
              end
              item
                AutoSize = True
                Caption = '$AVZ1009'
                MinWidth = 100
              end>
            ReadOnly = True
            SortType = stText
            TabOrder = 0
            ViewStyle = vsReport
          end
          object mbDeleteChecked: TBitBtn
            Left = 319
            Top = 219
            Width = 214
            Height = 25
            Anchors = [akLeft, akBottom]
            Caption = '$AVZ1088'
            TabOrder = 1
            OnClick = mbDeleteCheckedClick
            Glyph.Data = {
              F6000000424DF600000000000000760000002800000010000000100000000100
              0400000000008000000000000000000000001000000010000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
              FFFFFFFFFFFFFFFFF1FFFFF1FFFFFFFFFFFFFF111FFFFFFF1FFFFF111FFFFFF1
              FFFFFFF111FFFF11FFFFFFFF111FF11FFFFFFFFFF11111FFFFFFFFFFFF111FFF
              FFFFFFFFF11111FFFFFFFFFF111FF1FFFFFFFF1111FFFF11FFFFF1111FFFFFF1
              1FFFF11FFFFFFFFF11FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
          end
          object mbQurantineChecked: TBitBtn
            Left = -1
            Top = 219
            Width = 312
            Height = 25
            Anchors = [akLeft, akBottom]
            Caption = '$AVZ0371'
            TabOrder = 2
            OnClick = mbQurantineCheckedClick
            Glyph.Data = {
              F6000000424DF600000000000000760000002800000010000000100000000100
              0400000000008000000000000000000000001000000010000000000000000000
              8000008000000080800080000000800080008080000080808000C0C0C0000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFF1
              1111FFFFFFFFFFF11F11FFFFFF4444411F11FFFFFF4FFFF11111FFFFFF4F7771
              FFF10000004FFFFF111F0FFFFF4F77777F4F0F77774FFFFFFF4F0FFFFF4F77F4
              444F0F77774FFFF4F4FF0FFFFF4FFFF44FFF0F77F0444444FFFF0FFFF0F0FFFF
              FFFF0FFFF00FFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFF}
          end
        end
      end
    end
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 168
    Top = 176
  end
end
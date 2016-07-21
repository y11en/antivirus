object Main: TMain
  Left = 234
  Top = 118
  Width = 729
  Height = 640
  Caption = '$AVZ1248'
  Color = clBtnFace
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poDesktopCenter
  Scaled = False
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 16
  object StatusBar1: TStatusBar
    Left = 0
    Top = 567
    Width = 721
    Height = 19
    Panels = <>
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 721
    Height = 36
    Align = alTop
    TabOrder = 1
    object mbScriptWizard: TBitBtn
      Left = 8
      Top = 5
      Width = 169
      Height = 25
      Caption = '$AVZ1254'
      TabOrder = 0
      OnClick = mbScriptWizardClick
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        0400000000008000000000000000000000001000000010000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFF7FFFFF7FFFFFFFF717FFF717FFFFFFFF7FFFFF7FFF
        FFFFFFFFFFFFFFFFFF00FFFFF7FFFFFFF000F7FF717FFFFF000F717FF7FFFFF0
        00FFF7FFFFFFFF000FFFFFFFFFFFF000FFFFFFF7FFFF000FFFFFFF717FF000FF
        FFFFFFF7FF0B0FFFFFFFFFFFF0B0FFFFFFFFFFFFF70FFFFFFFFF}
    end
    object mbAddCmd: TBitBtn
      Left = 184
      Top = 5
      Width = 161
      Height = 25
      Caption = '$AVZ1255'
      TabOrder = 1
      OnClick = mbAddCmdClick
    end
    object mbGenerateTxt: TBitBtn
      Left = 534
      Top = 5
      Width = 161
      Height = 25
      Caption = '$AVZ1256'
      TabOrder = 2
      OnClick = mbGenerateTxtClick
    end
    object mbChkSyn: TBitBtn
      Left = 350
      Top = 5
      Width = 179
      Height = 25
      Caption = '$AVZ0809'
      TabOrder = 3
      OnClick = mbChkSynClick
    end
  end
  object mmSyntax: TfsSyntaxMemo
    Left = 0
    Top = 36
    Width = 721
    Height = 426
    Cursor = crIBeam
    Align = alClient
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New Cyr'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    TabOrder = 2
    TabStop = True
    BlockColor = clHighlight
    BlockFontColor = clHighlightText
    CommentAttr.Charset = RUSSIAN_CHARSET
    CommentAttr.Color = clNavy
    CommentAttr.Height = -13
    CommentAttr.Name = 'Courier New Cyr'
    CommentAttr.Style = [fsItalic]
    KeywordAttr.Charset = RUSSIAN_CHARSET
    KeywordAttr.Color = clWindowText
    KeywordAttr.Height = -13
    KeywordAttr.Name = 'Courier New Cyr'
    KeywordAttr.Style = [fsBold]
    UserKeywordAttr.Charset = DEFAULT_CHARSET
    UserKeywordAttr.Color = clGreen
    UserKeywordAttr.Height = -13
    UserKeywordAttr.Name = 'Courier New Cyr'
    UserKeywordAttr.Style = []
    StringAttr.Charset = RUSSIAN_CHARSET
    StringAttr.Color = clNavy
    StringAttr.Height = -13
    StringAttr.Name = 'Courier New Cyr'
    StringAttr.Style = []
    TextAttr.Charset = RUSSIAN_CHARSET
    TextAttr.Color = clWindowText
    TextAttr.Height = -13
    TextAttr.Name = 'Courier New Cyr'
    TextAttr.Style = []
    Lines.Strings = (
      '')
    ReadOnly = False
    SyntaxType = stPascal
    ShowFooter = True
    ShowGutter = True
    OnCheckUserKeyword = mmSyntaxCheckUserKeyword
    OnKeyDown = mmSyntaxKeyDown
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 462
    Width = 721
    Height = 105
    Align = alBottom
    Caption = ' $AVZ1257 '
    TabOrder = 3
    object mmHelp: TMemo
      Left = 2
      Top = 18
      Width = 681
      Height = 85
      Align = alClient
      ReadOnly = True
      ScrollBars = ssVertical
      TabOrder = 0
    end
    object Panel2: TPanel
      Left = 683
      Top = 18
      Width = 36
      Height = 85
      Align = alRight
      TabOrder = 1
      object sbWebHelp: TSpeedButton
        Left = 8
        Top = 8
        Width = 23
        Height = 22
        Hint = '$AVZ1258'
        Glyph.Data = {
          76010000424D7601000000000000760000002800000020000000100000000100
          04000000000000010000120B0000120B00001000000000000000000000000000
          800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF0033333CCCCC33
          33333FFFF77777FFFFFFCCCCCC808CCCCCC3777777F7F777777F008888070888
          8003777777777777777F0F0770F7F0770F0373F33337F333337370FFFFF7FFFF
          F07337F33337F33337F370FFFB99FBFFF07337F33377F33337F330FFBF99BFBF
          F033373F337733333733370BFBF7FBFB0733337F333FF3337F33370FBF98BFBF
          0733337F3377FF337F333B0BFB990BFB03333373FF777FFF73333FB000B99000
          B33333377737777733333BFBFBFB99FBF33333333FF377F333333FBF99BF99BF
          B333333377F377F3333333FB99FB99FB3333333377FF77333333333FB9999FB3
          333333333777733333333333FBFBFB3333333333333333333333}
        NumGlyphs = 2
        ParentShowHint = False
        ShowHint = True
        OnClick = sbWebHelpClick
      end
    end
  end
  object lbCmdList: TTextListBox
    Left = 0
    Top = 36
    Width = 721
    Height = 426
    Align = alClient
    ItemHeight = 16
    TabOrder = 4
    Visible = False
    OnDblClick = lbCmdListDblClick
    OnExit = lbCmdListExit
    OnKeyDown = lbCmdListKeyDown
  end
  object MainMenu1: TMainMenu
    Left = 144
    Top = 216
    object mmFile: TMenuItem
      Caption = '$AVZ1109'
      object N2: TMenuItem
        Caption = '$AVZ1259'
        OnClick = N2Click
      end
      object N14: TMenuItem
        Caption = '$AVZ1254'
        OnClick = N14Click
      end
      object N7: TMenuItem
        Caption = '$AVZ1260'
        OnClick = N7Click
      end
      object N3: TMenuItem
        Caption = '$AVZ1261'
        OnClick = N3Click
      end
      object N4: TMenuItem
        Caption = '$AVZ1029'
        OnClick = N4Click
      end
      object N10: TMenuItem
        Caption = '-'
      end
      object N6: TMenuItem
        Caption = '$AVZ0195'
        OnClick = N6Click
      end
    end
    object mmEdit: TMenuItem
      Caption = '$AVZ1262'
      object N19: TMenuItem
        Caption = '$AVZ1263'
        ShortCut = 16474
        OnClick = N19Click
      end
      object N18: TMenuItem
        Caption = '-'
      end
      object N11: TMenuItem
        Caption = '$AVZ0359'
        ShortCut = 16451
        OnClick = N11Click
      end
      object N12: TMenuItem
        Caption = '$AVZ1264'
        ShortCut = 16472
        OnClick = N12Click
      end
      object N13: TMenuItem
        Caption = '$AVZ1265'
        ShortCut = 16470
        OnClick = N13Click
      end
      object N15: TMenuItem
        Caption = '$AVZ0173'
        ShortCut = 16449
        OnClick = N15Click
      end
    end
    object mmService: TMenuItem
      Caption = '$AVZ0945'
      object N21: TMenuItem
        Caption = '$AVZ1255'
        ShortCut = 16397
        OnClick = N21Click
      end
      object N22: TMenuItem
        Caption = '$AVZ0809'
        ShortCut = 120
        OnClick = N22Click
      end
      object Utythbhjdfnmntrcn1: TMenuItem
        Caption = '$AVZ1256'
        OnClick = Utythbhjdfnmntrcn1Click
      end
    end
    object mmHlp: TMenuItem
      Caption = '$AVZ1041'
      object N9: TMenuItem
        Caption = '$AVZ0529'
        OnClick = N9Click
      end
    end
  end
  object XPManifest: TXPManifest
    Left = 176
    Top = 216
  end
  object Timer1: TTimer
    Interval = 200
    OnTimer = Timer1Timer
    Left = 216
    Top = 216
  end
  object OpenDialog: TOpenDialog
    DefaultExt = '.txt'
    Filter = '$AVZ0988|*.avz|$AVZ1058'
    Left = 144
    Top = 256
  end
  object SaveDialog: TSaveDialog
    DefaultExt = '.txt'
    Filter = '$AVZ0988|*.avz|$AVZ1058'
    Left = 176
    Top = 256
  end
end

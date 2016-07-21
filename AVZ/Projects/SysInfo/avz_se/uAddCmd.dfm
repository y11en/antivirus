object AddCmdDLG: TAddCmdDLG
  Left = 319
  Top = 155
  Width = 617
  Height = 640
  BorderIcons = [biSystemMenu]
  Caption = '$AVZ1250'
  Color = clBtnFace
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  object Panel1: TPanel
    Left = 0
    Top = 546
    Width = 609
    Height = 41
    Align = alBottom
    TabOrder = 0
    DesignSize = (
      609
      41)
    object BitBtn1: TBitBtn
      Left = 416
      Top = 8
      Width = 91
      Height = 25
      Anchors = [akTop, akRight]
      TabOrder = 0
      Kind = bkOK
    end
    object BitBtn2: TBitBtn
      Left = 512
      Top = 8
      Width = 89
      Height = 25
      Anchors = [akTop, akRight]
      Caption = '$AVZ0612'
      TabOrder = 1
      Kind = bkCancel
    end
    object cbComment: TCheckBox
      Left = 8
      Top = 16
      Width = 329
      Height = 17
      Caption = '$AVZ1251'
      Checked = True
      State = cbChecked
      TabOrder = 2
    end
  end
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 609
    Height = 546
    ActivePage = tsCmdList
    Align = alClient
    TabOrder = 1
    OnChange = PageControlChange
    object tsCmdList: TTabSheet
      Caption = '$AVZ1252'
      object TextListBox: TTextListBox
        Left = 0
        Top = 0
        Width = 601
        Height = 437
        Align = alClient
        ExtendedSelect = False
        ItemHeight = 16
        ParentShowHint = False
        ShowHint = True
        Sorted = True
        TabOrder = 0
        OnClick = TextListBoxClick
        OnDblClick = TextListBoxDblClick
      end
      object GroupBox1: TGroupBox
        Left = 0
        Top = 437
        Width = 601
        Height = 78
        Align = alBottom
        Caption = ' $AVZ1131 '
        TabOrder = 1
        DesignSize = (
          601
          78)
        object Label1: TLabel
          Left = 8
          Top = 24
          Width = 61
          Height = 16
          Caption = '$AVZ0331'
        end
        object Label2: TLabel
          Left = 8
          Top = 48
          Width = 61
          Height = 16
          Caption = '$AVZ1253'
        end
        object cbGroupFilter: TComboBox
          Left = 84
          Top = 16
          Width = 513
          Height = 24
          Style = csDropDownList
          Anchors = [akLeft, akTop, akRight]
          ItemHeight = 16
          TabOrder = 0
          OnChange = cbGroupFilterChange
        end
        object ceFilter: TComboEdit
          Left = 120
          Top = 44
          Width = 475
          Height = 21
          NumGlyphs = 1
          TabOrder = 1
          OnChange = ceFilterChange
        end
      end
    end
    object tsTop20: TTabSheet
      Caption = 'Top 20'
      ImageIndex = 1
      object Top20ListBox: TTextListBox
        Left = 0
        Top = 0
        Width = 601
        Height = 515
        Align = alClient
        ExtendedSelect = False
        ItemHeight = 16
        ParentShowHint = False
        ShowHint = True
        Sorted = True
        TabOrder = 0
        OnClick = TextListBoxClick
        OnDblClick = TextListBoxDblClick
      end
    end
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 587
    Width = 609
    Height = 19
    Panels = <>
    SimplePanel = True
  end
end

; Kaspersky Anti-Virus/Internet Security 6.0/7.0 Web Package
; ----------------------------------------------------------

; Checking input arguments

!ifndef ARG_INDIR
!error "ARG_INDIR not defined"
!endif

!ifndef ARG_OUTDIR
!error "ARG_OUTDIR not defined"
!endif

!ifndef ARG_VERSION_MAJOR | ARG_VERSION_MINOR | ARG_VERSION_MPACK | ARG_VERSION_BUILD
!error "ARG_VERSION_* not defined"
!endif

!ifndef ARG_LOCALIZATION | ARG_LOCALIZATION_SHORT
!error "ARG_LOCALIZATION_* not defined"
!endif

!ifdef ARG_PRODUCT_KAV
  !define KAV
!else ifdef ARG_PRODUCT_KIS
  !define KIS
!else ifdef ARG_PRODUCT_WKS
  !define WKS
!else ifdef ARG_PRODUCT_SRV
  !define SRV
!else ifdef ARG_PRODUCT_SOS
  !define SOS
!else
  !error "ARG_PRODUCT_* is not defined"
!endif

; Settings

!define VER_NAME "${ARG_VERSION_MAJOR}.${ARG_VERSION_MINOR}"
!define VER_FULL "${ARG_VERSION_MAJOR}.${ARG_VERSION_MINOR}.${ARG_VERSION_MPACK}.${ARG_VERSION_BUILD}"

!ifdef KAV
  !define PRODUCT_PATH     "Kaspersky Anti-Virus ${VER_FULL}"
  !define PRODUCT_FILE     "kav"
  !define BITMAP_SUFFIX    kav
!else ifdef KIS
  !define PRODUCT_PATH     "Kaspersky Internet Security ${VER_FULL}"
  !define PRODUCT_FILE     "kis"
  !define BITMAP_SUFFIX    kis
!else ifdef WKS
  !define PRODUCT_PATH     "Kaspersky Anti-Virus ${VER_FULL} for Windows Workstations"
  !define PRODUCT_FILE     "kavwks"
  !define BITMAP_SUFFIX    wks
!else ifdef SRV
  !define PRODUCT_PATH     "Kaspersky Anti-Virus ${VER_FULL} for Windows Servers"
  !define PRODUCT_FILE     "kavsrv"
  !define BITMAP_SUFFIX    srv
!else ifdef SOS
  !define PRODUCT_PATH     "Kaspersky Anti-Virus ${VER_FULL} SOS"
  !define PRODUCT_FILE     "kavsos"
  !define BITMAP_SUFFIX    sos
!endif

!define COMPANY_SETUPFILES_PATH "Kaspersky Lab Setup Files"

OutFile "${ARG_OUTDIR}\${PRODUCT_FILE}${VER_FULL}${ARG_LOCALIZATION_SHORT}.exe"

!define MUI_VERBOSE 4
!include "MUI.nsh"

RequestExecutionLevel user
SetCompressor /SOLID lzma
;SetCompress off
ShowInstDetails nevershow
BrandingText " "

!define MUI_ICON "res\kavpers-inst.ico"
!define MUI_UNICON "res\kavpers-inst.ico"

!ifndef ARG_DIR

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

ChangeUI all "res\wpres.exe"

!else

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "res\ppp_small_04.bmp"
!define MUI_HEADERIMAGE_RIGHT

!define MUI_WELCOMEFINISHPAGE_BITMAP "res\install_${BITMAP_SUFFIX}.bmp"
!define MUI_WELCOMEPAGE_TITLE $(TEXT_WELCOMEPAGE_TITLE)
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_WELCOMEPAGE_TEXT  $(TEXT_WELCOMEPAGE_TEXT)
!insertmacro MUI_PAGE_WELCOME

!define MUI_DIRECTORYPAGE_TEXT_TOP $(TEXT_DIRECTORYPAGE_TOP)
!define MUI_DIRECTORYPAGE_TEXT_DESTINATION $(TEXT_DIRECTORYPAGE_DESTINATION)
!insertmacro MUI_PAGE_DIRECTORY

!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_TITLE $(TEXT_FINISHPAGE_TITLE)
!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TEXT $(TEXT_FINISHPAGE_TEXT)
!define MUI_FINISHPAGE_RUN "$INSTDIR\setup.exe"
!define MUI_FINISHPAGE_RUN_TEXT $(TEXT_FINISHPAGE_RUN)
!insertmacro MUI_PAGE_FINISH

!endif

!insertmacro MUI_LANGUAGE "${ARG_LOCALIZATION}"
!include "localization\${ARG_LOCALIZATION}\webpackloc.nsh"

Name $(TEXT_PRODUCT_NAME)
Caption $(TEXT_SETUP_CAPTION)

Function .onInit
  SetShellVarContext all
  StrCpy $INSTDIR "$APPDATA\${COMPANY_SETUPFILES_PATH}\${PRODUCT_PATH}\${ARG_LOCALIZATION}"
FunctionEnd


Section "WebPackage"

  SetOutPath $INSTDIR
  SetOverwrite on
  File /r ${ARG_INDIR}\*.*

;  IfErrors 0 +3
;    MessageBox MB_ICONSTOP|MB_OK "Unable to write files into $INSTDIR. Please, check that you have sufficient disk space and write permissions to this directory."
;    Abort

!ifndef ARG_DIR
  Exec '"$INSTDIR\setup.exe"'
!endif

SectionEnd

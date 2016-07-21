!ifdef KAV
!define PRODUCT_FULLNAME_NOM "Kaspersky Anti-Virus ${VER_NAME}"
!define PRODUCT_FULLNAME_GEN "Kaspersky Anti-Virus ${VER_NAME}"

!define PRODUCT_SHRTNAME_NOM "${PRODUCT_FULLNAME_NOM}"
!define PRODUCT_SHRTNAME_GEN "${PRODUCT_FULLNAME_GEN}"

!else ifdef KIS
!define PRODUCT_FULLNAME_NOM "Kaspersky Internet Security ${VER_NAME}"
!define PRODUCT_FULLNAME_GEN "${PRODUCT_FULLNAME_NOM}"

!define PRODUCT_SHRTNAME_NOM "${PRODUCT_FULLNAME_NOM}"
!define PRODUCT_SHRTNAME_GEN "${PRODUCT_FULLNAME_GEN}"

!else ifdef WKS
!define PRODUCT_FULLNAME_NOM "Kaspersky Anti-Virus ${VER_NAME} for Windows Workstations"
!define PRODUCT_FULLNAME_GEN "Kaspersky Anti-Virus ${VER_NAME} for Windows Workstations"

!define PRODUCT_SHRTNAME_NOM "Kaspersky Anti-Virus ${VER_NAME}"
!define PRODUCT_SHRTNAME_GEN "Kaspersky Anti-Virus ${VER_NAME}"

!else ifdef SRV
!define PRODUCT_FULLNAME_NOM "Kaspersky Anti-Virus ${VER_NAME} for Windows Servers"
!define PRODUCT_FULLNAME_GEN "Kaspersky Anti-Virus ${VER_NAME} for Windows Servers"

!define PRODUCT_SHRTNAME_NOM "Kaspersky Anti-Virus ${VER_NAME}"
!define PRODUCT_SHRTNAME_GEN "Kaspersky Anti-Virus ${VER_NAME}"

!else ifdef SOS
!define PRODUCT_FULLNAME_NOM "Kaspersky Anti-Virus ${VER_NAME} SOS"
!define PRODUCT_FULLNAME_GEN "Kaspersky Anti-Virus ${VER_NAME} SOS"

!define PRODUCT_SHRTNAME_NOM "Kaspersky Anti-Virus ${VER_NAME}"
!define PRODUCT_SHRTNAME_GEN "Kaspersky Anti-Virus ${VER_NAME}"
!endif

LangString TEXT_PRODUCT_NAME  ${LANG_GERMAN} "${PRODUCT_SHRTNAME_NOM}"

LangString TEXT_SETUP_CAPTION ${LANG_GERMAN} "Installationsprogramm für ${PRODUCT_SHRTNAME_GEN}"

LangString TEXT_WELCOMEPAGE_TITLE         ${LANG_GERMAN} "Willkommen beim Extrahierprogramm der Distribution von ${PRODUCT_SHRTNAME_GEN}"
LangString TEXT_WELCOMEPAGE_TEXT          ${LANG_GERMAN} "Dieses Programm entpackt die Distribution von ${PRODUCT_FULLNAME_GEN} auf Ihren Computer.\r\n\r\nKlicken Sie auf 'Weiter', um fortzufahren."

LangString MUI_TEXT_DIRECTORY_TITLE       ${LANG_GERMAN} "Zielordner wählen"
LangString MUI_TEXT_DIRECTORY_SUBTITLE    ${LANG_GERMAN} "Wählen Sie einen Installationsordner"

LangString TEXT_DIRECTORYPAGE_TOP         ${LANG_GERMAN} "Das Programm entpackt die Distribution von ${PRODUCT_FULLNAME_GEN} in den angegebenen Ordner. Um die Distribution in einen anderen Ordner zu entpacken, klicken Sie auf 'Durchsuchen' und geben Sie den Ordner an. Klicken Sie auf 'Installieren', um die Installation zu starten."
LangString TEXT_DIRECTORYPAGE_DESTINATION ${LANG_GERMAN} "Ordner für die Distribution"

LangString ^DirBrowseText ${LANG_GERMAN} "Geben Sie den Ordner zum Entpacken von ${PRODUCT_SHRTNAME_GEN} an"

LangString MUI_TEXT_INSTALLING_TITLE      ${LANG_GERMAN} "Dateien werden kopiert"
LangString MUI_TEXT_INSTALLING_SUBTITLE   ${LANG_GERMAN} "Bitte warten Sie. Die Distributionsdateien von ${PRODUCT_FULLNAME_GEN} werden kopiert..."

LangString TEXT_FINISHPAGE_TITLE          ${LANG_GERMAN} "Entpacken der Distribution von ${PRODUCT_SHRTNAME_GEN} abgeschlossen"
LangString TEXT_FINISHPAGE_TEXT           ${LANG_GERMAN} "Das Entpacken der Distribution von ${PRODUCT_FULLNAME_GEN} wurde ausgeführt.\r\n\r\nKlicken Sie auf die Schaltfläche $\"Fertig$\", um das Programm zu beenden."
LangString TEXT_FINISHPAGE_RUN            ${LANG_GERMAN} "Installation von ${PRODUCT_SHRTNAME_GEN} starten"

;Version info
VIProductVersion "${VER_FULL}"
VIAddVersionKey /LANG=${LANG_GERMAN} "FileDescription" "Installationsprogramm für ${PRODUCT_FULLNAME_GEN}"
VIAddVersionKey /LANG=${LANG_GERMAN} "CompanyName" "Kaspersky Lab"
VIAddVersionKey /LANG=${LANG_GERMAN} "FileVersion" "${VER_FULL}"
VIAddVersionKey /LANG=${LANG_GERMAN} "LegalTrademarks" "Kaspersky Anti-Virus® ist ein eingetragenes Warenzeichen von Kaspersky Lab."
VIAddVersionKey /LANG=${LANG_GERMAN} "LegalCopyright" 'Kaspersky Lab Ltd., 1996-2007. Alle Rechte vorbehalten.'
VIAddVersionKey /LANG=${LANG_GERMAN} "ProductName" "${PRODUCT_FULLNAME_NOM}"
VIAddVersionKey /LANG=${LANG_GERMAN} "ProductVersion" "${VER_FULL}"

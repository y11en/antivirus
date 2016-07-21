!ifdef KAV
!define PRODUCT_FULLNAME "Kaspersky Anti-Virus ${VER_NAME}"
!define PRODUCT_SHRTNAME ${PRODUCT_FULLNAME}
!else ifdef KIS
!define PRODUCT_FULLNAME "Kaspersky Internet Security ${VER_NAME}"
!define PRODUCT_SHRTNAME ${PRODUCT_FULLNAME}
!else ifdef WKS
!define PRODUCT_FULLNAME "Kaspersky Anti-Virus ${VER_NAME} for Windows Workstations"
!define PRODUCT_SHRTNAME "Kaspersky Anti-Virus ${VER_NAME}"
!else ifdef SRV
!define PRODUCT_FULLNAME "Kaspersky Anti-Virus ${VER_NAME} for Windows Servers"
!define PRODUCT_SHRTNAME "Kaspersky Anti-Virus ${VER_NAME}"
!else ifdef SOS
!define PRODUCT_FULLNAME "Kaspersky Anti-Virus ${VER_NAME} SOS"
!define PRODUCT_SHRTNAME "Kaspersky Anti-Virus ${VER_NAME}"
!endif

LangString TEXT_PRODUCT_NAME   ${LANG_ENGLISH} "${PRODUCT_SHRTNAME}"

LangString TEXT_SETUP_CAPTION  ${LANG_ENGLISH} "${PRODUCT_FULLNAME} Setup"

LangString TEXT_WELCOMEPAGE_TITLE         ${LANG_ENGLISH} "Welcome to the ${PRODUCT_SHRTNAME} Setup Wizard"
LangString TEXT_WELCOMEPAGE_TEXT          ${LANG_ENGLISH} "This wizard will unpack the installation files of ${PRODUCT_FULLNAME} to your computer.\r\n\r\nClick Next to continue."

LangString MUI_TEXT_DIRECTORY_TITLE       ${LANG_ENGLISH} "Choose Unpack Location"
LangString MUI_TEXT_DIRECTORY_SUBTITLE    ${LANG_ENGLISH} "Choose the folder in which to unpack ${PRODUCT_SHRTNAME} Setup."

LangString TEXT_DIRECTORYPAGE_TOP         ${LANG_ENGLISH} "This program will unpack ${PRODUCT_FULLNAME} Setup to the following folder. To unpack it to a different folder, click Browse and select another folder. Click Install to start the unpacking."
LangString TEXT_DIRECTORYPAGE_DESTINATION ${LANG_ENGLISH} "Destination Folder"

LangString ^DirBrowseText ${LANG_ENGLISH} "Select the folder to unpack ${PRODUCT_SHRTNAME} Setup to:"

LangString MUI_TEXT_INSTALLING_TITLE      ${LANG_ENGLISH} "Unpacking"
LangString MUI_TEXT_INSTALLING_SUBTITLE   ${LANG_ENGLISH} "Please wait while ${PRODUCT_SHRTNAME} is being unpacked."

LangString TEXT_FINISHPAGE_TITLE          ${LANG_ENGLISH} "Unpacking of ${PRODUCT_SHRTNAME} Setup is complete."
LangString TEXT_FINISHPAGE_TEXT           ${LANG_ENGLISH} "Unpacking of the installation files of ${PRODUCT_FULLNAME} is finished.\r\n\r\nClick Finish to close this wizard."
LangString TEXT_FINISHPAGE_RUN            ${LANG_ENGLISH} "Run ${PRODUCT_SHRTNAME} Setup"

;Version settings
VIProductVersion "${VER_FULL}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${PRODUCT_FULLNAME} Setup"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Kaspersky Lab"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VER_FULL}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "Kaspersky™ Anti-Virus ® is a registered trademark of Kaspersky Lab."
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright © Kaspersky Lab, 1996-2007. All rights reserved."
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${PRODUCT_FULLNAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VER_FULL}"

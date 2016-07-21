!ifdef KAV
!define PRODUCT_FULLNAME_NOM "Антивирус Касперского ${VER_NAME}"
!define PRODUCT_FULLNAME_GEN "Антивируса Касперского ${VER_NAME}"

!define PRODUCT_SHRTNAME_NOM "${PRODUCT_FULLNAME_NOM}"
!define PRODUCT_SHRTNAME_GEN "${PRODUCT_FULLNAME_GEN}"

!else ifdef KIS
!define PRODUCT_FULLNAME_NOM "Kaspersky Internet Security ${VER_NAME}"
!define PRODUCT_FULLNAME_GEN "${PRODUCT_FULLNAME_NOM}"

!define PRODUCT_SHRTNAME_NOM "${PRODUCT_FULLNAME_NOM}"
!define PRODUCT_SHRTNAME_GEN "${PRODUCT_FULLNAME_GEN}"

!else ifdef WKS
!define PRODUCT_FULLNAME_NOM "Антивирус Касперского ${VER_NAME} для Windows Workstations"
!define PRODUCT_FULLNAME_GEN "Антивируса Касперского ${VER_NAME} для Windows Workstations"

!define PRODUCT_SHRTNAME_NOM "Антивирус Касперского ${VER_NAME}"
!define PRODUCT_SHRTNAME_GEN "Антивируса Касперского ${VER_NAME}"

!else ifdef SRV
!define PRODUCT_FULLNAME_NOM "Антивирус Касперского ${VER_NAME} для Windows Servers"
!define PRODUCT_FULLNAME_GEN "Антивируса Касперского ${VER_NAME} для Windows Servers"

!define PRODUCT_SHRTNAME_NOM "Антивирус Касперского ${VER_NAME}"
!define PRODUCT_SHRTNAME_GEN "Антивируса Касперского ${VER_NAME}"

!else ifdef SOS
!define PRODUCT_FULLNAME_NOM "Антивирус Касперского ${VER_NAME} SOS"
!define PRODUCT_FULLNAME_GEN "Антивируса Касперского ${VER_NAME} SOS"

!define PRODUCT_SHRTNAME_NOM "Антивирус Касперского ${VER_NAME}"
!define PRODUCT_SHRTNAME_GEN "Антивируса Касперского ${VER_NAME}"
!endif

LangString TEXT_PRODUCT_NAME  ${LANG_RUSSIAN} "${PRODUCT_SHRTNAME_NOM}"

LangString TEXT_SETUP_CAPTION ${LANG_RUSSIAN} "Программа установки ${PRODUCT_SHRTNAME_GEN}"

LangString TEXT_WELCOMEPAGE_TITLE         ${LANG_RUSSIAN} "Вас приветствует программа распаковки дистрибутива ${PRODUCT_SHRTNAME_GEN}"
LangString TEXT_WELCOMEPAGE_TEXT          ${LANG_RUSSIAN} "Эта программа распакует дистрибутив ${PRODUCT_FULLNAME_GEN} на ваш компьютер.\r\n\r\nНажмите кнопку 'Далее' для продолжения."

LangString MUI_TEXT_DIRECTORY_TITLE       ${LANG_RUSSIAN} "Выбор папки установки"
LangString MUI_TEXT_DIRECTORY_SUBTITLE    ${LANG_RUSSIAN} "Выберите папку для установки"

LangString TEXT_DIRECTORYPAGE_TOP         ${LANG_RUSSIAN} "Программа распакует дистрибутив ${PRODUCT_FULLNAME_GEN} в указанную папку. Чтобы распаковать дистрибутив в другую папку, нажмите кнопку 'Обзор' и укажите ее. Нажмите кнопку 'Установить', чтобы начать распаковку."
LangString TEXT_DIRECTORYPAGE_DESTINATION ${LANG_RUSSIAN} "Папка для дистрибутива"

LangString ^DirBrowseText ${LANG_RUSSIAN} "Укажите папку для распаковки ${PRODUCT_SHRTNAME_GEN}"

LangString MUI_TEXT_INSTALLING_TITLE      ${LANG_RUSSIAN} "Копирование файлов"
LangString MUI_TEXT_INSTALLING_SUBTITLE   ${LANG_RUSSIAN} "Подождите, идет копирование файлов дистрибутива ${PRODUCT_FULLNAME_GEN}..."

LangString TEXT_FINISHPAGE_TITLE          ${LANG_RUSSIAN} "Распаковка дистрибутива ${PRODUCT_SHRTNAME_GEN} завершена"
LangString TEXT_FINISHPAGE_TEXT           ${LANG_RUSSIAN} "Распаковка дистрибутива ${PRODUCT_FULLNAME_GEN} выполнена.\r\n\r\nНажмите кнопку $\"Готово$\" для выхода из программы."
LangString TEXT_FINISHPAGE_RUN            ${LANG_RUSSIAN} "Запустить установку ${PRODUCT_SHRTNAME_GEN}"

;Version info
VIProductVersion "${VER_FULL}"
VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileDescription" "Программа установки ${PRODUCT_FULLNAME_GEN}"
VIAddVersionKey /LANG=${LANG_RUSSIAN} "CompanyName" "Лаборатория Касперского"
VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileVersion" "${VER_FULL}"
VIAddVersionKey /LANG=${LANG_RUSSIAN} "LegalTrademarks" "Антивирус Касперского® - зарегистрированная торговая марка Лаборатории Касперского."
VIAddVersionKey /LANG=${LANG_RUSSIAN} "LegalCopyright" 'ЗАО "Лаборатория Касперского", 1996-2007. Все права защищены.'
VIAddVersionKey /LANG=${LANG_RUSSIAN} "ProductName" "${PRODUCT_FULLNAME_NOM}"
VIAddVersionKey /LANG=${LANG_RUSSIAN} "ProductVersion" "${VER_FULL}"

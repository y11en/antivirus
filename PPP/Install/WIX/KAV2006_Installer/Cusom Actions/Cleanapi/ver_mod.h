#ifndef __SETUP_VER_MOD_H__
#define __SETUP_VER_MOD_H__

// localization strings begin

#if defined(__PRODTYPE_KAV)
	#define PRODUCT_NAME_NOM	"Антивирус Касперского 7.0"
	#define PRODUCT_NAME_GEN	"Антивируса Касперского 7.0"
#elif defined(__PRODTYPE_KIS)
	#define PRODUCT_NAME_NOM	"Kaspersky Internet Security 7.0"
	#define PRODUCT_NAME_GEN	PRODUCT_NAME_NOM
#elif defined(__PRODTYPE_WKS)
	#define PRODUCT_NAME_NOM	"Антивирус Касперского 7.0 для Windows Workstations"
	#define PRODUCT_NAME_GEN	"Антивируса Касперского 7.0 для Windows Workstations"
#elif defined(__PRODTYPE_FS)
	#define PRODUCT_NAME_NOM	"Антивирус Касперского 7.0 для Windows Servers"
	#define PRODUCT_NAME_GEN	"Антивируса Касперского 7.0 для Windows Servers"
#elif defined(__PRODTYPE_SOS)
	#define PRODUCT_NAME_NOM	"Антивирус Касперского 7.0 SOS"
	#define PRODUCT_NAME_GEN	"Антивируса Касперского 7.0 SOS"
#else
	#error __PRODTYPE_XXX not defined
#endif

#define VER_FILEDESCRIPTION_STR	 "Программа установки " PRODUCT_NAME_GEN

#define VER_COMPANYNAME_STR      "Лаборатория Касперского"
#define VER_LEGALCOPYRIGHT_STR   "ЗАО ""Лаборатория Касперского"", 1996-2007. Все права защищены."
#define VER_LEGALTRADEMARKS_STR  "Антивирус Касперского® - зарегистрированный товарнй знак ""Лаборатории Касперского""."

#define VER_VERSION_UNICODE_LANG "041904B0"         //LANG_RUSSIAN/SUBLANG_DEFAULT, Unicode CP
#define VER_VERSION_TRANSLATION  0x419, 0x04B0      

// localization strings end

#define VER_ORIGINALFILENAME_STR	"SETUP.EXE"
#define VER_FILETYPE                    VFT_APP

#define __VER_KL_H
#include <version/ver_product.h>

#include <version/ver_itoa.h>

#undef  VER_PRODUCTNAME_STR
#define VER_PRODUCTNAME_STR PRODUCT_NAME_NOM

#define VER_PRODUCTVERSION_HIGH_STR        VER_ITOA( VER_PRODUCTVERSION_HIGH)
#define VER_PRODUCTVERSION_LOW_STR         VER_ITOA( VER_PRODUCTVERSION_LOW)
#define VER_PRODUCTVERSION_BUILD_STR       VER_ITOA( VER_PRODUCTVERSION_BUILD)
#define VER_PRODUCTVERSION_COMPILATION_STR VER_ITOA( VER_PRODUCTVERSION_COMPILATION)

#define VER_FILEVERSION_HIGH               VER_PRODUCTVERSION_HIGH
#define VER_FILEVERSION_LOW                VER_PRODUCTVERSION_LOW
#define VER_FILEVERSION_BUILD              VER_PRODUCTVERSION_BUILD
#define VER_FILEVERSION_COMPILATION        VER_PRODUCTVERSION_COMPILATION

#define VER_FILEVERSION_HIGH_STR           VER_ITOA( VER_FILEVERSION_HIGH)
#define VER_FILEVERSION_LOW_STR            VER_ITOA( VER_FILEVERSION_LOW)
#define VER_FILEVERSION_BUILD_STR          VER_ITOA( VER_FILEVERSION_BUILD)
#define VER_FILEVERSION_COMPILATION_STR    VER_ITOA( VER_FILEVERSION_COMPILATION)

#define VER_PRODUCTVERSION       VER_PRODUCTVERSION_HIGH,VER_PRODUCTVERSION_LOW,VER_PRODUCTVERSION_BUILD,VER_PRODUCTVERSION_COMPILATION
#define VER_PRODUCTVERSION_STR   VER_PRODUCTVERSION_HIGH_STR "." VER_PRODUCTVERSION_LOW_STR "." VER_PRODUCTVERSION_BUILD_STR "." VER_PRODUCTVERSION_COMPILATION_STR

#define VER_FILEVERSION          VER_FILEVERSION_HIGH,VER_FILEVERSION_LOW,VER_FILEVERSION_BUILD,VER_FILEVERSION_COMPILATION
#define VER_FILEVERSION_STR      VER_FILEVERSION_HIGH_STR "." VER_FILEVERSION_LOW_STR "." VER_FILEVERSION_BUILD_STR "." VER_FILEVERSION_COMPILATION_STR

#ifdef RC_INVOKED
#include <version/ver_kl.ver>
#endif

#endif

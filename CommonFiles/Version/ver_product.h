// This file was auto generated with O:\PPP\Install\_AVP_BUILD.MAK
//  
#ifndef __VER_PRODUCT_H
#define __VER_PRODUCT_H
#define VER_PRODUCTNAME_STR                 "Kaspersky Anti-Virus"
#define VER_PRODUCTVERSION_HIGH             8
#define VER_PRODUCTVERSION_LOW              0
#define VER_PRODUCTVERSION_BUILD            0
#define VER_PRODUCTVERSION_COMPILATION      167

// -------------------------------------------------------------------------------------------------
#define VER_COMPANY_REGISTRY_PATH			"Software\\KasperskyLab"
#define VER_PRODUCT_REGISTRY_ROOT			VER_COMPANY_REGISTRY_PATH "\\protected"
#define VER_PRODUCT_REGISTRY_NAME			"AVP8"
#define VER_PRODUCT_REGISTRY_PATH			VER_PRODUCT_REGISTRY_ROOT "\\" VER_PRODUCT_REGISTRY_NAME

#define VER_PRODUCT_APPDATA_NAME            "AVP8"
// -------------------------------------------------------------------------------------------------
#define VER_COMPANY_REGISTRY_PATH_W			L"Software\\KasperskyLab"
#define VER_PRODUCT_REGISTRY_ROOT_W			VER_COMPANY_REGISTRY_PATH_W L"\\protected"
#define VER_PRODUCT_REGISTRY_NAME_W			L"AVP8"
#define VER_PRODUCT_REGISTRY_PATH_W			VER_PRODUCT_REGISTRY_ROOT_W L"\\" VER_PRODUCT_REGISTRY_NAME_W

#define VER_PRODUCT_APPDATA_NAME_W          L"AVP8"

#define VER_PRODUCT_REGISTRY_PROT_W			L"KASPERSKYLAB\\PROTECTED"
// -------------------------------------------------------------------------------------------------

#define VER_PRODUCT_REGISTRY_AK_FS			"KAVFS8\\8.0.0.0"
#define VER_PRODUCT_REGISTRY_AK_WKS			"KAVWKS8\\8.0.0.0"
#define VER_PRODUCT_REGISTRY_AK_SOS			"KAVSOS8\\8.0.0.0"

#define VER_PRODUCT_STATE_STR               "Beta"
#define VER_PRODUCT_STATE_SUFFIX_STR        " Beta"
#define VER_PRODUCT_WMIH_ID                 _g_ver_WmiHlpProdGuid
#define VER_PRODUCT_WMIH_LEGACYREGKEY       "KasperskyAntiVirus"

const unsigned char _g_ver_WmiHlpProdGuid[16] = {0xC6, 0x4B, 0x4D, 0x2C, 0x93, 0x07, 0x56, 0x49, 0xA9, 0xF9, 0xE2, 0x52, 0x43, 0x54, 0x69, 0xC0};

#include "ver_kl.h"
#endif//__VER_PRODUCT_H

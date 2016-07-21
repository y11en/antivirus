/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	st_prefix.h
 * \author	Andrew Kazachkov
 * \date	29.10.2003 15:48:31
 * \brief	
 * 
 */
#ifndef __KL_ST_PREFIX_H__
#define __KL_ST_PREFIX_H__

#include "std/base/kldefs.h"
#include <string>

#if defined(UNICODE) || defined(_UNICODE)
#define KLSTD_StParseCommandine KLSTD_StParseCommandineW
#define KLSTD_StGetPrefix       KLSTD_StGetPrefixW
#define KLSTD_StGetDefDir       KLSTD_StGetDefDirW
#define KLSTD_StGetRootKey      KLSTD_StGetRootKeyW
#define KLSTD_StGetProductKey   KLSTD_StGetProductKeyW
#define KLSTD_StGetCmdlnPostfix KLSTD_StGetCmdlnPostfixW
#else
#define KLSTD_StParseCommandine KLSTD_StParseCommandineA
#define KLSTD_StGetPrefix       KLSTD_StGetPrefixA
#define KLSTD_StGetDefDir       KLSTD_StGetDefDirA
#define KLSTD_StGetRootKey      KLSTD_StGetRootKeyA
#define KLSTD_StGetProductKey   KLSTD_StGetProductKeyA
#define KLSTD_StGetCmdlnPostfix KLSTD_StGetCmdlnPostfixA
#endif

KLCSC_DECL void KLSTD_StParseCommandineW(wchar_t** argv);
KLCSC_DECL void KLSTD_StParseCommandineA(char** argv);
KLCSC_DECL std::wstring KLSTD_StGetPrefixW();
KLCSC_DECL std::string KLSTD_StGetPrefixA();
KLCSC_DECL std::wstring KLSTD_StGetDefDirW();
KLCSC_DECL std::string KLSTD_StGetDefDirA();
KLCSC_DECL std::wstring KLSTD_StGetCmdlnPostfixW();
KLCSC_DECL std::string KLSTD_StGetCmdlnPostfixA();

#ifdef _WIN32
KLCSC_DECL std::wstring KLSTD_StGetRootKeyW();
KLCSC_DECL std::string KLSTD_StGetRootKeyA();
KLCSC_DECL std::wstring KLSTD_StGetProductKeyW(const wchar_t* product, const wchar_t* version);
KLCSC_DECL std::string KLSTD_StGetProductKeyA(const wchar_t* product, const wchar_t* version);
#endif

KLCSC_DECL void StCallRegProduct(bool bForce);

#endif //__KL_ST_PREFIX_H__
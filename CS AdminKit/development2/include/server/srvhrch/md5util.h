/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	md5util.h
 * \author	Andrew Lashenkov
 * \date	19.01.2005 16:19:06
 * \brief	
 * 
 */

#ifndef __KLMD5UTIL_H__
#define __KLMD5UTIL_H__

#include <std/base/klstd.h>

namespace KLSTD
{
    KLCSSRV_DECL std::wstring GetMD5HashW(const void* pData, size_t nData);
    KLCSSRV_DECL std::string GetMD5HashA(const void* pData, size_t nData);
}

#endif //__KLMD5UTIL_H__

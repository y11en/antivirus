/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	bin2hex.h
 * \author	Andrew Kazachkov
 * \date	04.11.2004 12:33:06
 * \brief	
 * 
 */

#ifndef __KLBIN2HEX_H__
#define __KLBIN2HEX_H__

#include <string>
#include <vector>
#include <avp/avp_data.h>

namespace KLSTD
{
    std::string     MakeHexDataA(const void* pData, size_t nData);

    std::wstring    MakeHexDataW(const void* pData, size_t nData);

    bool GetBinFromHexA(const char* pData, size_t nData, std::vector<AVP_byte>& vecData);

    bool GetBinFromHexW(const wchar_t* pData, size_t nData, std::vector<AVP_byte>& vecData);
};

#endif //__KLBIN2HEX_H__

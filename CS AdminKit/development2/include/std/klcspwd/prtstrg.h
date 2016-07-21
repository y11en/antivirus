/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prtstrg.h
 * \author	Andrew Kazachkov
 * \date	29.10.2004 10:48:37
 * \brief	
 * 
 */

#include "./klcspwd_common.h"

#ifndef __KLPRTSTRG_H__
#define __KLPRTSTRG_H__

namespace KLCSPWD
{
    const char c_szLocalPubKey[]    = "LOC-PUB-6EEB50F8D2EB46029DB4CCB77E0DA651";
    const char c_szLocalPrvKey[]    = "LOC-PRV-6EEB50F8D2EB46029DB4CCB77E0DA651";
    const char c_szGlobalPubKey[]   = "GLB-PUB-6EEB50F8D2EB46029DB4CCB77E0DA651";
    const char c_szGlobalPrvKey[]   = "GLB-PRV-6EEB50F8D2EB46029DB4CCB77E0DA651";
    const char c_szServerPubKey[]   = "SRVR-PUB-6EEB50F8D2EB46029DB4CCB77E0DA651";
    const char c_szServerPrvKey[]   = "SRVR-PRV-6EEB50F8D2EB46029DB4CCB77E0DA651";
    
    KLCSPWD_DECL int PutData(const char* szKey, const void* pData, size_t nData);
    KLCSPWD_DECL int GetData(const char* szKey, void*& pData, size_t& nData);
};

#endif //__KLPRTSTRG_H__

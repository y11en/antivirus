/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	cert_load.h
 * \author	Andrew Kazachkov
 * \date	08.01.2004 10:45:53
 * \brief	
 * 
 */

#ifndef __KLPAR_CERT_LOAD_H__
#define __KLPAR_CERT_LOAD_H__

#include <vector>
#include <algorithm>
#include <std/par/par_conv.h>

namespace KLPAR
{
    KLSTD::CAutoPtr<KLPAR::BinaryValue> LoadCert(
                                            KLPAR::Params*  pSection,
                                            const wchar_t*  szwName,
                                            const wchar_t*  szwDefValue,
                                            bool            bTrowException);

    //returns true if fills pData
    bool StoreCert(
                    KLPAR::Params*                  pSection,
                    KLPAR::BinaryValue*             pCert,                    
                    const wchar_t*                  szwName,
                    const wchar_t*                  szwDefPathValue,
                    KLSTD::CAutoPtr<KLPAR::Params>& pData);

};

#endif //__KLPAR_CERT_LOAD_H__

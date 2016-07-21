/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	strg_unix.cpp
 * \author	Andrew Kazachkov
 * \date	29.10.2004 10:47:35
 * \brief	
 * 
 */

#include "std/base/klstd.h"
#include <std/thr/locks.h>
#include "std/klcspwd/klcspwd_common.h"

#ifdef __unix

#include "./conv.h"
#include "std/klcspwd/prtstrg.h"
#include "std/klcspwd/strg_unix.h"

#include "avp/parray.h"
#include "avp/text2bin.h"

#define KLCS_MODULENAME L"KLCSPWD"

namespace KLCSPWD
{
    extern KLCSPWD::ProtectedStorage* g_pProtectedStorage;
    extern KLSTD::ObjectLock*       g_plckPwdStg;

    void SetStorageCallbacks(ProtectedStorage* pStorage)
    {
    KLERR_BEGIN
        if(g_plckPwdStg)
        {
            g_plckPwdStg->Disallow();
            g_plckPwdStg->Wait();
            g_pProtectedStorage = pStorage;
            g_plckPwdStg->Allow();
        };
    KLERR_ENDT(1)
    };

    int PutData(const char* szKey, const void* pData, size_t nData)
    {
        int nResult = -1;
        if(g_plckPwdStg)
        {
            KLSTD::CAutoObjectLock aol(*g_plckPwdStg);
            nResult = g_pProtectedStorage->PutData(szKey, pData, nData);
        };
        return nResult;
    };

    int GetData(const char* szKey, void*& pData, size_t& nData)
    {
        int nResult = -1;
        if(g_plckPwdStg)
        {
            KLSTD::CAutoObjectLock aol(*g_plckPwdStg);
            nResult = g_pProtectedStorage->GetData(szKey, pData, nData);
        };
        return nResult;
    };
};
#endif

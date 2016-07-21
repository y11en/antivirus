/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	strg_unix.h
 * \author	Andrew Kazachkov
 * \date	20.10.2006 11:41:23
 * \brief	
 * 
 */

#include "std/base/klstd.h"

#ifdef __unix

namespace KLCSPWD
{
    class KLSTD_NOVTABLE ProtectedStorage
    {
    public:
        virtual int PutData(const char* szKey, const void* pData, size_t nData) = 0;
        virtual int GetData(const char* szKey, void*& pData, size_t& nData) = 0;
    };

    void SetStorageCallbacks(ProtectedStorage* pStorage);
};
#endif

/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klcspwd.cpp
 * \author	Andrew Kazachkov
 * \date	27.10.2004 16:05:15
 * \brief	
 * 
 */

#include <stdlib.h>
#ifndef N_PLAT_NLM
 #include <memory.h>
#endif

#include "std/klcspwd/klcspwd.h"
#include "std/klcspwd/prtstrg.h"
#include "std/klcspwd/crypto.h"
#include "std/klcspwd/klcspwdr.h"

namespace KLCSPWD
{    
    int EncryptData(
                    const void* pPublicKey,
                    size_t      nPublicKey,
                    const void* pPrefix,
                    size_t      nPrefix,
                    const void* pPlainText,
                    size_t      nPlainText,
                    void*&      pCipherText,
                    size_t&     nCipherText)
	{
		return (-1);
	}

    int DecryptData(
                    const void* pPrivateKey,
                    size_t      nPrivateKey,
                    size_t      nPrefix,
                    const void* pCipherText,
                    size_t      nCipherText,
                    void*&      pPlainText,
                    size_t&     nPlainText)
	{
		return (-1);
	}

	void Clean(void* pData, size_t nData)
	{
	}
    
    void Free(void* pData, size_t nData)
	{
	}

	int GetData(const char* szKey, void*& pData, size_t& nData)
    {
		return (-1);
	}

	int PutData(const char* szKey, const void* pData, size_t nData)
    {
		return (-1);
	}
};

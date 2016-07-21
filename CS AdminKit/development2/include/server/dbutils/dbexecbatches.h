/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	dbexecbatches.h
 * \author	Mikhail Karmazine & Andrew Kazachkov
 * \date	08.09.2005 17:32:20
 * \brief	
 * 
 */

#ifndef __KLDBEXECBATCHES_H__
#define __KLDBEXECBATCHES_H__

#include <string>
#include <vector>

namespace KLDBUT
{
    class ExecBatchesControl
    {
    public:
        virtual void ExecuteBatch(const wchar_t* szwCommand) = 0;
        virtual void SetPercent(size_t nPercent) = 0;
    };
    
    void ExecBatches(
        const std::vector<std::string>& vectBatches, 
        ExecBatchesControl*             pControl,
        size_t&                         nBatchesProcessed);

    void SplitScript(
                const std::string&          str,
                std::vector<std::string>&   vectValues);
    
    void GetResourceData(
                    void*           hModule,
                    unsigned        uID,
                    const wchar_t*  szwType,
                    void*&          pData,
                    size_t&         nData);

    void LoadTextFromResource(
                    void*           hModule,
                    unsigned        uID,
                    std::string&    strText,
                    const wchar_t*  szwType);
};

#endif //__KLDBEXECBATCHES_H__

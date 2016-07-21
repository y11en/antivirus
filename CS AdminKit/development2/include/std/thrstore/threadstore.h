/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	threadstore.h
 * \author	Mikhail Karmazine
 * \date	12.03.2003 16:06:45
 * \brief	средство для ассоциации объектов с потоком
 * 
 */

#ifndef __KLSTD_THREADSTORE_H__
#define __KLSTD_THREADSTORE_H__

#include <string>
#include "std/base/klbase.h"

namespace KLSTD
{
    class KLSTD_NOVTABLE ThreadStore : public KLSTD::KLBase
    {
    public:
        virtual ~ThreadStore () {};

        virtual void AddObject( const std::wstring & wstrObjName, KLSTD::KLBase* pObj ) = 0;

        //virtual void GetObject( const std::wstring & wstrObjName, KLSTD::KLBase** ppObj ) = 0;
        virtual void GetStoredObject( const std::wstring & wstrObjName, KLSTD::KLBase** ppObj ) = 0;

        virtual void RemoveObject( const std::wstring & wstrObjName, KLSTD::KLBase** ppObj = NULL ) = 0;

        virtual bool IsObjectHere( const std::wstring & wstrObjName ) = 0;

        virtual void CleanupThreadObjects() = 0;
    };

    class ThreadObjectsAutoCleaner
    {
    public:
        ThreadObjectsAutoCleaner( CAutoPtr<ThreadStore> pThreadStore )
            : m_pThreadStore(pThreadStore)
        {
        }

        virtual ~ThreadObjectsAutoCleaner()
        {
            if( m_pThreadStore != NULL )
            {
                m_pThreadStore->CleanupThreadObjects();
            }
        }

    private:
        CAutoPtr<ThreadStore> m_pThreadStore;
    };
}

void KLSTD_InitializeGlobalThreadStore();
void KLSTD_DeinitializeGlobalThreadStore();

KLCSC_DECL KLSTD::CAutoPtr<KLSTD::ThreadStore> KLSTD_GetGlobalThreadStore();

#endif //__KLSTD_THREADSTORE_H__

// Local Variables:
// mode: C++
// End:

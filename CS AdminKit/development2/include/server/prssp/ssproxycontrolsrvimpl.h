/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prssp/ssproxycontrolsrvimpl.h
 * \author	Mikhail Karmazine
 * \date	15.04.2003 16:06:40
 * \brief	Реализация интерфейса SSProxyControlSrv
 * 
 */

#ifndef __PRSSP_SSPROXYCONTROLSRVIMPL_H__
#define __PRSSP_SSPROXYCONTROLSRVIMPL_H__

#include "ssproxycontrolsrv.h"

#include <std/tmstg/timeoutstore.h>

namespace KLSRV
{
    class SSProxyControlSrvImpl
        :   public KLSTD::KLBaseImpl<KLPRSSP::SSProxyControlSrv>
        ,   public KLSTD::ModuleClass
    {
    public:
        SSProxyControlSrvImpl();

        ~SSProxyControlSrvImpl();

        void Initialize();

        void AddSSHolder(
            KLPRSSP::SSHolder * pSSHolder,
            long nTimeout,
            KLSTD::KLBase * pObjToKeepAlive );

        KLSTD::CAutoPtr<KLPRSSP::SSHolder>
            GetSSHolder( const std::wstring & wstrSSId );

        void AddCallback(
            const std::wstring & wstrSSId,
            KLPRSSP::OperationCallbacks* operationCallbacks );

        void ApplyChanges( const KLPRSSP::SSHolder * pSSHolder );

        void ReleaseSSProxy( const KLPRSSP::SSHolder * pSSHolder );

        void BeforeSSWrite(
            const KLPRSSP::SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            KLPAR::ParamsPtr& pData,
            int nOperationType );

        void AfterSSWrite(
            const KLPRSSP::SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            KLPAR::Params* pData,
            KLPAR::Params* pResult,
            int nOperationType );

        void BeforeSSSectionOperation(
            const KLPRSSP::SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            int nOperationType );

        void AfterSSSectionOperation(
            const KLPRSSP::SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            int nOperationType );
        
        virtual void AfterSSRead(
            const KLPRSSP::SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            KLPAR::ParamsPtr& pData);
    private:
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
    };
}

#endif //__PRSSP_SSPROXYCONTROLSRVIMPL_H__
/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ssproxycontrolsrv.h
 * \author	Mikhail Karmazine
 * \date	15.04.2003 12:48:44
 * \brief	Интерфейс для получения интерфейса SettingStorage для модуля 
                PRSSP в KLCSS
 * 
 */

#ifndef __PRSSP_CONTROLSRV_H__
#define __PRSSP_CONTROLSRV_H__

#include <std/base/klstd.h>
#include <kca/prss/settingsstorage.h>
#include <srvp/prssp/common.h>

namespace KLPRSSP
{
    class SSHolder : public KLSTD::KLBase
    {
    public:
        std::wstring wstrSSId;
        virtual KLSTD::CAutoPtr<KLPRSS::SettingsStorage> GetSettingsStorage() = 0;
        virtual long GetGroupId() const = 0;
        virtual long GetPolicyId() const = 0;
    };

    class OperationCallbacks
    {
    public:
        virtual void OnBeforeSSWrite(
            const SSHolder * pSSHolder, 
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            KLPAR::ParamsPtr &pData,
            int nOperationType )
        {
        };

        virtual void OnAfterSSWrite(
            const SSHolder * pSSHolder, 
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            KLPAR::Params* pData,
            KLPAR::Params* pResult,
            int nOperationType )
        {
        };

        virtual void OnBeforeSSSectionOperation(
            const SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            int nOperationType )
        {
        };

        virtual void OnAfterSSSectionOperation(
            const SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            int nOperationType )
        {
        };

        virtual void OnApplyChanges( const SSHolder * pSSHolder ){};

        virtual void OnBeforeRemoveSS( const SSHolder * pSSHolder ) {};

        virtual void OnAfterSSRead(
            const SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            KLPAR::ParamsPtr &pData){};
    };


    class SSProxyControlSrv : public KLSTD::KLBase
    {
    public:
        virtual void AddSSHolder(
            SSHolder * pSSHolder,
            long nTimeout,
            KLSTD::KLBase * pObjToKeepAlive ) = 0;

        virtual KLSTD::CAutoPtr<SSHolder> GetSSHolder( const std::wstring & wstrSSId ) = 0;

        virtual void AddCallback(
            const std::wstring & wstrSSId,
            OperationCallbacks* operationCallbacks ) = 0;

        virtual void ApplyChanges( const SSHolder * pSSHolder ) = 0;

        virtual void ReleaseSSProxy( const SSHolder * pSSHolder ) = 0;

        virtual void BeforeSSWrite(
            const SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            KLPAR::ParamsPtr &pData,
            int nOperationType ) = 0;

        virtual void AfterSSWrite(
            const SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            KLPAR::Params* pData,
            KLPAR::Params* pResult,
            int nOperationType ) = 0;

        virtual void BeforeSSSectionOperation(
            const SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            int nOperationType ) = 0;

        virtual void AfterSSSectionOperation(
            const SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            int nOperationType ) = 0;

        virtual void AfterSSRead(
            const SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            KLPAR::ParamsPtr &pData) = 0;

    };
}

#endif //__PRSSP_CONTROLSRV_H__
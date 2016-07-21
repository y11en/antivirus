/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	smart_rw.cpp
 * \author	Andrew Kazachkov
 * \date	29.12.2003 17:54:00
 * \brief	
 * 
 */
#include "std/base/klbase.h"
#include "std/err/klerrors.h"
#include "kca/prss/settingsstorage.h"
#include "./ss_smartwrite.h"
#include "kca/prss/helpersi.h"

#define KLCS_MODULENAME L"KLPRSS"

namespace KLPRSS
{
    void AdjustTaskParams(
                        const std::wstring&         wstrType,
                        const std::wstring&         wstrProductName,
                        const std::wstring&         wstrProductVersion,
                        const std::wstring&         wstrSsLocation,
                        KLPRSS::SettingsStorage*    pSsProxy,
                        KLPAR::Params*              pTaskParams,
                        KLPAR::Params**             ppResult,
                        long                        lTimeout)
    {
        KLSTD::CAutoPtr<KLPAR::Params> pResult;
        bool bWasHandled=false;
        KLERR_BEGIN
            KLSTD::CAutoPtr<KLPAR::Params> pRoot;
            KLPAR::param_entry_t pars[]=
            {
                KLPAR::param_entry_t(wstrType.c_str(), pTaskParams)
            };
            KLPAR::CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pRoot);
            KLSTD::CAutoPtr<SettingsStorage> pSS;
            const std::wstring& wstrLocation=wstrSsLocation.empty()
                                ?   KLPRSS_GetSettingsStorageLocation()
                                :   wstrSsLocation;
            if(pSsProxy)
                pSsProxy->CreateSettingsStorage(
                                wstrLocation,
                                KLSTD::CF_OPEN_EXISTING,
                                KLSTD::AF_READ,
                                &pSS);
            else
                KLPRSS_CreateSettingsStorageDirect(
                                wstrLocation,
                                KLSTD::CF_OPEN_EXISTING,
                                KLSTD::AF_READ,
                                &pSS);
            pSS->SetTimeout(lTimeout);
            pSS->Read2(
                wstrProductName,
                wstrProductVersion,
                KLPRSS_TASKS_POLICY,
                pRoot,
                &pResult);
            pResult=KLPAR::GetParamsValue(pResult, wstrType);
            bWasHandled=true;
        KLERR_END
        if(!bWasHandled)
            pResult=pTaskParams;
        pResult.CopyTo(ppResult);
    }

    void SmartReadTaskParams(
                        const std::wstring& wstrType,
                        const std::wstring& wstrProductName,
                        const std::wstring& wstrProductVersion,
                        KLPAR::Params*      pTaskParams,
                        KLPAR::Params**     ppResult,
                        long                lTimeout)
    {
        KLSTD_CHKINPTR(pTaskParams);
        KLSTD_CHKOUTPTR(ppResult);
        KLSTD_CHK(wstrType, !wstrType.empty());
        KLSTD_CHK(wstrProductName, !wstrProductName.empty());
        KLSTD_CHK(wstrProductVersion, !wstrProductVersion.empty());
        AdjustTaskParams(
                            wstrType,
                            wstrProductName,
                            wstrProductVersion,
                            L"",
                            NULL,
                            pTaskParams,
                            ppResult,
                            lTimeout);
    }

    void SmartWriteTaskParams(
                            const std::wstring&         wstrProductName,
                            const std::wstring&         wstrProductVersion,
                            const std::wstring&         wstrType,
                            const std::wstring&         wstrTsProduct,
                            const std::wstring&         wstrTsVersion,
                            const std::wstring&         wstrTsSection,
                            KLPRSS::SettingsStorage*    pTs,
                            KLPAR::Params*              pTaskParams,
                            long                        lTimeout)
    {
        KLSTD_CHKINPTR(pTs);
        KLSTD_CHKINPTR(pTaskParams);
        KLSTD_CHK(wstrType, !wstrType.empty());
        KLSTD_CHK(wstrProductName, !wstrProductName.empty());
        KLSTD_CHK(wstrProductVersion, !wstrProductVersion.empty());
        KLSTD_CHK(wstrTsProduct, !wstrTsProduct.empty());
        KLSTD_CHK(wstrTsVersion, !wstrTsVersion.empty());
        KLSTD_CHK(wstrTsSection, !wstrTsSection.empty());
        ;
        KLSTD::CAutoPtr<KLPRSS::SsExtendedWrite> pTsEx;
        pTs->QueryInterface(KLSTD_IIDOF(KLPRSS::SsExtendedWrite), (void**)&pTsEx);
        KLSTD_CHK(pTs, pTsEx != NULL);
        ;
        KLSTD::CAutoPtr<KLPAR::Params> pPolicy;
        //reading policy
        KLERR_BEGIN
            const std::wstring& wstrPolicyLocation=KLPRSS_GetPolicyLocation(
                                                    wstrProductName,
                                                    wstrProductVersion);
            KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSsPolicy;
            KLPRSS_CreateSettingsStorageDirect(
                                wstrPolicyLocation,
                                KLSTD::CF_OPEN_EXISTING,
                                KLSTD::AF_READ,
                                &pSsPolicy);
            pSsPolicy->SetTimeout(lTimeout);
            KLSTD::CAutoPtr<KLPAR::Params> pPolicySection;
            pSsPolicy->Read(
                            wstrProductName,
                            wstrProductVersion,
                            KLPRSS_TASKS_POLICY,
                            &pPolicySection);

            const wchar_t* path[]=
            {
                KLPRSS::c_szwSPS_Value,
                NULL
            };
            KLSTD::CAutoPtr<KLPAR::ParamsValue> pParamsValue;
            KLPAR_GetValue(pPolicySection, path, wstrType, (KLPAR::Value**)&pParamsValue);
            if(pParamsValue && pParamsValue->GetType() == KLPAR::Value::PARAMS_T)
                pPolicy = pParamsValue->GetValue();
        KLERR_END;                
        pTsEx->WriteSectionUsingPolicy(
                                    wstrTsProduct,
                                    wstrTsVersion,
                                    wstrTsSection,
                                    KLSTD::CF_OPEN_ALWAYS,
                                    pTaskParams,
                                    pPolicy);
    };
};

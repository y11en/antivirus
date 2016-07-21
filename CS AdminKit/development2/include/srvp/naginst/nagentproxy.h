/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	NagentProxy.h
 * \author	Andrew Kazachkov
 * \date	22.04.2003 9:21:54
 * \brief	Прокси сетевого агента
 * 
 */


#if !defined(AFX_NAGENTPROXY_H__F1470A1F_59CB_4CAC_AF3F_FF3177C8EBEE__INCLUDED_)
#define AFX_NAGENTPROXY_H__F1470A1F_59CB_4CAC_AF3F_FF3177C8EBEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <kca/prci/cmp_helpers.h>
#include <kca/prcp/proxybase.h>

namespace KLNAG
{

    class NagentProxy : public KLPRCP::ComponentProxy
    {
    public:
        virtual void GetRunTimeInfo(
                        KLPAR::Params* pFilter,
                        KLPAR::Params** ppInfo) = 0;
        
        virtual void GetStaticInfo(
                        KLPAR::Params* pFilter,
                        KLPAR::Params** ppInfo) = 0;

        virtual void GetStatistics(
                        KLPAR::Params* pFilter,
                        KLPAR::Params** ppInfo) = 0;

        virtual void GetInstancePort(
                        const std::wstring& wstrProduct,
                        const std::wstring& wstrVersion,
                        const std::wstring& wstrComponent,
                        const std::wstring& wstrInstance,
                        long&               nPort,
                        std::wstring&       wstrLocation,
                        KLPRCI::InstanceListenerType*   pConnType=NULL) = 0;
        
        virtual bool StartProduct(
                        const std::wstring& wstrProductName,
                        const std::wstring& wstrProductVersion,
                        long                lWaitTimeout,
                        volatile bool*      bCancelWait=NULL) = 0;
        
        virtual bool StopProduct(
                    const std::wstring&     wstrProductName,
                    const std::wstring&     wstrProductVersion,
                    long                    lWaitTimeout,
                    volatile bool*          bCancelWait=NULL) = 0;

        virtual void NotifySSChange(
                        const wchar_t*  szwProduct,
					    const wchar_t*  szwVersion,
                        const wchar_t*  szwSection,
                        long            lSsType) = 0;

        virtual void RemoveStores(
                        const wchar_t**         pStores,
                        size_t                  nStores,
                        std::vector<size_t>&    vecUnremoved) = 0;

        virtual void StartNewIntegration(const wchar_t* szwNewHostName) = 0;
        
        virtual void GetHostPublicKey(
                                KLSTD::MemoryChunk**    ppOpenKey,
                                std::wstring&           wstrHash) = 0;

        virtual void PutGlobalKeys(
                        const void* pKey1,
                        size_t      nKey1,
                        const void* pKey2,
                        size_t      nKey2) = 0;
        
        virtual void OnForceSync() = 0;
    };
};

void KLCSSRVP_DECL KLNAG_CreateNagentProxy(
						const wchar_t*              szwAddress,
                        const std::wstring&         wstrID,
						KLNAG::NagentProxy**        ppProxy,
						int*                        pPorts=NULL,
                        const KLPRCP::conn_attr_t*  pExtraAttributes = NULL);

void KLCSSRVP_DECL KLNAG_CreateNagentProxyForConnection(
						const std::wstring&     wstrLocalComponentName,
						const std::wstring&     wstrConnName,
						KLNAG::NagentProxy**	ppProxy);

void KLCSSRVP_DECL KLNAG_CreateGatewayedNagentProxy(
                        KLPRCP::gateway_locations_t&    vecLocations,
                        long                            lTimeout,
                        bool                            bSSL,                        
                        KLNAG::NagentProxy**	        ppProxy,
                        const KLPRCP::conn_attr_t*      pExtraAttributes = NULL);


#endif // !defined(AFX_NAGENTPROXY_H__F1470A1F_59CB_4CAC_AF3F_FF3177C8EBEE__INCLUDED_)

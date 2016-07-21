/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	proxybase.cpp
 * \author	Andrew Kazachkov
 * \date	24.02.2003 17:55:47
 * \brief	
 * 
 */

#include <build/general.h>
#include "soapH.h"

#include "std/base/klbase.h"
#include "std/base/kldefs.h"
#include "std/err/error.h"
#include "std/err/error_localized.h"
#include "std/err/errloc_intervals.h"
#include "std/par/paramsi.h"
#include "std/sign/chksgn.h"
#include "kca/prss/settingsstorage.h"
#include "transport/tr/transportproxy.h"
#include "transport/tr/transport.h"
#include "transport/ev/common.h"
#include "transport/ev/eventsource.h"
#include "transport/tr/errors.h"
#include "kca/prcp/internal.h"
#include "kca/prci/cmp_helpers.h"
#include "transport/wat/authserver.h"

#include "kca/prcp/proxybase.h"

#include "common/measurer.h"

#include "std/tmstg/timeoutstore.h"
#include <kca/prcp/errlocids.h>

#ifdef _WIN32
    #include <atlbase.h>
    #include <avpregid.h>
    #pragma comment(lib, "Ws2_32.lib")
#endif

using namespace std;
using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRSS;
using namespace KLTRAP;

#define KLCS_MODULENAME L"KLPRCP"

std::wstring KLPRCP_MakeAddress2(const std::wstring& wstrHost, int nPort)
{
    std::wstring wstrResult;
    wstrResult.reserve(wcslen(KLTRAP::c_TRLocationPrefic) + wstrHost.size() + (nPort?16:0));
    wstrResult+=KLTRAP::c_TRLocationPrefic;
    wstrResult+=wstrHost;
    if(nPort)
    {
        wstrResult+=L":";
        wchar_t szwBuffer[32]=L"";
        wstrResult+=_ltow(nPort, szwBuffer, 10);//_ltow is defined in kldefs.h
    }
    return wstrResult;
};

std::wstring KLPRCP_MakeConnectionAddress(int nPort, bool bLocalOnly)
{
    if(bLocalOnly)
        return KLPRCP_MakeAddress2(L"127.0.0.1", nPort);
    else
        return KLPRCP_MakeAddress2(L"localhost", nPort);
};

std::wstring KLPRCP_MakeAddress(int nPort, bool bLocalOnly)
{
    std::wstring wstrResult;
    if(bLocalOnly)
    {
        wstrResult.reserve(wcslen(KLTRAP::c_TRLocalLocationPrefic)+(nPort?16:0));        
#ifdef KLSTD_USE_HOSTNAME
        std::wstring wstrTmpHostName;
        KLSTD_GetHostAndDomainName(wstrTmpHostName, NULL, NULL);
        wstrResult+=KLTRAP::c_TRLocationPrefic;
        wstrResult+=wstrTmpHostName;
#else
        wstrResult+=KLTRAP::c_TRLocalLocationPrefic;
#endif
    }
    else
    {
        wstrResult.reserve(
                        wcslen(KLTRAP::c_TRLocationPrefic) +
                        wcslen(KLTRAP::c_AnyAddrLocation) + (nPort?16:0) );    

        wstrResult+=KLTRAP::c_TRLocationPrefic;
#ifdef KLSTD_USE_HOSTNAME
        std::wstring wstrTmpHostName;
        KLSTD_GetHostAndDomainName(wstrTmpHostName, NULL, NULL);
        wstrResult+=wstrTmpHostName;
#else
        wstrResult+=KLTRAP::c_AnyAddrLocation;
#endif
    };

    if (nPort) {
#ifdef _WIN32
        wstrResult+=L":";
        wchar_t szwBuffer[32]=L"";
        wstrResult+=_ltow(nPort, szwBuffer, 10);
#else
        wstringstream ws;
        ws << ":" << nPort;
        wstrResult += ws.str();
#endif
    }
    return wstrResult;
};

#define KLPRCP_PB_BEGIN(_addr) \
                std::wstring wstrAddress(_addr);   \
                KLERR_TRY

#define KLPRCP_PB_END() \
                KLERR_CATCH(pError) \
                    LocalizeCreateConnectionError(pError, wstrAddress.c_str()); \
                    KLERR_RETHROW();    \
                KLERR_ENDTRY

namespace KLPRCP
{
    static long g_lProxyId = 0;
    static long g_cfProxies = 0;
    
    static void LocalizeCreateConnectionError(KLERR::Error* pError, const wchar_t* szwAddress);

	CProxyBase::CProxyBase()
			:	KLTRAP::TransportProxy()
			,	m_bDestroyConnection(false)
            ,   m_lProxyId(KLSTD_InterlockedIncrement(&g_lProxyId))
            ,   m_bRemoveAsync(false)
	{
		KLSTD_InterlockedIncrement(&g_cfProxies);
	};

	CProxyBase::~CProxyBase()
	{
		ClearConnections();
        KLSTD_InterlockedDecrement(&g_cfProxies);
	}

	void CProxyBase::Initialize(
					const std::wstring&	wstrLocalComponentName,
					const std::wstring&	wstrConnName)
	{
    KL_TMEASURE_BEGIN(L"CProxyBase::Initialize", 3);
		m_wstrProxyName=wstrLocalComponentName;
		m_wstrRemoteComponentName=wstrConnName;
		m_bDestroyConnection=false;

		TransportProxy::Initialize(
								m_wstrProxyName.c_str(),
								m_wstrRemoteComponentName.c_str());
        KLTR_GetTransport()->GetLocation(m_wstrRemoteComponentName.c_str(), m_wstrRemoteAddress);
    KL_TMEASURE_END();
	}

	void CProxyBase::InitializeLocal(
                        const KLPRCI::ComponentId&  idComponent,
                        const conn_attr_t*          pExtraAttributes)
	{
    KL_TMEASURE_BEGIN(L"CProxyBase::InitializeLocal", 3);
    KLPRCP_PB_BEGIN(m_wstrRemoteAddress)
		KLPRCI::ComponentId idProxy;
        MakeProxyName(idComponent, pExtraAttributes, idProxy, &m_wstrProxyName);
        m_wstrRemoteComponentName = idComponent.PutToString();
        ;
		bool bAdded=false;		
		m_wstrRemoteAddress=KLPRCI_FindInstance(idComponent);

        if(!m_wstrRemoteAddress.empty())
        {        
            KLSTD::CAutoPtr<KLWAT::ClientContext> pCustomAuthClientCtx;
            CAutoPtr<KLWAT::AuthServer> pAuthServer;
            KLWAT_GetGlobalAuthServer(&pAuthServer);
            KLSTD_ASSERT_THROW(pAuthServer);
            pCustomAuthClientCtx = pAuthServer->GenClientContext(
                                        KLWAT::c_szwEventsOnlyContext);
		    bAdded = KLTR_GetTransport()->AddClientConnection(
								m_wstrProxyName.c_str(),
								m_wstrRemoteComponentName.c_str(),
								m_wstrRemoteAddress.c_str(),
                                KLTRAP::c_Undefined,
                                false,
                                NULL,   //pCredentials
                                KLTRAP::Transport::PriorityNormal,
                                false, //bMakeTwoWayAuthentication
                                pCustomAuthClientCtx);    
        };
        on_connection_created(bAdded);
    KLPRCP_PB_END()
    KL_TMEASURE_END();
	};

    void CProxyBase::create_new_client_connection(
				const KLPRCI::ComponentId&  idComponent,
				const wchar_t*			    szwAddress,
                const int*                  pPorts,
                bool                        bUseSSL,
                const conn_attr_t*          pExtraAttributes)
    {
    KL_TMEASURE_BEGIN(L"create_new_client_connection)", 3);
        KLSTD_CHK(szwAddress, szwAddress && szwAddress[0]);
        KLSTD_CHK(pExtraAttributes, !pExtraAttributes || pExtraAttributes->m_nSize >= sizeof(conn_attr_t));
        ;
        KLPRCI::ComponentId idProxy;
        MakeProxyName(
                    idComponent,
                    pExtraAttributes,
                    idProxy,
                    &m_wstrProxyName);
        ;
        m_wstrRemoteComponentName = idComponent.PutToString();
        ;
        bool bAdded=false;
        if(pPorts)
        {
		    CPortEnumerator addresses(szwAddress, pPorts);
            
            std::wstring wstrRemoteAddress;
		    for(; !bAdded && addresses.GetNext(wstrRemoteAddress);)
		    {
            KLPRCP_PB_BEGIN(wstrRemoteAddress)
                bAdded = add_connection(
                            wstrRemoteAddress.c_str(),
                            bUseSSL,
                            pExtraAttributes);
            KLPRCP_PB_END()
                if(bAdded)
                    m_wstrRemoteAddress = wstrRemoteAddress;
            };
        }
        else
        {
        KLPRCP_PB_BEGIN(szwAddress)
            bAdded = add_connection(
                        szwAddress,
                        bUseSSL,
                        pExtraAttributes);
        KLPRCP_PB_END()
            if(bAdded)
                m_wstrRemoteAddress = szwAddress;
        };
        KLPRCP_PB_BEGIN(szwAddress)
            on_connection_created(bAdded);
        KLPRCP_PB_END()
    KL_TMEASURE_END();
    };
    
    void CProxyBase::create_new_gatewayed_connection(
                    const gateway_locations_t&  vecLocations,
                    long                        lTimeout,
                    bool                        bUseSSL,
                    const conn_attr_t*          pExtraAttributes)
    {
    KL_TMEASURE_BEGIN(L"CProxyBase::create_new_gatewayed_connection", 3)
        KLSTD_CHK(vecLocations, vecLocations.size() > 0);
        KLSTD_CHK(pExtraAttributes, !pExtraAttributes || pExtraAttributes->m_nSize >= sizeof(conn_attr_t));
        ;
        m_wstrRemoteComponentName = vecLocations[vecLocations.size()-1].
                                                    remoteComponentName;
        ;
        KLPRCI::ComponentId idComponent;
        idComponent.GetFromString(m_wstrRemoteComponentName);
        ;
        KLPRCI::ComponentId idProxy;
        MakeProxyName(idComponent, pExtraAttributes, idProxy, &m_wstrProxyName);
        ;        
        bool bMakeTwoWayAuthentication = false;
        KLSTD::CAutoPtr<KLWAT::ClientContext> pCustomAuthClientCtx;
        AUTH_HANDSHAKE_MODE nMode = AHM_BACKWARD_EVENTS_ONLY;

        if(pExtraAttributes)
            nMode = pExtraAttributes->m_nHandshakeMode;

        switch(nMode)
        {
        case AHM_FORWARD_ONLY:
            bMakeTwoWayAuthentication = false;
            break;
        case AHM_BACKWARD_EVENTS_ONLY:
            {
                CAutoPtr<KLWAT::AuthServer> pAuthServer;
                KLWAT_GetGlobalAuthServer(&pAuthServer);
                KLSTD_ASSERT_THROW(pAuthServer);
                pCustomAuthClientCtx = pAuthServer->GenClientContext(
                                        KLWAT::c_szwEventsOnlyContext);
            };
            break;
        case AHM_BACKWARD_SPECIFIED:
            pCustomAuthClientCtx = pExtraAttributes->m_pBwCallsContext;
            break;
        default:
            KLSTD_THROW(STDE_NOTPERM);
        };
        ;
        bool bAdded=false;
        KLPRCP_PB_BEGIN(m_wstrRemoteComponentName)
        if(bUseSSL)
        {
            bAdded = KLTR_GetTransport()->CreateSSLGatewayConnection(
                            m_wstrProxyName.c_str(),
                            const_cast<gateway_locations_t&>(vecLocations),
                            pExtraAttributes
                                ?   pExtraAttributes->m_pCert
                                :   NULL,
                            pExtraAttributes
                                ?   pExtraAttributes->m_pCredentials
                                :   NULL,
                            lTimeout,
                            pExtraAttributes
                                ?   pExtraAttributes->m_nPriority
                                :   KLTRAP::Transport::PriorityNormal,
                            pCustomAuthClientCtx);
        }
        else
        {
            KLSTD_CHK(pExtraAttributes, !pExtraAttributes || pExtraAttributes->m_pCredentials == NULL);
            bAdded = KLTR_GetTransport()->CreateGatewayConnection(
                                m_wstrProxyName.c_str(),
                                const_cast<gateway_locations_t&>(vecLocations),
                                lTimeout,
                                pExtraAttributes
                                    ?   pExtraAttributes->m_nPriority
                                    :   KLTRAP::Transport::PriorityNormal,
                                pCustomAuthClientCtx);
        };
        on_connection_created(bAdded);
        KLPRCP_PB_END()
    KL_TMEASURE_END();
    };

    bool CProxyBase::add_connection(
                    const wchar_t*      szwAddress,
                    bool                bUseSSL,
                    const conn_attr_t*  pExtraAttributes)
    {
        bool bAdded = false;

        bool bMakeTwoWayAuthentication = false;
        KLSTD::CAutoPtr<KLWAT::ClientContext> pCustomAuthClientCtx;
        AUTH_HANDSHAKE_MODE nMode = AHM_BACKWARD_EVENTS_ONLY;

        if(pExtraAttributes)
            nMode = pExtraAttributes->m_nHandshakeMode;

        switch(nMode)
        {
        case AHM_FORWARD_ONLY:
            bMakeTwoWayAuthentication = false;
            break;
        case AHM_TWOWAY:
            if(bUseSSL)
            {
                KLSTD_ASSERT(pExtraAttributes->m_nHandshakeMode != AHM_TWOWAY);
                KLSTD_THROW(STDE_NOTPERM);
            };
            bMakeTwoWayAuthentication = true;
            break;
        case AHM_BACKWARD_EVENTS_ONLY:
            {
                CAutoPtr<KLWAT::AuthServer> pAuthServer;
                KLWAT_GetGlobalAuthServer(&pAuthServer);
                KLSTD_ASSERT_THROW(pAuthServer);
                pCustomAuthClientCtx = pAuthServer->GenClientContext(
                                        KLWAT::c_szwEventsOnlyContext);
            };
            break;
        case AHM_BACKWARD_SPECIFIED:
            pCustomAuthClientCtx = pExtraAttributes->m_pBwCallsContext;
            break;
        default:
            KLSTD_THROW(STDE_NOTPERM);
        };
        
        const ProxyInfo* pProxyInfo = NULL;        
        if( pExtraAttributes &&
            pExtraAttributes->m_pProxyAuth &&
            !pExtraAttributes->m_pProxyAuth->location.empty())
        {
            pProxyInfo = pExtraAttributes->m_pProxyAuth;
        };

        KLTRAP::Transport::ConnectionInfo ci;
        ci.localName                =   m_wstrProxyName;
        ci.remoteName               =   m_wstrRemoteComponentName;
        ci.remoteLocation           =   szwAddress;
        //.pingTimeout              =   ;
        ci.priority                 =   pExtraAttributes
                            ?   pExtraAttributes->m_nPriority
                            :   KLTRAP::Transport::PriorityNormal;
        ci.pCustomCredentials       =   pExtraAttributes
                            ?   pExtraAttributes->m_pCredentials
                            :   NULL;
        ci.pCustomAuthClientCtx     =   pCustomAuthClientCtx;
        ci.bCreateSSLConnection     =   bUseSSL;
        ci.pRemotePublicSSLKey      =   pExtraAttributes
                            ?   pExtraAttributes->m_pCert
                            :   NULL;
        ci.pLocalPublicSSLKey       =   NULL;
        ci.bConnForGateway          =   pExtraAttributes
                            ?   pExtraAttributes->m_bForGateway
                            :   false;
        ci.pProxyInfo               =   pProxyInfo;
        ci.bMakeTwoWayAuth          =   bMakeTwoWayAuthentication;
        ci.bCompressTraffic         =   pExtraAttributes
                            ?   pExtraAttributes->m_bCompressTraffic
                            :   false;

        KLSTD::Trace(
                4,
                KLCS_MODULENAME,
                L"Connecting to address: '%ls', "
                L"me is: '%ls', "
                L"peer is: '%ls', "
                L"bIsGateway = '%u', "
                L"bUseSSL = '%u', "
                L"bCompressTraffic = '%u' "
                L"\n",
                szwAddress,
                m_wstrProxyName.c_str(),
                m_wstrRemoteComponentName.c_str(),
                ci.bConnForGateway,
                bUseSSL,
                ci.bCompressTraffic);

        bAdded = KLTR_GetTransport()->AddClientConnection2(ci);

        if(!bAdded)
        {
            KLSTD_TRACE1(4, L"...connecting to address '%ls' failed\n", szwAddress);
        }
        else
        {
            KLSTD_TRACE1(4, L"...connecting to address '%ls' succeeded !!!\n", szwAddress);
        };

        return bAdded;
    };   

    void CProxyBase::MakeProxyName(
                    const KLPRCI::ComponentId&  idComponent,
                    const conn_attr_t*          pExtraAttributes,
                    KLPRCI::ComponentId&        idProxy,
                    std::wstring*               pwstrProxyName)
    {
        if( pExtraAttributes &&
            pExtraAttributes->m_pszwProxyName &&
            pExtraAttributes->m_pszwProxyName[0])
        {
            if(pwstrProxyName)
            {
                *pwstrProxyName = pExtraAttributes->m_pszwProxyName;
                idProxy.GetFromString(*pwstrProxyName);
            }
            else
                idProxy.GetFromString(pExtraAttributes->m_pszwProxyName);
        }
        else
        {
            idProxy.productName = idComponent.productName;
            idProxy.version = idComponent.version;
            idProxy.componentName = 
                (pExtraAttributes && pExtraAttributes->m_bIsTransparent)
                    ?   c_szwTransparentProxyName
                    :   c_szwProxyName;
            idProxy.instanceId = KLPRCI_CreateInstanceId(NULL);
            ;
            if(pwstrProxyName)
                *pwstrProxyName = idProxy.PutToString();
        };
    };
    
    void CProxyBase::on_connection_created(bool bWasAdded)
    {
		if(bWasAdded)
		{
			m_bDestroyConnection = true;
			TransportProxy::Initialize(
									m_wstrProxyName.c_str(),
									m_wstrRemoteComponentName.c_str());
            KLSTD_TRACE3(
                        3,
                        L"Created proxy #%u for '%ls' from %u\n",
                        m_lProxyId,
                        m_wstrRemoteComponentName.c_str(),
                        g_cfProxies);
		}
		else
		{
			KLSTD_TRACE2(
						3,
						L"Failed to create proxy for connection '%ls'-'%ls'\n",
						m_wstrProxyName.c_str(),
						m_wstrRemoteComponentName.c_str());
            ;			
            KLERR_MYTHROW1(
                        KLPRCP::ERR_CANT_CONNECT,
                        m_wstrRemoteComponentName.c_str());
		};
    }

    class CAsyncConnectionRemover : public KLSTD::KLBaseImpl<KLSTD::KLBase>
    {
    public:
        CAsyncConnectionRemover(std::wstring& wstrLocal, std::wstring& wstrRemote)
            :   m_wstrLocal(wstrLocal)
            ,   m_wstrRemote(wstrRemote)
            ,   m_bRemoveWasCalled(false)
        {;};

        virtual ~CAsyncConnectionRemover()
        {
            Remove();
        };

        virtual void Remove()
        {
            if(!m_bRemoveWasCalled)
            {
                KLSTD_TRACE2(
                        3,
                        L"Asynchronously closing connection '%ls','%ls'\n",
                        m_wstrLocal.c_str(),
                        m_wstrRemote.c_str());
			    KL_TMEASURE_BEGIN(L"CloseClientConnection", 3)
				    KLERR_IGNORE(KLTR_GetTransport()->CloseClientConnection(
											    m_wstrLocal.c_str(),
											    m_wstrRemote.c_str()));
			    KL_TMEASURE_END()
                m_bRemoveWasCalled=true;
            };
        };

    protected:
        std::wstring m_wstrLocal, m_wstrRemote;
        bool    m_bRemoveWasCalled;
    };

	//! clears connection
	KLSTD_NOTHROW void CProxyBase::ClearConnections() throw()
	{
    KLERR_BEGIN
		if(!m_bRemoveAsync)
        {
            if(m_bDestroyConnection)
		    {
			KL_TMEASURE_BEGIN(L"CProxyBase::ClearConnections", 3)
				KLERR_IGNORE(KLTR_GetTransport()->CloseClientConnection(
										m_wstrProxyName.c_str(),
										m_wstrRemoteComponentName.c_str()));
            KL_TMEASURE_END()
			    m_bDestroyConnection=false;
		    };
        }
        else
        {
            CAutoPtr<CAsyncConnectionRemover> pRemover;
            pRemover.Attach(new CAsyncConnectionRemover(m_wstrProxyName, m_wstrRemoteComponentName));            
            KLSTD_CHKMEM(pRemover);
            KLTMSG::AsyncCall0T<CAsyncConnectionRemover>::Start(
                                        pRemover,
                                        &CAsyncConnectionRemover::Remove);
        };
    KLERR_ENDT(1);
	};
	
	void CProxyBase::CheckAndWait(
					KLSTD::Semaphore*	pSemaphore, 
					long				lTimeout,
					long				lTimeSlice,
                    volatile bool*      pbCancelWait,
                    bool                bCheckShutdown)
	{
        KLSTD_ASSERT(pSemaphore);
        KLSTD_ASSERT(lTimeSlice > 0);
        unsigned long clkStart=KLSTD::GetSysTickCount();
		for(;;)
		{
			if(pSemaphore->Wait(lTimeSlice))
				break;
            if(pbCancelWait && (*pbCancelWait))
                KLSTD_THROW(STDE_CANCELED);
            if( lTimeout > 0 && 
                KLSTD::DiffTickCount(KLSTD::GetSysTickCount(), clkStart)  >= (unsigned)lTimeout)
            {
				KLSTD_THROW(STDE_TIMEOUT);
            };
			if(!KLTR_GetTransport()->IsConnectionActive(
							m_wstrProxyName.c_str(),
							m_wstrRemoteComponentName.c_str()))
            {
                KLERR::ErrLocAdapt locinfo(
                            KLPRCP::PRCPEL_ERR_UNEXPECTED_STOP,
                            KLERR_LOCMOD_PRCP);
                KLERR_LOCTHROW0(locinfo, KLSTD::szwModule, KLSTD::STDE_UNAVAIL);
            };
            if(bCheckShutdown && KLSTD_GetShutdownFlag())
                KLSTD_THROW(STDE_UNAVAIL);
		};
	};

    static void LocalizeCreateConnectionError(KLERR::Error* pError, const wchar_t* szwAddress)
    {
        if(!pError)
            return;
        long nID = 0;
        switch(pError->GetId())
        {
        case KLTRAP::TRERR_WRONG_ADDR:
            nID = PRCPEL_TR_WRONG_ADDR;
            break;
        case KLTRAP::TRERR_REMOTE_CONN_ALREADY_EXISTS:
            nID = PRCPEL_TR_REMOTE_CONN_ALREADY_EXISTS;
            break;
        case KLTRAP::TRERR_MAX_CONNECTIONS:
            nID = PRCPEL_TR_MAX_CONNECTIONS;
            break;
        case KLTRAP::TRERR_AUTHENTICATION_FAILED:
            nID = PRCPEL_TR_AUTHENTICATION_FAILED;
            break;
        case KLTRAP::TRERR_SSL_CONNECT_ERR:
            nID = PRCPEL_TR_SSL_CONNECT_ERR;
            break;
        case KLTRAP::TRERR_SSL_SERVER_AUTH_ERR:
            nID = PRCPEL_TR_SSL_SERVER_AUTH_ERR;
            break;
        case KLTRAP::TRERR_HOST_NOT_FOUND:
            nID = PRCPEL_TR_HOST_NOT_FOUND;
            break;
        case KLTRAP::TRERR_SOCKET_NOBUFS:
            nID = PRCPEL_TR_SOCKET_NOBUFS;
            break;
        case KLPRCP::ERR_CANT_CONNECT:
            nID = PRCPEL_ERR_CANT_CONNECT;
            break;
        default:
            if( wcscmp( KLTRAP::KLTRAP_ModuleName,
                        pError->GetModuleName()) == 0 )
            {
                wchar_t szwBuffer[16] = {0};
                KLSTD_SWPRINTF(szwBuffer, KLSTD_COUNTOF(szwBuffer), L"0x%X", pError->GetId());
                KLERR_SetErrorLocalization(
                            pError,                             
                            KLPRCP::PRCPEL_TR_GENERAL, 
                            KLERR_LOCMOD_PRCP, 
                            szwAddress, 
                            szwBuffer);
            };
            break;
        };
        if(nID)
        {
            KLERR_SetErrorLocalization(
                        pError,                         
                        nID,
                        KLERR_LOCMOD_PRCP, 
                        szwAddress);
        };
    };
}


/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	proxybase.h
 * \author	Andrew Kazachkov
 * \date	24.02.2003 17:13:28
 * \brief	
 * 
 */

#ifndef __KLPROXYBASE_H__
#define __KLPROXYBASE_H__

#include "transport/tr/transportproxy.h"
#include "kca/prci/componentid.h"
#include "internal.h"
#include "proxy_attrib.h"

//! Makes address for listen location
std::wstring KLPRCP_MakeAddress(int nPort, bool bLocalOnly=false);

std::wstring KLPRCP_MakeConnectionAddress(int nPort, bool bLocalOnly=false);

std::wstring KLPRCP_MakeAddress2(const std::wstring& wstrHost, int nPort);

#define KLPRCP_WAITTIMESLICE  5000L

namespace KLPRCP
{
    class CProxyBase : public KLTRAP::TransportProxy
	{
	public:
		CProxyBase();
		virtual ~CProxyBase();

		//! clear connection
		KLSTD_NOTHROW void ClearConnections() throw();
		
        //! Initialize with existing connection
        virtual void Initialize(
						const std::wstring&	wstrLocalComponentName,
						const std::wstring&	wstrConnName);
		
        //! Initialize local proxy
		virtual void InitializeLocal(
						const KLPRCI::ComponentId&  idComponent,
                        const conn_attr_t*          pExtraAttributes = NULL);

        //! creates new gateway connection
        void create_new_gatewayed_connection(
                    const gateway_locations_t&  vecLocations,
                    long                        lTimeout,
                    bool                        bUseSSL = false,
                    const conn_attr_t*          pExtraAttributes = NULL);

        //! creates new client connection
        void create_new_client_connection(
					const KLPRCI::ComponentId&  idComponent,
					const wchar_t*			    szwAddress,
                    const int*                  pPorts = NULL,
                    bool                        bUseSSL = false,
                    const conn_attr_t*          pExtraAttributes = NULL);
	    
        //Wait for semaphore
		void CheckAndWait(
						KLSTD::Semaphore*	pSemaphore, 
						long				lTimeout,
						long				lTimeSlice=KLPRCP_WAITTIMESLICE,
                        volatile bool*      pbCancelWait=NULL,
                        bool                bCheckShutdown = false);

    protected:
        static void MakeProxyName(
                        const KLPRCI::ComponentId&  idComponent,
                        const conn_attr_t*          pExtraAttributes,
                        KLPRCI::ComponentId&        idProxy,
                        std::wstring*               pwstrProxyName = NULL);

        bool add_connection(
                        const wchar_t*      szwAddress,
                        bool                bUseSSL,
                        const conn_attr_t*  pExtraAttributes);

        void on_connection_created(bool bWasAdded);
    public:		
		std::wstring	m_wstrProxyName,
						m_wstrRemoteComponentName,
                        m_wstrRemoteAddress;
        const long      m_lProxyId;
        bool            m_bDestroyConnection,
                        m_bRemoveAsync;
	};

    KLSTD_INLINEONLY std::wstring makeRemoteAddress(int nPort)
    {
        return KLPRCP_MakeAddress(nPort, false);
    };

    KLSTD_INLINEONLY std::wstring makeRemoteAddress(const wchar_t* foo, int nPort)
    {
        return KLPRCP_MakeAddress(nPort, false);
    };

    //! Makes address for listen location
	KLSTD_INLINEONLY std::wstring makeLocalAddress(int nPort)
    {
        return KLPRCP_MakeAddress(nPort, true);
    };

};


#define KLPRCP_CHKRESULT()          (KLPRCP::test_excpt(r.error))

#define KLPRCP_PROXY_LOCK2(_ptr)	KLTRAP::TransportConnectionLocker   \
                    _locker(static_cast<KLTRAP::TransportProxy*>(_ptr))

#define KLPRCP_PROXY_LOCK()		    KLPRCP_PROXY_LOCK2(this)
#define KLPRCP_PROXY_CHECK()	    _locker.CheckResult()
#define KLPRCP_PROXY_SOAP()		    _locker.Get()
#define KLPRCP_DEF_RESPONSE(_pref,_name)		\
				_pref##_name##Response r;\
				soap_default_##_pref##_name##Response(KLPRCP_PROXY_SOAP(), &r);

#endif //__KLPROXYBASE_H__

// Local Variables:
// mode: C++
// End:

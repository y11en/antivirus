#ifndef _KLPRCP_INTERNAL_H_
#define _KLPRCP_INTERNAL_H_

#include <std/par/params.h>
#include <kca/prcp/agentproxy.h>
#include <vector>
#include <sstream>
#include <transport/tr/transport.h>

//#include "soapStub.h"

struct soap;
class SOAPComponentId;
struct param_error;

namespace KLPRCP
{
    struct conn_attr_t;
}

KLCSKCA_DECL bool CheckAgentExistence(
   						const std::wstring&			wstrProductName,
						const std::wstring&			wstrVersion);

KLCSKCA_DECL std::wstring KLPRCP_CreateLocalAddress(int nPort);
KLCSKCA_DECL std::wstring KLPRCP_CreateNonLocalAddress(const wchar_t* szwHostName, int nPort);

KLCSKCA_DECL void KLPRCP_SplitAddress(
				const wchar_t*	/*[in]*/	address,
				std::wstring*	/*[out]*/	protocol,
				int*        	/*[out]*/	port,
				std::wstring*	/*[out]*/	hostname);

KLCSKCA_DECL void KLPRCP_TestExcpt(const param_error &error);

KLCSKCA_DECL void KLPRCP_AttachComponentProxy(
					const std::wstring&         wstrLocalConnection,
					const std::wstring&         wstrRemoteConnection,
					KLPRCP::ComponentProxy**    ppProxy,
                    KLSTD::KLBase*              pOwner = NULL);

KLCSKCA_DECL void KLPRCP_CreateComponentProxy2( 
                        const KLPRCI::ComponentId&  id,
                        KLPRCP::ComponentProxy**	ppProxy,
                        const KLPRCP::conn_attr_t*  pExtraAttr = NULL);

KLCSKCA_DECL void KLPRCP_CreateGatewayedComponentProxy(
                        KLTRAP::Transport::LocationsList&   vecLocations,
                        long                                lTimeout,
                        bool                                bSSL,
                        const KLPRCP::conn_attr_t*          pExtraAttr,
                        KLPRCP::ComponentProxy**            ppProxy);

//Special functions for agent
KLCSKCA_DECL void KLPRCP_CreateAgentProxyForConnection(
					const std::wstring& wstrLocalConnection,
					const std::wstring& wstrRemoteConnection,
					KLPRCP::AgentProxy** ppProxy);

KLCSKCA_DECL void KLPRCP_CreateGatewayedAgentProxy(
                        KLTRAP::Transport::LocationsList&   vecLocations,
                        long                                lTimeout,
                        bool                                bSSL,
                        const KLPRCP::conn_attr_t*          pExtraAttr,
                        KLPRCP::AgentProxy**	            ppProxy);

namespace KLPRCP
{
	///KLCSKCA_DECL std::wstring FindInstance(const KLPRCI::ComponentId& id);
//#ifdef soapStub_H

	KLSTD_NOTHROW void ComponentIdForSOAP(
					struct soap*				soap,
					SOAPComponentId&			sid,
					const KLPRCI::ComponentId&	id,
					bool						bCopyStrings) throw();

	KLSTD_NOTHROW void ComponentIdFromSOAP(
					KLPRCI::ComponentId&		id,
					const SOAPComponentId&		sid) throw();
//#endif

	KLSTD_INLINEONLY void test_excpt(const param_error &error)
	{
		KLPRCP_TestExcpt(error);
	};
	//const static wchar_t c_szwLocalAddress[]=L"http://127.0.0.1";


    template<class T>
	class CPortEnumeratorT
	{
	public:
		CPortEnumeratorT(const wchar_t* szwAddress, const T* pPorts=NULL)
            :   m_ptrPort(0)
		{
			int nPort = 0;
			KLPRCP_SplitAddress(szwAddress, &m_wstrProto, &nPort, &m_wstrHost);
            ;
            {
                int nPorts;
			    for(nPorts=0; pPorts && pPorts[nPorts]; ++nPorts);
                if(nPort)
                    ++nPorts;
                m_vecPorts.reserve(nPorts+1);
            };
            ;
			if(nPort)
				m_vecPorts.push_back(nPort);
            ;
			for(int j=0; pPorts && pPorts[j]; ++j)
				if(pPorts[j] != nPort)
					m_vecPorts.push_back(pPorts[j]);
			m_vecPorts.push_back(0);
		};

		bool GetNext(std::wstring& wstrNextAddress)
		{
			if(m_ptrPort < 0)
				return false;
			T nPort=m_vecPorts[m_ptrPort++];
			if(!nPort || m_ptrPort > (int)m_vecPorts.size())
			{
				m_ptrPort=-1;
				return false;
			}
			wchar_t szwTmp[16]=L"";
			wstrNextAddress.clear();//=L"";
			wstrNextAddress.reserve(256);
			wstrNextAddress+=m_wstrProto;
			if(!m_wstrProto.empty())
				wstrNextAddress+=L"://";
			wstrNextAddress+=m_wstrHost;
			wstrNextAddress+=L':';
			wstrNextAddress+=_ltow(nPort, szwTmp, 10);//function _ltow is defined in file kldefs.h
			return true;
		};
	protected:
		std::wstring	m_wstrProto, m_wstrHost;
		std::vector<T>  m_vecPorts;
		int				m_ptrPort;
	};

    typedef CPortEnumeratorT<int> CPortEnumerator;
};

//! OBSOLETE
KLCSKCA_DECL void KLPRCP_CreateTransparentComponentProxy( 
                        const KLPRCI::ComponentId & id,
                        KLPRCP::ComponentProxy**	ppProxy);

#endif

// Local Variables:
// mode: C++
// End:

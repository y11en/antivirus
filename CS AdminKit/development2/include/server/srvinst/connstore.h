/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	connstore.h
 * \author	Andrew Kazachkov & Andrew Lashenkov
 * \date	27.05.2003 12:41:29
 * \brief	
 * 
 */

#ifndef __KL_CONNSTORE_H__
#define __KL_CONNSTORE_H__

//#include <srvp/naginst/nagentproxy.h>
#include <kca/prcp/proxybase.h>
#include <kca/prcp/componentproxy.h>
#include <server/srvinst/srv_certdata.h>

#include <map>

namespace KLSRV
{
    /*
    class KLSTD_NOVTABLE NagentConnection : public KLSTD::KLBase
    {
        friend class CNagentConnectionsStorage;
    protected:
        virtual void Detach()=0;    
        virtual void GetLocationData(std::wstring& wstrLocation, int& nUsedPort)=0;
    public:
        KLSTD_NOTHROW virtual bool IsAlive() throw() = 0;
        virtual std::wstring GetNagentInstanceId() = 0;
        virtual std::wstring GetRemoteName() = 0;
        virtual std::wstring GetLocalName() = 0;
        virtual void GetTransportNames(std::wstring& wstrLocal, std::wstring& wstrRemote) = 0;
        virtual void CreateProxyBase(KLPRCP::CProxyBase& proxy) = 0;
        virtual void CreateNagentProxy(KLNAG::NagentProxy** ppNagent) = 0;
        virtual void CreateComponentProxy(KLPRCP::ComponentProxy** ppComponent) = 0;
    };


    class CNagentConnectionsStorage
    {
    public:
        CNagentConnectionsStorage();
        ~CNagentConnectionsStorage();
        ;
        bool IfExists(const std::wstring& wstrInstanceId);
        bool Get(const std::wstring& wstrInstanceId, NagentConnection** ppConn);
        void MarkForRemove(const std::wstring& wstrInstanceId);
        void Clear();
        size_t GetSize();
        ;
        //Methods called from tr-t callback ONLY !!!
        void OnComponentConnected(const std::wstring& wstrInstanceId);
        void OnComponentDisconnected(const std::wstring& wstrInstanceId);
        ;
        void Lock(const std::wstring& wstrInstanceId, NagentConnection* p);
        void Unlock(const std::wstring& wstrInstanceId, NagentConnection* p);
    protected:
        struct con_data_t
        {
            con_data_t()
                :   m_lLockCount(0)
                ,   m_bMarkedForDelete(false)
            {;};
            con_data_t(const con_data_t& x)
                :   m_pConn(x.m_pConn)
                ,   m_lLockCount(x.m_lLockCount)
                ,   m_bMarkedForDelete(x.m_bMarkedForDelete)
            {;}
            con_data_t(NagentConnection* pConn)
                :   m_lLockCount(0)
                ,   m_pConn(pConn)
                ,   m_bMarkedForDelete(false)
            {;}

            KLSTD::CAutoPtr<NagentConnection>   m_pConn;
            volatile long                       m_lLockCount;
            volatile bool                       m_bMarkedForDelete;
        };
        typedef std::map<std::wstring, con_data_t> conns_t;
        conns_t                                 m_mapConnections;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
    };
    */

//{ Version 2.0 ////////////////////////////////////////////////////////////////////

    class KLSTD_NOVTABLE ConnectionInfo : public KLSTD::KLBase
    {
    public:
        KLSTD_NOTHROW virtual bool IsAlive() throw() = 0;
        virtual KLPRCI::ComponentId GetRemoteComponentId() = 0;
        virtual KLPRCI::ComponentId GetLocalComponentId() = 0;
        virtual std::wstring GetRemoteName() = 0;
        virtual std::wstring GetLocalName() = 0;
        virtual void GetTransportNames(std::wstring& wstrLocal, std::wstring& wstrRemote) = 0;
        virtual void CreateProxyBase(KLPRCP::CProxyBase& proxy) = 0;
        virtual void CreateComponentProxy(KLPRCP::ComponentProxy** ppComponent) = 0;
        virtual void CreateRemoteComponentProxy(KLPRCP::ComponentProxy** ppRemoteComponentProxy) = 0;
    };

    class CConnectionsInfoStorage
    {
    public:
        CConnectionsInfoStorage(const KLPRCI::ComponentId& idRemoteFilter);
        ~CConnectionsInfoStorage();
        
        bool IfExists(const std::wstring& wstrInstanceId);
        bool Get(const std::wstring& wstrInstanceId, ConnectionInfo** ppConn);
        void MarkForRemove(const std::wstring& wstrInstanceId);
        void Clear();
        size_t GetSize();

        //Methods called from tr-t callback ONLY !!!
        void OnComponentConnected(const KLPRCI::ComponentId& idRemote);
        void OnComponentDisconnected(const KLPRCI::ComponentId& idRemote);
        ;
        void Lock(const std::wstring& wstrInstanceId, ConnectionInfo* p);
        void Unlock(const std::wstring& wstrInstanceId, ConnectionInfo* p);

		// Implementation:

		static void CreateConnectionInfo(
			const KLPRCI::ComponentId&  idRemoteComponentId,
			const KLPRCI::ComponentId&  idLocalComponentId,
			KLSTD::CAutoPtr<KLSRV::ConnectionInfo>& pConn);

		void CreateRemoteComponentProxy(
			const KLPRCI::ComponentId&	idLocal,
			const KLPRCI::ComponentId&	idRemote,
			KLPRCP::ComponentProxy** ppRemoteComponent);

    protected:
        struct con_data_t
        {
            con_data_t()
            :   
				m_lLockCount(0),
                m_bMarkedForDelete(false)
            {
			}
            con_data_t(const con_data_t& x)
            :   
				m_pConn(x.m_pConn),
                m_lLockCount(x.m_lLockCount),
                m_bMarkedForDelete(x.m_bMarkedForDelete)
            {
			}
            con_data_t(ConnectionInfo* pConn)
            :
				m_lLockCount(0),
                m_pConn(pConn),
                m_bMarkedForDelete(false)
            {
			}

            KLSTD::CAutoPtr<ConnectionInfo> m_pConn;
            volatile long                       m_lLockCount;
            volatile bool                       m_bMarkedForDelete;
        };

        typedef std::map<std::wstring, con_data_t> conns_t;
        conns_t                                 m_mapConnections;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
		KLPRCI::ComponentId m_idRemoteFilter;
        bool                m_bServerFlagFixTrCallbacksOrder;
    };
//} Version 2.0 ////////////////////////////////////////////////////////////////////

    typedef ConnectionInfo          NagentConnection;
    typedef CConnectionsInfoStorage CNagentConnectionsStorage;
};


/*!
  \brief	Creates new connection with nagent

  \param	wstrInstanceId - instanceid of nagent
  \param	wstrNagentAddress - address of nagent
  \param	idProxy - name of listener (and proxy)
  \param	wstrServerLocation - listener address; if empty than you should
                create listener by yourself
  \param	lUnicastTimeout - timeout to wait for nagent's reaction to unicast
  \param	ppConn - variable where pointer to proxy will be written
*/
/*
void KLSRV_CreateNagentConnection(
                        const std::wstring&         wstrInstanceId,
                        const std::wstring&         wstrNagentAddress,
                        const KLPRCI::ComponentId&  idProxy,
                        const std::wstring&         wstrServerLocation,
                        long                        lUnicastTimeout,
                        KLSRV::CertData*            pCertData,
                        KLSRV::NagentConnection**   ppConn);
*/


//{ Version 2.0 ////////////////////////////////////////////////////////////////////
/*!
  \brief	Creates new connection with remote component

  \param	wstrInstanceId - instanceid of remote component
  \param	wstrRemoteAddress - address of remote component
  \param	idProxy - name of listener (and proxy)
  \param	wstrServerLocation - listener address; if empty than you should
                create listener by yourself
  \param	lUnicastTimeout - timeout to wait for nagent's reaction to unicast
  \param	ppConn - variable where pointer to proxy will be written
*/
void KLSRV_CreateConnectionInfo(
    const KLPRCI::ComponentId&	idRemote,
    const std::wstring&         wstrRemoteComponentAddress,
    const KLPRCI::ComponentId&  idProxy,
    const std::wstring&         wstrServerLocation,
    long                        lUnicastTimeout,
    KLSRV::CertData*            pCertData,
    KLSRV::ConnectionInfo**   ppConn);

void KLSRV_CreateGatewayConnectionInfo(
	const std::wstring& wstrLocalProduct,
	const std::wstring& wstrLocalVersion,
	const std::vector<KLPRCI::ComponentId>& vecSlavePath,
	int nTimeoutInSec,
	KLSRV::ConnectionInfo** ppConn);


//} Version 2.0 ////////////////////////////////////////////////////////////////////


#endif //__KL_CONNSTORE_H__
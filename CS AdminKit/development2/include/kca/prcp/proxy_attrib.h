/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	proxy_attrib.h
 * \author	Andrew Kazachkov
 * \date	14.10.2004 10:51:55
 * \brief	
 * 
 */

#ifndef __KLPROXY_ATTRIB_H__
#define __KLPROXY_ATTRIB_H__

#include "transport/tr/transport.h"

namespace KLPRCP
{

    typedef KLTRAP::Transport::ConnectionPriority   conn_priority_t;
    typedef KLTRAP::Transport::LocationsList        gateway_locations_t;
    typedef KLTRAP::ProxyInfo                       proxy_auth_t;

    enum AUTH_HANDSHAKE_MODE
    {
        AHM_FORWARD_ONLY,        //! only forward handshake will be made, there will be no context for backward calls
        AHM_TWOWAY,              //! both forward and backward handshake are implemented
        AHM_BACKWARD_EVENTS_ONLY,//! only forward handshake will be made, EventsOnly context will be used for backward calls
        AHM_BACKWARD_SPECIFIED,  //! only forward handshake made, specified context will be used for backward calls
    };
    
    struct conn_attr_t
    {
        conn_attr_t()
            :   m_nSize(sizeof(conn_attr_t))
            ,   m_pCert         (NULL)
            ,   m_pCredentials  (NULL)
            ,   m_pszwProxyName (NULL)
            ,   m_nPriority     (KLTRAP::Transport::PriorityNormal)
            ,   m_bIsTransparent(false)
            ,   m_bRemoveAsync  (false)
            ,   m_nHandshakeMode(AHM_BACKWARD_EVENTS_ONLY)
            ,   m_pBwCallsContext(NULL)
            ,   m_pProxyAuth(NULL)
            ,   m_bCompressTraffic(false)
            ,   m_bForGateway(false)
        {;}
        size_t                  m_nSize;
        KLPAR::BinaryValue*     m_pCert;
        KLPAR::Params*          m_pCredentials;
        const wchar_t*          m_pszwProxyName;
        conn_priority_t         m_nPriority;
        bool                    m_bIsTransparent;
        bool                    m_bRemoveAsync;
        AUTH_HANDSHAKE_MODE     m_nHandshakeMode;
        KLWAT::ClientContext*   m_pBwCallsContext;
        const proxy_auth_t*     m_pProxyAuth;
        bool                    m_bCompressTraffic;
        bool                    m_bForGateway;
    };
};
#endif //__KLPROXY_ATTRIB_H__

/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	admsrvconn.h
 * \author	Andrew Kazachkov
 * \date	20.04.2004 18:41:58
 * \brief	
 * 
 */

#ifndef __KL_ADMSRVCONN_H__
#define __KL_ADMSRVCONN_H__

#include <srvp/admsrv/admsrv.h>

namespace KLADMSRV
{
    class KLSTD_NOVTABLE AdmServerConn: public KLSTD::KLBaseQI
    {
    public:
        virtual void GetConnectionProperties(
                    KLPRCI::ComponentId&    idServer,
                    std::wstring&           wstrAddress,
                    bool&                   bUseSSL,
                    KLPAR::Params**         ppCredentials,
                    KLPAR::BinaryValue**    ppCert,
                    KLPRCP::proxy_auth_t&   proxyhost,
                    vec_comps_t&            vecSlavePath,
                    long&                   lTimeoutGw,
                    bool&                   bCompressTraffic) = 0;
        
        virtual long GetServerVersion() = 0;
        
    };
    
    class KLSTD_NOVTABLE ConnDescr : public KLSTD::KLBaseQI
    {
    public:
        //returns true if we can use it and false if it is already used
        virtual void Initialize(
                        const wchar_t* szwLocalName,
                        const wchar_t* szwRemoteName) = 0;

        virtual void GetTransportNames(
                            std::wstring& wstrLocalName,
                            std::wstring& wstrRemoteName) = 0;

        virtual bool IsConnectionActive() = 0;

        virtual void CloseConnection() = 0;

        virtual bool get_Unused() = 0;

        virtual void put_Unused() = 0;
    };

    KLCSSRVP_DECL bool CheckServerVersion(KLSTD::KLBaseQI* p, long lRequiredVersion, bool bThrow = true);

    void AddSlavePathToLocations(
            const vec_comps_t&              vecSlavePath,
            KLPRCP::gateway_locations_t&    m_vecLocationsGw);
};
#endif //__KL_ADMSRVCONN_H__
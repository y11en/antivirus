/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	srv_gsyn.h
 * \author	Andrew Kazachkov
 * \date	14.07.2005 18:04:09
 * \brief	Server-side gsyn interface
 * 
 */

#include <srvp/gsyn/groupsync.h>
#include <server/db/dbconnection.h>

#include <server/srvinst/klserver.h>

#ifndef __KL_SRV_GSYN_H__
#define __KL_SRV_GSYN_H__

namespace KLSRV
{
    enum
    {
        GSF_DEFAULT = 0xFFFFFFFF,
        GSF_FORCE = 0x1,
        //GSF_UPDATE_GSHOSTS = 0x2,
        GSF_NO_UPDATE_GSHOSTS = 0x4,
        GSF_FORCE_UPDATE_GSHOSTS = 0x8
    };

    typedef AVP_dword   GSYN_FLAGS;

    enum SrvGS_UpdateAction
    {
        SGSUA_REPLACE = 0,
        SGSUA_ADD,
        SGSUA_REMOVE
    };

    class KLSTD_NOVTABLE SrvGroupSync
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Creates new gsyn object for group

          \param	pCon                    db connection
          \param	szwName                 gsyn name, gsyn with similar names shadows each other
          \param	szwSyncObjLocation      location, up to 512 wide symbols
          \param	szwProductName          product name, up to 31 narrow symbols
          \param	szwProductVersion       product version, up to 31 narrow symbols
          \param	nDirection              must be GSYND_ONEWAY.
          \param	lGroup                  group id
          \param	lElapse                 currently not used
          \param	bCutoffChildren         must be false
          \param	dwFlags                 flags, GSF_DEFAULT means GSF_FORCE
          \return	gsyn_id
        */
        virtual long AddSyncObjG(
                    KLDB::DbConnectionPtr   pCon,
                    const wchar_t*          szwName,
                    const wchar_t*          szwSyncObjLocation,
                    const wchar_t*          szwProductName,
                    const wchar_t*          szwProductVersion,
                    KLGSYN::GSYN_DIRECTION  nDirection,
                    long                    lGroup,
                    long                    lElapse,
                    bool                    bCutoffChildren,
                    GSYN_FLAGS              dwFlags = GSF_DEFAULT) = 0;

		virtual long AddSyncObjH(
                    KLDB::DbConnectionPtr   pCon,
					const wchar_t*		    szwName,
					const wchar_t*		    szwSyncObjLocation,
                    const wchar_t*		    szwProductName,
                    const wchar_t*		    szwProductVersion,
					KLGSYN::GSYN_DIRECTION  nDirection,
                    long*                   pHosts,
					size_t				    nHosts,
					long				    lElapse,
                    KLSRV::GSYN_FLAGS       dwFlags = GSF_DEFAULT) = 0;

		virtual void RemoveSyncObj(
                    KLDB::DbConnectionPtr   pCon,
                    long	                lSync,
                    GSYN_FLAGS              dwFlags = GSF_DEFAULT) = 0;

		virtual void UpdateHostsSyncObjH(
                    KLDB::DbConnectionPtr   pCon,
                    long                    lGsynId,
                    long*                   pHosts,
                    size_t                  nHosts,
                    SrvGS_UpdateAction      nAction = SGSUA_REPLACE) = 0;

        virtual AVP_qword IncrementVersion(
                    KLDB::DbConnectionPtr   pCon,
                    long                    lSync) = 0;
        
        virtual void ForceGroupSync(
                    KLDB::DbConnectionPtr   pCon,
                    long                    idSync,
                    GSYN_FLAGS              dwFlags = GSF_DEFAULT) = 0;

        virtual void ForceGroupSyncAsync(
                    long            idSync,
                    GSYN_FLAGS      dwFlags = GSF_DEFAULT) = 0;

        virtual void MayRecalcHostsList(
                    KLDB::DbConnectionPtr   pCon,
                    long                    idSync) = 0;
        
        virtual void ForceRecalcAllGsync(
                    KLDB::DbConnectionPtr   pCon) = 0;

        virtual void GetGSyncsByName(
                    KLDB::DbConnectionPtr   pCon,
                    const std::wstring&     wstrName,
                    std::vector<long>&      vecGsyncs) = 0;

        //!obsolete
		long AddSyncObjH(
                    KLDB::DbConnectionPtr   pCon,
					const wchar_t*		    szwName,
					const wchar_t*		    szwSyncObjLocation,
                    const wchar_t*		    szwProductName,
                    const wchar_t*		    szwProductVersion,
					KLGSYN::GSYN_DIRECTION  nDirection,
                    KLGSYN::host_id_t*		pHosts,
					long				    nHosts,
					long				    lElapse)
        {
            std::vector<long> vecHosts;
            FillHosts(pCon, pHosts, nHosts, vecHosts);
            return AddSyncObjH(
                        pCon,
                        szwName,
                        szwSyncObjLocation,
                        szwProductName,
                        szwProductVersion,
                        nDirection,
                        vecHosts.empty()?NULL:&vecHosts[0],
                        vecHosts.size(),
                        lElapse);
        };

        //!obsolete
		void UpdateHostsSyncObjH(
                    KLDB::DbConnectionPtr   pCon,
                    long                    lGsynId,
                    KLGSYN::host_id_t*      pHosts,
                    size_t                  nHosts)
        {
            std::vector<long> vecHosts;
            FillHosts(pCon, pHosts, nHosts, vecHosts);
            UpdateHostsSyncObjH(
                        pCon,
                        lGsynId,
                        vecHosts.empty()?NULL:&vecHosts[0],
                        vecHosts.size());
        };

    protected:
        void FillHosts(
                    KLDB::DbConnectionPtr   pCon,
                    KLGSYN::host_id_t*      pHosts,
                    size_t                  nHosts,
                    std::vector<long>&      vecHosts)
        {
            if(pHosts && nHosts)
            {
                KLSTD::CAutoPtr<KLSRV::ServerHelpers> pServerHelpers;
                this->QueryInterface(
                            KLSTD_IIDOF(KLSRV::ServerHelpers), 
                            (void**)&pServerHelpers);
                vecHosts.clear();
                vecHosts.reserve(nHosts);
                for(size_t i = 0; i < (size_t)nHosts; ++i)
                {
                    long lHost = pServerHelpers->FindHost(pCon, pHosts[i].host);
                    if( lHost <= 0 )
                        continue;
                    vecHosts.push_back(lHost);
                };
            };
        };
    };
};

#endif //__KL_SRV_GSYN_H__

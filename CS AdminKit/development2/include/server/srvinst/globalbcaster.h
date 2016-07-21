#ifndef __KL_GLOBALBCASTER_H__
#define __KL_GLOBALBCASTER_H__

#include <std/base/kldefs.h>
#include <common/reporter.h>
#include <server/db/dbconnection.h>

#include <server/srvinst/klserver.h>
#include <server/srvinst/connstore.h>
#include <server/srvinst/storages.h>
#include <server/srvinst/srvinstdata.h>
//#include <server/srvinst/klserver.h>

//#include <server/db/importado.h>

namespace KLSRVBCASTER
{
    typedef enum
    {
        GSBF_SSS    = 0x1,  //! SS_SETTINGS 
        GSBF_SSP    = 0x2,  //! SS_PRODINFO
        GSBF_TS     = 0x4,  //! tasks storage
        GSBF_GS     = 0x8,  //! group syncs
        GSBF_TESTGS = 0x10, //! test gsyn
        GSBF_KEYS =   0x20, //! keys
        GSBF_EXTRA =  0x40, //! nlst an so on
        GSBF_GSFORCE = 0x80, //! force group syncs (necessary if product was reinstalled)

        GSBF_ALL    = GSBF_SSS|GSBF_SSP|GSBF_TS|GSBF_GS|GSBF_EXTRA//|GSBF_KEYS
    };


    class KLSTD_NOVTABLE GlobalSrvBroadcaster : public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Marks host for sync

          \param	idHost - host id
          \param	dwFlags - flags GSBF_*
          \param	bMaxPriority - rise priority
        */
        virtual void MarkHostForSync(
                long        idHost,
                AVP_dword   dwFlags,
                bool        bMaxPriority)  = 0;

        virtual void DoPass() = 0;

        /*!
          \brief	Puts stop flag to 'true'
        */
        virtual void Stop() = 0;

        /*!
          \brief	Синхронизировать сейчас указанный хост

          \param	pConn   -   connection with nagent
          \param	data    -   sync data
          \return	false - if host was locked
        */
        virtual bool SynchronizeNow(
                        KLDB::DbConnectionPtr       pCon,
                        KLSRV::NagentConnection*    pNagentCon,
                        long                        lHost,
                        const std::wstring&         wstrHostId,
                        AVP_dword                   dwFlags,
                        KLPAR::Params*              pData,
                        bool                        bForce) = 0;

        virtual size_t GetSize() = 0;
        
        virtual long volatile& get_SyncCnt() = 0;
        virtual long volatile& get_SyncRealCnt() = 0;
        virtual long volatile& get_SyncSucCnt() = 0;
        virtual long volatile& get_SyncJnCnt() = 0;
        virtual long get_SyncQueueLength() = 0;

       
        virtual void Close() = 0;
        
        /*
        virtual bool DoMergePolicies(
                            KLDB::DbConnectionPtr           pCon,
                            long                            lHost,
                            std::wstring                    wstrHostId) = 0;
        */

    };
};

void KLSRVBCASTER_CreateGlobalSrvBroadcaster(
                    KLSRV::SrvData*         pSrvData,
                    KLSRV::CStorages*       pStorages,
                    KLSRV::ServerHelpers*   pServerHelpers,
                    const std::wstring&     wstrServerId,
                    long			        lSyncLifeTime,
		            long			        lSyncLockTime,
		            long			        lSyncSecPacketSize,
                    KLSRV::CertData*        pCertData,
                    KLPAR::BinaryValue*     pPubServerKey,
                    KLPAR::BinaryValue*     pPrvServerKey,
                    const std::wstring&     wstrLocalNagent,
                    KLSRVBCASTER::GlobalSrvBroadcaster**  ppGC);

#endif //__KL_GLOBALBCASTER_H__
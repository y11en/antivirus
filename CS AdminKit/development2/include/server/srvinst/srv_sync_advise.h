/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	srv_sync_advise.h
 * \author	Andrew Kazachkov
 * \date	07.09.2005 16:27:44
 * \brief	
 * 
 */

#ifndef __SRV_SYNC_ADVISE_H__
#define __SRV_SYNC_ADVISE_H__

#include <server/db/dbconnection.h>
#include <server/srvinst/connstore.h>

namespace KLSRV
{
    class KLSTD_NOVTABLE SrvSyncAdviseSink
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Checks whether synchronization is required for 
                    specified host. This method is called from ping 
                    handler.

          \param	lHost IN host id
          \param	pCon IN db connection
          \param    pExtraData IN reserved
          \return	true if synchronization is required
          \exception KLERR::Error*
        */
        virtual bool IsSyncRequired(
                    long                    lHost, 
                    KLDB::DbConnectionPtr   pCon,
                    KLPAR::Params*          pExtraData) = 0;

        /*!
          \brief	DoSync

          \param	lHost IN host id
          \param	pCon IN db connection
          \param	pNagent IN nagent connection
          \exception KLERR::Error*  - failed to synchronize. Exception will 
                    be written to Kaspersky EventLog.
        */
        virtual void DoSync(
                    long                    lHost, 
                    KLDB::DbConnectionPtr   pCon,
                    KLSRV::ConnectionInfo*  pNagent) = 0;
    };

    typedef KLSTD::CAutoPtr<SrvSyncAdviseSink> SrvSyncAdviseSinkPtr;

    KLSTD_DECLARE_HANDLE(HSRVSYNCADVISE)

    class KLSTD_NOVTABLE SrvSyncAdviseControl
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Stores pointer to SrvSyncAdviseSink

          \param	pSink IN pointer to SrvSyncAdviseSink
          \param	hSink OUT sink handle
        */
        virtual void Advise(
                        SrvSyncAdviseSink*  pSink,
                        HSRVSYNCADVISE&     hSink) = 0;

        /*!
          \brief	Removes pointer to SrvSyncAdviseSink. Method doesn't 
                    wait until SrvSyncAdviseSink::* finish execution.

          \param	hSink IN sink handle
          \return   true if success false if no such advise
        */
        virtual bool Unadvise(
                        HSRVSYNCADVISE      hSink) = 0;

        /*!
          \brief	Initiates sync for specified hosts

          \param	plHosts IN pointer to array of hosts
          \param	nHosts IN number of hosts
        */
        virtual void InitiateSyncForHosts(
                            long*   plHosts, 
                            size_t  nHosts) = 0;
    };
};

#endif //__SRV_SYNC_ADVISE_H__

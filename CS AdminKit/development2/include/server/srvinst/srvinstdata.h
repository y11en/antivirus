/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	srvinstdata.h
 * \author	Andrew Kazachkov
 * \date	07.07.2005 14:35:39
 * \brief	
 * 
 */

#include <std/base/klstd.h>
#include <common/reporter.h>
#include <std/thr/locks.h>
#include <std/tmstg/timeoutstore.h>
#include <kca/prci/componentid.h>
#include <common/klaof.h>

#ifndef __KLSRVINSTDATA_H__2
#define __KLSRVINSTDATA_H__2

#define KLSRV_EXTCALL()    \
    KLSTD::CAutoObjectLock srvacc(m_pSrvData->m_lckExternal);    \
    if(!srvacc) \
    {   \
        KLSTD_THROW_APP_PENDING();   \
    };

#define KLSRV_AUTOPTR(_name, _type, _val)   \
    KLSTD::CAutoPtr<_type> _name;   \
    _val->QueryInterface(KLSTD_IIDOF(_type),(void**)&_name);    \
    KLSTD_ASSERT_THROW(_name);


#define KLSRV_AOF_EXTCALL()    \
    KLSTD::CAutoObjectLock srvacc(m_pAOF_Data->m_lckExternal);    \
    if(!srvacc) \
    {   \
        KLSTD_THROW_APP_PENDING();   \
    };

namespace KLSRV
{
    const KLDB::dbconn_type_t c_nDbCatGuiCall = KLDB::DBCT_CATEGORY_GUI;

    class SrvData
    {
    public:
        SrvData(
                KLDB::DbConnStoragePtr      pDbConnStorage,
                KLTMSG::TimeoutStore2*      pTimeoutStore,
                CONTIE::ConnectionTiedObjectsHelper*
                                            pTiedObjects,
                KLSTD::KLBaseQI*            pServer,
                KLSTD::ObjectLock&          lckExternal,
                long                        lDbTmtGuiCall,
                KLSTD::Reporter*            pReporter,
                const KLPRCI::ComponentId&  idServer,
                const KLPRCI::ComponentId&  idServerForSlaves,
                long                        lTrRemoteSndRcvTime);
        KLDB::DbConnectionPtr AcquireDbForGuiCall();
    public:
        //!db connection storage
        KLDB::DbConnStoragePtr  m_pDbConnStorage;
        //!
        KLDB::DbLiteralsPtr     m_pLiterals;
        
        //!lock for external (via gsoap) calls
        KLSTD::ObjectLock&      m_lckExternal;        

        //!timeout for accessing db for gui
        long                    m_lDbTmtGuiCall;

        //!timeout storage
        KLSTD::CAutoPtr<KLTMSG::TimeoutStore2>  m_pTimeoutStore;

        //!connection-related storage
        KLSTD::CAutoPtr< CONTIE::ConnectionTiedObjectsHelper>
                                        m_pTiedObjects;
        
        //!Reporter ptr. May be NULL !!!
        KLSTD::Reporter*        m_pReporter;
        
        const KLPRCI::ComponentId     m_idServer;

        const KLPRCI::ComponentId     m_idServerForSlaves;

        long                          m_lTrRemoteSndRcvTime;

        //!Server instance pointer
        KLSTD::KLBaseQI*            m_pServer;
    };

    typedef KLAOF::AOF_AttachedBase<SrvData>    SrvModuleBase;
    typedef KLAOF::AOF_MainBase<SrvData>        SrvMainBase;
}

#endif //__KLSRVINSTDATA_H__2

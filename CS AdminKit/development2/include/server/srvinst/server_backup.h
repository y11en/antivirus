#ifndef __SERVER_BACKUP_H__
#define __SERVER_BACKUP_H__

#include <server/srvinst/server_constants.h>
#include <srvp/evp/notificationproperties.h>
#include <cisco/cisco_const.h>
#include <agent/aginst/agent_info.h>

namespace KLSRVBACKUP
{
    typedef const wchar_t* klbcpstr_t;

    struct section_t
    {
        klbcpstr_t  m_szwProduct;
        klbcpstr_t  m_szwVersion;
        klbcpstr_t  m_szwSection;  
        
        //! array terminated by NULL pointer
        //! zero m_pszVariables means backing up section as a whole 
        const klbcpstr_t* m_pszVariables;
    };

    const klbcpstr_t c_Vars_KLSRV_COMMON_SETTINGS[]=
    {
        KLSRV_SP_REMOTE_CONNECT_TIMEOUT,
        KLSRV_SP_REMOTE_SNDRCV_TIMEOUT,
        KLSRV_SP_REMOTE_UNICAST_TIMEOUT,
        KLSRV_SP_LOCAL_CONNECT_TIMEOUT,
        KLSRV_SP_LOCAL_SNDRCV_TIMEOUT,

        KLSRV_ANY_SCAN_ENABLED,
        KLSRV_NET_SCAN_ENABLED,
        KLSRV_SP_FASTUPDATENET_PERIOD,
        KLSRV_SP_FULLUPDATENET_PERIOD,
        KLSRV_SP_SCAN_AD,
        KLSRV_AD_SCAN_ENABLED,
        KLSRV_DPNS_SCAN_PERIOD,
        KLSRV_DPNS_SCAN_ENABLED,

        KLSRV_SP_SYNC_LIFETIME,
        KLSRV_SP_SYNC_LOCKTIME,
        KLSRV_SP_SERVER_PORTS_ARRAY,
        KLSRV_SP_SERVER_SSL_PORTS_ARRAY,
        KLSRV_SP_SERVER_AKLWNGT_PORTS_ARRAY,
        KLSRV_SP_OPEN_AKLWNGT_PORT,
        KLSRV_SP_INSTANCE_ID,
        KLEVP::c_NLMaxEventsToSendPerPeriod,
        KLEVP::c_NLTestPeriodToSendEvent,
        KLEVP::c_NLMaxVirusEventsForOutbreakFiles,
        KLEVP::c_NLMaxVirusEventsForOutbreakEmail,
        KLEVP::c_NLMaxVirusEventsForOutbreakPerim,
        KLEVP::c_NLTestPeriodToOutbreakFiles,
        KLEVP::c_NLTestPeriodToOutbreakEmail,
        KLEVP::c_NLTestPeriodToOutbreakPerim,
        KLSRV_SP_MAX_EVENTS_IN_DB,
        NULL
    };

    const klbcpstr_t c_Vars_KLSRV_HOSTSTATUS_SECTION[]={
        KLSRV_VISIBILITY_TIMEOUT,
        KLSRV_HSTSTAT_CRITICAL,
        KLSRV_HSTSTAT_WARNING,
        NULL
    };

    const klbcpstr_t c_Vars_KLSRV_MASTER_SRV_SECTION[] = {
        KLSRV_MASTER_SRV_USE,
        KLSRV_MASTER_SRV_SSL_CERT,
        KLSRV_MASTER_SRV_ADDR,
        KLSRV_MASTER_SRV_PORTS,
        KLSRV_MASTER_SRV_INST_ID,
        KLSRV_MASTER_SRV_VER,
        KLSRV_MASTER_SRV_PROXYHOST_USE,
        KLSRV_MASTER_SRV_PROXYHOST_LOCATION,
        KLSRV_MASTER_SRV_PROXYHOST_LOGIN,
        KLSRV_MASTER_SRV_PROXYHOST_PASSWORD,
        NULL
    };


    //! variables from SS_SETTINGS to backup
    const section_t c_Sections[] = 
    {
        {KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER, KLSRV_CONSOLE_SERVER_INIT_SECTION, NULL},
        {KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER, KLSRV_COMMON_SETTINGS, c_Vars_KLSRV_COMMON_SETTINGS},
        {KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER, KLSRV_HOSTSTATUS_SECTION, c_Vars_KLSRV_HOSTSTATUS_SECTION},
        {KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER, KLSRV_MASTER_SRV_SECTION, c_Vars_KLSRV_MASTER_SRV_SECTION},
		{KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER, KLCISCO_SETTINGS, NULL}
    };

    //! variables from Administration server's private storage to backup
    const section_t c_AdmSrv_PrivSections[] = 
    {
        {KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER, KLAG_TSID_2_ACTION_ID_SECTION, NULL}
    };
};

#endif //__SERVER_BACKUP_H__

/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	server_constants.h
 * \author	Andrew Kazachkov
 * \date	29.04.2003 12:39:05
 * \brief	
 * 
 */

#ifndef __KLSERVER_CONSTANTS_H__
#define __KLSERVER_CONSTANTS_H__

#include <std/stress/st_prefix.h>
#include "./server_ports.h"

//!AdmServer mutex name
#define MUTEX_NAME_ADMSERVER          _T("klserver-rwidfgvhwjif")
#define MUTEX_TS_NAME_ADMSERVER       KLWTS_GLOBAL MUTEX_NAME_ADMSERVER

//!Admserver service name
#define KLSRV_SERVICE_NAME                  L"CSAdminServer"

#define KLSRV_FTI_IS_COMPLETED              L"FTICompleted"
#define KLSRV_FTI_UPGRD_NEEDED              L"NeedUpgrade"

//! Server flag. 
#define KLSRV_LAST_STATUSLEGACY         L"SrvLegacyHstStatCRC"
#define KLSRV_LAST_STATUSLEGACY_DEF     AVP_dword(0)

//! Server flag. Set if the network was fastscanned al least once
#define KLSRV_NETWORK_WAS_SCANNED           L"NScanned"

//! Server flag. Last fast scan.
#define KLSRV_NETSCAN_LASTFAST              L"NetScan_LastFast"

//! Server flag. Last full scan.
#define KLSRV_NETSCAN_LASTFULL              L"NetScan_LastFull"

//! Server flag. Last ad scan.
#define KLSRV_NETSCAN_LASTAD                L"NetScan_LastAd"

//! Server flag. Last dpns scan.
#define KLSRV_NETSCAN_LASTDPNS              L"NetScan_LastDpns"

//! Server flag. Set if package folder permissions were adjusted
#define KLSRV_PKGFOLDER_WAS_ADJUSTED        L"PkgDirAdjusted"

//! Server flag. This value indicates old version before upgrade
#define KLSRV_UPGRD_FIRST_START             L"UpgdFirstStart"
#define KLSRV_UPGRD_VERSION_RELEASE         1
#define KLSRV_UPGRD_VERSION_MP1             2
#define KLSRV_UPGRD_VERSION_MP2             3
#define KLSRV_UPGRD_VERSION_MP3             4
#define KLSRV_UPGRD_VERSION_6               5

//! Server flag. Upgrade after restore previous version from backup occured
#define KLSRV_UPGRD_AFTER_RESTORE           L"UpgdAfterRestore"

//! Server flag. Restore have ben processed, low-level processing is required
#define KLSRV_AFTER_ANY_RESTORE_LL          L"SrvRestoreDoneLl"

//! Server flag. Restore from backup of old version occured 
#define KLSRV_AFTER_RESTORE           L"AfterRestore"

//! Server flag. Mp1 was installed (bug in installer of MP1 must be fixed)
#define KLSRV_UPGRD_FIX_MP1RC               L"UpgdFixMP1RC"

//! Server flag. Db recovery mode must be switched into SIMPLE
#define KLSRV_UPGRD_FIX_RECOVERY_MODE       L"UpgdFixRecoveryMode"

//! Server flag. 
#define KLSRV_ALTER_DB_SQL2005              L"AlterDb_SQL2005"
#define KLSRV_ALTER_DB_SQL2005_DEF          false

//! Server flag. Old server policy is upgraded
#define KLSRV_UPGRD_OLD_SRVPOL_UPGRADED     L"UpgdOldSrvPolDone"
#define KLSRV_UPGRD_OLD_SRVPOL_UPGRADED_DEF 0L

//! Server flag. Old server policy is upgraded
#define KLSRV_UPGRD_SRVPOL_MODIFY_FIXED     L"UpgdSrvPolModifyFixed"
#define KLSRV_UPGRD_SRVPOL_MODIFY_FIXED_DEF 0L

//! Server flag. Time when "DiskFull" was last published.
#define KLSRV_LASTPUB_DISKFULL          L"SpcNoDiskSpace"

//! Server flag. Time when "DbFull" was last published.
#define KLSRV_LASTPUB_DBFULL            L"SpcNoDbSpace"

//! Server flag. Time when "Not enough disk space" was last published.
#define KLSRV_LASTPUB_DISKSPACE         L"SpcDiskSpace"

//! Server flag. Time when "Not enough db space" was last published.
#define KLSRV_LASTPUB_DBSPACE           L"SpcDbSpace"

//! Server flag. Time when licensing event was last published.
#define KLSRV_LASTPUB_LIC_EVENT         L"LicLastPublished"

//! Server flag. Set if master server was completely adjusted
#define KLSRV_MASTER_INTEGRATED         L"SrvhrchIntegrated"

#define KLSRV_MASTER_PUBKEY     (("MASTER-PUB-25A13042F52348f08E3673B483467AF2" + KLSTD_StGetPrefixA()).c_str())
#define KLSRV_MASTER_PRVKEY     (("MASTER-PRV-25A13042F52348f08E3673B483467AF2" + KLSTD_StGetPrefixA()).c_str())

//! Server flag. Max amount of socket connections.
#define KLSRV_MAX_CONNECTIONS               L"TrMaxConnections"
#define KLSRV_MAX_CONNECTIONS_DEFAULT       1500

//! Server flag. Max amount of hosts with "keep" connections set.
#define KLSRV_MAX_KEEP_CONNECTION           L"MaxKeepConnection"
#define KLSRV_MAX_KEEP_CONNECTION_DEFAULT   100

//! Server flag. If set then the network scan must be turned off.
#define KLSRV_NO_NETWORK_SCAN               L"NoNetworkScan"
#define KLSRV_NO_NETWORK_SCAN_DEFAULT       false

//! Server flag. If set then full network scan must be turned off.
#define KLSRV_NO_FULL_NETWORK_SCAN               L"NoFullNetworkScan"
#define KLSRV_NO_FULL_NETWORK_SCAN_DEFAULT       false

//! Server flag. Turns on/off settings modification by policy
#define KLSRV_POL_MAY_MODIFY_SETTINGS            L"SrvPolMayModifySettings"
#define KLSRV_POL_MAY_MODIFY_SETTINGS_DEFAULT    true

//! Server flag. Delay after each ss modification by policy
#define KLSRV_POL_MODIFY_SETTINGS_DELAY             L"SrvPolModifySettingsDelay"
#define KLSRV_POL_MODIFY_SETTINGS_DELAY_DEFAULT     AVP_dword(200)


//! Server flag. Delay during network scan.
#define KLSRV_NETWORKSCAN_DELAY             L"NetworkScanDelay"
#define KLSRV_NETWORKSCAN_DELAY_DEFAULT     500
#define KLSRV_NETWORKSCAN_DELAY_MAX         10000

//! Server flag. Wait timeout for ss while processing ping
#define KLSRV_PING_SS_WAIT                  L"SrvPingSsWait"
#define KLSRV_PING_SS_WAIT_DEFAULT          10000

//! Server flag. Max amount of pings in ping queue.
#define KLSRV_PING_LIMIT                    L"SrvPingLimit"
#define KLSRV_PING_LIMIT_DEFAULT            50

//! Server flag.
#define KLSRV_FORCE_CONN_CLOSE              L"SrvForceConnClose"
#define KLSRV_FORCE_CONN_CLOSE_DEFAULT      false

//! Server flag. 
#define KLSRV_HSTAV_MAXUNAV_PERIOD          L"HstAvMaxUnavPeriod"
#define KLSRV_HSTAV_MAXUNAV_PERIOD_DEFAULT  7200000

//! Server flag. 
#define KLSRV_REMOVE_OLD_NAME_PERIOD            L"SrvRemoveOldNamePeriod" //seconds
#define KLSRV_REMOVE_OLD_NAME_PERIOD_DEFAULT    (3600*24)

//! Server flag.
#define KLSRV_HSTAV_ICMP_PERIOD             L"HstIcmpPeriod"
#define KLSRV_HSTAV_ICMP_PERIOD_DEFAULT     180000

//! Server flag.
#define KLSRV_HSTAV_ICMP_TIMEOUT             L"HstIcmpTimeout"
#define KLSRV_HSTAV_ICMP_TIMEOUT_DEFAULT     3000

//! Server flag.
#define KLSRV_HSTAV_ICMP_MAXPINGS           L"HstIcmpMaxPings"
#define KLSRV_HSTAV_ICMP_MAXPINGS_DEFAULT   3

//! Server flag.
#define KLSRV_HSTAD_SCAN_PERIOD             L"SrvADScanPeriod"
#define KLSRV_HSTAD_SCAN_PERIOD_DEFAULT     3600000UL

//! Server flag.
#define KLSRV_HSTAD_INCLUDE_CONTAINERS          L"SrvADScanIncludeContainers"
#define KLSRV_HSTAD_INCLUDE_CONTAINERS_DEFAULT  true

//! Server flag.
#define KLSRV_NETS_MAX_THREADS      L"SrvSDA_MaxThreads"
#define KLSRV_NETS_MAX_THREADS_DEF  AVP_dword(5)

//! Server flag.
#define KLSRV_NETS_PING_TIMEOUT     L"SrvSDA_IcmpTimeout"
#define KLSRV_NETS_PING_TIMEOUT_DEF AVP_dword(1000)

//! Server flag.
#define KLSRV_NETS_PING_PACKETS     L"SrvSDA_IcmpPings"
#define KLSRV_NETS_PING_PACKETS_DEF AVP_dword(1)

//! Server flag.
#define KLSRV_NETS_PING_SLEEP       L"SrvSDA_IcmpSleep"
#define KLSRV_NETS_PING_SLEEP_DEF   AVP_dword(0)

//! Server flag.
#define KLSRV_DPNS_EXTRA_SLEEP       L"SrvSDA_DpnsExtraSleep"
#define KLSRV_DPNS_EXTRA_SLEEP_DEF   AVP_dword(2000)

//! 
#define KLSRV_FULL_SCAN_MAX_THREADS     L"SrvFullScan_MaxThreads"
#define KLSRV_FULL_SCAN_MAX_THREADS_DEF AVP_dword(5)

#define KLSRV_FAST_SCAN_MAX_THREADS     L"SrvFastScan_MaxThreads"
#define KLSRV_FAST_SCAN_MAX_THREADS_DEF AVP_dword(5)

#define KLSRV_FAST_SCAN_USE_AD     L"SrvFastScan_UseAD"
#define KLSRV_FAST_SCAN_USE_AD_DEF AVP_dword(true)

//! Server flag.
#define KLSRV_NETS_PROCESS_UNRESOLVED   L"SrvSDA_ProcessNakedIps"
#define KLSRV_NETS_PROCESS_UNRESOLVED_DEF   false

//! Server flag.
#define KLSRV_DPNS_PINGS_PER_THREAD         L"SrvSDA_PingsPerThread"
#define KLSRV_DPNS_PINGS_PER_THREAD_DEF     AVP_dword(16)

//! Server flag.
#define KLSRV_NETSCAN_USE_MAC       L"SrvNetScan_UseMAC"
#define KLSRV_NETSCAN_USE_MAC_DEF   true

//! Server flag. Turning on/off writing policy to data
#define KLSRV_MAY_MERGE_POLICY      L"PolMayChangeData"
#define KLSRV_MAY_MERGE_POLICY_DEF  true

/*! Server flag. Extra checking for resolving ip addresses to names
*/
#define KLSRV_INVRES_EXTRA_CHECK        L"SrvInvResExtraCheck"
#define KLSRV_INVRES_EXTRA_CHECK_DEF    false

/*! Server flag. Timeout while extra checking for resolving ip addresses to names
*/
#define KLSRV_INVRES_EXTRA_CHECK_TIMEOUT        L"SrvInvResExtraCheckTimeout"
#define KLSRV_INVRES_EXTRA_CHECK_TIMEOUT_DEF    AVP_dword(0)

/*! Server flag. Turnes on reporting in case of address resolution errors
*/
#define KLSRV_INVRES_REPORT             L"SrvInvResReportErrors"
#define KLSRV_INVRES_REPORT_DEF         false

//! Server flag. Fixes transport bug (transport callback may be called in improper order). For stress-tests only !!!
#define KLSRV_FIX_TR_CALLBACK_ORDER     L"SrvFixTrCallbacksOrder"
#define KLSRV_FIX_TR_CALLBACK_ORDER_DEF false

//! Server flag. Turns off ss caching
#define KLSRV_FLAG_NO_SS_CACHING            L"SrvNoSsCaching"
#define KLSRV_FLAG_NO_SS_CACHING_DEFAULT    false

//! Server flag. Adjust size of ss cache
#define KLSRV_FLAG_LWNGT_THREADS            L"SrvLwNgtThreads"
#define KLSRV_FLAG_LWNGT_THREADS_DEFAULT    40L

//! Server flag. Adjusts period of extended rules execution, ms; zero turns off
#define KLSRV_FLAG_EXRULES_FULL_PERIOD           L"SrvExRulesFullPeriod"
#define KLSRV_FLAG_EXRULES_FULL_PERIOD_DEFAULT   (20L*60L*1000L) // 20 minutes

//! Server flag. Adjusts period of group check for extended rules execution, ms; zero turns off
#define KLSRV_FLAG_EXRULES_GRPCHK_PERIOD           L"SrvExRulesGrpChkPeriod"
#define KLSRV_FLAG_EXRULES_GRPCHK_PERIOD_DEFAULT   (5L*1000L) // 5 seconds

//! Server flag. Autokeys recalc period
#define KLSRV_FLAG_AUTOKEY_RECALC_PRD       L"SrvAutoKeyRecalcPrd"
#define KLSRV_FLAG_AUTOKEY_RECALC_PRD_DEFAULT   (6L*60L*1000L) // 6 minutes

#define KLSRV_SERVER_FLAGS_SUBKEY           L"ServerFlags"

#define KLSRV_ERRIGNORE KLERR_IGNORE

#define KLSRV_INTERNAL_FILES_SECTION    KLCS_COMPONENT_SERVER
#define KLSRV_INTERNAL_FILES            L"KLSRV_INTERNAL_FILES"
#define KLSRV_POLICY_PATH               L"KLSRV_POLICY_PATH"

#define KLSRV_SSL_PRIVKEY_PSWD_ID       ((L"L$EE34595D-B89D-4e70-B9CB-178A7E8CEF45" + KLSTD_StGetPrefixW()).c_str())

#define KLSRV_SSL_SERVER_CERT           L"KLSRV_SSL_SERVER_CERT"    //STRING_T (filename) or BINARY_T
#define KLSRV_SSL_SERVER_CERT_DEFAULT   (KLSTD_StGetDefDirW() + L"klsrv.cer")

#define KLSRV_SSL_SERVER_PRIVKEY            L"KLSRV_SSL_SERVER_PRIVKEY" //STRING_T (filename) or BINARY_T
#define KLSRV_SSL_SERVER_PRIVKEY_DEFAULT    (KLSTD_StGetDefDirW() + L"klsrv.prk")


//! Главная секция настроек
#define KLSRV_COMMON_SETTINGS   KLCS_COMPONENT_SERVER

//! Открывать порт для мобильных клиентов, BOOL_T
#define KLSRV_SP_OPEN_AKLWNGT_PORT              L"KLSRV_SP_OPEN_AKLWNGT_PORT"
#define KLSRV_SP_OPEN_AKLWNGT_PORT_DEFAULT      false

//! Файл локализации
#define KLSRV_SP_LOCFILE            L"KLSRV_SP_LOCFILE"
#define KLSRV_SP_LOCFILE_DEFAULT    L"srvloc.dll"

#define KLSRV_DBCONN_REPORT_TIMEOUT_DEFAULT     (10*60*1000)

//! Удалённый транспортный таймаут коннекта
#define KLSRV_SP_REMOTE_CONNECT_TIMEOUT         L"KLSRV_SP_REMOTE_CONNECT_TIMEOUT"
#define KLSRV_SP_REMOTE_CONNECT_TIMEOUT_DEFAULT 30000

//! Удалённый транспортный таймаут send/receive
#define KLSRV_SP_REMOTE_SNDRCV_TIMEOUT          L"KLSRV_SP_REMOTE_SNDRCV_TIMEOUT"
#define KLSRV_SP_REMOTE_SNDRCV_TIMEOUT_DEFAULT  180000

//! Удалённый транспортный таймаут unicast
#define KLSRV_SP_REMOTE_UNICAST_TIMEOUT         L"KLSRV_SP_REMOTE_UNICAST_TIMEOUT"
#define KLSRV_SP_REMOTE_UNICAST_TIMEOUT_DEFAULT 30000

//! Удалённый транспортный таймаут коннекта
#define KLSRV_SP_LOCAL_CONNECT_TIMEOUT          L"KLSRV_SP_LOCAL_CONNECT_TIMEOUT"
#define KLSRV_SP_LOCAL_CONNECT_TIMEOUT_DEFAULT 10000

//! Удалённый транспортный таймаут send/receive
#define KLSRV_SP_LOCAL_SNDRCV_TIMEOUT           L"KLSRV_SP_LOCAL_SNDRCV_TIMEOUT"
#define KLSRV_SP_LOCAL_SNDRCV_TIMEOUT_DEFAULT   INT_MAX

//! Разрешено сканирование сети 
#define KLSRV_ANY_SCAN_ENABLED             L"KLSRV_ANY_SCAN_ENABLED"
#define KLSRV_ANY_SCAN_ENABLED_DEFAULT     true

//! Сканирование сети MS
    //! Разрешено сканирование сети MS
    #define KLSRV_NET_SCAN_ENABLED             L"KLSRV_NET_SCAN_ENABLED"
    #define KLSRV_NET_SCAN_ENABLED_DEFAULT     true

    //! Период быстрого обновления сети
    #define KLSRV_SP_FASTUPDATENET_PERIOD   L"KLSRV_SP_FASTUPDATENET_PERIOD"
    #define KLSRV_SP_FASTUPDATENET_PERIOD_DEFAULT   (15*60*1000)  // 15 min

    //! Период полного обновления сети
    #define KLSRV_SP_FULLUPDATENET_PERIOD   L"KLSRV_SP_FULLUPDATENET_PERIOD"
    #define KLSRV_SP_FULLUPDATENET_PERIOD_DEFAULT   (60*60*1000) // 60 min

//! Сканирование AD
    //! Период обновления информации из AD
    #define KLSRV_SP_SCAN_AD                        L"KLSRV_SP_SCAN_AD"
    #define KLSRV_SP_SCAN_AD_DEFAULT                (60*60*1000) // 60 min

    //! Разрешено сканирование AD
    #define KLSRV_AD_SCAN_ENABLED             L"KLSRV_AD_SCAN_ENABLED"
    #define KLSRV_AD_SCAN_ENABLED_DEFAULT     true

//! Сканирование диапазонов ip-адресов
    //! Период сканирования диапазонов ip-адресов
    #define KLSRV_DPNS_SCAN_PERIOD              L"KLSRV_SP_DPNS_PERIOD"
    #define KLSRV_DPNS_SCAN_PERIOD_DEFAULT      (7*60*60*1000)

    //! Разрешено сканирование диапазонов ip-адресов
    #define KLSRV_DPNS_SCAN_ENABLED             L"KLSRV_SP_DPNS_ENABLE"
    #define KLSRV_DPNS_SCAN_ENABLED_DEFAULT     true

//! Максимальное время жизни синхронизации 
#define KLSRV_SP_SYNC_LIFETIME          L"KLSRV_SP_SYNC_LIFETIME"
#define KLSRV_SP_SYNC_LIFETIME_DEFAULT  180000  // 3 min

//! Максимальное время блокировки синхронизацией
#define KLSRV_SP_SYNC_LOCKTIME          L"KLSRV_SP_SYNC_LOCKTIME"
#define KLSRV_SP_SYNC_LOCKTIME_DEFAULT  30000   // 30 s

//! По сколько секций переcылать за 1 цикл синхронизации
#define KLSRV_SP_SYNC_SEC_PACKET_SIZE   L"KLSRV_SP_SYNC_SEC_PACKET_SIZE"
#define KLSRV_SP_SYNC_SEC_PACKET_SIZE_DEFAULT   100

//! Корневая папка синхронизации
#define KLSRV_SP_GSYNC_ROOT             L"KLSRV_SP_GSYNC_ROOT"
#define KLSRV_SP_GSYNC_ROOT_DEFAULT     L"DbgServer"

//! Приходящие с управляющего сервера
#define KLSRV_SP_SUPER_GSYNC_ROOT             L"KLSRV_SP_GSYNC_SUPER_ROOT"
#define KLSRV_SP_SUPER_GSYNC_ROOT_DEFAULT     L"GsynSuper"

//! Корневая папка синхронизированных хранилищ
#define KLSRV_SP_STORES_ROOT            L"KLSRV_SP_STORES_ROOT"
#define KLSRV_SP_STORES_ROOT_DEFAULT    L"Storages"

//! Максимальное число хранимых евентов в базе данных
#define KLSRV_SP_MAX_EVENTS_IN_DB       L"KLSRV_SP_MAX_EVENTS_IN_DB"
#define KLSRV_SP_MAX_EVENTS_IN_DB_DEFAULT   400000

#define KLSRV_DEF_SS_SERVER             KLSSS::c_szwDefaultServerId
#define KLSRV_SP_SERVERID               L"KLSRV_SP_SERVERID"

//! Уникальный InstanceId, используемый для соединений между серверами в иерархии.
#define KLSRV_SP_INSTANCE_ID			L"KLSRV_SP_INSTANCE_ID"

#define KLSRV_SHARE_SECTION             L"Packages"
#define KLSRV_SHARE_LOCAL_PATH          L"LocalPackagesRoot"
#define KLSRV_SHARE_NETWORK_PATH        L"NetworkPackagesRoot"
#define KLSRV_NEW_SHARE_NAME            L"NewShareName"

#define KLSRV_RETRANSLATION_FOLDER_NAME L"Updates"

#define KLSRV_EVP_SEVERITY  KLEVP::c_er_severity
#define KLSRV_EVP_STDID     KLEVP::c_er_locid
#define KLSRV_STDPAR1       KLEVP::c_er_par1
#define KLSRV_STDPAR2       KLEVP::c_er_par2
#define KLSRV_STDPAR3       KLEVP::c_er_par3
#define KLSRV_STDPAR4       KLEVP::c_er_par4
#define KLSRV_STDPAR5       KLEVP::c_er_par5
#define KLSRV_STDPAR6       KLEVP::c_er_par6
#define KLSRV_STDPAR7       KLEVP::c_er_par7
#define KLSRV_STDPAR8       KLEVP::c_er_par8
#define KLSRV_STDPAR9       KLEVP::c_er_par9
#define KLSRV_STDDESCR      KLEVP::c_er_descr
#define KLSRV_STDDN         KLEVP::c_er_event_type_display_name


//! section in ss_prodinfo (KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER, KLSRV_ADDRESSES_SECTION)
#define KLSRV_ADDRESSES_SECTION L"KLSRV_SRVADDRESS"

//! section in ss_prodinfo (KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER, KLSRV_OLDADDRESSES_SECTION)
#define KLSRV_OLDADDRESSES_SECTION L"KLSRV_SRVADDRESS_OLD"

//! array of address strings
#define KLSRV_SRV_ADDRESSES_ARRAY   L"KLSRV_SRV_ADDRESSES_ARRAY"


/*! 
    Настройки статусов хостов
    Структура настроек
        KLSRV_VISIBILITY_TIMEOUT
        KLSRV_HSTSTAT_CRITICAL
            KLSRV_HSTSTAT_MASK
            KLSRV_HSTSTAT_SP_VIRUSES
            KLSRV_HSTSTAT_SP_RPT_DIFFERS
            KLSRV_HSTSTAT_OLD_FSCAN
            KLSRV_HSTSTAT_OLD_AV_BASES
            KLSRV_HSTSTAT_OLD_LAST_CONNECT
            KLSRV_HSTSTAT_SP_UNCURED
        KLSRV_HSTSTAT_WARNING
            KLSRV_HSTSTAT_MASK
            KLSRV_HSTSTAT_SP_VIRUSES
            KLSRV_HSTSTAT_SP_RPT_DIFFERS
            KLSRV_HSTSTAT_OLD_FSCAN
            KLSRV_HSTSTAT_OLD_AV_BASES
            KLSRV_HSTSTAT_OLD_LAST_CONNECT
            KLSRV_HSTSTAT_OLD_LICENSE
            KLSRV_HSTSTAT_SP_UNCURED
            
    Parameter KLSRV_HSTSTAT_SP_RPT_DIFFERS must contain
    either one of KLBL::GlobalRptState enumeration members or
    any of KLHST::RtpStateMask bits
*/

//! Период после последнего коннекта (мин), по истечении которого хост считается невидимым
#define KLSRV_VISIBILITY_TIMEOUT    L"KLSRV_VISIBILITY_TIMEOUT" //INT_T
#define KLSRV_VISIBILITY_TIMEOUT_DEFAULT    60L

#define KLSRV_HOSTSTATUS_SECTION    L"KLSRV_HOST_STATUS"
#define KLSRV_HSTSTAT_CRITICAL      L"KLSRV_HSTSTAT_CRITICAL"   //PARAMS_T
#define KLSRV_HSTSTAT_WARNING       L"KLSRV_HSTSTAT_WARNING"    //PARAMS_T

//!Status settings are inherited 
#define KLSRV_HSTSTAT_INHERITED         L"KLSRV_HSTSTAT_INHERITED" //BOOL_T

//!Status settings may be inherited 
#define KLSRV_HSTSTAT_INHERITABLE       L"KLSRV_HSTSTAT_INHERITABLE" //BOOL_T

//!Status settings are locked 
#define KLSRV_HSTSTAT_LOCKED            L"KLSRV_HSTSTAT_LOCKED" //BOOL_T

#define KLSRV_HSTSTAT_MASK              L"KLSRV_HSTSTAT_MASK"               //INT_T
#define KLSRV_HSTSTAT_SP_VIRUSES        L"KLSRV_HSTSTAT_SP_VIRUSES"         //INT_T
#define KLSRV_HSTSTAT_SP_RPT_DIFFERS    L"KLSRV_HSTSTAT_SP_RPT_DIFFERS"     //INT_T
#define KLSRV_HSTSTAT_OLD_FSCAN         L"KLSRV_HSTSTAT_OLD_FSCAN"          //INT_T
#define KLSRV_HSTSTAT_OLD_AV_BASES      L"KLSRV_HSTSTAT_OLD_AV_BASES"       //INT_T
#define KLSRV_HSTSTAT_OLD_LAST_CONNECT  L"KLSRV_HSTSTAT_OLD_LAST_CONNECT"   //INT_T
#define KLSRV_HSTSTAT_OLD_LICENSE       L"KLSRV_HSTSTAT_OLD_LICENSE"        //INT_T
#define KLSRV_HSTSTAT_SP_UNCURED        L"KLSRV_HSTSTAT_SP_UNCURED"         //INT_T

#define KLSRV_HSTSTAT_MASK_CRITICAL_DEFAULT             503L
#define KLSRV_HSTSTAT_SP_VIRUSES_CRITICAL_DEFAULT       0L
#define KLSRV_HSTSTAT_SP_RPT_DIFFERS_CRITICAL_DEFAULT   0xF80000L
#define KLSRV_HSTSTAT_OLD_FSCAN_CRITICAL_DEFAULT        14L
#define KLSRV_HSTSTAT_OLD_AV_BASES_CRITICAL_DEFAULT     14L
#define KLSRV_HSTSTAT_OLD_LAST_CONNECT_CRITICAL_DEFAULT 14L
#define KLSRV_HSTSTAT_OLD_LICENSE_CRITICAL_DEFAULT      0L
#define KLSRV_HSTSTAT_SP_UNCURED_CRITICAL_DEFAULT       0L

#define KLSRV_HSTSTAT_MASK_WARNING_DEFAULT              496L
#define KLSRV_HSTSTAT_SP_VIRUSES_WARNING_DEFAULT        0L
#define KLSRV_HSTSTAT_SP_RPT_DIFFERS_WARNING_DEFAULT    0xF80000L
#define KLSRV_HSTSTAT_OLD_FSCAN_WARNING_DEFAULT         7L
#define KLSRV_HSTSTAT_OLD_AV_BASES_WARNING_DEFAULT      7L
#define KLSRV_HSTSTAT_OLD_LAST_CONNECT_WARNING_DEFAULT  7L
#define KLSRV_HSTSTAT_OLD_LICENSE_WARNING_DEFAULT       7L
#define KLSRV_HSTSTAT_SP_UNCURED_WARNING_DEFAULT        0L


#define KLSRV_MASTER_SRV_SECTION	L"KLSRV_MASTER_SRV"
	#define KLSRV_MASTER_SRV_USE	L"KLSRV_MASTER_SRV_USE"	//BOOL_T
	#define KLSRV_MASTER_SRV_SSL_CERT	L"KLSRV_MASTER_SRV_SSL_CERT"	//BINARY_T
	#define KLSRV_MASTER_SRV_ADDR		L"KLSRV_MASTER_SRV_ADDR"		//STRING_T
	#define KLSRV_MASTER_SRV_PORTS		L"KLSRV_MASTER_SRV_PORTS"		//ARRAY_T (INT_T)
	#define KLSRV_MASTER_SRV_INST_ID	L"KLSRV_MASTER_SRV_INST_ID"		//STRING_T
	#define KLSRV_MASTER_SRV_VER		L"KLSRV_MASTER_SRV_VER"			//STRING_T
	#define KLSRV_MASTER_SRV_PROXYHOST_USE		L"KLSRV_MASTER_SRV_PROXYHOST_USE"	//BOOL_T
	#define KLSRV_MASTER_SRV_PROXYHOST_LOCATION		L"KLSRV_MASTER_SRV_PROXYHOST_LOCATION"	//STRING_T
	#define KLSRV_MASTER_SRV_PROXYHOST_LOGIN		L"KLSRV_MASTER_SRV_PROXYHOST_LOGIN"		//STRING_T
	#define KLSRV_MASTER_SRV_PROXYHOST_PASSWORD		L"KLSRV_MASTER_SRV_PROXYHOST_PASSWORD"	//BINARY_T

// В секции хранится признак необходимости запуска Quick Start Wizard'a
#define KLSRV_CONSOLE_SERVER_INIT_SECTION	L"KLSRV_CONSRVINIT"	
	#define KLSRV_CONSOLE_SERVER_INIT	L"KLSRV_CONSRVINIT_VAL"		// true - если QSW уже запускался


//! Section contains some default for the network agent auto-created package
    #define KLSRV_DEFAULT_NAGENT_PACKAGE_ADDRESS L"KLSRV_DEF_NAGENT_PACKAGE"	
    //! Section may conatin following variables
    //  KLSRV_DEFAULT_NAGENT_PACKAGE_ADDRESS
    //      KLSRV_AUTOPKG_DEF_ALLOW_NETBIOS_NAME_SERVICE
    //      KLSRV_AUTOPKG_DEF_SERVER_ADDRESS

    //! BOOL_T, Network agent should open ports required for NetBIOS name service (137 UDP) in anti-hacker 6.0
    #define KLSRV_AUTOPKG_DEF_ALLOW_NETBIOS_NAME_SERVICE        L"KLSRV_AUTOPKG_DEF_ALLOW_NETBIOS_NAME_SERVICE"
    #define KLSRV_AUTOPKG_DEF_ALLOW_NETBIOS_NAME_SERVICE_DEF    1

    //! STRING_T, Network agent should open ports required for NetBIOS name service (137 UDP) in anti-hacker 6.0
    #define KLSRV_AUTOPKG_DEF_SERVER_ADDRESS                    L"KLSRV_AUTOPKG_DEF_SERVER_ADDRESS"

#define KLSRV_EVENTS_TO_SERVER_CHECK_TIMEOUT	 (10*1000)

//const wchar_t c_wszFTWorkingFolder[] = L"$FTTmp";
#define KLSRV_FT_WORKING_FOLDER			L"FTServer"
#define KLSRV_FT_WORKING_FOLDER_CLIENT	L"FTClient"

//const wchar_t c_wszFTUpdaterFolder[] = L"$Updater";
#define KLSRV_FT_UPDATER_FOLDER L"$Updater"

#define KLSRV_SPL_WORKING_FOLDER L""

namespace KLSRV
{

    //!\brief 
    const wchar_t c_szwConnStr[]=L"KLSRV_CONNECTION_STRING";

    const wchar_t c_szwSQLServerInstance[]=L"KLSRV_SERVERINSTANCENAME";
    const wchar_t c_szwSQLDatabaseName[]=L"KLSRV_DATABASENAME";
    
    const wchar_t c_szwConnData[]=L"KLSRV_CONNECTION_DATA";//PARAMS_T

    //!Sql server db server type
    #define KLDBCON_DBTYPE_MSSQL            L"MSSQLSRV"

    //!MySql server db server type
    #define KLDBCON_DBTYPE_MYSQL            L"MYSQL"

    //!Database server type (one of KLDBCON_DBTYPE_* defined above)
    #define KLDBCON_DBTYPE      L"KLDBCON_DBTYPE"

    //!Host address
    #define KLDBCON_HOST        L"KLDBCON_HOST"

    //!Port
    #define KLDBCON_PORT        L"KLDBCON_PORT"

    //!Login
    #define KLDBCON_LOGIN       L"KLDBCON_LOGIN"

    //!Password
    #define KLDBCON_PASSWORD    L"KLDBCON_PASSWORD"

    //!Default schema name
    #define KLDBCON_DB          L"KLDBCON_DB"

//Events    

    const wchar_t c_evLicenseCheck90[]=L"KLSRV_EV_LICENSE_CHECK_90";
    
    #define KLSRV_LICENCE_CHECK_90  1024L
    #define KLSRV_LICENCE_CHECK_90_SEVERITY    KLEVP_EVENT_SEVERITY_INFO
    // %1-keyfile; %2-serial, %3-lic.count
    #define KLSRV_LICENCE_CHECK_90_DEF L"License count for key '%1' (serial number '%2', lic. count %3) is more than 90%%"    

    
    const wchar_t c_evLicenseCheck_100_110[]=L"KLSRV_EV_LICENSE_CHECK_100_110";
    
    #define KLSRV_LICENCE_CHECK_100_110  1025L
    #define KLSRV_LICENCE_CHECK_100_110_SEVERITY    KLEVP_EVENT_SEVERITY_WARNING
    // %1-keyfile; %2-serial, %3-lic.count
    #define KLSRV_LICENCE_CHECK_100_110_DEF L"License count for key '%1' (serial number '%2', lic. count %3) is more than 100%%"

    
    const wchar_t c_evLicenseCheck_more_110[]=L"KLSRV_EV_LICENSE_CHECK_MORE_110";
    
    #define KLSRV_LICENCE_CHECK_MORE_110  1026L
    #define KLSRV_LICENCE_CHECK_MORE_110_SEVERITY    KLEVP_EVENT_SEVERITY_CRITICAL
    // %1-keyfile; %2-serial, %3-lic.count
    #define KLSRV_LICENCE_CHECK_MORE_110_DEF L"License count for key '%1' (serial number '%2', lic. count %3) is too much"

    
    const wchar_t c_evHostNewDetected[]=L"KLSRV_EVENT_HOSTS_NEW_DETECTED";
    
    #define KLSRV_NEW_HOST_DETECTED     1027L
    #define KLSRV_NEW_HOST_DETECTED_SEVERITY    KLEVP_EVENT_SEVERITY_INFO
    // %1-hostname; %2-domain
    #define KLSRV_NEW_HOST_DETECTED_DEF L"New host '%1' found (domain name '%2')"

    
    const wchar_t c_evHostNewRedirected[]=L"KLSRV_EVENT_HOSTS_NEW_REDIRECTED";

    #define KLSRV_NEW_HOST_REDIRECTED     1028L
    #define KLSRV_NEW_HOST_REDIRECTED_SEVERITY    KLEVP_EVENT_SEVERITY_INFO
    // %1-hostname; %2-domain; %3-groupid
    #define KLSRV_NEW_HOST_REDIRECTED_DEF L"Host '%1' (domain name '%2') was automaticaly put into group '%3'"

    const wchar_t c_evHostsConflict[]=L"KLSRV_EVENT_HOSTS_CONFLICT";
    
    #define KLSRV_HOSTS_CONFLICT        1029L
    #define KLSRV_HOSTS_CONFLICT_SEVERITY    KLEVP_EVENT_SEVERITY_WARNING
    // %1-hostname; %2-domain
    #define KLSRV_HOSTS_CONFLICT_DEF L"Several hosts '%1' are found in domain '%2'"

    const wchar_t c_evHostsNotVisible[]=L"KLSRV_EVENT_HOSTS_NOT_VISIBLE";    
    #define KLSRV_HOSTS_NOT_VISIBLE        1030L
    #define KLSRV_HOSTS_NOT_VISIBLE_SEVERITY    KLEVP_EVENT_SEVERITY_WARNING
    // %1-hostname; %2-number of days
    #define KLSRV_HOSTS_NOT_VISIBLE_DEF L"Host '%1' wasn't visible for '%2' days"
    
    const wchar_t c_evHostsNotVisibleAndRemoved[]=L"KLSRV_INVISIBLE_HOSTS_REMOVED";
    #define KLSRV_INVISIBLE_HOSTS_REMOVED 1031L
    #define KLSRV_INVISIBLE_HOSTS_REMOVED_SEVERITY  KLEVP_EVENT_SEVERITY_INFO
    // '%1'-hostname; '%2'-number of days; '%3'-group "full name"
    #define KLSRV_INVISIBLE_HOSTS_REMOVED_DEF L"Host '%1' was automatically removed from group '%3' as it hasn't been visible for %2 days"

    const wchar_t c_evNoSpaceOnVolumes[] = L"KLSRV_NO_SPACE_ON_VOLUMES";
    #define KLSRV_NO_SPACE_ON_VOLUMES 1032L
    #define KLSRV_NO_SPACE_ON_VOLUMES_SEVERITY  KLEVP_EVENT_SEVERITY_WARNING
    // '%1'-volumes list 
    #define KLSRV_NO_SPACE_ON_VOLUMES_DEF L"Following volumes are filled near capacity: %1."

    const wchar_t c_evNoSpaceInDB[] = L"KLSRV_NO_SPACE_IN_DATABASE";
    #define KLSRV_NO_SPACE_IN_DATABASE 1033L
    #define KLSRV_NO_SPACE_IN_DATABASE_SEVERITY  KLEVP_EVENT_SEVERITY_WARNING
    // '%1'- database name
    #define KLSRV_NO_SPACE_IN_DATABASE_DEF L"Database '%1' is filled near maximum capacity."

    const wchar_t c_evDiskIsFull[] = L"KLSRV_DISK_FULL";
    #define KLSRV_DISK_FULL 1034L
    #define KLSRV_DISK_FULL_SEVERITY  KLEVP_EVENT_SEVERITY_ERROR
    // '%1'- volume name
    #define KLSRV_DISK_FULL_DEF L"Disk '%1' is full."

    const wchar_t c_evSharedFolderUnavailable[] = L"KLSRV_SHARED_FOLDER_UNAVAILABLE";
    #define KLSRV_SHARED_FOLDER_UNAVAILABLE 1035L
    #define KLSRV_SHARED_FOLDER_UNAVAILABLE_SEVERITY  KLEVP_EVENT_SEVERITY_ERROR
    // '%1'- share name
    #define KLSRV_SHARED_FOLDER_UNAVAILABLE_DEF L"Share '%1' is unavailable."

    const wchar_t c_evDatabaseUnavailable[] = L"KLSRV_DATABASE_UNAVAILABLE";
    #define KLSRV_DATABASE_UNAVAILABLE 1036L
    #define KLSRV_DATABASE_UNAVAILABLE_SEVERITY  KLEVP_EVENT_SEVERITY_ERROR
    // '%1'- instance name
    // '%2'- db name
    // '%3'- connection string
    #define KLSRV_DATABASE_UNAVAILABLE_DEF L"Database server '%1' is unavailable. DB name is '%2'. Connection string is '%3'."

    const wchar_t c_evDatabaseIsFull[] = L"KLSRV_DATABASE_FULL";
    #define KLSRV_DATABASE_FULL 1037L
    #define KLSRV_DATABASE_FULL_SEVERITY  KLEVP_EVENT_SEVERITY_ERROR
    // '%1'- instance name
    // '%2'- database name
    #define KLSRV_DATABASE_FULL_DEF L"Database is full. Instance name is'%1'. Database name is '%2'."

	#define c_evEvent_VirusOutbreak         KLEVP::c_szwGnrlEvent_VirusOutbreak
	#define KLSRV_EVENT_VIRUS_OUTBREAK      1038L
	#define KLSRV_EVENT_VIRUS_OUTBREAK_FW   1090L
	#define KLSRV_EVENT_VIRUS_OUTBREAK_MAIL 1091L
	#define KLSRV_EVENT_VIRUS_OUTBREAK2     1074L
	#define KLSRV_EVENT_VIRUS_OUTBREAK_SEVERITY    KLEVP_EVENT_SEVERITY_CRITICAL

    const wchar_t c_evHostOutControl[] = L"KLSRV_HOST_OUT_CONTROL";
    #define KLSRV_HOST_OUT_CONTROL 1039L
    #define KLSRV_HOST_OUT_CONTROL_SEVERITY  KLEVP_EVENT_SEVERITY_CRITICAL
    // '%1'- display name
    #define KLSRV_HOST_OUT_CONTROL_DEF  L"Host '%1' is out of control."

    const wchar_t c_evHostMayBeOutControl[] = L"KLSRV_HOST_MAY_BE_OUT_CONTROL";
    #define KLSRV_HOST_MAY_BE_OUT_CONTROL 1040L
    #define KLSRV_HOST_MAY_BE_OUT_CONTROL_SEVERITY  KLEVP_EVENT_SEVERITY_WARNING
    // '%1'- display name
    #define KLSRV_HOST_MAY_BE_OUT_CONTROL_DEF   L"Host '%1' may be out of control."

    const wchar_t c_evHostStatusCritical[] = L"KLSRV_HOST_STATUS_CRITICAL";
    #define KLSRV_HOST_STATUS_CRITICAL_SEVERITY  KLEVP_EVENT_SEVERITY_CRITICAL
    // '%1'- display name    
    #define KLSRV_HOST_STATUS_CRITICAL_0 1041L
    #define KLSRV_HOST_STATUS_CRITICAL_0_DEF   L"Host '%1' changed its status to \"Critical\": host is out of control."
    #define KLSRV_HOST_STATUS_CRITICAL_1 1042L
    #define KLSRV_HOST_STATUS_CRITICAL_1_DEF   L"Host '%1' changed its status to \"Critical\": runtime protection is not running."
    #define KLSRV_HOST_STATUS_CRITICAL_2 1043L
    #define KLSRV_HOST_STATUS_CRITICAL_2_DEF   L"Host '%1' changed its status to \"Critical\": anti-virus is not running."
    #define KLSRV_HOST_STATUS_CRITICAL_3 1044L
    #define KLSRV_HOST_STATUS_CRITICAL_3_DEF   L"Host '%1' changed its status to \"Critical\": number of viruses found is too much."
    #define KLSRV_HOST_STATUS_CRITICAL_4 1045L
    #define KLSRV_HOST_STATUS_CRITICAL_4_DEF   L"Host '%1' changed its status to \"Critical\": runtime protection level differs from one specified by administrator."
    #define KLSRV_HOST_STATUS_CRITICAL_5 1046L
    #define KLSRV_HOST_STATUS_CRITICAL_5_DEF   L"Host '%1' changed its status to \"Critical\": anti-virus protection is not installed."
    #define KLSRV_HOST_STATUS_CRITICAL_6 1047L
    #define KLSRV_HOST_STATUS_CRITICAL_6_DEF   L"Host '%1' changed its status to \"Critical\": full scan hasn't been performed for a long time."
    #define KLSRV_HOST_STATUS_CRITICAL_7 1048L
    #define KLSRV_HOST_STATUS_CRITICAL_7_DEF   L"Host '%1' changed its status to \"Critical\": anti-virus bases are too old."
    #define KLSRV_HOST_STATUS_CRITICAL_8 1049L
    #define KLSRV_HOST_STATUS_CRITICAL_8_DEF   L"Host '%1' changed its status to \"Critical\": host hasn't connected to administration server for a long time."
    #define KLSRV_HOST_STATUS_CRITICAL_9 1075L
    #define KLSRV_HOST_STATUS_CRITICAL_9_DEF   L"Host '%1' changed its status to \"Critical\": license expired."
    #define KLSRV_HOST_STATUS_CRITICAL_10 1077L
    #define KLSRV_HOST_STATUS_CRITICAL_10_DEF   L"Host '%1' changed its status to \"Critical\": number of uncured objects found is too much."

    

    const wchar_t c_evHostStatusWarning[] = L"KLSRV_HOST_STATUS_WARNING";
    #define KLSRV_HOST_STATUS_WARNING_SEVERITY  KLEVP_EVENT_SEVERITY_WARNING
    // '%1'- display name
    #define KLSRV_HOST_STATUS_WARNING_0 1050L
    #define KLSRV_HOST_STATUS_WARNING_0_DEF   L"Host '%1' changed its status to \"Warning\": host is out of control."
    #define KLSRV_HOST_STATUS_WARNING_1 1051L
    #define KLSRV_HOST_STATUS_WARNING_1_DEF   L"Host '%1' changed its status to \"Warning\": runtime protection is not running."
    #define KLSRV_HOST_STATUS_WARNING_2 1052L
    #define KLSRV_HOST_STATUS_WARNING_2_DEF   L"Host '%1' changed its status to \"Warning\": anti-virus is not running."
    #define KLSRV_HOST_STATUS_WARNING_3 1053L
    #define KLSRV_HOST_STATUS_WARNING_3_DEF   L"Host '%1' changed its status to \"Warning\": number of viruses found is too much."
    #define KLSRV_HOST_STATUS_WARNING_4 1054L
    #define KLSRV_HOST_STATUS_WARNING_4_DEF   L"Host '%1' changed its status to \"Warning\": runtime protection level differs from one specified by administrator."
    #define KLSRV_HOST_STATUS_WARNING_5 1055L
    #define KLSRV_HOST_STATUS_WARNING_5_DEF   L"Host '%1' changed its status to \"Warning\": anti-virus protection is not installed."
    #define KLSRV_HOST_STATUS_WARNING_6 1056L
    #define KLSRV_HOST_STATUS_WARNING_6_DEF   L"Host '%1' changed its status to \"Warning\": full scan hasn't been performed for a long time."
    #define KLSRV_HOST_STATUS_WARNING_7 1057L
    #define KLSRV_HOST_STATUS_WARNING_7_DEF   L"Host '%1' changed its status to \"Warning\": anti-virus bases are too old."
    #define KLSRV_HOST_STATUS_WARNING_8 1058L
    #define KLSRV_HOST_STATUS_WARNING_8_DEF   L"Host '%1' changed its status to \"Warning\": host hasn't connected to administration server for a long time."
    #define KLSRV_HOST_STATUS_WARNING_9 1076L
    #define KLSRV_HOST_STATUS_WARNING_9_DEF   L"Host '%1' changed its status to \"Warning\": license is expiring."
    #define KLSRV_HOST_STATUS_WARNING_10 1078L
    #define KLSRV_HOST_STATUS_WARNING_10_DEF   L"Host '%1' changed its status to \"Warning\": number of uncured objects found is too much."


    const wchar_t c_evSlaveServerConnected[]=L"KLSRV_EV_SLAVE_SRV_CONNECTED";
    #define KLSRV_EV_SLAVE_SRV_CONNECTED  1059L
    #define KLSRV_EV_SLAVE_SRV_CONNECTED_SEVERITY    KLEVP_EVENT_SEVERITY_INFO
    // %1-display name;
    #define KLSRV_EV_SLAVE_SRV_CONNECTED_DEF L"Connection with slave server '%1' established."

    const wchar_t c_evSlaveServerDisconnected[]=L"KLSRV_EV_SLAVE_SRV_DISCONNECTED";
    #define KLSRV_EV_SLAVE_SRV_DISCONNECTED  1060L
    #define KLSRV_EV_SLAVE_SRV_DISCONNECTED_SEVERITY    KLEVP_EVENT_SEVERITY_WARNING
    // %1-display name;
    #define KLSRV_EV_SLAVE_SRV_DISCONNECTED_DEF L"Connection with slave server '%1' broken."

    const wchar_t c_evMasterServerConnected[]=L"KLSRV_EV_MASTER_SRV_CONNECTED";
    #define KLSRV_EV_MASTER_SRV_CONNECTED  1061L
    #define KLSRV_EV_MASTER_SRV_CONNECTED_SEVERITY    KLEVP_EVENT_SEVERITY_INFO
    // %1-address;
    #define KLSRV_EV_MASTER_SRV_CONNECTED_DEF L"Connection with master server established (address: %1)."

    const wchar_t c_evMasterServerDisconnected[]=L"KLSRV_EV_MASTER_SRV_DISCONNECTED";
    #define KLSRV_EV_MASTER_SRV_DISCONNECTED  1062L
    #define KLSRV_EV_MASTER_SRV_DISCONNECTED_SEVERITY    KLEVP_EVENT_SEVERITY_WARNING
    // %1-display name; %2-group;
    #define KLSRV_EV_MASTER_SRV_DISCONNECTED_DEF L"Connection with master server broken."


    #define KLSRV_TSK_BACKUP_INVALID_CMDLINE  1063L
    #define KLSRV_TSK_BACKUP_INVALID_CMDLINE_SEVERITY    KLEVP_EVENT_SEVERITY_ERROR

    #define KLSRV_TSK_BACKUP_INVALID_PASSWORD  1064L
    #define KLSRV_TSK_BACKUP_INVALID_PASSWORD_SEVERITY    KLEVP_EVENT_SEVERITY_ERROR

    #define KLSRV_TSK_BACKUP_FAILED_OPEN_SCM  1065L
    #define KLSRV_TSK_BACKUP_FAILED_OPEN_SCM_SEVERITY    KLEVP_EVENT_SEVERITY_ERROR

    #define KLSRV_TSK_BACKUP_FAILED_STOP_SERVICE  1066L
    #define KLSRV_TSK_BACKUP_FAILED_STOP_SERVICE_SEVERITY    KLEVP_EVENT_SEVERITY_ERROR

    #define KLSRV_TSK_BACKUP_FAILED_START_SERVICE  1067L
    #define KLSRV_TSK_BACKUP_FAILED_START_SERVICE_SEVERITY    KLEVP_EVENT_SEVERITY_ERROR

    #define KLSRV_TSK_BACKUP_INVALID_DST_PATH  1068L
    #define KLSRV_TSK_BACKUP_INVALID_DST_PATH_SEVERITY    KLEVP_EVENT_SEVERITY_ERROR

    #define KLSRV_TSK_BACKUP_DST_NOT_EMPTY  1069L
    #define KLSRV_TSK_BACKUP_DST_NOT_EMPTY_SEVERITY    KLEVP_EVENT_SEVERITY_ERROR

    #define KLSRV_TSK_BACKUP_DB_BACKUP_FAILED  1070L
    #define KLSRV_TSK_BACKUP_DB_BACKUP_FAILED_SEVERITY    KLEVP_EVENT_SEVERITY_ERROR

    #define KLSRV_TSK_BACKUP_UNEXPECTED  1071L
    #define KLSRV_TSK_BACKUP_UNEXPECTED_SEVERITY    KLEVP_EVENT_SEVERITY_ERROR

    #define KLSRV_TSK_BACKUP_UNKNOWN  1072L
    #define KLSRV_TSK_BACKUP_UNKNOWN_SEVERITY    KLEVP_EVENT_SEVERITY_ERROR

    #define KLSRV_TSK_BACKUP_ALREADY_RUNNING  1073L
    #define KLSRV_TSK_BACKUP_ALREADY_RUNNING_SEVERITY    KLEVP_EVENT_SEVERITY_ERROR

    const wchar_t c_evHostsMovedWithRuleEx[] = L"KLSRV_HOST_MOVED_WITH_RULE_EX";
    #define KLSRV_HOST_MOVED_WITH_RULE_EX 1074L
    #define KLSRV_HOST_MOVED_WITH_RULE_EX_SEVERITY  KLEVP_EVENT_SEVERITY_INFO
    // '%1'-hostname; '%2'- name of rule; %3 - src group; %4 - dst group
    #define KLSRV_HOST_MOVED_WITH_RULE_EX_DEF L"Host '%1' was automatically moved by the rule '%2' from group '%3' into the group '%4'"

    // %1-error code;

    #define KLSRV_TSK_BACKUP_  106L
    #define KLSRV_TSK_BACKUP__SEVERITY    KLEVP_EVENT_SEVERITY_ERROR

//Tasks
    #define KLSRV_TSK_UPDATE_NET        L"UpdateNet-Task-KLServer"
    #define KLSRV_TSK_FASTUPDATE_NET    L"FastUpdateNet-Task-KLServer"
    #define KLSRV_TSK_DPNS              L"KLSRV_TSK_DPNS"

    #define KLSRV_TSK_FORCE_SYNC        L"KLSRV_TSK_FORCE_SYNC"
    //IN c_tpForceSync_Type, and one of (c_tpForceSync_Host, c_tpForceSync_Hosts, c_tpForceSync_Group)
        const wchar_t c_tpForceSync_Type[]  =L"KLSRV_TP_FORCE_SYNC_TYPE";   //STRING_T
        const wchar_t c_tpForceSync_Host[]  =L"KLSRV_TP_FORCE_SYNC_HOST";   //STRING_T
        const wchar_t c_tpForceSync_Hosts[]  =L"KLSRV_TP_FORCE_SYNC_HOSTS";   //ARRAY_T|STRING_T
        const wchar_t c_tpForceSync_Group[]  =L"KLSRV_TP_FORCE_SYNC_GROUP";   //INT_T        

    #define KLSRV_TSK_MOVE_HOSTS        L"KLSRV_TSK_MOVE_HOSTS"
    //IN
        const wchar_t c_tpMoveHosts_Src[]  =L"KLSRV_TP_MOVE_HOSTS_SRC";   //INT_T
        const wchar_t c_tpMoveHosts_Dst[]  =L"KLSRV_TP_MOVE_HOSTS_DST";   //INT_T
        
    #define KLSRV_TSK_EXEC_RULES       L"KLSRV_TSK_EXEC_RULES"
    //IN
        /*
            par1 - lGroupId
            par2 - rules array (opt)
            par3 - options
        */

        
    #define KLSRV_TSK_ZERO_VCOUNT        L"KLSRV_TSK_ZERO_VCOUNT"
    //IN
        const wchar_t c_tpZeroVCount_Hosts[]  =L"KLSRV_TP_ZERO_VCOUNT_HOSTS";   //INT_T if group id OR ARRAY_T|STRING_T if hosts

    #define KLSRV_TSK_CHK_UNAVAIL       L"KLSRV_TSK_CHK_UNAVAIL"
    
    #define KLSRV_TSK_SCAN_AD           L"KLSRV_TSK_SCAN_AD"

    #define KLSRV_TSK_EMAIL_REPORT		L"KLSRV_TSK_EMAIL_REPORT"
    //IN
		const wchar_t c_tpSendReportTask_EMail[] = L"KLSRV_TP_EMAIL_REPORT_EMAIL";		// STRING_T

// for compatibility with old servers only !!
		const wchar_t c_tpSendReportTask_ReportID[] = L"KLSRV_TP_EMAIL_REPORT_ID";		// INT_T
// for compatibility with old servers only !!

		const wchar_t c_tpSendReportTask_SendMail[] = L"KLSRV_TP_EMAIL_REPORT_SEND_MAIL";	// BOOL_T
		const wchar_t c_tpSendReportTask_Save2Folder[] = L"KLSRV_TP_EMAIL_REPORT_SAVE";	// BOOL_T
			const wchar_t c_tpSendReportTask_Folder[] = L"KLSRV_TP_EMAIL_REPORT_FOLDER";	// STRING_T

		const wchar_t c_tpSendReportTask_ReportIDs[] = L"KLSRV_TP_EMAIL_REPORT_IDS";	// ARRAY_T of INT_T
		const wchar_t c_tpSendReportTask_Subject[] = L"KLSRV_TP_EMAIL_REPORT_SUBJECT";	// STRING_T
		const wchar_t c_tpSendReportTask_AttachmentType[] = L"KLSRV_TP_EMAIL_REPORT_ATTACHMENT_TYPE";	// INT_T
        typedef enum {
			TSK_EMAIL_REPORT_ATYPE_HTML,		// Html only
			TSK_EMAIL_REPORT_ATYPE_CAB,			// as attached cab-file
			TSK_EMAIL_REPORT_ATYPE_HTML_BODY	// Html with pictures
        } TSK_EMAIL_REPORT_ATTACHMENT_TYPE;
		const wchar_t c_tpSendReportTask_UseServerSettings[] = L"KLSRV_TP_EMAIL_REPORT_USE_SERVER_SETTINGS";	// BOOL_T
		const wchar_t c_tpSendReportTask_SMTPServer[] = L"KLSRV_TP_EMAIL_REPORT_SMTPSERVER";					// STRING_T
		const wchar_t c_tpSendReportTask_SMTPPort[] = L"KLSRV_TP_EMAIL_REPORT_SMTPPORT";						// INT_T
		const wchar_t c_tpSendReportTask_ESMTPUser[] = L"KLSRV_TP_EMAIL_REPORT_ESMTPUSER";						// STRING_T
		const wchar_t c_tpSendReportTask_ESMTPPassword[] = L"KLSRV_TP_EMAIL_REPORT_ESMTPPASSWORD";				// STRING_T
		
		const bool c_tpSendReportTask_UseServerSettings_Default = true;

		// Errors
		#define KLSRV_TSK_SEND_REPORT_ERROR_QUERYING_DATABASE			3001L
		#define KLSRV_TSK_SEND_REPORT_ERROR_QUERYING_DATABASE_SEVERITY	KLEVP_EVENT_SEVERITY_ERROR

		#define KLSRV_TSK_SEND_REPORT_ERROR_SAVE_REPORT					3002L
		#define KLSRV_TSK_SEND_REPORT_ERROR_SAVE_REPORT_SEVERITY		KLEVP_EVENT_SEVERITY_ERROR
		
		#define KLSRV_TSK_SEND_REPORT_ERROR_CREATING_AUX_FILES			3003L
		#define KLSRV_TSK_SEND_REPORT_ERROR_CREATING_AUX_FILES_SEVERITY		KLEVP_EVENT_SEVERITY_ERROR

		#define KLSRV_TSK_SEND_REPORT_ERROR_CREATING_REPORT				3004L
		#define KLSRV_TSK_SEND_REPORT_ERROR_CREATING_REPORT_SEVERITY	KLEVP_EVENT_SEVERITY_ERROR

		#define KLSRV_TSK_SEND_REPORT_ERROR_EMAILING_REPORT				3005L
		#define KLSRV_TSK_SEND_REPORT_ERROR_EMAILING_REPORT_SEVERITY	KLEVP_EVENT_SEVERITY_ERROR

		#define KLSRV_TSK_SEND_REPORT_ERROR_SOCKET_ERROR				3006L
		#define KLSRV_TSK_SEND_REPORT_ERROR_SOCKET_ERROR_SEVERITY	KLEVP_EVENT_SEVERITY_ERROR

		#define KLSRV_TSK_SEND_REPORT_ERROR_CONNECT_TO_SMTP_SERVER				3007L
		#define KLSRV_TSK_SEND_REPORT_ERROR_CONNECT_TO_SMTP_SERVER_SEVERITY	KLEVP_EVENT_SEVERITY_ERROR

		#define KLSRV_TSK_SEND_REPORT_ERROR_SERVER_WELCOME_MESSAGE				3008L
		#define KLSRV_TSK_SEND_REPORT_ERROR_SERVER_WELCOME_MESSAGE_SEVERITY	KLEVP_EVENT_SEVERITY_ERROR

		#define KLSRV_TSK_SEND_REPORT_ERROR_INVALID_ARGUMENTS				3009L
		#define KLSRV_TSK_SEND_REPORT_ERROR_INVALID_ARGUMENTS_SEVERITY	KLEVP_EVENT_SEVERITY_ERROR

		#define KLSRV_TSK_SEND_REPORT_ERROR_AUTHENTICATION_FAILED				3010L
		#define KLSRV_TSK_SEND_REPORT_ERROR_AUTHENTICATION_FAILED_SEVERITY	KLEVP_EVENT_SEVERITY_ERROR

		#define KLSRV_TSK_SEND_REPORT_ERROR_SENDING_SMTP_COMMAND				3011L
		#define KLSRV_TSK_SEND_REPORT_ERROR_SENDING_SMTP_COMMAND_SEVERITY	KLEVP_EVENT_SEVERITY_ERROR
        
		#define KLSRV_TSK_SEND_REPORT_ERROR_SMTP_SERVER_ADDRESS_EMPTY				3012L
		#define KLSRV_TSK_SEND_REPORT_ERROR_SMTP_SERVER_ADDRESS_EMPTY_SEVERITY	KLEVP_EVENT_SEVERITY_ERROR

    #define KLSRV_TSK_WAKE_ON_LAN		L"KLSRV_TSK_WAKE_ON_LAN"
    //IN
		const wchar_t c_tpWakeOnLanTask_OriginalTaskId[] = L"KLSRV_TP_WAKE_ON_LAN_TSK_ID";		// INT_T

    #define KLSRV_TSK_BACKUP			L"KLSRV_TSK_BACKUP"
    //IN
		const wchar_t c_tpBackupTask_BasePath[] = L"KLSRV_TP_BACKUP_BASE_PATH";		// STRING_T
		const wchar_t c_tpBackupTask_Password[] = L"KLSRV_TP_BACKUP_PASSWORD";		// BINARY_T
		const wchar_t c_tpBackupTask_ReserveCopiesCount[] = L"KLSRV_TP_BACKUP_COPIES_COUNT";	// INT_T
        
    #define KLSRV_TSK_DEL_GROUPS_SUBTREE   L"KLSRV_TSK_DEL_GRP_SUBTREE"
    //IN 
        const wchar_t c_tpDelGrpSubtree_GroupId[]  =L"KLSRV_TP_DELGRPTR_ID";   //INT_T
		
//Statistics
    //!Number of all connections
    #define KLSRV_ST_ALL_CONS_CNT       L"KLSRV_ST_ALL_CONS_CNT"

    //!Number of all nagent connections
    #define KLSRV_ST_NAG_CONS_CNT       L"KLSRV_ST_NAG_CONS_CNT"

    //!Number of controlled nagent connections
    #define KLSRV_ST_CTLNGT_CONS_CNT    L"KLSRV_ST_CTLNGT_CONS_CNT"

    //!Number of pings processed
    #define KLSRV_ST_PING_CNT           L"KLSRV_ST_PING_CNT"

    //!Number of pings processing just now
    #define KLSRV_ST_PING_JN_CNT        L"KLSRV_ST_PING_JN_CNT"

    //!Number of syncs attempts processed
    #define KLSRV_ST_SYNC_CNT           L"KLSRV_ST_SYNC_CNT"

    //!Number of real syncs processed
    #define KLSRV_ST_SYNC_REAL_CNT      L"KLSRV_ST_SYNC_REAL_CNT"

    //!Number of successfull syncs attempts
    #define KLSRV_ST_SYNC_SUC_CNT           L"KLSRV_ST_SYNC_SUC_CNT"

    //!Number of syncs processing just now
    #define KLSRV_ST_SYNC_JN_CNT            L"KLSRV_ST_SYNC_JN_CNT"

    //!Date/time of last full network scan
    #define KLSRV_ST_LAST_FULLNETSCAN_TIME  L"KLSRV_ST_LAST_FULLNETSCAN_TIME"

    //!Date/time of last fast network scan
    #define KLSRV_ST_LAST_FASTNETSCAN_TIME  L"KLSRV_ST_LAST_FASTNETSCAN_TIME"

    //!Date/time of last AD network scan
    #define KLSRV_ST_LAST_ADSCAN_TIME  L"KLSRV_ST_LAST_ADSCAN_TIME"

    //!Date/time of last DPNS network scan
    #define KLSRV_ST_LAST_DPNSSCAN_TIME  L"KLSRV_ST_LAST_DPNSSCAN_TIME"

    //!Number of host wth 'keep connection' attribute set
    #define KLSRV_ST_KEEPCONN_HOSTS_CNT     L"KLSRV_ST_KEEPCONN_HOSTS_CNT"
        
    //!Number of in sync-tion queue
    #define KLSRV_ST_SYNC_QUEUE_SIZE        L"KLSRV_ST_SYNC_QUEUE_SIZE"
        
    //! True if network was successfully scanned at least once, BOOL_T
    #define KLSRV_ST_NETWORK_SCANNED        L"KLSRV_ST_NETWORK_SCANNED"
        
    //! Network fast scan task completion percent, INT_T
    #define KLSRV_ST_NETWORK_SCAN_PERCENT L"KLSRV_ST_NETWORK_SCAN_PERCENT"

    //! Network full scan task completion percent, INT_T
    #define KLSRV_ST_FULL_SCAN_PERCENT L"KLSRV_ST_FULL_SCAN_PERCENT"

    //! Network dpns scan task completion percent, INT_T
    #define KLSRV_ST_DPNS_SCAN_PERCENT L"KLSRV_ST_DPNS_SCAN_PERCENT"

    //! Network ad scan task completion percent, INT_T
    #define KLSRV_ST_AD_SCAN_PERCENT L"KLSRV_ST_AD_SCAN_PERCENT"
       
    //! Currently scanned domain name, STRING_T //'*' means  list of domains is being retrieved
    #define KLSRV_ST_NETWORK_DOMAIN_SCANNED L"KLSRV_ST_NETWORK_DOMAIN_SCANNED"

    //! Admin basic authentication
    #define KLSRV_BA_USER       L"KLSRV_BA_1"
    #define KLSRV_BA_DOMAIN     L"KLSRV_BA_2"
    #define KLSRV_BA_PASSWORD   L"KLSRV_BA_3"

    #define IDS_KLSRV_CANNOT_CREATE_LISTENER_DEF  L"Cannot open port %1"

    #define KLSRV_SSTYPE_POLICY_DN  L"KLSRV_SSTYPE_POLICY_DN"
    
    #define KLSRV_SSTYPE_POLICY_FLAGS  L"KLSRV_SSTYPE_POLICY_FLAGS"

    //!andkaz: bugfix №15806
    #define KLSRV_SRVHRCH_INTEGRATED    L"SRVHRCH_INTEGRATED" //BOOL_T
        
    #define KLSRV_SRVHRCH_  L"SRVHRCH_INTEGRATED"

	#define KLSRV_FORCE_UPD_SLAVES		L"KLSRV_FORCE_UPD_SLAVES"
		
    #define KLSRV_SRVHRCH_SKIP_COLLABORATION    L"SRVHRCH_SKIP_COLLABORATION"

    //! version of slave server
    #define KLSRV_SRVHRCH_NUM_VERSION    L"SRVHRCH_NUM_VERSION" //INT_T

	//! automatic updates
	#define KLSRV_SERVER_AUTOUPDATES		L"KLSRV_SERVER_AUTOUPDATES" //BOOL_T
	#define KLSRV_NAGENT_AUTOUPDATES		L"KLSRV_NAGENT_AUTOUPDATES"	//BOOL_T
};

//!obsolete
#define KLSRV_SP_ENABLE_IP_DIAPASON_SCANNING            KLSRV_DPNS_SCAN_ENABLED
#define KLSRV_SP_ENABLE_IP_DIAPASON_SCANNING_DEFAULT    KLSRV_DPNS_SCAN_ENABLED_DEFAULT

#endif //__KLSERVER_CONSTANTS_H__

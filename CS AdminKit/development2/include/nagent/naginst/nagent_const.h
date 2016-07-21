/*!
 * (C) 2003 Kaspersky Lab
 *
 * \file	nagent_const.h
 * \author	Andrew Kazachkov
 * \date	12.06.2003 14:16:28
 * \brief
 *
 */

#ifndef __KL_NAGENT_CONST_H__
#define __KL_NAGENT_CONST_H__

#include <avp/klids.h>
#include <transport/ev/ev_general.h>
#include <server/srvinst/server_ports.h>
#include <std/stress/st_prefix.h>
#include <common/klak_versions.h>
#include <common/kltchar.h>



#define KLNAG_NEWAPP_INSTALLED  _T("NewAppInstalled") //REG_DWORD
#define KLNAG_INST_NTFSRV       _T("InstNtfSrv")      //REG_SZ
#define KLNAG_INST_NTFSRV_RES   _T("InstNtfSrvRes")   //REG_SZ
#define KLNAG_CMDLN_OPEN_PORTS_WFW      _T("ports-wfw")


#define KLNAG_TIMEOUT_WFW   180000

//Installer notifications
#define KLNAG_NTFINST_NAME  L"KLNAG_NTFINST_NAME"       //STRING_T
#define KLNAG_NTFINST_HOST  L"KLNAG_NTFINST_HOST"       //STRING_T
#define KLNAG_NTFINST_TYPE  L"KLNAG_NTFINST_TYPE"       //STRING_T
#define KLNAG_NTFINST_RES   L"KLNAG_NTFINST_RES"        //STRING_T

#define KLNAG_NTFINST_TYPE_UNINSTALL    L"uninstall"
#define KLNAG_NTFINST_TYPE_RECONNECT    L"reconnect"

#define KLNAG_PRODUCT_NAME		KLCS_PRODUCT_NAGENT
#define KLNAG_PRODUCT_VERSION	KLCS_VERSION_NAGENT
#define KLNAG_COMPONENT_NAME	KLCS_COMPONENT_NAGENT
#define KLNAG_COMPONENT_VERSION	L"1.0.0.0"

//!Nagent settings storage server name
#define KLNAG_DEF_SS_SERVER     L"KLNAG_DEF_SS_SERVER"

//!Nagent flags subkey
#define KLNAG_NAGENT_FLAGS_SUBKEY L"NagentFlags"

#define KLNAG_FLAG_CHKCONNECTPRS    L"CheckConnectors"

#define KLNAG_PRODUCT_DYNAMICDATA   KLNAG_PRODUCT_NAME
#define KLNAG_VERSION_DYNAMICDATA   KLNAG_PRODUCT_VERSION
#define KLNAG_SECTION_DYNAMICDATA   KLNAG_COMPONENT_NAME


//! Cleaner info section; filled on the host, read on server
#define KLNAG_SECTION_CLEANER_DATA	L"KLNAG_CLNR_DATA"

//---------------------------------------------------------
//   BEGIN of section KLNAG_SECTION_CLEANER_DATA
//---------------------------------------------------------

#define KLNAG_CLNR_SOFTWARE_NAME            L"SoftwareName"

//---------------------------------------------------------
//   END of section KLNAG_SECTION_CLEANER_DATA
//---------------------------------------------------------


//---------------------------------------------------------
//   BEGIN of nagent version information. SS_PRODINFO: (KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT, KLNAG_VERSIONINFO)
//---------------------------------------------------------

//! Section name
#define KLNAG_VERSIONINFO   L"KLNAG_VERSIONINFO"

//! Nagent version KLAK_VER_*, INT_T
#define KLNAG_VERSION       L"KLNAG_VERSION"


//---------------------------------------------------------
//   END of nagent version information
//---------------------------------------------------------


//---------------------------------------------------------
//   BEGIN of main settins section KLNAG_COMPONENT_NAME
//---------------------------------------------------------

#define KLNAG_COMPRESS_TRAFFIC          L"KLNAG_COMPRESS_TRAFFIC"
#define KLNAG_COMPRESS_TRAFFIC_DEFAULT  true

#define KLNAG_TRIM_WS_PERIOD            L"KLNAG_TRIM_WS_PERIOD"
#define KLNAG_TRIM_WS_PERIOD_DEFAULT    (1*60000)

#define KLNAG_TRIM_WS_PERIOD            L"KLNAG_TRIM_WS_PERIOD"
#define KLNAG_TRIM_WS_PERIOD_DEFAULT    (1*60000)

#define KLNAG_SSL_CUSTOM_CERT           L"KLNAG_SSL_CUSTOM_CERT"
#define KLNAG_SSL_CUSTOM_CERT_DEFAULT   ((KLSTD_StGetDefDirW() + L"klserver.cer").c_str())

#define KLNAG_CONN_ROOTDIR              L"KLNAG_CONN_ROOTDIR"
#define KLNAG_CONN_ROOTDIR_DEFAULT      L"products"

#define KLNAG_SERVER_INSTALLED                  L"KLNAG_SERVER_INSTALLED"
#define KLNAG_SERVER_INSTALLED_DEFAULT          false

#define KLNAG_PRESUMED_GROUP_NAME               L"KLNAG_PRESUMED_GROUP_NAME"
#define KLNAG_PRESUMED_DISPLAY_NAME               L"KLNAG_PRESUMED_DISPLAY_NAME"

#define KLNAG_NEWINST_CHECK_TIMEOUT             L"KLNAG_NEWINST_CHECK_TIMEOUT"
#define KLNAG_NEWINST_CHECK_TIMEOUT_DEFAULT     (10*1000)

#define KLNAG_UNINST_CHECK_TIMEOUT             L"KLNAG_UNINST_CHECK_TIMEOUT"
#define KLNAG_UNINST_CHECK_TIMEOUT_DEFAULT     (10*1000)

#define KLNAG_SERVER_WATCHDOG_PERIOD           L"KLNAG_SERVER_WATCHDOG_PERIOD"
#define KLNAG_SERVER_WATCHDOG_PERIOD_DEFAULT   (60*1000)

#define KLNAG_SERVER_WATCHDOG_MAXCHECK_TIMEOUT           L"KLNAG_SERVER_WATCHDOG_MAXCHECK_TIMEOUT"
#define KLNAG_SERVER_WATCHDOG_MAXCHECK_TIMEOUT_DEFAULT   (15*60*1000)

#define KLNAG_SERVER_WATCHDOG_TIMEOUT           L"KLNAG_SERVER_WATCHDOG_TIMEOUT"
#define KLNAG_SERVER_WATCHDOG_TIMEOUT_DEFAULT   (60*1000)


#define KLNAG_EVENTS_TO_SERVER_CHECK_TIMEOUT	 ((1000 * 2) / 3)
#define KLNAG_GROUP_TASK_HANDLER_CHECK_TIMEOUT	 (30*1000)

//! Таймаут пинга нагента к серверу
#define KLNAG_SP_SERVER_PING_TIMEOUT            L"ServerPingTimeout"
#define KLNAG_SP_SERVER_PING_TIMEOUT_DEFAULT (15*60*1000)

//! Таймаут проверки необходимости пинга
#define KLNAG_SP_SERVER_PING_CHECK_TIMEOUT_DEFAULT (10000)

//#define KLNAG_SP_SERVER_LOCALSERVER_CHECK_TIMEOUT         L"LocalServerCheckTimeout"
#define KLNAG_SP_SERVER_LOCALSERVER_CHECK_TIMEOUT_DEFAULT (30*1000)


//! Удалённый транспортный таймаут коннекта
#define KLNAG_SP_REMOTE_CONNECT_TIMEOUT         L"KLNAG_SP_REMOTE_CONNECT_TIMEOUT"
#define KLNAG_SP_REMOTE_CONNECT_TIMEOUT_DEFAULT 30000

//! Удалённый транспортный таймаут send/receive
#define KLNAG_SP_REMOTE_SNDRCV_TIMEOUT          L"KLNAG_SP_REMOTE_SNDRCV_TIMEOUT"
#define KLNAG_SP_REMOTE_SNDRCV_TIMEOUT_DEFAULT  180000

//! Удалённый транспортный таймаут коннекта
#define KLNAG_SP_LOCAL_CONNECT_TIMEOUT          L"KLNAG_SP_LOCAL_CONNECT_TIMEOUT"
#define KLNAG_SP_LOCAL_CONNECT_TIMEOUT_DEFAULT 10000

//! Удалённый транспортный таймаут send/receive
#define KLNAG_SP_LOCAL_SNDRCV_TIMEOUT           L"KLNAG_SP_LOCAL_SNDRCV_TIMEOUT"
#define KLNAG_SP_LOCAL_SNDRCV_TIMEOUT_DEFAULT   INT_MAX

//! Порты сетевого агента
#define KLNAG_SP_NAGENTPORTS            L"Nagentports"
#define KLNAG_SP_NAGENTPORTS_DEFAULT    \
    {   \
        15000, 15001, 15002, 15003, 15004, 15005, 15006, 15007, 15008, 15009,   \
        15010, 15011, 15012, 15013, 15014, 15015, 15016, 15017, 15018, 15019,   \
        15020, 15021, 15022, 15023, 15024, 15025, 15026, 15027, 15028, 15029,   \
        15030, 15031, 15032, 15033, 15034, 15035, 15036, 15037, 15038, 15039,   \
        15040, 15041, 15042, 15043, 15044, 15045, 15046, 15047, 15048, 15049,   \
        15050, 15051, 15052, 15053, 15054, 15055, 15056, 15057, 15058, 15059,   \
        15060, 15061, 15062, 15063, 15064, 15065, 15066, 15067, 15068, 15069,   \
        15070, 15071, 15072, 15073, 15074, 15075, 15076, 15077, 15078, 15079,   \
        15080, 15081, 15082, 15083, 15084, 15085, 15086, 15087, 15088, 15089,   \
        15090, 15091, 15092, 15093, 15094, 15095, 15096, 15097, 15098, 15099,   \
        15100, 15101, 15102, 15103, 15104, 15105, 15106, 15107, 15108, 15109,   \
        15110, 15111, 15112, 15113, 15114, 15115, 15116, 15117, 15118, 15119,   \
        15120, 15121, 15122, 15123, 15124, 15125, 15126, 15127, 15128, 15129,   \
        15130, 15131, 15132, 15133, 15134, 15135, 15136, 15137, 15138, 15139,   \
        15140, 15141, 15142, 15143, 15144, 15145, 15146, 15147, 15148, 15149,   \
        15150, 15151, 15152, 15153, 15154, 15155, 15156, 15157, 15158, 15159,   \
        15160, 15161, 15162, 15163, 15164, 15165, 15166, 15167, 15168, 15169,   \
        15170, 15171, 15172, 15173, 15174, 15175, 15176, 15177, 15178, 15179,   \
        15180, 15181, 15182, 15183, 15184, 15185, 15186, 15187, 15188, 15189,   \
        15190, 15191, 15192, 15193, 15194, 15195, 15196, 15197, 15198, 15199    \
    }

//! Открывать ли порт UDP
#define KLNAG_OPEN_UDPPORT              L"OpenUdpPort"
#define KLNAG_OPEN_UDPPORT_DEFAULT      true

//! Использовать ли Ssl при соединении с сервером
#define KLNAG_USE_SSL                   L"UseSsl"
#define KLNAG_USE_SSL_DEFAULT           true

//! Number of UDP-packets to send
#define KLNAG_SERVER_UDP_PACKETS            L"ServerUdpPackets"
#define KLNAG_SERVER_UDP_PACKETS_DEFAULT    5

//!Server address
#define KLNAG_SERVER_ADDRESS            L"ServerAddress"            //STRING_T
#define KLNAG_SERVER_ADDRESS_DEFAULT    L"localhost"

//!Server ports
#define KLNAG_SERVER_PORTS              L"Serverports"
#define KLNAG_SERVER_PORTS_DEFAULT      KLSRV_SP_SERVER_PORTS_ARRAY_DEFAULT

//!Server SSL ports
#define KLNAG_SERVER_SSL_PORTS          L"ServerSslPorts"
#define KLNAG_SERVER_SSL_PORTS_DEFAULT  KLSRV_SP_SERVER_SSL_PORTS_ARRAY_DEFAULT

#define KLNAG_PROXYHOST_USE                 L"KLNAG_PROXYHOST_USE"      //BOOL_T
#define KLNAG_PROXYHOST_USE_DEFAULT         false

#define KLNAG_PROXYHOST_LOCATION            L"KLNAG_PROXYHOST_LOCATION" //STRING_T
#define KLNAG_PROXYHOST_LOCATION_DEFAULT    L""

#define KLNAG_PROXYHOST_PORT_DEFAULT        3128

#define KLNAG_PROXYHOST_LOGIN               L"KLNAG_PROXYHOST_LOGIN"    //STRING_T
#define KLNAG_PROXYHOST_LOGIN_DEFAULT       L""

//Binary data converted to string
#define KLNAG_PROXYHOST_PASSWORD            L"KLNAG_PROXYHOST_PASSWORD" //STRING_T
#define KLNAG_PROXYHOST_PASSWORD_DEFAULT    L""

//!Nagent's instanceId
#define KLNAG_NAGENT_INSTANCEID         L"NagentConnName"

//! how much to wait until all products are 1st time replicated before sending ping to server
#define KLNAG_WAIT_FIRST_REPLICATION_TIMEOUT_DEF    (180*1000)

//! Need to open ports required for NetBIOS name service (137 UDP) in anti-hacker 6.0
#define KLNAG_ALLOW_NETBIOS_NAME_SERVICE        L"KLNAG_ALLOW_NETBIOS_NAME_SERVICE"
#define KLNAG_ALLOW_NETBIOS_NAME_SERVICE_DEF    true

/*!
    Nagent's private unsynchronizable settings
    this ss contains following variables

    KLNAG_SERVER_ADDRESS
    KLNAG_SERVER_PORTS
    KLNAG_SERVER_SSL_PORTS
*/

#define KLNAG_NAGENT_PRIVATE_SS             L"KLNAG_NAGENT_PRIVATE_SS"
#define KLNAG_NAGENT_PRIVATE_SS_DEFAULT     L"condata.dat"


#define KLNAG_SP_LOCFILE                    L"KLNAG_SP_LOCFILE"
#define KLNAG_SP_LOCFILE_DEFAULT            L"klnagloc.dll"

//! таймаут репликации коннектора
#define KLNAG_SP_CONN_REPL_TIMEOUT          L"KLNAG_SP_CONN_REPL_TIMEOUT"
#define KLNAG_SP_CONN_REPL_TIMEOUT_DEFAULT  180000

//! 'data' directory for nagent
#define KLNAG_DATA_DIR						L"KLNAG_DATA_DIR"    //STRING_T (filename)
#define KLNAG_DATA_DIR_DEFAULT				(KLSTD_StGetDefDirW() + L"data/")

//! cleaner update period
#define KLNAG_CLNR_UPD_PERIOD				L"KLNAG_CLNR_UPD_PERIOD"    //INT_T
#define KLNAG_CLNR_UPD_PERIOD_DEFAULT		3600000

//! If nagent opens ports in wfw
#define KLNAG_SP_OPEN_PORT_IN_WFW           L"KLNAG_SP_OPEN_PORT_IN_WFW"
#define KLNAG_SP_OPEN_PORT_IN_WFW_DEFAULT   true

//private ss parameter
#define KLNAG_SP_SHOULD_MODIFY_WITH_POLICY          L"KLNAG_CH_WITH_POL"
#define KLNAG_SP_SHOULD_MODIFY_WITH_POLICY_DEFAULT  true


//private ss parameter
#define KLNAG_WAS_AT_LEAST_ONE_SYN          L"KLNAG_WAS_AT_LEAST_ONE_SYN"
#define KLNAG_WAS_AT_LEAST_ONE_SYN_DEFAULT  false

//---------------------------------------------------------
//   END of main settins section KLNAG_COMPONENT_NAME
//---------------------------------------------------------

//! In this section hostid is stored (product private storage)
#define KLNAG_STORED_HOSTID_SECTION L"KLNAG_HOSTID_SECTION"

//! dupfix info is stored here
#define KLNAG_STORED_DUPFIX_SECTION L"KLNAG_DUPFIX_SECTION"
//  c_spPingMacAddresses
//  c_spPingWinHostName

#define KLNAG_SERVER_FAIL_REPORT_PERIOD_DEFAULT     (6L*3600L*1000L) //6h
#define KLNAG_MAY_SERVER_FAIL_REPORT_PERIOD_DEFAULT (10L*60L*1000L)  //10m

#ifdef _WIN32
#define KLNAG_FT_WORKING_FOLDER L"$FTClTmp"
#else
#define KLNAG_FT_WORKING_FOLDER ((KLSTD_StGetDefDirW() + L"$FTClTmp").c_str())
#endif

//---------------------------------------------------------
//  BEGIN of section KLNAG_SECTION_SERVERDATA.
//      This section is filled by the server only
//---------------------------------------------------------


#define KLNAG_PRODUCT_SERVERDATA    KLNAG_PRODUCT_NAME
#define KLNAG_VERSION_SERVERDATA    KLNAG_PRODUCT_VERSION
#define KLNAG_SECTION_SERVERDATA    L"KLNAG_SECTION_SERVERDATA"

#define KLNAG_SSL_SERVER_CERT           L"KLNAG_SSL_SERVER_CERT"
#define KLNAG_SSL_SERVER_CERT_DEFAULT   L"klserver.cer"

#define KLNAG_HOSTID                    L"KLNAG_HOSTID"
#define KLNAG_HOSTID_DEFAULT            L""

#define KLNAG_RECONNECTING              L"KLNAG_RECONNECTING"
#define KLNAG_RECONNECTING_DEFAULT      false

#define KLNAG_RECONNECT_DUPFIX          L"KLNAG_RECONNECT_DUPFIX"
#define KLNAG_RECONNECT_DUPFIX_DEFAULT  false

//---------------------------------------------------------
//  END of section KLNAG_SECTION_SERVERDATA.
//---------------------------------------------------------

/*!
    Section KLNAG_SECTION_RECONNECTDATA.
    Contains variables:
        KLNAG_SERVER_ADDRESS
        KLNAG_SERVER_PORTS
        KLNAG_SERVER_SSL_PORTS
        KLNAG_SSL_SERVER_CERT
        KLNAG_BACKINGUP
*/

#define KLNAG_PRODUCT_RECONNECTDATA KLPRSS_PRODUCT_CORE
#define KLNAG_VERSION_RECONNECTDATA KLNAG_PRODUCT_VERSION
#define KLNAG_SECTION_RECONNECTDATA L"KLNAG_SECTION_RECONNECTDATA"

#define KLNAG_BACKINGUP             L"KLNAG_BACKINGUP"
#define KLNAG_BACKINGUP_DEFAULT     bool(false)

#define KLNAG_SAVE_CERT             L"KLNAG_SAVE_CERT"
#define KLNAG_SAVE_CERT_DEFAULT     bool(false)

//---------------------------------------------------------
//  BEGIN of Custom authentication data
//---------------------------------------------------------

//!Custom authentication data
#define KLNAG_AUTH_NAGENT_NAME          L"KLNAG_AUTH_NAGENT_NAME"
#define KLNAG_AUTH_NAGENT_ID            L"KLNAG_AUTH_NAGENT_ID"

//---------------------------------------------------------
//  END of Custom authentication data
//---------------------------------------------------------

//---------------------------------------------------------
//  BEGIN of Installer specific data
//---------------------------------------------------------

#define KLNAG_SS_INSTALLER_SECTION_NAME			L"NGTINSTALLERDATA"
#define KLNAG_INSTALLER_UNINSTALL_PASSWORD		L"NGT_UNINSTALL_PASSWORD"	// STRING_T Пароль деинсталляции

//---------------------------------------------------------
//  END of Installer specific data
//---------------------------------------------------------

#define KLNAG_PRIV_TRCACHED_LOCS_SECTION    L"KLNAG_TRCAHCEDLOCS"

#define KLNAG_PRIV_TRCACHED_LOCS L"KLNAG_TRCACHED_LOCS"

namespace KLNAG
{

    const int c_pNagentPortsDefault[]=KLNAG_SP_NAGENTPORTS_DEFAULT;
//! Главная секция настроек
	const wchar_t c_spServerAddress[]=KLNAG_SERVER_ADDRESS;
	const wchar_t c_spServerPorts[]=KLNAG_SERVER_PORTS;	                    //ARRAY_T INT_T
    const wchar_t c_spServerSslPorts[]=KLNAG_SERVER_SSL_PORTS;	                    //ARRAY_T INT_T
	const wchar_t c_spServerPingTimeout[]=KLNAG_SP_SERVER_PING_TIMEOUT;	    //INT_T
	const wchar_t c_spNagentPorts[]=KLNAG_SP_NAGENTPORTS;                   //ARRAY_T INT_T
    const wchar_t c_spOpenUdpPort[]=KLNAG_OPEN_UDPPORT;                     //BOOL_T
	const wchar_t c_spNagentConnName[]=KLNAG_NAGENT_INSTANCEID;             //STRING_T

//!Custom authentication data
    //Имя сетевого агента - совпадает с его instanceid
    const wchar_t c_spAuthNagentName[] = KLNAG_AUTH_NAGENT_NAME;    //STRING_T
    const wchar_t c_spAuthNagentId[] = KLNAG_AUTH_NAGENT_ID;        //STRING_T

//! Пинги
    const wchar_t c_spNagentAddress[]=L"KLNAG_PEER_ADDRESS";        //INT_T
    const wchar_t c_spPingHostId[]=L"KLNAG_PING_HOSTID";            //STRING_T
    const wchar_t c_spPingNagentId[]=L"KLNAG_PING_NAGENTID";        //STRING_T
    const wchar_t c_spPingWinHostName[]=L"KLNAG_PING_WINHOSTNAME";  //STRING_T
    const wchar_t c_spPingWinDomain[]=L"KLNAG_PING_WINDOMAIN";      //STRING_T
    const wchar_t c_spPingWinDomainType[]=L"KLNAG_PING_WINDOMAINTYPE";  //INT_T
    const wchar_t c_spPingPresumedDN[]=L"KLNAG_PING_PRESUMED_DN";   //STRING_T
    const wchar_t c_spPingPresumedGroup[]=L"KLNAG_PING_PRESUMED_GROUP"; //STRING_T
    const wchar_t c_spPingDnsName[]=L"KLNAG_PING_DNS_NAME";         //STRING_T
    const wchar_t c_spPingDnsDomain[]=L"KLNAG_PING_DNS_DOMAIN";         //STRING_T
    const wchar_t c_spPingAddress[]=L"KLNAG_PING_ADDRESS";          //STRING_T
    const wchar_t c_spPingUdpPort[]=L"KLNAG_PING_UDP_PORT";          //INT_T
    const wchar_t c_spPingIsReconnecting[]=L"KLNAG_PING_RECONNECTING"; //BOOL_T
    const wchar_t c_spProductsStates[]=L"KLNAG_PRODUCTS_STATES";//ARRAY_T|PARAMS_T
        const wchar_t c_spProductName[]=L"KLNAG_PRODUCT_NAME";//STRING_T
        const wchar_t c_spProductVersion[]=L"KLNAG_PRODUCT_VERSION";//STRING_T
        const wchar_t c_spProductDispVersion[]=L"KLNAG_PRODUCT_DISP_VERSION";//STRING_T
        const wchar_t c_spNagentDisplayName[]=L"KLNAG_PRODUCT_DISP_NAME";//STRING_T
        const wchar_t c_spProductState[]=L"KLNAG_PRODUCT_STATE";//INT_T
        const wchar_t c_spLicinfoChanged[]=L"KLNAG_LICINFO_CHANGED";//BOOL_T
        const wchar_t c_spProdCompChanged[]=L"KLNAG_PRODCOMP_CHANGED";//BOOL_T
        const wchar_t c_spCurrentKey[]=L"KLNAG_CURRENT_KEY";    //PARAMS_T
        const wchar_t c_spNextKey[]=L"KLNAG_NEXT_KEY";          //PARAMS_T
        const wchar_t c_spAvBasesDate[]=L"KLNAG_AV_BASES_DATE"; //DATE_TIME_T
        const wchar_t c_spAvBasesRecords[]=L"KLNAG_AV_BASES_RECORDS"; //INT_T
        const wchar_t c_spUpdateDate[]=L"KLNAG_UPDATE_DATE"; //DATE_TIME_T
        const wchar_t c_spInstallDate[]=L"KLNAG_INSTALL_DATE"; //DATE_TIME_T
    const wchar_t c_spSsSettingsChanged[]=L"KLNAG_SS_SETTINGS_CHANGED"; //BOOL_T
    const wchar_t c_spSsProdinfoChanged[]=L"KLNAG_SS_PRODINFO_CHANGED"; //BOOL_T
    const wchar_t c_spNeedCertificate[] = L"KLNAG_SSL_NEED_CERTIFICATE"; //BINARY_T
    const wchar_t c_spPingHasRtp[]=L"KLNAG_PING_HAS_RTP";       //BOOL_T
    const wchar_t c_spPingRtpState[]=L"KLNAG_PING_RTP_STATE";   //INT_T
    const wchar_t c_spPingRtpErrorCode[]=L"KLNAG_PING_RTP_ERROR_CODE";   //INT_T
    const wchar_t c_spPingMacAddresses[]=L"KLNAG_PING_MAC_ADDRESSES";   //ARRAY_T|STRING_T
    const wchar_t c_spPingHostKeyHash[] = L"KLNAG_PING_HOSTKEY_HASH";   //BINARY_T
    const wchar_t c_spPingGlobalKeyHash[] = L"KLNAG_PING_GLOBALKEY_HASH";   //BINARY_T
    const wchar_t c_spPingMacAddressesSrv[] = L"KLNAG_PING_MACADDR_SRV"; //ARRAY_T|STRING_T
    const wchar_t c_spPingHstIds[] = L"KLNAG_PING_HST_IDS";//ARRAY_T|STRING_T
    const wchar_t c_spPingHstIdsSrv[] = L"KLNAG_PING_HST_IDS_SRV";//ARRAY_T|STRING_T
    const wchar_t c_spNagentVersion[] = L"KLNAG_VERSION"; //INT_T, one of KLCSAK_SV_*
    const wchar_t c_spPingNameSrv[] = L"KLNAG_PING_NAME_SRV"; //STRING_T
    const wchar_t c_spPingInRoamingMode[] = L"KLNAG_PING_ROAMING_MODE"; //BOOL_T

    //c_spTasksStorages

    /*
    const wchar_t c_spNagentDisplayName[]=L"klnag-DisplayName";	//STRING_T
	const wchar_t c_spNagentName[]=L"klnag-Name";	            //STRING_T
	const wchar_t c_spNagentAddress[]=L"klnag-Address";         //STRING_T
	const wchar_t c_spNagentHost[]=L"klnag-Host";	            //STRING_T
	const wchar_t c_spNagentDomain[]=L"klnag-Domain";	        //STRING_T
	const wchar_t c_spNagentDomainType[]=L"klnag-DomainType";	//INT_T
    */

	//! Запрос порта
	const wchar_t c_spInstancePort[]=L"klnag-InstancePort";

	//! Запрос задач

	//c_spRtTsksInfo	//PARAMS_T
	//	<ProductName>	//PARAMS_T
	//		c_spProductVersion	//STRING_T
	//		c_spRtCmpArray	//ARRAY_T|STRING_T
	//		c_spRtTskArray	//PARAMS_T|ARRAY_T
	//			KLHST::c_tsk_Type	//STRING_T
	//			KLHST::c_tsk_State	//INT_T
	//			KLHST::c_tsk_Id		//INT_T
	//			KLHST::c_tsk_StorageId // STRING_T
	//c_spRtInstArray	//ARRAY_T|STRING_T

// Static info
	//Installed products
	const wchar_t c_spProducts[]=L"KLNAG_STINFPRODUCTS";//ARRAY_T|PARAMS_T
    //
    const wchar_t c_spTasksStorages[]=L"KLNAG_STINF_TASKSTORAGEFILES";//PARAMS_T
    /*
        Запрос
        <ProductName>   //product
            <Version>   //any
        Ответ
        <ProductName>   //product
            <Version>   //ARRAY_T|PARAMS_T
                c_spTasksStoragesNames
                c_spTasksStoragesFlags
    */
    const int c_nTasksStoragesFlags_New        =   0x0000001;
    const int c_nTasksStoragesFlags_Changed    =   0x0000002;
    const wchar_t c_spTasksStoragesNames[]=L"KLNAG_STINF_TS_FILES_NAMES";//ARRAY_T|STRING_T
    const wchar_t c_spTasksStoragesFlags[]=L"KLNAG_STINF_TS_FILES_FLAGS";//ARRAY_T|INT_T
    const wchar_t c_spTasksStoragesServerIds[]=L"KLNAG_STINF_TS_SERVER_IDS";//ARRAY_T|STRING_T
    const wchar_t c_spTasksStoragesNagentIds[]=L"KLNAG_STINF_TS_NAGENT_IDS";//ARRAY_T|STRING_T

    const wchar_t c_spTasksStoragesFilesRemove[]=L"KLNAG_FilesToRemove";//ARRAY_T
    const wchar_t c_trUndeletedFilesIndexes[]=L"KLNAG_TR_UNREMOVED";//ARRAY_T
// Runtime Info
	//Statistics
	const wchar_t c_spStatistics[]=L"klnag-inst-Statistics";//PARAMS_T
	//	<instanceid> PARAMS_T

	//General
	const wchar_t c_spTimeout[]=L"klnag-Timeout";			//INT_T
//	const wchar_t c_spProductName[]=L"klnag-ProductName";	//STRING_T
//	const wchar_t c_spProductVersion[]=L"klnag-ProductVersion";	//STRING_T
//	const wchar_t c_spComponentName[]=L"klnag-ComponentName";	//STRING_T
//	const wchar_t c_spInstanceId[]=L"klnag-InstanceId";	//STRING_T


    const long c_nMaxKeySize=1024*100;
    const wchar_t c_tpKeyContents[]=L"KLNAG_TP_KEY_CONTENTS"; //BINARY_T
    const wchar_t c_tpKeyFileName[]=L"KLNAG_TP_KEY_FNAME";
    const wchar_t c_tpIsCurrentKey[]=L"KLNAG_TP_IS_CURRENT_KEY";


    //! Events
    #define KLNAG_EVENT_CODES_BASE  1024L

    #define KLNAG_EVP_SEVERITY  KLEVP::c_er_severity
    #define KLNAG_EVP_STDID     KLEVP::c_er_locid
    #define KLNAG_STDPAR1       KLEVP::c_er_par1
    #define KLNAG_STDPAR2       KLEVP::c_er_par2
    #define KLNAG_STDPAR3       KLEVP::c_er_par3
    #define KLNAG_STDPAR4       KLEVP::c_er_par4
    #define KLNAG_STDPAR5       KLEVP::c_er_par5
    #define KLNAG_STDPAR6       KLEVP::c_er_par6
    #define KLNAG_STDPAR7       KLEVP::c_er_par7
    #define KLNAG_STDPAR8       KLEVP::c_er_par8
    #define KLNAG_STDPAR9       KLEVP::c_er_par9
    #define KLNAG_STDDESCR      KLEVP::c_er_descr
    #define KLNAG_STDDN         KLEVP::c_er_event_type_display_name


    //! Application started
        const wchar_t c_evAppStarted[]=L"KLNAG_EV_APP_STARTED";

        #define KLNAG_APPLICATION_STARTED           (KLNAG_EVENT_CODES_BASE+0)
        #define KLNAG_APPLICATION_STARTED_SEVERITY  KLEVP_EVENT_SEVERITY_INFO
        // %1-appname; %2-appversion
        // L"Application '%1' v %2 started"
        #define KLNAG_APPLICATION_STARTED_DEF       L"Application '%1' v %2 started"

    //! Application stopped
        const wchar_t c_evAppStopped[]=L"KLNAG_EV_APP_STOPPED";

        #define KLNAG_APPLICATION_STOPPED  (KLNAG_EVENT_CODES_BASE+1)
        #define KLNAG_APPLICATION_STOPPED_SEVERITY  KLEVP_EVENT_SEVERITY_INFO
        // %1-appname; %2-appversion
        // L"Application '%1' v %2 stopped"
        #define KLNAG_APPLICATION_STOPPED_DEF   L"Application '%1' v %2 stopped"


    //! Application unexpectedly stopped
        const wchar_t c_evAppStoppedUnexpectedly[]=L"KLNAG_EV_APP_TERMINATED_UNEXPECTEDLY";

        #define KLNAG_APPLICATION_STOPPED_UNEXPECTEDLY  (KLNAG_EVENT_CODES_BASE+2)
        #define KLNAG_APPLICATION_STOPPED_UNEXPECTEDLY_SEVERITY  KLEVP_EVENT_SEVERITY_WARNING
        // %1-appname; %2-appversion
        // L"Application '%1' v %2 unexpectedly stopped"
        #define KLNAG_APPLICATION_STOPPED_UNEXPECTEDLY_DEF       L"Application '%1' v %2 unexpectedly stopped"

    //! Host enters roaming mode
        const wchar_t c_evHostEnterRoamingMode[]=L"KLNAG_EV_ENTER_ROAMING";
        #define KLNAG_EV_ENTER_ROAMING_DN       (KLNAG_EVENT_CODES_BASE+3)
        #define KLNAG_EV_ENTER_ROAMING_DN_DEF   L"Enter roaming mode"
        #define KLNAG_EV_ENTER_ROAMING          (KLNAG_EVENT_CODES_BASE+4)
        #define KLNAG_EV_ENTER_ROAMING_SEVERITY KLEVP_EVENT_SEVERITY_INFO
        // L"Enter roaming mode"
        #define KLNAG_EV_ENTER_ROAMING_DEF       L"Host entered roaming mode"

    //! Host leaves roaming mode
        const wchar_t c_evHostLeaveRoamingMode[]=L"KLNAG_EV_LEAVE_ROAMING";
        #define KLNAG_EV_LEAVE_ROAMING_DN       (KLNAG_EVENT_CODES_BASE+5)
        #define KLNAG_EV_LEAVE_ROAMING_DN_DEF   L"Leave roaming mode"
        #define KLNAG_EV_LEAVE_ROAMING          (KLNAG_EVENT_CODES_BASE+6)
        #define KLNAG_EV_LEAVE_ROAMING_SEVERITY KLEVP_EVENT_SEVERITY_INFO
        // L"Enter leave mode"
        #define KLNAG_EV_LEAVE_ROAMING_DEF       L"Host left roaming mode"

//! Internal events

    //! Event contains increment for viruses found on host
        const wchar_t c_evVirusFoundIncrement[] = L"KLNAG_EV_VCOUNT_INC";
        //%1-delta

    //! Event contains increment for viruses found on host
        const wchar_t c_evLastFullScanTime[] = L"KLNAG_EV_LAST_FSCAN";
        //%1-time

    //! Event contains new rtp state for host
        const wchar_t c_evRtpStateChanged[] = L"KLNAG_EV_RTP_STATE_CHANGED";
        //1 - new state KLBL::GlobalRptState
        //2 - error code

    //! Event contains new app state[s] for host
        const wchar_t c_evAppStateChanged[] = L"KLNAG_EV_APP_STATE_CHANGED";
        //1 - ARRAY_T|PARAMS_T
            //2 - App name
            //3 - App version
            //4 - new state
        //or if single state changed
        //2 - App name
        //3 - App version
        //4 - new state

    //! Event contains new task state[s] for host
        const wchar_t c_evTskStateChanged[] = L"KLNAG_EV_TSK_STATE_CHANGED";
        //1 - ARRAY_T|PARAMS_T
            //1 - App name
            //2 - App version
        //2 - ARRAY_T|PARAMS_T
            //1 - index of product
            //2 - new state
            //3 - taskstorageId
        
    //! Event contains new app state[s] for host
        const wchar_t c_evAppAvDbStateChanged[] = L"KLNAG_EV_APP_AVDB_STATE_CHANGED";
        //1 - ARRAY_T|PARAMS_T
            //2 - App name
            //3 - App version
            //4 - new state, PARAMS_T
        

    //! Event contains uncured objects number for host
        const wchar_t c_evAppUncuredChanged[] = L"KLNAG_EV_APP_UNCURED_CHANGED";
        //1 - ARRAY_T|PARAMS_T
            //1 - App name
            //2 - App version
            //3 - new state, long


    //! Event change roaming mode
        const wchar_t c_evChangeRoamingMode[] = L"KLNAG_EV_CHANGE_RM";
        //1 - new rm value (true if enter, false otherwise) ,BOOL_T
        //2 - reason code: 1 - netchecker, 2 - ping failure, 3 - ping success, INT_T
        enum ChangeRmReason
        {
            CHRMR_NETCHECK = 1,
            CHRMR_PING_FAIL,
            CHRMR_ACCESS_SUCCESS
        };

	//!
    #define KLNAG_TASK_INSTALL_KEY      L"KLNAG_TASK_INSTALL_KEY"
    //c_tpKeyContents
    //c_tpKeyFileName
    //c_tpIsCurrentKey
    //c_spProductName
    //c_spProductVersion

	#define KLNAG_TASK_CHANGE_SECTION	L"KLNAG_TASK_CHANGE_SECTION"

	#define KLNAG_TASK_FULL_SYNC		L"KLNAG_TASK_FULL_SYNC"
        
    #define KLNAG_TASK_ACDT_API         L"KLNAG_TASK_ACDT_API"
        #define KLNAG_TASK_ACDT_DATA        L"Data" //PARAMS_T

    #define KLNAG_TASK_RECONNECTION     L"KLNAG_TASK_RECONNECTION"
        /*! настройки задачи KLNAG_TASK_RECONNECTION
            1. KLNAG_SERVER_ADDRESS - адрес сервера
            2. KLNAG_SERVER_PORTS - порты сервера
            3. KLNAG_SERVER_SSL_PORTS - SSL порты сервера
            4. KLNAG_SSL_SERVER_CERT - сертификат сервера
            5. KLNAG_TASK_RECONNECTION_TIMEOUT - сертификат сервера
        */
        #define KLNAG_TASK_RECONNECTION_TIMEOUT         L"KLNAG_TASK_RCN_TIMEOUT" //INT_T
        #define KLNAG_TASK_RECONNECTION_TIMEOUT_DEFAULT 60000

    #define KLNAG_TASK_REMOTE_INSTALL  L"KLNAG_TASK_REMOTE_INSTALL"
        /*! настройки задачи KLNAG_TASK_REMOTE_INSTALL
            1. KLFT::EVENT_BODY_FILE_ID - ID пересылаемого файла инсталлятора
            2. KLNAG_SERVER_TASK_FILE_ID - m_wstrFileIdentity групповой задачи сервера
            3. KLNAG_SERVER_TASK_ID -	ID групповой задачи сервера
        */
		#define KLNAG_SERVER_TASK_FILE_ID L"KLNAG_SERVER_TASK_FILE_ID"
		#define KLNAG_SERVER_TASK_ID L"KLNAG_SERVER_TASK_ID"

		// Событие TASK_COMPLETED может иметь доп. параметры, в т.ч., KLNAG_TASK_REMOTE_INSTALL_STATUS:
		#define KLNAG_TASK_REMOTE_INSTALL_STATUS L"KLNAG_TASK_REMOTE_INSTALL_STATUS"
			#define KLNAG_TASK_REMOTE_INSTALL_COMPLETED_WAITING_FOR_FILE 0
			#define KLNAG_TASK_REMOTE_INSTALL_COMPLETED_SETUP_STARTED 1
			#define KLNAG_TASK_REMOTE_INSTALL_COMPLETED_SETUP_COMPLETED 2
			#define KLNAG_TASK_REMOTE_INSTALL_COMPLETED_SETUP_TIMEOUT 3
			#define KLNAG_TASK_REMOTE_INSTALL_COMPLETED_WAITING_FOR_OTHER_TASK 4
			#define KLNAG_TASK_REMOTE_INSTALL_COMPLETED_UNEXPECTED_REBOOT 5
		#define KLNAG_TASK_REMOTE_INSTALL_RESULT_CODE L"KLNAG_TASK_REMOTE_INSTALL_RESULT_CODE"
		#define KLNAG_TASK_REMOTE_INSTALL_PROC_RESULT_CODE L"KLNAG_TASK_REMOTE_INSTALL_PROC_RESULT_CODE"
		#define KLNAG_TASK_REMOTE_INSTALL_SCRPT_RESULT_CODE L"KLNAG_TASK_REMOTE_INSTALL_SCRPT_RESULT_CODE"
		#define KLNAG_TASK_REMOTE_INSTALL_SCRPT_RESULT_CODE_EX L"KLNAG_TASK_REMOTE_INSTALL_SCRPT_RESULT_CODE_EX"
		#define KLNAG_TASK_REMOTE_INSTALL_SCRPT_RPT L"KLNAG_TASK_REMOTE_INSTALL_SCRPT_RPT"

    #define KLNAG_TASK_START_STOP_APP   L"KLNAG_TASK_START_STOP_APP"
        /*! настройки задачи KLNAG_TASK_START_STOP_APP
            1. KLNAG_TASK_START_STOP_APP_ACTIONSTART - true, если стартовать, false, если остановить
            2. KLNAG_TASK_START_STOP_APP_LISTAPPS - массив Params, в каждом Params 4 параметра:
                    c_spProductName, c_spProductVersion, c_spProductDispVersion, c_spNagentDisplayName
        */
        #define KLNAG_TASK_START_STOP_APP_ACTIONSTART    L"KLNAG_TASK_START_STOP_APP_ACTIONSTART" //BOOL_T
        #define KLNAG_TASK_START_STOP_APP_LISTAPPS    L"KLNAG_TASK_START_STOP_APP_LISTAPPS" //ARRAY_T|PARAMS_T

    #define KLNAG_TASK_CONN_REPLICATION     L"KLNAG_TASK_CONN_REPLICATION"
        #define KLNAG_TASK_CONN_REPLICATION_PRODUCTS    L"PRODUCTS"
        /*
            KLNAG_TASK_CONN_REPLICATION_PRODUCTS    ARRAY_T|PARAMS_T
                c_spProductName,
                c_spProductVersion
        */
    #define KLNAG_TASK_CONN_DEBUGFLAGS      L"KLNAG_TASK_CONN_DEBUGFLAGS"
        #define KLNAG_TASK_CONN_DEBUGFLAGS_FLAGS    L"FLAGS"

    #define KLNAG_TASK_SHOW_MSG     L"KLNAG_TASK_SHOW_MSG" // Задача уведомления пользователя
        /*! настройки задачи KLNAG_TASK_SHOW_MSG
            1. KLNAG_MSG_CAPTION - заголовок сообщения
            2. KLNAG_MSG_TEXT - текст сообщения
        */
        #define KLNAG_MSG_CAPTION	L"KLNAG_MSG_CAPTION" // STRING_T
        #define KLNAG_MSG_TEXT		L"KLNAG_MSG_TEXT" // STRING_T

    #define KLNAG_TASK_COMP_CONTROL     L"KLNAG_TASK_COMP_CONTROL"
        /*! настройки задачи KLNAG_TASK_COMP_CONTROL
			1. KLNAG_TASK_COMP_CONTROL_SHUTDOWN - команда управления - выключить компьютер или включить. TRUE = выкл. // BOOL_T
			2. KLNAG_SHUTDOWN_TURN_ON_AFTER_SHUTDOWN - включать компьютер после выключения (перезагрузка )// BOOL_T
			3. KLNAG_SHUTDOWN_ASK_CAPTION - заголовок окна сообщения о выключении/перезагрузке // STRING_T
			4. KLNAG_SHUTDOWN_ASK_QUESTION - текст вопроса с предложением о выключении/перезагрузки // STRING_T
			5. KLNAG_SHUTDOWN_IMMEDIATELY - немедленное выключение/перезагрузка // BOOL_T
			6. KLNAG_SHUTDOWN_ASK - вопрос о выключении/перезагрузке // BOOL_T
			7. KLNAG_SHUTDOWN_ASK_PERIOD - периодичность вопроса о выключении/перезагрузке // INT_T
			8. KLNAG_SHUTDOWN_FORCE - форсировать выключение/перезагрузку // BOOL_T
			9. KLNAG_SHUTDOWN_FORCE_TIME - интервал времени, по истечении которого форсировать выключение/перезагрузку // INT_T
			10. KLNAG_SHUTDOWN_FROCE_CLOSE_APP - форсировать закрытие приложений // BOOL_T
			11.KLNAG_SHUTDOWN_FROCE_CLOSE_APP_PERIOD - интервал времени, по истечении которого форсировать закрытие приложений // INT_T
        */
		#define KLNAG_TASK_COMP_CONTROL_SHUTDOWN L"KLNAG_SHUTDOWN_TURN_ON_AFTER_SHUTDOWN" // BOOL_T
		#define KLNAG_SHUTDOWN_TURN_ON_AFTER_SHUTDOWN L"KLNAG_SHUTDOWN_TURN_ON_AFTER_SHUTDOWN" // BOOL_T
		#define KLNAG_SHUTDOWN_ASK_CAPTION L"KLNAG_SHUTDOWN_ASK_CAPTION" // STRING_T
		#define KLNAG_SHUTDOWN_ASK_QUESTION L"KLNAG_SHUTDOWN_ASK_QUESTION" // STRING_T
		#define KLNAG_SHUTDOWN_IMMEDIATELY L"KLNAG_SHUTDOWN_IMMEDIATELY" // BOOL_T
		#define KLNAG_SHUTDOWN_ASK L"KLNAG_SHUTDOWN_ASK" // BOOL_T
		#define KLNAG_SHUTDOWN_ASK_PERIOD L"KLNAG_SHUTDOWN_ASK_PERIOD" // INT_T
		#define KLNAG_SHUTDOWN_FORCE L"KLNAG_SHUTDOWN_FORCE" // BOOL_T
		#define KLNAG_SHUTDOWN_FORCE_TIME L"KLNAG_SHUTDOWN_FORCE_TIME" // INT_T
		#define KLNAG_SHUTDOWN_FROCE_CLOSE_APP L"KLNAG_SHUTDOWN_FROCE_CLOSE_APP" // BOOL_T
		#define KLNAG_SHUTDOWN_FROCE_CLOSE_APP_PERIOD L"KLNAG_SHUTDOWN_FROCE_CLOSE_APP_PERIOD" // INT_T

	#define KLNAG_TASK_CHECK_BASES		L"KLNAG_TASK_CHECK_BASES"
		/*! настройки задачи KLNAG_TASK_CHECK_BASES
			1. KLNAG_TASK_CHECK_BASES_NEED_RESTART - true, если перезагружать компьютер после закачки баз
			2. KLNAG_TASK_CHECK_BASES_CHECK_RDP_STATE - true проверять статуст RDP после закачки и после перезапуска
			3. KLNAG_TASK_CHECK_BASES_LISTAPPS - массив Params, в каждом Params 4 параметра:
                    c_spProductName, c_spProductVersion, 
					KLNAG_TASK_CHECK_BASES_UPDATE_TASKSTORAGE_ID, 
					KLNAG_TASK_CHECK_BASES_SCAN_TASKSTORAGE_ID
		*/
		#define KLNAG_TASK_CHECK_BASES_NEED_RESTART L"KLNAG_TASK_CHECK_BASES_NEED_RESTART" // BOOL_T
		#define KLNAG_TASK_CHECK_BASES_CHECK_RDP_STATE L"KLNAG_TASK_CHECK_BASES_CHECK_RDP_STATE" // BOOL_T
		#define KLNAG_TASK_CHECK_BASES_LISTAPPS L"KLNAG_TASK_CHECK_BASES_LISTAPPS" //ARRAY_T|PARAMS_T 
			#define KLNAG_TASK_CHECK_BASES_UPDATE_TASKSTORAGE_ID L"KLNAG_TASK_CHECK_BASES_UPDATE_TASKSTORAGE_ID" // STRING_T
			#define KLNAG_TASK_CHECK_BASES_SCAN_TASKSTORAGE_ID L"KLNAG_TASK_CHECK_BASES_SCAN_TASKSTORAGE_ID" // STRING_T

    //Statistics
    #define KLNAG_ST_PING_ATTEMPTS_CNT      L"KLNAG_ST_PING_ATTEMPTS_CNT"
    #define KLNAG_ST_PING_ATTEMPTS_SUC_CNT  L"KLNAG_ST_PING_ATTEMPTS_SUC_CNT"
    #define KLNAG_ST_SYNC_ATTEMPTS_CNT      L"KLNAG_ST_SYNC_ATTEMPTS_CNT"
    #define KLNAG_ST_SYNC_ATTEMPTS_SUC_CNT  L"KLNAG_ST_SYNC_ATTEMPTS_SUC_CNT"
    #define KLNAG_ST_LAST_PING_ATTEMPT      L"KLNAG_ST_LAST_PING_ATTEMPT"
    #define KLNAG_ST_CONNECTORS             L"KLNAG_ST_CONNECTORS"

	#define KLNAG_ST_UA_STATISTICS			L"KLNAG_ST_UA_STATISTICS"

		#define KLNAG_ST_UA_CREATE_TIME				L"KLNAG_ST_UA_CREATE_TIME"

		#define KLNAG_ST_UA_REMOTE_INSTALL			L"KLNAG_ST_UA_REMOTE_INSTALL"

			#define KLNAG_ST_UA_NUMBER_OF_FILES			L"KLNAG_ST_UA_NUMBER_OF_FILE"
			#define KLNAG_ST_UA_SIZE_OF_FILES			L"KLNAG_ST_UA_SIZE_OF_FILES"
			#define KLNAG_ST_UA_SUCCEDED_UPLOADS		L"KLNAG_ST_UA_SUCCEDED_UPLOADS"
			#define KLNAG_ST_UA_SUCCEDED_UPLOADS_SIZE	L"KLNAG_ST_UA_SUCCEDED_UPLOADS_SIZE"
			#define KLNAG_ST_UA_TCP_UPLOADED_SIZE		L"KLNAG_ST_UA_TCP_UPLOADED_SIZE"
			#define KLNAG_ST_UA_MULITICAST_UPLOADED_SIZE	L"KLNAG_ST_UA_MULITICAST_UPLOADED_SIZE"
			#define KLNAG_ST_UA_MULITICAST_SENT_SIZE	L"KLNAG_ST_UA_MULITICAST_SENT_SIZE"
			#define KLNAG_ST_UA_MULITICAST_PERCENT_USAGE	L"KLNAG_ST_UA_MULITICAST_PERCENT_USAGE"

		#define KLNAG_ST_UA_FOLDER_SYNC				L"KLNAG_ST_UA_FOLDER_SYNC"

			#define KLNAG_ST_UA_FOLDER_SYNC_LAST_SERVER_SYNC_TIME	L"KLNAG_ST_UA_FOLDER_SYNC_LAST_SERVER_SYNC_TIME"
			#define KLNAG_ST_UA_FOLDER_SYNC_SERVER_SYNC_COUNT		L"KLNAG_ST_UA_FOLDER_SYNC_SERVER_SYNC_COUNT"
			#define KLNAG_ST_UA_FOLDER_SYNC_MULTICAST_SENT_COUNT	L"KLNAG_ST_UA_FOLDER_SYNC_MULTICAST_SENT_COUNT"
			#define KLNAG_ST_UA_FOLDER_SYNC_MULTICAST_SENT_SIZE		L"KLNAG_ST_UA_FOLDER_SYNC_MULTICAST_SENT_SIZE"
			#define KLNAG_ST_UA_FOLDER_SYNC_TCP_UPLOADED_COUNT		L"KLNAG_ST_UA_FOLDER_SYNC_TCP_UPLOADED_COUNT"
			#define KLNAG_ST_UA_FOLDER_SYNC_TCP_UPLOADED_SIZE		L"KLNAG_ST_UA_FOLDER_SYNC_TCP_UPLOADED_SIZE"
			#define KLNAG_ST_UA_FOLDER_SYNC_MULITICAST_PERCENT_USAGE	L"KLNAG_ST_UA_FOLDER_SYNC_MULITICAST_PERCENT_USAGE"

    //SS_PRODINFO: (".core", ".independent", KLNAG_SEC_COMPUTER_INFO)
    #define KLNAG_SEC_COMPUTER_INFO         L"KLNAG_COMPUTER_INFO"
    #define KLNAG_CI_CTYPE                  L"ctype"    //INT_T
    #define KLNAG_CI_PTYPE                  L"ptype"    //INT_T
    #define KLNAG_CI_OSVER                  L"osver"    //LONG_T
    
    // methods
    const wchar_t c_szwMethod__AvPrt_UpdateAvInstalled[] = L"KLNAG_MTH__AvPrt_UpdateAvInstalled";
    const wchar_t c_szwMethod__AvPrt_UpdateAvRunning[] = L"KLNAG_MTH__AvPrt_UpdateAvRunning";
    const wchar_t c_szwMethod__AvPrt_UpdateRtpState[] = L"KLNAG_MTH__AvPrt_UpdateRtpState";
    const wchar_t c_szwMethod__AvPrt_UpdateLastFScan[] = L"KLNAG_MTH__AvPrt_UpdateLastFScan";
    const wchar_t c_szwMethod__AvPrt_UpdateAvBasesDate[] = L"KLNAG_MTH__UpdateAvBasesDate";
    const wchar_t c_szwMethod__AvPrt_UpdateLastConnected[] = L"KLNAG_MTH__UpdateLastConnected";
    const wchar_t c_szwMethod__AvPrt_ApplicationStateChanged[] = L"KLNAG_MTH__ApplicationStateChanged";
    const wchar_t c_szwMethod__AvPrt_ApplicationBasesChanged[] = L"KLNAG_MTH__ApplicationBasesChanged";
    

    //! klnagent installation id (in product settings)
    #define KLNAG_INSTALLATION_ID   L"KLNAG_INSTALLATION_ID" // STRING_T
    
    /*! sets autokey info: 
            KLNAG_AUTOKEYINFO  ARRAY_T|STRING_T  array of serials
    */
    #define KLNAG_TASK_SET_AUTOKEYINFO       L"KLNAG_TASK_SET_AUTOKEYINFO" // 

        //section in nagent private storage
        #define KLNAG_AUTOKEYINFO_SECTION       L"KLNAG_AUTOKEYINFO_SECTION"

        // ARRAY_T|STRING_T
        #define KLNAG_AUTOKEYS_ALLOWED          L"KLNAG_AUTOKEYS_ALLOWED"

        // ARRAY_T|STRING_T
        #define KLNAG_AUTOKEYS_DISALLOWED       L"KLNAG_AUTOKEYS_DISALLOWED"
    
        //event
        #define KLNAG_EVENT_AUTOKEYINFO_CHANGED L"KLNAG_EVENT_AUTOKEYINFO_CHANGED"
};

// <-- OBSOLETE
#define KLNAG_DEFOPT_SERVER_PING_TIMEOUT	     KLNAG_SP_SERVER_PING_TIMEOUT_DEFAULT
// --> OBSOLETE

#endif //__KL_NAGENT_CONST_H__

// Local Variables:
// mode: C++
// End:


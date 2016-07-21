/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file conn/connector_install.h
 * \author Андрей Казачков
 * \date 2004
 * \brief Constants for connector installation
 *
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef __KL_CONNECTOR_INSTALL_H__
#define __KL_CONNECTOR_INSTALL_H__

#ifdef _WIN32
    #include <tchar.h>
    #define CONN_T  _T
#else
    #define CONN_T(x)   CONN_T_(x)
    #define CONN_T_(x)  L ## x
#endif

#define KLNAG_VER_MP3           0x00053000
#define KLNAG_VER_5_MP3         KLNAG_VER_MP3
#define KLNAG_VER_6             0x00060000
#define KLNAG_VER_MP2           0x00062000

/*
    Connector insatallation parameters has following structure:
    
Windows

    HKEY_LOCAL_MACHINE\KLCONN_REGKEY_INSTALL_ROOT   [KEY]
    +---<ProductName>                               [KEY]
    +-------<ProductShortVersion>                   [KEY]
            +---KLCONN_REGVAL_DISPLAYNAME           [REG_SZ]
            +---KLCONN_REGVAL_PRODVERSION           [REG_SZ]
            +---KLCONN_REGVAL_CONNECTOR_VERSION     [REG_SZ]
            +---KLCONN_REGVAL_CONNECTOR_PATH        [REG_SZ]
            +---KLCONN_REGVAL_NAGENT_MIN_VER        [REG_DWORD]
            +---KLCONN_REGVAL_CONNECTOR_FLAGS       [REG_DWORD]
            +---KLCONN_REGKEY_TASKS                 [KEY]
                +---<task_type_1>                   [KEY]
                        +-----------
                +---<task_type_2>                   [KEY]
                        +-----------
                    ...
                +---<task_type_N>                   [KEY]
                        +-----------
                        
    Product remote uninstallation info
    HKEY_LOCAL_MACHINE\SOFTWARE\KasperskyLab\Components\34  [KEY]
    +---<ProductName>                                       [KEY]
    +-------<ProductShortVersion>                           [KEY]
        +---"Installer"                                     [KEY]
            +---"UninstallString3"                          [REG_SZ]
            +---"UninstallString3KPD"                       [REG_SZ]
            +---"ProductCode"                               [REG_SZ]

  "UninstallString3"    - commandline to start deinstaller
    may constain following substitutions
        %% - character %
        %p - uninstall password
        %i - TASK_ID
        %d - SETUPEXEDIR

  "ProductCode"         - product code
  "UninstallString3KPD" - full file name of kpd-file

Unix and Novell
  Under Unix and Novell each product has its conf file installed into 
  <settings_root>/connectors.d directory. 

    Linux & BSD:
    /etc/opt/kaspersky/klnagent/connectors.d/
  
  Each conf file has .conf extension following structure
    [KLCONN_CONF_SECTION_GLOBAL]
        KLCONN_CONF_PRODUCT = ...
        KLCONN_CONF_VERSION = ...
        KLCONN_REGVAL_DISPLAYNAME = ...
        KLCONN_REGVAL_PRODVERSION = ...
        KLCONN_REGVAL_CONNECTOR_VERSION = ...
        KLCONN_REGVAL_CONNECTOR_PATH = ...
        KLCONN_REGVAL_NAGENT_MIN_VER = ...
        KLCONN_REGVAL_CONNECTOR_FLAGS = ...
        KLCONN_REGKEY_TASKS = "<comma-separated tasks type names list>"
  [<task_type_name_1>]
        <task type options (reserved for future use)>
  ...
  [<task_type_name_N>]
        <task type options (reserved for future use)>

  "tasks type name" cannot contain spaces and slashes
  
  Sample conf 

[global]
    product = "ConnProd"
    version = "1.0.0.0"
    ProdDisplayName = "ConnProd"
    ProdVersion = "1.0.0.0"
    ConnectorVersion = "1.0.0.0"
    ConnectorPath = "libconn_prod.so"
    NagentMinVer = 339968
    ConnectorFlags = 497
    ProdTasks = "ConnProd-TaskType1, ConnProd-TaskType2, ConnProd-TaskType3"
*/

//! Connectors root key
#define KLCONN_REGKEY_INSTALL_ROOT  \
            CONN_T("SOFTWARE\\KasperskyLab\\Components\\34\\Connectors")

//! Product display name. Non-empty string not longer than 255 characters. 
#define KLCONN_REGVAL_DISPLAYNAME       CONN_T("ProdDisplayName")

//! Product version as w.x.y.z. Non-empty string not longer than 255 characters. 
#define KLCONN_REGVAL_PRODVERSION       CONN_T("ProdVersion")

//! Root for product task types
#define KLCONN_REGKEY_TASKS             CONN_T("ProdTasks")

//! Connector version as w.x.y.z. Non-empty string not longer than  255 characters. 
#define KLCONN_REGVAL_CONNECTOR_VERSION CONN_T("ConnectorVersion")

//! Conector dll path
#define KLCONN_REGVAL_CONNECTOR_PATH    CONN_T("ConnectorPath")

/*! Network agent minimum version. One of KLNAG_VER_* constants.
*/
#define KLCONN_REGVAL_NAGENT_MIN_VER    CONN_T("NagentMinVer")//REG_DWORD

/*! Connector is enabled. By default TRUE.
*/
#define KLCONN_REGVAL_CONNECTOR_ENABLED CONN_T("Enabled")//REG_DWORD

//! Connector flags ( KLCONN::CIF_* )
#define KLCONN_REGVAL_CONNECTOR_FLAGS   CONN_T("ConnectorFlags")

//! Connector component name (optional)
#define KLCONN_REGVAL_COMPONENT_NAME    CONN_T("ConnectorCompName")

//! Connector instance id (optional)
#define KLCONN_REGVAL_INSTANCE_ID       CONN_T("ConnectorInstId")


/*! Notification about connector installation/uninstallation

    HKEY_LOCAL_MACHINE\KLCONN_REGKEY_CONN_INSTALLED [KEY]
    +---KLCONN_REGVAL_CONN_INSTALLED                [REG_DWORD]

    This value should be set by connector installer to notify 
    network agent about connector installation/uninstallation.
    The variable must have any nonzero value.
    Value should be set only if key 
    HKEY_LOCAL_MACHINE\KLCONN_REGKEY_CONN_INSTALLED
    exists !!! Installer shouldn't create this key.
*/

#define KLCONN_REGKEY_CONN_INSTALLED    \
            CONN_T("SOFTWARE\\KasperskyLab\\Components\\34\\1103\\1.0.0.0")
#define KLCONN_REGVAL_CONN_INSTALLED    CONN_T("ConnInstalled")

//! Connectors directory (unix, novell)
#define KLCONN_DIR_CONNECTORS           CONN_T("connectors.d")

/*KLCSAK_BEGIN_PRIVATE*/
//! Connectors conf files mask (unix, novell)
#define KLCONN_DIR_CONNECTORS_MASK      CONN_T("*.conf")
/*KLCSAK_END_PRIVATE*/

//! Product settings compatibility version name (unix, novell)
#define KLCONN_CONF_SECTION_GLOBAL      CONN_T("global")

/*! Product name (unix, novell). Cannot be an empty string, 
    cannot be longer than 31 characters, and cannot contain 
    characters /\:*?"<>. This value is intended to be used 
    internally by Administration Kit. It isn't shown to user.
*/
#define KLCONN_CONF_PRODUCT             CONN_T("product")

/*! Product settings compatibility version name (unix, novell)
    Cannot be an empty string, cannot be longer than 31 characters, 
    and cannot contain characters /\:*?"<>. This value is intended 
    to be used internally by Administration Kit. It isn't shown to user.
*/
#define KLCONN_CONF_VERSION             CONN_T("version")

//! OBSOLETE. Use KLCONN_REGVAL_CONNECTOR_PATH instead
#define KLCONN_REGVAL_CONNECTOR_DLL     CONN_T("ConnectorDll")

namespace KLCONN
{
    typedef enum
    {        
        CIF_SINGLE_THREADED = 1, //! All connector methods must be called in the context of the same thread
        CIF_SUPPORT_CALLS = 2, //! Connector supports direct calls from gui plugin
        CIF_SUPPORT_AK_POLICIES = 4, //! Connector supports adminkit policies. If this flag is not set policy will simply modify product settings
        CIF_REPLICABLE_INACTIVE = 8, //! Replication may run even if the product is inactive
        CIF_PUT_POLICY = 16,         //! Puts policy to connector even if CIF_SUPPORT_AK_POLICIES is not set
        CIF_OWN_SCHEDULER = 32,      //! Application uses its own scheduler only
        CIF_PROVIDES_RTP = 64,       //! Application provides runtime protection
        CIF_SUPPORTS_NETWORK_LIST = 128, //! Supports NetworkList interface
        CIF_HAS_FIREWALL = 256,      //! Application contains firewall
        CIF_PRIVATE_POLICY_FORMAT = 512, /* ! Application supports interface SettingsModifier and 
                                             modifies its settings and local tasks by itself (recommended)
                                             Otherwise application settings and local tasks will be modified 
                                             by network agent. Supported since version KLNAG_VER_MP2 of network agent. 
                                        */
        CIF_PROVIDES_NOTIFICATIONS_DEFAULTS = 1024, /*! Connector provides defaults for notifications, 
                                                        notification is subscription for event to send to the administration server,
                                                        by default notifications may be set via policy only (using Administration Console Plug-in),
                                                        see AppNotificationDefaults
                                                    */
        CIF_SUPPORTS_AUTOKEYS = 2048 /*! Application supports autokeys feature */
    }CONN_INSTALL_FLAGS;
};
#endif //__KL_CONNECTOR_INSTALL_H__

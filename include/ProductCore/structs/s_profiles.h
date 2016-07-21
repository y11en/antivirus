#ifndef __S_PROFILES_H
#define __S_PROFILES_H

//-----------------------------------------------------------------------------
// Profiles ID's

#define AVP_PROFILE_PROTECTION              "Protection"
#define AVP_PROFILE_PROTECTION_L           L"Protection"
#define AVP_PROFILE_PRODUCT                 AVP_PROFILE_PROTECTION

#define AVP_PROFILE_AVMONITOR				"Antivirus"
#define AVP_PROFILE_FILEMONITOR             "File_Monitoring"
#define AVP_PROFILE_MAILMONITOR             "Mail_Monitoring"
#define AVP_PROFILE_WEBMONITOR              "Web_Monitoring"
#define AVP_PROFILE_SCRIPTCHECKER           "sc"
#define AVP_PROFILE_HTTPSCAN                "httpscan"
#define AVP_PROFILE_BEHAVIORMONITOR         "Behavior_Blocking"
#define AVP_PROFILE_BEHAVIORMONITOR2        "Behavior_Blocking2"
#define AVP_PROFILE_PDM                     "pdm"
#define AVP_PROFILE_PDM2                    "pdm2"
#define AVP_PROFILE_REGGUARD2               "regguard2"
#define AVP_PROFILE_AIC2                    "aic2"
#define AVP_PROFILE_OFFICEGUARD             "og"
#define AVP_PROFILE_ANTIHACKER              "Anti_Hacker"
#define AVP_PROFILE_ANTISPAM                "Anti_Spam"
#define AVP_PROFILE_FIREWALL                "fw"
#define AVP_PROFILE_INTRUSIONDETECTOR       "ids"
#define AVP_PROFILE_STEALTHMODE             "stm"
#define AVP_PROFILE_SCANOBJECTS             "Scan_Objects"
#define AVP_PROFILE_SCANQUARANTINE          "Scan_Quarantine"
#define AVP_PROFILE_SCAN_STARTUP            "Scan_Startup"
#define AVP_PROFILE_SCAN_VULNERABILITIES    "Scan_Vulnerabilities"
#define AVP_PROFILE_ACTIVE_DISINFECT        "Active_Disinfect"
#define AVP_PROFILE_FULLSCAN                "Scan_My_Computer"
#define AVP_PROFILE_UPDATER                 "Updater"
#define AVP_PROFILE_TRAFFICMONITOR          "TrafficMonitor"
#define AVP_PROFILE_MAIL_WASHER             "MailWasher"
#define AVP_PROFILE_ANTISPY                 "Anti_Spy"
#define AVP_PROFILE_POPUPBLOCKER            "popupchk"
#define AVP_PROFILE_ANTIDIAL                "antidial"
#define AVP_PROFILE_ANTIBANNER              "AdBlocker"
#define AVP_PROFILE_ANTIPHISHING            "antiphishing"
#define AVP_PROFILE_LICENCE                 "LicMgr"
#define AVP_PROFILE_ROLLBACK                "Rollback"
#define AVP_PROFILE_RETRANSLATION           "Retranslation"
#define AVP_PROFILE_RETRANSLATIONCFG        "RetranslationCfg"
#define AVP_PROFILE_SELFPROT                "selfprot"
#define AVP_PROFILE_PRIVACYCONTROL          "PrivacyControl"
#define AVP_PROFILE_PARENTALCONTROL         "ParCtl"
#define AVP_PROFILE_WEBMON_URLFLT           "WMUF"
#define AVP_PROFILE_CONTENT_FILTER          "ContentFilter"
#define AVP_PROFILE_ONLINE_SECURITY         "OnlineSecurity"
#define AVP_PROFILE_HIPS                    "Hips"
#define AVP_PROFILE_STATISTICS              "Statistics"
#define AVP_PROFILE_SYSTEM_WATCHER          "SysWatch"
#define AVP_PROFILE_NETWORK_WATCHER         "NetWatch"
#define AVP_PROFILE_AVZ_SCRIPT              "AVZ_RunScript"

//-----------------------------------------------------------------------------
// Service ID's

#define AVP_SERVICE_AVS                     "AVService"
#define AVP_SERVICE_STARTUP                 "StartupService"
#define AVP_SERVICE_SCHEDULER               "scheduler"
#define AVP_SERVICE_BACKUP                  "backup"
#define AVP_SERVICE_QUARANTINE              "quarantine"
#define AVP_SERVICE_LICENCE                 "licence"
#define AVP_SERVICE_ANTIPHISHING            "AntiPhishingService"
#define AVP_SERVICE_TRAFFICMONITOR          "TrafficMonitor"
#define AVP_SERVICE_TRAFFICMONITOR_HTTP     "HTTP"
#define AVP_SERVICE_TRAFFICMONITOR_SMTP     "SMTP"
#define AVP_SERVICE_TRAFFICMONITOR_POP3     "POP3"
#define AVP_SERVICE_TRAFFICMONITOR_IMAP     "IMAP"
#define AVP_SERVICE_TRAFFICMONITOR_NNTP     "NNTP"
#define AVP_SERVICE_TRAFFICMONITOR_MWASHER  "MailWasher"
#define AVP_SERVICE_SPAMTEST                "Spamtest"
#define AVP_SERVICE_PROCESS_MONITOR         "ProcMon"
#define AVP_SERVICE_ADVANCED_DISINFECTION   "AdvDis"

#define sid_TM_TRAFFIC_MON                  AVP_SERVICE_TRAFFICMONITOR  // = 5,
#define sid_TM_STARTUP_SVC                  AVP_SERVICE_STARTUP         // = 7,
#define sid_TM_ANTIPHISH_SVC                AVP_SERVICE_ANTIPHISHING    // = 8,

//-----------------------------------------------------------------------------
// Tasks ID's

#define AVP_TASK_BEHAVIORBLOCKER            "bb"
#define AVP_TASK_BEHAVIORBLOCKER2           "bb2"
#define AVP_TASK_ANTIHACKER                 "antihacker"
#define AVP_TASK_ANTISPAM                   "antispam"
#define AVP_TASK_ANTIDIAL                   AVP_PROFILE_ANTIDIAL
#define AVP_TASK_ANTIPHISHING               "antiphishing"
#define AVP_TASK_ANTIBANNER                 "adblocker"
#define AVP_TASK_PARENTALCONTROL            "parctl"
#define AVP_TASK_POPUPBLOCKER               "popupchk"
#define AVP_TASK_INTRUSIONDETECTOR          "ids"
#define AVP_TASK_FIREWALL                   "fw"
#define AVP_TASK_MAILCHECKER                "mc"
#define AVP_TASK_TRAFFICMONITOR             "mc_tm"
#define AVP_TASK_OUTLOOKPLUGIN              "mcop"
#define AVP_TASK_ONACCESSSCANNER            "oas"
#define AVP_TASK_ONDEMANDSCANNER            "ods"
#define AVP_TASK_QUARANTINESCANNER          "qs"
#define AVP_TASK_SCRIPTCHECKER              "sc"
#define AVP_TASK_UPDATER                    "updater"
#define AVP_TASK_OFFICEGUARD                "og"
#define AVP_TASK_PDM                        "pdm"
#define AVP_TASK_PDM2                       "pdm2"
#define AVP_TASK_REGGUARD2                  "regguard2"
#define AVP_TASK_GUI                        "gui"
#define AVP_TASK_LICENCE                    "InstallKey"
#define AVP_TASK_SELFPROT                   AVP_PROFILE_SELFPROT
#define AVP_TASK_ROLLBACK                   "rollback"
#define AVP_TASK_RETRANSLATION              "retran"
#define AVP_TASK_WEBMONITOR                 "wm"
#define AVP_TASK_WEBMON_URLFLT              "wmuf"
#define AVP_TASK_PRIVACYCONTROL				"privctrl"
#define AVP_TASK_ANTISPY					"antispy"
#define AVP_TASK_HIPS						"hips"
#define AVP_TASK_SYSTEM_WATCHER             AVP_PROFILE_SYSTEM_WATCHER



//-----------------------------------------------------------------------------
// Profiles ID's

#define AVP_PRODUCT_KIS                     "kis"
#define AVP_PRODUCT_KAV                     "kav"
#define AVP_PRODUCT_WKS                     "wks"
#define AVP_PRODUCT_FS                      "fs"
#define AVP_PRODUCT_AVPTOOL                 "kat"

#endif//  __S_PROFILES_H

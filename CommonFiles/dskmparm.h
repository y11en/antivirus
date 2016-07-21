//////////////////////////////////////////////////////////////////////////
//
// dskmparm.h
// 
// Файл dskmparm.h предназначен для использования компонентами
// системы DSKM для проверки подписи. 
// Генерируется из базы GlobalBase.
//

#ifndef __dskmparm_h__
#define __dskmparm_h__

//////////////////////////////////////////////////////////////////////////
//
// Идентификаторы параметров фильтрации ключа:

#define DSKM_KEYFILTER_PARAM_APPID_ID     0x0001
#define DSKM_KEYFILTER_PARAM_COMPID_ID    0x0002
#define DSKM_KEYFILTER_PARAM_TYPE_ID      0x0003

//////////////////////////////////////////////////////////////////////////
//
// Значения параметров фильтрации ключа:

// APPID'S

#define APPID_KASPERSKY_INTERNET_SECURITY_2006                          "KIS6"
#define APPID_KASPERSKY_ANTI_VIRUS_2006                                 "KAV6"
#define APPID_KASPERSKY_FILE_SERVER_ENTERPRISE_EDITION                  "FSEE"
          
// COMPID'S                                                              
#define COMPID_KASPERSKY_ANTI_HACKER                                    "AH"
#define COMPID_KASPERSKY_ANTI_VIRUS_CORE_COMPONENTS                     "Core"
#define COMPID_KASPERSKY_ANTI_VIRUS_MAIN_MODULE                         "Main"
#define COMPID_KASPERSKY_ANTI_SPAM_LICENSE_PER_E_MAIL_ADDRESS           "AS"
#define COMPID_KASPERSKY_ANTI_SPAM_TRAFFIC_LIMITED                      "AS"
#define COMPID_ANTI_VIRUS_SERVER                                        "AVS"
#define COMPID_ANTI_FISHING                                             "AP"
#define COMPID_ADS_BLOCKER                                              "ADB"
#define COMPID_REGISTRY_MONITOR                                         "RM"

// TYPE'S
#define TYPEID_BINARY                                                   "BIN"
#define TYPEID_BASE                                                     "BASE"

#endif	

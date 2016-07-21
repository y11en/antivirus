#ifndef H_UPDATER_SETTINGS_KEYWORDS_57C67F61_8922_482e_B374_6D611B4A0084
#define H_UPDATER_SETTINGS_KEYWORDS_57C67F61_8922_482e_B374_6D611B4A0084

#define SS_KEY_BlackListMark        L".blacklist"
#define SS_KEY_UseDefaultRegion    L"<default>"

// for change default primary index name functionality
#define SS_KEY_primaryIndexFileName  L"primaryIndexFileName"
#define SS_KEY_primaryIndexRelativeUrlPath L"primaryIndexRelativeUrlPath"
#define SS_KEY_primaryIndexLocalPath L"primaryIndexLocalPath"

#define SS_KEY_productIdentifier           L"ProdName"
#define SS_KEY_productVersion            L"ProdVersion"

#define SS_KEY_componentsToUpdate  L"components"
#define SS_KEY_optionalComponents    L"optionalComponents"


#define SS_KEY_inet_update         L"inet_update"
#define SS_KEY_try_inet               L"try_inet"
#define SS_KEY_asrv_update        L"asrv_update"
#define SS_KEY_master_asrv_update L"master_asrv_update"
#define SS_KEY_retranslation_dir   L"retranslation_dir"
#define SS_KEY_temporary_dir      L"temporary_dir"
#define SS_KEY_self_region          L"self_region"
#define SS_KEY_proxy                  L"proxy"
#define SS_KEY_bypassProxyServerForLocalAddresses L"bypassProxyServerForLocalAddresses"
#define SS_KEY_use_ie_proxy       L"use_ie_proxy"
#define SS_KEY_proxy_url             L"proxy_url"
#define SS_KEY_proxy_port           L"proxy_port"
#define SS_KEY_proxy_authorisation  L"proxy_authorisation"
#define SS_KEY_proxy_ntlm_access_token    L"proxy_ntlm_access_token"
#define SS_KEY_proxy_login         L"proxy_login"
#define SS_KEY_proxy_pwd          L"proxy_pwd"
#define SS_KEY_passive_ftp         L"passive_ftp"
#define SS_KEY_tryActiveFtpIfPassiveFails    L"tryActiveFtpIfPassiveFails"
#define SS_KEY_displayAllEventsInHistory     L"displayAllEventsInHistory"


// the variable is used for substitution to build path
//   files by default goes to "DataFolder" path
#define SS_KEY_UpdaterDataFolder   L"DataFolder"
// the variable is used for substitution to build path
//   XML tag 'LocalPath' in update index uses this variable to specify location to place bases
#define SS_KEY_BaseFolder             L"BaseFolder"
// the variable is used for substitution to build path
#define SS_KEY_UpdaterRootFolder   L"UpdateRoot"
// used in Ini-Updater to determine folder with rollback
#define SS_KEY_productFolder          L"Folder"

// the variable defines the name of the section in product settings storage
//  for full path including file name where KCA Updater writes update results
#define SS_KEY_KCAResultListFile   L"ListsFile"


// filter settings for update
#define SS_KEY_update_bases      L"update_bases"
#define SS_KEY_apply_urgent_updates  L"apply_urgent_updates"
#define SS_KEY_automatically_apply_available_updates L"automatically_apply_available_updates"

// filter settings for retranslation
#define SS_KEY_updates_retranslation   L"updates_retranslation"
#define SS_KEY_retranslate_only_listed_apps L"retranslate_only_listed_apps"
#define SS_KEY_retranslate_bases    L"retranslate_bases"
#define SS_KEY_retranslate_urgent_updates   L"retranslate_urgent_updates"
#define SS_KEY_retranslate_available_updates   L"retranslate_available_updates"



// Recent files list keywords                              
#define SS_KEY_RecentFileName     L"FileName"
#define SS_KEY_RecentLocalPath     L"LocalPath"
#define SS_KEY_RecentRelativeSrvPath L"RelativeSrvPath"
#define SS_KEY_RecentFileSize        L"Size"
#define SS_KEY_RecentFileDate        L"Date"
#define SS_KEY_RecentFileComment L"Comment"
#define SS_KEY_RecentType             L"Type"
#define SS_KEY_RecentApp              L"Application"
#define SS_KEY_RecentComponentList  L"ComponentList"
#define SS_KEY_RecentFromAppVer  L"FromAppVersion"
#define SS_KEY_RecentToAppVer     L"ToAppVersion"
#define SS_KEY_RecentLocal            L"Localization"
#define SS_KEY_RecentLang             L"LocLang"
#define SS_KEY_RecentUpdateObligation   L"UpdateObligation"

// localization set to show localized component set
#define SS_KEY_LocalizationSet6      L"LocalizationSet6"
// localization set contains difference for AdminKit to show localized component set
#define SS_KEY_LocalizationSetAK   L"LocalizationSetAK"
// the element from update set with 'default' value in for localization
//   set should be used by default in case nothing else suits
#define SS_VALUE_LocalizationSetDefaultAK     L"default"


// Recent changed files status values
#define SS_KEY_RecentStatusAdded    L"Added"
#define SS_KEY_RecentStatusModified L"Modified"
#define SS_KEY_PostponedUnchanged   L"Unchanged"

// component and application filter list
#define SS_KEY_RetranslationListSection L"RetranslationList"
#define SS_KEY_ApplicationsSection     L"Applications"
#define SS_KEY_ComponentsSection     L"Components"

// changed files section keywords
#define SS_KEY_ChangeStatus     L"ChangeStatus"
#define SS_KEY_Code                  L"Code"
#define SS_KEY_Base                  L"Base"
#define SS_KEY_Patch                 L"Patch"
#define SS_KEY_Desc                  L"Desc"
#define SS_KEY_Blst                    L"Blst"


// source description constants
#define SS_KEY_UpdateSourceList    L"UpdateSourceList"
#define SS_KEY_SourceType            L"type"
#define SS_KEY_SourceURL             L"url"
#define SS_KEY_SourceUseProxy     L"use_proxy"


// sections in rslt_lists.h for KCA Updater
#define SS_KEY_RecentChangedFiles    L"RecentChangedFiles"
#define SS_KEY_RetranslateTree           L"RecentRetranlatedFiles"
#define SS_KEY_RollbackTree               L"AvailableForRollback"


// Types of generic variables stored in SS                                      
#define SS_KEY_keyw_type_STRING_T     L"wchar"
#define SS_KEY_keyw_type_BOOL_T        L"bool"
#define SS_KEY_keyw_type_INT_T 	        L"int"
#define SS_KEY_keyw_type_LONG_T        L"long"
#define SS_KEY_keyw_type_DATE_TIME_T L"date"


#define SS_KEY_connect_tmo       L"connect_tmo"
#define SS_KEY_update_srv_url     "update_srv_url"
#define SS_KEY_crypto_lib            L"crypto_lib"


// name of key which contains application identifier value which is going to be sent to Update Source
#define PAR_APPLICATION_ID  L"ApplicationID"
// build number parameter name in settings storage get value for identification on Update Sources
#define PAR_BuildIdentifier         L"buildNumberIdentifier"
// name of key which contains installation identifier value which is going to be sent to Update Source
#define PAR_INSTALLATION_ID L"InstallationID"



// DEPRECATE: constants are used in Administration Server
//  TODO: move to Administration Kit project
#define SS_KEY_RetranslationApplications SS_KEY_ApplicationsSection
#define SS_KEY_RetranslationComponents SS_KEY_ComponentsSection
#define SS_KEY_UpdateTree         L"RecentChangedFiles"


#endif  // #ifndef H_UPDATER_SETTINGS_KEYWORDS_57C67F61_8922_482e_B374_6D611B4A0084

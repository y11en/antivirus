#ifndef KLUPD__UPDATERCOMPCONSTANTS_H__
#define KLUPD__UPDATERCOMPCONSTANTS_H__

#include <updater/keepup2date/client/helper/updater_settings_keywords.h>
#include <updater/keepup2date/client/helper/kavset_xml.h>

#ifdef WIN32
    #define LL0(A)  L##A
    #define LL(A)   LL0(A)
#endif

#define KLUPD_inet_update								LL(SS_KEY_inet_update)	// BOOL
#define KLUPD_proxy										LL(SS_KEY_proxy)	// BOOL
#define KLUPD_use_ie_proxy								LL(SS_KEY_use_ie_proxy)	// BOOL
#define KLUPD_proxy_url									LL(SS_KEY_proxy_url)	// STR
#define KLUPD_proxy_port								LL(SS_KEY_proxy_port)	// INT
#define KLUPD_proxy_authorisation						LL(SS_KEY_proxy_authorisation)	// BOOL
#define KLUPD_proxy_login								LL(SS_KEY_proxy_login)	// STR
#define KLUPD_proxy_pwd									LL(SS_KEY_proxy_pwd)	// STR
#define KLUPD_passive_ftp								LL(SS_KEY_passive_ftp)	// BOOL
#define KLUPD_update_srv_url							LL(SS_KEY_update_srv_url)	// STR
#define KLUPD_updates_retranslation						LL(SS_KEY_updates_retranslation)	// BOOL
#define KLUPD_retranslation_dir							LL(SS_KEY_retranslation_dir)	// STR
#define KLUPD_connect_tmo								LL(SS_KEY_connect_tmo)	// INT
#define KLUPD_try_inet									LL(SS_KEY_try_inet)	// BOOL
#define KLUPD_update_bases								LL(SS_KEY_update_bases)	// BOOL
#define KLUPD_apply_urgent_updates						LL(SS_KEY_apply_urgent_updates)	// BOOL
//#define KLUPD_check_for_available_updates				LL(SS_KEY_check_for_available_updates)	// BOOL
#define KLUPD_automatically_apply_available_updates		LL(SS_KEY_automatically_apply_available_updates)	// BOOL
#define KLUPD_retranslate_bases							LL(SS_KEY_retranslate_bases)	// BOOL
#define KLUPD_retranslate_urgent_updates				LL(SS_KEY_retranslate_urgent_updates)	// BOOL
#define KLUPD_retranslate_available_updates				LL(SS_KEY_retranslate_available_updates)	// BOOL
#define KLUPD_patching_allowed							LL(SS_KEY_patching_allowed)	// BOOL
#define KLUPD_proxy_ntlm_access_token					LL(SS_KEY_proxy_ntlm_access_token)	// BOOL
#define KLUPD_asrv_update								LL(SS_KEY_asrv_update) // BOOL

#define KLUPD_retranslate_only_listed_apps				LL(SS_KEY_retranslate_only_listed_apps)
#define KLUPD_displayAllEventsInHistory					LL(SS_KEY_displayAllEventsInHistory)
#define KLUPD_RetranslationComponents					LL(SS_KEY_RetranslationComponents)
#define KLUPD_RetranslationApplications					LL(SS_KEY_RetranslationApplications)
#define KLUPD_UpdateSourceList							LL(SS_KEY_UpdateSourceList)
#define KLUPD_BaseFolder								LL(SS_KEY_BaseFolder)
#define KLUPD_UpdaterDataFolder 						LL(SS_KEY_UpdaterDataFolder)
#define KLUPD_RecentLocal 								LL(SS_KEY_RecentLocal)
#define KLUPD_DSKM 										L"DSLM"
#define KLUPD_Data 										L"Data"

#define KLUPD_Desc										LL(SS_KEY_Desc)
#define KLUPD_Base										LL(SS_KEY_Base)
#define KLUPD_Patch										LL(SS_KEY_Patch)
#define KLUPD_Blst										LL(SS_KEY_Blst)
#define KLUPD_SourceType								LL(SS_KEY_SourceType)
#define KLUPD_SourceURL									LL(SS_KEY_SourceURL)
#define KLUPD_SourceUseProxy							LL(SS_KEY_SourceUseProxy)
#define KLUPD_RecentFileName							LL(SS_KEY_RecentFileName)
#define KLUPD_RecentApp									LL(SS_KEY_RecentApp)
#define KLUPD_RecentAppList								L"AppList"
#define KLUPD_RecentComponentList						LL(SS_KEY_RecentComponentList)
#define KLUPD_RecentToAppVer							LL(SS_KEY_RecentToAppVer)
#define KLUPD_RecentFileComment							LL(SS_KEY_RecentFileComment)
#define KLUPD_ChangeStatus								LL(SS_KEY_ChangeStatus)
#define KLUPD_RecentFileSize							LL(SS_KEY_RecentFileSize)
#define KLUPD_RecentType 								LL(SS_KEY_RecentType)
#define KLUPD_RecentFileDate 							LL(SS_KEY_RecentFileDate)
#define KLUPD_RecentFromAppVer 							LL(SS_KEY_RecentFromAppVer)
#define KLUPD_RetranslateTree							LL(SS_KEY_RetranslateTree)
#define KLUPD_UseDefaultRegion							LL(SS_KEY_UseDefaultRegion)
#define KLUPD_self_region								LL(SS_KEY_self_region)
#define KLUPD_ComponentsSection							LL(SS_KEY_ComponentsSection)
#define KLUPD_ApplicationsSection						LL(SS_KEY_ApplicationsSection)
#define KLUPD_RetranslationListSection					LL(SS_KEY_RetranslationListSection)
#define KLUPD_KCAResultListFile							LL(SS_KEY_KCAResultListFile)
#define KLUPD_UpdaterRootFolder							LL(SS_KEY_UpdaterRootFolder)
#define KLUPD_RecentRelativeSrvPath						LL(SS_KEY_RecentRelativeSrvPath)
#define KLUPD_RecentLocalPath							LL(SS_KEY_RecentLocalPath)
#define KLUPD_UpdateTree								LL(SS_KEY_UpdateTree)
#define KLUPD_RecentUpdateObligation					LL(SS_KEY_RecentUpdateObligation)
#define KLUPD_BlackListMark								LL(SS_KEY_BlackListMark)
#define KLUPD_master_asrv_update						LL(SS_KEY_master_asrv_update)
#define KLUPD_LocalizationSet6							LL(SS_KEY_LocalizationSet6)
#define KLUPD_LocalizationSetAK							LL(SS_KEY_LocalizationSetAK)
#define KLUPD_LocalizationSetDefaultAK					LL(SS_VALUE_LocalizationSetDefaultAK)

#define KLUPD_keyw_AVRecords							LL(keyw_AVRecords)
#define KLUPD_ComponentidAVS							L"AVS"
#define KLUPD_ComponentidAH								L"AH"
#define KLUPD_ComponentidKASBASES						L"KASBASES"
// Updater Component: updater configuration files
#define KLUPD_ComponentidUpdater						L"Updater"
// the signature 6 registry elements
#define KLUPD_ComponentidCORE							L"CORE"
// component names localization
#define KLUPD_ComponentidLOCALIZATION					L"RT"

#define KLUPD_ComponentidINDEX50						L"INDEX50"
#define KLUPD_ComponentidINDEX60						L"INDEX60"

namespace KLUPD
{
    // required for KCA libraries (TODO: move declaration to KCA)
    const wchar_t c_szwComponentName[] = L"Updater";

    // Имя задачи обновления
    const wchar_t c_szwUpdateTaskName[] = L"DoUpdate";
    // Имя задачи отката
    const wchar_t c_szwRollbackTaskName[] = L"DoRollback";
    
    // Имя параметра с результатом ("TaskResultInt":int)
    const wchar_t c_szwTaskResultInt[] = L"TaskResultInt";

    // update event, localized by Updater localization library
    // ("UPD_Error", "Params")
    const wchar_t c_szwEvent[] = L"UPD_Code";

	// Obsolete result codes:
	//const CORE_NET_RESOURCE_LOCKED = 7;
	//const CORE_NET_INTERNAL_ERROR = 8;

	// Moved from Components\Updater\KeepUp2Date\Client\helper\localize.h:
	///////////////////////////////////////////////
	// BEGIN deprecate codes for Administration Kit
	//  NEVER USE THIS CODES
    const int CORE_NET_RESOURCE_LOCKED = 7;
    const int CORE_NET_INTERNAL_ERROR = 8;
    const int EVENT_UPDATE_STARTED = 109;
    const int EVENT_REPLACING_FILES = 115;
    #define UPD_IDS_ERR_UPDATE_CANCELLED 136
    #define UPD_IDS_ERR_WRONG_FILE_SIGNATUTE 141
    #define UPD_IDS_ERR_FAILED_TO_CREATE_DIR 109
    #define UPD_IDS_ERR_PROXY_AUTH_FAILED 155
    #define UPD_IDS_ERR_WRITE_TO_RESERVE_DIR_FAILED 140
    #define UPD_IDS_ERR_TIMED_OUT 146
    #define UPD_IDS_ERR_DOWNLOAD_ERROR 103
    #define UPD_IDS_ERR_WRITE_TO_PRODUCT_DIR_FAILED 139
	// END deprecate codes for Administration Kit
	
}; // KLUPD

namespace KLUPDSRV
{
    const wchar_t c_spUpdate_Bundles[]=L"KLUPDSRV_BUNDLES";//ARRAY_T
    const wchar_t c_spUpdate_BundleId[]=L"KLUPDSRV_BUNDLE_ID";//STRING_T
    const wchar_t c_spUpdate_BundleName[]=KLUPD_RecentFileName;//STRING_T
    const wchar_t c_spUpdate_BundleApp[]=KLUPD_RecentApp;//STRING_T
    const wchar_t c_spUpdate_BundleComponent[]=L"Component";//STRING_T
    const wchar_t c_spUpdate_BundleComponentList[]=KLUPD_RecentComponentList;//STRING_T
    const wchar_t c_spUpdate_BundleToVersion[]=KLUPD_RecentToAppVer;//STRING_T
    const wchar_t c_spUpdate_BundleComment[]=KLUPD_RecentFileComment;//STRING_T
    const wchar_t c_spUpdate_BundleStatus[]= KLUPD_ChangeStatus;//STRING_T
    const wchar_t c_spUpdate_BundleSize[]=KLUPD_RecentFileSize;//LONG_T!!!!!!!!!!!!!!!!!!!!!!!
	const wchar_t c_spUpdate_BundleRecordsCount[]=L"KLUPD_AV_RECORDS_COUNT";
    const wchar_t c_spUpdate_BundleType[]=KLUPD_RecentType;//STRING_T
    const wchar_t c_spUpdate_BundleDate[]=KLUPD_RecentFileDate;//DATE_TIME_T
    const wchar_t c_spUpdate_BundleDownloadDate[]=L"KLUPDSRV_BUNDLE_DWL_DATE";//DATE_TIME_T
    //const wchar_t c_spUpdate_BundlePostponedToLoad[]=KLUPD_PostponedToLoad);//BOOL_T
    const wchar_t c_spUpdate_BundleFromVersions[]=KLUPD_RecentFromAppVer;//PARAMS_T - содержит пары version / fake bool value
    const wchar_t c_spUpdate_BundleAutoInstall[]=L"KLUPD_AUTO_INSTALL";//BOOL_T
    const wchar_t c_spUpdate_BundleDependencies[]=L"KLUPDSRV_BUNDLE_DEPENDENCIES";//ARRAY_T - содержит bundle id пакетов, от которых зависит данный пакет - см. описание GetUpdatesInfo
    const wchar_t c_spUpdate_BundleTypeDescr[]=L"KLUPDSRV_BUNDLE_TYPE_DESCR";//STRING_T - содержит название типа обновления.

    // internal params names
    //const wchar_t c_spUpdate_AutoInstallAVDB[]=L"KLUPD_AUTO_INSTALL_AVDB";//BOOL_T
    //const wchar_t c_spUpdate_AutoInstallPatches[]=L"KLUPD_AUTO_INSTALL_PATCHES";//BOOL_T
    //const wchar_t c_spUpdate_AutoInstallUU[]=L"KLUPD_AUTO_INSTALL_UU";//BOOL_T

    // old stuff
    const wchar_t c_spUpdate_Packages[]=L"KLUPDSRV_PACKAGES";//ARRAY_T
    const wchar_t c_spUpdate_PackageNname[]=L"KLUPDSRV_NAME";
    const wchar_t c_spUpdate_Version[]=L"KLUPDSRV_VERSION";
    const wchar_t c_spUpdate_PackageSize[]=L"KLUPDSRV_SIZE";    
    const wchar_t c_spUpdate_PackageDate[]=L"KLUPDSRV_DATE";
    const wchar_t c_spUpdate_IsAvailableOffline[]=L"KLUPDSRV_AVAIL_OFFLINE";


    const wchar_t c_spRetranslationListData[]=L"RetranslationListData";

    const wchar_t c_spUpdate_LastRetranslationTime[]=L"KLUPDSRV_LAST_RETRANSLATION_TIME";//DATE_TIME_T

	const wchar_t c_spUpdate_retranslation_network_path[] = L"retranslation_network_path"; // STRING_T

    const wchar_t c_spRetranslateForInstalledProds[] = L"KLUPDSRV_RetrForInstalledProds"; // BOOL_T
	const bool c_bRetranslateForInstalledProdsDefault = false;
    const wchar_t c_spRetranslateForInstalledProdsOnSlaves[] = L"KLUPDSRV_RetrForInstalledProdsOnSlaves"; // BOOL_T
	const bool c_bRetranslateForInstalledProdsOnSlavesDefault = true;

	const wchar_t c_spUpdate_UseReserveFolder[] = L"KLUPDSRV_UseReserveFolder"; // BOOL_T	

	const wchar_t c_spUpdate_folder_for_checked_updates[] = L"folder_for_checked_updates"; // STRING_T	

	const wchar_t c_spUpdate_checkbases_taskid[] = L"checkbases_taskid"; // LONG_T	



	#define KLUPD_WKS_6_PRODUCT_NAME	TEXT("1190")
	#define KLUPD_FS_6_PRODUCT_NAME		TEXT("1191")
	#define KLUPD_6_PRODUCT_NAME		TEXT("KAV6")

	#define KLUPD_RETR_UPDATES_LIST		TEXT("KLUpdates") // PARAMS_T
	#define KLUPD_RETR_COMPONENT_ID		TEXT("CompId")	// STRING_T
	#define KLUPD_RETR_APP_ID			TEXT("AppId")	// STRING_T
	#define KLUPD_RETR_UPDATE_NAME		TEXT("Name")	// STRING_T
	#define KLUPD_RETR_UPDATE_DESCR		TEXT("Descr")	// STRING_T
	#define KLUPD_RETR_UPDATE_HIDDEN	TEXT("Hidden")	// BOOL_T
	#define KLUPD_RETR_APP_SUPER_ID		TEXT("AppSuperId")	// STRING_T

	const wchar_t c_ComponentKAS20EXCH[] = L"KAS20EXCH";
};

#endif // KLUPD__UPDATERCOMPCONSTANTS_H__

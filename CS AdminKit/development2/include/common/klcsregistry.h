#ifndef _KLCS_REGISTRY_H
#define _KLCS_REGISTRY_H

#ifdef _WIN32

#include <avpregid.h>

#define REG_KEY_ADMIN_KIT							AVP_REGKEY_COMPONENTS_FULL _SEP_ TEXT("34")

// WKS 5

#define KL_ANTIVIRUS_FOR_WS_PRODUCT_NAME			TEXT("Workstation")
#define KL_ANTIVIRUS_FOR_WS_SCAN_TASK_NAME			TEXT("KLODS_TA_SCAN_TASK")
#define KL_ANTIVIRUS_FOR_WS_UPDATE_TASK_NAME		TEXT("KLUPD_TA_UPDATE_TASK")
#define KL_ANTIVIRUS_FOR_WS_COMPONENT_ID			TEXT("BL")

// WKS & FS 6

#define KL_ANTIVIRUS_FOR_WS6_PRODUCT_NAME			TEXT("KAVWKS6")
#define KL_ANTIVIRUS_FOR_FS6_PRODUCT_NAME			TEXT("KAVFS6")
#define KL_ANTIVIRUS_FOR_FSEE_PRODUCT_NAME			TEXT("KAVFSEE")
#define KL_ANTIVIRUS_FOR_WS6_SCAN_TASK_NAME			TEXT("ods")
#define KL_ANTIVIRUS_FOR_WS6_COMPONENT_ID			TEXT("Connector")
#define KL_ANTIVIRUS_FOR_WS6_UPDATE_TASK_NAME		TEXT("updater")

// Resident tasks
#define KL_ANTIVIRUS_FOR_WS_OAS_MONITOR_TASK_NAME	TEXT("KLOAS_TA_MONITOR_TASK")
#define KL_ANTIVIRUS_FOR_WS_KAH_TA_MONITOR_TASK		TEXT("KLCKAH_TA_MONITOR_TASK")
#define KL_ANTIVIRUS_FOR_WS_MC_TA_MONITOR_TASK		TEXT("KLMC_TA_MONITOR_TASK")
#define KL_ANTIVIRUS_FOR_WS_OG_TA_MONITOR_TASK		TEXT("KLOG_TA_MONITOR_TASK")
#define KL_ANTIVIRUS_FOR_WS_SC_TA_MONITOR_TASK		TEXT("KLSC_TA_MONITOR_TASK")

// Криво определенные имена задач WKS 6
#define KL_ANTIVIRUS_FOR_WS6_LICENSE_TASK_NAME		TEXT("lic")

#define KL_ANTIVIRUS_FOR_FS_PRODUCT_NAME			TEXT("Fileserver")

#define KL_KAV_WKS_6_PRODUCT_NAME					KL_ANTIVIRUS_FOR_WS6_PRODUCT_NAME
#define KL_KAV_FS_6_PRODUCT_NAME					KL_ANTIVIRUS_FOR_FS6_PRODUCT_NAME
#define KL_KSE_PRODUCT_NAME							TEXT("1168")
#define KL_KAVE_PRODUCT_NAME						TEXT("1147")

/////////////////////////////////////////////////////////////////////////////
// Registry format
// __________________________________________________________________________
//
// HKLM/KasperskyLab/Components/<34>
//		Products	<KLCSR_PRODUCTS_SECTION>
//			<ProductID>
//				.default - локализованый имя продукта
//				Configurator	- CLSID объекта - конфигуратора продукта
//
//				Tasks	<KLCSR_TASKS_SECTION>
//					<TaskType>	- тип задачи
//						.default - локализованый тип задачи
//						Configurator	- CLSID объекта - конфигуратора задачи
//
/////////////////////////////////////////////////////////////////////////////

#define KLCSR_SHOW_LOCAL_COMPUTER					TEXT("ShowLocalComputer")	// HKCU : show local computer node

#define KLCSR_FIRST_TIME_INITIALIZATION_COMPLETED	TEXT("FTICompleted")

#define KLCSR_PRODUCTS_SECTION						TEXT("Products")
#define KLCSR_TASKS_SECTION							TEXT("Tasks")
#define KLCSR_POLICY_SECTION						TEXT("Policy")
#define KLCSR_SUBPAGES_SECTION						TEXT("SubPages")
#define KLCSR_SUBPAGE_LINK_VALUE					TEXT("Link")
#define KLCSR_SUBPAGE_ID_VALUE						TEXT("Id")

#define KLCSR_PRODUCT_VERSION_VALUE					TEXT("Version")
#define KLCSR_PRODUCT_DISPLAY_VERSION_VALUE			TEXT("DisplayVersion")
#define KLCSR_PRODUCT_PLUGIN_VERSION_VALUE			TEXT("PluginVersion")
#define KLCSR_CONFIGURATOR_VALUE					TEXT("Configurator")
#define KLCSR_CID_VERSION_VALUE						TEXT("CID_Version")
#define KLCSR_CID_COMPONENTID_VALUE					TEXT("CID_ComponentID")
#define KLCSR_POLICY_CONFIGURATOR_VALUE				TEXT("Configurator")
#define KLCSR_PRODUCT_INSTALLER_CONFIGURATOR_VALUE	TEXT("Configurator")
#define KLCSR_TASK_SCHEDULER_PAGE_TYPES				TEXT("SchedulerPages")
#define KLCSR_SERVER_SIDE_TASK						TEXT("ServerSideTask")	// The task is performed by the server
#define KLCSR_UNIQUE_TASK							TEXT("UniqueTask")		// The task cannot be created or deleted by user
#define KLCSR_LOCAL_TASK							TEXT("LocalTask")		// The task cannot be group or global
#define KLCSR_ONLY_ONE_TASK_INSTANCE_ALLOWED		TEXT("OnlyOneTaskInstanceAllowed") // Only one task can be created

// флаги для управляющих кнопок
typedef enum {
	KLNH_CTRL_BTN_START	= 0x0001,
	KLNH_CTRL_BTN_STOP	= 0x0002,
	KLNH_CTRL_BTN_PAUSE	= 0x0004,
	KLNH_CTRL_BTN_RESUME = 0x0008
};

#define KLCSR_PRODUCT_CTRL_BUTTONS					TEXT("CtrlButtons")			// Optional: default is START/STOP
#define KLCSR_TASK_CTRL_BUTTONS						TEXT("CtrlButtons")			// Optional: default is START/STOP
#define KLCSR_PRODUCT_SUPPORT_POLICIES				TEXT("SupportsPolicies")	// Optional: default is TRUE
#define KLCSR_SUPPORT_DIFFERENT_TASK_ACCOUNTS		TEXT("SupportAccounts")		// Optional: default is TRUE
#define KLCSR_SHOW_ENABLE_TASK_CTRL					TEXT("ShowEnableTaskCtrl")	// Optional: default is TRUE

// Console nodes autorefreshing timeout

#define KLCSR_TASKS_REFRESHING_TIMEOUT				TEXT("TasksRefreshingTimeout")
#define KLCSR_GROUPS_REFRESHING_TIMEOUT				TEXT("GroupsRefreshingTimeout")
#define KLCSR_UNASS_GROUPS_REFRESHING_TIMEOUT		TEXT("UnassignedGroupsRefreshingTimeout")
#define KLCSR_UPDATES_REFRESHING_TIMEOUT			TEXT("UpdatesRefreshingTimeout")

// Configuration plugin

#define KLCP_DEF_INTERFACE_PARAMS					L"Params"
#define KLCP_EVENTS_ARRAY_NAME						L"Events"
#define KLCP_EVENTS_ARRAY_EVENT_TYPE_NAME			L"EventID"
#define KLCP_EVENTS_ARRAY_EVENT_TYPE_LOC_NAME		L"LocalizedEventIDName"
#define KLCP_EVENTS_ARRAY_EVENT_SEVERITY_NAME		L"Severity"

#define SETUP_EXE_COMMAND_LINE						TEXT("klcfginst.exe")
#define EXE_INSTALLER_NAME							TEXT("klriwrap.ex_")
#define DEST_EXE_INSTALLER_NAME						TEXT("setup.exe")
#define EXECUTABLE_SUBFOLDER						TEXT("exec")

// .kpd file values
#define KLKPD_PRODUCT_SECTION_NAME					TEXT("Product")
#define KLKPD_PRODUCT_NAME_VALUE					TEXT("Name")			// String
#define KLKPD_PRODUCT_LOCALIZED_NAME_VALUE			TEXT("LocalizedName")	// String
#define KLKPD_VERSION_SECTION_NAME					TEXT("Version")
#define KLKPD_VERSION_MAJOR_VALUE					TEXT("VerMajor")		// Int
#define KLKPD_VERSION_MINOR_VALUE					TEXT("VerMinor")		// Int
#define KLKPD_VERSION_BUILD_VALUE					TEXT("VerBuild")		// Int
#define KLKPD_VERSION_PATCH_VALUE					TEXT("VerPatch")		// Int
#define KLKPD_PRODUCT_PLUGIN_VERSION_VALUE			TEXT("PluginVersion")	// String
#define KLKPD_PRODUCT_DISPLAY_VERSION_VALUE			TEXT("DisplayVersion")	// String
#define KLKPD_ADAPTED_PRODUCT_VALUE					TEXT("AdaptedProduct")	// Int: 1 - adapted

#define KLKPD_UPD_PROD_SECTION_NAME					TEXT("UpdProducts")	
#define KLKPD_UPD_COMP_SECTION_NAME					TEXT("UpdComponents")	

#define KLRIWRP_SETUP_SECTION						TEXT("Setup")
#define KLRIWRP_SECTION								KLRIWRP_SETUP_SECTION
#define KLRIWRP_EXECUTABLE_RELPATH					TEXT("ExecutableRelPath")
#define KLRIWRP_EXECUTABLE							TEXT("Executable")
#define KLRIWRP_PARAMS								TEXT("Params")
#define KLRIWRP_USE_WRAPPER							TEXT("UseWrapper")
#define KLRIWRP_REBOOT_DELEGATED					TEXT("RebootDelegated")
#define KLRIWRP_CFG_FILE							TEXT("ConfigFile")

#define KLRIWRP_RSLT_SECTION						TEXT("Section")
#define KLRIWRP_RSLT_VALUE							TEXT("Value")
#define KLRIWRP_FILE								TEXT("File")
#define KLRIWRP_RSLT_DESCR_FILE						TEXT("DescriptionFile")
#define KLRIWRP_SHOULD_WAIT							TEXT("Wait")
#define KLRIWRP_EMPTY								TEXT("")

#define KLRIWRP_SETUP_PROC_RSLT_SECTION				TEXT("SetupProcessResult")
#define KLRIWRP_SETUP_PROC_RSLT_SUC_SECTION			TEXT("SetupProcessResult_SuccessCodes")
#define KLRIWRP_SETUP_PROC_RSLT_ERR_SECTION			TEXT("SetupProcessResult_ErrorCodes")
#define KLRIWRP_SETUP_PROC_RSLT_WRN_SECTION			TEXT("SetupProcessResult_WarningCodes")

#define KLRIWRP_SETUP_MAIN_RSLT_SECTION				TEXT("SetupMainResult")
#define KLRIWRP_SETUP_MAIN_RSLT_SUC_SECTION			TEXT("SetupMainResult_SuccessCodes")
#define KLRIWRP_SETUP_MAIN_RSLT_ERR_SECTION			TEXT("SetupMainResult_ErrorCodes")
#define KLRIWRP_SETUP_MAIN_RSLT_WRN_SECTION			TEXT("SetupMainResult_WarningCodes")

#define KLRIWRP_SETUP_DETL_RSLT_SECTION				TEXT("SetupDetailedResult")
#define KLRIWRP_SETUP_DETL_RSLT_SUC_SECTION			TEXT("SetupDetailedResult_SuccessCodes")
#define KLRIWRP_SETUP_DETL_RSLT_ERR_SECTION			TEXT("SetupDetailedResult_ErrorCodes")
#define KLRIWRP_SETUP_DETL_RSLT_WRN_SECTION			TEXT("SetupDetailedResult_WarningCodes")

#define KLRIWRP_SETUP_RESTART_SECTION				TEXT("SetupRestart")

#define KLRIWRP_INCOMP_APPS_SECTION					TEXT("IncompatibleApps")
#define KLRIWRP_INCOMP_APPS_FILE					TEXT("TxtFile")
#define KLRIWRP_INCOMP_APPS_CAN_UNINST				TEXT("CanUninst")
#define KLRIWRP_INCOMP_APPS_UNINST_INI				TEXT("IncompatibleUninstIni")
#define KLRIWRP_INCOMP_APPS_UNINST_SECTION			TEXT("IncompatibleUninstSection")
#define KLRIWRP_INCOMP_APPS_UNINST_KEY				TEXT("IncompatibleUninstKey")

//Uninstall section
#define KLRIWRP_UNINSTALL_SECTION                   TEXT("Uninstall")
//Product to uninstall
#define KLRIWRP_UNINSTALL_PRODUCT                   TEXT("Product")
//Product version (internal) to uninstall
#define KLRIWRP_UNINSTALL_VERSION                   TEXT("Version")
//Product uninstall password
#define KLRIWRP_UNINSTALL_PSWD						TEXT("UninstallData")
//Competitor Software Uninstall section
#define KLRIWRP_CMPTSFT_UNINSTALL_SECTION			TEXT("CompetitorSoftwareUninstall")
//Path where removed product ini-file is lied
//#define KLRIWRP_CMPTSFT_INI_PATH					TEXT("CompetitorSoftwareIniPath")

//Reboot section
#define KLRBTAGT_SECTION							TEXT("Action")
#define KLRBTAGT_REBOOT_IMMEDIATELY					TEXT("RebootImmediately")
#define KLRBTAGT_DLG_CAPTION						TEXT("Caption")
#define KLRBTAGT_ASK_REBOOT							TEXT("AskReboot")
#define KLRBTAGT_ASK_REBOOT_MSG						TEXT("AskRebootMsg")
#define KLRBTAGT_ASK_REBOOT_QUESTION				TEXT("AskRebootQuestion")
#define KLRBTAGT_ASK_REBOOT_PERIOD					TEXT("AskRebootPeriod")
#define KLRBTAGT_FORCE_REBOOT						TEXT("ForceReboot")
#define KLRBTAGT_FORCE_REBOOT_TIME					TEXT("ForceRebootTime")
#define KLRBTAGT_FORCE_REBOOT_TIME_LEFT_MSG			TEXT("ForceRebootTimeLeftMsg")
#define KLRBTAGT_FORCE_APPS_CLOSED					TEXT("ForceAppsClosed")
#define KLRBTAGT_BTN_YES							TEXT("BtnYes")
#define KLRBTAGT_BTN_NO								TEXT("BtnNo")
#define KLRBTAGT_BTN_CANCEL							TEXT("BtnCancel")
#define KLRBTAGT_BTN_RESTART						TEXT("BtnRestart")
#define KLRBTAGT_BTN_DELAY_REBOOT					TEXT("BtnDelayReboot")
#define KLRBTAGT_UNICODE_PREFIX						TEXT("$HEX_")
#define KLRBTAGT_UNICODE_PREFIX_LEN					5

#endif // WIN32

#endif

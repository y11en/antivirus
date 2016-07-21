////////////////////////////////////////////////////////////////////
//
//      AVPRegID.h
//      AVP registry key and value name definitions
//      General purpose
//      Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//      Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef __AVPREGID_H__
#define __AVPREGID_H__

#include <TCHAR.H>

#ifdef __IS__
#if defined(__T)
#undef __T
#define __T(x)      x
#endif
#endif


#define AVP_REGKEY_ROOT					    _T("Software\\KasperskyLab")

#define AVP_REGKEY_COMPONENTS				_T("Components")
#define AVP_REGKEY_TASKS						_T("Tasks")
#define AVP_REGKEY_INSTALLED				_T("InstalledProducts")
#define AVP_REGKEY_SHAREDFILES			_T("SharedFiles")
#define AVP_REGKEY_AUTOINSTALL			_T("AutoInstallableProducts")
#define AVP_REGKEY_APPLICATIONIDS		_T("ApplicationIDs") // Subkey of "Component\<num>" for application with more than one appID

#define AVP_REGKEY_WS								_T("AVP for WS")
#define AVP_REGKEY_NCC							_T("AVP Network Control Centre")
#define AVP_REGKEY_SERVER						_T("AVP Server")
#define AVP_REGKEY_OFFICE						_T("AVP for Office 2000")
#define AVP_REGKEY_FIREWALL					_T("AVP for Firewall")

#define AVP_REGKEY_PLATINUM					_T("AVP Platinum")


#ifndef __IS__

#define _SEP_ _T("\\")

#else

#define _SEP_ ^

#endif

#define AVP_REGKEY_COMPONENTS_FULL		AVP_REGKEY_ROOT _SEP_ AVP_REGKEY_COMPONENTS
#define AVP_REGKEY_INSTALLED_FULL			AVP_REGKEY_ROOT _SEP_ AVP_REGKEY_INSTALLED
#define AVP_REGKEY_SHAREDFILES_FULL		AVP_REGKEY_ROOT _SEP_ AVP_REGKEY_SHAREDFILES
#define AVP_REGKEY_AUTOINSTALL_FULL		AVP_REGKEY_ROOT _SEP_ AVP_REGKEY_AUTOINSTALL

#define AVP_REGKEY_INST_WS					AVP_REGKEY_INSTALLED _SEP_ AVP_REGKEY_WS
#define AVP_REGKEY_INST_NCC					AVP_REGKEY_INSTALLED _SEP_ AVP_REGKEY_NCC
#define AVP_REGKEY_INST_SERVER			AVP_REGKEY_INSTALLED _SEP_ AVP_REGKEY_SERVER
#define AVP_REGKEY_INST_OFFICE			AVP_REGKEY_INSTALLED _SEP_ AVP_REGKEY_OFFICE
#define AVP_REGKEY_INST_FIREWALL		AVP_REGKEY_INSTALLED _SEP_ AVP_REGKEY_FIREWALL


#define AVP_REGKEY_INST_WS_FULL					AVP_REGKEY_INSTALLED_FULL _SEP_ AVP_REGKEY_WS
#define AVP_REGKEY_INST_NCC_FULL				AVP_REGKEY_INSTALLED_FULL _SEP_ AVP_REGKEY_NCC
#define AVP_REGKEY_INST_SERVER_FULL			AVP_REGKEY_INSTALLED_FULL _SEP_ AVP_REGKEY_SERVER
#define AVP_REGKEY_INST_OFFICE_FULL			AVP_REGKEY_INSTALLED_FULL _SEP_ AVP_REGKEY_OFFICE
#define AVP_REGKEY_INST_FIREWALL_FULL		AVP_REGKEY_INSTALLED_FULL _SEP_ AVP_REGKEY_FIREWALL


#define AVP_REGKEY_AI_NCC						AVP_REGKEY_AUTOINSTALL	_SEP_ AVP_REGKEY_NCC
#define AVP_REGKEY_AI_SERVER				AVP_REGKEY_AUTOINSTALL	_SEP_ AVP_REGKEY_SERVER
#define AVP_REGKEY_AI_PLATINUM			AVP_REGKEY_AUTOINSTALL	_SEP_ AVP_REGKEY_PLATINUM

#define AVP_REGKEY_AI_NCC_FULL				AVP_REGKEY_AUTOINSTALL_FULL _SEP_ AVP_REGKEY_NCC
#define AVP_REGKEY_AI_SERVER_FULL			AVP_REGKEY_AUTOINSTALL_FULL _SEP_ AVP_REGKEY_SERVER
#define AVP_REGKEY_AI_PLATINUM_FULL		AVP_REGKEY_AUTOINSTALL_FULL _SEP_ AVP_REGKEY_PLATINUM


#define AVP_REGVALUE_FOLDER							_T("Folder")
#define AVP_REGVALUE_MAINDIR						_T("MainDir")
#define AVP_REGVALUE_ICONFILE						_T("IconFile") 
#define AVP_REGVALUE_NAME								_T("Name")
#define AVP_REGVALUE_EXENAME						_T("EXEName")
#define AVP_REGVALUE_UNINSTALLER				_T("UnInstaller")
#define AVP_REGVALUE_VERSION						_T("Version")
#define AVP_REGVALUE_BUILD							_T("Build")
#define AVP_REGVALUE_VERMAJOR						_T("VerMajor")
#define AVP_REGVALUE_VERMINOR						_T("VerMinor")
#define AVP_REGVALUE_VERBUILD						_T("VerBuild")
#define AVP_REGVALUE_VERPATCH						_T("VerPatch")
#define AVP_REGVALUE_USERNAME						_T("UserName")
#define AVP_REGVALUE_USERCOMPANY				_T("UserCompany")
#define AVP_REGVALUE_APPLICATIONID			_T("ApplicationID")
#define AVP_REGVALUE_ASSERVICE					_T("AsService")
#define AVP_REGVALUE_SERVICE_NAME				_T("ServiceName")
#define AVP_REGVALUE_ASINPROCDLL				_T("AsInprocDll")
#define AVP_REGVALUE_IPCSERVERNAME			_T("IPCServerName")
#define AVP_REGVALUE_SERVER							_T("Server")
#define AVP_REGVALUE_QUARANTINE_FOLDER	_T("QuarantineFolder")
#define AVP_REGVALUE_LANGUAGE						_T("Language")
#define AVP_REGVALUE_HELPFILE						_T("HelpFile")
#define AVP_REGVALUE_LOCFILE						_T("LocFile")
#define AVP_REGVALUE_LOCABOUTBMPID			_T("AboutBMPId")
#define AVP_REGVALUE_LOCSPLASHBMPID			_T("SplashBMPId")
#define AVP_REGVALUE_LOCAPPNAMEID				_T("AppNameId")
#define AVP_REGVALUE_LOCSUPPORTURL			_T("SupportURLId")
#define AVP_REGVALUE_LASTINSTALLTYPE		_T("LastInstallType")	// string
#define AVP_REGVALUE_LITE_EXENAME				_T("LiteEXEName")		// KAV Lite Updater string
#define AVP_REGVALUE_TASK_CFG_OBJ_CLSID	_T("CfgObjectCLSID")
#define AVP_REGVALUE_TASK_CFG_DLL_NAME	_T("CfgDLL")
#define AVP_REGVALUE_TASK_ICON					_T("Icon")
#define AVP_REGVALUE_TASK_TYPE					_T("Type")
#define AVP_REGVALUE_TASK_DESCRIPTION		_T("Description")



//////////////////////////////////////////////////////////////////////////
// Key name:
//		"HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{7f5e2a5a-92c5-4df1-808d-1688c50cbfee}"
// Value:
//		"Edition"

// Kaspersky Lab Products
#define KAV_REGKEY_PERSONAL_EDITION			1	// Personal
#define KAV_REGKEY_PERSONAL_PRO_EDITION		2	// Personal Pro
#define KAV_REGKEY_WORKSTATION_EDITION		3	// Workstation
#define KAV_REGKEY_SERVERNT_EDITION			4	// Server NT
#define KAV_REGKEY_LITE_EDITION				5	// Lite
#define KAV_REGKEY_LITEOEM_EDITION			6	// Lite OEM
#define KAV_REGKEY_ELITE_EDITION			7	// E-Lite
#define KAV_REGKEY_CYBERGUARD_EDITION		8	// CyberGuard

// Co-branding Products
#define KAV_REGKEY_DEERFIELDLITE_EDITION	101	// Deerfield - Lite
#define KAV_REGKEY_LITEPREMIUM_EDITION		102	// Lite Premium



//////////////////////////////////////////////////////////////////////////
// Key name:
//		HKEY_LOCAL_MACHINE\SOFTWARE\KasperskyLab\Components\NNN\Tasks\NNN\
// Value:
//		AVP_REGVALUE_CC_CTRL_BTN_MASK
// Data - Combination of flags:
//		START				0x01
//		STOP				0x02
//		TCB_PAUSE			0x04
//		RELOAD_BASES		0x08
//		RESET_STATISTICS	0x10
#define AVP_REGVALUE_CC_CTRL_BTN_MASK		_T("CtrlButtonMask")

#define AVP_REGVALUE_DEF_REPORT_FOLDER		_T("ReportFileFolder")
#define AVP_REGVALUE_DEF_PROFILE_NAME			_T("DefaultProfileName")

// Values of AVP_REGKEY_SHAREDFILES_FULL key
#define AVP_REGVALUE_VE_DATA_FILE_PATH		_T("VEDataFilePath")		// Virus encyclopaedia data file path
#define AVP_REGVALUE_VE_INDEX_FILE_PATH	  _T("VEIndexFilePath")		// Virus encyclopaedia index file path
#define AVP_REGVALUE_FSI_DRVPATH					_T("FSInterceptorDriverPath") //	FileSystems Interceptor	Driver Path

////////////////////////////////////////////////////////////////////////////////
// Alert info (DWORD):
// +-------------------------------+
// | byte3 | byte2 | word0		   |
// +-------------------------------+
//
// Word0 - alert number
//		   ATTENTION !!! For all components, which can send the	'Virus found' 
//						 alert the number of this alert	must be AVP_ALERT_ID_VIRUS_FOUND
//
// Byte2 - Alert severity (See AVP_ALERT_SEVERITY_XXXX defines below)
// Byte3 - Component ID (CID) (See AVP_CID_XXXX defines below)

#define AVP_ALERT_ID_VIRUS_FOUND			0x0202

// All components must support at least next alerts:
// 'Error executing task'       - When task cannot be completed
// 'Task aborted by user'       - When task is aborted by user

#define AVP_ALERT_SEVERITY_CRITICAL			0			// Virus found etc.
#define AVP_ALERT_SEVERITY_HIGH			        1
#define AVP_ALERT_SEVERITY_LOW			        2
#define AVP_ALERT_SEVERITY_INFORMATIONAL        3			// Task	aborted by user etc.

#define AVP_ALERT_MASK_NUMBER				0x0000FFFF
#define AVP_ALERT_MASK_COMPID				0xFF000000
#define AVP_ALERT_MASK_SEVERITY				0x00FF0000

////////////////////////////////////////////////////////////////////////////////
// Components types (DWORD):
// +-------------------------------+
// | byte3 | byte2 | byte1 | byte0 |
// +-------------------------------+
// 31			...			   0
//
// byte	  description
// ------------------
// 0	   component ID	(1 - scanner, 2 - monitor, 3 - updates...)
// 1	   component flags
//		   bit	  description
//		   ------------------
//		   0	  Does component use bases: 0 -	no, 1 - yes
//
// 2,3	   reserved

////////////////////////////////////////////////////////////////////////////////
// Task types (DWORD)
// +-------------------------------+
// | byte3 | byte2 | byte1 | byte0 |
// +-------------------------------+
// 31			...			   0
// byte	  description
// ------------------
// 0	   component number AVP_CID_XXXX(1 - scanner, 2	- monitor, 3 - updates...)
// 1	   task	number in components available task list(0, 1...)
// 2, 3 flags:
//		bit    description
//		------------------
//		0, 1, 2	Reserved
//		3	0 - Run	once task, 1 - Continuous task

// AVP bases
#define AVP_CID_BASES									0x00000000	// ID		        
#define AVP_CTYPE_BASES									0x00000100	// Type

// AVP32 scaner
#define AVP_CID_SCANNER									0x00000001	// ID		        
#define AVP_CTYPE_SCANNER								0x00000101	// Type

// AVP monitor
#define AVP_CID_MONITOR									0x00000002	// ID		        
#define AVP_CTYPE_MONITOR								0x00000102	// Type

// AVP Updates
#define AVP_CID_UPDATES									0x00000003	// ID		        
#define AVP_CTYPE_UPDATES								0x00000003	// Type

// AVP Inspector
#define AVP_CID_INSPECTOR								0x00000004	// ID		        
#define AVP_CTYPE_INSPECTOR								0x00000004	// Type

// Kaspersky Web Inspector
#define AVP_CID_WEB_INSPECTOR							0x00000024	// ID		   
#define AVP_CTYPE_WEB_INSPECTOR							0x00000024	// Type

// AVP Server
#define AVP_CID_SERVER									0x00000005	// ID		        
#define AVP_CTYPE_SERVER								0x00000005	// Type

// AVP for FireWall
#define AVP_CID_FW										0x00000006	// ID		        
#define AVP_CTYPE_FW									0x00000106	// Type

// AVP Control Centre
#define AVP_CID_CC										0x00000007	// ID		        
#define AVP_CTYPE_CC									0x00000007	// Type

// AVP Network Control Centre
#define AVP_CID_NCC										0x00000008	// ID		        
#define AVP_CTYPE_NCC									0x00000008	// Type

// AVP Office Monitor
#define AVP_CID_OFFICE_MONITOR							0x0000000B	// ID		        
#define AVP_CTYPE_OFFICE_MONITOR						0x0000010B	// Type

// AVP Lite
#define AVP_CID_LITE									0x0000000C	// ID		        
#define AVP_CTYPE_LITE									0x0000010C	// Type

// AVP Exchange
#define AVP_CID_EXCHANGE								0x00000025	// ID		        
#define AVP_CTYPE_EXCHANGE								0x00000125	// Type

// AVP Exchange Interface
#define AVP_CID_EXCHANGE_INTERFACE						0x00000026	// ID		        
#define AVP_CTYPE_EXCHANGE_INTERFACE					0x00000026	// Type

// AVP OfficeGuard
#define AVP_CID_OFFICE_GUARD							0x00000029	// ID		        
#define AVP_CTYPE_OFFICE_GUARD							0x00000029	// Type

// AVP Office Mail checker
#define AVP_CID_OFFICE_MAIL_CHECKER						0x0000002B	// ID		        
#define AVP_CTYPE_OFFICE_MAIL_CHECKER					0x0000012B	// Type

// AVP Script Checker
#define AVP_CID_SCRIPT_CHECKER							0x0000002C	// ID		        
#define AVP_CTYPE_SCRIPT_CHECKER						0x0000002C	// Type

// AVP Rescue Disk Set
#define AVP_CID_RESCUE_DISK								0x0000002D	// ID		        
#define AVP_CTYPE_RESCUE_DISK							0x0000002D	// Type

// AVP Report Viewer
#define AVP_CID_REPORT_VIEWER							0x0000002E	// ID		        
#define AVP_CTYPE_REPORT_VIEWER							0x0000002E	// Type

// AVP Deployment Tool
#define AVP_CID_DT										0x0000002F	// ID		        
#define AVP_CTYPE_DT									0x0000002F	// Type

// AVP Scanner Lite
#define AVP_CID_SCANNER_LITE							0x00000030	// ID		        
#define AVP_CTYPE_SCANNER_LITE							0x00000030	// Type

// Kaspersky Anti-Virus for Palm
#define AVP_CID_PALM									0x00000031	// ID
#define AVP_CTYPE_PALM									0x00000031  //	Type

// AVP Core Components
#define AVP_CID_CORE_COMPONENTS							0x000000FF	// ID
#define AVP_CTYPE_CORE_COMPONENTS						0x000000FF	// Type

// Kaspersky Personal Firewall
#define AVP_CID_PERSONAL_FW								0x00000009	// ID
#define AVP_CTYPE_PERSONAL_FW							0x00000109	// Type

// Steganos Personal Firewall 7
#define AVP_CID_STEGANOS_PERSONAL_FW					0x0000000A	// ID
#define AVP_CTYPE_STEGANOS_PERSONAL_FW					0x0000010A	// Type

// Kaspersky Anti-Virus for MS Exchange 2000 MMC Snap-In
#define AVP_CID_EXCHANGE2000_MMC_SNAPIN				    0x00000027	// ID
#define AVP_CTYPE_EXCHANGE2000_MMC_SNAPIN			    0x00000027	// Type

// Kaspersky Content Filter
#define AVP_CID_CONTENT_FILTER							0x00000032	// ID
#define AVP_CTYPE_CONTENT_FILTER						0x00000032  //	Type

// Kaspersky Anti-Virus for CE
#define AVP_CID_CE										0x00000033	// ID
#define AVP_CTYPE_CE									0x00000033  //	Type

// Kaspersky Anti-Virus Administration Kit
#define AVP_CID_ADMINKIT								0x00000034	// ID
#define AVP_CTYPE_ADMINKIT								0x00000034  //	Type

// Kaspersky Anti-Virus Administration Kit MMC Snap-In (Console)
#define AVP_CID_ADMINKIT_CONSOLE						0x00000028	// ID
#define AVP_CTYPE_ADMINKIT_CONSOLE						0x00000028  //	Type

// Kaspersky KCA KAV Main Module (KAVMM)
#define AVP_CID_KCA_KAVMM						        0x0000002A	// ID
#define AVP_CTYPE_KCA_KAVMM     						0x0000002A  //	Type

// Kaspersky Anti-Virus Command Shell
#define AVP_CID_KAVSHELL								0x00000035	// ID
#define AVP_CTYPE_KAVSHELL								0x00000035  //	Type

///////////////////////////////////////////////////////////////////////////
//
// Report context menu add-ins
//
///////////////////////////////////////////////////////////////////////////

// AVP_REGKEY_RPT_CTX_MENU_INFO
//		        |
//			+--- REP_CM_CTXID_XXX
//			  |
//			  +--- REP_CM_ACTIONID_XXX
//			        |
//				+--- AVP_REGVALUE_RPT_CTX_MENU_ACTION_NAME		// Action name (required)
//				+--- AVP_REGVALUE_RPT_CTX_MENU_ACTION_GROUP	// Action group	(required) - for grouping menu items in context menu
//				+--- AVP_REGVALUE_RPT_CTX_MENU_ACTION_CLSID	// Processing object CLSID
//				+--- AVP_REGVALUE_RPT_CTX_MENU_ACTION_IID		// Processing object IID

// User bases
#define REP_CM_CTXID_USER_BASE						50				// User	context id base
#define REP_CM_GROUP_USER_BASE						100			// User	action group base

// Predefined context identifiers

#define REP_CM_CTXID_VOID							0				// Void	column

#define REP_CM_CTXID_DATE							1				// Date
#define REP_CM_CTXID_TIME							2				// Time
#define REP_CM_CTXID_FILE							3				// File
#define REP_CM_CTXID_VIRUS_NAME						4				// Virus name
#define REP_CM_CTXID_NET_ADDRESS					5				// Network address
#define REP_CM_CTXID_RESULT							6				// Result of operation
#define REP_CM_CTXID_DATE_TIME						7				// Date	Time

// Predefined context menu groups
#define REP_CM_GROUP_DEFAULT						0				// Default menu	group
#define REP_CM_GROUP_FILE							10				// File	operations group
#define REP_CM_GROUP_CUSTOM_ACTION					20				// Application specific	operations group

// Predefined actions identifiers
#define REP_CM_ACTIONID_FILE_COPY					0				// Copy	file
#define REP_CM_ACTIONID_FILE_MOVE					1				// Move	file
#define REP_CM_ACTIONID_FILE_RENAME					2				// Rename file
#define REP_CM_ACTIONID_FILE_DELETE					3				// Delete file
#define REP_CM_ACTIONID_FILE_OPEN_FOLDER			6				// Open file's folder with Win Explorer

#define REP_CM_ACTIONID_VIEW_VIR_INFO				4				// View	virus info

#define REP_CM_ACTIONID_CALL_COM_OBJECT				5				// Call	specified COM object

// Registry keys (Under self registry key of each task of components)
#define AVP_REGKEY_RPT_CTX_MENU_INFO				_T("RptCtxIDs")	// Root	key

// Registry values (Under AVP_REGKEY_REPORT_CONTEXT_MENU_INFO\ContentsID registry key of each task of components)
#define AVP_REGVALUE_RPT_CTX_MENU_ACTION_NAME				_T("ActionName")	// Action name (required)
#define AVP_REGVALUE_RPT_CTX_MENU_ACTION_GROUP			_T("ActionGroup")	// Action group	(required) - for grouping menu items in context menu
#define AVP_REGVALUE_RPT_CTX_MENU_ACTION_CLSID			_T("ActionCLSID")	// Processing object CLSID
#define AVP_REGVALUE_RPT_CTX_MENU_ACTION_CMD_INDEX  _T("CommandIndex")  // Internal command ID (for Processing COM object)

#endif

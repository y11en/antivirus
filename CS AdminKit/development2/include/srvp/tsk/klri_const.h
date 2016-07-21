/*!
 * (C) 2007 "Kaspersky Lab"
 *
 * \file srvp\tsk\klri_const.h
 * \author Andrew Lashchenkov
 * \date 2007
 * \brief ѕубличные константы, используемые в задачах удаленной установки/деинсталл€ции.
 *
 */

#ifndef __TSK_RI_CONST_H__
#define __TSK_RI_CONST_H__

//! BEGIN of remote installation task settings

#define KLTSK_MAX_TRY_COUNT                 L"MaxTryCount"
#define KLTSK_MAX_TRY_COUNT_DEFAULT         3L
#define INFINITE_TRY_COUNT                  -1L

#define KLTSK_SYNC_PACKAGES_USE_PACKAGES_IDS	L"UsePackagesIds"			// BOOL_T
#define KLTSK_RI_PACKAGE_ID                 L"PackageId"					// INT_T
#define KLTSK_RI_PACKAGE_GUID               L"KLTSK_RI_PACKAGE_GUID"		// STRING_T
#define KLTSK_RI_PACKAGES_IDS				L"KLTSK_RI_PACKAGES_IDS"		// ARRAY_T <INT_T>
#define KLTSK_RI_PACKAGES_GUIDS				L"KLTSK_RI_PACKAGES_GUIDS"		// ARRAY_T <STRING_T>
#define KLTSK_RI_SUBTASKS_IDS				L"KLTSK_RI_SUBTASKS_IDS"		// ARRAY_T <INT_T>
#define KLTSK_MAIN_TASK_ID					L"KLTSK_MAIN_TASK_ID"			// INT_T

#define KLTSK_RI_USE_LOGIN_SCRIPT           L"UseLoginScript"
#define KLTSK_RI_USE_LOGIN_SCRIPT_DEFAULT   0L

#define KLTSK_RI_LOGIN_LIST                 L"LoginList"

#define KLTSK_RI_USER_NAME                  L"UserName"

#define KLTSK_RI_SKIP_PRESENT_PRODS         L"KLTSK_RI_SKIP_PRESENT_PRODS"	// BOOL_T
#define KLTSK_RI_SKIP_PRESENT_PRODS_DEFAULT true

#define KLTSK_RI_USE_SHARE					L"KLTSK_RI_USE_SHARE"			// BOOL_T
#define KLTSK_RI_USE_SHARE_DEFAULT          true

#define KLTSK_RI_USE_NAGENT					L"KLTSK_RI_USE_NAGENT"			// BOOL_T
#define KLTSK_RI_USE_NAGENT_DEFAULT         true

#define KLTSK_RI_MAX_DOWNLOADS				L"KLTSK_RI_MAX_DOWNLOADS"		// INT_T
#define KLTSK_RI_MAX_DOWNLOADS_DEFAULT      5L

//! ID of the group where to move the host when the RI task succeedes.
#define KLTSK_RI_GROUP_TO_MOVE_HOST			L"KLTSK_RI_GROUP_TO_MOVE_HOST"	// INT_T

#define KLTSK_SKIP_COMPLETED				L"KLTSK_SKIP_COMPLETED"			// BOOL_T

namespace KLTSK{
	enum RIFailReason{	// KLEVP::c_er_par1
		/* Unknown error. */
		RI_UNKNOWN = 0,
		/* Unexpected exception occured. */
		RI_FATAL_EXCEPTION = 1,
		/* The package is absent on the Administration server. */
		RI_NO_PACKAGE = 2,
		/* Host seems to be offline or hidden by firewall. */
		RI_HOST_UNAVAILABLE = 3,
		/* Unexpected system error occured. */
		RI_SYSTEM_ERROR = 4,
		/* Access denied. The specified account has no permissons for the operation, 
			or invalid account password specified. */
		RI_ACCESS_DENIED = 5,
		/* The value specified by the lpRemoteName member is not acceptable to any network resource provider, 
			either because the resource name is invalid, or because the named resource cannot be located.*/
		RI_BAD_NET_NAME = 6, 
		/* The operation cannot be performed because a network component is not started 
			or because a specified name cannot be used. */
		RI_NO_NET_OR_BAD_PATH = 7,
		/* The network is unavailable. */
		RI_NO_NETWORK = 8,
		/* Multiple connections to a server or shared resource by the same user, using more than one user name, 
			are not allowed. Disconnect all previous connections to the server or shared resource and try again.*/
		RI_SESSION_CREDENTIAL_CONFLICT = 9,
		/* Not enough storage is available on target computer to process this command.  */
		RI_NO_DISK_SPACE = 10,
		/* Timeout expired.  */
		RI_TIMEOUT_EXPIRED = 11,
		/* Can't upload the package using Network agent since it is not installed yet. */
		RI_NO_NET_AGENT = 12,
		/* Another remote installation / deinstallation task is active on the target host. */
		RI_OTHER_TASK_ACTIVE = 13,
		/* Try count exceeded. */
		RI_TRY_COUNT_EXCEEDED = 14,
		/* Unexpected OS reboot occured. The result of the installation is unknown. */
		RI_UNEXPECTED_REBOOT = 15,
		/* Some necessary file is absent in the installation package. */
		RI_FILE_NOT_FOUND_IN_PKG = 16,
		/* Product installer error. */
		RI_INSTALLER_ERROR = 17,
		/* Operation system on the target conputer is not supported by the product to be installed. */
		RI_WRONG_OS = 18,
		/* Incompatible application is installed on the target computer. */
		RI_INCOMPATIBLE_APP_FOUND = 19
	};
};

//! END of remote installation task settings

//! BEGIN of remote deinstallation task settings

#define KLTSK_RU_UNINSTALL_COMPETITOR_APP	L"UninstallCompetitorApp"		// BOOL_T
#define KLTSK_RU_PRODUCT_NAME				L"ProductName"
#define KLTSK_RU_COMPETITOR_PRODUCTS		L"CompetitorProducts"			// ARRAY_T
#define KLTSK_RU_PRODUCT_VERSION			L"ProductVersion"
#define KLTSK_RU_PRODUCT_DISPLAY_NAME		L"ProductDisplayName"
#define KLTSK_RU_DEINSTALLATION_PASSWORD	L"DeinstallationPassword"		// STRING_T

#define KLTSK_RU_REBOOT_IMMEDIATELY         L"KLTSK_RU_REBOOT_IMMEDIATELY"	// BOOL_T
#define KLTSK_RU_REBOOT_ASK_FOR_REBOOT      L"KLTSK_RU_REBOOT_ASK_FOR_REBOOT"	// BOOL_T
#define KLTSK_RU_REBOOT_ASK_FOR_REBOOT_PERIOD	L"KLTSK_RU_REBOOT_ASK_FOR_REBOOT_PERIOD"	// INT_T (in minutes)
#define KLTSK_RU_REBOOT_FORCE_REBOOT_PERIOD	L"KLTSK_RU_REBOOT_FORCE_REBOOT_PERIOD"	// INT_T (in minutes)
#define KLTSK_RU_REBOOT_MESSAGE				L"KLTSK_RU_REBOOT_MESSAGE"		// STRING_T
#define KLTSK_RU_FORCE_APPS_CLOSED			L"KLTSK_RU_FORCE_APPS_CLOSED"	// BOOL_T

#define KLTSK_RI_REBOOT_USE_TSK_SETTINGS    L"KLTSK_RI_RBT_USE_TSK_SETTINGS" // BOOL_T
#define KLTSK_RI_REBOOT_IMMEDIATELY         KLTSK_RU_REBOOT_IMMEDIATELY
#define KLTSK_RI_REBOOT_ASK_FOR_REBOOT      KLTSK_RU_REBOOT_ASK_FOR_REBOOT
#define KLTSK_RI_REBOOT_ASK_FOR_REBOOT_PERIOD	KLTSK_RU_REBOOT_ASK_FOR_REBOOT_PERIOD
#define KLTSK_RI_REBOOT_FORCE_REBOOT_PERIOD	KLTSK_RU_REBOOT_FORCE_REBOOT_PERIOD
#define KLTSK_RI_REBOOT_MESSAGE				KLTSK_RU_REBOOT_MESSAGE
#define KLTSK_RI_FORCE_APPS_CLOSED			KLTSK_RU_FORCE_APPS_CLOSED

//#define KLTSK_RI_SS_INSTALL_USE_TSK_SETTINGS    L"KLTSK_RI_SS_INSTALL_USE_TSK_SETTINGS" // BOOL_T
//#define KLTSK_RI_SS_INSTALL    L"KLTSK_RI_SS_INSTALL" // PARAMS_T

#define KLTSK_PKG_FOLDER_NAME				L"Tasks"

//! END of remote deinstallation task settings

#endif // __TSK_RI_CONST_H__

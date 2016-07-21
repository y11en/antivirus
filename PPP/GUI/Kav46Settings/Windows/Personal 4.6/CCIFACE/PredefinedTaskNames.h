#ifndef _PREDEFINEDTASKNAME_H__INCLUDED_
#define _PREDEFINEDTASKNAME_H__INCLUDED_

#define CC_TASK_UID_PREFIX						_T("CCT_")

// task names
#define CC_TASK_UID_USERTASK_PREFIX				CC_TASK_UID_PREFIX _T("USERTASK_")
#define CC_TASK_UID_FULLSCAN					CC_TASK_UID_PREFIX _T("FullScan")
#define CC_TASK_UID_REMOVABLESCAN				CC_TASK_UID_PREFIX _T("RemovableScan")
#define CC_TASK_UID_STARTUPSCAN					CC_TASK_UID_PREFIX _T("StartupScan")
#define CC_TASK_UID_UPDATER						CC_TASK_UID_PREFIX _T("Updater")
#define CC_TASK_UID_ON_ACCESS					CC_TASK_UID_PREFIX _T("OnAccess")
#define CC_TASK_UID_MAILCHECKER					CC_TASK_UID_PREFIX _T("MailChecker")
#define CC_TASK_UID_SCRIPTCHECKER				CC_TASK_UID_PREFIX _T("ScriptChecker")
#define CC_TASK_UID_QUARANTINESCAN				CC_TASK_UID_PREFIX _T("QuarantineScan")
#define CC_TASK_UID_MEMSCAN						CC_TASK_UID_PREFIX _T("MemoryScan")
#define CC_TASK_UID_CKAH						CC_TASK_UID_PREFIX _T("CKAH")
#define CC_TASK_UID_OFFICEGUARD					CC_TASK_UID_PREFIX _T("OfficeGuard")
#define CC_TASK_UID_CRITICALAREASSCAN			CC_TASK_UID_PREFIX _T("CriticalAreas")
#define CC_TASK_UID_GLOBALTIF_SCAN				CC_TASK_UID_PREFIX _T("GlobalTIFScan")
#define CC_TASK_UID_ROLLBACK					CC_TASK_UID_PREFIX _T("Rollback")
#define CC_TASK_UID_ANTISPAM					CC_TASK_UID_PREFIX _T("AntiSpam")
#define CC_TASK_UID_SERVICE						CC_TASK_UID_PREFIX _T("Service")

// префикс задачи сканирования пользовательских объектов
// используется сам по себе при получении исключений для 
// задачи сканирования пользовательских объектов
#define CC_TASK_UID_USER_OBJECT_SCAN			CC_TASK_UID_PREFIX _T("UserObjectScan")
// префикс задачи сканирования пользовательских объектов, запущенной из kavshell
#define CC_TASK_UID_USER_OBJECT_SCAN_SHELL		CC_TASK_UID_PREFIX _T("UserObjectScanShell")

// task names for GUI subscription only
#define CC_TASK_UID_PRODUCT_STATUS				CC_TASK_UID_PREFIX _T("ProductStatus")
#define CC_TASK_UID_USER_SETTINGS_STATUS		CC_TASK_UID_PREFIX _T("UserSettingsStatus")
#define CC_TASK_UID_REPORT_CHANGE_NOTIFICATION	CC_TASK_UID_PREFIX _T("ReportChangeNotification")
#define CC_TASK_UID_QUARANTINE_NOTIFICATION		CC_TASK_UID_PREFIX _T("QuarantineNotification")
#define CC_TASK_UID_BACKUP_NOTIFICATION			CC_TASK_UID_PREFIX _T("BackupNotification")

#define CC_TASK_UID_USERTASK_NOTIFICATION		CC_TASK_UID_PREFIX _T("UserTaskNotification") // data is CUserTaskChangeNotification*
#define CC_TASK_UID_TIF_CHANGE_NOTIFICATION		CC_TASK_UID_PREFIX _T("TIFChangeNotification")
#define CC_TASK_UID_COMPONENT_INSTALL_NOTIFICATION		CC_TASK_UID_PREFIX _T("ComponentInstallNotification")

#endif // _PREDEFINEDTASKNAME_H__INCLUDED_
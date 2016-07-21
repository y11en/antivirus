#pragma once

#ifndef PID_H_96FF0558_8767_433f_804D_8A7AA9C84EB2
#define PID_H_96FF0558_8767_433f_804D_8A7AA9C84EB2

// PID format
// DWORD : (WORD1,WORD0)
// WORD1 : unused
// WORD0 : bits
// 0xFNN(F - flags, NNN - number)
// Flags : bit 0 = 1 - Array of object, 0 - single object

#define PID_POLICY_DAT          0x0F000010
#define PID_POLICY_DAT2         0x0F0000F0

#define PID_BASE                0x10000001

#define PID_SINGLE_TIME         0x00000001
#define PID_TIME                0x00000002
#define PID_TASK                0x00000003
#define PID_TASK_LIST           0x00001004
#define PID_ALERT               0x00000005
#define PID_ADDRESS             0x00000006
#define PID_COMPONENT           0x00000007
#define PID_ARRAY               0x00001008
#define PID_ITEM                0x00000009
#define PID_ALERTS_LIST         0x0000100A
#define PID_ICON_STORE			0x0000000B
#define PID_BMP_STORE			0x0000000C
#define PID_SERVER_SETTINGS		0x0000000F
#define PID_ARRAY_ARI           0x0000000D
#define PID_ARI					0x0000000E
#define PID_ARRAY_SI			0x0000000F
#define PID_SI					0x00000021
#define PID_PI					0x00000022
#define PID_UPSI				0x00000023
#define PID_RI					0x00000024
#define PID_PPS					0x00000025
#define PID_SETTINGSINSTALLER	0x00000026
#define PID_OPS					0x00000027
#define PID_WORKSTATION			0x00000010
#define PID_WORKSTATION_LIST	0x00001011
#define PID_SERVER				0x00000012
#define PID_SERVER_LIST			0x00001013
#define PID_GROUP				0x00000014
#define PID_NETOBJECT			0x00000015
#define PID_ARRAYNETOBJECTS		0x00001016
#define PID_OBJECT_LIST			0x00001017
#define PID_PI_LIST				0x00001018
#define PID_RI_LIST				0x00001019
#define PID_AIP					0x00000028
#define PID_AIP_LIST			0x00001021
#define PID_SECURIY_SETTINGS	0x00000029
#define PID_AIPA				0x0000002A
#define PID_SS_SERVER			0x0000002B
#define PID_SETTINGS_OPTIONS	0x0000002E
#define PID_REMOTE_MANAGEMENT_SETTINGS	0x0000002F
#define PID_REMOTE_MANAGEMENT_USING_NCC	0x00000030
#define PID_EVENT_INFO					0x00000031
#define PID_CONTAINER_EVENT_INFO		0x00001031
#define PID_OSVERSION_INFO				0x00000032
#define PID_TIME_STORE					0x00000033
#define PID_KEY_INFO					0x00000034
#define PID_KEY_INFO_CONT				0x00001034
#define PID_QUARANTINE_SETTINGS			0x00000035
// PID for MenageComputer call
#define PID_ITEM_PROP							0x00000020
#define PID_AVPS_STORAGE_ITEM_INFO				0x00000040
#define PID_AVPS_STORAGE_ITEM_INFO_CONTAINER	0x00001041

#define PID_GROUP_WS					0x00000050

#define PID_USER_SETTINGS						0x00000080
#define PID_USER_SETTINGS_RTP					0x00000081
#define PID_USER_SETTINGS_UPDATER				0x00000082
#define PID_USER_SETTINGS_ODS					0x00000083
#define PID_USER_SETTINGS_GUI					0x00000084
#define PID_USER_SETTINGS_SCAN_OBJECT			0x00000085
#define PID_USER_SETTINGS_SCAN_OBJECTS			0x00000086
#define PID_USER_SETTINGS_SELECT_OBJECT_OBJECT	0x00000087
#define PID_USER_SETTINGS_SELECT_OBJECT_OBJECTS	0x00000088
#define PID_USER_SETTINGS_UPDATER_GUI           0x00000089
#define PID_USER_SETTINGS_QUARANTINE			0x0000008A
#define	PID_USER_SETTINGS_SCHEDULE				0x0000008B
#define	PID_REPORT_SETTINGS_ODS					0x0000008C
#define	PID_TASK_STATE							0x0000008D
#define PID_USER_SETTINGS_BACKUP				0x0000008E
#define PID_SCHEDULE_POINT						0x0000008F
#define PID_PROXY_SETTINGS						0x00000090
#define	PID_USER_SETTINGS_CKAH					0x00000091
#define PID_USER_SETTINGS_OFFICE_GUARD			0x00000092
#define PID_USER_SETTINGS_AV                    0x00000093
#define PID_USER_TASK_SETTINGS                  0x00000094
#define PID_USER_SETTINGS_MC					0x00000096	// для PPro настройки для Mail Checker
#define PID_USER_SETTINGS_SC					0x00000097	// для PPro настройки для Script Checker
#define PID_USER_SETTINGS_OAS					0x00000098	// для PPro настройки для On-Access Scanner
#define PID_PATCH_LIST							0x00000099	// Список патчей
#define PID_USER_SETTINGS_EXCLUDE_ITEM			0x0000009A	// исключение из скнирования
#define PID_USER_SETTINGS_EXCLUDE_LIST			0x0000009B	// спикок исключений
#define	PID_REPORT_SETTINGS_RTP					0x0000009C	// настройки RTP c exclude для сохранения в отчёте
#define	PID_UPDATER_SOURCE     					0x0000009D	// 
#define PID_USER_SETTINGS_QB_RESTORE			0x0000009E	// Настройки восстановления для карантина и бэкапа
#define PID_USER_SETTINGS_FIREWALL				0x0000009F	// для PPro настройки для Firewall
#define PID_USER_SETTINGS_MANDATORY_FLAG_SET    0x000000a0  // набор флагов для источника значений настроек (политика/настройки)
#define PID_POLICY								0x000000a1	// политика + флаги
#define PID_POLICY_USER_SETTINGS				0x000000a2	// локальные настройки
#define PID_USER_SETTINGS_EXCLUDED_PROCESS_OBJECT 0x000000a3// исключаемый процесс
#define PID_USER_SETTINGS_EXCLUDED_PROCESSES	0x000000a4	// исключаемые процессы
#define PID_UPDATER_TASK_SETTINGS				0x000000a5	// настройки задач обновления
#define PID_USER_SETTINGS_MANDATORY_FLAG_SET_UPDATER 0x000000a6  // набор флагов для источника значений настроек updater (политика/настройки)
#define PID_USER_SETTINGS_MANDATORY_FLAG_SET_ODS 0x000000a7  // набор флагов для источника значений настроек ODS (политика/настройки)
#define PID_UPDATE_OPTIONS						0x000000a8	// options for update mode
#define PID_USER_SETTINGS_RISKWARE_ITEM			0x000000a9  // элемент списка Riskware
#define PID_UPDATE_RETRANSLATION			    0x000000aa  // options for update retranslation
#define PID_AMDIN_EXCLUDE_SETTINGS			    0x000000ab 
#define PID_INSTALL_KEY_TASK_SETTINGS			0x000000ac // настройки задач инсталляции ключа
#define PID_ROLLBACK_TASK_SETTINGS				0x000000ad // настройки задач отката баз
#define PID_APP_DATA_FOLDERS					0x000000ae // пути к папкам продукта
#define PID_USER_SETTINGS_KNOWN_EXCLUDED_PROCESS_OBJECT 0x000000af// исключаемый процесс

#define PID_PRODUCT_STATUS						0x000000b0
#define PID_PRODUCT_STATUS_RTP					0x000000b1
#define PID_PRODUCT_STATUS_FULLSCAN				0x000000b2
#define PID_PRODUCT_STATUS_UPDATER				0x000000b3
#define PID_PRODUCT_VERSION_INFO				0x000000b4
#define PID_PRODUCT_STATISTICS					0x000000b5	// Статистика проверенных объектов и обнаруженных вирусов
#define PID_PRODUCT_INFO						0x000000b6
 
#define PID_LICENSING_LICINFO					0x000000c1
#define PID_LICENSING_ADD_KEY					0x000000c2
#define PID_LICENSING_REVOKE_KEY				0x000000c4
#define PID_LICENSING_CHECK_KEY					0x000000c5
#define PID_UPDATER_BASES_REPLACE_REQUEST		0x000000c6
#define PID_UPDATER_CUSTOM_QUESTION				0x000000c7
#define PID_LICENSING_CHECK_KEY_AS_RESERVED		0x000000c8
#define PID_LOGON_CREDENTIALS					0x000000ca
#define PID_UPDATER_STORAGE						0x000000cb	// Хранилище для Updater
#define PID_AVAILABLE_UPDATE_REGIONS			0x000000cd	// доступные регионы обновления
#define PID_QUARANTINE_STATISTICS				0x000000ce
#define PID_AK_STUFF_STORAGE					0x000000cf // Помойка для AdminKit-related объектов
#define PID_INSTALL_LIC_KEY_TASK_SETTINGS		0x000000d2 // данные задачи инсталяции лицензионного ключа
#define PID_POLICY_SECTION						0x000000d3 // Секция политики

#define PID_SERVICE_REQUEST						0x000000e1 // Запрос из сервиса
#define PID_SERVICE_RESPONSE					0x000000e2 // Ответ на зпрос из сервиса

#define	PID_REPORT_SETTINGS_UPDATER				0x000000e3	// настройки Update/Rollback для сохранения в отчёте

////////////////////////////////////////////////////////////////////////////////////
// CCService Command pid's.

#define PID_CCCMD_SCREENSAVE					0xF0000201	// Screen saver starts
#define PID_CCCMD_EXECUTE						0xF0000202	// Execute file
#define PID_CCCMD_SERVERSTARTS					0xF0000203	// CCUser IPC Server starts
#define PID_CCCMD_SETDATA						0xF0000204	// CCUser requests current settings from CCService to Man
#define PID_CCCMD_QUIT							0xF0000205	// Quit CCService
#define PID_CCCMD_LOCKUI						0xF0000206	// Lock User interface part
#define PID_CCCMD_UNLOCKUI						0xF0000207	// UnLock User interface part
#define PID_CCCMD_REFRESH_COMPONENT_LIST		0xF0000208	// Refresh compnent list
#define PID_CCCMD_CUSTOM_COMMAND				0xF0000209	// Execute custom command
#define PID_CCCMD_IMPORT_DATA					0xF000020A	// Import setings from user file
#define PID_CCUSERCMD_SETDATA					0xF0000300	// Send data to User part
#define PID_CCCMD_TASK_FAILED					0xF0000301	// Notification about CC Task failed
#define PID_CCCMD_CUSTOM_COMMAND_EX				0xF0000302	// Execute custom command non blocking user part server
#define PID_CCCMD_QUERYIMPERSONATION			0xF0000303	// Query impersonation
#define PID_CCCMD_TASK_STATISTICS				0xF0000304	// Task statistics
#define PID_CCCMD_TASK_EVENT					0xF0000305	// Task event
#define PID_CCCMD_TASK_STATE					0xF0000306	// Task state
#define PID_CCCMD_PRODUCT_STATUS				0xF0000307	// Product status
#define PID_CCCMD_USER_SETTINGS_STATUS			0xF0000308	// User settings status
#define PID_CCCMD_REPORT_CHANGE_NOTIFICATION	0xF0000309	// Report change notification
#define PID_CCCMD_QUARANTINE_NOTIFICATION		0xF000030A	// Quarantine operation result notification
#define PID_CCCMD_QUARANTINE_OVERFLOW			0xF000030B	// Quarantine overflow
#define PID_CCCMD_BACKUP_NOTIFICATION			0xF000030C	// Backup operation result notification
#define PID_CCCMD_BACKUP_OVERFLOW				0xF000030D	// Backup overflow
#define PID_CCCMD_USER_TASK_CHANGE_NOTIFICATION	0xF000030F	// UserTask change notification
#define PID_CCCMD_GLOBAL_TIF_CHANGE				0xF0000310	// tif storage change
#define PID_CCCMD_LIC_KEYS_CHANGED				0xF0000311	// Licensing keys changed
#define PID_CCCMD_VERSION_CHANGE_NOTIFICATION	0xF0000312	// Product version changed
#define PID_CCCMD_EVENT							0xF0000313	// Admin Kit event
#define PID_CCCMD_DANGEROUS_OBJECT_NOTIFICATION	0xF0000314	// Notification of dangerous object
#define PID_CCCMD_INSTALL_FAILED_NOTIFICATION	0xF0000315	// Install of component has failed

////////////////////////////////////////////////////////////////////////////////////
// AVPMan IPC Command pid's.

#define PID_IPCMAN_IPCSERVERSTARTS				0xF0000105	// Man IPC Server starts
#define PID_IPCMAN_SET_DATA						0xF0000101	// Set data to CC from Man
#define PID_IPCMAN_EXEC_TASK					0xF0000102	// Execute task now
#define PID_IPCMAN_INSTALL						0xF0000103	// Install component
#define PID_IPCMAN_UNINSTALL					0xF0000104	// Uninstall component
#define PID_IPCMAN_GETTASKSTATE					0xF0000106	// Get task state command
#define PID_IPCMAN_RELOAD_BASES					0xF0000108	// Send reload bases message to component
#define PID_IPCMAN_RESET_TASK_STATISTICS		0xF0000109	// Reset task statistics
#define PID_IPCMAN_CCCOMMAND					0xF000010A	// CC command

#define PID_IPCMAN_SET_USER_SETTINGS			0xF0000110	// Settings update from GUI
#define PID_IPCMAN_GET_USER_SETTINGS			0xF0000111	// Settings request from GUI to service

#define PID_IPCMAN_GETENGINEPERFORMANCE			0xF0000112	// Engine performance request
#define PID_IPCMAN_GET_TASKREPORTID				0xF0000113	// Task report ID

#define PID_IPCMAN_QUARANTINE_COMMAND			0xF0000114	// Process command related to quarantine

#define PID_IPCMAN_DELETE_REPORT_RECORD			0xF0000115	// Delete report manager record
#define PID_IPCMAN_DELETE_ALL_REPORT_RECORDS	0xF0000116	// Delete all report manager records
#define PID_IPCMAN_CANCEL_UPDATER_WAITING		0xF0000117	// Cancel Updaters waiting of the Internet
#define PID_IPCMAN_GETNETSTATE					0xF0000118	// Get network state
#define PID_IPCMAN_GETUSERTASKS					0xF0000119	// Get user tasks
#define PID_IPCMAN_GETUSERTASK					0xF000011A	// Get user task
#define PID_IPCMAN_SETUSERTASK					0xF000011B	// Set user task
#define PID_IPCMAN_ADDUSERTASK					0xF000011C	// Add user task
#define PID_IPCMAN_DELETEUSERTASK				0xF000011D	// Delete user task
#define PID_IPCMAN_RUNUSERTASK					0xF000011E	// Run user task
#define PID_IPCMAN_EXPORT_PRODUCT_SETTINGS		0xF0000120	// Export product settings
#define PID_IPCMAN_IMPORT_PRODUCT_SETTINGS		0xF0000121	// Import product settings
#define PID_IPCMAN_SET_DEFAULT_PRODUCT_SETTINGS	0xF0000122	// Set default product settings
#define PID_IPCMAN_SETTASKEXCLUDE				0xF0000124	// Set task excludes
#define PID_IPCMAN_GETTASKEXCLUDE				0xF0000125	// Get task excludes
#define PID_IPCMAN_GETBASETIMEFORNEXTSTART		0xF0000126	// Get base time for next task start
#define PID_IPCMAN_GETQBSTATISTICS				0xF0000127	// Get QB statistics
#define PID_IPCMAN_GETUSERTASKPASSWORD			0xF0000128	// Get user task plain password from Run As... page
#define PID_IPCMAN_SETUSERTASKAKPASSWORDBLOB	0xF0000129	// Set user task AK password blob
#define PID_IPCMAN_DISCARD_TIF_RECORDS			0xF000012A	// Delete tif entries
#define PID_IPCMAN_IDS_NOTIFY_OF_REBOOT			0xF000012B	// Notify of reboot because of IDS invoked
#define PID_IPCMAN_INSTALL_COMPONENT			0xF000012C	// Install component
#define PID_IPCMAN_MERGEUSERTASKSETTINGS		0xF000012D	// Merge (apply policy to) user task settings

// Install key task
#define AVP_CID_INSTALL_KEY						0x00000055	// ID
#define AVP_CTYPE_INSTALL_KEY					0x00000055	// Type

#endif // PID_H_96FF0558_8767_433f_804D_8A7AA9C84EB2

#ifndef CFGPAGESREGISTEREDWINDOWMESSAGES_H
#define CFGPAGESREGISTEREDWINDOWMESSAGES_H

// Command IDs
#define CMD_START_TASK							0xFFFF1004
#define CMD_STOP_TASK							0xFFFF1005
#define CMD_PAUSE_TASK							0xFFFF1006
#define CMD_RESUME_TASK							0xFFFF1007

#define CMD_DELETE_EVENT						0xFFFF2001
#define CMD_CLEAR_EVENTS						0xFFFF2002

// Notifications to pages (NID): 
#define CFGPAGE_NID_AVPS_STORAGE_CHANGED         0x1000

// Registered window messages to CC Configurator Pages

// Notification about new task state
// uParam: 0
// lParam: LPPOLICY pPolicy
extern UINT RWM_NOTIFY_NEW_TASK_STATE;

// Notification about new task statistics
// uParam: 0
// lParam: LPPOLICY pPolicy
extern UINT RWM_NOTIFY_NEW_TASK_STATISTICS;

// Notification about new Event from Task
// uParam: 0
// lParam: LPPOLICY pPolicy
extern UINT RWM_NOTIFY_NEW_TASK_EVENT;

// Notification about multiply object selection 
// uParam: 0
// lParam: LPPOLICY pPolicy
extern UINT RWM_NOTIFY_MULTIPLY_OBJECTS_SELECTED;

// Various Notification for Cfg Pages
// uParam: IN	  DWORD dwNotificationID
// lParam: IN/OUT LPPOLICY pPolicy
extern UINT RWM_NOTIFY_CFG_PAGES;

/////////////////////////////////////////////////////////////////////////////

// Registered window messages from CC Configurator Pages

// Start/Stop/Pause/Resume task
// uParam: DWORD dwControlCommand (CMD_START_TASK, etc.)
// lParam: DWORD dwTaskNumInList
extern UINT RWM_CFGPAGE_CMD_TASK_CONTROL;				

// Reload task antivirus bases
// uParam: 0
// lParam: DWORD dwTaskNumInList
extern UINT RWM_CFGPAGE_CMD_RELOAD_TASK_AVBASES;

// Reset task statistics
// uParam: 0
// lParam: DWORD dwTaskNumInList
extern UINT RWM_CFGPAGE_CMD_RESET_TASK_STATISTICS;

// Run component executable
// uParam: HWND hComponentPageWnd
// lParam: LPCSTR pszComponentExecutable
extern UINT RWM_CFGPAGE_CMD_RUN_COMPONENT;

// User selects 'New task' command on 'Components' page of CC Cfg
// uParam: 0
// lParam: IN DWORD dwTaskType
extern UINT RWM_CFGPAGE_CMD_NEW_TASK_REQUEST;

// 'New task' command to 'Tasks' page of CC Cfg
// uParam: 0
// lParam: IN DWORD dwTaskType
extern UINT RWM_CFGPAGE_CMD_NEW_TASK;						

// Notification about Cfg Page settings modified
// uParam: 0
// lParam: 0
extern UINT RWM_CFGPAGE_NOTIFY_SETTINGS_MODIFIED;

// Notification about modifying task list (adding/removing task)
// uParam: DWORD dwTaskNumInList
// lParam: CTask* pTask
extern UINT RWM_CFGPAGE_NOTIFY_TASK_LIST_MODIFIED;

// Notification for Cfg Pages about aplying changes
// uParam: IN CSettings* pSettings
// lParam: IN LPPOLICY	pSettingsPolicy
extern UINT RWM_CFGPAGE_NOTIFY_REFRESH_OTHER_SETTINGS;		

// Request for configured object (WS or Server) network info (address, port, etc.)
// uParam: IN Pointer to malloc
// lParam: OUT LPPOLICY* ppNetInfoPolicy
extern UINT RWM_CFGPAGE_GET_COBJECT_NET_INFO;

// Request for 'About' info from configured object (WS or Server)
// uParam: IN DWORD dwComponentType
// lParam: OUT LPPOLICY* ppAboutInfoPolicy
extern UINT RWM_CFGPAGE_GET_COBJECT_ABOUT_INFO;

// Request for MAPI Profiles info from configured object (WS or Server)
// uParam: 0
// lParam: OUT LPPOLICY* ppMAPISettingsPolicy
extern UINT RWM_CFGPAGE_GET_COBJECT_MAPI_PROFILES;

// Request for current task state
// uParam: OUT PDWORD pdwTaskState
// lParam: OUT LPCTSTR pszTaskName
extern UINT RWM_CFGPAGE_GET_COBJECT_TASK_STATE;

// Verifying of component installation
// uParam: IN DWORD dwComponentID
// lParam: OUT PBOOL pbResult
extern UINT RWM_CFGPAGE_GET_COBJECT_COMPONENT_INSTALLATION;

// Command from CC Cfg pages
// uParam: 0 (Always PID_IPCMAN_CCCOMMAND)
// lParam: IN/OUT LPPOLICY pCommandPolicy
extern UINT RWM_CFGPAGE_CMD_SEND_COMMAND;

// Command from CC Cfg pages (Extended command version)
// uParam: IN	  DWORD dwCommand
// lParam: IN/OUT LPPOLICY pCommandPolicy
extern UINT RWM_CFGPAGE_CMD_SEND_COMMAND2;

// Command from AVPS Cfg pages
// uParam: 0
// lParam: IN/OUT LPPOLICY pCommandPolicy
extern UINT RWM_AVPSCFG_CMD_SEND_COMMAND;

// Command from AVPS Cfg pages (Extended command version)
// uParam: IN	  DWORD dwCommand
// lParam: IN/OUT LPPOLICY pCommandPolicy
extern UINT RWM_AVPSCFG_CMD_SEND_COMMAND2;

// Test is requested component installed on target computer
// uParam: IN	  DWORD dwComponentID
// lParam: IN/OUT LPBOOL pbInstalled
extern UINT RWM_CFGPAGE_IS_COMPONENT_INSTALLED;

// Registered messages CCUser <-> CC Configurator Pages

// Restore CC Cfg Window when duplucate copy of CC executes
// uParam: 0
// lParam: 0
extern UINT RWM_RESTORE_CCCFG_WND;

// Notification from CCCfg Window about its creation
// uParam: 0
// lParam: IN HWND hCCCfgWnd
extern UINT RWM_CCCFG_WND_CREATED;

////////////////////////////////////////////////////////////// 
// Old message names

//UINT LM_TASK_STATE					// Notification about new task state
//UINT LM_TASK_STATISTICS				// Notification about new task statistics
//UINT LM_TASK_EVENT					// Event from Task 
//UINT AM_EXECUTE_TASK_NOW				// Start/Stop task
//UINT AM_RELOAD_BASES					// Reload task antivirus bases
//UINT AM_RESET_TASK_STATISTICS			// Reset task statistics
//UINT AM_RUN							// Run component executable
//UINT MM_SETTINGS_CHANGED				// Cfg Page settings modified
//UINT MM_GET_NET_WS_SETTINGS			// Request for configured object (WS or Server) info
//UINT MM_GET_ABOUT_INFO				// Request for 'About' info from configured object (WS or Server)
//UINT LM_GET_MAPI_PROFILES				// Request for MAPI Profiles info from configured object (WS or Server)
//UINT LM_NEW_TASK_REQUEST				// User selects 'New task' command on 'Components' page of CC Cfg
//UINT LM_NEW_TASK						// 'New task' command to 'Tasks' page of CC Cfg
//UINT LM_GET_TASK_STATE				// Request for current task state
//UINT LM_IS_COMPONENT_INSTALLED		// Verifying of component installation
//UINT LM_TASK_LIST_MODIFIED			// Notification about modifying task list (adding/removing task)
//UINT GM_AVPCCCFG_SEND_COMMAND			// Command from CC Cfg pages
//UINT GM_AVPCCCFG_SEND_COMMAND2		// Command from CC Cfg pages (Extended command version)
//UINT AVPLM_REFRESH_OTHER_SETTINGS		// Notification for Cfg Pages about aplying changes
//UINT AM_RESTORE_MAN_WND				// Restore CC Cfg Window when duplucate copy of CC executes
//UINT LM_CFG_WINDOW_CREATED			// Notification from CCCfg Window about its creation
//UINT PNM_MULTIPLY_OBJECTS_SELECTED;	// Notification about multiply object selection 

#endif
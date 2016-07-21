#include <Windows.h>

//////////////////////////////////////////////////////////////////////////////////////
// Registering Cfg Pages Window Messages

// Registered window messages to CC Configurator Pages
UINT RWM_NOTIFY_NEW_TASK_STATE = RegisterWindowMessage("LM_TASK_STATE");						// Notification about new task state
UINT RWM_NOTIFY_NEW_TASK_STATISTICS = RegisterWindowMessage("LM_TASK_STATISTICS");				// Notification about new task statistics
UINT RWM_NOTIFY_NEW_TASK_EVENT = RegisterWindowMessage("LM_TASK_EVENT");						// Notification about new event from Task
UINT RWM_NOTIFY_MULTIPLY_OBJECTS_SELECTED = RegisterWindowMessage("PNM_MULTIPLY_OBJECTS_SELECTED");		// Notification about multiply object selection 
UINT RWM_NOTIFY_CFG_PAGES = RegisterWindowMessage("BM_CFG_PAGES_NOTIFICATION");					// Various Notification for Cfg Pages

// Registered window messages from CC Configurator Pages
UINT RWM_CFGPAGE_CMD_TASK_CONTROL = RegisterWindowMessage("AM_EXECUTE_TASK_NOW");				// Start/Stop/Pause/Resume task
UINT RWM_CFGPAGE_CMD_RELOAD_TASK_AVBASES = RegisterWindowMessage("AM_RELOAD_BASES");			// Reload task antivirus bases
UINT RWM_CFGPAGE_CMD_RESET_TASK_STATISTICS = RegisterWindowMessage("AM_RESET_TASK_STATISTICS");	// Reset task statistics
UINT RWM_CFGPAGE_CMD_RUN_COMPONENT = RegisterWindowMessage("AM_RUN");							// Run component executable

UINT RWM_CFGPAGE_CMD_NEW_TASK_REQUEST = RegisterWindowMessage("LM_NEW_TASK_REQUEST");			// User selects 'New task' command on 'Components' page of CC Cfg
UINT RWM_CFGPAGE_CMD_NEW_TASK = RegisterWindowMessage("LM_NEW_TASK");							// 'New task' command to 'Tasks' page of CC Cfg

UINT RWM_CFGPAGE_NOTIFY_SETTINGS_MODIFIED = RegisterWindowMessage("MM_SETTINGS_CHANGED");		// Cfg Page settings modified
UINT RWM_CFGPAGE_NOTIFY_TASK_LIST_MODIFIED = RegisterWindowMessage("LM_TASK_LIST_MODIFIED");	// Notification about modifying task list (adding/removing task)
UINT RWM_CFGPAGE_NOTIFY_REFRESH_OTHER_SETTINGS = RegisterWindowMessage("AVPLM_REFRESH_OTHER_SETTINGS");		// Notification for Cfg Pages about aplying changes

UINT RWM_CFGPAGE_GET_COBJECT_NET_INFO = RegisterWindowMessage("MM_GET_NET_WS_SETTINGS");		// Request for configured object (WS or Server) info
UINT RWM_CFGPAGE_GET_COBJECT_ABOUT_INFO = RegisterWindowMessage("MM_GET_ABOUT_INFO");			// Request for 'About' info from configured object (WS or Server)
UINT RWM_CFGPAGE_GET_COBJECT_MAPI_PROFILES = RegisterWindowMessage("LM_GET_MAPI_PROFILES");		// Request for MAPI Profiles info from configured object (WS or Server)
UINT RWM_CFGPAGE_GET_COBJECT_TASK_STATE = RegisterWindowMessage("LM_GET_TASK_STATE");			// Request for current task state
UINT RWM_CFGPAGE_GET_COBJECT_COMPONENT_INSTALLATION = RegisterWindowMessage("LM_IS_COMPONENT_INSTALLED");	// Verifying of component installation

UINT RWM_CFGPAGE_CMD_SEND_COMMAND = RegisterWindowMessage("GM_AVPCCCFG_SEND_COMMAND");			// Command from CC Cfg pages
UINT RWM_CFGPAGE_CMD_SEND_COMMAND2 = RegisterWindowMessage("GM_AVPCCCFG_SEND_COMMAND2");		// Command from CC Cfg pages (Extended command version)

UINT RWM_AVPSCFG_CMD_SEND_COMMAND = RegisterWindowMessage("GM_AVPSCFG_SEND_OLD_COMMAND");		// Command from AVPS Cfg pages
UINT RWM_AVPSCFG_CMD_SEND_COMMAND2 = RegisterWindowMessage("GM_AVPSCFG_SEND_COMMAND");			// Command from AVPS Cfg pages (Extended command version)
UINT RWM_CFGPAGE_IS_COMPONENT_INSTALLED = RegisterWindowMessage("LM_IS_COMPONENT_INSTALLED");	// Test is requested component installed on target computer

// Registered messages CCUser <-> CC Configurator Pages
UINT RWM_RESTORE_CCCFG_WND = RegisterWindowMessage("AM_RESTORE_MAN_WND");						// Restore CC Cfg Window when duplucate copy of CC executes
UINT RWM_CCCFG_WND_CREATED = RegisterWindowMessage("LM_CFG_WINDOW_CREATED");				// Notification from CCCfg Window about its creation

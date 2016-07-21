#ifndef __s_startupenu2
#define __s_startupenu2

#include <Prague/pr_cpp.h>
#include <Prague/pr_serdescriptor.h>
#include <Prague/pr_serializable.h>
#include <Prague/pr_vector.h>

#include <AV/plugin/p_startupenum2.h>
    
#define npPROP_PROGRESS_COUNT_PROCENT   ((tSTRING)("npPROP_PROGRESS_COUNT_PROCENT")) // Имя DWORD свойства. //eng:Name of DWORD property.

typedef enum _STURTUP_OBJ_TYPES{
	OBJECT_UNKNOWN_TYPE,
	OBJECT_REGISTRY_TYPE,
	OBJECT_REGISTRY_KEY_TYPE,
	OBJECT_REGISTRY_VALUE_TYPE,
	OBJECT_REGISTRY_DATA_TYPE,
	OBJECT_INI_TYPE,
	OBJECT_BAT_TYPE,
	OBJECT_START_UP_MENU_TYPE,
	OBJECT_LINK_FILE_TYPE,
	OBJECT_TASK_MANAGER_TYPE,
	OBJECT_TYPE_RUNNING_PROCESS,
	OBJECT_HOSTS_TYPE,
}STURTUP_OBJ_TYPES;

typedef enum _ActionType{
	UnknownAct,
	ScanAct,
	FileToDel,
	CutData,
	DelData,
	RestDef,
	DelKey,
	DelThisKey,
	SuspiciousDetect,
}ActionType;

typedef struct _ERR_MESSAGE_DATA{
	wchar_t*			m_sPath;		//	REGISTRY_TYPE,INI_TYPE,BAT_TYPE
	wchar_t*			m_sSection;	//	INI_TYPE
	wchar_t*			m_sValue;		//	REGISTRY_TYPE,INI_TYPE
	STURTUP_OBJ_TYPES	m_ObjType;
	tERROR				m_error;		//	error code
} ERR_MESSAGE_DATA, *pERR_MESSAGE_DATA;

typedef struct _ACTION_MESSAGE_DATA {
	wchar_t*			m_sFilePath;	//	Founded file path
	wchar_t*			m_sSection;
	wchar_t*			m_sValue;		//	REGISTRY_TYPE-value,INI_TYPE-value,START_UP_MENU/BAT_TYPE/LINK_FILE/TASK_MANAGER-empty
	STURTUP_OBJ_TYPES	m_ObjType;
	wchar_t*			m_sRoot;		//	REGISTRY_TYPE-key,INI_TYPE/BAT_TYPE/LINK_FILE-file,START_UP_MENU-folder,TASK_MANAGER-taskName
	ActionType			m_dAction;
} ACTION_MESSAGE_DATA, *pACTION_MESSAGE_DATA;

#endif // __s_ipresolver

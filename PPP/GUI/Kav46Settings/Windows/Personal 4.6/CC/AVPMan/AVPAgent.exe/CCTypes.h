#ifndef CC_TYPES
#define CC_TYPES

#include "../../../cciface/BaseMarshal.h"
#include "../../../cciface/UserSettings.h"
#include "../../../cciface/TaskStatus.h"
#include "../../../cciface/UserTasks.h"
#include "PatchInfo.h"
#include "task.h"

#ifndef _DATAONLY_
#	include "../../Common/UserTasksIface.h"
#endif

struct CUserTask;
class CSettings;
class CContainerTasks;
class CContainerComponents;
class CTask;

////////////////////////////////////////////////////////////////////////////////////////
// class CTask (base task class)
// Task flags
#define TASK_CID_MASK			0x000000FF
#define TASK_NUMBER_MASK		0x0000FF00

////////////////////////////////////////////////////////////////////////////////////////
// class CContainerTasks
class CContainerTasks : public CContainer
{
public:
	CContainerTasks();
	
	CTask* CreateTask(LPCTSTR szName, DWORD dwType);
	CTask* GetTaskByName(LPCTSTR sTaskName);
	CTask& operator[](int n);

	CTask* CreateUserTask(const CUserTask& usertask, bool bMerged, CSettings& settings);
	bool RemoveUserTask(LPCSTR szTaskName, CExcludeList& excludes);

	CContainerTasks &operator=(const CContainerTasks &that);

	virtual CBase* NewItem(PVOID pData);
};


namespace Private
{
	struct CUpdaterStorage_v1 : CCBaseMarshal<PID_UPDATER_STORAGE>
	{
		unsigned long m_InstallationID;
		unsigned long m_UpdateSessionID;

		CUpdaterStorage_v1()
			: m_InstallationID(1)
			// m_UpdateSessionID не инициализируем, по умолчанию должен быть random
		{
		}
		
		PoliType &MarshalImpl (PoliType &pt)
		{
			return pt << m_InstallationID << m_UpdateSessionID;
		}
	};
	
	struct CKnownExcludedProcessObject_v1 : public CCBaseMarshal<PID_USER_SETTINGS_KNOWN_EXCLUDED_PROCESS_OBJECT>
	{
		CKnownExcludedProcessObject_v1()
		{
			memset(m_ProcessHash, 0, sizeof(m_ProcessHash));
		}
		
		std::wstring m_ProcessPath; //!< Реальный путь к программе
		std::wstring m_OriginalProcessPath; //!< Путь к программе, который первоначального передовался в OAS
		unsigned char m_ProcessHash[16];
		
		bool operator==(const CKnownExcludedProcessObject_v1 &that) const
		{
			return m_ProcessPath == that.m_ProcessPath &&
				m_OriginalProcessPath == that.m_OriginalProcessPath &&
				memcmp(m_ProcessHash, that.m_ProcessHash, 16) == 0;
		}
		bool operator!=(const CKnownExcludedProcessObject_v1 &that) const
		{
			return !operator==(that);
		}
		
		PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl(pt) 
				<< m_ProcessPath
				<< m_OriginalProcessPath
				<< SIMPLE_DATA_SER(m_ProcessHash);
		}
	};
}
typedef DeclareExternalName<Private::CUpdaterStorage_v1> CUpdaterStorage;
typedef DeclareExternalName<Private::CKnownExcludedProcessObject_v1> CKnownExcludedProcessObject;

// class CSettings
////////////////////////////////////////////////////////////////////////////////////////

class CSettings : public CBase
{
public:
	CContainerTasks m_arTasks;

	CUserSettings m_UserSettings;
    CProductStatus m_ProductStatus;
	CPatchList m_PatchList;
	std::vector<BYTE> m_RollbackInfo;
	CUpdaterStorage m_UpdaterStorage;
	CAKStuffStorage m_AKStorage;
	std::vector<CKnownExcludedProcessObject> m_KnownProcesses;

    __int64 m_QuarantineTotalSize;
	__int64 m_BackupTotalSize;

	CSettings();

	virtual int Store(LPPOLICY* ppPolicy) const;
	virtual int Load(LPPOLICY pPolicy);
};

//////////////////////////////////////////////////////////////////////
// CPolicyDat
//////////////////////////////////////////////////////////////////////

class CPolicyDat : public CBase
{
public:
	CPolicyDat();

	CSettings m_Settings;		// настройки
	CPolicy	m_Policy;			// политика

	virtual int Store(LPPOLICY* ppPolicy) const;
	virtual int Load(LPPOLICY pPolicy);
};

#endif // CC_TYPES
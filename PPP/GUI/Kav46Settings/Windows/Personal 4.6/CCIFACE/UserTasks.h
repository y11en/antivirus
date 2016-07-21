#ifndef __USERSETTINGS_H_
#define __USERSETTINGS_H_

#include "DEFS.H"
#include "BaseMarshal.h"
#include "UserSettings.h"

//******************************************************************************
//
//******************************************************************************
// эта структура €вл€етс€ частью CTask, хран€щей данные, относ€щиес€ к UserTask
namespace Private
{
	class CCIFACEEXPORT CUserTaskSettings_v1 :	public CCBaseMarshal<PID_USER_TASK_SETTINGS>
	{
	public:
		CUserTaskSettings_v1(): m_bOnProtectionPage(false) {}
		std::string m_sUserTaskName;
		bool m_bOnProtectionPage;
		
		const CScheduleSettings &GetBaseScheduleSettings() const;
		void SetBaseScheduleSettings(const CScheduleSettings &s);

		const CUserSettingsODS &GetBaseODSSettings() const;
		void SetBaseODSSettings(const CUserSettingsODS &s);

		virtual PoliType &MarshalImpl (PoliType& pt);
	private:
		CScheduleSettings m_ScheduleSettings;
		CUserSettingsODS m_ODSSettings; // базовые настройки без примененной политики
	};

	class CCIFACEEXPORT CUserTaskSettings_v2 :	public DeclareNextVersion<CUserTaskSettings_v1>
	{
	public:
		CUserTaskSettings_v2(): m_bAdminTask(false) {}

		bool m_bAdminTask;

		virtual PoliType &MarshalImpl (PoliType& pt);
	};

	class CCIFACEEXPORT CUpdaterTaskSettings_v1 :	public CCBaseMarshal<PID_UPDATER_TASK_SETTINGS>
	{
	public:
		CUpdaterTaskSettings_v1()
			: m_bOnProtectionPage(true)
			, m_bAdminTask(false)
		{
		}

		std::string m_sUserTaskName;
		bool m_bOnProtectionPage;
		bool m_bAdminTask;

		// Ѕазовые настройки updater (до применени€ политики).
		// ƒл€ админских задач можно мен€ть только из AdminKit
		CUserSettingsUpdater m_BaseUpdaterSettings;

		virtual PoliType &MarshalImpl (PoliType& pt);
	};

	class CCIFACEEXPORT CInstallKeyTaskSettings_v1 : public CCBaseMarshal<PID_INSTALL_KEY_TASK_SETTINGS>
	{
	public:
		CInstallKeyTaskSettings_v1()
			: m_bAdminTask(false)
			, m_bUseSchedule(false)
		{
		}

		std::string m_sUserTaskName;
		bool m_bAdminTask;
		bool m_bUseSchedule; // дл€ хранени€ свойства из AK

		CUserSettingsInstallKey m_InstallKeySettings;

		virtual PoliType &MarshalImpl (PoliType& pt);
	};

	class CCIFACEEXPORT CRollbackTaskSettings_v1 : public CCBaseMarshal<PID_ROLLBACK_TASK_SETTINGS>
	{
	public:
		CRollbackTaskSettings_v1()
			: m_bAdminTask(false)
			, m_bOnProtectionPage(false)
			, m_bUseSchedule(false)
		{
		}

		std::string m_sUserTaskName;
		bool m_bOnProtectionPage;
		bool m_bAdminTask;
		bool m_bUseSchedule; // дл€ хранени€ свойства из AK

		virtual PoliType &MarshalImpl (PoliType& pt);
	};

} // namespace Private

typedef DeclareExternalName<Private::CUserTaskSettings_v2> CUserTaskSettings;
typedef DeclareExternalName<Private::CUpdaterTaskSettings_v1> CUpdaterTaskSettings;
typedef DeclareExternalName<Private::CInstallKeyTaskSettings_v1> CInstallKeyTaskSettings;
typedef DeclareExternalName<Private::CRollbackTaskSettings_v1> CRollbackTaskSettings;

//////////////////////////////////////////////////////////////////////////

#endif
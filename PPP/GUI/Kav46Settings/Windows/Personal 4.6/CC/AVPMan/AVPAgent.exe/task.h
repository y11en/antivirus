#ifndef _TASK_H__INCLUDED_
#define _TASK_H__INCLUDED_

#include "../../../cciface/BaseMarshal.h"
#include "../../../cciface/UserSettings.h"
#include "../../../cciface/UserTasks.h"
#include "../../../cciface/ScheduleHelper.h"

#ifndef _DATAONLY_
#	include "../../Common/UserTasksIface.h"
#	include "netdetect.h"
#endif

struct CUserTask;
class CSettings;

//////////////////////////////////////////////////////////////////////////

struct CTaskStartSettings_v1 : CCBaseMarshal<8365>
{
	// настройки, переданные при старте задачи из GUI
	std::vector<BYTE> m_Param;
	std::vector<BYTE> m_AcceptableEvents;

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		return BaseMarshal::MarshalImpl (pt) << m_Param << m_AcceptableEvents;
	}
};

typedef DeclareExternalName<CTaskStartSettings_v1> CTaskStartSettings;

//////////////////////////////////////////////////////////////////////////

// надстройка над CUserTaskSettings
class CTaskUserTaskSettings : public CUserTaskSettings
{
public:
	// возвращает настройки с примененной политикой
	CUserSettingsODS GetMergedUserSettingsODS() const;
	// устанавливает настройки с учетом политики
	void SetMergedUserSettingsODS(const CUserSettingsODS &settings);
	// возвращает расписание с примененной политикой
	CScheduleSettings GetMergedScheduleSettings() const;
	// устанавливает расписание с учетом политики
	void SetMergedScheduleSettings(const CScheduleSettings &schedule);
	// устанавливает m_UserTaskSettings.m_ODSSettings.m_blOkInfoEventsOff, m_eUseIChecker, m_eUseIStreams
	void SetWriteAllEventsInUserSettingsODS(bool bWriteAllEvents);
	void SetICheckerUsageInUserSettingsODS(EICheckerType type);
	void SetIStreamsUsageInUserSettingsODS(EICheckerType type);
};

class CTaskUpdaterTaskSettings : public CUpdaterTaskSettings
{
public:
	// возвращает настройки с примененной политикой
	CUserSettingsUpdater GetMergedUserSettingsUpdater() const;
	// устанавливает настройки с учетом политики
	void SetMergedUserSettingsUpdater(const CUserSettingsUpdater &settings);
	// устанавливает m_UserTaskSettings.m_ODSSettings.m_blOkInfoEventsOff
	void SetWriteAllEventsInUserSettingsUpdater(bool bWriteAllEvents);
};

class CTaskBase : public CBase
{
public:
	CTaskBase (LPCTSTR szName, DWORD dwType);

	// флаг постоянной задачи (передается в RunningTask и означает 'не удалять из списка при завершении')
	BOOL m_bPermanent;
	
	// logon-параметры запуска
	CLogonCredentials m_LogonCredentials;

	// требуется ли неявная имперсонация на UI (если он есть)
	// в случае если другими способами имперсонироваться не удается (user+pass или thread token)
	bool m_bImplicitImpersonationDeprecated;

	// настройки задачи
	CTaskStartSettings m_StartSettings;

	// настройки для ODS User task
	enum ETaskType
	{
		eTask = 0,
		eUserTask = 1,
		eUpdaterTask = 2,
		eInstallKeyTask = 3,
		eRollbackTask = 4
	};
	ETaskType m_eTaskType;
	// настройки ODS задачи
	CTaskUserTaskSettings m_UserTaskSettings;
	CTaskUpdaterTaskSettings m_UpdaterTaskSettings;
	CInstallKeyTaskSettings m_InstallKeyTaskSettings;
	CRollbackTaskSettings m_RollbackTaskSettings;

	virtual int Store(LPPOLICY* ppPolicy) const;
	virtual int Load(LPPOLICY pPolicy);

protected:
	// запускать ли проверку просроченного старта
	bool m_bDeferredSchedule;
	unsigned m_nMaxRunningTimeMs;
	// параметры запуска по расписанию
	std::vector<CSchedulePoint> m_vecSchedule;
	__int64 m_tLastChecked;
	__int64 m_tLastTaskExecTimeBySchedule;

	// спрашивать пользователя перед запуском
	bool m_bAskUser;
	
	// запускать ли при старте
	bool m_bRunOnStartup;
	// задержка при старте (необходимо на 9x)
	DWORD m_nStartDelay;
	//! Дата создания задачи
	KLUTIL::CSysTime m_dtCreationDate;
	//! Время последнего успешного завершения задачи
	KLUTIL::CSysTime m_dtLastSuccessfulFinish;

	// Время следующего отложенного запуска
	__int64 m_tDeferred;
	
	// сериализованная ветка TASK_INFO_PARAMS\KLPRSS_EVPNotifications для WKS
	std::vector<BYTE> m_TaskNotificationDescriptionBlob;
};

#ifndef _DATAONLY_

class CTask : public CTaskBase
{
public:
	CTask ();
	CTask (LPCTSTR szName, DWORD dwType);
	// Для только что загруженных CTask надо вызывать ResetLastCheck(). (Его невызов грозит запуском по расписанию просроченной задачи, для которой просроченные запуски отменяются).
	void ResetLastCheck ();

	std::string GetExeName() const;

	// Checks happenning of the event
	bool IsTimeForTask();
	bool IsWaitingInternet() const { return m_bWaitingInternet; }

	bool IsNeedAskUser() const;

	bool IsStartupTask() const;
	DWORD GetStartupDelay() const;
	void SetStartup(bool bStartup);

	void SetDeferredTime(const KLUTIL::CSysTime& dtDeferred);
	void ClearDeferredTime();

	void TaskExecutedByScheduler();
	void SetBaseSchedule(const CScheduleSettings& ScheduleSettings);
	__int64 GetLastTaskExecTimeBySchedule() const { return m_tLastTaskExecTimeBySchedule; }
	__int64 GetBaseTimeForNextStart() const;
	KLUTIL::CSysTime GetNextTaskStart() const;
	__int64 GetNextTaskStartI64() const;

	unsigned GetMaxRunningTimeMs() const { return m_nMaxRunningTimeMs; }
	
	//! Приостанавливает старт задачи из scheduler.
	void PauseScheduleStart();
	//! Возобновляет возможность старта задачи из scheduler.
	void UnpauseScheduleStart();

	CUserTask GetUserTask(bool bMerged) const;
	void SetUserTask(const CUserTask& usertask, bool bMerged, CSettings& settings);
	bool IsAdminTask() const;

	//! Присваивает задаче ГУЁвое имя, если оно не задано. 
	void CheckUserName();

	//! Возвращает ГУЁвое имя по умолчанию.
	std::string GetDefaultUserName() const;

	//! Возвращает заданное задаче ГУЁвое имя.
	std::string GetUserName() const;

	//! Обработчик успешного завершения задачи.
	void OnSuccessfulyFinished();

private:
	// Возвращает время следующего запуска с учетом отложенной задачи
	__int64 GetNextStartI64(__int64 ftLastExecute, const std::vector<CSchedulePoint> &Schedule, bool& bNeedInternet) const;

	//! Подписчик на изменение состояние подключения к Интернету.
	Notifier::CMethodSubscriberT<ENetDetectEvent, CTask> m_InetDetectSubscriber;
	//! Событие "состояние сети", "состояние подключения к Интернету" (для WKS)
	Notifier::CEvent m_NetStateEvent, m_InternetStateEvent;
	//! Callback для NetDetect.
	void OnInternetStateChanged(const ENetDetectEvent& ev);
	//! Состояние интернета согласно NetDetect.
	bool m_bInternetIsOnline;
	//! Флаг ожидания интернета. true - для запуска задачи нужен интернет.
	bool m_bWaitingInternet;

	// проверять ли расписание
	bool m_bPauseSchedule;
};

#endif // _DATAONLY_

#endif // _TASK_H__INCLUDED_
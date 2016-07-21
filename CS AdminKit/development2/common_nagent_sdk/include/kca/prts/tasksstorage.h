/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRTS/TasksStorage.h
 * \author Андрей Брыксин
 * \date 2002
 * \brief Интерфейс для работы со списоком задач, запланированных к выполнению для
 *        компонентов продуктов.
 *
 */

#ifndef KLPRTS_TASKSSTORAGE_H
#define KLPRTS_TASKSSTORAGE_H


#include <std/err/error.h>
#include <std/par/params.h>


#include <std/sch/task.h>

namespace KLPRTS
{
	/*!
		Имя типа для задачи Updater для __ВСЕХ__ продуктов
	*/
	const wchar_t PRTS_PRODUCT_UPDATER_TASK_TYPE_NAME[] = L"KLUPD_TA_UPDATE_TASK";

	/*!
		Специализированные константы для полей в контейнере 'params' настроек задачи
		для управления поведением задачи
	*/
	const wchar_t c_szwTaskSystemFlag[]=L"klprts-TaskSystemFlag";	// BOOL_T Flag for system task

	const wchar_t c_szwTaskSystemEnableFlag[]=L"klprts-TaskEnableSystemFlag";	// BOOL_T если задача системная то данный параметр разрешает запуск задачи по расписанию

	// Данные атрибуты добавляются в контейнер 'params' настроек задачи в TaskStorage.

	/*!
		Поля добавляемые в Params задач при их запуске
	*/
	const wchar_t c_szwTaskScheduledFlag[]=L"klprts-TaskScheduledFlag";	// BOOL_T Flag for task run by scheduler

	/*!
		Поля добавляемые в Params задач при их запуске
	*/
	const wchar_t c_szwTaskPreparativeStartFlag[]=L"klprts-TaskPreparativeStartFlag";	// BOOL_T Flag for preparative start of task
	
	/*!
		Поля добавляемые в Params задач при их запуске
	*/
	const wchar_t c_szwTaskPreparativeTimeout[]=L"klprts-TaskPreparativeTimeout";	// INT_T Perparative task timeout. exists only if it's preparative start

	/*!
		Поля добавляемые в Params задач при их запуске
		Если данного поля нет в параметрах запуска, то для данной задачи пользователь и пароль не заданы
		иначе результат операции LogonUser
	*/
	const wchar_t c_szwTaskLogonUserResult[]=L"klprts-TaskLogonUserResult";	// BOOL_T result of LogonUser

	/*!
		Флаг взводится при создании задачи и означает, что список целевых компьютеров для задачи не должен изменятся
	*/
	const wchar_t c_szwTaskHasPredefinedTargetList[]=L"klprts-TaskHasPredefinedTargetList";	// BOOL_T

	/*!
		Поля добавляемые в Params задач при их запуске
	*/
	const wchar_t c_szwTaskParamTaskInfo[]=L"klprts-TaskParamTaskInfo";	// PARAMS_T TaskInfo запущенной задачи

	/*!
		Идентификатор задачи в TaskStorage.
	*/
	const wchar_t	c_szwTaskStorageId[]=L"klprts-TaskStorageId";// STRING_T

    /*!
    * \brief INT_T - задержка старта задачи (в минутах) с расписанием KLPRTS_TSKSCHTYPE_STARTUP_EX
					 после старта приложения
    */
	const wchar_t c_szwDelayOnAppTaskStart[] = L"klprts-DelayOnAppTaskStart";

    /*!
    * \brief PARAMS_T - Настройка запуска задачи по собыию
			KLPRTS_START_CONDITION
				KLPRTS_START_CONDITION_TYPE
				KLPRTS_START_CONDITION_VALUE
    */
	enum ETskStartCondition{
		TSC_NONE,
		TSC_AV_DB_UPDATED,
		TSC_VIRUS_ATTACK,
		TSC_TASK_COMPLETED
	};
	const wchar_t KLPRTS_START_CONDITION[] = L"KLPRTS_START_CONDITION";				// PARAMS_T
	const wchar_t KLPRTS_START_CONDITION_TYPE[] = L"KLPRTS_START_CONDITION_TYPE";	// INT_T (см. ETskStartCondition)
	/*!
    * \brief  зависит от типа условия
		TSC_AV_DB_UPDATED : отсутствует
		TSC_VIRUS_ATTACK : INT_T (см. KLEVP::EVirusOutbreakType)
		TSC_TASK_COMPLETED : STRING_T (task_id групповой/глобальной задачи, по завершении который следует запустить данную)
	*/
	const wchar_t KLPRTS_START_CONDITION_VALUE[] = L"KLPRTS_START_CONDITION_VALUE";

	/*!
    * \brief  зависит от типа условия
		TSC_AV_DB_UPDATED : отсутствует
		TSC_VIRUS_ATTACK : отсутствует
		TSC_TASK_COMPLETED : INT_T (KLPRTS::TaskState - TASK_COMPLETED или TASK_FAILURE)
	*/
	const wchar_t KLPRTS_START_CONDITION_VALUE_EX[] = L"KLPRTS_START_CONDITION_VALUE_EX";

	/*!
		Учетная запись для запуска задачи.
	*/
	const wchar_t c_szwTaskAccounts[]=L"klprts-TaskAccounts";				// ARRAY_T Account: массив учетных записей для задачи (если поддерживается задачей)
	const wchar_t c_szwTaskAccountUser[]=L"klprts-TaskAccountUser";			// STRING_T Account: user name
	const wchar_t c_szwTaskAccountPassword[]=L"klprts-TaskAccountPassword";	// BYNARY_T Account: encrypted password

	// Данные атрибуты добавляются в контейнер 'paramsTaskInfo' настроек задачи в TaskStorage.

	const wchar_t c_szwWakeOnLAN[]=L"klprts-WakeOnLAN";	// INT_T Число милисекунд перед запуском задачи для пробуждения компьютера по сети

	const wchar_t c_szwTaskNotificationOverall[]=L"klprts-TaskNotificationOverall";	// BOOL_T Notification Overall
	const wchar_t c_szwTaskNotificationFailure[]=L"klprts-TaskNotificationFailure";	// BOOL_T Notification failure
	const wchar_t c_szwTaskNotificationAllResults[]=L"klprts-TaskNotificationAllResults";	// BOOL_T Notification All Results

//	const wchar_t c_szwTaskSaveResults[]=L"klprts-TaskSaveResults";// BOOL_T Save task results
	const wchar_t c_szwTaskSaveResultsLifespan[]=L"klprts-TaskSaveResultsLifespan";	// INT_T(days) Save Results Lifespan
	const wchar_t c_szwTaskSaveResultsOnClient[]=L"klprts-TaskSaveResultsOnClient";	// BOOL_T(days) Save Results on client
	const wchar_t c_szwTaskSaveResultsOnClientEventLog[]=L"klprts-TaskSaveResultsOnClientEventLog";	// BOOL_T(days) Save Results on client EventLog
	const wchar_t c_szwTaskSaveResultsOnServerEventLog[]=L"klprts-TaskSaveResultsOnServerEventLog";	// BOOL_T(days) Save Results on server EventLog

	const wchar_t c_szwTaskMaxRuningTime[]=L"klprts-TaskMaxRunningTime";	// INT_T (secs)
	const wchar_t c_szwScheduleSubtype[] = L"klprts-TaskScheduleSubtype";	// INT_T тип выбранного расписания для задачи (см. KLPRTS_TSKSCHTYPE_XXXX)

	/*!
    * \brief  c_szwScheduleSubtypeEx - INT_T расширенный тип выбранного расписания для задачи
	*	Может содержать:
	*	KLPRTS_TSKSCHTYPE_START_AFTER_SERVER_UPDATE
	*	KLPRTS_TSKSCHTYPE_ON_VIRUS_OUTBREAK
	*	KLPRTS_TSKSCHTYPE_ON_OTHER_TASK_END
	* !! при этом в c_szwScheduleSubtype для совместимости прописывается тип KLPRTS_TSKSCHTYPE_MANUALLY !!
	*/
	const wchar_t c_szwScheduleSubtypeEx[] = L"klprts-TaskScheduleSubtypeEx";	

	const wchar_t c_szwShutdownSystemAfterRun[]=L"klprts-ShutdownSystemAfterRun";	// BOOL_T признак необходимости выключения компьютера сразу после выполнения задачи

	typedef enum _tagKLPRTS_TSKSCHTYPE{
		KLPRTS_TSKSCHTYPE_NHOURS = 0x01,		// Каждые N - часов, начиная с (дата)
		KLPRTS_TSKSCHTYPE_NDAYS = 0x02,			// Каждые N - дней, начиная с (дата)
		KLPRTS_TSKSCHTYPE_NWEEKS = 0x04,		// Каждые N - недель, начиная с (дата)
		KLPRTS_TSKSCHTYPE_DAYLY = 0x08,
		KLPRTS_TSKSCHTYPE_WEEKLY = 0x10,
		KLPRTS_TSKSCHTYPE_MONTHLY = 0x20,
		KLPRTS_TSKSCHTYPE_ONCE = 0x40,
		KLPRTS_TSKSCHTYPE_STARTUP = 0x80,
		KLPRTS_TSKSCHTYPE_MANUALLY = 0x100,
		KLPRTS_TSKSCHTYPE_IMMEDIATELY = 0x200,
		KLPRTS_TSKSCHTYPE_EVERY_N_MINUTES = 0x400,
		KLPRTS_TSKSCHTYPE_START_AFTER_UPDATE = 0x800,
		KLPRTS_TSKSCHTYPE_STARTUP_EX = 0x1000,
		KLPRTS_TSKSCHTYPE_AUTOMATIC = 0x2000,
		KLPRTS_TSKSCHTYPE_START_AFTER_SERVER_UPDATE = 0x4000,	// see KLTSK::ETskStartCondition
		KLPRTS_TSKSCHTYPE_ON_VIRUS_OUTBREAK = 0x8000,			// see KLTSK::ETskStartCondition
		KLPRTS_TSKSCHTYPE_ON_OTHER_TASK_END = 0x10000,
		KLPRTS_TSKSCHTYPE_WEEKLY_EX = 0x20000
	} KLPRTS_TSKSCHTYPE;

	const long KLPRTS_TSKSCHTYPE_DEFAULT =	KLPRTS_TSKSCHTYPE_NHOURS |
											KLPRTS_TSKSCHTYPE_DAYLY | 
											KLPRTS_TSKSCHTYPE_WEEKLY | 
											KLPRTS_TSKSCHTYPE_MONTHLY | 
											KLPRTS_TSKSCHTYPE_ONCE | 
//											KLPRTS_TSKSCHTYPE_STARTUP | 
											KLPRTS_TSKSCHTYPE_MANUALLY |
											KLPRTS_TSKSCHTYPE_IMMEDIATELY |
											KLPRTS_TSKSCHTYPE_START_AFTER_SERVER_UPDATE |
											KLPRTS_TSKSCHTYPE_ON_VIRUS_OUTBREAK |
											KLPRTS_TSKSCHTYPE_ON_OTHER_TASK_END;

    /*!
    * \brief BOOL_T - данная задаче не может быть удалена средствами консоли Admin Kit
    */
	const wchar_t c_szwTaskCannotBeDeleted[] = L"klprts-TaskCannotBeDeleted";

    /*!
    * \brief BOOL_T - данная задача не должна распространяться на подчиненные сервера
    */
	const wchar_t c_szwDontApplyToSlaveServers[] = L"klprts-DontApplyToSlaveServers";

    /*!
    * \brief Идентификатор задачи не задан
    */
	const wchar_t KLPRTS_TASK_INVALID_ID[] = L"";
	const wchar_t KLPRTS_LOCAL_TASK_ATTRIBUTE[] = L"_LOCAL_";

    /*!
    * \brief Имена хранилищ задач
    *
    */
	const wchar_t TASK_STORAGE_NAME_LOCAL[] = L"PRTS_TASK_STORAGE_LOCAL";	// Локальное хранилище

    /*!
    * \brief Имя параметра, содержащего display name для task'a в
            контейнере Params* paramsTaskInfo (см. метод AddTask)
    *
    */
    const wchar_t TASK_DISPLAY_NAME[] = L"DisplayName";			// STRING_T

    /*!
    * \brief Имя параметра, содержащего флаг Enabled для задачи в
            контейнере Params* parTaskInfo
    *
    */
    const wchar_t TASK_ENABLED_FLAG[] = L"PRTS_TASK_ENABLED";	// BOOL_T

    /*!
    * \brief Имя параметра, содержащего тип выбора целевых компьютеров для задачи в
            контейнере Params* parTaskInfo (см. TCT_XXXX)
    *
    */
    const wchar_t TASK_TARGET_COMPUTERS_TYPE[] = L"PRTS_TASK_TARGET_COMPUTERS_TYPE";	// INT_T
	typedef enum _tagTCT{
		TCT_WINDOWS_NETWORKING,
		TCT_IP_NETWORKING
	} TCT;

    /*!
    * \brief Имя параметра, содержащего ID группы в которой задача определена в
            контейнере Params* parTaskInfo(только для групповых задач)
    *
    */
    const wchar_t TASK_GROUPID_FOR_GROUP_TASKS[] = L"PRTS_TASK_GROUPID";	// INT_T

    /*!
    * \brief Имя параметра, содержащего время создания задачи в
            контейнере Params* parTaskInfo
    *
    */
    const wchar_t TASK_CREATION_DATE[] = L"PRTS_TASK_CREATION_DATE";	// DATETIME_T

    /*!
    * \brief Имена параметров в EventBody
    *
    */

	const wchar_t EVENT_BODY_PARAM_NAME_TASKID[] = L"PRTS_TaskID";		// [int] ID задачи, вызвавшей генерацию события

    /*!
    * \brief Генерируемые события
    */

	const wchar_t EVENT_TYPE_NEW_TASK[] = L"PRTS_NewTask";				// Создана новая задача
	const wchar_t EVENT_TYPE_TASK_DELETED[] = L"PRTS_TaskDeleted";		// Задача удалена
	const wchar_t EVENT_TYPE_TASK_UPDATED[] = L"PRTS_TaskUpdated";		// Изменены параметры задачи

    /*!
    * \brief Типы Eventов запуска задачи
    */

	const wchar_t PRTS_TASK_START_EVENT_NONE[] = L"PRTS_EVENT_NONE";	// Не задан Event запуска задачи

}


#endif // KLPRTS_TASKSSTORAGE_H


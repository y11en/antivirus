/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file conn/task_settings_synchronizer.h
 * \author Андрей Казачков
 * \date 2004
 * \brief Интерфейс, отвечающий за синхронизацию настроек задач
 *        приложения с настройками задач приложения для системы
 *        администрирования.
 *
 */

#ifndef KLCONN_TASK_SETTINGS_SYNCHRONIZER_H
#define KLCONN_TASK_SETTINGS_SYNCHRONIZER_H

#include "./conn_types.h"
#include "./settings_synchronizer_base.h"
#include <std/sch/task.h>


namespace KLCONN
{
    //! Application must save this parametr
    const wchar_t c_szwPar_GroupTaskFlag[] = L"PRTS_TASK_GROUPID";	// INT_T

    /*!
     \brief Интерфейс, отвечающий за синхронизацию настроек задач
            приложения с настройками задач приложения для системы
            администрирования.

        Интерфейс реализуется библиотекой интеграции.
        Интерфейс используется системой администрирования.

        Вызовы методов интерфейса могут производится на разных нитях, но при
        этом сериализуются вызывающей стороной.
    */

    /*
        Some special variables:

        Login/Password for task start.
            Use function KLCSPWD::UnprotectData to decrypt password.
            Use function KLCSPWD::ProtectDataLocally to encrypt
            password for local tasks.
            These variables are put into the pParams container.

            KLPRTS::c_szwTaskAccountUser        login (STRING_T)
	        KLPRTS::c_szwTaskAccountPassword    encrypted password (BINARY_T)

    */

    class KLSTD_NOVTABLE TaskSettingsSynchronizer : public SettingsSynchronizerBase
    {
    public:

    /*!
      \brief Возвращает список идентификаторов задач приложения.
            В случае ошибки выбрасывается исключение типа KLERR::Error*

	  \param vecTasks [out]  Список имеющихся у приложения идентификаторов задач.
                             Возвращаемую память освобождает вызывающая сторона.

    */
        virtual void GetTasksList (KLSTD::AKWSTRARR& vecTasks)  = 0;


    /*!
      \brief Возвращает описание задачи с заданным ID из списка
        запланированных к выполнению компонентами продуктов задач.
             В случае ошибки выбрасывается исключение типа KLERR::Error*

        \param szwTaskId        [in]	Идентификатор задачи.
        \param wstrTaskType     [out]	Тип задачи.
        \param ppTaskSch        [out]	Расписание задачи.
		\param ppParams         [out]	Параметры задачи
		\param ppParamsTaskInfo [out]	Дополнительные параметры задачи

		Если задача с указанным ID не найдена то будет выброшено исключение KLSTD::STDE_NOTFOUND
    */
        virtual void GetTask(
						const wchar_t*  szwTaskId,
                        KLSTD::AKWSTR&  wstrTaskType,
                        KLSCH::Task**   ppTaskSch,
						KLPAR::Params** ppParams,
						KLPAR::Params** ppParamsTaskInfo) = 0;



    /*!
      \brief Добавляет новую задачу для приложения. Если задача уже
            существует, то ее настройки заменяются на указанные.
            В случае ошибки выбрасывается исключение типа KLERR::Error*
            Для того, чтобы групповые задачи могли быть удалены после
            переустановки сетевого агента, параметр c_szwPar_GroupTaskFlag
            из pParamsTaskInfo должен сохраняться на стороне приложения.


		\param szwTaskId        [in]  Идентификатор задачи для обновления.
        \param szwTaskType      [in]  Тип задачи.
        \param pTaskSch         [in]  Новое расписание запланированной к выполнению задачи.
		\param pParams          [in]  Параметры задачи
		\param pParamsTaskInfo  [in]  Дополнительные параметры задачи
        \param ppParamsUnsuported [out] неподдерживаемые переменные, т.е.
                неизвестные коннектору переменные из pParams, которые не
                могут быть им коннектором сохранены.

        \param ppTaskInfoUnsuported [out] неподдерживаемые переменные, т.е.
                неизвестные коннектору переменные из pParamsUnsuported,
                которые не могут быть им коннектором сохранены.
    */
        virtual void PutTask (
			const wchar_t*          szwTaskId,
            const wchar_t*          szwTaskType,
            const KLSCH::Task*      pTaskSch,
		    const KLPAR::Params*    pParams,
		    const KLPAR::Params*    pParamsTaskInfo,
                  KLPAR::Params**   ppParamsUnsuported,
                  KLPAR::Params**   ppTaskInfoUnsuported) = 0;

    /*!
      \brief Удаляет задачу с заданным идентификатором.
        В случае ошибки выбрасывается исключение типа KLERR::Error*

		\param taskId		[in]	Идентификатор задачи для удаления.

		\return false - если задачи с заданным идентификатором не
            существует
        \return true - задача была успешно удалена

    */
        virtual bool DeleteTask (
			const wchar_t*  szwTaskId )  = 0;

    /*!
      \brief Создает новый идентификатор для задачи. Возвращаемую память
            освобождает вызывающая сторона.

    */
        virtual KLSTD::AKWSTR CreateNewTaskId() = 0;
    };

    //!Container in pParamsTaskInfo that contains information about task start event, PARAMS_T
    const wchar_t  c_szwTaskStartEvent[] = L"KLCONN_TASK_START_EVENT";
}


#endif // KLCONN_TASK_SETTINGS_SYNCHRONIZER_H



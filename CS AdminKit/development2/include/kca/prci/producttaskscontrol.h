/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRCI/ProductTasks.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Интерфейс для выполнения компонентом продукта административных задач.
 *
 */

#ifndef KLPRCI_PRODUCTTASKSCONTROL_H
#define KLPRCI_PRODUCTTASKSCONTROL_H

#include <string>

#include "prci_const.h"
#include "std/err/error.h"
#include "std/par/params.h"

namespace KLPRCI {


    /*!
		\brief Callback - функция для выполнения компонентом административных задач.  
                Реализация этой функции должна быть потокобезопасной.

        Компонент должен принять на выполнение задачу типа taskName с параметрами params
        и идентификатором taskId для вновь заведенной задачи.

        Модель выполнения задачи - асинхронная.  Компонент принимает задачу на выполнение
        и после этого нотифицирует о статусе выполнения задачи при помощи событий.
        Создаются события следующего типа:
            c_EventTaskState
            c_EventTaskCompletion
            c_EventTaskStarted
         
        \param taskName       [in]  Имя выполняемой задачи.
        \param params         [in]  Параметры для выполнения задачи.
								К параметрам выполнения задачи библиотека
								добавляет переменную с именем, заданным
								константой  c_szwTaskAsyncId, типа STRING_T,
								содержащую значение параметра asyncId метода
								ComponentProxy::RunTask.
        \param taskId         [in]  Идентификатор запущенной задачи.
        \param timeout        [in]  Максимальное время выполнения задачи (msec).

        \return  Идентификатор запущенной задачи. Уникален для всех задач, 
                 выполняемых компонентом.

	*/
    typedef void  (*RunTasksCallback)(
					void*					context,
					const std::wstring&		taskName,
							KLPAR::Params*	params,
					long					taskId,
					long					timeout) ;

        //! Возможное состояние задачи. 
    enum TaskState {
        TASK_CREATED,   ///< Задача только что создана.
        TASK_RUNNING,   ///< Задача выполняется.
        TASK_SUSPENDED, ///< Задача приостановила работу.
        TASK_FAILURE,   ///< Фатальная ошибка при выполнении задачи.
        TASK_COMPLETED, ///< Задача завершила свою работу.
        TASK_TIMEOUT,   ///< Задача выполняется больше, чем положено.
        TASK_RELOADING,
        TASK_INACTIVE
    };


        //! Возможные действия над выполняемой задачей. 
    enum TaskAction {
        ACTION_STOP,      ///< Прекратить выполнение задачи.
        ACTION_SUSPEND,   ///< Приостановить выполнение задачи.
        ACTION_RESUME,    ///< Возобновить выполнение задачи.
        ACTION_STATUS,    ///< Указать текущий статус задачи.
        /*! Перезагрузка настроек задачи. Если задача не поддерживает
            динамическую перезагрузку настроек, она должна кинуть исключение.
            Новые настройки будут доступны в качестве натроек задачи только
            после успешного выхода из колбэка.
            Внутри колбэка для получения новых настроек следует вызывать
            метод GetTaskParams.
        */
        ACTION_RELOAD,   ///< Перезагрузить настройки
        ACTION_START
    };


    /*!
		\brief Callback - функция для контроля над выполняемыми компонентом 
               административными задачами.Реализация этой функции должна быть 
               потокобезопасной.

        \param taskId       [in]  Идентификатор запущенной задачи.
        \param action       [in]  Тип действия, который требуется выполнить над задачей.
	*/
    typedef void (*TasksControlCallback)(
					void*		context,
					long		taskId,
					TaskAction	action);

    /*!
    * \brief Интерфейс для выполнения компонентом продукта административных задач.
    */


    class KLSTD_NOVTABLE ProductTasksControl: public KLSTD::KLBaseQI
    {
    public:

    /*!
      \brief Устанавливает текущее состояние задачи.

      \param taskId       [in] Идентификатор запущенной задачи.
      \param state        [in] Текущее состояние задачи.
	  \param results      [in] Дополнительные параметры, передаваемые в параметре 
                               KLPRCI::c_evpTaskResults генерируемого события 
							   о состоянии задачи (KLPRCI::c_EventTaskState).
                               Помимо прочих дополнительных параметров, может содержать 
							   стандартные параметры события, которые впоследстие будут 
							   трактоваться как параметры соответствующего события 
							   KLPRCI::c_EventTaskState:

                               - KLEVP::c_er_descr – локализованное продуктом описание 
                               результата выполнения задачи (если оно необходимо – 
                               например, в случае возникновения ошибки и перевода задачи 
                               в состояние KLPRCI::TASK_FAILURE);

                               - KLEVP::c_er_locid – идентификатор локализации, используемый 
                               для локализации события в консоли администрирования.

                               - KLEVP::c_er_par1, ...  KLEVP::c_er_par6 – дополнительные 
                               параметры, используемые при локализации; их конкретное 
							   использование зависит от идентификатора локализации.

                               - KLEVP::c_er_completedPercent – опциональный дополнительный 
                               параметр, используемый для отображения процента выполнения 
                               задачи, в случае, если ее выполнение можно разбить на 
                               несколько этапов, и каждый из этапов сопровождать 
                               публикацией соответствующего события KLPRCI::TASK_RUNNING.
    */

        virtual void SetState(long taskId, TaskState state, const KLPAR::Params * results = 0)  = 0;


    /*!
      \brief Устанавливает процент завершения задачи.

      \param taskId       [in]  Идентификатор запущенной задачи.
      \param percent      [in]  Процент выполнения задачи (0-100).
 
    */

        virtual void SetTaskCompletion(long taskId, int percent)  = 0;


    /*!
		\brief Устанавливает callback-функцию для запуска задач, выполняемых компонентом.

        \param callback        [in]  Callback - функция для запуска задач.
	*/

       virtual void SetRunTasksCallback( 
						void*				context,
                        RunTasksCallback	callback )  = 0;

    /*!
		\brief Устанавливает callback-функцию для контроля задач, выполняемых компонентом.

        \param callback        [in]  Callback - функция для контроля задач.
	*/

       virtual void SetTasksControlCallback(
						void*					context,
                        TasksControlCallback	callback )  = 0;

	   virtual long CallRunTasksCallback(
						const std::wstring & taskName,
						const KLPAR::Params * params,
						const std::wstring& asyncId=L"",
						long timeout=0) = 0;

	   virtual void* SetTaskData(long taskId, void* pData) = 0;

       virtual void GetTaskParams(long idTask, KLPAR::Params** pParams) = 0;
      };
}

#endif // KLPRCI_PRODUCTTASKSCONTROL_H

// Local Variables:
// mode: C++
// End:

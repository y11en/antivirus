/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file conn/tasks_synchronizer.h
 * \author Андрей Казачков
 * \date 2004
 * \brief Интерфейс, отвечающий за синхронизацию состояния задач приложения
 *        с состоянием задач в системе администрирования. 
 *
 */

#ifndef KLCONN_TASKS_SYNCHRONIZER_H
#define KLCONN_TASKS_SYNCHRONIZER_H

#include "./conn_types.h"

namespace KLCONN
{
    typedef enum
    {
        TSK_ACTION_START    =   5, //KLPRCI::ACTION_START,
        TSK_ACTION_STOP     =   0, //KLPRCI::ACTION_STOP,
        TSK_ACTION_SUSPEND  =   1, //KLPRCI::ACTION_SUSPEND,
        TSK_ACTION_RESUME   =   2  //KLPRCI::ACTION_RESUME
    }TskAction;

    typedef enum
    {
        TSK_STATE_INACTIVE  = 7, //KLPRCI::TASK_INACTIVE,
        TSK_STATE_RUNNING   = 1, //KLPRCI::TASK_RUNNING,
        TSK_STATE_SUSPENDED = 2, //KLPRCI::TASK_SUSPENDED,
        TSK_STATE_COMPLETED = 4, //KLPRCI::TASK_COMPLETED,
        TSK_STATE_FAILED    = 3  //KLPRCI::TASK_FAILURE
    }TskState;

    /*!
     \brief Интерфейс, отвечающий за синхронизацию состояния задач приложения
	        с состоянием задач в системе администрирования.

        Интерфейс реализуется библиотекой интеграции.
        Интерфейс используется системой администрирования.

        Вызовы методов интерфейса могут производится на разных нитях, но при
        этом сериализуются вызывающей стороной.
    */

    class KLSTD_NOVTABLE TasksSynchronizer : public KLSTD::KLBaseQI
    {
    public:
    /*!
      \brief Возвращает текущий статус задачи.

        \param szwTaskId    [in]	Идентификатор задачи для получения статуса.
        \param nState	    [out]	Текущий статус задачи.
        \param ppResult     [out]   дополнительные параметры.
        \param tmRiseTime   [out]   время публикации события в UTC.

        \exception KLSTD::STDE_NOTFOUND - задача с указанным
            идентификатором не найдена
    */
        virtual void GetTaskStatus(
                        const wchar_t*          szwTaskId, 
                        KLCONN::TskState&       nState, 
                        KLPAR::Params**         ppResult,
                        KLCONN::raise_time_t&   tmRiseTime) = 0;
    /*!
      \brief Возвращает процент выполнения задачи.

        \param szwTaskId    [in]	Идентификатор задачи для получения статуса.
        \param nPercent	    [out]	Текущий процент выполнения задачи.
        \param tmRiseTime   [out]   время публикации события в UTC.

        \exception KLSTD::STDE_NOTFOUND - задача с указанным
            идентификатором не найдена
        \exception KLSTD::STDE_NOFUNC - задача не имеет процента выполнения
    */
        virtual void GetTaskCompletion(
                        const wchar_t*          szwTaskId,
                        int&                    nPercent,
                        KLCONN::raise_time_t&   tmRiseTime) = 0;

    /*!
      \brief Посылает задаче команду.

		\param wstrId   [in]    Идентификатор задачи
		\param nAction  [in]    Действие, которое требуется выполнить для задачи.
        \param pData    [in]    Дополнительные параметры.

		\exception KLSTD::STDE_NOTFOUND - задача с указанным
            идентификатором не найдена
        \exception KLSTD::STDE_NOFUNC - задача не поддерживает указанное
            действие
    */
        virtual void SendTaskAction(
                        const wchar_t*      szwTaskId, 
                        KLCONN::TskAction   nAction)  = 0;

    };
}


#endif // KLCONN_TASKS_SYNCHRONIZER_H



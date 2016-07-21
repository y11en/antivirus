/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Errors.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Описание кодов ошибок, возвращаемых модулем Scheduler.
 *
 */
/*KLCSAK_PUBLIC_HEADER*/


#ifndef KLSCH_ERRORS_H
#define KLSCH_ERRORS_H

#include "std/err/errintervals.h"

namespace KLSCH {


    //! Перечисление кодов ошибок модуля Scheduler. 
    //  Любой из методов модуля может вернуть одну из этих ошибок.
    enum Errors {
        ERR_NONE = KLSCHERRSTART+0, 
        ERR_NO_MORE_TASKS,	/*!< Более нет задач на выполнение */
        ERR_TASK_TIMEOUT,	/*!< Задача выполняется более положенного времени */
        ERR_ID_EXISTS,		/*!< Задача с таким идентификатором уже существует */
        ERR_NO_MEMORY,		/*!< Нет памяти для заведения объекта */
        ERR_TIME_INTERVAL,	/*!< Неправильно задан интервал времени */
        ERR_TASK_ID,		/*!< Неправильный идентификатор задачи */
		ERR_TASK_STATE,		/*!< Некорректное состояние задачи */
        ERR_NO_TASK_ID,		/*!< Несуществующий идентификатор задачи */
        ERR_TASK_CLASS_ID,	/*!< Неправильный идентификатор класса задачи */
        ERR_NO_TASK_CLASS_ID, /*!< Несуществующий идентификатор класса задачи */
        ERR_SCHEDULE,		/*!< Неверное расписание для запуска задачи */
		ERR_SCHEDULE_EXPIRED,	/*!< Прошло время актуальности расписания */
        ERR_CALLBACK,		/*!< Неверно задан callback для запуска задачи */
        ERR_READONLY,		/*!< Клиент пытается модифицировать объект, 
										доступный только для чтения */
		ERR_SUSPEND_MODE,	/*!< Планировшик находиться в приостановленном состоянии */
		ERR_SCHEDULER_STATE,/*!< Некорректное состояние планировщика */
		ERR_INVALID_OBJECT, /*!< Неправильно инициализированный объект */
		ERR_TEST	        /*!< Ошибка тестирования библиотеки */
    }; 


}

#endif // KLSCH_ERRORS_H

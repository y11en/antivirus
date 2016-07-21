/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EveryWeekSchedule.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением класса расписания задачи 
 *        для запуска модулем-планировщиком еженедельно.
 *
 */

#ifndef KLSCH_EVERYWEEKSCHEDULE_H
#define KLSCH_EVERYWEEKSCHEDULE_H

#include "std/sch/errors.h"
#include "schedule.h"

#include "std/base/klbase.h"

namespace KLSCH {


   
    /*!
    * \brief Расписание задачи для запуска ее модулем-планировщиком
    *        еженедельно.
    *
    */ 


    class EveryWeekSchedule : public Schedule
	{

    public:

		/*!
			\brief Виртуальный деструктор.
		*/
		virtual ~EveryWeekSchedule() {}

		/*!
			\brief Возвращает тип расписания
		*/
		virtual ScheduleType GetType() const = 0;

		/*!
            \brief Устанавливает время для еженедельного выполнения задачи.

            \param day [in] День для запуска задачи (0-6). Воскресенье - 0.
            \param hours [in] Час для запуска задачи (0-23).
            \param min [in] Минута для запуска задачи (0-59).
            \param sec [in] Секунда для запуска задачи (0-59).
            \return код ошибки
				\retval <ERR_TIME_INTERVAL>

        */
        virtual Errors SetExecutionTime( int day = 0,
                                 int hours = 0, 
                                 int min = 0,
                                 int sec = 0) = 0;


        /*!
            \brief Возвращает время для ежедневного выполнения задачи.

            \param day [out] День для запуска задачи (0-6). Воскресенье - 0.
            \param hours [out] Час для запуска задачи (0-23).
            \param min [out] Минута для запуска задачи (0-59).
            \param sec [out] Секунда для запуска задачи (0-59).
            \return код ошибки
				\retval <ERR_SCHEDULE> внутренние значения класса не инициализированы

        */
        virtual Errors GetExecutionTime( int& day, int& hours, int& min, int& sec) const = 0;
		
		/*!
            \brief Устанавливает период выполнения задачи в неделях
				( по умолчанию период равен 1 недели )

            \param weeks [in] Период выполнения задачи в неделях
            \return код ошибки

        */
        virtual Errors SetPeriod( int weeks = 1 ) = 0;

        /*!
            \brief Возвращает период выполнения задачи в неделях

            \param weeks [out] Период выполнения задачи в неделях
            \return код ошибки

        */
        virtual Errors GetPeriod( int& weeks ) const = 0;
    };

}

/*
	\brief Метод для создания объекта MillisecondsSchedule
*/
KLCSC_DECL KLSCH::EveryWeekSchedule *KLSCH_CreateEveryWeekSchedule();

#endif // KLSCH_EVERYWEEKSCHEDULE_H

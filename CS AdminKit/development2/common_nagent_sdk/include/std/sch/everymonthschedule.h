/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EveryMonthSchedule.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением класса расписания задачи 
 *        для запуска модулем-планировщиком ежемесячно.
 *
 */

#ifndef KLSCH_EVERYMONTHSCHEDULE_H
#define KLSCH_EVERYMONTHSCHEDULE_H

#include "std/sch/errors.h"
#include "schedule.h"

#include "std/base/klbase.h"

namespace KLSCH {


   
    /*!
    * \brief Расписание задачи для запуска ее модулем-планировщиком
    *        ежемесячно.
    *
    */ 


    class EveryMonthSchedule : public Schedule
	{

    public:
		
		/*!
			\brief Виртуальный деструктор.
		*/
		virtual ~EveryMonthSchedule() {}

		/*!
			\brief Возвращает тип расписания
		*/
		virtual ScheduleType GetType() const = 0;

        /*!
            \brief Устанавливает время для ежемесячного выполнения задачи.

            \param day [in] День месяца для запуска задачи (1-31).
            \param hours [in] Час для запуска задачи (0-23).
            \param min [in] Минута для запуска задачи (0-59).
            \param sec [in] Секунда для запуска задачи (0-59).
            \return код ошибки

        */
        virtual Errors SetExecutionTime( int day = 0,
                                 int hours = 0, 
                                 int min = 0,
                                 int sec = 0) = 0;


        /*!
            \brief Возвращает время для ежемесячного выполнения задачи.

            \param day [out] День месяца для запуска задачи (1-31).
            \param hours [out] Час для запуска задачи (0-23).
            \param min [out] Минута для запуска задачи (0-59).
            \param sec [out] Секунда для запуска задачи (0-59).
            \return код ошибки

        */
        virtual Errors GetExecutionTime( int& day, int& hours, int& min, int& sec) const = 0;
		
		/*!
            \brief Устанавливает период выполнения задачи в месяцах
				( по умолчанию период равен 1 месяц )

            \param months [in] Период выполнения задачи в месяцах
            \return код ошибки

        */
        virtual Errors SetPeriod( int months = 1 ) = 0;

        /*!
            \brief Возвращает период выполнения задачи в месяцах

            \param months [out] Период выполнения задачи в месяцах
            \return код ошибки

        */
        virtual Errors GetPeriod( int& months ) const = 0;
    };
}

/*
	\brief Метод для создания объекта MillisecondsSchedule
*/
KLCSC_DECL KLSCH::EveryMonthSchedule *KLSCH_CreateEveryMonthSchedule();

#endif // KLSCH_EVERYMONTHSCHEDULE_H

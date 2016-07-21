/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EverydaySchedule.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением класса расписания задачи 
 *        для запуска модулем-планировщиком ежедневно.
 *
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef KLSCH_EVERYDAYSCHEDULE_H
#define KLSCH_EVERYDAYSCHEDULE_H

#include "std/sch/errors.h"
#include "schedule.h"

#include "std/base/klbase.h"

namespace KLSCH {


   
    /*!
    * \brief Расписание задачи для запуска ее модулем-планировщиком
    *        ежедневно.
    *
    */ 


    class EverydaySchedule : public Schedule
	{

    public:

		/*!
			\brief Виртуальный деструктор.
		*/
		virtual ~EverydaySchedule() {}

		/*!
			\brief Возвращает тип расписания
		*/
		virtual ScheduleType GetType() const = 0;

        /*!
            \brief Устанавливает время для ежедневного выполнения задачи.

            \param hours [in] Час для запуска задачи (0-23).
            \param min [in] Минута для запуска задачи (0-59).
            \param sec [in] Секунда для запуска задачи (0-59).
            \return код ошибки

        */
        virtual Errors SetExecutionTime( int hours = 0, 
                                 int min = 0,
                                 int sec = 0) = 0;


        /*!
            \brief Возвращает время для ежедневного выполнения задачи.

            \param hours [out] Час для запуска задачи (0-23).
            \param min [out] Минута для запуска задачи (0-59).
            \param sec [out] Секунда для запуска задачи (0-59).
            \return код ошибки

        */
        virtual Errors GetExecutionTime( int& hours, int& min, int& sec) const = 0;
		

		/*!
            \brief Устанавливает период выполнения задачи в днях
				( по умолчанию период равен 1 день )

            \param days [in] Период выполнения задачи в днях
            \return код ошибки

        */
        virtual Errors SetPeriod( int days = 1 ) = 0;

        /*!
            \brief Возвращает период выполнения задачи в днях

            \param days [out] Период выполнения задачи в днях.
            \return код ошибки

        */
        virtual Errors GetPeriod( int& days ) const = 0;		
    };
}

/*
	\brief Метод для создания объекта MillisecondsSchedule
*/
KLCSC_DECL KLSCH::EverydaySchedule *KLSCH_CreateEverydaySchedule();

#endif // KLSCH_EVERYDAYSCHEDULE_H

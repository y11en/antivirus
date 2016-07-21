/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EverydayScheduleImp.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением класса расписания задачи 
 *        для запуска модулем-планировщиком ежедневно.
 *
 */


#ifndef KLSCH_EVERYDAYSCHEDULEIMP_H
#define KLSCH_EVERYDAYSCHEDULEIMP_H

#include "std/sch/errors.h"
#include "std/sch/schedule.h"

#include "std/sch/everydayschedule.h"

namespace KLSCH {


   
    /*!
    * \brief Расписание задачи для запуска ее модулем-планировщиком
    *        ежедневно.
    *
    */ 


    class EverydayScheduleImp : public EverydaySchedule
	{

    public:

		/*!
            \brief Конструктор.
		*/
		EverydayScheduleImp();

		/*!
            \brief Конструтор копирования.
		*/
		EverydayScheduleImp( const EverydayScheduleImp &schedule );

		/*!
			\brief Возвращает тип расписания
		*/
		virtual ScheduleType GetType() const { return SCH_TYPE_EVERYDAY; }

        /*!
            \brief Устанавливает время для ежедневного выполнения задачи.

            \param hours [in] Час для запуска задачи (0-23).
            \param min [in] Минута для запуска задачи (0-59).
            \param sec [in] Секунда для запуска задачи (0-59).
            \return код ошибки

        */
        Errors SetExecutionTime( int hours = 0, 
                                 int min = 0,
                                 int sec = 0);


        /*!
            \brief Возвращает время для ежедневного выполнения задачи.

            \param hours [out] Час для запуска задачи (0-23).
            \param min [out] Минута для запуска задачи (0-59).
            \param sec [out] Секунда для запуска задачи (0-59).
            \return код ошибки

        */
        Errors GetExecutionTime( int& hours, int& min, int& sec) const;
		
		/*!
            \brief Устанавливает период выполнения задачи в днях
				( по умолчанию период равен 1 день )

            \param days [in] Период выполнения задачи в днях
            \return код ошибки

        */
        Errors SetPeriod( int days = 1 );

        /*!
            \brief Возвращает период выполнения задачи в днях

            \param days [out] Период выполнения задачи в днях.
            \return код ошибки

        */
        Errors GetPeriod( int& days ) const;

		/*!
            \brief Содает копию объекта.

            \param newCopy [out] Указатель на скопированный объект.
            \return код ошибки

        */
		virtual Errors Clone( Schedule **newCopy );

	private:
		/** Внутренние переменные */

		int		hours;		//!< Час запуска задача ( 0 - 23 )
		int		  min;		//!< Минута запуска задача ( 0 - 59 )
		int		  sec;		//!< Секунда запуска задача ( 0 - 59 )

		int		daysPeriod;	//!< Период расписания в днях

	protected:
		/** class helper functions */

		/*!
            \brief Фунция расчета времени следущего запуска. 
		*/
		virtual Errors CalculateNextExecutionTime( time_t &nextTime, int &msec, time_t currTime,
			time_t lastExeSec, int lastExeMsec, int startDelta = 0 );

    };

}

#endif // KLSCH_EVERYDAYSCHEDULEIMP_H

/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file SecondsScheduleImp.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением класса расписания задачи 
 *        для запуска задачи модулем-планировщиком каждые N секунд
 *
 */


#ifndef KLSCH_SECONDSSCHEDULEIMP_H
#define KLSCH_SECONDSSCHEDULEIMP_H

#include "std/sch/errors.h"
#include "std/sch/schedule.h"
#include "std/sch/secondsschedule.h"

namespace KLSCH {
   
    /*!
    * \brief Расписание задачи для запуска ее модулем-планировщиком каждые
    *        N миллисекунд
    *
    */ 

    class SecondsScheduleImp : public SecondsSchedule
	{

    public:
	
		/*!
            \brief Конструтор.
		*/
		SecondsScheduleImp();

		/*!
			\brief Возвращает тип расписания
		*/
		virtual ScheduleType GetType() const { return SCH_TYPE_SECONDS; }

		/*!
            \brief Конструтор копирования.
		*/
		SecondsScheduleImp( const SecondsScheduleImp &schedule );

        /*!
            \brief Устанавливает период выполнения задачи.

            \param sec [in] Период выполнения задачи в sec.
            \return код ошибки

        */
        Errors SetPeriod( int sec = 0 );

        /*!
            \brief Возвращает период выполнения задачи.

            \param sec [out] Период выполнения задачи в sec.
            \return код ошибки

        */

        Errors GetPeriod( int& sec ) const;

		/*!
            \brief Содает копию объекта.

            \param newCopy [out] Указатель на скопированный объект.
            \return код ошибки

        */
		virtual Errors Clone( Schedule **newCopy );

	private:
		/** Внутренние переменные */

		int		period;			//!< Период запуска ( в сeкундах )		
		time_t	firstStart;		//!< Время первого запуска
		int     periodCounter;	//!< Подсчет периода запуска

	protected:
		/** class helper functions */

		/*!
            \brief Фунция расчета времени следущего запуска. 
		*/
		virtual Errors CalculateNextExecutionTime( time_t &nextTime, int &msec, time_t currTime,
			time_t lastExeSec, int lastExeMsec, int startDelta = 0 );

		void GetExecutionTimes( time_t currTime, time_t &executionTimeBeforeCurrentSec,			
			time_t &executionTimeAfterCurrentSec, time_t lastExeSec, int lastExeMsec );
    };

}

#endif // KLSCH_SECONDSSCHEDULEIMP_H

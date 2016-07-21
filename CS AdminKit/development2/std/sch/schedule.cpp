/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Scheduler.cpp
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с реализацией класса расписание для задачи 
 *
 */

#include "std/sch/common.h"
#include "std/sch/errors.h"
#include "std/sch/schedule.h"

#include <limits.h>

namespace KLSCH {

	//!\brief Конструктор.
	Schedule::Schedule()
	{	
		lifeTime  = c_Undefined;
		runMissed = false;
		firstExeTime = c_Undefined;
		firstExemsec = c_Undefined;
	}
	
	//!\brief Деструктор.
	Schedule::~Schedule()
	{		
	}
	
	//!\brief Устанавливает флаг запуска пропущенных задач.
	Errors Schedule::SetRunMissedFlag( bool rMis /*= false*/ )
	{
		runMissed = rMis;
		return ERR_NONE;
	}
	
	//!\brief Возвращает флаг запуска пропущенных задач.
	Errors Schedule::GetRunMissedFlag( bool& rMis ) const
	{
		rMis = runMissed;		
		return ERR_NONE;
	}
	
	/*!\brief Устанавливает период актуальности расписания. Задача с
	просроченным периодом актуальности не будет выполнена.
	*/
	Errors Schedule::SetLifetime( time_t lt )
	{
		lifeTime = lt;
		return ERR_NONE;
	}
	
	
	/*!\brief Возвращает период актуальности расписания. Задача с
	просроченным периодом актуальности не будет выполнена.
	*/
	Errors Schedule::GetLifetime( time_t& lt ) const
	{
		lt = lifeTime;		
		return ERR_NONE;
	}
	
	
	//!\brief Устанавливает время для выполнения задачи первый раз.
	Errors Schedule::SetFirstExecutionTime( time_t sec /* = 0*/, int msec /* = 0 */)
	{
		firstExeTime = sec;
		firstExemsec = msec;
		return ERR_NONE;
	}
	
	//!\brief Возвращает время для выполнения задачи первый раз.
	Errors Schedule::GetFirstExecutionTime( time_t& sec, int& msec) const
	{
		sec  = firstExeTime;
		msec = firstExemsec;
		if ( sec==c_Undefined ) sec = 0;
		if ( msec==c_Undefined ) msec = 0;

		return ERR_NONE;
	}

	void Schedule::CopyFrom( const Schedule *from )
	{
		runMissed = from->runMissed;
		lifeTime = from->lifeTime;
		firstExeTime = from->firstExeTime;
		firstExemsec = from->firstExemsec;
	}

	/*!
    * \brief Пустое расписание
    *
    */ 
    class EmptySchedule : public Schedule
	{

    public:

		EmptySchedule() {}
		/*!
			\brief Виртуальный деструктор.
		*/
		~EmptySchedule() {}

		/*!
			\brief Возвращает тип расписания
		*/
		ScheduleType GetType() { return SCH_TYPE_NONE; }
		
		/*!
            \brief Устанавливает период выполнения задачи.
        */
        Errors SetPeriod( int period = 0 ) { return ERR_NONE; }

        /*!
            \brief Возвращает период выполнения задачи.
        */
        Errors GetPeriod( int& period ) const { return ERR_NONE; }	

		Errors Clone( Schedule **newCopy )
		{
			*newCopy = new EmptySchedule();
			(*newCopy)->CopyFrom( this ); // копирование базового класса
			return ERR_NONE;
		}

	protected:
		Errors CalculateNextExecutionTime( time_t &nextTime, int &msec, time_t currTime,
			time_t lastExeSec, int lastExeMsec, int startDelta = 0 )
		{
			nextTime = ULONG_MAX;
			return ERR_SCHEDULE;
		}
    };

}	// end namespace KLSCH

KLSCH::Schedule *KLSCH_CreateEmptySchedule()
{
	return new KLSCH::EmptySchedule;
}

/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TaskClassesList.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с определением интерфейса списка классов задач ( используется как вспомогательный класс ).
 *
 */


#ifndef KL_TASK_CLASSES_LIST_H
#define KL_TASK_CLASSES_LIST_H

#include <ctime>

#include "std/sch/common.h"
#include "std/sch/errors.h"

#include "std/sch/task.h"

namespace KLSCH {

   /*!
    * \brief Список задач планировщика.
    *
    */ 

	class TaskClassesList
	{
    public:

		/*! \struct ListUnit
			\brief Используется для хранения информации относящейся к конкретному классу задач
		*/
		struct ListUnit {

			ListUnit()
			{
				ctId = c_Undefined;
				nextExecutesec = 0;
				nextExecutemsec = 0;
				nextTaskId = c_Undefined;
			}

			TaskClassId		ctId;	//!< Идентификатор класса задач
			int			waitTime;	//!< Время ожидания для данного класса задач

			/** Описание следующей задачи для запуска */
			time_t	nextExecutesec;	//!< Время следущего запуска задачи данного класса
			int    nextExecutemsec;

			TaskId		nextTaskId;	//!< Идентификатор билжайшей задачи данного класса
		};
		
		/*!
            \brief Конструктор.
        */
		TaskClassesList( );

		/*!
            \brief Деструктор. 
				   Освобождает список классов задач.				   
        */
        ~TaskClassesList( );
		
		/** Добавляет в массив описатель указанного класса задач */		
		ListUnit *AddDescUnit( const TaskClassId ctId,
			int waitTime = c_Undefined );

		/** Заполняет WaitTime для указанного класса задач. Если запись для данного класса задач
		*    еще не создана, то создает запись.
		*/
		ListUnit *SetDescUnitWaitTime( const TaskClassId ctId,
			int waitTime = c_Undefined );

		/** Заполняет описатель указанного класса задач */
		ListUnit *SetDescUnitParams( TaskClassId ctId,
			time_t nextExecutesec, int nextExecutemsec, TaskId nextTaskId );

		/** Возвращает описатель указанного класса задач */		
		ListUnit *GetDescUnit( const TaskClassId ctId );

		/** Возвращает подходящий описатель класа задач с задачей для выполения */
		ListUnit *GetNextDescUnit( const TaskClassId ctId, time_t currTime, int currTimeMSec, 
			int waitTime );

		/** Итератор по всем описателям */
		ListUnit *GetFirst(  );
		ListUnit *GetNext( TaskClassId prevCid );
		        
	private:
		/** Внутренние переменные */

		ListUnit	*taskClasses;		//!< Динамический массив описания классов задач		
		int			 maxtaskClassesNum; //!< Выделенное количество элементов в массиве taskClasses

	protected:
		/** class helper functions */

		/** Возвращает свободную ячеку */
		TaskClassesList::ListUnit *GetFreeUnit();

    };

}	// end namespace KLSCH

#endif // KL_TASK_CLASSES_LIST_H

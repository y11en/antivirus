/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TasksIteratorImp.cpp
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с реализацией класса класса-итератора для
 *        просмотра списка задач.
 *
 */

#include <ctime>
#include <stdio.h>

#include "std/sch/common.h"
#include "std/sch/errors.h"
#include "schedulerimp.h"

#include "std/sch/task.h"
#include "tasksiteratorimp.h"

#include "private.h"

namespace KLSCH {

	//!\brief Конструктор.	
	TasksIteratorImp::TasksIteratorImp()
	{
		scheduler=NULL;
	}
	
	//!\brief Деструктор.
	TasksIteratorImp::~TasksIteratorImp()
	{	
		if ( scheduler!=NULL ) scheduler->NotifyIteratorDestroy( this );
	}
	
	//! \brief Функция инициализации итератора.
	void TasksIteratorImp::Init( SchedulerImp *sch, TaskList *lst, TaskClassId fCid,
		bool hanging )
	{
		if ( scheduler!=NULL ) scheduler->NotifyIteratorDestroy( this );

		scheduler	= sch;
		taskList	= lst;	
		currentTid  = c_Undefined;
		filterCid   = fCid;
		onlyHanging = hanging;
	}

	//!\brief Устанавливает итератор на начало списка.
    Errors TasksIteratorImp::Reset()
	{
		currentTid  = c_Undefined;

		return ERR_NONE;
	}

    //!\brief Возвращает константный указатель на следующую задачу в списке.
    Errors TasksIteratorImp::Next( const Task ** t, bool& hasMoreTasks )
	{
		if ( scheduler==NULL || taskList==NULL ) return ERR_INVALID_OBJECT;

//        XTRACE( "+1 " );
		LightMutex::AutoUnlock  autoUnlock( &scheduler->GetInternalMutex() );
//        XTRACE( "+2 " );

		hasMoreTasks = false;
		*t = NULL;
		
		TaskList::ListUnit *findUnit = taskList->GetNextUnit( currentTid, filterCid );

		if ( findUnit!=NULL )
		{
			if ( onlyHanging && !findUnit->task.IsTaskHanging() ) 
				findUnit = NULL;
			else
			{				
				findUnit->task.GetTaskId( currentTid );
				
				TaskList::ListUnit *nextUnit = taskList->GetNextUnit( currentTid, filterCid );
				if ( nextUnit!=NULL && onlyHanging && !nextUnit->task.IsTaskHanging() ) 
					nextUnit = NULL;
				
				if ( nextUnit ) hasMoreTasks = true;
				
				*t = &(findUnit->task);
			}
		}

		return ERR_NONE;
	}

}	// end namespace KLSCH

/*
	\brief Метод для создания объекта Scheduler
*/
KLSCH::TasksIterator *KLSCH_CreateTasksIterator()
{
	return new KLSCH::TasksIteratorImp;
}

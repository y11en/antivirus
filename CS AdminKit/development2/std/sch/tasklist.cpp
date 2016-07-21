/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TaskList.cpp
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Файл с реализацией класса списка задач.
 *
 */

#include <ctime>

#include "std/sch/common.h"
#include "std/sch/errors.h"
#include "private.h"

#include "tasklist.h"

namespace KLSCH {

	//!\brief Конструктор.
	TaskList::TaskList( )
	{
		first = NULL;
	}
	
	//!\brief Деструктор. 
	TaskList::~TaskList( )
	{
		// очищаем список
		ListUnit *curr = first;

		while( curr!=NULL )
		{
			ListUnit *next = curr->next;
			delete curr;
			curr = next;
		}
	}
	
	//!\brief Устанавливает идентификатор задачн.
	TaskList::ListUnit *TaskList::AddTask( const Task *t )
	{
		// заводим новую запись
		ListUnit *lstUnit = new ListUnit(*(const TaskImp *)t);

		// ишем конец списка
		ListUnit *curr = first;
		ListUnit *prev = NULL;

		while( curr!=NULL )
		{
			prev = curr; curr = curr->next;
		}

		if ( prev==NULL ) first = lstUnit;
		else		
			prev->next = lstUnit;

		return lstUnit;
	}	
	
	//!\brief Производить поиск задачи в списке.
	bool TaskList::FindTask( TaskList::ListUnit **taskUnit, const TaskId tId )
	{
		*taskUnit = NULL;
		TaskId  curTId;

		// ишем в списке
		ListUnit *curr = first;

		while( curr!=NULL )
		{
			curr->task.GetTaskId( curTId );
			if ( curTId==tId ) break;
			curr = curr->next;
		}

		if ( curr!=NULL ) {
			*taskUnit = curr;
			return true;
		}
		else
			return false;
	}
	
	//!\brief Удаляет задачу из списка.
	bool TaskList::DeleteTask( const TaskId tId )
	{
		// ишем в списке
		ListUnit *curr = first;
		ListUnit *prev = NULL;
		TaskId  curTId;

		while( curr!=NULL )
		{			
			curr->task.GetTaskId( curTId );
			if ( curTId==tId ) break;
			prev = curr;
			curr = curr->next;
		}

		if ( curr==NULL ) return false;	// не нашли
		
		if ( prev==NULL ) first = curr->next;
		else prev->next = curr->next;

		delete curr;

		return true;
	}	

	//!\brief Возвращает следущую задачу для выполенения.
    TaskList::ListUnit *TaskList::FindNextTask( const TaskClassId ctId )
	{
		// ишем в списке задачу с самым ближним временем запуска
		ListUnit *curr = first;		
		ListUnit *foundUnit = NULL;
		time_t	 minTimeSec  = 0;
		int		 minTimeMSec = 0;
		time_t	 curTimeSec;
		int		 curTimeMSec;
		Task::State  curState;
		int curtcId;

		while( curr!=NULL )
		{	
			curTimeSec  = curr->task.nextExeTime;
			curTimeMSec = curr->task.nextExemsec;
			curState    = curr->task.state;
			curtcId     = curr->task.cId;
			
			if ( curtcId == ctId && curState==Task::Scheduled &&
				 (minTimeSec==0  ||
				  CompareTimes( minTimeSec, minTimeMSec, curTimeSec, curTimeMSec )>0) ) 			
			{
				minTimeSec = curTimeSec, minTimeMSec = curTimeMSec;
				foundUnit = curr;
			}

			curr = curr->next;
		}

		if ( foundUnit==NULL )
			Trace( 4, "TaskList::FindNextTask NULL\n" );
		else
			Trace( 4, "TaskList::FindNextTask ctId - %d TaskId - %d minTimeSec - %d minTimeMSec - %d\n", 
					ctId, foundUnit->task.tId, minTimeSec, minTimeMSec );

		return foundUnit;
	}

	
	//!\brief Вызывает задачу на выполенение.
	bool TaskList::CallTaskById( const TaskId tId, void *threadContext, TaskResults** result )
	{
		ListUnit *taskUnit;

		bool res = FindTask( &taskUnit, tId );

		if ( res )
		{
			bool delAfterRunFlag = false;

			taskUnit->task.SetThreadContext( threadContext );
			taskUnit->task.SetCurrState( Task::Running );

			Trace( 4, "Call task. TaskId - %d ClientThreadContext - %d\n", tId, threadContext );
			// вызов задачи
			taskUnit->task.Call( result, delAfterRunFlag );

			taskUnit->task.SetThreadContext( NULL );
			taskUnit->task.SetCurrState( Task::Scheduled );

			return true;
		}
		else
			return false;
	}

	//!\brief Функция преднозначена для перебора описателей задач.
	TaskList::ListUnit *TaskList::GetNextUnit( const TaskId prevtId, TaskClassId tCid )
	{
		TaskId		 curTId;
		TaskClassId  curCTId;

		// ишем в списке
		ListUnit *curr = first;

		while( curr!=NULL )
		{
			curr->task.GetTaskId( curTId );
			
			if ( prevtId==c_Undefined || curTId==prevtId  ) 
			{			
				if ( prevtId!=c_Undefined ) curr = curr->next;
				if ( curr!=NULL && tCid!=0)
				{
					curr->task.GetClassId( curCTId );
					if ( curCTId!=tCid ) continue;
				}
				break;
			}
			curr = curr->next;
		}

		return curr;
	}

	//!\brief Удаляет все задачи, у которых текущий статус Deleted, из списка.			
	void TaskList::EraseAllDeletedTasks()
	{
		Task::State  curState;		

		// ишем в списке
		ListUnit *curr = first;
		ListUnit *prev = NULL;

		while( curr!=NULL )
		{
			curr->task.GetCurrentState( curState );
			
			if ( curState==Task::Deleted ) 
			{			
				ListUnit *found = curr;
				if ( prev!=NULL )				
					prev->next = curr->next;				
				else
					first = curr->next;

				curr = curr->next;
				delete found;
			}
			else			
				prev = curr, curr = curr->next;
		}

	}

}	// end namespace KLSCH

/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file SchedulerImp.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Главный интерфейс модуля-планировщика
 *
 *  Данный файл содержит описание интерфейса модуля-планировщика Scheduler.
 *  Модуль-планировщик позволяет определять задачи и вызывать задачи на выполнение
 *  в определенный момент времени.  Полное описание модели работы модуля Scheduler
 *  содержится в файле CS AdminKit\DESIGN\Modules\Scheduler.doc
 *
 */


#ifndef KLSCHIMP_H
#define KLSCHIMP_H

#ifdef _MSC_VER
	#pragma warning (disable : 4786)
#endif

#include <list>

#include "std/sch/common.h"
#include "std/sch/errors.h"
#include "private.h"

#include "std/sch/scheduler.h"

#include "std/sch/task.h"
#include "tasklist.h"
#include "taskclasseslist.h"
#include "tasksiteratorimp.h"

namespace KLSCH {

/*!
 * \brief Главный интерфайс модуля Scheduler.  Изучение функциональности модуля
 *		  надо начинать с этого интерфейса.
 */ 

    class Task;
    class TasksIterator;
    class TaskResults;
// class SchedulerImp;
//    class SchedulerImp::WaitingThreadContext;
// typedef std::list<KLSCH::SchedulerImp::WaitingThreadContext *> container_type;

    // _WaitingThreadContext defined here to workarounv VC6 bug
    // with incomplete class/inner class definition, like
    // BaseClass::InnerClass, in the context like
    // list<BaseClass::InnerClass *>.

    /*! \struct WaitingThreadContext
      \brief Используется для хранения информации ожидающего задачи потока
    */
    struct _WaitingThreadContext {
        /*\enum Тип ожидания */
        enum WaitingType {
          NO_WAITING,			//!< Признак свободного контекста
          TASK_WAITING,		//!< Поток ожидает определенную задачу
          NO_TASK_WAITING,	//!< Задачи для потока нет, он просто ждет необходимый timeout
          LEAVE_WAITING,		//!< Данный статус простовляется внешним потоком ( признак того что поток выведен из ожидания )
          TASK_RUNNING		//!< Контекст выполняет задачу
        };

        _WaitingThreadContext() : sem(0)
          {
            wType				= NO_WAITING;
            tId					= c_Undefined;
            tcId				= c_Undefined;
            clientThreadCntx	= NULL;
          }		

        KLSCH::Semaphore 	 sem;	//!< Семафор для операций ожидания

        WaitingType		   wType;	//!< Тип ожидания
        TaskId			     tId;	//!< Идентификатор задачи, которую ждем
        time_t		     timeSec;	//!< Время запуска задача
        int				timeMSec;
        TaskClassId			tcId;	//!< Класс задачи
        
        void  *clientThreadCntx;	//!< Контекст клиентского потока ( передаеться в функцию WaitTaskExecution )
    };

	class SchedulerImp : public Scheduler
	{
		friend class TasksIteratorImp;

    public:

	/*!
      \brief Конструктор.
	  \param waitTime [in] Число миллисекунд для ожидания потоком момента 
                           выполнения задачи.
	 */
		SchedulerImp( int waitTime = c_Infinite );

	/*!
		\brief Деструктор.
	*/
		~SchedulerImp( );

    /*!
      \brief Метод добавляет новую задачу к списку задач.

      Scheduler копирует описание задачи во внутренний список и
      возвращает клиенту идентификатор добавленной задачи. Клиент
      может передать идентификатор задачи в переменной id_ptr. В
      этом случае Scheduler либо заводит задачу с заданным идентификатором,
      либо возвращает ошибку ERR_ID_EXISTS если задача с таким
      идентификатором уже существует.

      \param t		[in]  Описание добавляемой задачи.
      \param id_ptr [out] Указатель на переменную, которая будет проинициализована 
						  идентификатором новой добавленной задачи. Если в объекте Task
						  параметр TaskId опредлен ( см. метод SetTaskId ) то он и будет
						  использован в качестве идентификатора задачи ( автоматическая 
						  генерация идентификатора поизводиться не будет ).
      \return код ошибки

    */
      
        Errors AddTask( const Task *t, TaskId& id_ptr );

    /*!
      \brief Удаляет задачу с заданным идентификатором.

      Scheduler удаляет задачу из списка задач на выполнение.  Поток,
      ожидающий выполнения задачи, переключается на выполнение другой
      задачи либо ему возвращается ошибка ERR_NO_MORE_TASKS.

      \param id [in] Идентификатор удаляемой задачи.
      \return код ошибки

    */

        Errors DelTask( const TaskId id );


	/*!
      \brief Устанавливает параметры для указанной задачи

      \param id [in] Идентификатор 
	  \param par [in] Ссылка на параметры для запуска задачи.
      \return код ошибки

    */

        Errors SetPramsForTask( const TaskId id, TaskParams * par );


	/*!
      \brief Приостанавливает работы указанной задачи.

      Scheduler не будет ставить данную задач на выполнение пока для нее не будет вызвана
	  команда ResumeTask.

      \param id [in] Идентификатор задачи.
      \return код ошибки ( ERR_TASK_ID, ERR_TASK_STATE, ERR_NONE )

    */

        Errors SuspendTask( const TaskId id );

	/*!
      \brief Возобновляет работу приостанавливает работы указанной задачи.

      \param id [in] Идентификатор задачи.
      \return код ошибки ( ERR_TASK_ID, ERR_TASK_STATE, ERR_NONE )

    */

        Errors ResumeTask( const TaskId id );

	/*!
      \brief Возвращает следующее время запуска задачи.

      \param id [in] Идентификатор задачи.
	  \param sec [out] Запланированное время выполнения задачи в формате time_t.
      \param msec [out] Запланорованное время выполнения задачи в msec. Значения
                             параметров sec и msec складываются.
      \return код ошибки ( ERR_TASK_ID, ERR_NONE )
    */

        Errors GetTaskNextExecutionTime( const TaskId id, time_t& sec, int& msec );

    /*!
      \brief Создает итератор для просмотра списка задач.

      Создает итератор для просмотра списка задач и возвращает его
      клиенту.  Клиент может передать класс задач для просмотра.
      По умолчанию просматриваются все задачи.

      \param it [out] Итератор для просмотра списка задач.
      \param cid [in] Класс задач для просмотра итератором.
      \return код ошибки

    */


        Errors CreateTasksIterator( 
            TasksIterator *it, 
            TaskClassId cid = 0 );


    /*!
      \brief Создает итератор для просмотра списка "подвисших" задач.

      Создает итератор для просмотра списка "подвисших" задач (т.е. задач,
      которые не завершили выполнение в течении отведенного для этого времени)
      и возвращает его клиенту. Клиент может передать класс задач для просмотра.
      По умолчанию просматриваются все задачи.

      \param it [out] Итератор для просмотра списка "подвисших" задач.
      \param cid [in] Класс задач для просмотра итератором.
      \return код ошибки

    */


        Errors CreateHangingTasksIterator( 
            TasksIterator *it, 
            TaskClassId cid = 0 );


    /*!
      \brief Возвращает число задач, ожидающих выполнения внутри Scheduler.

      \param num [out] Число задач, определенных внутри Scheduler.
      \param cid [in] Класс задач для получения числа задач.  По умолчанию
                      возвращается полное число задач.
      \return код ошибки

    */


        Errors GetTasksNumber( int& num, TaskClassId cid = 0) const;

	/*!
	  \brief Переводит scheduler в режим приостановлен.

      При работе в данном режиме функция WaitTaskExecution возвращает ERR_SUSPEND_MODE.
	  Все потоки, находящиесся в момент вызова функции в WaitTaskExecution буде отпущены из
	  функции с кодом возврата ERR_SUSPEND_MODE.
      
      \return код ошибки

    */	

		Errors Suspend( );

	/*!
	  \brief Преводит планировшик из режима приостановлен в нормальный.
            
      \return код ошибки

    */	

		Errors Resume( );

    /*!
      \brief Устанавливает максимальное время ожидания потока до начала 
             выполнения задачи (msec).

      Метод устанавливает максимальное время ожидания потока до начала 
      выполнения задачи заданного класса в миллисекундах.  В случае, если 
      время до начала выполнения задачи менее установленного в данном методе,
      поток блокируется в методе WaitTaskExecution.  В противном случае
      метод WaitTaskExecution возвращает управление немедленно с ошибкой
      ERR_NO_MORE_TASKS.  По умолчанию время ожидания устанавливается для задач
      всех классов.

      \param waitTime [in] Число миллисекунд для ожидания потоком момента 
                           выполнения задачи.
      \param cid [in] Класс задач для установления времени ожидания.
      \return код ошибки

    */

        Errors SetWaitTime( int waitTime = 10000, TaskClassId cid = 0 );


    /*!
      \brief Возвращает максимальное время ожидания потока до начала 
             выполнения задачи (msec).

      \param waitTime [out] Число миллисекунд для ожидания потоком момента 
                           выполнения задачи.
      \param cid [in] Класс задач для получения времени ожидания.
      \return код ошибки

    */

        Errors GetWaitTime( int& waitTime, TaskClassId cid = 0 );


    /*!
      \brief Блокирует поток клиента до момента выполнения задачи.

      Метод блокирует поток клиента до момента выполнения задачи. Определенный
      для задачи callback вызывается в этом методе и результаты выполнения
      возвращаются клиенту. После этого клиент отвечает за результаты выполнения
      задачи и должен удалить их после окончания работы с ними.
      Клиент может определить, выполнение задач какого
      класса он ожидает внутри данного метода.  По умолчанию ожидается выполнение
      задачи любого класса.  Если время до выполнения ближайшей задачи более 
      заданного, возвращается ошибка ERR_NO_MORE_TASKS.

      \param res [out] Результат выполнения задачи.  Клиент должен удалить
                       результаты выполнения задачи после окончания работы
                       с ними.
	  \param threadContex [in] Контекст вызываемого потока. Формат контекст опеределяется 
							   самим вызвавшым функцию потоком. Данный контекст будет сохранен
							   в описание задачи ( класс Task ) на время ее выполения. Может быть
							   использован при работе с завишими задачами.
      \param waitTime [in] Число миллисекунд для ожидания потоком момента 
                           выполнения задачи.  По умолчанию используется
                           время, определенное в SetWaitTime.
      \param cid [in] Класс задач для запуска. По умолчанию запускается задача
                      любого класса.
	  \param waitFlag [in] Признак наобходимости ожидания внутри функции даже в случае если нет
					  готовых на выполение задач. По умолчанию используется
                      время, определенное в SetWaitTime.
      \return код ошибки

    */

        Errors WaitTaskExecution(   TaskResults** res,
									void *threadContex,
                                    int waitTime = 0, 
                                    TaskClassId cid = 0,
									bool waitFlag = true );

	private:
		/** Внутренние структуры и классы */

        typedef _WaitingThreadContext WaitingThreadContext;
        typedef std::list<KLSCH::_WaitingThreadContext *> container_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;


		/** Внутренние переменные */

		int			internalTaskIdCounter;	//!< Используется для генерации taskId

		TaskList				 taskList;	//!< Список задач
		
		TaskClassesList	  taskClassesList;	//!< Список описаний классов задач		
        container_type wtc_list;  //!< Динамический массив WTC

		LightMutex					mutex;	//!< Mutex для сериализация доступа ко внутренним переменным объекта

		bool				  suspendMode;	//!< Флаг приостановленного режима планировщика

		int		      createdIteratorsNum;	//!< Счетчик созданных итераторов ( используется в функции удаление задачи )

		bool			  needFreeDelTask;	//!< Флаг необходимости физически удалить задачи из списка после уничтожений последнего итератора

	protected:
		/** @name class helper functions */
		//@{

		/** @name Функции для работы с массивом контекстов ожидаещего потока*/		
		//@{		
		/** Возвращает свободную ячейку в массиве */
		WaitingThreadContext *GetFreeWTCUnit( );	
		/** Возвращает первый ожидающий контекст данного класса задач */
		WaitingThreadContext *GetWaitingWTCByClassTaskId( const TaskClassId ctId );	
		/** Освобождает wtc */
		void FreeWTCUnit( WaitingThreadContext *wtc );
		//@}

		/** Вычисляет время запуска данной задачи. Производить обновление WTC */
		void WriteTaskNextExecutionTime( TaskList::ListUnit *taskDesc );

		/** Вычищает данную задачу из текущих заполненных структур */
		void RemoveTaskFromWork( TaskId tId, TaskList::ListUnit *taskUnit, bool findInWTC );

		/** Возвращает ссылку на внутренний mutex. Необходимо для TasksIterator */
		LightMutex &GetInternalMutex();

		/** Функция нотификации о уничтожении объекта итератор. Вызывается из класса TasksIterator */
		void NotifyIteratorDestroy( TasksIteratorImp *tIter );

		
		//@}
    };

} // namespace KLSCH

#endif // KLSCHIMP_H

// Local Variables:
// mode: C++
// End:

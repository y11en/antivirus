/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	simp_comp.h
 * \author	Andrew Kazachkov
 * \date	10.10.2002 16:18:46
 * \brief	
 * 
 */

#ifndef _BASE_H__A7511925_019D_4f4a_B241_3515E33E4F1E
#define _BASE_H__A7511925_019D_4f4a_B241_3515E33E4F1E

#include <std/base/klstd.h>
#include <std/par/params.h>
#include <kca/prss/settingsstorage.h>
#include <kca/prci/componentinstance.h>

#include <map>

namespace SIMP_COMP
{
	/*!
		\brief	Базовый класс задачи компонента.
	*/
	class KLSTD_NOVTABLE CTaskBase
	{
		friend class CTaskStorage;
	public:

		/*!
		  \brief	Конструктор

		  \param	pInstance		Экземпляр компонента
		  \param	wstrTaskName	Имя задачи
		  \param	idTask			Идентификатор задачи
		*/
		CTaskBase(
				KLPRCI::ComponentInstance*	pInstance,
				const std::wstring			wstrTaskName,
				long						idTask,
				bool						bAutoImpersonate=true);

		virtual ~CTaskBase();
		/*!
		  \brief	Вызывается асинхронно для передачи команд задаче (стоп, пауза, возобновление, ...)
		  \param	action идентификатор команды
		*/
		virtual void OnControl(KLPRCI::TaskAction action);
		/*!
		  \brief	Тело задачи - вызывается в отдельной нити (создаваемой в StartWorkingThread)
		  \return	код завершения задачи, который станет кодом завершения нити
		*/
		virtual unsigned long Run()=0;
		
	protected:

		/*!
		  \brief	Создаёт нить, в которой вызывается метод Run(). Данный метод вызывается
			внутри CTaskStorage. е следует вызыввать его непосредственно.
		*/
		virtual void StartWorkingThread();	

		/*!
		  \brief	Процедурв нити, в которой вызывается метод Run().
		*/
		static unsigned long KLSTD_THREADDECL threadproc(void *arpg);

		const std::wstring								m_wstrTaskName;
		const long										m_idTask;
		KLSTD::CAutoPtr<KLPRCI::ComponentInstance>		m_pInstance;
		KLSTD::CAutoPtr<KLPRCI::ProductTasksControl>	m_pTasksControl;
		std::auto_ptr<KLSTD::Thread>					m_pThread;
		KLSTD::CAutoPtr<KLPRCI::SecContext>				m_pClientSecurityContext;
		bool											m_bAutoImpersonate;
	public:
		volatile bool									m_bDone;
	};

	typedef std::map<long, CTaskBase*> tasks_t;

	/*!
		\brief	Хранилище задач.
	*/
	class CTaskStorage
	{
	public:		
		CTaskStorage();
		virtual ~CTaskStorage();
		

		/*!
		  \brief	Блокировка хранилища для эксклюзивного доступа к отображению m_mapTasks.
			Каждому вызову Lock() должен соответствовать вызов Unlock(). Возвращаемый указатель 
			tasks_t* нельзя использовать после вызова метода Unlock().
		  \return	указатель на отображения иденитификатора задачи в указатель на клас CTaskBase.
		*/
		tasks_t*	Lock();

		/*!
		  \brief	Разблокировка хранилища. 
		*/
		void		Unlock();

		/*!
		  \brief	Добавление задачи в хранилище

		  \param	идентификатор задачи, добавляемой в хранилище.
		  \param	указатель на CTaskBase* задачи, добавляемой в хранилище.
		  \except	KLSTD::STDE_EXIST задача с идентификатором idTask уже имеется в хранилище
		*/
		void		TaskNew(long idTask, CTaskBase* pTask);

     	void Clear();

	protected:
		KLSTD::CAutoPtr<KLSTD::CriticalSection>	m_pCS;
		tasks_t									m_mapTasks;
	};

	//! \brief Вспомогательный класс для автоматической разблокировки хранилища CTaskStorage.
	class AutoTaskStorage
	{
	public:
		AutoTaskStorage(CTaskStorage* pStorage, tasks_t* &tasks):
			m_pStorage(pStorage)
		{			
			tasks=pStorage->Lock();
		};
		virtual ~AutoTaskStorage()
		{
			m_pStorage->Unlock();
		};
	protected:
		CTaskStorage* m_pStorage;
	};


	//! \brief Базовый класс управляемого компонента
	class KLSTD_NOVTABLE CComponentBase
	{
	public:

		/*!
		  \brief	Конструктор

		  \param	idComponent идентификатор компонента
		  \return	
		*/
		CComponentBase(const KLPRCI::ComponentId& idComponent);		
		virtual ~CComponentBase();
		;

		/*!
		  \brief	Инициализация CComponentBase, создание экземпляра ComponentInstance.
			Для проведения инициализации унаследованного класса, использующей m_pInstance,
			необходимо перегрузить данный метод.
		*/
		virtual void Initialize();
		;

		/*!
		  \brief	Вызывается асинхронно для передачи команд компонету
		  \param	action команда
		*/
		virtual void OnInstanceControl(KLPRCI::InstanceAction action);

		/*!
		  \brief	Вызывается асинхронно для передачи команд задачам. Реализация
					по умолчанию обращается к хранилищу, ищет задачу с идентификатором
					taskId и вызывает её callback

		  \param	taskId	идентификатор задачи
		  \param	action	команда
		*/
		virtual void OnTasksControl(long taskId, KLPRCI::TaskAction action);


		/*!
		  \brief	Тело компонента. Реализация по умолчанию посылает пинги агенту.
			Выход из метода означает завершение работы агента. Метод завершается
			при инкременте семафора m_pDoneSemaphore. При возникновении ошибки
			выбрасывается исключение.

		  \return	код возврата. Реализация по умолчанию возвращает 0.
		*/
		virtual unsigned long	Run();

		/*!
		  \brief	Метод инкрементирует семафор m_pDoneSemaphore
		*/
		virtual void			Stop();


		/*!
		  \brief	Вызывается когда внешний клиент хочет запустить задачу с имененм taskName.
			Если задача с таким имененем не поддерживается, следует выбрасывать исключение
			STDE_NOFUNC (макрос KLSTD_NOTIMP() ).
			\param	taskName	имя задачи
			\param	params		параметры задачи
			\param	taskId		Идентификатор создаваемой задачи
			\param	timeout		Тайм-аут работы задачи.
		*/
		virtual void OnRunTasks(
						const std::wstring&		taskName,
						KLPAR::Params*			params,
						long					taskId,
						long					timeout)=0;
	protected:
		static void InstanceControlCallback(void* context, KLPRCI::InstanceAction action);

		static void RunTasksCallback(
						void*					context,
						const std::wstring&		taskName,
						KLPAR::Params*			params,
						long					taskId,
						long					timeout);
		static void TasksControlCallback(
						void*				context,
						long				taskId,
						KLPRCI::TaskAction	action);

		const KLPRCI::ComponentId					m_idComponent;
		KLSTD::CAutoPtr<KLPRCI::ComponentInstance>	m_pInstance;
		KLSTD::CAutoPtr<KLSTD::Semaphore>			m_pDoneSemaphore;
		CTaskStorage								m_TaskStorage;
	};

};

#endif // _BASE_H__A7511925_019D_4f4a_B241_3515E33E4F1E

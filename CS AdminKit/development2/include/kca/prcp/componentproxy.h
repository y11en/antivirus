/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	ComponentProxy.h
 * \author	Andrew Kazachkov
 * \date	01.10.2002 12:34:51
 * \brief	Главный интерфейс модуля Product Component Proxy (PRCP).
 * 
 */

#ifndef KLPRCP_COMPONENTPROXY_H
#define KLPRCP_COMPONENTPROXY_H

#include <time.h>
#include <string>
#include <vector>

#include "std/err/error.h"
#include "std/par/params.h"
#include "kca/prci/componentinstance.h"
#include "kca/prci/producttaskscontrol.h"
#include "errors.h"
#include "prcp_const.h"

namespace KLPRCP {	

   /*!
		\brief Callback - функция для получения информации о событии, 
         возбужденном в компоненте продукта.  Реализация этой функции должна быть 
         потокобезопасной.

        \param id           [in]  Идентификатор компонента, публикующего события.
		\param eventType    [in]  Тип события.
        \param eventBody    [in]  Параметры события.
        \param time         [in]  Время возникновения события.
        \param context      [in]  Контекст, переданный в подписке на события.

        Внутри колбэка EventCallback запрещены следующие действия.
        1. Создание прокси
        2. Подписка на события и отписка.
        3. Синхронный запуск компонентов и задач.
        4. Обращения к SettingsStorage, EventStorage и TasksStorage.
        5. Ожидание прихода события.
	*/

    typedef void (*EventCallback)(
        const KLPRCI::ComponentId & id,
        const std::wstring & eventType,
        const KLPAR::Params * eventBody,
        time_t time,
        void * context );
    
    //! using this constant as asyncid results in calling method instead of a task
    const wchar_t c_szwAsyncMethod[] = L"KLPRCP_METHOD";


    /*!
    * \brief Главный интерфейс модуля Product Component Proxy (PRCP).
    *  
    *  Данный интерфейс является представителем функциональности компонента продукта 
    *  для других компонентов продукта. 
    *
    */
    

    class KLSTD_NOVTABLE ComponentProxy: public KLSTD::KLBase {
    public:
    /*!
      \brief Возвращает идентификатор компонента.

      Идентификатор компонента является уникальным для всех компонентов одного
      и того же типа, принадлежащих одному и тому же продукту. Идентификатор
      компонента всегда больше нуля.
      
      \return Идентификатор компонента.
 
    */

        virtual void GetInstanceId(std::wstring& wstrID) const = 0;


    /*!
      \brief Возвращает время, в течении которого запущенный компонент должен
                оповестить агента администрирования о своей нормальной работе.

      \return   Максимальное время оповещения о нормальной работе (msec) или
                KLSTD_INFINITE, если компонент не генерирует оповещения.
      
      \exception Данный метод не выбрасывает исключений.
 
    */
        virtual long GetPingTimeout() const = 0;



    /*!
      \brief Возвращает текущее состояние компонента.

      \return Состояние компонента.
 
    */

        virtual KLPRCI::ComponentInstanceState GetState() const = 0;


    /*!
      \brief Возвращает статистику работы компонента. Если в контейнере определены
      только часть полей, возвращается статистика только для этих полей.

       \param statistics [in,out] Статистика работы компонента.

    */

        virtual void GetStatistics(KLPAR::Params * statistics) const = 0;


    /*!
      \brief Останавливает работу компонента.

    */

        virtual void Stop() = 0;


    /*!
      \brief Приостанавливает работу компонента.

    */

        virtual void Suspend() = 0;


    /*!
      \brief Возобновляет работу компонента.

    */

        virtual void Resume() = 0;
    

    /*!
      \brief Запускает задачу компонента на выполнение.

        \param taskName       [in]  Имя выполняемой задачи.
        \param params         [in]  Параметры для выполнения задачи.
        \param timeout        [in]  Максимальное время выполнения задачи (msec).

        \return Идентификатор запущенной задачи.

    */

        virtual long RunTask(
								const std::wstring& taskName,
								const KLPAR::Params* params,
								const std::wstring& asyncId=L"",
								long timeout = 0) = 0;

    /*!
      \brief Запускает задачу компонента на выполнение (синхронно) и
            ожидает завершения задачи.

        \param taskName       [in]  Имя выполняемой задачи.
        \param params         [in]  Параметры для выполнения задачи.
        \param results        [out] Результаты выполнения задачи. 
                                    Если задача не возвращает результатов, 
        \param timeout        [in]  Максимальное время выполнения задачи (msec). 
                                    <=0 - бесконечное время.
        \param pTaskState     [out] Соcтояние, с которым завершилась задача

        \return Идентификатор выполненной задачи.

    */

		virtual long RunTaskSync(
                        const std::wstring&		taskName, 
                        const KLPAR::Params*	params,
                        KLPAR::Params**			results,
                        long					timeout=KLSTD_INFINITE,
						const std::wstring&		asyncId = L"",
                        KLPRCI::TaskState*      pTaskState=NULL) = 0;

	/*!
	  \brief	Возвращает имя задачи с идентификатором taskId.

	  \param	taskId [in]
	  \param	wstrTaskName [out]
	*/
		virtual void GetTaskName(long taskId, std::wstring& wstrTaskName) const =0;

    /*!
      \brief Останавливает выполнение задачи компонентом и удаляет задачу из
             списка выполняемых компонентом задач.  Вызов метода асинхронный.
             результаты выполнения команды возвращаются в событии c_EventTaskState.

        \param taskId         [in]  Идентификатор запущенной задачи.

    */		
        virtual void StopTask(long taskId) = 0;

    /*!
      \brief Приостанавливает выполнение задачи компонентом.  Вызов метода 
                асинхронный.

        \param taskId         [in]  Идентификатор запущенной задачи.

    */
        virtual void SuspendTask(long taskId) = 0;


    /*!
      \brief Возобновляет выполнение задачи компонентом.  Вызов метода асинхронный.

        \param taskId         [in]  Идентификатор запущенной задачи.

    */
        virtual void ResumeTask(long taskId) = 0;

        virtual void ReloadTask(long taskId, KLPAR::Params* pNewSettings) = 0;
        

    /*!
      \brief Возвращает список выполняемых компонентом задач.

        \param ids [out] Список идентификаторов выполняемых компонентом задач.

    */
        virtual void GetTasksList( std::vector<long> & ids) const  = 0;

    /*!
      \brief Возвращает параметры задачи.
	*/		
		virtual void GetTaskParams(
							long			idTask,
							KLPAR::Params*	pFilter, 
							KLPAR::Params**	ppResult) = 0;

    /*!
      \brief Возвращает текущее состояние выполняемой компонентом задачи.

        \param taskId         [in]  Идентификатор задачи.

        \return Состояние выполняемой компонентом задачи.

    */
        virtual KLPRCI::TaskState GetTaskState(long taskId) const = 0;

    /*!
      \brief Возвращает процент выполнения задачи.

        \param taskId         [in]  Идентификатор задачи.

        \return Процент выполнения задачи.

    */
        virtual int GetTaskCompletion(long taskId) const = 0;



    /*!
      \brief Подписывает клиента на получение оповещений о 
            возникновении событий в компоненте и устанавливает callback 
            для получения оповещений.
       
        Если клиент не задает тип события то фильтрация событий по этому параметру 
        не производится.  Для фильтра на параметры событий проверка производится
        только на равенство полей, то есть если поле есть и в фильтре и в событии и
        эти поля равны, то событие доставляется подписчику.

        Клиент может подписаться на несколько типов событий.  В этом случае он получит
        отдельные нотификации для каждого из подходящих по условиям подписки событий.

		\param eventType        [in]  Тип события.
        \param eventBodyFilter  [in]  Фильтр на параметры события, м.б. равен NULL.
        \param callback         [in]  Callback - функция для получения оповещения о 
                                        возникновении события.
        \param context          [in]  Контекст подписки для передачи в callback - функцию.
                                      Данный указатель должен быть корректным все время
                                      подписки.
        \param hSubscribe       [out] Идентификатор подписки для последующего вызова 
                                        StopSubscription.

    */

       virtual void Subscribe( 
                        const std::wstring&		eventType,
                        const KLPAR::Params*	eventBodyFilter,
                        EventCallback			callback,
                        void*					context,
						KLPRCI::HSUBSCRIBE&		hSubscribe)= 0;

    /*!
      \brief Прекращает подписку клиента на события определенного типа.
       
        Прекращается действие подписки для данного ComponentProxy, 
        параметры которых совпадают с параметрами, переданными в вызове метода.

		\param hSubscribe       [in]  Идентификатор подписки.

    */

       virtual void StopSubscription(KLPRCI::HSUBSCRIBE hSubscribe)=0;


	/*!
	  \brief	Возвращает ComponentId proxy

	  \return	ComponentId proxy
	*/
	   virtual KLPRCI::ComponentId GetComponentID()=0;


	/*!
	  \brief	Возвращает ComponentId компонента, с которым proxy "держит связь"

	  \return	ComponentId компонента
	*/
	   virtual KLPRCI::ComponentId GetRemoteComponentID()=0;  

		/*!
		  \brief	Проверяет наличие активного соединения с компонентом. Если
					возвращает false, то соединение с компонентом разорвано, и
					пользоваться данной прокси уже нельзя.
					Следует однако учитывать, что соединение может разорваться
					после вызова данной функции, но до использования методов
					прокси.

		  \return	true, если соединение наличиствует
		*/
	   virtual bool IsValid() = 0;
    };

}

/*!
	\brief Создает новый объект ComponentProxy для указанного клиентом компонента.

	Если несколько компонентов подходят к идентификатору компонента, переданному в
	параметре id, то возвращается указатель на произвольный компонент.

	\param id      [in]  Идентификатор компонента получения proxy компонента.
	\param proxy   [out] Указатель на переменную, в которую будет записан
		указатель на созданный объект.

	\exception	ERR_CANT_CONNECT означенный компонент не запущен
*/

KLCSKCA_DECL void KLPRCP_CreateComponentProxy( 
                        const KLPRCI::ComponentId & id,
                        KLPRCP::ComponentProxy**	ppProxy);


/*!
  \brief	Принудительно выгружает все загруженные в процесс компоненты.
			Эту функцию процесс должен вызывать перед своим завершением.
*/
KLCSKCA_DECL void KLPRCP_UnloadInProcessComponents();

/*!
  \brief	Возвращает список компонентов, загруженных в адресное пространство
			данного процесса.
*/
KLCSKCA_DECL void KLPRCP_GetLoadedInProcessComponents(
						KLPRCI::ComponentId&			filter,
						std::vector<KLPRCI::ComponentId>&	ids);



#endif // KLPRCI_COMPONENTPROXY_H

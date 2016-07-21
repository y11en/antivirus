/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EventSource.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Интерфейс класса событие, который является основной информационной единицей 
 *			библиотеки Eventing
 * 
 */


#ifndef KLEV_EVENT_SOURCE_H
#define KLEV_EVENT_SOURCE_H

#include <string>

#include "event.h"
#include "subscriptioniterator.h"
#include "std/tp/threadspool.h"

namespace KLEV {

	/*!
		\brief EventsSource - основной класс работы с системой Eventing.
			Данный класс предназначен для подписки и публикации событий.
	*/

	/*!
		\brief Callback - функция оповешения о получении события, 
			возбужденном в компоненте продукта. Реализация этой функции должна быть 
			потокобезопасной.

        \param subscriber  [in]  Компонента подписчика.
        \param publisher   [in]  Компонента опубликовавшая событие.        
		\param event	   [in]  Событие.
	*/
    typedef void (*EventCallback)(
        const KLPRCI::ComponentId & subscriber, 
        const KLPRCI::ComponentId & publisher,        
        const Event *event,
		void *context );

	// идентификатор подписки
	typedef int SubscriptionId;

	// неопредленный идентификатор подписки
	const SubscriptionId UndefinedSubId = (-1);

	struct SubscriptionOptions {
		SubscriptionOptions() 
		{
			persiatance = false;
			lifeTime	= 0;
			sendToAgent = false;
			local		= false;
			useSubscriberConnectionsOnly	= false;
			useOnlyLocalConnection			= true;
		}

		bool			persiatance;	//!< признак постоянной подписки
		unsigned long	lifeTime;		//!< Время жизни подписки
		bool			sendToAgent;	/*!< Флаг посылки подписки агенту. В данном случае 
										*    стандартный процесс распространения данной подписки 
										*	 начентся только в агенте, куда данная подписка
										*    первоначально будет послана. Используется для 
										*	 подписки на события от компонент, прямой связи
										*	 с которыми компонента подписчик не имеет.
										*/
		bool			local;			/*!< Признак сугубо локальной подписки ( подписка
										*    с данным признаком не будет распростроняться на
										*	 другие процессы
										*/
		std::wstring	agentProductName;	/*!< Используется для подписки через агента. Данная */
		std::wstring	agentVersionName;	/*	информация используя для опеределения на какого
											*	агента необходимо переслать подписку
											*/
		bool			useSubscriberConnectionsOnly;	
							/*!< Используется в случае необходимости отсылки подписки на 
								 удаленный компьютер. В случае установки данного фалга 
								 подписка будет отправленна только по соединению созданного
								 от имени подписчика ( Subscriber ) */
		bool			useOnlyLocalConnection;
							/*!< Для рассылки данной подписки будут использоваться 
								только локальные соединений */
														
	};

	class EventSource : public KLTP::ThreadsPool::Worker
	{
		public:

			virtual ~EventSource() {}

			/*!
				\brief Функция подписки на события.
			
					Если клиент не задает один из параметров подписки (имя продукта, версия,
					имя компонента, идентификатор компонента, тип события), 
					то фильтрация событий по этому параметру 
					не производится.  Для фильтра на параметры событий проверка производится
					только на равенство полей, то есть если поле есть и в фильтре и в событии и
					эти поля равны, то событие доставляется подписчику.
			   
				 
				 \param subscriber [in] Компонента подписчика 
						( имя компоненты от лица которой производиться подписка ).
						Используется в информативных целях.
				 \param publisher [in] Компонента публикатора ( имя компонента, события от которой интересуют подписчика )
				 \param type [in] Тип события
				 \param filterBody [in] Фильтр на параметры события
				 \param callback [in]  Callback - функция для получения оповещения о 
                                        возникновении события.
				 \param callbackContext [in] Параметры для callback функции
			*/
			virtual SubscriptionId Subscribe( const KLPRCI::ComponentId &subscriber,
				const KLPRCI::ComponentId &publisher, const std::wstring &type, 
				KLPAR::Params * eventBodyFilter, EventCallback callback,
				void *callbackContext,
				const SubscriptionOptions &subOptions = SubscriptionOptions() )= 0;

			/*!
				\brief Производит отписку подписчика.
				
				  Удаляет подписки соответствующие указанной паре значений 
				  подписчик - публикатор.

				\param subscriber [in] Компонент подписчик
				\param publisher [in] Компонента публикатор. Если = NULL то удаляются все 
						подписки указанного подписчика
			*/
			virtual void Unsubscribe( const KLPRCI::ComponentId &subscriber, 
				const KLPRCI::ComponentId *publisher = NULL ) = 0;

			/*!
				\brief Производит отписку.
				
				  Удаляет подписки с соответствующим идентификатором.

				\param subId [in] Идентификатор подписки				
			*/
			virtual bool Unsubscribe( const SubscriptionId &subId ) = 0;

			/*!
				\breif Возвращает итератор подписок
			*/
			virtual void CreateSubscriptionIterator( SubscriptionIterator **pSubsIterator ) = 0;

			/*!
				\brief Функция опеределяет есть ли для данного типа события подписки.
				
				\param publisher [in] Публикатор события
				\param eventType [in] Тип события
				\param eventBody [in] Параметры события ( может равняться NULL )
				\return true - если на данной типа события есть подписки
			*/
			virtual bool CheckEventsSubscription( const KLPRCI::ComponentId &publisher, 
				const std::wstring &eventType, 
				KLPAR::Params * eventBody = NULL ) = 0;

			/*!
				\brief Функция публикации события.

					Функция распространяется указанное событие по всем подпискам в системе.

				\param event [in] Событие для публикации 
			*/
			virtual void PublishEvent( Event *event ) = 0;

			/*!
				\brief Функция ожидает освобождения очереди событий если 
					есть подпсики от указанной компоненты

				\param componentName [in] - Имя компоненты в транспортном виде
			*/
			virtual void WaitForCleanEventsQueue( const std::wstring &transportComponentName,
				bool remoteConn = false ) = 0;
			

			//!\brief Функция завершения
			virtual void Stop() = 0;
	};

} // namespace KLEV


#endif // KLEV_EVENT_SOURCE_H

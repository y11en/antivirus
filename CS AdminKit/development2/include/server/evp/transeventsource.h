/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TransEventSource.h
 * \author Михаил Кармазин
 * \date 18:40 22.11.2002
 * \brief 
 */


#ifndef _KLTRANSEVENTSOURCE_H__808F894F_2D08_4a03_ABA1_460E42CE1C1D__
#define _KLTRANSEVENTSOURCE_H__808F894F_2D08_4a03_ABA1_460E42CE1C1D__

#include <transport/ev/eventsource.h>

namespace KLEVP {

	typedef struct
	{
		KLPRCI::ComponentId componentId;
		std::wstring wstrHostDomain;
		std::wstring wstrHostName;
	} caller_id_t;

	/*!
	\brief Callback - функция для получения информации о событии, 
		 возбужденном в компоненте продукта.  Реализация этой функции должна быть 
		 потокобезопасной. Если функция выбрасывает исключение, то событие считаеться
		 обработанным некорректно,и транзакция откатываеться.
		 Если все callback'и отработали нормально (т.е. без выброса исключений), то
		 происходит Commit транзакции.

		\param caller		[in]  Данные компонента, сообщившего о событии
		\param pDatabase    [in]  База данных, в рамках которого идет транзакция.
					Реальный тип передаваемого значения зависит от реализации.
		\param eventType    [in]  Тип события.
		\param eventBody    [in]  Параметры события.
		\param time         [in]  Время возникновения события.
		\param pParam		[in]  Параметр, переданный при подписке на событие.
	*/
	typedef void (*TransactionalEventCallback)(
		const caller_id_t& caller,
		void* pDatabase,
		const std::wstring & eventType,
		const KLPAR::Params * eventBody,
		KLSTD::precise_time_t tEventRiseTime,
		void* pParam );



    /*!
    * \brief Интерфейс для подписки и отказа от подписки для транзакционных событий
	*		
    */

    class KLSTD_NOVTABLE TransEventSource : public KLSTD::KLBase {
    public:
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

			\param localId			[in]  ID локального компонента (т.е. части сервера)
			\param remoteId			[in]  ID удаленного компонента (т.е. агента)
			\param eventType        [in]  Тип события.
			\param eventBodyFilter  [in]  Фильтр на параметры события, м.б. равен NULL.
			\param callback         [in]  Callback - функция для получения оповещения о 
											возникновении события.
			\param context          [in]  Контекст подписки для передачи в callback - функцию.
										  Данный указатель должен быть корректным все время
										  подписки.
			\param subscriptionId   [out] Идентификатор подписки для последующего вызова 
											StopSubscription.

		*/
		virtual void Subscribe(
			const std::wstring&			wstrSubscriberPersistentId,
			const std::wstring&			eventType,
			const KLPAR::Params*		eventBodyFilter,
			TransactionalEventCallback	callback,
			void*						context,
			KLEV::SubscriptionId&		subscriptionId )= 0;


		/*!
		\brief Прекращает подписку клиента на события определенного типа.

			Прекращается действие подписки для данного ComponentProxy, 
			параметры которых совпадают с параметрами, переданными в вызове метода.

			\param hSubscribe       [in]  Идентификатор подписки.

		*/
		virtual void StopSubscription( KLEV::SubscriptionId subscriptionId )=0;
	};
}

#endif // _KLTRANSEVENTSOURCE_H__808F894F_2D08_4a03_ABA1_460E42CE1C1D__

// Local Variables:
// mode: C++
// End:


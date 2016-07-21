/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRES/EventsStorage.h
 * \author Андрей Брыксин
 * \date 2002
 * \brief Интерфейс для работы со списком событий, сохраняемых в долговременной памяти.
 *
 */

#ifndef KLPRES_EVENTSSTORAGE_H
#define KLPRES_EVENTSSTORAGE_H

#include <time.h>
#include <string>

#include "std/err/error.h"
#include "std/par/params.h"

#include "kca/prci/componentid.h"

namespace KLPRES {
    /*!
    * \brief Range - диапазон записей. Запись попадает в диапазон, если её ID находится между
        first и second, или равен одному их этих значений. Допускается перемена местами
        значений first и second.
    */
    typedef std::pair<long, long> Range;

    typedef std::vector<Range> RangeVector;


	// События генерируемые PRES при добавлении/удалении подписки
	/* const wchar_t PRES_EVENT_SUBSCRIPTION_ADDED[] = L"KLPRES_EVENT_SUBSCRIPTION_ADDED";			// добавлена подписка
	const wchar_t PRES_EVENT_SUBSCRIPTION_DELETED[] = L"KLPRES_EVENT_SUBSCRIPTION_DELETED";		// удалена подписка
	const wchar_t PRES_EVENT_SUBSCRIPTION_UPDATED[] = L"KLPRES_EVENT_SUBSCRIPTION_UPDATED";		// обновлена подписка */

	// События генерируемые PRES при добавлении события
	const wchar_t PRES_EVENT_ADDED[] = L"KLPRES_EVENT_ADDED";	// событие подписки помещено в хранилище
																// событие генерируется раз в 5 секунд если в этот 
																// промежуток было сохранено хотя бы одно событие

	// Событие генерируется PRES при ошибке добавления события в хранилище
	const wchar_t PRES_ERR_ADD_EVENT[] = L"KLPRES_ERR_ADD_EVENT";

	// Имена полей в EventBody
	const wchar_t PRES_EVENT_BODY_PARAM_SUBSCR_NAME[] = L"KLPRES_EVENT_BODY_PARAM_SUBSCR_NAME";		// STRING_T имя добавленной/удаленной подписки

	const int EVENTS_STORING_TIME_INTERVAL_INFINITE = -1;		// Время сохранения событий для подписки неограничено
	const int EVENTS_STORING_NUMBER_INFINITE = -1;				// Количество сохраняемых событий для подписки неограничено
	const int JRNL_PAGE_SIZE_INFINITE = -1;						// размер страницы журнала событий неограничен

    /* ID_ACROSS_PRES_INSTANCE - LONG_T, ID события, уникальное среди событий, сохраненных
        одним экземпляром PRES-сервера. Если два события сохранены одним и тем же
        экземпляром PRESа, то у сохраненного раньше ID будет меньше. */
    const wchar_t ID_ACROSS_PRES_INSTANCE[] = L"PRES_ID_ACROSS_INSTANCE";	

    /* PRES_INSTANCE_ID - STRING_T, GUID - идентификатор экземпляра PRES-сервера. Т.е. пара
        PRES_INSTANCE_ID / ID_ACROSS_PRES_INSTANCE - уникальный идентификатор события, сохраненного в PRESе
    */
    const wchar_t PRES_INSTANCE_ID[] = L"PRES_INSTANCE_ID";

    /*!
    * \brief Имена хранилищ событий
    *  
    */
	const wchar_t EVENT_STORAGE_NAME_LOCAL[] = L"PRES_EVENTS_STORAGE_LOCAL";	// Локальное хранилище

    /*!
    * \brief Параметры события, используемые при формировании фильтра SubscriptionInfo::parEventParametersMask
		Общий формат фильтра:
			Фильтр (Params)
				|
				+--[EVENT_CLASS_MASK_XXX1 (XxxValue)]
				|	....
				+--[EVENT_CLASS_MASK_XXXn (XxxValue)]

		Value всех элементов, кроме EVENT_CLASS_MASK_EVENT_PARAMS игнорируется.
		EVENT_CLASS_MASK_EVENT_PARAMS содержит собственно тело события
    */

	const wchar_t EVENT_CLASS_MASK_PUBLISHER_CID[] = L"KLPRES_ECM_PUBLISHER_SID";	// STRING_T
	const wchar_t EVENT_CLASS_MASK_EVENT_TYPE[] = L"KLPRES_ECM_EVENT_TYPE";			// STRING_T
	const wchar_t EVENT_CLASS_MASK_EVENT_TIME[] = L"KLPRES_ECM_EVENT_TIME";			// DATE_TIME_T
	const wchar_t EVENT_CLASS_MASK_EVENT_LIFETIME[] = L"KLPRES_ECM_EVENT_LIFETIME";	// INT_T
	const wchar_t EVENT_CLASS_MASK_EVENT_PARAMS[] = L"KLPRES_ECM_EVENT_PARAMS";		// PARAMS_T

    class SubscriptionInfo {
	public:
		typedef enum {
			SI_STORE_AS_BINARY = 0,			// Хранить события в бинарном виде
			SI_STORE_AS_TEXT				// Хранить события в виде теста, разделенного символом <Tab> (0x08)
		};

		KLPRCI::ComponentId filter;			///< Фильтр на идентификаторы компонентов для регистрации событий.
		std::vector<std::wstring> eventTypes;		///< Типы событий для регистрации.     
		KLSTD::CAutoPtr<KLPAR::Params> bodyFilter;  ///< Фильтр на тело события для регистрации.
		KLPRCI::ComponentId subscriber;		// Подписчик, для которого предназначены регистрируемые события.
		bool isSubscriptionPersist;			// Является ли подписка долговременной.  Долговременная
											// подписка возобновляется автоматически при старте компонента.
		int maxEvents;		// Максимальный размер (в кб) списка событий, сохраняемых для данной подписки.
		int maxPageSize;	// Максимальный размер страницы журнала (в кб) (см. также JRNL_PAGE_SIZE_INFINITE)
		KLSTD::CAutoPtr<KLPAR::Params> parEventParametersMask;	// Маска параметров события которые будут сохранятся в ES
		std::vector<std::wstring> arEventParametersSortMask;	// Маска параметров события определяющая порядок полей в файле при SI_STORE_AS_TEXT
																// если не задана, то используется сортировка по алфавиту
																// для создания адреса используйте функцию _MakeSortMaskItem
		int nCloseWriteOpenedJournalTimeout;// Время (msec) через которое будет закрыто хранилище событий открытое на запись
		bool bPaused;						// Подписка не регистрирует события

		int nStoreEventsAs;					// Указывает в каком виде хранить события на диске
		wchar_t cFieldDelimiter;			// Задает символ-разделитель для разделения полей события в хранилище (при типе SI_STORE_AS_TEXT)
		std::wstring sEventsListFileName;	// Полный путь к файлу для сохранения событий 
											// (если не задано используются уникальные имена и заданный при инициализации ES сервера каталог)
		bool bUseRecNumIndex;				// Создавать или нет индекс по номеру записи в хранилище
		bool bKeepEventsWhenSubscriptionIsDeleted;	// Оставлять или нет файл с событиями подписки на диске 
													// когда подписка удаляется

		SubscriptionInfo() :
			isSubscriptionPersist(false),
			maxEvents(10),
			maxPageSize(JRNL_PAGE_SIZE_INFINITE),
			nCloseWriteOpenedJournalTimeout(0),
			bPaused(false),
			nStoreEventsAs(SI_STORE_AS_BINARY),
			cFieldDelimiter(L'\t'),
			bUseRecNumIndex(false),
			bKeepEventsWhenSubscriptionIsDeleted(false)
		{
		}
	};

	KLCSKCA_DECL std::wstring SubscriptionInfo_MakeSortMaskItem(std::vector<std::wstring> &arAddress);

	KLCSKCA_DECL long SubscriptionInfo_GetAdjustableMaxEvents(); // Заданный в настройках максимальный размер (в кб) списка событий, сохраняемых для подписки.
	KLCSKCA_DECL void SubscriptionInfo_SetAdjustableMaxEvents(long lAdjustableMaxEvents);

    /*!
    * \brief Интерфейс для работы со списком событий, сохраняемых в долговременной 
    *        либо оперативной памяти.
    *  
    *  Все компоненты на локальной машине работают с одним и тем же хранилищем событий.
    *  Компонент может работать с хранилищем событий независимо от того, запущен на машине
    *  агент администрирования или нет.  Хранилище событий предоставляет следующую
    *  функциональность:
    *
    *   1. Возможность для компонента записывать требуемые события в хранилище событий.
    *   2. Возможность получать события из хранилища событий.
    *   3. Возможность временной либо постоянной подписки на события.
    *   4. Возможность просмотреть и редактировать список подписок для компонента.
    *  
    */

    class EventsStorage : public KLSTD::KLBase {
    public:

    /*!
      \brief Устанавливает, какие события должны регистрироваться компонентом
            в Events Storage.

        \param info         [in]  Информация о подписке.
		\param subscription [in/out]  Идентификатор подписки. Если передаваемая строка не пустая, 
									  то она используется как идентификатор новой подписки, иначе -
									  будет создан новый уникальный идентификатор.
    */
        virtual void StartSubscription (
            const SubscriptionInfo & info,
			std::wstring &subscription )  = 0;

    /*!
      \brief Обновляет параметры заданной подписки в Events Storage.

		\param subscription [in]  Идентификатор подписки. Если передаваемая строка не пустая, 
									  то она используется как идентификатор новой подписки, иначе -
									  будет создан новый уникальный идентификатор.
        \param info         [in]  Информация о подписке.
								  !!! На данный момент возможно изменение поля eventTypes и bodyFilter. 
								  !!! Остальные поля игнорирются.

    */
	    virtual void UpdateSubscription(
			const std::wstring subscription, 
			const SubscriptionInfo &info) = 0;

    /*!
      \brief Подписка перестаёт регистрировать события(постоянно до команды ResumeSubscription)

		\param subscription [in]  Идентификатор подписки.
    */
	    virtual void PauseSubscription(
			const std::wstring subscription) = 0;

    /*!
      \brief Подписка снова начинает регистрировать события

		\param subscription [in]  Идентификатор подписки.
    */
	    virtual void ResumeSubscription(
			const std::wstring subscription) = 0;
		
    /*!
      \brief Устанавливает на начало итератор для выборки всех подписок
             для некоторого компонента-подписчика.

        \param subscriber   [in]  Идентификатор компонента-подписчика. Если
                                    одно или несколько полей не задано, возвращаются все
                                    подписки для всех компонентов, у которых
                                    совпадают оставшиеся поля.  Если не задано ни
                                    одно из полей, возвращаются все подписки в
                                    EventsStorage.
        \param nTimeout [in]      Для прокси на локальный EventsStorage параметр
            игнорируется. Для прокси на EventsStorage, находящийся в другом
            процессе этот параметр определяет время жизни итератора (в сек.) на
            сервере после заключительного обращения к итератору. Итератор также
            удаляется с сервера при удалении на клиентской стороне прокси
            на EventsStorage, и при достижении конца итератора.
    */
        virtual void ResetSubscriptionsIterator ( 
            const KLPRCI::ComponentId & subscriber,
            int nTimeout = 60 ) = 0;


    /*!
      \brief Возвращает информацию о подписке.


        \param info         [out]  Информация о подписке.
        \param subscription [out]  Идентификатор подписки.

		\return true, если есть еще события в списке.       
    */
        virtual bool GetNextSubscription (
            SubscriptionInfo** info,
            std::wstring &subscription) = 0;


    /*!
      \brief Прекращает подписку с заданным идентификатором.

        \param subscription   [in]  Идентификатор подписки.

       
    */
        virtual void StopSubscription ( const std::wstring subscription )  = 0;


    /*!
      \brief Устанавливает на начало итератор для выборки списка событий,
             сохраняемых для некоторого компонента.  Возвращаемые
             события определяются по идентификатору подписки.

        \param subscription   [in]  Идентификатор подписки.

		\
       
    */
        virtual void ResetEventsIterator (const std::wstring subscription)  = 0;


   /*!
      \brief Переходит на следующее событие в списке событий.  

        \param subscription     [in]   Идентификатор подписки. Если "",
                        то используется подписка, указанная при вызове SetEventsIterator, MoveToEventWithIndex или
                        ResetEventsIterator. Параметр работает только для локального прокси, для GSOAP-прокси игнорируется.

		\return		true если есть событие
    */
        virtual bool NextEvent(const std::wstring & subscription = L"") = 0;

   /*!
      \brief Опредеяет указывает ли итератор на существующее событие или это конец списка

		\return		true если есть событие
    */
        virtual bool IsEvent() = 0;

    /*!
      \brief	SetEventsIterator
        Устанавливает итератор на запись с заданным ID. Если такой записи нет, то
        итератор устанавливается на следующую запись. Если следующеая запись
        отсутствует, то указатель устанавливается на конец журнала (IsEOF()==true).
		\param subscription   [in]  Идентификатор подписки.
        \param	nId [in] - ID записи.
    */
		virtual void SetEventsIterator( const std::wstring subscription, long eventID ) = 0;

    /*!
      \brief	MoveToEventWithIndex
        Устанавливает итератор на N-ю запись от начала. Первая запись имеет индекс 0.
        Если nRecIdx больше или равно количеству записей в журнале, то итератор 
        устанавливается на конец журнала (IsEOF()==true).
		\param subscription   [in]  Идентификатор подписки.
        \param	nIndex [in] - Номер записи от начала. Первая запись имеет номер 0.

    */
        virtual void MoveToEventWithIndex( const std::wstring subscription, long nIndex ) = 0;
        
    /*!
      \brief	GetEventsCount
        Возвращает количество событий.
		\param subscription   [in]  Идентификатор подписки.
        \param	long - количество событий
    */
        virtual long GetEventsCount(const std::wstring subscription) = 0;

   /*!
      \brief Возвращает информацию о событии, возникшем в компоненте 
             и сохраненном в EventsStorage.  
       
        \param id               [out]  Идентификатор компонента, возбудившего событие.
		\param eventType        [out]  Тип события.
        \param eventBody        [out]  Параметры события.
        \param eventTime        [out]  Время возникновения события.
		\param eventID			[out]  Идентификатор события.
        \param subscription     [in]   Идентификатор подписки. Если "",
                        то используется подписка, указанная при вызове SetEventsIterator, MoveToEventWithIndex или
                        ResetEventsIterator. Параметр работает только для локального прокси, для GSOAP-прокси игнорируется.
        
		\return		true если есть событие
    */
        virtual bool GetEvent(
                        KLPRCI::ComponentId & id,
                        std::wstring & eventType,
                        KLPAR::Params ** eventBody,
                        time_t  & eventTime,
						long &eventID,
                        const std::wstring & subscription = L"")  = 0;

   /*!
      \brief Удаляет события от начала до указанного события включительно
       
		\param subscription   [in]  Идентификатор подписки.
		\param StopEventID	  [in]  Идентификатор конечного события.
    */
        virtual void DeleteEvents(const std::wstring subscription, const long StopEventID) = 0;

   /*!
      \brief DeleteOneEventByID Удаляет события с указанным ID в указанной подписке
       
		\param subscription   [in]  Идентификатор подписки.
		\param nEventID	  [in]  Идентификатор события.
    */
		virtual void DeleteOneEventByID(const std::wstring subscription, const long nEventID) = 0;
        
   /*!
      \brief DeleteRanges - удаляет записи из журнала, попадающие в переданные диапазоны.
		\param subscription   [in]  Идентификатор подписки.
		\param	vectRanges [in] - список диапазонов записей, которые надо удалить.
    */
		virtual void DeleteRanges(const std::wstring subscription, const RangeVector & vectRanges) = 0;


    /*

    /*!
      \brief Очищает список событий, сохраняемых для некоторой подписки.

        \param subscription   [in]  Идентификатор подписки.
    */
        virtual void ClearEventsList (const std::wstring subscription) = 0;

    /*!
      \brief Возвращает текущий размер хранилища событий

        \param subscription   [in]  Идентификатор подписки.
    */
        virtual AVP_longlong GetEventStorageFileSize (const std::wstring subscription) = 0;

    /*!
      \brief Удаляет из подписки просроченные и самые старые по дате события,
			 превышающие maxEvents подписки.

        \param subscription   [in]  Идентификатор подписки.
    */
        virtual void RemoveExpiredEvents (const std::wstring subscription)  = 0;

    /*!
      \brief Устанавливает на начало итератор для выборки списка событий,
             сохраняемых для некоторого компонента.  Возвращаемые
             события определяются по идентификатору подписки.

        \param subscription   [in]  Идентификатор подписки.

		Возвращает идентификатор файла журнала, который генерируется всякий раз, 
		когда создается новый Central File
    */
        virtual std::wstring ResetEventsIteratorEx(const std::wstring& subscription) = 0;
		
    /*!
      \brief	SetEventsIterator
        Устанавливает итератор на запись с заданным ID. Если такой записи нет, то
        итератор устанавливается на следующую запись. Если следующеая запись
        отсутствует, то указатель устанавливается на конец журнала (IsEOF()==true).
		\param subscription   [in]  Идентификатор подписки.
        \param	nId [in] - ID записи.

		Возвращает идентификатор файла журнала, который генерируется всякий раз, 
		когда создается новый Central File
    */
		virtual std::wstring SetEventsIteratorEx(const std::wstring subscription, long eventID) = 0;
    };
    
}

/*!
  \brief Создает proxy-объект для EventsStorage

    \param sServerObjectID		[in] Уникальный Идентификатор для доступа к экземпляру серверного объекта
	\param cidLocalComponent	[in] Наименование клиента, от лица которого будет производиться удаленные вызовы
	\param cidComponent			[in] Наименование удаленного клиента (Event Storage Server)
    \param ppEventsStorage      [out] Указатель на proxy-объект
	\param bLocalProxy			[in] Использовать или нет транспорт для доступа к серверной части
*/
KLCSKCA_DECL void KLPRES_CreateEventsStorageProxy(const std::wstring sServerObjectID, 
												const KLPRCI::ComponentId& cidLocalComponent, 
												const KLPRCI::ComponentId& cidComponent, 
												KLPRES::EventsStorage** ppEventsStorage, 
												bool bLocalProxy = false);
/*!
  \brief Создает объект EventsStorage

    \param sServerObjectID					[in] Уникальный Идентификатор экземпляра серверного объекта
    \param sProductName						[in] Имя продукта
    \param sProductVersion					[in] Версия продукта
    \param sSubscriptionStorageFilePath     [in] Полный путь к файлу хранилища Subscriptions
	\param sEventsStorageFolder			    [in] Полный путь к папке хранилищ Events
    \param bSubscribeToEventsToStore        [in] Если true, то сервер будет подписываться на события,
                                                    которые он должен сохранять в подписках.
                                                    Если false, то сервер будет использоваться только
                                                    как средство доступа к подпискам (через инт-с
                                                    EventsStorage), в т.ч. для
                                                    добавления/обновления/удаления подписок.
*/
KLCSKCA_DECL void KLPRES_CreateEventsStorageServer(const std::wstring sServerObjectID, 
												 const std::wstring sProductName,
												 const std::wstring sProductVersion,
												 const std::wstring sSubscriptionsStorageFilePath,
												 const std::wstring sEventsStorageFolder,
                                                 bool bSubscribeToEventsToStore = true );

/*!
  \brief Создает объект EventsStorage

    \param sServerObjectID					[in] Уникальный Идентификатор экземпляра серверного объекта
    \param cidAgent							[in] Component ID агента
    \param sSubscriptionStorageFilePath     [in] Полный путь к файлу хранилища Subscriptions
	\param sEventsStorageFolder			    [in] Полный путь к папке хранилищ Events
    \param bSubscribeToEventsToStore        [in] Если true, то сервер будет подписываться на события,
                                                    которые он должен сохранять в подписках.
                                                    Если false, то сервер будет использоваться только
                                                    как средство доступа к подпискам (через инт-с
                                                    EventsStorage), в т.ч. для
                                                    добавления/обновления/удаления подписок.
*/
KLCSKCA_DECL void KLPRES_CreateEventsStorageServer2(const std::wstring sServerObjectID, 
												 const KLPRCI::ComponentId& cidAgent, 
												 const std::wstring sSubscriptionsStorageFilePath,
												 const std::wstring sEventsStorageFolder,
                                                 bool bSubscribeToEventsToStore = true );

///*!
//  \brief Создает объект EventsStorage
//
//    \param sServerObjectID					[in] Уникальный Идентификатор экземпляра серверного объекта
//    \param sProductName						[in] Имя продукта
//    \param sProductVersion					[in] Версия продукта
//    \param sSubscriptionStorageFilePath     [in] Полный путь к файлу хранилища Subscriptions
//	\param sEventsStorageFolder			    [in] Полный путь к папке хранилищ Events
//*/
//KLCSKCA_DECL void KLPRES_CreateEventsStorageServerAndListener(const std::wstring sServerObjectID, 
//												 const KLPRCI::ComponentId& cidComponent,
//												 const std::wstring sProductName,
//												 const std::wstring sProductVersion,
//												 const std::wstring sSubscriptionsStorageFilePath,
//												 const std::wstring sEventsStorageFolder);
//
/*!
  \brief удаляет серверный объект EventsStorage

    \param sServerObjectID		[in] Уникальный Идентификатор для доступа к экземпляру серверного объекта
	\param bWaitEventQueueEmpty	[in] Ждать пока все события в очереди EventsSource будут сохранены в хранилище
									 ВЫЗЫВАТЬ ПРИ ДЕИНИЦИАЛИЗАЦИИ ПРОГРАММЫ
*/
KLCSKCA_DECL void KLPRES_DeleteEventsStorageServer(const std::wstring sServerObjectID, bool bWaitEventQueueEmpty = false);

#endif // KLPRES_EVENTSSTORAGE_H

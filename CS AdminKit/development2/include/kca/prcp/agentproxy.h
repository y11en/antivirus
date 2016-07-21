/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	prcp/AgentProxy.h
 * \author	Andrew Kazachkov
 * \date	01.10.2002 13:51:07
 * \brief	Интерфейс агента администрирования для остальных компонентов продукта.
 * 
 */

#ifndef KLPRCP_AGENTPROXY_H
#define KLPRCP_AGENTPROXY_H

#include <string>

#include "std/err/error.h"
#include "std/par/params.h"
#include "kca/prci/componentid.h"
#include "kca/prci/componentinstance.h"
#include "kca/prts/tasksstorage.h"
#include "kca/pres/eventsstorage.h"
#include "kca/prcp/componentproxy.h"
//#include <bl/agentbusinesslogic.h>

namespace KLPRCP {

	const wchar_t c_CID_ProductName[]	= KLPRCI_PRODUCT_NAME;
	const wchar_t c_CID_Version[]		= KLPRCI_PRODUCT_VERSION;
	const wchar_t c_CID_ComponentName[] = KLPRCI_COMPONENT_NAME;
	const wchar_t c_CID_InstanceId[]	= KLPRCI_INSTANCE_ID;

	const wchar_t c_LauncherComponentId[]	= L"LauncherComponentId";
	const wchar_t c_AsyncId[]				= KLPRCI_ASYNCID;

	const wchar_t c_EventComponentStarted[]		= L"ComponentStarted";
	const wchar_t c_EventComponentShutdowned[]	= L"ComponentShutdowned";

	//! События о состоянии dialup соединения
	const wchar_t c_EventDialupConnectionEstablished[]	= L"Dialup Established";
	const wchar_t c_EventDialupConnectionBroken[]		= L"Dialup Broken";

	const wchar_t c_evpUID[]=L"KLPRCP_UID";

	//! Событие о результате запуска задачи из TS
	const wchar_t c_EventTaskTsResult[]=L"KLPRCP_EVENT_TASK_TS_RESULT";
	/*!
		Тело события имеет следующую структуру:
		c_evpUID
		Кроме того,
		- в случае успеха
			KLBLAG::c_szwServerProduct		- Имя продукта 
			KLBLAG::c_szwServerVersion		- Версия продукта 
			KLBLAG::c_szwServerComponent	- Имя компонента
			KLBLAG::c_szwServerTaskName		- Имя задачи
			KLBLAG::c_szwServerInstance		- Имя экземпляра компонента, на котором была запущена задача
			сKLBLAG::_szwTaskId				- Идентификатор запущенной задачи
		- в случае неудачи
			KLBLAG::c_szwErrorInfo			- 
				KLBLAG::c_szwErrorModule	- Имя модуля, в котором произошла ошибка
				KLBLAG::c_szwErrorCode		- Код ошибки
				KLBLAG::c_szwErrorMsg		- Сообщение об ошибке
				KLBLAG::c_szwErrorFileName	- Имя файла, в котором проиошла ошибка
				KLBLAG::c_szwErrorLineNumber	- Строка, на которой произошла ошибка
	*/
    
    const wchar_t c_EventPingTimeoutElapsed[]=L"KPRCP_PING_TIMEOUT_ELAPSED";
    /*!
        c_CID_ProductName
        c_CID_Version
        c_CID_ComponentName
        c_CID_InstanceId
    */


	/*!
		\brief Флаги для метода StartComponentEx.
	*/

	enum{		
		SCF_AUTOSTOP=1,			/*!	Экземпляр компонента получает команду
									INSTANCE_AUTOSTOP каждый раз, когда счётчик
									ссылок на него переходит из 1 в 0 (т.е. к
									нему нет ни одной прокси, и не запущено ни
									одной задачи).
								*/

		SCF_SYNC=2,				/*!	Синхронный запуск: метод не вернёт управление,
									пока запущенный компонент не проинициализируется.
								*/

		SCF_SHAREPROCESS=8,		/*!	Только для компонентов типа MTF_DLL. Компонент
									будет загружен в адресное пространство процесса,
									в котором был вызван метод.
								*/
		SCF_LOADTOAGENT=0		/*!	Только для компонентов типа MTF_DLL. Компонент
									будет загружен в адресное пространство агента.
									Это значение используется по умолчанию.
								*/
	};


	/*!
		\brief Типы логона (только для компонентов типа MTF_EXE).
	*/


	enum{
		/*! Только для компонентов типа MTF_EXE. Компонент будет запущен в
			контексте логон-сессии агента. Это значение используется по
			умолчанию.
		*/
		LT_SHAREAGENTLOGON=0,	


		/*!	Только для компонентов типа MTF_EXE. Компонент будет запущен в
			контексте учётной записи нити, вызвавшей метод StartComponentEx.
			Может использоваться совместно с флагом LT_LOADPROFILE.
		*/	
		LT_LOGON_THIS=1,


		/*! Только для компонентов типа MTF_EXE. Для запуска компонента будет
			создана новая logon session с заданными szwUser, szwDomain,
			szwPassword. Может использоваться совместно с флагом
			LT_LOADPROFILE. Учётная запись должна иметь право
			SE_INTERACTIVE_LOGON_NAME. 
		*/
		LT_LOGON_INTERACTIVE=2,	


		/*! Только для компонентов типа MTF_EXE. Для запуска компонента будет
			создана новая logon session с заданными szwUser, szwDomain,
			szwPassword. Может использоваться совместно с флагом
			LT_LOADPROFILE. Учётная запись должна иметь право
			SE_SERVICE_LOGON_NAME.
		*/
		LT_LOGON_SERVICE=3,

		/*!	Только для компонентов типа MTF_EXE. Загрузка профиля учётной
			записи. Игнорируется для LT_SHAREAGENTLOGON.
		*/
		LT_LOADPROFILE=0x80000000
	};


    /*!
    * \brief Главный интерфейс агента администрирования.
    *  
    *  Данный интерфейс предоставляет функциональность агента администрирования 
    *  для других компонентов продукта. 
    *
    */


    class KLSTD_NOVTABLE AgentProxy : public ComponentProxy
	{
    public:

    /*!
      \brief
			Запускает компонент продукта асинхронно или синхронно. 
		Результат вызова возвращается в событии ComponentInstanceStarted.
		Идентификатор экземпляра запускаемого компонента определяет сам
		запущенный компонент. Событие ComponentInstanceStarted содержит поле
		asyncId с идентификатором, переданным клиентом.
			В случае асинхронного запуска, клиент может узнать, что стартовал
		именно тот компонент, который он запрашивал, только подписавшись
		на это событие.
			Если вызов синхронный, метод вернёт управление после прихода
		события ComponentInstanceStarted для запущенного компонента, или в
		случае истечения таймаута lAsyncTimeout (в этом случае будет выброшено
		исключение KLSTD::STDE_TIMEOUT) - подписываться на событие
		ComponentInstanceStarted нет необходимости.

       
        \param id		[in/out] Идентификатор компонента для запуска.
        \param pParameters	[in] Параметры для старта компонента.
        \param wstrAsyncId	[in] Уникальный идентификатор для асинхронного 
									события о старте компонента.  Это значение
									содержится в поле asyncId в теле события
									ComponentInstanceStarted. Данный параметр
									является обязательным только в случае,
									если компонент запущен синхронно (указан
									флаг SCF_SYNC).
		\param dwFlags		[in] Флаги SCF_*.
		\param lAsyncTimeout[in] Максимальное время старта и инициализации
									компонента, мс. Данный параметр игнорируется,
									если не указан флаг SCF_SYNC.
		\param lTimeoutStop	[in] Время после последнего обращения к экземпляру
									компонента, через которое он получит
									команду INSTANCE_TIMEOUTSTOP (если атрибут
									равен KLSTD_INFINITE, то никогда), мс.
		\param lLogonType	[in] Тип логона. Игнорируется, если тип компонента
									отличен от  MTF_EXE. Одно из значений LT_*.									
		\param szwUser		[in] Имя учётной записи, под которой будет запущен
									компонент. Игнорируется, если lLogonType не
									равен LT_LOGON_INTERACTIVE. Только для
									компонентов типа MTF_EXE.
		\param szwDomain	[in] Имя домена. Игнорируется, если lLogonType не
									равен LT_LOGON_INTERACTIVE.
		\param szwPassword	[in] Пароль.Игнорируется, если lLogonType не
									равен LT_LOGON_INTERACTIVE.
    */

       virtual void StartComponentEx( 
						KLPRCI::ComponentId&	id,
						KLPAR::Params*			pParameters,
						std::wstring			wstrAsyncId,
						AVP_dword				dwFlags,
						long					lAsyncTimeout,
						long					lTimeoutStop=KLSTD_INFINITE,
						long					lLogonType=LT_SHAREAGENTLOGON,
						const wchar_t*			szwUser=NULL,
						const wchar_t*			szwDomain=NULL,
						const wchar_t*			szwPassword=NULL)  = 0;

    /*!
      \brief Запускает компонент продукта.  
      
        Вызов асинхронный. Результат вызова возвращается в событии 
        ComponentInstanceStarted.  Идентификатор копии запускаемого
        компонента определяет система администрирования. Событие
        ComponentInstanceStarted содержит поле asyncId с идентификатором,
        переданным клиентом.  Таким образом клиент может узнать, что стартовал
        именно тот компонент, который он запрашивал.

       
        \param id           [in]  Идентификатор компонента для запуска.
        \param pParameters  [in]  Параметры для старта компонента.
        \param asyncId      [in]  Уникальный идентификатор для асинхронного 
                                  события о старте компонента.  Это значение
                                  содержится в поле asyncId в теле события
                                  ComponentInstanceStarted.
		\param bAutoStop	[in]  Если true, то экземпляр компонента
									получает команду INSTANCE_AUTOSTOP каждый
									раз, когда счётчик ссылок на него переходит
									из 1 в 0 (т.е. к нему нет ни одной прокси,
									и не запущено ни одной задачи).
		\param bStartInThisProcess [in] Если true, то компонента будет запущена в 
									рамках вызвавшего данную функцию процесса, если
									false компонетна будет запущена в рамках
									процесса агента.
		\param lTimeoutStop	[in]  Время после последнего обращения к экземпляру
									компонента, через которое он получит
									команду INSTANCE_TIMEOUTSTOP (если атрибут
									равен KLSTD_INFINITE, то никогда).

    */
       virtual void StartComponent( 
		   const KLPRCI::ComponentId&	id,
		   KLPAR::Params*				pParameters,
		   std::wstring					asyncId,
		   bool							bAutoStop=true,
		   bool							bStartInThisProcess = false,
		   long							lTimeoutStop=KLSTD_INFINITE)  = 0;

	/*!
      \brief Запускает компонент продукта.  
      
        Вызов синхронный. Клиент ожидает, пока компонент не будет запущен Агентом.
        В случае, если истекает timeout на запуск компонента, клиент получает
        соответствующее исключение STDE_TIMEOUT.
       
        \param id        [in/out] Идентификатор компонента для запуска.
									В случае успешного запуска, поле id.instanceId обновляется 
									идентификатором запущенного экземпляра.
        \param pParameters  [in]  Параметры для старта компонента.
        \param timeout      [in]  Максимальное время старта и инициализации компонента (msec).
                                  Если значение timeout < 0, то время ожидания бесконечное.
		\param bAutoStop	[in]  Если true, то экземпляр компонента
									получает команду INSTANCE_AUTOSTOP каждый
									раз, когда счётчик ссылок на него переходит
									из 1 в 0 (т.е. к нему нет ни одной прокси,
									и не запущено ни одной задачи).
		\param lTimeoutStop	[in]  Время после последнего обращения к экземпляру
									компонента, через которое он получит
									команду INSTANCE_TIMEOUTSTOP (если атрибут
									равен KLSTD_INFINITE, то никогда).

    */
       virtual void StartComponentSync( 
				KLPRCI::ComponentId&		id,
				KLPAR::Params*				pParameters,
				long						timeout=KLSTD_INFINITE,
				bool						bAutoStop=true,
				bool						bStartInThisProcess = false,
				long						lTimeoutStop=KLSTD_INFINITE) = 0;



    /*!
      \brief Возвращает список запущенных копий компонентов.  
      
        \param filter       [in]  Фильтр на идентификатор компонента для получения списка.
        \param components   [out] Списко идентификаторов запущенных копий компонентов.

    */

       virtual void GetComponentsList(
		   const KLPRCI::ComponentId&			filter,
		   std::vector<KLPRCI::ComponentId>&	components) = 0;

    /*!
      \brief Проверяет, запущен ли данный компонент.
       
        \param id      [in]  Идентификатор компонента.
		\return		true -- запущен

    */
        virtual bool IsComponentStarted(
					const KLPRCI::ComponentId&	id)  = 0;


	/*!
	  \brief	Возвращает информацию о компоненте.

	  \param	id [in]			Идентификатор экземпляра компонента
	  \param	ppParams [out]	Контейнер с атрибутами экземпляра компонента.
				Может содержать атрибуты
					KLPRCI::c_AutoStopAllowed, BOOL_T,	экземпляр компонента
						получает команду INSTANCE_AUTOSTOP каждый раз, когда
						счётчик ссылок на него переходит из 1 в 0 (т.е. к нему
						нет ни одной прокси, и не запущено ни одной задачи).
					KLPRCI::c_TimeoutStopAllowed, INT_T, время после последнего
						обращения к экземпляру компонента, через которое он получит
						команду INSTANCE_TIMEOUTSTOP (если атрибут отсутствует в
						контейнере или равен KLSTD_INFINITE, то никогда).
	*/
		virtual void GetInstanceInfo(
					const KLPRCI::ComponentId&	id,
					KLPAR::Params**				ppParams)=0;

    /*!
      \brief Подписывает клиента на получение оповещений о 
            возникновении событий и устанавливает callback для получения оповещений.
       
        Если клиент не задает один из параметров подписки (имя продукта, версия,
        имя компонента, идентификатор компонента, тип события), 
        то фильтрация событий по этому параметру 
        не производится.  Для фильтра на параметры событий проверка производится
        только на равенство полей, то есть если поле есть и в фильтре и в событии и
        эти поля равны, то событие доставляется подписчику.

        Клиент может подписаться на несколько типов событий.  В этом случае он получит
        отдельные нотификации для каждого из подходящих по условиям подписки событий.

        \param componentFilter  [in]  Фильтр на идентификаторы компонентов для 
                                        подписки на события.
		\param eventType        [in]  Тип события.
        \param eventBodyFilter  [in]  Фильтр на параметры события.
        \param callback         [in]  Callback - функция для получения оповещения о 
                                        возникновении события.
        \param context          [in]  Контекст подписки для передачи в callback - функцию.
                                      Данный указатель должен быть корректным все время
                                      подписки.
        \param hSubscribe       [out] Идентификатор подписки для дальнейшего прекращения подписки.

    */

       virtual void SubscribeEx( 
                        const KLPRCI::ComponentId&	componentFilter, 
                        const std::wstring&			eventType,
                        const KLPAR::Params*		eventBodyFilter,
                        EventCallback				callback,
                        void*						context,
						KLPRCI::HSUBSCRIBE&			hSubscribe) = 0;


    /*!
      \brief Прекращает подписку клиента на события определенного типа.
       
        Прекращается действие всех подписок для данного AgentProxy, 
        параметры которых совпадают с параметрами, переданными в вызове метода.

        \param hSubscribe   [in] Идентификатор подписки, полученный в SubscribeEx.
    */

       virtual void StopSubscriptionEx(KLPRCI::HSUBSCRIBE hSubscribe)  = 0;


    /*!
      \brief Возвращает объект класса TasksStorage для работы со списком запланированных
                к выполнению задач.
       
        \param storage       [out] Объект класса TasksStorage.

    */
        virtual void GetTasksStorage( 
                        KLPRTS::TasksStorage ** storage)  = 0;


    /*!
      \brief Возвращает объект класса EventsStorage для работы со списком cобытий,
            регистрируемых в долговременной памяти.
       
        \param storage       [out] Объект класса EventsStorage.

    */
        virtual void GetEventsStorage( 
                        KLPRES::EventsStorage ** storage)  = 0;

	/*!
		
      \brief Создает новый объект ComponentProxy для указанного клиентом компонента.

        Если несколько компонентов подходят к идентификатору компонента, переданному в
        параметре id, то возвращается указатель на произвольный компонент.
       
        \param id      [in]  Идентификатор компонента получения proxy компонента.
        \param proxy   [out] Proxy - объект для компонента.

    */
        virtual void CreateComponentProxy( 
							const KLPRCI::ComponentId & id,
							ComponentProxy ** proxy) = 0;

	/*!		
		\brief Запускает задачу из Хранилища задач.

        \param wstrTaskId	[in]	идентификатор задачи в Хранилище
		\param lTimeout		[in]	Таймаут ожидания запуска задачи, мс
        \param idComponent	[out]	идентификатор компонента
        \param wstrTaskName	[out]	имя задачи
        \param lTaskId		[out]	идентификатор запущенной задачи
	*/

		virtual void StartTasksFromTS(
						const std::wstring&			wstrTaskId,
						long						lTimeout,
						KLPRCI::ComponentId&		idComponent,
						std::wstring&				wstrTaskName,
						long&						lTaskId) = 0;

    };
}

/*!
	\brief Метод для создания объекта класса AgentProxy. Создает объект-
		представитель агента администрирования для использования компонентом
		продукта.

	\param wstrProductName [in] Имя продукта.
	\param wstrVersion [in] Версия продукта.
	\param proxy [out] Указатель на переменную, в которую будет записан
		указатель на созданный объект.
*/

KLCSKCA_DECL void KLPRCP_CreateAgentProxy(
							const std::wstring&		wstrProductName,
							const std::wstring&		wstrVersion,
							KLPRCP::AgentProxy**	ppAgentProxy);

/*!
  \brief
	Загружает асинхронно компонент, реализованный в виде ДЛЛ в адресное
	пространство процесса. Результат вызова возвращается в событии
	ComponentInstanceStarted.
   
    \param id		[in/out] Идентификатор компонента для запуска.
    \param pParameters	[in] Параметры для старта компонента.
    \param wstrAsyncId	[in] Уникальный идентификатор для асинхронного 
								события о старте компонента.  Это значение
								содержится в поле asyncId в теле события
								ComponentInstanceStarted.
	\param dwFlags		[in] Флаги SCF_*: SCF_AUTOSTOP.
	\param lAsyncTimeout[in] Данный параметр игнорируется.
	\param lTimeoutStop	[in] Время после последнего обращения к экземпляру
								компонента, через которое он получит
								команду INSTANCE_TIMEOUTSTOP (если атрибут
								равен KLSTD_INFINITE, то никогда), мс.
*/

KLCSKCA_DECL void KLPRCP_LoadInProcessComponent(
						KLPRCI::ComponentId&	id,
						KLPAR::Params*			pParameters,
						std::wstring			wstrAsyncId,
						AVP_dword				dwFlags,
						long					lAsyncTimeout,
						long					lTimeoutStop=KLSTD_INFINITE);


KLCSKCA_DECL bool KLPRCP_UnloadInProcessComponent(KLPRCI::ComponentId& id);

#endif // KLPRCP_AGENTPROXY_H

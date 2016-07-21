/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRCI/ComponentInstance.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Главный интерфейс модуля Product Component Instance (PRCI).
 *
 */

#ifndef KLPRCI_COMPONENTINSTANCE_H
#define KLPRCI_COMPONENTINSTANCE_H

#include <string>

#include "std/base/kldefs.h"
#include "std/err/error.h"
#include "std/par/params.h"

#include "componentid.h"
#include "kca/prci/prci_const.h"
#include "kca/prci/productsettings.h"
#include "producttaskscontrol.h"
#include "productstatistics.h"

namespace KLPRCI
{
		//const wchar_t c_ComponentProcessEnvironmentFormatStr[] = L"componentId=";	
	// имя переменной окружения, в которой передаётся componentId
	const wchar_t c_szwEnvComponentIdName[]=L"KLCOMPONENTID_F55AB293C1A7494D90268A7F20BA5D15";
	const wchar_t c_szwEnvAsyncIdName[]=L"KLASYNCID_23494E258D2641A9A867CD6084B62C8D";

	/*!
		\brief Интерфейс загружаемых DLL.
		DLL зарегистрированные с флагом MSF_CANBESTARTED должны экспортировать
		функции KLPRCIDLLCMP_INIT и KLPRCIDLLCMP_DEINIT (см. ниже). 
        
        Вызов этих функций сериализован в контексте процесса, т.е. одновременно 
        не более чем одна нить может находится внутри любой из этих функций.
        Из функций KLPRCIDLLCMP_INIT, KLPRCIDLLCMP_DEINIT нельзя загружать
        другие компоненты, это может приводить к блокировкам или зацикливанию.

	*/

	/*!
		\brief Инициализация  компонента. Всякий раз, когда производиться старт
		компонента находящегося в DLL, вызывается данная функция с
		идентификатором необходимого компонента. Параметры componentId и
		startingParams предназначены для дальнейшей передачи в функцию
		KLPRCI_CreateDllComponentInstance.
			Внутри InitComponent компонент должен проинициализироваться и
		сразу же вернуть управление.
        
        Внутри функции нельзя загружать другие компоненты!

		EXTERN_C __declspec(dllexport)
			void KLPRCIDLLCMP_INIT(
								KLPRCI::ComponentId&	componentId,
								KLPAR::Params*			startingParams,
								KLERR::Error**			error)
		{			
			//Выполняем какие-либо действия
			//.............................
		};

      \param	componentId [in/out] 
	  \param	startingParams [in] 
	  \param	error [out] 

	*/

	#define KLPRCIDLLCMP_INIT			KLPRCI_InitComponent
	
	typedef void (*InitComponent)(
						KLPRCI::ComponentId&	componentId,
						KLPAR::Params*			startingParams,
						KLERR::Error**			error);



	/*!
		\brief Деинициализация компонента.
		Функция должна вернуть управление только по завершении
		деинициализации экземпляра компонента componentId, т.к. непосредственно
		за вызовом данной функции следует вызов FreeLibrary.
        
        Внутри функции нельзя загружать другие компоненты!

	EXTERN_C __declspec(dllexport)
		void KLPRCIDLLCMP_DEINIT(const KLPRCI::ComponentId&	componentId)
		{			
			//Выполняем какие-либо действия
			//.............................
		};
	
	*/

	#define KLPRCIDLLCMP_DEINIT			KLPRCI_DeinitComponent

	typedef void (*DeinitComponent)(
						const KLPRCI::ComponentId&	componentId);



    //! Возможные состояния компонента. 
    enum ComponentInstanceState {		

        /*! Компонент не запущен.
		*/
		STATE_INACTIVE,

		/*! Компонент только что запущен. К компоненту нет доступа извне.
			Генерируется автоматически при инстанциации ComponentInstance.
			Данное состояние не следует устанавливать непосредственно. 
		*/
        STATE_CREATED,

		/*! Компонент инициализируется. После установки данного состояния
			компонент становится доступен извне (кроме вызова колбэков).
		*/
        STATE_INITIALIZING,  

		/*! Компонент завершил инициализацию и готов к выполнению задач.
			Колбэки могут быть вызваны только после установки данного состояния.
		*/
        STATE_RUNNING,

		/*! Компонент приостановил свою работу.
		*/
        STATE_SUSPENDING,

		/*!
			Компонент начал деинициализацию.  Компонент остаётся доступным
			извне, но колбэки вызваны быть не могут.
		*/
		STATE_DEINITIALIZING,

		/*!
			Произошла фатальная ошибка в компоненте. После установки данного
			состояния компонент перестаёт быть доступным извне.
		*/
        STATE_FAILURE,

		/*!
			Компонент прекращает свою работу. После установки данного
			состояния компонент перестаёт быть доступным извне.
		*/
        STATE_SHUTTING_DOWN,

		/*!
			Компонент уничтожен. Генерируется автоматически при удалении
			ComponentInstance. Данное состояние не следует устанавливать
			непосредственно.
		*/
		STATE_DESTROYED
    };

    enum InstanceAction{
		/*!
			Прекратить выполнение.
		*/
        INSTANCE_STOP,

		/*!
			Приостановить выполнение
		*/
        INSTANCE_SUSPEND,

		/*!
			Возобновить выполнение
		*/
        INSTANCE_RESUME,

		/*
			Не осталось ни одной работающей задачи и ни одной прокси на
			компонент.
		*/
		INSTANCE_AUTOSTOP,

		/*
			Истек интервл неактивности.
		*/
		INSTANCE_TIMEOUTSTOP
    };


    typedef void (*InstanceControlCallback)(
					void*			context,
					InstanceAction	action);


    /*!
    * \brief Главный интерфейс модуля Product Component Instance (PRCI).
    *  
    *  Данный интерфейс является точкой входа для системы администрирования продуктов
    *  компании "Лаборатория Касперского".  Разработчки какого-либо компонента продукта
    *  могут начинать изучение функциональности системы администрирования начиная с этого
    *  компонента.
    *
    *  Компонентом продукта является либо исполняемый файл, либо динамическая библиотека.
    *  Запущенный компонент (component instance) является минимальной единицей адресации
    *  для системы администрирования.
    *
    *  При первоначальной инициализации компонента разрабочик компонента должен завести
    *  объект класса ComponentInstance который представляет компонент продукта в системе
    *  администрирования.  Заведенный компонент является точкой входа для интерфейса 
    *  администрирования и вся функциональность системы администрирования получается с
    *  использованием этого компонента.
    *
    *  Компоненты идентифицируются внутри продукта по имени компонента.  Разрешается
    *  создавать несколько копий одного и того же компонента, в этом случае компоненты
    *  идентифицируются по переменной InstanceId, значение которой уникально для 
    *  компонентов одного и того же типа в рамках одного продукта.
    *
    *  
    *
    */
    
    /*
        Внутри колбэков ComponentInstance запрещены следующие действия.
        1. Вызов методов ComponentInstance::SetInstanceControlCallback,
           ProductTasksControl::SetRunTasksCallback,
           ProductTasksControl::SetTasksControlCallback,
           ProductTasksControl::CallRunTasksCallback,
           ProductSettings::SubscribeOnSettingsChange,
           ProductSettings::CancelSettingsChangeSubscription.
        2. Создание прокси и вызов любых методов ComponentProxy и AgentProxy.
        3. Обращения к SettingsStorage, EventStorage и TasksStorage
    */

    class KLSTD_NOVTABLE ComponentInstance: public KLSTD::KLBaseQI
	{
    public:


	/*!
	  \brief	Возвращает идентификатор компонента.

	  \param	componentId [out] идентификатор компонента
	*/
		virtual void GetComponentId(KLPRCI::ComponentId &componentId) = 0;
    /*!
      \brief	Возвращает параметры запуска экземпляра компонента.
				Если компонент был запущен агентом, то контейнер Params, наряду с параметрами старта,
				будет содержать переменную KLPRCI::c_WasStartedByAgent. В противном случае это будет
				пустой контейнер.
      \param	ppParams [out] параметры запуска экземпляра компонента.
				Вносить изменения в данный контейнер нельзя.

    */
		virtual void GetStartParameters(KLPAR::Params** ppParams)=0;

    /*!
      \brief Возвращает идентификатор компонента.

      Идентификатор компонента является уникальным для всех компонентов одного
      и того же типа, принадлежащих одному и тому же продукту. Идентификатор
      компонента всегда больше нуля.
      \param wstrID [out] Идентификатор компонента.
 
    */

        virtual void GetInstanceId(std::wstring& wstrID) const  = 0;


    /*!
      \brief Возвращает время, в течении которого запущенный компонент должен
                оповестить агента администрирования о своей нормальной работе.

      \return   Максимальное время оповещения о нормальной работе (msec) или
                KLSTD_INFINITE, если компонент не генерирует оповещения.
      
      \exception Данный метод не выбрасывает исключений.
 
    */
        virtual long GetPingTimeout() const  = 0;


    /*!
      \brief Функция оповещения агента администрирования о своей нормальной работе.

    */

        virtual void Ping() const  = 0;


    /*!
      \brief Возвращает текущее состояние компонента.

      \return Состояние компонента.
 
    */

        virtual ComponentInstanceState GetState() const  = 0;


    /*!
      \brief Устанавливает текущее состояние компонента.

      \param state [in] Текущее состояние компонента.
 
    */

        virtual void SetState(ComponentInstanceState state)  = 0;




    /*!
      \brief Возвращает объект класса ProductSettings для получения настроек компонента.

       \param settings [out] Настройки компонента.

    */

        virtual void GetSettings(KLPRCI::ProductSettings** ppSettings) = 0;

    /*!
      \brief Возвращает объект класса ProductTasksControl, используемый для 
            выполнения компонентом административных задач.

       \param ppTasks [out] Объект для работы с административными задачами.

    */

        virtual void GetTasksControl(KLPRCI::ProductTasksControl** ppTasks) = 0;

    /*!
      \brief Возвращает объект класса ProductStatistics, используемый для
            предоставления компонентом статистики своей работы.

       \param ppStatistics [out] Объект для предоставления компонентом статистики работы.

    */

       virtual void GetStatistics(KLPRCI::ProductStatistics** ppStatistics) = 0;

    /*!
		\brief Устанавливает callback-функцию для контроля.

        \param context   [in] Данные, передаваемые в Callback - функцию
		\param callback  [in]  Callback - функция для контроля.
	*/
       virtual void	SetInstanceControlCallback(
						void*					context,
                        InstanceControlCallback	callback)  = 0;

	/*!
		\brief Функция проверяет наличие подписок на данный тип события.

		\param eventType        [in]  Тип события.
        \param eventBody        [in]  Возможные параметры события.
		\return true - если на данный тип события есть подписки
	*/
	   virtual bool CheckEventsSubscription( 
						const std::wstring &eventType, 
						KLPAR::Params * eventBody = NULL ) = 0;

    /*!
		\brief Функция для публикации события.

        \param eventType        [in]  Тип события для публикации.
        \param eventBody        [in]  Параметры события.
        \param lifetime         [in]  Время жизни события (msec).
	*/
		virtual void Publish(
				const std::wstring&	eventType, 
				KLPAR::Params*		eventBody,
				long				lifetime) = 0;



		/*!
		  \brief	Инициирует выгрузку компонента. 
			Вызов данного метода приводит к асинхронному вызову функции
			KLPRCIDLLCMP_DEINIT и выгрузке компонента. Вызов данного метода
			можно выполнить, например, внутри колбэка InstanceControlCallback,
			в ответ на команду INSTANCE_STOP.
			
			Данный метод не имеет эффекта, если компонент не размещён в DLL.
			Метод не выбрасывает исключений.
		*/
		KLSTD_NOTHROW virtual void InitiateUnload() throw() = 0;
        
        KLSTD_NOTHROW virtual std::wstring GetLocation() throw() =0;


        /*!
          \brief	Method set state STATE_SHUTTING_DOWN and destroys 
                    ComponentInstance object.
                    \param bSuccess 
        */
        KLSTD_NOTHROW virtual void Close(bool bSuccess) throw() = 0;
        
    };
	
	//! Контекст безопасности
    class KLSTD_NOVTABLE SecContext : public KLSTD::KLBase
	{
	public:
		virtual bool ThreadImpersonate(bool bThrowException=true) = 0;
		virtual bool ThreadRevert(bool bThrowException=false) = 0;
	};
};


/*!
  \brief	Данную функцию следует вызывать в колбэках для получения контекста
            пользователя, инициировавшего вызов колбэка.

  \param	ppContext указатель на переменную, в которую будет записан указатель
            на интерфейс. 
  \return   Возвращает true в случае успеха и false в случае неудачи.
  \exception Функция не выбрасывает исключений.
*/
KLCSKCA_DECL bool KLPRCI_GetClientContext(KLPRCI::SecContext** ppContext);

namespace KLPRCI
{
	class AutoImpersonate
	{
	public:
		AutoImpersonate(KLPRCI::SecContext* pContext, bool bThrowIfFailed=true)
			:	m_pContext(pContext)
		{
			KLSTD_ASSERT(m_pContext);
			m_pContext->ThreadImpersonate(bThrowIfFailed);
		};

		~AutoImpersonate()
		{
			KLSTD_ASSERT(m_pContext);
			m_pContext->ThreadRevert(false);//< Cannot throw exception from destructor
		};
	protected:
		KLSTD::CAutoPtr<KLPRCI::SecContext>	m_pContext;
	};
};
/*!
    \brief Метод для создания объекта класса ComponentInstance.

    Создает объект-представитель компонента для системы администрирования.
    Клиент модуля должен создать идентификатор копии создаваемого компонента 
    (поле componentId.instanceId) с помощью функции KLPRCI_CreateInstanceId.

        \param componentId      [in]  Идентификатор компонента для системы
                                        администрирования. 
		\param component		[out] Указатель на вновь созданный объект.
        \param pDesirablePorts  [in]  Ограниченный нулём массив желательных для использования
										портов TCP/IP. Если NULL, то порт будет использован
										автоматически. Если все заданные порты окажутся занятыми,
										также будет использован выбранный автоматически порт.
		\param	szwAddress		[in] Адрес. Может принимать следующие значения:
            если NULL, пустая строка или KLPRCI:: c_szAddrLocal -- компонент доступен только локально
            KLPRCI::c_szAddrAny - компонент доступен как локально, так и по сети

		\exception	ERR_INSTNAME_EXISTS Экземпляр компонента с таким InstanceId уже существует.

*/
KLCSKCA_DECL void KLPRCI_CreateComponentInstance(
                    const KLPRCI::ComponentId&	componentId,
					KLPRCI::ComponentInstance**	component,
					unsigned short*				pDesirablePorts=NULL,
					const wchar_t*				szwAddress = NULL);

KLCSKCA_DECL void KLPRCI_CreateComponentInstance2(
                    const KLPRCI::ComponentId&	componentId,
                    const wchar_t*				szwAsyncId,
					KLPRCI::ComponentInstance**	component);


/*!
    \brief Метод для создания объекта класса ComponentInstance.

    Создает объект-представитель компонента для системы администрирования.

        \param componentId      [in]  Идентификатор компонента для системы
                                        администрирования. 
		\param pParams		    [in]  Параметры компонента, переданные в
									функцию KLPRCIDLLCMP_INIT
		\param component		[out] Указатель на вновь созданный объект.
        \param pDesirablePorts  [in]  Ограниченный нулём массив желательных для использования
										портов TCP/IP. Если NULL, то порт будет использован
										автоматически. Если все заданные порты окажутся занятыми,
										также будет использован выбранный автоматически порт.
		\param	szwAddress		[in] Адрес. Может принимать следующие значения:
            если NULL, пустая строка или KLPRCI:: c_szAddrLocal -- компонент доступен только локально
            KLPRCI::c_szAddrAny - компонент доступен как локально, так и по сети

		\exception	ERR_INSTNAME_EXISTS Экземпляр компонента с таким InstanceId уже существует.

*/
KLCSKCA_DECL void KLPRCI_CreateDllComponentInstance(
										const KLPRCI::ComponentId&	componentId,
										KLPAR::Params*				pParams,
										KLPRCI::ComponentInstance**	component,
										unsigned short*				pDesirablePorts=NULL,
										const wchar_t*				szwAddress = NULL);

/*!
    \brief Генерирует идентификатор экземпляра компонента.

	Идентификатор экземпляра компонента представляет собой уникальную строку,
		однозначно идентифицирующее экземпляр во всей сети организации.

	\param wstrWellKnownName [in] "Well-known name" компонента. Компоненты,
				работающие в одном экземпляре имеют "Well-known name", например,
				"Well-known name" агента -- KLPRCI_WELLKNOWN_AGENT. Если компонент
				не имеет "Well-known name", szwWellKnownName должно быть равно NULL.
	\return Идентификатор экземпляра компонента
*/

KLCSKCA_DECL std::wstring KLPRCI_CreateInstanceId(const wchar_t* szwWellKnownName);


/*!
  \brief	KLPRCI_CreateRemoteInstanceId

  \param	szwRemoteHost DNS-имя удалённого хоста.
  \param	szwWellKnownName см. KLPRCI_CreateComponentInstance
*/
KLCSKCA_DECL std::wstring KLPRCI_CreateRemoteInstanceId(
									  const wchar_t* szwRemoteHost,
									  const wchar_t* szwWellKnownName);

/*!
  \brief Возвращает переданнаный для данного процесса componentId

  СomponentId передается запущенному процессу испульзуя environment процесса. Идентификатор
  компоненты передается только в случае запуска процесса агентом администрирования.

  \param	[out] сcomponentIt Идентификатор компонеты

  \return false - для запуска данного процеса идентификатор компоненты не был указан
*/
KLCSKCA_DECL bool KLPRCI_GetStartedProcessComponentId( KLPRCI::ComponentId &componentId );

//KLCSKCA_DECL std::string GetHostDnsName();


/*!
  \brief	Sets behaviour for case when module with absent/mismatched
            signature is attempted to be loaded.

  \param	bFailIfMismatched - fails to load such modules if true
*/
KLCSKCA_DECL void KLPRCI_SetSignCheckMode(bool bFailIfMismatched);

/*!
  \brief	Checks for module signature

  \param	hModule - HMODULE
*/
KLCSKCA_DECL void KLPRCI_CheckModuleSignature(void* hModule);


/*!
  \brief	KLPRCI_GetModuleSignCheckStatus

  \return	true if there is at least one module with absent/mismatched signature
*/
KLCSKCA_DECL bool KLPRCI_GetModuleSignatureMisMatch();

#endif // KLPRCI_COMPONENTINSTANCE_H

// Local Variables:
// mode: C++
// End:

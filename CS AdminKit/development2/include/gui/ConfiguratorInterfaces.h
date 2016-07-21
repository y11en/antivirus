#ifndef __CONFIGURATOR_INTERFACES_H__
#define __CONFIGURATOR_INTERFACES_H__

#include <std/base/klbase.h>

#ifdef CLASSES_EXPORT
#define CALL_SPEC __declspec(dllexport)
#else
#define CALL_SPEC __declspec(dllimport)
#endif

namespace KLPlugin
{
	// Название сообщения - уведомления окна-родителя страницы о ее изменении пользователем
	const wchar_t RWM_KLAKCON_PPN_PAGE_MODIFIED[] = L"RWM_KLAKCON_PPN_PAGE_MODIFIED";
	const wchar_t RWM_KLAKCON_PPN_PAGE_UNMODIFIED[] = L"RWM_KLAKCON_PPN_PAGE_UNMODIFIED";

	// Название сообщения - уведомления страницы плагина о переключении активной подстраницы (для политик WKS6)
	// wParam = WCHAR_T* - текстовый идентификатор подстраницы
	const wchar_t RWM_KLAKCON_PPN_ACTIVATE_SUBPAGE[] = L"RWM_KLAKCON_PPN_ACTIVATE_SUBPAGE";

	// Название сообщения - уведомления страницы плагина о переключении в режим "только просмотр"
	// wParam == 1 - режим ReadOnly
	// lParam = LPARAM* pResult на выходе принимает значение 1 - если сообщение обработано и 0 - в противном случае 
	const wchar_t RWM_KLAKCON_PPN_SET_READONLY[] = L"RWM_KLAKCON_PPN_SET_READONLY";
	
	// Имена интерфейсов предоставляемых плагину консолью (вызовом IEnumInterfaces::QueryInterface())
	const wchar_t KAV_INTERFACE_PROPERTY_SHEET[] = L"IPropertySheet";
	const wchar_t KAV_INTERFACE_POLICY_SETTINGS[] = L"POLICY_SETTINGS";
	const wchar_t KAV_INTERFACE_LOCAL_SETTINGS[] = L"LOCAL_SETTINGS";
	const wchar_t KAV_INTERFACE_TASK_PARAMS[] = L"TASK_PARAMS";
	
	// Этот вызов может занимать длительное время. 
	// Рекомендуется делать вызов на отдельном потоке. Для прерывания операции вызовите метод
	// QueryInterface с ppInterface == NULL : QueryInterface(KAV_INTERFACE_AGENT_PROXY, NULL);
	const wchar_t KAV_INTERFACE_AGENT_PROXY[] = L"AGENT_PROXY";

	const wchar_t KAV_INTERFACE_DATA_PROTECTION[] = L"DATA_PROTECTION";
	const wchar_t KAV_INTERFACE_LICENSING[] = L"LICENSING";
	const wchar_t KAV_INTERFACE_HOSTS[] = L"HOSTS";							// Added in NodesHost v5.0 MP2
	const wchar_t KAV_INTERFACE_TASKS_STORAGE[] = L"HOST_TASKS";			// Added in NodesHost v5.0 MP3
	
	// Этот вызов может занимать длительное время. 
	// Рекомендуется делать вызов на отдельном потоке. Для прерывания операции вызовите метод
	// QueryInterface с ppInterface == NULL : QueryInterface(KAV_INTERFACE_GUI_CALLS, NULL);
	const wchar_t KAV_INTERFACE_GUI_CALLS[] = L"GUI_CALLS";					// Added in NodesHost v6.0 MP2

	// Added in NodesHost v5.0 MP4
	// Возвращает интерфейс KLPKG::PackageConfigurator (include/srvp/pkg/pkgcfg.h)
	const wchar_t KAV_INTERFACE_PACKAGE_SETTINGS[] = L"PACKAGE_SETTINGS";

	// brief Интерфейс для передачи набора интерфейсов плагин.
	interface IEnumInterfaces
	{
		virtual ~IEnumInterfaces (){};
		virtual BOOL QueryInterface( LPCWSTR lpszName, void** ppInterface ) const PURE; 
	};
	
	/*!
	  \brief Наборы интерфейсов к данным, поставляемым к плагинам:
	  
				Вид вызова плагина					Название интерфейса				Тип интерфейса		Предназначение
				GROUP_POLICY						POLICY_SETTINGS					SettingsStorage		Настройки групповой политики
				LOCAL_POLICY						LOCAL_SETTINGS					SettingsStorage		Настройки локальной политики
													POLICY_SETTINGS*				SettingsStorage		Настройки групповой политики
													AGENT_PROXY*					AgentProxy			Интерфейс к прокси агента (для Карантина)
				TASKS								TASK_PARAMS						Params				Настройки задачи
													POLICY_SETTINGS*				SettingsStorage		Настройки групповой политики
				Тестовый плагин						Params*							Params				Исходные данные для редактирования
				Тестовый плагин						ChangesParams*					Params				Измененные данные
				Тестовый плагин						Settings*						Params				Настройки плагина (ширины колонок, etc)
				
			* - Интерфейс может отсутствовать
	*/

	/*!
		\brief Интерфейс страницы
	*/
	interface IKLConfiguratorPage : public KLSTD::KLBase
	{
		virtual ~IKLConfiguratorPage (){};

		/*!
			\brief Передает в класс страницы набор интерфейсов, необходимый для его работы. 
			Если в наборе отсутствуют интерфейсы, необходимые классу,
			генерируется исключение KLERR::Error (KLSTD::STDE_BADPARAM)
			Родительское окно страницы извещается о ее изменении пользователем
			посылкой сообщения, зарегистрированного с именем L"RWM_KLAKCON_PPN_PAGE_MODIFIED"
		*/
		virtual void SetInterfaces( IEnumInterfaces* pIEnumInterfaces ) PURE; 

		/*!
			\brief Возвращает handle окна страницы
		*/
		virtual HWND GetPageHWND() const PURE;

		/*!
			\brief Возвращает TRUE, если какие-то значения в окне страницы были изменены пользователем,
			но эти изменения еще не были применены. В противном случае возвращается FALSE.
		*/
		virtual BOOL IsPageDirty() const PURE;

		/*!
			\brief Вызывается при попытке применения изменений (при нажатии кнопок Apply, OK)
			Если в попытка применить изменения не удалась, генерируется исключение KLERR::Error (KLSTD::STDE_BADFORMAT)
		*/
		virtual void Apply() PURE;

		/*!
			\brief Вызывается при попытке применения изменений (при нажатии кнопок Apply, OK) для проверки 
			правильности введенных пользователем данных. Возвращает TRUE если данные корректны.
			Вызывается в следующих случаях:
				1. В режиме KLPlugin::IKLConfigurator::PROPERTY_PAGE - 
						При нажатии Apply, Ok или при переключении на другую закладку
				2. В режиме KLPlugin::IKLConfigurator::WIZARD97_PAGE - при нажатии кнопки Next
		*/
		virtual BOOL ValidateUserInput() PURE;

		/*!
			\brief Вызывается при обновлении данных, переданных ранее в страницу
		*/
		virtual void NotifyDataUpdated() PURE;

		/*!
			\brief Вызывается в момент активации страницы. Возвращает TRUE, если
			активация возможна, и FALSE, если активация невозможна
		*/
		virtual BOOL NotifyPageSelecting() PURE;

		/*!
			\brief Вызывается после того, как страница стала активной
		*/
		virtual void NotifyPageSelected() PURE;

		/*!
			\brief Вызывается в момент деактивации страницы. Возвращает TRUE, если
			деактивация возможна, и FALSE, если деактивация невозможна
		*/
		virtual BOOL NotifyPageDeselecting() PURE;

		/*!
			\brief Вызывается при попытке сохранить значения по умолчанию.
			Значения по умолчанию всех контролов сохраняются в storage и выводятся на экран.
			Если попытка сохранить значения не удалась, генерируется исключение KLERR::Error (KLSTD::STDE_BADFORMAT)
		*/
		virtual void ApplyDefaultData() PURE;		
	};


	/*! 
		\brief Автоуказатель, использущийся для хранения/ссылки на страницу.
		Внимание! Объекты страниц, на которые ссылаются указатели, должны быть
		уничтожены до момента выгрузки исполняемого кода плагина.
	*/
	typedef KLSTD::CAutoPtr<IKLConfiguratorPage> IKLConfiguratorPagePtr;


/*! Group Policy structure
   
	 При передачи данных групповой политики в страницу используются следующие соглашения:
    Структура данных:
        <container1_name>
         +---<var1_name>=value1 //type1
         +---<var2_name>=value2 //type2


    Соответствующая ей структура политики:
        <container1_name>                              //PARAMS
         +---<c_szwSPS_Value>                          //PARAMS_T
         |          +---<var1_name>                    //PARAMS_T
         |               +---<c_szwSPS_Value>=value1   //type1
         |               +---c_szwSPS_Mandatory        //BOOL_T
         |               +---c_szwSPS_Locked           //BOOL_T
         |               +---c_szwSPS_LockedPolicyName //STRING_T
         |               +---c_szwSPS_LockedGroupName  //STRING_T
         |          +---<var2_name>                    //PARAMS_T
         |               +---<c_szwSPS_Value>=value2   //type2
         |               +---c_szwSPS_Mandatory        //BOOL_T
         |               +---c_szwSPS_Locked           //BOOL_T
         |               +---c_szwSPS_LockedPolicyName //STRING_T
         |               +---c_szwSPS_LockedGroupName  //STRING_T
         +---c_szwSPS_Mandatory                        //BOOL_T
         +---c_szwSPS_Locked                           //BOOL_T
         +---c_szwSPS_LockedPolicyName                 //STRING_T
         +---c_szwSPS_LockedGroupName                  //STRING_T

	//Mandatory attributes
		 // value
		 static const wchar_t c_szwSPS_Value[]=L"KLPRSS_Val";        //
		 // is value mandatory
		 static const wchar_t c_szwSPS_Mandatory[]=L"KLPRSS_Mnd";    //BOOL_T
	//Optional attributes
		 // is value locked ?
		 static const wchar_t c_szwSPS_Locked[]=L"KLPRSS_ValLck";    //BOOL_T
		 // policy name (if value is locked)
		 static const wchar_t c_szwSPS_LockedPolicyName[]=L"KLPRSS_ValLckPolicy";//STRING_T
		 // group name (if value is locked)
		 static const wchar_t c_szwSPS_LockedGroupName[]=L"KLPRSS_ValLckGroup";//STRING_T
*/

/*! Local Policy structure
   
	 При передачи данных локальной политики в страницу используются следующие соглашения:
			- В настройках локальной политики передаются значения настроек с именами и путями
			  к ним, определенные в спецификациях для конкретных приложений
			- При передачи обоих интерфейсов, значения для каждого контрола берутся из настроек
			  групповой политики, если для этой настройки флаг c_szwSPS_Locked выставлен TRUE, 
			  или из настроек локальной политики, если флаг c_szwSPS_Locked групповой политики
			  выставлен FALSE. В случае c_szwSPS_Locked == TRUE из настроек групповой политики
			  выбираются также значения c_szwSPS_LockedPolicyName и c_szwSPS_LockedGroupName. 
*/

/*! Task Params structure
   
	 При передачи данных задачи в страницу используются следующие соглашения:
			- В настройках задачи передается плоская структура Params со значениями настроек,
			  специфицированными именами, определенными в спецификациях для конкретных приложений
			- При передачи обоих интерфейсов, значения для каждого контрола берутся из настроек
			  групповой политики, если для этой настройки флаг c_szwSPS_Locked выставлен TRUE, 
			  или из настроек задачи, если флаг c_szwSPS_Locked групповой политики
			  выставлен FALSE. В случае c_szwSPS_Locked == TRUE из настроек групповой политики
			  выбираются также значения c_szwSPS_LockedPolicyName и c_szwSPS_LockedGroupName. 
*/


	/*!
		\brief Интерфейс конфигуратора страниц
	*/
	interface IKLConfigurator : public KLSTD::KLBase
	{
		virtual ~IKLConfigurator (){};

		/*!
			\brief Enumerator типов страниц. Используется в методе CreatePage (ePageType)
		*/
		typedef enum tagPAGE_TYPES
		{
			PROPERTY_PAGE,
			WIZARD97_PAGE
		} PAGE_TYPES;

		/*!
			\brief Enumerator типов конфигураторов. Используется в методе CreateConfigurator (eConfiguratorType)
		*/
		typedef enum tagCONFIGURATOR_TYPES
		{
			GROUP_POLICY_CONFIGURATOR,
			LOCAL_POLICY_CONFIGURATOR,
			TASK_CONFIGURATOR,
			INSTALLER_CONFIGURATOR
		} CONFIGURATOR_TYPES;

		/*!
			\brief Возвращает количество страниц для данного конфигуратора
		*/
		virtual unsigned long GetPagesCount() const PURE;

		/*!
			\brief Создает страницу с определенным индексом
			Создаваемый объект должен быть уничтожен до выгрузки исполняемого кода плагина
		*/
		virtual IKLConfiguratorPagePtr CreatePage(HWND hParent, unsigned long nIndex ) PURE;

		/*!
			\brief 
		*/
		virtual void GetPageHelpInfo(unsigned long nIndex, LPWSTR* pbstrHelpFilePath,long* pnHelpID)  const PURE;

		/*!
			\brief Возвращает заголовок страницы
		*/
		virtual LPCWSTR GetPageTitle(unsigned long nIndex) const PURE;

		/*!
			\brief Возвращает подзаголовок страницы
		*/
		virtual LPCWSTR GetPageSubtitle(unsigned long nIndex) const PURE;

		/*!
			\brief Возвращает handle пиктограммы, ассоциированной со страницей
		*/
		virtual HICON GetPageIcon(unsigned long nIndex) const PURE;

		/*!
			\brief Передает в класс страницы набор интерфейсов, необходимый для его работы. 
			Если в наборе отсутствуют интерфейсы, необходимые классу,
			генерируется исключение KLERR::Error (KLSTD::STDE_BADPARAM)
			Родительское окно страницы извещается о ее изменении пользователем
			посылкой сообщения, зарегистрированного с именем L"RWM_KLAKCON_PPN_PAGE_MODIFIED"
		*/
		virtual void SetInterfaces( IEnumInterfaces* pIEnumInterfaces ) PURE; 

		/*!
			\brief Вызывается при попытке сохранить значения по умолчанию.
			Значения по умолчанию всех контролов сохраняются в storage и выводятся на экран.
			Если попытка сохранить значения не удалась, генерируется исключение KLERR::Error (KLSTD::STDE_BADFORMAT)
		*/
		virtual void ApplyDefaultData() PURE;		
	};

	
	/*! 
		\brief Автоуказатель, использущийся для хранения ссылки на конфигуратор.
		Внимание! Объекты конфигураторов, на которые ссылаются указатели, должны быть
		уничтожены до момента выгрузки исполняемого кода плагина.
	*/
	typedef KLSTD::CAutoPtr<IKLConfigurator> IKLConfiguratorPtr;


	/*!
		\brief Интерфейс локализатора. Содержит методы локализации информации о событиях и статистиках,
		приложения, управляемого с помощью плагина. Временем жизни классов, реализующих интерфейс, управляет плагин.
	*/
	interface IKLLocalizer
	{
		/*!
			\brief Локализует информацию о событии, сгенерированном приложением, которым управляет плагин.
			Генерирует исключение KLERR::Error (KLSTD::STDE_BADPARAM) в случае передачи некорректных входных данных.
			Использует следующую структуру входных данных:
			UINT nResourceID идентификатор ресурса для строки формата
			LPCWSTR lpszFormat строка форматирования
			Формат строки форматирования следующий:

				<Текст>%<номер параметра><Текст>%<номер параметра><Текст>%<номер параметра><Текст>;;Параметр1;;Параметр2;;Параметр3;;

			где Параметр1, Параметр2, Параметр3 - локализованные названия параметров

			Root Params
				|- wchar_t[] (L"event_id"):						INT_T				
				|- wchar_t[] (L"severity"):						INT_T		
				|- wchar_t[] (L"domain_name"):					STRING_T
				|- wchar_t[] (L"hostname"):						STRING_T
				|- wchar_t[] (L"product_name"):					STRING_T
				|- wchar_t[] (L"product_version"):				STRING_T
				|- wchar_t[] (L"event_type"):						STRING_T
				|- wchar_t[] (L"event_type_display_name"):	STRING_T			// не используется при локализации
				|- wchar_t[] (L"descr"):							STRING_T			// не используется при локализации
				|- wchar_t[] (L"rise_time"):						DATE_TIME_T
				|- wchar_t[] (L"body"):								PARAMS_T			// не используется при локализации
				|- wchar_t[] (L"par1"):								STRING_T			// название параметра варьируется в зависимости от типа события
				|- wchar_t[] (L"par2"):								STRING_T			// название параметра варьируется в зависимости от типа события
				|- wchar_t[] (L"par3"):								STRING_T			// название параметра варьируется в зависимости от типа события
				|- wchar_t[] (L"par4"):								STRING_T			// название параметра варьируется в зависимости от типа события
				|- wchar_t[] (L"par5"):								STRING_T			// название параметра варьируется в зависимости от типа события
				|- wchar_t[] (L"par6"):								STRING_T			// название параметра варьируется в зависимости от типа события

			Cтруктура выходных данных:
			Root Params 1
				|-wchar_t[] (L"GNRL_EA_DESCRIPTION_DISP_NAME"):		STRING_T

			
			Root Params 2
				|- wchar_t[] (L"event_type_display_name"):	STRING_T			
				|- wchar_t[] (<par1>):								STRING_T			// название параметра варьируется в зависимости от типа события
				|- wchar_t[] (<par2>):								STRING_T			// название параметра варьируется в зависимости от типа события
				|- wchar_t[] (<par3>):								STRING_T			// название параметра варьируется в зависимости от типа события
				|- wchar_t[] (<par4>):								STRING_T			// название параметра варьируется в зависимости от типа события
				|- wchar_t[] (<par5>):								STRING_T			// название параметра варьируется в зависимости от типа события
				|- wchar_t[] (<par6>):								STRING_T			// название параметра варьируется в зависимости от типа события
		*/
		virtual void LocalizeEvent (UINT nResourceID, KLPAR::Params* pSrcParams, KLPAR::Params* pTrgParams1, KLPAR::Params* pTrgParams2 ) PURE; 

		/* версия метода для работы без ресурсов*/
		virtual void LocalizeEvent (LPCWSTR lpszFormat, KLPAR::Params* pSrcParams, KLPAR::Params* pTrgParams1, KLPAR::Params* pTrgParams2 ) PURE; 
		
		/*!
			\brief 
			
			!!! OBSOLETE !!! Не будет вызыватся консолью

			Возвращает локализованное название продукта. Алгоритм генерации названия одинаков для всех продуктов.
			Генерирует исключение KLERR::Error (KLSTD::STDE_BADPARAM) в случае передачи некорректных входных данных.
		*/
		virtual std::wstring GetLocalizedProductName (LPCWSTR lpszProductName, LPCWSTR lpszProductVersion) PURE;

		/*!
			\brief Локализует информацию о статистики работы приложения, которым управляет плагин.
			Генерирует исключение KLERR::Error (KLSTD::STDE_BADPARAM) в случае передачи некорректных входных данных.
			Использует следующую структуру входных данных:
			Root Params
				|- KLHST::c_spRtInstInfo:								PARAMS_T				// не используется при локализации
					|- <ProductName>:										PARAMS_T				// имя атрибута=идентификатору продукта
						|- KLHST::c_spProductVersion:					STRING_T				// версия продукта
						|- KLHST::c_spRtInstInfo:						ARRAY_T
								|- Item0										PARAMS_T
								|		|- KLHST::c_spComponentName:	STRING_T				// имя компонента - used for defining componets -> structure of statistics
								|		|- KLHST::c_spInstanceId:		STRING_T				// идентификатор  экземпляра компонента
								|		|- KLHST::c_inst_Statistics:	PARAMS_T				// статистика данного экземпляра компонента
								|				|- <name1>:					type1					
								|				|- <name2>:					type2					// названия статистик и их типов значений варьируются в зависимости от компонента
								|			  ...														// (см. их описание в документе "Описание KCA интерфейсов приложений")
								|				|- <nameN>:					typeN
							................................
								|- ItemN										PARAMS_T
										|- KLHST::c_spComponentName:	STRING_T				// имя компонента - used for defining componets -> structure of statistics
										|- KLHST::c_spInstanceId:		STRING_T				// идентификатор  экземпляра компонента
										|- KLHST::c_inst_Statistics:	PARAMS_T				// статистика данного экземпляра компонента
												|- <name1>:					type1					
												|- <name2>:					type2					// названия статистик и их типов значений варьируются в зависимости от компонента
											  ...														// (см. их описание в документе "Описание KCA интерфейсов приложений")
												|- <nameN>:					typeN

			Cтруктура выходных данных:
			Root Params
				|- <LocStatDescription>:		PARAMS_T				// статистика данного экземпляра компонента. Название узла - локализованное описание набора статистики
						|- <name1>:					type1					
						|- <name2>:					type2					// названия статистик и их типов значений варьируются в зависимости от компонента
					  ...														// (см. их описание в документе "Описание KCA интерфейсов приложений")
						|- <nameN>:					typeN


  			const wchar_t c_spRtInstInfo[]=L"klhst-rt-InstInfo";			//PARAMS_T
			const wchar_t c_spProductVersion[]=L"klhst-ProductVersion";	//STRING_T
			const wchar_t c_spRtTskArray[]=L"klhst-rt-TskArray";			//ARRAY_T<PARAMS_T>
			const wchar_t c_spComponentName[]=L"klhst-ComponentName";	//STRING_T
			const wchar_t c_spInstanceId[]=L"klhst-InstanceId";			//STRING_T
			const wchar_t c_inst_Statistics[]=L"instStatistics";			//PARAMS_T

		*/
		virtual void LocalizeStatistic (KLPAR::Params* pSrcParams, KLPAR::Params* pTrgParams) PURE;	

	};


	/*!
		\brief Интерфейс доступа к реализуемой плагином функциональности.
		Временем жизни классов, реализующих интерфейс, управляет плагин.
	*/
	interface IKLAccessor
	{
		/*!
			\brief Создает объект конфигуратора заданного типа.
			Создаваемый объект должен быть уничтожен до выгрузки исполняемого кода плагина
			При создании конфигуратора для задачи, необходимо указывать ее идентификатор
			во втором параметре метода, в остальных случаях второй параметр смысла не имеет/
		*/
		virtual IKLConfiguratorPtr CreateConfigurator(IKLConfigurator::CONFIGURATOR_TYPES eConfiguratorType, 
																	 IKLConfigurator::PAGE_TYPES ePageType, 
																	 LPCWSTR lpszTaskID = NULL) PURE; 

		/*!
			\brief Возвращает ссылку на объект локализатора. Обязанность за удаление объекта лежит на плагине.
		*/
		virtual IKLLocalizer* GetLocalizer() PURE; 


		/*!
			\brief Возвращает список задач, поддерживаемых плагином. Выходная структура данных имеет следующий вид:
				Root Params
					|- wchar_t[] (L"Tasks"): ARRAY_T
							|- Item0
							|		|- wchar_t[] (L"Component"):			 STRING_T	// Имя компонента, выполняющего задачи данного типа
							|		|- wchar_t[] (L"TaskID"):				 STRING_T	// Идентификатор задачи (см. описание в документе "Описание KCA интерфейсов приложений")
							|		|- wchar_t[] (L"LocalizedTaskName"): STRING_T	// Локализованное имя задачи
							|- Item1
							|		|- wchar_t[] (L"Component"):			 STRING_T	// Имя компонента, выполняющего задачи данного типа
							|		|- wchar_t[] (L"TaskID"):				 STRING_T	// Идентификатор задачи (см. описание в документе "Описание KCA интерфейсов приложений")
							|		|- wchar_t[] (L"LocalizedTaskName"): STRING_T	// Локализованное имя задачи
						............														
							|- ItemN
									|- wchar_t[] (L"Component"):			 STRING_T	// Имя компонента, выполняющего задачи данного типа
									|- wchar_t[] (L"TaskID"):				 STRING_T	// Идентификатор задачи (см. описание в документе "Описание KCA интерфейсов приложений")
									|- wchar_t[] (L"LocalizedTaskName"): STRING_T	// Локализованное имя задачи
		*/
		virtual void GetTasks(KLPAR::Params* pParams) PURE; 

		/*!
			\brief Возвращает список событий, поддерживаемых плагином. Выходная структура данных имеет следующий вид:
				Root Params
					|- wchar_t[] (L"Events"): ARRAY_T
							|- Item0
							|		|- wchar_t[] (L"Component"):			 STRING_T	// Имя компонента, генерирущего события данного типа (STRING_T)
							|		|- wchar_t[] (L"EventID"):				 STRING_T	// Идентификатор события (см. описание в документе "Описание KCA интерфейсов приложений")
							|		|- wchar_t[] (L"Severity")				 INT_T		// ID серьезности ошибки [1-4]
							|		|- wchar_t[] (L"LocalizedEventIDName")STRING_T	// Локализованное название типа события
							|- Item1
							|		|- wchar_t[] (L"Component"):			 STRING_T	// Имя компонента, генерирущего события данного типа (STRING_T)
							|		|- wchar_t[] (L"EventID"):				 STRING_T	// Идентификатор события (см. описание в документе "Описание KCA интерфейсов приложений")
							|		|- wchar_t[] (L"Severity")				 INT_T		// ID серьезности ошибки [1-4]
							|		|- wchar_t[] (L"LocalizedEventIDName")STRING_T	// Локализованное название типа события
						............														
							|- ItemN
									|- wchar_t[] (L"Component"):			 STRING_T	// Имя компонента, генерирущего события данного типа (STRING_T)
									|- wchar_t[] (L"EventID"):				 STRING_T	// Идентификатор события (см. описание в документе "Описание KCA интерфейсов приложений")
									|- wchar_t[] (L"Severity")				 INT_T		// ID серьезности ошибки [1-4]
									|- wchar_t[] (L"LocalizedEventIDName")STRING_T	// Локализованное название типа события

				Для описания Severity используются следующие константы:
					
					1 - Information
					2 - Warning
					3 - Failure
					4 - Critical
		*/
		virtual void GetEvents(KLPAR::Params* pParams) PURE; 

		/*!
			\brief Возвращает название продукта, управляемого плагином. 
		*/
		virtual LPCWSTR GetProductName() const PURE;

		/*!
			\brief Возвращает название версии продукта, управляемого плагином. 
		*/
		virtual LPCWSTR GetProductVersion() const PURE;
	};

	/*!
		\brief Интерфейс доступа к PropertySheet. 
	*/
	interface IPropertySheet  
	{
		virtual ~IPropertySheet (){};

		/*!
			\brief Возвращает TRUE, если флаг Inherit включен, в противном случае возвращает FALSE
		*/
		virtual BOOL IsInherit() const PURE;
	};

	/*!
		\brief Возвращает интерфейс доступа к реализуемой плагином функциональности
	*/
	CALL_SPEC IKLAccessor* GetAccessor(); 

// Версия 2 - Дополнительные функции

	/*!
		\brief Инициализирует DLL плагина
	*/
	CALL_SPEC void InitPlugin(); 

	/*!
		\brief Деинициализирует DLL плагина
	*/
	CALL_SPEC void DeinitPlugin(); 
};

#endif // __CONFIGURATOR_INTERFACES_H__

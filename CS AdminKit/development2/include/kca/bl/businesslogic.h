/*KLCSAK_PUBLIC_HEADER*/

#ifndef KLBL_BUSINESSLOGIC_H
#define KLBL_BUSINESSLOGIC_H

/*KLCSAK_BEGIN_PRIVATE*/
#include <vector>
#include <string>


#include <std/base/klbase.h>
#include <std/par/params.h>

#include <kca/prss/settingsstorage.h>

#include <kca/prci/componentid.h>
#include <kca/prcp/componentproxy.h>
/*KLCSAK_END_PRIVATE*/

namespace KLBL
{
/*KLCSAK_BEGIN_PRIVATE*/

    /*! Наличие данного параметра, установленного в 1, в секции
        KLPRSS_COMPONENT_PRODUCT хранилища SS_PRODINFO означает, что
        сетевой агент может контролировать автозапуск приложения.
        Если сетевой агент не должен контролировать автозапуск приложения,
        данный параметр либо не следует создавать либо в него следует
        записывать 0.
    */
    const wchar_t c_szwAutoStartControl[] = L"AutoStartControl";//INT_T

    /*! В данном параметре задается список компонентов, которые следует
        запускать автоматически. Параметр должен содержать массив контейнеров
        Params, каждый из которых содержит обязательный параметр 
        c_szwAutoStartComponentName и необязательный параметр 
        c_szwAutoStartRegValName (см. далее) - по элементу массива на
        каждый компонент, нуждающийся в автозапуске.
    */
    const wchar_t c_szwAutoStartComponents[] = L"AutoStartComponents";//ARRAY_T|PARAMS_T

    /*!
        Имя компонента, нуждающегося в автозапуске. Значение 
        KLPRSS_COMPONENT_PRODUCT (см. файл PRSS/SettingsStorage.h) 
        означает продукт в целом.
    */
    const wchar_t c_szwAutoStartComponentName[] = L"ComponentName";//STRING_T


    /*!
        Данный параметр используется только, если компонент имеет тип
        запуска MTF_EXE и автозапуск производится через раздел Реестра
        HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run
        Значение параметра представляет собой имя переменной, добавляемой
        в означенный раздел Реестра.
    */
    const wchar_t c_szwAutoStartRegValName[] = L"RegValName";//STRING_T
    /*!
        Данная секция находится в SS_SETTINGS. Если переменная 
        c_szwAutoStartControl (см. выше) установлена в 1, сетевой агент
        отслеживает изменения данной секции и изменяет состояние
        автозапуска приложения. Секция содержит переменную 
        c_szwAutoStartStates (см. ниже).
    */
    #define KLBL_GLOBAL_AUTOSTART_SECTION   L"KLBL_GLOBAL_AUTOSTART"

    /*!
        Содержит переменные <имя компонента> типа BOOL. Если переменная 
        с именем X имеет значение true, то компонент с именем X должен
        запускаться автоматически; значение false означает, что 
        автоматический старт для компонента с именем X отключен.
       
        Если компонент c именем <имя компонента> имеет тип запуска MTF_EXE,
        то сетевой агент при выставлении переменной <имя компонента> в
        true, создает  в разделе Реестра 
        HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run
        переменную с именем, содержащимся в переменной
        c_szwAutoStartRegValName и записывает в нее командную строку
        компонента,  которую берет из переменной 
        <имя компонента>\KLPRSS_VAL_CUSTOMNAME.
        При выставлении переменной c именем <имя компонента> в false,
        сетевой  агент удаляет означенную переменную Реестра.

        Если компонент имеет тип запуска MTF_NTSERVICE, сетевой агент
        меняет тип старта службы (имя службы берется также из 
        <имя компонента>\KLPRSS_VAL_CUSTOMNAME).

        "Имя компонента" может равняться KLPRSS_COMPONENT_PRODUCT  
        (см. файл PRSS/SettingsStorage.h).
    */

    const wchar_t c_szwAutoStartStates[] = L"AutoStartStates";//PARAMS_T

    //!OBSOLETE
    const wchar_t c_szwAutoStartEnabled[] = L"EnableAutoStart"; //BOOL_T

/*KLCSAK_END_PRIVATE*/
    
    /*! Данное событие следует публиковать при завершении полного
        сканирования компьютера. Событие не имеет параметров.
    */
    const wchar_t c_szwFulscanCompletedEvent[]=L"KLBL_EV_FSCAN_COMPLETED";

    typedef enum
    {
        RtpState_Unknown = 0,
        RtpState_Stopped,
        RtpState_Suspended,
        RtpState_Starting,
        RtpState_Running,        
        RtpState_Running_MaxProtection,
        RtpState_Running_MaxSpeed,
        RtpState_Running_Recomended,
        RtpState_Running_Custom,
        RtpState_Failure
    }
    GlobalRptState;

/*KLCSAK_BEGIN_PRIVATE*/
    #define KLBL_GLOBAL_STATE_PRODUCT  KLPRSS_PRODUCT_CORE
    #define KLBL_GLOBAL_STATE_VERSION  KLPRSS_VERSION_INDEPENDENT
    #define KLBL_GLOBAL_STATE_SECTION  L"KLBL_GLOBAL_STATE_SECTION"

    /*! Секция (KLBL_GLOBAL_STATE_PRODUCT,
        KLBL_GLOBAL_STATE_VERSION, KLBL_GLOBAL_STATE_SECTION) находится в
        хранилище SS_PRODINFO.
        При инсталляции приложение, осуществляющее RTP-защиту должно
        записать в эту секцию следующие переменные:
        c_swzValComponentRtpState
        c_swzValPathRtpState

    */

    /*!
        Компонент, отображающий в статистике состояние Rtp и публикующий
        событие c_szwRptStateChengedEvent о его изменении. Формат:
        <продукт>/<версия>/<компонент>[/<экземпляр>].
        Если <экземпляр> отсутствует, истспользуется любой имеющийся
        экземпляр.
    */
    const wchar_t c_swzValComponentRtpState[]=L"KLBL_GSVAL_COMPONENT_RTPSTATE";

    /*! Путь в статистике компонента, указанного с помощью переменной
        c_swzValComponentRtpState, к переменным c_swzValRptState и
        c_swzValRptStateErrorCode
    */
    const wchar_t c_swzValPathRtpState[] = L"KLBL_GSVAL_VALPATH_RTPSTATE";
    
    //! Состояние rpt-защиты
    const wchar_t c_swzValRptState[]=L"KLBL_GSVAL_RPTSTATE";    //INT_T

    //! Код ошибки (для состояния RtpState_Failure)
    const wchar_t c_swzValRptStateErrorCode[]=L"KLBL_GSVAL_RPTSTATE_ECODE"; //INT_T

    /*! Событие c_szwRptStateChengedEvent следует публиковать 
        при изменении состояния rpt-защиты.
        Параметры события
            c_swzValRptState
            c_swzValRptStateErrorCode
    */
    const wchar_t c_szwRptStateChengedEvent[]=L"KLBL_EV_RPTSTATE_CHANGED";

    /*!
        Секция настроек деинсталляции.
        Секция <product>/<version>/KLBL_UNINSTALL_OPTIONS располагается
        в SS_SETTINGS. 
        Секция содержит следующие переменные
            szwUninstallPswdHash
    */
    #define KLBL_UNINSTALL_OPTIONS   L"KLBL_UNINSTALL_OPTIONS"

    /*!
		Переменная содержит хеш MD5 пароля деинсталляции. Хеш считается 
        для юникодной строки пароля (без нулевого символа на конце).
	*/
    const wchar_t szwUninstallPswdHash[] = L"KLBL_UNINST_PSWD_HASH";//BINARY_T

    /*!
		Событие, которое генерируется, когда компонент запрашивает у BL выбор
		из набора возможных действий.
	*/
    const wchar_t c_szwActionAskedEvent[] = L"KLBL_EV_ACTION_ASKED";

    /*!
		Событие, которое генерируется, когда BL отвечает компоненту на вопрос
		о выборе действия.
	*/
    const wchar_t c_szwActionResponsed[] = L"KLBL_EV_ACTION_RESPONSED";

    /*!
		\brief Описание структуры с описанием события, на которое запрашивается
		действие пользователя.
    */

    struct EventToAsk{
		///< Идентификатор компонента, в котором произошло событие.
		KLPRCI::ComponentId id; 
		///< Тип события, которое произошло.
		std::wstring eventType;
		///< Тело события.
		KLSTD::CAutoPtr<KLPAR::Params> eventBody;
		///< Идентификатор задачи, для которой произошло событие. 
        long taskId;
		///< Возможные действия для данного события.
		KLSTD::CAutoPtr<KLPAR::Params> actions;
	};

	typedef std::vector<KLSTD::KLAdapt<KLSTD::CAutoPtr<KLPRCP::ComponentProxy> > > proxies_t;

    /*!
		\brief	Интерфейс реализует функциональность модуля бизнес-логики для
				компонентов-клиентов.
		Данный интерфейс позволяет компонентам-клиентам получить следующую
		функциональность:
		1)	Получение proxy на компонент, требуемый компоненту-клиенту. При
			этом модуль BL запустит компонент самостоятельно, если это
			потребуется.
		2)	Запрос возможных действий при возникновении тех или иных событий.
    */
    
    class BusinessLogicProxy : public KLSTD::KLBase
	{
    public:

    /*!
		\brief	Метод возвращает компоненту-клиенту proxy на требуемый
				компонент.
		Данный метод реализует "старт по требованию". Если требуемый компонент
		уже запущен, этот метод возвращает proxy на запущенный компонент, а
		если компонент не запущен, запускает его в некотором процессе. В каком
		именно процессе будет запущен компонент, определяется бизнес-логикой
		приложения.

        \param idServer [in] Идентификатор требуемого компонента-сервера.
        \param idClient [in] Идентификатор компонента-клиента, который будет
							использовать компонент-сервер.
        \param vecProxies [out] Массив proxy на возможные компоненты-сервера,
							которые может использовать компонент-клиент.
		\param lTimeout [in] Тайм-аут

		\exception	STDE_TIMEOUT истёк тайм-аут
       
    */
        virtual void GetComponentProxies (
						KLPRCI::ComponentId&	idServer,
						KLPRCI::ComponentId&	idClient,
						proxies_t&				vecProxies,
						long					lTimeout) = 0;

    /*!
		\brief	Метод возвращает компоненту-клиенту proxy на требуемый
				компонент.
		Данный метод реализует "старт по требованию". Если требуемый компонент
		уже запущен, этот метод возвращает proxy на запущенный компонент, а
		если компонент не запущен, запускает его в некотором процессе. В каком
		именно процессе будет запущен компонент, определяется бизнес-логикой
		приложения.

        \param idServer [in] Идентификатор требуемого компонента-сервера.
        \param idClient [in] Идентификатор компонента-клиента, который будет
							использовать компонент-сервер.
        \param vecIDs [out] Массив ComponentId возможных компонентов-серверов,
							которые может использовать компонент-клиент.
		\param lTimeout [in] Тайм-аут

		\exception	STDE_TIMEOUT истёк тайм-аут
       
    */
        virtual void GetComponentIDs (
						KLPRCI::ComponentId&				idServer,
						KLPRCI::ComponentId&				idClient,
						std::vector<KLPRCI::ComponentId>&	vecIDs,
						long								lTimeout) = 0;

		/*!
		  \brief	Возвращает информацию о запущенном экземпляре компонента.
		  Пример кода, извлекающего идентифиткатор процесса:
			   KLSTD::CAutoPtr<KLPAR::Params> pInfo;
			   pBL->GetRuntimeInfo(id, KLSTD_INFINITE, &pInfo);
			   long nPid=KLPAR::GetIntValue(pInfo, KLPRSS_VAL_PID);

		  \param	id			[in] идентификатор экземпляра компонента
		  \param	lTimeout	[in] тайм-аут доступа к хранилищу.
		  \param	ppInfo		[out] контейнер Params. Содержит следующие
					переменные: KLPRSS_VAL_INSTANCEPORT, KLPRSS_VAL_LASTSTART,
					KLPRSS_VAL_PID (см. файл prss/settingsstorage.h)
										
		  \return	pid	процесса
		*/
		virtual void GetRuntimeInfo(
					KLPRCI::ComponentId&	id,
					long					lTimeout,
					KLPAR::Params**			ppInfo) = 0;


    /*!
		\brief	Метод возвращает компоненту-клиенту ответ на событие,
				случившееся в компоненте.
		Компонент-клиент вызывает этот метод, когда ему требуется получить
		информацию о дальнейшей реакции на некоторое событие. Описание
		произошедшего события содержится в параметре event. Компонент BL
		принимает решение о том, как реагировать на это событие (возможно,
		запрашивая об этом пользователя) и возвращает информацию о принятом
		решении в параметре action. При этом компонент BL также генерируется
		пару событий ACTION_ASKED, ACTION_RESPONSED.

        \param Event    [in] Описание события, по которому запрашивается действие.
		\param lTimeout [in] Тайм-аут
        \param ppAction   [out] Действие, которое выбирается для данного события.

		\exception	STDE_TIMEOUT истёк тайм-аут
		\exception	STDE_UNAVAIL не доступен модуль GUI
       
    */

        virtual void AskAction(
						EventToAsk&		Event,
						long			lTimeout,
						KLPAR::Params**	ppAction) = 0;


    /*!
		\brief	Метод возвращает массив полных имен (с путями) исполняемых
				модулей (только EXE), которые работают в рамках данного
				продукта.

		\param vecNames [out] массив имен исполняемых модулей.
       
    */
		virtual void GetProcessNames(std::vector<std::wstring>& vecNames, long lTimeout) = 0;

    };

/*KLCSAK_END_PRIVATE*/
};

/*KLCSAK_BEGIN_PRIVATE*/
/*!
  \brief Создает объект класса BusinessLogicProxy.

  \param idComponent	[in] Идентификатор компонента, который будет использовать BusinessLogicProxy.
  \param ppBL			[out] Созданный объект класса BusinessLogicProxy.

*/



namespace KLBL
{
    typedef void (*pCreateBusinessLogicProxyProc_t)(
									KLPRCI::ComponentId&		idComponent,
									KLBL::BusinessLogicProxy**	ppBL);

    #define KLBL_BUSINESSLOGIC_PROXY_PROC   KLBL_CreateBusinessLogicProxyProc

    /*
    void KLBL_BUSINESSLOGIC_PROXY_PROC(
									    KLPRCI::ComponentId&		idComponent,
									    KLBL::BusinessLogicProxy**	ppBL);
    */

};

KLCSC_DECL void KLBL_CreateBusinessLogicProxy(
									KLPRCI::ComponentId&		idComponent,
									KLBL::BusinessLogicProxy**	ppBL);
/*KLCSAK_END_PRIVATE*/
#endif // KLBL_BUSINESSLOGIC_H

/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file evp/notificationproperties.h
 * \author Михаил Кармазин
 * \date 14:21 22.11.2002
 * \brief Интерфейс для просмотра и редактирования параметров нотификации о событиях,
 *		возникающих в системе. Сервер выставляет этот интерфейс для интсрумента 
 *		администрирования (например, для snap-in'а).
 *
 */

/*!
* \brief Интерфейс реализует функциональность по просмотру и редактированию способов 
*		нотификации о событиях, зарегистрированных на сервере.
*
*		Нотификация осуществляеться:
*			а) Через e-mail (т.е. требуеться указать SMTP сервер и собственно e-mail)
*			
*			б) с помощью net send, для этого нужно указать компьютер, на который посылать
*					сообщение.
*			
*			в) С помощью выполнения заданного пользователем скрипта (или exe-файла)
*		
*		Настройки могут быть как общие для всех, так и специализрованные для некоторых ситуаций -
*		например, для события "Virus detected", пришедшего от продукта "Kaspersky Antivirus".
*		При этом специализированные настройки перекрывают настройки общие. Кроме того, возможна
*		частичная специализация настроек - например, для всех событий, приходящих от
*		продукта "Kaspersky Antivirus". Или наоборот, для события "Virus detected", приходящего
*		от любого продукта. 
*		
*		При этом:
*		1. Существует три типа условий, пересечение которые могут специализировать настройки. 
*			Это группа, продукт и тип события.
*			
*		2. При определении того, какие конкретно настройки надо использовать, работает 
*			следующее правило: более глубокая специализация перекрывает менее глубокую.
*			
*		3. При пересечении одноуровневых специализаций выполняеться merge настроек.
*			Может работать любой алгоритм. Например, один из таких:
*
*			3.1 У каждого условия есть приоритет, и по нему выбирается, какие именно настройки использовать.
*			
*			3.2 Настройки объединяються - если у события "Virus detected" от всех продуктов
*			настроена нотификация по e-mail для "user1@bbb.com", а у 
*			продукта "Kaspersky Antivirus" для всех событий стоит "user2@bbb.com", то
*			нотификация посылается обоим пользователям.
*			
*			3.3 Настройки максимизируются - например, для "Store information about event 
*			in the database for NNN days" NNN может браться максимальным.
*			
*			...
*			
*			3.N Особенный алгоритм для каких-то специфичных настроек.
*			
*/


#ifndef _KLNOTIFICATPROPS_H__011D27B2_EFF7_48fe_BBF5_3FCFCBB876AC__
#define _KLNOTIFICATPROPS_H__011D27B2_EFF7_48fe_BBF5_3FCFCBB876AC__

#include <std/par/params.h>

// String params:
#define KLEVP_NPAR_SEVERITY_STRING     L"SEVERITY"
#define KLEVP_NPAR_SOURCE_HOST         L"COMPUTER"
#define KLEVP_NPAR_SOURCE_DOMAIN       L"DOMAIN"
#define KLEVP_NPAR_TYPE                L"EVENT"
#define KLEVP_NPAR_DESCR               L"DESCR"
#define KLEVP_NPAR_RISE_TIME           L"RISE_TIME"
#define KLEVP_NPAR_TSK_DISPLAY_NAME    L"KLCSAK_EVENT_TASK_DISPLAY_NAME"
#define KLEVP_NPAR_PRODUCT             L"KL_PRODUCT"
#define KLEVP_NPAR_VERSION             L"KL_VERSION"
#define KLEVP_NPAR_SEVERITY_NUM        L"KLCSAK_EVENT_SEVERITY_NUM"
#define KLEVP_NPAR_SOURCE_HOST_IP      L"HOST_IP"
#define KLEVP_NPAR_SOURCE_HOST_CONN_IP L"HOST_CONN_IP"

#define KLEVP_MSG_TEMPL_BOUNDARY       L"%" // will work if it is one symbol. Otherwise change EventsProcessorImpl::GetMessageText() function.
#define _BOUND_STRING(S) L"%"##S##L"%"
#define _MSG_TEMPL(S)  _BOUND_STRING(S)

#define KLEVP_MSG_TEMPL_SEVERITY_STRING     _MSG_TEMPL(KLEVP_NPAR_SEVERITY_STRING)
#define KLEVP_MSG_TEMPL_SEVERITY_NUM        _MSG_TEMPL(KLEVP_NPAR_SEVERITY_NUM)
#define KLEVP_MSG_TEMPL_SOURCE_HOST         _MSG_TEMPL(KLEVP_NPAR_SOURCE_HOST)
#define KLEVP_MSG_TEMPL_SOURCE_DOMAIN       _MSG_TEMPL(KLEVP_NPAR_SOURCE_DOMAIN)
#define KLEVP_MSG_TEMPL_TYPE                _MSG_TEMPL(KLEVP_NPAR_TYPE)
#define KLEVP_MSG_TEMPL_DESCR               _MSG_TEMPL(KLEVP_NPAR_DESCR)
#define KLEVP_MSG_TEMPL_RISE_TIME           _MSG_TEMPL(KLEVP_NPAR_RISE_TIME)
#define KLEVP_MSG_TEMPL_TSK_DISPLAY_NAME    _MSG_TEMPL(KLEVP_NPAR_TSK_DISPLAY_NAME)
#define KLEVP_MSG_TEMPL_PRODUCT				_MSG_TEMPL(KLEVP_NPAR_PRODUCT) 
#define KLEVP_MSG_TEMPL_VERSION				_MSG_TEMPL(KLEVP_NPAR_VERSION) 
#define KLEVP_MSG_TEMPL_SOURCE_HOST_IP		_MSG_TEMPL(KLEVP_NPAR_SOURCE_HOST_IP)
#define KLEVP_MSG_TEMPL_SOURCE_HOST_CONN_IP	_MSG_TEMPL(KLEVP_NPAR_SOURCE_HOST_CONN_IP)
// Obsolete:
#define KLEVP_MSG_TEMPL_TIME                _MSG_TEMPL(L"TIME") 

#define KLEVP_ENV_VAR_SEVERITY_STRING		KLEVP_NPAR_SEVERITY_STRING
#define KLEVP_ENV_VAR_SEVERITY_NUM			KLEVP_NPAR_SEVERITY_NUM
#define KLEVP_ENV_VAR_SOURCE_HOST			KLEVP_NPAR_SOURCE_HOST
#define KLEVP_ENV_VAR_SOURCE_DOMAIN			KLEVP_NPAR_SOURCE_DOMAIN
#define KLEVP_ENV_VAR_TYPE					KLEVP_NPAR_TYPE
#define KLEVP_ENV_VAR_DESCR					KLEVP_NPAR_DESCR
#define KLEVP_ENV_VAR_RISE_TIME				KLEVP_NPAR_RISE_TIME
#define KLEVP_ENV_VAR_TSK_DISPLAY_NAMEE		KLEVP_NPAR_TSK_DISPLAY_NAME
#define KLEVP_ENV_VAR_PRODUCT				KLEVP_NPAR_PRODUCT
#define KLEVP_ENV_VAR_VERSION				KLEVP_NPAR_VERSION
#define KLEVP_ENV_VAR_SOURCE_HOST_IP		KLEVP_NPAR_SOURCE_HOST_IP
#define KLEVP_ENV_VAR_SOURCE_HOST_CONN_IP	KLEVP_NPAR_SOURCE_HOST_CONN_IP

#define	KLTSK_VIRTUAL_DAYS_TO_STORE_EVENTS	-1
#define KLTSK_DEFAULT_DAYS_TO_STORE_EVENTS	7

namespace KLEVP {

	/*!
    * \brief notification_descr_t - условие и адреса нотификации.
	*
	*/
    /*
     *	устаревшая структура
     */
	struct notification_descr_t
	{
		std::wstring	wstrProduct;
        std::wstring	wstrVersion;
		std::wstring	wstrEventType;
        long            nSeverity;

        std::wstring	wstrSMTPServer;
        long            nSMTPPort;
		std::wstring	wstrEMail;
        bool            bUseEmail;

		std::wstring	wstrNetSendParameter;
        bool            bUseNetSend;

		std::wstring	wstrScript;
        bool            bUseScript;

		long			lDaysToStoreEvent;
        std::wstring	wstrMessageTemplate;
        bool            bStoreAtSeverEventLog;
        bool            bStoreAtClientEventLog;
        bool            bStoreAtClientPRES;
        
        KLSTD::CAutoPtr<KLPAR::Params> pBodyFilter;

        notification_descr_t()
        {
            nSMTPPort = 0;
		    lDaysToStoreEvent = 0;
            nSeverity = 0;
            bStoreAtSeverEventLog = false;
			bUseNetSend = false;
			bUseScript = false;
			bUseEmail = false;
        }
	};
    
	struct notification_descr_t2: public notification_descr_t{
		std::wstring	wstrEMailSubj;
		std::wstring	wstrEMailFrom;
		std::wstring	wstrEMailESMTPUserName;
		std::wstring	wstrEMailESMTPUserPassword;
	};

    struct MessageArguments
    {
        std::wstring wstrDomain;
        std::wstring wsrtHost;
        std::wstring wstrEventType;
        std::wstring wstrEventTypeDispl;
        time_t tEventTime;
        int nEventSeverity;
        std::wstring wstrDescripton;
        std::wstring wstrTaskDislpayName;
		// Added by Andrey Bryksin 4/3/2005
		std::wstring wstrAttachment;
		// Added by Andrey Lashchenkov 07/09/2005
		std::wstring wstrProductId;
		std::wstring wstrProductDisplayName;
		std::wstring wstrDisplayVersion;
		unsigned long ulHostIP;
		unsigned long ulHostConnectIP;
		// Added by Andrey Lashchenkov 27/10/2005
		//std::wstring wsrtWinHostName;
		std::wstring wsrtHostDisplayName;

		// Added by Andrey Bryksin 4/3/2005
        std::wstring GetMessageSeverity() const;
    };

    // Имя переменной STRING_T, содержащий идентификатор, генерируемый на сервере:
    const wchar_t c_NotificationDescrId[]    = L"KLEVP_NOTIFICATION_DESCR_ID";

    // Имя переменной STRING_T, содержащий идентификатор, генерируемый на сервере:
    const wchar_t c_NotificationDescrChecksum[]    = L"KLEVP_NOTIFICATION_CHECKSUM";

    // old stuff
    const wchar_t c_NotificationDescrArray[]    = L"KLEVP_NOTIFICATION_DESCR_ARRAY";

    // Имя переменной ARRAY_T, массива из Params, соотвествующий notification_descr_t
    // структуре notification_descr_t:
    const int c_nNFDescrSize = 3;

    // длина строк c_NF*Descr должна быть c_nNFDescrSize:
    const wchar_t c_NFInfoDescr[]      = L"INF";  // для событий с severity = info
    const wchar_t c_NFWarningDescr[]   = L"WRN"; // для событий с severity = warning
    const wchar_t c_NFErrorDescr[]     = L"ERR"; // для событий с severity = error
    const wchar_t c_NFCriticalDescr[]  = L"CRT"; // для событий с severity = critical

    
    // Имена переменных в контейнере c_NDFld_Product: 
    const wchar_t c_NDFld_Product[]             = L"KLEVP_ND_PRODUCT";				// STRING_T
    const wchar_t c_NDFld_Version[]             = L"KLEVP_ND_VERSION";				// STRING_T

    const wchar_t c_NDFld_EventType[]           = L"KLEVP_ND_EVETN_TYPE";			// STRING_T
    const wchar_t c_NDFld_SMTPServer[]          = L"KLEVP_ND_SMTP_SERVER";			// STRING_T
    const wchar_t c_NDFld_SMTPPort[]            = L"KLEVP_ND_SMTP_PORT";			// STRING_T
    const wchar_t c_NDFld_EMail[]               = L"KLEVP_ND_EMAIL";				// INT_T
    const wchar_t c_NDFld_UseEmail[]            = L"KLEVP_ND_USE_EMAIL";			// BOOL_T
    const wchar_t c_NDFld_NetSend[]             = L"KLEVP_ND_NET_SEND";				// STRING_T
    const wchar_t c_NDFld_UseNetSend[]          = L"KLEVP_ND_USE_NET_SEND";			// BOOL_T
    const wchar_t c_NDFld_Script[]              = L"KLEVP_ND_SCRIPT";				// STRING_T
    const wchar_t c_NDFld_UseScript[]           = L"KLEVP_ND_USE_SCRIPT";			// BOOL_T
    const wchar_t c_NDFld_DaysToStoreEvent[]    = L"KLEVP_ND_DAYS_TO_STORE_EVENT";	// INT_T
    const wchar_t c_NDFld_MessageTemplate[]     = L"KLEVP_ND_MESSAGE_TEMPLATE";		// STRING_T
    const wchar_t c_NDFld_EmailFrom[]			= L"KLEVP_ND_EMAIL_FROM";			// STRING_T
    const wchar_t c_NDFld_EmailSubject[]		= L"KLEVP_ND_EMAIL_SUBJECT";		// STRING_T
    const wchar_t c_NDFld_EmailESMTPUserName[]	= L"KLEVP_ND_EMAIL_ESMTP_USER";		// STRING_T
    const wchar_t c_NDFld_EmailESMTPUserPassword[] = L"KLEVP_ND_EMAIL_ESMTP_PASSWORD";	// STRING_T
    const wchar_t c_NDFld_StoreAtServerLog[]    = L"KLEVP_ND_STORE_AT_SERVER_LOG";	// BOOL_T
    const wchar_t c_NDFld_StoreAtClientLog[]    = L"KLEVP_ND_STORE_AT_CLIENT_LOG";	// BOOL_T
    const wchar_t c_NDFld_StoreAtClientPRES[]   = L"KLEVP_ND_STORE_AT_CLIENT_PRES"; // BOOL_T
    const wchar_t c_NDFld_BodyFilter[]          = L"KLEVP_ND_BODY_FILTER";			// BOOL_T
    const wchar_t c_NDFld_DenyDefaults[]		= L"KLEVP_ND_DENY_DEFAULTS";		// BOOL_T

    // internal stuff
    const wchar_t c_NLMaxEventsToSendPerPeriod[]= L"KLEVP_MAX_EVENTS_TO_SEND_PER_PERIOD";	// INT_T
    const wchar_t c_NLTestPeriodToSendEvent[]   = L"KLEVP_TEST_PERIOD_TO_SEND_EVENTS";		// INT_T
    const wchar_t c_NLMaxVirusEventsForOutbreakFiles[]= L"KLEVP_MAX_VIRUS_EVENTS_FOR_OUTBREAK";	// INT_T
    const wchar_t c_NLMaxVirusEventsForOutbreakEmail[]= L"KLEVP_MAX_VIRUS_EVENTS_FOR_OUTBREAK_E";	// INT_T
    const wchar_t c_NLMaxVirusEventsForOutbreakPerim[]= L"KLEVP_MAX_VIRUS_EVENTS_FOR_OUTBREAK_P";	// INT_T
    const wchar_t c_NLTestPeriodToOutbreakFiles[]	= L"KLEVP_TEST_PERIOD_TO_OUTBREAK";			// INT_T
    const wchar_t c_NLTestPeriodToOutbreakEmail[]	= L"KLEVP_TEST_PERIOD_TO_OUTBREAK_E";			// INT_T
    const wchar_t c_NLTestPeriodToOutbreakPerim[]	= L"KLEVP_TEST_PERIOD_TO_OUTBREAK_P";			// INT_T

	enum EVirusOutbreakType{
		VOT_FILESYSTEM	= 1,
		VOT_EMAIL		= 2,
		VOT_PERIMETER	= 4
	};

	const size_t VOT_COUNT = 3;
    
    const AVP_dword VOT_MASK = ~(unsigned(-1) << VOT_COUNT);

	class VirusOutbreakSource{
	public:
		EVirusOutbreakType m_eType;
        int m_nMaxVirusEventsForOutbreak;
        int m_nTestPeriodToOutbreak;
        VirusOutbreakSource(EVirusOutbreakType eType = VOT_FILESYSTEM) 
		:
			m_eType(eType),
            m_nMaxVirusEventsForOutbreak(0),
            m_nTestPeriodToOutbreak(0)
		{
		}

        bool CheckVirusOutbreak() const
        {
            return ( m_nMaxVirusEventsForOutbreak > 0 ) && ( m_nTestPeriodToOutbreak > 0 );
        };

		bool GetParamsNames(std::wstring& wstrMaxVirusEvents, std::wstring& wstrTestPeriod) const
		{
			return GetParamsNames(m_eType, wstrMaxVirusEvents, wstrTestPeriod);
		}

		static bool GetParamsNames(EVirusOutbreakType eType, std::wstring& wstrMaxVirusEvents, std::wstring& wstrTestPeriod)
		{
			switch( eType ) 
			{
				case VOT_FILESYSTEM:
					wstrMaxVirusEvents = c_NLMaxVirusEventsForOutbreakFiles;
					wstrTestPeriod = c_NLTestPeriodToOutbreakFiles;
					break;
				case VOT_EMAIL:
					wstrMaxVirusEvents = c_NLMaxVirusEventsForOutbreakEmail;
					wstrTestPeriod = c_NLTestPeriodToOutbreakEmail;
					break;
				case VOT_PERIMETER:
					wstrMaxVirusEvents = c_NLMaxVirusEventsForOutbreakPerim;
					wstrTestPeriod = c_NLTestPeriodToOutbreakPerim;
					break;
				default:
					return false;
			}
			return true;
		}
	};

    struct NotificationLimits
    {
        int nMaxEventsToSendPerPeriod;
        int nTestPeriodToSendEvents;

		std::vector<VirusOutbreakSource> m_vecVirusOutbreakSources;

        bool UseLimits() const
        {
            return ( nMaxEventsToSendPerPeriod > 0 ) && ( nTestPeriodToSendEvents > 0 );
        };

        NotificationLimits() :
            nMaxEventsToSendPerPeriod(0),
            nTestPeriodToSendEvents(0){};

		VirusOutbreakSource& GetVirusOutbreakSettings(EVirusOutbreakType eType)
		{
			for( size_t i = 0; i < m_vecVirusOutbreakSources.size(); ++i )
			{
				if( m_vecVirusOutbreakSources[i].m_eType == eType )
				{
					return m_vecVirusOutbreakSources[i];
				}
			}
			VirusOutbreakSource vo(eType);
			m_vecVirusOutbreakSources.push_back(vo);
			return m_vecVirusOutbreakSources[m_vecVirusOutbreakSources.size()-1];
		}

		void SetVirusOutbreakSettings(
			EVirusOutbreakType eType, int nMaxVirusEventsForOutbreak, int nTestPeriodToOutbreak)
		{
			/*
			for( size_t i = 0; i < m_vecVirusOutbreakSources.size(); ++i )
			{
				if( m_vecVirusOutbreakSources[i].m_eType == eType )
				{
					m_vecVirusOutbreakSources[i].m_nMaxVirusEventsForOutbreak = nMaxVirusEventsForOutbreak;
					m_vecVirusOutbreakSources[i].m_nTestPeriodToOutbreak = nTestPeriodToOutbreak;
					return;
				}
			}
			VirusOutbreakSource vo(eType);
			vo.m_nMaxVirusEventsForOutbreak = nMaxVirusEventsForOutbreak;
			vo.m_nTestPeriodToOutbreak = nTestPeriodToOutbreak;
			m_vecVirusOutbreakSources.push_back(vo);
			*/
			VirusOutbreakSource& vo = GetVirusOutbreakSettings(eType);
			vo.m_nMaxVirusEventsForOutbreak = nMaxVirusEventsForOutbreak;
			vo.m_nTestPeriodToOutbreak = nTestPeriodToOutbreak;
		}

		bool IsVirusOutbreakActive()
		{
			for( size_t i = 0; i < m_vecVirusOutbreakSources.size(); ++i )
			{
				if( m_vecVirusOutbreakSources[i].m_nMaxVirusEventsForOutbreak > 0 &&  
					m_vecVirusOutbreakSources[i].m_nTestPeriodToOutbreak > 0 )
				{
					return true;
				}
			}
			return false;
		}
    };
    
	/* brief описание см.выше.
	*
	   устаревший интерфейс
	*/
    class KLSTD_NOVTABLE NotificationProperties : public KLSTD::KLBaseQI {
    public:
		virtual void GetDefaultSettings(
            std::wstring & wstrSMTPServer,
            long & nSMTPPort,
		    std::wstring & wstrEMail,
            bool & bUseEmail,
		    std::wstring & wstrNetSendParameter,
            bool & bUseNetSend,
		    std::wstring& wstrScript,
            bool & bUseScript,
            std::wstring & wstrMessageTemplate ) = 0;

		virtual void SetDefaultSettings(
            const std::wstring & wstrSMTPServer,
            long nSMTPPort,
		    const std::wstring & wstrEMail,
            bool bUseEmail,
		    const std::wstring & wstrNetSendParameter,
            bool bUseNetSend,
		    const std::wstring & wstrScript,
            bool bUseScript,
            const std::wstring & wstrMessageTemplate ) = 0;
        
		virtual void TestEmail(
            const std::wstring & wstrSMTPServer,
            long nSMTPPort,
		    const std::wstring & wstrEMail,
            const std::wstring & wstrMessageTemplate ) = 0;

		virtual void TestNetSend(
            const std::wstring & wstrNetSendParameter,
            const std::wstring & wstrMessageTemplate ) = 0;

		virtual void TestScript(
            const std::wstring & wstrScript,
            const std::wstring & wstrMessageTemplate ) = 0;

        virtual KLEVP::NotificationLimits GetNotificationLimits() = 0;

        virtual void SetNotificationLimits( const NotificationLimits & nl ) = 0;

		/*! 
		  \brief Метод выдает настройки нотификации, не специализированные по группам, 
					но, возможно, специализированные по типам событий и по продукстам.
			\param vectNotificationDescr [out] - настройки. Если в элементе вектора
			notification_descr_t::notificationCondition   атрибуты wstrProduct и wstrEventType - 
			это пустая строки, то это самые общие настройки. Если только один из этих атрибутов - пустая строка,
			то этот элемент вектора частична специализация. Если оба атрибута содержат какие-то значения - то
			этот элемент описывает полную специализацию для общих настроек (специализация погруппам 
			рассматриваеться отдельно). 
			vectNotificationDescr всегда содержит как минмимум один элемент с общими настройками.

		*/
        /* устаревший метод */
		virtual void GetCommonProperties(
			std::vector<notification_descr_t>& vectNotificationDescr ) = 0;


		/*! 
		  \brief Метод выдает настройки нотификации для конктреной группы.
			\param nGroupID - ID группы.
			\param vectNotificationDescr [out] - настройки. Все то же самое, как в методе GetCommonProperties,
					но применительно к конкретной группе. Кроме того, vectNotificationDescr может не содержать
					ни одного элемента - в таком случае используються общие настройки, возвращаемые методом
					GetCommonProperties().
		*/
        /* устаревший метод */
		virtual void GetGroupProperties(
			long nGroupID,
			std::vector<notification_descr_t>& vectNotificationDescr ) = 0;

		/*! 
		  \brief Метод выдает настройки нотификации для задачи.
			\param wstrTaskId - ID задачи.
			\param vectNotificationDescr [out] - настройки. Все то же самое, как в методе GetCommonProperties,
					но применительно к задаче.
		*/
        /* устаревший метод */
		virtual void GetTaskProperties(
			const std::wstring& wstrTaskId,
			std::vector<notification_descr_t>& vectNotificationDescr ) = 0;


		/*! 
		  \brief Метод сохраняет общие настройки нотификации.
			\param vectNotificationDescr - см. описание метода GetCommonProperties().
				vectNotificationDescr должен содержать как минмимум один элемент с общими настройками.
		*/
        /* устаревший метод */
		virtual void SetCommonProperties(
			const std::vector<notification_descr_t>& vectNotificationDescr
		) = 0;

		/*! 
		  \brief Метод сохраняет настройки нотификации для группы.
			\param vectNotificationDescr - см. описание метода GetCommonProperties().
				vectNotificationDescr может не содержать ни одного элемента, в таком случае используються 
				общие настройки.

		*/
        /* устаревший метод */
		virtual void SetGroupProperties(
			long nGroupID,
			const std::vector<notification_descr_t>& vectNotificationDescr
		) = 0;


		/*! 
		  \brief Метод сохраняет настройки нотификации для задачи.
            \param wstrTaskId - ID задачи.
			\param vectNotificationDescr - см. описание метода GetCommonProperties().
		*/
        /* устаревший метод */
		virtual void SetTaskProperties(
			const std::wstring& wstrTaskId,
			const std::vector<notification_descr_t>& vectNotificationDescr
		) = 0;
	};
	
    class KLSTD_NOVTABLE NotificationProperties2 : public KLSTD::KLBaseQI {
    public:
		virtual void GetDefaultSettings(
            std::wstring & wstrSMTPServer,
            long & nSMTPPort,
		    std::wstring & wstrEMail,
            bool & bUseEmail,
		    std::wstring & wstrNetSendParameter,
            bool & bUseNetSend,
		    std::wstring& wstrScript,
            bool & bUseScript,
            std::wstring & wstrMessageTemplate,
			std::wstring & wstrEmailSubject,
			std::wstring & wstrEmailFrom) = 0;

		virtual void SetDefaultSettings(
            const std::wstring & wstrSMTPServer,
            long nSMTPPort,
		    const std::wstring & wstrEMail,
            bool bUseEmail,
		    const std::wstring & wstrNetSendParameter,
            bool bUseNetSend,
		    const std::wstring & wstrScript,
            bool bUseScript,
            const std::wstring & wstrMessageTemplate,
			const std::wstring & wstrEmailSubject,
			const std::wstring & wstrEmailFrom) = 0;

		virtual void TestEmail(
            const std::wstring & wstrSMTPServer,
            long nSMTPPort,
		    const std::wstring & wstrEMail,
            const std::wstring & wstrMessageTemplate,
			const std::wstring & wstrEmailSubject,
			const std::wstring & wstrEmailFrom,
			bool bDenyDefaults = false) = 0;

		virtual void TestNetSend(
            const std::wstring & wstrNetSendParameter,
            const std::wstring & wstrMessageTemplate,
			bool bDenyDefaults = false) = 0;

		virtual void TestScript(
            const std::wstring & wstrScript,
            const std::wstring & wstrMessageTemplate,
			bool bDenyDefaults = false) = 0;

        virtual KLEVP::NotificationLimits GetNotificationLimits() = 0;

        virtual void SetNotificationLimits( const NotificationLimits & nl ) = 0;
	};

    class KLSTD_NOVTABLE NotificationProperties3 : public NotificationProperties2 {
    public:
		virtual void GetDefaultSettings(notification_descr_t2& nd) = 0;

		virtual void SetDefaultSettings(const notification_descr_t2& nd) = 0;

		virtual void TestEmail2(
            const std::wstring & wstrSMTPServer,
            long nSMTPPort,
		    const std::wstring & wstrEMail,
            const std::wstring & wstrMessageTemplate,
			const std::wstring & wstrEmailSubject,
			const std::wstring & wstrEmailFrom,
			const std::wstring & wstrEmailESMTPUserName,
			const std::wstring & wstrEmailESMTPUserPassword,
			bool bDenyDefaults = false) = 0;
	};
}

/*!
  \brief Создает proxy-объект класса KLEVP::NotificationProperties

    \param wstrURL      [in] адрес soap-сервера
    \param ppEventProperties   [out] Указатель на объект класса KLEVP::NotificationProperties
*/
KLCSSRVP_DECL void KLEVP_CreateNotificationPropertiesProxy(
                        const wchar_t*  szwAddress,
                        KLEVP::NotificationProperties**           pp,
                        const unsigned short* pPorts = NULL,
                        bool            bUseSsl = true);

/*!
  \brief Создает proxy-объект класса KLEVP::NotificationProperties2

    \param wstrURL      [in] адрес soap-сервера
    \param ppEventProperties   [out] Указатель на объект класса KLEVP::NotificationProperties2
*/
KLCSSRVP_DECL void KLEVP_CreateNotificationProperties2Proxy(
                        const wchar_t*  szwAddress,
                        KLEVP::NotificationProperties2**           pp,
                        const unsigned short* pPorts = NULL,
                        bool            bUseSsl = true);

#endif // _KLNOTIFICATPROPS_H__011D27B2_EFF7_48fe_BBF5_3FCFCBB876AC__

// Local Variables:
// mode: C++
// End:

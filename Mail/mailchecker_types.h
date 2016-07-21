/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	mailchecker_types.h
*		\brief	Типы настроек для MailCommon[Prague], наследники от cSerializable

		\
		|
		+-+ hMTMSettings
		  +-+ hMailTaskParams[]
		    +- szMailTaskType
		    +- dwThreadCount
		    +- szFilterList
		    +-+ hMailFilterParams[]
		      +- szFilterName
		      +- hMailPluginParams ("process_params")
			  +-+ hMailFilterResultActions[]
			    +- szResultName				
			    +- dwResultId					
			    +- PrimaryAction				
			    +- szPlaceholderTemplate		
			    +- NextFilter					
			    +- bStopProcessing				
			    +- TotalPrimaryAction			
			    +- szAdminNotifyAddress		
			    +- szNotifyFromAddress			
			    +- szNotifyAdminTemplate		
			    +- szNotifyRecipientTemplate	
			    +- szNotifySenderTemplate		

*		
*		\author Vitaly DVi Denisov
*		\date	24.12.2003 17:29:52
*		
*		
*/		


#ifndef _MAILCHECKER_TYPES_H_
#define _MAILCHECKER_TYPES_H_

#pragma warning(disable:4786)

// Prague headers
#include <prague.h>      // Prague main header: types, system intrface, errors etc...
//#include <iface/i_os.h>
//#include <iface/i_serializer.h>
//#include <iface/i_engine.h>
#include <map>
//#include <iface/i_mailtask.h>
//#include <iface/i_mailfilter.h>
//#include <pr_err.h>
//#include <ppp.h>

#include <structs/s_mc.h>
#include <string>
#include <vector>



//
//	//! Перечень основных действий, производимых над объектом письма
//	typedef enum enum_PrimaryAction
//	{
//		mailpa_Pass = 0,
//		mailpa_DeleteObject,
//		mailpa_ChangeObjectToText
//		// mailpa_Disinfect - Не факт, что такое можно будет сделать
//	}	tPrimaryAction;
//
//	//! Перечень основных действий, производимых над письмом
//	//! Определение перенесено в <iface/i_mailtask.h>
//	//typedef enum enum_TotalPrimaryAction
//	//{
//	//	mailtpa_Pass = 0,
//	//	mailtpa_DeleteOrBlockMessage,
//	//	mailtpa_RejectMessage,
//	//	mailtpa_PassMessageAsAttachment - Не факт, что такое можно будет сделать тут
//	//}	tTotalPrimaryAction;
//
//	//////////////////////////////////////////////////////////////////////////
//	//
//	//			+ MC
//	//			+-+ MailPlugins
//	//			  +-+ 40002					// PID плагина (количество таких секций не ограничено, каждая секция задает параметры для конкретного плагина)
//	//			  | +- <plugin_params>		// Неструктурированные параметры инициализации плагина
//	//			  | +-+ PROCESS_OVERWRITE	// Секция для перекрытия параметров процессирования (может отсутствовать)
//	//			  | | +- <process_params>	// Неструктурированные параметры процессирования
//	//			  | +-+ PROCESS_DEFAULT		// Секция умолчательных параметров процессирования (обязательная секция)
//	//			  | | +- <process_params>
//	//			  | +-+ ProcessParams1		// Секция параметров процессирования (количество таких секций не ограничено). В случае отсутствия секции с заданным именем (либо отсутствия части параметров процессирования <process_params>) плагин должен взять их из секции PROCESS_DEFAULT. По окончании чтения параметров процессирования следует переопределить их значения параметрами из секции PROCESS_OVERWRITE.
//	//			  | | +- <process_params>
//	//			  | ...
//	//			  | |
//	//			  | +-+ ProcessParams(N)
//	//			  |   +- <process_params>
//	//			  ...
//	//			  |
//	//			  +-+ PID_XXX
//	//			    +- <plugin_params>
//	//			    +-+ PROCESS_DEFAULT
//	//			      +- <process_params>
//	//
//
//
//	//
//	//////////////////////////////////////////////////////////////////////////
//
//
//	struct cMailFilterParams;
//
//	//! Настройки действий по результату фильтра
//	struct cMailFilterResultActions : public cSerializable 
//	{
//		cMailFilterResultActions() : 
//			cSerializable(),
//			szResultName				(""),
//			dwResultId					(0),
//			PrimaryAction				(mailpa_Pass),
//			szPlaceholderTemplate		(L""),
//			szNextFilter				(""),
//			bStopProcessing				(cFALSE),
//			TotalPrimaryAction			(mailtpa_Pass),
//			bSendReportToAdmin			(cFALSE),
//			szAdminNotifyAddress		(L""),
//			szNotifyFromAddress			(L""),
//			szNotifyAdminTemplate		(L""),
//			bSendReportToRecipient		(cFALSE),
//			szNotifyRecipientTemplate	(L""),
//			bSendReportToSender			(cFALSE),
//			szNotifySenderTemplate		(L"")
//			{m_unique = SERID_MAILFILTERRESULTACTIONS_SETTINGS;};
//
//		cMailFilterResultActions( const cMailFilterResultActions& o ) : 
//			cSerializable(),
//			szResultName( o.szResultName ),
//			dwResultId( o.dwResultId ),
//			PrimaryAction( o.PrimaryAction ),
//			szPlaceholderTemplate( o.szPlaceholderTemplate ),
//			szNextFilter( o.szNextFilter ),
//			bStopProcessing( o.bStopProcessing ),
//			TotalPrimaryAction( o.TotalPrimaryAction ),
//			bSendReportToAdmin( o.bSendReportToAdmin ),
//			szAdminNotifyAddress( o.szAdminNotifyAddress ),
//			szNotifyFromAddress( o.szNotifyFromAddress ),
//			szNotifyAdminTemplate( o.szNotifyAdminTemplate ),
//			bSendReportToRecipient( o.bSendReportToRecipient ),
//			szNotifyRecipientTemplate( o.szNotifyRecipientTemplate ),
//			bSendReportToSender( o.bSendReportToSender ),
//			szNotifySenderTemplate( o.szNotifySenderTemplate )
//			{m_unique = SERID_MAILFILTERRESULTACTIONS_SETTINGS;};
//			
//		std::string szResultName;				//!< Имя результата фильтрации
//		tDWORD dwResultId;						//!< Идентификатор результата фильтрации
//		//////////////////////////////////////////////////////////////////////////
//		// primary action is following next
//		tPrimaryAction PrimaryAction;			//!< Основное действие с объектом 
//		std::wstring szPlaceholderTemplate;		//!< Шаблон заменяющего текста
//		//////////////////////////////////////////////////////////////////////////
//		std::string szNextFilter;				//!< Указатель на следующий фильтр (м.б. = "")
//		tBOOL bStopProcessing;					//!< Остановка фильтрации
//		//////////////////////////////////////////////////////////////////////////
//		// total actions are following next (used only for "bStopProcessing == cTRUE")
//		tTotalPrimaryAction TotalPrimaryAction;	//!< Основное главное действие 
//		tBOOL bSendReportToAdmin;				//!< Посылать ли отчет администратору?
//		std::wstring szAdminNotifyAddress;		//!< Адрес администратора для рассылки отчетов
//		std::wstring szNotifyFromAddress;		//!< Адрес ответственного лица за рассылку отчетов (м.б. = szAdminNotifyAddress)
//		std::wstring szNotifyAdminTemplate;		//!< Шаблон отчета администратору
//		tBOOL bSendReportToRecipient;			//!< Посылать ли отчет получателю?
//		std::wstring szNotifyRecipientTemplate;	//!< Шаблон отчета получателю
//		tBOOL bSendReportToSender;				//!< Посылать ли отчет отправителю?
//		std::wstring szNotifySenderTemplate;	//!< Шаблон отчета отправителю
//	};
//
//	//! Настройки runtime-фильтра (алгоритм фильтрации одного фильтра) + настройки процессирования плагина ("process_params")
//	struct cMailFilterParams : public cSerializable 
//	{
//		cMailFilterParams() : 
//			cSerializable(),
//			dwPID( PID_ANY ),
//			szFilterName (""),
//			process_params ("")
//			{m_unique = SERID_MAILFILTER_SETTINGS;};
//
//		cMailFilterParams( const cMailFilterParams& o ) : 
//			cSerializable(),
//			dwPID( o.dwPID ),
//			szFilterName ( o.szFilterName ),
//			process_params ( o.process_params )
//			{m_unique = SERID_MAILFILTER_SETTINGS;};
//			
//		tDWORD dwPID;							//!< Идентификатор пражского плагина. Настройки данного плагина расположены в интерфейсе iRegistry свойства i_task.h::pgTASK_CONFIG в одноименной ветке
//		std::string szFilterName;				//!< Уникальное имя runtime-фильтра 
//		std::string process_params;				//!< Краткое имя ветки реестра, содержащей настройки данного фильтра. 
//	};
//
//	//! Список фильтров
//	struct cMBLParams :  cSerializable
//	{
//		cMBLParams() :
//			cSerializable(),
//			szMailTaskType( "" ),
//			szMailFilterList (""),
//			szMailPartFilterList (""),
//			ulThreadCount( 1 )
//			{m_unique = SERID_MBL_SETTINGS;};
//		
//		cMBLParams( const cMBLParams& o ) :
//			cSerializable(),
//			szMailTaskType( o.szMailTaskType ),
//			MailInterceptorLoaders( o.MailInterceptorLoaders ),
//			szMailFilterList( o.szMailFilterList ),
//			szMailPartFilterList( o.szMailPartFilterList ),
//			ulThreadCount( o.ulThreadCount ),
//			MailFilters( o.MailFilters ),
//			MailPartFilters( o.MailPartFilters )
//
//			{m_unique = SERID_MBL_SETTINGS;}
//			
//		std::string szMailTaskType;				//!< Тип задачи почтовой фильтрации
//		typedef std::vector<tDWORD> MailInterceptorLoaders_t;
//		MailInterceptorLoaders_t MailInterceptorLoaders;	//!< Список почтовых перехватчиков (PID'ы их загрузчиков)
//		std::string szMailFilterList;			//!< Упорядоченный список фильтров уровня письма (имена фильтров cMailFilterParams::szFilterName через разделитель)
//		std::string szMailPartFilterList;		//!< Упорядоченный список фильтров уровня части письма (имена фильтров cMailFilterParams::szFilterName через разделитель)
//		tULONG ulThreadCount;					//!< Количество потоков, в которых должен исполняться алгоритм MBL
//		typedef std::vector<cMailFilterParams> MailFilterParams_t;
//		MailFilterParams_t MailFilters;			//!< Алгоритмы фильтрации (уровня письма)
//		MailFilterParams_t MailPartFilters;		//!< Алгоритмы фильтрации (уровня части письма)
//	};
//
//	//! Настройки MTM (MailTaskManager)
//	struct cMTMParams : public cSerializable
//	{
//		cMTMParams() :
//			cSerializable(),
//			dwProtectionLevel(PROT_RECOMMENDED)
//			{m_unique = SERID_MTM_SETTINGS;};
//
//		cMTMParams( const cMTMParams& o ) :
//			cSerializable(),
//			dwProtectionLevel( o.dwProtectionLevel ),
//			MailPlugins( o.MailPlugins ),
//			MBLs( o.MBLs )
//			{m_unique = SERID_MTM_SETTINGS;};
//
//		tDWORD dwProtectionLevel;				//!< Уровень защиты
//		typedef std::vector<tPID> MailPluginParams_t;
//		MailPluginParams_t MailPlugins;			//!< Список плагинов
//		typedef std::vector<cMBLParams> MBLParams_t;
//		MBLParams_t MBLs;						//!< Списки фильтров и алгоритмы фильтрации
//	};


//! \enum: cPLUGIN_RES_t
typedef enum {
	cPLUGIN_RES_DEFAULT				= -2,	//! Неопределенный вердикт. cMailFilterResultActions на данном вердикте исполняются в случае неопределенности действия на любом другом вердикте.
	cPLUGIN_RES_FAIL	 			= -1,	//!	Вердикт: ошибка. В фильтре произошла ошибка, из-за которой объект не мог быть проверен
	cPLUGIN_RES_OK					= 0,	//!	Вердикт: чисто. Признак фильтрации не обнаружен, объект чист.
	cPLUGIN_RES_FOUND				= 1,	//!	Вердикт: обнаружен признак фильтрации (например, объект заражен).
	cPLUGIN_RES_DELETED				= 2,	//!	Вердикт: признак фильтрации обнаружен и успешно удален (например, объект вылечен от вируса).
	cPLUGIN_RES_SUSPICIOUS			= 3,	//!	Вердикт: подозрение на признак фильтрации.
	cPLUGIN_RES_UNKNOWN				= 4,	//!	Вердикт: объекта неизвестного типа, фильтрация невозможна.
	cPLUGIN_RES_ENCRYPTED			= 5,	//!	Вердикт: объект зашифрован, фильтрация невозможна.
	cPLUGIN_RES_PASSWORD_PROTECTED	= 6,	//!	Вердикт: объект запаролен. фильтрация невозможна.
} enumPLUGIN_RES_t;	//! Здесь перечислены возможные вердикты плагинов. Список может пополняться

//////////////////////////////////////////////////////////////////////////
//
// Вердикты плагинов в строковом виде. Список может пополняться
//
#define cszPLUGIN_RES_DEFAULT				"DEFAULT"
#define cszPLUGIN_RES_FAIL	 				"FAIL"
#define cszPLUGIN_RES_OK					"OK"
#define cszPLUGIN_RES_FOUND					"FOUND"
#define cszPLUGIN_RES_DELETED				"DELETED"
#define cszPLUGIN_RES_SUSPICIOUS			"SUSPICIOUS"
#define cszPLUGIN_RES_UNKNOWN				"UNKNOWN"
#define cszPLUGIN_RES_ENCRYPTED				"ENCRYPTED"
#define cszPLUGIN_RES_PASSWORD_PROTECTED	"PASSWORD_PROTECTED"
//
// Вердикты плагинов в строковом виде. Список может пополняться
//
//////////////////////////////////////////////////////////////////////////


typedef std::map< std::string, int > szPLUGIN_RES_t;
struct cPLUGIN_RES: public szPLUGIN_RES_t
{
	cPLUGIN_RES():
	szPLUGIN_RES_t()
	{
		(*this)[ cszPLUGIN_RES_DEFAULT				] = cPLUGIN_RES_DEFAULT				;
		(*this)[ cszPLUGIN_RES_FAIL					] = cPLUGIN_RES_FAIL				;
		(*this)[ cszPLUGIN_RES_OK					] = cPLUGIN_RES_OK					;
		(*this)[ cszPLUGIN_RES_FOUND				] = cPLUGIN_RES_FOUND				;
		(*this)[ "INFECTED"							] = cPLUGIN_RES_FOUND				;
		(*this)[ cszPLUGIN_RES_DELETED				] = cPLUGIN_RES_DELETED				;
		(*this)[ "CURED"							] = cPLUGIN_RES_DELETED				;
		(*this)[ cszPLUGIN_RES_SUSPICIOUS			] = cPLUGIN_RES_SUSPICIOUS			;
		(*this)[ cszPLUGIN_RES_UNKNOWN				] = cPLUGIN_RES_UNKNOWN				;
		(*this)[ cszPLUGIN_RES_ENCRYPTED			] = cPLUGIN_RES_ENCRYPTED			;
		(*this)[ cszPLUGIN_RES_PASSWORD_PROTECTED	] = cPLUGIN_RES_PASSWORD_PROTECTED	;
	}
}; //! Здесь перечислены возможные вердикты плагинов (строковые соответствия). Список может пополняться
extern cPLUGIN_RES g_hPluginAnswers;


//	struct cMailCommonStatistics: public cCommonStatistics 
//	{
//		cMailCommonStatistics():
//			cCommonStatistics(),
//			m_szSenderName(""),
//			m_nNumObjects(0),
//			m_timeLastObject(-1)
//		{ m_unique = SERID_MC_STATISTICS; };
//
//		cMailCommonStatistics& operator+=(const cMailCommonStatistics& o)
//		{
//			if ( m_szSenderName == o.m_szSenderName ) 
//			{
//				if ( -1 == m_timeStart ) 
//					m_timeStart = o.m_timeStart;
//				if ( -1 == m_timeFinish ) 
//					m_timeFinish = o.m_timeFinish;
//
//				m_nNumObjects += o.m_nNumObjects;
//				// m_nNumFiltersVerdicts
//				{
//					if ( o.m_nNumVerdicts.size() > m_nNumVerdicts.size() ) 
//						m_nNumVerdicts.resize( o.m_nNumVerdicts.size() );
//					for (long i=0; i < m_nNumVerdicts.size(); i++)
//						m_nNumVerdicts.at(i) += o.m_nNumVerdicts.at(i);
//				}
//				m_timeLastObject = o.m_timeLastObject;
//			}
//			return *this;
//		}
//
//		std::string       m_szSenderName;                   // Имя того объекта, который опубликовал данную статистику. 
//															// Это позволяет собирать статистику отдельно для каждого модуля
//		long              m_nNumObjects;                    // Количество отданных на проверку в MailCommon объектов
//		std::vector<long> m_nNumVerdicts;			        // Количество вердиктов (по типам). TODO: Не очень хорошее решение с точки зрения требования к памяти: для того, чтобы прописать значение сотого элементв, необходимо разпределить память для 99 лишних элементов!
//		time_t            m_timeLastObject;                 // Время обработки последнего объекта
//		
//	};

#endif //_MAILCHECKER_TYPES_H_
#ifndef __S_MC_H
#define __S_MC_H

#include <AV/structs/s_avs.h>
#include <Mail/iface/i_mailtask.h>
#include <Mail/plugin/p_mctask.h>
#include <ProductCore/structs/s_taskmanager.h>

//////////////////////////////////////////////////////////////////////////
//! Перечень вердиктов, выдаваемых на письмо (битовые маски)
enum MailCheckerVerdicts_t
{
	mcv_MESSAGE_CHECKED				=	0x00000001 ,	// Сообщение проверено
	mcv_MESSAGE_CHANGED				=	0x00000002 ,	// Сообщение было изменено при проверке
	mcv_MESSAGE_AV_INFECTED			=	0x00000004 ,	// Антивирус: Сообщение инфицировано
	mcv_MESSAGE_AV_WARNING			=	0x00000008 ,	// Антивирус: Сообщение подозревается на вирус
	mcv_MESSAGE_AV_DISINFECTED		=	0x00000010 ,	// Антивирус: Сообщение вылечено
	mcv_MESSAGE_AV_SOME_SKIPPED		=	0x00000020 ,	// Антивирус: Некоторые объекты в письме не были проверены
	mcv_MESSAGE_AS_SPAM				=	0x00000040 ,	// Антиспам: В сообщении содержится спам
	mcv_MESSAGE_AS_PROBABLE_SPAM	=	0x00000080 ,	// Антиспам: Сообщение подозревается на спам
	mcv_MESSAGE_DELETED				=	0x00000100 ,	// Сообщение удалено (IO уже закрыто)
};
//! Перечень вердиктов, выдаваемых на письмо (битовые маски)
//////////////////////////////////////////////////////////////////////////

//! Перечень типов источников объектов писем
enum MailCheckerDirectionTypes
{
	maildt_All = 0,
	maildt_Incoming,
	maildt_Outgoing
};

//-----------------------------------------------------------------------------
// cMCSettings structure

struct cMCSettings : public cProtectionSettings
{
	cMCSettings():
		cProtectionSettings(),
		m_bCheckTrafficMonitorMessages(cTRUE),
		m_bCheckExternalPluginMessages(cTRUE),
		m_bCheckIncomingMessagesOnly(cFALSE),
		m_bCheckMassMailer(cTRUE),
		m_dwMassMailerCount(5),
		m_dwMassMailerTimeout(5)
		{};

	DECLARE_IID_SETTINGS( cMCSettings, cProtectionSettings, PID_MCTASK );

	tBOOL		m_bCheckTrafficMonitorMessages;
	tBOOL		m_bCheckExternalPluginMessages;
	tBOOL		m_bCheckIncomingMessagesOnly;

	tBOOL		m_bCheckMassMailer;		//!< Включать детект масс-мейлеров
	tDWORD		m_dwMassMailerCount;	//!< Максимальное количество писем за временное окно m_dwMassMailerTimeout
	tDWORD		m_dwMassMailerTimeout;	//!< Временное окно для детекта масс-мейлера, секунды
};

IMPLEMENT_SERIALIZABLE_BEGIN( cMCSettings, 0 )
	SER_BASE( cProtectionSettings,  0 )
	SER_VALUE( m_bCheckTrafficMonitorMessages, tid_BOOL,   "CheckTrafficMonitorMessages"  )
	SER_VALUE( m_bCheckExternalPluginMessages, tid_BOOL,   "CheckExternalPluginMessages"  )
	SER_VALUE( m_bCheckIncomingMessagesOnly,   tid_BOOL,   "CheckIncomingMessagesOnly"  )
	SER_VALUE( m_bCheckMassMailer,             tid_BOOL,   "CheckMassMailer" )
	SER_VALUE( m_dwMassMailerCount,            tid_DWORD,  "MassMailerCount" )
	SER_VALUE( m_dwMassMailerTimeout,          tid_DWORD,  "MassMailerTimeout" )
IMPLEMENT_SERIALIZABLE_END()


//struct cMCSettings : public cProtectionSettings
//{
//	cMCSettings():
//		cProtectionSettings(),
//		m_eDirectionType(maildt_All),
//		m_dwSMTPPort(25),
//		m_dwPOP3Port(110),
//		m_bCheckSMTPTraffic(cTRUE),
//		m_bCheckPOP3Traffic(cTRUE),
//		m_bCheckOutlookTraffic(cTRUE),
//		m_ulSettingsVersion(0)
//		{}
//
//	DECLARE_IID_SETTINGS( cMCSettings, cProtectionSettings, PID_MCTASK );
//
//	MailCheckerDirectionTypes m_eDirectionType;
//	tWORD	                  m_dwSMTPPort;
//	tWORD	                  m_dwPOP3Port;
//	tBOOL	                  m_bCheckSMTPTraffic;
//	tBOOL	                  m_bCheckPOP3Traffic;
//	tBOOL	                  m_bCheckOutlookTraffic;
//	tULONG	                  m_ulSettingsVersion;
//};
//
//IMPLEMENT_SERIALIZABLE_BEGIN( cMCSettings, 0 )
//	SER_BASE( cProtectionSettings,  0 )
//	SER_VALUE( m_eDirectionType,       tid_WORD,  "DirectionType" )
//	SER_VALUE( m_dwSMTPPort,           tid_WORD,  "SMTPPort" )
//	SER_VALUE( m_dwPOP3Port,           tid_WORD,  "POP3Port" )
//	SER_VALUE( m_bCheckSMTPTraffic,    tid_BOOL,  "CheckSMTPTraffic" )
//	SER_VALUE( m_bCheckPOP3Traffic,    tid_BOOL,  "CheckPOP3Traffic" )
//	SER_VALUE( m_bCheckOutlookTraffic, tid_BOOL,  "CheckOutlookTraffic" )
//	SER_VALUE( m_ulSettingsVersion,    tid_DWORD, "SettingsVersion" )
//IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

//! Перечень основных действий, производимых над объектом письма
typedef enum
{
	mailpa_Pass = 0,
	mailpa_DeleteObject,
	mailpa_ChangeObjectToText
	// mailpa_Disinfect - Не факт, что такое можно будет сделать
}	tPrimaryAction;

typedef enum
{
	mcstruct_MAILFILTERRESULTACTIONS_SETTINGS = 1,
	mcstruct_MAILFILTER_SETTINGS		     ,
	mcstruct_MBL_SETTINGS                    ,
	mcstruct_MTM_SETTINGS                    ,
	mcstruct_MC_STATISTICS                   ,
	mcstruct_MC_PLUGINS                      ,
	mcstruct_MC_SETTINGS_VERSION             ,
	mcstruct_MC_PROCESS_PARAMS               ,
	mcstruct_MC_PluginSettings               ,
	mcstruct_LAST
} mcstruct_t;

//-----------------------------------------------------------------------------
// Входные и выходные параметры пражского сообщения (для эмуляции функции Process)
struct cMCProcessParams : public cSerializable
{
	cMCProcessParams():
		cSerializable(),
		p_dwTimeOut(-1),
		p_MailCheckerVerdicts(0),
		p_dwASReason(0)
		{}
			
	DECLARE_IID( cMCProcessParams, cSerializable, PID_MCTASK, mcstruct_MC_PROCESS_PARAMS );
		
	tDWORD p_dwTimeOut;				// [IN] таймаут
	tDWORD p_MailCheckerVerdicts;	// [OUT] MailCheckerVerdicts_t
	cStringObj p_szVirusName;       // [OUT] имя вируса, найденного в письме
	tDWORD p_dwASReason;            // [OUT] причина антиспама (asDetectReason_t)
};

IMPLEMENT_SERIALIZABLE_BEGIN( cMCProcessParams, 0 )
	SER_VALUE( p_dwTimeOut,           tid_DWORD,      "TimeOut" )
	SER_VALUE( p_MailCheckerVerdicts, tid_DWORD,      "MailCheckerVerdicts" )
	SER_VALUE( p_szVirusName,         tid_STRING_OBJ, "VirusName" )
	SER_VALUE( p_dwASReason,          tid_DWORD,      "ASReason"  )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cMCPluginSettings : public cSerializable
{
	cMCPluginSettings():
		m_bCheckOnDelivery(cTRUE),
		m_bCheckOnRead(cTRUE),
		m_bCheckOnSend(cTRUE)
	{}
	
	DECLARE_IID(cMCPluginSettings, cSerializable, PID_MCTASK, mcstruct_MC_PluginSettings);
	
	tBOOL m_bCheckOnDelivery;
	tBOOL m_bCheckOnRead;
	tBOOL m_bCheckOnSend;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cMCPluginSettings, 0 )
	SER_VALUE( m_bCheckOnDelivery,       tid_BOOL,                   "CheckOnDelivery" )
	SER_VALUE( m_bCheckOnRead,           tid_BOOL,                   "CheckOnRead" )
	SER_VALUE( m_bCheckOnSend,           tid_BOOL,                   "CheckOnWriteComplete" )
IMPLEMENT_SERIALIZABLE_END()


#define npNO_NEED_TREATMENT ((tSTRING)("NO_NEED_TREATMENT")) // указание не лечить (все другое возможно) при проверке

//-----------------------------------------------------------------------------

//	//-----------------------------------------------------------------------------
//	// Настройки плагина, необходимые для MailCommon
//
//	struct cMailPluginSettings_Common : public cSerializable
//	{
//		cMailPluginSettings_Common() :
//			bOptional (cTRUE)
//			{}
//
//		DECLARE_IID(cMailPluginSettings_Common, cSerializable, PID_MCTASK, mcstruct_MC_PLUGINS);
//
//		tBOOL bOptional;	//!< cFALSE - трактовать  как ошибку неудачную загрузку этого плагина
//	};
//
//	/*
//	IMPLEMENT_SERIALIZABLE_BEGIN( cMailPluginSettings_Common )
//		SER_VALUE( cMailPluginSettings_Common, bOptional, tid_BOOL, "Optional" ),	//!< cFALSE - трактовать  как ошибку неудачную загрузку этого плагина
//	IMPLEMENT_SERIALIZABLE_END( cMailPluginSettings_Common, 0 );
//	*/
//
//	//-----------------------------------------------------------------------------
//	//! Настройки действий по результату фильтра
//
//	struct cMailFilterParams;
//
//	struct cMailFilterResultActions : public cSerializable 
//	{
//		cMailFilterResultActions() : 
//			cSerializable(),
//			szResultName				(),
//			dwResultId					(0),
//			PrimaryAction				(mailpa_Pass),
//			szPlaceholderTemplate		(),
//			szNextFilter				(),
//			bStopProcessing				(cFALSE),
//			TotalPrimaryAction			(0),
//			bSendReportToAdmin			(cFALSE),
//			szAdminNotifyAddress		(),
//			szNotifyFromAddress			(),
//			szNotifyAdminTemplate		(),
//			bSendReportToRecipient		(cFALSE),
//			szNotifyRecipientTemplate	(),
//			bSendReportToSender			(cFALSE),
//			szNotifySenderTemplate		()
//			{}
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
//			{}
//
//		DECLARE_IID( cMailFilterResultActions, cSerializable, PID_MCTASK, mcstruct_MAILFILTERRESULTACTIONS_SETTINGS );
//			
//		cStringObj szResultName;				//!< Имя результата фильтрации
//		tDWORD dwResultId;						//!< Идентификатор результата фильтрации
//		//////////////////////////////////////////////////////////////////////////
//		// primary action is following next
//		tPrimaryAction PrimaryAction;			//!< Основное действие с объектом 
//		cStringObj szPlaceholderTemplate;		//!< Шаблон заменяющего текста
//		//////////////////////////////////////////////////////////////////////////
//		cStringObj szNextFilter;				//!< Указатель на следующий фильтр (м.б. = "")
//		tBOOL bStopProcessing;					//!< Остановка фильтрации
//		//////////////////////////////////////////////////////////////////////////
//		// total actions are following next (used only for "bStopProcessing == cTRUE")
//		tTotalPrimaryAction TotalPrimaryAction;	//!< Основное главное действие 
//		tBOOL bSendReportToAdmin;				//!< Посылать ли отчет администратору?
//		cStringObj szAdminNotifyAddress;		//!< Адрес администратора для рассылки отчетов
//		cStringObj szNotifyFromAddress;			//!< Адрес ответственного лица за рассылку отчетов (м.б. = szAdminNotifyAddress)
//		cStringObj szNotifyAdminTemplate;		//!< Шаблон отчета администратору
//		tBOOL bSendReportToRecipient;			//!< Посылать ли отчет получателю?
//		cStringObj szNotifyRecipientTemplate;	//!< Шаблон отчета получателю
//		tBOOL bSendReportToSender;				//!< Посылать ли отчет отправителю?
//		cStringObj szNotifySenderTemplate;		//!< Шаблон отчета отправителю
//	};
//
//	/*
//	IMPLEMENT_SERIALIZABLE_BEGIN( cMailFilterResultActions )
//		SER_COOL_SERIALIZER_IN_PLACE( cMailFilterResultActions, szResultName, &gCSS_str, tid_STRING, "ResultName" ),				//!< Имя результата фильтрации
//		//SER_VALUE( cMailFilterResultActions, dwResultId, tid_DWORD, "ResultId" ),						//!< Идентификатор результата фильтрации
//
//		//////////////////////////////////////////////////////////////////////////
//		// primary action is following next
//		SER_VALUE( cMailFilterResultActions, PrimaryAction, tid_DWORD, "PrimaryAction" ),			//!< Основное действие с объектом 
//		SER_COOL_SERIALIZER_IN_PLACE( cMailFilterResultActions, szPlaceholderTemplate, &gCSS_wstr, tid_WSTRING, "PlaceholderTemplate" ),		//!< Шаблон заменяющего текста
//
//		//////////////////////////////////////////////////////////////////////////
//		SER_COOL_SERIALIZER_IN_PLACE( cMailFilterResultActions, szNextFilter, &gCSS_str, tid_STRING, "NextFilter" ),				//!< Указатель на следующий фильтр (м.б. = "")
//		SER_VALUE( cMailFilterResultActions, bStopProcessing, tid_BOOL, "StopProcessing" ),					//!< Остановка фильтрации
//
//		//////////////////////////////////////////////////////////////////////////
//		// total actions are following next (used only for "bStopProcessing == cTRUE")
//		SER_VALUE( cMailFilterResultActions, TotalPrimaryAction, tid_DWORD, "TotalPrimaryAction" ),	//!< Основное главное действие 
//		SER_VALUE( cMailFilterResultActions, bSendReportToAdmin, tid_BOOL, "SendReportToAdmin" ),				//!< Посылать ли отчет администратору?
//		SER_COOL_SERIALIZER_IN_PLACE( cMailFilterResultActions, szAdminNotifyAddress, &gCSS_wstr, tid_WSTRING, "AdminNotifyAddress" ),		//!< Адрес администратора для рассылки отчетов
//		SER_COOL_SERIALIZER_IN_PLACE( cMailFilterResultActions, szNotifyFromAddress, &gCSS_wstr, tid_WSTRING, "NotifyFromAddress" ),		//!< Адрес ответственного лица за рассылку отчетов (м.б. = szAdminNotifyAddress)
//
//		SER_COOL_SERIALIZER_IN_PLACE( cMailFilterResultActions, szNotifyAdminTemplate, &gCSS_wstr, tid_WSTRING, "NotifyAdminTemplate" ),		//!< Шаблон отчета администратору
//		SER_VALUE( cMailFilterResultActions, bSendReportToRecipient, tid_BOOL, "SendReportToRecipient" ),			//!< Посылать ли отчет получателю?
//		SER_COOL_SERIALIZER_IN_PLACE( cMailFilterResultActions, szNotifyRecipientTemplate, &gCSS_wstr, tid_WSTRING, "NotifyRecipientTemplate" ),	//!< Шаблон отчета получателю
//		SER_VALUE( cMailFilterResultActions, bSendReportToSender, tid_BOOL, "SendReportToSender" ),				//!< Посылать ли отчет отправителю?
//		SER_COOL_SERIALIZER_IN_PLACE( cMailFilterResultActions, szNotifySenderTemplate, &gCSS_wstr, tid_WSTRING, "NotifySenderTemplate" ),	//!< Шаблон отчета отправителю
//	IMPLEMENT_SERIALIZABLE_END_KEY_NAME_BY_FIELD_VALUE( cMailFilterResultActions, dwResultId, tid_DWORD, 0 );
//	*/
//
//	//-----------------------------------------------------------------------------
//	//! Настройки runtime-фильтра (алгоритм фильтрации одного фильтра) + настройки процессирования плагина ("process_params")
//
//	struct cMailFilterParams : public cSerializable 
//	{
//		cMailFilterParams() : 
//			cSerializable(),
//			dwPID( PID_ANY ),
//			szFilterName (),
//			process_params ()
//			{}
//
//		cMailFilterParams( const cMailFilterParams& o ) : 
//			cSerializable(),
//			dwPID( o.dwPID ),
//			szFilterName ( o.szFilterName ),
//			process_params ( o.process_params )
//			{}
//			
//		DECLARE_IID( cMailFilterParams, cSerializable, PID_MCTASK, mcstruct_MAILFILTER_SETTINGS );
//
//		tDWORD dwPID;							//!< Идентификатор пражского плагина. Настройки данного плагина расположены в интерфейсе iRegistry свойства i_task.h::pgTASK_CONFIG в одноименной ветке
//		cStringObj szFilterName;				//!< Уникальное имя runtime-фильтра 
//		cStringObj process_params;				//!< Краткое имя ветки реестра, содержащей настройки данного фильтра. 
//	};
//
//	/*
//	IMPLEMENT_SERIALIZABLE_BEGIN( cMailFilterParams )
//		SER_VALUE( cMailFilterParams, dwPID, tid_DWORD, "PID" ),							//!< Идентификатор пражского плагина. Настройки данного плагина расположены в интерфейсе iRegistry свойства i_task.h::pgTASK_CONFIG в одноименной ветке
//		//SER_COOL_SERIALIZER_IN_PLACE( cMailFilterParams, szFilterName, &gCSS_str, tid_STRING, "FilterName" ),				//!< Уникальное имя runtime-фильтра 
//		SER_COOL_SERIALIZER_IN_PLACE( cMailFilterParams, process_params, &gCSS_str, tid_STRING, "process_params" ),				//!< Полное имя ветки реестра, содержащей настройки данного фильтра. Настройки читаются через интерфейс pGetProp
//	IMPLEMENT_SERIALIZABLE_END_KEY_NAME_BY_FIELD_VALUE( cMailFilterParams, szFilterName, tid_STRING, &gCSS_strByKey );
//	*/
//
//	//-----------------------------------------------------------------------------
//	//! Список фильтров
//
//	struct cMBLParams : public cSerializable
//	{
//		cMBLParams() :
//			cSerializable(),
//			szMailTaskType(),
//			szMailFilterList (),
//			szMailPartFilterList (),
//			ulThreadCount( 1 )
//			{}
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
//			{}
//			
//		DECLARE_IID( cMBLParams, cSerializable, PID_MCTASK, mcstruct_MBL_SETTINGS );
//
//		cStringObj szMailTaskType;				//!< Тип задачи почтовой фильтрации
//		typedef cVector<tPID> MailInterceptorLoaders_t;
//		MailInterceptorLoaders_t MailInterceptorLoaders;	//!< Список почтовых перехватчиков (PID'ы их загрузчиков)
//		cStringObj szMailFilterList;			//!< Упорядоченный список фильтров уровня письма (имена фильтров cMailFilterParams::szFilterName через разделитель)
//		cStringObj szMailPartFilterList;		//!< Упорядоченный список фильтров уровня части письма (имена фильтров cMailFilterParams::szFilterName через разделитель)
//		tULONG ulThreadCount;					//!< Количество потоков, в которых должен исполняться алгоритм MBL
//		typedef cVector<cMailFilterParams> MailFilterParams_t;
//		MailFilterParams_t MailFilters;			//!< Алгоритмы фильтрации (уровня письма)
//		MailFilterParams_t MailPartFilters;		//!< Алгоритмы фильтрации (уровня части письма)
//	};
//
//	/*
//	IMPLEMENT_SERIALIZABLE_BEGIN( cMBLParams )
//		SER_COOL_SERIALIZER( cMBLParams, MailInterceptorLoaders, &gCSV_dwByKey, tid_DWORD, "MailInterceptorLoaders" ),	//!< Список почтовых перехватчиков (PID'ы их загрузчиков)
//		SER_COOL_SERIALIZER_IN_PLACE( cMBLParams, szMailFilterList, &gCSS_str, tid_STRING, "MailFilterList" ),			//!< Упорядоченный список фильтров уровня письма (имена фильтров cMailFilterParams::szFilterName через разделитель)
//		SER_COOL_SERIALIZER_IN_PLACE( cMBLParams, szMailPartFilterList, &gCSS_str, tid_STRING, "MailPartFilterList" ),		//!< Упорядоченный список фильтров уровня части письма (имена фильтров cMailFilterParams::szFilterName через разделитель)
//		SER_VALUE( cMBLParams, ulThreadCount, tid_DWORD, "ThreadCount" ),					//!< Количество потоков, в которых должен исполняться алгоритм MBL
//		SER_COOL_SERIALIZER( cMBLParams, MailFilters, &gCSV_MailFilterParams, tid_VOID, "MailFilters" ),			//!< Алгоритмы фильтрации (уровня письма)
//		SER_COOL_SERIALIZER( cMBLParams, MailPartFilters, &gCSV_MailFilterParams, tid_VOID, "MailPartFilters" ),		//!< Алгоритмы фильтрации (уровня части письма)
//	IMPLEMENT_SERIALIZABLE_END_KEY_NAME_BY_FIELD_VALUE( cMBLParams, szMailTaskType, tid_STRING, &gCSS_strByKey );
//	*/
//
//	//-----------------------------------------------------------------------------
//	//! Настройки MTM (MailTaskManager)
//
//	struct cMTMParams : public cSerializable
//	{
//		cMTMParams() :
//			cSerializable(),
//			dwProtectionLevel(0)
//			{}
//
//		cMTMParams( const cMTMParams& o ) :
//			cSerializable(),
//			dwProtectionLevel( o.dwProtectionLevel ),
//			MailPlugins( o.MailPlugins ),
//			MBLs( o.MBLs )
//			{}
//
//		DECLARE_IID( cMTMParams, cSerializable, PID_MCTASK, mcstruct_MTM_SETTINGS );
//
//		tDWORD dwProtectionLevel;				//!< Уровень защиты
//		typedef cVector<tPID> MailPluginParams_t;
//		MailPluginParams_t MailPlugins;			//!< Список плагинов
//		typedef cVector<cMBLParams> MBLParams_t;
//		MBLParams_t MBLs;						//!< Списки фильтров и алгоритмы фильтрации
//	};
//
//	/*
//	IMPLEMENT_SERIALIZABLE_BEGIN( cMTMParams )
//		SER_VALUE( cMTMParams, dwProtectionLevel, tid_DWORD, "ProtectionLevel" ),
//		SER_COOL_SERIALIZER( cMTMParams, MBLs, &gCSV_MBLParams, tid_VOID, "MBLs" ),						//!< Списки фильтров и алгоритмы фильтрации
//		SER_COOL_SERIALIZER( cMTMParams, MailPlugins, &gCSV_dwByKey, tid_DWORD, "MailPlugins" ),		//!< Список плагинов
//	IMPLEMENT_SERIALIZABLE_END( cMTMParams, 0 );
//	*/
//
//	//-----------------------------------------------------------------------------
//	//! Статистика, предоставляемая MailCommon
//
//	struct cMailCommonStatistics: public cTaskStatistics 
//	{
//		cMailCommonStatistics():
//			cTaskStatistics(),
//			m_szSenderName(),
//			m_nNumObjects(0),
//			m_timeLastObject(-1)
//		{}
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
//					for (tUINT i=0; i < m_nNumVerdicts.size(); i++)
//						m_nNumVerdicts[i] += o.m_nNumVerdicts[i];
//				}
//				m_timeLastObject = o.m_timeLastObject;
//			}
//			return *this;
//		}
//
//		DECLARE_IID( cMailCommonStatistics, cTaskStatistics , PID_MCTASK, mcstruct_MC_STATISTICS );
//
//		cStringObj        m_szSenderName;                   // Имя того объекта, который опубликовал данную статистику. 
//																												// Это позволяет собирать статистику отдельно для каждого модуля
//		long              m_nNumObjects;                    // Количество отданных на проверку в MailCommon объектов
//		cVector<long>     m_nNumVerdicts;			        // Количество вердиктов (по типам). TODO: Не очень хорошее решение с точки зрения требования к памяти: для того, чтобы прописать значение сотого элементв, необходимо разпределить память для 99 лишних элементов!
//		__time32_t        m_timeLastObject;                 // Время обработки последнего объекта
//		
//	};
//
//	//-----------------------------------------------------------------------------
//
//	struct cMCSettingsVersion : public cSerializable
//	{
//		cMCSettingsVersion():
//			cSerializable(),
//			m_ulSettingsVersion(0)
//			{}
//
//		DECLARE_IID( cMCSettingsVersion, cSerializable, PID_MCTASK, mcstruct_MC_SETTINGS_VERSION );
//
//		tULONG	m_ulSettingsVersion;
//	};
//
//	IMPLEMENT_SERIALIZABLE_BEGIN( cMCSettingsVersion, 0 )
//		SER_VALUE( m_ulSettingsVersion, tid_BOOL, "SettingsVersion" )
//	IMPLEMENT_SERIALIZABLE_END()
//
//	//-----------------------------------------------------------------------------

#endif//  __S_MC_H

// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  26 October 2004,  17:09 --------
// File Name   -- (null)i_avpg.cpp
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_avpg__37b13221_ae7b_11d4_b757_00d0b7170e50 )
#define __i_avpg__37b13221_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// AVPG interface implementation
// Short comments -- Описание интерфейса для взаимодействия с драйвером-перехватчиком
//    Интерфейс предоставляет доступ к драйверу avpg (sys/vxd) Позволяет устанавливать в драйвере заранее определённую систему фильтров для получения уведомления о событиях в файловой системе (открытие, запсиь, закрытие файлов, переименование, удаление); установку защиты на текущий процесс; распараллеливание обработки событий.
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_AVPG  ((tIID)(61004))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         tAVPG_CLIENT_ACTIVITY_MODE; // режим активности клиента
typedef tDWORD                         tAVPG_PROCESS_OBJECT_RESULTS; // описание результата обработки события
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )

// ACTIVITY_MODE_PROPERTY_CONSTANT
// константы для описания возможого состояния клиента
#define cAVPG_CLIENT_ACTIVITY_MODE_SLEEP ((tAVPG_CLIENT_ACTIVITY_MODE)(0)) // события для клиента не генерируются перехватчиком
#define cAVPG_CLIENT_ACTIVITY_MODE_ACTIVE ((tAVPG_CLIENT_ACTIVITY_MODE)(1)) // клиент активен и обязан обрабатывать события за указанное промежуток времени

// EVENT ATTRIBUTES
// описание атрибутов данного события
// атрибуты события или обрабатываемого объекта
#define cAVPG_EVENT_DEFINITION_PROP    ((tSTRING)("i_avpg event definiton")) // описывает внутренние данные о событии
#define cAVPG_USER_DEFINITION_PROP     ((tSTRING)("i_avpg User info")) // описывает пользователя, вызвавшего событие
#define cAVPG_PROCESS_DEFINITION_PROP  ((tSTRING)("i_avpg Process")) // содержит имя процесса, вызвавшего событие, в ANSI кодировке
#define cAVPG_THREAD_DEFINITION_PROP   ((tSTRING)("i_avpg Thread definition")) // содержит данные о рабочем потоке обрабатывающего событие
#define cAVPG_PROP_OBJECT_MODIFIED     ((tSTRING)("i_avpg_object modified")) // наличие данного атрибута у объектауказывает что он был изменён до генерации события
#define cAVPG_PROP_EVENT_PROCESSID     ((tSTRING)("i_avpg_event process id")) // ProcessID перехваченного процесса
#define cAVPG_CRUSER_DEFINITION_PROP   ((tSTRING)("i_avpg creator User info")) // описывает пользователя, создавшего событие

// Notification messages
// нотификационные сообщения
// все сообщения выполняются на рабочем потоке, обрабатывающего событие
#define pmc_AVPG                       ((tDWORD)(1222188959)) // идентификатор класса нотификационных сообщений
#define pm_AVPG_NOTIFY_THREAD_STARTED  ((tDWORD)(201)) // создан новый рабочий поток
#define pm_AVPG_NOTIFY_THREAD_STOPPED  ((tDWORD)(202)) // рабочий поток будет остановлен после обработки нотификации
#define pm_AVPG_NOTIFY_MISSED_OBJECT   ((tDWORD)(203)) // при обработке события перехватчик не останавливал обработку объекта до получения вердикта клиента
#define pm_AVPG_NOTIFY_FOREACHTHREAD   ((tDWORD)(204)) // обработка указанной в параметрах сообщения для каждого рабочего потока (in/out)
#define pm_AVPG_NOTIFY_VOLUME_MOUNTED  ((tDWORD)(205)) // системой выполено подключение нового тома
#define pm_AVPG_NOTIFY_DRIVER_CONNECTION_IS_BROKEN ((tDWORD)(206)) // сообщение что по каким-либо внутренним причинам разорван канал получения событий от перехватчика
#define pm_AVPG_NOTIFY_WRITEBOOT_DETECTED ((tDWORD)(207)) // нотификация о изменении служебных областей тома
#define pm_AVPG_NOTIFY_CACHESIZE_CHANGED ((tDWORD)(208)) // нотификация что при регистрации перехватчик изменил запрашиваемый размер кэша (нехватка памяти...)
#define pm_AVPG_NOTIFY_TERMINATE_PROCESS_DETECTED ((tDWORD)(209)) // событие что текущиё процесс пытаются остановить извне
#define pm_AVPG_NOTIFY_FORMAT_TRACK    ((tDWORD)(210)) // нотификация о форматировании служебных областей тома

// Object processing
// запрос на обработку события или объекта
#define pm_AVPG_PROCEED_OBJECT         ((tDWORD)(301)) // запрос на обработку объекта

// Control messages
// управляющие сообщения
#define pmc_AVPG_CONTROL               ((tDWORD)(1222188960)) // управляющие сообщения для экземпляра объекта
#define pm_AVPG_CONTROL_PROCESS_PROTECTION_ON ((tDWORD)(401)) // включить защиту для текущего процесса
#define pm_AVPG_CONTROL_PROCESS_PROTECTION_OFF ((tDWORD)(402)) // отключить защиту для текущего процесса
#define pm_AVPG_CONTROL_PROCESS_PROTECTION_QUERY ((tDWORD)(403)) // запрос на текущее состояние защиты
#define pm_AVPG_CONTROL_INCREASEUNSAFE ((tDWORD)(404)) // управление уровнем безопасного открытия при остановленной обработке события
#define pm_AVPG_CONTROL_DECREASEUNSAFE ((tDWORD)(405)) // уменьшить уровень безопасности открытия при остановленной обработке объекта
#define pm_AVPG_CONTROL_SET_STAT_LOGFILENAME ((tDWORD)(501)) // установить имя файла с отчетом по статистике
#define pm_AVPG_CONTROL_SET_PREVERDICT ((tDWORD)(601)) // установить предварительный Verdict для обрабатываемого объекта
#define pm_AVPG_CONTROL_GETEVENT_THREAD_ID ((tDWORD)(602)) // позволяет получить идентификатор потока, сгенерировавшего данное событие
#define pm_AVPG_CONTROL_PREFETCH       ((tDWORD)(603)) // управление предварительным чтением (prefetch) при открытии файла
#define pm_AVPGS_INCOME               ((tDWORD)(0x8cdea98d))
#define pm_AVPG_CONTROL_CHECKBACKUP   ((tDWORD)(604)) // проверять бакап операции с файлами
#define pm_AVPG_CONTROL_SKIPBACKUP    ((tDWORD)(605)) // не проверять бакап операции с файлами
#define pm_AVPG_CONTROL_GET_PROCESS_HASH ((tDWORD)(606)) // выдать хэш пускового файла процесса

// Return codes for pm_AVPG_PROCEED_OBJECT
// коды возврата для обрабатываемого объекта
#define cAVPG_RC_ALLOW                 ((tAVPG_PROCESS_OBJECT_RESULTS)(0)) // разрешить дальнейшую обработку объекта. Текущий результат не кешировать
#define cAVPG_RC_ALLOW_CACHE           ((tAVPG_PROCESS_OBJECT_RESULTS)(1)) // разрешить дальнейшую обработку объекта. объект поместить в кэш
#define cAVPG_RC_DENY_ACCESS           ((tAVPG_PROCESS_OBJECT_RESULTS)(2)) // запретить дальнейшую обработку объекта
#define cAVPG_RC_DELETE_OBJECT         ((tAVPG_PROCESS_OBJECT_RESULTS)(3)) // запретить дальнейшую обработку объекта. После обработки объект удалить
#define cAVPG_RC_KILL_PROCESS          ((tAVPG_PROCESS_OBJECT_RESULTS)(4)) // запретить дальнейшую обработку объекта. После обработки остановить процесс, вызвавший событие
#define cAVPG_RC_GO_DELAYED            ((tAVPG_PROCESS_OBJECT_RESULTS)(5)) // разрешить дальнейшую обработку объекта. событие перекладывается на повторную отложенную обработку

// - statistic property
// custom свойства по сбору статистики
// Все проперти устанавливаются на hString - базовый объект с именем обрабатываемого объекта (кроме проперти LogFileName)
#define cAVPG_STAT_TIME_EVENT_START_PROCESSING ((tSTRING)("i_avpg Event start processing")) // стартовое время статистики (PerfomanceTimer)
#define cAVPG_STAT_TIME_BEFORE_CREATE_IO ((tSTRING)("i_avpg Before create io")) // замер времени перед созданием IO
#define cAVPG_STAT_TIME_BEFORE_CHECK1  ((tSTRING)("i_avpg Before engine 1 check")) //  --
#define cAVPG_STAT_TIME_BEFORE_CHECK2  ((tSTRING)("i_avpg Before engine 2 check")) //  --
#define cAVPG_STAT_TIME_BEFORE_CHECK3  ((tSTRING)("i_avpg Before engine 3 check")) //  --
#define cAVPG_STAT_TIME_EVENT_END_PROCESSING ((tSTRING)("i_avpg Event end processing")) // устанавливается на объект после получения результата проверки
#define cAVPG_STAT_RESULT_CREATE_IO    ((tSTRING)("i_avpg Create IO result")) //  --
#define cAVPG_STAT_RESULT_PROCESSING1  ((tSTRING)("i_avpg Processing1")) //  --
#define cAVPG_STAT_RESULT_PROCESSING_VERDICT ((tSTRING)("i_avpg Processing result")) //  --
#define cAVPG_STAT_TIME_ICHECKER_READ  ((tSTRING)("i_avpg IChecker read")) //  --
#define cAVPG_STAT_TIME_EVENT_VERDICT_TIME ((tSTRING)("i_avpg Event verdict time")) //  --

// - Custom origin
#define cAVPG_PROP_CUSTOM_ORIGIN       ((tSTRING)("i_avpg custom origin")) // содержит имя свойства Origin для объекта hString
#define cAVPG_PROP_DOS_DEVICE_NAME     ((tSTRING)("i_avpg dos device name")) //  --

// - custom io
#define cAVPG_PROP_CUSTOM_IO           ((tSTRING)("i_avpg allow create special io")) // like MiniKavIO
#define cAVPG_PROP_STEAMED_FIXED_MEDIA ((tSTRING)("i_avpg streamed fixed media")) // Идентификатор свойства, используемого для объектов, находящихся на FIXED дисках с файловой системой поддерживающей Alternative Data Streams (ADS)
#define cAVPG_PROP_DELAYED_PROCESSING  ((tSTRING)("i_avpg this is delayed processing")) // повторная проверка при ответе cAVPG_RC_GO_DELAYED

// -
#define cAVPG_WORKING_MODE_ON_ACCESS   ((tDWORD)(0)) //  --
#define cAVPG_WORKING_MODE_ON_MODIFY   ((tDWORD)(1)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hAVPG;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iAVPGVtbl;
typedef struct iAVPGVtbl iAVPGVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cAVPG;
	typedef cAVPG* hAVPG;
#else
	typedef struct tag_hAVPG 
	{
		const iAVPGVtbl*   vtbl; // pointer to the "AVPG" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hAVPG;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( AVPG_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpAVPG_ChangeActivityMode)  ( hAVPG _this, tBOOL* result, tAVPG_CLIENT_ACTIVITY_MODE ActivityMode ); // -- Изменение режима активности;
	typedef   tERROR (pr_call *fnpAVPG_YieldEvent)          ( hAVPG _this, hOBJECT EventObject, tDWORD YieldTimeout ); // -- Подтверждение обработки события;
	typedef   tERROR (pr_call *fnpAVPG_ResetDriverCache)    ( hAVPG _this );                // -- Очистка кэша объектов;
	typedef   tERROR (pr_call *fnpAVPG_ForEachThreadNotify) ( hAVPG _this, tDWORD* result, tDWORD* pNotifyParamData, tDWORD NotifyParamDataSize ); // -- Указание выполнить рассылку сообщения pm_AVPG_NOTIFY_FOREACHTHREAD на каждом рабочем потоке;
	typedef   tERROR (pr_call *fnpAVPG_StopAllDogsThread)   ( hAVPG _this, tDWORD* result );                // -- Вернуть управление после остановки всех рабочих потоков;
	typedef   tERROR (pr_call *fnpAVPG_AddInvisibleFile)    ( hAVPG _this, tSTRING szFileName ); // -- Установить указанные файловое имя как невидимое;
	typedef   tERROR (pr_call *fnpAVPG_CreateIO)            ( hAVPG _this, hOBJECT* result, hSTRING hObjectName ); // -- Создать IO по указанному имени;
	typedef   tERROR (pr_call *fnpAVPG_CopyConext)          ( hAVPG _this, hOBJECT SourceObject, hOBJECT DestinationObject ); // -- Скопировать собственные служебные атрибуты для указанных объектов;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iAVPGVtbl 
{
	fnpAVPG_ChangeActivityMode   ChangeActivityMode;
	fnpAVPG_YieldEvent           YieldEvent;
	fnpAVPG_ResetDriverCache     ResetDriverCache;
	fnpAVPG_ForEachThreadNotify  ForEachThreadNotify;
	fnpAVPG_StopAllDogsThread    StopAllDogsThread;
	fnpAVPG_AddInvisibleFile     AddInvisibleFile;
	fnpAVPG_CreateIO             CreateIO;
	fnpAVPG_CopyConext           CopyConext;
}; // "AVPG" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( AVPG_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION     mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT     mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgWorkingThreadMaxCount mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001004 )
#define pgDriverCacheSize       mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001005 )
#define pgClientActivityMode    mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001006 )
#define pgWorkingMode           mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001007 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_AVPG_ChangeActivityMode( _this, result, ActivityMode )                            ((_this)->vtbl->ChangeActivityMode( (_this), result, ActivityMode ))
	#define CALL_AVPG_YieldEvent( _this, EventObject, YieldTimeout )                               ((_this)->vtbl->YieldEvent( (_this), EventObject, YieldTimeout ))
	#define CALL_AVPG_ResetDriverCache( _this )                                                    ((_this)->vtbl->ResetDriverCache( (_this) ))
	#define CALL_AVPG_ForEachThreadNotify( _this, result, pNotifyParamData, NotifyParamDataSize )  ((_this)->vtbl->ForEachThreadNotify( (_this), result, pNotifyParamData, NotifyParamDataSize ))
	#define CALL_AVPG_StopAllDogsThread( _this, result )                                           ((_this)->vtbl->StopAllDogsThread( (_this), result ))
	#define CALL_AVPG_AddInvisibleFile( _this, szFileName )                                        ((_this)->vtbl->AddInvisibleFile( (_this), szFileName ))
	#define CALL_AVPG_CreateIO( _this, result, hObjectName )                                       ((_this)->vtbl->CreateIO( (_this), result, hObjectName ))
	#define CALL_AVPG_CopyConext( _this, SourceObject, DestinationObject )                         ((_this)->vtbl->CopyConext( (_this), SourceObject, DestinationObject ))
#else // if !defined( __cplusplus )
	#define CALL_AVPG_ChangeActivityMode( _this, result, ActivityMode )                            ((_this)->ChangeActivityMode( result, ActivityMode ))
	#define CALL_AVPG_YieldEvent( _this, EventObject, YieldTimeout )                               ((_this)->YieldEvent( EventObject, YieldTimeout ))
	#define CALL_AVPG_ResetDriverCache( _this )                                                    ((_this)->ResetDriverCache( ))
	#define CALL_AVPG_ForEachThreadNotify( _this, result, pNotifyParamData, NotifyParamDataSize )  ((_this)->ForEachThreadNotify( result, pNotifyParamData, NotifyParamDataSize ))
	#define CALL_AVPG_StopAllDogsThread( _this, result )                                           ((_this)->StopAllDogsThread( result ))
	#define CALL_AVPG_AddInvisibleFile( _this, szFileName )                                        ((_this)->AddInvisibleFile( szFileName ))
	#define CALL_AVPG_CreateIO( _this, result, hObjectName )                                       ((_this)->CreateIO( result, hObjectName ))
	#define CALL_AVPG_CopyConext( _this, SourceObject, DestinationObject )                         ((_this)->CopyConext( SourceObject, DestinationObject ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iAVPG 
	{
		virtual tERROR pr_call ChangeActivityMode( tBOOL* result, tAVPG_CLIENT_ACTIVITY_MODE ActivityMode ) = 0; // -- Изменение режима активности
		virtual tERROR pr_call YieldEvent( hOBJECT EventObject, tDWORD YieldTimeout ) = 0; // -- Подтверждение обработки события
		virtual tERROR pr_call ResetDriverCache() = 0; // -- Очистка кэша объектов
		virtual tERROR pr_call ForEachThreadNotify( tDWORD* result, tDWORD* pNotifyParamData, tDWORD NotifyParamDataSize ) = 0; // -- Указание выполнить рассылку сообщения pm_AVPG_NOTIFY_FOREACHTHREAD на каждом рабочем потоке
		virtual tERROR pr_call StopAllDogsThread( tDWORD* result ) = 0; // -- Вернуть управление после остановки всех рабочих потоков
		virtual tERROR pr_call AddInvisibleFile( tSTRING szFileName ) = 0; // -- Установить указанные файловое имя как невидимое
		virtual tERROR pr_call CreateIO( hOBJECT* result, hSTRING hObjectName ) = 0; // -- Создать IO по указанному имени
		virtual tERROR pr_call CopyConext( hOBJECT SourceObject, hOBJECT DestinationObject ) = 0; // -- Скопировать собственные служебные атрибуты для указанных объектов
	};

	struct pr_abstract cAVPG : public iAVPG, public iObj 
	{
		OBJ_IMPL( cAVPG );
		operator hOBJECT() { return (hOBJECT)this; }
		operator hAVPG()   { return (hAVPG)this; }

		tDWORD pr_call get_pgWorkingThreadMaxCount() { return (tDWORD)getDWord(pgWorkingThreadMaxCount); }
		tERROR pr_call set_pgWorkingThreadMaxCount( tDWORD value ) { return setDWord(pgWorkingThreadMaxCount,(tDWORD)value); }

		tDWORD pr_call get_pgDriverCacheSize() { return (tDWORD)getDWord(pgDriverCacheSize); }
		tERROR pr_call set_pgDriverCacheSize( tDWORD value ) { return setDWord(pgDriverCacheSize,(tDWORD)value); }

		tAVPG_CLIENT_ACTIVITY_MODE pr_call get_pgClientActivityMode() { tAVPG_CLIENT_ACTIVITY_MODE value = {0}; get(pgClientActivityMode,&value,sizeof(value)); return value; }
		tERROR pr_call set_pgClientActivityMode( tAVPG_CLIENT_ACTIVITY_MODE value ) { return set(pgClientActivityMode,&value,sizeof(value)); }

		tDWORD pr_call get_pgWorkingMode() { return (tDWORD)getDWord(pgWorkingMode); }
		tERROR pr_call set_pgWorkingMode( tDWORD value ) { return setDWord(pgWorkingMode,(tDWORD)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_avpg__37b13221_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end




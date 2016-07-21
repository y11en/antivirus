/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EventProperties.h
 * \author Михаил Кармазин
 * \date 16:56 22.11.2002
 * \brief Интерфейсы для просмотра и удаления описания событий. 
 *			Под описанием событий имееться в виду представление событий, 
 *			возникающих в системе ( "Virus found" и т.п.), в human-readbale form.
 *			Сервер  предоставляет эти интерфейсы для интсрумента администрирования 
 *			(например, для snap-in'а), чтобы тот мог реализовать соотв. функциональность в 
 *			пользовательском интерфейсе.
 */



#ifndef _KLEVENTROPS_H__5745B60A_831F_4678_816D_9265D1AD094D__
#define _KLEVENTROPS_H__5745B60A_831F_4678_816D_9265D1AD094D__

#include <std/base/klstd.h>
#include <std/par/params.h>
#include <transport/ev/ev_general.h>

namespace KLEVP{
    
	typedef void (*RangeCalcProgressCallback)(
        int nProgress, ///< от 0 до 100
        bool & bStop, ///< поставить в true если надо прекратить вычисления
		void* pParam );///< параметр, переданный при указании колбэка.

    const wchar_t c_szwIteratorId[]                     = L"EVP_ITERATOR_ID"; // STRING_T
    const wchar_t c_szwDeletionResult[]                 = L"EVP_DELETION_RESULT"; // INT_T, см. enum DeletionResult
    const wchar_t c_szwDeletionProgress[]               = L"EVP_DELETION_PROGRESS"; // INT_T
    const wchar_t c_szwDeletionErrId[]                  = L"EVP_DELETION_ERR_ID"; // INT_T

    const wchar_t c_szwMaxSelectedRecNumber[]   = L"EVP_MAX_SELECTED_REC_NUMBER"; // INT_T
    const wchar_t c_szwBlocks[]                 = L"EVP_EVENT_BLOCKS"; // ARRAY_T
    const wchar_t c_szwBlockBegin[]             = L"EVP_EVENT_BLOCK_BEGIN"; // INT_T
    const wchar_t c_szwBlockEnd[]               = L"EVP_EVENT_BLOCK_END"; // INT_T

    const wchar_t c_szwIncludeGnrlEvents[]		= L"EVP_INCL_GNRL_EVENTS"; // BOOL_T
    const wchar_t c_szwLastEventsOnly[]			= L"EVP_LAST_EVENTS_ONLY"; // BOOL_T

	const wchar_t c_szwIncludeTaskStates[]		= L"EVP_INCL_TASK_STATES"; // BOOL_T

    const wchar_t c_szwMaxEventsCount[]			= L"EVP_MAX_EVENTS_COUNT"; // INT_T
	
    const wchar_t c_er_registration_time[]      = L"registration_time";	// DATE_TIME_T
	
    enum DeletionResult {
        DELETION_SUCCEED, ///< Удаление успешно завершено.
        DELETION_FAILED,   ///< Не удалось завершить удаление.
        DELETION_ABORTED,  ///< Удаление прекращено командой (пользователя або еще каго)
        SHALL_REFRESH_BEFORE  ///<  Надо перезапустить (видимо, юзеру) процесс 
                        /// удаления, предварительно обновив данные (т.е. открыв заново итератор),
                        /// т.к. те данные, которые он видит, были частично удалены к моменту инициации удаления,
                        /// кем-то еще.
    };

    /*!
        Событие c_DeletionFinished публикуется сервером при окончании операции
                        массового удаления (инициированной функцией InitiateDelete)
        Параметры:
            c_szwIteratorId        STRING_T - Идентификатор операции удаления, возвращенный ф-й InitiateDelete
            c_szwDeletionResult    INT_T, см. enum DeletionResult
            c_er_descr             STRING_T в случае, когда c_szwDeletionResult == DELETION_FAILED, содержит текстовое описание ошибки
            c_szwDeletionErrId     INT_T в случае, когда c_szwDeletionResult == DELETION_FAILED, содержит код ошибки.

    */
	const wchar_t c_DeletionFinished[]=L"KLEVP_DeletionFinished";

    /*!
        Событие c_DeletionProgress публикуется сервером в процессе операции
                        массового удаления (инициированной функцией InitiateDelete)
        Параметры:
            c_szwIteratorId             STRING_T - Идентификатор операции удаления, возвращенный ф-й InitiateDelete
            c_szwDeletionProgress       INT_T - число от 0 до 100
    */
	const wchar_t c_DeletionProgress[]=L"KLEVP_DeletionProgress";

	// events virtual parameters (not published by product, but selected from db):
	const wchar_t c_EventHostStatus[] = L"KLEVP_EVENT_HOST_STATUS";		//  INT_T    
	const wchar_t c_EventHostStatusEx[] = L"KLEVP_EVENT_HOST_STATUS_EX";	//  INT_T    
	const wchar_t c_EventTaskName[] = L"KLEVP_EVENT_TASK_NAME";	//  STRING_T    
	const wchar_t c_IsRemoteInstall[] = L"KLEVP_EVENT_IS_RI";	//  INT_T    
	const wchar_t c_NeedReboot[] = L"KLEVP_EVENT_NEED_REBOOT";	//  INT_T    

    enum EventPropertiesType { eptAll, eptGroupTask, eptHost };

    /*!
    * \brief Интерфейс реализует функциональность по просмотру и удалению описания событий.
	*		
    */
    class KLSTD_NOVTABLE EventProperties : public KLSTD::KLBase {
    public:
        /*!
          \brief	GetRecordCount

          \param	long возвращает количество записей в списке
        */
        virtual long GetRecordCount() = 0;

		/*
			\brief Возвращает заданный интервал записей.

            \param	long nStart [in] - порядковый номер начальной записи
            \param	long nEnd [in] - порядковый номер конечной записи
            \param paramProperties [out] - результат, содержит переменную c именем c_grp_array_hosts типа ARRAY_T.
		*/
        virtual void GetRecordRange(
            long nStart,
            long nEnd,
            KLPAR::Params** pParamsEvents,
            RangeCalcProgressCallback fnProgress = NULL,
            void * pCallbackParam = NULL)  = 0;

        /*!
          \brief	FindRecordByFirstChars - ищет запись, в которой заданное поле
                начинается с заданных символов.

          \param	wstrFieldToSearch [in] - поле, по которому производится поиск.
          \param	wstrFirstChars [in] - символы, которые ищутся.
          \return	long - порядковый номер первой записи, у которой
            поле wstrFieldToSearch начинается с wstrFirstChars. Если запись не
            найдена, то возращается значение < 0.
        */
        virtual long FindRecordByFirstChars( const std::wstring & wstrFieldToSearch, 
                                             const std::wstring & wstrFirstChars )  = 0;

		/*! 
		  \brief удаляет события, ID которых указаны в переданном параметре.
			\param vectIDs [in] - ID тех событий, которые надо удалить.
		*/
		virtual void DeleteProperties( const std::vector<long>& vectIDs  ) = 0;
		/*! 
		  \brief инициирует массовое удаление событий
			\param pSettings [in]
                c_szwMaxSelectedRecNumber // INT_T
                c_szwBlocks // ARRAY_T
                    +---<array element> // PARAMS_T
                        +---c_szwBlockBegin // INT_T, id записи с началом блока
                        +---c_szwBlockEnd // INT_T
                    +---<array element> // PARAMS_T
                        ...
                    ...
                    
            \return std::wstring - luid, который сервер пишет в тело событий, публикуемых
                в процессе удаления событий. Нужен только для того, чтобы отличить "свои" события
		*/
        virtual std::wstring InitiateDelete( KLSTD::CAutoPtr<KLPAR::Params> pSettings ) = 0;

        virtual void CancelDelete( KLSTD::CAutoPtr<KLPAR::Params> pSettings ) = 0;

		/*! 
		  \brief возвращает идентификатор итератора, используемого при удалении событий, 
		  (см. комментарий к функции InitiateDelete). В общем случае валиден только после вызова InitiateDelete.
		*/
        virtual std::wstring GetIteratorId( KLSTD::CAutoPtr<KLPAR::Params> pSettings ) = 0;
    };
}

/* ! KLEVP_NF_TASK_ID - имя переменной в EventInfo::body,
которая заполняется в событии от групповой задачи. Переменная должна быть 
типа STRING_T и содержать ID задачи. Если значение 
отсутствует, то это обычное событие. */
#define KLEVP_EVENT_GROUP_TASK_ID                    L"KLEVP_EVENT_GROUP_TASK_ID"

/* ! KLEVP_EVENT_SEVERITY - имя опциональной переменной в EventInfo::body,
Переменная должна содержать значения KLEVP_EVENT_SEVERITY_* - "серьезность" события */
#define KLEVP_EVENT_SEVERITY         KLEVP::c_er_severity  // INT_T

// ! brief имя переменной в EventInfo::body, которая заполняется в событии от 
// групповой задачи и в которой содержиться описание события
#define KLEVP_EVENT_DESCR           KLEVP::c_er_descr  // STRING_T

// ! brief имя переменной в EventInfo::body, которая заполняется в событии от 
// групповой задачи и в которой содержиться имя задачи
#define KLEVP_EVENT_TASK_DISPLAY_NAME    KLPRTS::TASK_DISPLAY_NAME //L"KLEVP_EVENT_TASK_DISPLAY_NAME" // STRING_T

// ! brief имя переменной в EventInfo::body, которая содержит user-friendly имя типа события.
// Например, еслди тип события 'klevp-task-on-host-finished', то это поле может
// содержать 'Задача завершена'
#define KLEVP_EVENT_TYPE_DISPLAY_NAME       L"KLEVP_EVENT_TYPE_DISPLAY_NAME" // STRING_T

#define KLEVP_EVENT_HOST            L"KLEVP_EVENT_HOST"     // STRING_T
#define KLEVP_EVENT_DOMAIN          L"KLEVP_EVENT_DOMAIN"   // STRING_T
#define KLEVP_EVENT_PRODUCT         L"KLEVP_EVENT_PRODUCT"     // STRING_T
#define KLEVP_EVENT_VERSION         L"KLEVP_EVENT_VERSION"   // STRING_T
#define KLEVP_EVENT_HOST_DISPNAME   L"KLEVP_EVENT_HOST_DISPNAME"	// STRING_T
#define KLEVP_EVENT_TASK_NAME       L"KLEVP_EVENT_TASK_NAME"   // STRING_T
#define KLEVP_EVENT_HOST_NETBIOSNAME	L"KLEVP_EVENT_HOST_NETBIOSNAME"	// STRING_T

#define	KLEVP_EVENT_TYPE				L"KLEVP_EVENT_TYPE"					// STRING_T
#define	KLEVP_EVENT_GNRL_TYPE			L"KLEVP_EVENT_GNRL_TYPE"			// STRING_T
#define	KLEVP_EVENT_GNRL_TYPES_ARRAY	L"KLEVP_EVENT_GNRL_TYPES_ARRAY"		// ARRAY_T (STRING_T)
#define	KLEVP_EVENT_TSK_STATE_TYPE		L"KLEVP_EVENT_TSK_STATE_TYPE"		// STRING_T
#define KLEVP_EVENT_RISE_TIME_LEAST		L"KLEVP_EVENT_RISE_TIME_LEAST"		// DATE_TIME_T
#define KLEVP_EVENT_RISE_TIME_GREATEST	L"KLEVP_EVENT_RISE_TIME_GREATEST"	// DATE_TIME_T
#define KLEVP_EVENT_RISE_TIME_LAST_DAYS	L"KLEVP_EVENT_RISE_TIME_LAST_DAYS"	// INT_T
#define KLEVP_EVENT_HOST_GROUP			L"KLEVP_EVENT_HOST_GROUP"			// STRING_T
#define	KLEVP_EVENT_TASK_STATE			KLEVP::c_er_task_new_state			// INT_T

#define KLEVP_EVENT_HOST_IP_FROM		L"KLEVP_EVENT_HOST_IP_FROM"	// INT_T if 0 - ignore
#define KLEVP_EVENT_HOST_IP_TO			L"KLEVP_EVENT_HOST_TO"		// INT_T if 0 - ignore

#define KLEVP_EVENT_RI_NEED_REBOOT   L"KLEVP_EVENT_RI_NEED_REBOOT"	// BOOL_T
#define KLEVP_EVENT_RI_ERR_REASONS   L"KLEVP_EVENT_RI_ERR_REASONS"	// ARRAY_T (INT_T)

#endif // _KLEVENTROPS_H__5745B60A_831F_4678_816D_9265D1AD094D__

// Local Variables:
// mode: C++
// End:

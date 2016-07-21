/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	GSYN/GroupSync.h
 * \author	Andrew Kazachkov
 * \date	29.11.2002 10:53:33
 * \brief	Модуль, отвечающий за групповую синхронизацию файла SettingsStorage,
 *        сохраняемого на сервере администрирования, и группой узлов.
 * 
 */

#ifndef KLGSYN_GROUPSYNC_H
#define KLGSYN_GROUPSYNC_H

#include <vector>
#include <string>

#include <std/base/klbase.h>
#include <srvp/grp/groups.h>
#include <srvp/csp/klcsp.h>

namespace KLGSYN {

	//! Состояния заданной синхронизации
	enum GSYN_STATUS{
		//! Синхронизация ещё не началась
		GSYNS_SCHEDULED,
		//! Синхронизация ещё не закончилась
		GSYNS_RUNNING,
		//! Синхронизация завершена т.к. истекло время ожидания
		GSYNS_ELAPSED,
		//! Синхронизация завершилась успешно на всех хостах
		GSYNS_FINISHED,
		//! Синхронизация не прошла из-за ошибки
		GSYNS_FAILED
	};

	//! Состояния синхронизации на заданном хосте
	enum GSYN_HOST_STATUS{
		//! Синхронизация ещё не началась
		GSYNH_SCHEDULED,
		//! Синхронизация ещё не закончилась
		GSYNH_RUNNING,
		//! Синхронизация завершилась успешно
		GSYNH_FINISHED,
		//! Синхронизация не прошла из-за ошибки
		GSYNH_FAILED
	};
	
	//! Направления синхронизации
	enum GSYN_DIRECTION{
		GSYND_ONEWAY,
		GSYND_TWOWAY
	};

	//! Структура, содержащая идентификатор удаленного компьютера.
    typedef KLCSP::host_id_t host_id_t;

	/*! Структура, содержащая текущий статус синхронизации для удаленного
		компьютера.
	*/
	typedef struct {
		//! Идентификатор хоста
		host_id_t			idHost;
		//! Текущий статус синхронизации.
		GSYN_HOST_STATUS	status;
		//! Идентификатор события о последнем статусе синхронизации.
		std::wstring		eventId;
		//! Описание события о последнем статусе синхронизации.
		std::wstring		eventDesc;
		//! Последняя синхронизированная версия.
		long				lastSyncVersion;
	} sync_host_status_t;

	
	//! Идентификатор синхронизации
	const wchar_t c_gsyn_id[]=L"gsynId";				//INT_T
	//! Относительный путь к файлу SettingsStorage для групповой синхронизации.
	const wchar_t c_gsyn_path[]=L"filePath";			//STRING_T
	//! Направление синхронизации (GSYN_DIRECTION)
	const wchar_t c_gsyn_direction[]=L"gsynDirection";	//INT_T
	//! Состояние синхронизации (GSYN_STATUS)
	const wchar_t c_gsyn_status[]=L"gsynStatus";		//INT_T
	//! Время создания синхронизации
	const wchar_t c_gsyn_creation[]=L"creationTime";	//DATE_TIME_T
	//! Время конца ожидания завершения синхронизации
	const wchar_t c_gsyn_elapse[]=L"elapseTime";		//DATE_TIME_T
	//! Имя синхронизации
	const wchar_t c_gsyn_name[]=L"name";				//STRING_T
	//! Результат синхронизации
	const wchar_t c_gsyn_result[]=L"result";			//STRING_T
	//!
	const wchar_t c_gsyn_cutoff_children[]=L"cutoff_children";//BOOL_T

	//! Массив хостов, участвующих в синхронизации
	const wchar_t c_gsyn_hosts[]=KLCSP_ITERATOR_ARRAY;			//ARRAY_T
	//! Идентификатор группы
	const wchar_t c_gsyn_groupId[]=L"groupId";			//INT_T

	//! Имя хоста
	const wchar_t c_gsyn_hostName[]=L"hostName";		//STRING_T
    //! NetBIOS-имя хоста
    const wchar_t c_gsyn_winhostName[]=L"winhostName";		//STRING_T
    //! Отображаемое имя хоста
    const wchar_t c_gsyn_hostDisplayName[]=L"hostDisplayName";
	//! Имя домена
	const wchar_t c_gsyn_domainName[]=L"domainName";	//STRING_T
	//! Состояние синхронизации (GSYN_HOST_STATUS)
	const wchar_t c_gsyn_hostStatus[]=L"hostStatus";	//INT_T
	//! Идентификатор события о последнем статусе синхронизации.
	const wchar_t c_gsyn_eventName[]=L"eventName";		//STRING_T
	//! Описание события о последнем статусе синхронизации.
	const wchar_t c_gsyn_eventDesc[]=L"eventDesc";		//STRING_T
	//! Последняя синхронизированная версия.
	const wchar_t c_gsyn_version[]=L"version";			//LONG_T	
    //! Время последней синхронизации
    const wchar_t c_gsyn_lastsync[]=L"last_sync";			//DATE_TIME_T	    
    
    //! Time when host required sync for the last time ( since KLADMSRV_SV70 )
    const wchar_t c_gsyn_last_required[]=L"last_sync_req";			//DATE_TIME_T	   
    //! Код ошибки
    const wchar_t c_gsyn_errorcode[]=L"error_code";//INT_T
    
//! Счётчики хостов, на которых проходит синхронизация
    //! Количество хостов подпадающих под синхронизацию
    const wchar_t c_gsyn_cnt_FullCount[]=L"gsyn_cnt_FullCount";    
    //! Количество хостов, нуждающихся в синхронизации (т.е. синхронизация на них ещё не прошла или прошла, но не удалась)
    const wchar_t c_gsyn_cnt_HostsToSyncronize[]=L"gsyn_cnt_HostsToSyncronize";
    //! Количество хостов в состоянии GSYNH_SCHEDULED
    const wchar_t c_gsyn_cnt_Scheduled[]=L"gsyn_cnt_Scheduled";
    //! Количество хостов в состоянии GSYNH_RUNNING
    const wchar_t c_gsyn_cnt_Running[]=L"gsyn_cnt_Running";
    //! Количество хостов в состоянии GSYNH_FINISHED
    const wchar_t c_gsyn_cnt_Finished[]=L"gsyn_cnt_Finished";
    //! Количество хостов в состоянии GSYNH_FAILED
    const wchar_t c_gsyn_cnt_Failed[]=L"gsyn_cnt_Failed";

/*!
	\brief Интерфейс реализует функциональность групповой синхронизации между
	файлами SettingsStorage,  сохраняемыми на Сервере администрирования и
	группой удаленных узлов.
	Модуль позволяет определить файл SettingsStorage, требующий синхронизации
	для набора компьютеров.  Далее модуль позволяет просматривать текущий
	статус синхронизации между локальным файлом и удаленными узлами, добавлять
	и удалять узлы к списку синхронизации.
	Синхронизация может проводиться как для логических групп сервера
	администрирования (для всех компьютеров, включенных в группу), так и для
	отдельного списка компьютеров. Одному и тому же файлу может соответствовать
	несколько списков синхронизации.
	Синхронизируемые файлы имеют версии.  Синхронизация начинается сразу после
	изменения версии файла. Существует возможность узнать текущую версию
	синхронизированного файла на удаленном узле (версию последней успешной
	синхронизации).
*/


    typedef KLCSP::ForwardIterator ForwardIterator;

	typedef KLCSP::ChunkAccessor ChunkAccessor;
		
    class GroupSync : public KLSTD::KLBaseQI
	{
    public:
    /*!
      \brief Метод запускает для заданного файла SettingsStorage процедуру
             групповой синхронизации.

        \param wstrSyncObjPath [in]	Относительный путь к локальному файлу
								SettingsStorage для групповой синхронизации.
								Уникальный идентификатор. 
		\param nDirection [in]	Направление синхронизации
        \param groupId [in]	Идентификатор группы, для которой требуется
							синхронизация.
		\param lElapse [in]	Максимальный срок в секундах, по истечении
							которого синхронизация считается завершённой, даже
							если она прошла не на всех хостах.

        \return long Идентификатор групповой синхронизации.
						Уникальный идентификатор.
       
    */
		virtual long AddSyncObjG(
					const wchar_t*		szwName,
					const wchar_t*		szwSyncObjPath,
					GSYN_DIRECTION		nDirection,
					long				lGroup,
					long				lElapse,
					bool				bCutoffChildren=false,
                    bool                bOwnTransaction=true) = 0;

    /*!
      \brief Метод запускает для заданного файла SettingsStorage процедуру
             групповой синхронизации.

        \param wstrSyncObjPath [in]	Относительный путь к локальному файлу
						SettingsStorage для групповой синхронизации. Уникальный
						идентификатор.
		\param nDirection [in]	Направление синхронизации
        \param pHosts [in]	Массив идентификаторов хостов для которых 
							требуется синхронизация.
		\param nHosts [in]	Длина массива идентификаторов хостов
		\param lElapse [in]	Максимальный срок в секундах, по истечении
							которого синхронизация считается завершённой, даже
							если она прошла не на всех хостах.

        \return long Идентификатор групповой синхронизации. 
					Уникальный идентификатор.
       
    */

		virtual long AddSyncObjH(
					const wchar_t*		szwName,
					const wchar_t*		szwSyncObjPath,
					GSYN_DIRECTION		nDirection,
					host_id_t*			pHosts,
					long				nHosts,
					long				lElapse,
                    bool                bOwnTransaction=true) = 0;

	/*!
		\brief	Возвращает информацию о синхронизации.

		\param	lSync [in] идентификатор синхронизации.
		\param	pFields [in] Массив имён требуемых атрибутов. Могут быть
				указаны следующие атрибуты:
					c_gsyn_path,
					c_gsyn_direction,
					c_gsyn_status,
					c_gsyn_creation,
					c_gsyn_elapse,
					c_gsyn_name,
					c_gsyn_result,
					c_gsyn_groupId,// для синхронизаций, созданных AddSyncObjG
					c_gsyn_hosts // для синхронизаций, созданных AddSyncObjH
				прочие атрибуты игнорируются.
		\param	nFields [in] количество элементов в массиве pFields
		\param	ppInfo [out] указатель на переменную, в которую будет
				записан указатель на контейнер Params, содержащий заданные
				атрибуты.
		\except	STDE_NOTFOUND синхронизация с заданным идентификатором не
				найдена.
	*/
		virtual void GetSyncInfo(
					long			lSync,
					const wchar_t**	pFields,
					int				nFields,
					KLPAR::Params**	ppInfo)=0;

		/*!
		\brief	Удаляет объект сихронизации
		\param	lSync -- идентификатор синхронизации.
        \param  bOwnTransaction - если true, будет создана новая транзакция,
                иначе операция будет производится в рамках текущей транзакции.
		\except	STDE_NOTFOUND синхронизация с заданным идентификатором не
			найдена.
		*/
		virtual void RemoveSyncObj(long	lSync, bool bOwnTransaction=true) = 0;

		/*!
			\brief	Возвращает информацию о всех хостах, участвующих в
					синхронизации.
			\param	lSync [in] идентификатор синхронизации.
			\param	pFields [in] Массив имён требуемых атрибутов. Могут быть
					указаны следующие атрибуты:
						c_gsyn_hostName,
						c_gsyn_domainName,
						c_gsyn_hostStatus,
						c_gsyn_eventName,
						c_gsyn_eventDesc,
						c_gsyn_version,
                        c_gsyn_lastsync,
                        c_gsyn_last_required.
					прочие атрибуты игнорируются.
			\param	nFields [in] количество элементов в массиве pFields
			\param	lTimeout [in] время жизни итератора,с
			\param	ppIterator [out] указатель на переменную, в которую будет
					записан указатель на интерфейс ForwardIterator.

			\except	STDE_NOTFOUND синхронизация с заданным идентификатором не
				найдена.
		*/
        virtual void GetSyncHostsInfo(
					long				lSync, 
					const wchar_t**		pFields,
					long				nFields,
					long				lTimeout,
					ForwardIterator**	ppIterator)  = 0;

		/*!
			\brief	Ищет хосты соответствующие заданному фильтру.
			\param	lSync [in] идентификатор синхронизации.
			\param	szwFilter [in] фильтр [примерно как в RFC2254]. В фильтре
					могут быть упомянуты следующие атрибуты:
							c_gsyn_hostName,
							c_gsyn_domainName,
							c_gsyn_hostStatus,
							c_gsyn_eventName,
							c_gsyn_eventDesc,
							c_gsyn_version,
                            c_gsyn_lastsync,
                            c_gsyn_last_required.
			\param	pFields [in] Массив имён требуемых атрибутов. Могут быть
					указаны следующие атрибуты:
							c_gsyn_hostName,
							c_gsyn_domainName,
							c_gsyn_hostStatus,
							c_gsyn_eventName,
							c_gsyn_eventDesc,
							c_gsyn_version,
                            c_gsyn_lastsync,
                            c_gsyn_last_required
					прочие атрибуты игнорируются.
			\param	nFields [in] количество элементов в массиве pFields
			\param	lTimeout [in] время жизни итератора, с
			\param	ppIterator [out] указатель на переменную, в которую будет
					записан указатель на интерфейс ForwardIterator.

			\except	STDE_NOTFOUND синхронизация с заданным идентификатором не
				найдена.
		*/
		virtual void FindHosts(
					long				lSync, 
					const wchar_t*		szwFilter,
					const wchar_t**		pFields,
					long				nFields,
					long				lTimeout,
					ForwardIterator**	ppIterator)  = 0;

        virtual AVP_qword IncrementVersion(
                            long            lSync,
                            const wchar_t*  szwSyncObjPath,
                            long            lWaitTimeout,
                            bool            bOwnTransaction=true) = 0;

        virtual void ForceGroupSync(
                            long    idSync,
                            bool    bOwnTransaction=true) = 0;

		/*!
			\brief	Возвращает информацию о всех хостах, участвующих в
					синхронизации.
			\param	lSync [in] идентификатор синхронизации.
			\param	vect_fields [in] вектор имён требуемых атрибутов. Могут быть
					указаны следующие атрибуты:
						c_gsyn_hostName,
						c_gsyn_domainName,
						c_gsyn_hostStatus,
						c_gsyn_eventName,
						c_gsyn_eventDesc,
						c_gsyn_version.
					прочие атрибуты игнорируются.
			\param	vect_fields_to_order [in] вектор полей для сортировки.
			\param	lTimeout [in] время жизни итератора,с
			\param	ppAccessor [out] указатель на переменную, в которую будет
					записан указатель на интерфейс ChunkAccessor.

			\return long - Число найденных записей.

			\except	STDE_NOTFOUND синхронизация с заданным идентификатором не
				найдена.
		*/
        virtual long GetSyncHostsInfo2(
					long				lSync, 
					const std::vector<std::wstring>& vect_fields,
					const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
					long				lTimeout,
					ChunkAccessor**		ppAccessor)  = 0;

		/*!
			\brief	Ищет хосты соответствующие заданному фильтру.
			\param	lSync [in] идентификатор синхронизации.
			\param	szwFilter [in] фильтр [примерно как в RFC2254]. В фильтре
					могут быть упомянуты следующие атрибуты:
							c_gsyn_hostName,
							c_gsyn_domainName,
							c_gsyn_hostStatus,
							c_gsyn_eventName,
							c_gsyn_eventDesc,
							c_gsyn_version.
			\param	vect_fields [in] вектор имён требуемых атрибутов. Могут быть
					указаны следующие атрибуты:
						c_gsyn_hostName,
                        c_gsyn_hostDisplayName,
						c_gsyn_domainName,
						c_gsyn_hostStatus,
						c_gsyn_eventName,
						c_gsyn_eventDesc,
						c_gsyn_version.
					прочие атрибуты игнорируются.
			\param	vect_fields_to_order [in] вектор полей для сортировки.
			\param	lTimeout [in] время жизни итератора, с
			\param	ppAccessor [out] указатель на переменную, в которую будет
					записан указатель на интерфейс ChunkAccessor.

			\return long - Число найденных записей.

			\except	STDE_NOTFOUND синхронизация с заданным идентификатором не
				найдена.
		*/
		virtual long FindHosts2(
					long				lSync, 
					const wchar_t*		szwFilter,
					const std::vector<std::wstring>& vect_fields,
					const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
					long				lTimeout,
					ChunkAccessor**		ppAccessor)  = 0;


		virtual long AddSyncObjG2(
                    long				lGroup,
					const wchar_t*		szwSyncObjLocation,
                    const wchar_t*		szwProductName,
                    const wchar_t*		szwProductVersion,                    
                    const wchar_t*		szwGsyncName = NULL,
                    long				lElapse = 0,
                    bool                bOwnTransaction=true,
                    bool				bCutoffChildren=false,                    
					GSYN_DIRECTION		nDirection = GSYND_ONEWAY) = 0;

		virtual long AddSyncObjH2(
					host_id_t*			pHosts,
					size_t              nHosts,
                    const wchar_t*		szwSyncObjLocation,
                    const wchar_t*		szwProductName,
                    const wchar_t*		szwProductVersion,
                    const wchar_t*		szwGsyncName = NULL,
                    long				lElapse = 0,                    
                    bool                bOwnTransaction=true,                    
					GSYN_DIRECTION		nDirection = GSYND_ONEWAY) = 0;
        

        /*!
          \brief	Allows to change hosts list for global syn-tion.

          \param	lGsynId [in]
          \param	pHosts [in]
          \param	nHosts [in]
          \param	bOwnTransaction [in]
        */
        virtual void UpdateHostsSyncObjH(
                    long                lGsynId,
                    host_id_t*          pHosts,
                    size_t              nHosts,
                    bool                bOwnTransaction=true) = 0;
    };
};

/*!
  \brief Создает объект класса KLGSYN::GroupSync

    \param ppGS       [out] Указатель на переменную, в которую будет записан
						указатель на интерфейс KLGSYN::GroupSync

*/

KLCSSRVP_DECL void KLGSYN_CreateGroupSync(
                        const wchar_t*          szwAddress,
                        KLGSYN::GroupSync**     pp,
                        const unsigned short*   pPorts = NULL,
                        bool                    bUseSsl = true);

void KLGSYN_CreateServerGroupSync(KLGSYN::GroupSync**     pp);


#endif // KLGSYN_GROUPSYNC_H

// Local Variables:
// mode: C++
// End:

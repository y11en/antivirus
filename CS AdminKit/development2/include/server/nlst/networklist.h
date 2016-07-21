/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file networklist.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Интерфейс класса для работы с обобщенными ( сетевыми ) списками
 */


#if !defined(KLNLST_NETWORKLIST)
#define KLNLST_NETWORKLIST

#include <string>
#include <list>

#include <std/base/kldefs.h>
#include <std/base/klbase.h>
#include <std/tmstg/timeoutstore.h>

#include <kca/prci/componentid.h>
#include <kca/contie/connectiontiedobjectshelper.h>
#include <kca/prss/settingsstorage.h>

#include <server/nlst/common.h>

#include <srvp/evp/transeventstypes.h>
#include <server/evp/transeventsource.h>

#include <server/db/dbconnection.h>
#include <server/srvinst/srvinstdata.h>

#include <server/nlst/listitemiterator.h>

namespace KLNLST {
	
	/*!
     \brief Интерфейс для работы с пользовательскими списками
    */

	class CustomNetworkList : public KLSTD::KLBaseQI
	{
	public:
		/*!
			\brief Вызывается при изменении\добавлении элемента списка 

			\param szwListName		[in]	имя сетевого списка.
			\param nHostId			[in]	идентификатор хоста с которого пришел данный элемент
			\param szwListItemId	[in]	идентификатор элемента списка 
			\param pItemParams		[in]	параметры элемента
		*/
		virtual void UpdateListItem( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &szwListName,
			const long nHostId,
			const std::wstring &szwListItemId, 			
			KLPAR::Params *pItemParams ) = 0;

		/*!
			\brief Вызывается при удалении элемента списка

			\param szwListName		[in]	имя сетевого списка.
			\param nHostId			[in]	идентификатор хоста с которого пришел данный элемент
			\param szwListItemId	[in]	идентификатор элемента списка 
		*/
		virtual void DeleteListItem( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &szwListName,
			const long nHostId,
			const std::wstring &szwListItemId ) = 0;

		/*!
			\brief Удаляет все элемента из списка
		*/
		virtual void DeleteAllListItems( KLDB::DbConnectionPtr pDbConn,
			const std::wstring &szwListName,
			const long nHostId ) = 0;
	};

	class NetworkList : public KLSTD::KLBaseQI
	{
	public:
		
		/*
			\brief Функция инициализации
		*/
		virtual void Initialize( KLSRV::SrvData* pSrvData, 
			KLSTD::CAutoPtr<KLEVP::TransEventSource> pTransEventSource ) = 0;

		virtual void Deinitialize() = 0;

		/*
			\brief Функция регистрации нового списка

			\param listName [in] Имя регестрируемого списка
			\param listComponentId [in] Идентификатор компоненты, контролирующей список
			\param listControlComponentTaskName [in] Имя задачи на указанной компоненте, 
					которая предназначена для получения информации об элементах 
					списка на конечном хосте

			\exception NLSTERR_WRONG_LIST_NAME
		*/
		virtual void RegisterList( const std::wstring& listName, 
			const KLPRCI::ComponentId &listComponentId,
			const std::wstring &listControlComponentTaskName ) = 0;

		/*
			\brief Функция возвращает итератор элементов списка
		*/
		virtual void GetListItemsIterator( KLNLST::ListItemIterator **ppIterator ) = 0;

		/*
			\brief Функция обнавляет полный список элементов хоста

			\exception NLSTERR_WRONG_LIST_NAME, NLSTERR_WRONG_ARGS
		*/
		virtual void UpdateHostNetworkList( const std::wstring &listName,
			const std::wstring &listComponentName,
			const std::wstring &remoteConnName,
			const NetworkListItemsList &itemsList ) = 0;

		/*
			\brief Функция добавляет задачу для элемента списка

			\exception NLSTERR_WRONG_LIST_NAME, NLSTERR_WRONG_ARGS
		*/
		virtual void AddListItemTask( int itemId, const std::wstring &taskName,
			KLPAR::Params *pParams ) = 0;


		/*
			\brief Функция добавляет задачу для элементов списка

			\exception NLSTERR_WRONG_LIST_NAME, NLSTERR_WRONG_ARGS
		*/
		virtual void AddListItemsTask( std::vector<long> itemsIds, const std::wstring &taskName,
			KLPAR::Params *pParams ) = 0;
		

		/*
			\brief Функция оповещения об измененнии политики на сервере администрирования			
		*/
		virtual void PolicyChangedNotify( 
			const KLPRSS::product_version_t&	productVersion,
			const std::wstring&					sectionName,
			const KLPAR::Params*				sectionParams,
			KLDB::DbConnectionPtr				pDbConn,
			const int							groupSyncId ) = 0;
		
		/*
			\brief Функция оповещения об активации\дезактивации политики
		*/
		virtual void PolicyActivatedNotify( 
			const KLPRSS::product_version_t&	productVersion,
			bool								activatedFlag,
			const std::wstring&					settingsStoragePath,
			KLDB::DbConnectionPtr				pDbConn,
			const int							groupSyncId ) = 0;


		/*!
			\brief Регестрирует пользовательский список
		 */
		virtual void RegisterCustomNetworkList( const std::wstring &listName,  
			CustomNetworkList *pCustomList ) = 0;

		/*!
			\brief Разрегестрирует пользовательский список
		 */
		virtual void UnregisterCustomNetworkList( const std::wstring &listName ) = 0;
		
		/*
			\brief Функция возвращает информацию об элементе списка

			\exception NLSTERR_WRONG_ARGS
		*/
		virtual void GetItemInfo( int itemId, std::wstring &hostName,
			std::wstring &listName, std::wstring &itemIdName ) = 0;
	};

} // end namespace KLNLST

KLNLST::NetworkList *KLNLST_GetNetworkList();

void KLNLST_GetNetworkList( KLNLST::NetworkList **ppNetworkList );

void KLNLST_CreateNetworkList( KLNLST::NetworkList **ppNetList );

/*
	\brief Функции инициализаци/деинициализации библиотеки
*/
DECLARE_MODULE_INIT_DEINIT2( KLCSSRV_DECL, KLNLST );

#endif // !defined(KLNLST_NETWORKLIST)
/*!
* (C) 2007 "Kaspersky Lab"
*
* \file trafficmanagerserver.h
* \author Шияфетдинов Дамир
* \date 2007
* \brief Интерфейс класса для ограничения траффика
*
*/

#ifndef FTS_TRAFFICMANAGERSERVER_H
#define FTS_TRAFFICMANAGERSERVER_H

#include <std/base/klbase.h>
#include <std/err/klerrors.h>

#include <transport/tr/trafficmanager.h>

#include <kca/contie/connectiontiedobjectshelper.h>

#include <server/db/dbconnection.h>
#include <server/srvinst/srvinstdata.h>

namespace KLTRFS {

	class KLSTD_NOVTABLE TrafficManagerServer : public KLSTD::KLBase 
	{
	public:

		/*!
			\brief Инициализация подсистемы 
		*/
		virtual void InitServer( KLSRV::SrvData* pSrvData ) = 0;

		/*!
			\brief Прекрашает работу подсистемы
		*/
		virtual void Shutdown() = 0;

		/*!
		  \brief Добавляет ограничение траффика для указанного интервала IP адресов
		*/
		virtual long ReplaceRestriction( const KLTR::TrafficManager::Restriction &restriction ) = 0;

		/*!
		  \brief Удаляет указанного ограничение
		*/
		virtual void DeleteRestriction( long nRestrictionId ) = 0;

		/*!
		  \brief Возвращает все ограничения траффика 
		*/
		virtual void GetRestrictions( KLTR::TrafficManager::RestrictionsMap &restrictList ) = 0;
	};

} // end namespace KLTRFS

/*
	\brief Функции инициализаци/деинициализации библиотеки передачи файлов
*/
DECLARE_MODULE_INIT_DEINIT2( KLCSSRV_DECL, KLTRFS );


/*\brief Возвращает указатель на глобальный объект класса TrafficManagerServer */
KLCSSRV_DECL KLTRFS::TrafficManagerServer *KLTRFS_GetTrafficManagerServer();

#endif // FTS_TRAFFICMANAGERSERVER_H
/*!
* (C) 2007 "Kaspersky Lab"
*
* \file securitypolicyserver.h
* \author Шияфетдинов Дамир
* \date 2007
* \brief Интерфейс подсистемы подержки прав доступа на уровне БД сервера администрирования
*
*/

#ifndef SPLS_SECURITYPOLICYSERVER_H
#define SPLS_SECURITYPOLICYSERVER_H

#include <std/base/klbase.h>
#include <std/err/klerrors.h>

#include <transport/wat/common.h>

#include <server/db/dbconnection.h>
#include <server/srvinst/srvinstdata.h>

namespace KLSPLS {

	class KLSTD_NOVTABLE SecurityPolicyServer : public KLSTD::KLBase 
	{
	public:

		/*!
			\brief Инициализация подсистемы 
		*/
		virtual void Init( KLSRV::SrvData* pSrvData ) = 0;

		/*!
			\brief Прекрашает работу подсистемы
		*/
		virtual void Shutdown() = 0;

		/*!
			\brief функция нотификации об изменении иерархии групп администрирования
		*/
		virtual void NotifyGroupHierarchyChanged() = 0;

		/*!
			\breif Функция заполнения таблицы прав доступа для текущего пользователя

			\param pClientContext [in,out] - контекст клиента. на основе контекста клиента будет 
				заполнена таблица прав доступа и добавлены в контекст идентификатор пользователя в БД
				и его системный GUID
		*/
		virtual void PrepareClientContext( int transportConnId, 
			KLSTD::CAutoPtr<KLWAT::ClientContext> pClientContext ) = 0;

		/*!
			\breif Функция оповещении о закрытии соединения от пользователя
		*/
		virtual void NotifyClientContextDestroy( KLSTD::CAutoPtr<KLWAT::ClientContext> pClientContext ) = 0;


	};

} // end namespace KLSPLS

/*
	\brief Функции инициализаци/деинициализации
*/
DECLARE_MODULE_INIT_DEINIT2( KLCSSRV_DECL, KLSPLS );

/*\brief Возвращает указатель на глобальный объект класса SecurityPolicyServer */
KLCSSRV_DECL KLSPLS::SecurityPolicyServer *KLSPLS_GetSecurityPolicyServer();

#endif // SPLS_SECURITYPOLICYSERVER_H
/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file srvhrch/srvhrch_lst.h
 * \author Andrew Lashenkov
 * \date	24.12.2004 18:54:00
 * \brief Интерфейс списка подчиненных серверов.
 *
 */

#ifndef KLSRVH_SRVHRCH_LST_H
#define KLSRVH_SRVHRCH_LST_H

#include <srvp/srvhrch/srvhrch.h>
#include <std/base/klbase.h>
#include <std/par/params.h>

namespace KLSRVH {

	class KLSTD_NOVTABLE ChildServers : public KLSTD::KLBaseQI {
	public:
	/*!
	  \brief Возвращает список подчиненных серверов.

		\param nGroupId			[in] Группа, для которой запрашивается список подчиненных серверов; 
									GROUPID_INVALID_ID для поиска по всем группам.
		\param servers			[out] Список подчиненных серверов.
	*/
		virtual void GetChildServers(
			long nGroupId,
			std::vector<child_server_info_t>& vecServers) = 0;

	/*!
	  \brief Возвращает информацию о подчиненном сервере по его идентификатору.

		\param nId		[in] Идентификатор сервера.
		\param fields	[in] Список полей, которые требуется получить.
		\param size		[in] Число полей в списке fields.
		\param info		[out] Информация о сервере.
		
		  \exception STDE_NOTFOUND указанный сервер не найден
   
	*/
		virtual void GetServerInfo(
			long nId,
			const wchar_t**	fields,
			int size,
			KLPAR::Params** info) = 0;

	/*!
	  \brief Возвращает информацию о подчиненном сервере по wstrInstanceId.

		\param wstrInstanceId	[in] wstrInstanceId.
		\param fields			[in] Список полей, которые требуется получить.
		\param size				[in] Число полей в списке fields.
		\param info				[out] Информация о компьютере.
		
		  \exception STDE_NOTFOUND указанный сервер не найден
   
	*/
		virtual void GetServerInfoByInstanceId(						
			const std::wstring& wstrInstanceId,
			const wchar_t**		fields,
			int					size,
			KLPAR::Params**     info) = 0;

	/*!
	  \brief Возвращает информацию о подчиненном сервере по MD5-hash сертификата.

		\param wstrPublicKeyHash	[in] MD5-hash публичного ключа сертификата.
		\param fields				[in] Список полей, которые требуется получить.
		\param size					[in] Число полей в списке fields.
		\param info					[out] Информация о компьютере.
		
		  \exception STDE_NOTFOUND указанный сервер не найден
   
	*/
		virtual void GetServerInfoByPublicKeyHash(			
			const std::wstring& wstrPublicKeyHash,
			const wchar_t**		fields,
			int					size,
			KLPAR::Params**     info) = 0;

	/*!
	  \brief Регистрирует новый подчиненный сервер для заданной группы.

		\param wstrDisplName	[in] Отображаемое имя сервера.
		\param wstrNetAddress	[in] Сетевой адрес сервера.
		\param nGroupId			[in] Идентификатор группы, в которую включен данный сервер.
		\param pAdditionalInfo	[in] Доп. информация о подчиненном сервере.

		  \return идентификатор добавленного хоста

		  \exception STDE_OBJ_EXISTS - Подчиненный сервер с таким именем уже зарегистрирован.
	*/
		virtual long RegisterServer(
			const std::wstring& wstrDisplName,
			long nGroupId,
			void* pCertificate,
			long nCertificateLen,
			const std::wstring& wstrNetAddress,
			KLSTD::CAutoPtr<KLPAR::Params> pAdditionalInfo) = 0;
		
	/*!
	  \brief Удаляет подчиненный сервер.

		\param nId [in] Идентификатор сервера.
   
	*/
		virtual void DelServer(long nId) = 0;

	/*!
	  \brief Обновляет информацию о подчиненном сервере.

		\param nId		[in] Идентификатор сервера.
		\param pInfo	[in] Информация о компьютере.
			
		  \exception STDE_NOTFOUND - Сервер не найден.
		  \exception STDE_OBJ_EXISTS - Подчиненный сервер с таким именем уже зарегистрирован.
   
	*/
		virtual void UpdateServer(
			long nId,
			KLSTD::CAutoPtr<KLPAR::Params> pInfo ) = 0;


//		/*!
//		  \brief    Форсирует синхронизацию указанного в параметре wstrSSType
//					хранилища. Вызов синхронный, т.е. метод не возвращает
//					управление до тех пор, пока не выполнится любое из
//					следующих условий: синхронизация не пройдёт до
//					конца или или не произойдёт ошибка.
//
//		  \param    nId [in] - Идентификатор подчиненного сервера.
//		  \param    wstrSSType [in] - идентификатор хранилища (пустая строка
//						означает, что необходмо провести синхронизацию для всех
//						хранилищ)
//		  \param    lWaitTimeout [in] - максимальное время ожидания в мс - число
//						большее нуля
//		  \param    bCancelWait [in] - запись в переменную, на которую указывает
//						данный указатель, значения true приводит к прерыванию
//						ожидания завершения и выбрасыванию исключения
//						STDE_CANCELED.
//
//		    \exception    KLSTD::STDE_CANCELED - ожидание было прервано
//		    \exception    KLSTD::STDE_TIMEOUT - истёк таймаут lWaitTimeout
//		    \exception    KLSRV::KLSRV_ERR_NO_HOST_ADDRESS - серверу неизвестен адрес хоста.
//		    \exception    KLSRV::KLSRV_ERR_HOST_NOT_RESPONDING - хост не отвечает
//		*/
//		virtual void ForceSynchronizationSync(
//						long					nId,
//						const std::wstring&     wstrSSType,
//						long                    lWaitTimeout,
//						volatile bool*          bCancelWait=NULL) = 0;
	};
};

namespace KLSRV{
	class ConnectionInfo;
};

KLCSSRVP_DECL void KLSRVH_CreateChildServersProxy(
	KLSTD::CAutoPtr<KLSRV::ConnectionInfo> pConnectionInfo, 
	KLSRVH::ChildServers** ppChildServersProxy);

#endif // KLSRVH_SRVHRCH_LST_H

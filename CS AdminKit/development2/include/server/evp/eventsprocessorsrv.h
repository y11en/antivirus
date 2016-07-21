/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file server/evp/eventsprocessorsrv.h
 * \author Andrew Lashchenkov
 * \date 21/07/2005
 * \brief Серверный интерфейс EventsProcesor
 *
 */

#ifndef KLEVP_EVENTSPROCESSORSRV_H
#define KLEVP_EVENTSPROCESSORSRV_H

#include <server/srv_common.h>
#include <srvp/evp/eventsprocessor.h>
#include <server/db/dbconnection.h>

#define KLEVP_EVENT_DB_ID  L"KLEVP_EVENT_DB_ID"

namespace KLSRV {
    struct EventAndCallerInfo : public KLEVP::EventInfo
    {
        EventAndCallerInfo()
        {
        }

        EventAndCallerInfo( const KLEVP::EventInfo& eventInfo )
        {
            *((KLEVP::EventInfo*)this) = eventInfo;
        }

        std::wstring deliveryId;
        std::wstring domain;
        std::wstring hostName;
        std::wstring agentId;
    };
};

namespace KLEVP {

    class EventsProcessorSrv : public KLSTD::KLBaseImpl<KLSTD::KLBase> {
    public:
    /*!
      \brief Метод принимает на обработку пакет событий от Агента.

		\param pDbConn		[in] Соединение с БД
        \param package      [in] Пакет событий для обработки.
        \param deliveryId   [in] Идентификатор пакета событий, доставленный 
                                 на Сервер Администрирования.
        \param domain       [in] Идентификатор домена узла, который доставляет событие.
        \param hostName     [in] Идентификатор хоста, который доставляет событие.
		\param nHostId		[in] Идентификатор хоста; может быть 0, тогда вычисляется по hostName
        \param agentId      [in] Идентификатор агента администрирования, который
                                 доставляет событие.
       
    */
        virtual void Process ( 
			KLDB::DbConnectionPtr pDbConn,
            const std::vector<EventInfo>& package,
            const std::wstring & deliveryId,
            const std::wstring & domain,
            const std::wstring & hostName,
			long nHostId,
            const std::wstring & agentId,
			std::vector<long>* pVecEventsDbIds) = 0;

		virtual void ProcessHostNFSection(
			KLDB::DbConnectionPtr pDbConn, 
			const std::wstring& wstrHostName, 
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			KLPAR::ParamsPtr parHostNFSection) = 0;
		
        // virtual KLSTD::CAutoPtr<KLSTD::CriticalSection> GetCriticalSection() = 0;
    };

    class EventsProcessorSrvFactory : public KLSTD::KLBase
    {
    public:
        virtual void CreateInstance( EventsProcessorSrv ** pEventsProcessor ) = 0;
    };

}

#endif // KLEVP_EVENTSPROCESSORSRV_H


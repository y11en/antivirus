/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file srvhrch/srvhrch_master.h
 * \author Andrew Lashenkov
 * \date	20.01.2005 10:16:00
 * \brief Интерфейс мастер-сервера иерархии серверов.
 *
 */

#ifndef KLSRVH_SRVHRCH_MASTER_H
#define KLSRVH_SRVHRCH_MASTER_H

#include <srvp/srvhrch/srvhrch.h>
#include <std/par/params.h>
#include <server/srvinst/connstore.h>

namespace KLSRVH {

	class SrvHierarchyMaster : public KLSTD::KLBaseQI {
	public:
		virtual ~SrvHierarchyMaster(){}
	/*!
	  \brief Устанавливает для подчиненного сервера его InstanceId по завершении хэндшейка.

		\param pParams	[in] В c_srv_instance_id содержит InstanceId подчененного сервера.
	*/
		virtual void InitialPingMaster(						
			KLSTD::CAutoPtr<KLPAR::Params> pInParams,
			KLSTD::CAutoPtr<KLPAR::Params>& pOutParams) = 0;

	/*!
	  \brief Ping.

		\param pParams	[in] Произвольные параметры пинга.
	*/
		virtual void PingMaster(						
			KLSTD::CAutoPtr<KLPAR::Params> pInParams,
			KLSTD::CAutoPtr<KLPAR::Params>& pOutParams) = 0;

	/*!
	  \brief Доставка результатов отчета с подчиненного сервера.

		\param wstrRequestId			[in] - идентификатор запроса;
		\param nRecipientDeliverLevel	[in] - число уровней, на которые нужно передать данные;
		\param nSenderDeliverLevel		[in] - уровень вложенности сендера;
		\param wstrSenderInstanceId		[in] - идентификатор сендера;
		\param parDeliveryParams		[in] - дополнительные параметры доставки;
		\param parReportResult			[in] - данные отчета
	*/
		virtual void DeliverReport(						
			std::wstring wstrRequestId,
			int nRecipientDeliverLevel,
			int nSenderDeliverLevel,
			std::wstring wstrSenderInstanceId,
			KLSTD::CAutoPtr<KLPAR::Params> parDeliveryParams,
			KLSTD::CAutoPtr<KLPAR::Params> parReportResult) = 0;

	/*!
	  \brief Доставка результатов обновления версии сервера.

		\param wstrTaskFileId			[in] - идентификатор исходной задачи;
		\param parResultTaskStateEvent	[in] - событие результата установки
	*/
		virtual void ReportRemoteInstallationResult(
			std::wstring wstrTaskTsId,
			KLSTD::CAutoPtr<KLPAR::Params> parResultTaskStateEvent) = 0;

	/*!
	  \brief Сигнал о завершении генерации отчета.

		\param wstrRequestId			[in] - идентификатор запроса;
		\param lResultCode				[in] - код результата;
		\param parParams				[in] - дополнительные параметры 
	*/
		virtual void NotifyOnReportResult(
			std::wstring wstrRequestId,
			long lResultCode,
			KLSTD::CAutoPtr<KLPAR::Params> parParams) = 0;
	};

} // namespace KLSRVH

KLCSSRV_DECL void KLSRVH_CreateSrvHierarchyMasterProxy(
	KLSTD::CAutoPtr<KLSRV::ConnectionInfo> pConnectionInfo, 
	KLSRVH::SrvHierarchyMaster** ppSrvHierarchyMasterProxy);

#endif // KLSRVH_SRVHRCH_MASTER_H


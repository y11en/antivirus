/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file srvhrch/srvhrch_slave.h
 * \author Andrew Lashenkov
 * \date	20.01.2005 10:32:00
 * \brief Интерфейс подчиненного сервера в иерархии серверов.
 *
 */

#ifndef KLSRVH_SRVHRCH_SLAVE_H
#define KLSRVH_SRVHRCH_SLAVE_H

#include <srvp/srvhrch/srvhrch.h>
#include <std/par/params.h>
#include <server/srvinst/connstore.h>

namespace KLSRVH {

	class SrvHierarchySlave : public KLSTD::KLBaseQI {
	public:
		virtual ~SrvHierarchySlave(){}
	/*!
	  \brief Ping.

		\param pParams	[in] Произвольные параметры пинга.
	*/
		virtual void PingSlave(						
			KLSTD::CAutoPtr<KLPAR::Params> pParams,
			KLSTD::CAutoPtr<KLPAR::Params>& pOutParams) = 0;
		
	/*!
	  \brief ForceRetranslateUpdates.

		\param pParams	[in] Произвольные дополнительные параметры ретрансляции.
	*/
		virtual bool ForceRetranslateUpdates(						
			KLSTD::CAutoPtr<KLPAR::Params> pParams) = 0;
	/*!
	  \brief Воссоздание пакета удаленной установки, переданного по FT.

		\param wstrPackageGUID [in] - GUID пакета
		\param wstrNewPackageName [in] - предполагаемое имя пакета
		\param wstrFileId [in] - идентификатор файла в FT
		\param wstrProductName [in] - имя продукта в дистрибутиве
		\param wstrProductVersion [in] - версия продукта в дистрибутиве
		\param wstrProductDisplName [in] - дисплэйное имя продукта в дистрибутиве
		\param wstrProductDisplVersion [in] - дисплэйная версия продукта в дистрибутиве
	*/
		virtual bool RecreateInstallationPackage(						
			const std::wstring& wstrPackageGUID,
			const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFileId,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			bool bReplaceExisting,
			std::wstring& wstrCreatedPackageName) = 0;

	/*!
	  \brief Запуск задачи на подчиненном сервере.

		\param wstrTaskTsId [in] - идентификатор задачи
		\param wstrTaskTypeName [in] - тип задачи
		\param parParams [in] - произвольные параметры
		\param parOutParams [out] - произвольные параметры
	*/
		virtual void StartTask(						
			const std::wstring& wstrTaskTsId,
			const std::wstring& wstrTaskTypeName,
			KLSTD::CAutoPtr<KLPAR::Params> parParams,
			KLSTD::CAutoPtr<KLPAR::Params>& parOutParams) = 0;
		
	/*!
	  \brief Запуск задачи удаленной установки.

		\param parTaskInfo [in] - parTaskInfo исходной задачи
		\param parTaskParams [in] - parTaskParams исходной задачи
		\return Результат операции. false - пакет отсутствует на данном сервере
	*/
		virtual bool StartRemoteInstallation(
			KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
			bool& bAlreadyInstalledSkipped) = 0;
	};
	
} // namespace KLSRVH

KLCSSRV_DECL void KLSRVH_CreateSrvHierarchySlaveProxy(
	KLSTD::CAutoPtr<KLSRV::ConnectionInfo> pConnectionInfo, 
	KLSRVH::SrvHierarchySlave** ppSrvHierarchySlaveProxy);

#endif // KLSRVH_SRVHRCH_SLAVE_H


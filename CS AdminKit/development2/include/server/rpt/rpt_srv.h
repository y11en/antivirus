/*!
 * (C) 2007 Kaspersky Lab 
 * 
 * \file	server/rpt/reports.h
 * \author	Andrew Lashchenkov
 * \date	10.06.2003 15:30:06
 * \brief	интерфейс для получения отчетов
 * 
 */


#ifndef __KLPRT_RPT_SRV_H__
#define __KLPRT_RPT_SRV_H__

#include <srvp/rpt/reports.h>

namespace KLSRV
{
    /*!
        \brief Reports - интерфейс для редактирования Reports */
    class ReportsSrv : public KLSTD::KLBaseQI
    {
    public:
        virtual void GetReportTypes(
			KLDB::DbConnectionPtr pDbConnection,
			KLRPT::ReportTypeCommonVector & vectReportTypes ) = 0;

        virtual void GetReportTypeDetailedData(
			KLDB::DbConnectionPtr pDbConnection,
            const KLRPT::ReportType & reportType,
            KLRPT::ReportTypeDetailedData & details ) = 0;

        virtual void GetConstantOutputForReportType(
			KLDB::DbConnectionPtr pDbConnection,
            const KLRPT::ReportType & reportType,
			const KLRPT::ERptXmlTargetType & xmlTargetType,
            std::string & strOutput ) = 0;

        virtual void GetReportIds( 
			KLDB::DbConnectionPtr pDbConnection,
			KLRPT::ReportIdVector & vectReportIds ) = 0;

        virtual void GetReports( 
			KLDB::DbConnectionPtr pDbConnection,
			KLRPT::ReportCommonVector & vectReports ) = 0;

        virtual void GetReportCommonData(
			KLDB::DbConnectionPtr pDbConnection,
            const KLRPT::ReportId & reportId,
            KLRPT::ReportCommonData & reportCommonData ) = 0;

        virtual void GetReportDetailedData(
			KLDB::DbConnectionPtr pDbConnection,
            const KLRPT::ReportId & reportId,
            KLRPT::ReportDetailedData & details ) = 0;

        virtual void AddReport( 
			KLDB::DbConnectionPtr pDbConnection,
			KLRPT::ReportDetailedData & details ) = 0;

        virtual void UpdateReport( 
			KLDB::DbConnectionPtr pDbConnection,
			KLRPT::ReportDetailedData & details ) = 0;

        virtual void DeleteReport( 
			KLDB::DbConnectionPtr pDbConnection,
			const KLRPT::ReportId & reportId ) = 0;

        virtual void ExecuteReport(
			KLDB::DbConnectionPtr pDbConnection,
            const KLRPT::ReportId & reportId,
            time_t  tStart,
            time_t  tEnd,
			KLSTD::CAutoPtr<KLPAR::Params> pLocalizationParams,
            //const std::vector<KLRPT::HostDescr> & vectHosts,
            std::string & strOutput,
			KLSTD::CAutoPtr<KLPAR::Params>& pChartParams) = 0;

        virtual void CreateDefaultReportData( 
			KLDB::DbConnectionPtr pDbConnection,
			const KLRPT::ReportType & reportType, 
			KLRPT::ReportDetailedData & details ) = 0;

        virtual void ExecuteReportAsync(
			KLDB::DbConnectionPtr pDbConnection,
            const KLRPT::ReportId & reportId,
            time_t  tStart,
            time_t  tEnd,
			KLSTD::CAutoPtr<KLPAR::Params> pLocalizationParams,
            const std::wstring & wstrRequestId,
			KLADMSRV::AdmServerAdviseSink*  pSink,
            KLADMSRV::HSINKID&              hSink) = 0;

        virtual void ExecuteReportAsyncCancel(
			KLDB::DbConnectionPtr pDbConnection,
            const std::wstring & wstrRequestId) = 0;

        virtual void ExecuteReportAsyncGetData(
			KLDB::DbConnectionPtr pDbConnection,
            const std::wstring & wstrRequestId,
			long lDataBufSize,
			void* pDataBuf,
			long& nDataSize,
			long& nDataSizeRest,
			KLSTD::CAutoPtr<KLPAR::Params>& pChartParams) = 0;

        virtual void GetFilterSettings(
			KLDB::DbConnectionPtr pDbConnection,
            const KLRPT::ReportType & reportType,
			KLSTD::CAutoPtr<KLPAR::Params>& pFilterSettings) = 0;

        virtual void CheckReportType(const KLRPT::ReportType reportType) = 0;

		virtual void CheckReportsCache(KLDB::DbConnectionPtr pDbConnection) = 0;

        virtual void ExecuteReportAsyncCancelWaitingForSlaves(
			KLDB::DbConnectionPtr pDbConnection,
            const std::wstring & wstrRequestId) = 0;
    };
};

#endif //__KLPRT_RPT_SRV_H__

/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	rpt/reports.h
 * \author	Mikhail Karmazine
 * \date	10.06.2003 15:30:06
 * \brief	интерфейс для получения отчетов
 * 
 */


#ifndef __KLPRT_REPORTS_H__
#define __KLPRT_REPORTS_H__

#pragma warning  (disable : 4786 )
#include <vector>

#include <std/base/klstd.h>
#include <kca/prss/settingsstorage.h>
#include <srvp/grp/groups.h>

namespace KLRPT
{
	typedef enum {
		RT_PROTECTION = 0,
		RT_VIRUS_ACTIVITY,
		RT_MOST_INFECTED,
		RT_LICENSES,
		RT_SOFT_VERSION,
		RT_AVB_VERSION,
		RT_ERRORS,
		RT_NET_ATTACKS,
		RT_COMPETITOR_SOFT,
		RT_AV_PROD_INFO_FS,
		RT_AV_PROD_INFO_EML,
		RT_AV_PROD_INFO_PH,
		RT_AV_PROD_INFO,
		RT_MOST_INFECTING_USR,
		RT_INVENTORY,
		RT_INVALID_REPORT_TYPE = -1
	} REPORT_TYPES;

	enum EAVProdType{
		AVP_WKS = 0,
		AVP_FSRV = 1,
		AVP_EMAIL = 2,
		AVP_PH = 3
	};

	enum EFldType{
		FLD_TYPE_UNKNOWN = 0,

		FLD_TYPE_DATA_TYPE_STRING	= 0x000001,
		FLD_TYPE_DATA_TYPE_INT		= 0x000002,
		FLD_TYPE_DATA_TYPE_BOOL		= 0x000004,
		FLD_TYPE_DATA_TYPE_DATE		= 0x000008,
		FLD_TYPE_DATA_TYPE_DATETIME	= 0x000010,
		FLD_TYPE_DATA_TYPE_IP		= 0x000020,
		FLD_TYPE_DATA_TYPE_BITMASK	= 0x000040,

		FLD_TYPE_DATA_MASK_TYPE		= 0x0000ff,

		FLD_TYPE_SEMANTIC_PRODUCT		= 0x000100,
		FLD_TYPE_SEMANTIC_VERSION		= 0x000200,
		FLD_TYPE_SEMANTIC_GROUPNAME		= 0x000400,
		FLD_TYPE_SEMANTIC_DOMAIN		= 0x000800,
		FLD_TYPE_SEMANTIC_HOSTNAME		= 0x001000,
		FLD_TYPE_SEMANTIC_KEY			= 0x002000,
		FLD_TYPE_SEMANTIC_VIRTYPE		= 0x004000,
		FLD_TYPE_SEMANTIC_VIRACTION		= 0x008000,
		FLD_TYPE_SEMANTIC_NETPROTOCOL	= 0x010000,
		FLD_TYPE_SEMANTIC_PROTECTSTATUS	= 0x020000,
		FLD_TYPE_SEMANTIC_PROTECTREASON	= 0x040000,

		FLD_TYPE_DATA_MASK_SEMANTIC		= 0xffff00,

		/*
		FLD_TYPE_SEMANTIC_GROUPID			= 0x020000,
		FLD_TYPE_SEMANTIC_HOSTID			= 0x040000,
		FLD_TYPE_SEMANTIC_PRODUCTID			= 0x080000,
		FLD_TYPE_SEMANTIC_PROTECTSTATUSID	= 0x100000,
		FLD_TYPE_SEMANTIC_PROTECTREASONID	= 0x200000,
		*/

		FLD_TYPE_PRODUCT		= FLD_TYPE_SEMANTIC_PRODUCT			| FLD_TYPE_DATA_TYPE_STRING,
		FLD_TYPE_VERSION		= FLD_TYPE_SEMANTIC_VERSION			| FLD_TYPE_DATA_TYPE_STRING,
		FLD_TYPE_GROUPNAME		= FLD_TYPE_SEMANTIC_GROUPNAME		| FLD_TYPE_DATA_TYPE_STRING,
		FLD_TYPE_DOMAIN			= FLD_TYPE_SEMANTIC_DOMAIN			| FLD_TYPE_DATA_TYPE_STRING,
		FLD_TYPE_HOSTNAME		= FLD_TYPE_SEMANTIC_HOSTNAME		| FLD_TYPE_DATA_TYPE_STRING,
		FLD_TYPE_KEY			= FLD_TYPE_SEMANTIC_KEY				| FLD_TYPE_DATA_TYPE_STRING,
		FLD_TYPE_VIRTYPE		= FLD_TYPE_SEMANTIC_VIRTYPE			| FLD_TYPE_DATA_TYPE_INT,
		FLD_TYPE_VIRACTION		= FLD_TYPE_SEMANTIC_VIRACTION		| FLD_TYPE_DATA_TYPE_STRING,
		FLD_TYPE_NETPROTOCOL	= FLD_TYPE_SEMANTIC_NETPROTOCOL		| FLD_TYPE_DATA_TYPE_STRING,
		FLD_TYPE_PROTECTSTATUS	= FLD_TYPE_SEMANTIC_PROTECTSTATUS	| FLD_TYPE_DATA_TYPE_BITMASK,
		FLD_TYPE_PROTECTREASON	= FLD_TYPE_SEMANTIC_PROTECTREASON	| FLD_TYPE_DATA_TYPE_BITMASK,
		
		/*
		,
		FLD_TYPE_GROUPID	= FLD_TYPE_SEMANTIC_GROUPID		| FLD_TYPE_DATA_TYPE_INT,
		FLD_TYPE_HOSTID		= FLD_TYPE_SEMANTIC_HOSTID		| FLD_TYPE_DATA_TYPE_STRING,
		FLD_TYPE_PRODUCTID	= FLD_TYPE_SEMANTIC_PRODUCTID	| FLD_TYPE_DATA_TYPE_STRING
		FLD_TYPE_PROTECTSTATUSID	= FLD_TYPE_SEMANTIC_PROTECTSTATUSID	| FLD_TYPE_DATA_TYPE_INT,
		FLD_TYPE_PROTECTREASONID	= FLD_TYPE_SEMANTIC_PROTECTREASONID	| FLD_TYPE_DATA_TYPE_INT,
		*/

		FLD_TYPE_DATA_MASK_NOTNULL		= 0x1000000

	};
	
    /*!
        \brief Событие c_EventRptExecDone публикуется при завершении асинхронной генерации отчета, в т.ч. при ошибке
		    c_evpRptAsyncExecId, STRING_T - Идентификатор запроса, глобально-уникальный ключ
			c_evpRptAsyncExecResult, INT_T - Результат (c_nRptAsyncExecResultOk - успех)
			c_evpRptAsyncExecPercentComplete, INT_T - Процент выполнения (может отсутствовать)
    */
	const wchar_t c_EventRptExecDone[]=L"KLPPT_EventRptExecDone";

		const wchar_t c_evpRptAsyncExecId[]=L"KLPPT_RptAsyncExecId"; // STRING_T
		const wchar_t c_evpRptAsyncExecResult[]=L"KLPPT_RptAsyncExecResult"; // INT_T
			const long c_nRptAsyncExecResultOk = 0;
			const long c_nRptAsyncExecResultInProgress = 1;
			const long c_nRptAsyncExecResultDataReady = 2;
			const long c_nRptAsyncExecResultFail = -1;
		const wchar_t c_evpRptAsyncExecPercentComplete[]=L"KLPPT_RptAsyncExecPercentComplete"; // INT_T
		const wchar_t c_evpRptAsyncExecInProgressStatus[] = L"KLPPT_RptAsyncExecInProgressStatus"; // INT_T
			const long c_nRptAsyncExecInProgressStatusMasterDataReady = 1;

	/*
	 *	Параметры используемые для локализации некоторых значений при генерации отчета
	 */
	const wchar_t c_ProductsLocalizationInfo[] = L"KLRPT_LOC_PRODUCTS";					// Params
		const wchar_t c_ProductsLocalizationInfo_Name[] = L"KLRPT_PROD_NAME";				// String
		const wchar_t c_ProductsLocalizationInfo_Version[] = L"KLRPT_PROD_VER";				// String
		const wchar_t c_ProductsLocalizationInfo_DisplayName[] = L"KLRPT_PROD_NAME_LOC";	// String
		const wchar_t c_ProductsLocalizationInfo_DisplayVersion[] = L"KLRPT_PROD_VER_LOC";	// String

    /*! 
        \brief ReportType - тип, использующийся для идентификации типов отчетов. */
    typedef long ReportType;

	const wchar_t c_RptFldTypes[] = L"KLRPT_FLD_TYPES";			// PARAMS_T
		const wchar_t c_RptFldTypeName[] = L"KLRPT_FLD_TYPE_NAME";	// STRING_T
		const wchar_t c_RptFldTypeId[] = L"KLRPT_FLD_TYPE_ID";		// INT_T

    /*! 
        \brief c_RptDefFldDtl - опциональное поле в ReportTypeDetailedData::m_parExtraData - 
		массив STRING_T - полей, которые по умолчанию 
		должны быть в ReportDetailedData::vectDetFields. */
	const wchar_t c_RptDefFldDtl[] = L"KLRPT_DEF_FLD_DTL";			// ARRAY_T

    /*! 
        \brief c_RptDefFldSum - опциональное поле в ReportTypeDetailedData::m_parExtraData - 
		массив STRING_T - полей, которые по умолчанию 
		должны быть в ReportDetailedData::vectSumFields. */
	const wchar_t c_RptDefFldSum[] = L"KLRPT_DEF_FLD_SUM";			// ARRAY_T

    /*! 
        \brief c_RptDefFldTtl - опциональное поле в ReportTypeDetailedData::m_parExtraData - 
		массив STRING_T - полей, которые по умолчанию 
		должны быть в ReportDetailedData::vectTotals. */
	const wchar_t c_RptDefFldTtl[] = L"KLRPT_DEF_FLD_TTL";			// ARRAY_T

    /*! 
        \brief c_RptDefFldDtlOrd - опциональное поле в ReportTypeDetailedData::m_parExtraData - PARAMS_T,
		задает сортировку по умолчанию для полей, которые должны быть по умолчанию в ReportDetailedData::vectDetFields;
		имя - имя поля, значение (BOOL_T) - тип сортировки. */
	const wchar_t c_RptDefFldDtlOrd[] = L"KLRPT_DEF_FLD_DTL_ORD";			// PARAMS_T

    /*! 
        \brief c_RptDefFldSumOrd - опциональное поле в ReportTypeDetailedData::m_parExtraData - PARAMS_T,
		задает сортировку по умолчанию для полей, которые должны быть по умолчанию в ReportDetailedData::vectSumFields;
		имя - имя поля, значение (BOOL_T) - тип сортировки. */
	const wchar_t c_RptDefFldSumOrd[] = L"KLRPT_DEF_FLD_SUM_ORD";			// PARAMS_T

    /*! 
        \brief параметры фильтрации по полям details в ReportCommonData::pExtraData.
			KLRPT::c_RptFldFilter:Params
				<Отображаемое имя поля>:Params
					c_Rpt_flt_condition
					c_Rpt_flt_value
					c_Rpt_flt_value2
	*/

	enum EFltCond {
		COND_NONE = 0x0,
		COND_EQUAL = 0x1,
		COND_NOT_EQUAL = 0x2,
		COND_LESS = 0x4,
		COND_GREAT = 0x8
	};
	enum EFltActionID {
		AID_CURED,
		AID_REMOVED,
		AID_FAILED
	};
	const wchar_t c_RptFldFilter[] = L"KLRPT_FLD_FILTER";				// PARAMS_T
		const wchar_t c_Rpt_flt_condition[] = L"KLRPT_FLT_CONDITION";	// INT_T (see EFltCond)
		const wchar_t c_Rpt_flt_value[] = L"KLRPT_FLT_VALUE";			// STRING_T
		const wchar_t c_Rpt_flt_value2[] = L"KLRPT_FLT_VALUE2";			// STRING_T

    /*! 
        \brief c_RptSlaveExecTimeoutSec - параметр в ReportCommonData::pExtraData -
			время ожидания ответа от подчиненных серверов при генерации отчета, сек. */
	const wchar_t c_RptSlaveExecTimeoutSec[] = L"KLRPT_SLAVE_EXEC_TIMEOUT";			// INT_T
	const long c_nRptSlaveExecTimeoutSec_Dflt = 300; //!brief - дефолтное значение для c_RptSlaveExecTimeoutSec.

	enum ERptTargetType{
		RTT_XML = 0,
		RTT_CSV = 1
	};
    /*! 
        \brief c_RptTargetType - параметр в pLocalizationParams (ExecuteReportAsync) - формат отчета 
		(ERptTargetType). По умолчанию - RTT_XML. */
	const wchar_t c_RptTargetType[] = L"KLRPT_TARGET_TYPE";			// INT_T

	enum ERptXmlTargetType{
		RTT_HTML = 0,
		RTT_XLS = 1
	};
	
    struct ReportTypeCommonData
    {
        ReportType reportType;
        std::wstring wstrDisplayName;

		ReportTypeCommonData()
		{
			reportType = -1;
		}
    };
    typedef std::vector<ReportTypeCommonData> ReportTypeCommonVector;

    struct ReportTypeDetailedData
    {
        std::wstring wstrDescr;
        std::vector< std::wstring > vectPossibleDetFields;
        std::vector< std::wstring > vectPossibleSumFields;
        std::vector< std::wstring > vectPossibleTotals;
        bool bAcceptsTimeInterval;
        bool bAcceptsComputerList;
		KLPAR::ParamsPtr m_parExtraData;	/*! \brief 
							+-- c_RptFldTypes //PARAMS_T
									+-- [отображаемое имя поля] // PARAMS_T
										+-- c_RptFldTypeName // STRING_T - внутреннее имя поля, используемое в фильтре
										+-- c_RptFldTypeId	// INT_T - тип поля, см. EFldType
											*/
		ReportTypeDetailedData()
		{
			bAcceptsTimeInterval = false;
			bAcceptsComputerList = false;
		}
    };

    /*! 
        \brief ReportId - тип, использующийся в качестве 
        уникального идентификатора для отчета */
    typedef long ReportId;
    typedef std::vector<ReportId> ReportIdVector;

    /*!
        \brief ReportCommonData - общие данные для отчета. */
    struct ReportCommonData
    {
        ReportId reportId;
        std::wstring wstrDisplayName;
        ReportType reportType;
        time_t tCreated;
        time_t tModified;
		long nGroupId; /*! \brief Идентификатор группы; 
						= KLGRP::GROUPID_INVALID_ID, если отчет не групповой */
        KLSTD::CAutoPtr<KLPAR::Params> pExtraData;//! extra parameters
		
		ReportCommonData()
		{
	        reportId = -1;
			tCreated = 0;
			tModified = 0;
			reportType = -1;
			nGroupId = KLGRP::GROUPID_INVALID_ID;
		}

		ReportCommonData(const ReportCommonData& rcd)
		{
			reportId = rcd.reportId;
			wstrDisplayName = rcd.wstrDisplayName;
			reportType = rcd.reportType;
			tCreated = rcd.tCreated;
			tModified = rcd.tModified;
			nGroupId = rcd.nGroupId;
			if( rcd.pExtraData )
			{
				KLPAR_CreateParams(&pExtraData);
				pExtraData->CopyFrom(rcd.pExtraData);
			}
		}

		ReportCommonData& operator=(const ReportCommonData& rcd)
		{
			reportId = rcd.reportId;
			wstrDisplayName = rcd.wstrDisplayName;
			reportType = rcd.reportType;
			tCreated = rcd.tCreated;
			tModified = rcd.tModified;
			nGroupId = rcd.nGroupId;
			if( rcd.pExtraData )
			{
				KLPAR_CreateParams(&pExtraData);
				pExtraData->CopyFrom(rcd.pExtraData);
			}
			return *this;
		}
    };
    typedef std::vector<ReportCommonData> ReportCommonVector;


    typedef std::pair< std::wstring, bool > FieldOrder;

    struct HostDescr
    {
        std::wstring wstrDomainName;
        std::wstring wstrHostInternalName;
        std::wstring wstrHostDisplayName;
    };
    typedef std::vector<HostDescr> HostVector;

    struct ReportDetailedData : ReportCommonData
    {
        ReportDetailedData() : tStart(0), tEnd(0), nMaxDetailsRecords(-1), nPictureType(0) {};

        std::vector<std::wstring>   vectDetFields;
        std::vector<FieldOrder>     vectDetOrder;

        std::vector<std::wstring>   vectSumFields;
        std::vector<FieldOrder>     vectSumOrder;

        std::vector<std::wstring>   vectTotals;

        time_t      tStart;
        time_t      tEnd;

		long nMaxDetailsRecords;

		long nPictureType;

        HostVector  vectHosts; /*! \brief Игнорируется, если 
							   ReportCommonData::nGroupId != KLGRP::GROUPID_INVALID_ID */
    };

    /*!
        \brief Reports - интерфейс для редактирования Reports */
    class Reports : public KLSTD::KLBaseQI
    {
    public:
        virtual void GetReportTypes( ReportTypeCommonVector & vectReportTypes ) = 0;

        virtual void GetReportTypeDetailedData(
            const ReportType & reportType,
            ReportTypeDetailedData & details ) = 0;

        virtual void GetConstantOutputForReportType(
            const ReportType & reportType,
            std::string & strOutput ) = 0;

        virtual void GetReportIds( ReportIdVector & vectReportIds ) = 0;

        virtual void GetReports( ReportCommonVector & vectReports ) = 0;

        virtual void GetReportCommonData(
            const ReportId & reportId,
            ReportCommonData & reportCommonData ) = 0;

        virtual void GetReportDetailedData(
            const ReportId & reportId,
            ReportDetailedData & details ) = 0;

        virtual void AddReport( ReportDetailedData & details ) = 0;

        virtual void UpdateReport( ReportDetailedData & details ) = 0;

        virtual void DeleteReport( const ReportId & reportId ) = 0;

        virtual void ExecuteReport(
            const ReportId & reportId,
            time_t  tStart,
            time_t  tEnd,
			KLSTD::CAutoPtr<KLPAR::Params> pLocalizationParams,
            //const std::vector<KLRPT::HostDescr> & vectHosts,
            std::string & strOutput,
			KLSTD::CAutoPtr<KLPAR::Params>& pChartParams) = 0;

        virtual void CreateDefaultReportData( 
			const ReportType & reportType, 
			ReportDetailedData & details ) = 0;

        virtual void ExecuteReportAsync(
            const ReportId & reportId,
            time_t  tStart,
            time_t  tEnd,
			KLSTD::CAutoPtr<KLPAR::Params> pLocalizationParams,
            const std::wstring & wstrRequestId,
			KLADMSRV::AdmServerAdviseSink*  pSink,
            KLADMSRV::HSINKID&              hSink) = 0;

        virtual void ExecuteReportAsyncCancel(
            const std::wstring & wstrRequestId) = 0;

        virtual void ExecuteReportAsyncGetData(
            const std::wstring & wstrRequestId,
			long lDataBufSize,
			void* pDataBuf,
			long& nDataSize,
			long& nDataSizeRest,
			KLSTD::CAutoPtr<KLPAR::Params>& pChartParams) = 0;

        virtual void GetFilterSettings(
            const ReportType & reportType,
			KLSTD::CAutoPtr<KLPAR::Params>& pFilterSettings) = 0;

		virtual void ExecuteReportAsyncCancelWaitingForSlaves(
			const std::wstring & wstrRequestId) = 0;
		};

	    class Reports2 : public Reports
		{
			virtual void GetConstantOutputForReportType2(
				const ReportType & reportType,
				const ERptXmlTargetType & xmlTargetType,
				std::string & strOutput ) = 0;
		};


    /*!
        \brief SlaveSrvReports - интерфейс получения отчетов с подчиненных серверов серверов */
    class SlaveSrvReports : public KLSTD::KLBaseQI{
    public:
		virtual void Deinitialize() = 0;
		
        virtual void ExecuteSlaveReportAsync(
			const ReportDetailedData& details,
			const KLSTD::CAutoPtr<KLPAR::Params> pLocalizationParams,
            const std::wstring& wstrRequestId,
			int nTimeoutInSec,
			KLEV::AdviseEvSink* pSink,
            KLEV::HSINKID& hSink) = 0;

        virtual void ExecuteSlaveReportAsyncCancel(
            const std::wstring & wstrRequestId) = 0;

        virtual void ExecuteSlaveReportAsyncGetData(
            const std::wstring& wstrRequestId,
			int nRecordsCount,
			KLPAR::Params** pParamsRecords,
			bool& bMoreDataAvailable) = 0;
	};
};

#define RPT_TIMEZONE_INFO       L"RPT_TIMEZONE_INFO"        //PARAMS_T

#define RPT_TZ_BIAS             L"RPT_TZ_BIAS"              //INT_T
#define RPT_TZ_STD_NAME         L"RPT_TZ_STD_NAME"          //STRING_T
#define RPT_TZ_STD_DATE         L"RPT_TZ_STD_DATE"          //PARAMS_T
#define RPT_TZ_STD_BIAS         L"RPT_TZ_STD_BIAS"          //INT_T
#define RPT_TZ_DAYLIGHT_NAME    L"RPT_TZ_DAYLIGHT_NAME"     //STRING_T
#define RPT_TZ_DAYLIGHT_DATE    L"RPT_TZ_DAYLIGHT_DATE"     //PARAMS_T
#define RPT_TZ_DAYLIGHT_BIAS    L"RPT_TZ_DAYLIGHT_BIAS"     //INT_T

#define RPT_STM_YEAR            L"RPT_STM_YEAR"
#define RPT_STM_MONTH           L"RPT_STM_MONTH"
#define RPT_STM_DAYOFWEEK       L"RPT_STM_DAYOFWEEK"
#define RPT_STM_DAY             L"RPT_STM_DAY"
#define RPT_STM_HOUR            L"RPT_STM_HOUR"
#define RPT_STM_MINUTE          L"RPT_STM_MINUTE"
#define RPT_STM_SECOND          L"RPT_STM_SECOND"
#define RPT_STM_MILLISECOND     L"RPT_STM_MILLISECOND"

#define RPT_LOC_LOCALE           L"RPT_LOC_LOCALE"          //INT_T

/*!
    \brief Глубина рекурсии по подчиненным серверам (хранится в KLRPT::ReportCommonData::pExtraData)
		0 - не учитывать подчиненные сервера;
		-1 - полная рекурсия по подчиненным серверам */
#define KLRPT_SLAVE_REC_DEPTH	L"KLRPT_SLAVE_REC_DEPTH"	// INT_T

/*!
    \brief Отчеты с подчиненных серверов подчиненных серверов интегрировать в отчеты с подчиненных серверов. 
		(хранится в KLRPT::ReportCommonData::pExtraData) */
#define KLRPT_SLAVE_REC_AGGREGATE	L"KLRPT_SLAVE_REC_AGGREGATE"	// BOOL_T

#define KLRPT_USE_UTC				L"KLRPT_USE_UTC"	// BOOL_T
#define KLRPT_DONT_USE_SPACES_FOR_SLASHES	L"KLRPT_DONT_USE_SPACES_FOR_SLASHES" // BOOL_T

/*!
    \brief Признак того, что отчет является временным и должен удаляться после генерации.
*/
#define KLRPT_TEMPORAL	L"KLRPT_TEMPORAL"	// BOOL_T

/*!
    \brief Период генерации отчета на подчиненных серверах для формирования кеша (в минутах).
*/
#define KLRPT_CACHE_PERIOD_MINUTES	L"KLRPT_CACHE_PERIOD_MINUTES"	// INT_T

/*!
    \brief Включать ли в отчет строки details, собиравемые с подчиненных серверов.
*/
#define KLRPT_INCLUDE_SLV_DTL_RECORDS L"KLRPT_INCLUDE_SLV_DTL_RECORDS" // BOOL_T

/*!
    \brief Макс. число строк details, собиравемых с подчиненных серверов (-1 - неограничено).
*/
#define KLRPT_MAX_SLV_DTL_RECORDS L"KLRPT_MAX_SLV_DTL_RECORDS" // INT_T
#define KLRPT_MAX_SLV_DTL_RECORDS_DEF 1000

namespace KLSRV{
	class ConnectionInfo;
};

KLCSSRVP_DECL void KLRPT_InitializeDefaultReportDetailedData(
	const KLRPT::ReportTypeCommonData& reportTypeCommonData,														 
	const KLRPT::ReportTypeDetailedData& reportTypeDetailedData, 
	KLRPT::ReportDetailedData& reportDetailedData);

KLCSSRV_DECL void KLRPT_CreateSlaveSrvReportsProxy(
	KLSTD::CAutoPtr<KLSRV::ConnectionInfo> pConnectionInfo, 
	KLRPT::SlaveSrvReports** ppSlaveSrvReportsProxy);

KLCSSRVP_DECL void KLRPT_DefineTimeInterval(
	const KLRPT::ReportDetailedData& rdd, time_t& tStart, time_t& tEnd);

#endif //__KLPRT_REPORTS_H__

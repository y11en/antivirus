/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file srvstgs/admsrvstngs.h
 * \author Andrew Lashenkov
 * \date	01.07.2005 11:08:00
 * \brief Настройки сервера администрирования.
 *
 */

#include <std/base/klbase.h>

#ifndef __KLSRVP_ADMSRVSTNGS_H__
#define __KLSRVP_ADMSRVSTNGS_H__

//#pragma warning  (disable : 4786 )
#include <std/base/klstd.h>
#include <srvp/admsrv/admsrv_sinks.h>
#include <srvp/admsrv/admsrv.h>

namespace KLSRVS {
	enum EChangeShareResult{
		CS_OK = 0,
		CS_IN_PROGRESS = 1,
		CS_ERR = -1
	};

	enum EChangeShareProgressStage{
		CS_NONE,
		CS_FILE_COPY_PREPARE,
		CS_FILE_COPY,
		CS_DATA_UPDATE
	};

	/*
	enum EChangeShareErrorCode{
		CS_ERR_NO_ERROR = 0,
		CS_ERR_EXCEPTION_OCCURED,
		CS_ERR_PATH_NOT_FOUND,
		CS_ERR_NOT_ENOUGH_SPACE,
		CS_ERR_ACCESS_DENIED,
		CS_ERR_FILE_OPERATION_FAILED
	};
	
	KLSTD::STDE_NOTDIR
	KLSTD::STDE_NOSPC

	*/
    /*!
        \brief Событие c_EventChangeSharedFolderDone публикуется в ходе и при завершении процеса смены шары, в т.ч. при ошибке.
		    c_evpChangeSharedFolder_Path,	STRING_T - Идентификатор запроса, глобально-уникальный ключ
			c_evpChangeSharedFolder_Result, INT_T - Результат (EChangeShareResult)
			c_evpChangeSharedFolder_Stage, INT_T - статус выполнения (EChangeShareProgressState)
			c_evpChangeSharedFolder_ErrorCode, INT_T - код ошибки
			c_evpChangeSharedFolder_PercentComplete, INT_T - Процент выполнения (может отсутствовать)
    */
	const wchar_t c_EventChangeSharedFolderDone[]=L"KLSRVS_EventChangeSharedFolderDone";

		const wchar_t c_evpChangeSharedFolder_Path[]=L"KLSRVS_ChangeSharedFolder_Path"; // STRING_T
		const wchar_t c_evpChangeSharedFolder_Result[]=L"KLSRVS_ChangeSharedFolder_Result"; // INT_T
		const wchar_t c_evpChangeSharedFolder_Stage[]=L"KLSRVS_ChangeSharedFolder_Stage"; // INT_T
		const wchar_t c_evpChangeSharedFolder_ErrorCode[]=L"KLSRVS_ChangeSharedFolder_ErrorCode"; // INT_T
		const wchar_t c_evpChangeSharedFolder_PercentComplete[]=L"KLSRVS_ChangeSharedFolder_PercentComplete"; // INT_T

    /*!
        \brief AdmServerSettings - интерфейс для изменения настроек сервера */
    class AdmServerSettings : public KLSTD::KLBaseQI
    {
    public:
        virtual std::wstring GetSharedFolder() = 0;

        virtual void ChangeSharedFolder(
			const std::wstring&	wstrNetworkPath,
			KLADMSRV::AdmServerAdviseSink* pSink,
            KLADMSRV::HSINKID&      hSink) = 0;
	};
} // namespace KLSRVS

std::wstring KLSRVS_GetAdmServerNetworkShare();

#endif // __KLSRVP_ADMSRVSTNGS_H__

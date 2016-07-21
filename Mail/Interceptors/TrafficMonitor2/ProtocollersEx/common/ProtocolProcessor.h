#ifndef _PROTOCOLPROCESSOR_H_
#define _PROTOCOLPROCESSOR_H_

#include <ThreadImp.h>
#include <TrafficProtocoller.h>
using namespace TrafficProtocoller;

#include "../../StateManager.h"
#include <Extract/plugin/p_msoe.h>
#include <Mail/structs/s_mc.h>
#include <Mail/structs/s_mc_trafficmonitor.h>
#include "../interfaces/IProtocolAnalyzer.h"
#include <Mail/common/GetAppInfoByPID.h>
#include "noopthread.h"

class CProtocolProcessor : public CThreadImp
{
public:
	CProtocolProcessor();
	virtual ~CProtocolProcessor();

	detect_code_t Detect(
		data_source_t dsDataSource, 
		tBYTE* pData, 
		tDWORD dwDataSize
		);
	tERROR Process(
		data_source_t dsDataSource, 
		tBYTE* pData, 
		tDWORD dwDataSize
		);

	//////////////////////////////////////////////////////////////////////////
	//
	// параметры инициализации
	//
	struct InitParams_t
	{
		hOBJECT hParent;
		session_t* pSession;            // Сессия, в рамках которой осуществляется публикация
		hOBJECT hTrafficMonitor;     // Указатель на объект ТраффикМонитора
		IProtocolAnalyzer* pAnalyzer;	   // Анализатор протокола
		protocoller_order_t hProtocolType; // Тип протокола
	};
	//
	// параметры инициализации
	//
	//////////////////////////////////////////////////////////////////////////

	tERROR Init(InitParams_t* pInitParams);
	tERROR _Run();

private:
	InitParams_t m_InitParams;
	IContentItem* m_pContentItem;
	tBOOL m_bIsStarted;

public:
	const detect_code_t GetDCStatus() const {return m_dcResult;};
private:
	detect_code_t m_dcResult;
};

//tERROR DETECT(
//	IN tINT Order,
//	IN session_t* pSession,          // Контекст сессии
//	IN data_source_t dsDataSource,   // Источник данных
//	IN tVOID* pLastReceivedData,     // Буфер с данными для оценки
//	IN tULONG ulLastReceivedData,    // Размер буфера
//	OUT detect_code_t* pdcDetectCode,// Результат детектирования протокола
//	OUT hOBJECT* phDetectorHandle    // Хендл протоколлера
//	);
//tERROR PROCESS(
//	IN tINT Order,
//	IN session_t* pSession,          // Контекст сессии
//	IN data_source_t dsDataSource,   // Источник данных
//	IN tVOID* pData,                 // Блок данных
//	IN tULONG ulDataSize             // Размер блока данных
//	);
//

template <typename ProtocolAnalyzer>
tERROR ProtocollerMsgReceive( const char *task_prefix, hOBJECT _this, tDWORD Order, tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len, tDWORD& m_dwSessionCount, hCRITICAL_SECTION m_hSessionCS, tTaskState m_state, ProtocolAnalyzer* = 0 )
{
	tERROR err = errOK;

	// Place your code here
	if ( p_msg_cls_id == pmcTRAFFIC_PROTOCOLLER )
	{
		if ( 
			p_msg_id == pmSESSION_START &&
			((cSerializable*)p_par_buf)->isBasedOn(session_t::eIID) &&
			((session_t*)p_par_buf)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
			((session_t*)p_par_buf)->dwSessionCtxCount >= Order
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// SESSION_START
			//
			if ( m_state == TASK_STATE_RUNNING )
			{
				CProtocolProcessor* pSessionData = new CProtocolProcessor();
				if ( pSessionData )
				{
					CProtocolProcessor::InitParams_t hInitParams;
					hInitParams.hParent = _this;
					hInitParams.pSession = (session_t*)p_par_buf;
					hInitParams.hTrafficMonitor = p_send_point;
					hInitParams.pAnalyzer = new ProtocolAnalyzer();
					hInitParams.hProtocolType = (protocoller_order_t)Order;
					if ( PR_SUCC( err = pSessionData->Init(&hInitParams) ))
					{
						((session_t*)p_par_buf)->pdwSessionCtx[Order] = pSessionData;
						InterlockedIncrement((PLONG)&m_dwSessionCount); //m_dwSessionCount++;
						PR_TRACE((0, prtIMPORTANT, "%s\tSession created (session count is %d)", task_prefix, m_dwSessionCount));
					}
					else
					{
						delete pSessionData;
						pSessionData = NULL;
					}
			}}

			//
			// SESSION_START
			//
			//////////////////////////////////////////////////////////////////////////
		}
		else if ( 
			p_msg_id == pmSESSION_STOP &&
			((cSerializable*)p_par_buf)->isBasedOn(session_t::eIID) &&
			((session_t*)p_par_buf)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
			((session_t*)p_par_buf)->dwSessionCtxCount >= Order
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// SESSION_STOP
			//
			cAutoCS _cs_(m_hSessionCS, true);
			CProtocolProcessor* pSessionData = (CProtocolProcessor*)((session_t*)p_par_buf)->pdwSessionCtx[Order];
			if ( pSessionData )
			{
				pSessionData->Stop();
				delete pSessionData;
				pSessionData = NULL;
				InterlockedDecrement((PLONG)&m_dwSessionCount); //m_dwSessionCount--;
				PR_TRACE((0, prtIMPORTANT, "%s\tSession stopped (session count is %d)", task_prefix, m_dwSessionCount));
			}
			else
			{
				PR_TRACE((0, prtIMPORTANT, "%s\tSession stopped (no data)", task_prefix));
			}
			((session_t*)p_par_buf)->pdwSessionCtx[Order] = NULL;
			//
			// SESSION_STOP
			//
			//////////////////////////////////////////////////////////////////////////
		}
		else if (
			p_msg_id == pmSESSION_PSEUDO_STOP &&
			((cSerializable*)p_par_buf)->isBasedOn(pseudo_stop_t::eIID) &&
			((pseudo_stop_t*)p_par_buf)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
			((pseudo_stop_t*)p_par_buf)->dwSessionCtxCount >= Order &&
			((pseudo_stop_t*)p_par_buf)->phProtocollerHandle &&
			*(((pseudo_stop_t*)p_par_buf)->phProtocollerHandle) == (hOBJECT)_this
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// SESSION_PSEUDO_STOP
			//
			cAutoCS _cs_(m_hSessionCS, true);
			CProtocolProcessor* pSessionData = (CProtocolProcessor*)((session_t*)p_par_buf)->pdwSessionCtx[Order];
			if ( pSessionData )
			{
				//////////////////////////////////////////////////////////////////////////
				//
				// Уведомляем GUI о выходе из защищенного коннекта
				//
				switch ( pSessionData->GetDCStatus() ) 
				{
				case TrafficProtocoller::dcProtocolDetected:
				case TrafficProtocoller::dcDataDetected:
					{
						cProtocollerAskBeforeStop AskAction;
						AskAction.m_strProfile   = AVP_SERVICE_TRAFFICMONITOR;
						AskAction.m_strTaskType  = AVP_TASK_TRAFFICMONITOR;
						AskAction.m_nAction      = pmSESSION_DISCONNECT;
						AskAction.m_dwClientPID  = ((session_t*)p_par_buf)->dwClientPID;
						AskAction.m_szServerName = ((session_t*)p_par_buf)->szServerName;
						AskAction.m_nServerPort  = ((session_t*)p_par_buf)->nServerPort;
						GetAppInfoByPid(AskAction.m_dwClientPID, AskAction.m_szImagePath);
						_this->sysSendMsg(pmc_EVENTS_IMPORTANT, 0, 0, &AskAction, SER_SENDMSG_PSIZE);
					}
					break;
				}
				//
				// Уведомляем GUI о выходе из защищенного коннекта
				//
				//////////////////////////////////////////////////////////////////////////
				pSessionData->Stop();
				delete pSessionData;
				pSessionData = NULL;
				InterlockedDecrement((PLONG)&m_dwSessionCount); //m_dwSessionCount--;
				PR_TRACE((0, prtIMPORTANT, "%s\tSession pseudo stopped (session count is %d)", task_prefix, m_dwSessionCount));
				if ( !m_dwSessionCount )
					err = errOK_DECIDED;
			}
			else
			{
				PR_TRACE((0, prtIMPORTANT, "%s\tSession pseudo stopped (no processor)", task_prefix));
			}
			((session_t*)p_par_buf)->pdwSessionCtx[Order] = NULL;
			//
			// SESSION_PSEUDO_STOP
			//
			//////////////////////////////////////////////////////////////////////////
		}
		else if ( 
			p_msg_id == pmDETECT &&
			((cSerializable*)p_par_buf)->isBasedOn(detect_t::eIID) &&
			((detect_t*)p_par_buf)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
			((detect_t*)p_par_buf)->dwSessionCtxCount >= Order
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// DETECT
			//
			cAutoCS _cs_(m_hSessionCS, false);
			detect_t* detect_data = (detect_t*)p_par_buf;
			CProtocolProcessor* pSessionData = ((CProtocolProcessor*)detect_data->pdwSessionCtx[Order]);
			if ( pSessionData )
			{
				*(detect_data->pdcDetectCode) = pSessionData->Detect( 
					detect_data->dsDataSource, 
					(tBYTE*)detect_data->pLastReceivedData, 
					detect_data->ulLastReceivedData 
					);
				*(detect_data->phDetectorHandle) = (hOBJECT)_this;
			}
			else
				err = errOBJECT_NOT_FOUND;

			if ( 
				PR_SUCC(err) && 
				detect_data->pdcDetectCode && 
				(
					*(detect_data->pdcDetectCode) == dcDataDetected ||
					*(detect_data->pdcDetectCode) == dcProtocolDetected
				))
				return errOK_DECIDED;
			else
				return errOK;
			//
			// DETECT
			//
			//////////////////////////////////////////////////////////////////////////
		}
		else if (
			p_msg_id == pmPROCESS &&
			((cSerializable*)p_par_buf)->isBasedOn(process_t::eIID) &&
			((process_t*)p_par_buf)->dwInterfaceVersion == TRAFFIC_PROTOCOLLER_INTERFACE_VERSION &&
			((process_t*)p_par_buf)->dwSessionCtxCount >= Order
			)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// PROCESS
			//
			cAutoCS _cs_(m_hSessionCS, false);
			process_t* process_data = (process_t*)p_par_buf;
			if (
				process_data->phDetectorHandle &&
				*(process_data->phDetectorHandle) == (hOBJECT)_this 
				)
			{
				CProtocolProcessor* pSessionData = ((CProtocolProcessor*)process_data->pdwSessionCtx[Order]);
				if ( pSessionData )
					err = pSessionData->Process(
						process_data->dsDataSource, 
						(tBYTE*)process_data->pData, 
						process_data->ulDataSize
						);
				if ( PR_FAIL(err) && process_data->psStatus  )
					*(process_data->psStatus) = psProcessFailed;
				else 
				{
					*(process_data->psStatus) = psKeepAlive;
					return errOK_DECIDED;
				}
			}
			else
			if ( process_data->psStatus )
				*(process_data->psStatus) = psProcessFailed;
			return errOK; 
			//
			// PROCESS
			//
			//////////////////////////////////////////////////////////////////////////
		}
	}

	return err;
}

#endif//_PROTOCOLPROCESSOR_H_
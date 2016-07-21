/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	TrafficProtocoller.h
*		\brief	Этот файл описывает интерфейс обмена данными между ТраффикМонитором (прокси) и протоколлерами
*		
*		\author Vitaly DVi Denisov
*		\date	24.01.2005 12:51:39
*		
*		Example:	
		Интерфейс предназначен для передачи данных в контексте одной сессии прокси.
		Сессия начинается в момент соединения приложения-клиента к серверу.
		Порядок работы:
		1. ProxySession: Создать объект структуры session_t m_hSessionCtx. Установить данные в этом объекте:
				tDWORD dwSessionCtx[po_last];
				memset(dwSessionCtx, 0, po_last*sizeof(tDWORD));
				dwSessionCtx[po_ProxySession] = (tDWORD)this;
				m_hSessionCtx.pdwSessionCtx = dwSessionCtx;
				m_hSessionCtx.dwSessionCtxCount = po_last;
		2. ProxySession: Оповестить протоколлеры о начале новой сессии:
				sysSendMsg( 
					pmcTRAFFIC_PROTOCOLLER, 
					pmSESSION_START, 
					0, 
					&m_hSessionCtx, 
					SER_SENDMSG_PSIZE 
					);
		3. Protocoller: Выделить память для данных, ассоциированных с данной сессией, 
		   и сохранить указатель в массиве m_hSessionCtx.pdwSessionCtx:
  				CSessionData* pSessionData = new CSessionData(this);
				if ( pSessionData )
					phSessionCtx->pdwSessionCtx[po_SMTP] = (tDWORD)pSessionData;
		4. ProxySession: При приеме данных от клиента или от сервера передавать эти данные протоколлерам 
		   для распознавания протокола:
				detect_t hDetectParams;
				hDetectParams.assign(m_hSessionCtx, false);
				hDetectParams.dsDataSource = dsDataSource;
				hDetectParams.pLastReceivedData = pLastReceivedData;
				hDetectParams.ulLastReceivedData = ulLastReceivedData;
				hDetectParams.pdcDetectCode = &dcDetectCode;
				hDetectParams.phDetectorHandle = &m_hProtocoller;
				err = m_InitParams.hParent->sysSendMsg( 
					pmcTRAFFIC_PROTOCOLLER, 
					pmDETECT, 
					0, 
					&hDetectParams, 
					SER_SENDMSG_PSIZE 
					);
				if ( PR_FAIL(err) ) 
				{
					dcDetectCode = dcNeedMoreInfo;
					m_hProtocoller = NULL;
				}
		5. Protocoller: Попытаться найти известную команду в переданном сессией буфере. 
		   При этом следует учитывать, что размер команды может превышать размер переданного буфера, и накапливать
		   эти буфера, "склеивая" их между собой. Подсказка: момент очистки буфера совпадает с моментом переключения
		   направления данных. Т.е. буфер потока данных "клиент->сервер" становится неинтересным для протоколлера в момент
		   прихода буфера из потока "сервер->клиент".
		   Кроме того, вряд ли меет смысл накапливание буфера размером больше, чем максимальныя длина известной 
		   команды протокола.
		   Если протоколлер однозначно определяет свой протокол, он должен 
		   установить: 
				*hDetectParams.pdcDetectCode = dcProtocolDetected;
				hDetectParams.phDetectorHandle = this;
				return errOK_DECIDED; //Этот код возврата прекращает опрос других протоколлеров.
		   Если протоколлер однозначно определяет команду начала передачи данных, он должен 
		   установить:
				*hDetectParams.pdcDetectCode = dcDataDetected;
				hDetectParams.phDetectorHandle = this;
				return errOK_DECIDED; //Этот код возврата прекращает опрос других протоколлеров.
		6. ProxySession: Если все протоколлеры вернули из сообщения pmDETECT 
				hDetectParams.pdcDetectCode == dcNeedMoreInfo, 
		   следует отослать оригинальный буфер данных, не производя над ним никаких действий.
		7. ProxySession: Если протоколлер вернул из сообщения pmDETECT 
				hDetectParams.pdcDetectCode == dcProtocolUnknown, 
		   можно (в целях оптимизации) исключить данный протоколлер из дальнейшей обработки 
		   (к сожалению, это не реализуемо в рамках функции sysSendMsg).
		8. ProxySession: Если протоколлер вернул из сообщения pmDETECT 
				hDetectParams.pdcDetectCode == dcProtocolDetected, 
		   можно (в целях оптимизации) исключить все остальные протоколлеры из дальнейшей обработки 
		   (к сожалению, это не реализуемо в рамках функции sysSendMsg).
		9. ProxySession: Если протоколлер вернул из сообщения pmDETECT 
				hDetectParams.pdcDetectCode == dcDataDetected, 
		   следует все следующие буфера отдавать данному протоколлеру на обработку:
				process_status_t psStatus;
				process_t hProcessParams;
				hProcessParams.assign(m_hSessionCtx, false);
				hProcessParams.dsDataSource = dsDataSource;
				hProcessParams.pData = pLastReceivedData;
				hProcessParams.ulDataSize = ulLastReceivedData;
				hProcessParams.phDetectorHandle = &m_hProtocoller;
				hProcessParams.psStatus = &psStatus;
				err = m_hProtocoller->sysSendMsg( 
					pmcTRAFFIC_PROTOCOLLER, 
					pmPROCESS, 
					0, 
					&hProcessParams, 
					SER_SENDMSG_PSIZE 
					);
				if ( psStatus == psProcessFailed ) 
				{
					detect_code_t& dcDetectCode = m_dcTraffic;
					dcDetectCode = dcNeedMoreInfo;
					m_hProtocoller = NULL;
					err = errOBJECT_NOT_FOUND;
				}
		10. Protocoller: Протоколлер накапливает в себе данные, приходящие в сообщении pmPROCESS,
		    и обрабатывает их. Во время накопления и обработки он может передавать любые данные
			в сессию (например, для поддержания неразрывной связи между клиентом и сервером)
			сообщением pmKEEP_ALIVE
				process_msg_t process_msg;
				process_msg.assign(pSession, false);
				process_msg.dsDataTarget = dsServer;
				process_msg.psStatus = psKeepAlive;
				process_msg.hData = pSessionData->m_Data;
				err = sysSendMsg( 
					pmcTRAFFIC_PROTOCOLLER, 
					pmKEEP_ALIVE, 
					0, 
					&process_msg, 
					SER_SENDMSG_PSIZE 
					);
		11. ProxySession: В момент прихода сообщения pmKEEP_ALIVE сессия передает полученные 
		    данные в указанном направлении:
				do
				{
					if ( PR_SUCC(err= ppmCommand->hData->SeekRead(&dwRead, pos, data, 1024)) && dwRead )
					{
						pos += dwRead;
						tINT nSend = send( to, data, dwRead, 0 );
					}
				}
				while( PR_SUCC(err) && dwRead  );
		12. Protocoller: По окончание обработки протоколлер должен передать обработанные данные в сессию:
				process_msg_t process_msg;
				process_msg.assign(pSession, false);
				process_msg.dsDataTarget = dsServer;
				process_msg.psStatus = psProcessFinished;
				process_msg.hData = pSessionData->m_Data;
				err = sysSendMsg( 
					pmcTRAFFIC_PROTOCOLLER, 
					pmPROCESS_FINISHED, 
					0, 
					&process_msg, 
					SER_SENDMSG_PSIZE 
					);
		13. ProxySession: В момент прихода сообщения pmPROCESS_FINISHED сессия передает полученные 
		    данные в указанном направлении, и перестает перекачивать буфера данному протоколлеру на обработку:
				if ( ppmCommand->psStatus == psProcessFinished )
				{
					m_dcTraffic = dcNeedMoreInfo;
					m_hProtocoller = NULL;
				}
				do
				{
					if ( PR_SUCC(err= ppmCommand->hData->SeekRead(&dwRead, pos, data, 1024)) && dwRead )
					{
						pos += dwRead;
						tINT nSend = send( to, data, dwRead, 0 );
					}
				}
				while( PR_SUCC(err) && dwRead  );
		14. ProxySession: При разрыве соединени оповестить протоколлеры об окончании сессии:
				sysSendMsg( 
					pmcTRAFFIC_PROTOCOLLER, 
					pmSESSION_STOP, 
					0, 
					&m_hSessionCtx, 
					SER_SENDMSG_PSIZE 
					);
		15. Protocoller: Все протоколлеры должны очистить данные, связанные с закончившейся сессией:
				delete ((session_t*)p_par_buf)->pdwSessionCtx[po_SMTP];
				((session_t*)p_par_buf)->pdwSessionCtx[po_SMTP] = NULL;
*		
*		
*		
*/		



#ifndef _TRAFFICPROTOCOLLER_H_
#define _TRAFFICPROTOCOLLER_H_

#include <prague/prague.h>
#include <prague/pr_cpp.h>
#include <prague/pr_serializable.h>
#include <prague/iface/i_io.h>
#include <mail/plugin/p_trafficmonitor.h>
#include "trafficprotocollertypes.h"

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

#define TRAFFIC_PROTOCOLLER_INTERFACE_VERSION    1       //!< Версия интерфейса между прокси и протоколлерами

namespace TrafficProtocoller
{
	//! Здесь указаны порядковые номера всех протоколлеров, 
	//! которые могут работать на траффике.
	//! Эти номера необходимы для корректной работы с контекстами сессий
	//! в протоколлерах
	enum protocoller_order_t
	{
		po_ProxySession = 0,             //!< За этим номером закреплен контекст самой сессии
		po_SMTP,                         //!< SMTP
		po_POP3,                         //!< POP3
		po_IMAP4,                        //!< IMAP4
		po_NNTP,                         //!< NNTP
		po_HTTP,                         //!< HTTP
		po_FTP,                          //!< FTP
		po_MailWasher,                   //!< MailWasher (дополнительный протоколлер POP3)
		po_last,                          
	};

	#define pmcTRAFFIC_PROTOCOLLER 0x94d63274 //!< Класс сообщений, которыми обмениваются прокси и протоколлеры
	//////////////////////////////////////////////////////////////////////////
	//
	// Команды ProxySession=>Protocoller
	//
	#define pmSESSION_START			0x80388a31   //!< Начало сессии
	#define pmDETECT				0xa2a5a26a   //!< Запрос на детектирование команды
	#define pmDETECT_EX				0xfd21541    //!< Запрос на детектирование команды (при состоянии dcProtocolLikeDetected). Данные притормаживаются до смены состояния. При смене на dcDataDetected все накопленные данные передаются в pmPROCESS.
	#define pmPROCESS				0x79705be3   //!< Передача данных для обработки
	#define pmSESSION_STOP			0x220ac645   //!< Конец сессии
	#define pmSESSION_PSEUDO_STOP	0xf01b6b48   //!< Псевдостоп сессии (останавливается только тот протоколлер, от чьего имени послан запрос на псевдостоп)
	//
	// Команды ProxySession=>Protocoller
	//
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//
	// Команды Protocoller=>ProxySession
	//
	#define pmPROCESS_FINISHED		0x850eac8c		 //!< Сигнал об окончании обработки данных
	#define pmKEEP_ALIVE			0xa8bdd659		 //!< Команда на публикацию буфера данных
	#define pmSESSION_PSEUDO_STOP_REQUEST 0x2276f3ba //!< запрос на псевдостоп (остановится только тот протоколлер, от чьего имени послан запрос на псевдостоп)
	//
	// Команды Protocoller=>ProxySession
	//
	//////////////////////////////////////////////////////////////////////////

	//! Эта структура посылается в sysSendMsg 
	//! для создания (pmSESSION_START)
	//! и уничтожения (pmSESSION_STOP)
	//! контекста сессии в протоколлере
	struct session_t : public cSerializable
	{
		session_t():
			dwInterfaceVersion(TRAFFIC_PROTOCOLLER_INTERFACE_VERSION),
			pdwSessionCtx(NULL),
			dwSessionCtxCount(0),
			nServerPort(0),
			dwClientPID(0),
			dwClientRequest(0),
			bSSL(cFALSE)
			{}
	    DECLARE_IID(session_t, cSerializable, PID_TRAFFICMONITOR, 100) ;
		IN tDWORD dwInterfaceVersion;        //!< Версия интерфейса между прокси и протоколлерами
		IN OUT tPTR *pdwSessionCtx;          //!< Указатель на массив контекстов сессии (в самой сессии и в протоколлерах)
		IN tDWORD dwSessionCtxCount;         //!< Размер массива контекстов сессии в протоколлерах (д.б. = максимальному количеству протоколлеров)
		cStringObj szServerName;             //!< Имя сервера
		tINT nServerPort;                    //!< Порт сервера
		tDWORD dwClientPID;                  //!< PID процесса, инициировавшего соединение
		IN OUT tDWORD dwClientRequest;       //!< Список задач, обрабатывающих данные в этой сессии (флаги). Пока (19 май 2005) не используется
		tBOOL bSSL;                          //!< На этой сессии декодируется SSL
	};

	//! Эта структура посылается в sysSendMsg для детектирования протокола
	struct detect_t : public session_t
	{
		detect_t():
			dsDataSource(dsUnknown),
			pLastReceivedData(0),
			ulLastReceivedData(0),
			pdcDetectCode(0),
			phDetectorHandle(0),
			session_t()
			{}
	    DECLARE_IID(detect_t, session_t, PID_TRAFFICMONITOR, 101) ;
		IN data_source_t dsDataSource;   //!< Источник данных
		IN tVOID* pLastReceivedData;     //!< Буфер с данными для оценки
		IN tULONG ulLastReceivedData;    //!< Размер буфера
		OUT detect_code_t* pdcDetectCode;//!< Результат детектирования протокола
		OUT hOBJECT* phDetectorHandle;   //!< Хендл протоколлера
	};
	
	//! Эта структура посылается в sysSendMsg для расширенного детектирования протокола (данные задерживаются на время состояния dcProtocolLikeDetected)
	struct detect_ex_t : public detect_t
	{
		detect_ex_t():
			pDataBuffer(0),
			ulDataBuffer(0),
			detect_t()
			{}
	    DECLARE_IID(detect_ex_t, detect_t, PID_TRAFFICMONITOR, 102) ;
		IN tVOID* pDataBuffer;           //!< Полный буфер с данными для оценки (все данные, переданные в направлении dsDataSource в состоянии dcProtocolLikeDetected)
		IN tULONG ulDataBuffer;          //!< Размер полного буфера
	};

	enum process_status_t
	{
		psProcessFailed = -1,                   //!< Произошла ошибка при обработке. Отмена бработки
		psUnknown = 0,                          //!< Статус неизвестен
		psProcessFinished = pmPROCESS_FINISHED, //!< Обработка завершена
		psKeepAlive = pmKEEP_ALIVE,             //!< Обработка продолжается
		psCloseConnection,                      //!< Необходимо разорвать соединение
		ps_last,
	};
	//! Эта структура посылается в sysSendMsg для обработки данных
	struct process_t : public session_t
	{
		process_t():
			dsDataSource(dsUnknown),
			pData(0),
			ulDataSize(0),
			session_t()
			{
			}
	    DECLARE_IID(process_t, session_t, PID_TRAFFICMONITOR, 103) ;
		IN data_source_t dsDataSource;   //!< Источник данных
		IN tVOID* pData;                 //!< Блок данных
		IN tULONG ulDataSize;            //!< Размер блока данных
		IN hOBJECT* phDetectorHandle;    //!< Хендл протоколлера //TODO: Нужен только для процессирования через sysSendMsg
		OUT process_status_t* psStatus;  //!< Статус проверки
	};
	typedef data_source_t data_target_t;
	//! Эта структура принимается в MsgReceive для отсылки обработанных данных
	struct process_msg_t : public session_t
	{
		process_msg_t():
			dsDataTarget(dsUnknown),
			psStatus(psUnknown),
			hData(0),
			session_t()
			{
			}
	    DECLARE_IID(process_msg_t, session_t, PID_TRAFFICMONITOR, 104) ;
		IN data_target_t dsDataTarget;   //!< Приемщик данных
		IN process_status_t psStatus;    //!< Статус проверки
		IN hIO hData;                    //!< Данные для перекачки
	};

	//! Эта структура посылается в sysSendMsg для как сигнал на псевдо-остановку сессии (Остановиться должен только указанный протоколлер)
	struct pseudo_stop_t : public session_t
	{
		pseudo_stop_t():
			phProtocollerHandle(0),
			session_t()
			{}
	    DECLARE_IID(pseudo_stop_t, session_t, PID_TRAFFICMONITOR, 105) ;
		IN hOBJECT* phProtocollerHandle;	//!< Хендл протоколлера, запросившего остановку
	};

	//! Эта структура передается протоколлером в сторону задач-клиентов. Заинтересованная задача-клиент должна вернуть errOK_DECIDED. При возврате errNO_DECIDERS протоколлер должен оптимизировать свою работу, исключив процессирование
	struct anybody_here_t : public session_t
	{
		anybody_here_t():
			hProtocolType(po_ProxySession),
			session_t()
			{}
	    DECLARE_IID(anybody_here_t, session_t, PID_TRAFFICMONITOR, 106) ;
		protocoller_order_t hProtocolType;   //!< Тип протокола
		cStringObj szObjectName;             //!< Имя объекта (если доступно). Для HTTP - URL объекта
		cStringObj szObjectType;             //!< Type объекта (если доступно). Для HTTP - ContentType объекта
	};
}


#endif//_TRAFFICPROTOCOLLER_H_

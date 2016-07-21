// MailWasher.cpp: implementation of the CMailWasher class.
//
//////////////////////////////////////////////////////////////////////

#define PR_IMPEX_DEF

#pragma warning(disable:4786)
#include "MailWasher.h"
#include "MW_utils.h"
#include "lookahead.h"
#include "../POP3Protocoller/NOOPThread.h"
#include "DispatchProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMailWasher::CMailWasher(session_t* pSessionCtx):
	m_hSessionCtx(*pSessionCtx),
	m_bStarted(cFALSE),
	m_bHasBeenStarted(cFALSE),
	m_dcPOP3DetectCode(dcProtocolDetected),
	m_pDispatchProcess(NULL),
	m_dwFirstMessageNumber(0),
	m_retr_io(NULL),
	m_rCommand(recv_t::retr_no)
{
	HMODULE hMod = LoadLibrary("Kernel32.dll");
	*(void**)&fnProcessIdToSessionId = hMod ? GetProcAddress(hMod, "ProcessIdToSessionId") : NULL;
	m_hMessageRequest = CreateEvent(0,0,0,0);
	m_hMessageRemove  = CreateEvent(0,0,0,0);
	m_hCancelRequest  = CreateEvent(0,0,0,0);
	//////////////////////////////////////////////////////////////////////////
	//
	//	m_hSessionCtx - это копия *pSessionCtx
	//	Обнуляем указатели на другие протоколлеры в m_hSessionCtx,
	//	т.к. его мы будем использовать в дочернем потоке и не сможем 
	//	гарантировать корректное убиение контекстов чужих протоколлеров.
	//	Этим занимается трафикМонитор, а не майлВошер, в главном потоке.
	//
	//	P.S. Обнуление указателей приведет лишь к тому, что переключение 
	//	с протокола POP3 на любой другой протокол в одном соединении
	//	станет невозможным. Этим функционалом можно пожертвовать.
	//
	m_hSessionCtx.pdwSessionCtx = new tPTR[pSessionCtx->dwSessionCtxCount];
	if ( m_hSessionCtx.pdwSessionCtx )
	{
		memset(m_hSessionCtx.pdwSessionCtx, 0, m_hSessionCtx.dwSessionCtxCount*sizeof(tDWORD));
		for (tDWORD i = 0; i < m_hSessionCtx.dwSessionCtxCount; i++)
		{
			switch (i) 
			{
			case po_ProxySession:
			case po_POP3:
				m_hSessionCtx.pdwSessionCtx[i] = pSessionCtx->pdwSessionCtx[i];
				break;
			case po_MailWasher:
				m_hSessionCtx.pdwSessionCtx[i] = this;
				break;
			}
		}
	}
	else
		m_hSessionCtx.pdwSessionCtx = pSessionCtx->pdwSessionCtx;

}

CMailWasher::~CMailWasher()
{
	if ( m_hCancelRequest )
	{
		CloseHandle(m_hCancelRequest);
		m_hCancelRequest = NULL;
	}
	if ( m_hMessageRemove )
	{
		CloseHandle(m_hMessageRemove);
		m_hMessageRemove = NULL;
	}
	if ( m_hMessageRequest )
	{
		CloseHandle(m_hMessageRequest);
		m_hMessageRequest = NULL;
	}
	if ( m_pDispatchProcess )
	{
		delete m_pDispatchProcess;
		m_pDispatchProcess = NULL;
	}
	if ( m_retr_io )
	{
		m_retr_io->sysCloseObject();
		m_retr_io = NULL;
	}
	if ( 
		m_hSessionCtx.pdwSessionCtx &&
		m_hSessionCtx.pdwSessionCtx != m_InitParams.pdwSessionContext
		)
	{
		// Если m_hSessionCtx.pdwSessionCtx создан локально, удалим его
		delete []m_hSessionCtx.pdwSessionCtx;
		m_hSessionCtx.pdwSessionCtx = NULL;
		m_hSessionCtx.dwSessionCtxCount = 0;
	}
}

tERROR CMailWasher::Init(InitParams_t* pInitParams)
{
	if ( !pInitParams ) 
		return errNOT_OK;
	cERROR err = errOK;
	if ( PR_SUCC(err) ) 
		m_InitParams = *pInitParams;
	if ( PR_SUCC(err) ) 
	{
		m_hPOP3Session.m_dwPOP3Session = (tDWORD)&m_hSessionCtx;
		m_hPOP3Session.m_dwUserSession = 0;
		if ( fnProcessIdToSessionId )
			fnProcessIdToSessionId(m_hSessionCtx.dwClientPID, (DWORD*)&(m_hPOP3Session.m_dwUserSession));
	}
	if ( errOK_DECIDED != SessionNeeded() )
		return errNOT_OK;
	if ( PR_SUCC(err) ) 
		err = m_send.Init(m_InitParams.hParent);
	if ( PR_SUCC(err) ) 
		err = m_recv.Init(m_InitParams.hParent);
	return err;
}

tERROR CMailWasher::Send( data_target_t dsDataTarget, tCHAR* buf, tINT len, tBOOL bCloseConnection)
{
	cERROR err = errOK;
	PR_TRACE(( m_InitParams.hParent, prtNOTIFY, "mc\tCMailWasher::Send => %s", buf ));

	err = m_send.Clear();
	if ( PR_SUCC(err) )
	{
		auto_io_t auto_io(m_send);
		hIO io = auto_io.io();
		if ( io )
		{
			if ( PR_SUCC(err = io->SeekWrite(0, 0, buf, len)) )
				err = Translate(dsDataTarget, io, bCloseConnection);
		}
	}
//	if ( PR_SUCC(err) )
//		err = m_recv.Clear();
	
	return err;
}
tERROR CMailWasher::Translate( data_target_t dsDataTarget, hIO io, tBOOL bCloseConnection)
{
	process_msg_t process_msg;
	process_msg.assign(m_hSessionCtx, false);
	process_msg.dsDataTarget = dsDataTarget;
	process_msg.psStatus = bCloseConnection ? psCloseConnection : psKeepAlive; //bIsLastCommand ? psProcessFinished : psKeepAlive;
	process_msg.hData = io;
	return SAFE_sysSendMsg( 
		m_InitParams.hTrafficMonitor,
		pmcTRAFFIC_PROTOCOLLER, 
		bCloseConnection ? pmPROCESS_FINISHED : pmKEEP_ALIVE, 
		0, 
		&process_msg, 
		SER_SENDMSG_PSIZE 
		);
}

tERROR CMailWasher::Recv(data_source_t dsDataSource, tBOOL bRecvIsMultyLine)
{
	return m_recv.Recv(dsDataSource, bRecvIsMultyLine, m_hStopEvent);
}


tERROR CMailWasher::Receive(data_source_t dsDataSource, tCHAR* buf, tINT len)
{
	return m_recv.Receive(dsDataSource, buf, len, m_hStopEvent);
}

#define CHECK_STOP_EVENT() \
	if ( WAIT_TIMEOUT != WaitForSingleObject(m_hStopEvent, 0) ) \
	{ \
		PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: Stop event signalled")); \
		goto FINALIZE; \
	}

tERROR CMailWasher::_Run()
{
	PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: started"));
	cERROR err = errOK;
	cStringObj request;
	CNOOPThread hNoopClientThread;
	tDWORD dwFirstMessageNumber = m_dwFirstMessageNumber;
	hIO& retr_io = m_retr_io;
	HANDLE hMultiWait[3] = {m_hStopEvent, m_hMessageRequest, m_hMessageRemove};
	tDWORD dwRes = 0;
	recv_t::retr_t rCommand = m_rCommand;

	if ( !retr_io )
	{
		err = m_InitParams.hParent->sysCreateObjectQuick( (hOBJECT*)&retr_io, IID_IO, PID_TMPFILE );
		if ( PR_SUCC(err) )
		{
			PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: waiting for RETR..."));
			err = m_recv.WaitForRETR(
				dwFirstMessageNumber, 
				retr_io, 
				m_hSessionCtx, 
				m_InitParams.hTrafficMonitor, 
				m_InitParams.hPOP3Protocoller, 
				m_dcPOP3DetectCode, 
				m_hStopEvent,
				rCommand
				);
			if(rCommand == recv_t::retr_quit || PR_FAIL(err))
				goto FINALIZE;
			CHECK_STOP_EVENT();
			PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: RETR received"));
		}
	}

	if ( PR_SUCC(err) )
	{// Определяем номер первого затребованного письма
		m_hPOP3Session.m_dwFirstNewMessage = dwFirstMessageNumber;
		PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: First message number is %d", dwFirstMessageNumber));
	}
	if ( PR_SUCC(err) )
	{
		// Запускаем поток фигни в сторону клиента (имитируем перекачку письма)
		m_recv.Clear();
		request = "+OK\r\n";
		if ( PR_SUCC(Send( dsClient, request.c_str(cCP_ANSI), request.size() )))
			hNoopClientThread.Start(
				&m_hSessionCtx, 
				"X-Kaspersky: MailDispatcher\r\n", 
				strlen("X-Kaspersky: MailDispatcher\r\n"), 
				dsClient, 
				10000, 
				m_InitParams.hTrafficMonitor
				);
	}
	//////////////////////////////////////////////////////////////////////////
	//
	//	13.11.2006 -> Отключаем проверку CAPA, т.к. слишком много сервером ее не поддерживает.
	//	Будем отваливаться, если на первый TOP нам вернут -ERR
	//
	//	if ( PR_SUCC(err) )
	//	{
	//		// Узнаем, поддерживает ли сервер команду TOP
	//		m_recv.Clear();
	//		request = "CAPA\r\n";
	//		if ( PR_SUCC(Send( dsServer, request.c_str(cCP_ANSI), request.size() )))
	//			Recv( dsServer, cTRUE );
	//		if ( PR_SUCC(err) && !m_recv.IsOK() ) 
	//			err = errOBJECT_INCOMPATIBLE;
	//		if ( PR_SUCC(err) ) 
	//		{
	//			auto_io_t* auto_io = new auto_io_t(m_recv);
	//			if ( auto_io )
	//			{
	//				lookahead_stream* data = new lookahead_stream(1024); 
	//				if ( data )
	//				{
	//					data->open(auto_io->io());
	//					tQWORD size = data->get_size();
	//					tQWORD dwLineBegin = 0;
	//					tQWORD dwLineEnd = size;
	//					#define THERE_IS_(_s_)	FindPos(*data, dwLineBegin, dwLineEnd, _s_)
	//					if ( !(THERE_IS_("TOP\r\n") || THERE_IS_("top\r\n") || THERE_IS_("Top\r\n")) )
	//					{
	//						//	m_recv.Release();
	//						delete data;
	//						delete auto_io;
	//						goto FINALIZE;
	//					}
	//					delete data;
	//				}
	//				delete auto_io;
	//			}
	//		}
	//	}
	CHECK_STOP_EVENT();
	if ( PR_SUCC(err) )
	{
		// Узнаем количество писем на сервере и их размеры
		PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: Sending LIST"));
		m_recv.Clear();
		request = "LIST\r\n";
		if ( PR_SUCC(Send( dsServer, request.c_str(cCP_ANSI), request.size() )))
			Recv( dsServer, cTRUE );
		if ( PR_SUCC(err) )
		{
			err = ParseLIST(
				&m_hMessageList, 
				auto_io_t(m_recv).io(), 
				&m_hPOP3Session,
				&m_hPOP3Session.m_dwMessageCount, 
				&m_hPOP3Session.m_dwMailboxSize
				);
			PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: Account contains %d message(s), size is %d", m_hPOP3Session.m_dwMessageCount, m_hPOP3Session.m_dwMailboxSize));
		}
	}
	CHECK_STOP_EVENT();

	// Основной цикл (выход - по m_hStopEvent)
	if ( PR_SUCC(err) )
	{
		PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: Sending SessionBegin"));
		if ( errOK_DECIDED == SessionBegin() )
		{
			cERROR l_err = errOK;
			do
			{
				dwRes = WaitForMultipleObjects(3, hMultiWait, FALSE, 10000);				
				switch (dwRes) 
				{
				case WAIT_OBJECT_0 + 0:
					PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: Stop event signalled"));
					break;
				case WAIT_OBJECT_0 + 1: // m_hMessageRequest
					PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: SessionMessageRequestImpl..."));
					l_err = SessionMessageRequestImpl();
					PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: SessionMessageRequestImpl done (%terr)", l_err));
					break;
				case WAIT_OBJECT_0 + 2: // m_hMessageRemove
					PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: SessionMessageRemoveImpl..."));
					SessionMessageRemoveImpl();
					PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: SessionMessageRemoveImpl done"));
					break;
				default:
					// Поддерживаем контакт с сервером посредством NOOP'ов
					PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: Sending NOOP..."));
					m_recv.Clear();
					request = "NOOP\r\n";
					if ( PR_SUCC(Send( dsServer, request.c_str(cCP_ANSI), request.size() )))
						Recv( dsServer, cFALSE );
					PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: Sending NOOP done"));
//					// И - в сторону клиента
//					m_recv.Clear();
//					request = "X-Kaspersky: MailDispatcher\r\n";
//					Send( dsClient, request.c_str(cCP_ANSI), request.size() );
					break;
				}
			}
			while(PR_SUCC(l_err) && dwRes != WAIT_OBJECT_0);
		}
		PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: Sending SessionEnd"));
		SessionEnd();
	}
	
FINALIZE:
	m_recv.Clear();
//	if ( hNoopClientThread.IsStarted())
	if ( rCommand == recv_t::retr_yes )
	{
		tBOOL bMessageHasBeenDeleted = cFALSE; // Если данное письмо было удалено - выставить этот флаг!
		tBOOL bMessageHasBeenDownloaded = cFALSE; // Если данное письмо было полностью скачано - выставить этот флаг!
		cPOP3Message& hMessage = m_hMessageList[dwFirstMessageNumber];
		if ( m_hMessageList.size() > 0 && m_hMessageList[dwFirstMessageNumber].m_bDeleted )
			bMessageHasBeenDeleted = cTRUE;
		if ( m_hMessageList.size() > 0 && (m_hMessageList[dwFirstMessageNumber].m_dwSizeOf == -1) )
			bMessageHasBeenDownloaded = cTRUE;
		// Останавливаем поток фигни в сторону клиента
		err = hNoopClientThread.Stop();
		if ( retr_io )
		{
			if ( bMessageHasBeenDeleted )
			{
				// Если письмо БЫЛО нами удалено - заменяем текст письма на свой
				request = "";
				CreateReportLetter(bMessageHasBeenDownloaded, &hMessage.m_szData, request);
				Send(dsClient, request.c_str(cCP_ANSI), request.size(), cFALSE);
			}
			else if ( bMessageHasBeenDownloaded )
			{
				request = "";
				hMessage.m_szData.replace_all("\r\n.\r\n", 0, "\r\n..\r\n");
				request += hMessage.m_szData;
				request += "\r\n";
				request += ".";
				request += "\r\n";
				Send(dsClient, request.c_str(cCP_ANSI), request.size(), cFALSE);
			}
			else
			{
				// Запускаем, наконец, оригинальный запрос RETR в сторону сервера
				request = "X-Kaspersky: Original server data starting here: ";
				if ( PR_SUCC(Send(dsClient, request.c_str(cCP_ANSI), request.size(), cFALSE)))
				{
					{
						PRAutoLocker _cs_(m_cs);
						m_bStarted = cFALSE;
					}
					{
						// Отправляем на DETECT клиентский RETR, чтобы узнать хендл POP3Protocoller'а
						detect_code_t& dcDetectCode = m_dcPOP3DetectCode;
						hOBJECT hDetectorHandle = NULL;
						detect_t hDetectParams;
						hDetectParams.assign(m_hSessionCtx, false);
						hDetectParams.dsDataSource = dsClient;
						hDetectParams.pLastReceivedData = "RETR ";
						hDetectParams.ulLastReceivedData = 5;
						hDetectParams.pdcDetectCode = &dcDetectCode;
						hDetectParams.phDetectorHandle = &m_InitParams.hPOP3Protocoller;
						err = SAFE_sysSendMsg(m_InitParams.hParent, pmcTRAFFIC_PROTOCOLLER, pmDETECT, 0, &hDetectParams, SER_SENDMSG_PSIZE );
					}
				}
				// Если письмо не было нами удалено - отдаем его в траффикМонитор
				Translate(dsServer, retr_io, cFALSE);
			}
		}
	}

	if(retr_io)
	{
		retr_io->sysCloseObject();
		retr_io = NULL;
	}

	{
		PRAutoLocker _cs_(m_cs);
		m_bStarted = cFALSE;
	}

	PR_TRACE((m_InitParams.hParent, prtNOTIFY, "mc\tWasher thread: exiting"));

	return err;
}

tERROR CMailWasher::GetInitParams(InitParams_t* pInitParams)
{
	if ( pInitParams ) 
	{
		*pInitParams = m_InitParams;
		return errOK;
	}
	return errOBJECT_INCOMPATIBLE;
}

tBOOL CMailWasher::WaitForRETR(tBOOL bWaitOnce)
{
	// Ищем первый RETR в m_recv
	cERROR err = errOK;
	if ( !m_retr_io )
		err = m_InitParams.hParent->sysCreateObjectQuick( (hOBJECT*)&m_retr_io, IID_IO, PID_TMPFILE );
	if ( PR_SUCC(err) )
	{
		err = m_recv.WaitForRETR(
			m_dwFirstMessageNumber, 
			m_retr_io, 
			m_hSessionCtx, 
			m_InitParams.hTrafficMonitor, 
			m_InitParams.hPOP3Protocoller, 
			m_dcPOP3DetectCode, 
			m_hStopEvent,
			m_rCommand,
			bWaitOnce
			);
		if ( m_rCommand == recv_t::retr_yes )
			return cTRUE;
	}
	return cFALSE;
}

// удаляем на сервере письма помеченые на удаление
tDWORD CMailWasher::CollectMarkedLetters(vector<tDWORD>& letterIds)
{
	{
		PRAutoLocker _cs_(m_csMessageOperation);
		
		MessageList_t::const_iterator it		= m_hMessageList.begin();
		MessageList_t::const_iterator it_end	= m_hMessageList.end();

		for (; it != it_end; ++it)
		{
			const cPOP3Message& hPOP3Message = it->second;
			if (hPOP3Message.m_bDeleted)
				letterIds.push_back(hPOP3Message.m_dwNumber);
		}		
	}

	return letterIds.size();
}


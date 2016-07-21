#include "dispatchprocess.h"

#define	SOCKETBUFSIZE 0x2000

void ClearCallback( CSimpleBufferWindow* _this, tDWORD context, tVOID* data )
{
	// Отослать данные на детектирование
	tBYTE* pData = _this->View();
	tDWORD dwDataSize = _this->GetSize();
	if ( dwDataSize )
	{
		//TODO: На детект надо отправлять не текущие данные, а предыдущие (бывшие до переключения контекста)!
		detect_t hDetectParams;
		hDetectParams.assign(*((session_t*)data), false);
		hDetectParams.dsDataSource = ((ClearCallbackData*)data)->dsDataSource;
		hDetectParams.pLastReceivedData = pData;
		hDetectParams.ulLastReceivedData = dwDataSize;
		hDetectParams.pdcDetectCode = ((ClearCallbackData*)data)->pdcDetectCode;
		hDetectParams.phDetectorHandle = ((ClearCallbackData*)data)->phDetectorHandle;
		cERROR err = errOK;
		if ( !hDetectParams.phDetectorHandle || !*hDetectParams.phDetectorHandle )
			err = errOBJECT_BAD_PTR;
		if ( PR_SUCC(err) )
			err = (*hDetectParams.phDetectorHandle)->sysSendMsg( pmcTRAFFIC_PROTOCOLLER, pmDETECT, 0, &hDetectParams, SER_SENDMSG_PSIZE );
		if ( PR_FAIL(err) ) 
			*((ClearCallbackData*)data)->pdcDetectCode = dcNeedMoreInfo;
	}
}

void CreateReportLetter(IN tBOOL bFullMessage, IN cStringObj* szOriginalLetter, OUT cStringObj& szReportLetter)
{
	szOriginalLetter->replace_all("\r\n.\r\n", 0, "\r\n..\r\n");
	szReportLetter += "From: Kaspersky MailDispatcher\r\n";
	szReportLetter += "Subject: [!! SPAM] The message has been deleted\r\n";
	szReportLetter += CLocalTime::getRfc822Date().c_str();
	szReportLetter += "\r\n";
	szReportLetter += "Content-Type: multipart/mixed;\r\n";
	szReportLetter +=		"\tboundary=Kaspersky-MailDispatcher\r\n";
	szReportLetter += "MIME-Version: 1.0\r\n";
	
//	szReportLetter += "\r\n--Kaspersky-MailDispatcher\r\n";
//	szReportLetter += "Content-type: text/plain; \r\n";
//	szReportLetter +=	"\tcharset=us-ascii\r\n";
//	szReportLetter += "\r\n";
	szReportLetter += "The message has been deleted\r\n";
	
	szReportLetter += "\r\n--Kaspersky-MailDispatcher\r\n";
	szReportLetter += bFullMessage ? "Content-type: message/rfc822\r\n" :"Content-type: text/plain\r\n" ;
	szReportLetter += "Content-Transfer-Encoding: 8bit\r\n";
	szReportLetter += "\r\n";
	szReportLetter += (char*)szOriginalLetter->c_str(cCP_ANSI);
	szReportLetter += "\r\n";

	szReportLetter += "\r\n--Kaspersky-MailDispatcher--\r\n";
	szReportLetter += "\r\n";
	szReportLetter += ".";
	szReportLetter += "\r\n";
}

CDispatchProcess::CDispatchProcess(
	CMailWasher* pMailWasher,
	hOBJECT&   hParent,
	session_t& hSessionCtx,
	recv_t&    p_recv,
	send_t&    p_send,
	detect_code_t& dcPOP3DetectCode,
	hOBJECT&   hPOP3Protocoller
	):
	m_pMailWasher(pMailWasher),
	m_hParent(hParent),
	m_recv(p_recv),
	m_send(p_send),
	m_dcPOP3DetectCode(dcPOP3DetectCode),
	m_hPOP3Protocoller(hPOP3Protocoller),
	m_dsDataSource(dsUnknown),
	m_bWaitServerDeleResponse(false)
{
	m_ccdClient.assign(hSessionCtx, false);
	m_ccdClient.dsDataSource = dsClient; 
	m_ccdClient.pdcDetectCode = &m_dcPOP3DetectCode; 
	m_ccdClient.phDetectorHandle = &m_hPOP3Protocoller;
	m_ccdServer.assign(hSessionCtx, false);
	m_ccdServer.dsDataSource = dsServer; 
	m_ccdServer.pdcDetectCode = &m_dcPOP3DetectCode; 
	m_ccdServer.phDetectorHandle = &m_hPOP3Protocoller;
	m_pBuf_Client = new CSimpleBufferWindow(SOCKETBUFSIZE * 2, ClearCallback, (tDWORD)this, &m_ccdClient);
	m_pBuf_Server = new CSimpleBufferWindow(SOCKETBUFSIZE * 2, ClearCallback, (tDWORD)this, &m_ccdServer);
}

CDispatchProcess::~CDispatchProcess()
{
	if ( m_pBuf_Client )
	{
		delete m_pBuf_Client;
		m_pBuf_Client = NULL;
	}
	if ( m_pBuf_Server )
	{
		delete m_pBuf_Server;
		m_pBuf_Server = NULL;
	}
}

tERROR CDispatchProcess::DispatchProcess(process_t* pProcessParams)
{
	PR_TRACE((m_hParent, prtIMPORTANT, "mc\tCMailWasher: Dispatching data (ds is %s)...", GetDataSourceStr(pProcessParams->dsDataSource)));

	cERROR err = errOK;
	detect_code_t& dcDetectCode = m_dcPOP3DetectCode;
	hOBJECT& hProtocoller = m_hPOP3Protocoller;
	
	*(pProcessParams->psStatus) = psKeepAlive;

	//////////////////////////////////////////////////////////////////////////
	//
	//	1. Если идет сигнал RETR на удаленное мною письмо - не пропускать его! Отвечать самостоятельно "+OK\r\n"
	//	2. Если идет сигнал DELE на удаленное мною письмо - не пропускать его! Отвечать самостоятельно 
	//	3. Если идет сигнал LIST на удаленное мною письмо - не пропускать его! Отвечать самостоятельно 
	//	4. Если идет сигнал TOP на удаленное мною письмо - не пропускать его! Отвечать самостоятельно
	//	5. Любые другие сигналы можно пропускать
	//
	tBOOL bRecvIsDELE	= cFALSE;
	tBOOL bRecvIsRETR	= cFALSE;
	tBOOL bRecvIsList	= cFALSE;
	tBOOL bRecvIsTop	= cFALSE;
	tBOOL bRecvQuit		= cFALSE;
	tBOOL bFullClientCmd = cFALSE;

	{
		tBOOL bCommandHasBeenDeleted = cFALSE;
		data_source_t dsDataSource = pProcessParams->dsDataSource;
		if ( 
			dsDataSource == dsClient && 
			!m_recv.NeedMore("\r\n", 2) 
			)
		{
			bFullClientCmd = cTRUE;

			tDWORD dwDeleNumber = 0;
			tDWORD dwRetrNumber = 0;
			tDWORD dwListNumber = 0;
			tDWORD dwTopNumber = 0;

			bRecvIsDELE = m_recv.IsDELE(dwDeleNumber);
			bRecvIsRETR = m_recv.IsRETR(dwRetrNumber);
			bRecvIsList = m_recv.IsLIST(dwListNumber);
			bRecvIsTop	= m_recv.IsTOP(dwTopNumber);
			bRecvQuit	= m_recv.IsQUIT();

			cStringObj szReplacedData;
			cPOP3Message* pOperatedMessage = NULL;
			bCommandHasBeenDeleted = 
				(bRecvIsDELE && m_pMailWasher->HasBeenDeleted(dwDeleNumber, &pOperatedMessage)) ||
				(bRecvIsRETR && m_pMailWasher->HasBeenDeleted(dwRetrNumber, &pOperatedMessage)) ||
				(bRecvIsList && m_pMailWasher->HasBeenDeleted(dwListNumber, &pOperatedMessage)) ||
				(bRecvIsTop  && m_pMailWasher->HasBeenDeleted(dwTopNumber,  &pOperatedMessage))
				;
			tBOOL bCommandHasBeenDownloaded = 
				(bRecvIsRETR && m_pMailWasher->HasBeenDownloaded(dwRetrNumber, &pOperatedMessage))
				;
			if ( bRecvIsDELE && bCommandHasBeenDeleted)
			{
				// Удаляем письмо прямо сейчас, а не по "quit" чтобы в ответе на команду "list" были изменения
				szReplacedData.format(cCP_ANSI, "DELE %d\r\n", dwDeleNumber);
				m_pMailWasher->Send(dsServer, szReplacedData.c_str(cCP_ANSI), szReplacedData.length());
				//pOperatedMessage->m_bDeleted = cFALSE;
				m_recv.Clear();
				m_pBuf_Client->Clear(cFALSE);
				return errOK_DECIDED;
			}
			else if ( bRecvIsList && bCommandHasBeenDeleted)
			{
				// Я должен ответить "+OK N M\r\n" (где N - номер сообщения, M - его размер), чтобы клиент не порвал связь
				szReplacedData.format(cCP_ANSI, "+OK %d %d\r\n", dwListNumber, pOperatedMessage->m_dwSize);
				m_pMailWasher->Send(dsClient, szReplacedData.c_str(cCP_ANSI), szReplacedData.length());
				m_recv.Clear();
				m_pBuf_Client->Clear(cFALSE);
				return errOK_DECIDED;
			}
			else if ( bRecvIsTop && bCommandHasBeenDeleted)
			{
				// Я должен ответить, чтобы клиент не порвал связь
				szReplacedData = "+OK\r\n";
				CreateReportLetter(bCommandHasBeenDownloaded, &pOperatedMessage->m_szData, szReplacedData);
				m_pMailWasher->Send(dsClient, szReplacedData.c_str(cCP_ANSI), szReplacedData.length());
				m_recv.Clear();
				m_pBuf_Client->Clear(cFALSE);
				return errOK_DECIDED;
			}
			else if ( bRecvIsRETR && bCommandHasBeenDeleted )
			{
				// Таки нашелся дин клиент, который рвет транзакцию в случае, если ему ответят "-ERR". Это Outlook2003 ((
				// Придется всем отвечать "+OK\r\n"
				szReplacedData = "+OK\r\n";
				CreateReportLetter(bCommandHasBeenDownloaded, &pOperatedMessage->m_szData, szReplacedData);
				m_pMailWasher->Send(dsClient, szReplacedData.c_str(cCP_ANSI), szReplacedData.length());
				m_recv.Clear();
				m_pBuf_Client->Clear(cFALSE);
				return errOK_DECIDED;
			}
			else if ( bRecvIsRETR && bCommandHasBeenDownloaded )
			{
				// Отправляю уже скачанное письмо
				szReplacedData = "+OK\r\n";
				pOperatedMessage->m_szData.replace_all("\r\n.\r\n", 0, "\r\n..\r\n");
				szReplacedData += pOperatedMessage->m_szData;
				szReplacedData += "\r\n";
				szReplacedData += ".";
				szReplacedData += "\r\n";
				m_pMailWasher->Send(dsClient, szReplacedData.c_str(cCP_ANSI), szReplacedData.length());
				m_recv.Clear();
				m_pBuf_Client->Clear(cFALSE);
				return errOK_DECIDED;
			}
			else if (bRecvQuit)
			{
				// Пользователь выходит (удаляем все помеченые на удаление письма и отправляем quit серверу)

				// запрашиваем номера писем для удаления
				CollectMarkedLetters();

				// пытаемся поочередно удалять помеченые письма и в конце отправим QUIT
				m_bWaitServerDeleResponse = RemoveNextLetter();

				// теперь ждем от сервера ответа на наше удаление
				
				m_recv.Clear();
				m_pBuf_Client->Clear(cFALSE);
				return errOK_DECIDED;
			}
		}

		if (dsDataSource == dsServer && m_bWaitServerDeleResponse)
		{
			// получили ответ от сервера об удалении писем
			if (!m_recv.NeedMore("\r\n", 2))
			{
				// ответ полный, можно удалять следующее письмо если нужно
				m_bWaitServerDeleResponse = RemoveNextLetter();
				m_recv.Clear();
				m_pBuf_Server->Clear(cFALSE);
			}

			return errOK_DECIDED;
		}
	}
	//
	//	1. Если идет сигнал RETR на удаленное мною письмо - не пропускать его! Отвечать самостоятельно "+OK\r\n"
	//	2. Если идет сигнла DELE на удаленное мною письмо - не пропускать его! Отвечать самостоятельно "+OK\r\n"
	//	3. Любые другие сигналы можно пропускать
	//
	//////////////////////////////////////////////////////////////////////////

	// Эмулируем поведение ReadWriteSocket
	{
		if ( pProcessParams->dsDataSource == dsClient )
		{
			m_pBuf_Client->Insert((tBYTE*)pProcessParams->pData, pProcessParams->ulDataSize, cFALSE);
			if ( m_dsDataSource != pProcessParams->dsDataSource )
			{
				// Распечатываем содержимое окна сервера, и обнуляем его
				m_dsDataSource = pProcessParams->dsDataSource;
				m_pBuf_Server->Clear();
			}
		}
		else if (  pProcessParams->dsDataSource == dsServer )
		{
			m_pBuf_Server->Insert((tBYTE*)pProcessParams->pData, pProcessParams->ulDataSize, cFALSE);
			if ( m_dsDataSource != pProcessParams->dsDataSource )
			{
				// Распечатываем содержимое окна клиента, и обнуляем его
				m_dsDataSource = pProcessParams->dsDataSource;
				m_pBuf_Client->Clear();
			}
		}

		if ( dcDetectCode != dcDataDetected )
		{
			// данные серверу (команды клиента) можно посылать только если они полные
			if ((dsClient == pProcessParams->dsDataSource) && bFullClientCmd)
			{
				// передаем команду на сервер
				// как-только в ответ от сервера пойдут данные, клиентский буфер уйдет на детектирование
				// и возможно выставиться dcDataDetected
				m_pMailWasher->Translate(dsServer, auto_io_t(m_recv).io(), cFALSE);
			}
			else if (dsServer == pProcessParams->dsDataSource)
			{
				m_pMailWasher->Translate(dsClient, auto_io_t(m_recv).io(), cFALSE);
			}
		}
		else
		{
			// задетектились данные
			// Посылаем данные протоколлеру
			m_pBuf_Server->Clear(cFALSE);
			m_pBuf_Client->Clear(cFALSE);
			process_t hProcessParams;
			hProcessParams.assign(*pProcessParams, false);
			hProcessParams.phDetectorHandle = &hProtocoller;
			err = hProtocoller->sysSendMsg( pmcTRAFFIC_PROTOCOLLER, pmPROCESS, 0, &hProcessParams, SER_SENDMSG_PSIZE );
			if ( *(hProcessParams.psStatus) == psProcessFailed ) 
				dcDetectCode = dcNeedMoreInfo;
			else if(dcDetectCode == dcNeedMoreInfo && bRecvIsRETR)
				dcDetectCode = dcDataDetected;
		}

		if (!m_recv.NeedMore("\r\n", 2) || dsServer == pProcessParams->dsDataSource)
		{
			// теперь можно очищать
			m_recv.Clear();
		}
	}
	

	*(pProcessParams->psStatus) = psKeepAlive;

	PR_TRACE((m_hParent, prtIMPORTANT, "mc\tCMailWasher: Dispatching data done"));

	return errOK_DECIDED;
}

tERROR CDispatchProcess::DispatchProtocollerMessage(process_msg_t* ppmCommand)
{
	cERROR err = errOK;
	detect_code_t& dcDetectCode = m_dcPOP3DetectCode;
	PR_TRACE(( m_hParent, prtNOTIFY, "mc\tCMailWasher::DispatchProtocollerMessage => " ));
	if ( 
		ppmCommand->psStatus == psProcessFinished ||
		ppmCommand->psStatus == psProcessFailed ||
		ppmCommand->psStatus == psCloseConnection
		)
	{
		PR_TRACE(( m_hParent, prtNOTIFY, "mc\tCMailWasher::DispatchProtocollerMessage => psProcessFinished" ));
		dcDetectCode = dcNeedMoreInfo;
	}
	err = m_pMailWasher->Translate(ppmCommand->dsDataTarget, ppmCommand->hData, (ppmCommand->psStatus == psCloseConnection));

	return err;
}

tERROR CMailWasher::DispatchProcess(process_t* pProcessParams)
{
	if ( !m_pDispatchProcess )
		m_pDispatchProcess = new CDispatchProcess(
			this,
			m_InitParams.hParent,
			m_hSessionCtx,
			m_recv,
			m_send,
			m_dcPOP3DetectCode,
			m_InitParams.hPOP3Protocoller
			);
	if ( m_pDispatchProcess )
		return m_pDispatchProcess->DispatchProcess(pProcessParams);
	return errOBJECT_NOT_CREATED;
}

tERROR CMailWasher::DispatchProtocollerMessage(process_msg_t* ppmCommand)
{
	if ( !m_pDispatchProcess )
		m_pDispatchProcess = new CDispatchProcess(
			this,
			m_InitParams.hParent,
			m_hSessionCtx,
			m_recv,
			m_send,
			m_dcPOP3DetectCode,
			m_InitParams.hPOP3Protocoller
			);
	if ( m_pDispatchProcess )
		return m_pDispatchProcess->DispatchProtocollerMessage(ppmCommand);
	return errOBJECT_NOT_CREATED;
}

tDWORD CDispatchProcess::CollectMarkedLetters()
{
	m_lettersIdToDelete.clear();
	return  m_pMailWasher->CollectMarkedLetters(m_lettersIdToDelete);
}

bool CDispatchProcess::RemoveNextLetter()
{
	bool bSuccessSend = false;
	while (!m_lettersIdToDelete.empty() && !bSuccessSend)
	{
		cStringObj request;
		request.format(cCP_ANSI, "DELE %d\r\n", m_lettersIdToDelete.back());

		if (PR_SUCC(m_pMailWasher->Send(dsServer, request.c_str(cCP_ANSI), request.length())))
		{
			bSuccessSend = true;
			m_bWaitServerDeleResponse = true;
		}
		
		m_lettersIdToDelete.erase(m_lettersIdToDelete.end() - 1);
	}

	if (!bSuccessSend)
	{
		// если не получилось отправить ни одной команды
		// или все помеченые письма уже удалены просто посылаем QUIT
		cStringObj request;
		request.format(cCP_ANSI, "QUIT\r\n");
		m_pMailWasher->Send(dsServer, request.c_str(cCP_ANSI), request.length());
	}

	// теперь ждем от сервера ответа на наше удаление

	return bSuccessSend;
}
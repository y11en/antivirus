#pragma warning(disable:4786)
#include "mailwasher.h"

#include <Prague/pr_oid.h>

#include <Extract/iface/i_mailmsg.h>
#include <Extract/plugin/p_msoe.h>

#include <Mail/iface/i_antispamfilter.h>
#include <Mail/structs/s_mc.h>

#include "../MIMETransport.h"
#include <string>

tERROR CMailWasher::SessionNeeded()
{
	// Запрос новой POP3-сессии майлвошера: майлвошер посылает сообщение => GUI отвечает, стоит ли ему создавать новую сессию
	return m_InitParams.hParent->sysSendMsg(
		pmc_MAIL_WASHER, 
		pm_SESSION_NEEDED, 
		NULL, 
		&m_hPOP3Session,		
		SER_SENDMSG_PSIZE
	);
}
tERROR CMailWasher::SessionBegin()
{
	// Начало новой POP3-сессии майлвошера: майлвошер посылает сообщение => GUI отрисовывает окно с пустым списком
	return m_InitParams.hParent->sysSendMsg(
		pmc_MAIL_WASHER, 
		pm_SESSION_BEGIN, 
		NULL, 
		&m_hPOP3Session,		
		SER_SENDMSG_PSIZE
	);
}
tERROR CMailWasher::SessionEnd()
{
	// Конец POP3-сессии майлвошера: может посылать как майлвошер, так и GUI
	return m_InitParams.hParent->sysSendMsg(
		pmc_MAIL_WASHER, 
		pm_SESSION_END, 
		NULL, 
		&m_hPOP3Session,		
		SER_SENDMSG_PSIZE
	);
}
tERROR CMailWasher::SessionMessageFound(cSerializable* pPOP3Message)
{
	// Обнаружено новое сообщение на сервере: майлвошер посылает сообщение => GUI показывает новую строчку в списке
	return m_InitParams.hParent->sysSendMsg(
		pmc_MAIL_WASHER, 
		pm_MESSAGE_FOUND, 
		NULL, 
		pPOP3Message, 
		pPOP3Message ? SER_SENDMSG_PSIZE : NULL
	);
}
tERROR CMailWasher::SessionMessageDeleted(cSerializable* pPOP3Message)
{
	// Письмо удалено с сервера: майлвошер посылает сообщение => GUI отмечает соответствующую строчку в списке
	return m_InitParams.hParent->sysSendMsg(
		pmc_MAIL_WASHER, 
		pm_MESSAGE_DELETED, 
		NULL, 
		pPOP3Message, 
		pPOP3Message ? SER_SENDMSG_PSIZE : NULL
	);
}

tERROR CMailWasher::SessionMessageRequest(cPOP3MessageReguest* pPOP3MessageReguest)
{
	PRAutoLocker _cs_(m_csMessageOperation);
	m_hPOP3MessageReguest = *pPOP3MessageReguest;
	SetEvent(m_hMessageRequest);
	return errOK;
}
tERROR CMailWasher::SessionMessageRequestImpl()
{
	cERROR err = errOK;
	HANDLE hMultyEvent[2] = {m_hCancelRequest, m_hStopEvent};
	tDWORD dwMessagesCount = 0;
	tDWORD dwWhiteCounter = 0;
	tDWORD dwHiBound = 0;
	tDWORD dwLoBound = 0;
	{
		PRAutoLocker _cs_(m_csMessageOperation);
		dwMessagesCount = m_hMessageList.size();
		dwHiBound = m_hPOP3MessageReguest.m_dwBegin >= dwMessagesCount ? 0 : dwMessagesCount - m_hPOP3MessageReguest.m_dwBegin;
		dwLoBound = m_hPOP3MessageReguest.m_dwCount >= dwHiBound ? 0 : dwHiBound - m_hPOP3MessageReguest.m_dwCount;
	}

	// используем итератор, т.к. ключи могут не идти последовательно
	// и можем зависнуть при попытке получения отсутствующего сообщения
	MessageList_t::reverse_iterator it		= m_hMessageList.rbegin();
	MessageList_t::reverse_iterator it_end	= m_hMessageList.rend();
	
	// необходимо попасть на элемент dwHiBound
	size_t	counter = m_hMessageList.size() - dwHiBound;
	for (size_t index = 0; (it != it_end) && (index < counter); ++it, ++index)
	{
	}

	for ( tDWORD dwCounter = dwHiBound; dwCounter > dwLoBound; dwCounter--, ++it)
	{
		cPOP3Message hPOP3Message;
		tDWORD		 dwMailId = 0;
		{
			PRAutoLocker _cs_(m_csMessageOperation);
			hPOP3Message	= it->second; //m_hMessageList[dwCounter];
			dwMailId		= it->first;
		}

		// если сообщение непусто и оно либо выкачано полностью либо более чем достаточно
		// не вычитываем его повторно
		if ( !hPOP3Message.m_szData.empty() &&
			((-1 == hPOP3Message.m_dwSizeOf) ||	(hPOP3Message.m_dwSizeOf > m_hPOP3MessageReguest.m_dwSizeOf))
			)
		{
			err = SessionMessageFound(&hPOP3Message);
			continue;
		}

		cStringObj request; 
		if ( 
			( m_hPOP3MessageReguest.m_dwSizeOf < 0 ) ||
			( (tDWORD)m_hPOP3MessageReguest.m_dwSizeOf >= hPOP3Message.m_dwSize / 80 )
			)
		{
			request.format(cCP_ANSI, "RETR %d\r\n", dwMailId);
			hPOP3Message.m_dwSizeOf = -1;
		}
		else
		{
			request.format(cCP_ANSI, "TOP %d %d\r\n", dwMailId, m_hPOP3MessageReguest.m_dwSizeOf);
			hPOP3Message.m_dwSizeOf = m_hPOP3MessageReguest.m_dwSizeOf;
		}
		m_recv.Clear();
		err = Send( dsServer, request.c_str(cCP_ANSI), request.size() );
		if ( PR_SUCC(err) )
		{
			// сервер может ответить как -ERR single line response
			// так и +OK multi line response
			
			// попробуем сначала получить первую линию ответа
			err = Recv(dsServer, cFALSE);

			if (m_recv.IsOK())
				err = Recv( dsServer, cTRUE );
		}
		if ( PR_SUCC(err) )
		{
			if ( !m_recv.IsOK() && (dwCounter == dwHiBound) )
			{
				// Если на первый же TOP нам вернули "-ERR", считаем, что сервер не знает эту команду
				// Автоматом закрываем окно диспетчера (для этого выставляем dwWhiteCounter = dwMessagesCount)
				dwWhiteCounter = dwMessagesCount;
				err = errOBJECT_INCOMPATIBLE;
				break;
			}
			{
				{
					// Вынимаем имя письма
					cERROR err = errOK;
					hOS hMessageOS = NULL;
					auto_io_t auto_io(m_recv);
					hIO io_to_recv = auto_io.io();
					hIO message_io = NULL;
					if ( PR_SUCC(io_to_recv->sysCreateObjectQuick( (hOBJECT*)&message_io, IID_IO, PID_TMPFILE )))
						ReplaceCRLFDotDotToCRLFDot(io_to_recv, message_io);
					else
						message_io = io_to_recv;
					err = message_io->set_pgOBJECT_ORIGIN(OID_MAIL_MSG_MIME);
					if (PR_SUCC(err))
						err = message_io->sysCreateObjectQuick( (hOBJECT*)&hMessageOS, IID_OS, PID_MSOE );
					if ( PR_SUCC(err) )
					{
						tCHAR data[1025];
						tDWORD dwReadPos = 0;
						tDWORD dwRead = 0;
						while(1) 
						{
							err = message_io->SeekRead(&dwRead, dwReadPos, data, 1024);
							if(PR_FAIL(err) || !dwRead)
								break;
							dwReadPos += dwRead;
							data[dwRead] = 0;
							hPOP3Message.m_szData += data;
						}
						if (PR_SUCC(err))
							err = hPOP3Message.m_szFrom.assign(    hMessageOS, pgMESSAGE_FROM);
						if (PR_SUCC(err))
							err = hPOP3Message.m_szTo.assign(      hMessageOS, pgMESSAGE_TO);
						if (PR_SUCC(err))
							err = hPOP3Message.m_szSubject.assign( hMessageOS, pgMESSAGE_SUBJECT);
						if (PR_SUCC(err))
							err = hPOP3Message.m_szDateTime.assign(hMessageOS, pgMESSAGE_DATE); 
						if (PR_SUCC(err))
							err = hPOP3Message.m_szName.assign(    hMessageOS, pgOBJECT_NAME); 
						//////////////////////////////////////////////////////////////////////////
						//
						// Спросим у антиспама
						//
						if (PR_SUCC(err))
						{
							cMCProcessParams params;
							hOBJECT send_point = /*m_hTrafficMonitor ? m_hTrafficMonitor : */m_InitParams.hTrafficMonitor;
							tPROPID g_propMessageIsIncoming = NULL;
							tPROPID g_propMailerPID = NULL;
							tPROPID g_propMessageCheckOnly = NULL;
							tPROPID g_propMessageUncomplete = NULL;

							if (!g_propMessageIsIncoming) g_root->RegisterCustomPropId(
								&g_propMessageIsIncoming, 
								npMESSAGE_IS_INCOMING, 
								pTYPE_BOOL | pCUSTOM_HERITABLE
								);
							if (!g_propMailerPID) g_root->RegisterCustomPropId(
								&g_propMailerPID, 
								npMAILER_PID, 
								pTYPE_DWORD | pCUSTOM_HERITABLE
								);
							if (!g_propMessageCheckOnly) g_root->RegisterCustomPropId(
								&g_propMessageCheckOnly,
								npMESSAGE_CHECK_ONLY,
								pTYPE_DWORD | pCUSTOM_HERITABLE
								);
							if (!g_propMessageUncomplete) g_root->RegisterCustomPropId(
								&g_propMessageUncomplete,
								npMESSAGE_UNCOMPLETE,
								pTYPE_DWORD | pCUSTOM_HERITABLE
								);
							hMessageOS->propSetBool(g_propMessageIsIncoming, cTRUE);
							hMessageOS->propSetBool(g_propMessageCheckOnly, cTRUE);
							hMessageOS->propSetDWord(g_propMessageUncomplete, hPOP3Message.m_dwSizeOf >= 0);
							hMessageOS->propSetDWord(g_propMailerPID, m_hSessionCtx.dwClientPID);
							send_point->sysSendMsg( 
								(hPOP3Message.m_dwSizeOf == 0) ? pmc_ANTISPAM_PROCESS : pmc_MAILTASK_PROCESS, 
								NULL, 
								hMessageOS, 
								&params, 
								SER_SENDMSG_PSIZE
								);
							if ( params.p_MailCheckerVerdicts & mcv_MESSAGE_CHECKED )
							{
								if ( 
									(params.p_MailCheckerVerdicts & mcv_MESSAGE_AS_SPAM) ||
									(params.p_MailCheckerVerdicts & mcv_MESSAGE_AV_INFECTED) ||
									(params.p_MailCheckerVerdicts & mcv_MESSAGE_AV_WARNING) 
									)
									hPOP3Message.m_bMarkToDelete = cTRUE;
								hPOP3Message.m_dwReason = params.p_dwASReason;
								if ( 
									(params.p_MailCheckerVerdicts & mcv_MESSAGE_AV_INFECTED) ||
									(params.p_MailCheckerVerdicts & mcv_MESSAGE_AV_WARNING) 
									)
									hPOP3Message.m_szVirusname = params.p_szVirusName;
								switch ( hPOP3Message.m_dwReason )
								{
								case asdr_WhiteString:
								case asdr_WhiteAddress:
									dwWhiteCounter++;
								}
							}
						}
						//
						// Спросим у антиспама
						//
						//////////////////////////////////////////////////////////////////////////
						err = hMessageOS->sysCloseObject();
						if (PR_SUCC(err))
							err = SessionMessageFound(&hPOP3Message);
					}
					err = message_io->set_pgOBJECT_ORIGIN(0);
					if ( message_io != io_to_recv )
					{
						message_io->sysCloseObject();
						message_io = NULL;
					}
				}
				{
					PRAutoLocker _cs_(m_csMessageOperation);
					m_hMessageList[dwMailId] = hPOP3Message;
				}
			}
		}
		if ( WAIT_TIMEOUT != WaitForMultipleObjects(2, hMultyEvent, FALSE, 0) )
			break;
	}
	// Если все письма забираются одним махом, и все они белые - закрываем окно майлВошера автоматически.
	if ( dwWhiteCounter >= dwMessagesCount )
		SessionGUIFinish();
	else
	{
		// Оповещаем, что закончили обработку задания
		SessionMessageFound(&m_hPOP3Session);
	}
		
	return err;
}

tERROR CMailWasher::SessionMessageRemove(cPOP3MessageList* pPOP3MessageList)
{
	PRAutoLocker _cs_(m_csMessageOperation);
	m_hPOP3MessageListToRemove = *pPOP3MessageList;
	SetEvent(m_hMessageRemove);
	return errOK;
}
tERROR CMailWasher::SessionMessageRemoveImpl()
{
	cERROR err = errOK;
	HANDLE hMultyEvent[2] = {m_hCancelRequest, m_hStopEvent};
	tDWORD dwMessageCount = 0;
	{
		PRAutoLocker _cs_(m_csMessageOperation);
		dwMessageCount = m_hPOP3MessageListToRemove.m_POP3MessageList.count();
	}
	for ( tDWORD dwCounter = 0; dwCounter < dwMessageCount; dwCounter++ )
	{
		tDWORD dwNumber = 0;
		tBOOL bMarkToDelete = cFALSE;
		cPOP3Message hPOP3Message;
		{
			PRAutoLocker _cs_(m_csMessageOperation);
			hPOP3Message = m_hPOP3MessageListToRemove.m_POP3MessageList.at(dwCounter);
			dwNumber = hPOP3Message.m_dwNumber;
			bMarkToDelete = hPOP3Message.m_bMarkToDelete;
		}
		if ( bMarkToDelete )
		{
			{
				PRAutoLocker _cs_(m_csMessageOperation);
				// Отмечаем в своем списке, что данное письмо помечено на удаление (hPOP3Message.m_bDeleted)
				hPOP3Message.m_bDeleted = cTRUE;
				m_hMessageList[hPOP3Message.m_dwNumber] = hPOP3Message;
			}
			err = SessionMessageDeleted(&hPOP3Message);

			if ( PR_FAIL(err) )
				break;
		}
		if ( WAIT_TIMEOUT != WaitForMultipleObjects(2, hMultyEvent, FALSE, 10) )
			break;
	}
	// Оповещаем, что закончили обработку задания
	SessionMessageDeleted(&m_hPOP3Session);
	return err;
}
tERROR CMailWasher::SessionGUIFinish()
{
	cERROR err = errOK;
	//err = Stop();

	if(m_hStopEvent && m_hThread)
	{
		SetEvent(m_hStopEvent);
	}

	return err;
}
tERROR CMailWasher::SessionCancelRequest()
{
	if ( m_hCancelRequest )
	{
		SetEvent(m_hCancelRequest);
		return errOK;
	}
	return errNOT_OK;
}

#define DECODE_MESSAGE(sz)	\
{	\
	std::string szDecodedText;	\
	if ( m_hDecoder.Decode(	\
		sz.c_str(cCP_ANSI),	\
		szDecodedText,	\
		pPOP3MessageToDecode->m_DecodeFrom.c_str(cCP_ANSI),	\
		pPOP3MessageToDecode->m_szCurrentEncode.c_str(cCP_ANSI)	\
		))	\
		sz = szDecodedText.c_str();	\
}

tERROR CMailWasher::SessionMessageDecode(cPOP3MessageDecode* pPOP3MessageToDecode)
{
	cPOP3Message hPOP3Message = m_hMessageList[pPOP3MessageToDecode->m_dwNumber];
	pPOP3MessageToDecode->assign(hPOP3Message, false);
	DECODE_MESSAGE(pPOP3MessageToDecode->m_szData);
	DECODE_MESSAGE(pPOP3MessageToDecode->m_szSubject);
	DECODE_MESSAGE(pPOP3MessageToDecode->m_szFrom);
	DECODE_MESSAGE(pPOP3MessageToDecode->m_szTo);
	DECODE_MESSAGE(pPOP3MessageToDecode->m_szName);
	DECODE_MESSAGE(pPOP3MessageToDecode->m_szDateTime);
	pPOP3MessageToDecode->m_szCurrentEncode = pPOP3MessageToDecode->m_DecodeFrom;
	return errOK;
}
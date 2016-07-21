#include "send_recv.h"

//////////////////////////////////////////////////////////////////////////
//
// send_t
//
send_t::send_t():
	m_dsDataSource(dsUnknown),
	m_io(NULL)
{
	Clear();
}
send_t::~send_t()
{
	if ( m_io )
	{
		PR_TRACE(( m_io, prtERROR, "mc\tsend_t::~send_t => MailWasher is going to destroy TempIO" ));
		m_io->sysCloseObject();
		m_io = NULL;
	}
	m_dsDataSource = dsUnknown;
}

tERROR send_t::Init(hOBJECT hParent)
{
	PRAutoLocker _cs_(m_cs);
	if ( !m_io )
		return hParent->sysCreateObjectQuick( (hOBJECT*)&m_io, IID_IO, PID_TMPFILE );
	else
		return errOBJECT_ALREADY_EXISTS;
}

tERROR send_t::Clear()
{
	PRAutoLocker _cs_(m_cs);
	m_dsDataSource = dsUnknown;
	if ( m_io )
		return m_io->SetSize(0);
	else
		return errOK;
}

hIO send_t::Get(OUT data_source_t* pdsDataSource)
{
	if ( m_io ) m_cs.lock();
	if ( pdsDataSource ) *pdsDataSource = m_dsDataSource;
	return m_io;
}

void send_t::Release()
{
	if ( m_io ) m_cs.unlock();
}
//
// send_t
//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
// recv_t
//
recv_t::recv_t(): send_t()
{
}

recv_t::~recv_t()
{
}

tBOOL recv_t::Is(tCHAR* szCommand, tINT nCommandSize)
{
	PRAutoLocker _cs_(m_cs);
	lookahead_stream data(nCommandSize); data.open(m_io);
	if ( data.get_size() >= nCommandSize )
		if ( data.compare(szCommand, nCommandSize) )
		{
			PR_TRACE(( m_io, prtNOTIFY, "mc\trecv_t::Is(%s) => cTRUE", szCommand ));
			return cTRUE;
		}
	PR_TRACE(( m_io, prtNOTIFY, "mc\trecv_t::Is(%s) => cFALSE", szCommand ));
	return cFALSE;
}
tBOOL recv_t::IsOK()
{
	PRAutoLocker _cs_(m_cs);
	if ( m_dsDataSource == dsClient )
		return cTRUE;
	return Is("+OK", 3);
}
tBOOL recv_t::IsRETR()
{
	PRAutoLocker _cs_(m_cs);
	if ( m_dsDataSource == dsServer )
		return cFALSE;
	return ( Is("RETR", 4) || Is("retr", 4) );
}

#define DEF_IS_POP3COMMAND(_command, szCommandH, szCommandL, dwSize, dwMessageNumber)	\
tBOOL recv_t::Is##_command(OUT tDWORD& dwMessageNumber)	\
{	\
	PRAutoLocker _cs_(m_cs);	\
	if ( m_dsDataSource == dsServer )	\
		return cFALSE;	\
	if ( Is(szCommandH, dwSize) || Is(szCommandL, dwSize) )	\
	{/* Определяем номер затребованного письма */	\
		lookahead_stream data(4); data.open(m_io);	\
		tQWORD dwLineBegin = 0;	\
		tQWORD dwLineEnd = data.get_size();	\
		if ( FindPos(data, dwLineBegin, dwLineEnd, "\r\n") )	\
			if ( PR_SUCC(GetIntLineParameter(1, data, dwLineBegin, dwLineEnd, dwMessageNumber)))	\
				return cTRUE;	\
	}	\
	return cFALSE;	\
}

DEF_IS_POP3COMMAND(RETR, "RETR", "retr", 4, dwMessageNumber);
DEF_IS_POP3COMMAND(DELE, "DELE", "dele", 4, dwMessageNumber);
DEF_IS_POP3COMMAND(LIST, "LIST", "list", 4, dwMessageNumber);
DEF_IS_POP3COMMAND(TOP,  "TOP",  "top",  3, dwMessageNumber);

tBOOL recv_t::IsQUIT()
{
	PRAutoLocker _cs_(m_cs);
	if ( m_dsDataSource == dsServer )
		return cFALSE;
	return ( Is("QUIT", 4) || Is("quit", 4) );
}

tBOOL recv_t::NeedMore(tCHAR* end, tDWORD size)
{
	PRAutoLocker _cs_(m_cs);
	lookahead_stream data(1024); data.open(m_io);
	data.seek(data.get_size()-size);
	tBOOL bRet = !data.compare(end, size);
	return bRet;
}


tERROR recv_t::Receive(data_source_t dsDataSource, tCHAR* buf, tINT len, HANDLE hStopEvent)
{
	cERROR err = errOK;
	tQWORD qwSize = 0;

	PR_TRACE(( m_io, prtNOTIFY, "mc\trecv_t::Receive => enter (%s)...", GetDataSourceStr(dsDataSource)));
	//////////////////////////////////////////////////////////////////////////
	//
	// Синхронизируем запись/чтение данных в m_recv
	//
	data_source_t ds = dsUnknown;
	{
		PRAutoLocker _cs_(m_cs);
		ds = m_dsDataSource;
	}
	PR_TRACE(( m_io, prtNOTIFY, "mc\trecv_t::Receive => current m_dsDataSource is %s", GetDataSourceStr(ds) ));
	while ( ds != dsDataSource && ds != dsUnknown )
	{	
		// Ждем, пока прочитаются эти данные в другом потоке
		if ( WAIT_TIMEOUT != WaitForSingleObject(hStopEvent, 0) )
			break;
		{
			PRAutoLocker _cs_(m_cs);
			ds = m_dsDataSource;
			PR_TRACE(( m_io, prtNOTIFY, "mc\trecv_t::Receive => waiting for dsDataSource (%s)...", GetDataSourceStr(ds) ));
		}
	}
	//
	// Синхронизируем запись/чтение данных в m_recv
	//
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//
	// Записываем данные в m_io
	//
	{
		PRAutoLocker _cs_(m_cs);
		m_dsDataSource = dsDataSource;
		if ( PR_SUCC(err) )
			err = m_io->GetSize(&qwSize, IO_SIZE_TYPE_EXPLICIT);
		if ( PR_SUCC(err) )
			err = m_io->SeekWrite( 0, qwSize, buf, len );
		
		PR_TRACE((m_io, prtNOTIFY, "mc\trecv_t::Receive => Data of size %d written (%terr), m_dsDataSource is %s", len, err, GetDataSourceStr(ds)));
	}
	//
	// Записываем данные в m_io
	//
	//////////////////////////////////////////////////////////////////////////

	PR_TRACE(( m_io, prtNOTIFY, "mc\trecv_t::Receive => exit" ));
	return err;
}

tERROR recv_t::Recv(data_source_t dsDataSource, tBOOL bRecvIsMultyLine, HANDLE hStopEvent, tBOOL bRecvOnce)
{
	PR_TRACE(( m_io, prtNOTIFY, "mc\trecv_t::Recv(%s, %d) => enter...", GetDataSourceStr(dsDataSource), bRecvIsMultyLine ));
	tBOOL bIsOK = cFALSE;
	tQWORD qwSize = 0;
	cERROR err = errOK;

	{
		PRAutoLocker _cs_(m_cs);
		if(dsDataSource != dsUnknown && m_dsDataSource != dsUnknown && m_dsDataSource != dsDataSource)
			return errOBJECT_NOT_FOUND;
	}
	
	do
	{
		{
			PRAutoLocker _cs_(m_cs);
			err = m_io->GetSize(&qwSize, IO_SIZE_TYPE_EXPLICIT);
		}
		
		if (qwSize > 0)
		{
			PR_TRACE((m_io, prtNOTIFY, "mc\trecv_t::Recv => looking for \\r\\n"));
			tCHAR end_line[] = "\r\n";
			tCHAR end_block[] = "\r\n.\r\n";
			tDWORD end_size = 2;
			tCHAR* end_buf = end_line;
			if ( bRecvIsMultyLine )
			{
				end_buf = end_block;
				end_size = 5;
			}
			PR_TRACE(( m_io, prtNOTIFY, "mc\trecv_t::Recv => Waiting data..." ));
			while(NeedMore(end_buf, end_size))
			{
				if(bRecvOnce || WAIT_TIMEOUT != WaitForSingleObject(hStopEvent, 10))
					break;
			}
			PR_TRACE(( m_io, prtNOTIFY, "mc\trecv_t::Recv => Waiting data done" ));
		}
		else
		{
			if ( bRecvOnce || WAIT_TIMEOUT != WaitForSingleObject(hStopEvent, 0) )
				break;
		}

	}
	while(!qwSize);
	
	PR_TRACE(( m_io, prtNOTIFY, "mc\trecv_t::Recv => exit..." ));
	return err;
}

tERROR recv_t::Translate(session_t hSessionCtx, hOBJECT hTrafficMonitor, hOBJECT& hPOP3Protocoller, detect_code_t& dcPOP3DetectCode, HANDLE hStopEvent)
{	
	cERROR err = errOK;
	tBOOL bHasBeenSent = cFALSE;
	{
		PRAutoLocker _cs_(m_cs);
		tBYTE pLastReceivedData[129] = {0};	
		tDWORD ulLastReceivedData = 0;	
		tQWORD qwPos = 0;	
		while ( PR_SUCC(m_io->SeekRead(&ulLastReceivedData, qwPos, pLastReceivedData, 128)) && ulLastReceivedData)	
		{	
			detect_code_t& dcDetectCode = dcPOP3DetectCode;	
			hOBJECT hDetectorHandle = NULL;	
			detect_t hDetectParams;	
			hDetectParams.assign(hSessionCtx, false);	
			hDetectParams.dsDataSource = m_dsDataSource;	
			hDetectParams.pLastReceivedData = pLastReceivedData;	
			hDetectParams.ulLastReceivedData = ulLastReceivedData;	
			hDetectParams.pdcDetectCode = &dcDetectCode;	
			hDetectParams.phDetectorHandle = &hPOP3Protocoller;	
			if(hStopEvent && WaitForSingleObject(hStopEvent, 0) != WAIT_TIMEOUT)
			{
				err = errOBJECT_IS_CLOSING;
				break;
			}
			err = SAFE_sysSendMsg( hTrafficMonitor, pmcTRAFFIC_PROTOCOLLER, pmDETECT, 0, &hDetectParams, SER_SENDMSG_PSIZE );	
			memset(pLastReceivedData,0,129);	
			qwPos += ulLastReceivedData;	
			bHasBeenSent = cTRUE;
		}	
	}
	if ( bHasBeenSent )
	{
		hIO retr_io = NULL;
		if ( PR_SUCC(hTrafficMonitor->sysCreateObjectQuick( (hOBJECT*)&retr_io, IID_IO, PID_TMPFILE )))
		{
			data_source_t ds = dsUnknown;
			{
				PRAutoLocker _cs_(m_cs);
				err = IOCopy(m_io, retr_io);
				ds = m_dsDataSource;
				Clear();
			}
			if ( PR_SUCC(err))
			{
				process_msg_t process_msg;
				process_msg.assign(hSessionCtx, false);
				process_msg.dsDataTarget = ds == dsClient ? dsServer : dsClient;
				process_msg.psStatus = psKeepAlive; 
				process_msg.hData = retr_io;
				if(!hStopEvent || WaitForSingleObject(hStopEvent, 0) == WAIT_TIMEOUT)
					err = SAFE_sysSendMsg( 
						hTrafficMonitor,
						pmcTRAFFIC_PROTOCOLLER, 
						pmKEEP_ALIVE,
						0, 
						&process_msg, 
						SER_SENDMSG_PSIZE 
						);
				else
					err = errOBJECT_IS_CLOSING;
			}
			retr_io->sysCloseObject();
			retr_io = NULL;
		}
	}
	else if(hStopEvent && WaitForSingleObject(hStopEvent, 0) != WAIT_TIMEOUT)
		err = errOBJECT_IS_CLOSING;

	return err;
}

tERROR recv_t::WaitForRETR(
						   tDWORD& dwRetrNum, 
						   hIO& retr_io, 
						   session_t hSessionCtx, 
						   hOBJECT hTrafficMonitor, 
						   hOBJECT& hPOP3Protocoller, 
						   detect_code_t& dcPOP3DetectCode, 
						   HANDLE hStopEvent, 
						   retr_t& rCommand,
						   tBOOL bWaitOnce
						   )
{
	cERROR err = errOK;
	rCommand = retr_no;
	PR_TRACE((m_io, prtNOTIFY, "mc\trecv_t: Waiting for RETR..."));
	do 
	{
		data_source_t ds = dsUnknown;
		{
			PRAutoLocker _cs_(m_cs);
			ds = m_dsDataSource;
		}
		if ( ds != dsClient )
		{
			// Команды сервера передаем напрямую клиенту
			PR_TRACE((m_io, prtNOTIFY, "mc\trecv_t: Sending server data to client..."));
			if ( PR_FAIL(err = Translate( hSessionCtx, hTrafficMonitor, hPOP3Protocoller, dcPOP3DetectCode, hStopEvent)))
			{
				PR_TRACE((m_io, prtERROR, "mc\trecv_t: Sending server data to client failed (%terr)", err));
				break;
			}
			PR_TRACE((m_io, prtNOTIFY, "mc\trecv_t: Sending server data to client done"));
		}
		else
		{
			// Команды клиента - оцениваем. Ждем RETR или QUIT
			PR_TRACE((m_io, prtNOTIFY, "mc\trecv_t: Reading client data..."));
			cERROR err = Recv(dsClient, cFALSE, hStopEvent, bWaitOnce);
			if(IsRETR(dwRetrNum))
			{
				rCommand = retr_yes;
				IOCopy(m_io, retr_io);
				PR_TRACE((m_io, prtNOTIFY, "mc\trecv_t: Client data is RETR"));
				break;
			}
			else if(IsQUIT())
			{
				rCommand = retr_quit;
				PR_TRACE((m_io, prtNOTIFY, "mc\trecv_t: Client data is QUIT, sending data to server..."));
				err = Translate(hSessionCtx, hTrafficMonitor, hPOP3Protocoller, dcPOP3DetectCode, hStopEvent);
				PR_TRACE((m_io, prtNOTIFY, "mc\trecv_t: Sending data to server done (%terr)", err));
				break;
			}
			else 
			{
				PR_TRACE((m_io, prtNOTIFY, "mc\trecv_t: Nor RETR, nor QUIT, sending client data to server..."));
				if ( PR_FAIL(err = Translate( hSessionCtx, hTrafficMonitor, hPOP3Protocoller, dcPOP3DetectCode, hStopEvent)))
				{
					PR_TRACE((m_io, prtERROR, "mc\trecv_t: Sending data to server failed (%terr)", err));
					break;
				}
				PR_TRACE((m_io, prtNOTIFY, "mc\trecv_t: Sending data to server done"));
			}
		}
	} 
	while (!bWaitOnce && WaitForSingleObject(hStopEvent, 100) == WAIT_TIMEOUT);

	if(WaitForSingleObject(hStopEvent, 0) != WAIT_TIMEOUT)
	{
		// надо скинуть неотосланные данные, иначе чревато проблемами
		// некоторые клиенты говорят QUIT и сразу рвут соединение, и QUIT до сервера не доходит
		// как следствие не удаляются письма, если до QUIT был DELE (см. багу 28013)
		Translate(hSessionCtx, hTrafficMonitor, hPOP3Protocoller, dcPOP3DetectCode, NULL);
	}

	PR_TRACE((m_io, prtNOTIFY, "mc\trecv_t: Waiting for RETR done"));

	return err;
}

//
// recv_t
//
//////////////////////////////////////////////////////////////////////////

tERROR IOCopy(hIO src, hIO dst, tQWORD pos_begin, tQWORD length)
{
	cERROR err = errOK;
	tBYTE buf[1024];
	tDWORD size = 1024;
	tDWORD size_to_write = 0;
	tQWORD pos_src = pos_begin;
	tQWORD pos_dst = 0;
	if ( length == 0 )
		err = src->GetSize(&length, IO_SIZE_TYPE_EXPLICIT);
	if ( PR_FAIL(err) ) return err;
	do 
	{
		memset(buf, 0, 1024);
		err = src->SeekRead(&size, pos_src, buf, 1024);
		if ( PR_FAIL(err) ) 
			return err;
		pos_src += size;
		size_to_write = (length-pos_dst >= size) ? size : (DWORD)(length-pos_dst);
		err = dst->SeekWrite(0, pos_dst, buf, size_to_write);
		if ( PR_FAIL(err) )	
			return err;
		pos_dst += size_to_write;
	} 
	while ( PR_SUCC(err) && size && (pos_dst < length) );
	return err;
}
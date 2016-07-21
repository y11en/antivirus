// SessionData.cpp: implementation of the CPOP3SessionProcessor class.
//
//////////////////////////////////////////////////////////////////////

#include "POP3SessionAnalyzer.h"
#include "../bin_macro.h"
#include <inline.h>
#include "../MIMETransport.h"

#include <Extract/plugin/p_msoe.h>
#include <Mail/structs/s_mc.h>

CMailContent::CMailContent( data_source_t dsDataSource, content_t ctContentType, hIO hOriginalData, tLONG dwSizeToCopy ):
        m_dsDataSource(dsDataSource),
        m_ctContentType(ctContentType),
        m_hOriginalData(hOriginalData),
        m_hData(NULL), 
        m_bEncoded(cFALSE)
{
    cERROR err = errOK;

    err = m_hOriginalData->sysCreateObjectQuick( (hOBJECT*)&m_hData, IID_IO, PID_TMPFILE);

    if ( PR_SUCC(err) )
    {
        if ( m_ctContentType != contentProtocolSpecific )
            err = ReplaceCRLFDotDotToCRLFDot( m_hOriginalData, m_hData );
        else
        {
            // Простое копирование данных
            tDWORD dwRead = 0;
            tQWORD pos = 0;
            char data[1024] = {0};
            do
            {
                memset(data, 0, 1024);
                if ( PR_SUCC(err= hOriginalData->SeekRead(
                    &dwRead, 
                    pos, 
                    data, 
                    ((dwSizeToCopy < 1024) && (dwSizeToCopy > 0)) ? dwSizeToCopy : 1024
                    )) && dwRead )
                {
                    err = m_hData->SeekWrite( NULL, pos, data, dwRead );
                    if ( dwSizeToCopy != -1 )
                        dwSizeToCopy -= dwRead;
                    pos += dwRead;
                }
            }

            // ???
            while( PR_SUCC(err) && dwRead && dwSizeToCopy );

            if ( err == errEOF )
                err = errOK;
            //#ifdef _DEBUG
            //				tDWORD dwWritten = 0;
            //				memset(data, 0, 1024);
            //				itoa(::GetCurrentThreadId(), data, 10);
            //				err = m_hData->SeekWrite( &dwWritten, pos, data, strlen(data) );
            //				pos += dwWritten;
            //				err = m_hData->SeekWrite( &dwWritten, pos, "\r\n", 2 );
            //#endif // _DEBUG
        }
    }
};

CMailContent::~CMailContent()
{
    if ( m_hData )
    {
        m_hData->sysCloseObject();
        m_hData = NULL;
    }
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define NUM_POP3_COMMANDS	17
tCHAR* g_szClientCommands[NUM_POP3_COMMANDS] = {0};
tCHAR* g_szServerCommands[NUM_POP3_COMMANDS] = {0};
tDWORD g_dwCommands[NUM_POP3_COMMANDS] = {0};

#define DEF_COMMAND_ID( _name, _data )	\
const tDWORD g_dwCommand_##_name = _data;

#define DEF_COMMAND( _source, _num, _name, _const )	\
g_sz##_source##Commands[_num] = _const;	\
g_dwCommands[_num] = g_dwCommand_##_name;

DEF_COMMAND_ID( USER, BIN16(00000000,00000001) )		//USER
DEF_COMMAND_ID( PASS, BIN16(10000000,00000000) )		//PASS
DEF_COMMAND_ID( QUIT, BIN16(00000000,00000010) )		//QUIT
DEF_COMMAND_ID( STAT, BIN16(00000000,00000100) )		//STAT
DEF_COMMAND_ID( LIST, BIN16(00000000,00001000) )		//LIST
DEF_COMMAND_ID( RETR, BIN16(00000000,00010000) )		//RETR
DEF_COMMAND_ID( DELE, BIN16(00000000,00100000) )		//DELE
DEF_COMMAND_ID( NOOP, BIN16(00000000,01000000) )		//NOOP
DEF_COMMAND_ID( RSET, BIN16(00000000,10000000) )		//RSET
DEF_COMMAND_ID( APOP, BIN16(00000001,00000000) )		//APOP
DEF_COMMAND_ID( TOP , BIN16(00000010,00000000) )		//TOP"
DEF_COMMAND_ID( UIDL, BIN16(00000100,00000000) )		//UIDL
DEF_COMMAND_ID( CAPA, BIN16(00001000,00000000) )		//CAPA
DEF_COMMAND_ID( AUTH, BIN16(00010000,00000000) )		//AUTH
DEF_COMMAND_ID( OK  , BIN16(00100000,00000000) )		//+OK
DEF_COMMAND_ID( ERR , BIN16(01000000,00000000) )		//-ERR

struct cmds 
{
	cmds()
	{
		DEF_COMMAND( Client, 0,  USER, "USER" )		//USER
		DEF_COMMAND( Client, 1,  PASS, "PASS" )		//PASS
		DEF_COMMAND( Client, 2,  QUIT, "QUIT" )		//QUIT
		DEF_COMMAND( Client, 3,  STAT, "STAT" )		//STAT
		DEF_COMMAND( Client, 4,  LIST, "LIST" )		//LIST
		DEF_COMMAND( Client, 5,  RETR, "RETR" )		//RETR
		DEF_COMMAND( Client, 6,  DELE, "DELE" )		//DELE
		DEF_COMMAND( Client, 7,  NOOP, "NOOP" )		//NOOP
		DEF_COMMAND( Client, 8,  RSET, "RSET" )		//RSET
		DEF_COMMAND( Client, 9,  APOP, "APOP" )		//APOP
		DEF_COMMAND( Client, 10, TOP , "TOP"  )		//TOP
		DEF_COMMAND( Client, 11, UIDL, "UIDL" )		//UIDL
		DEF_COMMAND( Client, 12, CAPA, "CAPA" )		//CAPA
		DEF_COMMAND( Client, 13, AUTH, "AUTH" )		//AUTH
		DEF_COMMAND( Server, 14, OK  , "+OK"  )		//+OK
		DEF_COMMAND( Server, 15, ERR , "-ERR" )		//-ERR
	}
};

cmds gCommands;

tBOOL ProtocolDetected(IN tDWORD dwCommandsFound)
{
	//static cmds hcmds;
	static tDWORD g_dwCommandsNeeded = 
		g_dwCommand_OK |
		g_dwCommand_USER |
		g_dwCommand_PASS /*|
		g_dwCommand_LIST*/
		;
	static tDWORD g_dwCommandsNeededAlt = 
		g_dwCommand_OK |
		g_dwCommand_APOP /*|
		g_dwCommand_LIST*/
		;
	static tDWORD g_dwCommandsNeededAlt2 = 
		g_dwCommand_OK |
		g_dwCommand_AUTH /*|
		g_dwCommand_LIST*/
		;
	static tDWORD g_dwCommandsNeededAlt3 = 
		g_dwCommand_OK |
		g_dwCommand_USER |
		g_dwCommand_PASS /*|
		g_dwCommand_STAT*/
		;
	static tDWORD g_dwCommandsNeededAlt4 = 
		g_dwCommand_OK |
		g_dwCommand_APOP /*|
		g_dwCommand_STAT*/
		;
	static tDWORD g_dwCommandsNeededAlt5 = 
		g_dwCommand_OK |
		g_dwCommand_AUTH /*|
		g_dwCommand_STAT*/
		;
	static tDWORD g_dwCommandsNeededAlt6 = 
		g_dwCommand_OK |
		g_dwCommand_USER |
		g_dwCommand_PASS |
		g_dwCommand_UIDL
		;
	static tDWORD g_dwCommandsNeededAlt7 = 
		g_dwCommand_OK |
		g_dwCommand_APOP |
		g_dwCommand_UIDL
		;
	static tDWORD g_dwCommandsNeededAlt8 = 
		g_dwCommand_OK |
		g_dwCommand_AUTH |
		g_dwCommand_UIDL
		;
	
	//static tDWORD g_dwCommandsNeededAlt9 = 
	//	g_dwCommand_OK |
	//	g_dwCommand_USER /*|
	//	g_dwCommand_RETR*/
	//	;

	static tDWORD g_dwCommandsNeededAlt10 = 
		g_dwCommand_OK |
		g_dwCommand_APOP |
		g_dwCommand_RETR
		;
	static tDWORD g_dwCommandsNeededAlt11 = 
		g_dwCommand_OK |
		g_dwCommand_AUTH |
		g_dwCommand_RETR
		;
	static tDWORD g_dwCommandsNeededAlt12 = /* T-Online uses IP-authorization */
		g_dwCommand_OK |
		g_dwCommand_UIDL /*|
		g_dwCommand_RETR*/
		;
	static tDWORD g_dwCommandsNeededAlt13 = 
		g_dwCommand_OK |
		g_dwCommand_USER |
		g_dwCommand_PASS |
		g_dwCommand_RETR
		;
	
	if ( 
		((dwCommandsFound & g_dwCommandsNeeded    ) == g_dwCommandsNeeded     ) ||
		((dwCommandsFound & g_dwCommandsNeededAlt ) == g_dwCommandsNeededAlt  ) ||
		((dwCommandsFound & g_dwCommandsNeededAlt2) == g_dwCommandsNeededAlt2 ) ||
		((dwCommandsFound & g_dwCommandsNeededAlt3) == g_dwCommandsNeededAlt3 ) ||
		((dwCommandsFound & g_dwCommandsNeededAlt4) == g_dwCommandsNeededAlt4 ) ||
		((dwCommandsFound & g_dwCommandsNeededAlt5) == g_dwCommandsNeededAlt5 ) ||
		((dwCommandsFound & g_dwCommandsNeededAlt6) == g_dwCommandsNeededAlt6 ) ||
		((dwCommandsFound & g_dwCommandsNeededAlt7) == g_dwCommandsNeededAlt7 ) ||
		((dwCommandsFound & g_dwCommandsNeededAlt8) == g_dwCommandsNeededAlt8 ) ||
		//((dwCommandsFound & g_dwCommandsNeededAlt9) == g_dwCommandsNeededAlt9 ) ||
		((dwCommandsFound & g_dwCommandsNeededAlt10) == g_dwCommandsNeededAlt10 ) ||
		((dwCommandsFound & g_dwCommandsNeededAlt11) == g_dwCommandsNeededAlt11 ) ||
		((dwCommandsFound & g_dwCommandsNeededAlt12) == g_dwCommandsNeededAlt12 ) ||
		((dwCommandsFound & g_dwCommandsNeededAlt13) == g_dwCommandsNeededAlt13 )
		)
		return cTRUE;
	return cFALSE;
}

extern tPROPID g_propMessageIsIncoming;
extern tPROPID g_propMailMessageOS_PID;
extern tPROPID g_propMailerPID;

CPOP3SessionProcessor::CPOP3SessionProcessor():
	m_hTrafficMonitor(NULL),
	m_pMailContent(NULL),
	m_bStarted(cFALSE)
{
}

tERROR CPOP3SessionProcessor::Init(InitParams_t* pInitParams)
{
	if ( !pInitParams ) 
		return errNOT_OK;

	cERROR err = errOK;

	if ( PR_SUCC(err) ) 
		m_InitParams = *pInitParams;

	if ( PR_SUCC(err) ) 
		m_pAnalyzer = new CPOP3SessionAnalyzer(m_InitParams.hParent, m_InitParams.pSession, m_InitParams.hTrafficMonitor);

	if ( !m_pAnalyzer || !m_pAnalyzer->IsInitedOK() )
		err = errOBJECT_CANNOT_BE_INITIALIZED;
	
	return err;
}

CPOP3SessionProcessor::~CPOP3SessionProcessor()
{
	Flush();

	if ( m_pAnalyzer )
	{
		delete m_pAnalyzer;
		m_pAnalyzer = NULL;
	}
}

detect_code_t CPOP3SessionProcessor::Detect(data_source_t dsDataSource, tBYTE* pData, tDWORD dwDataSize)
{
	return m_pAnalyzer->Detect(dsDataSource, pData, dwDataSize);
}

detect_code_t CPOP3SessionAnalyzer::GetStatus()
{
	return m_dcResult;
}

detect_code_t CPOP3SessionProcessor::GetStatus()
{
	return m_pAnalyzer->GetStatus();
}

void CPOP3SessionProcessor::Flush()
{
	// Нужно отменить процессирование, вернуть данные и вернуть управление
	Stop();
	CMailContent* pMailContent = NULL;
	cERROR err = m_pAnalyzer->ExtractContent((IContentItem**)&pMailContent, cTRUE);
	if ( PR_SUCC(err) && pMailContent )
	{
		if ( pMailContent->Type() == contentProtocolSpecific )
		{
			process_msg_t process_msg;
			process_msg.assign(*m_InitParams.pSession, false);
			process_msg.dsDataTarget = dsClient;
			process_msg.psStatus = psProcessFinished;
			process_msg.hData = pMailContent->DataIO();
			hOBJECT send_point = m_hTrafficMonitor ? m_hTrafficMonitor : m_InitParams.hTrafficMonitor;
			err = send_point->sysSendMsg( 
				pmcTRAFFIC_PROTOCOLLER, 
				pmPROCESS_FINISHED, 
				0, 
				&process_msg, 
				SER_SENDMSG_PSIZE 
				);
			m_pAnalyzer->ReleaseContent(pMailContent);
			pMailContent = NULL;
		}
	}
}
tERROR CPOP3SessionProcessor::Process(data_source_t dsDataSource, tBYTE* pData, tDWORD dwDataSize)
{
	cERROR err = errOK;
	if ( dsDataSource == dsClient ) 
	{
		// Проблема POP3: клиент перестал ждать данные, и что-то нам написал.
		// Это нестандартная ситуация
		// Нужно отменить процессирование и вернуть управление
//		Stop();
//		return errOBJECT_INCOMPATIBLE;
		{
			// Отсылаем ненужные данные обратно в сессию
			hIO io_temp = NULL;
			err = m_InitParams.hParent->sysCreateObjectQuick( (hOBJECT*)&io_temp, IID_IO, PID_TMPFILE );
			if ( PR_SUCC(err) )
			{
				err = io_temp->SeekWrite(0, 0, pData, dwDataSize);
				if ( PR_SUCC(err) )
				{
					tBOOL bFinish = !m_pAnalyzer->HasContent();
					process_msg_t process_msg;
					process_msg.assign(*m_InitParams.pSession, false);
					process_msg.dsDataTarget = dsServer;
					process_msg.psStatus = !bFinish ? psKeepAlive : psProcessFinished;
					process_msg.hData = io_temp;
					hOBJECT send_point = m_hTrafficMonitor ? m_hTrafficMonitor : m_InitParams.hTrafficMonitor;
					err = send_point->sysSendMsg( 
						pmcTRAFFIC_PROTOCOLLER, 
						!bFinish ? pmKEEP_ALIVE : pmPROCESS_FINISHED, 
						0, 
						&process_msg, 
						SER_SENDMSG_PSIZE 
						);
				}
				io_temp->sysCloseObject();
				io_temp = NULL;
			}
		}
		return errOK;
	}

	// Это может быть "+OK\r\n" от сервера
	// Распарсим входной буфер на "+OK\r\n" и вызовем FlushResponse() на каждый "+OK\r\n"
	// Оставшееся сольем в m_pAnalyzer
	if ( 
		m_hNOOPServerThread.IsWaitingForResponse() &&
		PR_SUCC(m_hNOOPServerThread.EnqueueData(dsDataSource, pData, dwDataSize))
		)
	{
		tBOOL bNeedMoreProcess = cFALSE;
		IContentItem* pContentItem = NULL;
		m_hNOOPServerThread.ExtractContent(&pContentItem);

		if ( pContentItem && pContentItem->DataIO() )
		{
			tBYTE  l_data[1024] = {0};
			tDWORD l_size = 1024;
			tDWORD l_read = 0;
			tQWORD l_pos = 0;
			while ( PR_SUCC(pContentItem->DataIO()->SeekRead(&l_read, l_pos, l_data, l_size)) && l_read)
			{
				l_pos += l_read;
				m_pAnalyzer->EnqueueData(pContentItem->Source(), l_data, l_read);
				memset(l_data,0,l_size);
				bNeedMoreProcess = cTRUE;
			}
		}

		m_hNOOPServerThread.ReleaseContent(pContentItem);
		pContentItem = NULL;

		if ( !bNeedMoreProcess )
			return errOK;
	}
	else
		err = m_pAnalyzer->EnqueueData(dsDataSource, pData, dwDataSize);
	
	if ( PR_SUCC(err) )
	{
		_Run();
//		if ( m_pAnalyzer->HasContent() && !m_bStarted )
//			err = Start();
	}
	else
	{
		// Как обработать эту ситуацию?
		// - Порвать коннект.
		process_msg_t process_msg;
		process_msg.assign(*m_InitParams.pSession, false);
		process_msg.dsDataTarget = dsClient;
		process_msg.psStatus = psCloseConnection;
		process_msg.hData = NULL;
		hOBJECT send_point = m_hTrafficMonitor ? m_hTrafficMonitor : m_InitParams.hTrafficMonitor;
		
        err = send_point->sysSendMsg( 
			                    pmcTRAFFIC_PROTOCOLLER, 
			                    pmPROCESS_FINISHED, 
			                    0, 
			                    &process_msg, 
			                    SER_SENDMSG_PSIZE 
			                    );
		return errOK;
	}

//	if ( PR_SUCC(err) && m_pAnalyzer->HasContent() )
//	{
//		err = m_pAnalyzer->ExtractContent((IContentItem**)&m_pMailContent);
//		if ( PR_SUCC(err) && m_pMailContent )
//		{
//			if ( m_pMailContent->Type() == contentProtocolSpecific )
//			{
//				tBOOL bIsOK = m_pMailContent->IsOK();
//				process_msg_t process_msg;
//				process_msg.assign(*m_InitParams.pSession, false);
//				process_msg.dsDataTarget = dsClient;
//				process_msg.psStatus = bIsOK ? psKeepAlive : psProcessFinished;
//				process_msg.hData = m_pMailContent->DataIO();
//				hOBJECT send_point = m_hTrafficMonitor ? m_hTrafficMonitor : m_InitParams.hTrafficMonitor;
//				err = send_point->sysSendMsg( 
//					pmcTRAFFIC_PROTOCOLLER, 
//					bIsOK ? pmKEEP_ALIVE : pmPROCESS_FINISHED, 
//					0, 
//					&process_msg, 
//					SER_SENDMSG_PSIZE 
//					);
//				m_pAnalyzer->ReleaseContent(m_pMailContent);
//				m_pMailContent = NULL;
//			}
//			else
//				// Запустим задачу обработки данных
//				err = Start();
//		}
//	}
		
	return err;
}


tERROR CPOP3SessionProcessor::_Run()
{
	m_bStarted = cTRUE;

	cERROR err = errOK;
	
	while ( m_pAnalyzer->HasContent() )
	{
		IContentItem* pMailContent = NULL;
		err = m_pAnalyzer->ExtractContent( &pMailContent );
		if ( !pMailContent )
			continue;
		
		if ( pMailContent->Type() == contentProtocolSpecific )
		{
			tBOOL bIsOK = ((CMailContent*)pMailContent)->IsOK();
			tBOOL bFinish = !bIsOK && (pMailContent->Source() == dsServer);
			process_msg_t process_msg;
			process_msg.assign(*m_InitParams.pSession, false);
			process_msg.dsDataTarget = pMailContent->Source() == dsClient ? dsServer : dsClient;
			process_msg.psStatus     = !bFinish ? psKeepAlive : psProcessFinished;
			process_msg.hData        = pMailContent->DataIO();
			hOBJECT send_point       = m_hTrafficMonitor ? m_hTrafficMonitor : m_InitParams.hTrafficMonitor;

			err = send_point->sysSendMsg( 
				                pmcTRAFFIC_PROTOCOLLER, 
				                !bFinish ? pmKEEP_ALIVE : pmPROCESS_FINISHED, 
				                0, 
				                &process_msg, 
				                SER_SENDMSG_PSIZE 
				                );

			m_pAnalyzer->AssembleContent(pMailContent);
			m_pAnalyzer->ReleaseContent(pMailContent);
		}
		else
			//////////////////////////////////////////////////////////////////////////
			//
			// Обрабатываем полученный объект
			//
		{
			m_hNOOPServerThread.Start(
				                m_InitParams.pSession, 
				                "NOOP\r\n", 
				                strlen("NOOP\r\n"), 
				                dsServer, 
				                10000,
				                m_InitParams.hTrafficMonitor
				                );

			if ( PR_SUCC(err) )
			{
				pMailContent->DataIO()->set_pgOBJECT_ORIGIN(OID_MAIL_MSG_MIME);
				pMailContent->DataIO()->propSetDWord( g_propMailMessageOS_PID, PID_MSOE);
				pMailContent->DataIO()->propSetBool( g_propMessageIsIncoming, cTRUE);
				pMailContent->DataIO()->propSetDWord( g_propMailerPID, m_InitParams.pSession->dwClientPID);

				if ( PR_SUCC(err) )
				{
					cMCProcessParams params;
					hOBJECT send_point = /*m_hTrafficMonitor ? m_hTrafficMonitor : */m_InitParams.hTrafficMonitor;
					send_point->sysSendMsg( 
						        pmc_MAILTASK_PROCESS, 
						        NULL, 
						        pMailContent->DataIO(), 
						        &params, 
						        SER_SENDMSG_PSIZE
						        );
				}
			}

			m_hNOOPServerThread.Stop();

			if ( PR_SUCC(err) )
				err = m_pAnalyzer->AssembleContent(pMailContent);

			if ( PR_SUCC(err) )
			{
				// Обсылаем объект обратно в сессию
				process_msg_t process_msg;
				process_msg.assign(*m_InitParams.pSession, false);
				process_msg.dsDataTarget = dsClient;
				process_msg.psStatus = m_hNOOPServerThread.IsWaitingForResponse() ? psKeepAlive : psProcessFinished;
				process_msg.hData = pMailContent->DataIO();
				hOBJECT send_point = m_hTrafficMonitor ? m_hTrafficMonitor : m_InitParams.hTrafficMonitor;
				
                err = send_point->sysSendMsg( 
					pmcTRAFFIC_PROTOCOLLER, 
					m_hNOOPServerThread.IsWaitingForResponse() ? pmKEEP_ALIVE : pmPROCESS_FINISHED, 
					0, 
					&process_msg, 
					SER_SENDMSG_PSIZE 
					);
			}
			m_pAnalyzer->ReleaseContent(pMailContent);
		}
		//
		// Обрабатываем полученный объект
		//
		//////////////////////////////////////////////////////////////////////////
		if ( PR_FAIL(err) )
			int i = 0;
		
		if ( WaitForSingleObject(m_hStopEvent, 0) == WAIT_OBJECT_0 )
			break;
	}

	m_bStarted = cFALSE;

	return errOK;
}



//////////////////////////////////////////////////////////////////////////
//
//	CPOP3SessionAnalyzer

CPOP3SessionAnalyzer::CPOP3SessionAnalyzer( hOBJECT hParent, session_t* pSession, hOBJECT hTrafficMonitor ):
	m_hParent(hParent),
	m_pSession(pSession),
	m_hTrafficMonitor(hTrafficMonitor),
	m_err(errOK),
	m_Data(NULL),
	m_pData(NULL),
	m_dcResult(dcNeedMoreInfo),
	m_pClientData(NULL),
	m_pServerData(NULL),
	m_pos(0),
	m_pServerResponse(NULL),
	m_dwCommandsFound(0)
{
	m_err = hParent->sysCreateObjectQuick( (hOBJECT*)&m_Data, IID_IO, PID_TMPFILE );

	if ( PR_SUCC(m_err) ) 
	{
		m_pClientData = new CSimpleBufferWindow(COMMAND_DATA_SIZE, 0, 0, 0);
		
        if ( !m_pClientData || !m_pClientData->View() )
			m_err = errOBJECT_CANNOT_BE_INITIALIZED;

		m_pServerData = new CSimpleBufferWindow(COMMAND_DATA_SIZE, 0, 0, 0);

		if ( !m_pServerData || !m_pServerData->View() )
			m_err = errOBJECT_CANNOT_BE_INITIALIZED;
	}
}

CPOP3SessionAnalyzer::~CPOP3SessionAnalyzer()
{
	delete m_pClientData;
	delete m_pServerData;
	if ( m_Data )
		m_Data->sysCloseObject();
}
	
tERROR CPOP3SessionAnalyzer::ExtractContent( IN IContentItem** ppContent, tBOOL bForce )
{
	cERROR err = m_err;
	if ( PR_SUCC(err) )
	{
		if ( m_pServerResponse )
			*ppContent = (IContentItem*)m_pServerResponse;
		else if ( m_pData )
			*ppContent = (IContentItem*)m_pData;
		else if ( bForce )
		{
			tQWORD io_size = 0;
			cERROR err = m_Data->GetSize(&io_size, IO_SIZE_TYPE_EXPLICIT);
			if ( io_size )
				*ppContent = new CMailContent( dsServer, contentProtocolSpecific, m_Data );
			else 
				return errOBJECT_NOT_FOUND;
		}
		else
			return errOBJECT_NOT_FOUND;
		return errOK;
	}
	return errOBJECT_NOT_FOUND;
}

tERROR CPOP3SessionAnalyzer::AssembleContent( IN IContentItem* pContent ) 
{
	cERROR err = m_err;

	if ( PR_SUCC(err) && pContent )
	{
		CMailContent* pMailContent = (CMailContent*)pContent;
		pMailContent->Encode(); 
		if ( pMailContent == m_pData )
			StopNOOPClentThread();
		return errOK;
	}
	return err;
}

tERROR CPOP3SessionAnalyzer::ReleaseContent( IN IContentItem* pContent ) 
{
	if ( pContent == m_pServerResponse )
	{
		tQWORD io_size = 0;
		if ( m_pServerResponse->IsOK() )
			StartNOOPClentThread();
		delete m_pServerResponse;
		m_pServerResponse = NULL;
	}
	else if ( pContent == m_pData )
	{
		delete m_pData;
		m_pData = NULL;
		m_Data->SetSize(0);
		m_pos = 0;
	}
	else
		delete (CMailContent*)pContent;
	pContent = NULL;
	return errOK;
}

tERROR CPOP3SessionAnalyzer::EnqueueData( data_source_t dsDataSource, tBYTE* pData, tDWORD dwDataSize )
{
	cERROR err = m_err;
	if ( PR_SUCC(err) )
	{
		tDWORD dwWritten = 0;
		err = m_Data->SeekWrite( &dwWritten, m_pos, pData, dwDataSize );
		if ( PR_SUCC(err) && dwWritten )
			m_pos += dwWritten;
		else
			return m_err = err;

		if ( !m_pServerResponse )
			// Вырезаем ответ сервера из pSessionData->m_Data
			CutServerResponse();
		
		tQWORD io_size = 0;
		err = m_Data->GetSize(&io_size, IO_SIZE_TYPE_EXPLICIT);
		tCHAR end_of_data[5] = {0};
		tDWORD dwRead = 0;
		if (
			io_size > 5 &&
			PR_SUCC( m_Data->SeekRead(&dwRead, io_size - 5, end_of_data, 5) ) &&
			dwRead == 5
			)
		{
			if ( 0 == strncmp(end_of_data, "\r\n.\r\n",5) )
			{
				// Достигнут конец данных! Надо их отрабатывать!
				if ( !m_pData )
				{
					m_pData = new CMailContent( dsServer, contentMime, m_Data );
					if ( !m_pData )
						err = errOBJECT_NOT_INITIALIZED;
				}
			}
		}
	}
	return err;
}

tBOOL CPOP3SessionAnalyzer::HasContent() const
{
	cERROR err = m_err;
	if ( PR_SUCC(err) )
	{
		if ( m_pServerResponse ) 
			return cTRUE;
		if ( m_pData ) 
			return cTRUE;

	}
	return cFALSE;
}

#define FIND_END_OF_SERVER_RESPONSE( _server_response, _size, _dwEND_OF_SERVER_RESPONSE )	\
{	                                        \
	for ( tDWORD i = 0; i < _size; i++ )	\
	{	                                    \
		if (	                            \
			_server_response[i] == '\r' &&	\
			i+1 < _size &&	                \
			_server_response[i+1] == '\n'	\
			)	                            \
		{	                                \
			_dwEND_OF_SERVER_RESPONSE = i+2;	\
			break;	                        \
		}	                                \
	}	                                    \
}

tBOOL CPOP3SessionAnalyzer::CutServerResponse()
{
	cERROR err = errOK;
	tQWORD io_size = 0;
	err = m_Data->GetSize(&io_size, IO_SIZE_TYPE_EXPLICIT);
	tCHAR server_response[1024] = {0};
	tDWORD dwRead = 0;

	if ( 
		io_size > 5 &&
		PR_SUCC( m_Data->SeekRead(&dwRead, 0, server_response, 1024) ) &&
		dwRead >= 4
		)
	{
		tBOOL bServerERR = cFALSE;
		// Поднимаем регистр server_response (первые 4 символа)
		INLINE::ToUpper(server_response, 4);

		tDWORD dwEND_OF_SERVER_RESPONSE = 0;
		
        if ( 0 == memcmp( "+OK", server_response, 3 ) )
			FIND_END_OF_SERVER_RESPONSE(server_response, dwRead, dwEND_OF_SERVER_RESPONSE)
		else if ( 0 == memcmp( "-ERR", server_response, 4 ) )
		{
			dwEND_OF_SERVER_RESPONSE = (tDWORD)io_size;
			bServerERR = cTRUE;
		}
		
		if ( dwEND_OF_SERVER_RESPONSE )
		{
			if ( !m_pServerResponse )
			{
				m_pServerResponse = new CMailContent( dsServer, contentProtocolSpecific, m_Data, dwEND_OF_SERVER_RESPONSE );

				if ( m_pServerResponse )
				{
					// Теперь нам надо сдвинуть данные в m_Data на начало
					tQWORD qwPos = dwEND_OF_SERVER_RESPONSE;
					tCHAR data[1024] = {0};
					do 
					{
						tDWORD dwWritten = 0;
						dwRead = 0;
						err = m_Data->SeekRead( &dwRead, qwPos, data, 1024 );
						if ( PR_SUCC(err) && dwRead )
							err = m_Data->SeekWrite( &dwWritten, qwPos-dwEND_OF_SERVER_RESPONSE, data, dwRead );
						if ( PR_SUCC(err) && dwWritten )
							qwPos += dwRead;
					} 
					while( PR_SUCC(err) && dwRead );
					err = m_Data->SetSize(io_size - dwEND_OF_SERVER_RESPONSE);
					m_pos -= dwEND_OF_SERVER_RESPONSE;
					return cTRUE;
				}
			}
		}
		else
			err = errEOF;
	}
	else
		err = errEOF;
	
	return cFALSE;
}

tERROR CPOP3SessionAnalyzer::ClearData()
{
	if ( ProtocolDetected(m_dwCommandsFound) )
		m_dcResult = dcProtocolDetected;
	else
		m_dcResult = dcNeedMoreInfo;
	if ( m_pServerResponse )
	{
		delete m_pServerResponse;
		m_pServerResponse = NULL;
	}
	if ( m_pData )
	{
		delete m_pData;
		m_pData = NULL;
	}
	m_Data->SetSize(0);
	m_pos = 0;
	return errOK;
}

detect_code_t CPOP3SessionAnalyzer::Detect(data_source_t dsDataSource, tBYTE* pData, tDWORD dwDataSize)
{
	if ( PR_FAIL(m_err) )
		return dcNeedMoreInfo;

	tBYTE* lpb = 0;
	tBOOL bNeedToClearClientData = cFALSE;
	tBOOL bNeedToClearServerData = cFALSE;

	switch ( dsDataSource ) 
	{
	case dsClient:
		m_pServerData->Clear();
		m_pClientData->Insert( pData, (dwDataSize>COMMAND_DATA_SIZE)?COMMAND_DATA_SIZE:dwDataSize );
		m_pClientData->Lock();
		// Поискать команду протокола POP3, обычную для клиента
		{
			INLINE::ToUpper((tCHAR*)m_pClientData->View(), m_pClientData->GetSize());
			tULONG ulCounter = 0;
			tDWORD dwLastCommand = 0;
			for ( ulCounter = 0; ulCounter < NUM_POP3_COMMANDS; ulCounter++ )
			{
				if (
					g_szClientCommands[ulCounter] &&
					0 == strncmp(
						(tCHAR*)m_pClientData->View(), 
						g_szClientCommands[ulCounter], 
						strlen(g_szClientCommands[ulCounter])
						)
					)
				{
					dwLastCommand = g_dwCommands[ulCounter];
					m_dwCommandsFound |= dwLastCommand;
					break;
				}
			}
			if ( ProtocolDetected(m_dwCommandsFound) )
				m_dcResult = dcProtocolDetected;

			if ( m_dcResult == dcProtocolDetected && dwLastCommand == g_dwCommand_RETR )
			{
				ClearData();
				m_dcResult = dcDataDetected;
				bNeedToClearClientData = cTRUE;
			}
		}

		m_pClientData->Unlock();

		if ( bNeedToClearClientData )
			m_pClientData->Clear();
		break;
	case dsServer:
		m_pServerData->Insert( pData, (dwDataSize>COMMAND_DATA_SIZE)?COMMAND_DATA_SIZE:dwDataSize  );
		m_pServerData->Lock();
		// Поискать команду протокола POP3, обычную для сервера
		{
			INLINE::ToUpper((tCHAR*)m_pServerData->View(), m_pServerData->GetSize());
			
			tULONG ulCounter = 0;
			tDWORD dwLastCommand = 0;
			for ( ulCounter = 0; ulCounter < NUM_POP3_COMMANDS; ulCounter++ )
			{
				if (
					g_szServerCommands[ulCounter] &&
					0 == strncmp(
						(tCHAR*)m_pServerData->View(), 
						g_szServerCommands[ulCounter], 
						strlen(g_szServerCommands[ulCounter])
						)
					)
				{
					dwLastCommand = g_dwCommands[ulCounter];
					m_dwCommandsFound |= dwLastCommand;
					break;
				}
			}
			if ( ProtocolDetected(m_dwCommandsFound) )
				m_dcResult = dcProtocolDetected;
		}
		m_pServerData->Unlock();
		m_pClientData->Clear();
		break;
	}

	return m_dcResult;
}

tERROR CPOP3SessionAnalyzer::StartNOOPClentThread()
{
	m_hNOOPClientThread.Stop();
	return m_hNOOPClientThread.Start(
		m_pSession, 
		"X-Kaspersky: Checking \r\n", 
		strlen("X-Kaspersky: Checking \r\n"), 
		dsClient, 
		10000,
		m_hTrafficMonitor
		);
}

tERROR CPOP3SessionAnalyzer::StopNOOPClentThread()
{
	return m_hNOOPClientThread.Stop();
}

tBOOL CPOP3SessionAnalyzer::IsInitedOK()
{
	return PR_SUCC(m_err);
}

tERROR CPOP3SessionProcessor::Start()
{
	m_bStarted = cTRUE;
	return CMailSessionProcessor::Start("POP3 Session Processor");
}

//////////////////////////////////////////////////////////////////////////

CNOOPServerThread::CNOOPServerThread() : CNOOPThread(), m_hData(NULL)
{
}

CNOOPServerThread::~CNOOPServerThread()
{
	if ( m_hData )
		m_hData->sysCloseObject();
	m_hData = NULL;
}

tERROR CNOOPServerThread::EnqueueData(data_source_t dsDataSource, tBYTE* pData, tDWORD dwDataSize )
{
	cERROR err = errOK;
	tQWORD io_size = 0;
	tDWORD dwEND_OF_SERVER_RESPONSE = 0;
	if ( !m_hData )
		m_InitParams.hParent->sysCreateObjectQuick( (hOBJECT*)&m_hData, IID_IO, PID_TMPFILE );
	if ( dsDataSource == dsServer && m_hData )
	{
		err = m_hData->SeekWrite(0, io_size, pData, dwDataSize);
		if ( PR_SUCC(err) )
			err = m_hData->GetSize(&io_size, IO_SIZE_TYPE_EXPLICIT);
		if ( PR_SUCC(err) )
		{
			tCHAR server_response[1024] = {0};
			tDWORD dwRead = 0;
			
			while ( 
				io_size >= 5 &&
				PR_SUCC( m_hData->SeekRead(&dwRead, 0, server_response, 1024) ) &&
				dwRead >= 4 &&
				IsWaitingForResponse()
				)
			{
				tBOOL bServerERR = cFALSE;
				// Поднимаем регистр server_response (первые 4 символа)
				INLINE::ToUpper(server_response, 4);
				
				if ( 0 == memcmp( "+OK", server_response, 3 ) )
					FIND_END_OF_SERVER_RESPONSE(server_response, dwRead, dwEND_OF_SERVER_RESPONSE)
					else if ( 0 == memcmp( "-ERR", server_response, 4 ) )
				{
					dwEND_OF_SERVER_RESPONSE = (tDWORD)io_size;
					bServerERR = cTRUE;
				}
				
				if ( dwEND_OF_SERVER_RESPONSE )
				{
					FlushResponse();
					// Теперь нам надо сдвинуть данные в m_Data на начало
					tQWORD qwPos = dwEND_OF_SERVER_RESPONSE;
					tCHAR data[1024] = {0};
					do 
					{
						tDWORD dwWritten = 0;
						dwRead = 0;
						err = m_hData->SeekRead( &dwRead, qwPos, data, 1024 );
						if ( PR_SUCC(err) && dwRead )
							err = m_hData->SeekWrite( &dwWritten, qwPos-dwEND_OF_SERVER_RESPONSE, data, dwRead );
						if ( PR_SUCC(err) && dwWritten )
							qwPos += dwRead;
					} 
					while( PR_SUCC(err) && dwRead );
					err = m_hData->SetSize(io_size -= dwEND_OF_SERVER_RESPONSE);
				}
				else
					break;
			}
		}
	}
	else
		err = errOBJECT_INCOMPATIBLE;
	return err;
}

tERROR CNOOPServerThread::ExtractContent(OUT IContentItem** ppMailContent)
{
	if ( ppMailContent ) 
        *ppMailContent = NULL;

	if ( IsWaitingForResponse() ) 
        return errOBJECT_CANNOT_BE_INITIALIZED;

	if ( !ppMailContent )         
        return errOBJECT_BAD_PTR;

	if ( !m_hData )               
        return errOBJECT_CANNOT_BE_INITIALIZED;

	*ppMailContent = new CMailContent( dsServer, contentProtocolSpecific, m_hData );
	if ( *ppMailContent )
        return errOK;

    return errOBJECT_CANNOT_BE_INITIALIZED;
}
tERROR CNOOPServerThread::ReleaseContent( IN IContentItem* pMailContent )
{
	if ( pMailContent )
		delete (CMailContent*)pMailContent;

	return errOK;
}
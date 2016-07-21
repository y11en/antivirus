// SessionData.cpp: implementation of the CSMTPSessionAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#define PR_IMPEX_DEF

#include <Prague/prague.h>
#include <Extract/plugin/p_msoe.h>

#include "SMTPSessionAnalyzer.h"
#include "../bin_macro.h"
#include <inline.h>
#include "../MIMETransport.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define NUM_SMTP_COMMANDS	16
tCHAR* g_szCommands[NUM_SMTP_COMMANDS] = {0};
tDWORD g_dwCommands[NUM_SMTP_COMMANDS] = {0};

#define DEF_COMMAND_ID( _name, _data )	\
const tDWORD g_dwCommand_##_name = _data;

#define DEF_COMMAND(_num, _name, _const)	\
g_szCommands[_num] = _const;	\
g_dwCommands[_num] = g_dwCommand_##_name;

DEF_COMMAND_ID( HELO, BIN16(00000000,00000001) )		//HELO <SP> <domain> <CRLF>			
DEF_COMMAND_ID( EHLO, BIN16(00000000,00000001) )		//EHLO <SP> Domain <CRLF>
DEF_COMMAND_ID( MAIL, BIN16(00000000,00000010) )		//MAIL <SP> FROM:<reverse-path> <CRLF>	
DEF_COMMAND_ID( RCPT, BIN16(00000000,00000100) )		//RCPT <SP> TO:<forward-path> <CRLF>
DEF_COMMAND_ID( DATA, BIN16(00000000,00001000) )		//DATA <CRLF>		
DEF_COMMAND_ID( RSET, BIN16(00000000,00010000) )		//RSET <CRLF>
DEF_COMMAND_ID( SEND, BIN16(00000000,00100000) )		//SEND <SP> FROM:<reverse-path> <CRLF>
DEF_COMMAND_ID( SOML, BIN16(00000000,01000000) )		//SOML <SP> FROM:<reverse-path> <CRLF>
DEF_COMMAND_ID( SAML, BIN16(00000000,10000000) )		//SAML <SP> FROM:<reverse-path> <CRLF>
DEF_COMMAND_ID( VRFY, BIN16(00000001,00000000) )		//VRFY <SP> <string> <CRLF>
DEF_COMMAND_ID( EXPN, BIN16(00000010,00000000) )		//EXPN <SP> <string> <CRLF>
DEF_COMMAND_ID( HELP, BIN16(00000100,00000000) )		//HELP [<SP> <string>] <CRLF>
DEF_COMMAND_ID( NOOP, BIN16(00001000,00000000) )		//NOOP <CRLF>
DEF_COMMAND_ID( QUIT, BIN16(00010000,00000000) )		//QUIT <CRLF>
DEF_COMMAND_ID( TURN, BIN16(00100000,00000000) )		//TURN <CRLF>

struct cmds 
{
	cmds()
	{
		DEF_COMMAND( 0,  HELO, "HELO" )		//HELO <SP> <domain> <CRLF>			
		DEF_COMMAND( 1,  EHLO, "EHLO" )		//EHLO <SP> Domain <CRLF>
		DEF_COMMAND( 2,  MAIL, "MAIL FROM" )//MAIL <SP> FROM:<reverse-path> <CRLF>	
		DEF_COMMAND( 3,  RCPT, "RCPT TO" )	//RCPT <SP> TO:<forward-path> <CRLF>
		DEF_COMMAND( 4,  DATA, "DATA" )		//DATA <CRLF>		
		DEF_COMMAND( 5,  RSET, "RSET" )		//RSET <CRLF>
		DEF_COMMAND( 6,  SEND, "SEND" )		//SEND <SP> FROM:<reverse-path> <CRLF>
		DEF_COMMAND( 7,  SOML, "SOML" )		//SOML <SP> FROM:<reverse-path> <CRLF>
		DEF_COMMAND( 8,  SAML, "SAML" )		//SAML <SP> FROM:<reverse-path> <CRLF>
		DEF_COMMAND( 9,  VRFY, "VRFY" )		//VRFY <SP> <string> <CRLF>
		DEF_COMMAND( 10, EXPN, "EXPN" )		//EXPN <SP> <string> <CRLF>
		DEF_COMMAND( 11, HELP, "HELP" )		//HELP [<SP> <string>] <CRLF>
		DEF_COMMAND( 12, NOOP, "NOOP" )		//NOOP <CRLF>
		DEF_COMMAND( 13, QUIT, "QUIT" )		//QUIT <CRLF>
		DEF_COMMAND( 14, TURN, "TURN" )		//TURN <CRLF>
	}
};
cmds hcmds;

tDWORD g_dwCommandsNeeded = 
	g_dwCommand_HELO |
	g_dwCommand_MAIL |
	g_dwCommand_RCPT |
	g_dwCommand_DATA
	;

extern tPROPID g_propMessageIsIncoming;
extern tPROPID g_propMailMessageOS_PID;
extern tPROPID g_propMailerPID;
extern tPROPID g_propMessageCheckOnly;

CSMTPSessionAnalyzer::CSMTPSessionAnalyzer():
	m_Data(NULL),
	m_dcResult(dcNeedMoreInfo),
	m_pClientData(NULL),
	m_pServerData(NULL),
	m_pos(0),
	m_dwCommandsFound(0),
	m_dwLastCommand(0)
{
}

CSMTPSessionAnalyzer::~CSMTPSessionAnalyzer()
{
	if ( m_pClientData )
		delete m_pClientData;
	m_pClientData = NULL;
	if ( m_pServerData )
		delete m_pServerData;
	m_pServerData = NULL;
	if ( m_Data )
		m_Data->sysCloseObject();
	m_Data = NULL;
}

tERROR CSMTPSessionAnalyzer::Init(InitParams_t* pInitParams)
{
	if ( !pInitParams ) 
		return errNOT_OK;

	cERROR err = errOK;
	if ( PR_SUCC(err) ) 
	{
		m_InitParams = *pInitParams;
		err = m_InitParams.hParent->sysCreateObjectQuick( (hOBJECT*)&m_Data, IID_IO, PID_TMPFILE );
		if ( PR_SUCC(err) ) 
		{
			m_pClientData = new CSimpleBufferWindow(COMMAND_DATA_SIZE, 0, 0, 0);
			if ( !m_pClientData )
				err = errOBJECT_CANNOT_BE_INITIALIZED;
			m_pServerData = new CSimpleBufferWindow(COMMAND_DATA_SIZE, 0, 0, 0);
			if ( !m_pServerData )
				err = errOBJECT_CANNOT_BE_INITIALIZED;
		}
	}
	
	return err;
}

detect_code_t CSMTPSessionAnalyzer::Detect(data_source_t dsDataSource, tBYTE* pData, tDWORD dwDataSize)
{
	tBYTE* lpb = 0;
	tBOOL bNeedToClearClientData = cFALSE;
	tBOOL bNeedToClearServerData = cFALSE;

	switch ( dsDataSource ) 
	{
	case dsClient:
        {
            m_pServerData->Clear();
            m_pClientData->Insert( pData, (dwDataSize>COMMAND_DATA_SIZE)?COMMAND_DATA_SIZE:dwDataSize );
            m_pClientData->Lock();
            // Поискать команду протокола SMTP, обычную для клиента

            INLINE::ToUpper((tCHAR*)m_pClientData->View(), m_pClientData->GetSize());
            
            for ( tULONG ulCounter = 0; ulCounter < NUM_SMTP_COMMANDS; ulCounter++ )
            {
                if ( g_szCommands[ulCounter] && 0 == strncmp(
                                                        (tCHAR*)m_pClientData->View(), 
                                                        g_szCommands[ulCounter], 
                                                        strlen(g_szCommands[ulCounter]) ) )
                {
                    m_dwLastCommand = g_dwCommands[ulCounter];
                    m_dwCommandsFound |= m_dwLastCommand;
                    break;
                }
            }
            if ( (m_dwCommandsFound & g_dwCommandsNeeded) == g_dwCommandsNeeded )
                m_dcResult = dcProtocolDetected;
            //			if ( m_dcResult == dcProtocolDetected && m_dwLastCommand == g_dwCommand_DATA )
            //			{
            //				ClearData();
            //				m_dcResult = dcDataDetected;
            //			}

            m_pClientData->Unlock();
        }
		break;
	case dsServer:
		m_pServerData->Insert( pData, (dwDataSize>COMMAND_DATA_SIZE)?COMMAND_DATA_SIZE:dwDataSize  );
		m_pServerData->Lock();

		if ( 
			m_dcResult == dcProtocolDetected &&
			m_dwLastCommand == g_dwCommand_DATA &&
			0 == strncmp( (tCHAR*)m_pServerData->View(), "354", 3 )
			)
		{
			ClearData();
			m_dcResult = dcDataDetected;
			bNeedToClearServerData = cTRUE;
		}

		m_pServerData->Unlock();
		m_pClientData->Clear();

		if ( bNeedToClearServerData )
			m_pServerData->Clear();
		// Поискать команду протокола SMTP, обычную для сервера
		// !!! Это нам не нужно в в протоколе SMTP
		break;
	}

	return m_dcResult;
}

tERROR CSMTPSessionAnalyzer::Process(data_source_t dsDataSource, tBYTE* pData, tDWORD dwDataSize)
{
	cERROR err = errOK;

	if ( dsDataSource == dsServer ) 
	{
		// Проблема SMTP: сервер перестал ждать данные, и что-то нам написал.
		// Это нестандартная ситуация
		// Нужно отменить процессирование и вернуть управление
		Stop();
		return errNOT_OK;
	}

	tDWORD dwWritten = 0;
	err = m_Data->SeekWrite( &dwWritten, m_pos, pData, dwDataSize );
	if ( PR_SUCC(err) && dwWritten )
		m_pos += dwWritten;

	cMCProcessParams mcParams;
	cStringObj szMessageName;
	tBOOL bEndOfFile = cFALSE;
	tQWORD io_size = 0;
    tCHAR end_of_data[5] = {0};
    tDWORD dwRead = 0;

	err = m_Data->GetSize(&io_size, IO_SIZE_TYPE_EXPLICIT);
	
	if (
		io_size > 5 &&
		PR_SUCC( m_Data->SeekRead(&dwRead, io_size - 5, end_of_data, 5) ) &&
		dwRead == 5
		)
	{
		if ( 0 == strncmp(end_of_data, "\r\n.\r\n",5) )
		{
			// Достигнут конец данных! Надо их отрабатывать!
			// Запустим задачу обработки данных
			bEndOfFile = cTRUE;
			err = Check(mcParams, szMessageName);
		}
	}

	{
		// Обсылаем объект обратно в сессию
		process_msg_t process_msg;
		process_msg.assign(*m_InitParams.pSession, false);
		hIO hTempIO = NULL;
		err = m_Data->sysCreateObjectQuick( (hOBJECT*)&hTempIO, IID_IO, PID_TMPFILE);
		if ( PR_SUCC(err) )
		{
			process_msg.dsDataTarget = (dsDataSource == dsServer) ? dsClient : dsServer;
			process_msg.hData = hTempIO;
			if ( !bEndOfFile )
			{
				// Отсылаем как есть
				process_msg.psStatus = psKeepAlive ;
				hTempIO->SeekWrite(0, 0, pData, dwDataSize);
			}
			else if (
				(
				   (mcParams.p_MailCheckerVerdicts & mcv_MESSAGE_CHANGED)
				|| (mcParams.p_MailCheckerVerdicts & mcv_MESSAGE_AV_INFECTED)
				|| (mcParams.p_MailCheckerVerdicts & mcv_MESSAGE_AV_WARNING)
				|| (mcParams.p_MailCheckerVerdicts & mcv_MESSAGE_AV_DISINFECTED)
				|| (mcParams.p_MailCheckerVerdicts & mcv_MESSAGE_DELETED)
				))
			{
				// Пишем ошибку клиенту и рвем соединение
				process_msg.dsDataTarget = dsClient;
				process_msg.psStatus = psCloseConnection ;
				//char buf[] = "451 Last email has been infected!\r\n\0";
				cStringObj sz;
				sz.format( cCP_UNICODE, L"451 Virus %S has been found in the %S message! More info at http://www.viruslist.com/en/search?VN=%S\r\n", (tWCHAR*)mcParams.p_szVirusName.c_str(cCP_UNICODE), (tWCHAR*)szMessageName.c_str(cCP_UNICODE), (tWCHAR*)mcParams.p_szVirusName.c_str(cCP_UNICODE));
				hTempIO->SeekWrite(0, 0, sz.c_str(cCP_ANSI), sz.length());
			}
			else
			{
				// Отсылаем как есть последний кусок письма
				process_msg.psStatus = psProcessFinished ;
				hTempIO->SeekWrite(0, 0, pData, dwDataSize);
			}
			err = m_InitParams.hTrafficMonitor->sysSendMsg( 
				pmcTRAFFIC_PROTOCOLLER, 
				process_msg.psStatus == psKeepAlive ? pmKEEP_ALIVE : pmPROCESS_FINISHED, 
				0, 
				&process_msg, 
				SER_SENDMSG_PSIZE 
				);
			hTempIO->sysCloseObject();
			hTempIO = NULL;
			if ( bEndOfFile )
				ClearData();
		}
	}
	return err;
}

tERROR CSMTPSessionAnalyzer::ClearData()
{
	if ( (m_dwCommandsFound & g_dwCommandsNeeded) == g_dwCommandsNeeded )
		m_dcResult = dcProtocolDetected;
	else
		m_dcResult = dcNeedMoreInfo;
	m_dwLastCommand = 0;
	m_pos = 0;
	return m_Data->SetSize(0);
}

tERROR CSMTPSessionAnalyzer::_Run()
{
	return errOK;
}
tERROR CSMTPSessionAnalyzer::Check(OUT cMCProcessParams& mcParams, OUT cStringObj& szMessageName)
{
	cERROR err = errOK;
	//////////////////////////////////////////////////////////////////////////
	//
	// Обрабатываем полученный объект
	//
	{
		cMCProcessParams& params = mcParams;
		hIO hTempIO = NULL;
		err = m_Data->sysCreateObjectQuick( (hOBJECT*)&hTempIO, IID_IO, PID_TMPFILE);
		if ( PR_SUCC(err) )
		{
			// 1. Декодируем из транспортного формата в формат MIME (заменяем  \r\n.. на \r\n.)
			err = ReplaceCRLFDotDotToCRLFDot( m_Data, hTempIO );
			// 2. Процессируем
			{
				if ( PR_SUCC(err) )
				{
					hTempIO->set_pgOBJECT_ORIGIN(OID_MAIL_MSG_MIME);
					hTempIO->propSetDWord( g_propMailMessageOS_PID, PID_MSOE);
					hTempIO->propSetBool( g_propMessageIsIncoming, cFALSE);
					hTempIO->propSetDWord( g_propMessageCheckOnly,  cTRUE);
					hTempIO->propSetDWord( g_propMailerPID, m_InitParams.pSession->dwClientPID);
					if ( PR_SUCC(err) )
					{
						hOS hTempOS = NULL;
						if ( PR_SUCC(hTempIO->sysCreateObjectQuick( (hOBJECT*)&hTempOS, IID_OS, PID_MSOE )))
						{
							szMessageName.assign(hTempOS, pgOBJECT_NAME);
							m_InitParams.hTrafficMonitor->sysSendMsg( 
								pmc_MAILTASK_PROCESS, 
								NULL, 
								hTempOS, 
								&params, 
								SER_SENDMSG_PSIZE
								);
							hTempOS->sysCloseObject();
							hTempOS = NULL;
						}
						else
						{
							m_InitParams.hTrafficMonitor->sysSendMsg( 
								pmc_MAILTASK_PROCESS, 
								NULL, 
								hTempIO, 
								&params, 
								SER_SENDMSG_PSIZE
								);
						}
					}
				}
			}
			// 3. Кодируем из MIME в транспортный формат (заменяем \r\n. на \r\n..)
			if ( !(params.p_MailCheckerVerdicts & mcv_MESSAGE_DELETED) )
			{
				ReplaceCRLFDotToCRLFDotDot( hTempIO, m_Data );
				hTempIO->sysCloseObject();
				hTempIO = NULL;
			}
		}
	}
	//
	// Обрабатываем полученный объект
	//
	//////////////////////////////////////////////////////////////////////////
	return err;
}

#include "ProtocolProcessor.h"

//! страничка, которая появляется в результате удаления содержимого http пакета
char const templ_text[] = 
						"The requested file infected with %VIRUSNAME% virus has been deleted.\r\n"
						"You can read about the virus at this address: http://www.viruslist.com/en/search?VN=%VIRUSNAME%\r\n"
						"Best regards, Kaspersky Internet Security 2006 team\r\n"
						;

extern tPROPID g_propMessageIsIncoming;
extern tPROPID g_propMailMessageOS_PID;
extern tPROPID g_propMailerPID;
tPROPID g_propMailMessageNoNeedTreatment;

CProtocolProcessor::CProtocolProcessor():
	m_pContentItem(NULL),
	m_bIsStarted(cFALSE)
{
}

tERROR CProtocolProcessor::Init(InitParams_t* pInitParams)
{
	if ( !pInitParams ) 
		return errNOT_OK;
	if ( !pInitParams->pAnalyzer )
		return errNOT_OK;
	m_InitParams = *pInitParams;
	return errOK;
}

CProtocolProcessor::~CProtocolProcessor()
{
	if ( m_InitParams.pAnalyzer )
	{
		delete m_InitParams.pAnalyzer;
		m_InitParams.pAnalyzer = NULL;
	}
}

detect_code_t CProtocolProcessor::Detect(data_source_t dsDataSource, tBYTE* pData, tDWORD dwDataSize)
{
	m_dcResult = m_InitParams.pAnalyzer->Detect(dsDataSource, pData, dwDataSize);
	if ( 
		m_dcResult == dcDataDetected &&
		!m_bIsStarted
		)
	{
		if ( PR_FAIL(Start("ProtocolProcessor")))
			return m_dcResult = dcNeedMoreInfo;
		m_bIsStarted = cTRUE;
	}
	return m_dcResult;
}

tERROR CProtocolProcessor::Process(data_source_t dsDataSource, tBYTE* pData, tDWORD dwDataSize)
{
	if ( dsDataSource == dsServer )
		PR_TRACE(( m_InitParams.hParent, prtNOTIFY, "CProtocolProcessor::Process => << %s", (char*)pData ));
	else
		PR_TRACE(( m_InitParams.hParent, prtNOTIFY, "CProtocolProcessor::Process => >> %s", (char*)pData ));
	cERROR err = errOK;
	err = m_InitParams.pAnalyzer->EnqueueData(dsDataSource, pData, dwDataSize);
	return err;
}

tERROR CProtocolProcessor::_Run()
{
	cERROR err = errOK;
	
	while ( true )
	{
		IContentItem* pMailContent = NULL;
		while ( m_InitParams.pAnalyzer->HasContent() )
		{
			err = m_InitParams.pAnalyzer->ExtractContent( &pMailContent );
			if ( PR_SUCC(err) )
			{
				if ( pMailContent->Type() == contentProtocolSpecific )
				{
					process_msg_t process_msg;
					process_msg.assign(*m_InitParams.pSession, false);
					process_msg.dsDataTarget = pMailContent->Source() == dsClient ? dsServer : dsClient;
					process_msg.psStatus = psKeepAlive;
					process_msg.hData = pMailContent->DataIO();
					err = m_InitParams.hTrafficMonitor->sysSendMsg( 
						pmcTRAFFIC_PROTOCOLLER, 
						pmKEEP_ALIVE, 
						0, 
						&process_msg, 
						SER_SENDMSG_PSIZE 
						);
					m_InitParams.pAnalyzer->AssembleContent(pMailContent);
					m_InitParams.pAnalyzer->ReleaseContent(pMailContent);
				}
				else
				{
					CNOOPThread hNOOPServerThread;
					//////////////////////////////////////////////////////////////////////////
					//
					// Обрабатываем полученный объект
					//
					//	if ( PR_SUCC(err) )
					//		hNOOPServerThread.Start(
					//			m_InitParams.pSession, 
					//			"NOOP\r\n", 
					//			strlen("NOOP\r\n"), 
					//			dsServer, 
					//			10000,
					//			m_InitParams.hTP
					//			);

					hIO io = pMailContent->DataIO();
					tBOOL bAntivirusOnly = cFALSE;
					if ( pMailContent->Type() != contentPartition )
					{
						io->set_pgOBJECT_ORIGIN(OID_MAIL_MSG_MIME);
						io->propSetDWord( g_propMailMessageOS_PID, PID_MSOE);
					}
					else
					{
						// считаем что возвращается вложение
						bAntivirusOnly = cTRUE;
											
						io->propSetStr( 0, pgOBJECT_NAME, "Mail attachment");
						io->propSetStr( 0, pgOBJECT_FULL_NAME, "Mail attachment");
						io->set_pgOBJECT_ORIGIN(OID_MAIL_MSG_ATTACH);

						// указываем, что вложение не надо лечить (все другое возможно)
						// это связано с невозможностью обратного кодирования вложения после проверки
						hROOT root = (hROOT)io->sysGetParent(IID_ROOT);
						if ( root && PR_SUCC(root->RegisterCustomPropId(
							&g_propMailMessageNoNeedTreatment, 
							npNO_NEED_TREATMENT,
							pTYPE_BOOL | pCUSTOM_HERITABLE
							)))
							io->propSetBool( g_propMailMessageNoNeedTreatment, cTRUE);
					}

					if ( m_InitParams.hProtocolType == TrafficProtocoller::po_NNTP )
						bAntivirusOnly = cTRUE;
					io->propSetBool( g_propMessageIsIncoming, pMailContent->Source() == dsServer ? cTRUE : cFALSE);
					io->propSetDWord( g_propMailerPID, m_InitParams.pSession->dwClientPID);
					tPROPID g_propProtocolType = 0;
					hROOT root = (hROOT)io->sysGetParent(IID_ROOT);
					if ( root && PR_SUCC(root->RegisterCustomPropId(
						&g_propProtocolType, 
						npPROTOCOL_TYPE, 
						pTYPE_DWORD | pCUSTOM_HERITABLE
						)))
						io->propSetDWord( g_propProtocolType, m_InitParams.hProtocolType);

					if ( PR_SUCC(err) )
					{
						cMCProcessParams params;
						m_InitParams.hTrafficMonitor->sysSendMsg( 
							bAntivirusOnly ? pmc_MAILCHECKER_PROCESS : pmc_MAILTASK_PROCESS, 
							NULL, 
							pMailContent->DataIO(), 
							&params, 
							SER_SENDMSG_PSIZE
							);
						if ( params.p_MailCheckerVerdicts & mcv_MESSAGE_DELETED )
						{
							cStringObj szNewContent = templ_text;
							szNewContent.replace_all("%VIRUSNAME%", 0, (tCHAR*)params.p_szVirusName.c_str(cCP_ANSI));
							pMailContent->DetachDataIO();
							pMailContent->RecreateDataIO( (tCHAR*)szNewContent.c_str(cCP_ANSI), szNewContent.length() );
						}
					}
					if ( g_propProtocolType && root )
						root->UnregisterCustomPropId(npPROTOCOL_TYPE);

					//	hNOOPServerThread.Stop(cTRUE);
					if ( PR_SUCC(err) )
						err = m_InitParams.pAnalyzer->AssembleContent(pMailContent);
					if ( PR_SUCC(err) )
					{
						// Обсылаем объект обратно в сессию
						process_msg_t process_msg;
						process_msg.assign(*m_InitParams.pSession, false);
						process_msg.dsDataTarget = pMailContent->Source() == dsClient ? dsServer : dsClient;
						process_msg.psStatus = psKeepAlive;
						process_msg.hData = pMailContent->DataIO();
						err = m_InitParams.hTrafficMonitor->sysSendMsg( 
							pmcTRAFFIC_PROTOCOLLER, 
							pmPROCESS_FINISHED, 
							0, 
							&process_msg, 
							SER_SENDMSG_PSIZE 
							);
					}
					m_InitParams.pAnalyzer->ReleaseContent(pMailContent);
					//
					// Обрабатываем полученный объект
					//
					//////////////////////////////////////////////////////////////////////////
				}
			}
			pMailContent = NULL;

			if ( 
				WaitForSingleObject(m_hStopEvent, 0) == WAIT_OBJECT_0 &&
				!m_InitParams.pAnalyzer->HasContent()
				)
				break;
		}
		
		if ( 
			WaitForSingleObject(m_hStopEvent, 10) == WAIT_OBJECT_0 &&
			!m_InitParams.pAnalyzer->HasContent()
			)
			break;
	}

	m_bIsStarted = cFALSE;
	return errOK;
}


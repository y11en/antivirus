// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  05 July 2007,  17:25 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



#pragma warning(disable:4786)

// AVP Prague stamp begin( Includes for interface,  )
#include "task.h"
//! #include "taskex.h"
// AVP Prague stamp end




#include <Extract/plugin/p_mailmsg.h>

#include <Mail/iface/i_antispamfilter.h>
#include <Mail/iface/i_mailtask.h>
#include <Mail/plugin/p_as_trainwizard.h>
#include <Mail/plugin/p_imapprotocoller.h>
#include <Mail/plugin/p_mctask.h>
#include <Mail/plugin/p_nntpprotocoller.h>
#include <Mail/plugin/p_pop3protocoller.h>
#include <Mail/plugin/p_smtpprotocoller.h>
#include <Mail/plugin/p_spamtest.h>
#include <Mail/plugin/p_trafficmonitor.h>
#include <Mail/structs/s_mc.h>
#include <Mail/structs/s_mailwasher.h>

#include <map>

#include "antispam_interface_imp2.h"
#include "../../Interceptors/TrafficMonitor2/TrafficProtocoller.h"

#ifdef _TRAIN_AUTOMAT
#include "GetUserNameByProcessID.h"
#endif

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end



tPROPID g_propMessageIsIncoming;
tPROPID g_propMessageCheckOnly;
tPROPID g_propMessageUncomplete;
static tPROPID g_propMailerPID;
static tPROPID g_propProtocolType;

inline tERROR _REGISTER_MSG_HANDLER_( tDWORD pmc, tIID iid, hTASKMANAGER tm, hOBJECT _this )
{
	tERROR err = _this->sysRegisterMsgHandler( pmc, rmhLISTENER, tm, iid, PID_ANY, IID_ANY, PID_ANY );
	if ( err == errOBJECT_ALREADY_EXISTS ) err = errOK;
	return err;
}
inline tERROR _REGISTER_MSG_HANDLER( tDWORD pmc, tIID iid, hTASKMANAGER tm, hOBJECT _this )
{
	tERROR err = _this->sysRegisterMsgHandler( pmc, rmhDECIDER, tm, iid, PID_ANY, IID_ANY, PID_ANY );
	if ( err == errOBJECT_ALREADY_EXISTS ) err = errOK;
	return err;
}
inline tERROR _UNREGISTER_MSG_HANDLER( tDWORD pmc, tIID iid, hTASKMANAGER tm, hOBJECT _this )
{
	return _this->sysUnregisterMsgHandler( pmc, tm );
}
#define REGISTER_MSG_HANDLER_( pmc, iid )	_REGISTER_MSG_HANDLER_( pmc, iid, m_tm, *this )
#define REGISTER_MSG_HANDLER( pmc, iid )	_REGISTER_MSG_HANDLER( pmc, iid, m_tm, *this )
#define UNREGISTER_MSG_HANDLER( pmc, iid )	_UNREGISTER_MSG_HANDLER( pmc, iid, m_tm, *this )
#define REGISTER_MSG_HANDLERS( error, iid )	\
{	\
	if ( PR_SUCC(error) ) error = REGISTER_MSG_HANDLER_(   pmc_MAIL_WASHER, iid );	\
	if ( PR_SUCC(error) ) error = REGISTER_MSG_HANDLER(   pmc_MAILTASK_PROCESS, iid );	\
	if ( PR_SUCC(error) ) error = REGISTER_MSG_HANDLER(   pmc_ANTISPAM_PROCESS, iid );	\
	if ( PR_SUCC(error) ) error = REGISTER_MSG_HANDLER(   pmc_ANTISPAM_TRAIN,   iid );	\
	if ( PR_SUCC(error) ) error = REGISTER_MSG_HANDLER(   pmc_TASK_STATE_CHANGED,iid );	\
}
#define UNREGISTER_MSG_HANDLERS( error, iid )	\
{	\
	/*if ( PR_SUCC(error) ) error = */ UNREGISTER_MSG_HANDLER( pmc_MAIL_WASHER, iid );	\
	/*if ( PR_SUCC(error) ) error = */ UNREGISTER_MSG_HANDLER( pmc_MAILTASK_PROCESS, iid );	\
	/*if ( PR_SUCC(error) ) error = */ UNREGISTER_MSG_HANDLER( pmc_ANTISPAM_PROCESS, iid );	\
	/*if ( PR_SUCC(error) ) error = */ UNREGISTER_MSG_HANDLER( pmc_ANTISPAM_TRAIN,   iid );	\
	/*if ( PR_SUCC(error) ) error = */ UNREGISTER_MSG_HANDLER( pmc_TASK_STATE_CHANGED,iid );\
}
#define RELEASE_SERVICE( error, service )	\
{	\
	if ( service && PR_SUCC(sysCheckObject((hOBJECT)service)) )	\
	{	\
		/*error = */m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)service);	\
		service = NULL;	\
	}	\
}
#define GET_RELEASE_SERVICE( error, service_id, service, release )	\
{	\
		if ( !service && !release )		\
			error = m_tm->GetService( TASKID_TM_ITSELF, *this, service_id, &service);	\
		if ( PR_FAIL(error) || (release && service) )	\
			RELEASE_SERVICE(error, service);	\
}
#define MessageIsFromExternalPlugin(_send_point)	PID_TRAFFICMONITOR != _send_point->propGetPID()
#define MessageIsFromTrafficMonitor(_send_point)	PID_TRAFFICMONITOR == _send_point->propGetPID()

tERROR CTask::GetServices( tBOOL bIncomingOnly, tBOOL bRelease ) 
{
	cERROR err = errOK;
	static hOBJECT hSMTP       = NULL;
	static hOBJECT hPOP3       = NULL;
	static hOBJECT hMailWasher = NULL;
	static hOBJECT hIMAP       = NULL;
	static hOBJECT hNNTP       = NULL;

	if ( !bIncomingOnly )
		GET_RELEASE_SERVICE(err, AVP_SERVICE_TRAFFICMONITOR_SMTP,    hSMTP, bRelease)
	else
		RELEASE_SERVICE(err, hSMTP)
	if (PR_SUCC(err))
		GET_RELEASE_SERVICE(err, AVP_SERVICE_TRAFFICMONITOR_POP3,    hPOP3, bRelease)
	if (PR_SUCC(err))
		GET_RELEASE_SERVICE(err, AVP_SERVICE_TRAFFICMONITOR_MWASHER, hMailWasher, bRelease)
	if (PR_SUCC(err))
		GET_RELEASE_SERVICE(err, AVP_SERVICE_TRAFFICMONITOR_IMAP,    hIMAP, bRelease)
	if (PR_SUCC(err))
		GET_RELEASE_SERVICE(err, AVP_SERVICE_TRAFFICMONITOR_NNTP,    hNNTP, bRelease)
	return err;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CTask::ObjectInitDone() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	// Place your code here
	m_iidToHandle = IID_ANY;
	m_hAntispamFilter = NULL;

	if (PR_SUCC(error))
		m_hAntispamFilter = new CFilterManager(*this);
	if ( !m_hAntispamFilter )
		error = errOBJECT_CANNOT_BE_INITIALIZED;
	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);
	
	PR_TRACE(( this, prtIMPORTANT, "Create antispamfilter result 0x%x", error));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CTask::ObjectPreClose() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	// Place your code here
	UNREGISTER_MSG_HANDLERS( error, m_iidToHandle );
	GetServices(m_settings.m_bCheckIncomingMessagesOnly, cTRUE);

	if ( m_hAntispamFilter )
	{
		m_hAntispamFilter->Stop();
		delete m_hAntispamFilter;
		m_hAntispamFilter = NULL;
	}

	return error;
}
// AVP Prague stamp end



tERROR CTask::CheckTrainingWizard(tDWORD dwPID, tBOOL bIsUnknownTrainState)
{
#ifdef _TRAIN_AUTOMAT
	if ( dwPID == 0 )
		return errOBJECT_NOT_FOUND;

	cStringObj szUserName;
	GetUserNameByProcessID(dwPID, szUserName);

	typedef std::map<std::string, bool> users_trained_t;
	static users_trained_t g_users_trained;
	if ( g_users_trained[szUserName.c_str(cCP_ANSI)] )
		return errOK;

	cAntispamPersistance hAntispamPersistance;
	if ( bIsUnknownTrainState && m_persistance_storage )
	{
		cSerializable* p = &hAntispamPersistance;
		g_root->RegDeserialize(
			&p, 
			m_persistance_storage, 
			szUserName.c_str(cCP_ANSI), 
			cAntispamPersistance::eIID
			);
		if ( !hAntispamPersistance.m_bJustTrained )
		{
			cAntispamAskTrain hAsk;
			hAsk.m_dwSessionId = dwSID;
			if ( errOK_DECIDED == sysSendMsg( pmcRUN_TRAINWIZARD, 0, 0, &hAsk, SER_SENDMSG_PSIZE))
				hAntispamPersistance.m_bJustTrained = cTRUE;
		}
	}
	else
		hAntispamPersistance.m_bJustTrained = cTRUE;
	if ( hAntispamPersistance.m_bJustTrained && m_persistance_storage )
		g_root->RegSerialize(
			&hAntispamPersistance, 
			m_persistance_storage, 
			szUserName.c_str(cCP_ANSI);
			);
	g_users_trained[pszUserName] = hAntispamPersistance.m_bJustTrained ? true : false;
#endif//_TRAIN_AUTOMAT
	
	return errOK;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR CTask::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "MailTask::MsgReceive method" );

	// Place your code here
	switch( p_msg_cls_id )
	{
	case pmc_MAIL_WASHER:
		if (
			pm_MESSAGE_DELETED == p_msg_id &&
			(SER_SENDMSG_PSIZE == p_par_buf_len) &&
			((cSerializable*)p_par_buf)->isBasedOn(cPOP3Message::eIID) &&
			(m_state == TASK_STATE_RUNNING)
			)
		{
			// Посылаем отчет
			cAntispamReport rep;
			cPOP3Message* msg = (cPOP3Message*)p_par_buf;
			rep.m_szLastObjectName = msg->m_szName;
			rep.m_szLastMessageSender = msg->m_szFrom;
			rep.m_szLastMessageSubj = msg->m_szSubject;
			rep.m_dwLastMessageCategory = cAntispamStatisticsEx::IsDeleted;
			rep.m_dwLastReason = asdr_MailDispatcher;
			rep.m_timeFinish = (tDWORD)time(0);
			sysSendMsg( pmc_ANTISPAM_REPORT, NULL, NULL, &rep, SER_SENDMSG_PSIZE );
			return errOK;
		}
		break;
	case pmc_MAILTASK_PROCESS:
	case pmc_ANTISPAM_PROCESS:
		{
			tBOOL bCheckIncomingMessagesOnly   = cFALSE;
			tBOOL bCheckExternalPluginMessages = cFALSE;
			tBOOL bCheckTrafficMonitorMessages = cFALSE;
			{
				cAutoCS cs(m_lock, false); // Синхронизируем функции изменения настроек
				bCheckIncomingMessagesOnly   = m_settings.m_bCheckIncomingMessagesOnly;
				bCheckExternalPluginMessages = m_settings.m_bCheckExternalPluginMessages;
				bCheckTrafficMonitorMessages = m_settings.m_bCheckTrafficMonitorMessages;
			}
			if (
				!bCheckExternalPluginMessages &&
				MessageIsFromExternalPlugin(p_send_point)
				)
				return errOK;
			if (
				!bCheckTrafficMonitorMessages &&
				MessageIsFromTrafficMonitor(p_send_point)
				)
				return errOK;
			
			if (
				(SER_SENDMSG_PSIZE == p_par_buf_len) &&
				((cSerializable*)p_par_buf)->isBasedOn(cMCProcessParams::eIID) &&
				(m_state == TASK_STATE_RUNNING)
				)
			{
				// Если письмо на процессирование пришло от ТраффикМонитора, и оно качается Аутлуком, и mcou_antispam запущен - отказываемся от проверки
				{
					cERROR err = errOK;
					if (
						MessageIsFromTrafficMonitor(p_send_point) &&
						bCheckExternalPluginMessages &&
						p_ctx->propGetBool(g_propMessageIsIncoming) &&
						!p_ctx->propGetBool(g_propMessageCheckOnly)
						)
					{
						tDWORD dwSize = sizeof(tDWORD);
						cAntispamAskPlugin hAntispamAskPlugin;
						hAntispamAskPlugin.m_dwProcessID = p_ctx->propGetDWord(g_propMailerPID);
						hAntispamAskPlugin.m_dwProtocolType = p_ctx->propGetDWord(g_propProtocolType);
						if (
							errOK_DECIDED == sysSendMsg(pmc_ANTISPAM_ASK_PLUGIN, 0, 0, &hAntispamAskPlugin, SER_SENDMSG_PSIZE)
							)
							return errOK;
					}
				}
				
				if ( m_hAntispamFilter && p_ctx )
				{
					tBOOL dwMessageIsIncoming = p_ctx->propGetBool(g_propMessageIsIncoming);
					if ( bCheckIncomingMessagesOnly && !dwMessageIsIncoming )
						// Если письмо - исходящее, а нам не сказано ничего делать с исходящими - возвращаем errOK
						return errOK;
					
					if ( dwMessageIsIncoming )
					{
						// Если это - первое письмо данного пользователя, пришедшее на проверку с момента установки продукта - запустить ТренингВизард!
						CheckTrainingWizard(p_ctx->propGetDWord(g_propMailerPID), cTRUE);
						
						// Процессируем письмо
						CAntispamImplementer::CKLAntispamFilter::ProcessStatistics_t statistics;
						tDWORD& result = (((cMCProcessParams*)p_par_buf)->p_MailCheckerVerdicts);
						error = m_hAntispamFilter->Process(
							&result,
							(cSerializable*)&m_settings,
							p_ctx,
							(((cMCProcessParams*)p_par_buf)->p_dwTimeOut),
							&statistics
							);
						if ( PR_SUCC(error) )
						{
							// Посылаем отчет
							cAntispamReport& rep = statistics;
							((cMCProcessParams*)p_par_buf)->p_dwASReason = rep.m_dwLastReason;
							if ( result & mcv_MESSAGE_AS_SPAM )
								rep.m_dwLastMessageCategory = cAntispamReport::IsSpam;
							else if ( result & mcv_MESSAGE_AS_PROBABLE_SPAM )
								rep.m_dwLastMessageCategory = cAntispamReport::IsProbableSpam;
							else
								rep.m_dwLastMessageCategory = cAntispamReport::IsHam;
							if ( !p_ctx->propGetBool(g_propMessageCheckOnly) )
								sysSendMsg( pmc_ANTISPAM_REPORT, NULL, NULL, &rep, SER_SENDMSG_PSIZE );
						}
						else if ( error == errOPERATION_CANCELED )
							error = errOK;
					}
					else
					{
						if ( p_ctx->propGetPID() == PID_MAILMSG )
						{
							// ФИЧА: Как выяснилось, в Аутлуке исходящее письмо может быть отловлено дважды
							// (если оно было форварднуто или зареплайено).
							// Обучаться надо на том, которое не имеет отправителя
							// - это настоящее письмо
							// Остальные - игнорируем
							cStringObj szFrom; szFrom.assign(p_ctx, pgMESSAGE_FROM);
							if ( szFrom != "\"\" <:>" )
								return errOK;
						}
						// Обучаемся на исходящих письмах
						CAntispamImplementer::CKLAntispamFilter::ProcessStatistics_t statistics;
						CAntispamImplementer::CKLAntispamFilter::TrainParams_t trainParams;
						trainParams.Action = cAntispamTrainParams::TrainAsHam;
						error = m_hAntispamFilter->Train(
							&trainParams,
							p_ctx,
							(((cAntispamTrainParams*)p_par_buf)->p_dwTimeOut),
							&statistics
							);
						if ( PR_SUCC(error) )
						{
							// Посылаем отчет
							cAntispamReport& rep = statistics;
							rep.m_dwLastMessageCategory = cAntispamReport::IsHam;
							sysSendMsg( pmc_ANTISPAM_REPORT, NULL, NULL, &rep, SER_SENDMSG_PSIZE );
						}
						else if ( error == errOPERATION_CANCELED )
							error = errOK;
					}
				}
			}
				if ( !p_ctx && m_state == TASK_STATE_RUNNING )
					error = errOK_DECIDED;
		}
		break;
	case pmc_ANTISPAM_TRAIN:
		if ( SER_SENDMSG_PSIZE == p_par_buf_len )
		{
			CAntispamImplementer::CKLAntispamFilter::TrainParams_t trainParams;
			CAntispamImplementer::CKLAntispamFilter::ProcessStatistics_t hStatistics;
			CAntispamImplementer::CKLAntispamFilter::ProcessStatistics_t* statistics = &hStatistics;
			if ( ((cSerializable*)p_par_buf)->isBasedOn(cAntispamTrainParams::eIID) )
			{
				cAntispamTrainParams* pParams = (cAntispamTrainParams*)p_par_buf;
				trainParams.Action = (cAntispamTrainParams::Action_t)pParams->p_Action;
				if ( pParams->p_pStat ) statistics = pParams->p_pStat;
			}
			else if ( ((cSerializable*)p_par_buf)->isBasedOn(cAntispamTrainParamsEx::eIID) )
			{
				cAntispamTrainParamsEx* pParams = (cAntispamTrainParamsEx*)p_par_buf;
				trainParams.Action = (cAntispamTrainParams::Action_t)pParams->m_AntispamTrainParams.p_Action;
				statistics = &(pParams->m_AntispamReport);
			}
			else
				return errOK;
		
			if ( m_hAntispamFilter )
			{
				CheckTrainingWizard(((cAntispamTrainParams*)p_par_buf)->p_dwPID, cFALSE);
				if ( p_ctx && !p_ctx->propGetDWord(g_propMailerPID) )
					p_ctx->propSetDWord(g_propMailerPID, ((cAntispamTrainParams*)p_par_buf)->p_dwPID);
				// Обучаемся
				error = m_hAntispamFilter->Train(
					&trainParams,
					p_ctx,
					(((cAntispamTrainParams*)p_par_buf)->p_dwTimeOut),
					statistics
					);
				if ( PR_SUCC(error) )
				{
					// Посылаем отчет
					cAntispamReport& rep = *statistics;
					switch (trainParams.Action)
					{
					case cAntispamTrainParams::TrainAsHam:
					case cAntispamTrainParams::RemoveFromSpam:
						rep.m_dwLastMessageCategory = cAntispamReport::IsHam;
						break;
					case cAntispamTrainParams::TrainAsSpam:
					case cAntispamTrainParams::RemoveFromHam:
						rep.m_dwLastMessageCategory = cAntispamReport::IsSpam;
						break;
					}
					sysSendMsg( pmc_ANTISPAM_REPORT, NULL, NULL, &rep, SER_SENDMSG_PSIZE );
				}
				else if ( error == errOPERATION_CANCELED )
					error = errOK;
				if ( trainParams.Action == cAntispamTrainParams::Train_Begin )
					break;
			}
		}
		if ( !p_ctx && m_state == TASK_STATE_RUNNING )
			error = errOK_DECIDED;
		break;
	case pmc_TASK_STATE_CHANGED:
		{
			if( !(p_msg_id & STATE_FLAG_MALFUNCTION) )
				break;
			
			tDWORD nPluginPID = p_send_point->propGetPID();
			if( nPluginPID != PID_SMTPPROTOCOLLER &&
				nPluginPID != PID_POP3PROTOCOLLER &&
				nPluginPID != PID_IMAPPROTOCOLLER &&
				nPluginPID != PID_NNTPPROTOCOLLER &&
				nPluginPID != PID_SPAMTEST
				)
				break;

			tBOOL bChangeState = cFALSE;
			{
				cAutoCS cs(m_lock, false);
				if( m_state & STATE_FLAG_MALFUNCTION )
					break;

				switch(nPluginPID)
				{
				case PID_SMTPPROTOCOLLER:
					if( m_settings.m_bCheckIncomingMessagesOnly )
						break;
				case PID_POP3PROTOCOLLER:
				case PID_IMAPPROTOCOLLER:
				case PID_NNTPPROTOCOLLER:
					bChangeState = m_settings.m_bCheckTrafficMonitorMessages;
					break;
				case PID_SPAMTEST:
					bChangeState = m_settings.m_bUseGSG || m_settings.m_bUsePDB || m_settings.m_bUseRecentTerms;
					break;
				}
				if( bChangeState )
					m_state = p_msg_id;
			}
			if( bChangeState )
				sysSendMsg(pmc_TASK_STATE_CHANGED, p_msg_id, NULL, NULL, NULL);
		}
		break;
	default:
		break;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR CTask::SetSettings( const cSerializable* p_settings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	// Place your code here

	cAntispamSettings m_settings_old;
	{
		cAutoCS cs(m_lock, true);
		m_settings_old = m_settings;
		error = m_settings.assign(*p_settings,false);
#ifdef USE_FISHER
		m_settings.m_bUseFisher = cTRUE;
#endif
	}

	if ( PR_SUCC(error) && m_hAntispamFilter )
		error = m_hAntispamFilter->SetSettings(p_settings);

	if ( PR_SUCC(error) && (m_state == TASK_STATE_RUNNING) )
		error = SetState( (tTaskRequestState)m_state );

	if ( PR_SUCC(error) )
		error = errOK;

	if ( PR_FAIL(error) )
	{
		cAutoCS cs(m_lock, true);
		m_settings = m_settings_old;
	}

	PR_TRACE( (this,prtIMPORTANT,"(antispam)CTask::SetSettings [%terr], %d, %d",error, m_settings.m_bUseWhiteAddresses, ((cAntispamSettings*)p_settings)->m_bCheckExternalPluginMessages) );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR CTask::GetSettings( cSerializable* p_settings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Place your code here
	cAutoCS cs(m_lock, false); // Синхронизируем с функциями изменения настроек
	if ( m_hAntispamFilter )
		error = m_hAntispamFilter->GetSettings(p_settings);
	else
		error = p_settings->assign(m_settings, false);

	PR_TRACE( (this,prtIMPORTANT,"(antispam)CTask::GetSettings [%terr], %d, %d",error, ((cAntispamSettings*)p_settings)->m_bUseWhiteAddresses, ((cAntispamSettings*)p_settings)->m_bCheckExternalPluginMessages) );

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR CTask::AskAction( tActionId p_actionId, cSerializable* p_actionInfo ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
//! tERROR CTask::SetState( tTaskState p_state ) {
tERROR CTask::SetState( tTaskRequestState p_state ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	// Place your code here
	tDWORD l_state = m_state;
	m_state = TASK_STATE_FAILED;
	if ( PR_SUCC(error) )
	{
		tBOOL bCheckIncomingMessagesOnly   = cFALSE;
		tBOOL bCheckExternalPluginMessages = cFALSE;
		tBOOL bCheckTrafficMonitorMessages = cFALSE;
		{
			cAutoCS cs(m_lock, false); // Синхронизируем функции изменения настроек
			bCheckIncomingMessagesOnly   = m_settings.m_bCheckIncomingMessagesOnly;
			bCheckExternalPluginMessages = m_settings.m_bCheckExternalPluginMessages;
			bCheckTrafficMonitorMessages = m_settings.m_bCheckTrafficMonitorMessages;
		}
		if ( p_state & TASK_REQUEST_RUN )
		{
			tIID iid = /*bCheckExternalPluginMessages ?*/ IID_ANY /*: IID_MAILTASK*/;
			bool bNeedToCheck =
					bCheckExternalPluginMessages ||
					bCheckTrafficMonitorMessages
					;
			if ( bNeedToCheck )
				REGISTER_MSG_HANDLERS( error, iid );
			if (
				(iid != m_iidToHandle) ||
				!bNeedToCheck
				)
			{
				UNREGISTER_MSG_HANDLERS( error, m_iidToHandle );
				m_iidToHandle = iid;
			}
			// Требуем сервис ТраффикМонитора, если нам надо его обслуживать
			if ( PR_SUCC(error) && bCheckTrafficMonitorMessages )
				error = GetServices(bCheckIncomingMessagesOnly, cFALSE);
			else
				error = GetServices(bCheckIncomingMessagesOnly, cTRUE);
				
			if ( PR_SUCC(error) )
				m_state = TASK_STATE_RUNNING;
		}
		else if ( p_state & TASK_REQUEST_STOP )
		{
			GetServices(bCheckIncomingMessagesOnly, cTRUE);
			// wait MailWasher
			time_t last_trace = 0, t;
			cProfileBase profMailWasher;
			while(PR_SUCC(m_tm->GetProfileInfo(AVP_SERVICE_TRAFFICMONITOR_MWASHER, &profMailWasher)) && 
					!(IS_TASK_NOT_RUNNING(profMailWasher.m_nState) || profMailWasher.m_nState == PROFILE_STATE_DISABLED))
			{
				if(last_trace != (t = time(0)))
				{
					PR_TRACE((this, prtIMPORTANT, "Waiting MailWasher (state is %d)...", profMailWasher.m_nState));
					last_trace = t;
				}
				Sleep(100);
			}
			if(last_trace)
				PR_TRACE((this, prtIMPORTANT, "Waiting MailWasher done"));
			UNREGISTER_MSG_HANDLERS( error, m_iidToHandle );
			
			if ( PR_SUCC(error) )
				m_state = TASK_STATE_STOPPED;
		}
		else if ( p_state & TASK_REQUEST_PAUSE )
		{
			UNREGISTER_MSG_HANDLERS( error, m_iidToHandle );
			
			if ( PR_SUCC(error) )
				m_state = TASK_STATE_PAUSED;
		}
		else
			m_state = l_state;
	}

	if ( PR_SUCC(error) )
		if ( m_hAntispamFilter )
			error = m_hAntispamFilter->SetState((tTaskRequestState)m_state);

	if ( m_state != l_state )
		sysSendMsg(pmc_TASK_STATE_CHANGED, m_state, NULL, NULL, NULL);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR CTask::GetStatistics( cSerializable* p_statistics ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here

	if ( m_hAntispamFilter )
	{
		cAntispamStatistics statistics;
		error = m_hAntispamFilter->GetStatistics((cSerializable*)(&statistics));
		if ( PR_SUCC(error) )
			m_statistics = statistics;
	}
	error = p_statistics->assign(m_statistics, false);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR CTask::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "1", 2 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, CTask, m_tm, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, CTask, m_state, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, CTask, m_session_id, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_PERSISTENT_STORAGE, CTask, m_persistance_storage, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Task)
	mEXTERNAL_METHOD(Task, SetSettings)
	mEXTERNAL_METHOD(Task, GetSettings)
	mEXTERNAL_METHOD(Task, AskAction)
	mEXTERNAL_METHOD(Task, SetState)
	mEXTERNAL_METHOD(Task, GetStatistics)
mEXTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "Task::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASK,                             // interface identifier
		PID_ANTISPAMTASK,                       // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                         // interface version
		VID_DENISOV,                            // interface developer
		&i_Task_vtbl,                         // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_Task_vtbl,                         // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),   // external function table size
		Task_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable), // property table size
		sizeof(CTask)-sizeof(cObjImpl),       // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Task(IID_TASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propMessageIsIncoming, 
			npMESSAGE_IS_INCOMING, 
			pTYPE_BOOL | pCUSTOM_HERITABLE
			);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propMailerPID, 
			npMAILER_PID, 
			pTYPE_DWORD | pCUSTOM_HERITABLE
			);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propProtocolType, 
			npPROTOCOL_TYPE, 
			pTYPE_DWORD | pCUSTOM_HERITABLE
			);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propMessageCheckOnly,
			npMESSAGE_CHECK_ONLY,
			pTYPE_DWORD | pCUSTOM_HERITABLE
			);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propMessageUncomplete,
			npMESSAGE_UNCOMPLETE,
			pTYPE_DWORD | pCUSTOM_HERITABLE
			);


// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Task_Register( hROOT root ) { return CTask::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_ID
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end




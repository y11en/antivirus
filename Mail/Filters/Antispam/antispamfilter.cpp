// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  06 May 2005,  13:51 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- antispamfilter.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define AntispamFilter_VERSION ((tVERSION)1)
// AVP Prague stamp end



#pragma warning(disable: 4786)

// AVP Prague stamp begin( Includes for interface,  )
#include "antispamfilter.h"
// AVP Prague stamp end



#include <Prague/pr_wrappers.h>
#include <Extract/plugin/p_mailmsg.h>
#include <Mail/structs/s_mc.h>

#include "GetUserNameByProcessID.h"

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "AntispamFilter". Static(shared) property table variables
// AVP Prague stamp end


static tPROPID g_propMessageIsIncoming;
static tPROPID g_propMailerPID;
static tPROPID g_propMessageCheckOnly;

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CAntispamFilter::ObjectInitDone() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AntispamFilter::ObjectInitDone method" );

	// Place your code here
	m_pFilterImp = NULL;
	m_pFilterTrain = NULL;

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CAntispamFilter::ObjectPreClose() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AntispamFilter::ObjectPreClose method" );

	// Place your code here
	cAutoCS cs(m_lock, true);

	if ( m_pFilterImp )
	{
		delete m_pFilterImp;
		m_pFilterImp = NULL;
	}
	if ( m_pFilterTrain )
	{
		delete m_pFilterTrain;
		m_pFilterTrain = NULL;
	}

	return error;
}
// AVP Prague stamp end



enum ProcessResult_t
{
	pr_HAM,
	pr_PROBABLE_SPAM,
	pr_SPAM
};

const tCHAR *g_strSubjectsPrefixesClean[] = 
{
	"[?? Probable Spam]",
	"[!! SPAM]",
};
const tCHAR *g_strSubjectsPrefixesKAS[] = 
{
	"[?? Probable Spam] ",
	"[!! SPAM] ",
};
const tCHAR *g_strSubjectsPrefixes[] = 
{
	"[?? Probable Spam]  ",
	"[!! SPAM]  ",
};

void NormalyzeSubject(cStringObj& sz)
{

	// Вычленяем оригинальный сабджект из письма
	tDWORD idx;
	for( int i = 0; i < 2; i++ )
	{
		while( (idx = sz.find(g_strSubjectsPrefixes[i])) != cStringObj::npos )
			sz.erase(idx, strlen(g_strSubjectsPrefixes[i]));
		while( (idx = sz.find(g_strSubjectsPrefixesKAS[i])) != cStringObj::npos )
			sz.erase(idx, strlen(g_strSubjectsPrefixesKAS[i]));
		while( (idx = sz.find(g_strSubjectsPrefixesClean[i])) != cStringObj::npos )
			sz.erase(idx, strlen(g_strSubjectsPrefixesClean[i]));
	}
}

tERROR ChangeSubject(hOS os, ProcessResult_t pr)
{
	cERROR err = errOK;
	//////////////////////////////////////////////////////////////////////////
	//
	// Код изменения subject'а письма
	//
	{
		cERROR error = errOK; // Заводим локальную переменную, чтобы не выдвигать некритичную ошибку наверх
		cStringObj szNewSubject;
		cStringObj szOldSubject;
		cStringObj szUnchangedSubject;
		err = szOldSubject.assign(os, pgMESSAGE_SUBJECT);
		err = szUnchangedSubject.assign(os, pgMESSAGE_SUBJECT);
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// Формируем новую тему по формату "<subj_status>:<original_subj>"
			//
			{
				// Вычленяем оригинальный сабджект из письма
				NormalyzeSubject(szOldSubject);
				switch ( pr ) 
				{
				case pr_SPAM:
					szNewSubject = g_strSubjectsPrefixes[1];
					break;
				case pr_PROBABLE_SPAM:
					szNewSubject = g_strSubjectsPrefixes[0];
					break;
				case pr_HAM:
					break;
				}
				szNewSubject += szOldSubject;
				if ( szNewSubject != szUnchangedSubject )
					err = szNewSubject.copy(os, pgMESSAGE_SUBJECT);
				if ( PR_SUCC(err) )
					PR_TRACE( (os,prtNOTIFY,"CAntispamFilter::ChangeSubject => subject changed to: %S", (tWCHAR*)szNewSubject.c_str(cCP_UNICODE)) );
				else
					PR_TRACE( (os,prtNOTIFY,"CAntispamFilter::ChangeSubject => cannot change subject : %S", (tWCHAR*)szUnchangedSubject.c_str(cCP_UNICODE)) );
			}
			//
			// Формируем новую тему по формату "<subj_status>:<original_subj>"
			//
			//////////////////////////////////////////////////////////////////////////
		}
	}
	//
	// Код изменения subject'а письма
	//
	//////////////////////////////////////////////////////////////////////////
	return err;
}


// AVP Prague stamp begin( External (user called) interface method implementation, Process )
// Parameters are:
//! tERROR CAntispamFilter::Process( tDWORD* result, cSerializable* p_pProcessParams, hOBJECT p_hObjectToCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics ) {
tERROR CAntispamFilter::Process( tDWORD* result, cSerializable* p_pProcessParams, hOBJECT p_hObjectToCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics ) {
	tERROR error = errOK;
	tDWORD ret_val = 0;
	ProcessResult_t pr = pr_HAM;
	PR_TRACE_FUNC_FRAME( "AntispamFilter::Process method" );

	// Place your code here
	cAutoCS cs(m_lock, false); // Синхронизируемся с функциями изменения настроек
	if ( m_state != TASK_STATE_RUNNING )
		return errOPERATION_CANCELED;

	if ( !m_pFilterImp )
		return errOPERATION_CANCELED;
	
	{
		CAntispamCaller::CObjectToCheck_hObject hObj2Check(p_hObjectToCheck);
		CAntispamCaller::CObjectToCheck_hOS hObjOS2Check(&hObj2Check);
		CAntispamImplementer::CKLAntispamFilter::ProcessParams_t* pProcessParams = p_pProcessParams;
		CAntispamImplementer::CKLAntispamFilter::ProcessStatistics_t statistics;
		CAntispamImplementer::CKLAntispamFilter::ProcessStatistics_t* pStatistics =
			p_pStatistics ?
			(CAntispamImplementer::CKLAntispamFilter::ProcessStatistics_t*)p_pStatistics :
			&statistics;
		long double AntispamResult = 0;
		pStatistics->m_timeStart = (tDWORD)time(0);
		if ( p_hObjectToCheck && pStatistics->m_szUserName.empty() )
		{
			pStatistics->m_dwPID = p_hObjectToCheck->propGetDWord(g_propMailerPID);
			GetUserNameByProcessID(pStatistics->m_dwPID, pStatistics->m_szUserName);
		}
		error = m_pFilterImp->Process(&hObjOS2Check, pProcessParams, &AntispamResult, p_dwTimeOut, pStatistics);
		if ( PR_SUCC(error) )
		{
			*result |= mcv_MESSAGE_CHECKED;
			if ( AntispamResult > (long double)m_settings.m_dProbableSpamHighLevel/100 )
			{
				pr = pr_SPAM;
				*result |= mcv_MESSAGE_AS_SPAM;
				pStatistics->m_dwMailSpamCount++;
			}
			else if ( AntispamResult > (long double)m_settings.m_dProbableSpamLowLevel/100 )
			{
				pr = pr_PROBABLE_SPAM;
				*result |= mcv_MESSAGE_AS_PROBABLE_SPAM;
			}
			{
				pStatistics->m_dwMailCount++;
				pStatistics->m_dLastResult = AntispamResult;
				pStatistics->m_szLastObjectName.assign(hObjOS2Check.GetOS(), pgOBJECT_NAME);
				pStatistics->m_szLastMessageSubj.assign(hObjOS2Check.GetOS(), pgMESSAGE_SUBJECT);
				pStatistics->m_szLastMessageSender.assign(hObjOS2Check.GetOS(), pgMESSAGE_FROM);
				pStatistics->m_szLastMessageDate.assign(hObjOS2Check.GetOS(), pgMESSAGE_DATE);
				if ( !p_hObjectToCheck->propGetBool(g_propMessageCheckOnly) )
					m_statistics += *((cAntispamStatistics*)pStatistics);
				pStatistics->m_timeFinish = (tDWORD)time(0);
			}
			if ( pr != pr_HAM )
				ChangeSubject(hObjOS2Check.GetOS(), pr);
		}
	}
	if ( IID_IO == p_hObjectToCheck->propGetIID() )
		((hIO)p_hObjectToCheck)->Flush();

	GetStatistics(NULL);
	
	return error;
}
// AVP Prague stamp end

#define FORCE_SAVE_DATABASE(_filter)	\
{	\
	CAntispamImplementer::CKLAntispamFilter::TrainParams_t hSave;	\
	hSave.Action = cAntispamTrainParams::Train_End_Apply;	\
	_filter->Train(0, &hSave, 0, 0);	\
}


// AVP Prague stamp begin( External (user called) interface method implementation, Train )
// Parameters are:
//! tERROR CAntispamFilter::Train( cSerializable* p_pTrainParams, hOBJECT p_hObjectForCheck, tDWORD p_dwTimeOut ) {
//! tERROR CAntispamFilter::Train( cSerializable* p_pTrainParams, hOBJECT p_hObjectForCheck, tDWORD p_dwTimeOut ) {
tERROR CAntispamFilter::Train( cSerializable* p_pTrainParams, hOBJECT p_hObjectForCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AntispamFilter::Train method" );

	//////////////////////////////////////////////////////////////////////////
	//
	// Выстраиваем все вызовы Train в очередь
	//
	static PRLocker cs;
	PRAutoLocker _cs_train_(cs);
	//
	// Выстраиваем все вызовы Train в очередь
	//
	//////////////////////////////////////////////////////////////////////////


	if ( m_state != TASK_STATE_RUNNING )
		return errOPERATION_CANCELED;

	CAntispamImplementer::CKLAntispamFilter::ProcessStatistics_t statistics;
	CAntispamImplementer::CKLAntispamFilter::ProcessStatistics_t* pStatistics =
		p_pStatistics ?
		(CAntispamImplementer::CKLAntispamFilter::ProcessStatistics_t*)p_pStatistics :
		&statistics;
	CAntispamImplementer::CKLAntispamFilter::TrainParams_t* pTrainParams =
		(CAntispamImplementer::CKLAntispamFilter::TrainParams_t*)p_pTrainParams;
	pStatistics->m_bSettingsChanged = cFALSE;
	pStatistics->m_timeStart = (tDWORD)time(0);
	if ( p_hObjectForCheck && pStatistics->m_szUserName.empty() )
	{
		pStatistics->m_dwPID = p_hObjectForCheck->propGetDWord(g_propMailerPID);
		GetUserNameByProcessID(pStatistics->m_dwPID, pStatistics->m_szUserName);
	}
	if ( pTrainParams )
	{
		switch ( pTrainParams->Action )
		{
		case cAntispamTrainParams::Train_Begin :
			//////////////////////////////////////////////////////////////////////////
			//
			// Создаем новый экземпляр m_pFilterTrain
			//
			error = errOPERATION_CANCELED;
			if ( m_pFilterTrain )
			{
				//	delete m_pFilterTrain;
				// Не даем стартовать новому тренингу, пока не завершен старый
				return error;
			}
			m_pFilterTrain = new CAntispamImplementer::CKLAntispamFilter();
			if ( m_pFilterTrain )
			{
				cAutoCS cs(m_lock, false); // Синхронизируемся с функциями изменения настроек
				cStringObj szBayesTableFileName_Original = m_settings.m_szBayesTableFileName_Original;
				static int i = 0;
				m_settings.m_szBayesTableFileName_Original.format(cCP_ANSI, "trainbase_#%d", i++);
				CAntispamImplementer::CKLAntispamFilter::InitParams_t hInitParams;
				hInitParams.pAntispamSettings = &m_settings;
				hInitParams.persistance_storage = m_persistance_storage;
				hInitParams.hAntispamTask = sysGetParent(IID_ANY);
				error = m_pFilterTrain->Init(NULL, &hInitParams);
				m_settings.m_szBayesTableFileName_Original = szBayesTableFileName_Original;
			}
			if ( !PR_SUCC(error) )
				delete m_pFilterTrain;
			pStatistics->m_szLastObjectName = "Anti-Spam Training Wizard : Start training";
			pStatistics->m_dwLastMessageCategory = cAntispamReport::IsUnknown;
			pStatistics->m_dLastResult = -1;
			pStatistics->m_timeFinish = (tDWORD)time(0);
			return error;
			break;
			//
			// Создаем новый экземпляр m_pFilterTrain
			//
			//////////////////////////////////////////////////////////////////////////
		case cAntispamTrainParams::Train_End_Cancel :
			//////////////////////////////////////////////////////////////////////////
			//
			// Отменяем все внесенные изменения
			//
			error = errOK;
			if ( m_pFilterTrain )
			{
				cAutoCS cs(m_lock, true); // Синхронизируем функции изменения настроек
				if ( m_pFilterTrain->GetAddressList()->HasBeenChanged() ) 
					m_pFilterTrain->GetAddressList()->Rollback();
				delete m_pFilterTrain;
				m_pFilterTrain = NULL;
			}
			FORCE_SAVE_DATABASE(m_pFilterImp);
			pStatistics->m_szLastObjectName = "Anti-Spam Training Wizard : Cancel training";
			pStatistics->m_dwLastMessageCategory = cAntispamReport::IsUnknown;
			pStatistics->m_dLastResult = -1;
			pStatistics->m_timeFinish = (tDWORD)time(0);
			return error;
			break;
			//
			// Отменяем все внесенные изменения
			//
			//////////////////////////////////////////////////////////////////////////
		case cAntispamTrainParams::Train_End_Apply :
			//////////////////////////////////////////////////////////////////////////
			//
			// Применяем новую переобученную базу
			//
			error = errOPERATION_CANCELED;
			
			// список так же надо очистить
			m_szLastObjectNames.clear();

			if ( m_pFilterTrain )
			{
				error = errOK;
				if ( m_pFilterImp )
				{
					cAutoCS cs(m_lock, true); // Синхронизируем функции изменения настроек
					delete m_pFilterImp;
				}
				m_pFilterImp = m_pFilterTrain;
				m_pFilterTrain = NULL;
			}
			FORCE_SAVE_DATABASE(m_pFilterImp);
			pStatistics->m_szLastObjectName = "Anti-Spam Training Wizard : Apply training";
			pStatistics->m_dwLastMessageCategory = cAntispamReport::IsUnknown;
			pStatistics->m_dLastResult = -1;
			pStatistics->m_timeFinish = (tDWORD)time(0);
			if ( PR_SUCC(error) && m_pFilterImp->GetAddressList()->HasBeenChanged() ) 
			{
				pStatistics->m_bSettingsChanged = cTRUE;
				sysGetParent(IID_ANY)->sysSendMsg(pmc_TASK_SETTINGS_CHANGED,0,0,0,0);
			}
			return error;
			break;
			//
			// Применяем новую переобученную базу
			//
			//////////////////////////////////////////////////////////////////////////
		case cAntispamTrainParams::MergeDatabase :
			//////////////////////////////////////////////////////////////////////////
			//
			// Сливаем новую переобученную базу со старой
			//
			error = errOPERATION_CANCELED;
			if ( m_pFilterTrain )
			{
				error = errOK;
				pTrainParams->pFilter = m_pFilterImp;
				if ( m_pFilterImp )
				{
					m_pFilterTrain->Train(0, pTrainParams, 0, 0);
					cAutoCS cs(m_lock, true); // Синхронизируем функции изменения настроек
					delete m_pFilterImp;
				}
				m_pFilterImp = m_pFilterTrain;
				m_pFilterTrain = NULL;
			}
			FORCE_SAVE_DATABASE(m_pFilterImp);
			pStatistics->m_szLastObjectName = "Anti-Spam Training Wizard : Merge training";
			pStatistics->m_dwLastMessageCategory = cAntispamReport::IsUnknown;
			pStatistics->m_dLastResult = -1;
			pStatistics->m_timeFinish = (tDWORD)time(0);
			if ( PR_SUCC(error) && m_pFilterImp->GetAddressList()->HasBeenChanged() ) 
			{
				pStatistics->m_bSettingsChanged = cTRUE;
				sysGetParent(IID_ANY)->sysSendMsg(pmc_TASK_SETTINGS_CHANGED,0,0,0,0);
			}
			return error;
			break;
			//
			// Сливаем новую переобученную базу со старой
			//
			//////////////////////////////////////////////////////////////////////////
		default:
			break;
		}
	}

	CAntispamImplementer::CKLAntispamFilter* pFilterTrain = m_pFilterTrain ? m_pFilterTrain : m_pFilterImp;

	if ( !pFilterTrain )
		return errOPERATION_CANCELED;

	CAntispamCaller::CObjectToCheck_hObject hObj2Check(p_hObjectForCheck);
	CAntispamCaller::CObjectToCheck_hOS hObjOS2Check(&hObj2Check);
	hOS pOS = hObjOS2Check.GetOS();
	if ( pOS )
	{
		pStatistics->m_szLastObjectName.assign(pOS,    pgOBJECT_NAME);
		pStatistics->m_szLastMessageSubj.assign(pOS,   pgMESSAGE_SUBJECT);
		pStatistics->m_szLastMessageSender.assign(pOS, pgMESSAGE_FROM);
		pStatistics->m_szLastMessageDate.assign(pOS, pgMESSAGE_DATE);
	}
	//////////////////////////////////////////////////////////////////////////
	//
	//	Проверяем, не подсунули нам одно из последних 10 обученных писем
	//
	{
		cStringObj szLastObjectName;
		if ( pOS && !pOS->propGetBool(g_propMessageIsIncoming) )
			// Для исходящих писем ограничиваемся проверкой темы.
			szLastObjectName = pStatistics->m_szLastMessageSubj;
		else
			szLastObjectName = pStatistics->m_szLastObjectName;
		NormalyzeSubject(szLastObjectName);
		for ( tULONG i = 0; i < m_szLastObjectNames.count(); i++ )
		{
			if ( szLastObjectName == m_szLastObjectNames.at(i).sz )
			{
				if  (
					m_szLastObjectNames.at(i).action != cAntispamTrainParams::TrainAsHam &&
					pTrainParams->Action == cAntispamTrainParams::TrainAsHam
					)
				{
					// Пользователь явно хочет отменить последнее действие по обучению
					pTrainParams->Action = cAntispamTrainParams::RemoveFromSpam;
					error = pFilterTrain->Train(&hObjOS2Check, pTrainParams, p_dwTimeOut, pStatistics);
					pTrainParams->Action = cAntispamTrainParams::TrainAsHam;
					m_szLastObjectNames.remove(i);
					break;
				}
				else if  (
					m_szLastObjectNames.at(i).action != cAntispamTrainParams::TrainAsSpam &&
					pTrainParams->Action == cAntispamTrainParams::TrainAsSpam
					)
				{
					// Пользователь явно хочет отменить последнее действие по обучению
					pTrainParams->Action = cAntispamTrainParams::RemoveFromHam;
					error = pFilterTrain->Train(&hObjOS2Check, pTrainParams, p_dwTimeOut, pStatistics);
					pTrainParams->Action = cAntispamTrainParams::TrainAsSpam;
					m_szLastObjectNames.remove(i);
					break;
				}
				else
				{
					pStatistics->m_dwLastReason = asdr_HasBeenTrained;
					pStatistics->m_dLastResult = -2;
					pStatistics->m_timeFinish = (tDWORD)time(0);
					PR_TRACE( (this,prtNOTIFY,"CAntispamFilter::Train => message has been trained") );
					if ( pOS && pOS->propGetDWord(pgPLUGIN_ID) == PID_MAILMSG )
					{
						if (
							(pTrainParams->Action == cAntispamTrainParams::TrainAsHam) ||
							(pTrainParams->Action == cAntispamTrainParams::RemoveFromSpam)
							)
							// Стираем метки [!! SPAM] и [?? Probable Spam] из темы письма, указанного как pr_HAM
							ChangeSubject(hObjOS2Check.GetOS(), pr_HAM);
						else if (
							(pTrainParams->Action == cAntispamTrainParams::TrainAsSpam) ||
							(pTrainParams->Action == cAntispamTrainParams::RemoveFromHam)
							)
							// Устанавливаем метки [!! SPAM] на тему письма, указанного как pr_Spam
							ChangeSubject(hObjOS2Check.GetOS(), pr_SPAM);
					}
					// если письмо - подсунутое, и заказанное действие дублирует уже сделанное, то выходим
					PR_TRACE( (this,prtNOTIFY,"CAntispamFilter::Train => return sdr_HasBeenTrained") );
					return errOK;
				}
			}
		}
		LastObject_t hLastObject(szLastObjectName, pTrainParams->Action);
		m_szLastObjectNames.push_back(hLastObject);
		while ( m_szLastObjectNames.count() > 100 )
			m_szLastObjectNames.remove(0);
	}
	//
	//	Проверяем, не подсунули нам одно из последних 10 обученных писем
	//
	//////////////////////////////////////////////////////////////////////////

	error = pFilterTrain->Train(&hObjOS2Check, pTrainParams, p_dwTimeOut, pStatistics);
	if ( PR_SUCC(error) )
	{
		// m_statistics += *((cAntispamStatistics*)pStatistics); 
		// После тренировки нам надо лишь проапдейтить m_bNeedMore<...>ToTrain.
		m_statistics.m_dwSpamBaseSize = pStatistics->m_dwSpamBaseSize;
		m_statistics.m_dwHamBaseSize = pStatistics->m_dwHamBaseSize;
		m_statistics.m_bNeedMoreSpamToTrain = pStatistics->m_bNeedMoreSpamToTrain;
		m_statistics.m_bNeedMoreHamToTrain = pStatistics->m_bNeedMoreHamToTrain;
		if ( pFilterTrain == m_pFilterImp )
			FORCE_SAVE_DATABASE(m_pFilterImp);
	}
	pStatistics->m_timeFinish = (tDWORD)time(0);

	// На Win98 это действие при запущенном OE любой версии приводит к зависанию!
	// Поэтому сделаем это только для писем большого OU.
	if ( PR_SUCC(error) )
	{
		if ( pOS && pOS->propGetDWord(pgPLUGIN_ID) == PID_MAILMSG )
		{
			if (
				(pTrainParams->Action == cAntispamTrainParams::TrainAsHam) ||
				(pTrainParams->Action == cAntispamTrainParams::RemoveFromSpam)
				)
				// Стираем метки [!! SPAM] и [?? Probable Spam] из темы письма, указанного как pr_HAM
				ChangeSubject(hObjOS2Check.GetOS(), pr_HAM);
			else if (
				(pTrainParams->Action == cAntispamTrainParams::TrainAsSpam) ||
				(pTrainParams->Action == cAntispamTrainParams::RemoveFromHam)
				)
				// Устанавливаем метки [!! SPAM] на тему письма, указанного как pr_Spam
				ChangeSubject(hObjOS2Check.GetOS(), pr_SPAM);
		}
	}

	GetStatistics(NULL);

	//////////////////////////////////////////////////////////////////////////
	//
	// Если был изменен white_list, об этом надо уведомить TM
	//
	tBOOL bSettingsHasBeenChanged = cFALSE;
	{
		cAutoCS cs(m_lock, false); // Синхронизируемся с функциями изменения настроек
		bSettingsHasBeenChanged = (m_settings.m_bUseWhiteAddresses || m_settings.m_bUseBlackAddresses);
	}
	if ( PR_SUCC(error) )
		if ( bSettingsHasBeenChanged )
			if ( !m_pFilterTrain )
				if ( pFilterTrain->GetAddressList()->HasBeenChanged() )
				{
					if ( pStatistics ) pStatistics->m_bSettingsChanged = cTRUE;
					PR_TRACE( (this,prtNOTIFY,"CAntispamFilter::Train => pmc_TASK_SETTINGS_CHANGED") );
					sysGetParent(IID_ANY)->sysSendMsg(pmc_TASK_SETTINGS_CHANGED,0,0,0,0);
				}
	//
	// Если был изменен white_list, об этом надо уведомить TM
	//
	//////////////////////////////////////////////////////////////////////////

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR CAntispamFilter::SetState( tTaskRequestState p_state ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AntispamFilter::SetState method" );

	// Place your code here
	cAutoCS cs(m_lock, true); // Синхронизируем функции изменения настроек
	if ( !m_pFilterImp )
		return errOPERATION_CANCELED;

	m_state = p_state;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
//! tERROR CAntispamFilter::GetStatistics( cSerializable p_statistics ) {
tERROR CAntispamFilter::GetStatistics( cSerializable* p_statistics ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AntispamFilter::GetStatistics method" );

	// Place your code here
	if ( m_pFilterImp )
	{
		cAutoCS cs(m_lock, false); // Синхронизируемся с функциями изменения настроек
		cAntispamStatistics stat;
		if ( PR_SUCC(m_pFilterImp->GetStatistics(&stat)))
		{
			m_statistics.m_dwSpamBaseSize = stat.m_dwSpamBaseSize;
			m_statistics.m_dwHamBaseSize = stat.m_dwHamBaseSize;
			m_statistics.m_bNeedMoreSpamToTrain = stat.m_bNeedMoreSpamToTrain;
			m_statistics.m_bNeedMoreHamToTrain = stat.m_bNeedMoreHamToTrain;
		}
	}
	if ( p_statistics )
		error = p_statistics->assign(m_statistics, false);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR CAntispamFilter::SetSettings( const cSerializable* p_pSettings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AntispamFilter::SetSettings method" );

	// Place your code here
	cAntispamSettings m_settings_old;
	if ( !p_pSettings->isBasedOn(cAntispamSettings::eIID) )
		return errOBJECT_INCOMPATIBLE;

	{
		cAutoCS cs(m_lock, true);
		m_settings_old = m_settings;
		error = m_settings.assign(*p_pSettings,false);

		if ( 
			((cAntispamSettings*)p_pSettings)->m_bUseBayesian &&
			((cAntispamSettings*)p_pSettings)->m_szBayesTableFileName.empty()
			)
		{
			PR_TRACE( (this,prtDANGER,"CAntispamFilter::SetSettings => m_szBayesTableFileName.empty() !!!") );
			// При экспорте настроек во время удаления проекта по невыясненной причине
			// m_szBayesTableFileName_Original и m_szBayesTableFileName становятся пустыми (см. багу 19593 в TT).
			// Этот костыль вставлен, чтобы при установке более новой версии и подхвате испорченных настроек,
			// не было сбоя антиспама - сбрасываем имена файлов в дефолтовые значения.
			cAntispamSettings def;
			hTASKMANAGER tm = (hTASKMANAGER)sysGetParent(IID_ANY)->propGetObj(pgTASK_TM);
			if ( tm )
				tm->GetSettingsByLevel(AVP_PROFILE_ANTISPAM, SETTINGS_LEVEL_RESET, cFALSE, &def);
			m_settings.m_szBayesTableFileName_Original = def.m_szBayesTableFileName_Original;
			m_settings.m_szBayesTableFileName = def.m_szBayesTableFileName;
		}
	}

	if ( PR_SUCC(error) )
		error = Init(&m_settings);

	if ( PR_FAIL(error) )
	{
		cAutoCS cs(m_lock, true);
		m_settings = m_settings_old;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR CAntispamFilter::GetSettings( cSerializable* p_pSettings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AntispamFilter::GetSettings method" );

	// Place your code here
	cAutoCS cs(m_lock, false); // Синхронизируем с функциями изменения настроек
	error = p_pSettings->assign(m_settings, false);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Init )
// Parameters are:
tERROR CAntispamFilter::Init( const cSerializable* p_pSettings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AntispamFilter::Init method" );

	// Place your code here
	cAutoCS cs(m_lock, true); // Синхронизируем функции изменения настроек
	if ( m_pFilterImp )
	{
		delete m_pFilterImp;
		m_pFilterImp = NULL;
	}

	if ( !m_pFilterImp )
	{
		m_pFilterImp = new CAntispamImplementer::CKLAntispamFilter();
		if ( m_pFilterImp )
		{
			cStringObj szBayesTableFileName_Original = m_settings.m_szBayesTableFileName_Original;
			m_settings.m_szBayesTableFileName_Original = m_settings.m_szBayesTableFileName;
			CAntispamImplementer::CKLAntispamFilter::InitParams_t hInitParams;
			hInitParams.pAntispamSettings = &m_settings;
			hInitParams.persistance_storage = m_persistance_storage;
			hInitParams.hAntispamTask = sysGetParent(IID_ANY);
			error = m_pFilterImp->Init(NULL, &hInitParams);
			if ( PR_FAIL(error) )
			{
				delete m_pFilterImp;
				m_pFilterImp = NULL;
			}
			m_settings.m_szBayesTableFileName_Original = szBayesTableFileName_Original;
		}
		else
			error = errNOT_OK;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "AntispamFilter". Register function
tERROR CAntispamFilter::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(AntispamFilter_PropTable)
	mpLOCAL_PROPERTY_BUF( plTASK_PERSISTANCE_STORAGE, CAntispamFilter, m_persistance_storage, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(AntispamFilter_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(AntispamFilter)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(AntispamFilter)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN_EX(AntispamTaskAntispamFilter, AntispamFilter)
	mEXTERNAL_METHOD(AntispamFilter, Process)
	mEXTERNAL_METHOD(AntispamFilter, Train)
	mEXTERNAL_METHOD(AntispamFilter, SetState)
	mEXTERNAL_METHOD(AntispamFilter, GetStatistics)
	mEXTERNAL_METHOD(AntispamFilter, SetSettings)
	mEXTERNAL_METHOD(AntispamFilter, GetSettings)
	mEXTERNAL_METHOD(AntispamTaskAntispamFilter, Init)
mEXTERNAL_TABLE_END(AntispamFilter)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "AntispamFilter::Register method", &error );


//! 	error = CALL_Root_RegisterIFace(
//! 		root,                                   // root object
//! 		IID_ANTISPAMFILTER,                     // interface identifier
//! 		PID_ANTISPAMTASK,                       // plugin identifier
//! 		0x00000000,                             // subtype identifier
//! 		AntispamFilter_VERSION,                 // interface version
//! 		VID_DENISOV,                            // interface developer
//! 		NULL,                                   // internal(kernel called) function table
//! 		0,                                      // internal function table size
//! 		&e_AntispamFilter_vtbl,                 // external function table
//! 		(sizeof(e_AntispamFilter_vtbl)/sizeof(tPTR)),// external function table size
//! 		AntispamFilter_PropTable,               // property table
//! 		mPROPERTY_TABLE_SIZE(AntispamFilter_PropTable),// property table size
//! 		sizeof(CAntispamFilter)-sizeof(cObjImpl),// memory size
//! 		0                                       // interface flags
//! 	);
//! 	error = CALL_Root_RegisterIFace(
//! 		root,                                   // root object
//! 		IID_ANTISPAMFILTER,                     // interface identifier
//! 		PID_ANTISPAMTASK,                       // plugin identifier
//! 		0x00000000,                             // subtype identifier
//! 		AntispamFilter_VERSION,                 // interface version
//! 		VID_DENISOV,                            // interface developer
//! 		&i_AntispamFilter_vtbl,                 // internal(kernel called) function table
//! 		(sizeof(i_AntispamFilter_vtbl)/sizeof(tPTR)),// internal function table size
//! 		&e_AntispamFilter_vtbl,                 // external function table
//! 		(sizeof(e_AntispamFilter_vtbl)/sizeof(tPTR)),// external function table size
//! 		AntispamFilter_PropTable,               // property table
//! 		mPROPERTY_TABLE_SIZE(AntispamFilter_PropTable),// property table size
//! 		sizeof(CAntispamFilter)-sizeof(cObjImpl),// memory size
//! 		0                                       // interface flags
//! 	);
//! 	error = CALL_Root_RegisterIFace(
//! 		root,                                   // root object
//! 		IID_ANTISPAMFILTER,                     // interface identifier
//! 		PID_ANTISPAMTASK,                       // plugin identifier
//! 		0x00000000,                             // subtype identifier
//! 		AntispamFilter_VERSION,                 // interface version
//! 		VID_DENISOV,                            // interface developer
//! 		&i_AntispamFilter_vtbl,                 // internal(kernel called) function table
//! 		(sizeof(i_AntispamFilter_vtbl)/sizeof(tPTR)),// internal function table size
//! 		&e_AntispamFilter_vtbl,                 // external function table
//! 		(sizeof(e_AntispamFilter_vtbl)/sizeof(tPTR)),// external function table size
//! 		NULL,                                   // property table
//! 		0,                                      // property table size
//! 		sizeof(CAntispamFilter)-sizeof(cObjImpl),// memory size
//! 		0                                       // interface flags
//! 	);
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_ANTISPAMFILTER,                     // interface identifier
		PID_ANTISPAMTASK,                       // plugin identifier
		0x00000000,                             // subtype identifier
		AntispamFilter_VERSION,                 // interface version
		VID_DENISOV,                            // interface developer
		&i_AntispamFilter_vtbl,                 // internal(kernel called) function table
		(sizeof(i_AntispamFilter_vtbl)/sizeof(tPTR)),// internal function table size
		&e_AntispamFilter_vtbl,                 // external function table
		(sizeof(e_AntispamFilter_vtbl)/sizeof(tPTR)),// external function table size
		AntispamFilter_PropTable,               // property table
		mPROPERTY_TABLE_SIZE(AntispamFilter_PropTable),// property table size
		sizeof(CAntispamFilter)-sizeof(cObjImpl),// memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"AntispamFilter(IID_ANTISPAMFILTER) registered [%terr]",error) );
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
			&g_propMessageCheckOnly,
			npMESSAGE_CHECK_ONLY,
			pTYPE_DWORD | pCUSTOM_HERITABLE
			);


// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call AntispamFilter_Register( hROOT root ) { return CAntispamFilter::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end




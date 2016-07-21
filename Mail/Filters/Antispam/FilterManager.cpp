#pragma warning(disable: 4786)

#include "filtermanager.h"
#include "antispam_interface_imp2.h"

#define _GET_LAST_STATISTICS(h)	\
if (h)	\
{	\
	cAntispamStatistics statistics;	\
	cERROR err = h->GetStatistics((cSerializable*)(&statistics));	\
	if ( PR_SUCC(err) )	\
	{	\
		m_statistics = m_statistics_old;	\
		m_statistics += statistics;	\
	}	\
}	

#define GET_LAST_STATISTICS()	\
{	\
	cAutoFilter hAutoFilter(*this);	\
	hANTISPAMFILTER h = hAutoFilter.GetFilter();	\
	_GET_LAST_STATISTICS(h);	\
}


CFilterManager::CFilterManager(hOBJECT hParent):
	m_hParent(hParent),
	m_hAntispamFilter(NULL),
	m_state(TASK_STATE_UNKNOWN),
	m_dwCounter(0),
	m_hStartTimeout(NULL),
	m_hStopTimeout(NULL),
	m_csSettings(NULL),
	m_bValid(cFALSE),
	m_bStarted(cFALSE),
	m_bCanDestroy(cTRUE)
{
	m_hStartTimeout = CreateEvent(0,FALSE,FALSE,0);
	m_hStopTimeout  = CreateEvent(0,FALSE,FALSE,0);
	cERROR error    = m_hParent->sysCreateObjectQuick((hOBJECT*)&m_csSettings, IID_CRITICAL_SECTION, PID_WIN32LOADER_2LEVEL_CSECT);
	if(PR_SUCC(error))
		error = m_hParent->sysCreateObjectQuick((hOBJECT*)&m_csFilter, IID_CRITICAL_SECTION, PID_WIN32LOADER_2LEVEL_CSECT);
	m_bValid = m_hStartTimeout && m_hStopTimeout && PR_SUCC(error);
}

CFilterManager::~CFilterManager()
{
	if( m_hStartTimeout )
		CloseHandle(m_hStartTimeout);

	if( m_hStopTimeout )
		CloseHandle(m_hStopTimeout);

	DestroyFilter();
	if ( m_csSettings )
		m_csSettings->sysCloseObject();
	if ( m_csFilter )
		m_csFilter->sysCloseObject();
}

// вызывается только из cAutoFilter
hANTISPAMFILTER CFilterManager::GetFilter()
{
	if ( !m_bValid ) return NULL;
	m_csFilter->Enter(SHARE_LEVEL_WRITE);
	ResetEvent(m_hStartTimeout);
	SetEvent(m_hStopTimeout);
	m_dwCounter++;
	if ( !m_bStarted ) 
	{
		m_bStarted = cTRUE;
		Start("Antispam FilterManager");
	}
	return CreateFilter();
}

// вызывается только из cAutoFilter
void CFilterManager::ReleaseFilter()
{
	if ( !m_bValid ) return;
	if ( !--m_dwCounter && m_bCanDestroy )
	{
		ResetEvent(m_hStopTimeout);
		SetEvent(m_hStartTimeout);
	}
	m_csFilter->Leave(0);
}

// вызывается только из GetFilter, то есть из cAutoFilter (лочка ставится в GetFilter)
hANTISPAMFILTER CFilterManager::CreateFilter()
{
	if ( !m_bValid ) return NULL;
	if ( !m_hAntispamFilter )
	{
		cERROR error = errOK;
		error = m_hParent->sysCreateObject((hOBJECT*)&m_hAntispamFilter, IID_ANTISPAMFILTER, PID_ANTISPAMTASK);
		if ( PR_SUCC(error) )
			error = m_hAntispamFilter->sysCreateObjectDone();
		if ( PR_SUCC(error) )
		{
			cAutoCS cs(m_csSettings, false);
			error = m_hAntispamFilter->SetSettings(&m_settings);
		}
		if ( PR_SUCC(error) )
			error = m_hAntispamFilter->SetState((tTaskRequestState)m_state);
		if ( PR_FAIL(error) )
			DestroyFilter();
	}
	return m_hAntispamFilter;
}

void CFilterManager::DestroyFilter()
{
	if ( !m_bValid ) return;
	if ( !m_dwCounter )
	{
		cAutoCS cs(m_csFilter, true);
		if ( m_hAntispamFilter )
		{
			cAntispamStatistics statistics;
			cERROR err = m_hAntispamFilter->GetStatistics((cSerializable*)(&statistics));
			if ( PR_SUCC(err) )
			{
				m_statistics_old = m_statistics_old;
				m_statistics_old += statistics;
			}
			m_hAntispamFilter->sysCloseObject();
			m_hAntispamFilter = NULL;
		}
	}
}

tERROR CFilterManager::_Run()
{
	if ( !m_bValid ) return errNOT_OK;
	DWORD dwRes = 0;
	do
	{
		HANDLE hTimeout[]  = 
		{ 
			m_hStopEvent, 
			m_hStopTimeout,
			m_hStartTimeout 
		};
		dwRes = WaitForMultipleObjects( 
			sizeof(hTimeout)/sizeof(HANDLE), 
			hTimeout, 
			FALSE, 
			INFINITE 
			);
		if ( dwRes == WAIT_OBJECT_0 )         // нам сказали "Остановить работу"
			break;
		if ( dwRes == WAIT_OBJECT_0 + 1 )     // нам сказали "Отменить таймаут"
			continue;

		while (true)
		{
			// Начинаем таймаут
			dwRes = WaitForMultipleObjects( 
				sizeof(hTimeout)/sizeof(HANDLE), 
				hTimeout, 
				FALSE, 
				60000 
				);
			if ( dwRes == WAIT_OBJECT_0 )     // нам сказали "Остановить работу"
				break;
			if ( dwRes == WAIT_OBJECT_0 + 1 ) // нам сказали "Отменить таймаут"
				break;
			if ( dwRes == WAIT_OBJECT_0 + 2 ) // нам сказали "Начать таймаут"
				continue;
			else if ( dwRes == WAIT_TIMEOUT ) // Таймаут истек
				DestroyFilter();
			break;
		}
	}
	while ( dwRes != WAIT_OBJECT_0 );
	
	return errOK;
}

tERROR CFilterManager::GetStatistics ( cSerializable* p_statistics )
{
	return p_statistics->assign(m_statistics, false);
}
tERROR CFilterManager::SetSettings ( const cSerializable* p_pSettings )
{
	cERROR err;
	cAntispamSettings m_settings_old;
	{
		cAutoCS cs(m_csSettings, true);
		m_settings_old = m_settings;
		err = m_settings.assign(*p_pSettings, false);
	}

	cAutoFilter hAutoFilter(*this); // << Вот тут настройки будут переданы в фильтр! Если они окажутся некорректными - мы узнаем об этом в следующей строчке!
	hANTISPAMFILTER h = hAutoFilter.GetFilter();
	if ( h )
		err = h->SetSettings(p_pSettings);
	if ( !h || PR_FAIL(err) )
	{
		cAutoCS cs(m_csSettings, true);
		m_settings = m_settings_old;
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}
	GET_LAST_STATISTICS();
	return err;
}
tERROR CFilterManager::GetSettings ( cSerializable* p_pSettings )
{
	cAutoCS cs(m_csSettings, false);
	return p_pSettings->assign(m_settings, false);
}
tERROR CFilterManager::SetState ( tTaskRequestState p_state )
{
	m_state = (tTaskState)p_state;
	cAutoCS cs(m_csFilter, false);
	if ( m_hAntispamFilter ) 
		return m_hAntispamFilter->SetState(p_state);
	return errOK;
}
tERROR CFilterManager::Process ( tDWORD* result, cSerializable* p_pProcessParams, hOBJECT p_hObjectToCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics )
{
	cAutoFilter hAutoFilter(*this);
	hANTISPAMFILTER h = hAutoFilter.GetFilter();
	if ( !h ) return errOBJECT_CANNOT_BE_INITIALIZED;
	cERROR err = h->Process(result, p_pProcessParams, p_hObjectToCheck, p_dwTimeOut, p_pStatistics);
	if ( PR_SUCC(err) )
	{
		GET_LAST_STATISTICS();
		if ( PR_SUCC(err) && (!p_pStatistics || ((cAntispamStatisticsEx*)p_pStatistics)->m_bSettingsChanged) )
		{
			cAutoCS cs(m_csSettings, true);
			err = h->GetSettings(&m_settings);
		}
	}
	return err;
}
tERROR CFilterManager::Train ( cSerializable* p_pTrainParams, hOBJECT p_hObjectForCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics )
{
	cAutoFilter hAutoFilter(*this);
	hANTISPAMFILTER h = hAutoFilter.GetFilter();
	if ( !h ) return errOBJECT_CANNOT_BE_INITIALIZED;

	CAntispamImplementer::CKLAntispamFilter::TrainParams_t* pTrainParams =
		(CAntispamImplementer::CKLAntispamFilter::TrainParams_t*)p_pTrainParams;
	switch ( pTrainParams->Action )
	{
	case cAntispamTrainParams::Train_Begin :
		m_bCanDestroy = cFALSE;
		break;
	case cAntispamTrainParams::Train_End_Cancel :
	case cAntispamTrainParams::Train_End_Apply :
	case cAntispamTrainParams::MergeDatabase :
		m_bCanDestroy = cTRUE;
		break;
	}
	cERROR err = h->Train(p_pTrainParams, p_hObjectForCheck, p_dwTimeOut, p_pStatistics);
	if ( PR_SUCC(err) )
	{
		GET_LAST_STATISTICS();
		if ( PR_SUCC(err) && (!p_pStatistics || ((cAntispamStatisticsEx*)p_pStatistics)->m_bSettingsChanged) )
		{
			cAutoCS cs(m_csSettings, true);
			err = h->GetSettings(&m_settings);
		}
		h->sysSendMsg( pmc_ANTISPAM_HAS_BEEN_TRAINED, 0,0,0,0 );
	}
	return err;
}

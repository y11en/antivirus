// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  04 April 2007,  19:28 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- avs.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define AVS_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_ktrace.h>
#include <Prague/iface/i_loader.h> 
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>
#include <Prague/plugin/p_win32loader.h>

#include <AV/plugin/p_avs.h>
#include <ProductCore/plugin/p_qb.h>

#include <Updater/updater.h>

// AVP Prague stamp end


// AVP Prague stamp begin( Includes for interface,  )
#include "avs.h"
// AVP Prague stamp end

#include <Prague/plugin/p_vmarea.h>

#include <string.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <Prague/iface/e_ktime.h> 
	#include <Prague/iface/e_ktrace.h>
	#include <Prague/iface/e_loader.h> 
	#include <Prague/plugin/p_win32_nfio.h>
	#include <Prague/plugin/p_vmarea.h>

	#include <AV/plugin/p_avpmgr.h> 
	#include <AV/plugin/p_avlib.h> 
#if defined (_WIN32)
	#include <Prague/plugin/p_fsdrvplgn.h>
#endif

IMPORT_TABLE_END

#define THREATS_REPORT                 "detected.rpt"
#define THREATS_REPORT_INDEX           "detected.idx"

#define THREATS_INIT_COMMAND           1

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "AVS". Static(shared) property table variables
// AVP Prague stamp end



tPROPID	AVSImpl::m_propIsRemote;

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
tERROR AVSImpl::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter AVS::ObjectInitDone method" );

	// Place your code here
	m_AVP3hnd = CreateEngineHnd_AVP3(this);
	m_KLAVhnd = CreateEngineHnd_KLAV(this);

	sysCreateObjectQuick((hOBJECT*)&m_avspm, IID_AVSPM, PID_AVSPM);

	m_bIsRemote = propGetDWord(m_propIsRemote);

	if( PR_SUCC(error) )
	{
		if( m_tm )
		{
			sysRegisterMsgHandler(pmc_AVS_UPDATE, rmhDECIDER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
			sysRegisterMsgHandler(pmc_MATCH_EXCLUDES, rmhDECIDER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
			sysRegisterMsgHandler(pmc_UPDATER, rmhDECIDER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
			sysRegisterMsgHandler(pmc_QBSTORAGE, rmhLISTENER, m_tm, IID_ANY, PID_QB, IID_ANY, PID_ANY );

			if( !m_bIsRemote )
			{
				sysRegisterMsgHandler(pmc_AVS_SYNC_THREATS, rmhLISTENER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
				sysRegisterMsgHandler(pmc_AVS_QB_ACTION, rmhDECIDER, g_root, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
				sysRegisterMsgHandler(pmc_UPDATE_THREATS_LIST, rmhLISTENER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
				sysRegisterMsgHandler(pmc_THREATS_ADD_TO_QUARANTINE, rmhLISTENER, m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
			}
		}
	}

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&m_treatslock, IID_CRITICAL_SECTION);

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&m_nos, IID_OS, PID_NATIVE_FIO);

	if( PR_SUCC(error) )
	{
		cThreadPoolBase::init(this, 3);
		cThreadPoolBase::do_command(THREATS_INIT_COMMAND);
		PR_TRACE( (this, prtALWAYS_REPORTED_MSG, "avs\tAVS has been started") );
	}

	PR_TRACE_A1( this, "Leave AVS::ObjectInitDone method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Parameters are:
tERROR AVSImpl::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter AVS::ObjectPreClose method" );

	// Place your code here
	cThreadPoolBase::de_init();

	for(tDWORD i = 0; i < m_vecTreats.size(); i++)			
		m_vecTreats[i]->sysCloseObject();

	m_vecTreats.clear();

	if( m_hProcMon )
		m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hProcMon);

	delete m_AVP3hnd;
	delete m_KLAVhnd;

	PR_TRACE( (this, prtALWAYS_REPORTED_MSG, "avs\tAVS has been stopped") );
	
	PR_TRACE_A1( this, "Leave AVS::ObjectPreClose method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



tERROR AVSImpl::on_command(tDWORD command, tPTR ctx)
{
	if( command = THREATS_INIT_COMMAND )
		InitTreatsInfo();

	return errOK;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
tERROR AVSImpl::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter AVS::MsgReceive method" );

	// Place your code here
	switch( p_msg_cls_id )
	{
	case pmc_AVS_UPDATE:
		error = Update((cSerializable*)p_par_buf);
		break;

	case pmc_UPDATER:
		if( p_msg_id == pm_COMPONENT_PREINSTALLED )
			error = Update((cSerializable*)p_par_buf);
		break;

	case pmc_AVS_SYNC_THREATS:
	case pmc_UPDATE_THREATS_LIST:
	case pmc_THREATS_ADD_TO_QUARANTINE:
		{
			cInfectedObjectInfo* pObject = (cInfectedObjectInfo*)p_par_buf;

			cAutoObj<cAVSSession> avsSession;
			error = CreateSession(avsSession, (cTask*)this,
				ENGINE_DEFAULT, OBJECT_ORIGIN_GENERIC);

			if( PR_SUCC(error) )
			{
				cAVSSession* pSession = avsSession;
				if( p_msg_cls_id == pmc_THREATS_ADD_TO_QUARANTINE )
					error = AddThreatToQuaranine((AVSSession*)pSession, pObject);
				else
					error = UpdateTreatInfo((AVSSession*)pSession, pObject);
			}
		}
		break;

	case pmc_AVS_QB_ACTION:	
		{
			cAskQBAction* pRequest = (cAskQBAction*)p_par_buf;
			if( pRequest && pRequest->isBasedOn(cAskQBAction::eIID)  )
			{
				pRequest->m_hSource = (cIO*)p_ctx;
				sysSendMsg(pmcASK_ACTION, pRequest->m_nActionId, NULL, pRequest, SER_SENDMSG_PSIZE);
				error = errOK_DECIDED;
			}
		}
		break;

	case pmc_MATCH_EXCLUDES:
		{
			error = MatchExcludes((cSerializable*)p_par_buf);
			if( error == errOK_DECIDED )
			{
				cDetectObjectInfo * pInfo = (cDetectObjectInfo*)p_par_buf;
				pInfo->m_nObjectStatus = OBJSTATUS_NOTPROCESSED;
				pInfo->m_nDescription = NPREASON_MASKS;
			}
		}
		break;

	case pmc_QBSTORAGE:
		if( p_par_buf_len && *p_par_buf_len == sizeof(tOBJECT_ID))
			SyncThreatsWhithQB(p_msg_id, *(tOBJECT_ID*)p_par_buf);
		break;

	default:
		error = m_AVP3hnd->OnMsgReceive(p_msg_cls_id, p_msg_id, p_send_point, p_ctx, p_par_buf, p_par_buf_len);
	}

	PR_TRACE_A1( this, "Leave AVS::MsgReceive method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR AVSImpl::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter AVS::SetSettings method" );

	// Place your code here
	cAutoCS locker(m_lock, true);

	tDWORD nOldDetectMask = m_settings.m_nDetectMask;
	tBOOL bMultiThreaded = m_settings.m_bMultiThreaded;
	tBOOL bUpdate = cFALSE;

	error = p_settings->copy(m_settings, false);
	
	if( PR_SUCC(error) )
	{
		if( m_settings.m_sBaseFolder.empty() )
		{
			m_settings.m_sBaseFolder = "%Bases%";
			sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING,
				(hOBJECT)cAutoString(m_settings.m_sBaseFolder), 0, 0);
		}

		if( m_settings.m_sDataFolder.empty() )
		{
			m_settings.m_sDataFolder = "%Data%";
			sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING,
				(hOBJECT)cAutoString(m_settings.m_sDataFolder), 0, 0);
		}

		m_settings.m_sBaseFolder.check_last_slash();
		m_settings.m_sDataFolder.check_last_slash();

		PR_TRACE(( this, prtIMPORTANT, "avs\tSetSettings: BaseFolder %S", m_settings.m_sBaseFolder.data()));
		PR_TRACE(( this, prtIMPORTANT, "avs\tSetSettings: DataFolder %S", m_settings.m_sDataFolder.data()));

		tDWORD nCount = m_settings.m_aExcludes.size();
		for(tDWORD i = 0; i < nCount; i++)
		{
			cDetectExclude& pExclude = m_settings.m_aExcludes[i];
			if( pExclude.m_Object.m_strMask.empty() )
				continue;

			sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_SYSTEM_ONLY_STRING,
				(hOBJECT)cAutoString(pExclude.m_Object.m_strMask), 0, 0);

			cIOObj io(*this, cAutoString(pExclude.m_Object.m_strMask), fACCESS_READ, fOMODE_FOLDER_AS_IO);
			if( PR_SUCC(io.last_error()) && (io->propGetDWord(pgOBJECT_ATTRIBUTES) & fATTRIBUTE_DIRECTORY) )
				pExclude.m_Object.m_strMask.check_last_slash();

			if( !pExclude.m_VerdictPath.m_strMask.empty() )
				sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_SYSTEM_ONLY_STRING,
				(hOBJECT)cAutoString(pExclude.m_VerdictPath.m_strMask), 0, 0);

			cStrObj strTypes;
			if( pExclude.m_nTriggers & cDetectExclude::fTaskList )
			{
				for(tDWORD j = 0; j < pExclude.m_aTaskList.size(); j++)
					strTypes += pExclude.m_aTaskList.at(j), strTypes += L';';
			}

			PR_TRACE((this, prtIMPORTANT, "avs\t\texclude: %s object(%S) verdict(%S) tasks(%S)",
				pExclude.m_bEnable ? "enabled" : "disabled",
				pExclude.m_Object.m_strMask.data(),
				pExclude.m_strVerdictMask.data(), strTypes.data()));
		}

		if( m_bSettings && (nOldDetectMask != m_settings.m_nDetectMask) )
			DiscardTreatsByDetectMask();

		if( !m_bSettings )
			UpdateStatistics();
		else if( bMultiThreaded != m_settings.m_bMultiThreaded )
			bUpdate = cTRUE;

		m_bSettings = cTRUE;
	}

	locker.unlock();

	if( bUpdate )
		Update(NULL);

	if( PR_SUCC(error) )
		sysSendMsg(pmc_AVS, pm_SETTINGS_CHANGED, NULL, NULL, NULL);

	PR_TRACE_A1( this, "Leave AVS::SetSettings method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR AVSImpl::GetSettings( cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( this, "Enter AVS::GetSettings method" );

	// Place your code here
/*	cAutoCS locker(m_lock, true);
	error = m_settings.copy(*p_settings, true);
*/
	PR_TRACE_A1( this, "Leave AVS::GetSettings method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR AVSImpl::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( this, "Enter AVS::AskAction method" );

	// Place your code here

	PR_TRACE_A1( this, "Leave AVS::AskAction method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR AVSImpl::SetState( tTaskState p_state )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter AVS::SetState method" );

	// Place your code here
	PR_TRACE( (this, prtALWAYS_REPORTED_MSG, "avs\tAVS SetState(%d)", p_state) );

	PR_TRACE_A1( this, "Leave AVS::SetState method, ret %terr", error );
	return warnTASK_STATE_CHANGED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR AVSImpl::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( this, "Enter AVS::GetStatistics method" );

	// Place your code here
	if( !p_statistics )
		return errPARAMETER_INVALID;

	if( p_statistics->isBasedOn(cAVSPerformance::eIID) )
	{
		cAVSStatistics* pInfo = (cAVSStatistics*)p_statistics;

		if( pInfo->isBasedOn(cAVSStatistics::eIID) )
		{
			cAVSPerformance pPerf = *(cAVSPerformance*)p_statistics;
			error = pInfo->assign(m_statistics, true);
			*(cAVSPerformance*)pInfo = pPerf;

			tDWORD tmCurrent = cDateTime::now_utc();
			tDWORD tmBasesDate = m_statistics.m_tmBasesDate;
			pInfo->m_nBasesDateDaysOld = tmCurrent >= tmBasesDate ? (tmCurrent - tmBasesDate)/(24*60*60) : 0;
		}

		if( PR_SUCC(error) )
		{
			pInfo->m_nObjectOrigin = OBJECT_ORIGIN_UNKNOWN;
			if( pInfo->m_nCookie )
				for(int i = 0; i < 3; i++)
				{
					tLONG nDif = m_origin_times[i] - pInfo->m_nCookie;
					if( nDif > -3000 && nDif < 3000 )
					{
						pInfo->m_nObjectOrigin = (enObjectOrigin)i;
						break;
					}
				}
			error = GetPerformance(pInfo);
		}
	}

	PR_TRACE_A1( this, "Leave AVS::GetStatistics method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

tERROR AVSImpl::GetPerformance(cAVSPerformance* pData)
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVSPM::GetPerformance method" );

	// Place your code here
	tDWORD nDemandLastTime = pData->m_nCookie;
	tDWORD nDemandWorkingTime = pData->m_nWorkingTickCount;

	tDWORD nCurrentTime = PrGetTickCount();
	tDWORD nPerformance = 100;
	
	tDWORD nTotalWorkingTime = m_TotalWorkingTime;
	if( m_ProcessingCounter )
		nTotalWorkingTime += nCurrentTime - m_EnterToProcessTime;

	if( nCurrentTime != nDemandLastTime )
	{
		nPerformance = (nTotalWorkingTime - nDemandWorkingTime) * 100 /(nCurrentTime - nDemandLastTime);

		if( nPerformance > 100 )
			nPerformance = 100;
	}

	pData->m_nCookie = nCurrentTime;
	pData->m_nWorkingTickCount = nTotalWorkingTime;
	pData->m_nPerformance = nPerformance;

	return error;
}

tERROR AVSImpl::UpdateStatistics()
{
	cAutoCS locker(m_lock, true);

	m_statistics.m_errLoadBases = errOK;
	m_statistics.m_nBasesState = 0;
	m_statistics.m_nNumRecords = 0;
	m_statistics.m_tmBasesDate = 0;

	bool bSomeLoaded = false;
	for(int i = 0; i < MAX_ENGINES; i++)
	{
		CAVSEngineMng& engmng = m_vecEngins[i];
		if( PR_SUCC(engmng.m_dwState) )
		{
			bSomeLoaded = true;
			m_statistics.m_nBasesState |= bsLoaded;

			if( engmng.m_pEngine )
			{
				cAVP3BasesInfo info;
				if( PR_SUCC(engmng.m_pEngine->GetBasesInfo(info)) )
				{
					m_statistics.m_nNumRecords += info.m_dwViruseCount;

					tDWORD tmBases = cDateTime(&info.m_dtBasesDate);
					if( tmBases > m_statistics.m_tmBasesDate )
						m_statistics.m_tmBasesDate = tmBases;
				}
			}
		}
		else if( engmng.m_dwState != errNOT_READY )
		{
			m_statistics.m_nBasesState |= bsCorrupted;
			if( PR_SUCC(m_statistics.m_errLoadBases) )
				m_statistics.m_errLoadBases = engmng.m_dwState;
		}
	}

	if( !bSomeLoaded )
	{
		cAVP3BasesInfo info;
		cAutoObj<cAVP3Info> avp_info;
		tERROR error = sysCreateObjectQuick(avp_info, IID_AVP3INFO);
		if( PR_SUCC(error) )
			error = avp_info->GetBasesInfo(cAutoString(m_settings.m_sBaseFolder), &info);

		if( PR_SUCC(error) )
		{
			m_statistics.m_nNumRecords = info.m_dwViruseCount;
			m_statistics.m_tmBasesDate = cDateTime(&info.m_dtBasesDate);
		}
		else
			PR_TRACE((this, prtIMPORTANT, "avs\tAVSImpl::GetBasesInfo error=%terr", error));
	}

	return errOK;
}

// AVP Prague stamp begin( External (user called) interface method implementation, Update )
// Parameters are:
tERROR AVSImpl::Update( const cSerializable* p_update_info )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter AVS::Update method" );

	// Place your code here
	cAVSUpdateInfo pTemp;
	if( !p_update_info )
	{
		pTemp.m_eEngineMask = ENGINE_AVP3;
		p_update_info = &pTemp;
	}
	else if( p_update_info->isBasedOn(cUpdaterUpdatedFileList::eIID) )
	{
		cUpdaterUpdatedFileList *updatedFiles = (cUpdaterUpdatedFileList*)p_update_info;
		const cStrObj &sId = updatedFiles->m_selectedComponentIdentidier;

		if( sId == cUPDATE_CATEGORY_AVS )
			pTemp.m_eEngineMask = ENGINE_AVP3;
		else if( sId == cUPDATE_CATEGORY_Emulator )
			pTemp.m_eEngineMask = ENGINE_EMUL;
		else if( sId == cUPDATE_CATEGORY_WebMonitoring )
			pTemp.m_eEngineMask = ENGINE_STREAM;
		else if( sId == cUPDATE_CATEGORY_KLAVA )
			pTemp.m_eEngineMask = ENGINE_KLAV;
		else
			return errOK;

		pTemp.m_sSuffix = updatedFiles->m_updateSuffix;

		bool bSomeChanged = false;
		for(tDWORD i = 0, size = updatedFiles->m_files.size(); i < size; ++i)
		{
			const cUpdaterUpdatedFile &file = updatedFiles->m_files[i];
			if( file.m_status != cUpdaterUpdatedFile::unchanged )
			{
				bSomeChanged = true;
				if( file.m_replaceMode == cUpdaterUpdatedFile::suffix )
					pTemp.m_aFiles.push_back(file.m_filename);
			}
		}

		if( !bSomeChanged )
			return errOK;

		p_update_info = &pTemp;
	}

	if( !p_update_info->isBasedOn(cAVSUpdateInfo::eIID) )
		return errPARAMETER_INVALID;

	cAVSUpdateInfo* pInfo = (cAVSUpdateInfo*)p_update_info;

	tDWORD eMask = pInfo->m_eEngineMask;
	for(int i = 0; i < MAX_ENGINES; i++)
	{
		enEngineType eType = (enEngineType)(1 << i);
		if( !(eMask & eType) )
			continue;

		CAVSEngineMng& engmng = GetEngineMng(eType);

		CAVSEngine* pEngine = NULL;
		error = CreateEngine(eType, &pEngine, pInfo);

		bool bApplied = false;
		CAVSEngine* hToDestroy = NULL;
		cAutoObj<cEvent> hWaiter;

		if( PR_SUCC(error) )
		{
			cAutoCS locker(m_lock, true);
			if( engmng.m_nTaskRef )
			{
				engmng.m_dwState = errOK;
				if( engmng.m_pEngine )
				{
					if( !engmng.m_pEngine->m_nLockRef )
						hToDestroy = engmng.m_pEngine;
					else if( eType != ENGINE_AVP3 )
					{
						sysCreateObjectQuick(hWaiter, IID_EVENT);
						engmng.m_pEngine->m_hWaiter = hWaiter;
					}
				}

				engmng.m_pEngine = pEngine;
				bApplied = true;
			}
			
			if( PR_FAIL(engmng.m_dwState) )
				engmng.m_dwState = errNOT_READY;
		}

		if( !bApplied && pEngine )
			DestroyEngine(pEngine);
		else if( hToDestroy )
			DestroyEngine(hToDestroy);
		else if( !!hWaiter )
			hWaiter->Wait(cSYNC_INFINITE);
	}

	UpdateStatistics();

	PR_TRACE_A1( this, "Leave AVS::Update method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



CAVSEngineMng& AVSImpl::GetEngineMng(enEngineType eType)
{
	for(int i = 0; i < MAX_ENGINES; i++)
		if( eType == (1<<i) )
			return m_vecEngins[i-1];

	return m_vecEngins[0];
}

tERROR AVSImpl::CreateEngine(enEngineType eType, CAVSEngine** p_engine, cAVSUpdateInfo* pInfo)
{
	CAVSEngineHnd *enghnd = (eType == ENGINE_AVP3) ? m_AVP3hnd : m_KLAVhnd;

	CAVSEngine* engine = enghnd->CreateEngine(eType);
	engine->m_avs = this;
	engine->m_engtype = eType;
	engine->m_enghnd = enghnd;
	engine->m_nLockRef = 0;
	engine->m_hCtxObj = NULL;
	engine->m_hWaiter = NULL;
	engine->m_iChecker = engine->m_iSwift = NULL;

	PR_TRACE((this, prtIMPORTANT, "avs\tLoad engine(%d) begin...", engine->m_engtype));

	tERROR error = errOK;
	if( !pInfo )
	{
		const char* sCategory = (eType == ENGINE_EMUL) ? cUPDATE_CATEGORY_Emulator :
			(eType == ENGINE_STREAM) ? cUPDATE_CATEGORY_WebMonitoring : cUPDATE_CATEGORY_AVS;

		cBasesLoadLocker locker(*this, sCategory, cBasesLoadLocker::withLock);
		error = engine->Load(pInfo);
	}
	else
		error = engine->Load(pInfo);

	if( PR_SUCC(error) && engine->m_hCtxObj )
	{
		tERROR error = engine->m_hCtxObj->sysCreateObject((hOBJECT*)&engine->m_iChecker,
			IID_ICHECKER2, PID_ICHECKER2);

		if( PR_SUCC(error) )
		{
			cStringObj sfdb_path = m_settings.m_sDataFolder;
			sfdb_path += "sfdb.dat";
			if( PR_FAIL(error = sfdb_path.copy(engine->m_iChecker, plDB_PATHNAME)) )
				PR_TRACE(( this, prtIMPORTANT, "avs\tplDB_PATHNAME set failed %terr", error));
		}
		else
			PR_TRACE(( this, prtIMPORTANT, "avs\tCreateObject IID_ICHECKER2 failed %terr", error));

		engine->m_hCtxObj->sysCreateObjectQuick((hOBJECT*)&engine->m_iSwift,
			IID_ICHECKER2, PID_ICHECKERSA);
	}

	if( PR_SUCC(error) )
		error = engine->Init();

	cAVP3BasesInfo info;
	hOBJECT hCtx = pInfo ? NULL : (hOBJECT)this;

	if( PR_SUCC(error) )
	{
		engine->GetBasesInfo(info);

//		if( PR_FAIL(error = sysSendMsg(pmc_AVS, pm_BASES_PRELOAD, hCtx, &info, SER_SENDMSG_PSIZE)) )
//			PR_TRACE((this, prtIMPORTANT, "avs\tLoad engine(%d) denied %terr", error));
	}

	if( PR_FAIL(error) )
	{
		PR_TRACE((this, prtIMPORTANT, "avs\tLoad engine(%d) failed %terr", engine->m_engtype, error));
		
		DestroyEngine(engine);
		sysSendMsg(pmc_AVS, pm_BASES_LOADFAILED, hCtx, &info, SER_SENDMSG_PSIZE);

		return error;
	}

	*p_engine = engine;

	sysSendMsg(pmc_AVS, pm_BASES_LOADED, hCtx, &info, SER_SENDMSG_PSIZE);
	PR_TRACE((this, prtIMPORTANT, "avs\tLoad engine(%d) done", engine->m_engtype));
	return error;
}

tERROR AVSImpl::DestroyEngine(CAVSEngine* hEngine)
{
	cEvent* hWaiter = hEngine->m_hWaiter;
	hEngine->Unload();
	delete hEngine;

	if( hWaiter )
		hWaiter->SetState(cTRUE);

	return errOK;
}

tERROR AVSImpl::LockEngines(tDWORD eMask, tBOOL bLock, enEngineType* eBaseEngine)
{
	cAutoCS locker(m_lock, true);

	tERROR error = errNOT_FOUND;
	for(int i = 0; i < MAX_ENGINES; i++)
	{
		enEngineType eType = (enEngineType)(1 << i);
		if( !(eMask & eType) )
			continue;

		if( error == errNOT_FOUND )
			error = errOK;

		CAVSEngineMng& engmng = GetEngineMng(eType);

		if( bLock )
		{
			if( !*eBaseEngine )
				*eBaseEngine = eType;

			engmng.m_nTaskRef++;

			if( !engmng.m_pEngine && engmng.m_dwState == errNOT_READY )
			{
				PR_TRACE((this, prtIMPORTANT, "avs\tCreating engine (type 0x%04x)...", eType));
				engmng.m_dwState = CreateEngine(eType, &engmng.m_pEngine, NULL);
				PR_ASSERT_MSG(PR_SUCC(engmng.m_dwState), "CreateEngine failed");
			}

			if( PR_FAIL(engmng.m_dwState) && PR_SUCC(error) )
			{
				error = engmng.m_dwState;
				PR_TRACE((this, prtERROR, "avs\tNo engine (%terr)", engmng.m_dwState));
			}
		}
		else
		{
			if( !--engmng.m_nTaskRef && engmng.m_pEngine )
			{
				if( !engmng.m_pEngine->m_nLockRef )
				{
					DestroyEngine(engmng.m_pEngine);

					engmng.m_pEngine = NULL;
					engmng.m_dwState = errNOT_READY;
				}
			}
		}
	}
	UpdateStatistics();
	return error;
}

tERROR AVSImpl::LockEngine(enEngineType eType, CAVSEngine** p_engine)
{
	if( eType == ENGINE_DEFAULT )
		eType = !m_settings.m_sSetName.empty() ? ENGINE_AVP3 : ENGINE_KLAV;

	CAVSEngineMng& engmng = GetEngineMng(eType);

	cAutoCS locker(m_lock, true);
	if( !engmng.m_pEngine )
		return errNOT_FOUND;

	engmng.m_pEngine->m_nLockRef++;
	*p_engine = engmng.m_pEngine;
	return errOK;
}

tERROR AVSImpl::UnlockEngine(CAVSEngine* hEngine)
{
	CAVSEngineMng& engmng = GetEngineMng(hEngine->m_engtype);

	cAutoCS locker(m_lock, true);

	if( !--hEngine->m_nLockRef )
	{
		if( hEngine != engmng.m_pEngine || !engmng.m_nTaskRef )
		{
			DestroyEngine(hEngine);

			if( !engmng.m_nTaskRef )
			{
				engmng.m_pEngine = NULL;
				engmng.m_dwState = errNOT_READY;
			}
		}
	}
	return errOK;
}

// AVP Prague stamp begin( External (user called) interface method implementation, GetTreats )
// Parameters are:
tERROR AVSImpl::GetTreats( tDWORD p_taskid, hAVSTREATS* p_treats )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVS::GetTreats method" );

	// Place your code here
	AVSTreats* pTreats = NULL;

	cAutoCS locker(m_treatslock, true);
	for(tDWORD i = 0; i < m_vecTreats.size(); i++)
		if( m_vecTreats[i]->m_nTaskId == p_taskid )
		{
			pTreats = m_vecTreats[i];
			break;
		}

	if( !pTreats )
	{
		error = sysCreateObjectQuick((hOBJECT*)&pTreats, IID_AVSTREATS, PID_AVS);
		if( PR_SUCC(error) && !InitTreatsData(p_taskid, pTreats) )
		{
			((hAVSTREATS)pTreats)->sysCloseObject();
			pTreats = NULL;
			error = errUNEXPECTED;
		}

		if( PR_SUCC(error) )
			m_vecTreats.push_back(pTreats);
	}

	if( pTreats )
	{
		if( p_treats ) *p_treats = (hAVSTREATS)pTreats;
		pTreats->m_nRef++;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ReleaseTreats )
// Parameters are:
tERROR AVSImpl::ReleaseTreats( hAVSTREATS p_treats )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVS::ReleaseTreats method" );

	// Place your code here
	AVSTreats* pTreats = (AVSTreats*)p_treats;

	cAutoCS locker(m_treatslock, true);
	for(tDWORD i = 0; i < m_vecTreats.size(); i++)
		if( m_vecTreats[i] == pTreats )
		{
			if( !--m_vecTreats[i]->m_nRef )
			{
				m_vecTreats.remove(i);
				if( !pTreats->m_pTask )
					p_treats->sysCloseObject();
			}
			break;
		}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, MatchExcludes )
// Parameters are:
tERROR AVSImpl::MatchExcludes( const cSerializable* p_object_info )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AVS::MatchExcludes method" );

	// Place your code here
	if( !p_object_info || !p_object_info->isBasedOn(cDetectObjectInfo::eIID) )
	{
		PR_TRACE((this, prtIMPORTANT, "avs\tCheck excludes failed: errPARAMETER_INVALID"));
		return errPARAMETER_INVALID;
	}

	return MatchExcludesEx((cDetectObjectInfo*)p_object_info, false);
}
// AVP Prague stamp end



tERROR AVSImpl::MatchExcludesEx(cDetectObjectInfo* pInfo, bool bShort)
{
	tERROR error = errOK;

	cInfectedObjectInfo* pInfectedInfo = pInfo->isBasedOn(cInfectedObjectInfo::eIID) ?
		(cInfectedObjectInfo*)pInfo : NULL;

	cAutoCS locker(m_lock, true);

	if( pInfo->m_nDetectDanger != DETDANGER_UNKNOWN )
	{
		if( !(pInfo->m_nDetectDanger & m_settings.m_nDetectMask) )
		{
			PR_TRACE((this, prtIMPORTANT, "avs\tCheck excludes: not known danger or mask (return errOK_DECIDED)"));
			return errOK_DECIDED;
		}
	}

	CMaskChecker pObjectChecker(pInfo->m_strObjectName, bShort);
        
	tDWORD nCount = m_settings.m_aExcludes.size();
	for(tDWORD i = 0; i < nCount; i++)
	{
		cDetectExclude& pExclude = m_settings.m_aExcludes[i];
		if( !pExclude.m_bEnable )
			continue;

		if( !pExclude.IsForTask(pInfo->m_strTaskType) )
			continue;
		
		if( pExclude.IsObjectMask() )
			if( !pObjectChecker.MatchPath(pExclude.m_Object.m_strMask, !!pExclude.m_Object.m_bRecurse, true) )
				continue;

		if( !pExclude.IsVerdictMask() )
		{
			PR_TRACE((this, prtIMPORTANT, "avs\tCheck excludes: skip all (return errOK_DECIDED)"));
			return errOK_DECIDED;
		}

		if( pInfo->m_nDetectType == DETYPE_UNKNOWN )
			continue;

		if( !CMaskChecker::MatchStrings(pInfo->m_strDetectName, pExclude.m_strVerdictMask) )
			continue;

		if( !pInfectedInfo || !pExclude.IsVerdictPath() )
		{
			PR_TRACE((this, prtIMPORTANT, "avs\tCheck excludes: match with verdict string %S (return errOK_DECIDED)", pExclude.m_strVerdictMask.data()));
			return errOK_DECIDED;
		}

		cStrObj strVerdictPathMask = pExclude.m_VerdictPath.m_strMask;
		strVerdictPathMask.replace_all(L"\\", 0, L"\\\\");
		strVerdictPathMask.replace_all(L"/", 0, L"//");
		
		if( CMaskChecker::MatchStrings(pInfectedInfo->m_strDetectObject, strVerdictPathMask) )
		{
			PR_TRACE((this, prtIMPORTANT, "avs\tCheck excludes: match with verdict path string %S (return errOK_DECIDED)", pExclude.m_VerdictPath.m_strMask.data()));
			return errOK_DECIDED;
		}
	}

	return errNOT_MATCHED;
}

// AVP Prague stamp begin( External (user called) interface method implementation, CreateSession )
//! tERROR AVSImpl::CreateSession( hAVSSESSION* p_result, hTASK p_task, tBOOL p_lock_engine, tDWORD p_scan_origin )
tERROR AVSImpl::CreateSession( hAVSSESSION* p_result, hTASK p_task, tDWORD p_engine_flags, tDWORD p_scan_origin )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( this, "Enter AVS::CreateSession method" );

	// Place your code here
	hAVSSESSION avsession = NULL;
	error = sysCreateObject((hOBJECT*)&avsession, IID_AVSSESSION, PID_AVS);
	if( PR_SUCC(error) )
	{
		AVSSession* session = (AVSSession*)avsession;
		session->m_task = p_task;

		if( p_engine_flags == ENGINE_UNKNOWN )
			p_engine_flags = ENGINE_DEFAULT;

		if( p_engine_flags & ENGINE_DEFAULT )
		{
			if( m_settings.m_sSetName.empty() )
				p_engine_flags |= ENGINE_KLAV;
			else
				p_engine_flags |= ENGINE_AVP3|ENGINE_EMUL;

			p_engine_flags &= ~ENGINE_DEFAULT;
		}

		session->m_eEngineMask = p_engine_flags;
		session->m_nOrigin = (enObjectOrigin)p_scan_origin;

		error = avsession->sysCreateObjectDone();

		if( PR_SUCC(error) )
			error = avsession->SetState(TASK_REQUEST_RUN);
	}

	if( PR_FAIL(error) && avsession )
	{
		avsession->sysCloseObject();
		avsession = NULL;
	}

	if( p_result )
		*p_result = avsession;

	PR_TRACE_A1( this, "Leave AVS::CreateSession method, ret %terr", error );
	return error;
}
// AVP Prague stamp end


void AVSImpl::ProcessBegin(CScanContext& pScanCtx)
{
	pScanCtx.m_pScanCtx = &pScanCtx;
	pScanCtx.m_tmStart = PrGetTickCount();

	if( PrInterlockedIncrement((tLONG*)&m_ProcessingCounter) == 1 )
		m_EnterToProcessTime = pScanCtx.m_tmStart;

	enEngineType m_eEngine = pScanCtx.m_Session->m_eBaseEngine;
	if( pScanCtx.m_fStreamProcessing )
		m_eEngine = ENGINE_STREAM;

	LockEngine(m_eEngine, &pScanCtx.m_Engine);
	PrInterlockedIncrement(&pScanCtx.m_Session->m_nProcessCount);

	m_origin_times[pScanCtx.m_Session->m_nOrigin] = pScanCtx.m_tmStart;
}

bool AVSImpl::ProcessYield(CScanContext& pScanCtx, tDWORD nTime)
{
	m_origin_times[pScanCtx.m_Session->m_nOrigin] = nTime;

	if( m_avspm )
	{
		tERROR error = m_avspm->ProcessYield();
		if( error == errNOT_ENOUGH_MEMORY &&
			pScanCtx.m_Settings->m_nScanAction != SCAN_ACTION_DETECT )
			return false;
	}

	return true;
}

void AVSImpl::ProcessEnd(CScanContext& pScanCtx)
{
	if( pScanCtx.m_nProcessId )
	{
		cAutoCS locker(m_lock, true);
		pScanCtx.m_Session->m_nCurCancelId = 0;
		for(tDWORD i = 0; i < m_vecContexts.size(); i++)
			if( m_vecContexts[i] == &pScanCtx )
			{
				m_vecContexts.remove(i);
				locker.unlock();
				SendCancelProcessRequest(pScanCtx, cProcessCancelRequest::END);
				break;
			}
	}

	if( PrInterlockedDecrement((tLONG*)&m_ProcessingCounter) == 0 )
		m_TotalWorkingTime += PrGetTickCount() - m_EnterToProcessTime;

	cAutoCS locker(m_lock, true);

	switch(pScanCtx.m_Session->m_nOrigin)
	{
	case OBJECT_ORIGIN_GENERIC: m_statistics.m_nNumGenericScaned += pScanCtx.m_nNumScaned; break;
	case OBJECT_ORIGIN_MAIL:    m_statistics.m_nNumMailScaned += pScanCtx.m_nNumScaned; break;
	case OBJECT_ORIGIN_SCRIPT:  m_statistics.m_nNumScriptScaned += pScanCtx.m_nNumScaned; break;
	}

	UnlockEngine(pScanCtx.m_Engine);
	PrInterlockedDecrement(&pScanCtx.m_Session->m_nProcessCount);
}

// AVP Prague stamp begin( External (user called) interface method implementation, ProcessCancel )
tERROR AVSImpl::ProcessCancel( tDWORD p_process_id )
{
	tERROR error = errNOT_FOUND;
	PR_TRACE_FUNC_FRAME( "AVS::ProcessCancel method" );

	// Place your code here
	cAutoCS locker(m_lock, true);
	for(tDWORD i = 0; i < m_vecContexts.size(); i++)
		if( m_vecContexts[i]->m_nProcessId == p_process_id )
		{
			m_vecContexts[i]->m_fCancelRequest = 1;
			error = errOK;
			break;
		}

	return error;
}
// AVP Prague stamp end



void AVSImpl::AddCancelProcessRequest(CScanContext& pScanCtx)
{
	static tDWORD g_nProcessIdCounter = 0;

	if( pScanCtx.m_nProcessId )
		return;

	{
		cAutoCS locker(m_lock, true);
		pScanCtx.m_nProcessId = ++g_nProcessIdCounter;
		pScanCtx.m_Session->m_nCurCancelId = pScanCtx.m_nProcessId;
		pScanCtx.m_ProcessInfo->m_nProcessId = pScanCtx.m_nProcessId;
		m_vecContexts.push_back(&pScanCtx);
	}
	SendCancelProcessRequest(pScanCtx, cProcessCancelRequest::BEGIN);
}

void AVSImpl::SendCancelProcessRequest(CScanContext& pScanCtx, tDWORD nAction)
{
	cProcessCancelRequest pRequest;

	pRequest.m_strTaskType = "avs";
	pRequest.m_nTaskId = pScanCtx.m_Session->m_nTaskId;
	pRequest.m_nProcessId = pScanCtx.m_nProcessId;
	pRequest.m_strObjectName = pScanCtx.m_strObjectName;

	pScanCtx.m_Session->m_task->sysSendMsg(pmcASK_ACTION, nAction, NULL, &pRequest, SER_SENDMSG_PSIZE);
}

bool AVSImpl::InitTreatsInfo()
{
	if( m_bIsRemote )
		return true;

	cAutoCS locker(m_treatslock, true);

	PR_TRACE((this,prtALWAYS_REPORTED_MSG,"avs\tInit threats info begin..."));

	if( !m_tm )
		return false;

	tERROR error = m_tm->GetTaskReport(TASKID_TM_ITSELF, THREATS_REPORT, *this, fACCESS_RW, &m_pTreatsStore);
	if( PR_FAIL(error) )
	{
		PR_TRACE((this,prtERROR,"avs\t!!!!!!!!Cannot get threats report %terr", error));
		return false;
	}

	error = m_tm->GetTaskReport(TASKID_TM_ITSELF, THREATS_REPORT_INDEX, *this, fACCESS_RW, &m_pTreatsIdx);
	if( m_pTreatsIdx )
	{
		tDWORD nCount = 0;
		m_pTreatsIdx->GetRecordCount(&nCount);

		for(tDWORD i = 0; i < nCount; i++)
		{
			tDWORD nRecordTask = -1;
			m_pTreatsIdx->GetRecord(NULL, i, &nRecordTask, sizeof(tDWORD), NULL);
			m_vecIdxCache.push_back(nRecordTask);
		}
	}
	else
		PR_TRACE((this,prtERROR,"avs\t!!!!!!!!Cannot get threats index report %terr", error));

	m_statistics.m_nNumThreats = 0;
	m_statistics.m_nNumUntreated = 0;

	tDWORD nCount = 0;
	m_pTreatsStore->GetRecordCount(&nCount);

	time_t tmCurrent = cDateTime::now_utc();
	long tmWorkingTime = PrGetTickCount() / 1000;

	cInfectedObjectInfo pInfInfo;
	for(tDWORD i = 0; i < nCount; i++)
	{
		if( !GetTreatInfo(i, &pInfInfo) )
		{
			if( !i )
			{
				DiscardTreatsData(cDwordVector());
				break;
			}
			continue;
		}

		m_vecDetected.push_back(tDetectedIds(pInfInfo));

		if( IS_INVISIBLE(pInfInfo.m_nObjectStatus) )
			continue;

		m_statistics.m_nNumThreats++;
		if( IS_INFECTED(pInfInfo.m_nObjectStatus) )
			m_statistics.m_nNumUntreated++;

		if( !IS_REBOOT(pInfInfo.m_nObjectStatus) )
			continue;

		if( pInfInfo.m_tmTimeStamp + tmWorkingTime < tmCurrent )
		{
			if( !m_pToVerify )
				m_pToVerify = new cAskProcessTreats();
			m_pToVerify->m_aScanRecords.push_back(i);
		}
	}
	PR_TRACE((this,prtALWAYS_REPORTED_MSG,"avs\tInit threats info done..."));

	VerifyTreatsInfo();
	return true;
}

void AVSImpl::VerifyTreatsInfo()
{
	if( !m_pToVerify )
		return;

	PR_TRACE((this,prtALWAYS_REPORTED_MSG,"avs\tVerify threats begin..."));

	bool bCompleted = true;
/*	if( FSIsProcessedOnReboot )		bugfix 31857
	{
		cInfectedObjectInfo pInfInfo;
		tDWORD i, c = m_pToVerify->m_aScanRecords.size();
		for(i = 0; i < c; i++)
		{
			tDWORD pos = m_pToVerify->m_aScanRecords.at(i);
			if( !GetTreatInfo(pos, &pInfInfo) )
				continue;

			tERROR error = FSIsProcessedOnReboot(cAutoString(pInfInfo.m_strArchiveObject));
			if( error == errNOT_FOUND )
				continue;

			enObjectStatus& nCurStatus = pInfInfo.m_nObjectStatus;
			if( PR_FAIL(error) )
			{
				if( error == errOBJECT_NOT_FOUND && nCurStatus != OBJSTATUS_DISINFECTED_ON_REBOOT )
					nCurStatus = OBJSTATUS_UNKNOWN;
				else
					nCurStatus = OBJSTATUS_NOTDISINFECTED;
			}
			else switch(nCurStatus)
				{
				case OBJSTATUS_DELETED_ON_REBOOT:     nCurStatus = OBJSTATUS_DELETED; break;
				case OBJSTATUS_QUARANTINED_ON_REBOOT: nCurStatus = OBJSTATUS_QUARANTINED; break;
				case OBJSTATUS_DISINFECTED_ON_REBOOT: nCurStatus = OBJSTATUS_DISINFECTED; break;
				}

			tTreatInfo treatInfo(pInfInfo.m_qwUniqueId, pos, nCurStatus);
			UpdateTreatStatus(NULL, treatInfo, &pInfInfo);

			m_pToVerify->m_aScanRecords.remove(i--), c--;
		}

		bCompleted = !m_pToVerify->m_aScanRecords.size();
	}
	else*/
	{
		cAVSTreats* pTreats;
		if( PR_SUCC(GetTreats(0, &pTreats)) )
		{
			m_pToVerify->m_bByIndex = cFALSE;
			m_pToVerify->m_nAction = cAskProcessTreats::VERIFY;

			pTreats->Process(FILTER_NONE, m_pToVerify);
			ReleaseTreats(pTreats);
		}
	}

	if( bCompleted )
	{
		delete m_pToVerify;
		m_pToVerify = NULL;
	}

	PR_TRACE((this,prtALWAYS_REPORTED_MSG,"avs\tVerify threats done..."));
}

bool AVSImpl::InitTreatsData(tDWORD nTaskId, AVSTreats* pTreats)
{
	cAutoCS locker(m_treatslock, true);

	pTreats->m_pAVS = this;
	pTreats->m_nTaskId = nTaskId;
	pTreats->m_tmStamp = PrGetTickCount();
	pTreats->m_nIdxPos = -1;

	if( !m_pTreatsIdx )
	{
		PR_TRACE((this,prtERROR,"avs\t(!!!!!!!!!!!)Threats index report is NULL"));
		return false;
	}

	tDWORD i, nCount, nSize = 0;
	tDWORD* aIdxs = NULL;
	bool bAll = !nTaskId;

	if( bAll )
	{
		nCount = m_vecDetected.size();
	}
	else
	{
		nCount = m_vecIdxCache.size();
		for(i = 0; i < nCount; i++)
			if( m_vecIdxCache[i] == nTaskId )
			{
				pTreats->m_nIdxPos = i;
				break;
			}

		if( pTreats->m_nIdxPos != -1 )
		{
			m_pTreatsIdx->GetRecord(&nSize, pTreats->m_nIdxPos, NULL, 0, NULL);
			if( !nSize )
				return true;

			nCount = nSize/sizeof(tDWORD) - 1;
			aIdxs = new tDWORD[nCount + 1];
			m_pTreatsIdx->GetRecord(&nSize, pTreats->m_nIdxPos, (tPTR)aIdxs, nSize, NULL);
		}
		else
			nCount = 0;
	}

	for(i = 0; i < nCount; i++)
	{
		tDWORD nPos = bAll ? i : aIdxs[i+1];
		if( nPos >= m_vecDetected.size() )
			continue;

		tDetectedIds& pDetected = m_vecDetected[nPos];

		if( IS_INVISIBLE(pDetected.m_nStatus) )
			continue;

		pTreats->m_vecTreats.push_back(nPos);

		if( IS_INFECTED(pDetected.m_nStatus) )
			pTreats->m_vecUtreated.push_back(nPos);
	}

	if( aIdxs )
		delete[] aIdxs;

	return true;
}

bool AVSImpl::SaveTreatsData(AVSTreats* pTreats)
{
	cAutoCS locker(m_treatslock, true);
	if( !m_pTreatsIdx || !pTreats->m_nTaskId ||
		!pTreats->m_vecTreats.size() && pTreats->m_nIdxPos == -1 )
		return false;

	pTreats->m_vecTreats.insert(0) = pTreats->m_nTaskId;
	tDWORD nSize = pTreats->m_vecTreats.size()*sizeof(tDWORD);

	tERROR error;
	if( pTreats->m_nIdxPos == -1 )
	{
		error = m_pTreatsIdx->AddRecord(pTreats->m_vecTreats.data (), nSize, &pTreats->m_nIdxPos);
		m_vecIdxCache.push_back(pTreats->m_nTaskId);
	}
	else
		error = m_pTreatsIdx->UpdateRecord(pTreats->m_nIdxPos, pTreats->m_vecTreats.data(), nSize);

	return PR_SUCC(error);
}

void AVSImpl::DiscardTreatsData(const cDwordVector& vecPos)
{
	cAutoCS locker(m_treatslock, true);
	if(vecPos.size() < m_statistics.m_nNumThreats )
	{
		tDWORD nCount = vecPos.size();
		for(tDWORD i = 0; i < nCount; i++)
			DeleteTreatInfo(vecPos[i], i == (nCount-1) );
		return;
	}

	cQB * qb = NULL;
	if( m_tm )
		m_tm->GetService(0, (cObj*)this, sid_TM_QUARANTINE, (cObj**)&qb);

	if( qb )
	{
		tDWORD i, nCount;
		qb->GetCount(&nCount, cCOUNT_TOTAL | cCOUNT_REFRESH);
		for(i = 0; i < nCount; i++)
		{
			tQWORD qbId = 0;
			qb->GetObjectByIndex(NULL, &qbId, i, cFALSE, NULL);
			if( qbId && PR_SUCC(qb->DeleteObject(qbId)) )
				i--, nCount--;
		}
	}

	// Discard all
	if( m_pTreatsStore )
	{
		hIO io = (hIO)m_pTreatsStore->propGetObj(plIO);
		if( io ) io->propSetBool(pgOBJECT_DELETE_ON_CLOSE, cTRUE);
		m_pTreatsStore->sysCloseObject();
		m_pTreatsStore = NULL;
	}

	if( m_pTreatsIdx )
	{
		hIO io = (hIO)m_pTreatsIdx->propGetObj(plIO);
		if( io ) io->propSetBool(pgOBJECT_DELETE_ON_CLOSE, cTRUE);
		m_pTreatsIdx->sysCloseObject();
		m_pTreatsIdx = NULL;
	}

	m_vecDetected.clear();
	m_vecIdxCache.clear();

	for(tDWORD i = 0; i < m_vecTreats.size(); i++)
		m_vecTreats[i]->ClearTreatInfo();

	InitTreatsInfo();
	sysSendMsg(pmc_AVS, pm_THREATS_STATUS_CHANGED, NULL, NULL, NULL);
}

void AVSImpl::DiscardTreatsByDetectMask()
{
	cAutoCS locker(m_treatslock, true);

	cDwordVector vecPos;
	cInfectedObjectInfo pInfInfo;

	tDWORD nCount = m_vecDetected.size();
	for(tDWORD i = 0; i < nCount; i++)
	{
		if( !GetTreatInfo(i, &pInfInfo) )
			continue;

		if( IS_INVISIBLE(pInfInfo.m_nObjectStatus) )
			continue;

		if( !(pInfInfo.m_nDetectDanger & m_settings.m_nDetectMask) )
			vecPos.push_back(i);
	}
	DiscardTreatsData(vecPos);
}

bool AVSImpl::SyncThreatsWhithQB(tDWORD nMsgId, tQWORD qwId)
{
	cAutoCS locker(m_lock, true);

	tDWORD i, nCount = m_vecDetected.size();
	for(i = 0; i < nCount; i++)
		if( qwId == m_vecDetected[i].m_qwQBId )
		{
			cInfectedObjectInfo pInfo;
			GetTreatInfo(i, &pInfo);

			switch(nMsgId)
			{
			case pm_QB_IO_DELETE:  break;
			case pm_QB_IO_RESTORE:
				if( pInfo.m_nObjectStatus != OBJSTATUS_ADDEDBYUSER )
					pInfo.m_nObjectStatus = OBJSTATUS_NOTDISINFECTED;
				else
					pInfo.m_nObjectStatus = (enObjectStatus)(-1);
				break;
			case pm_QB_IO_STORE:
				pInfo.m_nObjectStatus = OBJSTATUS_QUARANTINED;
				break;
			default: return true;
			}

			if( nMsgId != pm_QB_IO_STORE )
				pInfo.m_qwQBObjectId = 0;
			else
				pInfo.m_qwQBObjectId = qwId;

			if( pInfo.m_qwUniqueId )
			{
				tTreatInfo treatInfo(pInfo.m_qwUniqueId, i, pInfo.m_nObjectStatus);
				UpdateTreatStatus(NULL, treatInfo, &pInfo);
			}
			break;
		}

	return true;
}

void AVSImpl::FillExcludedList(cDwordVector& vecIdx, tQWORD qwUniqueId)
{
	cAutoCS locker(m_treatslock, true);

	tDWORD i, nCount = m_vecDetected.size();
	for(i = 0; i < nCount; i++)
		if( qwUniqueId == m_vecDetected[i].m_qwParentId )
		{
			FillExcludedList(vecIdx, m_vecDetected[i].m_qwUniqueId);
			vecIdx.push_back(i);
		}
}

bool AVSImpl::UpdateTreatInfo(tDWORD nStorePos, cInfectedObjectInfo* pInfInfo)
{
	tChunckBuff buff; tDWORD nSize;

	tERROR error = g_root->StreamSerialize(pInfInfo, SERID_UNKNOWN, (tBYTE*)buff.ptr(), buff.count(), &nSize);
	if( error == errBUFFER_TOO_SMALL )
		error = g_root->StreamSerialize(pInfInfo, SERID_UNKNOWN, (tBYTE*)buff.get(nSize, false), nSize, &nSize);

	if( PR_SUCC(error) )
		error = m_pTreatsStore->UpdateRecord(nStorePos, buff.ptr(), nSize);

	if( m_tm && IS_DISCARDED(pInfInfo->m_nObjectStatus) )
	{
		cQB * qb = NULL; tQWORD qbId = m_vecDetected[nStorePos].m_qwQBId;
		if( qbId )
			m_tm->GetService(0, (cObj*)this, sid_TM_QUARANTINE, (cObj**)&qb);
		if( qb )
			qb->DeleteObject(qbId);
	}

	return PR_SUCC(error);
}

bool AVSImpl::UpdateTreatInfo(AVSSession* pSession, cInfectedObjectInfo* pInfInfo)
{
	if( !pInfInfo->m_qwUniqueId )
		return false;

	if( m_bIsRemote )
	{
		if( pSession )
			pInfInfo->m_nBaseId = pSession->m_nTaskId;
		sysSendMsg(pmc_AVS_SYNC_THREATS, 0, NULL, pInfInfo, SER_SENDMSG_PSIZE);
		return true;
	}

	cAutoCS locker(m_treatslock, true);

	if( pInfInfo->m_nObjectStatus == OBJSTATUS_ADDEDTOEXCLUDE )
	{
		cDwordVector vecIdx;
		FillExcludedList(vecIdx, pInfInfo->m_qwUniqueId);

		tDWORD posDetected = m_vecDetected.find(pInfInfo->m_qwUniqueId);
		if( posDetected != -1 )
			vecIdx.push_back(posDetected);

		DiscardTreatsData(vecIdx);
	}
	else
	{
		tDWORD posDetected = m_vecDetected.find(pInfInfo->m_qwUniqueId);
		if( posDetected == -1 )
		{
			AddTreatInfo(pSession, pInfInfo);
		}
		else
		{
			tTreatInfo treatInfo(pInfInfo->m_qwUniqueId, posDetected,
				pInfInfo->m_nObjectStatus, pInfInfo->m_nObjectScope);

			UpdateTreatStatus(pSession, treatInfo, pInfInfo);
		}
	}
	return true;
}

bool AVSImpl::DeleteTreatInfo(tDWORD nStorePos, bool bNotify)
{
	cAutoCS locker(m_treatslock, true);

	tTreatInfo pInfo(0, nStorePos, (enObjectStatus)(-1));

	tDWORD& nCurStatus = m_vecDetected[nStorePos].m_nStatus;
	if( IS_INVISIBLE(nCurStatus) )
		return true;

	if( m_pTreatsStore )
	{
		cInfectedObjectInfo pTreatInfo;
		pTreatInfo.m_nObjectStatus = (enObjectStatus)pInfo.m_nStatus;
		UpdateTreatInfo(nStorePos, &pTreatInfo);
	}

	m_statistics.m_nNumThreats--;

	if( IS_INFECTED(nCurStatus) )
		m_statistics.m_nNumUntreated--;

	nCurStatus = pInfo.m_nStatus;
	UpdateTasksThreat(NULL, pInfo, false);

	if( bNotify )
		sysSendMsg(pmc_AVS, pm_THREATS_STATUS_CHANGED, NULL, NULL, NULL);
	return true;
}

bool AVSImpl::GetTreatInfo(tDWORD nStoredPos, cInfectedObjectInfo* pInfInfo)
{
	cAutoCS locker(m_treatslock, true);

	tChunckBuff buff; tDWORD nSize;

	tERROR error = m_pTreatsStore->GetRecord(&nSize, nStoredPos, (tPTR)buff.ptr(), buff.count(), NULL);
	if( error == errBUFFER_TOO_SMALL )
		error = m_pTreatsStore->GetRecord(NULL, nStoredPos, (tPTR)buff.get(nSize, false), nSize, NULL);

	if( PR_SUCC(error) )
		error = g_root->StreamDeserialize((cSerializable**)&pInfInfo, (tBYTE*)buff.ptr(), nSize, NULL);

	return PR_SUCC(error);
}

tERROR AVSImpl::AddThreatToQuaranine(AVSSession* pSession, cInfectedObjectInfo* pInfInfo)
{
	cIOObj io(*this, cAutoString(pInfInfo->m_strObjectName),
		fACCESS_READ, fOMODE_FOLDER_AS_IO);

	if( PR_FAIL(io.last_error()) )
		return io.last_error();

	tERROR error = sysSendMsg(pmcASK_ACTION, cAskQBAction::QUARANTINE,
		NULL, pInfInfo, SER_SENDMSG_PSIZE);

	if( PR_FAIL(error) )
		return error;

	CScanContext ctx(io, pSession, NULL, NULL);
	ctx.m_Engine->GetReopenData(ctx, pInfInfo->m_pReopenData);
	ctx.CalcObjectHash(pInfInfo, NULL);

	UpdateTreatInfo(pSession, pInfInfo);
	return errOK;
}

bool AVSImpl::AddTreatInfo(AVSSession* pSession, cInfectedObjectInfo* pInfInfo)
{
	if( pInfInfo->m_nDescription && pInfInfo->m_nDescription != DTREASON_BYHASH )
		pInfInfo->m_nObjectStatus = OBJSTATUS_ARCHIVE;

	m_vecDetected.push_back(tDetectedIds(*pInfInfo));

	if( m_pTreatsStore )
	{
		tChunckBuff buff; tDWORD nSize;

		pInfInfo->m_tmTimeStamp = cDateTime::now_utc();

		tERROR error = g_root->StreamSerialize(pInfInfo, SERID_UNKNOWN, (tBYTE*)buff.ptr(), buff.count(), &nSize);
		if( error == errBUFFER_TOO_SMALL )
			error = g_root->StreamSerialize(pInfInfo, SERID_UNKNOWN, (tBYTE*)buff.get(nSize, false), nSize, &nSize);

		tDWORD nStoredPos;
		if( PR_SUCC(error) )
			error = m_pTreatsStore->AddRecord(buff.ptr(), nSize, &nStoredPos);
	}

	if( !IS_ARCHIVE(pInfInfo->m_nObjectStatus) )
	{
		m_statistics.m_nNumThreats++;
		if( IS_INFECTED(pInfInfo->m_nObjectStatus) )
			m_statistics.m_nNumUntreated++;

        tTreatInfo treatInfo(pInfInfo->m_qwUniqueId, m_vecDetected.size()-1, pInfInfo->m_nObjectStatus, pInfInfo->m_nObjectScope);
		UpdateTasksThreat(pSession, treatInfo, true);
		sysSendMsg(pmc_AVS, pm_THREATS_STATUS_CHANGED, NULL, NULL, NULL);
	}

	return true;
}

bool AVSImpl::UpdateTreatStatus(AVSSession* pSession, tTreatInfo& pInfo, cInfectedObjectInfo* pInfInfo, cInfectedObjectInfo* pLocked, bool bByHash)
{
	if( pInfo.m_nStoredPos >= m_vecDetected.size() )
		return false;

	tDWORD& nCurStatus = m_vecDetected[pInfo.m_nStoredPos].m_nStatus;
	tDWORD& nStatus = pInfo.m_nStatus;

	if( IS_INVISIBLE(nCurStatus) && nStatus == OBJSTATUS_UNKNOWN )
		return true;

	bool bArchiveObject = IS_ARCHIVE(nCurStatus);
	bool bRecursive = true;

	if( pInfInfo )
	{
		if( pInfInfo->m_qwQBObjectId )
			m_vecDetected[pInfo.m_nStoredPos].m_qwQBId = pInfInfo->m_qwQBObjectId;

		if( !pInfInfo->m_nDescription || pInfInfo->m_nDescription == DTREASON_BYHASH ) // was detection here
		{
			bArchiveObject = false;
			bByHash = pInfInfo->m_nDescription == DTREASON_BYHASH && IS_INFECTED(nStatus);
		}
		else if( bArchiveObject )
		{
			if( IS_INFECTED(nStatus) ||
					pInfInfo->m_nDescription == DTREASON_INFORMATION && nStatus == OBJSTATUS_DISINFECTED )
				return false;
		}
		else if( nStatus == OBJSTATUS_DISINFECTED )
			bRecursive = false;
	}

	if( IS_REBOOT(nCurStatus)  )
	{
		if( pSession )
		{
			if( !pSession->m_bVerifyThreats )
				nStatus = nCurStatus;
			else if( nStatus == OBJSTATUS_UNKNOWN )
			{
				if( nCurStatus == OBJSTATUS_DELETED_ON_REBOOT )
					nStatus = OBJSTATUS_DELETED;
				else if( nCurStatus == OBJSTATUS_QUARANTINED_ON_REBOOT )
					nStatus = OBJSTATUS_QUARANTINED;
			}
		}
	}
	else if( bRecursive && (!IS_INFECTED(nStatus) || bByHash) )
	{
		tDWORD nCount = m_vecDetected.size();
		for(tDWORD i = 0; i < nCount; i++)
			if( pInfo.m_qwUnique == m_vecDetected[i].m_qwParentId )
			{
				if( bByHash && !IS_INFECTED(m_vecDetected[i].m_nStatus) && !IS_ARCHIVE(m_vecDetected[i].m_nStatus) )
					continue;

				tTreatInfo treatInfo (m_vecDetected[i].m_qwUniqueId, i, (enObjectStatus)nStatus, pInfo.m_nScopeId);
				UpdateTreatStatus(pSession, treatInfo, NULL, pLocked, bByHash);
			}
	}

	if( nCurStatus != nStatus && !bArchiveObject )
	{
		if( m_pTreatsStore )
		{
			cInfectedObjectInfo pTreatInfo;
			GetTreatInfo(pInfo.m_nStoredPos, &pTreatInfo);

			if( !pInfInfo )
				pInfInfo = &pTreatInfo;
			else
			{
				if( !pTreatInfo.m_strObjectName.compare(pInfInfo->m_strObjectName,
						fSTRING_COMPARE_CASE_INSENSITIVE|fSTRING_COMPARE_SLASH_INSENSITIVE) )
					pInfInfo->m_strObjectName = pTreatInfo.m_strObjectName;

				if( pInfInfo->m_strUserName.empty() )
					pInfInfo->m_strUserName = pTreatInfo.m_strUserName;

				if( pInfInfo->m_strMachineName.empty() )
					pInfInfo->m_strMachineName = pTreatInfo.m_strMachineName;
			}

			if( pLocked )
				pInfInfo->m_strArchiveObject = pLocked->m_strArchiveObject;

			pInfInfo->m_nObjectStatus = (enObjectStatus)nStatus;
			pInfInfo->m_tmTimeStamp = cDateTime::now_utc();

			UpdateTreatInfo(pInfo.m_nStoredPos, pInfInfo);
		}

		if( IS_INVISIBLE(nCurStatus) )
			m_statistics.m_nNumThreats++;

		if( IS_INFECTED(nCurStatus) )
			m_statistics.m_nNumUntreated--;
		else if( IS_INFECTED(nStatus) )
			m_statistics.m_nNumUntreated++;

		nCurStatus = nStatus;
		sysSendMsg(pmc_AVS, pm_THREATS_STATUS_CHANGED, NULL, NULL, NULL);
	}

	if( !IS_ARCHIVE(nCurStatus) )
		UpdateTasksThreat(pSession, pInfo, false);
	return true;
}

bool AVSImpl::UpdateTasksThreat(AVSSession* pSession, tTreatInfo& pInfo, bool bNew)
{
	if( pSession && !pSession->m_pTreats && pSession->m_pAVS )
	{
		if( PR_SUCC(GetTreats(pSession->m_nTaskId, (hAVSTREATS*)&pSession->m_pTreats)) )
			if( !pSession->m_pTreats->m_pInitSession )
				pSession->m_pTreats->m_pInitSession = pSession;
	}

	for(tDWORD i = 0; i < m_vecTreats.size(); i++)
		m_vecTreats[i]->UpdateTreatInfo(pSession, pInfo, bNew);

	return true;
}

bool AVSImpl::IsLockedObjectProcessed(tQWORD qwUnique)
{
	cAutoCS locker(m_treatslock, true);
	return m_vecLocked.find(qwUnique) != -1;
}

tERROR AVSImpl::LockedObjectProcessed(AVSSession* pSession, cInfectedObjectInfo* pInfInfo, enObjectStatus nStatus)
{
	cAutoCS locker(m_treatslock, true);

	tDWORD posLocked = m_vecLocked.find(pInfInfo->m_qwUniqueId);
	if( posLocked == -1 )
		m_vecLocked.push_back(tDetectedIds(*pInfInfo, nStatus));
	else
		nStatus = (enObjectStatus)m_vecLocked[posLocked].m_nStatus;

	tDWORD posDetected = m_vecDetected.find(pInfInfo->m_qwUniqueId);
	if( posDetected != -1 )
	{
        tTreatInfo treatInfo(pInfInfo->m_qwUniqueId, posDetected, nStatus);
		UpdateTreatStatus(pSession, treatInfo, pInfInfo, pInfInfo);
	}
	return errOK;
}

bool AVSImpl::RegisterPassword(tQWORD qwUnique, cStringObj& strPswd)
{
	cAutoCS locker(m_treatslock, true);

	tDWORD nCount = m_vecPasswords.size();
	for(tDWORD i = 0; i < nCount; i++)
		if( m_vecPasswords[i].m_qwUniqueId == qwUnique )
			return true;

	tPassword& pswd = m_vecPasswords.push_back();
	pswd.m_qwUniqueId = qwUnique;
	pswd.m_strPswd = strPswd;
	return true;
}

bool AVSImpl::GetPassword(tQWORD qwUnique, cStringObj& strPswd)
{
	cAutoCS locker(m_lock, true);

	tDWORD nCount = m_vecPasswords.size();
	for(tDWORD i = 0; i < nCount; i++)
	{
		tDWORD pos = nCount - i - 1;
		if( m_vecPasswords[pos].m_qwUniqueId == qwUnique )
		{
			strPswd = m_vecPasswords[pos].m_strPswd;
			return true;
		}
	}
	return false;
}

bool AVSImpl::CheckActiveDetection(cStringObj& strPath)
{
	cAutoCS locker(m_lock, true);

	if( !m_hProcMon && m_tm )
		m_tm->GetService(TASKID_TM_ITSELF, (hOBJECT)this, AVP_SERVICE_PROCESS_MONITOR, (hOBJECT*)&m_hProcMon);

	if( !m_hProcMon || PR_FAIL(m_hProcMon->IsImageActive(strPath.data(), NULL)) )
		return false;

	PR_TRACE( (this, prtALWAYS_REPORTED_MSG, "avs\tAVS CheckActiveDetection - TRUE") );
	return true;
}

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "AVS". Register function
tERROR AVSImpl::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(AVS_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, AVS_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "AVS Imlementation", 18 )
	mSHARED_PROPERTY( pgTASK_TYPE, ((tDWORD)(TASK_AVS)) )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, AVSImpl, m_tm, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, AVSImpl, m_state, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, AVSImpl, m_taskId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_PARENT_ID, AVSImpl, m_parentId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mSHARED_PROPERTY( pgINTERFACE_COMPATIBILITY, ((tIID)(23)) )
mpPROPERTY_TABLE_END(AVS_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(AVS)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(AVS)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(AVS)
	mEXTERNAL_METHOD(AVS, SetSettings)
	mEXTERNAL_METHOD(AVS, GetSettings)
	mEXTERNAL_METHOD(AVS, AskAction)
	mEXTERNAL_METHOD(AVS, SetState)
	mEXTERNAL_METHOD(AVS, GetStatistics)
	mEXTERNAL_METHOD(AVS, Update)
	mEXTERNAL_METHOD(AVS, CreateSession)
	mEXTERNAL_METHOD(AVS, GetTreats)
	mEXTERNAL_METHOD(AVS, ReleaseTreats)
	mEXTERNAL_METHOD(AVS, MatchExcludes)
	mEXTERNAL_METHOD(AVS, ProcessCancel)
mEXTERNAL_TABLE_END(AVS)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_A0( root, "Enter AVS::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_AVS,                                // interface identifier
		PID_AVS,                                // plugin identifier
		0x00000000,                             // subtype identifier
		AVS_VERSION,                            // interface version
		VID_PETROVITCH,                         // interface developer
		&i_AVS_vtbl,                            // internal(kernel called) function table
		(sizeof(i_AVS_vtbl)/sizeof(tPTR)),      // internal function table size
		&e_AVS_vtbl,                            // external function table
		(sizeof(e_AVS_vtbl)/sizeof(tPTR)),      // external function table size
		AVS_PropTable,                          // property table
		mPROPERTY_TABLE_SIZE(AVS_PropTable),    // property table size
		sizeof(AVSImpl)-sizeof(cObjImpl),       // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"avs\tAVS(IID_AVS) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave AVS::Register method, ret %terr", error );
// AVP Prague stamp end



	if( PR_SUCC(error) )
		root->ResolveImportTable(NULL, import_table, PID_AVS);

	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(&m_propIsRemote, cpTASK_MANAGER_TASK_IS_REMOTE, pTYPE_BOOL );

// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call AVS_Register( hROOT root ) { return AVSImpl::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_SESSION_ID
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// AVP Prague stamp end




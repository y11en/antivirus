// ------------------------------------------------
#include <prague.h>
#include "tm.h"
// ------------------------------------------------

tERROR cTmProfile::InitTask(tTaskRequestState state)
{
//	if( m_dad && m_dad->m_dad )
//		m_request->wait_for_parent_inited();

	cAutoCS locker(m_lock, true);

	if( m_remoteTask )
	{
		m_request->m_flags |= REQUEST_FLAG_SAME_TASKID;
	}
	else if( !(m_request->m_flags & REQUEST_FLAG_RESTART_TASK) )
	{
		m_timeStart = (tDWORD)cDateTime::now_utc();
		m_statistics.init(m_nSerIdStatistics);
		m_stateFlags &= ~dfTaskRestarted;
	}
	else
	{
		m_request->m_flags |= REQUEST_FLAG_SAME_TASKID;
		m_stateFlags |= dfTaskRestarted;
	}

	if( m_dad && !((m_request->m_flags & REQUEST_FLAG_SAME_TASKID) && m_nRuntimeId != TASKID_UNK) )
		m_nRuntimeId = m_tm->GetUniqueId();

	m_request->set_inited();

	m_lastErr = errOK;
	m_timeFinish = -1;
	m_client = !persistent() ? m_request->m_client : NULL;
	m_dieWithClient = !!(m_request->m_flags & OPEN_TASK_MODE_CLOSE_TASK_IF_OWNER_DIED);
	m_bImpersonated = cFALSE;

	if( (m_request->m_flags & REQUEST_FLAG_BY_SCHEDULE) || schedule().auto_mode() )
		SaveScheduledRun();

	tDWORD i, c = m_aReports.count();
	if( !m_dad || !c )
		return errOK;
	
	for(i = 0; i < c; ++i)
	{
		cTaskReportDsc& dsc = m_aReports[i];

		tCHAR buff[MAX_PATH];
		if( dsc.m_id.empty() || !dsc.m_id.compare("sys") )
			pr_sprintf(buff, sizeof(buff), "%04x_%S.rpt", m_nRuntimeId, m_sName.data());
		else
			pr_sprintf(buff, sizeof(buff), "%04x_%S_%S.rpt", m_nRuntimeId, m_sName.data(), dsc.m_id.data());
		dsc.m_name = buff;
	}
	return errOK;
}

tERROR cTmProfile::LockTaskReports(bool bLock, bool bInit)
{
	if( !m_dad )
		return errOK;

	m_dad->LockTaskReports(bLock, false);

	m_reportLock.lock(false);
	if( !bLock )
	{
		if( !--m_reportRef )
		{
			for(tDWORD i = 0; i < m_vecReports.size(); i++)
				if( m_vecReports[i] )
					m_vecReports[i]->sysCloseObject();

			m_vecReports.clear();
		}
	}
	else
	{
		if( bInit && !m_vecReports.size() )
		{
			cAutoCS locker(m_lock, true);
			tDWORD i, j, k, c = m_aReports.count();
			for(i = 0; i < c; ++i)
			{
				cVector<tDWORD>& classes = m_aReports[i].m_aClasses;
				for(j = 0, k = classes.count(); j < k; ++j)
					m_host->sysRegisterMsgHandler(classes[j], rmhLISTENER, m_host, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

				cReport*& report = m_vecReports.push_back();
				m_tm->OpenReport(m_aReports[i].m_name.c_str(cCP_ANSI), (cObject*)m_host, fACCESS_RW, &report);
			}
		}
		m_reportRef++;
	}
	m_reportLock.unlock(false);
	return errOK;
}

tERROR cTmProfile::CreateTask()
{
	if( !m_iid || m_bTaskGroup && !isService() && m_cfg.m_bRemote )
		return errOK;

	tERROR err = errOK;
	cAutoObj<cObj> task;

	PR_TRACE((m_tm, prtIMPORTANT, "tm\t%S - creating task...", m_sName.data()));

	if( m_cfg.m_bRemote )
		err = m_tm->CreateRemoteTask(this, task);
	else
		err = m_host->sysCreateObject((cObj**)&task, m_iid, m_pid);

	if( PR_SUCC(err) )
	{
		tDWORD nActivityId = m_dad && m_dad->m_dad ? m_dad->id() : id();

		task->propSetDWord( pgTASK_ID, id() );
		task->propSetDWord( pgTASK_PARENT_ID, nActivityId );
		task->propSetDWord( m_tm->m_prop_taskid, nActivityId );
		m_sType.copy( task, m_tm->m_prop_task_type );
		m_sName.copy( task, m_tm->m_prop_profile );
		InitDataStorage(task);

		err = AttachTask(task);
	}

	if( PR_SUCC(err) )
		task.relinquish();
	else
		PR_TRACE((m_tm, prtERROR, "tm\t%S - cannot create task object - %terr", m_sName.data(), err));

	return err;
}

tERROR cTmProfile::AttachTask(cObj* task)
{
	tERROR err = errOK;

	m_host->sysRegisterMsgHandler(pmc_TASK_SETTINGS_CHANGED, rmhLISTENER, (cObj*)task, m_iid, m_pid, IID_ANY, PID_ANY);
	m_host->sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED,    rmhLISTENER, (cObj*)task, m_iid, m_pid, IID_ANY, PID_ANY);

	task->propSetObj( pgTASK_TM, (hOBJECT)m_tm );
	task->propSetPtr(m_tm->m_prop_this, this);

	if( m_remoteTask != task )
		err = task->sysCreateObjectDone();

	if( PR_SUCC(err) )
	{
		m_taskRef.lock(false);
		m_task = task;
		m_taskObj = PR_SUCC(g_root->sysCheckObject(m_task, IID_TASK)) ? (cTask*)m_task : NULL;
		m_taskRef.unlock(false);

		PR_TRACE((m_tm, prtIMPORTANT, "tm\t%S - task object(0x%08x) created", m_sName.data(), (cObj*)m_task));
	}
	return err;
}

tERROR cTmProfile::LockTask()
{
	if( m_lockRef++ )
		return errOK;

	AddRef();
	LockTaskReports(true);

	tERROR err = errOK;
	if( !m_remoteTask )
	{
		err = CreateTask();
		if( PR_SUCC(err) )
			err = SetTaskSettings();

		if( PR_SUCC(err) && m_taskObj && (m_stateFlags & dfTaskRestarted) )
			m_taskObj->SetSettings(m_statistics);
	}
	else if( !m_bTaskGroup )
		err = AttachTask(m_remoteTask);

	if( PR_FAIL(err) )
		StateChanged(PROFILE_STATE_FAILED, err);

	return err;
}

tERROR cTmProfile::UnlockTask()
{
	if( !m_lockRef )
		return errUNEXPECTED;

	if( --m_lockRef && !m_bFailOver )
		return (m_lockRef > 1) ? errOBJECT_ALREADY_EXISTS : errOK;

	m_taskRef.lock(false);
	m_taskObj = NULL;
	m_remoteTask = NULL;
	m_taskRef.unlock(false);

	if( m_task )
	{
		if( m_cfg.m_bRemote )
			m_tm->DestroyRemoteTask(m_task);
		else
			m_task->sysCloseObject();

		m_task = NULL;
		m_processId = PR_PROCESS_LOCAL;
	}

	close_object(m_token);

	PR_TRACE((m_tm, prtIMPORTANT, "tm\t%S - task closed (ref=%d)", m_sName.data(), m_ref));

	LockTaskReports(false);

	if( isService() && m_bClone )
		Delete();

	Release();
	return errOK;
}

tERROR cTmProfile::GetTaskStatistics(cSerializable* stat)
{
	if( !m_dad )
		return m_tm->m_bl ? m_tm->m_bl->GetStatistics(NULL, stat) : errUNEXPECTED;

	tERROR err = errUNEXPECTED;
	cTmTaskPtr task(this);
	if( task )
		err = task->GetStatistics(stat);

	if( !m_bTaskGroup )
		return err;

	cSerializableObj child_stat;
	if( PR_FAIL(child_stat.init(m_nSerIdStatistics)) )
		return err;

	bool bInited = PR_SUCC(err);

	cTmProfileEnum child(this);
	while( child.next() )
		if( PR_SUCC(child->GetTaskStatistics(child_stat)) )
			stat->assign(*child_stat, true, bInited), bInited = true;

	return errOK;
}

tERROR cTmProfile::GetTaskSettings(cSerializable*& settings)
{
	tERROR err = errOK;
	if( !settings && PR_FAIL(err = g_root->CreateSerializable(m_nSerIdSettings, &settings)) )
		return err;

	err = errUNEXPECTED;

	if( !m_dad )
	{
		if( m_tm->m_bl )
			err = m_tm->m_bl->GetSettings(settings);
	}
	else
	{
		cTmTaskPtr task(this, true);
		if( task )
			err = task->GetSettings(settings);
	}
	return err;
}

tERROR cTmProfile::SetTaskSettings(cSerializable* settings)
{
	if( !m_taskObj && m_dad && !m_bTaskGroup )
		return errOK;

	bool bDestroy = !settings;
	if( !settings )
		GetSettings(csdFROM_DATA_TO_TASK, settings);

	if( !settings )
		return errOK;

	tERROR err = m_bTaskGroup ? errOK : errUNEXPECTED;
	if( !m_dad )
	{
		if( m_tm->m_bl )
			err = m_tm->m_bl->SetSettings(settings);
	}
	else if( m_taskObj )
	{
		tBOOL bMakePersistent = cFALSE;
		if( m_tm->m_bl )
			m_tm->m_bl->CustomizeSettings(m_sName.c_str(cCP_ANSI),
				settings, m_cfg.level(), csdFROM_DATA_TO_TASK, &bMakePersistent);

		err = m_taskObj->SetSettings(settings);

		if( bMakePersistent )
			SetSettings(csdFROM_TASK_TO_DATA, NULL);
	}

	if( m_bTaskGroup )
	{
		cTmProfileEnum child(this);
		while( child.next() )
			child->SetTaskSettings(settings);
	}

	if( bDestroy )
		g_root->DestroySerializable(settings);

	if( PR_SUCC(err) )
		PR_TRACE((m_tm, prtIMPORTANT, "tm\t%S - task settings applied, task object(0x%08x)", m_sName.data(), (cObj*)m_task));
	else
		PR_TRACE((m_tm, prtERROR, "tm\t%S - task settings not applied(%terr), task object(0x%08x)", m_sName.data(), err, (cObj*)m_task));

	return err;
}

tERROR cTmProfile::SetTaskState(tTaskRequestState state)
{
	tERROR err = errOK;
	cTmTaskPtr task(this);
	if( !task )
		return warnTASK_STATE_CHANGED;

	if( state == TASK_REQUEST_RUN )
	{
		tERROR imperr = SetTaskImpersonation(false);
		if(m_cfg.m_runas.m_enable && !m_bImpersonated)
		{	
			PR_TRACE((m_tm, prtERROR, "tm\t%S - task failed to start under specified account", m_sName.data()));
			return imperr == errNOT_SUPPORTED ? errTASK_CANNOT_LOGON_IN_SAFE_MODE : errTASK_LOGON_FAILED;
		}
		cStrObj strUser;
		if(m_token)
			strUser.assign(m_token, pgOBJECT_NAME);
		else
			strUser.assign("SYSTEM");
		PR_TRACE((m_tm, prtIMPORTANT, "tm\t%S - task starting under '%S' account", m_sName.data(), strUser.data()));
	}

	err = task->SetState(state);

	PR_TRACE((m_tm, prtIMPORTANT, "tm\t%S - task object(0x%08x) SetState(%terr)",
		m_sName.data(), (cTask*)task, err));

	if( state == TASK_REQUEST_RUN )
		SetTaskImpersonation(true);
	return err;
}

tERROR cTmProfile::SetTaskImpersonation(bool bReset)
{
	if( !bReset )
	{
		close_object(m_token);
		m_token = m_request->m_token;
		m_request->m_token = NULL;
	}

	if( !m_token )
		return errOK;

	if( bReset )
		return m_token->Revert();

	cTaskRunAs& as = m_cfg.m_runas;
	if( !as.m_enable )
		return m_token->Impersonate();

	PR_TRACE((m_tm, prtIMPORTANT, "tm\t%S - try run as %S", m_sName.data(), as.m_sAccount.data()));

	tERROR err = errOK;
	cStrObj strPassword = as.m_sPassword;
	if( m_tm->m_hCryptoHelper )
	{
		err = m_tm->m_hCryptoHelper->CryptPassword(&cSerString(strPassword), PSWD_DECRYPT);
		if( PR_FAIL(err) )
		{
			PR_TRACE((m_tm, prtERROR, "tm\t%S - cannot decrypt password (%terr)", m_sName.data(), err));
			return err;
		}
	}

	err = m_token->Logon(cAutoString(as.m_sAccount), cAutoString(strPassword));
	strPassword.clean();

	if( PR_FAIL(err) )
		PR_TRACE((m_tm, prtFATAL, "tm\t%S - task cannot be impersonated (%terr) !!!!!!!!!!!!", m_sName.data(), err));
	else
		m_bImpersonated = cTRUE;

	return err;
}

// ------------------------------------------------

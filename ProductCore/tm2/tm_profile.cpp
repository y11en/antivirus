// ------------------------------------------------
#include <prague.h>
#include <hashutil/hashutil.h>
#include "tm.h"

// ------------------------------------------------

const char* GetStateDsc(int state)
{
	switch (state)
	{
	case TASK_REQUEST_RUN: return "RUN";
	case TASK_REQUEST_PAUSE: return "PAUSE";
	case TASK_REQUEST_STOP: return "STOP";
	case PROFILE_STATE_UNK: return "UNK";
	case PROFILE_STATE_RUNNING: return "RUNNING";
	case PROFILE_STATE_RUNNING_MALFUNCTION: return "RUNNING_MALFUNCTION";
	case PROFILE_STATE_PAUSED: return "PAUSED";
	case PROFILE_STATE_STOPPED: return "STOPPED";
	case PROFILE_STATE_COMPLETED: return "COMPLETED";
	case PROFILE_STATE_FAILED: return "FAILED";
	case PROFILE_STATE_ENABLED: return "ENABLED";
	case PROFILE_STATE_DISABLED: return "DISABLED";
	case PROFILE_STATE_STARTING: return "STARTING";
	case PROFILE_STATE_PAUSING: return "PAUSING";
	case PROFILE_STATE_RESUMING: return "RESUMING";
	case PROFILE_STATE_STOPPING: return "STOPPING";
	}
	// вообще сюда попадать не должны, так что ради простоты забьём на потокобезопасность
	static char buf[32] = {};
	sprintf(buf, "0x%08X", state);
	return buf;
}


tDWORD cTmProfile::AddRef()
{
	return PrInterlockedIncrement(&m_ref);
}

tDWORD cTmProfile::Release()
{
	tDWORD ref = PrInterlockedDecrement(&m_ref);
	if( !ref )
	{
		PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S has been released", m_sName.data () ));
		DeinitProfile();
		delete this;
	}
	return ref;
}

cTmProfile* cTmProfile::FindProfile(const tCHAR* profile, tDWORD task_id, hOBJECT object)
{
	cTmProfile* find = NULL;
	if( profile )
	{
		if( m_sName == (tCHAR*)profile )
		{
			if( object && m_tm->GetObjectProcess )
			{
				tDWORD pid = PR_PROCESS_LOCAL;
				m_tm->GetObjectProcess(object, &pid);

				if( pid != m_tm->m_processId )
					find = MakeRemoteProfile(pid);
			}

			if( !find )
				find = this;
		}
	}
	else if( object )
	{
		if( m_task == object || !m_dad && (cObj*)m_tm->m_bl == object )
			find = this;
	}
	else if( m_nRuntimeId == task_id )
		find = this;

	if( find )
	{
		find->AddRef();
		return find;
	}

	cTmProfileEnum child(this);
	while( child.next() )
		if( find = child->FindProfile(profile, task_id, object) )
			return find;

	return NULL;
}

cTmProfile* cTmProfile::MakeRemoteProfile(tDWORD pid)
{
	cStrObj strName;
	strName.format(cCP_ANSI, "%s%d", (tCHAR*)m_sName.c_str(cCP_ANSI),
		pid == PR_PROCESS_LOCAL ? m_aChildren.size() : pid);

	if( pid != PR_PROCESS_LOCAL )
	{
		cTmProfileEnum child(this);
		while( child.next() )
			if( child->m_sName == strName )
				return child;
	}

	cTmProfile* child = new cTmProfile();
	child->m_tm = m_tm;
	child->m_dad = this;
	child->m_processId = pid;
	child->m_bClone = cTRUE;
	child->m_iid = m_iid;
	child->m_pid = m_pid;
	child->m_nSerIdSettings = m_nSerIdSettings;
	child->m_nSerIdStatistics = m_nSerIdStatistics;
	child->m_bPersistent = m_bPersistent;
	child->m_bService = m_bService;
	child->m_eOrigin = pid != PR_PROCESS_LOCAL ? cProfile::poTemporary : m_eOrigin;
	child->m_sName = strName;
	child->m_sType = m_sType;
	child->m_cfg.m_bEnabled = cTRUE;
	child->m_cfg.m_bRemote = cTRUE;
	child->InitHost(true);

	UpdateChilds(child, true);
	m_bTaskGroup = cTRUE;
	return child;
}

cTmProfile* cTmProfile::RegisterRemoteTask(cTaskInfo* info, cObj* task, tDWORD processId)
{
	cTmProfile* profile = NULL;
	if( isService() )
	{
		if( m_cfg.m_bRemote )
			return NULL;

		profile = MakeRemoteProfile(processId);
	}
	else
	{
		if( m_cfg.m_bRemote )
			m_remoteTask = task;

		tINT i, c = m_aChildren.count();
		for( i = 0; i < c; ++i )
		{
			cTmProfile* _profile = GetChild(i);
			if( !_profile->m_remoteTask )
			{
				profile = _profile;
				break;
			}
		}
	}

	if( !profile && m_cfg.m_dwInstances == 1 )
		profile = this;

	if( !profile )
		return NULL;

	m_nRuntimeId = info->m_nParentId;

	profile->m_nRuntimeId = info->m_nTaskId;
	profile->m_remoteTask = task;

	if( isService() )
		profile->SetStateRequest(TASK_REQUEST_RUN, 0, REQUEST_FLAG_LOCK_TASK);
	else if( !persistent() )
		profile->SetStateRequest(TASK_REQUEST_RUN, 0, REQUEST_FLAG_RESTART_TASK);

	return profile;
}

void cTmProfile::UpdateChilds(cTmProfile* child, bool bAdd)
{
	bool bLock = m_tm->is_inited();
	if( bLock )
		m_enumRef.lock(false);

	if( bAdd )
	{
		m_aChildren.push_back().ptr_ref() = child;
	}
	else
	{
		tDWORD i, c = m_aChildren.size();
		for(i = 0; i < c; ++i)
		{
			cProfileExPtr& it =  m_aChildren.at(i);
			if( (cProfileEx*)it == child )
			{
				it.ptr_ref() = NULL;
				m_aChildren.remove(i,i);
				break;
			}
		}
	}

	if( bLock )
		m_enumRef.unlock(false);
}

tERROR cTmProfile::LockForReport(bool bLock)
{
	if( bLock )
		m_lock->Enter(SHARE_LEVEL_WRITE);
	else
		m_lock->Leave(NULL);

	cTmProfileEnum child(this);
	while( child.next() )
		child->LockForReport(bLock);
	return errOK;
}

bool cTmProfile::IsChildsDisabled()
{
	cTmProfileEnum child(this);
	while( child.next(true) )
		if( child->enabled() )
			return false;

	return true;
}

bool cTmProfile::IsEnableLocked()
{
	return m_cfg.m_dwMandatoriedFields.IsBitSet(m_tm->m_dwEnabledPolicyBit);
}

bool cTmProfile::IsLevelLocked()
{
	return m_cfg.m_dwMandatoriedFields.IsBitSet(m_tm->m_dwLevelPolicyBit);
}

tERROR cTmProfile::Clone(cTmProfile* clone, cProfile* data, bool bTmp)
{
	m_tm = clone->m_tm;

	cProfile* source = clone != m_tm ? clone : data;

	m_sName = source->m_sName;
	m_sType = source->m_sType;
	m_eOrigin = source->m_eOrigin;
	m_nAdmFlags = source->m_nAdmFlags & (PROFILE_ADMFLAG_ADMINABLE | PROFILE_ADMFLAG_GROUPTASK);
	m_sDescription = data->m_sDescription;
	m_tmCreationDate = m_tmLastModified = cDateTime::now_utc();
	m_bInstalled = cTRUE;

	m_cfg.m_dwLevel = source->m_cfg.m_dwLevel;
	m_cfg.m_bEnabled = cTRUE;
	m_cfg.m_schedule.m_eMode = schmManual;

	if( clone != m_tm )
	{
		m_iid = clone->m_iid;
		m_pid = clone->m_pid;
		m_runInSafeMode = clone->m_runInSafeMode;
		if( !clone->m_bHasTemplate )
			m_aReports = clone->m_aReports;

		if( data->m_sName.empty() || m_sName == data->m_sName )
		{
			tDWORD pos = m_sName.find_last_of("$");
			if( (cStrObj::npos != pos) && (cStrObj::npos == m_sName.find_first_not_of("0123456789",pos+1)) )
				m_sName.erase(pos);
			
			tCHAR str_id[10]; pr_sprintf( str_id, sizeof(str_id), "$%04u", m_tm->GetUniqueId());
			m_sName.append(str_id);
		}
		else
			m_sName = data->m_sName;

		clone->GetSettings(csdFROM_DATA_TO_GUI, m_cfg.m_settings.ptr_ref());
	}
	else
	{
		if( m_sName.empty() )
			return errPARAMETER_INVALID;

		m_cfg.assign(source->m_cfg, true);
	}

	if( bTmp )
		m_eOrigin = cProfile::poTemporary;

	if( m_eOrigin == cProfile::poUnk || m_eOrigin == cProfile::poSystem )
		m_eOrigin = cProfile::poUser;

	tERROR err = InitProfile(m_tm);

	if( m_eOrigin != cProfile::poTemporary )
	{
		m_tm->SaveData(this, m_sProfileKey, &m_tmCreationDate);
		m_tm->SaveData(this, m_sProfileKey, &m_eOrigin);
		m_tm->SaveData(this, m_sProfileKey, &m_sType);
		m_tm->SaveData(this, m_sProfileKey, &m_runInSafeMode);
		m_tm->SaveData(this, m_sProfileKey, &m_nAdmFlags);
		m_tm->SaveData(this, m_sProfileKey, &m_iid);
		m_tm->SaveData(this, m_sProfileKey, &m_pid);
		m_tm->SaveData(this, m_sProfileKey, &m_bInstalled);

		m_stateFlags = dfCfg|dfDescription|dfSettings;
		if( clone == m_tm )
			m_stateFlags |= dfSchedule|dfRunAs|dfSleepMode;

		SaveChanges();
	}

	if( PR_SUCC(err) )
	{
		m_tm->UpdateChilds(this, true);
		data->assign(*this, false);
		SendMessage(pmcPROFILE, pmPROFILE_CREATE);
	}

	InitSchedule(&m_cfg.schedule());

	ApplyChanges();

	return err;
}

tERROR cTmProfile::Delete()
{
	if( m_eOrigin == cProfile::poSystem && !m_bClone || m_bDeleted )
		return errOBJECT_INCOMPATIBLE;

	m_bDeleted = cTRUE;
	SendMessage(pmcPROFILE, pmPROFILE_DELETE);
	
	if( m_eOrigin != cProfile::poTemporary && !m_bClone )
	{
		SetStateRequest(TASK_REQUEST_STOP);
		m_tm->m_config->DeleteKey(cRegRoot, m_sProfileKey.c_str(cCP_ANSI));
	}

	m_dad->UpdateChilds(this, false);
	PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S deleted from list", m_sName.data() ));

	Release();
	return errOK;
}

tERROR cTmProfile::InitProfile(cTmProfile* pParent)
{
	m_dad = pParent;
	if( pParent )
		m_tm = pParent->m_tm;

	tERROR err = InitHost(true);

	if( !m_sType.empty() )
	{
		cStrObj strType("types");
		strType.check_last_slash();
		strType += m_sType;

		cTmTaskType type;
		if( m_tm->LoadData(&type, strType) )
		{
			strType.check_last_slash();
			m_sTypeKey         = strType;
			m_bHasTemplate     = cTRUE;
			m_nSerIdSettings   = type.m_nSerIdSettings;
			m_nSerIdStatistics = type.m_nSerIdStatistics;
			m_iid              = type.m_iid;
			m_pid              = type.m_pid;
			for(tDWORD i = 0; i < type.m_aReports.size(); i++)
				m_aReports.push_back(type.m_aReports[i]);
		}
	}

	InitProfileKey(m_sProfileKey);

	LoadSettings(&schedule(), 0);

	if( pParent && m_pid && !m_iid )
		PR_TRACE((m_tm, prtERROR, "tm\t%S.m_iid not initialized", m_sName.data() ));

	if( !m_nSerIdSettings )
	{
		if( m_pid != PID_ANY )
			m_nSerIdSettings = MAKE_SER_ID( m_pid, SER_SETTINGS_ID );
		else
			m_nSerIdSettings = cSerializable::eIID;
	}

	if( m_nSerIdStatistics == cSerializable::eIID )
		m_nSerIdStatistics = cTaskStatistics::eIID;
	else if( !m_nSerIdStatistics && (m_pid != PID_ANY) )
		m_nSerIdStatistics = MAKE_SER_ID( m_pid, SER_STATISTICS_ID );

	AdjustInactiveState(m_nState);
	m_statistics.init(m_nSerIdStatistics);

	if( !m_dad )
		m_nRuntimeId = 0;

	if( !m_tmCreationDate )
		m_tmCreationDate = cDateTime::now_utc();

	InitProfileInstances();

	tINT i, c = m_aChildren.count();
	for( i = c-1; i >= 0; --i )
		if( !GetChild(i)->m_bInstalled )
			m_aChildren.remove(i, i);

	m_bGroup = persistent() && m_aChildren.count();

	c = m_aReports.count();
	if( !c && !m_dad && !isService() )
	{
		cTaskReportDsc& tr = m_aReports.push_back();
		tr.m_id = "sys";
		tr.m_aClasses.push_back(pmc_TASK_REPORT);
	}

	for( i = 0; i < c && m_dad; ++i )
	{
		cTaskReportDsc& it = m_aReports[i];

		cTaskReportDsc* other = NULL;
		tDWORD j;
		for(j = 0; j < m_tm->m_aReports.size(); j++)
			if( m_tm->m_aReports[j].m_id == it.m_id )
			{
				other = &m_tm->m_aReports[j];
				break;
			}

		if( !other )
			continue;

		if( it.m_baseSerId == cSerializable::eIID )
			it.m_baseSerId = other->m_baseSerId;

		for(j = 0; j < other->m_aClasses.size(); ++j)
			it.m_aClasses.push_back(other->m_aClasses[j]);
	}

	if( PR_SUCC(err) )
		m_tm->ApplyPolicy(&m_cfg, this);

	if( PR_SUCC(err) )
	{
		c = m_aChildren.count();
		for( i = 0; i < c; ++i )
			GetChild(i)->InitProfile(this);
	}
	return err;
}

tERROR cTmProfile::InitProfileInstances()
{
	if( isService() || !persistent() || m_bClone )
		return errOK;

	tDWORD c = m_aChildren.count();
	if( c && !m_bTaskGroup)
		return errOK;

	if( !m_cfg.m_bRemote )
		c++;

	tLONG i, diff = m_cfg.m_dwInstances - c;

	m_bGroup = m_cfg.m_dwInstances > 1 || m_cfg.m_bRemote;

	if( diff > 0 )
	{
		if( !m_bGroup )
			return errOK;

		for( i = 0; i < diff; ++i )
			MakeRemoteProfile(PR_PROCESS_LOCAL);

		if( IsActive() )
			SetStateRequest(TASK_REQUEST_RUN);
	}
	else if( diff < 0 )
	{
		cTmProfileEnum child(this);
		for( i = 0; i < -diff && child.next(); ++i )
		{
			child->SetStateRequest(TASK_REQUEST_STOP);
			if( !isService() )
				child->PutRequest(new cTmProfileRequest(PROFILE_REQUEST_DELETE));
		}
	}

	return errOK;
}

tERROR cTmProfile::InitFromReport(cTmProfile* pParent, cProfileEx& data)
{
	m_dad = pParent;
	if( pParent )
		m_tm = pParent->m_tm;

	m_stateFlags = dfSettingsLoaded;
	m_bFromReport = cTRUE;

	*(cProfileEx*)this = data;
	tERROR err = InitHost(false);

	tINT i, c = m_aChildren.count();
	for( i = 0; i < c; ++i )
		GetChild(i)->InitFromReport(this, *data.m_aChildren.at(i));

	return err;
}

tERROR cTmProfile::InitFromReport(cProfileEx& data)
{
	if( m_stateFlags & dfHistoryInited )
		return errOK;

	m_nRuntimeId = data.m_nRuntimeId;
	m_lastErr = data.m_lastErr;
	m_nState = data.m_nState;
	m_aReports = data.m_aReports;
	m_statistics.clear();
	m_statistics.assign(*data.statistics());
	m_stateFlags |= dfHistoryInited;
	return errOK;
}

tERROR cTmProfile::DeinitProfile()
{
	tINT i, c = m_aChildren.count();
	for( i = 0; i < c; ++i )
		GetChild(i)->DeinitProfile();

//	if( m_task ) __asm{ int 3 }

	if( m_host )
		m_host->sysCloseObject();

	return errOK;
}

tERROR cTmProfile::InitHost(bool bLocker)
{
	cObj* parent = m_dad ? (cObj*)m_dad->m_host : (cObj*)m_tm;

	tERROR err = parent->sysCreateObject((hOBJECT*)&m_host, IID_MSG_RECEIVER);
	if( PR_SUCC(err) )
		err = m_host->propSetPtr(pgRECEIVE_CLIENT_ID, this);
	if( PR_SUCC(err) )
		err = m_host->propSetPtr(pgRECEIVE_PROCEDURE, (void*)MsgReceive);
	if( PR_SUCC(err) )
		err = m_host->sysCreateObjectDone();

	if( PR_SUCC(err) && bLocker )
		err = m_host->sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);	

	return err;
}

tERROR cTmProfile::InitDataStorage(cObject* obj, cRegistry** p_storage)
{
	tPROPID prop = m_dad ? pgTASK_PERSISTENT_STORAGE : pgBL_PRODUCT_DATA_STORAGE;
	if( !p_storage )
	{
		if( !obj || m_eOrigin == cProfile::poTemporary || m_bClone )
			return errOBJECT_BAD_INTERNAL_STATE;

		if( !obj->propSize(prop) )
			return errOK;
	}

	tERROR err = errOK;
	if( !m_storage )
	{
		tBOOL ro = cFALSE;
		while (1)
		{
		cAutoObj<cRegistry> registry;

		err = m_tm->sysCreateObject( registry, IID_REGISTRY, PID_REGMAP );
		cStrObj root;
		if( PR_SUCC(err) )
		{
			cStrObj root;
			root += m_sProfileKey;
			root.check_last_slash();
			root += "data";

			registry->propSetObj(plPARENT_REG, (cObj*)m_tm->m_config);
			registry->propSetStr(0, plPARENT_KEY_STR, root.c_str(cCP_ANSI));
		}

		if( PR_SUCC(err) )
				err = registry->propSetBool( pgOBJECT_RO, ro );

		if( PR_SUCC(err) )
			err = registry->sysCreateObjectDone();

			// bf 30020: т.к. в плагине для АК запущенном под ограниченным пользователем DataStorage не доступен
			// на запись, откроем на чтение
			if (PR_FAIL(err) && !m_dad && ro == cFALSE)
			{
				PR_TRACE(( this, prtIMPORTANT, "tm\tTrying to create READ ONLY data for BL"));
				ro = cTRUE;
				continue;
			}

		if( PR_SUCC(err) )
			m_storage = registry.relinquish();

			break;
		}
	}

	if( PR_SUCC(err) )
	{
		if( !p_storage )
		{
			// если задаче storage не нужен, то в ней не должно быть свойства pgTASK_PERSISTENT_STORAGE
			obj->propSetObj(prop, (cObj*)m_storage);
		}
		else
			*p_storage = m_storage;
	}
	return err;
}

tERROR cTmProfile::InitProfileKey(cStrObj& str)
{
	if( !m_dad )
		return errOK;

	m_dad->InitProfileKey(str);
	str.check_last_slash();
	str += "profiles";
	str.check_last_slash();
	str += m_sName;

	return errOK;
}

tERROR cTmProfile::InitSchedule(cTaskSchedule* sch, bool bApplyChanges)
{
	if( !m_tm->m_scheduler )
		return errUNEXPECTED;

	if( isService() )
		return errOBJECT_INCOMPATIBLE;

	if( sch )
	{
		schedule().assign(*sch, true);

		if( !schedule().m_nFirstRunTime )
			schedule().m_nFirstRunTime = cDateTime::now_local();

		if(schedule().m_eType == schtExactTime || !schedule().m_bRaiseIfSkipped)
			schedule().m_nLastRunTime = 0;
		else
			schedule().m_nLastRunTime = ((__time32_t)cDateTime::now_local()) - 1;

		m_changedFlags |= dfSchedule;
		if( bApplyChanges )
			ApplyChanges();
	}
	else
	{
		if( schedule().mode() == schmEnabled && schedule().type() == schtOnStartup )
				SetStateRequest(TASK_REQUEST_RUN, schedule().m_nStartDelay*60);

		cTmProfileEnum child(this);
		while( child.next() )
			child->InitSchedule();
	}

	if( schedule().mode() == schmManual ||
		schedule().type() == schtOnStartup ||
		schedule().type() == schtAfterUpdate )
	{
		if( m_scheduleId )
		{
			m_tm->m_scheduler->DeleteSchedule(pmc_SCHEDULER, m_scheduleId);
			m_scheduleId = 0;
		}
		return errOK;
	}

	cTaskSchedule schTemp(schedule());
	if( m_tm->m_bl )
	{
		tBOOL bMakePersistent = cFALSE;
		m_tm->m_bl->CustomizeSettings(m_sName.c_str(cCP_ANSI), &schTemp,
			SETTINGS_LEVEL_UNK, csdFROM_GUI_TO_DATA, &bMakePersistent);
	}

	m_scheduleId = iCountCRC32str(m_sName.c_str(cCP_ANSI));
	cScheduleParams params(*this, TASK_REQUEST_RUN, NULL);
	return m_tm->m_scheduler->SetSchedule(pmc_SCHEDULER, m_scheduleId, &schTemp, &params);
}

tERROR cTmProfile::LoadCfg(cCfg* cfg, bool bPolicy)
{
	tERROR error = m_tm->LoadData(cfg, m_sProfileKey, NULL, bPolicy);
	if( PR_SUCC(error) )
	{
		if( bPolicy)
			m_tm->ApplyPolicy(cfg, this);

		if( m_nSerIdSettings != cSerializable::eIID )
		{
			if( !cfg->m_settings )
				error = g_root->CreateSerializable(m_nSerIdSettings, &cfg->m_settings);
			if( PR_SUCC(error) )
				error = LoadSettings(cfg->m_settings, cfg->m_dwLevel, false, bPolicy);
		}
	}
	return error;
}

tERROR cTmProfile::LoadSettings(cSerializable* settings, tDWORD level, bool bApply, bool bPolicy)
{
	if( !settings )
		return errPARAMETER_INVALID;

	cStrObj sProfileKey(m_sProfileKey), sTypeKey;
	sProfileKey.check_last_slash();

	if( settings->isBasedOn(cCfg::eIID) )
	{
		cCfg& cfg = *(cCfg*)settings;
		cfg.m_dwLevel = level;
		LoadSettings(cfg.m_settings, level, bApply, bPolicy);

		if( level == SETTINGS_LEVEL_RESET )
		{
			cfg.m_bEnabled = cTRUE;
			cfg.m_dwLevel = SETTINGS_LEVEL_DEFAULT;
			cfg.m_runas = cTaskRunAs();
			LoadSettings(&cfg.m_schedule, level, bApply, bPolicy);
		}

		if( cfg.m_dwLevel == SETTINGS_LEVEL_DEFAULT )
		{
			cfg.m_smode.m_bEnableAppList = cFALSE;
			cfg.m_smode.m_bEnablePeriod = cFALSE;
		}
	}
	else if( settings->isBasedOn(cTaskSchedule::eIID) )
	{
		sProfileKey += "schedule";
		m_tm->LoadData(settings, sProfileKey, "def");

		if( level != SETTINGS_LEVEL_RESET )
			m_tm->LoadData(settings, sProfileKey);
		return errOK;
	}
	else
	{
		sProfileKey += "settings";

		if( m_bHasTemplate )
		{
			sTypeKey = m_sTypeKey;
			sTypeKey += "settings";
		}

		if( level == SETTINGS_LEVEL_RESET )
		{
			cSerializableObj def_settings;
			def_settings.init(m_nSerIdSettings);
			if( def_settings )
				g_root->CopySerializable(&settings, def_settings);
		}

		if( level == SETTINGS_LEVEL_RESET || !bApply )
		{
			if( m_bHasTemplate )
				m_tm->LoadData(settings, sTypeKey, "def", bPolicy);

			m_tm->LoadData(settings, sProfileKey, "def", bPolicy);
		}

		if( level != SETTINGS_LEVEL_CUSTOM )
		{
			if( m_bHasTemplate )
				m_tm->LoadData(settings, sTypeKey, "0", bPolicy);

			m_tm->LoadData(settings, sProfileKey, "0", bPolicy);

			if( level != SETTINGS_LEVEL_RESET && level != SETTINGS_LEVEL_DEFAULT )
			{
				tCHAR  load_key[10];
				pr_sprintf(load_key, sizeof(load_key), "%u", level);

				if( m_bHasTemplate )
					m_tm->LoadData(settings, sTypeKey, load_key, bPolicy);

				m_tm->LoadData(settings, sProfileKey, load_key, bPolicy);
			}
		}

		if( level != SETTINGS_LEVEL_RESET && !bApply )
			m_tm->LoadData(settings, sProfileKey, NULL, bPolicy);

		if( m_tm->m_bl )
		{
			tBOOL bMakePersistent = cFALSE;
			if( !m_dad )
				m_tm->m_bl->CustomizeSettings(m_sName.c_str(cCP_ANSI), settings,
					level, csdFROM_DATA_TO_TASK, &bMakePersistent);

			if( level == SETTINGS_LEVEL_RESET )
			{
				PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S LoadSettings::CustomizeSettings(Reset)", m_sName.data() ));
				m_tm->m_bl->CustomizeSettings(m_sName.c_str(cCP_ANSI), settings, level, csdDATA_INIT, &bMakePersistent);
			}
		}
	}

	if( bPolicy )
		m_tm->ApplyPolicy(settings, this);

	return errOK;
}

tERROR cTmProfile::SaveCfg(cCfg& cfg, tDWORD flags, bool bPolicy)
{
	if( flags & dfCfg )
	{
		if( !IsEnableLocked() == bPolicy )
			m_tm->SaveData(&cfg, m_sProfileKey, &cfg.m_bEnabled);
		if( !IsLevelLocked() == bPolicy )
			m_tm->SaveData(&cfg, m_sProfileKey, &cfg.m_dwLevel);
	}

	if( flags & dfInstances )
		m_tm->SaveData(&cfg, m_sProfileKey, &cfg.m_dwInstances);

	if( flags & dfSchedule )
		m_tm->SaveData(&cfg, m_sProfileKey, &cfg.m_schedule);

	if( flags & dfRunAs )
		m_tm->SaveData(&cfg, m_sProfileKey, &cfg.m_runas);

	if( flags & dfSleepMode )
		m_tm->SaveData(&cfg, m_sProfileKey, &cfg.m_smode);

	if( flags & dfAKUnsupportedFields )
		m_tm->SaveData(&cfg, m_sProfileKey, &cfg.m_aAKUnsupportedFields);

	if( flags & dfTaskSettings )
		if( PR_SUCC(GetTaskSettings(cfg.m_settings.ptr_ref())) )
			flags |= dfSettings;

	if( flags & dfSettings )
	{
		PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S settings store begin...", m_sName.data() ));
		m_tm->SaveData(&cfg, m_sProfileKey, &cfg.m_settings, bPolicy);
		m_tm->SaveData(this, m_sProfileKey, &m_tmLastModified);
		m_tm->SaveData(this, m_sProfileKey, &m_tmLastCompleted);
		PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S settings store done", m_sName.data() ));
	}

	return errOK;
}

tERROR cTmProfile::SaveChanges(tDWORD flags)
{
	cAutoCS locker(m_lock, true);

	ApplyChanges();

	tDWORD stateFlags = (m_stateFlags & flags);

	if( stateFlags & dfSendModified )
		m_stateFlags &= ~dfSendModified;

	if( !(m_stateFlags & dfDirtyFlags) )
	{
		if( !m_changedFlags && (stateFlags & dfSettingsLoaded) && m_eOrigin != cProfile::poTemporary )
		{
			m_stateFlags &= ~dfSettingsLoaded;
			m_cfg.m_settings.clear();
			PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S discard settings", m_sName.data() ));
		}
	}
	else if( flags & dfDirtyFlags )
	{
		if( m_eOrigin != cProfile::poTemporary && !m_bClone )
		{
			if( stateFlags & dfDescription )
				m_tm->SaveData(this, m_sProfileKey, &m_sDescription);

			SaveCfg(m_cfg, stateFlags, true);
		}

		if( (stateFlags & dfReport) && IsReportable() )
		{
			LockForReport(true);

			GetStatistics(statistics());

			if( !m_cfg.m_settings )
				GetSettings(csdFROM_DATA_TO_GUI, m_cfg.m_settings.ptr_ref(), true); //!!!

			tScheduleMode schMode = schedule().m_eMode;
			if( m_client )
				schedule().m_eMode = schmManual;

			m_tm->m_report.UpdateRecord(this);

			schedule().m_eMode = schMode;
			LockForReport(false);
		}
		else if ((stateFlags & dfReport) && !isService())
			GetStatistics(statistics());

		m_stateFlags &= ~(flags & dfDirtyFlags);
	}

//	if( flags & dfDirtyFlags )
		CheckSleepingMode();

	locker.unlock();

	if( stateFlags & dfSendModified )
	{
		SendMessage(pmcPROFILE, pmPROFILE_MODIFIED);
		SendMessage(pmcPROFILE, pmPROFILE_SETTINGS_MODIFIED);
	}

	if( (flags & dfDirtyFlags) && m_eOrigin == cProfile::poTemporary && !m_bClone )
		if( IS_TASK_FINISHED(m_nState) && PrGetTickCount() - m_timeAccess > 100000 )
			PutRequest(new cTmProfileRequest(PROFILE_REQUEST_DELETE));

	cTmProfileEnum child(this);
	while( child.next() )
		child->SaveChanges(flags);
	return errOK;
}

tERROR cTmProfile::CheckSleepingMode()
{
	tDWORD dwSleepingMode = 0;
	tBOOL  bSleepingAppAdded = cFALSE;

	cTaskSleepingMode& smode = m_cfg.m_smode;
	if( smode.m_bEnablePeriod )
	{
		tDWORD tmNow = (tDWORD)(_TIME_((time_t)cDateTime::now_local()));
		tDWORD tmTo = (tDWORD)(_TIME_(smode.m_tmTo));
		tDWORD tmFrom = (tDWORD)(_TIME_(smode.m_tmFrom));

		if( ((tmTo > tmFrom) && (tmNow > tmFrom) && (tmNow < tmTo)) ||
			((tmTo <= tmFrom) && ((tmNow < tmTo) || (tmNow > tmFrom))) )
			dwSleepingMode |= 1;
	}

	if( smode.m_bEnableAppList && m_tm->m_bl )
	{
		tERROR error = m_tm->m_bl->AskAction(this, cProfile::PREPROCESS_SLEEPING_MODE, &smode);
		if( PR_SUCC(error) )
		{
			dwSleepingMode |= 2;
			if( error == errOK_DECIDED )
				bSleepingAppAdded = cTRUE;
		}
	}

	if( !dwSleepingMode )
	{
		if( m_dwSleepingMode )
		{
			if( IsPaused() )
				SetStateRequest(TASK_REQUEST_RUN);
			m_dwSleepingMode = 0;
		}
	}
	else
	{
		if( ((dwSleepingMode & 1) && !(m_dwSleepingMode & 1)) || bSleepingAppAdded )
		{
			if( IsActive() )
			{
				SetStateRequest(TASK_REQUEST_PAUSE);
				m_dwSleepingMode = dwSleepingMode;
			}
		}
	}
	return errOK;
}

tERROR cTmProfile::CheckImpersonation(cTaskRunAs* pRunAs)
{
	if( m_bImpersonated )
		return errOK;

	if( !pRunAs || !pRunAs->isBasedOn(cTaskRunAs::eIID) )
		return errPARAMETER_INVALID;

	pRunAs->assign(m_cfg.m_runas, false);
	return errNOT_OK;
}

tERROR cTmProfile::SaveScheduledRun()
{
	tDATETIME dtNow;
	Now(&dtNow);
	schedule().m_nLastRunTime = cDateTime(&dtNow);
	return m_tm->SaveData(&m_cfg, m_sProfileKey, &m_cfg.m_schedule);
}

tERROR cTmProfile::ApplyChanges()
{
	cAutoCS locker(m_lock, true);
	tDWORD changedFlags = m_changedFlags;
	m_stateFlags |= m_changedFlags;
	m_changedFlags = 0;
	locker.unlock();

	if( changedFlags )
	{
		PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S send modified", m_sName.data() ));
		SendMessage(pmcPROFILE, pmPROFILE_MODIFIED);
	}

	if( changedFlags & dfSettings )
		SendMessage(pmcPROFILE, pmPROFILE_SETTINGS_MODIFIED);

	return errOK;
}

tERROR cTmProfile::CheckStopAllowed(bool bRecursive)
{
	tERROR error = errOK;
	if( m_tm->m_isClosing )
		return error;

	if( bRecursive )
	{
		cTmProfileEnum child(this);
		while( child.next() )
			if( PR_FAIL(error = child->CheckStopAllowed(true)) )
				return error;
	}

	cTmTaskPtr task(this);
	if( task )
	{
		error = task->sysSendMsg(pmcTM_EVENTS, pmTM_EVENT_CHECK_STOP_ALLOWED, 0, 0, 0);
		if (PR_FAIL(error))
			PR_TRACE(( this, prtIMPORTANT, "tm\tTask %S deny to stop, err(%terr)", m_sName.data(), error));
	}

	return error;
}

tERROR cTmProfile::OpenReport(const tCHAR* p_name, hOBJECT p_client, tDWORD access, cReport** p_report)
{
	if( !p_name )
		p_name = "sys";

	cAutoCS locker(m_lock, true);
	tDWORD i, c = m_aReports.size();
	for(i = 0; i < c; ++i)
	{
		cTaskReportDsc& dsc = m_aReports.at(i);
		if( dsc.m_id == (tCHAR*)p_name )
			return m_tm->OpenReport(dsc.m_name.c_str(cCP_ANSI), p_client, access, p_report);
	}
	return errNOT_FOUND;
}

tERROR cTmProfile::SendMessage(tDWORD msg_cls, tDWORD msg_id)
{
	if( m_bClone )
		return errOK;

	cProfileBase profile(*this);

	cObj* client = m_client;
	if( client && PR_FAIL(m_host->sysValidateLock(client)) )
		client = m_client = NULL;

	tERROR error = m_host->sysSendMsg(msg_cls, msg_id, client, &profile, SER_SENDMSG_PSIZE);

	if( client )
		client->sysRelease();
	return error;
}

tERROR cTmProfile::MsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT recieve_point, tPTR pbuff, tDWORD* blen)
{
	cTmProfile* pThis = (cTmProfile*)_this->propGetPtr(pgRECEIVE_CLIENT_ID);
	cSerializable* data = (blen == SER_SENDMSG_PSIZE) ? (cSerializable*)pbuff : NULL;

	if( obj == (cObj*)pThis->m_host )
		return errOK;

	switch( msg_cls )
	{
		case pmc_TASK_STATE_CHANGED:    pThis->StateChanged((tProfileState)msg_id, pbuff ? *(tERROR*)pbuff : errOK, obj); break;
		case pmc_TASK_SETTINGS_CHANGED: pThis->SetSettings(csdFROM_TASK_TO_DATA, NULL); break;
		default: if( data ) pThis->ProcessMessage(data, msg_cls, msg_id, obj);
	}
	return errOK;
}

tERROR cTmProfile::ProcessMessage(cSerializable* data, tDWORD msg_cls, tDWORD msg_id, cObj* sender)
{
	cTaskHeader* header = m_tm->InitTaskHeader(data, sender);

     if( m_client && m_cfg.m_bRemote )
         m_host->sysSendMsg(msg_cls, msg_id, NULL, data, SER_SENDMSG_PSIZE);

	if( m_tm->m_bl && PR_FAIL(m_tm->m_bl->FilterTaskMessage(this, 0, msg_cls, msg_id, data)) )
		return errOK;

	ReportMessage(data, msg_cls, header);
	return errOK;
}

tERROR cTmProfile::ReportMessage(cSerializable* data, tDWORD msg_cls, cTaskHeader* header)
{
	tERROR err = errNOT_FOUND;
	if( !m_dad )
		return err;

	cBuff<tBYTE,0x400> buff;
	tDWORD i, c = m_aReports.size();
	for(i = 0; i < c; ++i)
	{
		cTaskReportDsc& dsc = m_aReports.at(i);
		if( (dsc.m_baseSerId != cSerializable::eIID) && !data->isBasedOn(dsc.m_baseSerId) )
			continue;
		
		if( header && !dsc.m_sTaskType.empty() && dsc.m_sTaskType != header->m_strTaskType )
			continue;

		cReport* report = m_vecReports[i];
		if( !report )
			continue;

		tDWORD j, k = dsc.m_aClasses.count(), size;
		for(j = 0; j < k; ++j)
			if( dsc.m_aClasses[j] == msg_cls )
			{
				err = g_root->StreamSerialize(data, SERID_UNKNOWN, buff, buff.size(), &size);
				if( err == errBUFFER_TOO_SMALL )
					err = g_root->StreamSerialize(data, SERID_UNKNOWN, buff.get(size, false), size, NULL);
				if( PR_SUCC(err) )
					err = report->AddRecord(buff, size, NULL);
				break;
			}
	}

	if( PR_FAIL(err) && m_dad )
		err = m_dad->ReportMessage(data, msg_cls, header);

	return err;
}

tERROR cTmProfile::AskActionEx(tDWORD action_id, cSerializable* params, hOBJECT sender)
{
	if( m_bTaskGroup )
	{
		cTmProfileEnum child(this);
		while( child.next() )
			child->AskActionEx(action_id, params, sender);

		return errOK;
	}

	cTmTaskPtr task(this);
	if( !task )
		return errOBJECT_INVALID;

	return task->AskAction(action_id, params);
}

tERROR cTmProfile::SetStateRequest(tTaskRequestState state, tDWORD delay, tDWORD flags, cObj* client, tTaskId* id, cTmProfileRequest* parent)
{
	if( m_bFromReport )
		return errOBJECT_INCOMPATIBLE;

	if( PR_SUCC(ScheduleRequest(delay, flags, state, NULL)) )
		return errOK;

	tERROR err = errOK;
	tERROR errStartErrorReason = errOK;
	bool bPutParentRequest = false;
	if( state == TASK_REQUEST_RUN || state == TASK_REQUEST_UNK )
	{
		bool bDisabled = persistent() && !IsEnabled();

		if( m_tm->m_isClosing )
		{
			PR_TRACE((m_tm, prtWARNING, "tm\tTask %S cannot be started because tm closing", m_sName.data() ));
			err = errTASK_DISABLED;
		}
		else if( bDisabled && !(flags & REQUEST_FLAG_FORCE_RUN) )
		{
			PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S cannot be started because profile is disabled", m_sName.data() ));
			err = errTASK_DISABLED;
		}
		else if( schedule().type() == schtExactTime && !(flags & (REQUEST_FLAG_FORCE_RUN|REQUEST_FLAG_BY_SCHEDULE)) )
		{
			PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S cannot be started because profile is suspended", m_sName.data() ));
			err = errTASK_DISABLED;
		}
		else if( m_tm->m_safeBoot && (!m_runInSafeMode && IsPersistent() || (flags & REQUEST_FLAG_BY_SCHEDULE)) )
		{
			PR_TRACE((m_tm, prtWARNING, "tm\tTask %S cannot be started in safe mode", m_sName.data() ));
			errStartErrorReason = err = errTASK_CANNOT_START_IN_SAFE_NODE;
		}
		else if( !isService() && !m_bClone && m_tm->m_bl && PR_FAIL(err = m_tm->m_bl->AskAction(this, cProfile::PREPROCESS_START_TASK, NULL)) )
		{
			PR_TRACE((m_tm, prtERROR, "tm\tTask %S cannot be started because of bl error(%terr)", m_sName.data(), err));
			errStartErrorReason = err;
		}

		if( PR_SUCC(err) )
		{
			if( bDisabled )
				err = Enable(cTRUE, ENABLE_FLAG_NOSYNC);

			if( schedule().type() == schtExactTime && !(m_nAdmFlags & PROFILE_ADMFLAG_GROUPTASK) )
			{
				cTaskSchedule sch; sch.m_eMode = schmManual;
				InitSchedule(&sch, true);
			}

			if( !parent && m_dad && IsPersistent() && !(flags & REQUEST_FLAG_NON_RECURSIVE) )
			{
				parent = new cTmProfileRequest(parent, 0, state, NULL, NULL);
				bPutParentRequest = true;
			}
		}
	}
	else if( (flags & REQUEST_FLAG_FORCE_RUN) && IsEnableLocked() )
	{
		PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S cannot be stopped because of policy", m_sName.data()));
		errStartErrorReason = err = errTASK_DISABLED;
	}

	if( PR_SUCC(err) )
	{
		PR_TRACE((m_tm, prtIMPORTANT, "tm\t%S - SetStateRequest(%s)...", m_sName.data(), GetStateDsc(state)));

		cTmProfileRequest* request = new cTmProfileRequest(parent, flags, state, client, id);

		if( bPutParentRequest )
			m_dad->PutRequest(parent);

		if( !(flags & REQUEST_FLAG_NON_RECURSIVE) )
		{
			if( !persistent() && state == TASK_REQUEST_RUN )
				m_host->sysCreateObjectQuick((cObject**)&request->m_token, IID_TOKEN);

			cTmProfileEnum child(this);
			while( child.next() )
				if( (flags & REQUEST_FLAG_FORALL_TASKS) || child->IsPersistent() )
					child->SetStateRequest(state, 0, 0, NULL, NULL, request);
		}

		return PutRequest(request);
	}
	
	if( state == TASK_REQUEST_RUN && PR_FAIL(errStartErrorReason) )
	{
		cProfileBase profile(*this);
		profile.m_lastErr = errStartErrorReason;
		m_host->sysSendMsg(pmcPROFILE, pmPROFILE_CANT_START, NULL, &profile, SER_SENDMSG_PSIZE);
	}

	return err;
}

tERROR cTmProfile::SetInfoRequest(cSerializable* info, tDWORD delay, tDWORD flags, cTmProfileRequest* parent)
{
	if( m_bFromReport )
		return errOBJECT_INCOMPATIBLE;

	PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S SetInfoRequest", m_sName.data()));

	if( PR_SUCC(ScheduleRequest(delay, flags, TASK_REQUEST_UNK, info)) )
		return errOK;

	cTmProfileRequest* request = NULL;
	tERROR err;

	if( info->isBasedOn(cProfileEx::eIID) )
	{
		cProfileEx& profile = *(cProfileEx*)info;
		request = new cTmProfileRequest(NULL, flags, &profile, cProfile::eIID);

		tDWORD i, c = profile.m_aChildren.size();
		for(i = 0; i < c; i++)
		{
			cProfileEx* child = profile.m_aChildren[i];
			cTmProfilePtr profile(m_tm, child->m_sName.c_str(cCP_ANSI));
			if( profile )
				profile->SetInfoRequest(child, 0, 0, request);
		}
		err = PutRequest(request);
	}
	else if( info->isBasedOn(cCfgEx::eIID) )
	{
		cCfgEx& cfg = *(cCfgEx*)info;

		if( cfg.m_nFlags & cCfg::fPolicy )
			return m_tm->ApplyPolicy(NULL, &cfg, !cfg.m_aChildren.size() && !cfg.settings());

		request = NULL;
		if( !(cfg.m_nFlags & cCfg::fGroupOfCfg) )
			request = new cTmProfileRequest(NULL, flags, &cfg, cCfg::eIID);

		tDWORD i, c = cfg.m_aChildren.size();
		for(i = 0; i < c; i++)
		{
			cCfgEx& child = cfg.m_aChildren[i];
			cTmProfilePtr profile(m_tm, child.m_sName.c_str(cCP_ANSI));
			if( profile )
				profile->SetInfoRequest(&child, 0, 0, request);
		}
		err = request ? PutRequest(request) : errOK;
	}
	else
		err = PutRequest(request = new cTmProfileRequest(NULL, flags, info));

	return err;
}

tERROR cTmProfile::SetState()
{
	tTaskRequestState state = m_request->m_requestState;
	tERROR err = errOK;

	PR_TRACE((m_tm, prtIMPORTANT, "tm\t%S - SetState(%s)...", m_sName.data(), GetStateDsc(state)));

	if( m_request->m_taskId )
		*m_request->m_taskId = m_nRuntimeId;

	if( state == PROFILE_STATE_FAILED || state == PROFILE_STATE_COMPLETED )
	{
		StateChanged((tProfileState)state);
		return err;
	}

	if( m_request->m_flags & REQUEST_FLAG_UNLOCK_TASK )
	{
		if( PR_FAIL(UnlockTask()) )
			return err;
	}

	bool bIsActive = IsActive(), bSetState = false, bLockedSync = false, bTaskInited = false;

	if( state == TASK_REQUEST_UNK && bIsActive )
		return (m_request->m_flags & OPEN_TASK_MODE_CREATE) ? errFOUND : errOK;

	if( state == TASK_REQUEST_STOP && PR_FAIL(err = CheckStopAllowed(false)) )
	{
		PR_TRACE((m_tm, prtIMPORTANT, "tm\t%S - stop not allowed(%terr)", m_sName.data(), err));
		return err;
	}

	if( !bIsActive )
		bSetState = (state == TASK_REQUEST_RUN) && IsEnabled();
	else if( IsPaused() )
		bSetState = (state != TASK_REQUEST_PAUSE);
	else
		bSetState = (state != TASK_REQUEST_RUN);

	if( state == TASK_REQUEST_UNK || state == TASK_REQUEST_RUN )
	{
		if( !m_lockRef )
			bTaskInited = PR_SUCC(InitTask(state));
		else if( m_request->m_flags & REQUEST_FLAG_LOCK_TASK )
		{
			bSetState = false;
			if( m_nState == PROFILE_STATE_FAILED )
				err = m_lastErr;
		}
	}

	if( bSetState )
	{
		switch(state)
		{
		case TASK_REQUEST_RUN:
			StateChanged(IsPaused() ? PROFILE_STATE_RESUMING : PROFILE_STATE_STARTING);
			m_nNewState = PROFILE_STATE_RUNNING;
			break;
		case TASK_REQUEST_PAUSE:
			StateChanged(PROFILE_STATE_PAUSING);
			m_nNewState = PROFILE_STATE_PAUSED;
			break;
		case TASK_REQUEST_STOP: 
			StateChanged(PROFILE_STATE_STOPPING);
			m_nNewState = PROFILE_STATE_STOPPED;
			break;
		}
	}

	if( bTaskInited && state == TASK_REQUEST_UNK )
		err = LockTask();

	if( m_request->m_taskId )
	{
		*m_request->m_taskId = m_nRuntimeId;
		m_request->wait_for_childs_inited();
		m_request->signal(err);
	}

	if( bTaskInited && state != TASK_REQUEST_UNK )
		err = LockTask();

	if( m_request->m_flags & REQUEST_FLAG_LOCK_TASK )
	{
		if( PR_SUCC(err) )
			err = LockTask();

		if( m_request->m_flags & REQUEST_FLAG_WAIT )
			bLockedSync = true;
		else
			m_request->signal(err);
	}

	if( PR_FAIL(err) || !bSetState )
	{
		if( state == TASK_REQUEST_STOP && m_nState == PROFILE_STATE_FAILED && !m_lockRef && persistent() )
			StateChanged(PROFILE_STATE_STOPPED);

		return err;
	}

	m_bWaitState = cTRUE;
	if( (err = SetTaskState(state)) != errOK )
		m_bWaitState = cFALSE;

	if( err == warnTASK_STATE_CHANGED )
		err = errOK;

	m_request->wait_for_childs(m_bWaitState);

	StateChanged(PR_FAIL(err) ? PROFILE_STATE_FAILED : m_nNewState, err);
	UpdateGroupState();

	if( bLockedSync && !IsActive() )
	{
		UnlockTask();

		if( PR_SUCC(err) )
			err = errUNEXPECTED;
	}
	return err;
}

tERROR cTmProfile::StateChanged(tProfileState state, tERROR nError, cObject* task)
{
	if( state == PROFILE_STATE_UNK )
		state = PROFILE_STATE_FAILED;

	PR_TRACE((m_tm, prtIMPORTANT, "tm\t%S - state changed to %s...", m_sName.data(), GetStateDsc(state)));

	if( /*IS_TASK_FINISHED(state) && */nError != errOK )
		m_lastErr = nError;

	if( task )
	{
		if( !m_bWaitState )
		{
			if( state != PROFILE_STATE_FAILED && state != PROFILE_STATE_COMPLETED )
			{
				PR_TRACE((m_tm, prtERROR, "tm\t%S - StateChanged unexpected state(%s)", m_sName.data(), GetStateDsc(state)));
				return errOK;
			}

			if( !isService() )
				m_bFailOver = (nError == errPROXY_STATE_INVALID);

			if( m_bFailOver && m_tm->m_isClosing )
				return errOK;

			return SetStateRequest((tTaskRequestState)state, 0, 0, task);
		}

		m_bWaitState = cFALSE;
		m_nNewState = state;
		return errOK;
	}

	bool bCompleted = IS_TASK_FINISHED(state);
	if( bCompleted )
	{
		if( m_nRuntimeId == TASKID_UNK )
			return errOK;

		if( persistent() )
			AdjustInactiveState(state);
	}

	if( state == m_nState )
		return errOK;

	m_nState = state;

	cAutoCS locker(m_lock, true);
	m_stateFlags |= dfReport;
	locker.unlock();

	if( bCompleted )
		m_timeFinish = (tDWORD)cDateTime::now_utc();

	if( state == PROFILE_STATE_DISABLED )
		m_lastErr = errOK;

	if( state == TASK_STATE_COMPLETED && nError == errOK )
	{
		m_tmLastCompleted = cDateTime::now_utc();
		m_changedFlags |= dfSendModified|dfSettings;
	}

	if( bCompleted || m_nState == PROFILE_STATE_RUNNING || m_nState == PROFILE_STATE_PAUSED )
		SaveChanges();

	if( bCompleted )
		UnlockTask();

	SendMessage(pmcPROFILE, pmPROFILE_STATE_CHANDED);

	if( m_dad && persistent() )
		m_dad->UpdateGroupState();

	return errOK;
}

tERROR cTmProfile::AdjustInactiveState(tProfileState& state)
{
	cTmProfile* profile = this;
	while( profile )
	{
		if( !profile->enabled() )
		{
			state = PROFILE_STATE_DISABLED;
			return errOK;
		}
		else if( state == PROFILE_STATE_FAILED )
		{
			if( m_bFailOver && m_tm->m_bl &&
					PR_SUCC(m_tm->m_bl->AskAction(this, cProfile::PREPROCESS_RESTART_TASK, NULL)) )
				profile->SetStateRequest(TASK_REQUEST_RUN);
		}

		if( !profile->persistent() )
			break;

		profile = profile->m_dad;
	}

	if( state != PROFILE_STATE_FAILED )
		state = PROFILE_STATE_ENABLED;
	return errOK;
}

tERROR cTmProfile::UpdateGroupState()
{
	if( !m_bGroup )
		return errOK;

	tDWORD nChildMask = 0; bool bPaused = true;
	tERROR nChildError = errOK;

	cTmProfileEnum child(this);
	while( child.next(true) )
	{
		nChildMask |= (child->m_nState & (STATE_FLAG_MALFUNCTION|STATE_FLAG_ACTIVE));
		if( child->m_nState & STATE_FLAG_MALFUNCTION )
			nChildError = child->m_lastErr;

		if( child->m_nState & STATE_FLAG_ACTIVE )
			bPaused &= !!(child->m_nState & TASK_REQUEST_PAUSE);
	}

	if( m_bTaskGroup && !m_cfg.m_bRemote )
		nChildMask |= (m_nState & STATE_FLAG_ACTIVE);

	if( !IsActive() )
	{
		if( (nChildMask & STATE_FLAG_ACTIVE) && !(nChildMask & STATE_FLAG_MALFUNCTION) )
			SetStateRequest(TASK_REQUEST_RUN, 0, REQUEST_FLAG_NON_RECURSIVE);
	}
	else if( !(nChildMask & STATE_FLAG_ACTIVE) )
	{
		if( nChildMask & STATE_FLAG_MALFUNCTION )
			SetStateRequest((tTaskRequestState)PROFILE_STATE_FAILED, 0, REQUEST_FLAG_NON_RECURSIVE);
		else
			SetStateRequest(TASK_REQUEST_STOP, 0, REQUEST_FLAG_NON_RECURSIVE);
	}
	else if( nChildMask & STATE_FLAG_MALFUNCTION )
	{
		if( m_nState == PROFILE_STATE_RUNNING )
			StateChanged(PROFILE_STATE_RUNNING_MALFUNCTION, nChildError);
	}
	else
	{
		if( bPaused )
			SetStateRequest(TASK_REQUEST_PAUSE, 0, REQUEST_FLAG_NON_RECURSIVE);
		else if( m_nState == PROFILE_STATE_RUNNING_MALFUNCTION )
			StateChanged(PROFILE_STATE_RUNNING);
	}
	return errOK;
}

tERROR cTmProfile::GetInfo(cSerializable* info, bool bByTaskId)
{
	if( !info )
		return errPARAMETER_INVALID;

	tERROR err = errOK; tDWORD i, c;
	if( info->isBasedOn(cTaskStatisticsList::eIID) )
	{
		cTaskStatisticsList& tree = *(cTaskStatisticsList*)info;
		for( i = 0, c = tree.m_aItems.count(); i < c; ++i )
		{
			cTmProfilePtr profile(m_tm, tree.m_aItems[i].m_sProfile.c_str(cCP_ANSI));
			if( profile )
				profile->GetStatistics((cTaskStatistics*&)tree.m_aItems[i].m_statistics.ptr_ref());
		}
	}
	else if( info->isBasedOn(cReportInfo::eIID) )
	{
		err = m_tm->m_report.GetInfo(cREPORT_FILTER_NONE, -2, *(cReportInfo*)info);
	}
	else if( info->isBasedOn(cProfileBase::eIID) )
	{
		err = GetInfo(*(cProfileBase*)info, bByTaskId);
	}
	else if( info->isBasedOn(cTaskStatistics::eIID) )
	{
		err = GetStatistics((cTaskStatistics*&)info);
	}
	else if( m_nSerIdSettings != cSerializable::eIID && info->isBasedOn(m_nSerIdSettings) )
	{
		err = GetSettings(csdFROM_DATA_TO_GUI, info, bByTaskId);
	}
	else if( info->isBasedOn(cTaskBLSettings::eIID) )
	{
		cSerObj<cTaskSettings> settings;
		err = GetSettings(csdFROM_DATA_TO_GUI, settings.ptr_ref(), bByTaskId);
		if( PR_SUCC(err) )
			if( settings->m_pTaskBLSettings )
				err = info->assign(*settings->m_pTaskBLSettings, false);
			else
				err = errNOT_FOUND;
	}
	else if( info->isBasedOn(cTaskInfo::eIID) )
	{
		cTaskInfo& dst = *(cTaskInfo*)info;
		dst.m_nTaskId  = id();
		dst.m_nState   = (enTaskState)m_nState;
		dst.m_sType    = m_sType;
		dst.m_sProfile = m_bClone && m_dad ? m_dad->m_sName : m_sName;
		dst.m_pid      = m_pid;
		dst.m_iid      = m_iid;
		dst.m_bService = isService();
		dst.m_nParentId = m_dad ? m_dad->id() : TASKID_UNK;
	}
	else if( info->isBasedOn(cTaskParams::eIID) )
	{
		cTaskParams& dst = *(cTaskParams*)info;
		dst.m_sType = m_sType;
		dst.m_sProfile = m_sName;
		dst.m_sDescription = m_sDescription;
		err = GetSettings(csdFROM_DATA_TO_GUI, dst.m_settings.ptr_ref(), bByTaskId);
	}
	else if( info->isBasedOn(cTaskSchedule::eIID) )
	{
		err = info->assign(schedule(), true);
	}
	else if( info->isBasedOn(cCfg::eIID) )
	{
		cCfg& cfg = *(cCfg*)info;
		if( cfg.m_nFlags & cCfg::fPolicy )
			err = m_tm->GetPolicy((cCfgEx*)&cfg);
		else if( cfg.m_nFlags & cCfg::fApplyPolicy )
			err = m_tm->ApplyPolicy((cCfgEx*)&cfg, this);
		else
			err = GetCfg(cfg, bByTaskId);
	}
	else if( !m_dad && m_tm && m_tm->m_bl )
	{
		err = m_tm->m_bl->GetStatistics(NULL, info);
	}
	else
		err = errNOT_SUPPORTED;

	return err;
}

tERROR cTmProfile::GetInfo(cProfileBase& info, bool bByTaskId)
{
	info = (cProfileBase&)(*this);

	if( !info.isBasedOn(cProfile::eIID) )
		return errOK;

	cProfile& profile = (cProfile&)info;
	cProfileEx& profileEx = (cProfileEx&)info;
	bool bProfileEx = info.isBasedOn(cProfileEx::eIID);

	tDWORD flags = profile.flags();

	profile.m_sCategory        = m_sCategory;
	profile.m_nSerIdSettings   = m_nSerIdSettings;
	profile.m_nSerIdStatistics = m_nSerIdStatistics;
	profile.m_bService         = m_bService;
	profile.m_bSingleton       = m_bSingleton;
	profile.m_bEventsDeleted   = m_bEventsDeleted;
	profile.m_tmCreationDate   = m_tmCreationDate;
	profile.m_tmLastModified   = m_tmLastModified;
	profile.m_tmLastCompleted  = m_tmLastCompleted;

	GetCfg(profile.m_cfg, bByTaskId);

	if( flags & cProfile::fStatistics )
		GetStatistics((cTaskStatistics*&)profile.m_statistics.ptr_ref());

	if( bProfileEx && (flags & cProfile::fReports) )
	{
		cAutoCS locker(m_lock, true);
		profileEx.m_aReports = m_aReports;
	}

	if( !bProfileEx )
		return errOK;

	cTmProfileEnum child(this);
	while( child.next() )
	{
		if( child->m_bClone )
			continue;

		cProfileExPtr& child_ex = profileEx.m_aChildren.push_back();
		child_ex.init(cProfileEx::eIID);
		child_ex->flags(flags);
		child->GetInfo(*child_ex, bByTaskId);
	}
	return errOK;
}

tERROR cTmProfile::GetStatistics(cTaskStatistics*& stat)
{
	tERROR err;
	if( !stat && PR_FAIL(err = g_root->CreateSerializable(m_nSerIdStatistics, (cSerializable**)&stat)) )
		return err;

	err = errUNEXPECTED;
	if( !m_bFromReport )
		err = GetTaskStatistics(stat);

	if( PR_FAIL(err) && m_statistics )
	{
		cAutoCS locker(m_lock, true);
		err = stat->assign(*m_statistics, false);
	}

	if( IS_TIME_EMPTY(stat->m_timeStart) )
		stat->m_timeStart = m_timeStart;

	if( IS_TIME_EMPTY(stat->m_timeFinish) )
		stat->m_timeFinish = m_timeFinish;

	return err;
}

tERROR cTmProfile::GetSettings(tCustomizeSettingsDirection direction, cSerializable*& settings, bool bByTaskId)
{
	if( m_nSerIdSettings == cSerializable::eIID )
		return errOK;

	if( direction != csdFROM_DATA_TO_TASK && (bByTaskId || persistent()) )
		if( PR_SUCC(GetTaskSettings(settings)) )
			return errOK;

	if( m_bClone )
		return m_dad->GetSettings(csdFROM_DATA_TO_GUI, settings, bByTaskId);

	tERROR err = errOK;

	if( bByTaskId && !m_bFromReport && !IsActive() )
	{
		cTmProfilePtr pFromReport = m_tm->m_report.FindProfile(m_nRuntimeId);
		if( pFromReport )
			return pFromReport->GetSettings(direction, settings, bByTaskId);
	}

	if( !settings && PR_FAIL(err = g_root->CreateSerializable(m_nSerIdSettings, &settings)) )
		return err;

	cSerializable* locked_settings = NULL;
	if( PR_FAIL(err = LockSettings(&locked_settings)) )
		return err;

	if( locked_settings != settings )
		err = settings->assign(*locked_settings, true);

	UnlockSettings(locked_settings, cFALSE);
	return err;
}

tERROR cTmProfile::GetCfg(cCfg& cfg, bool bByTaskId)
{
	if( bByTaskId && !m_bFromReport && !IsActive() )
	{
		cTmProfilePtr pFromReport = m_tm->m_report.FindProfile(m_nRuntimeId);
		if( pFromReport )
			return pFromReport->GetCfg(cfg, bByTaskId);
	}

	tDWORD flags = cfg.m_nFlags;

	(cPolicySettings&)cfg = (cPolicySettings&)m_cfg;

	cfg.m_bEnabled    = m_cfg.m_bEnabled;
	cfg.m_dwLevel     = m_cfg.m_dwLevel;
	cfg.m_bRemote     = m_cfg.m_bRemote;
	cfg.m_dwInstances = m_cfg.m_dwInstances;

	if( flags & cCfg::fSchedule )
	{
		cfg.m_schedule = m_cfg.m_schedule;
		if( bByTaskId && m_client )
			cfg.m_schedule.m_eMode = schmManual;
	}

	if( flags & cCfg::fRunAs )
		cfg.m_runas = m_cfg.m_runas;

	if( flags & cCfg::fSleepMode )
		cfg.m_smode = m_cfg.m_smode;

	if( flags & cCfg::fSettings )
		GetSettings(csdFROM_DATA_TO_GUI, cfg.m_settings.ptr_ref(), bByTaskId);

	if( flags & cCfg::fWithoutPolicy )
		LoadCfg(&cfg, false);

	return errOK;
}

tERROR cTmProfile::SetInfo()
{
	cSerializable* info = m_request->m_data;

	tERROR err = errPARAMETER_INVALID;
	if( info->isBasedOn(cCfg::eIID) )
		err = SetCfg(*(cCfg*)info);
	
	else if( info->isBasedOn(m_nSerIdSettings) )
		err = SetSettings(csdFROM_GUI_TO_DATA, info, !(m_request->m_flags & REQUEST_FLAG_FOR_TASK_ONLY));
	
	else if( info->isBasedOn(cProfile::eIID) )
	{
		cProfile* profile = (cProfile*)info;

		if( m_eOrigin != cProfile::poSystem && m_sDescription != profile->m_sDescription )
		{
			m_sDescription = profile->m_sDescription;
			m_changedFlags |= dfDescription;
		}

		err = SetCfg(profile->m_cfg);
	}
	
	else if( info->isBasedOn(cTaskParams::eIID) )
	{
		cTaskParams& src = *(cTaskParams*)info;
		// Probably may be do something else..............!!!!!!!!
		err = SetSettings(csdFROM_GUI_TO_DATA, src.m_settings.ptr_ref(), false);
	}

	ApplyChanges();

	if( m_request->m_flags & REQUEST_FLAG_FLUSH_DATA )
		SaveChanges();

	return err;
}

tERROR cTmProfile::Enable(tBOOL p_enabled, tDWORD flags)
{
	if( !IsPersistent() )
		return errOK;

	cAutoCS locker(m_lock, true);
	bool bChanged = enabled() != p_enabled;
	if( bChanged )
	{
		if( !(flags & ENABLE_FLAG_BYCFG) && IsEnableLocked() )
		{
			PR_TRACE((m_tm, prtERROR, "tm\tTask %S cannot be enabled by policy", m_sName.data() ));
			return errTASK_DISABLED;
		}

		m_changedFlags |= dfEnable;
		m_cfg.m_bEnabled = p_enabled;
	}
	locker.unlock();

	tDWORD child_flags = 0;
	if( !m_dad )
		child_flags |= (flags & ENABLE_FLAG_RECURSIVE);

	if( !(flags & ENABLE_FLAG_BYCFG) )
	{
		tERROR err = errOK;
		if( p_enabled )
		{
			if( !child_flags && IsChildsDisabled() )
				child_flags = ENABLE_FLAG_NOSYNC;

			if( m_dad && !m_dad->enabled() )
				err = m_dad->Enable(cTRUE, ENABLE_FLAG_NOSYNC);
		}
		else
		{
			if( m_dad && m_dad->IsChildsDisabled() )
				err = m_dad->Enable(cFALSE, ENABLE_FLAG_NOSYNC);
		}
		if( PR_FAIL(err) )
			return err;
	}

	if( child_flags )
	{
		cTmProfileEnum child(this);
		while( child.next(true) )
			child->Enable(p_enabled, child_flags);
	}

	if( !(flags & ENABLE_FLAG_NOSYNC) && (bChanged || !(flags & ENABLE_FLAG_BYCFG)) )
	{
		if( !p_enabled )
			SetStateRequest(TASK_REQUEST_STOP);
		else if( !(flags & ENABLE_FLAG_BYCFG) || !IsPaused() )
		{
			SetStateRequest(TASK_REQUEST_RUN, 0, (flags & ENABLE_FLAG_BYCFG) ? 0 : REQUEST_FLAG_FORCE_RUN);
			if( (flags & ENABLE_FLAG_BYCFG) && m_dad && m_dad->IsPaused() )
				SetStateRequest(TASK_REQUEST_PAUSE, 0, REQUEST_FLAG_NON_RECURSIVE);
		}
	}

	if( !(flags & ENABLE_FLAG_BYCFG) )
		ApplyChanges();

	return errOK;
}

tERROR cTmProfile::SetCfg(cCfg& cfg)
{
	cAutoCS locker(m_lock, true);

	if( cfg.m_nFlags & cCfg::fWithoutPolicy )
		m_tm->ApplyPolicy(&cfg, this);

	tBOOL bMakePersistent = cFALSE;
	if( m_tm->m_bl )
		m_tm->m_bl->CustomizeSettings(m_sName.c_str(cCP_ANSI), &cfg, cfg.level(),
			csdFROM_GUI_TO_DATA, &bMakePersistent);

	tBOOL bChanged = (m_cfg != cfg);
	if( bChanged )
	{
		m_changedFlags |= dfCfg;

		if( m_request->m_flags & REQUEST_FLAG_APPLY_POLICY )
		{
			m_cfg.m_dwMandatoriedFields = cfg.m_dwMandatoriedFields;
			m_cfg.m_dwLockedFields = cfg.m_dwLockedFields;
		}


		PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S SetCfg(old_level=%d, new_level=%d)",
			m_sName.data(), m_cfg.m_dwLevel, cfg.m_dwLevel));

		if( m_cfg.m_dwLevel != cfg.m_dwLevel )
			m_cfg.m_dwLevel = cfg.m_dwLevel;
		else
			m_changedFlags &= ~dfLevel;

		if( m_cfg.m_dwInstances != cfg.m_dwInstances )
		{
			m_cfg.m_dwInstances = cfg.m_dwInstances;
			InitProfileInstances();
		}
		else
			m_changedFlags &= ~dfInstances;

		if( m_cfg.m_schedule != cfg.m_schedule )
			InitSchedule(&cfg.m_schedule);

		if( m_cfg.m_runas != cfg.m_runas )
		{
			m_cfg.m_runas = cfg.m_runas;
			m_changedFlags |= dfRunAs;
		}

		if( m_cfg.m_smode != cfg.m_smode )
		{
			m_cfg.m_smode = cfg.m_smode;
			m_changedFlags |= dfSleepMode;
		}

		if( !(m_cfg.m_aAKUnsupportedFields == cfg.m_aAKUnsupportedFields) )
		{
			m_cfg.m_aAKUnsupportedFields = cfg.m_aAKUnsupportedFields;
			m_changedFlags |= dfAKUnsupportedFields;
		}
	}
	locker.unlock();

	if( bChanged || !m_dad )
		Enable(cfg.enabled(), ENABLE_FLAG_BYCFG);

	if( cfg.m_settings )
		SetSettings(csdFROM_GUI_TO_DATA, cfg.m_settings);
	return errOK;
}

tERROR cTmProfile::SetSettings(tCustomizeSettingsDirection direction, cSerializable* info, bool bStore)
{
	tERROR err = errOK;
	if( direction == csdFROM_TASK_TO_DATA )
	{
		cAutoCS locker(m_lock, true);
		m_stateFlags |= (dfTaskSettings|dfSendModified);
		PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S settings changed", m_sName.data()));
		return err;
	}

	if( m_bClone )
		return err;

	tBOOL bModified = cTRUE;
	if( bStore )
	{
		cSerializable* locked_settings = NULL;
		if( PR_FAIL(err = LockSettings(&locked_settings)) )
			return err;

		bModified = (m_changedFlags & dfLevel) ||
					(m_stateFlags & dfTaskSettings) ||
					(m_request->m_flags & REQUEST_FLAG_APPLY_POLICY) ||
					!info->isEqual(locked_settings, 0);

		if( bModified )
		{
			if( m_tm->m_bl )
			{
				PR_TRACE((m_tm, prtIMPORTANT, "tm\tTask %S SetSettings::CustomizeSettings(level=%d)",
					m_sName.data(), m_cfg.level() ));
				
				tCustomizeSettingsDirection dir = csdFROM_GUI_TO_DATA;
				if( m_request->m_flags & REQUEST_FLAG_FROM_TASK )
					dir = csdFROM_TASK_TO_DATA;

				tBOOL bMakePersistent = cFALSE;
				m_tm->m_bl->CustomizeSettings(m_sName.c_str(cCP_ANSI), info, m_cfg.level(),
					dir, &bMakePersistent);
			}

			locked_settings->assign(*info, false);
			m_tmLastModified = cDateTime::now_utc();
		}

		UnlockSettings(locked_settings, bModified, cFALSE);
	}

	return bModified ? SetTaskSettings(info) : errOK;
}

tERROR cTmProfile::LockSettings(cSerializable** p_settings)
{
	if( m_lock )
		m_lock->Enter(SHARE_LEVEL_WRITE);

	tERROR err = errOK;
	if( !(m_stateFlags & dfSettingsLoaded) )
	{
		m_cfg.m_settings.init(m_nSerIdSettings);
		if( PR_SUCC(err = LoadSettings(m_cfg.settings(), m_cfg.level())) )
			m_stateFlags |= dfSettingsLoaded;
	}

	if( !m_cfg.settings() )
		err = errUNEXPECTED;

	if( PR_SUCC(err) )
		*p_settings = m_cfg.settings();
	else if( m_lock )
		m_lock->Leave(NULL);

	return err;
}

tERROR cTmProfile::UnlockSettings(cSerializable* p_settings, tBOOL p_modified, tBOOL p_apply)
{
	if( p_modified )
	{
		m_changedFlags |= dfSettings;
		m_changedFlags &= ~(dfTaskSettings|dfSendModified);
	}

	if( m_lock )
		m_lock->Leave(NULL);

	if( p_modified && p_apply )
		ApplyChanges();
	return errOK;
}

tERROR cTmProfile::ScheduleRequest(tDWORD delay, tDWORD& flags, tTaskRequestState state, cSerializable* info)
{
	if( m_dwReleaseTimeout && !(flags & REQUEST_FLAG_BY_SCHEDULE) &&
			(flags & REQUEST_FLAG_UNLOCK_TASK) && !m_tm->m_isClosing )
		delay = m_dwReleaseTimeout;

	if( !delay )
		return errDO_IT_YOURSELF;

	if( delay == cREQUEST_SYNCHRONOUS )
	{
		flags |= REQUEST_FLAG_WAIT;
		return errDO_IT_YOURSELF;
	}

	if( delay == cREQUEST_SYNCSTORE_DATA )
	{
		flags |= REQUEST_FLAG_WAIT|REQUEST_FLAG_FLUSH_DATA;
		return errDO_IT_YOURSELF;
	}

	if( delay & cREQUEST_DONOT_FORCE_RUN )
	{
		flags &= ~REQUEST_FLAG_FORCE_RUN;
		return errDO_IT_YOURSELF;
	}

	if( delay & cREQUEST_RESTART_PREVIOUS_TASK )
	{
		flags |= REQUEST_FLAG_RESTART_TASK;
		return errDO_IT_YOURSELF;
	}

	if( !m_tm->m_scheduler )
		return errOBJECT_BAD_INTERNAL_STATE;

	cTaskSchedule sch;
	sch.m_eMode = schmEnabled;
	sch.m_eType = schtExactTime;
	sch.m_nFirstRunTime = cDateTime::now_local();
	sch.m_nFirstRunTime += delay & 0x00FFFFFF;
	sch.m_bRaiseIfSkipped = cTRUE;

	if( delay & cREQUEST_DELAY_MAKE_PERSISTENT )
		return InitSchedule(&sch, true);

	cScheduleParams params(*this, state, info);

	if( delay & cREQUEST_DELAY_UP_TO_INTERNET_CONNECTION )
		params.m_bWaitForInet = cTRUE;

	params.m_nFlags = (flags & REQUEST_FLAG_UNLOCK_TASK);

	tDWORD schedule_id = iCountCRC32str(m_sName.c_str(cCP_ANSI)) ^ PrGetTickCount();
	m_tm->m_scheduler->SetSchedule(pmc_SCHEDULER, schedule_id, &sch, &params);
	return errOK;
}

tERROR cTmProfile::PutRequest(cTmProfileRequest* request)
{
	if( m_tm->m_isClosed )
		return errOK;

	if( !m_tm->is_inited() )
		return ProcessRequest(request);
//		return errUNEXPECTED;

	tERROR err = errOK;

	cAutoCS locker(m_lock, true);
	m_queue.push_back(request);

	cEvent* waiter = request->m_waiter;
	if( waiter )
		request->m_pResult = &err;

	if( m_bQueueEmpty )
	{
		AddRef();
		m_bQueueEmpty = false;
		cThreadTaskBase::start(*m_tm);
	}

	locker.unlock();

	if( waiter )
	{
		waiter->Wait(cSYNC_INFINITE);
		waiter->sysCloseObject();
	}
	return err;
}

cTmProfileRequest* cTmProfile::GetRequest()
{
	cTmProfileRequest* request = NULL;

	cAutoCS locker(m_lock, true);
	m_bQueueEmpty = !m_queue.size();
	if( !m_bQueueEmpty )
	{
		request = m_queue.at(0);
		m_queue.remove(0);
	}
	return request;
}

tERROR cTmProfile::ProcessRequest(cTmProfileRequest* request)
{
	m_request = request;

	tERROR err = errOK;
	switch(request->m_request)
	{
	case PROFILE_REQUEST_SETSTATE:  err = SetState(); break;
	case PROFILE_REQUEST_SETINFO:   err = SetInfo(); break;
	case PROFILE_REQUEST_DELETE:    err = Delete(); break;
	}

	tBOOL dummy = cFALSE;
	request->wait_for_childs(dummy);
	request->signal(err);
	delete request;

	return err;
}

void cTmProfile::do_work()
{
	cTmProfileRequest* request;

	while(request = GetRequest())
		ProcessRequest(request);
}

// ------------------------------------------------

cTmProfileRequest::cTmProfileRequest(cTmProfileRequest* parent, tDWORD flags, tTaskRequestState requestState, hOBJECT client, tDWORD* taskId) :
	m_request(PROFILE_REQUEST_SETSTATE), m_requestState(requestState), m_client(client), m_taskId(taskId)
{
	init(parent, flags);
}

cTmProfileRequest::cTmProfileRequest(cTmProfileRequest* parent, tDWORD flags, cSerializable* data, tDWORD serid) :
	m_request(PROFILE_REQUEST_SETINFO), m_client(NULL), m_taskId(NULL)
{
	init(parent, flags);

	if( data )
	{
		if( !serid )
			m_data.assign(data);
		else if( PR_SUCC(m_data.init(serid)) )
			data->copy(*m_data, false);
	}
}

cTmProfileRequest::cTmProfileRequest(enProfileRequest request) :
	m_request(request)
{
	init(NULL, 0);
}

cTmProfileRequest::~cTmProfileRequest()
{
	set_inited();

	if( m_token )
		m_token->sysCloseObject();

	if( m_pParent )
		PrInterlockedDecrement(&m_pParent->m_nChilds);
}

void cTmProfileRequest::signal(tERROR err)
{
	if( m_pResult )
		*m_pResult = err, m_pResult = NULL;

	if( m_waiter )
		m_waiter->SetState(cTRUE), m_waiter = NULL;
}

void cTmProfileRequest::set_inited()
{
	if( !m_bInited )
	{
		m_bInited = cTRUE;
		if( m_pParent )
			PrInterlockedDecrement(&m_pParent->m_nChildsInited);
	}
}

void cTmProfileRequest::wait_for_childs(tBOOL& flag)
{
	set_inited();

	while( m_nChilds || flag )
		PrSleep(50);
}

void cTmProfileRequest::wait_for_childs_inited()
{
	set_inited();

	while( m_nChildsInited )
		PrSleep(50);
}

void cTmProfileRequest::wait_for_parent_inited()
{
	if( m_pParent )
		while( !m_pParent->m_bInited )
			PrSleep(50);
}

void cTmProfileRequest::init(cTmProfileRequest* parent, tDWORD flags)
{
	m_flags = flags;
	m_waiter = NULL;
	m_pParent = parent;
	m_pResult = NULL;
	m_token = NULL;
	m_nChilds = 0;
	m_bInited = cFALSE;
	m_nChildsInited = 0;

	if( m_pParent )
	{
		PrInterlockedIncrement(&m_pParent->m_nChilds);
		PrInterlockedIncrement(&m_pParent->m_nChildsInited);
	}

	if( (m_flags & (REQUEST_FLAG_WAIT|REQUEST_FLAG_LOCK_TASK)) || m_taskId )
	{
		if (PR_SUCC(g_root->sysCreateObject((cObj**)&m_waiter, IID_EVENT)))
		{
			if (PR_FAIL(m_waiter->set_pgMANUAL_RESET(cTRUE))
				|| PR_FAIL(m_waiter->sysCreateObjectDone()))
			{
				PR_TRACE((g_root, prtFATAL, "tm\tFailed to initialize an event object"));
				m_waiter->sysCloseObject();
				m_waiter = NULL;
			}
		}
		else
		{
			PR_TRACE((g_root, prtFATAL, "tm\tFailed to create an event object"));
			m_waiter = NULL;
		}
	}
}

// ------------------------------------------------

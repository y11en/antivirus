// ------------------------------------------------
#include <prague.h>
#include "tm.h"

// ------------------------------------------------

tERROR cTmReport::Init(TMImpl* tm, tDWORD* taskId)
{
	tERROR err = errOK;
	if( PR_FAIL(err = tm->OpenReport(REPORT_MAIN_FILE, *tm, fACCESS_RW, &m_report)) )
		return err;

	m_tm = tm;
	m_nMaxId = 0;

	tINT i, count = 0;
	err = m_report->GetRecordCount((tDWORD*)&count);
	m_records.resize(count);

	cProfileEx profile;
	for(i = count-1; i >= 0; i--, profile.clear())
	{
		if( !PR_SUCC(GetRecord(i, &profile)) )
			continue;

		cRecord& record = m_records.at(i);
		if( profile.m_nState & STATE_FLAG_ACTIVE )
		{
			profile.m_nState = PROFILE_STATE_FAILED;
			profile.m_lastErr = errTASK_ABNORMAL_TERMINATION;
			UpdateRecord(i, &profile);

			// bf 19699
			// Чтобы событие о сбое задачи посылалось в АК один раз, выставим
			// для задачи ошибку  errTASK_ABNORMAL_TERMINATION_ONCE
			// коннектор его опубликует при подключении к avp.
			cTmProfilePtr find(tm, profile.m_sName.c_str(cCP_ANSI));
			if (find)
				find->m_lastErr = errTASK_ABNORMAL_TERMINATION_ONCE;
		}
		else
			InitRecord(record, profile);

		m_tmLastChange = record.m_tmLastChange = record.m_tmFinish;
		record.m_bHistory = cTRUE;

		if( m_nMaxId < record.m_taskId )
			m_nMaxId = record.m_taskId;
	}

	m_tm->sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);	

	*taskId = m_nMaxId + 1;
	return err;
}

tERROR cTmReport::Clear()
{
	tINT i, count = m_records.size();
	for(i = 0; i < i; i++)
	{
		cRecord& record = m_records.at(i);
		if( record.m_pProfile->getIID() == cProfileBase::eIID )
			delete record.m_pProfile;
	}

	close_object(m_report);
	close_object(m_lock);
	return errOK;
}

tERROR cTmReport::CleanUp(const tDATETIME* p_time_stamp)
{
	cAutoCS locker(m_lock, true);
	tINT i, c = m_records.size();
	
	cDateTime limit(p_time_stamp);
	for(i = c-1; i >= 0; --i)
		if( !p_time_stamp || m_records[i].m_tmLastChange <= (tDWORD)limit )
			DeleteRecordByPos(i);

	if( !p_time_stamp )
		CleanUp((cTmProfile*)NULL);

	m_tmLastUpdateSize = 0;
	return errOK;
}

tERROR cTmReport::CleanUp(cTmProfile* profile)
{
	if( !m_tm )
		return errOK;

	cEnvStrObj mask(cENVIRONMENT_REPORT);
	mask.check_last_slash();
	if( profile )
	{
		mask += L"*_";
		mask += profile->m_sName;
		mask += L"_*";
	}
	else
		mask += L"????_*";

	cAutoObj<cObjPtr> enumerator;
	tERROR err = m_tm->sysCreateObject(enumerator, IID_OBJPTR, PID_NATIVE_FIO);
	if( PR_SUCC(err) )
		err = mask.copy(enumerator, pgMASK);
	if( PR_SUCC(err) )
		err = enumerator->sysCreateObjectDone();

	if( PR_FAIL(err) )
		return err;

	tDWORD nRuntimeId;
	while( PR_SUCC(enumerator->Next()) )
	{
		if (enumerator->get_pgOBJECT_ATTRIBUTES() & fATTRIBUTE_DIRECTORY)
			continue;

		if( profile )
		{
			cStrObj strName; strName.assign(enumerator, pgOBJECT_NAME);
			sscanf((tCHAR*)strName.c_str(cCP_ANSI), "%04x", &nRuntimeId);

			if( profile->m_nRuntimeId == nRuntimeId )
				continue;
		}

		cAutoObj<cIO> io;
		if( PR_SUCC(enumerator->IOCreate(&io, NULL, fACCESS_READ, fOMODE_OPEN_IF_EXIST|fOMODE_SHARE_DENY_READ)) )
		{
			io->propSetDWord(pgOBJECT_DELETE_ON_CLOSE, cTRUE);

			if(profile)
			{
				cAutoCS locker(m_lock, true);
				SetEventsDeleted(nRuntimeId);
			}
		}
	}
	m_tmLastUpdateSize = 0;
	return errOK;
}

tERROR cTmReport::UpdateDataSize()
{
	if( !m_tm )
		return errOK;

	tDWORD tmNow = PrGetTickCount();
	if( m_tmLastUpdateSize + 5000 > tmNow )
		return errOK;

	m_tmLastUpdateSize = tmNow;
	m_nDataSize = 0;
	m_nDataFiles = 0;

	tDWORD i, c = m_records.size();
	for(i = 0; i < c; i++)
		m_records.at(i).m_nDataSize = 0;

	cEnvStrObj path(cENVIRONMENT_REPORT);
	path.check_last_slash();
	path += L"????_*";

	cAutoObj<cObjPtr> enumerator;
	tERROR err = m_tm->sysCreateObject(enumerator, IID_OBJPTR, PID_NATIVE_FIO);
	if( PR_SUCC(err) )
		err = path.copy(enumerator,pgMASK);
	if( PR_SUCC(err) )
		err = enumerator->sysCreateObjectDone();

	if( PR_FAIL(err) )
		return err;

	while( PR_SUCC(enumerator->Next()) )
	{
		if( enumerator->get_pgIS_FOLDER() )
			continue;

		cStrObj strName; strName.assign(enumerator, pgOBJECT_NAME);

		tDWORD nRuntimeId = 0;
		sscanf((tCHAR*)strName.c_str(cCP_ANSI), "%04x", &nRuntimeId);
		if( !nRuntimeId )
			continue;

		tQWORD size = enumerator->get_pgOBJECT_SIZE();

		m_nDataSize += size;
		m_nDataFiles ++;

		tDWORD pos = FindRecordPos(nRuntimeId);
		if( pos != -1 )
			m_records.at(pos).m_nDataSize += size;
	}

	m_tmLastChange = PrGetTickCount();
	return errOK;
}

tDWORD cTmReport::GetRecordPos(tDWORD flags, tLONG index)
{
	if( flags & (cREPORT_RECORD_NEXT|cREPORT_RECORD_PREV) )
		return FindRecordPos(index);

	if( !(flags & cREPORT_FILTER_RECENT) )
		return index < 0 ? m_records.size() : (tDWORD)index < m_records.size() ? index : -1;

	tDWORD i, pos;
	for(i = 0, pos = 0; i < m_records.size(); i++)
		if( !m_records.at(i).m_bHistory )
			if( (tDWORD)index == pos++ )
				return i;

	return index < 0 ? pos : -1;
}

tERROR cTmReport::GetInfo(tDWORD flags, tLONG index, cReportInfo& info)
{
	cAutoCS locker(m_lock, true);

	tLONG i, c = m_records.size();
	tLONG pos = GetRecordPos(flags, index);
	if( index < 0 )
	{
		UpdateDataSize();
		info.m_nReportCount = index == -1 ? pos : m_nDataFiles;
		info.m_nReportSize = m_nDataSize;
		info.m_tmLastChange = m_tmLastChange;
	}
	else
	{
		if( pos == -1 )
			return errPARAMETER_INVALID;

		cRecord& record = m_records.at(pos);
		if( !record.m_pProfile )
			return errUNEXPECTED;

		if( flags & cREPORT_RECORD_NEXT )
		{
			for(i = pos+1; i < c; i++)
			{
				cRecord& next = m_records.at(i);
				if( next.m_pProfile && next.m_pProfile->m_sName == record.m_pProfile->m_sName )
					return GetInfo(0, i, info);
			}
			return errNOT_FOUND;
		}
		else if( flags & cREPORT_RECORD_PREV )
		{
			for(i = pos-1; i >= 0; i--)
			{
				cRecord& prev = m_records.at(i);
				if( prev.m_pProfile && prev.m_pProfile->m_sName == record.m_pProfile->m_sName )
					return GetInfo(0, i, info);
			}
			return errNOT_FOUND;
		}
		else
		{
			(cProfileBase&)info = *record.m_pProfile;
			info.m_nReportCount = 1;
			info.m_nReportSize = record.m_nDataSize;
			info.m_nRuntimeId = record.m_taskId;
			info.m_nState = record.m_nState;
            info.m_lastErr = record.m_lastErr;
			info.m_tmStart = record.m_tmStart;
			info.m_tmFinish = record.m_tmFinish;
			info.m_tmLastChange = record.m_tmLastChange;
		}
	}
	return errOK;
}

tERROR cTmReport::UpdateRecord(cTmProfile* profile)
{
	if( !m_tm || profile->m_nRuntimeId == TASKID_UNK )
		return errUNEXPECTED;

	cAutoCS locker(m_lock, true);
	return UpdateRecord(m_records.find(profile->m_nRuntimeId), profile);
}

tERROR cTmReport::DeleteRecord(tDWORD p_task_id, tDWORD flags)
{
	cAutoCS locker(m_lock, true);
	return DeleteRecordByPos(m_records.find(p_task_id), flags);
}

cTmProfile* cTmReport::FindProfile(tDWORD p_task_id)
{
	cAutoCS locker(m_lock, true);

	tDWORD pos = FindRecordPos(p_task_id);
	if( pos == -1 )
		return NULL;

	cProfileEx data;
	if( PR_FAIL(GetRecord(pos, &data)) )
		return NULL;

	return FindProfile(&data, p_task_id);
}

// ------------------------------------------------

tERROR cTmReport::InitTaskIds(cDwordVector& vetTaskIds, cProfileEx* info)
{
	tINT i, c = info->m_aChildren.size();
	for(i = 0; i < c; i++)
	{
		cProfileEx* child = info->m_aChildren.at(i);
		if( !child )
			continue;

		if( child->m_nRuntimeId != TASKID_UNK )
		{
			vetTaskIds.push_back(child->m_nRuntimeId);
			if( m_nMaxId < child->m_nRuntimeId )
				m_nMaxId = child->m_nRuntimeId;
		}

		InitTaskIds(vetTaskIds, child);
	}
	return errOK;
}

tDWORD cTmReport::FindRecordPos(tDWORD p_task_id)
{
	tINT i, ic = m_records.size();
	for(i = 0; i < ic; i++)
	{
		cRecord& pRecord = m_records[i];
		if( pRecord.m_taskId == p_task_id )
			return i;

		tINT j, jc = pRecord.m_vetTaskIds.size();
		for(j = 0; j < jc; j++)
			if( pRecord.m_vetTaskIds[j] == p_task_id )
				return i;
	}
	return -1;
}

tERROR cTmReport::SetEventsDeleted(tDWORD p_task_id)
{
	tDWORD pos = FindRecordPos(p_task_id);
	if( pos == -1 )
		return errNOT_FOUND;

	cProfileEx to_update;
	tERROR error = GetRecord(pos, &to_update);

	if( PR_SUCC(error) )
	{
		to_update.m_bEventsDeleted = cTRUE;
		error = UpdateRecord(pos, &to_update);
	}
	return error;
}

cTmProfile* cTmReport::FindProfile(cProfileEx* info, tDWORD p_task_id)
{
	cTmProfile* profile = NULL;
	if( info->m_nRuntimeId == p_task_id )
	{
		profile = new cTmProfile();
		profile->InitFromReport(m_tm, *info);
		return profile;
	}

	tINT i, c = info->m_aChildren.size();
	for(i = 0; i < c; i++)
		if( profile = FindProfile(info->m_aChildren.at(i), p_task_id) )
			return profile;

	return profile;
}

tERROR cTmReport::GetRecord(tDWORD pos, cProfileEx* info)
{
	tChunckBuff buff; tDWORD size;
	tERROR err = m_report->GetRecord(&size, pos, (tPTR)buff.ptr(), buff.count(), NULL);
	if( err == errBUFFER_TOO_SMALL )
		err = m_report->GetRecord(NULL, pos, (tPTR)buff.get(size, false), size, NULL);

	if( PR_SUCC(err) )
		err = g_root->StreamDeserialize((cSerializable**)&info, (tBYTE*)buff.ptr(), size, NULL);
	return err;
}

tERROR cTmReport::InitRecord(cRecord& record, cProfileEx& profile)
{
	record.m_taskId = profile.m_nRuntimeId;
	record.m_nState = profile.m_nState;
    record.m_lastErr = profile.m_lastErr;
	record.m_vetTaskIds.clear();
	InitTaskIds(record.m_vetTaskIds, &profile);

	if( !record.m_pProfile )
	{
		if( !profile.isBasedOn(cTmProfile::eIID) )
		{
			cTmProfilePtr find(m_tm, profile.m_sName.c_str(cCP_ANSI));
			if( find )
			{
				record.m_pProfile = find;

				if( !profile.persistent() )
					find->InitFromReport(profile);
			}
		}
		else
			record.m_pProfile = &profile;

		if( !record.m_pProfile || profile.m_eOrigin != cProfileBase::poSystem )
			record.m_pProfile = new cProfileBase(profile);
	}

	if( profile.m_statistics && profile.m_statistics->isBasedOn(cTaskStatistics::eIID) )
	{
		cTaskStatistics* stat = (cTaskStatistics*)(cSerializable*)profile.m_statistics;
		record.m_tmStart = stat->m_timeStart;
		record.m_tmFinish = stat->m_timeFinish;
	}
	return errOK;
}

tERROR cTmReport::UpdateRecord(tDWORD pos, cProfileEx* info)
{
	tChunckBuff buff; tDWORD size;
	tERROR err = g_root->StreamSerialize(info, cProfileEx::eIID, (tBYTE*)buff.ptr(), buff.count(), &size);
	if( err == errBUFFER_TOO_SMALL )
		err = g_root->StreamSerialize(info, cProfileEx::eIID, (tBYTE*)buff.get(size, false), size, &size);

	if( PR_FAIL(err) )
		return err;

	if( pos == -1 )
		err = m_report->AddRecord(buff.ptr(), size, NULL);
	else
		err = m_report->UpdateRecord(pos, buff.ptr(), size);

	if( PR_SUCC(err) )
	{
		cRecord& record = (pos == -1 ? m_records.push_back() : m_records[pos]);
		m_tmLastChange = record.m_tmLastChange = (tDWORD)cDateTime::now_utc();
		err = InitRecord(record, *info);
	}
	return err;
}

tERROR cTmReport::DeleteRecordByPos(tDWORD pos, tDWORD flags)
{
	if( !m_tm )
		return errOK;

	if( pos == -1 )
		return errNOT_FOUND;

	cRecord& record = m_records[pos];
	if( record.m_nState & STATE_FLAG_ACTIVE )
		return errOBJECT_IS_LOCKED;

	cAutoObj<cOS> deleter;
	tERROR err = m_tm->sysCreateObjectQuick(deleter, IID_OS, PID_NATIVE_FIO);

	cProfileEx data;
	if( PR_SUCC(err) )
		GetRecord(pos, &data);

	tDWORD i, c = data.m_aReports.size();
	for(i = 0; i < c && PR_SUCC(err); ++i)
	{
		cTaskReportDsc& dsc = data.m_aReports[i];
		cEnvStrObj path(cENVIRONMENT_REPORT);
		path.check_last_slash();
		path += dsc.m_name;
		err = deleter->Delete(cAutoString(path));
		if( err == errOBJECT_NOT_FOUND )
			err = errOK;
	}

	if( !(flags & cREPORT_DELETE_FILES_ONLY) )
	{
		if( PR_SUCC(err) )
			err = m_report->DeleteRecord(pos);

		if( PR_SUCC(err) )
		{
			cRecord& record = m_records[pos];
			if( record.m_pProfile && record.m_pProfile->getIID() == cProfileBase::eIID )
				delete record.m_pProfile;
			m_records.remove(pos);
		}
	}
	return err;
}

// ------------------------------------------------

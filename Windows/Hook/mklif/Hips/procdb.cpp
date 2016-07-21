#include <crtdefs.h>
#include "procdb.h"

cProcessDB::cProcessDB()
{
	memset(&m_pids_0, 0, sizeof(m_pids_0));
}

cProcessDB::~cProcessDB()
{
}

bool cProcessDB::Init(alFile* pProcessDBFile)
{
	if (!m_db.Init(pProcessDBFile, sizeof(RECORD)))
		return false;
	if (!m_db.GetRecordCount())
	{ 
		RECORD rec = {0};
		m_db.AddRecord(&rec, sizeof(rec), NULL);
	}
	return true;
}

tUniqPid cProcessDB::GetNextUniquePid()
{
	return (tUniqPid)m_db.GetRecordCount() | UNIQ_PROC_ID_FLAG;
}

bool cProcessDB::ConvertPid2Uniq(tNativePid native_pid, tUniqPid* puniq_pid)
{
	if (!puniq_pid)
		return false;
	if (!native_pid)
	{
		*puniq_pid = 0;
		return false;
	}
	tUniqPid uniq = GetUniq(native_pid);
	if (!uniq)
	{
		tNativePid parent_pid = 0;
		tUniqPid parent_uniq = 0;
		uint64 start_time = 0;
		GetProcessInfo(native_pid, &start_time, &parent_pid);
		ConvertPid2Uniq(parent_pid, &parent_uniq);
		if (start_time)
		{
			// search for this process in DB
			for (tUniqPid n=(tUniqPid)m_db.GetRecordCount()-1; n>0; n--)
			{
				RECORD rec;
				if (!m_db.GetRecord(n, &rec, sizeof(rec), NULL))
					break;
				if (0 == rec.start_time)
					continue;
				if (start_time - 1*(10^6) < rec.start_time)
					break;
				if (start_time + 1*(10^6) > rec.start_time)
					continue;
				if (native_pid != rec.native_pid)
					continue;
				uniq = n | UNIQ_PROC_ID_FLAG;
				break;
			}
		}
		if (!uniq)
		{
			// register new process
			if (!OnProcessStart(native_pid, parent_uniq, start_time, 0, &uniq))
				return false;
		}
	}
	*puniq_pid = uniq;
	return true;
}


bool cProcessDB::OnProcessStart(tNativePid native_pid, tUniqPid parent_pid, uint64 start_time, tEventId eventid, OUT tUniqPid* puniq)
{
	RECORD rec = {0};
	rec.native_pid = native_pid;
	rec.parent_uniq = parent_pid;
	rec.start_time = start_time;
	rec.start_event = eventid;
	tRECID uniq;
	if (!m_db.AddRecord(&rec, sizeof(rec), &uniq ))
		return false;
	uniq |= UNIQ_PROC_ID_FLAG;
	if (puniq)
		*puniq = uniq;
	SetUniq(native_pid, uniq);
	return true;
}

bool cProcessDB::OnProcessExit(tUniqPid uniq, uint64 exit_time, tEventId eventid)
{
	RECORD rec;
	if (!GetProcRecord(uniq, &rec))
		return false;
	if (exit_time)
		rec.exit_time = exit_time;
	if (eventid)
		rec.exit_event = eventid;
	return SetProcRecord(uniq, &rec);
}

bool cProcessDB::GetProcRecord(tUniqPid uniq, OUT RECORD* pRecord)
{
	if (!(uniq & UNIQ_PROC_ID_FLAG))
		return false;
	return m_db.GetRecord(uniq & ~UNIQ_PROC_ID_FLAG, pRecord, sizeof(RECORD), NULL);
}

bool cProcessDB::SetProcRecord(tUniqPid uniq, IN RECORD* pRecord)
{
	if (!(uniq & UNIQ_PROC_ID_FLAG))
		return false;
	return m_db.UpdateRecord(uniq & ~UNIQ_PROC_ID_FLAG, pRecord, sizeof(RECORD));
}

tUniqPid cProcessDB::GetUniq(tNativePid pid)
{
	if (pid < PROCS_PER_PAGE)
		return m_pids_0.uniq[pid];

	AUTO_LOCK_R();
	for (std::vector<UNIQ_PIDS>::iterator it=m_pids.begin(); it!=m_pids.end(); it++)
	{
		if (it->base == (pid & ~PROCS_PER_PAGE_MASK))
			return it->uniq[pid & PROCS_PER_PAGE_MASK];
	}
	return 0;
}

void cProcessDB::SetUniq(tNativePid pid, tUniqPid uniq)
{
	if (pid < PROCS_PER_PAGE)
	{
		m_pids_0.uniq[pid] = uniq;
		return;
	}

	AUTO_LOCK_W();
	for (std::vector<UNIQ_PIDS>::iterator it=m_pids.begin(); it!=m_pids.end(); it++)
	{
		if (it->base == (pid & ~PROCS_PER_PAGE_MASK))
		{
			it->uniq[pid & PROCS_PER_PAGE_MASK] = uniq;
			return;
		}
	}

	UNIQ_PIDS page = {0};
	page.base = (tUniqPid)pid & ~PROCS_PER_PAGE_MASK;
	page.uniq[pid & PROCS_PER_PAGE_MASK] = uniq;
	m_pids.push_back(page);
	return;
}

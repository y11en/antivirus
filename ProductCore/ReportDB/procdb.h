#ifndef __PROCDB_H_
#define __PROCDB_H_

#include "rdb_types.h"
#include "plaindb.h"
#include "lock.h"
#include <vector>

#define PROCS_PER_PAGE (0x1000/sizeof(tUniqPid))
#define PROCS_PER_PAGE_MASK (PROCS_PER_PAGE - 1)

#define UNIQ_PROC_ID_FLAG ((tUniqPid)1<<(sizeof(tUniqPid)*8-1))
#define IS_UNIQ_PID(pid) (!!(pid & UNIQ_PROC_ID_FLAG))

class cProcessDB 
{
public:

	typedef struct tag_RECORD
	{
		tNativePid native_pid;
		tUniqPid   parent_uniq;
		tEventId   start_event;
		uint64     start_time;
		tEventId   exit_event;
		uint64     exit_time;
	} RECORD;

public:

	cProcessDB();
	~cProcessDB();
	bool cProcessDB::Init(alFile* pProcessDBFile);

	virtual bool GetProcessInfo(tNativePid native_pid, uint64* pstart_time, tNativePid* pparent_pid) { return false; }
	
	tUniqPid GetNextUniquePid();
	bool ConvertPid2Uniq(tNativePid native_pid, tUniqPid* puniq_pid, tAppId* papp_id);
	bool OnProcessStart(tNativePid native_pid, tUniqPid parent_pid, uint64 start_time, tEventId eventid, OUT tUniqPid* puniq);
	bool OnProcessExit(tUniqPid uniq, uint64 exit_time, tEventId eventid);
	bool GetProcRecord(tUniqPid uniq, OUT RECORD* pRecord);
	bool SetProcRecord(tUniqPid uniq, IN RECORD* pRecord);

private:

	typedef struct tag_UNIQ_PIDS {
		tUniqPid base;
		tUniqPid uniq[PROCS_PER_PAGE];
	} UNIQ_PIDS;

	cPlainDB m_db;
	UNIQ_PIDS m_pids_0;
	std::vector<UNIQ_PIDS> m_pids;

	tUniqPid GetUniq(tNativePid pid);
	void SetUniq(tNativePid pid, tUniqPid uniq);
	cLock m_Lock;
};

#endif // __PROCDB_H_

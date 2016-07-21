#if !defined(__task_report_h)
#define __task_report_h

#include <prague.h>

struct cTmReport
{
public:
	cTmReport() : m_tm(NULL), m_report(NULL), m_lock(NULL),
		m_nDataSize(0), m_nDataFiles(0), m_tmLastChange(0), m_tmLastUpdateSize(0){}

	tERROR      Init(TMImpl* tm, tDWORD* taskId);
	tERROR      Clear();
	tERROR      CleanUp(const tDATETIME* p_time_stamp);
	tERROR      CleanUp(cTmProfile* profile);
	tERROR      GetInfo(tDWORD flags, tLONG index, cReportInfo& info);
	tERROR      UpdateRecord(cTmProfile* profile);
	tERROR      DeleteRecord(tDWORD p_task_id, tDWORD flags);
	cTmProfile* FindProfile(tDWORD p_task_id);

private:
	struct cRecord
	{
		cRecord(tDWORD taskId = 0) : m_taskId(taskId), m_tmStart(-1), m_tmFinish(-1), 
			m_tmLastChange(-1), m_nDataSize(0), m_pProfile(NULL), m_bHistory(cFALSE){}

		bool operator == (const cRecord& c) const { return m_taskId == c.m_taskId; }

		tDWORD        m_taskId;
		tBOOL         m_bHistory;
		tDWORD        m_tmStart;
        tERROR        m_lastErr;
		tDWORD        m_tmFinish;
		tDWORD        m_tmLastChange;
		tQWORD        m_nDataSize;
		tProfileState m_nState;
		cProfileBase* m_pProfile;
		cDwordVector  m_vetTaskIds;
	};

	tERROR      InitTaskIds(cDwordVector& vetTaskIds, cProfileEx* info);
	tERROR      InitRecord(cRecord& record, cProfileEx& profile);
	tDWORD      FindRecordPos(tDWORD p_task_id);
	tDWORD      GetRecordPos(tDWORD flags, tLONG index);
	tERROR      SetEventsDeleted(tDWORD p_task_id);
	tERROR      UpdateDataSize();
	cTmProfile* FindProfile(cProfileEx* info, tDWORD p_task_id);

	tERROR      GetRecord(tDWORD pos, cProfileEx* info);
	tERROR      UpdateRecord(tDWORD pos, cProfileEx* info);
	tERROR      DeleteRecordByPos(tDWORD pos, tDWORD flags = 0);

protected:
	TMImpl*           m_tm;
	cReport*          m_report;
	cCriticalSection* m_lock;
	cVector<cRecord>  m_records;
	tQWORD            m_nDataSize;
	tDWORD            m_nDataFiles;
	tDWORD            m_tmLastChange;
	tDWORD            m_tmLastUpdateSize;
	tDWORD            m_nMaxId;
};

#endif // __task_report_h

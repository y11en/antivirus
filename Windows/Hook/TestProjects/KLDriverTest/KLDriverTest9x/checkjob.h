#ifndef __HOOK_JOBS
#define __HOOK_JOBS

#include "DrvEventList.h"
#include "TaskThread.h"

//+ ----------------------------------------------------------------------------------------

class CheckJob_File : public HookTask, public ThreadParam
{
private:
	DrvEventList* m_pDrvEventList;
	bool m_bAllow;
public:
	CheckJob_File(DrvEventList* pDrvEventList);
	~CheckJob_File();
	void SetAllowEvents(bool bAllow) {m_bAllow = bAllow;};

	bool Start();
	bool AddFilters();

	void SingleEvent();

	void UploadEvent();
};

#endif //__HOOK_JOBS
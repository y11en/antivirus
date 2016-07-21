#ifndef __HOOK_JOBS
#define __HOOK_JOBS

#include "TaskThread.h"

class Job_Process : public HookTask, public ThreadParam
{
public:
	Job_Process();
	~Job_Process();

	bool Start();
	bool AddFilters();

	
	void BeforeExit();
	void SingleEvent();
	void FiltersChanged();
};

#endif //__HOOK_JOBS
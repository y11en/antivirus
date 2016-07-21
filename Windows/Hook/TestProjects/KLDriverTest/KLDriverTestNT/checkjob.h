#ifndef __HOOK_JOBS
#define __HOOK_JOBS

#include "DrvEventList.h"
#include "TaskThread.h"

class CheckJob_System : public HookTask, public ThreadParam
{
private:
	DrvEventList* m_pDrvEventList;
public:
	CheckJob_System(DrvEventList* pDrvEventList);
	~CheckJob_System();

	bool Start();
	bool AddFilters();

	void SingleEvent();
};

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
};

//+ ----------------------------------------------------------------------------------------

class CheckJob_Reg : public HookTask, public ThreadParam
{
private:
	DrvEventList* m_pDrvEventList;
	bool m_bAllow;
public:
	CheckJob_Reg(DrvEventList* pDrvEventList);
	~CheckJob_Reg();
	void SetAllowEvents(bool bAllow) {m_bAllow = bAllow;};

	bool Start();
	bool AddFilters();

	void SingleEvent();
};

//+ ----------------------------------------------------------------------------------------

class CheckJob_Flt : public HookTask, public ThreadParam
{
private:
	HANDLE m_hEvent;
	DrvEventList* m_pDrvEventList;
	bool m_bAllow;
public:
	CheckJob_Flt(DrvEventList* pDrvEventList, HANDLE hEvent);
	~CheckJob_Flt();
	void SetAllowEvents(bool bAllow) {m_bAllow = bAllow;};

	bool Start();

	void SingleEvent();
	void FiltersChanged();
};

#endif //__HOOK_JOBS
#include "../eventmgr/include/envhelper.h"
#include "proclist.h"
#include "srv.h"
#include <assert.h>

#if defined(_WIN32) && (defined(_DEBUG) || defined(_RUNTIME_CHECKS_))
#define  _RUNTIME_CHECKS_IMPL_
#include <windows.h>
#endif

cProcessList::cProcessList(cEnvironmentHelper* pEnvironmentHelper) :
	m_pEnvironmentHelper(pEnvironmentHelper)
{
	m_next_slot_index = 0;
	m_next_uniq_pid = FLAG_UNIQ_PID | 1 ;
	memset(&m_Tid2Pid, 0, sizeof(m_Tid2Pid));
	memset(&m_Callers, 0, sizeof(m_Callers));
}

cProcessList::~cProcessList()
{
	clear();
}

bool cProcessList::ProcessCreate(tPid pid, tPid parent_pid, cFile& image_path, cPath& working_folder, tcstring cmd_line, uint64_t start_time)
{
	cProcessListItem* pProcess = NULL;
	tPid parent_uniq_pid = 0;
	bool bCreatedByParent = false;
	
	if (parent_pid && parent_pid > 8)
	{
		cProcess pParent = FindProcess(parent_pid);
		//assert(pParent);
		if (pParent)
		{
			parent_pid = pParent->m_pid;
			parent_uniq_pid = pParent->m_uniq_pid;
			if (pParent->m_CreatedImagesList.contain(image_path.getFull()))
				bCreatedByParent = true;
		}
	}

	cAutoLock auto_lock(m_Sync);
	ProcessTerminate(pid, start_time); // ensure no running process with the same pid

	pProcess = new cProcessListItem(m_pEnvironmentHelper, this, pid, parent_pid, image_path, working_folder, cmd_line);
	if (!pProcess)
		return false;

	pProcess->m_ftStartTime = start_time ? start_time : UINT64_MAX;
	pProcess->m_parent_uniq_pid = parent_uniq_pid;
	m_pEnvironmentHelper->ProcessAssignUniqPid(pProcess);
	if (!pProcess->m_uniq_pid)
		pProcess->m_uniq_pid = m_next_uniq_pid;
	if (!pProcess->m_parent_uniq_pid)
		pProcess->m_parent_uniq_pid = parent_uniq_pid;
	m_next_uniq_pid++;

	bool bInterpreteur = pProcess->GetFlag(pfInterpreter);
	if (bInterpreteur || bCreatedByParent)
		pProcess->m_CallerPid = parent_uniq_pid;
	if (pid && pid < MAX_PROCESS_ID)
	{
		if (bInterpreteur || bCreatedByParent)
		{
			m_Callers[pid/4].nCallerPid = parent_uniq_pid;
			m_Callers[pid/4].bInterpreteur = bInterpreteur;
		}
		else
		{
			memset(&m_Callers[pid/4], 0, sizeof(CALLER_INFO));
		}
	}

	m_ProcList.push_back(pProcess);

	return true;
}

bool cProcessList::ProcessTerminate(tPid pid, uint64_t exit_time)
{
	cProcess pProcess = FindProcess(pid);
	if (!pProcess)
		return false;
	pProcess->m_ftExitTime = exit_time ? exit_time : UINT64_MAX;
	return true;
}

bool cProcessList::ProcessDestroy(tPid pid)
{
	cAutoLock auto_lock(m_Sync);
	tProcList::iterator _it;
	for (_it = m_ProcList.begin(); _it != m_ProcList.end(); _it++)
	{
		cProcessListItem* pProcess = *_it;
		if (pProcess->m_pid == pid || pProcess->m_uniq_pid == pid)
		{
			pProcess->ReleaseRef();
			m_ProcList.erase(_it);
			return true;
		}
	}
	return false;
}

bool cProcessList::ThreadCreate(tPid pid, tTid tid, tPid dest_pid, tTid new_tid)
{
	if (pid == dest_pid) // remote thread
		return true;

	cProcess pProcess = FindProcess(dest_pid);
	if (!pProcess)
		return true;
	if (0 == pProcess->m_BaseThread)
	{
		pProcess->m_BaseThread = new_tid;
		return true;
	}
	pProcess.release();

	// CreateRemoteThread
	if (new_tid && new_tid < MAX_THREAD_ID)
	{
		pProcess = FindProcess(pid);
		if (pProcess)
			m_Tid2Pid[new_tid/4] = pProcess->m_uniq_pid;
	}
	return true;
}

bool cProcessList::ThreadTerminate(tPid pid, tTid tid, tPid dest_pid, tTid term_tid)
{
	if (tid < MAX_THREAD_ID)
		m_Tid2Pid[tid/4] = 0;
	return false;
}

bool cProcessList::GetRemoteThreadCreator(tTid tid, tPid* ppid)
{
	tPid caller_pid;
	// remote thread
	if (!tid)
		return false;
	if (tid < MAX_THREAD_ID)
	{
		caller_pid = m_Tid2Pid[tid/4];
		if (caller_pid)
		{
			if (ppid)
				*ppid = caller_pid;
			return true;
		}
	}

	return false;
}

bool cProcessList::GetCaller(tPid pid, tPid* ppid)
{
	tPid caller_pid;
	if (!pid)
		return false;
	if (pid < MAX_PROCESS_ID)
	{
		caller_pid = m_Callers[pid/4].nCallerPid;
	}
	else
	{
		cProcess pProcess = FindProcess(pid);
		if (!pProcess)
			return false;
		caller_pid = pProcess->m_CallerPid;
	}
	if (!caller_pid)
		return false;
	if (ppid)
		*ppid = caller_pid;
	return true;
}

bool cProcessList::IsInterpreteur(tPid pid)
{
	if (pid < MAX_PROCESS_ID)
		return m_Callers[pid/4].bInterpreteur;
	
	cProcess pProcess = FindProcess(pid);
	if (!pProcess)
		return false;
	return pProcess->GetFlag(pfInterpreter);
}

bool cProcessList::GetTopmostCaller(tPid pid, tTid tid, tPid* pCallerPid)
{
	tPid caller_pid = pid;
	bool result = GetRemoteThreadCreator(tid, &caller_pid);
	while (GetCaller(caller_pid, &caller_pid)) 
		result = true;
	if (result && pCallerPid)
		*pCallerPid = caller_pid;
	return result;
}

bool cProcessList::DoDelayedProcessesExit(uint64_t current_time, uint64_t delay_time)
{
	bool removed;
	cAutoLock auto_lock(m_Sync);
	do {
		removed = false;
		for (tProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); _it++)
		{
			cProcessListItem* pProcess = *_it;
			if (0 == pProcess->m_ftExitTime)
				continue;
			if (UINT64_MAX == pProcess->m_ftExitTime)
				continue;
			if (pProcess->m_ftExitTime + delay_time > current_time)
				continue;
			pProcess->ReleaseRef();
			m_ProcList.erase(_it);
			removed = true;
			break;
		}
	} while( removed );
	return true;
}

bool cProcessList::clear()
{
	cAutoLock auto_lock(m_Sync);
	for (tProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); _it++)
		(*_it)->ReleaseRef();
	m_ProcList.clear();
	return true;
}

size_t cProcessList::count()
{
	cAutoLock auto_lock(m_Sync, false);
	return m_ProcList.size();
}

cProcessListItem* cProcessList::iFindProcess(tPid pid, bool bFindExited /*= false*/)
{
	cProcessListItem* pRetProcess = NULL;

	m_Sync.lock(false);
	for (tProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); _it++)
	{
		cProcessListItem* pProcess = *_it;
		if (pProcess->m_uniq_pid == pid)
		{
			pRetProcess = pProcess;
			break;
		}
		if (pProcess->m_pid == pid)
		{
			if (bFindExited || 0 == pProcess->m_ftExitTime)
			{
				pRetProcess = pProcess;
				break;
			}
		}
	}
	if (pRetProcess)
		pRetProcess->AddRef();
	m_Sync.unlock();

	if (pRetProcess)
		pRetProcess->Lock();

	return pRetProcess;
}

cProcessListItem* _iFindProcess(cEventHandler* pHandler, tPid pid)
{
	return pHandler->m_pProcessList->iFindProcess(pid);
}

cProcess cProcessList::FindProcess(tPid pid, bool bFindExited /*= false*/)
{
	return iFindProcess(pid, bFindExited);
}

cProcess cProcessList::GetProcess(size_t index)
{
	cProcessListItem* pProcess = NULL;
	{
		cAutoLock auto_lock(m_Sync, false);
		if (index >= m_ProcList.size())
			return NULL;
		pProcess = m_ProcList[index];
		pProcess->AddRef();
	}
	pProcess->Lock();
	return pProcess;
}

bool cProcessList::GetSnapshot(tPid** pPidArray, size_t* pPidCount)
{
	if (!pPidArray || !pPidCount)
		return false;
	cAutoLock auto_lock(m_Sync, false);
	size_t nPidCount = m_ProcList.size();
	*pPidArray = 0;
	*pPidCount = nPidCount;
	if (!nPidCount)
		return true;
	*pPidArray = (tPid*)m_pEnvironmentHelper->Alloc(nPidCount * sizeof(tPid));
	if (NULL == *pPidArray)
	{
		*pPidCount = 0;
		return false;
	}

	nPidCount=0;
	for (tProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); _it++, nPidCount++)
		(*pPidArray)[nPidCount] = (*_it)->m_uniq_pid;

	return true;
}

bool cProcessList::GetChildSnapshot(tPid parent_pid, tPid** pChildPidArray, size_t* pnChildPidCount)
{
	if (!pChildPidArray || !pnChildPidCount)
		return false;
	cAutoLock auto_lock(m_Sync, false);
	size_t nChildPidCount = 0;
	tProcList::iterator _it;

	nChildPidCount = 0;
	for (_it = m_ProcList.begin(); _it != m_ProcList.end(); _it++)
	{
		if ((*_it)->m_parent_uniq_pid == parent_pid || (*_it)->m_parent_pid == parent_pid)
			nChildPidCount++;
	}

	*pChildPidArray = 0;
	*pnChildPidCount = nChildPidCount;
	if (!nChildPidCount)
		return true;
	*pChildPidArray = (tPid*)m_pEnvironmentHelper->Alloc(nChildPidCount * sizeof(tPid));
	if (NULL == *pChildPidArray)
	{
		*pnChildPidCount = 0;
		return false;
	}

	nChildPidCount = 0;
	for (_it = m_ProcList.begin(); _it != m_ProcList.end(); _it++)
	{
		if ((*_it)->m_parent_uniq_pid == parent_pid || (*_it)->m_parent_pid == parent_pid)
		{
			(*pChildPidArray)[nChildPidCount] = (*_it)->m_uniq_pid;
			nChildPidCount++;
		}
	}

	return true;
}

void cProcessList::ReleaseSnapshot(tPid** pPidArray)
{
	if (!pPidArray)
		return;
	if (!*pPidArray)
		return;
	m_pEnvironmentHelper->Free(*pPidArray);
	*pPidArray = 0;
	return;
}

tPid cProcessList::iFindLockedProcess(uint32_t tid)
{
	cProcessListItem* pProcess = NULL;
	{
		cAutoLock auto_lock(m_Sync, false);
		for (tProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); _it++)
		{
			if ((*_it)->m_lock_tid == tid)
				return (*_it)->m_uniq_pid;
		}
	}
	return 0;
}

bool cProcessList::AllocSlot(size_t *slot_index)
{
	if (!slot_index)
		return false;
	*slot_index = m_next_slot_index++;
	return true;
}

//cProcess cProcessList::operator[](size_t index)
//{
//	return at(index);
//}

// ===================================================================================

cProcessListItem::cProcessListItem(cEnvironmentHelper* pEnvironmentHelper, cProcessList* pProcessList, tPid pid, tPid parent_pid, cFile& image_path, cPath& working_folder, tcstring cmd_line) : 
	m_image(image_path), 
	m_orig_image(image_path),
	m_working_folder(working_folder),
	m_pEnvironmentHelper(pEnvironmentHelper), 
	m_pLastDownloadedFileHeader(0), 
	m_nLastDownloadedFileHeaderSize(0),
	m_CallerPid(0),
	m_pProcessList(pProcessList),
	m_argv(NULL),
	m_pid(pid),
	m_uniq_pid(0),
	m_parent_pid(parent_pid),
	m_parent_uniq_pid(0),
	m_BaseThread(0),
	m_ftStartTime(0),
	m_ftExitTime(0),
	m_nRefCount(1),
	m_nFlags(0),
	m_nAntiAV_Flags(0),
	m_nAntiAV_Count(0),
	m_lock_count(0),
	m_AppId(0)
{
	memset(&fi, 0, sizeof(fi));
	ParseCmdLine(cmd_line);
}

void _ProcListDefaultSlotDestructor(void* context, void* ptr)
{
	if (!ptr)
		return;
	assert(context);
	cProcessListItem* pProcess = (cProcessListItem*)context;
	pProcess->m_pEnvironmentHelper->Free(ptr);
}

cProcessListItem::~cProcessListItem()
{
	if (m_argv)
		//delete[] m_argv;
		m_pEnvironmentHelper->Free(m_argv);

	tstrfree(m_cmdline);
	tstrfree(m_orig_cmdline);
	tstrfree(m_private_args_str);

	m_pEnvironmentHelper->Free(m_pLastDownloadedFileHeader);
	for (tSlotsVector::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		if (it->data && it->destructor)
			it->destructor(it->context, it->data);
	}
}

void cProcessListItem::Lock(bool exclusive)
{
#ifdef _RUNTIME_CHECKS_IMPL_
	uint32_t tid = GetCurrentThreadId();
	if (1 == InterlockedIncrement(&m_lock_count))
	{
		tPid locked_pid = m_pProcessList->iFindLockedProcess(tid);
		assert(locked_pid == 0);
	}
#endif

	m_Sync.lock(exclusive);

#ifdef _RUNTIME_CHECKS_IMPL_
	m_lock_tid = tid;
#endif
}

void cProcessListItem::Unlock()
{
#ifdef _RUNTIME_CHECKS_IMPL_
	if (0 == InterlockedDecrement(&m_lock_count))
		m_lock_tid = 0;
#endif
	m_Sync.unlock();
}

bool cProcessListItem::RecognizeInterpreter(tstring* new_cmd_line)
{
	size_t i;
	if (m_argc < 2)
		return false;

	// TODO: Replace process command line

	if (m_image.compare(_T("%ComSpec%")))
	{
		for (i=1; i<m_argc-1; i++)
		{
			if (0 == tstricmp(m_argv[i], _T("/c"))
				|| 0 == tstricmp(m_argv[i], _T("/k")))
			{
				*new_cmd_line = tstrdup(m_cmdline + (m_argv[i+1] - m_private_args_str));
				return true;
			}
		}
	}

	if (m_image.compare(_T("%systemroot%\\REGEDIT.EXE"))
		|| m_image.compare(_T("%systemroot%\\SYSTEM32\\REGEDT32.EXE")))
	{
		for (i=1; i<m_argc; i++)
		{
			tcstring arg = m_argv[i];
			if (tstrchar(arg) == '/' || tstrchar(arg) == '-')
			{
				arg = tstrinc(arg);
				if (tstrchar(arg) == 'e') // export switch
					break;
				continue; // ignore other switches
			}
			// if not switch
			*new_cmd_line = tstrdup(m_cmdline + (arg - m_private_args_str));
			return true;
		}
	}

	if (m_image.compare(_T("%systemroot%\\SYSTEM32\\CSCRIPT.EXE"))
		|| m_image.compare(_T("%systemroot%\\SYSTEM32\\WSCRIPT.EXE"))
		|| m_image.compare(_T("%systemroot%\\SYSTEM32\\MSHTA.EXE")))
	{
		
		for (i=1; i<m_argc; i++)
		{
			tcstring arg = m_argv[i];
			if (tstrchar(arg) == '/' || tstrchar(arg) == '-')
			{
				continue; // ignore switches
			}
			// if not switch
			*new_cmd_line = tstrdup(m_cmdline + (arg - m_private_args_str));
			return true;
		}
	}
	return false;
}
void cProcessListItem::ParseCmdLine(tcstring cmd_line)
{
	m_argc = 0;
	m_orig_cmdline = tstrdup(cmd_line);
	m_cmdline = tstrdup(cmd_line);
	m_private_args_str = tstrdup(cmd_line);
	size_t max_args = srvParseCmdLine(m_pEnvironmentHelper, m_private_args_str, NULL, NULL, 0, GetFlag(pfInterpreter));
	if (max_args)
		//m_argv = new tcstring[max_args];
		m_argv = (tcstring*) m_pEnvironmentHelper->Alloc(max_args * sizeof(tcstring));
	if (m_argv)
	{
		tstring new_cmd_line = 0;
		tstring new_executable;
		do {
			m_argc = srvParseCmdLine(m_pEnvironmentHelper, m_private_args_str, NULL, m_argv, max_args, GetFlag(pfInterpreter));
			if (m_argc && new_cmd_line)
			{
				cPath new_path(m_pEnvironmentHelper, m_argv[0], 0, FILE_ATTRIBUTE_UNDEFINED, m_working_folder.get());
 				if (new_path.isExist())
					m_image = new_path;
				else if (new_executable = m_pEnvironmentHelper->PathFindExecutable(m_argv[0], m_working_folder.get())) {
					m_image = new_executable;
					tstrfree(new_executable);
				}
			}

			new_cmd_line = 0;
			if (RecognizeInterpreter(&new_cmd_line))
			{
				SetFlag(pfInterpreter);
				tstrfree(m_cmdline); m_cmdline = new_cmd_line;
				tstrfree(m_private_args_str); m_private_args_str = tstrdup(new_cmd_line);
			}
		} while (new_cmd_line);
	}
}

long cProcessListItem::AddRef()
{
	return m_pEnvironmentHelper->InterlockedInc(&m_nRefCount);
}

long cProcessListItem::ReleaseRef()
{
	long refcount = m_pEnvironmentHelper->InterlockedDec(&m_nRefCount);
	if (0 == refcount)
		delete this;
	return refcount;
}

long cProcessListItem::GetRefCount()
{
	return m_pEnvironmentHelper->InterlockedExchAdd(&m_nRefCount, 0);
}

cProcess cProcessListItem::get_parent()
{
	return m_pProcessList->FindProcess(m_parent_uniq_pid);
}

void* cProcessListItem::GetSlotData(size_t slot_index, size_t data_size)
{
	if (!data_size)
		return 0;
	if (m_slots.size() <= slot_index)
	{
		// new slot
		m_slots.resize(slot_index+1);
	}
	else
	{
		// existing slot
		if (m_slots[slot_index].destructor && m_slots[slot_index].destructor != _ProcListDefaultSlotDestructor) // data set with SetSlotPtr can be retrieved only with GetSlotPtr
			return NULL;
		void* ptr = m_slots[slot_index].data;
		if (ptr)
			return ptr;
	}
	// empty slot
	void* ptr = m_pEnvironmentHelper->Alloc(data_size);
	if (!ptr)
		return NULL;
	memset(ptr, 0, data_size);
	m_slots[slot_index].data = ptr;
	m_slots[slot_index].size = data_size;
	m_slots[slot_index].destructor = _ProcListDefaultSlotDestructor;
	m_slots[slot_index].context = this;
	return ptr;
}

bool cProcessListItem::SetSlotPtr(size_t slot_index, void* ptr, void (KL_CDECL *slot_destructor)(void* context, void* ptr), void* context)
{
	if (ptr && !slot_destructor)
		return false;
	if (m_slots.size() <= slot_index)
	{
		// new slot
		m_slots.resize(slot_index+1);
	}
	else
	{
		// existing slot
		if (m_slots[slot_index].destructor == _ProcListDefaultSlotDestructor) // data set with GetSlotData cannot be overridden with SetSlotPtr
			return false;
	}
	if (m_slots[slot_index].data && m_slots[slot_index].destructor)
		m_slots[slot_index].destructor(m_slots[slot_index].context, m_slots[slot_index].data);
	m_slots[slot_index].data = ptr;
	m_slots[slot_index].size = 0;
	m_slots[slot_index].destructor = slot_destructor;
	m_slots[slot_index].context = context;
	return true;
}

void* cProcessListItem::GetSlotPtr(size_t slot_index)
{
	if (m_slots.size() <= slot_index)
		return NULL;
	if (m_slots[slot_index].destructor == _ProcListDefaultSlotDestructor) // data set with GetSlotData cannot be overridden with SetSlotPtr
		return NULL;
	return m_slots[slot_index].data;
}


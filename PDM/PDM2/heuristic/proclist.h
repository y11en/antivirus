#ifndef __PROCESS_LIST__H__
#define __PROCESS_LIST__H__

class cProcessListItem;
class cProcess;
class cProcessList;

#include <vector>
#include "../eventmgr/include/types.h"
#include "../eventmgr/include/pathlist.h"
#include "../eventmgr/include/lock.h"
#include "../eventmgr/include/cpath.h"
#include "../eventmgr/include/opcache.h"
class cEvent;

#define MAX_THREAD_ID  0x1000
#define MAX_PROCESS_ID 0x1000

#define FLAG_UNIQ_PID ((tPid)1<<63)

typedef std::vector<cProcessListItem*> tProcList;


// process creation_flags values
#define cDEBUG_PROCESS           0x00000001
#define cDEBUG_ONLY_THIS_PROCESS 0x00000002
#define cCREATE_SUSPENDED        0x00000004

typedef enum enProcessFlags
{
	pfCMD               = 0x00,
	pfInterpreter       = 0x01,
	pfInstaller         = 0x02,
	
	pfDetected          = 0x38,
	pfTrusted           = 0x3F,
	// maximum 64 (0x00 - 0x3F) flags!!!
};
#define  MAKE_PROCESS_FLAG(enProcessFlags) (((uint64_t)1)<<enProcessFlags)

/*
#define _SPRC_FLAG_NONE				0x0000
#define _SPRC_FLAG_DONT_CHECK		0x0001
#define _SPRC_FLAG_SCM				0x0002
#define _SPRC_FLAG_INETBORWSER		0x0004
#define _SPRC_FLAG_SCRIPT			0x0008
#define _SPRC_FLAG_SHELL			0x0010
#define _SPRC_FLAG_CMD				0x0020
#define _SPRC_FLAG_TASKMGR			0x0040

#define _SPRC_FLAG_USER_TRUST	0x1000
#define _SPRC_FLAG_REG_TRUST	0x2000

class cDownloadInfo {
public:
	cStringObj m_sUrl;
	uint64_t     m_qwSize;
	tDWORD     m_PID;
	tBOOL      m_bIncoming;
	tBOOL      m_bPacketStream;
	tBOOL      m_bCompressed;
	time_t     m_Time;
	tDWORD     m_dwDataSize;
	tBYTE*     m_pData;
	cDownloadInfo() {
		m_qwSize = 0;
		m_PID = 0;
		m_bIncoming = cFALSE;
		m_bPacketStream = cFALSE;
		m_bCompressed = cFALSE;
		m_Time = 0;
		m_dwDataSize = 0;
		m_pData = NULL;
	}
	~cDownloadInfo() {
		if (m_pData)
			g_root->heapFree(m_pData);
		m_dwDataSize = 0;
		m_pData = NULL;
	}
	cDownloadInfo& operator = (cDownloadInfo& info) {
		memcpy(this, &info, sizeof(cDownloadInfo));
		if (!m_dwDataSize)
			m_pData = NULL;
		if (m_pData)
		{
			if (PR_FAIL(g_root->heapAlloc((tPTR*)&m_pData, m_dwDataSize)))
			{
				m_pData = NULL;
				m_dwDataSize = 0;
			}
			else
			{
				memcpy(m_pData, info.m_pData, m_dwDataSize);
			}
		}
		return *this;
	}
};

*/

class cProcessListItem
{
public:
	cProcessListItem(cEnvironmentHelper* pEnvironmentHelper, cProcessList* pProcessList, tPid pid, tPid parent_pid, cFile& image_path, cPath& working_folder, tcstring cmd_line);
	~cProcessListItem();

	long AddRef();
	long ReleaseRef();
	long GetRefCount();

	void Lock(bool exclusive = true);
	void Unlock();

	cProcess get_parent();

	void* GetSlotData(size_t slot_index, size_t data_size);

	bool SetSlotPtr(size_t slot_index, void* ptr, void (KL_CDECL *slot_destructor)(void* context, void* ptr), void* context);
	void* GetSlotPtr(size_t slot_index);

	//+ ------------------------------------------------------------------------------------------
public:
	tPid      m_parent_pid;
	tPid      m_parent_uniq_pid;
	tPid      m_pid;
	tPid      m_uniq_pid;
	cPath     m_image;
	cPath     m_orig_image;
	cPath     m_working_folder;
	tstring   m_cmdline;
	tstring   m_orig_cmdline;
	size_t    m_argc;
	tcstring* m_argv; 

	tTid  m_BaseThread;

	uint64_t  m_ftStartTime;
	uint64_t  m_ftExitTime;
	
	cPathList m_SelfCopyList;
	
	cPathList m_CreatedImagesList;
	cFileFormatList m_ModifiedImagesList;
	cFileFormatList m_DeletedFilesList;
	
	cPathList m_ModifiedProgFilesDirsList;
	cPathList m_ModifiedNonLocalFilesList;

	FILE_FORMAT_INFO fi;

	uint32_t  m_nAntiAV_Flags;
	uint32_t  m_nAntiAV_Count;

	uint32_t  m_nAntiPDM_Count;

	void*  m_pLastDownloadedFileHeader;
	size_t m_nLastDownloadedFileHeaderSize;

	tPid   m_CallerPid;
	
	cOpCache m_OpCacheFileCreate;
	cOpCache m_OpCacheFileModify;
	cOpCache m_OpCacheFileDelete;

//	uint32_t m_ProcessHandleInParentProcess; // R3 handle received by parent
//	tHANDLE m_ProcessHandle; // R3 handle opened by PDM
		
//	bool m_bRedirectedInOut;
//	CModifyHistory* m_pHistory;
//	bool m_bDetected;

//	bool m_CheckCommandCompleted;
//	bool m_CheckEnvCompleted;

//	tstring m_pwchTemp;
//	tstring m_pwchTmp;
	
//	bool IsType(uint32_t Mask);
	
//	bool m_bExecCommand;
//	_etFeatureReport m_HiddenReport;
//	_etFeatureReport m_KeyLogger;
	
//	void SetImageHash(__int64 nHash);
//	__int64 GetImageHash();
	
//	bool GetImageHashMD5(BYTE hash[16]);

//	void ClearMemWriteList(tPid pid);
	
//	bool m_bInternalControlled;
//	bool m_bAllUnknownDll_AddToSharedList;
//	bool m_bAllUnknownDll_Block;
	
//	SYSTEMTIME m_SysTime;
	
//	uint32_t m_nDroppedDLLs;
//	uint32_t m_nDroppedEXEs;
//	bool  m_bInstaller;
//	bool  m_bInstallerChecked;
//	bool  m_bWasTerminated;

//	cDownloadInfo DownloadInfo;

//	uint32_t m_HostsFlags;
//	uint32_t m_HostsCount;
	uint32_t m_AppId;

public:
	bool SetFlag(enProcessFlags flag)
	{
		m_nFlags |= MAKE_PROCESS_FLAG(flag);
		return true;
	}
	bool GetFlag(enProcessFlags flag)
	{
		return (0 != (m_nFlags & MAKE_PROCESS_FLAG(flag)));
	}
	uint64_t GetFlags(uint64_t flags)
	{
		return (m_nFlags & flags);
	}


	uint32_t m_lock_tid;
	cEnvironmentHelper* m_pEnvironmentHelper;
	cProcessList* m_pProcessList;

private:
	long m_nRefCount;
	uint64_t m_nFlags;
	tstring m_private_args_str;

	bool RecognizeInterpreter(tstring* new_cmd_line);
	void ParseCmdLine(tcstring cmd_line);

//	__int64 m_nImageHash;
//	bool    m_bImageHashMD5_cached;
//	BYTE    m_bImageHashMD5[16];
	cLock    m_Sync;
	long     m_lock_count;
	typedef struct {
		void*  data;
		size_t size;
		void (KL_CDECL *destructor)(void* context, void* ptr);
		void* context;
	} tSlot;
	typedef  std::vector<tSlot> tSlotsVector;
	tSlotsVector m_slots;
};


class cAutoProcessItemLock
{
public:
	cAutoProcessItemLock(cProcessListItem* pProcessItem, bool exclusive = true) : m_pProcessItem(pProcessItem)
	{
		m_pProcessItem->Lock(exclusive);
	}
	~cAutoProcessItemLock()
	{
		m_pProcessItem->Unlock();
	}
private:
	cProcessListItem* m_pProcessItem;
};

typedef struct tag_CALLER_INFO {
	tPid nCallerPid;
	bool bInterpreteur;
} CALLER_INFO;

class cProcessList
{
public:
	cProcessList(cEnvironmentHelper* pEnvironmentHelper);
	~cProcessList();
	
	bool ProcessCreate(tPid pid, tPid parent_pid, cFile& image_path, cPath& working_folder, tcstring cmd_line, uint64_t start_time);
	bool ProcessTerminate(tPid pid, uint64_t exit_time);
	bool ProcessDestroy(tPid pid);

	bool ThreadCreate(tPid pid, tTid tid, tPid dest_pid, tTid new_tid);
	bool ThreadTerminate(tPid pid, tTid tid, tPid dest_pid, tTid term_tid);

	bool DoDelayedProcessesExit(uint64_t current_time, uint64_t delay_time);

	size_t count();
	bool clear();

	cProcess FindProcess(tPid pid, bool bFindExited = false);

	bool GetSnapshot(tPid** pPidArray, size_t* pnPidCount);
	bool GetChildSnapshot(tPid parent, tPid** pChildPidArray, size_t* pnChildPidCount);
	void ReleaseSnapshot(tPid** pPidArray);

	bool AllocSlot(size_t* slot_index);

//	void InitList();
//	void InitListNt4();
//	void AddProcessByPid(tPid pid, uint32_t ParentPid);

//	void SearchHidden();
//	bool FindFirstHiddenProcess(cStrObj* pImage, DWORD* pPID);

//	void RefreshTrusted();

//	cProcessListItem* FindProcessByHandle(uint32_t ProcessHandleInParentProcess);
//	cProcessListItem* FindProcessByDroppedFile(PWSTR pwchDrvPath);

	//void UpdatepidByHandle(uint32_t ProcessHandleInParentProcess, tPid pid);
	
	// void ChangeStrProp(tstring* ppwchOld, tstring pwcnNew);

	//void FillChildList(uint32_t Parentpid, cVector<cPdmProcList>* pChildProcList);
	
	// void DetachParentHistoryByPid(tPid pid);

	// bool IsEqualProcessNames(uint32_t ProcId1, uint32_t ProcId2);

	//cProcess operator[](size_t index);
	cProcess GetProcess(size_t index);
	
	bool GetRemoteThreadCreator(tTid tid, tPid* ppid);
	bool GetCaller(tPid pid, tPid* ppid);
	bool IsInterpreteur(tPid pid);
	bool GetTopmostCaller(tPid pid, tTid tid, tPid* pCallerPid);

	tPid iFindLockedProcess(uint32_t tid);
	cProcessListItem* iFindProcess(tPid pid, bool bFindExited = false);
private:
	cEnvironmentHelper* m_pEnvironmentHelper;
	tPid m_next_uniq_pid;
	cLock m_Sync;
	tProcList m_ProcList;
	tPid m_Tid2Pid[MAX_THREAD_ID/4];
	CALLER_INFO m_Callers[MAX_PROCESS_ID/4];
	size_t m_next_slot_index;
};

cProcessListItem* _iFindProcess(cEventHandler* pEventHandler, tPid pid);

//+ ------------------------------------------------------------------------------------------

class cProcess
{
public:
	cProcess() : m_pProc(NULL)
	{
	}
	
	cProcess(cProcessListItem* pProc) : m_pProc(pProc)
	{
	}

/*	void operator= (cProcessListItem* pProc)
	{
		release();
		m_pProc = pProc;
	}
*/
	cProcessListItem* operator->()
	{
		return m_pProc;
	}
	void operator= (const cProcess& pProcRef)
	{
		release();
		m_pProc = pProcRef.m_pProc;
		if (m_pProc)
		{
			m_pProc->AddRef();
			m_pProc->Lock();
		}
	}
	operator cProcessListItem*()
	{
		return m_pProc;
	}
	~cProcess()
	{
		release();
	}
	void release()
	{
		if (m_pProc)
		{
			m_pProc->Unlock();
			m_pProc->ReleaseRef();
			m_pProc = NULL;
		}
	}
private:
	cProcessListItem* m_pProc;
};

#endif // __PROCESS_LIST__H__

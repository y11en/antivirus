#include "heur_invader.h"
#include "trace.h"

cEventHandlerHeurInvader::cEventHandlerHeurInvader()
{
}

cEventHandlerHeurInvader::~cEventHandlerHeurInvader()
{
}

bool cEventHandlerHeurInvader::IsSelfInvader(cEvent& event, tPid dest_pid)
{
	bool bRes = false;
	tcstring dst_image_path = 0;
	if (event.m_pid == dest_pid)
		return true;
	cProcess pProcessDst = m_pProcessList->FindProcess(dest_pid);
	if (!pProcessDst)
		return false;
	dst_image_path = tstrdup(pProcessDst->m_image.getFull());
	pProcessDst.release();
	if (!dst_image_path)
		return false;
	cProcess pProcessSrc = event.GetProcess();
	if (pProcessSrc)
	{
		if (0 == tstrcmp(pProcessSrc->m_image.getFull(), dst_image_path))
			bRes = true;
		else if (pProcessSrc->m_CreatedImagesList.contain(dst_image_path))
			bRes = true;
	}
	tstrfree(dst_image_path);
	return bRes;
}

bool cEventHandlerHeurInvader::IsInvader(cEvent& event, tPid dest_pid, tAddr addr)
{
	if (event.m_pid == dest_pid)
		return false;
	if (!IsAddressNotExecutable(event.m_pid, dest_pid, addr))
		return false;
	if (IsSelfInvader(event, dest_pid))
		return false;
	return true;
}

bool cEventHandlerHeurInvader::IsAddressNotExecutable(tPid src_pid, tPid dest_pid, tAddr addr)
{
	if (!addr)
		return false;

	PROCMEMINFO pmi;
	if (m_pEnvironmentHelper->ProcessVirtualQuery(dest_pid, addr, &pmi))
	{
		if (pmi.Type != cMEM_IMAGE)
			return true;
	}
	
	if (m_MemWriteList.find(src_pid, dest_pid, addr))
		return true;

	tAddr addrLoadLibrary;

	addrLoadLibrary = m_pEnvironmentHelper->ProcessGetProcAddr(dest_pid, _T("kernel32.dll"), "LoadLibraryA");
	if (addrLoadLibrary && addrLoadLibrary == addr)
		return true;
	addrLoadLibrary = m_pEnvironmentHelper->ProcessGetProcAddr(dest_pid, _T("kernel32.dll"), "LoadLibraryW");
	if (addrLoadLibrary && addrLoadLibrary == addr)
		return true;

	return false;
}

void cEventHandlerHeurInvader::OnThreadCreatePre(cEvent& event, tPid dest_pid, tAddr eip, tAddr thread_base)
{
	if (event.m_pid == dest_pid)
		return;

	cProcess pProcessDest = m_pProcessList->FindProcess(dest_pid);
	if (!pProcessDest)
		return;
	if (0 == pProcessDest->m_BaseThread) // this is creation of process base thread
		return;
	tPid owner_pid;
	if (m_pEnvironmentHelper->ThreadGetProcess(event.m_tid, &owner_pid))
	{
		// skip local thread creation
		if (dest_pid == owner_pid)
			return;
	}
	pProcessDest.release();

	if (IsInvader(event, dest_pid, eip))
		return m_pEventMgr->OnProcessInvader(event, dest_pid, eip);

	if (IsInvader(event, dest_pid, thread_base))
		return m_pEventMgr->OnProcessInvader(event, dest_pid, thread_base);

	return;
}

void cEventHandlerHeurInvader::OnThreadSetContextPre(cEvent& event, tPid dest_pid, tTid dest_tid, tAddr eip)
{
	if (event.m_pid == dest_pid)
		return;

	if (IsInvader(event, dest_pid, eip))
		return m_pEventMgr->OnProcessInvader(event, dest_pid, eip);
		
	return;
}

void cEventHandlerHeurInvader::OnWriteProcessMemoryPre(cEvent& event, tPid dest_pid, tAddr addr, void* pData, size_t nDataSize)
{
	if (event.m_pid == dest_pid) // write into itself
		return;

	if (addr==0x10000 || addr==0x20000 || addr==0x30000) // process init
		return;
	
	if (IsSelfInvader(event, dest_pid))
		return;

	PROCMEMINFO mem_info;
	if (m_pEnvironmentHelper->ProcessVirtualQuery(dest_pid, addr, &mem_info))
	{
		const uint32_t cExecuteFlags = cPAGE_EXECUTE | cPAGE_EXECUTE_READ | cPAGE_EXECUTE_READWRITE | cPAGE_EXECUTE_WRITECOPY;
		if ((mem_info.Type == cMEM_IMAGE) ||
			(mem_info.Protect & cExecuteFlags) ||
			(mem_info.AllocationProtect & cExecuteFlags))
		{
			PR_TRACE((g_root, prtIMPORTANT, TR "Process %d remotely writes to pid %d, image <%S> len %d",
				pEvt->m_ProcessID, DestProcId, *wcsModuleName ? wcsModuleName : L"unknown", DataLen));
			m_pEventMgr->OnProcessInvader(event, dest_pid, addr);
			if (event.GetVerdict() != evtVerdict_Default)
				return;
		}
	}

	m_MemWriteList.add(event.m_pid, dest_pid, addr, nDataSize);
			
	return;
}

void cEventHandlerHeurInvader::OnProcessExitPost(cEvent& event)
{
	m_MemWriteList.remove(event.m_pid);
	return;
}

void cEventHandlerHeurInvader::OnMapViewOfSectionPost(cEvent& event, tPid dest_pid, tAddr addr, size_t nSize)
{
	if (IsSelfInvader(event, dest_pid))
		return;
	m_MemWriteList.add(event.m_pid, dest_pid, addr, nSize);
	return;
}

void cEventHandlerHeurInvader::OnSetWindowsHook(cEvent& event, uint32_t idHook, tAddr lphf, tHANDLE hMod, tTid dwThreadId)
{
	tstring sDllName;
	if (hMod == 0)
		return;
	if (dwThreadId != 0)
		return;
	sDllName = m_pEnvironmentHelper->ProcessGetModuleFilename(event.m_primary_event->m_pid, hMod);
	if (!sDllName)
		return;
	cFile DllFile(m_pEnvironmentHelper, sDllName);
	tstrfree(sDllName);
	cProcess pProcess = event.GetProcess();
	if (!pProcess)
		return;
	if (!pProcess->m_CreatedImagesList.contain(DllFile.getFull()))
		return;
	pProcess.release();
	return m_pEventMgr->OnDllInvader(event, 0, DllFile);
}


#include <assert.h>

#include "../include/eventmgr.h"
#include "../../heuristic/srv.h"

//#include "../../../../virlab/imul/emul/emulator.h"
//#include "../../../../virlab/imul/common/DbgNew.h" // must be last!

#define CALL_NEXT(call) cEventHandler::call

cEventMgr::cEventMgr(cEnvironmentHelper* pEnvironmentHelper)
{
	m_pSubHandlersList = new tEventHandlerList;
	m_pSubHandlersListLock = new cLock();
	m_pProcessList = new cProcessList(pEnvironmentHelper);
	m_pEnvironmentHelper = pEnvironmentHelper;
	m_pEventMgr = this;
}

cEventMgr::~cEventMgr()
{
	if (m_pSubHandlersList)
	{
		cAutoLock _lock(*m_pSubHandlersListLock, true);
		for (tEventHandlerList::iterator _it = m_pSubHandlersList->begin(); _it != m_pSubHandlersList->end(); _it++)
		{
			if (_it->bDelete)
				delete _it->pEventHandler;
		}
		m_pSubHandlersList->clear();
		delete m_pSubHandlersList;
	}
	if (m_pSubHandlersListLock)
		delete m_pSubHandlersListLock;
	if (m_pProcessList)
		delete m_pProcessList;
}

bool cEventMgr::RegisterHandler(cEventHandler* pHandler, bool bDelete)
{
	if (!pHandler)
		return false;
	if (!m_pSubHandlersList || !m_pSubHandlersListLock)
		return false;
	pHandler->m_pEventMgr = this;
	pHandler->m_pEnvironmentHelper = m_pEnvironmentHelper;
	pHandler->m_pProcessList = m_pProcessList;
	if (!pHandler->Init())
		return false;
	cEventHandlerDescr dsc;
	dsc.pEventHandler = pHandler;
	dsc.bDelete = bDelete;
	cAutoLock _lock(*m_pSubHandlersListLock, true);
	m_pSubHandlersList->push_back(dsc);
	return true;
}

bool cEventMgr::UnregisterHandler(cEventHandler* pHandler)
{
	if (!pHandler)
		return false;
	if (!m_pSubHandlersList || !m_pSubHandlersListLock)
		return false;
	bool bFound = false;
	{
		cAutoLock _lock(*m_pSubHandlersListLock, true);
		for (tEventHandlerList::iterator _it = m_pSubHandlersList->begin(); _it != m_pSubHandlersList->end(); _it++)
		{
			if (_it->pEventHandler == pHandler)
			{
				m_pSubHandlersList->erase(_it);
				bFound = true;
				break;
			}
		}
	}
	if (bFound)
		pHandler->Done();
	return bFound;
}


void cEventMgr::OnProcessCreatePre(cEvent& event, cFile& image_path, tcstring cmd_line, uint32_t creation_flags)
{
	if (!event.m_bForwardCall)
	{
		INIT_EVENT(event);
	}
	CALL_NEXT(OnProcessCreatePre(event, image_path, cmd_line, creation_flags));
}

void cEventMgr::OnProcessCreatePost(cEvent& event, tPid new_proc_pid, cFile& image_path, cPath& working_folder, tcstring cmd_line, uint32_t creation_flags)
{
	if (!event.m_bForwardCall)
	{
		INIT_EVENT(event);
		m_pProcessList->ProcessCreate(new_proc_pid, event.m_pid, image_path, working_folder, cmd_line, event.GetTime());
	}
	CALL_NEXT(OnProcessCreatePost(event, new_proc_pid, image_path, working_folder, cmd_line, creation_flags));
}

void cEventMgr::OnProcessExitPre(cEvent& event)
{
	if (!event.m_bForwardCall)
	{
		INIT_EVENT(event);
	}
	CALL_NEXT(OnProcessExitPre(event));
}

void cEventMgr::OnProcessExitPost(cEvent& event)
{
	if (!event.m_bForwardCall)
	{
		INIT_EVENT(event);
		event.GetProcess(); // reference process, because after ProcessTerminate() GetProcess() will not find process by native pid
		m_pProcessList->ProcessTerminate(event.m_pid, event.GetTime());
	}
	CALL_NEXT(OnProcessExitPost(event));
}

void cEventMgr::OnThreadCreatePre(cEvent& event, tPid dest_pid, tAddr eip, tAddr thread_base)
{
	if (!event.m_bForwardCall)
	{
		INIT_EVENT(event);
	}
	CALL_NEXT(OnThreadCreatePre(event, dest_pid, eip, thread_base));
}

void cEventMgr::OnThreadCreatePost(cEvent& event, tPid dest_pid, tTid new_tid, tAddr eip, tAddr thread_base)
{
	if (!event.m_bForwardCall)
	{
		INIT_EVENT(event);
		m_pProcessList->ThreadCreate(event.m_pid, event.m_tid, dest_pid, new_tid);
	}
	CALL_NEXT(OnThreadCreatePost(event, dest_pid, new_tid, eip, thread_base));
}

void cEventMgr::OnThreadTerminatePost(cEvent& event, tPid dest_pid, tTid dest_tid)
{
	if (!event.m_bForwardCall)
	{
		INIT_EVENT(event);
		m_pProcessList->ThreadTerminate(event.m_pid, event.m_tid, dest_pid, dest_tid);
	}
	CALL_NEXT(OnThreadTerminatePost(event, dest_pid, dest_tid));
}

static bool iMoveFileFormatListItem(cFileFormatList& list_src, cFileFormatList& list_dst, cFile& file)
{
	cFileFormatList::iterator it_s = list_src.find(file.getFull());
	if (it_s == list_src.end())
		return false;
	if (!list_dst.add(file.getFull(), it_s->pfi))
		return false;
	it_s->pfi = 0;
	list_src.erase(it_s);
	return true;
}

enum enCreateDisposition {
	cdUnknown,
	cdOpen,
	cdCreate,
	cdTruncate
};

enCreateDisposition ResolveDisposition(cFile& file, uint32_t creation_disposition)
{
	switch(creation_disposition)
	{
	case OPEN_EXISTING: return cdOpen;
	case OPEN_ALWAYS:   return (file.isExist() ? cdOpen : cdCreate);
	case CREATE_NEW:    return cdCreate;
	case CREATE_ALWAYS: return (file.isExist() ? cdTruncate : cdCreate);
	case TRUNCATE_EXISTING: return cdTruncate;
	}
	return cdUnknown;
}

// void cEventMgr::__OnFileCreatePre(cEvent& event, cFile& file, uint32_t desired_access, uint32_t share_mode, uint32_t creation_disposition)
// {
// 	if (!m_pSubHandlersList)
// 		return;  
// 	INIT_EVENT(event);  
// 	cAutoLock _lock(*m_pSubHandlersListLock, false);
// 	if (!m_pProcessList->IsInterpreteur(event.m_pid))  
// 	{  
// 		for (tEventHandlerList::iterator _it = m_pSubHandlersList->begin(); _it != m_pSubHandlersList->end(); _it++)  
// 			_it->pEventHandler->OnFileCreatePre(event, file, desired_access, share_mode, creation_disposition);  
// 	}  
// 	if (event.m_bSkipForward) 
// 		return; 
// 	tPid caller_pid = event.m_pid; 
// 	void* context = event.m_context; 
// 	if (m_pProcessList->GetCaller(caller_pid, &caller_pid)) 
// 	{  
// 		cEvent event(context, caller_pid);  
// 		INIT_EVENT(event);  
// 		event.m_bForwardCall = true; 
// 		m_pEventMgr->OnFileCreatePre(event, file, desired_access, share_mode, creation_disposition);   
// 	}
// 	return;
// }

void cEventMgr::OnFileCreatePre(cEvent& event, cFile& file, uint32_t desired_access, uint32_t share_mode, uint32_t creation_disposition)
{
	INIT_EVENT(event);
	assert(file.getUniq());
	
	cEventHandler::OnFileCreatePre(event, file, desired_access, share_mode, creation_disposition);

	if (creation_disposition == OPEN_EXISTING)
		; // open for read-only 
	else
	{
		cProcess pProcess = event.GetProcess();
		if (pProcess)
		{
			enCreateDisposition disp = cdUnknown;
			switch(creation_disposition)
			{
			case OPEN_EXISTING:     disp = cdOpen; break;
			case OPEN_ALWAYS:       disp = (file.isExist() ? cdOpen : cdCreate); break;
			case CREATE_NEW:        disp = cdCreate; break;
			case CREATE_ALWAYS:     disp = (file.isExist() ? cdTruncate : cdCreate); break;
			case TRUNCATE_EXISTING: disp = cdTruncate; break;
			}

//			MESSAGE(DIL_DEBUG, std::endl << "m_OpCacheFileCreate.Add(" << disp << ") pid=" << event.m_pid << (event.m_bForwardCall ? " <FORWARD>" : ""));
			pProcess->m_OpCacheFileCreate.Add(file.getUniq(), (void*)disp);
			pProcess.release();

			bool bOldSkipForward = event.m_bSkipForward;
			event.m_bSkipForward = true;
			switch (disp)
			{
			case cdCreate:
				OnFileCreateNewPre(event, file);
				break;
			case cdTruncate:
				OnFileModifyPre(event, file);
				break;
			case cdOpen:
				break;
			default: 
				assert(0);
			}
			event.m_bSkipForward = bOldSkipForward;
		}
	}
	return;
}

void cEventMgr::OnFileCreatePost(cEvent& event, cFile& file, uint32_t desired_access, uint32_t share_mode, uint32_t creation_disposition)
{
	INIT_EVENT(event);
	assert(file.getUniq());
	if (creation_disposition == OPEN_EXISTING)
		; // open for read-only 
	else
	{
		cProcess pProcess = event.GetProcess();
		if (pProcess)
		{
			enCreateDisposition disp = cdUnknown;
			pProcess->m_OpCacheFileCreate.Remove(file.getUniq(), (void**)&disp);
//			MESSAGE(DIL_DEBUG, std::endl << "m_OpCacheFileCreate.Remove(" << disp << ") pid=" << event.m_pid << (event.m_bForwardCall ? " <FORWARD>" : ""));
			pProcess.release();

			bool bOldSkipForward = event.m_bSkipForward;
			event.m_bSkipForward = true;
			switch (disp)
			{
			case cdCreate:
				OnFileCreateNewPost(event, file);
				break;
			case cdTruncate:
				OnFileModifyPost(event, file);
				break;
			case cdOpen:
				break;
			default: 
				assert(0);
			}
			event.m_bSkipForward = bOldSkipForward;
		}
	}
	cEventHandler::OnFileCreatePost(event, file, desired_access, share_mode, creation_disposition);
	return;
}

void cEventMgr::OnFileCreateFail(cEvent& event, cFile& file, uint32_t desired_access, uint32_t share_mode, uint32_t creation_disposition)
{
	INIT_EVENT(event);
	assert(file.getUniq());
	if (creation_disposition == OPEN_EXISTING)
		; // open for read-only 
	else
	{
		cProcess pProcess = event.GetProcess();
		if (pProcess)
		{
			enCreateDisposition disp = cdUnknown;
			pProcess->m_OpCacheFileCreate.Remove(file.getUniq(), (void**)&disp);
///			MESSAGE(DIL_DEBUG, std::endl << "m_OpCacheFileCreate.Remove[fail](" << disp << ") pid=" << event.m_pid << (event.m_bForwardCall ? " <FORWARD>" : ""));
			pProcess.release();

			bool bOldSkipForward = event.m_bSkipForward;
			event.m_bSkipForward = true;
			switch (disp)
			{
			case cdCreate:
				OnFileCreateNewFail(event, file);
				break;
			case cdTruncate:
				OnFileModifyFail(event, file);
				break;
			case cdOpen:
				break;
			default: 
				assert(0);
			}
			event.m_bSkipForward = bOldSkipForward;
		}
	}
	cEventHandler::OnFileCreateFail(event, file, desired_access, share_mode, creation_disposition);
	return;
}

void cEventMgr::OnFileCreateNewPre(cEvent& event, cFile& file)
{
	INIT_EVENT(event);
	CALL_NEXT(OnFileCreateNewPre(event, file));
}

void cEventMgr::OnFileCreateNewPost(cEvent& event, cFile& file)
{
	INIT_EVENT(event);
	cProcess pProcess = event.GetProcess();
	if (pProcess)
	{
		if (iMoveFileFormatListItem(pProcess->m_DeletedFilesList, pProcess->m_ModifiedImagesList, file))
			; // file was deleted before -> modified
		else
			pProcess->m_CreatedImagesList.add(file.getFull());
	}
	pProcess.release();
	CALL_NEXT(OnFileCreateNewPost(event, file));
}

void cEventMgr::OnFileCreateNewFail(cEvent& event, cFile& file)
{
	INIT_EVENT(event);
	CALL_NEXT(OnFileCreateNewFail(event, file));
}

void cEventMgr::OnFileModifyPre(cEvent& event, cFile& file)
{
	INIT_EVENT(event);
	assert(file.getUniq());
	cProcess pProcess = event.GetProcess();
	if (pProcess)
	{
		if (pProcess->m_CreatedImagesList.contain(file.getFull()))
			; // file created by this process -> ignore modification
		else if (pProcess->m_ModifiedImagesList.contain(file.getFull()))
			; // already modified
		else 
		{
			srvGetFileFormat(m_pEnvironmentHelper, file);
			FILE_FORMAT_INFO* pfi = new FILE_FORMAT_INFO(file.fi);
			if (!pProcess->m_OpCacheFileModify.Add(file.getUniq(), pfi))
			{
				assert(0);
				delete pfi;
			}
		}
	}
	pProcess.release();
	CALL_NEXT(OnFileModifyPre(event, file));
}

void cEventMgr::OnFileModifyPost(cEvent& event, cFile& file)
{
	INIT_EVENT(event);
	assert(file.getUniq());
	cProcess pProcess = event.GetProcess();
	if (pProcess)
	{
		FILE_FORMAT_INFO* pfi = NULL;
		pProcess->m_OpCacheFileModify.Remove(file.getUniq(), (void**)&pfi);
		if (pProcess->m_CreatedImagesList.contain(file.getFull()))
			; // file created by this process -> ignore modification
		else if (pProcess->m_ModifiedImagesList.contain(file.getFull()))
			; // already modified
		else if (pProcess->m_ModifiedImagesList.add(file.getFull(), pfi))
			pfi = 0;
		if (pfi)
			delete pfi;
	}
	pProcess.release();
	CALL_NEXT(OnFileModifyPost(event, file));
}

void cEventMgr::OnFileModifyFail(cEvent& event, cFile& file)
{
	INIT_EVENT(event);
	assert(file.getUniq());
	cProcess pProcess = event.GetProcess();
	if (pProcess)
	{
		FILE_FORMAT_INFO* pfi = NULL;
		pProcess->m_OpCacheFileModify.Remove(file.getUniq(), (void**)&pfi);
		if (pfi)
			delete pfi;
	}
	pProcess.release();
	CALL_NEXT(OnFileModifyFail(event, file));
}

void cEventMgr::OnFileDeletePre(cEvent& event, cFile& file)
{
	INIT_EVENT(event);
	assert(file.getUniq());
	cProcess pProcess = event.GetProcess();
	if (pProcess)
	{
		FILE_FORMAT_INFO* pfi = NULL;
		if (pProcess->m_CreatedImagesList.contain(file.getFull()))
			; // file created by this process
		else
		{
			srvGetFileFormat(m_pEnvironmentHelper, file);
			pfi = new FILE_FORMAT_INFO(file.fi);
			if (!pProcess->m_OpCacheFileDelete.Add(file.getUniq(), pfi))
			{
				assert(0);
				delete pfi;
			}
		}
	}
	pProcess.release();
	CALL_NEXT(OnFileDeletePre(event, file));
}

void cEventMgr::OnFileDeletePost(cEvent& event, cFile& file)
{
	INIT_EVENT(event);
	assert(file.getUniq());
	cProcess pProcess = event.GetProcess();
	if (pProcess)
	{
		FILE_FORMAT_INFO* pfi = NULL;
		pProcess->m_OpCacheFileDelete.Remove(file.getUniq(), (void**)&pfi);
		if (pProcess->m_CreatedImagesList.remove(file.getFull()))
		{
			// file created by this process -> was temp file, just forget about it
		}
		else if (pProcess->m_ModifiedImagesList.contain(file.getFull()))
		{
			// if file was modified -> move it to deleted
			iMoveFileFormatListItem(pProcess->m_ModifiedImagesList, pProcess->m_DeletedFilesList, file);
		}
		else
		{
			if (pProcess->m_DeletedFilesList.add(file.getFull(), pfi))
				pfi = 0;
		}
		if (pfi)
			delete pfi;
	}
	pProcess.release();
	CALL_NEXT(OnFileDeletePost(event, file));
}

void cEventMgr::OnFileDeleteFail(cEvent& event, cFile& file)
{
	INIT_EVENT(event);
	assert(file.getUniq());
	cProcess pProcess = event.GetProcess();
	if (pProcess)
	{
		FILE_FORMAT_INFO* pfi = NULL;
		pProcess->m_OpCacheFileDelete.Remove(file.getUniq(), (void**)&pfi);
		if (pfi)
			delete pfi;
	}
	pProcess.release();
	CALL_NEXT(OnFileDeleteFail(event, file));
}

void cEventMgr::OnFileRenamePre(cEvent& event, cFile& src_file, cFile& dst_file)
{
	INIT_EVENT(event);
	bool bOldSkipForward = event.m_bSkipForward;
	event.m_bSkipForward = true;
	OnFileDeletePre(event, src_file);
	OnFileCreatePre(event, dst_file, GENERIC_WRITE, 0, CREATE_ALWAYS);
	event.m_bSkipForward = bOldSkipForward;
	CALL_NEXT(OnFileRenamePre(event, src_file, dst_file));
}

void cEventMgr::OnFileRenamePost(cEvent& event, cFile& src_file, cFile& dst_file)
{
	INIT_EVENT(event);
	bool bOldSkipForward = event.m_bSkipForward;
	event.m_bSkipForward = true;
	OnFileDeletePost(event, src_file);
	OnFileCreatePost(event, dst_file, GENERIC_WRITE, 0, CREATE_ALWAYS);
	OnFileCloseModifiedPre(event, dst_file);
	OnFileCloseModifiedPost(event, dst_file);
	event.m_bSkipForward = bOldSkipForward;
	CALL_NEXT(OnFileRenamePost(event, src_file, dst_file));
}

void cEventMgr::OnFileRenameFail(cEvent& event, cFile& src_file, cFile& dst_file)
{
	INIT_EVENT(event);
	bool bOldSkipForward = event.m_bSkipForward;
	event.m_bSkipForward = true;
	OnFileDeleteFail(event, src_file);
	OnFileCreateFail(event, dst_file, GENERIC_WRITE, 0, CREATE_ALWAYS);
	event.m_bSkipForward = bOldSkipForward;
	CALL_NEXT(OnFileRenameFail(event, src_file, dst_file));
}

void cEventMgr::OnWriteProcessMemoryPre(cEvent& event, tPid dest_pid, tAddr addr, void* pData, size_t nDataSize)
{
	if (event.m_pid == dest_pid)
		return;
	INIT_EVENT(event);
	CALL_NEXT(OnWriteProcessMemoryPre(event, dest_pid, addr, pData, nDataSize));
}

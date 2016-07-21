#include "eh_syswatch.h"

bool cEHSysWatch::Init()
{
	return cSystemWatcher::IsInited();
}

void cEHSysWatch::OnProcessCreatePost(cEvent& event, tPid parent_pid, cFile& image_path, cPath& working_folder, tcstring cmd_line, uint32_t creation_flags)
{
	if (event.m_bForwardCall)
		return;
	AddEvent(m_pEnvironmentHelper->GetTimeLocal(), parent_pid, evtProcessStart, objFile, image_path.getFull(), tstrlenbz(image_path.getFull()), 0, 0, 0, (uint32)event.m_pid); 
}

void cEHSysWatch::OnProcessExitPost(cEvent& event)
{
	if (event.m_bForwardCall)
		return;
	cProcess pProcess = event.GetProcess();
	if (!pProcess)
		return;
	AddEvent(event.GetTime(), event.m_pid, evtProcessStop, objFile, pProcess->m_image.getFull(), tstrlenbz(pProcess->m_image.getFull())); 
}

void cEHSysWatch::OnFileCreateNewPost(cEvent& event, cFile& file)
{
	if (event.m_bForwardCall)
		return;
	AddEvent(event.GetTime(), event.m_pid, evtCreate, objFile, file.getFull(), tstrlenbz(file.getFull())); 
}

void cEHSysWatch::OnFileModifyPost(cEvent& event, cFile& file)
{
	if (event.m_bForwardCall)
		return;
	AddEvent(event.GetTime(), event.m_pid, evtModify, objFile, file.getFull(), tstrlenbz(file.getFull())); 
}

void cEHSysWatch::OnFileDeletePost(cEvent& event, cFile& file)
{
	if (event.m_bForwardCall)
		return;
	AddEvent(event.GetTime(), event.m_pid, evtDelete, objFile, file.getFull(), tstrlenbz(file.getFull())); 
}

void cEHSysWatch::OnFileRenamePost(cEvent& event, cFile& src_file, cFile& dst_file)
{
	if (event.m_bForwardCall)
		return;
	AddEvent(event.GetTime(), event.m_pid, evtRename, objFile, src_file.getFull(), tstrlenbz(src_file.getFull()), dst_file.getFull(), tstrlenbz(dst_file.getFull())); 
}

void cEHSysWatch::OnRegKeyCreatePost(cEvent& event, const cRegKey& key)
{
	if (event.m_bForwardCall)
		return;
	AddEvent(event.GetTime(), event.m_pid, evtCreate, objRegKey, key.m_fullname, tstrlenbz(key.m_fullname)); 
}

void cEHSysWatch::OnRegKeyRenamePost(cEvent& event, const cRegKey& src_key, const cRegKey& dst_key)
{
	if (event.m_bForwardCall)
		return;
	AddEvent(event.GetTime(), event.m_pid, evtRename, objRegKey, src_key.m_fullname, tstrlenbz(src_key.m_fullname), dst_key.m_fullname, tstrlenbz(dst_key.m_fullname)); 
}

void cEHSysWatch::OnRegKeyDeletePost(cEvent& event, const cRegKey& key)
{
	if (event.m_bForwardCall)
		return;
	AddEvent(event.GetTime(), event.m_pid, evtDelete, objRegKey, key.m_fullname, tstrlenbz(key.m_fullname)); 
}

void cEHSysWatch::OnRegValueSetPost(cEvent& event, const cRegKey& key, tcstring sValueName, uint32_t nNewType, const void* pNewData, size_t nNewDataSize, uint32_t nValueType, const void* pValueData, size_t nValueDataSize)
{
	if (event.m_bForwardCall)
		return;
	tstring sFullName = tstrdup(key.m_fullname);
	if (sFullName) sFullName = tstrappend(sFullName, _T("\\\\"));
	if (sFullName) sFullName = tstrappend(sFullName, sValueName);
	if (!sFullName)
		return;
	AddEvent(event.GetTime(), event.m_pid, nValueDataSize ? evtModify : evtCreate, objRegValue, sFullName, tstrlenbz(sFullName)); 
}

void cEHSysWatch::OnRegValueDeletePost(cEvent& event, const cRegKey& key, tcstring sValueName, uint32_t nValueType, const void* pValueData, size_t nValueDataSize)
{
	if (event.m_bForwardCall)
		return;
	tstring sFullName = tstrdup(key.m_fullname);
	if (sFullName) sFullName = tstrappend(sFullName, _T("\\\\"));
	if (sFullName) sFullName = tstrappend(sFullName, sValueName);
	if (!sFullName)
		return;
	AddEvent(event.GetTime(), event.m_pid, evtDelete, objRegValue, sFullName, tstrlenbz(sFullName)); 
}
#include "heur_killav.h"
#include "heur_av.h"
#include "srv.h"

cEventHandlerHeurKillAV::cEventHandlerHeurKillAV()
{
}

cEventHandlerHeurKillAV::~cEventHandlerHeurKillAV()
{
}

void cEventHandlerHeurKillAV::OnProcessCreatePost(cEvent& event, tPid new_proc_pid, cFile& image_path, cPath& working_folder, tcstring cmd_line, uint32_t creation_flags)
{
	cProcess pProcess = m_pProcessList->FindProcess(new_proc_pid);
	if (!pProcess)
		return;
	if (pProcess->m_argc < 3)
		return;
	if (0 != tstricmp(pProcess->m_argv[1], _T("stop")) && 0 != tstricmp(pProcess->m_argv[1], _T("delete")))
		return;
	size_t i;
	for (i=0; i<countof(arrAntiViruses); i++)
	{
		if(arrAntiViruses[i].service_name == NULL)
			continue;
		if (0 == tstricmp(pProcess->m_argv[2], arrAntiViruses[i].service_name))
			break;
	}
	if (i >= countof(arrAntiViruses))
		return;
	pProcess.release();
	return m_pEventMgr->OnKillService(event, arrAntiViruses[i].service_name);
}

void cEventHandlerHeurKillAV::OnProcessTerminatePre(cEvent& event, tPid dest_pid)
{
	if (event.m_pid == dest_pid)
		return;
	cProcess pProcessDest = m_pProcessList->FindProcess(dest_pid);
	if (!pProcessDest)
		return;
	tcstring filename = pProcessDest->m_image.getFilename();
	if (!filename)
		return;
	size_t i;
	for (i=0; i<countof(arrAntiViruses); i++)
	{
		if (0 == tstricmp(filename, arrAntiViruses[i].file_name))
			break;
	}
	if (i >= countof(arrAntiViruses))
		return;
	pProcessDest.release();
	return m_pEventMgr->OnAntiAV(event, arrAntiViruses[i].id);
}


void cEventHandlerHeurKillAV::OnKillService(cEvent& event, tcstring service)
{
	for (size_t i=0; i<countof(arrAntiViruses); i++)
	{
		if(arrAntiViruses[i].service_name == NULL)
			continue;
		if (0 == tstricmp(service, arrAntiViruses[i].service_name))
			return m_pEventMgr->OnAntiAV(event, arrAntiViruses[i].id);
	}
	return;
}

void cEventHandlerHeurKillAV::OnAntiAV(cEvent& event, int32_t av_id)
{
	uint32_t nAntiAV_Count = 0;
	{
		cProcess pProcess = event.GetProcess();
		if (!pProcess)
			return;
		uint32_t anti_av_flag = MAKE_ANTI_AV_FLAG(av_id);
		if (0 != (pProcess->m_nAntiAV_Flags & anti_av_flag))
			return;
		pProcess->m_nAntiAV_Flags |= anti_av_flag;
		pProcess->m_nAntiAV_Count++;
		nAntiAV_Count = pProcess->m_nAntiAV_Count;
	}
	if (nAntiAV_Count >= 2)
		return m_pEventMgr->OnAntiAVMultiple(event, av_id);
	return;
}

void cEventHandlerHeurKillAV::OnRegValueSetStrPre(cEvent& event, const cRegKey& sKey, tcstring sValueName, tcstring sNewData, tcstring sValueData)
{
	if (sKey.m_root != crHKEY_LOCAL_MACHINE)
		return;
	if (tstrMatchWithPattern(sKey.m_name, _T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\IMAGE FILE EXECUTION OPTIONS\\AVP.*"), true))
		return m_pEventMgr->OnRegAvpImageFileExecOptions(event, sKey, sValueName, sNewData);
	if (sValueName && tstrMatchWithPattern(sKey.m_name, _T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\APPCOMPATFLAGS\\LAYERS"), true))
	{
		if (tstrMatchWithPattern(sValueName, _T("*\\AVP.EXE"), false)
			|| tstrMatchWithPattern(sValueName, _T("*\\AVP.COM"), false))
			return m_pEventMgr->OnRegAvpImageFileExecOptions(event, sKey, sValueName, sNewData);
	}
}

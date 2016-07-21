#include "heur_base.h"
#include "srv.h"
#include "trace.h"

cEventHandlerHeuristic::cEventHandlerHeuristic() 
{
}

cEventHandlerHeuristic::~cEventHandlerHeuristic()
{
	return;
}

bool cEventHandlerHeuristic::Init()
{
	m_RegAutoRuns.Init(m_pEnvironmentHelper, m_pEventMgr);
	m_Peer2PeerPaths.SearchPeer2PeerPaths(m_pEnvironmentHelper);

	tcstring g_DefaultStartupPaths[] = {
		_T("\\START MENU\\PROGRAMS\\STARTUP\\"),
		_T("\\STARTMENU\\PROGRAMME\\AUTOSTART\\"),
	};
	
	for (int i=0; i<countof(g_DefaultStartupPaths); i++)
		m_StartupPaths.add(g_DefaultStartupPaths[i]);
	return true;
}


void cEventHandlerHeuristic::OnProcessCreatePre(cEvent& event, cFile& image_path, tcstring cmd_line, uint32_t creation_flags)
{
	bool bSystemPath;
	bool bSystemName;
	
	bool bSystemFile = srvIsWinSystemFile(m_pEnvironmentHelper, image_path, &bSystemName, &bSystemPath);
	if (bSystemFile && (creation_flags & cCREATE_SUSPENDED))
		return m_pEventMgr->OnStartSystemFileSuspended(event, image_path, cmd_line);

	if (bSystemName && !bSystemPath)
	{
		cProcess proc = event.GetProcess();
		if (!proc)
			return;
		bool bStartWithSystemName = proc->m_CreatedImagesList.contain(image_path.getFull());
		proc.release();
		if (bStartWithSystemName)
			return m_pEventMgr->OnStartWithSystemName(event, image_path, cmd_line);
	}
	return;
}

void cEventHandlerHeuristic::OnFileCloseModifiedPre(cEvent& event, cFile& file)
{
	if (m_OpCacheFileCloseModified.DoPre(event, file))
		return iOnFileCloseModified(event, file);
	return;
}


void cEventHandlerHeuristic::OnFileCloseModifiedPost(cEvent& event, cFile& file)
{
	if (m_OpCacheFileCloseModified.DoPost(event, file))
		return iOnFileCloseModified(event, file);
	return;
}

void cEventHandlerHeuristic::iOnFileCloseModified(cEvent& event, cFile& file)
{
	bool bSelfCopy = false;

	if (!file.isExecutable())
	{
		CheckAutorunInf(event, file);
		//CheckConfigFiles(pProcess, pwchFilename);
		//CheckCryptor(pProcess, pwchFilename);
		return;
	}

	m_RegAutoRuns.ResolveAll(event); // resolve new files

	if (m_StartupPaths.containSubstrForPath(file.getFull()))
		m_pEventMgr->OnInstall(event, cRegKey(), 0, file);

	cProcess pProcess;
	pProcess = event.GetProcess();
	if (!pProcess)
		return;
	cFile src_file(pProcess->m_image, &pProcess->fi);
	if (!srvIsImageCopy(m_pEnvironmentHelper, src_file, file))
		return;

	if (0 == tstrcmp(pProcess->m_image.getFull(), file.getFull()))
	{
		PR_TRACE((g_root, prtNOTIFY, TR "Check selfcopy: Self-modify detected: src=%S, dst=%S", pProcess->m_image.getFull(), filename.getFull()));
		return;
	}

	pProcess.release();
	// self-copy found
	m_pEventMgr->OnSelfCopy(event, file);
	return;
}

void cEventHandlerHeuristic::OnCopyFilePre(cEvent& event, cFile& src, cFile& dst, bool bFailIfExist)
{
	cProcess pProcess = event.GetProcess();
	if (!pProcess)
		return;
	if (0 != tstrcmp(pProcess->m_image.getFull(), src.getFull()))
		return;
	pProcess.release();
	return m_pEventMgr->OnSelfCopy(event, dst);;
}

void cEventHandlerHeuristic::OnSelfCopy(cEvent& event, cFile& copy)
{
	size_t nProcessSelfCopyListCount = 0;
	size_t nProcessCreateDllCount = 0;

	{
		cProcess pProcess;
		pProcess = event.GetProcess();
		if (!pProcess)
			return;

		pProcess->m_SelfCopyList.add(copy.getFull());
		nProcessSelfCopyListCount = pProcess->m_SelfCopyList.count();
		m_selfcopy_list.add(pProcess->m_uniq_pid, copy.getFull());
		size_t nCreated = pProcess->m_CreatedImagesList.count();
		for (size_t i=0; i<nCreated; i++)
		{
			if (tstrMatchWithPattern(pProcess->m_CreatedImagesList[i], _T("*.DLL"), false))
				nProcessCreateDllCount++;
		}
	}

	tstring strTempPath = m_pEnvironmentHelper->ExpandEnvironmentStr(_T("%TEMP%\\*"));
	if (strTempPath)
	{
		strTempPath = tstrupr(strTempPath);
		if (strTempPath && !tstrMatchWithPattern(copy.getFull(), strTempPath, true))
			m_pEventMgr->OnSelfCopyNot2Temp(event, copy);
		if (strTempPath)
			tstrfree(strTempPath);
	}

	cRegAutoRun ar;
	if (m_RegAutoRuns.IsRegistered(event, copy, &ar))
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "Copy found in auto-registry:"
			"\n\t\treg:\t%S\\%S"
			"\n\t\tval:\t%S (%S)"
			"\n\t\tfile:\t%S",
			ar.key, ar.value_name,
			ar.value_data, ar.value_data_part,
			ar.filename));
		m_pEventMgr->OnSelfCopyAutoRun(event, ar.key,  ar.value_name, ar.value_data, ar.value_data_part, ar.filename);
		if (evtVerdict_Default != event.GetVerdict())
			return;
	}

	if (m_StartupPaths.containSubstrForPath(copy.getFull()))
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "Selfcopy into startup folder:"
			"\n\t\tsrc:\t%S"
			"\n\t\tdst:\t%S",
			pProcess->m_image.getFull(),
			copy.getFull()));

		m_pEventMgr->OnSelfCopyToStartup(event, ar.filename);
		if (evtVerdict_Default != event.GetVerdict())
			return;
	}

	if (m_pEnvironmentHelper->PathIsNetwork(copy.getFull()))
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "selfcopy to network path:"
			"\n\t\tsrc:\t%S"
			"\n\t\tdst:\t%S",
			pProcess->m_image,
			copy.getFull()));

		m_pEventMgr->OnSelfCopyToNetwork(event, copy);
		if (evtVerdict_Default != event.GetVerdict())
			return;
	}	

	if (m_Peer2PeerPaths.IsPeer2PeerPath(copy))
	{
		uint64_t FileSize = m_pEnvironmentHelper->FileSizeByName(copy.getFull());
		//	FileSize needed to control 0-value if file not exist for pre-events
		//	1500 (was 150) for testing purpose !!!
		if ((FileSize && (FileSize < 150*1024))                 // it's not installer 
			|| nProcessSelfCopyListCount >= 2)     // || many copies 
		{
			PR_TRACE((g_root, prtIMPORTANT, TR "selfcopy to network path(P2P):"
				"\n\t\tsrc:\t%S"
				"\n\t\tdst:\t%S",
				pProcess->m_image,
				copy.getFull()));

			m_pEventMgr->OnSelfCopyToPeerToPeerFolder(event, copy);
			if (evtVerdict_Default != event.GetVerdict())
				return;
		}
	}	

	if (nProcessSelfCopyListCount > 2 && nProcessCreateDllCount < 5)
	{
//		PR_TRACE((g_root, prtIMPORTANT, TR "selfcopy to multiple paths path(P2P): image=%S", pProcess->m_image));
//		for (size_t i=0; i<pProcess->m_SelfCopyList.count(); i++)
//		{
//			PR_TRACE((g_root, prtIMPORTANT, TR "\tcopy %d: %S", i, pProcess->m_SelfCopyList.m_List[i]));
//		}
		m_pEventMgr->OnSelfCopyMultiple(event, copy);
		if (evtVerdict_Default != event.GetVerdict())
			return;
	}
	return;
}

void cEventHandlerHeuristic::OnRegValueSetStrPre(cEvent& event, const cRegKey& key, tcstring sValueName, tcstring sNewData, tcstring sValueData)
{
	if (key.m_root == crHKEY_CLASSES_ROOT)
	{
		if (key.m_name && tstrchar(key.m_name) == '.') // starts with '.' - extension
		{
			const tcstring executable_exts[] = {_T(".COM"), _T(".EXE"), _T(".BAT"), _T(".CMD"), _T(".VBS"), _T(".VBE"), _T(".JS"), _T(".JSE"), _T(".WSF"), _T(".WSH")};
			for (size_t i=0; i<countof(executable_exts); i++)
			{
				if (0 == tstrcmp(key.m_name, executable_exts[i]))
				{
					if (!sValueName)
						sValueName = _T("");
					if (!tstrcmp(sValueName, _T("")) || !tstricmp(sValueName, _T("ContentType")))
						m_pEventMgr->OnChangeSystemRegistry(event, key, sValueName, sNewData, sValueData);
					break;
				}
			}
		}
	}
	uint32_t nARFlags = 0;
	if (!m_RegAutoRuns.IsAutoRun(key, sValueName, &nARFlags)) {
		return;
	}
	m_RegAutoRuns.Register(event, key, sValueName, sNewData, nARFlags);
	return;
}

void cEventHandlerHeuristic::OnRegValueSetStrPost(cEvent& event, const cRegKey& key, tcstring sValueName, tcstring sNewData, tcstring sValueData)
{
//<<-- added by Martynenko (P2P_SC_RDL_1.txt detection)
	if (sValueName && m_Peer2PeerPaths.IsP2PKey(key.m_fullname, sValueName)) {
		//trace_func(0, prtIMPORTANT, "cEventHandlerHeuristic::OnRegValueSetStrPost:: IsP2PKey return true");
		//	P2P key was changed
		cPath Path(m_pEnvironmentHelper, sNewData);
		m_Peer2PeerPaths.AddPeer2PeerPath(Path);

		cProcess proc = event.GetProcess();
		if (!proc)
			return;

		if (tstrstr(proc->m_image.getFull(), Path.getFull())
			|| m_selfcopy_list.containPathForSubstr(Path.getFull()))
		{
			cFile SelfName(m_pEnvironmentHelper, proc->m_image.getFull());
			proc.release();
			m_pEventMgr->OnSelfCopyToPeerToPeerFolder(event, SelfName);
		}
	}
//-->>

	if (sValueName == 0 || sValueName[0] == 0) // if default value
	{
		if (tstrMatchWithPattern(key.m_name, _T("CLSID\\{*}\\INPROCSERVER32"), true)
			|| tstrMatchWithPattern(key.m_name, _T("CLSID\\{*}\\LOCALSERVER32"), true))
		{
			m_RegAutoRuns.ResolveAll(event);
		}
	}
}

void cEventHandlerHeuristic::OnRegAutoRunRegisterResolved(cEvent& event, const cRegKey& sKey, tcstring sValueName, tcstring sValueData, tcstring sValueDataPart, tcstring sFilename)
{
	tPid selfcopy_pid;
	bool bSystemPath;
	bool bSystemName;

	cFile file(m_pEnvironmentHelper, sFilename, sFilename);
	srvIsWinSystemFile(m_pEnvironmentHelper, file, &bSystemName, &bSystemPath);

	if (bSystemName && !bSystemPath)
	{
		cProcess proc = event.GetProcess();
		if (!proc)
			return;
		bool bStartWithSystemName = proc->m_CreatedImagesList.contain(file.getFull());
		proc.release();
		if (bStartWithSystemName)
			m_pEventMgr->OnStartWithSystemName(event, file, sValueData);
	}

	if (m_selfcopy_list.contain(sFilename, &selfcopy_pid))
	{
		cEvent new_event(event, selfcopy_pid);
		m_pEventMgr->OnSelfCopyAutoRun(new_event, sKey, sValueName, sValueData, sValueDataPart, sFilename);
	}
	return;
}

void cEventHandlerHeuristic::OnFileRenamePost(cEvent& event, cFile& src_file, cFile& dst_file)
{
	cProcess pProcess = event.GetProcess();
	if (!pProcess)
		return;
	if (0 != tstrcmp(pProcess->m_image.getFull(), src_file.getFull()))
		return;
	if (0 == tstrcmp(pProcess->m_image.getFull(), dst_file.getFull()))
		return;
	pProcess.release();
	cEvent evtSelfCopy(event, event.m_pid);
	m_pEventMgr->OnSelfCopy(evtSelfCopy, dst_file);
	return;
}

void cEventHandlerHeuristic::OnRegKeyCreatePost(cEvent& event, const cRegKey& key)
{
	uint32_t nARFlags = 0;
	if (!m_RegAutoRuns.IsAutoRun(key, NULL, &nARFlags))
		return;
	m_RegAutoRuns.Register(event, key, NULL, NULL, nARFlags);
	return;
}

void cEventHandlerHeuristic::OnOpenSectionPre(cEvent& event, tcstring section_name, uint32_t desired_access)
{
	if (!section_name)
		return;
	if (0 != tstricmp(section_name, _T("\\Device\\PhysicalMemory")))
		return;
	bool bWrite = !!(desired_access & 2/*SECTION_MAP_WRITE*/);
	m_pEventMgr->OnPhysicalMemoryAccess(event, bWrite);
	return;
}

void cEventHandlerHeuristic::OnFileDeletePost(cEvent& event, cFile& file)
{
	cProcess pProcess = event.GetProcess();
	if (!pProcess)
		return;
	if (0 != tstrcmp(pProcess->m_image.getFull(), file.getFull()))
		return;
	pProcess.release();
	m_pEventMgr->OnDeleteSelf(event, file);
	return;
}

#if !defined(kl_stricmp)
#	if defined(__GNUC__)
#		define kl_stricmp	strcasecmp
#	elif defined(_MSC_VER)
#		define kl_stricmp	_stricmp
#	else
#		error "Unsupported compiler"
#	endif
#endif

void cEventHandlerHeuristic::CheckAutorunInf(cEvent & evt, cFile & file)
{
	if (!tstrMatchWithPattern(file.getFull(), _T("?:\\AUTORUN.INF"), false))
		return;

	if (!file.isReadable())
		return;

	char line_buf[1024];
	uint32_t line_len;

	uint32_t delim_len;
	if (!file.ReadLine(0, line_buf, sizeof(line_buf), &line_len, &delim_len) || line_len == sizeof(line_buf))
	{
		return;
	}
	line_buf[line_len] = '\0';

	if (kl_stricmp(line_buf, "[autorun]") != 0)
	{
		return;
	}

	size_t StrSize = 1024;
	tstring tPath = tstralloc(StrSize);
	if (!tPath)
		return;

	uint64_t pos = line_len + delim_len;
	unsigned line_count = 0;
	while (line_count++ < 16 && file.ReadLine(pos, line_buf, sizeof(line_buf), &line_len, &delim_len) && line_len < sizeof(line_buf))
	{
		line_buf[line_len] = '\0';
		const char * command = line_buf;
		char * path = strchr(line_buf, '=');

		if (!path)
			break;
		else
			*path++ = '\0';

		if (kl_stricmp(command, "open") == 0 || kl_stricmp(command, "shell\\open\\command") == 0)
		{
			StrSize = 1024;
			if (!m_pEventMgr->m_pEnvironmentHelper->ConvertANSIToTStr(path, tPath, &StrSize))
			{
				if (StrSize > 1024)
				{
					PR_TRACE((g_root, prtERROR, TR "ConvertANSIToTStr return StrSize==%d", StrSize));
					break;
				}
				PR_TRACE((g_root, prtERROR, TR "ConvertANSIToTStr FAIL"));
				break;
			}

			tstring fullPath;
			if (tPath[0] != 0 && tPath[1] == _T(':'))
			{
				fullPath = tstrdup(tPath);
			}
			else
			{
				fullPath = tstrdup(_T("?:\\"));
				if (!fullPath)
					break;
				fullPath[0] = file.getFull()[0];
				fullPath = tstrappend(fullPath, tPath);
			}
			if (!fullPath)
				break;

			cFile execFile(m_pEnvironmentHelper, fullPath);
			tstrfree(fullPath);

			m_RegAutoRuns.Register(evt, cRegKey(), NULL, execFile.getFull(), 0);
		}
		pos += line_len + delim_len;
	}
	tstrfree(tPath);
	return;
}
